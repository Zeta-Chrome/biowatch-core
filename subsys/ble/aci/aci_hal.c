#include "aci_hal.h"
#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"

ble_status_t aci_hal_write_config_data(uint8_t offset, uint8_t length, const uint8_t *value)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_write_config_data_cp0 *cp0 = (aci_hal_write_config_data_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->offset = offset;
    index_input += 1;
    cp0->length = length;
    index_input += 1;
    memcpy((void *)&cp0->value, (const void *)value, length);
    index_input += length;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
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

ble_status_t aci_hal_read_config_data(uint8_t offset, uint8_t *data_length, uint8_t *data)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_read_config_data_cp0 *cp0 = (aci_hal_read_config_data_cp0 *)(cmd_buffer);
    aci_hal_read_config_data_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->offset = offset;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x00d;
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

    *data_length = resp.data_length;
    memcpy((void *)data, (const void *)resp.data, *data_length);

    return BLE_STATUS_SUCCESS;
}

ble_status_t aci_hal_set_tx_power_level(uint8_t en_high_power, uint8_t pa_level)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_set_tx_power_level_cp0 *cp0 = (aci_hal_set_tx_power_level_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->en_high_power = en_high_power;
    index_input += 1;
    cp0->pa_level = pa_level;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x00f;
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

ble_status_t aci_hal_le_tx_test_packet_number(uint32_t *number_of_packets)
{
    hci_request_t rq;
    aci_hal_le_tx_test_packet_number_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x014;
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

ble_status_t aci_hal_tone_start(uint8_t rf_channel, uint8_t freq_offset)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_tone_start_cp0 *cp0 = (aci_hal_tone_start_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->rf_channel = rf_channel;
    index_input += 1;
    cp0->freq_offset = freq_offset;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x015;
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

ble_status_t aci_hal_tone_stop(void)
{
    hci_request_t rq;
    ble_status_t status = 0;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x016;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}

ble_status_t aci_hal_get_link_status(uint8_t *link_status, uint16_t *link_connection_handle)
{
    hci_request_t rq;
    aci_hal_get_link_status_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x017;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    memcpy((void *)link_status, (const void *)resp.link_status, 8);
    memcpy((void *)link_connection_handle, (const void *)resp.link_connection_handle, 16);

    return BLE_STATUS_SUCCESS;
}

ble_status_t aci_hal_set_radio_activity_mask(uint16_t radio_activity_mask)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_set_radio_activity_mask_cp0 *cp0 = (aci_hal_set_radio_activity_mask_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->radio_activity_mask = radio_activity_mask;
    index_input += 2;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x018;
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

ble_status_t aci_hal_get_anchor_period(uint32_t *anchor_period, uint32_t *max_free_slot)
{
    hci_request_t rq;
    aci_hal_get_anchor_period_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x019;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    *anchor_period = resp.anchor_period;
    *max_free_slot = resp.max_free_slot;

    return BLE_STATUS_SUCCESS;
}

ble_status_t aci_hal_set_event_mask(uint32_t event_mask)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_set_event_mask_cp0 *cp0 = (aci_hal_set_event_mask_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->event_mask = event_mask;
    index_input += 4;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x01a;
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

ble_status_t aci_hal_set_peripheral_latency(uint8_t enable)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_set_peripheral_latency_cp0 *cp0 = (aci_hal_set_peripheral_latency_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->enable = enable;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x020;
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

ble_status_t aci_hal_read_rssi(uint8_t *rssi)
{
    hci_request_t rq;
    aci_hal_read_rssi_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x022;
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

ble_status_t aci_hal_ead_encrypt_decrypt(uint8_t mode,
                                         const uint8_t *key,
                                         const uint8_t *iv,
                                         uint16_t in_data_length,
                                         const uint8_t *in_data,
                                         uint16_t *out_data_length,
                                         uint8_t *out_data)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_ead_encrypt_decrypt_cp0 *cp0 = (aci_hal_ead_encrypt_decrypt_cp0 *)(cmd_buffer);
    aci_hal_ead_encrypt_decrypt_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->mode = mode;
    index_input += 1;
    memcpy((void *)&cp0->key, (const void *)key, 16);
    index_input += 16;
    memcpy((void *)&cp0->iv, (const void *)iv, 8);
    index_input += 8;
    cp0->in_data_length = in_data_length;
    index_input += 2;
    memcpy((void *)&cp0->in_data, (const void *)in_data, in_data_length);
    index_input += in_data_length;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x02f;
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

    *out_data_length = resp.out_data_length;
    memcpy((void *)out_data, (const void *)resp.out_data, *out_data_length);

    return BLE_STATUS_SUCCESS;
}

ble_status_t aci_hal_read_radio_reg(uint8_t register_address, uint8_t *reg_val)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_read_radio_reg_cp0 *cp0 = (aci_hal_read_radio_reg_cp0 *)(cmd_buffer);
    aci_hal_read_radio_reg_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    int index_input = 0;
    cp0->register_address = register_address;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
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

    *reg_val = resp.reg_val;

    return BLE_STATUS_SUCCESS;
}

ble_status_t aci_hal_write_radio_reg(uint8_t register_address, uint8_t register_value)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_write_radio_reg_cp0 *cp0 = (aci_hal_write_radio_reg_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->register_address = register_address;
    index_input += 1;
    cp0->register_value = register_value;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
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

ble_status_t aci_hal_read_raw_rssi(uint8_t *value)
{
    hci_request_t rq;
    aci_hal_read_raw_rssi_rp0 resp;

    memset(&resp, 0, sizeof(resp));
    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x032;
    rq.ret_param = &resp;
    rq.ret_len = sizeof(resp);

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    if (resp.status)
        return resp.status;

    memcpy((void *)value, (const void *)resp.value, 3);

    return BLE_STATUS_SUCCESS;
}

ble_status_t aci_hal_rx_start(uint8_t rf_channel)
{
    hci_request_t rq;
    uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
    aci_hal_rx_start_cp0 *cp0 = (aci_hal_rx_start_cp0 *)(cmd_buffer);
    ble_status_t status = 0;
    int index_input = 0;

    cp0->rf_channel = rf_channel;
    index_input += 1;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
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

ble_status_t aci_hal_rx_stop(void)
{
    hci_request_t rq;
    ble_status_t status = 0;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x034;
    rq.ret_param = &status;
    rq.ret_len = 1;

    if (hci_send_req(&rq) == STATUS_TIMEOUT)
    {
        return BLE_STATUS_TIMEOUT;
    }

    return status;
}
