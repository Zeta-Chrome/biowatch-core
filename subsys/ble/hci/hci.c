#include "containers/stm_clist.h"
#include "hci.h"
#include "status.h"
#include "subsys/ble/tl/tl.h"
#include "sync/mutex.h"
#include "sync/semaphore.h"
#include "task/task.h"
#include "utils.h"
#include <string.h>

PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static tl_cmd_packet_t g_ble_cmd_buffer;

static tl_ble_conf_t g_ble_conf;
static mutex_t g_cmd_mutex;
static semaphore_t g_cmd_rx_sem;
static evt_callback_t g_evt_callback;
static stm_list_node_t g_cmd_evt_queue;

static void ble_evt_callback(tl_evt_packet_t *p_evt_pkt)
{
    if (p_evt_pkt->evt_serial.evt.evt_code == TL_BLEEVT_CC_OPCODE
        || p_evt_pkt->evt_serial.evt.evt_code == TL_BLEEVT_CS_OPCODE)
    {
        stm_list_insert_tail(&g_cmd_evt_queue, (stm_list_node_t *)p_evt_pkt);
        rtos_semaphore_give_from_isr(&g_cmd_rx_sem);
    }
    else
    {
        g_evt_callback(p_evt_pkt);
    }
}

void hci_init(evt_callback_t evt_callback)
{
    g_evt_callback = evt_callback;
    rtos_mutex_init(&g_cmd_mutex);
    stm_list_init(&g_cmd_evt_queue);
    rtos_semaphore_binary_init(&g_cmd_rx_sem);
    rtos_semaphore_take(&g_cmd_rx_sem, MAX_TIMEOUT);

    g_ble_conf.p_cmd_buffer = &g_ble_cmd_buffer;
    g_ble_conf.p_hci_acl_data_buffer = NULL;
    g_ble_conf.evt_callback = ble_evt_callback;
    g_ble_conf.acl_data_ack_callback = NULL;
    tl_ble_init(&g_ble_conf);
}

bw_status_t hci_send_req(hci_request_t *p_cmd)
{
    tl_cc_evt_t *p_cmd_complete_evt;
    tl_cs_evt_t *p_cmd_status_evt;
    tl_evt_packet_t *p_evt_pkt;
    uint16_t opcode = ((p_cmd->ocf) & 0x03ff) | ((p_cmd->ogf) << 10);

    rtos_mutex_lock(&g_cmd_mutex, MAX_TIMEOUT);

    g_ble_cmd_buffer.cmd_serial.cmd.cmd_code = opcode;
    g_ble_cmd_buffer.cmd_serial.cmd.payload_len = p_cmd->cmd_len;
    memcpy(g_ble_cmd_buffer.cmd_serial.cmd.payload, p_cmd->cmd_param, p_cmd->cmd_len);

    tl_ble_send_cmd();

    bool busy = true;
    while (busy)
    {
        bw_status_t status = rtos_semaphore_take(&g_cmd_rx_sem, 5000);
        if (status == STATUS_TIMEOUT)
        {
            BW_LOG("Timeout occured while waiting for hci request response\n");
            rtos_mutex_unlock(&g_cmd_mutex);
            return status;
        }

        while (!stm_list_is_empty(&g_cmd_evt_queue))
        {
            stm_list_remove_head(&g_cmd_evt_queue, (stm_list_node_t **)&p_evt_pkt);

            if (p_evt_pkt->evt_serial.evt.evt_code == TL_BLEEVT_CS_OPCODE)
            {
                p_cmd_status_evt = (tl_cs_evt_t *)p_evt_pkt->evt_serial.evt.payload;
                if (p_cmd_status_evt->cmd_code == opcode)
                {
                    *(uint8_t *)(p_cmd->ret_param) = p_cmd_status_evt->status;
                }

                if (p_cmd_status_evt->num_cmd != 0)
                {
                    busy = false;
                }
            }
            else
            {
                p_cmd_complete_evt = (tl_cc_evt_t *)p_evt_pkt->evt_serial.evt.payload;

                if (p_cmd_complete_evt->cmd_code == opcode)
                {
                    p_cmd->ret_len =
                        MIN(p_evt_pkt->evt_serial.evt.payload_len - TL_EVT_HDR_SIZE, p_cmd->ret_len);
                    memcpy(p_cmd->ret_param, p_cmd_complete_evt->payload, p_cmd->ret_len);
                }

                if (p_cmd_complete_evt->num_cmd != 0)
                {
                    busy = false;
                }
            }
        }
    }
    rtos_mutex_unlock(&g_cmd_mutex);

    return STATUS_OK;
}

ble_status_t hci_disconnect(uint16_t connection_handle, uint8_t reason)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_disconnect_cp0 *cp0 = (hci_disconnect_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;
    cp0->connection_handle = connection_handle;
    index_input += 2;
    cp0->reason = reason;
    index_input += 1;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x01;
    rq.ocf = 0x006;
    rq.event = 0x0f;
    rq.cmd_param = cmd_buffer;
    rq.cmd_len = index_input;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t hci_read_remote_version_information(uint16_t connection_handle)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_read_remote_version_information_cp0 *cp0 = (hci_read_remote_version_information_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;
    cp0->connection_handle = connection_handle;
    index_input += 2;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x01;
    rq.ocf = 0x01d;
    rq.event = 0x0f;
    rq.cmd_param = cmd_buffer;
    rq.cmd_len = index_input;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t hci_set_event_mask(const uint8_t *event_mask)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_set_event_mask_cp0 *cp0 = (hci_set_event_mask_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;
    memcpy((void *)&cp0->event_mask, (const void *)event_mask, 8);
    index_input += 8;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x03;
    rq.ocf = 0x001;
    rq.cmd_param = cmd_buffer;
    rq.cmd_len = index_input;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t hci_reset(void)
{
    hci_request_t rq;
    ble_status_t status = 0;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x03;
    rq.ocf = 0x003;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t
hci_read_transmit_power_level(uint16_t connection_handle, uint8_t type, uint8_t *transmit_power_level)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_read_transmit_power_level_cp0 *cp0 = (hci_read_transmit_power_level_cp0 *)(cmd_buffer);
    hci_read_transmit_power_level_rp0 resp;
    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->connection_handle = connection_handle;
    index_input += 2;
    cp0->type = type;
    index_input += 1;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x03;
    rq.ocf = 0x02d;
    rq.cmd_param = cmd_buffer;
    rq.cmd_len = index_input;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;
    *transmit_power_level = resp.transmit_power_level;
    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_set_controller_to_host_flow_control(uint8_t flow_control_enable)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_set_controller_to_host_flow_control_cp0 *cp0 =
        (hci_set_controller_to_host_flow_control_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;
    cp0->flow_control_enable = flow_control_enable;
    index_input += 1;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x03;
    rq.ocf = 0x031;
    rq.cmd_param = cmd_buffer;
    rq.cmd_len = index_input;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t hci_host_buffer_size(uint16_t host_acl_data_packet_length,
                                  uint8_t host_synchronous_data_packet_length,
                                  uint16_t host_total_num_acl_data_packets,
                                  uint16_t host_total_num_synchronous_data_packets)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_host_buffer_size_cp0 *cp0 = (hci_host_buffer_size_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;
    cp0->host_acl_data_packet_length = host_acl_data_packet_length;
    index_input += 2;
    cp0->host_synchronous_data_packet_length = host_synchronous_data_packet_length;
    index_input += 1;
    cp0->host_total_num_acl_data_packets = host_total_num_acl_data_packets;
    index_input += 2;
    cp0->host_total_num_synchronous_data_packets = host_total_num_synchronous_data_packets;
    index_input += 2;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x03;
    rq.ocf = 0x033;
    rq.cmd_param = cmd_buffer;
    rq.cmd_len = index_input;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t
hci_host_number_of_completed_packets(uint8_t number_of_handles,
                                     const host_nb_of_completed_pkt_pair_t *host_nb_of_completed_pkt_pair)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_host_number_of_completed_packets_cp0 *cp0 = (hci_host_number_of_completed_packets_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;
    cp0->number_of_handles = number_of_handles;
    index_input += 1;
    memcpy((void *)&cp0->host_nb_of_completed_pkt_pair,
           (const void *)host_nb_of_completed_pkt_pair,
           number_of_handles * (sizeof(host_nb_of_completed_pkt_pair_t)));
    index_input += number_of_handles * (sizeof(host_nb_of_completed_pkt_pair_t));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x03;
    rq.ocf = 0x035;
    rq.cmd_param = cmd_buffer;
    rq.cmd_len = index_input;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t hci_read_local_version_information(uint8_t *hci_version,
                                                uint16_t *hci_subversion,
                                                uint8_t *lmp_version,
                                                uint16_t *company_identifier,
                                                uint16_t *lmp_subversion)
{
    hci_request_t rq;
    hci_read_local_version_information_rp0 resp;
    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x04;
    rq.ocf = 0x001;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;
    *hci_version = resp.hci_version;
    *hci_subversion = resp.hci_subversion;
    *lmp_version = resp.lmp_version;
    *company_identifier = resp.company_identifier;
    *lmp_subversion = resp.lmp_subversion;
    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_read_local_supported_commands(uint8_t *supported_commands)
{
    hci_request_t rq;
    hci_read_local_supported_commands_rp0 resp;
    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x04;
    rq.ocf = 0x002;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;
    memcpy((void *)supported_commands, (const void *)resp.supported_commands, 64);
    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_read_local_supported_features(uint8_t *lmp_features)
{
    hci_request_t rq;
    hci_read_local_supported_features_rp0 resp;
    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x04;
    rq.ocf = 0x003;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;
    memcpy((void *)lmp_features, (const void *)resp.lmp_features, 8);
    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_read_bd_addr(uint8_t *bd_addr)
{
    hci_request_t rq;
    hci_read_bd_addr_rp0 resp;
    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x04;
    rq.ocf = 0x009;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;
    memcpy((void *)bd_addr, (const void *)resp.bd_addr, 6);
    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_read_rssi(uint16_t connection_handle, uint8_t *rssi)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_read_rssi_cp0 *cp0 = (hci_read_rssi_cp0 *)(cmd_buffer);
    hci_read_rssi_rp0 resp;
    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->connection_handle = connection_handle;
    index_input += 2;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x05;
    rq.ocf = 0x005;
    rq.cmd_param = cmd_buffer;
    rq.cmd_len = index_input;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;
    *rssi = resp.rssi;
    return BLE_STATUS_SUCCESS;
}
