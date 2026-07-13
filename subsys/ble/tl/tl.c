#include "drivers/ipcc/ipcc.h"
#include "drivers/pwr/pwr.h"
#include "lib/containers/stm_clist.h"
#include "lib/utils.h"
#include "tl.h"
#include "tl_defs.h"

PLACE_IN_SECTION("MAPPING_TABLE") static volatile mb_ref_table_t g_mb_ref_table;
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static mb_device_info_table_t g_mb_device_info_table;
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static mb_ble_table_t g_mb_ble_table;
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static mb_sys_table_t g_mb_sys_table;
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static mb_mem_manager_table_t g_mb_mem_manager_table;
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static mb_traces_table_t g_mb_traces_table;

PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static stm_list_node_t g_sys_evt_queue;
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static stm_list_node_t g_free_buf_queue;
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static stm_list_node_t g_traces_evt_queue;
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static stm_list_node_t g_ble_evt_queue;
PLACE_IN_SECTION("MB_MEM2")
ALIGN(4) static uint8_t g_cs_buffer[sizeof(tl_packet_header_t) + TL_EVT_HDR_SIZE + sizeof(tl_cs_evt_t)];

static tl_sys_conf_t *g_p_sys_conf;
static ipcc_handle_t g_ipcc_sys_evt_h;
static ipcc_handle_t g_ipcc_sys_cmd_h;
static tl_mm_conf_t *g_p_mm_conf;
static stm_list_node_t g_local_free_buf_queue;
static ipcc_handle_t g_ipcc_mm_release_h;
static tl_traces_conf_t *g_p_traces_conf;
static ipcc_handle_t g_ipcc_traces_evt_h;
static tl_ble_conf_t *g_p_ble_conf;
static ipcc_handle_t g_ipcc_ble_evt_h;
static ipcc_handle_t g_ipcc_ble_cmd_h;
static ipcc_handle_t g_ipcc_ble_acl_ack_h;

static void tl_sys_evt_callback(void *user_data);
static void tl_sys_cmd_callback(void *user_data);
static void tl_mm_release_callback(void *user_data);
static void send_free_buffer();
static void tl_traces_evt_callback(void *user_data);
static void tl_ble_evt_callback(void *user_data);
static void tl_ble_acl_ack_callback(void *user_data);

void tl_init()
{
    g_mb_ref_table.p_device_info_table = &g_mb_device_info_table;
    g_mb_ref_table.p_ble_table = &g_mb_ble_table;
    g_mb_ref_table.p_sys_table = &g_mb_sys_table;
    g_mb_ref_table.p_mem_manager_table = &g_mb_mem_manager_table;
    g_mb_ref_table.p_traces_table = &g_mb_traces_table;

    ipcc_init(5, 4);
}

void tl_enable()
{
    pwr_boot_cpu2();
}

void tl_sys_init(tl_sys_conf_t *conf)
{
    g_p_sys_conf = conf;
    stm_list_init(&g_sys_evt_queue);

    g_mb_sys_table.p_cmd_buffer = (uint8_t *)conf->p_cmd_buffer;
    g_mb_sys_table.sys_queue = (uint8_t *)&g_sys_evt_queue;

    g_ipcc_sys_cmd_h.callback = tl_sys_cmd_callback;
    g_ipcc_sys_evt_h.callback = tl_sys_evt_callback;

    ipcc_rx(IPCC_SYSTEM_EVENT_CHANNEL, &g_ipcc_sys_evt_h);
}

static void tl_sys_evt_callback(void *user_data)
{
    (void)user_data;
    tl_evt_packet_t *p_sys_evt_packet;
    while (!stm_list_is_empty(&g_sys_evt_queue))
    {
        stm_list_remove_head(&g_sys_evt_queue, (stm_list_node_t **)&p_sys_evt_packet);
        g_p_sys_conf->evt_callback(p_sys_evt_packet);
    }
}

void tl_sys_send_cmd()
{
    ((tl_cmd_packet_t *)(g_mb_ref_table.p_sys_table->p_cmd_buffer))->cmd_serial.type = TL_SYSCMD_PKT_TYPE;
    ipcc_tx(IPCC_SYSTEM_CMD_RSP_CHANNEL, &g_ipcc_sys_cmd_h);
}

static void tl_sys_cmd_callback(void *user_data)
{
    (void)user_data;
    g_p_sys_conf->cmd_callback();
}

void tl_mm_init(tl_mm_conf_t *conf)
{
    g_p_mm_conf = conf;
    stm_list_init(&g_free_buf_queue);
    stm_list_init(&g_local_free_buf_queue);

    g_mb_mem_manager_table.ble_pool = conf->p_async_evt_pool;
    g_mb_mem_manager_table.ble_pool_size = conf->async_evt_pool_size;
    g_mb_mem_manager_table.p_evt_free_buffer_queue = (uint8_t *)&g_free_buf_queue;
    g_mb_mem_manager_table.spare_ble_buffer = conf->p_ble_spare_evt_buffer;
    g_mb_mem_manager_table.spare_sys_buffer = conf->p_sys_spare_evt_buffer;
    g_mb_mem_manager_table.traces_evt_pool = conf->p_traces_evt_pool;
    g_mb_mem_manager_table.traces_pool_size = conf->traces_evt_pool_size;

    g_ipcc_mm_release_h.callback = tl_mm_release_callback;
}

void tl_mm_evt_done(tl_evt_packet_t *p_hci_evt)
{
    stm_list_insert_tail(&g_local_free_buf_queue, (stm_list_node_t *)p_hci_evt);

    if (!ipcc_is_tx_channel_occupied(IPCC_MM_RELEASE_BUFFER_CHANNEL))
    {
        send_free_buffer();
        ipcc_tx(IPCC_MM_RELEASE_BUFFER_CHANNEL, &g_ipcc_mm_release_h);
    }
}

static void tl_mm_release_callback(void *user_data)
{
    (void)user_data;

    if (!stm_list_is_empty(&g_local_free_buf_queue))
    {
        send_free_buffer();
        ipcc_tx(IPCC_MM_RELEASE_BUFFER_CHANNEL, &g_ipcc_mm_release_h);
    }
}

static void send_free_buffer()
{
    stm_list_node_t *p_node;

    while (!stm_list_is_empty(&g_local_free_buf_queue))
    {
        stm_list_remove_head(&g_local_free_buf_queue, &p_node);
        stm_list_insert_tail(&g_free_buf_queue, p_node);
    }
}

void tl_traces_init(tl_traces_conf_t *conf)
{
    g_p_traces_conf = conf;
    stm_list_init(&g_traces_evt_queue);

    g_mb_traces_table.traces_queue = (uint8_t *)&g_traces_evt_queue;

    g_ipcc_traces_evt_h.callback = tl_traces_evt_callback;

    ipcc_rx(IPCC_TRACES_CHANNEL, &g_ipcc_traces_evt_h);
}

static void tl_traces_evt_callback(void *user_data)
{
    (void)user_data;
    tl_evt_packet_t *p_traces_evt_packet;
    while (!stm_list_is_empty(&g_traces_evt_queue))
    {
        stm_list_remove_head(&g_traces_evt_queue, (stm_list_node_t **)&p_traces_evt_packet);
        g_p_traces_conf->evt_callback(p_traces_evt_packet);
    }
}

void tl_ble_init(tl_ble_conf_t *conf)
{
    g_p_ble_conf = conf;
    stm_list_init(&g_ble_evt_queue);

    g_mb_ble_table.p_cmd_buffer = (uint8_t *)conf->p_cmd_buffer;
    g_mb_ble_table.p_hci_acl_data_buffer = (uint8_t *)conf->p_hci_acl_data_buffer;
    g_mb_ble_table.p_cs_buffer = (uint8_t *)g_cs_buffer;
    g_mb_ble_table.p_evt_queue = (uint8_t *)&g_ble_evt_queue;

    g_ipcc_ble_evt_h.callback = tl_ble_evt_callback;
    g_ipcc_ble_acl_ack_h.callback = tl_ble_acl_ack_callback;

    ipcc_rx(IPCC_BLE_EVENT_CHANNEL, &g_ipcc_ble_evt_h);
}

void tl_ble_send_cmd()
{
    ((tl_cmd_packet_t *)(g_mb_ref_table.p_ble_table->p_cmd_buffer))->cmd_serial.type = TL_BLECMD_PKT_TYPE;
    ipcc_tx_masked(IPCC_BLE_CMD_CHANNEL, &g_ipcc_ble_cmd_h);
}

static void tl_ble_evt_callback(void *user_data)
{
    (void)user_data;
    tl_evt_packet_t *p_ble_evt_packet;
    while (!stm_list_is_empty(&g_ble_evt_queue))
    {
        stm_list_remove_head(&g_ble_evt_queue, (stm_list_node_t **)&p_ble_evt_packet);
        g_p_ble_conf->evt_callback(p_ble_evt_packet);
    }
}

void tl_ble_send_acl_data()
{
    ((tl_acl_data_packet_t *)(g_mb_ref_table.p_ble_table->p_hci_acl_data_buffer))->acl_data_serial.type =
        TL_ACL_DATA_PKT_TYPE;

    ipcc_tx(IPCC_HCI_ACL_DATA_CHANNEL, &g_ipcc_ble_acl_ack_h);
}

static void tl_ble_acl_ack_callback(void *user_data)
{
    (void)user_data;
    if (g_p_ble_conf->acl_data_ack_callback)
    {
        g_p_ble_conf->acl_data_ack_callback();
    }
}
