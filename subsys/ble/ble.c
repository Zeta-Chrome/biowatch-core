#include "aci/aci_gap.h"
#include "aci/aci_gatt.h"
#include "aci/aci_hal.h"
#include "ble.h"
#include "hci/hci_le.h"
#include "kernel/sync/event.h"
#include "kernel/task/task.h"
#include "lib/containers/stm_clist.h"
#include "lib/status.h"
#include "lib/utils.h"
#include "subsys/ble/ble_cfg.h"
#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"
#include "subsys/ble/shci/shci.h"
#include "subsys/ble/tl/tl.h"

#define DEVICE_ID_BASE 0x1FFF7590UL
#define NBR_OF_TRACES_CONFIG_PARAMETERS 4
#define NBR_OF_GENERAL_CONFIG_PARAMETERS 4
#define BLE_POOL_SIZE                  \
	(CFG_TLBLE_EVT_QUEUE_LENGTH * 4U * \
	 DIVC((sizeof(tl_packet_header_t) + TL_BLE_EVENT_FRAME_SIZE), 4U))
#define BLE_SYS_INIT_EVT BIT(0)

PLACE_IN_SECTION("MB_MEM2")
ALIGN(4) static uint8_t g_evt_pool[BLE_POOL_SIZE];
PLACE_IN_SECTION("MB_MEM2")
ALIGN(4) static uint8_t g_sys_spare_evt_buf[sizeof(tl_packet_header_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2")
ALIGN(4) static uint8_t g_ble_spare_evt_buf[sizeof(tl_packet_header_t) + TL_EVT_HDR_SIZE + 255];
PLACE_IN_SECTION("MB_MEM2")
ALIGN(4) static shci_c2_debug_traces_config_t g_debug_traces_config = { 0, 0, 0, 0 };
PLACE_IN_SECTION("MB_MEM2")
ALIGN(4)
static shci_c2_debug_general_config_t g_debug_general_config = { 0, 0, { 0, 0 }, 0, 0, 0, 0, 0 };

static uint8_t g_bd_addr[6];
static uint8_t g_manuf_data[14] = {
	sizeof(g_manuf_data) - 1,
	0xFF,
	0x01 /*SKD version */,
	0x00 /* Generic*/,
	0x00 /* GROUP A Feature  */,
	0x00 /* GROUP A Feature */,
	0x00 /* GROUP B Feature */,
	0x00 /* GROUP B Feature */,
	0x00, /* BLE MAC start -MSB */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00, /* BLE MAC stop */
};

static const uint8_t g_ble_ir_value[16] = CFG_BLE_IR;
static const uint8_t g_ble_er_value[16] = CFG_BLE_ER;
static stm_list_node_t g_sys_evt_queue;
static task_handle_t g_sys_evt_task_h;
static stm_list_node_t g_ble_evt_queue;
static task_handle_t g_ble_evt_task_h;
static struct event g_evt;

static void sys_evt_task(void *user_data);

static void sys_evt_callback(tl_evt_packet_t *p_evt_pkt)
{
	stm_list_insert_tail(&g_sys_evt_queue, (stm_list_node_t *)p_evt_pkt);
	kernel_task_notify_from_isr(g_sys_evt_task_h, 1, NOTIFY_ACTION_NONE);
}

static void traces_evt_callback(tl_evt_packet_t *p_evt_pkt)
{
	bw_print_s((char *)((tl_async_evt_t *)p_evt_pkt->evt_serial.evt.payload)->payload,
			   p_evt_pkt->evt_serial.evt.payload_len - 2U);
	tl_mm_evt_done(p_evt_pkt);
}

static bool sys_ready_evt_handler(tl_async_evt_t *p_sys_evt)
{
	shci_c2_ready_evt_t *p_sys_ready_event;

	shci_c2_config_cmd_param_t config_param = { 0 };
	uint32_t revision_id = 0;
	uint32_t device_id = 0;

	p_sys_ready_event = (shci_c2_ready_evt_t *)p_sys_evt->payload;

	if (p_sys_ready_event->sysevt_ready_rsp == WIRELESS_FW_RUNNING) {
		BW_LOG("SHCI code ready: BLE running\n");

		shci_c2_debug_init_cmd_packet_t dbg_cmd_pkt = {
			{ { 0, 0, 0 } }, /**< Does not need to be initialized */
			{ NULL, (uint8_t *)&g_debug_traces_config, (uint8_t *)&g_debug_general_config, 0,
			  NBR_OF_TRACES_CONFIG_PARAMETERS, NBR_OF_GENERAL_CONFIG_PARAMETERS }
		};

		struct tl_traces_conf tr_conf = { .evt_callback = traces_evt_callback };
		tl_traces_init(&tr_conf);
		enum shci_cmd_status status = shci_c2_debug_init(&dbg_cmd_pkt);
		if (status != SHCI_SUCCESS) {
			BW_LOG("Fail : shci_c2_debug_init command, result: %x\n", status);
			bw_error_handler();
		} else {
			BW_LOG("Success : shci_c2_debug_init command\n");
		}

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
		BW_LOG("DBGMCU Revision ID= %x \n", revision_id);

		device_id = dbg_mcu_get_device_id();
		config_param.device_id = (uint16_t)device_id;
		BW_LOG("DBGMCU Device ID= %x \n", device_id);

		status = shci_c2_config(&config_param);
		if (status != SHCI_SUCCESS) {
			BW_LOG("Fail : shci_c2_config command, result: %x\n", status);
			bw_error_handler();
		} else {
			BW_LOG("Success : shci_c2_config command\n");
		}

		return true;
	} else if (p_sys_ready_event->sysevt_ready_rsp == FUS_FW_RUNNING) {
		BW_LOG("SHCI code ready: FUS running\n\r");
	} else {
		BW_LOG("SHCI code ready: Unexpected case \n");
	}

	return false;
}

static void sys_err_evt_handler(tl_async_evt_t *p_sys_evt)
{
	enum shci_sys_err_code *p_sys_error_code;

	p_sys_error_code = (enum shci_sys_err_code *)p_sys_evt->payload;

	BW_LOG("SHCI sys error with error code %x \n", (*p_sys_error_code));

	if ((*p_sys_error_code) == ERR_BLE_INIT) {
		/* Error during BLE stack initialization */
		BW_LOG("BLE init error \n");
	} else {
		BW_LOG("BLE error\n");
	}
}

static void sys_evt_task(void *user_data)
{
	(void)user_data;
	tl_evt_packet_t *p_sys_pkt;
	tl_async_evt_t *p_sys_evt;
	struct wireless_fw_info wireless_info;

	while (1) {
		kernel_task_notify_wait(1, 0, NULL, MAX_TIMEOUT);

		while (!stm_list_is_empty(&g_sys_evt_queue)) {
			stm_list_remove_head(&g_sys_evt_queue, (stm_list_node_t **)&p_sys_pkt);

			p_sys_evt = (tl_async_evt_t *)(p_sys_pkt->evt_serial.evt.payload);
			switch (p_sys_evt->sub_evt_code) {
			case SHCI_SUB_EVT_CODE_READY:
				shci_get_wireless_fw_info(&wireless_info);
				BW_LOG("Wireless firmware version %d.%d.%d\n", wireless_info.version_major,
					   wireless_info.version_minor, wireless_info.version_sub);
				BW_LOG("Wireless firmware build %d\n", wireless_info.version_release_type);
				BW_LOG("FUS version %d.%d.%d\n", wireless_info.fus_version_major,
					   wireless_info.fus_version_minor, wireless_info.fus_version_sub);
				if (sys_ready_evt_handler(p_sys_evt)) {
					tl_mm_evt_done(p_sys_pkt);
				}
				kernel_event_set(&g_evt, BLE_SYS_INIT_EVT);
				break;
			case SHCI_SUB_EVT_ERROR_NOTIF:
				BW_LOG("SHCI error event\n");
				sys_err_evt_handler(p_sys_evt);
				break;
			case SHCI_SUB_EVT_BLE_NVM_RAM_UPDATE:
				BW_LOG("BLE RAM has been updated by CPU2\n");
				BW_LOG("Start address = %x , Size = %d\n",
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
		}
	}
}

static void ble_evt_callback(tl_evt_packet_t *p_evt_pkt)
{
	stm_list_insert_tail(&g_ble_evt_queue, (stm_list_node_t *)p_evt_pkt);
	kernel_task_notify_from_isr(g_ble_evt_task_h, 1, NOTIFY_ACTION_NONE);
}

static void ble_evt_task(void *user_data)
{
	(void)user_data;

	tl_evt_packet_t *p_evt_pkt;
	while (1) {
		kernel_task_notify_wait(1, 0, NULL, MAX_TIMEOUT);

		while (!stm_list_is_empty(&g_ble_evt_queue)) {
			stm_list_remove_head(&g_ble_evt_queue, (stm_list_node_t **)&p_evt_pkt);
			tl_mm_evt_done(p_evt_pkt);
		}
	}
}

static const uint8_t *ble_get_bd_address()
{
	uint32_t uid0 = *(uint32_t *)(DEVICE_ID_BASE);
	uint32_t uid1 = *(uint32_t *)(DEVICE_ID_BASE + 0x04);

	g_bd_addr[0] = (uint8_t)(uid1 >> 24);
	g_bd_addr[1] = (uint8_t)(uid1 >> 16);
	g_bd_addr[2] = (uint8_t)(uid0);
	g_bd_addr[3] = (uint8_t)(uid0 >> 8);
	g_bd_addr[4] = (uint8_t)(uid0 >> 16);
	g_bd_addr[5] = (uint8_t)(uid0 >> 24);

	return g_bd_addr;
}

static void ble_gap_gatt_init(const char *name, enum ble_appearance appearance,
							  enum ble_io_capability io_capability,
							  enum ble_mitm_protection mitm_protection,
							  enum ble_secure_support secure_support, bool bonding_mode)
{
	enum ble_status ret = hci_reset();
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : hci_reset command, result: %x \n", ret);
	} else {
		BW_LOG("Success : hci_reset command\n");
	}

	// Write BLE address
	const uint8_t *p_bd_addr = ble_get_bd_address();
	ret = aci_hal_write_config_data(CONFIG_DATA_PUBLIC_ADDRESS_OFFSET,
									CONFIG_DATA_PUBLIC_ADDRESS_LEN, (uint8_t *)p_bd_addr);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail: aci_hal_write_config_data command - CONFIG_DATA_PUBLIC_ADDRESS_OFFSET, "
			   "result: %x \n",
			   ret);
	} else {
		BW_LOG("Success: aci_hal_write_config_data command - CONFIG_DATA_PUBLIC_ADDRESS_OFFSET\n");
		BW_LOG("Public Bluetooth Address: %x:%x:%x:%x:%x:%x\n", p_bd_addr[5], p_bd_addr[4],
			   p_bd_addr[3], p_bd_addr[2], p_bd_addr[1], p_bd_addr[0]);
	}

	/* BLE MAC in ADV Packet */
	g_manuf_data[sizeof(g_manuf_data) - 6] = p_bd_addr[5];
	g_manuf_data[sizeof(g_manuf_data) - 5] = p_bd_addr[4];
	g_manuf_data[sizeof(g_manuf_data) - 4] = p_bd_addr[3];
	g_manuf_data[sizeof(g_manuf_data) - 3] = p_bd_addr[2];
	g_manuf_data[sizeof(g_manuf_data) - 2] = p_bd_addr[1];
	g_manuf_data[sizeof(g_manuf_data) - 1] = p_bd_addr[0];

	// Write Identity root key used to derive IRK and DHK(Legacy)
	ret = aci_hal_write_config_data(CONFIG_DATA_IR_OFFSET, CONFIG_DATA_IR_LEN,
									(uint8_t *)g_ble_ir_value);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_hal_write_config_data command - CONFIG_DATA_IR_OFFSET, result: %x \n",
			   ret);
	} else {
		BW_LOG("Success : aci_hal_write_config_data command - CONFIG_DATA_IR_OFFSET\n");
	}

	// Write Encryption root key used to derive LTK and CSRK
	ret = aci_hal_write_config_data(CONFIG_DATA_ER_OFFSET, CONFIG_DATA_ER_LEN,
									(uint8_t *)g_ble_er_value);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_hal_write_config_data command - CONFIG_DATA_ER_OFFSET, result: %x \n",
			   ret);
	} else {
		BW_LOG("Success : aci_hal_write_config_data command - CONFIG_DATA_ER_OFFSET\n");
	}

	// Set TX Power.
	ret = aci_hal_set_tx_power_level(1, CFG_TX_POWER);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_hal_set_tx_power_level command, result: %x \n", ret);
	} else {
		BW_LOG("Success : aci_hal_set_tx_power_level command\n");
	}

	// Initialize GATT interface
	ret = aci_gatt_init();
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gatt_init command, result: %x \n", ret);
	} else {
		BW_LOG("Success : aci_gatt_init command\n");
	}

	// GAP init
	uint8_t role = GAP_PERIPHERAL_ROLE;
	uint16_t gap_service_handle, gap_dev_name_char_handle, gap_appearance_char_handle;

	ret = aci_gap_init(role, PRIVACY_DISABLED, strlen(name), &gap_service_handle,
					   &gap_dev_name_char_handle, &gap_appearance_char_handle);

	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gap_init command, result: %x \n", ret);
	} else {
		BW_LOG("Success : aci_gap_init command\n");
	}

	ret = aci_gatt_update_char_value(gap_service_handle, gap_dev_name_char_handle, 0, strlen(name),
									 (uint8_t *)name);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gatt_update_char_value - Device Name, result: %x\n", ret);
	} else {
		BW_LOG("Success : aci_gatt_update_char_value - Device Name\n");
	}

	uint16_t appr[1] = { appearance };
	ret = aci_gatt_update_char_value(gap_service_handle, gap_appearance_char_handle, 0, 2,
									 (uint8_t *)&appr);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gatt_update_char_value - Appearance, result: %d\n", ret);
	} else {
		BW_LOG("Success : aci_gatt_update_char_value - Appearance\n");
	}

	ret = hci_le_set_default_phy(ALL_PHYS_PREFERENCE, TX_2M_PREFERRED, RX_2M_PREFERRED);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : hci_le_set_default_phy command, result: %x \n", ret);
	} else {
		BW_LOG("Success : hci_le_set_default_phy command\n");
	}

	/**
     * Initialize IO capability
     */
	ret = aci_gap_set_io_capability(io_capability);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gap_set_io_capability command, result: %x \n", ret);
	} else {
		BW_LOG("Success : aci_gap_set_io_capability command\n");
	}

	ret = aci_gap_set_authentication_requirement(
		bonding_mode, mitm_protection, secure_support, 0, CFG_ENCRYPTION_KEY_SIZE_MIN,
		CFG_ENCRYPTION_KEY_SIZE_MAX, USE_FIXED_PIN_FOR_PAIRING_FORBIDDEN, 0, GAP_PUBLIC_ADDR);
	if (ret != BLE_STATUS_SUCCESS) {
		BW_LOG("Fail : aci_gap_set_authentication_requirement command, result: %x \n", ret);
	} else {
		BW_LOG("Success : aci_gap_set_authentication_requirement command\n");
	}

	if (bonding_mode) {
		ret = aci_gap_configure_filter_accept_list();
		if (ret != BLE_STATUS_SUCCESS) {
			BW_LOG("Fail : aci_gap_configure_whitelist command, result: %x \n", ret);
		} else {
			BW_LOG("Success : aci_gap_configure_whitelist command\n");
		}
	}
	BW_LOG("End Ble_Hci_Gap_Gatt_Init function\n");
}

void ble_sys_init()
{
	tl_init();

	stm_list_init(&g_sys_evt_queue);
	kernel_task_create(sys_evt_task, "_BLE_SYS_TASK", 1, 256, NULL, &g_sys_evt_task_h);
	shci_init(sys_evt_callback);

	struct tl_mm_conf conf = {
		.p_async_evt_pool = g_evt_pool,
		.async_evt_pool_size = BLE_POOL_SIZE,
		.p_traces_evt_pool = NULL,
		.traces_evt_pool_size = 0,
		.p_ble_spare_evt_buffer = g_ble_spare_evt_buf,
		.p_sys_spare_evt_buffer = g_sys_spare_evt_buf,
	};
	tl_mm_init(&conf);

	kernel_event_init(&g_evt);
	tl_enable();

	enum bw_status status = kernel_event_wait(&g_evt, BLE_SYS_INIT_EVT, NULL, true, true, 1000);
	if (status == STATUS_TIMEOUT) {
		BW_LOG("Timeout occured while waiting for ble sys init\n");
	}
}

void ble_init(uint8_t num_gatt_attr, uint8_t num_gatt_srv, uint8_t num_link, const char *name,
			  enum ble_appearance appearance, enum ble_io_capability io_capability,
			  enum ble_mitm_protection mitm_protection, enum ble_secure_support secure_support,
			  bool bonding_mode)
{
	BW_ASSERT(num_link >= 1 && num_link <= 8, "Invalid num links: %d (Expected range 1-8)",
			  num_link);

	shci_c2_ble_init_cmd_packet_t ble_init_cmd_pkt = { { { 0, 0, 0 } }, /**< Header unused */
													   { 0, /** pBleBufferAddress not used */
														 0, /** BleBufferSize not used */
														 num_gatt_attr,
														 num_gatt_srv + 2,
														 CFG_BLE_ATT_VALUE_ARRAY_SIZE,
														 num_link,
														 CFG_BLE_DATA_LENGTH_EXTENSION,
														 CFG_BLE_PREPARE_WRITE_LIST_SIZE,
														 CFG_BLE_MBLOCK_COUNT(num_link),
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

	kernel_task_create(ble_evt_task, "_BLE_TASK", 1, 256, NULL, &g_ble_evt_task_h);
	stm_list_init(&g_ble_evt_queue);
	hci_init(ble_evt_callback);

	// LPM disable

	enum shci_cmd_status status = shci_c2_ble_init(&ble_init_cmd_pkt);
	if (status != SHCI_SUCCESS) {
		BW_LOG("Fail : SHCI_C2_BLE_Init command, result: %x\n", status);
		bw_error_handler();
	} else {
		BW_LOG("Success : SHCI_C2_BLE_Init command\n");
	}

	ble_gap_gatt_init(name, appearance, io_capability, mitm_protection, secure_support,
					  bonding_mode);

	// LPM enable
}
