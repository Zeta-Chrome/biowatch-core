#include "shci.h"
#include "subsys/ble/tl/shci_tl.h"
#include "subsys/ble/tl/tl.h"
#include <string.h>

/**
 *  C2 COMMAND
 *  These commands are sent to the CPU2
 */
uint8_t shci_c2_fus_get_state(enum shci_fus_getstate_error_code *p_error_code)
{
	/**
   * Buffer is large enough to hold command complete with payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE + 1];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_FUS_GET_STATE, 0, 0, p_rsp);

	if (p_error_code != 0) {
		*p_error_code = (enum shci_fus_getstate_error_code)(
			((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[1]);
	}

	return (((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_fw_upgrade(uint32_t fw_src_add, uint32_t fw_dest_add)
{
	/**
   * TL_BLEEVT_CC_BUFFER_SIZE is 16 bytes so it is large enough to hold the 8 bytes of command parameters
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;
	uint32_t *p_cmd;
	uint8_t cmd_length;

	p_cmd = (uint32_t *)local_buffer;
	cmd_length = 0;

	if (fw_src_add != 0) {
		*p_cmd = fw_src_add;
		cmd_length += 4;
	}

	if (fw_dest_add != 0) {
		*(p_cmd + 1) = fw_dest_add;
		cmd_length += 4;
	}

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_FUS_FW_UPGRADE, cmd_length, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_fw_delete(void)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_FUS_FW_DELETE, 0, 0, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_fw_purge(void)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_FUS_FW_PURGE, 0, 0, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_update_auth_key(shci_c2_fus_update_auth_key_cmd_param_t *p_param)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_FUS_UPDATE_AUTH_KEY, sizeof(shci_c2_fus_update_auth_key_cmd_param_t),
			  (uint8_t *)p_param, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_lock_auth_key(void)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_FUS_LOCK_AUTH_KEY, 0, 0, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_store_usr_key(shci_c2_fus_store_usr_key_cmd_param_t *p_param,
											   uint8_t *p_key_index)
{
	/**
   * Buffer is large enough to hold command complete with payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE + 1];
	tl_evt_packet_t *p_rsp;
	uint8_t local_payload_len;

	if (p_param->key_type == KEYTYPE_ENCRYPTED) {
		/**
     * When the key is encrypted, the 12 bytes IV Key is included in the payload as well
     * The IV key is always 12 bytes
     */
		local_payload_len = p_param->key_size + 2 + 12;
	} else {
		local_payload_len = p_param->key_size + 2;
	}

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_FUS_STORE_USR_KEY, local_payload_len, (uint8_t *)p_param, p_rsp);

	*p_key_index = (((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[1]);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_load_usr_key(uint8_t key_index)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	local_buffer[0] = key_index;

	shci_send(SHCI_OPCODE_C2_FUS_LOAD_USR_KEY, 1, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_start_ws(void)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_FUS_START_WS, 0, 0, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_lock_usr_key(uint8_t key_index)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	local_buffer[0] = key_index;

	shci_send(SHCI_OPCODE_C2_FUS_LOCK_USR_KEY, 1, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_unload_usr_key(uint8_t key_index)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	local_buffer[0] = key_index;

	shci_send(SHCI_OPCODE_C2_FUS_UNLOAD_USR_KEY, 1, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_fus_activate_anti_rollback(void)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_FUS_ACTIVATE_ANTIROLLBACK, 0, 0, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_ble_init(shci_c2_ble_init_cmd_packet_t *p_cmd_packet)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_BLE_INIT, sizeof(shci_c2_ble_init_cmd_param_t),
			  (uint8_t *)&p_cmd_packet->param, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_debug_init(shci_c2_debug_init_cmd_packet_t *p_cmd_packet)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_DEBUG_INIT, sizeof(shci_c2_debug_init_cmd_param_t),
			  (uint8_t *)&p_cmd_packet->Param, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_flash_erase_activity(enum shci_erase_activity erase_activity)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	local_buffer[0] = erase_activity;

	shci_send(SHCI_OPCODE_C2_FLASH_ERASE_ACTIVITY, 1, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_concurrent_set_mode(enum shci_c2_concurrent_mode_param mode)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	local_buffer[0] = mode;

	shci_send(SHCI_OPCODE_C2_CONCURRENT_SET_MODE, 1, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status
shci_c2_concurrent_get_next_ble_evt_time(shci_c2_concurrent_get_next_ble_evt_time_param_t *p_param)
{
	/**
   * Buffer is large enough to hold command complete with payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE + 4];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_CONCURRENT_GET_NEXT_BLE_EVT_TIME, 0, 0, p_rsp);

	memcpy((void *)&(p_param->relative_time),
		   (void *)&((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[1],
		   sizeof(p_param->relative_time));

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_flash_store_data(enum shci_c2_flash_ip ip)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	local_buffer[0] = ip;

	shci_send(SHCI_OPCODE_C2_FLASH_STORE_DATA, 1, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_flash_erase_data(enum shci_c2_flash_ip ip)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	local_buffer[0] = ip;

	shci_send(SHCI_OPCODE_C2_FLASH_ERASE_DATA, 1, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_radio_allow_low_power(enum shci_c2_flash_ip ip,
												   uint8_t flag_radio_low_power_on)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	local_buffer[0] = ip;
	local_buffer[1] = flag_radio_low_power_on;

	shci_send(SHCI_OPCODE_C2_RADIO_ALLOW_LOW_POWER, 2, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_reinit(void)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_REINIT, 0, 0, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_ext_pa_config(uint32_t gpio_port, uint16_t gpio_pin_number,
										   uint8_t gpio_polarity, uint8_t gpio_status)
{
	/**
   * TL_BLEEVT_CC_BUFFER_SIZE is 16 bytes so it is large enough to hold the 8 bytes of command parameters
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	((shci_c2_extpa_config_cmd_param_t *)local_buffer)->gpio_port = gpio_port;
	((shci_c2_extpa_config_cmd_param_t *)local_buffer)->gpio_pin_number = gpio_pin_number;
	((shci_c2_extpa_config_cmd_param_t *)local_buffer)->gpio_polarity = gpio_polarity;
	((shci_c2_extpa_config_cmd_param_t *)local_buffer)->gpio_status = gpio_status;

	shci_send(SHCI_OPCODE_C2_EXTPA_CONFIG, 8, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status
shci_c2_set_flash_activity_control(enum shci_c2_set_flash_activity_control_source source)
{
	/**
   * TL_BLEEVT_CC_BUFFER_SIZE is 16 bytes so it is large enough to hold the 1 byte of command parameter
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	local_buffer[0] = (uint8_t)source;

	shci_send(SHCI_OPCODE_C2_SET_FLASH_ACTIVITY_CONTROL, 1, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_config(shci_c2_config_cmd_param_t *p_cmd_packet)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	shci_send(SHCI_OPCODE_C2_CONFIG, sizeof(shci_c2_config_cmd_param_t), (uint8_t *)p_cmd_packet,
			  p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

enum shci_cmd_status shci_c2_set_system_clock(shci_c2_set_system_clock_cmd_param_t clock_sel)
{
	/**
   * Buffer is large enough to hold command complete without payload
   */
	uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
	tl_evt_packet_t *p_rsp;

	p_rsp = (tl_evt_packet_t *)local_buffer;

	local_buffer[0] = (uint8_t)clock_sel;

	shci_send(SHCI_OPCODE_C2_SET_SYSTEM_CLOCK, 1, local_buffer, p_rsp);

	return (enum shci_cmd_status)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

/**
 *  Local System COMMAND
 *  These commands are NOT sent to the CPU2
 */

enum shci_cmd_status shci_get_wireless_fw_info(struct wireless_fw_info *p_wireless_info)
{
	uint32_t ipccdba = 0;
	struct mb_ref_table *p_ref_table = NULL;
	uint32_t wireless_firmware_version = 0;
	uint32_t wireless_firmware_memorySize = 0;
	uint32_t wireless_firmware_infoStack = 0;
	struct mb_fus_device_info_table *p_fus_device_info_table = NULL;
	uint32_t fus_version = 0;
	uint32_t fus_memorySize = 0;

	ipccdba = FLASH->IPCCBR & FLASH_IPCCBR_IPCCDBA;

	/**
     * The Device Info Table mapping depends on which firmware is running on CPU2.
     * If the FUS is running on CPU2, FUS_DEVICE_INFO_TABLE_VALIDITY_KEYWORD shall be written in the table.
     * Otherwise, it means the Wireless Firmware is running on the CPU2
     */
	p_fus_device_info_table =
		(struct mb_fus_device_info_table *)(*(uint32_t *)((ipccdba << 2) + SRAM2A_BASE));

	if (p_fus_device_info_table->device_info_table_state ==
		FUS_DEVICE_INFO_TABLE_VALIDITY_KEYWORD) {
		/* The FUS is running on CPU2 */
		/**
         *  Retrieve the WirelessFwInfoTable
         *  This table is stored in RAM at startup during the TL (transport layer) initialization
         */
		wireless_firmware_version = p_fus_device_info_table->wireless_stack_version;
		wireless_firmware_memorySize = p_fus_device_info_table->wireless_stack_memory_size;
		wireless_firmware_infoStack = p_fus_device_info_table->wireless_firmware_ble_info;

		/**
         *  Retrieve the FusInfoTable
         *  This table is stored in RAM at startup during the TL (transport layer) initialization
         */
		fus_version = p_fus_device_info_table->fus_version;
		fus_memorySize = p_fus_device_info_table->fus_memory_size;
	} else {
		/* The Wireless Firmware is running on CPU2 */
		p_ref_table = (struct mb_ref_table *)((ipccdba << 2) + SRAM2A_BASE);

		/**
         *  Retrieve the WirelessFwInfoTable
         *  This table is stored in RAM at startup during the TL (transport layer) initialization
         */
		wireless_firmware_version =
			p_ref_table->p_device_info_table->wireless_fw_info_table.version;
		wireless_firmware_memorySize =
			p_ref_table->p_device_info_table->wireless_fw_info_table.memory_size;
		wireless_firmware_infoStack =
			p_ref_table->p_device_info_table->wireless_fw_info_table.info_stack;

		/**
         *  Retrieve the FusInfoTable
         *  This table is stored in RAM at startup during the TL (transport layer) initialization
         */
		fus_version = p_ref_table->p_device_info_table->fus_info_table.version;
		fus_memorySize = p_ref_table->p_device_info_table->fus_info_table.memory_size;
	}

	/**
     *  Retrieve the WirelessFwInfoTable
     *  This table is stored in RAM at startup during the TL (transport layer) initialization
     */
	p_wireless_info->version_major =
		((wireless_firmware_version & INFO_VERSION_MAJOR_MASK) >> INFO_VERSION_MAJOR_OFFSET);
	p_wireless_info->version_minor =
		((wireless_firmware_version & INFO_VERSION_MINOR_MASK) >> INFO_VERSION_MINOR_OFFSET);
	p_wireless_info->version_sub =
		((wireless_firmware_version & INFO_VERSION_SUB_MASK) >> INFO_VERSION_SUB_OFFSET);
	p_wireless_info->version_branch =
		((wireless_firmware_version & INFO_VERSION_BRANCH_MASK) >> INFO_VERSION_BRANCH_OFFSET);
	p_wireless_info->version_release_type =
		((wireless_firmware_version & INFO_VERSION_TYPE_MASK) >> INFO_VERSION_TYPE_OFFSET);

	p_wireless_info->memory_size_sram2b =
		((wireless_firmware_memorySize & INFO_SIZE_SRAM2B_MASK) >> INFO_SIZE_SRAM2B_OFFSET);
	p_wireless_info->memory_size_sram2a =
		((wireless_firmware_memorySize & INFO_SIZE_SRAM2A_MASK) >> INFO_SIZE_SRAM2A_OFFSET);
	p_wireless_info->memory_size_sram1 =
		((wireless_firmware_memorySize & INFO_SIZE_SRAM1_MASK) >> INFO_SIZE_SRAM1_OFFSET);
	p_wireless_info->memory_size_flash =
		((wireless_firmware_memorySize & INFO_SIZE_FLASH_MASK) >> INFO_SIZE_FLASH_OFFSET);

	p_wireless_info->stack_type =
		((wireless_firmware_infoStack & INFO_STACK_TYPE_MASK) >> INFO_STACK_TYPE_OFFSET);

	/**
     *  Retrieve the FusInfoTable
     *  This table is stored in RAM at startup during the TL (transport layer) initialization
     */
	p_wireless_info->fus_version_major =
		((fus_version & INFO_VERSION_MAJOR_MASK) >> INFO_VERSION_MAJOR_OFFSET);
	p_wireless_info->fus_version_minor =
		((fus_version & INFO_VERSION_MINOR_MASK) >> INFO_VERSION_MINOR_OFFSET);
	p_wireless_info->fus_version_sub =
		((fus_version & INFO_VERSION_SUB_MASK) >> INFO_VERSION_SUB_OFFSET);

	p_wireless_info->fus_memory_size_sram2b =
		((fus_memorySize & INFO_SIZE_SRAM2B_MASK) >> INFO_SIZE_SRAM2B_OFFSET);
	p_wireless_info->fus_memory_size_sram2a =
		((fus_memorySize & INFO_SIZE_SRAM2A_MASK) >> INFO_SIZE_SRAM2A_OFFSET);
	p_wireless_info->fus_memory_size_flash =
		((fus_memorySize & INFO_SIZE_FLASH_MASK) >> INFO_SIZE_FLASH_OFFSET);

	return SHCI_SUCCESS;
}
