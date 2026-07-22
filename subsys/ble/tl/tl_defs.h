#ifndef BLE_TL_DEFS_H
#define BLE_TL_DEFS_H

#include "lib/utils.h"

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
mb_wireless_info_table_t;

struct mb_device_info_table {
	mb_safe_boot_info_table_t safe_boot_info_table;
	mb_fus_info_table_t fus_info_table;
	mb_wireless_info_table_t wireless_fw_info_table;
};

struct mb_ble_table {
	uint8_t *p_cmd_buffer;
	uint8_t *p_cs_buffer;
	uint8_t *p_evt_queue;
	uint8_t *p_hci_acl_data_buffer;
};

struct mb_thread_table {
	uint8_t *notack_buffer;
	uint8_t *clicmdrsp_buffer;
	uint8_t *otcmdrsp_buffer;
	uint8_t *clinot_buffer;
};

struct mb_lld_tests_table {
	uint8_t *clicmdrsp_buffer;
	uint8_t *m0cmd_buffer;
};

struct mb_ble_lld_table {
	uint8_t *cmdrsp_buffer;
	uint8_t *m0cmd_buffer;
};

struct mb_zigbee_table {
	uint8_t *notifm0tom4_buffer;
	uint8_t *applicmdm4tom0_buffer;
	uint8_t *requestm0tom4_buffer;
};

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

struct mb_mac_802_15_4 {
	uint8_t *p_cmdrsp_buffer;
	uint8_t *p_notack_buffer;
	uint8_t *evt_queue;
};

struct mb_ref_table {
	struct mb_device_info_table *p_device_info_table;
	struct mb_ble_table *p_ble_table;
	struct mb_thread_table *p_thread_table;
	struct mb_sys_table *p_sys_table;
	struct mb_mem_manager_table *p_mem_manager_table;
	struct mb_traces_table *p_traces_table;
	struct mb_mac_802_15_4 *p_mac_802_15_4_table;
	struct mb_zigbee_table *p_zigbee_table;
	struct mb_lld_tests_table *p_lld_tests_table;
	struct mb_ble_lld_table *p_ble_lld_table;
};

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

struct mb_wireless_fw_info_table {
	// wireless info
	uint8_t version_major;
	uint8_t version_minor;
	uint8_t version_sub;
	uint8_t version_branch;
	uint8_t version_release_type;
	uint8_t memory_size_sram2b; /*< multiple of 1k */
	uint8_t memory_size_sram2a; /*< multiple of 1k */
	uint8_t memory_size_sram1; /*< multiple of 1k */
	uint8_t memory_size_flash; /*< multiple of 4k */
	uint8_t stack_type;
	// fus info
	uint8_t fus_version_major;
	uint8_t fus_version_minor;
	uint8_t fus_version_sub;
	uint8_t fus_memory_size_sram2b; /*< multiple of 1k */
	uint8_t fus_memory_size_sram2a; /*< multiple of 1k */
	uint8_t fus_memory_size_flash; /*< multiple of 4k */
};

struct mb_mm_conf_table {
	uint8_t *p_ble_spare_evt_buffer;
	uint8_t *p_sys_spare_evt_buffer;
	uint8_t *p_async_evt_pool;
	uint32_t async_evt_pool_size;
	uint8_t *p_traces_evt_pool;
	uint32_t traces_evt_pool_size;
};

/** CPU1 */
#define IPCC_BLE_CMD_CHANNEL 1
#define IPCC_SYSTEM_CMD_RSP_CHANNEL 2
#define IPCC_MM_RELEASE_BUFFER_CHANNEL 4
#define IPCC_HCI_ACL_DATA_CHANNEL 6

/** CPU2 */
#define IPCC_BLE_EVENT_CHANNEL 1
#define IPCC_SYSTEM_EVENT_CHANNEL 2
#define IPCC_TRACES_CHANNEL 4

#endif
