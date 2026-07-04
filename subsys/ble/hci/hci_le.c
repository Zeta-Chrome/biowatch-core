#include "hci_le.h"
#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"
#include <string.h>

ble_status_t hci_le_set_event_mask(const uint8_t *le_event_mask)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_event_mask_cp0 *cp0 = (hci_le_set_event_mask_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    memcpy((void *)&cp0->le_event_mask, (const void *)le_event_mask, 8);
    index_input += 8;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
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

ble_status_t hci_le_read_buffer_size(uint16_t *hc_le_acl_data_packet_length,
                                     uint8_t *hc_total_num_le_acl_data_packets)
{
    hci_request_t rq;
    hci_le_read_buffer_size_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x002;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *hc_le_acl_data_packet_length = resp.hc_le_acl_data_packet_length;
    *hc_total_num_le_acl_data_packets = resp.hc_total_num_le_acl_data_packets;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_read_local_supported_features_page_0(uint8_t *le_features)
{
    hci_request_t rq;
    hci_le_read_local_supported_features_page_0_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x003;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    memcpy((void *)le_features, (const void *)resp.le_features, 8);

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_set_random_address(const uint8_t *random_address)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_random_address_cp0 *cp0 = (hci_le_set_random_address_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    memcpy((void *)&cp0->random_address, (const void *)random_address, 6);
    index_input += 6;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x005;
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

ble_status_t hci_le_set_advertising_parameters(uint16_t advertising_interval_min,
                                               uint16_t advertising_interval_max,
                                               uint8_t advertising_type,
                                               uint8_t own_address_type,
                                               uint8_t peer_address_type,
                                               const uint8_t *peer_address,
                                               uint8_t advertising_channel_map,
                                               uint8_t advertising_filter_policy)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_advertising_parameters_cp0 *cp0 = (hci_le_set_advertising_parameters_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->advertising_interval_min = advertising_interval_min;
    index_input += 2;
    cp0->advertising_interval_max = advertising_interval_max;
    index_input += 2;
    cp0->advertising_type = advertising_type;
    index_input += 1;
    cp0->own_address_type = own_address_type;
    index_input += 1;
    cp0->peer_address_type = peer_address_type;
    index_input += 1;
    memcpy((void *)&cp0->peer_address, (const void *)peer_address, 6);
    index_input += 6;
    cp0->advertising_channel_map = advertising_channel_map;
    index_input += 1;
    cp0->advertising_filter_policy = advertising_filter_policy;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x006;
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

ble_status_t hci_le_read_advertising_physical_channel_tx_power(uint8_t *transmit_power_level)
{
    hci_request_t rq;
    hci_le_read_advertising_physical_channel_tx_power_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x007;
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

ble_status_t hci_le_set_advertising_data(uint8_t advertising_data_length, const uint8_t *advertising_data)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_advertising_data_cp0 *cp0 = (hci_le_set_advertising_data_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->advertising_data_length = advertising_data_length;
    index_input += 1;
    memcpy((void *)&cp0->advertising_data, (const void *)advertising_data, 31);
    index_input += 31;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x008;
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

ble_status_t hci_le_set_scan_response_data(uint8_t scan_response_data_length,
                                           const uint8_t *scan_response_data)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_scan_response_data_cp0 *cp0 = (hci_le_set_scan_response_data_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->scan_response_data_length = scan_response_data_length;
    index_input += 1;
    memcpy((void *)&cp0->scan_response_data, (const void *)scan_response_data, 31);
    index_input += 31;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x009;
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

ble_status_t hci_le_set_advertising_enable(uint8_t advertising_enable)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_advertising_enable_cp0 *cp0 = (hci_le_set_advertising_enable_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->advertising_enable = advertising_enable;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x00a;
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

ble_status_t hci_le_set_scan_parameters(uint8_t le_scan_type,
                                        uint16_t le_scan_interval,
                                        uint16_t le_scan_window,
                                        uint8_t own_address_type,
                                        uint8_t scanning_filter_policy)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_scan_parameters_cp0 *cp0 = (hci_le_set_scan_parameters_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->le_scan_type = le_scan_type;
    index_input += 1;
    cp0->le_scan_interval = le_scan_interval;
    index_input += 2;
    cp0->le_scan_window = le_scan_window;
    index_input += 2;
    cp0->own_address_type = own_address_type;
    index_input += 1;
    cp0->scanning_filter_policy = scanning_filter_policy;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x00b;
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

ble_status_t hci_le_set_scan_enable(uint8_t le_scan_enable, uint8_t filter_duplicates)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_scan_enable_cp0 *cp0 = (hci_le_set_scan_enable_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->le_scan_enable = le_scan_enable;
    index_input += 1;
    cp0->filter_duplicates = filter_duplicates;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x00c;
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

ble_status_t hci_le_create_connection(uint16_t le_scan_interval,
                                      uint16_t le_scan_window,
                                      uint8_t initiator_filter_policy,
                                      uint8_t peer_address_type,
                                      const uint8_t *peer_address,
                                      uint8_t own_address_type,
                                      uint16_t conn_interval_min,
                                      uint16_t conn_interval_max,
                                      uint16_t conn_latency,
                                      uint16_t supervision_timeout,
                                      uint16_t minimum_ce_length,
                                      uint16_t maximum_ce_length)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_create_connection_cp0 *cp0 = (hci_le_create_connection_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->le_scan_interval = le_scan_interval;
    index_input += 2;
    cp0->le_scan_window = le_scan_window;
    index_input += 2;
    cp0->initiator_filter_policy = initiator_filter_policy;
    index_input += 1;
    cp0->peer_address_type = peer_address_type;
    index_input += 1;
    memcpy((void *)&cp0->peer_address, (const void *)peer_address, 6);
    index_input += 6;
    cp0->own_address_type = own_address_type;
    index_input += 1;
    cp0->conn_interval_min = conn_interval_min;
    index_input += 2;
    cp0->conn_interval_max = conn_interval_max;
    index_input += 2;
    cp0->conn_latency = conn_latency;
    index_input += 2;
    cp0->supervision_timeout = supervision_timeout;
    index_input += 2;
    cp0->minimum_ce_length = minimum_ce_length;
    index_input += 2;
    cp0->maximum_ce_length = maximum_ce_length;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x00d;
    rq.event = 0x0F;
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

ble_status_t hci_le_create_connection_cancel(void)
{
    hci_request_t rq;
    ble_status_t status = 0;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x00e;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t hci_le_read_filter_accept_list_size(uint8_t *filter_accept_list_size)
{
    hci_request_t rq;
    hci_le_read_filter_accept_list_size_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x00f;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *filter_accept_list_size = resp.filter_accept_list_size;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_clear_filter_accept_list(void)
{
    hci_request_t rq;
    ble_status_t status = 0;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x010;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t hci_le_add_device_to_filter_accept_list(uint8_t address_type, const uint8_t *address)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_add_device_to_filter_accept_list_cp0 *cp0 =
        (hci_le_add_device_to_filter_accept_list_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->address_type = address_type;
    index_input += 1;
    memcpy((void *)&cp0->address, (const void *)address, 6);
    index_input += 6;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x011;
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

ble_status_t hci_le_remove_device_from_filter_accept_list(uint8_t address_type, const uint8_t *address)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_remove_device_from_filter_accept_list_cp0 *cp0 =
        (hci_le_remove_device_from_filter_accept_list_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->address_type = address_type;
    index_input += 1;
    memcpy((void *)&cp0->address, (const void *)address, 6);
    index_input += 6;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x012;
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

ble_status_t hci_le_connection_update(uint16_t connection_handle,
                                      uint16_t conn_interval_min,
                                      uint16_t conn_interval_max,
                                      uint16_t conn_latency,
                                      uint16_t supervision_timeout,
                                      uint16_t minimum_ce_length,
                                      uint16_t maximum_ce_length)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_connection_update_cp0 *cp0 = (hci_le_connection_update_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->connection_handle = connection_handle;
    index_input += 2;
    cp0->conn_interval_min = conn_interval_min;
    index_input += 2;
    cp0->conn_interval_max = conn_interval_max;
    index_input += 2;
    cp0->conn_latency = conn_latency;
    index_input += 2;
    cp0->supervision_timeout = supervision_timeout;
    index_input += 2;
    cp0->minimum_ce_length = minimum_ce_length;
    index_input += 2;
    cp0->maximum_ce_length = maximum_ce_length;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x013;
    rq.event = 0x0F;
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

ble_status_t hci_le_set_host_channel_classification(const uint8_t *le_channel_map)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_host_channel_classification_cp0 *cp0 =
        (hci_le_set_host_channel_classification_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    memcpy((void *)&cp0->le_channel_map, (const void *)le_channel_map, 5);
    index_input += 5;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x014;
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

ble_status_t hci_le_read_channel_map(uint16_t connection_handle, uint8_t *le_channel_map)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_read_channel_map_cp0 *cp0 = (hci_le_read_channel_map_cp0 *)(cmd_buffer);
    hci_le_read_channel_map_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->connection_handle = connection_handle;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x015;
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

    memcpy((void *)le_channel_map, (const void *)resp.le_channel_map, 5);

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_read_remote_features_page_0(uint16_t connection_handle)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_read_remote_features_page_0_cp0 *cp0 = (hci_le_read_remote_features_page_0_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->connection_handle = connection_handle;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x016;
    rq.event = 0x0F;
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

ble_status_t hci_le_encrypt(const uint8_t *key, const uint8_t *plaintext_data, uint8_t *encrypted_data)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_encrypt_cp0 *cp0 = (hci_le_encrypt_cp0 *)(cmd_buffer);
    hci_le_encrypt_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    memcpy((void *)&cp0->key, (const void *)key, 16);
    index_input += 16;
    memcpy((void *)&cp0->plaintext_data, (const void *)plaintext_data, 16);
    index_input += 16;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x017;
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

    memcpy((void *)encrypted_data, (const void *)resp.encrypted_data, 16);

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_rand(uint8_t *random_number)
{
    hci_request_t rq;
    hci_le_rand_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x018;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    memcpy((void *)random_number, (const void *)resp.random_number, 8);

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_enable_encryption(uint16_t connection_handle,
                                      const uint8_t *random_number,
                                      uint16_t encrypted_diversifier,
                                      const uint8_t *long_term_key)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_enable_encryption_cp0 *cp0 = (hci_le_enable_encryption_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->connection_handle = connection_handle;
    index_input += 2;
    memcpy((void *)&cp0->random_number, (const void *)random_number, 8);
    index_input += 8;
    cp0->encrypted_diversifier = encrypted_diversifier;
    index_input += 2;
    memcpy((void *)&cp0->long_term_key, (const void *)long_term_key, 16);
    index_input += 16;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x019;
    rq.event = 0x0F;
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

ble_status_t hci_le_long_term_key_request_reply(uint16_t connection_handle, const uint8_t *long_term_key)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_long_term_key_request_reply_cp0 *cp0 = (hci_le_long_term_key_request_reply_cp0 *)(cmd_buffer);
    hci_le_long_term_key_request_reply_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->connection_handle = connection_handle;
    index_input += 2;
    memcpy((void *)&cp0->long_term_key, (const void *)long_term_key, 16);
    index_input += 16;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x01a;
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

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_long_term_key_request_negative_reply(uint16_t connection_handle)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_long_term_key_request_negative_reply_cp0 *cp0 =
        (hci_le_long_term_key_request_negative_reply_cp0 *)(cmd_buffer);
    hci_le_long_term_key_request_negative_reply_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->connection_handle = connection_handle;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x01b;
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

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_read_supported_states(uint8_t *le_states)
{
    hci_request_t rq;
    hci_le_read_supported_states_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x01c;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    memcpy((void *)le_states, (const void *)resp.le_states, 8);

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_receiver_test(uint8_t rx_frequency)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_receiver_test_cp0 *cp0 = (hci_le_receiver_test_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->rx_frequency = rx_frequency;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x01d;
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
hci_le_transmitter_test(uint8_t tx_frequency, uint8_t length_of_test_data, uint8_t packet_payload)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_transmitter_test_cp0 *cp0 = (hci_le_transmitter_test_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->tx_frequency = tx_frequency;
    index_input += 1;
    cp0->length_of_test_data = length_of_test_data;
    index_input += 1;
    cp0->packet_payload = packet_payload;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x01e;
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

ble_status_t hci_le_test_end(uint16_t *number_of_packets)
{
    hci_request_t rq;
    hci_le_test_end_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x01f;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *number_of_packets = resp.number_of_packets;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_set_data_length(uint16_t connection_handle, uint16_t tx_octets, uint16_t tx_time)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_data_length_cp0 *cp0 = (hci_le_set_data_length_cp0 *)(cmd_buffer);
    hci_le_set_data_length_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->connection_handle = connection_handle;
    index_input += 2;
    cp0->tx_octets = tx_octets;
    index_input += 2;
    cp0->tx_time = tx_time;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x022;
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

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_read_suggested_default_data_length(uint16_t *suggested_max_tx_octets,
                                                       uint16_t *suggested_max_tx_time)
{
    hci_request_t rq;
    hci_le_read_suggested_default_data_length_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x023;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *suggested_max_tx_octets = resp.suggested_max_tx_octets;
    *suggested_max_tx_time = resp.suggested_max_tx_time;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_write_suggested_default_data_length(uint16_t suggested_max_tx_octets,
                                                        uint16_t suggested_max_tx_time)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_write_suggested_default_data_length_cp0 *cp0 =
        (hci_le_write_suggested_default_data_length_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->suggested_max_tx_octets = suggested_max_tx_octets;
    index_input += 2;
    cp0->suggested_max_tx_time = suggested_max_tx_time;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x024;
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

ble_status_t hci_le_read_local_p256_public_key(void)
{
    hci_request_t rq;
    ble_status_t status = 0;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x025;
    rq.event = 0x0F;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t hci_le_generate_dhkey(const uint8_t *remote_p256_public_key)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_generate_dhkey_cp0 *cp0 = (hci_le_generate_dhkey_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    memcpy((void *)&cp0->remote_p256_public_key, (const void *)remote_p256_public_key, 64);
    index_input += 64;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x026;
    rq.event = 0x0F;
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

ble_status_t hci_le_add_device_to_resolving_list(uint8_t peer_identity_address_type,
                                                 const uint8_t *peer_identity_address,
                                                 const uint8_t *peer_irk,
                                                 const uint8_t *local_irk)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_add_device_to_resolving_list_cp0 *cp0 = (hci_le_add_device_to_resolving_list_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->peer_identity_address_type = peer_identity_address_type;
    index_input += 1;
    memcpy((void *)&cp0->peer_identity_address, (const void *)peer_identity_address, 6);
    index_input += 6;
    memcpy((void *)&cp0->peer_irk, (const void *)peer_irk, 16);
    index_input += 16;
    memcpy((void *)&cp0->local_irk, (const void *)local_irk, 16);
    index_input += 16;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x027;
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

ble_status_t hci_le_remove_device_from_resolving_list(uint8_t peer_identity_address_type,
                                                      const uint8_t *peer_identity_address)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_remove_device_from_resolving_list_cp0 *cp0 =
        (hci_le_remove_device_from_resolving_list_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->peer_identity_address_type = peer_identity_address_type;
    index_input += 1;
    memcpy((void *)&cp0->peer_identity_address, (const void *)peer_identity_address, 6);
    index_input += 6;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x028;
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

ble_status_t hci_le_clear_resolving_list(void)
{
    hci_request_t rq;
    ble_status_t status = 0;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x029;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t hci_le_read_resolving_list_size(uint8_t *resolving_list_size)
{
    hci_request_t rq;
    hci_le_read_resolving_list_size_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x02a;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *resolving_list_size = resp.resolving_list_size;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_read_peer_resolvable_address(uint8_t peer_identity_address_type,
                                                 const uint8_t *peer_identity_address,
                                                 uint8_t *peer_resolvable_address)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_read_peer_resolvable_address_cp0 *cp0 = (hci_le_read_peer_resolvable_address_cp0 *)(cmd_buffer);
    hci_le_read_peer_resolvable_address_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->peer_identity_address_type = peer_identity_address_type;
    index_input += 1;
    memcpy((void *)&cp0->peer_identity_address, (const void *)peer_identity_address, 6);
    index_input += 6;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x02b;
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

    memcpy((void *)peer_resolvable_address, (const void *)resp.peer_resolvable_address, 6);

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_read_local_resolvable_address(uint8_t peer_identity_address_type,
                                                  const uint8_t *peer_identity_address,
                                                  uint8_t *local_resolvable_address)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_read_local_resolvable_address_cp0 *cp0 = (hci_le_read_local_resolvable_address_cp0 *)(cmd_buffer);
    hci_le_read_local_resolvable_address_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->peer_identity_address_type = peer_identity_address_type;
    index_input += 1;
    memcpy((void *)&cp0->peer_identity_address, (const void *)peer_identity_address, 6);
    index_input += 6;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x02c;
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

    memcpy((void *)local_resolvable_address, (const void *)resp.local_resolvable_address, 6);

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_set_address_resolution_enable(uint8_t address_resolution_enable)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_address_resolution_enable_cp0 *cp0 = (hci_le_set_address_resolution_enable_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->address_resolution_enable = address_resolution_enable;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x02d;
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

ble_status_t hci_le_set_resolvable_private_address_timeout(uint16_t rpa_timeout)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_resolvable_private_address_timeout_cp0 *cp0 =
        (hci_le_set_resolvable_private_address_timeout_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->rpa_timeout = rpa_timeout;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x02e;
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

ble_status_t hci_le_read_maximum_data_length(uint16_t *supported_max_tx_octets,
                                             uint16_t *supported_max_tx_time,
                                             uint16_t *supported_max_rx_octets,
                                             uint16_t *supported_max_rx_time)
{
    hci_request_t rq;
    hci_le_read_maximum_data_length_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x02f;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *supported_max_tx_octets = resp.supported_max_tx_octets;
    *supported_max_tx_time = resp.supported_max_tx_time;
    *supported_max_rx_octets = resp.supported_max_rx_octets;
    *supported_max_rx_time = resp.supported_max_rx_time;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_read_phy(uint16_t connection_handle, uint8_t *tx_phy, uint8_t *rx_phy)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_read_phy_cp0 *cp0 = (hci_le_read_phy_cp0 *)(cmd_buffer);
    hci_le_read_phy_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->connection_handle = connection_handle;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x030;
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

    *tx_phy = resp.tx_phy;
    *rx_phy = resp.rx_phy;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_set_default_phy(uint8_t all_phys, uint8_t tx_phys, uint8_t rx_phys)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_default_phy_cp0 *cp0 = (hci_le_set_default_phy_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->all_phys = all_phys;
    index_input += 1;
    cp0->tx_phys = tx_phys;
    index_input += 1;
    cp0->rx_phys = rx_phys;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
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

ble_status_t hci_le_set_phy(uint16_t connection_handle,
                            uint8_t all_phys,
                            uint8_t tx_phys,
                            uint8_t rx_phys,
                            uint16_t phy_options)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_phy_cp0 *cp0 = (hci_le_set_phy_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->connection_handle = connection_handle;
    index_input += 2;
    cp0->all_phys = all_phys;
    index_input += 1;
    cp0->tx_phys = tx_phys;
    index_input += 1;
    cp0->rx_phys = rx_phys;
    index_input += 1;
    cp0->phy_options = phy_options;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x032;
    rq.event = 0x0F;
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

ble_status_t hci_le_receiver_test_v2(uint8_t rx_frequency, uint8_t phy, uint8_t modulation_index)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_receiver_test_v2_cp0 *cp0 = (hci_le_receiver_test_v2_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->rx_frequency = rx_frequency;
    index_input += 1;
    cp0->phy = phy;
    index_input += 1;
    cp0->modulation_index = modulation_index;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
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

ble_status_t hci_le_transmitter_test_v2(uint8_t tx_frequency,
                                        uint8_t length_of_test_data,
                                        uint8_t packet_payload,
                                        uint8_t phy)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_transmitter_test_v2_cp0 *cp0 = (hci_le_transmitter_test_v2_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->tx_frequency = tx_frequency;
    index_input += 1;
    cp0->length_of_test_data = length_of_test_data;
    index_input += 1;
    cp0->packet_payload = packet_payload;
    index_input += 1;
    cp0->phy = phy;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x034;
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

ble_status_t hci_le_set_advertising_set_random_address(uint8_t advertising_handle,
                                                       const uint8_t *random_address)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_advertising_set_random_address_cp0 *cp0 =
        (hci_le_set_advertising_set_random_address_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->advertising_handle = advertising_handle;
    index_input += 1;
    memcpy((void *)&cp0->random_address, (const void *)random_address, 6);
    index_input += 6;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
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

ble_status_t hci_le_set_extended_advertising_parameters(uint8_t advertising_handle,
                                                        uint16_t adv_event_properties,
                                                        const uint8_t *primary_adv_interval_min,
                                                        const uint8_t *primary_adv_interval_max,
                                                        uint8_t primary_adv_channel_map,
                                                        uint8_t own_address_type,
                                                        uint8_t peer_address_type,
                                                        const uint8_t *peer_address,
                                                        uint8_t adv_filter_policy,
                                                        uint8_t adv_tx_power,
                                                        uint8_t primary_adv_phy,
                                                        uint8_t secondary_adv_max_skip,
                                                        uint8_t secondary_adv_phy,
                                                        uint8_t adv_sid,
                                                        uint8_t scan_req_notification_enable,
                                                        uint8_t *selected_tx_power)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_extended_advertising_parameters_cp0 *cp0 =
        (hci_le_set_extended_advertising_parameters_cp0 *)(cmd_buffer);
    hci_le_set_extended_advertising_parameters_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->advertising_handle = advertising_handle;
    index_input += 1;
    cp0->adv_event_properties = adv_event_properties;
    index_input += 2;
    memcpy((void *)&cp0->primary_adv_interval_min, (const void *)primary_adv_interval_min, 3);
    index_input += 3;
    memcpy((void *)&cp0->primary_adv_interval_max, (const void *)primary_adv_interval_max, 3);
    index_input += 3;
    cp0->primary_adv_channel_map = primary_adv_channel_map;
    index_input += 1;
    cp0->own_address_type = own_address_type;
    index_input += 1;
    cp0->peer_address_type = peer_address_type;
    index_input += 1;
    memcpy((void *)&cp0->peer_address, (const void *)peer_address, 6);
    index_input += 6;
    cp0->adv_filter_policy = adv_filter_policy;
    index_input += 1;
    cp0->adv_tx_power = adv_tx_power;
    index_input += 1;
    cp0->primary_adv_phy = primary_adv_phy;
    index_input += 1;
    cp0->secondary_adv_max_skip = secondary_adv_max_skip;
    index_input += 1;
    cp0->secondary_adv_phy = secondary_adv_phy;
    index_input += 1;
    cp0->adv_sid = adv_sid;
    index_input += 1;
    cp0->scan_req_notification_enable = scan_req_notification_enable;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x036;
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

    *selected_tx_power = resp.selected_tx_power;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_set_extended_advertising_data(uint8_t advertising_handle,
                                                  uint8_t operation,
                                                  uint8_t fragment_preference,
                                                  uint8_t advertising_data_length,
                                                  const uint8_t *advertising_data)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_extended_advertising_data_cp0 *cp0 = (hci_le_set_extended_advertising_data_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->advertising_handle = advertising_handle;
    index_input += 1;
    cp0->operation = operation;
    index_input += 1;
    cp0->fragment_preference = fragment_preference;
    index_input += 1;
    cp0->advertising_data_length = advertising_data_length;
    index_input += 1;
    memcpy((void *)&cp0->advertising_data, (const void *)advertising_data, advertising_data_length);
    index_input += advertising_data_length;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x037;
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

ble_status_t hci_le_set_extended_scan_response_data(uint8_t advertising_handle,
                                                    uint8_t operation,
                                                    uint8_t fragment_preference,
                                                    uint8_t scan_response_data_length,
                                                    const uint8_t *scan_response_data)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_extended_scan_response_data_cp0 *cp0 =
        (hci_le_set_extended_scan_response_data_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->advertising_handle = advertising_handle;
    index_input += 1;
    cp0->operation = operation;
    index_input += 1;
    cp0->fragment_preference = fragment_preference;
    index_input += 1;
    cp0->scan_response_data_length = scan_response_data_length;
    index_input += 1;
    memcpy((void *)&cp0->scan_response_data, (const void *)scan_response_data, scan_response_data_length);
    index_input += scan_response_data_length;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x038;
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
hci_le_set_extended_advertising_enable(uint8_t enable, uint8_t num_sets, const adv_set_t *adv_set)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_extended_advertising_enable_cp0 *cp0 =
        (hci_le_set_extended_advertising_enable_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->enable = enable;
    index_input += 1;
    cp0->num_sets = num_sets;
    index_input += 1;
    memcpy((void *)&cp0->adv_set, (const void *)adv_set, num_sets * (sizeof(adv_set_t)));
    index_input += num_sets * (sizeof(adv_set_t));

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x039;
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

ble_status_t hci_le_read_maximum_advertising_data_length(uint16_t *max_advertising_data_length)
{
    hci_request_t rq;
    hci_le_read_maximum_advertising_data_length_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x03a;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *max_advertising_data_length = resp.max_advertising_data_length;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_read_number_of_supported_advertising_sets(uint8_t *num_supported_advertising_sets)
{
    hci_request_t rq;
    hci_le_read_number_of_supported_advertising_sets_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x03b;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *num_supported_advertising_sets = resp.num_supported_advertising_sets;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_remove_advertising_set(uint8_t advertising_handle)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_remove_advertising_set_cp0 *cp0 = (hci_le_remove_advertising_set_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->advertising_handle = advertising_handle;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x03c;
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

ble_status_t hci_le_clear_advertising_sets(void)
{
    hci_request_t rq;
    ble_status_t status = 0;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x03d;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t hci_le_set_extended_scan_parameters(uint8_t own_address_type,
                                                 uint8_t scanning_filter_policy,
                                                 uint8_t scanning_phys,
                                                 const scan_param_phy_t *scan_param_phy)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_extended_scan_parameters_cp0 *cp0 = (hci_le_set_extended_scan_parameters_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->own_address_type = own_address_type;
    index_input += 1;
    cp0->scanning_filter_policy = scanning_filter_policy;
    index_input += 1;
    cp0->scanning_phys = scanning_phys;
    index_input += 1;
    memcpy((void *)&cp0->scan_param_phy, (const void *)scan_param_phy, 10);
    index_input += 10;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x041;
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
hci_le_set_extended_scan_enable(uint8_t enable, uint8_t filter_duplicates, uint16_t duration, uint16_t period)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_extended_scan_enable_cp0 *cp0 = (hci_le_set_extended_scan_enable_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->enable = enable;
    index_input += 1;
    cp0->filter_duplicates = filter_duplicates;
    index_input += 1;
    cp0->duration = duration;
    index_input += 2;
    cp0->period = period;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x042;
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

ble_status_t hci_le_extended_create_connection(uint8_t initiator_filter_policy,
                                               uint8_t own_address_type,
                                               uint8_t peer_address_type,
                                               const uint8_t *peer_address,
                                               uint8_t initiating_phys,
                                               const init_param_phy_t *init_param_phy)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_extended_create_connection_cp0 *cp0 = (hci_le_extended_create_connection_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->initiator_filter_policy = initiator_filter_policy;
    index_input += 1;
    cp0->own_address_type = own_address_type;
    index_input += 1;
    cp0->peer_address_type = peer_address_type;
    index_input += 1;
    memcpy((void *)&cp0->peer_address, (const void *)peer_address, 6);
    index_input += 6;
    cp0->initiating_phys = initiating_phys;
    index_input += 1;
    memcpy((void *)&cp0->init_param_phy, (const void *)init_param_phy, 48);
    index_input += 48;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x043;
    rq.event = 0x0F;
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

ble_status_t hci_le_read_transmit_power(uint8_t *min_tx_power, uint8_t *max_tx_power)
{
    hci_request_t rq;
    hci_le_read_transmit_power_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x04b;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *min_tx_power = resp.min_tx_power;
    *max_tx_power = resp.max_tx_power;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_read_rf_path_compensation(uint16_t *rf_tx_path_compensation,
                                              uint16_t *rf_rx_path_compensation)
{
    hci_request_t rq;
    hci_le_read_rf_path_compensation_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x04c;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *rf_tx_path_compensation = resp.rf_tx_path_compensation;
    *rf_rx_path_compensation = resp.rf_rx_path_compensation;

    return BLE_STATUS_SUCCESS;
}

ble_status_t hci_le_write_rf_path_compensation(uint16_t rf_tx_path_compensation,
                                               uint16_t rf_rx_path_compensation)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_write_rf_path_compensation_cp0 *cp0 = (hci_le_write_rf_path_compensation_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->rf_tx_path_compensation = rf_tx_path_compensation;
    index_input += 2;
    cp0->rf_rx_path_compensation = rf_rx_path_compensation;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x04d;
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

ble_status_t hci_le_set_privacy_mode(uint8_t peer_identity_address_type,
                                     const uint8_t *peer_identity_address,
                                     uint8_t privacy_mode)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_privacy_mode_cp0 *cp0 = (hci_le_set_privacy_mode_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->peer_identity_address_type = peer_identity_address_type;
    index_input += 1;
    memcpy((void *)&cp0->peer_identity_address, (const void *)peer_identity_address, 6);
    index_input += 6;
    cp0->privacy_mode = privacy_mode;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x04e;
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

ble_status_t hci_le_generate_dhkey_v2(const uint8_t *remote_p256_public_key, uint8_t key_type)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_generate_dhkey_v2_cp0 *cp0 = (hci_le_generate_dhkey_v2_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    memcpy((void *)&cp0->remote_p256_public_key, (const void *)remote_p256_public_key, 64);
    index_input += 64;
    cp0->key_type = key_type;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x05e;
    rq.event = 0x0F;
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

ble_status_t hci_le_set_resolvable_private_address_timeout_v2(uint16_t rpa_timeout_min,
                                                              uint16_t rpa_timeout_max)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    hci_le_set_resolvable_private_address_timeout_v2_cp0 *cp0 =
        (hci_le_set_resolvable_private_address_timeout_v2_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->rpa_timeout_min = rpa_timeout_min;
    index_input += 2;
    cp0->rpa_timeout_max = rpa_timeout_max;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x08;
    rq.ocf = 0x09e;
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
