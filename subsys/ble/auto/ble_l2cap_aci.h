/*****************************************************************************
 * @file    ble_l2cap_aci.h
 * @brief   STM32WB BLE API (L2CAP_ACI)
 *          Auto-generated file: do not edit!
 *****************************************************************************
 * @attention
 *
 * Copyright (c) 2018-2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************
 */

#ifndef BLE_L2CAP_ACI_H__
#define BLE_L2CAP_ACI_H__

#include "ble_types.h"

/**
 * @brief aci_l2cap_connection_parameter_update_req
 * sends an l2cap connection parameter update request from the peripheral to
 * the central.
 * an aci_l2cap_connection_update_resp_event event is raised when the central
 * responds to the request (accepts or rejects).
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param conn_interval_min minimum value for the connection event interval.
 *        time = n * 1.25 ms.
 *        values:
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @param conn_interval_max maximum value for the connection event interval.
 *        time = n * 1.25 ms.
 *        values:
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @param latency maximum peripheral latency for the connection in number of
 *        connection events.
 *        values:
 *        - 0x0000 ... 0x01f3
 * @param timeout_multiplier defines connection timeout parameter in the
 *        following manner: timeout multiplier * 10ms.
 * @return value indicating success or error code.
 */
ble_status_t aci_l2cap_connection_parameter_update_req(uint16_t connection_handle,
													   uint16_t conn_interval_min,
													   uint16_t conn_interval_max, uint16_t latency,
													   uint16_t timeout_multiplier);

/**
 * @brief aci_l2cap_connection_parameter_update_resp
 * accepts or rejects a connection update. this command should be sent in
 * response to an aci_l2cap_connection_update_req_event event from the
 * controller. the accept parameter has to be set if the connection parameters
 * given in the event are acceptable.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param conn_interval_min minimum value for the connection event interval.
 *        time = n * 1.25 ms.
 *        values:
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @param conn_interval_max maximum value for the connection event interval.
 *        time = n * 1.25 ms.
 *        values:
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @param latency maximum peripheral latency for the connection in number of
 *        connection events.
 *        values:
 *        - 0x0000 ... 0x01f3
 * @param timeout_multiplier defines connection timeout parameter in the
 *        following manner: timeout multiplier * 10ms.
 * @param minimum_ce_length information parameter about the minimum length of
 *        connection needed for this le connection.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0000 (0.000 ms)  ... 0xffff (40959.375 ms)
 * @param maximum_ce_length information parameter about the maximum length of
 *        connection needed for this le connection.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0000 (0.000 ms)  ... 0xffff (40959.375 ms)
 * @param identifier received identifier.
 * @param accept specify if connection update parameters are acceptable or not.
 *        values:
 *        - 0x00: reject
 *        - 0x01: accept
 * @return value indicating success or error code.
 */
ble_status_t aci_l2cap_connection_parameter_update_resp(
	uint16_t connection_handle, uint16_t conn_interval_min, uint16_t conn_interval_max,
	uint16_t latency, uint16_t timeout_multiplier, uint16_t minimum_ce_length,
	uint16_t maximum_ce_length, uint8_t identifier, uint8_t accept);

/**
 * @brief aci_l2cap_coc_connect
 * this command sends a credit based connection request packet on the specified
 * connection. see core specification [vol 3, part a].
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param spsm simplified protocol/service multiplexer.
 *        values:
 *        - 0x0001 ... 0x00ff
 * @param mtu maximum transmission unit.
 *        values:
 *        - 23 ... 65535
 *        - 64 ... 246: for enhanced att
 * @param mps maximum payload size (in octets).
 *        values:
 *        - 23 ... 248
 *        - 64 ... 248: for enhanced att
 * @param initial_credits number of k-frames that can be received on the
 *        created channel(s) by the l2cap layer entity sending this packet.
 *        values:
 *        - 0 ... 65535
 * @param channel_number number of channels to be created. if this parameter is
 *        set to 0, it requests the creation of one le credit based connection-
 *        oriented channel. otherwise, it requests the creation of one or more
 *        enhanced credit based connection-oriented channels.
 *        values:
 *        - 0 ... 5
 * @return value indicating success or error code.
 */
ble_status_t aci_l2cap_coc_connect(uint16_t connection_handle, uint16_t spsm, uint16_t mtu,
								   uint16_t mps, uint16_t initial_credits, uint8_t channel_number);

/**
 * @brief aci_l2cap_coc_connect_confirm
 * this command sends a credit based connection response packet. it must be
 * used upon receipt of a connection request through an
 * aci_l2cap_coc_connect_event event.
 * by setting the result parameter to 0x0000, the application can accept all
 * connections or only some. in this case, the number of accepted connections
 * depends on the max_channel_number parameter. note that if some connections
 * are refused, the result parameter is automatically modified by the ble
 * stack.
 * by setting the result parameter to a non-zero value, the application can
 * refuse all connections. the result value shall then be one of the
 * "connection refused" or "all connections refused" values.
 * see core specification [vol 3, part a].
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param mtu maximum transmission unit.
 *        values:
 *        - 23 ... 65535
 *        - 64 ... 246: for enhanced att
 * @param mps maximum payload size (in octets).
 *        values:
 *        - 23 ... 248
 *        - 64 ... 248: for enhanced att
 * @param initial_credits number of k-frames that can be received on the
 *        created channel(s) by the l2cap layer entity sending this packet.
 *        values:
 *        - 0 ... 65535
 * @param result indicates the outcome of the request. see core specification
 *        [vol 3, part a, table 4.16] for le credit based connection-oriented
 *        channels, or [vol 3, part a, table 4.17] for enhanced credit based
 *        connection-oriented channels.
 *        values:
 *        - 0x0000 ... 0x000f
 * @param max_channel_number indicates the maximum number of channels that can
 *        be created.
 *        values:
 *        - 0x01 ... 0x05
 * @param[out] channel_number number of created channels. it is the length of
 *        channel_index_list.
 *        values:
 *        - 0 ... 5
 * @param[out] channel_index_list list of channel indexes for which the
 *        primitive applies.
 * @return value indicating success or error code.
 */
ble_status_t aci_l2cap_coc_connect_confirm(uint16_t connection_handle, uint16_t mtu, uint16_t mps,
										   uint16_t initial_credits, uint16_t result,
										   uint8_t max_channel_number, uint8_t *channel_number,
										   uint8_t *channel_index_list);

/**
 * @brief aci_l2cap_coc_reconf
 * this command sends a credit based reconfigure request packet on the
 * specified connection. see core specification [vol 3, part a].
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param mtu maximum transmission unit.
 *        values:
 *        - 23 ... 65535
 *        - 64 ... 246: for enhanced att
 * @param mps maximum payload size (in octets).
 *        values:
 *        - 23 ... 248
 *        - 64 ... 248: for enhanced att
 * @param channel_number number of created channels. it is the length of
 *        channel_index_list.
 *        values:
 *        - 1 ... 5
 * @param channel_index_list list of channel indexes for which the primitive
 *        applies.
 * @return value indicating success or error code.
 */
ble_status_t aci_l2cap_coc_reconf(uint16_t connection_handle, uint16_t mtu, uint16_t mps,
								  uint8_t channel_number, const uint8_t *channel_index_list);

/**
 * @brief aci_l2cap_coc_reconf_confirm
 * this command sends a credit based reconfigure response packet. it must be
 * used upon receipt of a credit based reconfigure request through an
 * aci_l2cap_coc_reconf_event event. a result value of 0x0000 indicates success
 * while a non-zero value indicates the request is refused.
 * see core specification [vol 3, part a].
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param result indicates the outcome of the request. see core specification
 *        [vol 3, part a, table 4.18].
 *        values:
 *        - 0x0000 ... 0x0004
 * @return value indicating success or error code.
 */
ble_status_t aci_l2cap_coc_reconf_confirm(uint16_t connection_handle, uint16_t result);

/**
 * @brief aci_l2cap_coc_disconnect
 * this command sends a disconnection request signaling packet on the specified
 * connection-oriented channel. see core specification [vol 3, part a].
 * the aci_l2cap_coc_disconnect_event event is received when the disconnection
 * of the channel is effective.
 *
 * @param channel_index index of the connection-oriented channel for which the
 *        primitive applies.
 * @return value indicating success or error code.
 */
ble_status_t aci_l2cap_coc_disconnect(uint8_t channel_index);

/**
 * @brief aci_l2cap_coc_flow_control
 * this command sends a flow control credit signaling packet on the specified
 * connection-oriented channel. see core specification [vol 3, part a].
 *
 * @param channel_index index of the connection-oriented channel for which the
 *        primitive applies.
 * @param credits number of credits the receiving device can increment,
 *        corresponding to the number of k-frames that can be sent to the peer
 *        device sending the flow control credit packet.
 *        values:
 *        - 1 ... 65535
 * @return value indicating success or error code.
 */
ble_status_t aci_l2cap_coc_flow_control(uint8_t channel_index, uint16_t credits);

/**
 * @brief aci_l2cap_coc_tx_data
 * this command sends a k-frame packet on the specified connection-oriented
 * channel. see core specification [vol 3, part a].
 * note: for the first k-frame of the sdu, the information data shall contain
 * the l2cap sdu length coded on two octets followed by the k-frame information
 * payload. for the next k-frames of the sdu, the information data shall only
 * contain the k-frame information payload.
 * the length value must not exceed (ble_cmd_max_param_len - 3) i.e. 252 for
 * ble_cmd_max_param_len default value.
 *
 * @param channel_index index of the connection-oriented channel for which the
 *        primitive applies.
 * @param length length of data (in octets)
 * @param data information data
 * @return value indicating success or error code.
 */
ble_status_t aci_l2cap_coc_tx_data(uint8_t channel_index, uint16_t length, const uint8_t *data);

#endif /* BLE_L2CAP_ACI_H__ */
