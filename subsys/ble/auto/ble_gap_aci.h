/*****************************************************************************
 * @file    ble_gap_aci.h
 * @brief   STM32WB BLE API (GAP_ACI)
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

#ifndef BLE_GAP_ACI_H__
#define BLE_GAP_ACI_H__

#include "ble_types.h"

/**
 * @brief aci_gap_set_non_discoverable
 * this command stops advertising.
 * note: this command only supports legacy advertising. for extended
 * advertising, refer to aci_gap_adv_set_enable.
 *
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_non_discoverable(void);

/**
 * @brief aci_gap_set_limited_discoverable
 * puts the device in limited discoverable mode (as defined in core
 * specification [vol 3, part c, 9.2.3]). the device will be discoverable for
 * maximum period of tgap (lim_adv_timeout) = 180 seconds (from errata). the
 * advertising can be disabled at any time by issuing
 * aci_gap_set_non_discoverable command.
 * the adv_interval_min and adv_interval_max parameters are optional. if both
 * are set to 0, the gap will use default values for adv intervals for limited
 * discoverable mode (250 ms and 500 ms respectively).
 * to allow a fast connection, the host can set local_name, service_uuid_list,
 * conn_interval_min and conn_interval_max. if provided, these data will be
 * inserted into the advertising packet payload as ad data. these parameters
 * are optional in this command. these values can be set in advertised data
 * using aci_gap_update_adv_data command separately.
 * the total size of data in advertising packet cannot exceed 31 bytes.
 * with this command, the ble stack will also add automatically the following
 * standard ad types:
 * - ad flags
 * - power level
 * when advertising timeout happens (i.e. limited discovery period has
 * elapsed), controller generates aci_gap_limited_discoverable_event event.
 * note: this command only supports legacy advertising for extended
 * advertising, refer to aci_gap_adv_set_configuration and
 * aci_gap_adv_set_enable..
 *
 * @param advertising_type advertising type
 *        values:
 *        - 0x00: adv_ind (connectable undirected advertising)
 *        - 0x02: adv_scan_ind (scannable undirected advertising)
 *        - 0x03: adv_nonconn_ind (non connectable undirected advertising)
 * @param advertising_interval_min minimum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms)
 * @param advertising_interval_max maximum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms)
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @param advertising_filter_policy advertising filter policy: not applicable
 *        (the value of advertising_filter_policy parameter is not used inside
 *        the stack)
 * @param local_name_length length of the local_name field in octets.
 *        if length is set to 0x00, local_name parameter is not used.
 * @param local_name local name of the device. first byte must be 0x08 for
 *        shortened local name or 0x09 for complete local name. no null
 *        character at the end.
 * @param service_uuid_length length of the service uuid list in octets.
 *        if there is no service to be advertised, set this field to 0x00.
 * @param service_uuid_list this is the list of the uuids as defined in volume
 *        3, section 11 of gap specification. first byte is the ad type.
 * @param conn_interval_min connection interval minimum value suggested by
 *        peripheral.
 *        if conn_interval_min and conn_interval_max are not 0x0000, peripheral
 *        connection interval range ad structure will be added in advertising
 *        data.
 *        connection interval is defined in the following manner:
 *        connintervalmin = conn_interval_min x 1.25ms.
 *        values:
 *        - 0x0000 (nan)
 *        - 0xffff (nan) : no specific minimum
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @param conn_interval_max connection interval maximum value suggested by
 *        peripheral.
 *        if conn_interval_min and conn_interval_max are not 0x0000, peripheral
 *        connection interval range ad structure will be added in advertising
 *        data.
 *        connection interval is defined in the following manner:
 *        connintervalmax = conn_interval_max x 1.25ms
 *        values:
 *        - 0x0000 (nan)
 *        - 0xffff (nan) : no specific maximum
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_limited_discoverable(
	uint8_t advertising_type, uint16_t advertising_interval_min, uint16_t advertising_interval_max,
	uint8_t own_address_type, uint8_t advertising_filter_policy, uint8_t local_name_length,
	const uint8_t *local_name, uint8_t service_uuid_length, const uint8_t *service_uuid_list,
	uint16_t conn_interval_min, uint16_t conn_interval_max);

/**
 * @brief aci_gap_set_discoverable
 * puts the device in general discoverable mode (as defined in core
 * specification [vol 3, part c, 9.2.4]). the device will be discoverable until
 * the host issues the aci_gap_set_non_discoverable command. the
 * adv_interval_min and adv_interval_max parameters are optional. if both are
 * set to 0, the gap uses the default values for adv intervals for general
 * discoverable mode.
 * when using connectable undirected advertising events:
 * - adv_interval_min = 30 ms
 * - adv_interval_max = 60 ms
 * when using non-connectable advertising events or scannable undirected
 * advertising events:
 * - adv_interval_min = 100 ms
 * - adv_interval_max = 150 ms
 * host can set the local name, a service uuid list and the peripheral
 * connection interval range.
 * if provided, these data will be inserted into the advertising packet payload
 * as ad data.
 * these parameters are optional in this command. these values can be also set
 * using aci_gap_update_adv_data command separately.
 * the total size of data in advertising packet cannot exceed 31 bytes.
 * with this command, the ble stack will also add automatically the following
 * standard ad types:
 * - ad flags
 * - tx power level
 * note: this command only supports legacy advertising. for extended
 * advertising, refer to aci_gap_adv_set_configuration and
 * aci_gap_adv_set_enable.
 *
 * @param advertising_type advertising type
 *        values:
 *        - 0x00: adv_ind (connectable undirected advertising)
 *        - 0x02: adv_scan_ind (scannable undirected advertising)
 *        - 0x03: adv_nonconn_ind (non connectable undirected advertising)
 * @param advertising_interval_min minimum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms)
 * @param advertising_interval_max maximum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms)
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @param advertising_filter_policy advertising filter policy: not applicable
 *        (the value of advertising_filter_policy parameter is not used inside
 *        the stack)
 * @param local_name_length length of the local_name field in octets.
 *        if length is set to 0x00, local_name parameter is not used.
 * @param local_name local name of the device. first byte must be 0x08 for
 *        shortened local name or 0x09 for complete local name. no null
 *        character at the end.
 * @param service_uuid_length length of the service uuid list in octets.
 *        if there is no service to be advertised, set this field to 0x00.
 * @param service_uuid_list this is the list of the uuids as defined in volume
 *        3, section 11 of gap specification. first byte is the ad type.
 * @param conn_interval_min connection interval minimum value suggested by
 *        peripheral.
 *        if conn_interval_min and conn_interval_max are not 0x0000, peripheral
 *        connection interval range ad structure will be added in advertising
 *        data.
 *        connection interval is defined in the following manner:
 *        connintervalmin = conn_interval_min x 1.25ms.
 *        values:
 *        - 0x0000 (nan)
 *        - 0xffff (nan) : no specific minimum
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @param conn_interval_max connection interval maximum value suggested by
 *        peripheral.
 *        if conn_interval_min and conn_interval_max are not 0x0000, peripheral
 *        connection interval range ad structure will be added in advertising
 *        data.
 *        connection interval is defined in the following manner:
 *        connintervalmax = conn_interval_max x 1.25ms
 *        values:
 *        - 0x0000 (nan)
 *        - 0xffff (nan) : no specific maximum
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_discoverable(uint8_t advertising_type, uint16_t advertising_interval_min,
									  uint16_t advertising_interval_max, uint8_t own_address_type,
									  uint8_t advertising_filter_policy, uint8_t local_name_length,
									  const uint8_t *local_name, uint8_t service_uuid_length,
									  const uint8_t *service_uuid_list, uint16_t conn_interval_min,
									  uint16_t conn_interval_max);

/**
 * @brief aci_gap_set_direct_connectable
 * sets the device in directed connectable mode (as defined in core
 * specification [vol 3, part c, 9.3.3]). in this mode, the device advertises
 * using high duty cycle connectable directed advertising events or low duty
 * cycle connectable directed advertising events.
 * the device's own address used in advertising packets is defined by the
 * own_address_type parameter depending on whether privacy is enabled or not.
 * when using high duty cycle connectable directed advertising events, the
 * device stays in directed connectable mode only for 1.28 seconds. if no
 * connection is established within this duration, the device enters non
 * discoverable mode and advertising has to be again enabled explicitly.
 * the controller generates a hci_le_connection_complete_event event with the
 * status set to hci_advertising_timeout_err_code if the connection was not
 * established and ble_status_success (0x00) if the connection was successfully
 * established.
 * note: this command only supports legacy advertising. for extended
 * advertising, refer to aci_gap_adv_set_configuration and
 * aci_gap_adv_set_enable.
 *
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 * @param directed_advertising_type advertising type
 *        values:
 *        - 0x01: high duty cycle directed advertising
 *        - 0x04: low duty cycle directed advertising
 * @param direct_address_type the address type of the peer device.
 *        values:
 *        - 0x00: public device address
 *        - 0x01: random device address
 * @param direct_address initiator address
 * @param advertising_interval_min minimum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0006 (3.750 ms) : for high duty cycle directed advertising
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms) : for low duty cycle
 *          directed advertising
 * @param advertising_interval_max maximum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0006 (3.750 ms) : for high duty cycle directed advertising
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms) : for low duty cycle
 *          directed advertising
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_direct_connectable(uint8_t own_address_type,
											uint8_t directed_advertising_type,
											uint8_t direct_address_type,
											const uint8_t *direct_address,
											uint16_t advertising_interval_min,
											uint16_t advertising_interval_max);

/**
 * @brief aci_gap_set_io_capability
 * sets the io capabilities of the device. this command has to be given only
 * when the device is not in a connected state.
 *
 * @param io_capability io capability of the device.
 *        values:
 *        - 0x00: io_cap_display_only
 *        - 0x01: io_cap_display_yes_no
 *        - 0x02: io_cap_keyboard_only
 *        - 0x03: io_cap_no_input_no_output
 *        - 0x04: io_cap_keyboard_display
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_io_capability(uint8_t io_capability);

/**
 * @brief aci_gap_set_authentication_requirement
 * sets the authentication requirements for the device. this command has to be
 * given only when the device is not in a connected state.
 *
 * @param bonding_mode bonding mode.
 *        only if bonding is enabled (0x01), the bonding information is stored
 *        in flash
 *        values:
 *        - 0x00: no-bonding mode
 *        - 0x01: bonding mode
 * @param mitm_mode mitm mode.
 *        values:
 *        - 0x00: mitm protection not required
 *        - 0x01: mitm protection required as mandatory
 *        - 0x02: mitm protection required as optional
 * @param sc_support le secure connections support
 *        values:
 *        - 0x00: secure connections pairing not supported
 *        - 0x01: secure connections pairing supported but optional
 *        - 0x02: secure connections pairing supported and mandatory (sc only
 *          mode)
 * @param keypress_notification_support keypress notification support
 *        values:
 *        - 0x00: keypress notification not supported
 *        - 0x01: keypress notification supported
 * @param min_encryption_key_size minimum encryption key size to be used during
 *        pairing.
 * @param max_encryption_key_size maximum encryption key size to be used during
 *        pairing.
 * @param use_fixed_pin use or not fixed pin.
 *        if set to 0x00, then during the pairing process the application will
 *        not be requested for a pin (fixed_pin will be used). if set to 0x01,
 *        then during pairing process if a passkey is required the application
 *        will be notified
 *        values:
 *        - 0x00: use a fixed pin (deprecated option that should not be used)
 *        - 0x01: do not use a fixed pin
 * @param fixed_pin fixed pin to be used during pairing (deprecated parameter).
 *        values:
 *        - 0 ... 999999
 * @param identity_address_type identity address type
 *        values:
 *        - 0x00: public identity address
 *        - 0x01: random (static) identity address
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_authentication_requirement(uint8_t bonding_mode, uint8_t mitm_mode,
													uint8_t sc_support,
													uint8_t keypress_notification_support,
													uint8_t min_encryption_key_size,
													uint8_t max_encryption_key_size,
													uint8_t use_fixed_pin, uint32_t fixed_pin,
													uint8_t identity_address_type);

/**
 * @brief aci_gap_set_authorization_requirement
 * sets the authorization requirements of the device. this command has to be
 * given when connected to a device if authorization is required to access
 * services which require authorization.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param authorization_enable enable the authorization in the device and when
 *        a remote device tries to read/write a characteristic with
 *        authorization requirements, the stack will send back an error
 *        response with "insufficient authorization" error code. after pairing
 *        is complete an aci_gap_authorization_req_event will be sent to the
 *        host.
 *        values:
 *        - 0x00: authorization not required
 *        - 0x01: authorization required
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_authorization_requirement(uint16_t connection_handle,
												   uint8_t authorization_enable);

/**
 * @brief aci_gap_pass_key_resp
 * this command should be sent by the host in response to
 * aci_gap_pass_key_req_event event. the command parameter contains the pass
 * key which will be used during the pairing process.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param pass_key pass key that will be used during the pairing process.
 *        must be a six-digit decimal number.
 *        values:
 *        - 0 ... 999999
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_pass_key_resp(uint16_t connection_handle, uint32_t pass_key);

/**
 * @brief aci_gap_authorization_resp
 * authorizes a device to access attributes. this command should be sent by the
 * host in response to aci_gap_authorization_req_event event.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param authorize authorization response.
 *        values:
 *        - 0x01: authorize
 *        - 0x02: reject
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_authorization_resp(uint16_t connection_handle, uint8_t authorize);

/**
 * @brief aci_gap_init
 * initializes the gap layer. register the gap service with the gatt.
 * all the standard gap characteristics will also be added:
 * - device name
 * - appearance
 * - peripheral preferred connection parameters (peripheral role only).
 * note that if the peripheral preferred connection parameters characteristic
 * is added, its handle is equal to the appearance characteristic handle plus
 * 2.
 * note also that if privacy is enabled, this command automatically unmasks the
 * hci_le_enhanced_connection_complete_event event.
 *
 * @param role bitmap of allowed roles.
 *        flags:
 *        - 0x01: peripheral
 *        - 0x02: broadcaster
 *        - 0x04: central
 *        - 0x08: observer
 * @param privacy_enabled this parameter specifies if privacy is enabled or
 *        not. n.b.: only controller privacy is supported.
 *        values:
 *        - 0x00: privacy disabled
 *        - 0x02: privacy enabled
 * @param device_name_char_len length of the device name characteristic
 * @param[out] service_handle handle of the gap service
 * @param[out] dev_name_char_handle device name characteristic handle
 * @param[out] appearance_char_handle appearance characteristic handle
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_init(uint8_t role, uint8_t privacy_enabled, uint8_t device_name_char_len,
						  uint16_t *service_handle, uint16_t *dev_name_char_handle,
						  uint16_t *appearance_char_handle);

/**
 * @brief aci_gap_set_non_connectable
 * this command starts advertising in non connectable mode (i.e., this mode
 * does not support connection).
 * advertiser filter policy is internally set to 0 (i.e., the filter accept
 * list is not in use).
 * note: this command only supports legacy advertising. for extended
 * advertising, refer to aci_gap_adv_set_configuration and
 * aci_gap_adv_set_enable.
 *
 * @param advertising_event_type advertising type
 *        values:
 *        - 0x02: adv_scan_ind (scannable undirected advertising)
 *        - 0x03: adv_nonconn_ind (non connectable undirected advertising)
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_non_connectable(uint8_t advertising_event_type, uint8_t own_address_type);

/**
 * @brief aci_gap_set_undirected_connectable
 * this command starts advertising in undirected connectable mode.
 * if privacy is enabled in the device, a resolvable private address is
 * generated and used as the advertiser's address. if not, the address of the
 * type specified in own_address_type is used for advertising.
 * note: this command only supports legacy advertising. for extended
 * advertising, refer to aci_gap_adv_set_configuration and
 * aci_gap_adv_set_enable.
 *
 * @param advertising_interval_min minimum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms)
 * @param advertising_interval_max maximum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms)
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 * @param adv_filter_policy advertising filter policy.
 *        values:
 *        - 0x00: allow scan request from any, allow connect request from any
 *        - 0x03: allow scan request from filter accept list only, allow
 *          connect request from filter accept list only
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_undirected_connectable(uint16_t advertising_interval_min,
												uint16_t advertising_interval_max,
												uint8_t own_address_type,
												uint8_t adv_filter_policy);

/**
 * @brief aci_gap_peripheral_security_req
 * sends a peripheral security request to the central.
 * this command has to be issued to notify the central of the security
 * requirements of the peripheral. the central may encrypt the link, initiate
 * the pairing procedure, or reject the request.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_peripheral_security_req(uint16_t connection_handle);

/**
 * @brief aci_gap_update_adv_data
 * this command can be used to update the advertising data for particular ad
 * types. the data argument shall consist of valid advertising data composed of
 * one or more ad types. if one of the specified ad types does not exist in the
 * current advertising data, then it is added to the advertising data; while
 * the ad types already present in the current advertising data are updated
 * with the new values. if the overall advertising data length is more than 31
 * octets after the update, then the command is rejected and the old data is
 * retained.
 * note: this command only supports legacy advertising. for extended
 * advertising, refer to aci_gap_adv_set_adv_data.
 *
 * @param advdatalen length of advdata in octets
 * @param advdata advertising data used by the device while advertising.
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_update_adv_data(uint8_t advdatalen, const uint8_t *advdata);

/**
 * @brief aci_gap_delete_ad_type
 * this command can be used to delete the specified ad type from the
 * advertisement data if present.
 * note: this command only supports legacy advertising. for extended
 * advertising, refer to aci_gap_adv_set_adv_data.
 *
 * @param adtype one of the ad types as in core specification [vol 3, part c,
 *        11].
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_delete_ad_type(uint8_t adtype);

/**
 * @brief aci_gap_get_security_level
 * this command can be used to get the current security settings of the device.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param[out] security_mode security mode.
 *        values:
 *        - 0x01: security mode 1
 * @param[out] security_level security level.
 *        values:
 *        - 0x01: security level 1
 *        - 0x02: security level 2
 *        - 0x03: security level 3
 *        - 0x04: security level 4
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_get_security_level(uint16_t connection_handle, uint8_t *security_mode,
										uint8_t *security_level);

/**
 * @brief aci_gap_set_event_mask
 * it allows masking events from the gap. if the bit in the gap_evt_mask is set
 * to a one, then the event associated with that bit will be enabled.
 *
 * @param gap_evt_mask aci gap/l2cap event mask. default: 0xffff.
 *        flags:
 *        - 0x0000: no events
 *        - 0x0001: aci_gap_limited_discoverable_event
 *        - 0x0002: aci_gap_pairing_complete_event
 *        - 0x0004: aci_gap_pass_key_req_event
 *        - 0x0008: aci_gap_authorization_req_event
 *        - 0x0020: aci_gap_bond_lost_event
 *        - 0x0080: aci_gap_proc_complete_event
 *        - 0x0100: aci_l2cap_connection_update_req_event
 *        - 0x0200: aci_l2cap_connection_update_resp_event
 *        - 0x0400: aci_l2cap_proc_timeout_event
 *        - 0x0800: aci_gap_addr_not_resolved_event
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_event_mask(uint16_t gap_evt_mask);

/**
 * @brief aci_gap_configure_filter_accept_list
 * this command adds addresses of bonded devices into the controller's filter
 * accept list, which is cleared first. it returns an error if it was unable to
 * add all bonded devices into the filter accept list.
 * this command shall not be used when the device is advertising, scanning or
 * initiating with a filter policy using the filter accept list.
 *
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_configure_filter_accept_list(void);

/**
 * @brief aci_gap_terminate
 * commands the controller to terminate the connection. a
 * hci_disconnection_complete_event event is generated when the link is
 * disconnected.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param reason the reason for ending the connection.
 *        values:
 *        - 0x05: authentication failure
 *        - 0x13: remote user terminated connection
 *        - 0x14: remote device terminated connection due to low resources
 *        - 0x15: remote device terminated connection due to power off
 *        - 0x1a: unsupported remote feature
 *        - 0x3b: unacceptable connection parameters
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_terminate(uint16_t connection_handle, uint8_t reason);

/**
 * @brief aci_gap_clear_security_db
 * clears the bonding table. all the devices in the bonding table are removed.
 * see also aci_gap_remove_bonded_device to remove only one device.
 * note: as a fallback mode, in case the bonding table is full, the ble stack
 * automatically clears the bonding table just before putting into it
 * information about a new bonded device.
 *
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_clear_security_db(void);

/**
 * @brief aci_gap_allow_rebond
 * allows the security manager to complete the pairing procedure and re-bond
 * with the central. this command should be given by the application when it
 * receives the aci_gap_bond_lost_event if it wants the re-bonding to happen
 * successfully. if this command is not given on receiving the event, the
 * bonding procedure will timeout.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_allow_rebond(uint16_t connection_handle);

/**
 * @brief aci_gap_start_limited_discovery_proc
 * starts the limited discovery procedure. the controller is commanded to start
 * active scanning.
 * when this procedure is started, only the devices in limited discoverable
 * mode are returned to the upper layers.
 * the procedure is terminated when either the upper layers issue a command to
 * terminate the procedure by issuing the command aci_gap_terminate_gap_proc
 * with the procedure code set to 0x01 or a timeout happens (the timeout value
 * is fixed at 10.24 s.). when the procedure is terminated due to any of the
 * above  reasons, aci_gap_proc_complete_event event is returned with the
 * procedure code set to 0x01.
 * the device found when the procedure is ongoing is returned to the upper
 * layers through the event hci_le_advertising_report_event (or via
 * hci_le_extended_advertising_report_event when the extended advertising
 * feature is supported).
 *
 * @param le_scan_interval this is defined as the time interval from when the
 *        controller started its last le scan until it begins the subsequent le
 *        scan.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms)
 * @param le_scan_window amount of time for the duration of the le scan.
 *        le_scan_window shall be less than or equal to le_scan_interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms)
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @param filter_duplicates enable/disable duplicate filtering.
 *        values:
 *        - 0x00: duplicate filtering disabled
 *        - 0x01: duplicate filtering enabled
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_start_limited_discovery_proc(uint16_t le_scan_interval,
												  uint16_t le_scan_window, uint8_t own_address_type,
												  uint8_t filter_duplicates);

/**
 * @brief aci_gap_start_general_discovery_proc
 * starts the general discovery procedure. the controller is commanded to start
 * active scanning. the procedure is terminated when  either the upper layers
 * issue a command to terminate the procedure by issuing the command
 * aci_gap_terminate_gap_proc with the procedure code set to 0x02 or a timeout
 * happens (the timeout value is fixed at 10.24 s.). when the procedure is
 * terminated due to any of the above reasons, aci_gap_proc_complete_event
 * event is returned with the procedure code set to 0x02.
 * the devices found when the procedure is ongoing are returned via
 * hci_le_advertising_report_event (or via
 * hci_le_extended_advertising_report_event when the extended advertising
 * feature is supported).
 *
 * @param le_scan_interval this is defined as the time interval from when the
 *        controller started its last le scan until it begins the subsequent le
 *        scan.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms)
 * @param le_scan_window amount of time for the duration of the le scan.
 *        le_scan_window shall be less than or equal to le_scan_interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms)
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @param filter_duplicates enable/disable duplicate filtering.
 *        values:
 *        - 0x00: duplicate filtering disabled
 *        - 0x01: duplicate filtering enabled
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_start_general_discovery_proc(uint16_t le_scan_interval,
												  uint16_t le_scan_window, uint8_t own_address_type,
												  uint8_t filter_duplicates);

/**
 * @brief aci_gap_start_auto_connection_establish_proc
 * starts the auto connection establishment procedure. the devices specified
 * are added to the filter accept list of the controller and a
 * le_create_connection call will be made to the controller by gap with the
 * initiator filter policy set to "filter accept list is used to determine
 * which advertiser to connect to". when a command is issued to terminate the
 * procedure by upper layer, a le_create_connection_cancel call will be made to
 * the controller by gap.
 * the procedure is terminated when either a connection is successfully
 * established with one of the specified devices in the filter accept list or
 * the procedure is explicitly terminated by issuing the command
 * aci_gap_terminate_gap_proc with the procedure code set to 0x08. a
 * aci_gap_proc_complete_event event is returned with the procedure code set to
 * 0x08.
 * if privacy is enabled and the peer device (advertiser) is in the resolving
 * list then the link layer generates a rpa.
 *
 * @param le_scan_interval this is defined as the time interval from when the
 *        controller started its last le scan until it begins the subsequent le
 *        scan.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms) : legacy advertising
 *        - 0x0004 (2.500 ms)  ... 0x5dc0 (15000.000 ms) : extended advertising
 * @param le_scan_window amount of time for the duration of the le scan.
 *        le_scan_window shall be less than or equal to le_scan_interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms) : legacy advertising
 *        - 0x0004 (2.500 ms)  ... 0x5dc0 (15000.000 ms) : extended advertising
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 * @param conn_interval_min minimum value for the connection event interval.
 *        time = n * 1.25 ms.
 *        values:
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @param conn_interval_max maximum value for the connection event interval.
 *        time = n * 1.25 ms.
 *        values:
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @param conn_latency maximum peripheral latency for the connection in number
 *        of connection events.
 *        values:
 *        - 0x0000 ... 0x01f3
 * @param supervision_timeout supervision timeout for the le link.
 *        it shall be a multiple of 10 ms and larger than (1 +
 *        connperipherallatency) * conninterval * 2.
 *        time = n * 10 ms.
 *        values:
 *        - 0x000a (100 ms)  ... 0x0c80 (32000 ms)
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
 * @param num_of_peer_entries number of devices that have to be added to the
 *        filter accept list. each device is defined by peer_address_type and
 *        peer_address.
 * @param peer_entry see @ref peer_entry_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_start_auto_connection_establish_proc(
	uint16_t le_scan_interval, uint16_t le_scan_window, uint8_t own_address_type,
	uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t conn_latency,
	uint16_t supervision_timeout, uint16_t minimum_ce_length, uint16_t maximum_ce_length,
	uint8_t num_of_peer_entries, const peer_entry_t *peer_entry);

/**
 * @brief aci_gap_start_general_connection_establish_proc
 * starts a general connection establishment procedure. the host enables
 * scanning in the controller with the scanner filter policy set to "accept all
 * advertising packets" and from the scanning results, all the devices are sent
 * to the upper layer by the event hci_le_advertising_report_event (or by the
 * event hci_le_extended_advertising_report_event when the extended advertising
 * feature is supported). the upper layer then has to select one of the devices
 * to which it wants to connect by issuing the command
 * aci_gap_create_connection. if privacy is enabled, then either a private
 * resolvable address or a non-resolvable address, based on the address type
 * specified in the command is set as the scanner address but the gap create
 * connection always uses a private resolvable address if the general
 * connection establishment procedure is active.
 * before the call to aci_gap_create_connection, the procedure can be
 * terminated by issuing the command aci_gap_terminate_gap_proc with the
 * procedure code set to 0x10.
 * after the call to aci_gap_create_connection, the procedure is terminated
 * when a connection is established, or the upper layer terminates the
 * procedure by issuing the command aci_gap_terminate_gap_proc with the
 * procedure code set to 0x40. on completion of the procedure a
 * aci_gap_proc_complete_event event is generated with the procedure code set
 * to 0x40.
 * if privacy is enabled and the peer device (advertiser) is in the resolving
 * list then the link layer generates a rpa.
 *
 * @param le_scan_type passive or active scanning. with passive scanning, no
 *        scan request pdus are sent.
 *        values:
 *        - 0x00: passive scanning
 *        - 0x01: active scanning
 * @param le_scan_interval this is defined as the time interval from when the
 *        controller started its last le scan until it begins the subsequent le
 *        scan.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms) : legacy advertising
 *        - 0x0004 (2.500 ms)  ... 0x5dc0 (15000.000 ms) : extended advertising
 * @param le_scan_window amount of time for the duration of the le scan.
 *        le_scan_window shall be less than or equal to le_scan_interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms) : legacy advertising
 *        - 0x0004 (2.500 ms)  ... 0x5dc0 (15000.000 ms) : extended advertising
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @param scanning_filter_policy the scanning filter policy determines how the
 *        scanner's link layer processes advertising and scan response pdus.
 *        there is a choice of two primary filter policies: unfiltered and
 *        filtered.
 *        unfiltered: the link layer processes all advertising and scan
 *        response pdus (i.e., the filter accept list is not used).
 *        filtered: the link layer processes advertising and scan response pdus
 *        only from devices in the filter accept list.
 *        with extended scanning filter policies, a directed advertising pdu
 *        accepted by the primary filter policy shall nevertheless be ignored
 *        unless either the targeta field is identical to the scanner's device
 *        address, or targeta field is a resolvable private address.
 *        values:
 *        - 0x00: basic unfiltered scanning filter policy
 *        - 0x01: basic filtered scanning filter policy
 *        - 0x02: extended unfiltered scanning filter policy
 *        - 0x03: extended filtered scanning filter policy
 * @param filter_duplicates enable/disable duplicate filtering.
 *        values:
 *        - 0x00: duplicate filtering disabled
 *        - 0x01: duplicate filtering enabled
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_start_general_connection_establish_proc(
	uint8_t le_scan_type, uint16_t le_scan_interval, uint16_t le_scan_window,
	uint8_t own_address_type, uint8_t scanning_filter_policy, uint8_t filter_duplicates);

/**
 * @brief aci_gap_start_selective_connection_establish_proc
 * starts a selective connection establishment procedure. the gap adds the
 * specified device addresses into filter accept list and enables scanning in
 * the controller with a scanning filter policy that should be set to
 * "filtered". all the devices found are sent to the upper layer by the event
 * hci_le_advertising_report_event (or by the event
 * hci_le_extended_advertising_report_event when the extended advertising
 * feature is supported). the upper layer then has to select one of the devices
 * to which it wants to connect by issuing the command
 * aci_gap_create_connection.
 * before the call to aci_gap_create_connection, the procedure can be
 * terminated by issuing the command aci_gap_terminate_gap_proc with the
 * procedure code set to 0x20.
 * after the call to aci_gap_create_connection, the procedure is terminated
 * when a connection is established, or the upper layer terminates the
 * procedure by issuing the command aci_gap_terminate_gap_proc with the
 * procedure code set to 0x40. on completion of the procedure a
 * aci_gap_proc_complete_event event is generated with the procedure code set
 * to 0x40.
 * if privacy is enabled and the peer device (advertiser) is in the resolving
 * list then the link layer generates a rpa.
 *
 * @param le_scan_type passive or active scanning. with passive scanning, no
 *        scan request pdus are sent.
 *        values:
 *        - 0x00: passive scanning
 *        - 0x01: active scanning
 * @param le_scan_interval this is defined as the time interval from when the
 *        controller started its last le scan until it begins the subsequent le
 *        scan.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms) : legacy advertising
 *        - 0x0004 (2.500 ms)  ... 0x5dc0 (15000.000 ms) : extended advertising
 * @param le_scan_window amount of time for the duration of the le scan.
 *        le_scan_window shall be less than or equal to le_scan_interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms) : legacy advertising
 *        - 0x0004 (2.500 ms)  ... 0x5dc0 (15000.000 ms) : extended advertising
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @param scanning_filter_policy the scanning filter policy determines how the
 *        scanner's link layer processes advertising and scan response pdus.
 *        there is a choice of two primary filter policies: unfiltered and
 *        filtered.
 *        unfiltered: the link layer processes all advertising and scan
 *        response pdus (i.e., the filter accept list is not used).
 *        filtered: the link layer processes advertising and scan response pdus
 *        only from devices in the filter accept list.
 *        with extended scanning filter policies, a directed advertising pdu
 *        accepted by the primary filter policy shall nevertheless be ignored
 *        unless either the targeta field is identical to the scanner's device
 *        address, or targeta field is a resolvable private address.
 *        values:
 *        - 0x00: basic unfiltered scanning filter policy
 *        - 0x01: basic filtered scanning filter policy
 *        - 0x02: extended unfiltered scanning filter policy
 *        - 0x03: extended filtered scanning filter policy
 * @param filter_duplicates enable/disable duplicate filtering.
 *        values:
 *        - 0x00: duplicate filtering disabled
 *        - 0x01: duplicate filtering enabled
 * @param num_of_peer_entries number of devices that have to be added to the
 *        filter accept list. each device is defined by peer_address_type and
 *        peer_address.
 * @param peer_entry see @ref peer_entry_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_start_selective_connection_establish_proc(
	uint8_t le_scan_type, uint16_t le_scan_interval, uint16_t le_scan_window,
	uint8_t own_address_type, uint8_t scanning_filter_policy, uint8_t filter_duplicates,
	uint8_t num_of_peer_entries, const peer_entry_t *peer_entry);

/**
 * @brief aci_gap_create_connection
 * starts the direct connection establishment procedure. a le_create_connection
 * call will be made to the controller by gap with the initiator filter policy
 * set to "filter accept list is not used to determine which advertiser to
 * connect to". the procedure can be terminated explicitly by the upper layer
 * by issuing the command aci_gap_terminate_gap_proc. when a command is issued
 * to terminate the procedure by upper layer, a hci_le_create_connection_cancel
 * call will be made to the controller by gap.
 * on termination of the procedure, a hci_le_connection_complete_event (or
 * hci_le_enhanced_connection_complete_event if privacy or extended advertising
 * is used) is returned. the procedure can be explicitly terminated by the
 * upper layer by issuing the command aci_gap_terminate_gap_proc with the
 * procedure_code set to 0x40.
 * if privacy is enabled and the peer device (advertiser) is in the resolving
 * list then the link layer generates a rpa.
 *
 * @param le_scan_interval this is defined as the time interval from when the
 *        controller started its last le scan until it begins the subsequent le
 *        scan.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms) : legacy advertising
 *        - 0x0004 (2.500 ms)  ... 0x5dc0 (15000.000 ms) : extended advertising
 * @param le_scan_window amount of time for the duration of the le scan.
 *        le_scan_window shall be less than or equal to le_scan_interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms) : legacy advertising
 *        - 0x0004 (2.500 ms)  ... 0x5dc0 (15000.000 ms) : extended advertising
 * @param peer_address_type the address type of the peer device.
 *        values:
 *        - 0x00: public device address
 *        - 0x01: random device address
 * @param peer_address public device address or random device address of the
 *        device to be connected.
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 * @param conn_interval_min minimum value for the connection event interval.
 *        time = n * 1.25 ms.
 *        values:
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @param conn_interval_max maximum value for the connection event interval.
 *        time = n * 1.25 ms.
 *        values:
 *        - 0x0006 (7.50 ms)  ... 0x0c80 (4000.00 ms)
 * @param conn_latency maximum peripheral latency for the connection in number
 *        of connection events.
 *        values:
 *        - 0x0000 ... 0x01f3
 * @param supervision_timeout supervision timeout for the le link.
 *        it shall be a multiple of 10 ms and larger than (1 +
 *        connperipherallatency) * conninterval * 2.
 *        time = n * 10 ms.
 *        values:
 *        - 0x000a (100 ms)  ... 0x0c80 (32000 ms)
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
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_create_connection(uint16_t le_scan_interval, uint16_t le_scan_window,
									   uint8_t peer_address_type, const uint8_t *peer_address,
									   uint8_t own_address_type, uint16_t conn_interval_min,
									   uint16_t conn_interval_max, uint16_t conn_latency,
									   uint16_t supervision_timeout, uint16_t minimum_ce_length,
									   uint16_t maximum_ce_length);

/**
 * @brief aci_gap_terminate_gap_proc
 * terminates the specified gap procedure. an aci_gap_proc_complete_event event
 * is returned with the procedure code set to the corresponding procedure.
 * note: in case of gap procedure started with aci_gap_create_connection or
 * aci_gap_start_auto_connection_establish_proc, a
 * hci_le_create_connection_cancel is issued to the controller.
 *
 * @param procedure_code gap procedure bitmap.
 *        values:
 *        - 0x00: no events
 *        - 0x01: gap_limited_discovery_proc
 *        - 0x02: gap_general_discovery_proc
 *        - 0x08: gap_auto_connection_establishment_proc
 *        - 0x10: gap_general_connection_establishment_proc
 *        - 0x20: gap_selective_connection_establishment_proc
 *        - 0x40: gap_direct_connection_establishment_proc
 *        - 0x80: gap_observation_proc
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_terminate_gap_proc(uint8_t procedure_code);

/**
 * @brief aci_gap_start_connection_update
 * starts the connection update procedure (only when role is central). a
 * hci_le_connection_update is called.
 * on completion of the procedure, an hci_le_connection_update_complete_event
 * event is returned to the upper layer.
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
 * @param conn_latency maximum peripheral latency for the connection in number
 *        of connection events.
 *        values:
 *        - 0x0000 ... 0x01f3
 * @param supervision_timeout supervision timeout for the le link.
 *        it shall be a multiple of 10 ms and larger than (1 +
 *        connperipherallatency) * conninterval * 2.
 *        time = n * 10 ms.
 *        values:
 *        - 0x000a (100 ms)  ... 0x0c80 (32000 ms)
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
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_start_connection_update(uint16_t connection_handle, uint16_t conn_interval_min,
											 uint16_t conn_interval_max, uint16_t conn_latency,
											 uint16_t supervision_timeout,
											 uint16_t minimum_ce_length,
											 uint16_t maximum_ce_length);

/**
 * @brief aci_gap_send_pairing_req
 * sends the sm pairing request to start a pairing process. the authentication
 * requirements and io capabilities should be set before issuing this command
 * using the aci_gap_set_io_capability and
 * aci_gap_set_authentication_requirement commands.
 * a aci_gap_pairing_complete_event event is returned after the pairing process
 * is completed.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param force_rebond if 1, pairing request will be sent even if the device
 *        was previously bonded, otherwise pairing request is not sent.
 *        values:
 *        - 0x00: no
 *        - 0x01: yes
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_send_pairing_req(uint16_t connection_handle, uint8_t force_rebond);

/**
 * @brief aci_gap_set_broadcast_mode
 * this command puts the device into broadcast mode. a privacy enabled device
 * uses either a resolvable private address or a non-resolvable private address
 * as specified in the own_address_type parameter of the command.
 * note: this command only supports legacy advertising. for extended
 * advertising, refer to aci_gap_adv_set_configuration and
 * aci_gap_adv_set_enable.
 *
 * @param advertising_interval_min minimum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms)
 * @param advertising_interval_max maximum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms)
 * @param advertising_type advertising type
 *        values:
 *        - 0x02: adv_scan_ind (scannable undirected advertising)
 *        - 0x03: adv_nonconn_ind (non connectable undirected advertising)
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @param adv_data_length length of the advertising data in the advertising
 *        packet.
 * @param adv_data advertising data used by the device while advertising.
 * @param num_of_peer_entries number of devices that have to be added to the
 *        filter accept list. each device is defined by peer_address_type and
 *        peer_address.
 * @param peer_entry see @ref peer_entry_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_broadcast_mode(uint16_t advertising_interval_min,
										uint16_t advertising_interval_max, uint8_t advertising_type,
										uint8_t own_address_type, uint8_t adv_data_length,
										const uint8_t *adv_data, uint8_t num_of_peer_entries,
										const peer_entry_t *peer_entry);

/**
 * @brief aci_gap_start_observation_proc
 * starts an observation procedure when the device is in observer role. the
 * host enables scanning in the controller. the advertising reports are sent to
 * the upper layer using standard le advertising report event.
 * if privacy is enabled and the peer device (advertiser) is in the resolving
 * list then the link layer will generate a rpa, if it is not then the rpa/nrpa
 * generated by the host will be used.
 *
 * @param le_scan_interval this is defined as the time interval from when the
 *        controller started its last le scan until it begins the subsequent le
 *        scan.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms) : legacy advertising
 *        - 0x0004 (2.500 ms)  ... 0x5dc0 (15000.000 ms) : extended advertising
 * @param le_scan_window amount of time for the duration of the le scan.
 *        le_scan_window shall be less than or equal to le_scan_interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0004 (2.500 ms)  ... 0x4000 (10240.000 ms) : legacy advertising
 *        - 0x0004 (2.500 ms)  ... 0x5dc0 (15000.000 ms) : extended advertising
 * @param le_scan_type passive or active scanning. with passive scanning, no
 *        scan request pdus are sent.
 *        values:
 *        - 0x00: passive scanning
 *        - 0x01: active scanning
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @param filter_duplicates enable/disable duplicate filtering.
 *        values:
 *        - 0x00: duplicate filtering disabled
 *        - 0x01: duplicate filtering enabled
 * @param scanning_filter_policy the scanning filter policy determines how the
 *        scanner's link layer processes advertising and scan response pdus.
 *        there is a choice of two primary filter policies: unfiltered and
 *        filtered.
 *        unfiltered: the link layer processes all advertising and scan
 *        response pdus (i.e., the filter accept list is not used).
 *        filtered: the link layer processes advertising and scan response pdus
 *        only from devices in the filter accept list.
 *        with extended scanning filter policies, a directed advertising pdu
 *        accepted by the primary filter policy shall nevertheless be ignored
 *        unless either the targeta field is identical to the scanner's device
 *        address, or targeta field is a resolvable private address.
 *        values:
 *        - 0x00: basic unfiltered scanning filter policy
 *        - 0x01: basic filtered scanning filter policy
 *        - 0x02: extended unfiltered scanning filter policy
 *        - 0x03: extended filtered scanning filter policy
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_start_observation_proc(uint16_t le_scan_interval, uint16_t le_scan_window,
											uint8_t le_scan_type, uint8_t own_address_type,
											uint8_t filter_duplicates,
											uint8_t scanning_filter_policy);

/**
 * @brief aci_gap_get_bonded_devices
 * this command gets the list of the devices which are present in the bonding
 * table. it returns the number of addresses and the corresponding address
 * types and values.
 *
 * @param[out] num_of_addresses the number of bonded devices
 * @param[out] bonded_device_entry see @ref bonded_device_entry_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_get_bonded_devices(uint8_t *num_of_addresses,
										bonded_device_entry_t *bonded_device_entry);

/**
 * @brief aci_gap_check_bonded_device
 * the command finds whether the device, whose address is specified in the
 * command, is present in the bonding table. if the device is found, the
 * command returns "success".
 * note: the specified address can be a resolvable private address (rpa). in
 * this case, even if privacy is not enabled, this address is resolved to check
 * the presence of the peer device in the bonding table. if the address is
 * resolved successfully with any one of the irks present in the database, the
 * command returns success and also the corresponding public or static random
 * address stored with the irk in the database (i.e. the peer device identity
 * address distributed during bonding).
 *
 * @param peer_address_type the address type of the peer device.
 *        values:
 *        - 0x00: public device address
 *        - 0x01: random device address
 * @param peer_address public device address or random device address of the
 *        peer device
 * @param[out] id_address_type the address type of the peer device, distributed
 *        during pairing phase.
 * @param[out] id_address the public or static random address of the peer
 *        device, distributed during pairing phase.
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_check_bonded_device(uint8_t peer_address_type, const uint8_t *peer_address,
										 uint8_t *id_address_type, uint8_t *id_address);

/**
 * @brief aci_gap_numeric_comparison_value_confirm_yesno
 * this command allows the user to validate/confirm or not the numeric
 * comparison value showed through the aci_gap_numeric_comparison_value_event.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param confirm_yes_no indicates if the numeric values showed on both local
 *        and peer device are different or equal
 *        values:
 *        - 0x00: no (numeric values are different)
 *        - 0x01: yes (numeric values are equal)
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_numeric_comparison_value_confirm_yesno(uint16_t connection_handle,
															uint8_t confirm_yes_no);

/**
 * @brief aci_gap_passkey_input
 * this command permits to signal to the stack the input type detected during
 * passkey input.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param input_type passkey input type detected
 *        values:
 *        - 0x00: passkey entry started
 *        - 0x01: passkey digit entered
 *        - 0x02: passkey digit erased
 *        - 0x03: passkey cleared
 *        - 0x04: passkey entry completed
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_passkey_input(uint16_t connection_handle, uint8_t input_type);

/**
 * @brief aci_gap_get_oob_data
 * this command is sent by the user to get (i.e. to extract from the stack) the
 * oob data generated by the stack itself.
 *
 * @param oob_data_type type of oob data
 *        values:
 *        - 0x00: tk (legacy pairing)
 *        - 0x01: random (sc)
 *        - 0x02: confirm (sc)
 * @param[out] address_type identity address type
 *        values:
 *        - 0x00: public identity address
 *        - 0x01: random (static) identity address
 * @param[out] address public or random (static) address of this device
 * @param[out] oob_data_len length of oob data
 *        values:
 *        - 16
 * @param[out] oob_data local oob data
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_get_oob_data(uint8_t oob_data_type, uint8_t *address_type, uint8_t *address,
								  uint8_t *oob_data_len, uint8_t *oob_data);

/**
 * @brief aci_gap_set_oob_data
 * this command is sent (by the user) to input the oob data arrived via oob
 * communication.
 *
 * @param device_type oob device type
 *        values:
 *        - 0x00: local device (address_type and address are not used)
 *        - 0x01: remote device
 * @param address_type identity address type
 *        values:
 *        - 0x00: public identity address
 *        - 0x01: random (static) identity address
 * @param address public or random (static) identity address of the peer device
 * @param oob_data_type type of oob data
 *        values:
 *        - 0x00: tk (legacy pairing)
 *        - 0x01: random (sc)
 *        - 0x02: confirm (sc)
 * @param oob_data_len length of oob data
 *        values:
 *        - 0: sc random/confirm generation (oob_data and oob_data_type are not
 *          used)
 *        - 16
 * @param oob_data either local oob data or remote oob data received through
 *        oob from peer device (see device_type)
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_set_oob_data(uint8_t device_type, uint8_t address_type, const uint8_t *address,
								  uint8_t oob_data_type, uint8_t oob_data_len,
								  const uint8_t *oob_data);

/**
 * @brief aci_gap_remove_bonded_device
 * this command removes a specified device from bonding table; i.e. it removes
 * from bonding table all security and gatt information related to the
 * specified device.
 *
 * @param peer_identity_address_type identity address type
 *        values:
 *        - 0x00: public identity address
 *        - 0x01: random (static) identity address
 * @param peer_identity_address public or random (static) identity address of
 *        the peer device
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_remove_bonded_device(uint8_t peer_identity_address_type,
										  const uint8_t *peer_identity_address);

/**
 * @brief aci_gap_add_devices_to_list
 * this command is used to add specific device addresses to the filter accept
 * list and/or resolving list.
 *
 * @param num_of_list_entries number of devices that have to be added to the
 *        list.
 * @param list_entry see @ref list_entry_t
 * @param mode mode used for adding devices in the lists.
 *        values:
 *        - 0x00: append to the resolving list only
 *        - 0x01: clear and set the resolving list only
 *        - 0x02: append to the filter accept list only
 *        - 0x03: clear and set the filter accept list only
 *        - 0x04: append to both resolving list and filter accept list
 *        - 0x05: clear and set both resolving list and filter accept list
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_add_devices_to_list(uint8_t num_of_list_entries,
										 const list_entry_t *list_entry, uint8_t mode);

/**
 * @brief aci_gap_pairing_request_reply
 * this command is used to reply to aci_gap_pairing_request_event. it enables
 * to allow or reject either the pairing request from the central or the
 * security request from the peripheral.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param accept enables to accept or reject the pairing request.
 *        values:
 *        - 0x00: reject
 *        - 0x01: accept
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_pairing_request_reply(uint16_t connection_handle, uint8_t accept);

/**
 * @brief aci_gap_additional_beacon_start
 * this command starts an advertising beacon. it allows additional advertising
 * packets to be transmitted independently of the packets transmitted with gap
 * advertising commands such as aci_gap_set_discoverable or
 * aci_gap_set_limited_discoverable.
 *
 * @param adv_interval_min minimum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms)
 * @param adv_interval_max maximum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x0020 (20.000 ms)  ... 0x4000 (10240.000 ms)
 * @param adv_channel_map advertising channel map.
 *        flags:
 *        - 0x01: channel 37 shall be used
 *        - 0x02: channel 38 shall be used
 *        - 0x04: channel 39 shall be used
 * @param own_address_type own address type: public or static random.
 *        values:
 *        - 0x00: public device address
 *        - 0x01: random device address
 * @param own_address public device address or random device address.
 * @param pa_level power amplifier output level.
 *        values:
 *        - 0x00 ... 0x23
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_additional_beacon_start(uint16_t adv_interval_min, uint16_t adv_interval_max,
											 uint8_t adv_channel_map, uint8_t own_address_type,
											 const uint8_t *own_address, uint8_t pa_level);

/**
 * @brief aci_gap_additional_beacon_stop
 * this command stops the advertising beacon started with
 * aci_gap_additional_beacon_start.
 *
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_additional_beacon_stop(void);

/**
 * @brief aci_gap_additional_beacon_set_data
 * this command sets the data transmitted by the advertising beacon started
 * with aci_gap_additional_beacon_start. if the advertising beacon is already
 * started, the new data is used in subsequent beacon advertising events.
 *
 * @param adv_data_length length of adv_data in octets
 * @param adv_data advertising data used by the device while advertising.
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_additional_beacon_set_data(uint8_t adv_data_length, const uint8_t *adv_data);

/**
 * @brief aci_gap_adv_set_configuration
 * this command is used to set the extended advertising configuration for one
 * advertising set.
 * this command, in association with aci_gap_adv_set_scan_resp_data,
 * aci_gap_adv_set_adv_data and aci_gap_adv_set_enable, enables to start
 * extended advertising. these commands must be used in replacement of
 * aci_gap_set_discoverable, aci_gap_set_limited_discoverable,
 * aci_gap_set_direct_connectable, aci_gap_set_non_connectable,
 * aci_gap_set_undirected_connectable and aci_gap_set_broadcast_mode that only
 * support legacy advertising.
 * if bit 0 of adv_mode is set, the own_address_type parameter is ignored and
 * the own address shall be set with the aci_gap_adv_set_random_address
 * command. this mode is only valid for non-connectable advertising.
 *
 * @param adv_mode bitmap of extended advertising modes
 *        flags:
 *        - 0x01: use specific random address
 * @param advertising_handle used to identify an advertising set.
 *        values:
 *        - 0x00 ... 0xef
 * @param adv_event_properties type of advertising event.
 *        flags:
 *        - 0x0001: connectable advertising
 *        - 0x0002: scannable advertising
 *        - 0x0004: directed advertising
 *        - 0x0008: high duty cycle directed connectable advertising
 *        - 0x0010: use legacy advertising pdus
 *        - 0x0020: anonymous advertising
 *        - 0x0040: include txpower in at least one advertising pdu
 * @param primary_adv_interval_min minimum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x00000020 (20.000 ms)  ... 0x00ffffff (10485759.375 ms)
 * @param primary_adv_interval_max maximum advertising interval.
 *        time = n * 0.625 ms.
 *        values:
 *        - 0x00000020 (20.000 ms)  ... 0x00ffffff (10485759.375 ms)
 * @param primary_adv_channel_map advertising channel map.
 *        flags:
 *        - 0x01: channel 37 shall be used
 *        - 0x02: channel 38 shall be used
 *        - 0x04: channel 39 shall be used
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @param peer_address_type address type of the peer device.
 *        values:
 *        - 0x00: public device address or public identity address
 *        - 0x01: random device address or random (static) identity address
 * @param peer_address public device address, random device address, public
 *        identity address, or random (static) identity address of the device
 *        to be connected.
 * @param adv_filter_policy advertising filter policy
 *        values:
 *        - 0x00: process scan and connection requests from all devices (i.e.,
 *          the filter accept list is not in use)
 *        - 0x01: process connection requests from all devices and scan
 *          requests only from devices that are in the filter accept list.
 *        - 0x02: process scan requests from all devices and connection
 *          requests only from devices that are in the filter accept list.
 *        - 0x03: process scan and connection requests only from devices in the
 *          filter accept list.
 * @param adv_tx_power advertising tx power. units: dbm.
 *        values:
 *        - 127: host has no preference
 *        - -127 ... 20
 * @param secondary_adv_max_skip secondary advertising maximum skip.
 *        values:
 *        - 0x00: aux_adv_ind shall be sent prior to the next advertising event
 *        - 0x01 ... 0xff: maximum advertising events the controller can skip
 *          before sending the aux_adv_ind packets on the secondary advertising
 *          physical channel
 * @param secondary_adv_phy secondary advertising phy.
 *        values:
 *        - 0x01: secondary advertisement phy is le 1m
 *        - 0x02: secondary advertisement phy is le 2m
 * @param adv_sid value of the advertising sid subfield in the adi field of the
 *        pdu.
 *        values:
 *        - 0x00 ... 0x0f
 * @param scan_req_notification_enable scan request notifications.
 *        values:
 *        - 0x00: scan request notifications disabled
 *        - 0x01: scan request notifications enabled
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_adv_set_configuration(
	uint8_t adv_mode, uint8_t advertising_handle, uint16_t adv_event_properties,
	uint32_t primary_adv_interval_min, uint32_t primary_adv_interval_max,
	uint8_t primary_adv_channel_map, uint8_t own_address_type, uint8_t peer_address_type,
	const uint8_t *peer_address, uint8_t adv_filter_policy, uint8_t adv_tx_power,
	uint8_t secondary_adv_max_skip, uint8_t secondary_adv_phy, uint8_t adv_sid,
	uint8_t scan_req_notification_enable);

/**
 * @brief aci_gap_adv_set_enable
 * this command is used to request the controller to enable or disable one or
 * more extended advertising sets.
 *
 * @param enable enable/disable advertising.
 *        values:
 *        - 0x00: advertising is disabled
 *        - 0x01: advertising is enabled
 * @param num_sets number of advertising sets.
 *        values:
 *        - 0x00: disable all advertising sets
 *        - 0x01 ... 0x3f: number of advertising sets to enable or disable
 * @param adv_set see @ref adv_set_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_adv_set_enable(uint8_t enable, uint8_t num_sets, const adv_set_t *adv_set);

/**
 * @brief aci_gap_adv_set_adv_data
 * this command is used to set the data used in extended advertising pdus that
 * have a data field.
 *
 * @param advertising_handle used to identify an advertising set.
 *        values:
 *        - 0x00 ... 0xef
 * @param operation advertising operation.
 *        values:
 *        - 0x00: intermediate fragment of fragmented extended advertising data
 *        - 0x01: first fragment of fragmented extended advertising data
 *        - 0x02: last fragment of fragmented extended advertising data
 *        - 0x03: complete extended advertising data
 *        - 0x04: unchanged data (just update the advertising did)
 * @param fragment_preference fragment preference.
 *        values:
 *        - 0x00: the controller may fragment all data
 *        - 0x01: the controller should not fragment or should minimize
 *          fragmentation of data
 * @param advertising_data_length length of advertising_data in octets
 * @param advertising_data data formatted as defined in core specification [vol
 *        3, part c, 11].
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_adv_set_adv_data(uint8_t advertising_handle, uint8_t operation,
									  uint8_t fragment_preference, uint8_t advertising_data_length,
									  const uint8_t *advertising_data);

/**
 * @brief aci_gap_adv_set_scan_resp_data
 * this command is used to provide scan response data used during extended
 * advertising.
 *
 * @param advertising_handle used to identify an advertising set.
 *        values:
 *        - 0x00 ... 0xef
 * @param operation scan response operation.
 *        values:
 *        - 0x00: intermediate fragment of fragmented scan response data
 *        - 0x01: first fragment of fragmented scan response data
 *        - 0x02: last fragment of fragmented scan response data
 *        - 0x03: complete scan response data
 * @param fragment_preference fragment preference.
 *        values:
 *        - 0x00: the controller may fragment all data
 *        - 0x01: the controller should not fragment or should minimize
 *          fragmentation of data
 * @param scan_response_data_length length of scan_response_data in octets
 * @param scan_response_data data formatted as defined in core specification
 *        [vol 3, part c, 11].
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_adv_set_scan_resp_data(uint8_t advertising_handle, uint8_t operation,
											uint8_t fragment_preference,
											uint8_t scan_response_data_length,
											const uint8_t *scan_response_data);

/**
 * @brief aci_gap_adv_remove_set
 * this command is used to remove an advertising set from the controller.
 *
 * @param advertising_handle used to identify an advertising set.
 *        values:
 *        - 0x00 ... 0xef
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_adv_remove_set(uint8_t advertising_handle);

/**
 * @brief aci_gap_adv_clear_sets
 * this command is used to remove all existing advertising sets from the
 * controller.
 *
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_adv_clear_sets(void);

/**
 * @brief aci_gap_adv_set_random_address
 * this command is used to set the random device address of an advertising set
 * configured to use specific random address.
 *
 * @param advertising_handle used to identify an advertising set.
 *        values:
 *        - 0x00 ... 0xef
 * @param random_address random device address.
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_adv_set_random_address(uint8_t advertising_handle,
											const uint8_t *random_address);

/**
 * @brief aci_gap_ext_start_scan
 * this command is used to start a scanning procedure when the extended
 * advertising feature is supported.
 * the scanning_phys and subsequent parameters are used to specify the scanning
 * parameters as defined for hci_le_set_extended_scan_parameters: for more
 * details, refer to core specification [vol 4, part e, 7.8.64].
 * note: this more generic command can be used instead of
 * aci_gap_start_limited_discovery_proc, aci_gap_start_general_discovery_proc,
 * aci_gap_start_general_connection_establish_proc,
 * aci_gap_start_selective_connection_establish_proc or
 * aci_gap_start_observation_proc.
 *
 * @param scan_mode reserved, shall be set to 0.
 *        values:
 *        - 0x00
 * @param procedure scan procedure.
 *        values:
 *        - 0x01: gap_limited_discovery_proc
 *        - 0x02: gap_general_discovery_proc
 *        - 0x10: gap_general_connection_establishment_proc
 *        - 0x20: gap_selective_connection_establishment_proc
 *        - 0x80: gap_observation_proc
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address or a non-resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 *        - 0x03: non-resolvable private address
 * @param filter_duplicates duplicate filtering.
 *        values:
 *        - 0x00: duplicate filtering disabled
 *        - 0x01: duplicate filtering enabled
 *        - 0x02: duplicate filtering enabled, reset for each scan period
 * @param duration scan duration.
 *        time = n * 10 ms.
 *        values:
 *        - 0x0000 (0 ms) : scan continuously until explicitly disable
 *        - 0x0001 (10 ms)  ... 0xffff (655350 ms) : scan duration
 * @param period scan period.
 *        time = n * 1.28 s.
 *        values:
 *        - 0x0000 (0 ms) : scan continuously
 *        - 0x0001 (1280 ms)  ... 0xffff (83884800 ms) : time interval from
 *          when the controller started its last scan_duration until it begins
 *          the subsequent scan_duration
 * @param scanning_filter_policy the scanning filter policy determines how the
 *        scanner's link layer processes advertising and scan response pdus.
 *        there is a choice of two primary filter policies: unfiltered and
 *        filtered.
 *        unfiltered: the link layer processes all advertising and scan
 *        response pdus (i.e., the filter accept list is not used).
 *        filtered: the link layer processes advertising and scan response pdus
 *        only from devices in the filter accept list.
 *        with extended scanning filter policies, a directed advertising pdu
 *        accepted by the primary filter policy shall nevertheless be ignored
 *        unless either the targeta field is identical to the scanner's device
 *        address, or targeta field is a resolvable private address.
 *        values:
 *        - 0x00: basic unfiltered scanning filter policy
 *        - 0x01: basic filtered scanning filter policy
 *        - 0x02: extended unfiltered scanning filter policy
 *        - 0x03: extended filtered scanning filter policy
 * @param scanning_phys scan phys.
 *        flags:
 *        - 0x01: scan advertisements on the le 1m phy
 * @param scan_param_phy see @ref scan_param_phy_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_ext_start_scan(uint8_t scan_mode, uint8_t procedure, uint8_t own_address_type,
									uint8_t filter_duplicates, uint16_t duration, uint16_t period,
									uint8_t scanning_filter_policy, uint8_t scanning_phys,
									const scan_param_phy_t *scan_param_phy);

/**
 * @brief aci_gap_ext_create_connection
 * this command is used to create a connection with the local device in the
 * central role to an advertiser when the extended advertising feature is
 * supported.
 * the advertising_handle and subevent parameters are ignored.
 * the initiating_phys and subsequent parameters are used to specify the
 * initiating parameters as defined for hci_le_extended_create_connection: for
 * more details, refer to core specification [vol 4, part e, 7.8.66].
 * note: this more generic command can be used instead of
 * aci_gap_create_connection or aci_gap_start_auto_connection_establish_proc.
 *
 * @param initiating_mode reserved, shall be set to 0.
 *        values:
 *        - 0x00
 * @param procedure connection procedure.
 *        values:
 *        - 0x08: gap_auto_connection_establishment_proc
 *        - 0x40: gap_direct_connection_establishment_proc
 * @param own_address_type own address type: if privacy is disabled, the
 *        address can be public or static random; otherwise, it can be a
 *        resolvable private address.
 *        values:
 *        - 0x00: public address
 *        - 0x01: static random address
 *        - 0x02: resolvable private address
 * @param peer_address_type the address type of the peer device.
 *        values:
 *        - 0x00: public device address
 *        - 0x01: random device address
 * @param peer_address public device address or random device address of the
 *        device to be connected.
 * @param advertising_handle used to identify the subevent where a connection
 *        request shall be initiated from a periodic advertising train.
 *        values:
 *        - 0xff: parameter not used
 *        - 0x00 ... 0xef
 * @param subevent subevent where the connection request is to be sent.
 *        values:
 *        - 0xff: parameter not used
 *        - 0x00 ... 0x7f
 * @param initiator_filter_policy initiator filter policy.
 *        values:
 *        - 0x00: filter accept list is not used to determine which advertiser
 *          to connect to
 *        - 0x01: filter accept list is used to determine which advertiser to
 *          connect to (peer_address_type and peer_address are ignored)
 * @param initiating_phys initiating phys.
 *        flags:
 *        - 0x01: scan connectable advertisements on the le 1m phy- connection
 *          parameters for the le 1m phy
 *        - 0x02: connection parameters for the le 2m phy
 * @param init_param_phy see @ref init_param_phy_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gap_ext_create_connection(uint8_t initiating_mode, uint8_t procedure,
										   uint8_t own_address_type, uint8_t peer_address_type,
										   const uint8_t *peer_address, uint8_t advertising_handle,
										   uint8_t subevent, uint8_t initiator_filter_policy,
										   uint8_t initiating_phys,
										   const init_param_phy_t *init_param_phy);

#endif /* BLE_GAP_ACI_H__ */
