#ifndef BLE_TL_MBOX_H
#define BLE_TL_MBOX_H

#include "lib/utils.h"

/**
   * This file shall be identical between the CPU1 and the CPU2
   */

/**
   *********************************************************************************
   * TABLES
   *********************************************************************************
   */

/**
   * Version
   * [0:3]   = Build - 0: Untracked - 15:Released - x: Tracked version
   * [4:7]   = branch - 0: Mass Market - x: ...
   * [8:15]  = Subversion
   * [16:23] = Version minor
   * [24:31] = Version major
   *
   * Memory Size
   * [0:7]   = Flash ( Number of 4k sector)
   * [8:15]  = Reserved ( Shall be set to 0 - may be used as flash extension )
   * [16:23] = SRAM2b ( Number of 1k sector)
   * [24:31] = SRAM2a ( Number of 1k sector)
   */
typedef PACKED_STRUCT
{
	uint32_t version;
}
mb_safe_boot_info_table_t;

typedef PACKED_STRUCT
{
	uint32_t version;
	uint32_t memory_size;
	uint32_t fus_info;
}
mb_fus_info_table_t;

typedef PACKED_STRUCT
{
	uint32_t version;
	uint32_t memory_size;
	uint32_t info_stack;
	uint32_t reserved;
}
mb_wireless_fw_info_table_t;

struct mb_device_info_table {
	mb_safe_boot_info_table_t safe_boot_info_table;
	mb_fus_info_table_t fus_info_table;
	mb_wireless_fw_info_table_t wireless_fw_info_table;
};

struct mb_ble_table {
	uint8_t *p_cmd_buffer;
	uint8_t *p_cs_buffer;
	uint8_t *p_evt_queue;
	uint8_t *p_hci_acl_data_buffer;
};
/**
   * msg
   * [0:7]   = cmd/evt
   * [8:31] = Reserved
   */
struct mb_sys_table {
	uint8_t *p_cmd_buffer;
	uint8_t *sys_queue;
};

struct mb_mem_manager_table {
	uint8_t *spare_ble_buffer;
	uint8_t *spare_sys_buffer;
	uint8_t *ble_pool;
	uint32_t ble_pool_size;
	uint8_t *p_evt_free_buffer_queue;
	uint8_t *traces_evt_pool;
	uint32_t traces_pool_size;
};

struct mb_traces_table {
	uint8_t *traces_queue;
};

struct mb_ref_table {
	struct mb_device_info_table *p_device_info_table;
	struct mb_ble_table *p_ble_table;
	struct mb_thread_table *p_thread_table;
	struct mb_sys_table *p_sys_table;
	struct mb_mem_manager_table *p_mem_manager_table;
	struct mb_traces_table *p_traces_table;
	void *p_mac_802_15_4_table;
	void *p_zigbee_table;
	void *p_lld_tests_table;
	void *p_ble_lld_table;
};

/**
 * This table shall be used only in the case the CPU2 runs the FUS.
 * It is used by the command SHCI_GetWirelessFwInfo()
 */
struct mb_fus_device_info_table {
	uint32_t device_info_table_state;
	uint8_t reserved1;
	uint8_t last_fus_active_state;
	uint8_t last_wireless_stack_state;
	uint8_t current_wireless_stack_type;
	uint32_t safe_boot_version;
	uint32_t fus_version;
	uint32_t fus_memory_size;
	uint32_t wireless_stack_version;
	uint32_t wireless_stack_memory_size;
	uint32_t wireless_firmware_ble_info;
	uint32_t wireless_firmware_thread_info;
	uint32_t reserved2;
	uint64_t uid64;
	uint16_t device_id;
};

/**
 *********************************************************************************
 * IPCC CHANNELS
 *********************************************************************************
 */

/*  CPU1                                             CPU2
 *   |             (SYSTEM)                            |
 *   |----HW_IPCC_SYSTEM_CMD_RSP_CHANNEL-------------->|
 *   |                                                 |
 *   |<---HW_IPCC_SYSTEM_EVENT_CHANNEL-----------------|
 *   |                                                 |
 *   |            (ZIGBEE)                             |
 *   |----HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL------------>|
 *   |                                                 |
 *   |----HW_IPCC_ZIGBEE_CMD_CLI_CHANNEL-------------->|
 *   |                                                 |
 *   |<---HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL-------|
 *   |                                                 |
 *   |<---HW_IPCC_ZIGBEE_CLI_NOTIF_ACK_CHANNEL---------|
 *   |                                                 |
 *   |             (THREAD)                            |
 *   |----HW_IPCC_THREAD_OT_CMD_RSP_CHANNEL----------->|
 *   |                                                 |
 *   |----HW_IPCC_THREAD_CLI_CMD_CHANNEL-------------->|
 *   |                                                 |
 *   |<---HW_IPCC_THREAD_NOTIFICATION_ACK_CHANNEL------|
 *   |                                                 |
 *   |<---HW_IPCC_THREAD_CLI_NOTIFICATION_ACK_CHANNEL--|
 *   |                                                 |
 *   |             (BLE)                               |
 *   |----HW_IPCC_BLE_CMD_CHANNEL--------------------->|
 *   |                                                 |
 *   |----HW_IPCC_HCI_ACL_DATA_CHANNEL---------------->|
 *   |                                                 |
 *   |<---HW_IPCC_BLE_EVENT_CHANNEL--------------------|
 *   |                                                 |
 *   |             (BLE LLD)                           |
 *   |----HW_IPCC_BLE_LLD_CMD_CHANNEL----------------->|
 *   |                                                 |
 *   |<---HW_IPCC_BLE_LLD_RSP_CHANNEL------------------|
 *   |                                                 |
 *   |<---HW_IPCC_BLE_LLD_M0_CMD_CHANNEL---------------|
 *   |                                                 |
 *   |             (MAC)                               |
 *   |----HW_IPCC_MAC_802_15_4_CMD_RSP_CHANNEL-------->|
 *   |                                                 |
 *   |<---HW_IPCC_MAC_802_15_4_NOTIFICATION_ACK_CHANNEL|
 *   |                                                 |
 *   |             (BUFFER)                            |
 *   |----HW_IPCC_MM_RELEASE_BUFFER_CHANNE------------>|
 *   |                                                 |
 *   |             (TRACE)                             |
 *   |<----HW_IPCC_TRACES_CHANNEL----------------------|
 *   |                                                 |
 *
 *
 *
 */

/** CPU1 */
#define IPCC_BLE_CMD_CHANNEL 1
#define IPCC_SYSTEM_CMD_RSP_CHANNEL 2
#define IPCC_THREAD_OT_CMD_RSP_CHANNEL 3
#define IPCC_MM_RELEASE_BUFFER_CHANNEL 4
#define IPCC_THREAD_CLI_CMD_CHANNEL 5
#define IPCC_HCI_ACL_DATA_CHANNEL 6

/** CPU2 */
#define IPCC_BLE_EVENT_CHANNEL 1
#define IPCC_SYSTEM_EVENT_CHANNEL 2
#define IPCC_TRACES_CHANNEL 4

#endif
