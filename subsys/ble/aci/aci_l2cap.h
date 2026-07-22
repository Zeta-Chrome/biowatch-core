/*****************************************************************************
 * @file    aci_l2cap.h
 * @brief   Refactored STM32WB BLE L2CAP ACI Commands & Payload Structures
 *****************************************************************************
 */

#ifndef BLE_ACI_L2CAP_H
#define BLE_ACI_L2CAP_H

#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"
#include <stdint.h>
#include <string.h>

/* Pulled Serialization Command and Return Parameter Structs */
typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
	uint16_t latency;
	uint16_t timeout_multiplier;
}
aci_l2cap_connection_parameter_update_req_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_l2cap_connection_parameter_update_req_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
	uint16_t latency;
	uint16_t timeout_multiplier;
	uint16_t minimum_ce_length;
	uint16_t maximum_ce_length;
	uint8_t identifier;
	uint8_t accept;
}
aci_l2cap_connection_parameter_update_resp_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_l2cap_connection_parameter_update_resp_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t spsm;
	uint16_t mtu;
	uint16_t mps;
	uint16_t initial_credits;
	uint8_t channel_number;
}
aci_l2cap_coc_connect_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_l2cap_coc_connect_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t mtu;
	uint16_t mps;
	uint16_t initial_credits;
	uint16_t result;
	uint8_t max_channel_number;
}
aci_l2cap_coc_connect_confirm_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t channel_number;
	uint8_t channel_index_list[(HCI_EVENT_MAX_PARAM_LEN - 3) - 2];
}
aci_l2cap_coc_connect_confirm_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t mtu;
	uint16_t mps;
	uint8_t channel_number;
	uint8_t channel_index_list[HCI_COMMAND_MAX_PARAM_LEN - 7];
}
aci_l2cap_coc_reconf_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_l2cap_coc_reconf_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t result;
}
aci_l2cap_coc_reconf_confirm_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_l2cap_coc_reconf_confirm_rp0;

typedef __PACKED_STRUCT
{
	uint8_t channel_index;
}
aci_l2cap_coc_disconnect_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_l2cap_coc_disconnect_rp0;

typedef __PACKED_STRUCT
{
	uint8_t channel_index;
	uint16_t credits;
}
aci_l2cap_coc_flow_control_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_l2cap_coc_flow_control_rp0;

typedef __PACKED_STRUCT
{
	uint8_t channel_index;
	uint16_t length;
	uint8_t data[HCI_COMMAND_MAX_PARAM_LEN - 3];
}
aci_l2cap_coc_tx_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_l2cap_coc_tx_data_rp0;

/* API Declarations */
enum ble_status aci_l2cap_connection_parameter_update_req(uint16_t connection_handle,
														  uint16_t conn_interval_min,
														  uint16_t conn_interval_max,
														  uint16_t latency,
														  uint16_t timeout_multiplier);
enum ble_status aci_l2cap_connection_parameter_update_resp(
	uint16_t connection_handle, uint16_t conn_interval_min, uint16_t conn_interval_max,
	uint16_t latency, uint16_t timeout_multiplier, uint16_t minimum_ce_length,
	uint16_t maximum_ce_length, uint8_t identifier, uint8_t accept);
enum ble_status aci_l2cap_coc_connect(uint16_t connection_handle, uint16_t spsm, uint16_t mtu,
									  uint16_t mps, uint16_t initial_credits,
									  uint8_t channel_number);
enum ble_status aci_l2cap_coc_connect_confirm(uint16_t connection_handle, uint16_t mtu,
											  uint16_t mps, uint16_t initial_credits,
											  uint16_t result, uint8_t max_channel_number,
											  uint8_t *channel_number, uint8_t *channel_index_list);
enum ble_status aci_l2cap_coc_reconf(uint16_t connection_handle, uint16_t mtu, uint16_t mps,
									 uint8_t channel_number, const uint8_t *channel_index_list);
enum ble_status aci_l2cap_coc_reconf_confirm(uint16_t connection_handle, uint16_t result);
enum ble_status aci_l2cap_coc_disconnect(uint8_t channel_index);
enum ble_status aci_l2cap_coc_flow_control(uint8_t channel_index, uint16_t credits);
enum ble_status aci_l2cap_coc_tx_data(uint8_t channel_index, uint16_t length, const uint8_t *data);

#endif /* BLE_ACI_L2CAP_H */
