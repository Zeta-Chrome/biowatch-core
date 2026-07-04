#include "shci.h"
#include "status.h"
#include "subsys/ble/tl/tl.h"
#include "subsys/ble/tl/tl_defs.h"
#include "sync/mutex.h"
#include "sync/semaphore.h"
#include "task/task.h"
#include <string.h>

PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static tl_cmd_packet_t g_sys_cmd_buffer;

static tl_sys_conf_t g_sys_conf;
static mutex_t g_cmd_mutex;
static semaphore_t g_cmd_rx_sem;

static void shci_cmd_callback();

void shci_init(evt_callback_t evt_callback)
{
    rtos_mutex_init(&g_cmd_mutex);
    rtos_semaphore_binary_init(&g_cmd_rx_sem);
    rtos_semaphore_take(&g_cmd_rx_sem, MAX_TIMEOUT);

    g_sys_conf.p_cmd_buffer = &g_sys_cmd_buffer;
    g_sys_conf.cmd_callback = shci_cmd_callback;
    g_sys_conf.evt_callback = evt_callback;
    tl_sys_init(&g_sys_conf);
}

bw_status_t shci_send(uint16_t cmd_code, uint8_t cmd_pl_len, uint8_t *p_cmd_pl, tl_evt_packet_t *p_rsp)
{
    rtos_mutex_lock(&g_cmd_mutex, MAX_TIMEOUT);

    g_sys_cmd_buffer.cmd_serial.cmd.cmd_code = cmd_code;
    g_sys_cmd_buffer.cmd_serial.cmd.payload_len = cmd_pl_len;
    memcpy(g_sys_cmd_buffer.cmd_serial.cmd.payload, p_cmd_pl, cmd_pl_len);

    tl_sys_send_cmd();
    bw_status_t status = rtos_semaphore_take(&g_cmd_rx_sem, 1000);
    if (status == STATUS_TIMEOUT)
    {
        BW_LOG("Timeout occured in shci_send\n");
        rtos_mutex_unlock(&g_cmd_mutex);
        return status;
    }

    memcpy(&(p_rsp->evt_serial),
           &g_sys_cmd_buffer,
           ((tl_evt_serial_t *)&g_sys_cmd_buffer)->evt.payload_len + TL_EVT_HDR_SIZE);

    rtos_mutex_unlock(&g_cmd_mutex);
    return status;
}

static void shci_cmd_callback()
{
    rtos_semaphore_give_from_isr(&g_cmd_rx_sem);
}

shci_cmd_status_t shci_c2_config(shci_c2_config_cmd_param_t *p_cmd_packet)
{
    uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
    tl_evt_packet_t *p_rsp;

    p_rsp = (tl_evt_packet_t *)local_buffer;

    shci_send(SHCI_OPCODE_C2_CONFIG, sizeof(shci_c2_config_cmd_param_t), (uint8_t *)p_cmd_packet, p_rsp);

    return (shci_cmd_status_t)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

shci_cmd_status_t shci_c2_reinit(void)
{
    uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
    tl_evt_packet_t *p_rsp;

    p_rsp = (tl_evt_packet_t *)local_buffer;

    shci_send(SHCI_OPCODE_C2_REINIT, 0, 0, p_rsp);

    return (shci_cmd_status_t)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

shci_cmd_status_t shci_c2_debug_init(shci_c2_debug_init_cmd_packet_t *p_cmd_packet)
{
    uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
    tl_evt_packet_t *p_rsp;

    p_rsp = (tl_evt_packet_t *)local_buffer;

    shci_send(SHCI_OPCODE_C2_DEBUG_INIT,
              sizeof(shci_c2_debug_init_cmd_param_t),
              (uint8_t *)&p_cmd_packet->param,
              p_rsp);

    return (shci_cmd_status_t)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

shci_cmd_status_t shci_c2_ble_init(shci_c2_ble_init_cmd_packet_t *p_cmd_packet)
{
    uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
    tl_evt_packet_t *p_rsp;

    p_rsp = (tl_evt_packet_t *)local_buffer;

    shci_send(SHCI_OPCODE_C2_BLE_INIT,
              sizeof(shci_c2_ble_init_cmd_param_t),
              (uint8_t *)&p_cmd_packet->param,
              p_rsp);

    return (shci_cmd_status_t)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

shci_cmd_status_t shci_c2_ble_allow_lp(uint8_t flag_radio_lp_on)
{
    uint8_t local_buffer[TL_BLEEVT_CC_BUFFER_SIZE];
    tl_evt_packet_t *p_rsp;

    p_rsp = (tl_evt_packet_t *)local_buffer;

    local_buffer[0] = 0;
    local_buffer[1] = flag_radio_lp_on;

    shci_send(SHCI_OPCODE_C2_RADIO_ALLOW_LOW_POWER, 2, local_buffer, p_rsp);

    return (shci_cmd_status_t)(((tl_cc_evt_t *)(p_rsp->evt_serial.evt.payload))->payload[0]);
}

shci_cmd_status_t shci_get_wireless_fw_info(wireless_fw_info_t *p_wireless_info)
{
    uint32_t ipccdba = 0;
    mb_ref_table_t *p_ref_table = NULL;
    uint32_t wireless_firmware_version = 0;
    uint32_t wireless_firmware_memorySize = 0;
    uint32_t wireless_firmware_infoStack = 0;
    mb_fus_device_info_table_t *p_fus_device_info_table = NULL;
    uint32_t fus_version = 0;
    uint32_t fus_memorySize = 0;

    ipccdba = FLASH->IPCCBR & FLASH_IPCCBR_IPCCDBA;

    /**
     * The Device Info Table mapping depends on which firmware is running on CPU2.
     * If the FUS is running on CPU2, FUS_DEVICE_INFO_TABLE_VALIDITY_KEYWORD shall be written in the table.
     * Otherwise, it means the Wireless Firmware is running on the CPU2
     */
    p_fus_device_info_table = (mb_fus_device_info_table_t *)(*(uint32_t *)((ipccdba << 2) + SRAM2A_BASE));

    if (p_fus_device_info_table->device_info_table_state == FUS_DEVICE_INFO_TABLE_VALIDITY_KEYWORD)
    {
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
    }
    else
    {
        /* The Wireless Firmware is running on CPU2 */
        p_ref_table = (mb_ref_table_t *)((ipccdba << 2) + SRAM2A_BASE);

        /**
         *  Retrieve the WirelessFwInfoTable
         *  This table is stored in RAM at startup during the TL (transport layer) initialization
         */
        wireless_firmware_version = p_ref_table->p_device_info_table->wireless_fw_info_table.version;
        wireless_firmware_memorySize = p_ref_table->p_device_info_table->wireless_fw_info_table.memory_size;
        wireless_firmware_infoStack = p_ref_table->p_device_info_table->wireless_fw_info_table.info_stack;

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
    p_wireless_info->fus_version_sub = ((fus_version & INFO_VERSION_SUB_MASK) >> INFO_VERSION_SUB_OFFSET);

    p_wireless_info->fus_memory_size_sram2b =
        ((fus_memorySize & INFO_SIZE_SRAM2B_MASK) >> INFO_SIZE_SRAM2B_OFFSET);
    p_wireless_info->fus_memory_size_sram2a =
        ((fus_memorySize & INFO_SIZE_SRAM2A_MASK) >> INFO_SIZE_SRAM2A_OFFSET);
    p_wireless_info->fus_memory_size_flash =
        ((fus_memorySize & INFO_SIZE_FLASH_MASK) >> INFO_SIZE_FLASH_OFFSET);

    return SHCI_SUCCESS;
}
