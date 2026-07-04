#ifndef BLE_ACI_HAL_H
#define BLE_ACI_HAL_H

#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"
#include <stdint.h>
#include <string.h>

/* Pulled Serialization Command and Return Parameter Structs */
typedef __PACKED_STRUCT
{
    uint8_t offset;
    uint8_t length;
    uint8_t value[HCI_COMMAND_MAX_PARAM_LEN - 2];
}
aci_hal_write_config_data_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_hal_write_config_data_rp0;

typedef __PACKED_STRUCT
{
    uint8_t offset;
}
aci_hal_read_config_data_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
    uint8_t data_length;
    uint8_t data[(HCI_EVENT_MAX_PARAM_LEN - 3) - 2];
}
aci_hal_read_config_data_rp0;

typedef __PACKED_STRUCT
{
    uint8_t en_high_power;
    uint8_t pa_level;
}
aci_hal_set_tx_power_level_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_hal_set_tx_power_level_rp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
    uint32_t number_of_packets;
}
aci_hal_le_tx_test_packet_number_rp0;

typedef __PACKED_STRUCT
{
    uint8_t rf_channel;
    uint8_t freq_offset;
}
aci_hal_tone_start_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_hal_tone_start_rp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_hal_tone_stop_rp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
    uint8_t link_status[8];
    uint16_t link_connection_handle[8];
}
aci_hal_get_link_status_rp0;

typedef __PACKED_STRUCT
{
    uint16_t radio_activity_mask;
}
aci_hal_set_radio_activity_mask_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_hal_set_radio_activity_mask_rp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
    uint32_t anchor_period;
    uint32_t max_free_slot;
}
aci_hal_get_anchor_period_rp0;

typedef __PACKED_STRUCT
{
    uint32_t event_mask;
}
aci_hal_set_event_mask_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_hal_set_event_mask_rp0;

typedef __PACKED_STRUCT
{
    uint8_t enable;
}
aci_hal_set_peripheral_latency_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_hal_set_peripheral_latency_rp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
    uint8_t rssi;
}
aci_hal_read_rssi_rp0;

typedef __PACKED_STRUCT
{
    uint8_t mode;
    uint8_t key[16];
    uint8_t iv[8];
    uint16_t in_data_length;
    uint8_t in_data[HCI_COMMAND_MAX_PARAM_LEN - 27];
}
aci_hal_ead_encrypt_decrypt_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
    uint16_t out_data_length;
    uint8_t out_data[(HCI_EVENT_MAX_PARAM_LEN - 3) - 3];
}
aci_hal_ead_encrypt_decrypt_rp0;

typedef __PACKED_STRUCT
{
    uint8_t register_address;
}
aci_hal_read_radio_reg_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
    uint8_t reg_val;
}
aci_hal_read_radio_reg_rp0;

typedef __PACKED_STRUCT
{
    uint8_t register_address;
    uint8_t register_value;
}
aci_hal_write_radio_reg_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_hal_write_radio_reg_rp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
    uint8_t value[3];
}
aci_hal_read_raw_rssi_rp0;

typedef __PACKED_STRUCT
{
    uint8_t rf_channel;
}
aci_hal_rx_start_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_hal_rx_start_rp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_hal_rx_stop_rp0;

/* API Declarations */
ble_status_t aci_hal_write_config_data(uint8_t offset, uint8_t length, const uint8_t *value);
ble_status_t aci_hal_read_config_data(uint8_t offset, uint8_t *data_length, uint8_t *data);
ble_status_t aci_hal_set_tx_power_level(uint8_t en_high_power, uint8_t pa_level);
ble_status_t aci_hal_le_tx_test_packet_number(uint32_t *number_of_packets);
ble_status_t aci_hal_tone_start(uint8_t rf_channel, uint8_t freq_offset);
ble_status_t aci_hal_tone_stop(void);
ble_status_t aci_hal_get_link_status(uint8_t *link_status, uint16_t *link_connection_handle);
ble_status_t aci_hal_set_radio_activity_mask(uint16_t radio_activity_mask);
ble_status_t aci_hal_get_anchor_period(uint32_t *anchor_period, uint32_t *max_free_slot);
ble_status_t aci_hal_set_event_mask(uint32_t event_mask);
ble_status_t aci_hal_set_peripheral_latency(uint8_t enable);
ble_status_t aci_hal_read_rssi(uint8_t *rssi);
ble_status_t aci_hal_ead_encrypt_decrypt(uint8_t mode,
                                         const uint8_t *key,
                                         const uint8_t *iv,
                                         uint16_t in_data_length,
                                         const uint8_t *in_data,
                                         uint16_t *out_data_length,
                                         uint8_t *out_data);
ble_status_t aci_hal_read_radio_reg(uint8_t register_address, uint8_t *reg_val);
ble_status_t aci_hal_write_radio_reg(uint8_t register_address, uint8_t register_value);
ble_status_t aci_hal_read_raw_rssi(uint8_t *value);
ble_status_t aci_hal_rx_start(uint8_t rf_channel);
ble_status_t aci_hal_rx_stop(void);

#endif /* BLE_ACI_HAL_H */
