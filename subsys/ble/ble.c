#include "ble.h"
#include "ble_conf.h"
#include "ble_debug.h"
#include "kernel/sync/semaphore.h"
#include "kernel/task/task.h"
#include "stdbool.h"
#include "shci/shci.h"
#include "ble_defs.h"
#include "auto/ble_hal_aci.h"
#include "auto/ble_hci_le.h"
#include "auto/ble_gatt_aci.h"
#include "auto/ble_gap_aci.h"
#include "auto/ble_types.h"
#include "subsys/ble/auto/ble_vs_codes.h"
#include "subsys/lpm/lpm.h"
#include "svc/svcctl_p.h"
#include "tl/tl.h"
#include "tl/shci_tl.h"
#include "tl/hci_tl.h"
#include <stddef.h>
#include <string.h>

#define DEVICE_ID_BASE 0x1FFF7590UL
#define BLE_POOL_SIZE                  \
	(CFG_TLBLE_EVT_QUEUE_LENGTH * 4U * \
	 DIVC((sizeof(tl_packet_header_t) + TL_BLE_EVENT_FRAME_SIZE), 4U))
#define SYS_READY_DUR MAX_TIMEOUT

PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t g_ble_evt_pool[BLE_POOL_SIZE];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static tl_cmd_packet_t g_sys_cmd_buf;
PLACE_IN_SECTION("MB_MEM2")
ALIGN(4) static uint8_t g_sys_spare_evt_buf[sizeof(tl_packet_header_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2")
ALIGN(4) static uint8_t g_ble_spare_evt_buf[sizeof(tl_packet_header_t) + TL_EVT_HDR_SIZE + 255];
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static tl_cmd_packet_t g_ble_cmd_buf;
static struct semaphore g_sys_ready_sem;

static struct ble_conf *g_conf;
static struct {
	uint16_t conn_handle;
	enum ble_conn_status conn_status;
	struct {
		uint8_t len;
		uint8_t uuid[20];
	} uuid;
} g_ble_ctx;

static bool ble_sys_init();
static bool sys_evt_rx(tl_evt_packet_t *p_evt_pkt);
static void sys_err_evt_handler(tl_asynch_evt_t *p_sys_evt);
static bool sys_ready_evt_handler(tl_asynch_evt_t *p_sys_evt);
static bool ble_evt_rx(tl_evt_packet_t *p_evt_pkt);
static void ble_hci_gap_gatt_init(struct ble_conf *conf);
static void ble_stack_init();

void ble_init(struct ble_conf *conf)
{
	g_conf = conf;

	// Set RFWKUP
	MODIFY_FIELD(RCC->CSR, RCC_CSR_RFWKPSEL_Msk, RCC_CSR_RFWKPSEL_Pos, 0x1);

	if (ble_sys_init())
		ble_stack_init();
}

static bool ble_sys_init()
{
	tl_init();
	shci_init(&g_sys_cmd_buf, sys_evt_rx);

	struct tl_mm_conf mm_conf = {
		.p_async_evt_pool = g_ble_evt_pool,
		.async_evt_pool_size = BLE_POOL_SIZE,
		.p_traces_evt_pool = NULL,
		.traces_evt_pool_size = 0,
		.p_ble_spare_evt_buffer = g_ble_spare_evt_buf,
		.p_sys_spare_evt_buffer = g_sys_spare_evt_buf,
	};
	tl_mm_init(&mm_conf);

	tl_enable();

	kernel_semaphore_binary_init(&g_sys_ready_sem, true);
	if (kernel_semaphore_take(&g_sys_ready_sem, SYS_READY_DUR) != STATUS_OK) {
		BW_LOG("BLE system init timeout occured with status");
		return false;
	}

	return true;
}

static bool sys_evt_rx(tl_evt_packet_t *p_evt_pkt)
{
	bool evt_flow = true;
	struct wireless_fw_info wireless_info;
	tl_asynch_evt_t *p_sys_evt = (tl_asynch_evt_t *)(p_evt_pkt->evt_serial.evt.payload);

	switch (p_sys_evt->sub_evt_code) {
	case SHCI_SUB_EVT_CODE_READY:
		shci_get_wireless_fw_info(&wireless_info);
		BW_LOG("Wireless firmware version %d.%d.%d\n", wireless_info.version_major,
			   wireless_info.version_minor, wireless_info.version_sub);
		BW_LOG("Wireless firmware build %d\n", wireless_info.version_release_type);
		BW_LOG("FUS version %d.%d.%d\n", wireless_info.fus_version_major,
			   wireless_info.fus_version_minor, wireless_info.fus_version_sub);
		evt_flow = sys_ready_evt_handler(p_sys_evt);
		break;
	case SHCI_SUB_EVT_ERROR_NOTIF:
		BW_LOG("SHCI error event\n");
		sys_err_evt_handler(p_sys_evt);
		break;
	case SHCI_SUB_EVT_BLE_NVM_RAM_UPDATE:
		BW_LOG("BLE RAM has been updated by CPU2\n");
		BW_LOG("Start address = %p , Size = %d\n",
			   ((shci_c2_ble_nvm_ram_update_evt_t *)p_sys_evt->payload)->start_address,
			   ((shci_c2_ble_nvm_ram_update_evt_t *)p_sys_evt->payload)->size);
		break;

	case SHCI_SUB_EVT_NVM_START_WRITE:
		BW_LOG("BLE NVM start write\n");
		BW_LOG("Number of words = %d\n",
			   ((shci_c2_nvm_start_write_evt_t *)p_sys_evt->payload)->number_of_words);
		break;

	case SHCI_SUB_EVT_NVM_END_WRITE:
		BW_LOG("BLE NVM end write\n");
		break;

	case SHCI_SUB_EVT_NVM_START_ERASE:
		BW_LOG("BLE NVM start erase\n");
		BW_LOG("Number of sectors = %ld\n",
			   ((shci_c2_nvm_start_erase_evt_t *)p_sys_evt->payload)->number_of_sectors);
		break;

	case SHCI_SUB_EVT_NVM_END_ERASE:
		BW_LOG("BLE NVM end erase\n");
		break;

	default:
		break;
	}

	return evt_flow;
}

static void sys_err_evt_handler(tl_asynch_evt_t *p_sys_evt)
{
	enum shci_sys_err_code *p_sys_error_code;

	p_sys_error_code = (enum shci_sys_err_code *)p_sys_evt->payload;

	BW_LOG("SHCI sys error with error code %p \n", (*p_sys_error_code));

	if ((*p_sys_error_code) == ERR_BLE_INIT) {
		/* Error during BLE stack initialization */
		BW_LOG("BLE init error \n");
	} else {
		BW_LOG("BLE error\n");
	}
}

static bool sys_ready_evt_handler(tl_asynch_evt_t *p_sys_evt)
{
	shci_c2_ready_evt_t *p_sys_ready_event;

	shci_c2_config_cmd_param_t config_param = { 0 };
	uint32_t revision_id = 0;
	uint32_t device_id = 0;

	p_sys_ready_event = (shci_c2_ready_evt_t *)p_sys_evt->payload;

	if (p_sys_ready_event->sys_evt_ready_rsp == WIRELESS_FW_RUNNING) {
		BW_LOG("SHCI code ready: BLE running\n");

		ble_debug_init();

		/* Enable all events Notification */
		config_param.payload_cmd_size = SHCI_C2_CONFIG_PAYLOAD_CMD_SIZE;
		config_param.evt_mask1 = SHCI_C2_CONFIG_EVTMASK1_BIT0_ERROR_NOTIF_ENABLE +
								 SHCI_C2_CONFIG_EVTMASK1_BIT1_BLE_NVM_RAM_UPDATE_ENABLE +
								 SHCI_C2_CONFIG_EVTMASK1_BIT2_THREAD_NVM_RAM_UPDATE_ENABLE +
								 SHCI_C2_CONFIG_EVTMASK1_BIT3_NVM_START_WRITE_ENABLE +
								 SHCI_C2_CONFIG_EVTMASK1_BIT4_NVM_END_WRITE_ENABLE +
								 SHCI_C2_CONFIG_EVTMASK1_BIT5_NVM_START_ERASE_ENABLE +
								 SHCI_C2_CONFIG_EVTMASK1_BIT6_NVM_END_ERASE_ENABLE;

		revision_id = dbg_mcu_get_revision_id();
		config_param.revision_id = (uint16_t)revision_id;
		BW_LOG("DBGMCU Revision ID= %p \n", revision_id);

		device_id = dbg_mcu_get_device_id();
		config_param.device_id = (uint16_t)device_id;
		BW_LOG("DBGMCU Device ID= %p \n", device_id);

		enum shci_cmd_status status = shci_c2_config(&config_param);
		if (status != SHCI_SUCCESS) {
			BW_LOG("Fail : shci_c2_config command, result: %p\n", status);
			bw_error_handler();
		} else {
			BW_LOG("Success : shci_c2_config command\n");
		}

		kernel_semaphore_give(&g_sys_ready_sem); // Continue with ble init
		return true;
	} else if (p_sys_ready_event->sys_evt_ready_rsp == FUS_FW_RUNNING) {
		BW_LOG("SHCI code ready: FUS running\n");
		return false;
	} else {
		BW_LOG("SHCI code ready: Unexpected case \n");
		return false;
	}
}

static void ble_stack_init()
{
	/* USER CODE END APP_BLE_Init_1 */
	shci_c2_ble_init_cmd_packet_t ble_init_cmd_pkt = { { { 0, 0, 0 } }, /**< Header unused */
													   { 0, /** pBleBufferAddress not used */
														 0, /** BleBufferSize not used */
														 CFG_BLE_NUM_GATT_ATTRIBUTES,
														 CFG_BLE_NUM_GATT_SERVICES,
														 CFG_BLE_ATT_VALUE_ARRAY_SIZE,
														 CFG_BLE_NUM_LINK,
														 CFG_BLE_DATA_LENGTH_EXTENSION,
														 CFG_BLE_PREPARE_WRITE_LIST_SIZE,
														 CFG_BLE_MBLOCK_COUNT,
														 CFG_BLE_MAX_ATT_MTU,
														 CFG_BLE_PERIPHERAL_SCA,
														 CFG_BLE_CENTRAL_SCA,
														 CFG_BLE_LS_SOURCE,
														 CFG_BLE_MAX_CONN_EVENT_LENGTH,
														 CFG_BLE_HSE_STARTUP_TIME,
														 CFG_BLE_VITERBI_MODE,
														 CFG_BLE_OPTIONS,
														 0,
														 CFG_BLE_MAX_COC_INITIATOR_NBR,
														 CFG_BLE_MIN_TX_POWER,
														 CFG_BLE_MAX_TX_POWER,
														 CFG_BLE_RX_MODEL_CONFIG,
														 CFG_BLE_MAX_ADV_SET_NBR,
														 CFG_BLE_MAX_ADV_DATA_LEN,
														 CFG_BLE_TX_PATH_COMPENS,
														 CFG_BLE_RX_PATH_COMPENS,
														 CFG_BLE_CORE_VERSION,
														 CFG_BLE_OPTIONS_EXT,
														 CFG_BLE_MAX_ADD_EATT_BEARERS,
														 NULL,
														 0 } };
	hci_init(&g_ble_cmd_buf, ble_evt_rx);
	lpm_disable_mode(LPM_MODE_STANDBY, "BLE");

	enum shci_cmd_status status = shci_c2_ble_init(&ble_init_cmd_pkt);
	if (status != SHCI_SUCCESS) {
		BW_LOG("Fail : SHCI_C2_BLE_Init command, result: %p\n", status);
		bw_error_handler();
	} else {
		BW_LOG("Success : SHCI_C2_BLE_Init command\n");
	}

	ble_hci_gap_gatt_init(g_conf);

	// SVCCTL init
	ble_svcctl_init();

	// Fill the primary uuid
	struct ble_uuid *ad_svc_uuid;
	ad_svc_uuid = ble_svcctl_get_ad_svc_uuid();
	switch (ad_svc_uuid->type) {
	case BLE_SVC_UUID_TYPE_16:
		g_ble_ctx.uuid.len = 2 + 1;
		g_ble_ctx.uuid.uuid[0] = AD_TYPE_16_BIT_SERV_UUID;
		memcpy(&g_ble_ctx.uuid.uuid[1], &ad_svc_uuid->uuid, 2);
		break;

	case BLE_SVC_UUID_TYPE_128:
		g_ble_ctx.uuid.len = 16 + 1;
		g_ble_ctx.uuid.uuid[0] = AD_TYPE_128_BIT_SERV_UUID;
		memcpy(&g_ble_ctx.uuid.uuid[1], &ad_svc_uuid->uuid, 16);
		break;
	}

	ble_status_t ret = aci_hal_set_radio_activity_mask(0x0006);
	if (ret != BLE_STATUS_SUCCESS)
		BW_LOG("Fail   : aci_hal_set_radio_activity_mask command, result: 0x%p \n", ret);
	else
		BW_LOG("Success: aci_hal_set_radio_activity_mask command\n");
}

static void ble_get_bd_address(uint8_t *bd_addr)
{
	uint32_t uid0 = *(uint32_t *)(DEVICE_ID_BASE);
	uint32_t uid1 = *(uint32_t *)(DEVICE_ID_BASE + 0x04);

	bd_addr[0] = (uint8_t)(uid1 >> 24);
	bd_addr[1] = (uint8_t)(uid1 >> 16);
	bd_addr[2] = (uint8_t)(uid0);
	bd_addr[3] = (uint8_t)(uid0 >> 8);
	bd_addr[4] = (uint8_t)(uid0 >> 16);
	bd_addr[5] = (uint8_t)(uid0 >> 24);
}

static void ble_hci_gap_gatt_init(struct ble_conf *conf)
{
	// Reset to sync BLE stack
	ble_status_t ret = hci_reset();
	if (ret != BLE_STATUS_SUCCESS)
		BW_LOG("Fail : hci_reset command, result: %p \n", ret);
	else
		BW_LOG("Success : hci_reset command\n");

	// Write BLE address
	uint8_t bd_addr[6];
	ble_get_bd_address(bd_addr);
	ret = aci_hal_write_config_data(CONFIG_DATA_RANDOM_ADDRESS_OFFSET,
									CONFIG_DATA_RANDOM_ADDRESS_LEN, bd_addr);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail: aci_hal_write_config_data command - CONFIG_DATA_RANDOM_ADDRESS_OFFSET, "
			   "result: %p \n",
			   ret);
	} else {
		BW_LOG("Success: aci_hal_write_config_data command - CONFIG_DATA_RANDOM_ADDRESS_OFFSET\n");
		BW_LOG("Public Bluetooth Address: %p:%p:%p:%p:%p:%p\n", bd_addr[5], bd_addr[4], bd_addr[3],
			   bd_addr[2], bd_addr[1], bd_addr[0]);
	}

	const uint8_t ble_ir_value[16] = CFG_BLE_IR;
	// Write Identity root key used to derive IRK and DHK(Legacy)
	ret = aci_hal_write_config_data(CONFIG_DATA_IR_OFFSET, CONFIG_DATA_IR_LEN, ble_ir_value);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_hal_write_config_data command - CONFIG_DATA_IR_OFFSET, result: %p \n",
			   ret);
	} else {
		BW_LOG("Success : aci_hal_write_config_data command - CONFIG_DATA_IR_OFFSET\n");
	}

	const uint8_t ble_er_value[16] = CFG_BLE_ER;
	// Write Encryption root key used to derive LTK and CSRK
	ret = aci_hal_write_config_data(CONFIG_DATA_ER_OFFSET, CONFIG_DATA_ER_LEN, ble_er_value);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_hal_write_config_data command - CONFIG_DATA_ER_OFFSET, result: %p \n",
			   ret);
	} else {
		BW_LOG("Success : aci_hal_write_config_data command - CONFIG_DATA_ER_OFFSET\n");
	}

	ret = aci_hal_set_tx_power_level(1, CFG_TX_POWER);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_hal_set_tx_power_level command, result: %p \n", ret);
	} else {
		BW_LOG("Success : aci_hal_set_tx_power_level command\n");
	}

	ret = aci_gatt_init();
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gatt_init command, result: %p \n", ret);
	} else {
		BW_LOG("Success : aci_gatt_init command\n");
	}

	uint8_t role = GAP_PERIPHERAL_ROLE;
	uint16_t gap_service_handle, gap_dev_name_char_handle, gap_appearance_char_handle;

	ret = aci_gap_init(role, PRIVACY_DISABLED, strlen(conf->name), &gap_service_handle,
					   &gap_dev_name_char_handle, &gap_appearance_char_handle);

	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gap_init command, result: %p \n", ret);
	} else {
		BW_LOG("Success : aci_gap_init command\n");
	}

	ret = aci_gatt_update_char_value(gap_service_handle, gap_dev_name_char_handle, 0,
									 strlen(conf->name), (uint8_t *)conf->name);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gatt_update_char_value - Device Name, result: %p\n", ret);
	} else {
		BW_LOG("Success : aci_gatt_update_char_value - Device Name\n");
	}

	uint16_t appr[1] = { conf->appearance };
	ret = aci_gatt_update_char_value(gap_service_handle, gap_appearance_char_handle, 0, 2,
									 (uint8_t *)&appr);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gatt_update_char_value - Appearance, result: %d\n", ret);
	} else {
		BW_LOG("Success : aci_gatt_update_char_value - Appearance\n");
	}

	ret = hci_le_set_default_phy(ALL_PHYS_PREFERENCE, TX_2M_PREFERRED, RX_2M_PREFERRED);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : hci_le_set_default_phy command, result: %p \n", ret);
	} else {
		BW_LOG("Success : hci_le_set_default_phy command\n");
	}

	ret = aci_gap_set_io_capability(conf->io_capability);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gap_set_io_capability command, result: %p \n", ret);
	} else {
		BW_LOG("Success : aci_gap_set_io_capability command\n");
	}

	ret = aci_gap_set_authentication_requirement(
		conf->bonding_mode, conf->mitm_protection, conf->secure_support, 0,
		CFG_ENCRYPTION_KEY_SIZE_MIN, CFG_ENCRYPTION_KEY_SIZE_MAX,
		USE_FIXED_PIN_FOR_PAIRING_FORBIDDEN, 0, GAP_PUBLIC_ADDR);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gap_set_authentication_requirement command, result: %p \n", ret);
	} else {
		BW_LOG("Success : aci_gap_set_authentication_requirement command\n");
	}

	if (conf->bonding_mode) {
		ret = aci_gap_configure_filter_accept_list();
		if (ret != BLE_STATUS_SUCCESS) {
			BW_LOG("Fail : aci_gap_configure_whitelist command, result: %p \n", ret);
		} else {
			BW_LOG("Success : aci_gap_configure_whitelist command\n");
		}
	}
}

static bool ble_evt_rx(tl_evt_packet_t *p_evt_pkt)
{
	hci_event_pckt *p_hci_evt = (hci_event_pckt *)((hci_uart_pckt *)&p_evt_pkt->evt_serial)->data;

	// Route the hci event packet
	switch (p_hci_evt->evt) {
	case HCI_DISCONNECTION_COMPLETE_EVT_CODE: {
		BW_LOG("HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE\n");
		g_ble_ctx.conn_handle = 0xFFFF;
		g_ble_ctx.conn_status = BLE_CONN_STATUS_IDLE;
		if (g_conf->disconnection_evt_cb)
			g_conf->disconnection_evt_cb();
		break;
	}
	case HCI_LE_META_EVT_CODE: {
		evt_le_meta_event *p_meta_evt = (evt_le_meta_event *)p_hci_evt->data;

		switch (p_meta_evt->subevent) {
		case HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE: {
			BW_LOG("HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE\n");
			break;
		}
		case HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE: {
			BW_LOG("HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE\n");
			break;
		}
		case HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE: {
			hci_le_connection_complete_event_rp0 *p_conn_complete_evt =
				(hci_le_connection_complete_event_rp0 *)p_meta_evt->data;
			g_ble_ctx.conn_handle = p_conn_complete_evt->connection_handle;
			g_ble_ctx.conn_status = BLE_CONN_STATUS_CONNECTED_CLIENT;
			BW_LOG("HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE - Connection Handle: %p\n",
				   g_ble_ctx.conn_handle);
			if (g_conf->connection_evt_cb)
				g_conf->connection_evt_cb();
			break;
		}
		default: {
			BW_LOG("Unknown HCI_LE_META_EVT_CODE event occured %p\n", p_meta_evt->subevent);
			break;
		}
		}
		break;
	}

	case HCI_VENDOR_SPECIFIC_EVT_CODE: {
		evt_blecore_aci *p_blecore_evt = (evt_blecore_aci *)p_hci_evt->data;

		switch (p_blecore_evt->ecode) {
		case ACI_GAP_BOND_LOST_VSEVT_CODE: {
			BW_LOG("ACI_GAP_BOND_LOST_VSEVT_CODE\n");
			ble_status_t ret = aci_gap_allow_rebond(g_ble_ctx.conn_handle);
			if (ret == BLE_STATUS_SUCCESS)
				BW_LOG("\taci_gap_allow_rebond : Success\n");
			else
				BW_LOG("\taci_gap_allow_rebond : Fail, reason: %p\n", ret);
			break;
		}
		case ACI_GAP_ADDR_NOT_RESOLVED_VSEVT_CODE: {
			BW_LOG("ACI_GAP_ADDR_NOT_RESOLVED_VSEVT_CODE\n");
			break;
		}
		case ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE: {
			aci_gap_pairing_complete_event_rp0 *pairing_complete =
				(aci_gap_pairing_complete_event_rp0 *)p_blecore_evt->data;
			BW_LOG("ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE\n");
			if (pairing_complete->status == 0)
				BW_LOG("\tPairing Success\n");
			else
				BW_LOG("\tPairing KO\n\tStatus: %p\n\tReason: %p\n", pairing_complete->status,
					   pairing_complete->reason);
			break;
		}
		case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE: {
			aci_gatt_attribute_modified_event_rp0 *attribute_modified =
				(aci_gatt_attribute_modified_event_rp0 *)p_blecore_evt->data;
			ble_svcctl_dispatch_write(attribute_modified->connection_handle,
									  attribute_modified->attr_handle,
									  attribute_modified->attr_data,
									  attribute_modified->attr_data_length, false);
			break;
		}
		case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE: {
			aci_gatt_write_permit_req_event_rp0 *write_perm_req =
				(aci_gatt_write_permit_req_event_rp0 *)p_blecore_evt->data;
			ble_svcctl_dispatch_write(write_perm_req->connection_handle,
									  write_perm_req->attribute_handle, write_perm_req->data,
									  write_perm_req->data_length, true);
			break;
		}
		case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE: {
			aci_gatt_read_permit_req_event_rp0 *read_perm_req =
				(aci_gatt_read_permit_req_event_rp0 *)p_blecore_evt;
			ble_svcctl_dispatch_read_req(read_perm_req->connection_handle,
										 read_perm_req->attribute_handle, read_perm_req->offset);
			break;
		}
		case ACI_GATT_INDICATION_VSEVT_CODE: {
			aci_gatt_indication_event_rp0 *indication =
				(aci_gatt_indication_event_rp0 *)p_blecore_evt->data;
			aci_gatt_confirm_indication(indication->connection_handle);
			break;
		}
		case ACI_HAL_END_OF_RADIO_ACTIVITY_VSEVT_CODE:
			break;
		default: {
			BW_LOG("Unknown HCI_VENDOR_SPECIFIC_EVT_CODE event occured %p\n", p_blecore_evt->ecode);
			break;
		}
		};
		break;
	};

	default:
		BW_LOG("Unknown HCI event occurred %p\n", p_hci_evt->evt);
		break;
	}

	return true;
}

void ble_adv_start(enum ble_conn_status status)
{
	uint16_t min_inter, max_inter;
	if (status == BLE_CONN_STATUS_FAST_ADV) {
		min_inter = CFG_FAST_CONN_ADV_INTERVAL_MIN;
		max_inter = CFG_FAST_CONN_ADV_INTERVAL_MAX;
	} else {
		min_inter = CFG_LP_CONN_ADV_INTERVAL_MIN;
		max_inter = CFG_LP_CONN_ADV_INTERVAL_MAX;
	}

	ble_adv_stop(); // Stop current advertisement
	g_ble_ctx.conn_status = status;

	uint8_t name_len = strlen(g_conf->name);
	uint8_t adv_name[name_len + 1];
	adv_name[0] = AD_TYPE_COMPLETE_LOCAL_NAME;
	memcpy(adv_name + 1, g_conf->name, name_len);

	ble_status_t ret = aci_gap_set_discoverable(ADV_IND, min_inter, max_inter, GAP_PUBLIC_ADDR,
												0x00, sizeof(adv_name), adv_name,
												g_ble_ctx.uuid.len, g_ble_ctx.uuid.uuid, 0, 0);
	if (ret != BLE_STATUS_SUCCESS)
		BW_LOG("==>> aci_gap_set_discoverable - fail, result: %p \n", ret);
	else
		BW_LOG("==>> aci_gap_set_discoverable - Success\n");
}

void ble_adv_stop()
{
	if (g_ble_ctx.conn_status == BLE_CONN_STATUS_FAST_ADV ||
		g_ble_ctx.conn_status == BLE_CONN_STATUS_LP_ADV) {
		ble_status_t ret = aci_gap_set_non_discoverable();
		if (ret != BLE_STATUS_SUCCESS)
			BW_LOG("==>> aci_gap_set_non_discoverable - Stop Advertising Failed , result: %d \n",
				   ret);
		else
			BW_LOG("==>> aci_gap_set_non_discoverable - Successfully Stopped Advertising \n");
	}
}

void ble_terminate()
{
	ble_status_t ret = aci_gap_terminate(g_ble_ctx.conn_handle, 0x13);
	if (ret != BLE_STATUS_SUCCESS)
		BW_LOG("==>> aci_gap_terminate - Terminate Failed , result: %d \n", ret);
	else
		BW_LOG("==>> aci_gap_terminate - Successfully Terminated \n");
}

enum ble_conn_status ble_get_conn_status()
{
	return g_ble_ctx.conn_status;
}
