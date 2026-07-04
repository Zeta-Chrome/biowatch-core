/*****************************************************************************
 * @file    aci_gen.h
 * @brief   Refactored STM32WB BLE General ACI Commands & Payload Structures
 *****************************************************************************
 */

#ifndef BLE_ACI_GEN_H
#define BLE_ACI_GEN_H

#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"
#include <stdint.h>

/* Pulled Serialization Command and Return Parameter Structs */
typedef __PACKED_STRUCT
{
    uint8_t mode;
    uint32_t options;
}
aci_reset_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_reset_rp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
    uint32_t version[2];
    uint32_t options;
    uint32_t debug_info[3];
}
aci_get_information_rp0;

typedef __PACKED_STRUCT
{
    uint8_t offset;
    uint8_t length;
    uint8_t value[HCI_COMMAND_MAX_PARAM_LEN - 2];
}
aci_write_config_data_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_write_config_data_rp0;

typedef __PACKED_STRUCT
{
    uint8_t offset;
}
aci_read_config_data_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
    uint8_t data_length;
    uint8_t data[(HCI_EVENT_MAX_PARAM_LEN - 3) - 2];
}
aci_read_config_data_rp0;

/* API Declarations */
ble_status_t aci_reset(uint8_t mode, uint32_t options);
ble_status_t aci_get_information(uint32_t *version, uint32_t *options, uint32_t *debug_info);
ble_status_t aci_write_config_data(uint8_t offset, uint8_t length, const uint8_t *value);
ble_status_t aci_read_config_data(uint8_t offset, uint8_t *data_length, uint8_t *data);

#endif /* BLE_ACI_GEN_H */
