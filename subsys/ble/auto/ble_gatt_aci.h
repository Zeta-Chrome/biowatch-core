/*****************************************************************************
 * @file    ble_gatt_aci.h
 * @brief   STM32WB BLE API (GATT_ACI)
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

#ifndef BLE_GATT_ACI_H__
#define BLE_GATT_ACI_H__

#include "ble_types.h"

/**
 * @brief aci_gatt_init
 * initializes the gatt layer for server and client roles. it also adds the
 * gatt service with service changed characteristic.
 * until this command is issued the gatt channel does not process any commands
 * even if the connection is opened. this command has to be given before using
 * any of the gap features.
 *
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_init(void);

/**
 * @brief aci_gatt_add_service
 * add a service to gatt server. when a service is created in the server, the
 * host needs to reserve the handle ranges for this service using
 * max_attribute_records parameter. this parameter specifies the maximum number
 * of attribute records that can be added to this service (including the
 * service attribute, include attribute, characteristic attribute,
 * characteristic value attribute and characteristic descriptor attribute).
 * handle of the created service is returned in command complete event. service
 * declaration is taken from the service pool.
 * the attributes for characteristics and descriptors are allocated from the
 * attribute pool.
 *
 * @param service_uuid_type uuid type: 0x01 = 16 bits uuid while 0x02 = 128
 *        bits uuid
 * @param service_uuid see @ref service_uuid_t
 * @param service_type service type.
 *        values:
 *        - 0x01: primary service
 *        - 0x02: secondary service
 * @param max_attribute_records maximum number of attribute records that can be
 *        added to this service
 * @param[out] service_handle handle of the service.
 *        when this service is added, a handle is allocated by the server for
 *        this service.
 *        server also allocates a range of handles for this service from
 *        servicehandle to <servicehandle + max_attr_records - 1>
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_add_service(uint8_t service_uuid_type, const service_uuid_t *service_uuid,
								  uint8_t service_type, uint8_t max_attribute_records,
								  uint16_t *service_handle);

/**
 * @brief aci_gatt_include_service
 * include a service given by include_start_handle and include_end_handle to
 * another service given by service_handle. attribute server creates an include
 * definition attribute and return the handle of this attribute in
 * included_handle.
 *
 * @param service_handle handle of the service to which another service has to
 *        be included.
 * @param include_start_handle start handle of the service which has to be
 *        included in service
 * @param include_end_handle end handle of the service which has to be included
 *        in service
 * @param include_uuid_type uuid type: 0x01 = 16 bits uuid while 0x02 = 128
 *        bits uuid
 * @param include_uuid see @ref include_uuid_t
 * @param[out] include_handle handle of the include declaration
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_include_service(uint16_t service_handle, uint16_t include_start_handle,
									  uint16_t include_end_handle, uint8_t include_uuid_type,
									  const include_uuid_t *include_uuid, uint16_t *include_handle);

/**
 * @brief aci_gatt_add_char
 * adds a characteristic to a service.
 * the command returns the handle of the declaration attribute. the attribute
 * that holds the characteristic value is always allocated at the next handle
 * (char_handle + 1). the characteristic value is immediately followed, in
 * order, by:
 * - the server characteristic configuration descriptor if char_prop_broadcast
 * is selected;
 * - the client characteristic configuration descriptor if char_prop_notify or
 * char_prop_indicate properties is selected;
 * - the characteristic extended properties descriptor if char_prop_ext is
 * selected.
 * for instance, if char_prop_notify is selected but not char_prop_broadcast
 * nor char_prop_ext, then the client characteristic configuration attribute
 * handle is char_handle + 2.
 * additional descriptors can be added to the characteristic by calling the
 * aci_gatt_add_char_desc command immediately after calling this command.
 *
 * @param service_handle handle of the service to which the characteristic will
 *        be added
 * @param char_uuid_type uuid type: 0x01 = 16 bits uuid while 0x02 = 128 bits
 *        uuid
 * @param char_uuid see @ref char_uuid_t
 * @param char_value_length maximum length of the characteristic value.
 * @param char_properties characteristic properties (core specification [vol 3,
 *        part g, 3.3.1.1])
 *        flags:
 *        - 0x00: char_prop_none
 *        - 0x01: char_prop_broadcast (broadcast)
 *        - 0x02: char_prop_read (read)
 *        - 0x04: char_prop_write_without_resp (write w/o resp)
 *        - 0x08: char_prop_write (write)
 *        - 0x10: char_prop_notify (notify)
 *        - 0x20: char_prop_indicate (indicate)
 *        - 0x40: char_prop_signed_write (authenticated signed writes)
 *        - 0x80: char_prop_ext (extended properties)
 * @param security_permissions security permission flags.
 *        flags:
 *        - 0x00: none
 *        - 0x01: authen_read (need authentication to read)
 *        - 0x02: author_read (need authorization to read)
 *        - 0x04: encry_read (need encryption to read)
 *        - 0x08: authen_write (need authentication to write)
 *        - 0x10: author_write (need authorization to write)
 *        - 0x20: encry_write (need encryption to write)
 *        - 0x40: sc_read (need secure connections to read)
 *        - 0x80: sc_write (need secure connections to write)
 * @param gatt_evt_mask gatt event mask.
 *        flags:
 *        - 0x00: gatt_dont_notify_events
 *        - 0x01: gatt_notify_attribute_write
 *        - 0x02: gatt_notify_write_req_and_wait_for_appl_resp
 *        - 0x04: gatt_notify_read_req_and_wait_for_appl_resp
 *        - 0x08: gatt_notify_notification_completion
 * @param enc_key_size minimum encryption key size required to read the
 *        characteristic.
 *        values:
 *        - 0x07 ... 0x10
 * @param is_variable specify if the characteristic value has a fixed length or
 *        a variable length.
 *        values:
 *        - 0x00: fixed length
 *        - 0x01: variable length
 * @param[out] char_handle handle of the characteristic that has been added (it
 *        is the handle of the characteristic declaration).
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_add_char(uint16_t service_handle, uint8_t char_uuid_type,
							   const char_uuid_t *char_uuid, uint16_t char_value_length,
							   uint8_t char_properties, uint8_t security_permissions,
							   uint8_t gatt_evt_mask, uint8_t enc_key_size, uint8_t is_variable,
							   uint16_t *char_handle);

/**
 * @brief aci_gatt_add_char_desc
 * adds a characteristic descriptor to a service.
 * note that this command allocates the new handle for the descriptor after the
 * currently allocated handles. it is therefore advisable to call this command
 * following the call of the command aci_gatt_add_char which created the
 * characteristic containing this descriptor.
 *
 * @param service_handle handle of service to which the characteristic belongs
 * @param char_handle handle of the characteristic to which description has to
 *        be added
 * @param char_desc_uuid_type uuid type: 0x01 = 16 bits uuid while 0x02 = 128
 *        bits uuid
 * @param char_desc_uuid see @ref char_desc_uuid_t
 * @param char_desc_value_max_len the maximum length of the descriptor value
 * @param char_desc_value_length current length of the characteristic
 *        descriptor value
 * @param char_desc_value value of the characteristic description
 * @param security_permissions security permission flags.
 *        flags:
 *        - 0x00: none
 *        - 0x01: authen_read (need authentication to read)
 *        - 0x02: author_read (need authorization to read)
 *        - 0x04: encry_read (need encryption to read)
 *        - 0x08: authen_write (need authentication to write)
 *        - 0x10: author_write (need authorization to write)
 *        - 0x20: encry_write (need encryption to write)
 *        - 0x40: sc_read (need secure connections to read)
 *        - 0x80: sc_write (need secure connections to write)
 * @param access_permissions access permission
 *        flags:
 *        - 0x00: none
 *        - 0x01: read
 *        - 0x02: write
 *        - 0x04: write_wo_resp
 *        - 0x08: signed_write
 * @param gatt_evt_mask gatt event mask.
 *        flags:
 *        - 0x00: gatt_dont_notify_events
 *        - 0x01: gatt_notify_attribute_write
 *        - 0x02: gatt_notify_write_req_and_wait_for_appl_resp
 *        - 0x04: gatt_notify_read_req_and_wait_for_appl_resp
 * @param enc_key_size minimum encryption key size required to read the
 *        characteristic.
 *        values:
 *        - 0x07 ... 0x10
 * @param is_variable specify if the characteristic value has a fixed length or
 *        a variable length.
 *        values:
 *        - 0x00: fixed length
 *        - 0x01: variable length
 * @param[out] char_desc_handle handle of the characteristic descriptor
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_add_char_desc(uint16_t service_handle, uint16_t char_handle,
									uint8_t char_desc_uuid_type,
									const char_desc_uuid_t *char_desc_uuid,
									uint8_t char_desc_value_max_len, uint8_t char_desc_value_length,
									const uint8_t *char_desc_value, uint8_t security_permissions,
									uint8_t access_permissions, uint8_t gatt_evt_mask,
									uint8_t enc_key_size, uint8_t is_variable,
									uint16_t *char_desc_handle);

/**
 * @brief aci_gatt_update_char_value
 * updates a characteristic value in a service. if notifications (or
 * indications) are enabled on that characteristic, a notification (or
 * indication) is sent to any client that has registered for notifications (or
 * indications) via the client characteristic configuration.
 * notes:
 * - the command is disallowed if it would cause the generation of an
 * indication on a bearer which is still awaiting confirmation of a previous
 * indication.
 * - the command does not execute and returns ble_status_busy if notifications
 * from a previous call are not completed. the application can enable and wait
 * for the event aci_gatt_notification_complete_event to avoid this case.
 * - the command does not execute and returns ble_status_insufficient_resources
 * if there is no more room in the tx pool to allocate notification (or
 * indication) packets. this happens if notifications (or indications) are
 * enabled and the application calls this command at an higher rate than what
 * is allowed by the link. throughput on ble link depends on connection
 * interval and connection length parameters (decided by the central, see
 * aci_l2cap_connection_parameter_update_req for more information on how to
 * suggest new connection parameters from a peripheral). the application can
 * wait for the event aci_gatt_tx_pool_available_event before retrying a call
 * to this command. it can also retry the call until it does not return
 * ble_status_insufficient_resources anymore.
 * - when calling this command, the characteristic value is updated only if the
 * command returns ble_status_success or ble_status_sec_permission_error. the
 * security permission error means that at least one client has not been
 * notified due to security requirements not met.
 *
 * @param service_handle handle of service to which the characteristic belongs
 * @param char_handle handle of the characteristic declaration
 * @param val_offset the offset from which the attribute value has to be
 *        updated.
 *        if this is set to 0 and the attribute value is of variable length,
 *        then the length of the attribute will be set to the
 *        char_value_length.
 *        if the val_offset is set to a value greater than 0, then the length
 *        of the attribute will be set to the maximum length as specified for
 *        the attribute while adding the characteristic.
 * @param char_value_length length of the char_value parameter in octets.
 *        this value must not exceed (ble_cmd_max_param_len - 6) i.e. 249 for
 *        ble_cmd_max_param_len default value.
 * @param char_value characteristic value
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_update_char_value(uint16_t service_handle, uint16_t char_handle,
										uint8_t val_offset, uint8_t char_value_length,
										const uint8_t *char_value);

/**
 * @brief aci_gatt_del_char
 * deletes the specified characteristic from the service.
 *
 * @param serv_handle handle of service to which the characteristic belongs
 * @param char_handle handle of the characteristic which has to be deleted
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_del_char(uint16_t serv_handle, uint16_t char_handle);

/**
 * @brief aci_gatt_del_service
 * deletes the specified service from the gatt server database.
 *
 * @param serv_handle handle of the service to be deleted
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_del_service(uint16_t serv_handle);

/**
 * @brief aci_gatt_del_include_service
 * deletes the include definition from the service.
 *
 * @param serv_handle handle of the service to which the include service
 *        belongs
 * @param include_handle handle of the included service which has to be deleted
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_del_include_service(uint16_t serv_handle, uint16_t include_handle);

/**
 * @brief aci_gatt_set_event_mask
 * masks events from the gatt. if the bit in the gatt_evt_mask is set to a one,
 * then the event associated with that bit will be enabled.
 *
 * @param gatt_evt_mask aci gatt/att event mask. default: 0x000fffff.
 *        values:
 *        - 0x00000001: aci_gatt_attribute_modified_event
 *        - 0x00000002: aci_gatt_proc_timeout_event
 *        - 0x00000004: aci_att_exchange_mtu_resp_event
 *        - 0x00000008: aci_att_find_info_resp_event
 *        - 0x00000010: aci_att_find_by_type_value_resp_event
 *        - 0x00000020: aci_att_read_by_type_resp_event
 *        - 0x00000040: aci_att_read_resp_event
 *        - 0x00000080: aci_att_read_blob_resp_event
 *        - 0x00000100: aci_att_read_multiple_resp_event
 *        - 0x00000200: aci_att_read_by_group_type_resp_event
 *        - 0x00000800: aci_att_prepare_write_resp_event
 *        - 0x00001000: aci_att_exec_write_resp_event
 *        - 0x00002000: aci_gatt_indication_event
 *        - 0x00004000: aci_gatt_notification_event
 *        - 0x00008000: aci_gatt_error_resp_event
 *        - 0x00010000: aci_gatt_proc_complete_event
 *        - 0x00020000: aci_gatt_disc_read_char_by_uuid_resp_event
 *        - 0x00040000: aci_gatt_tx_pool_available_event
 *        - 0x00100000: aci_gatt_read_ext_event
 *        - 0x00200000: aci_gatt_indication_ext_event
 *        - 0x00400000: aci_gatt_notification_ext_event
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_set_event_mask(uint32_t gatt_evt_mask);

/**
 * @brief aci_gatt_exchange_config
 * performs an att mtu exchange procedure.
 * when the att mtu exchange procedure is completed, a
 * aci_att_exchange_mtu_resp_event event is generated. a
 * aci_gatt_proc_complete_event event is also generated to indicate the end of
 * the procedure.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_exchange_config(uint16_t connection_handle);

/**
 * @brief aci_att_find_info_req
 * sends a find information request.
 * this command is used to obtain the mapping of attribute handles with their
 * associated types. the responses of the procedure are given through the
 * aci_att_find_info_resp_event event. the end of the procedure is indicated by
 * a aci_gatt_proc_complete_event event.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param start_handle first requested handle number
 * @param end_handle last requested handle number
 * @return value indicating success or error code.
 */
ble_status_t aci_att_find_info_req(uint16_t connection_handle, uint16_t start_handle,
								   uint16_t end_handle);

/**
 * @brief aci_att_find_by_type_value_req
 * sends a find by type value request
 * the find by type value request is used to obtain the handles of attributes
 * that have a given 16-bit uuid attribute type and a given attribute value.
 * the responses of the procedure are given through the
 * aci_att_find_by_type_value_resp_event event.
 * the end of the procedure is indicated by a aci_gatt_proc_complete_event
 * event.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param start_handle first requested handle number
 * @param end_handle last requested handle number
 * @param uuid 2 octet uuid to find (little-endian)
 * @param attribute_val_length length of attribute value (maximum value is
 *        att_mtu - 7).
 * @param attribute_val attribute value to find
 * @return value indicating success or error code.
 */
ble_status_t aci_att_find_by_type_value_req(uint16_t connection_handle, uint16_t start_handle,
											uint16_t end_handle, uint16_t uuid,
											uint8_t attribute_val_length,
											const uint8_t *attribute_val);

/**
 * @brief aci_att_read_by_type_req
 * sends a read by type request.
 * the read by type request is used to obtain the values of attributes where
 * the attribute type is known but the handle is not known.
 * the responses are given through the aci_att_read_by_type_resp_event event.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param start_handle first requested handle number
 * @param end_handle last requested handle number
 * @param uuid_type uuid type: 0x01 = 16 bits uuid while 0x02 = 128 bits uuid
 * @param uuid see @ref uuid_t
 * @return value indicating success or error code.
 */
ble_status_t aci_att_read_by_type_req(uint16_t connection_handle, uint16_t start_handle,
									  uint16_t end_handle, uint8_t uuid_type, const uuid_t *uuid);

/**
 * @brief aci_att_read_by_group_type_req
 * sends a read by group type request.
 * the read by group type request is used to obtain the values of grouping
 * attributes where the attribute type is known but the handle is not known.
 * grouping attributes are defined at gatt layer. the grouping attribute types
 * are: "primary service", "secondary service" and "characteristic".
 * the responses of the procedure are given through the
 * aci_att_read_by_group_type_resp_event event.
 * the end of the procedure is indicated by a aci_gatt_proc_complete_event.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param start_handle first requested handle number
 * @param end_handle last requested handle number
 * @param uuid_type uuid type: 0x01 = 16 bits uuid while 0x02 = 128 bits uuid
 * @param uuid see @ref uuid_t
 * @return value indicating success or error code.
 */
ble_status_t aci_att_read_by_group_type_req(uint16_t connection_handle, uint16_t start_handle,
											uint16_t end_handle, uint8_t uuid_type,
											const uuid_t *uuid);

/**
 * @brief aci_att_prepare_write_req
 * sends a prepare write request.
 * the prepare write request is used to request the server to prepare to write
 * the value of an attribute.
 * the responses of the procedure are given through the
 * aci_att_prepare_write_resp_event event.
 * the end of the procedure is indicated by a aci_gatt_proc_complete_event.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param attr_handle handle of the attribute to be written
 * @param val_offset the offset of the first octet to be written
 * @param attribute_val_length length of attribute value (maximum value is
 *        att_mtu - 5).
 * @param attribute_val the value of the attribute to be written
 * @return value indicating success or error code.
 */
ble_status_t aci_att_prepare_write_req(uint16_t connection_handle, uint16_t attr_handle,
									   uint16_t val_offset, uint8_t attribute_val_length,
									   const uint8_t *attribute_val);

/**
 * @brief aci_att_execute_write_req
 * sends an execute write request.
 * the execute write request is used to request the server to write or cancel
 * the write of all the prepared values currently held in the prepare queue
 * from this client.
 * the result of the procedure is given through the
 * aci_att_exec_write_resp_event event.
 * the end of the procedure is indicated by a aci_gatt_proc_complete_event
 * event.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param execute execute or cancel writes.
 *        values:
 *        - 0x00: cancel all prepared writes
 *        - 0x01: immediately write all pending prepared values
 * @return value indicating success or error code.
 */
ble_status_t aci_att_execute_write_req(uint16_t connection_handle, uint8_t execute);

/**
 * @brief aci_gatt_disc_all_primary_services
 * starts the gatt client procedure to discover all primary services on the
 * server.
 * the responses of the procedure are given through the
 * aci_att_read_by_group_type_resp_event event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_disc_all_primary_services(uint16_t connection_handle);

/**
 * @brief aci_gatt_disc_primary_service_by_uuid
 * starts the procedure to discover the primary services of the specified uuid
 * on the server.
 * the responses of the procedure are given through the
 * aci_att_find_by_type_value_resp_event event.
 * the end of the procedure is indicated by a aci_gatt_proc_complete_event
 * event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param uuid_type uuid type: 0x01 = 16 bits uuid while 0x02 = 128 bits uuid
 * @param uuid see @ref uuid_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_disc_primary_service_by_uuid(uint16_t connection_handle, uint8_t uuid_type,
												   const uuid_t *uuid);

/**
 * @brief aci_gatt_find_included_services
 * starts the procedure to find all included services.
 * the responses of the procedure are given through the
 * aci_att_read_by_type_resp_event event.
 * the end of the procedure is indicated by a aci_gatt_proc_complete_event
 * event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param start_handle start attribute handle of the service
 * @param end_handle end attribute handle of the service
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_find_included_services(uint16_t connection_handle, uint16_t start_handle,
											 uint16_t end_handle);

/**
 * @brief aci_gatt_disc_all_char_of_service
 * starts the procedure to discover all the characteristics of a given service.
 * when the procedure is completed, a aci_gatt_proc_complete_event event is
 * generated. before procedure completion the response packets are given
 * through aci_att_read_by_type_resp_event event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param start_handle start attribute handle of the service
 * @param end_handle end attribute handle of the service
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_disc_all_char_of_service(uint16_t connection_handle, uint16_t start_handle,
											   uint16_t end_handle);

/**
 * @brief aci_gatt_disc_char_by_uuid
 * starts the procedure to discover all the characteristics specified by a
 * uuid.
 * when the procedure is completed, a aci_gatt_proc_complete_event event is
 * generated. before procedure completion the response packets are given
 * through aci_gatt_disc_read_char_by_uuid_resp_event event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param start_handle start attribute handle of the service
 * @param end_handle end attribute handle of the service
 * @param uuid_type uuid type: 0x01 = 16 bits uuid while 0x02 = 128 bits uuid
 * @param uuid see @ref uuid_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_disc_char_by_uuid(uint16_t connection_handle, uint16_t start_handle,
										uint16_t end_handle, uint8_t uuid_type, const uuid_t *uuid);

/**
 * @brief aci_gatt_disc_all_char_desc
 * starts the procedure to discover all characteristic descriptors within a
 * characteristic definition.
 * when the procedure is completed, a aci_gatt_proc_complete_event event is
 * generated. before procedure completion the response packets are given
 * through aci_att_find_info_resp_event event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param char_handle handle of the characteristic value
 * @param end_handle end handle of the characteristic
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_disc_all_char_desc(uint16_t connection_handle, uint16_t char_handle,
										 uint16_t end_handle);

/**
 * @brief aci_gatt_read_char_value
 * starts the procedure to read the attribute value.
 * when the procedure is completed, a aci_gatt_proc_complete_event event is
 * generated. before procedure completion the response packet is given through
 * aci_att_read_resp_event event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param attr_handle handle of the characteristic value (or descriptor) to be
 *        read
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_read_char_value(uint16_t connection_handle, uint16_t attr_handle);

/**
 * @brief aci_gatt_read_using_char_uuid
 * this command sends a read by type request packet to the server in order to
 * read the value attribute of the characteristics specified by the uuid.
 * when the procedure is completed, an aci_gatt_proc_complete_event event is
 * generated. before procedure completion, the response packet is given through
 * one aci_gatt_disc_read_char_by_uuid_resp_event event per reported attribute.
 * note: the number of bytes of a value reported by
 * aci_gatt_disc_read_char_by_uuid_resp_event event cannot exceed
 * ble_evt_max_param_len - 7 i.e. 248 bytes for default value of
 * ble_evt_max_param_len.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param start_handle starting handle of the range to be searched
 * @param end_handle end handle of the range to be searched
 * @param uuid_type uuid type: 0x01 = 16 bits uuid while 0x02 = 128 bits uuid
 * @param uuid see @ref uuid_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_read_using_char_uuid(uint16_t connection_handle, uint16_t start_handle,
										   uint16_t end_handle, uint8_t uuid_type,
										   const uuid_t *uuid);

/**
 * @brief aci_gatt_read_long_char_value
 * starts the procedure to read a long characteristic value (or descriptor).
 * when the procedure is completed, a aci_gatt_proc_complete_event event is
 * generated. before procedure completion the response packets are given
 * through aci_att_read_blob_resp_event event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param attr_handle handle of the characteristic value (or descriptor) to be
 *        read
 * @param val_offset offset from which the value needs to be read
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_read_long_char_value(uint16_t connection_handle, uint16_t attr_handle,
										   uint16_t val_offset);

/**
 * @brief aci_gatt_read_multiple_char_value
 * starts a procedure to read multiple characteristic values from a server.
 * the command must specify the handles of the characteristic values to be
 * read.
 * when the procedure is completed, a aci_gatt_proc_complete_event event is
 * generated. before procedure completion the response packets are given
 * through aci_att_read_multiple_resp_event event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param number_of_handles number of handles in the following table
 *        values:
 *        - 0x02 ... 0x7e
 * @param handle_entry see @ref handle_entry_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_read_multiple_char_value(uint16_t connection_handle,
											   uint8_t number_of_handles,
											   const handle_entry_t *handle_entry);

/**
 * @brief aci_gatt_write_char_value
 * starts the procedure to write a characteristic value (or a characteristic
 * descriptor).
 * when the procedure is completed, a aci_gatt_proc_complete_event event is
 * generated.
 * the length of the value to be written must not exceed (att_mtu - 3).
 * it must also not exceed (ble_cmd_max_param_len - 5) i.e. 250 for
 * ble_cmd_max_param_len default value.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param attr_handle handle of the characteristic value to be written
 * @param attribute_val_length length of the value to be written
 * @param attribute_val value to be written
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_write_char_value(uint16_t connection_handle, uint16_t attr_handle,
									   uint8_t attribute_val_length, const uint8_t *attribute_val);

/**
 * @brief aci_gatt_write_long_char_value
 * starts the procedure to write a long characteristic value (or a long
 * characteristic descriptor).
 * when the procedure is completed, a aci_gatt_proc_complete_event event is
 * generated. during the procedure, aci_att_prepare_write_resp_event and
 * aci_att_exec_write_resp_event events are raised.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param attr_handle handle of the characteristic value to be written
 * @param val_offset offset at which the attribute has to be written
 * @param attribute_val_length length of the value to be written.
 *        this value must not exceed (ble_cmd_max_param_len - 7) i.e. 248 for
 *        ble_cmd_max_param_len default value.
 * @param attribute_val value to be written
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_write_long_char_value(uint16_t connection_handle, uint16_t attr_handle,
											uint16_t val_offset, uint8_t attribute_val_length,
											const uint8_t *attribute_val);

/**
 * @brief aci_gatt_write_char_reliable
 * starts the procedure to write a characteristic reliably.
 * when the procedure is completed, a  aci_gatt_proc_complete_event event is
 * generated. during the procedure, aci_att_prepare_write_resp_event and
 * aci_att_exec_write_resp_event events are raised.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param attr_handle handle of the attribute to be written
 * @param val_offset offset at which the attribute has to be written
 * @param attribute_val_length length of the value to be written.
 *        this value must not exceed (ble_cmd_max_param_len - 7) i.e. 248 for
 *        ble_cmd_max_param_len default value.
 * @param attribute_val value to be written
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_write_char_reliable(uint16_t connection_handle, uint16_t attr_handle,
										  uint16_t val_offset, uint8_t attribute_val_length,
										  const uint8_t *attribute_val);

/**
 * @brief aci_gatt_write_without_resp
 * starts the procedure to write a characteristic value without waiting for any
 * response from the server. no events are generated after this command is
 * executed.
 * the length of the value to be written must not exceed (att_mtu - 3).
 * it must also not exceed (ble_cmd_max_param_len - 5) i.e. 250 for
 * ble_cmd_max_param_len default value.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param attr_handle handle of the characteristic value to be written
 * @param attribute_val_length length of the value to be written
 * @param attribute_val value to be written
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_write_without_resp(uint16_t connection_handle, uint16_t attr_handle,
										 uint8_t attribute_val_length,
										 const uint8_t *attribute_val);

/**
 * @brief aci_gatt_signed_write_without_resp
 * starts a signed write without response from the server.
 * the procedure is used to write a characteristic value with an authentication
 * signature without waiting for any response from the server. it cannot be
 * used when the link is encrypted.
 * the length of the value to be written must not exceed (att_mtu - 15).
 * it must also not exceed (ble_cmd_max_param_len - 5) i.e. 250 for
 * ble_cmd_max_param_len default value.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param attr_handle handle of the characteristic value to be written
 * @param attribute_val_length length of the value to be written
 * @param attribute_val value to be written
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_signed_write_without_resp(uint16_t connection_handle, uint16_t attr_handle,
												uint8_t attribute_val_length,
												const uint8_t *attribute_val);

/**
 * @brief aci_gatt_confirm_indication
 * allow application to confirm indication. this command has to be sent when
 * the application receives the event aci_gatt_indication_event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_confirm_indication(uint16_t connection_handle);

/**
 * @brief aci_gatt_permit_write
 * allow or reject a write request from a client.
 * this command has to be sent by the application when it receives the
 * aci_gatt_write_permit_req_event. if the write can be allowed, then the
 * status and error code have to be set to 0. if the write cannot be allowed,
 * then the status has to be set to 1 and the error code has to be set to the
 * error code that has to be passed to the client.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param attr_handle handle of the attribute that was passed in the event
 *        aci_gatt_write_permit_req_event
 * @param write_status if the value can be written or not.
 *        values:
 *        - 0x00: the value can be written to the attribute specified by
 *          attr_handle
 *        - 0x01: the value cannot be written to the attribute specified by the
 *          attr_handle
 * @param error_code the error code that has to be passed to the client in case
 *        the access is denied.
 *        values:
 *        - 0x08: insufficient authorization
 *        - 0x80 ... 0x9f: application error
 * @param attribute_val_length length of the value to be written as passed in
 *        the event aci_gatt_write_permit_req_event
 * @param attribute_val value as passed in the event
 *        aci_gatt_write_permit_req_event
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_permit_write(uint16_t connection_handle, uint16_t attr_handle,
								   uint8_t write_status, uint8_t error_code,
								   uint8_t attribute_val_length, const uint8_t *attribute_val);

/**
 * @brief aci_gatt_permit_read
 * this command is used to allow or deny the gatt server to send a response to
 * a read request from a client. the application must end this command when it
 * receives the permission request event, either aci_gatt_read_permit_req_event
 * or aci_gatt_read_multi_permit_req_event. the application should issue this
 * command within 30 seconds from the reception of the permission request
 * event; otherwise the gatt procedure issues a timeout.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param read_status specifies whether read access is allowed for all
 *        attributes from the permission request event (0), or denied for some
 *        or all attributes (1).
 *        values:
 *        - 0x00: read access is allowed
 *        - 0x01: read access is denied
 * @param error_code the error code that has to be passed to the client in case
 *        the access is denied.
 *        values:
 *        - 0x08: insufficient authorization
 *        - 0x80 ... 0x9f: application error
 * @param attr_handle this parameter is only valid if one or more read
 *        permissions are denied. it is the handle of the first attribute in
 *        the permission request event for which read permission is denied.
 *        when set to 0, reading all attributes from the event is denied.
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_permit_read(uint16_t connection_handle, uint8_t read_status,
								  uint8_t error_code, uint16_t attr_handle);

/**
 * @brief aci_gatt_set_security_permission
 * this command sets the security permission flags for the attribute handle
 * specified. it can be used to change the default value of these flags after
 * an attribute has been created.
 *
 * @param serv_handle handle of the service which contains the attribute whose
 *        permission has to be modified
 * @param attr_handle handle of the attribute whose permission has to be
 *        modified
 * @param security_permissions security permission flags.
 *        flags:
 *        - 0x00: none
 *        - 0x01: authen_read (need authentication to read)
 *        - 0x02: author_read (need authorization to read)
 *        - 0x04: encry_read (need encryption to read)
 *        - 0x08: authen_write (need authentication to write)
 *        - 0x10: author_write (need authorization to write)
 *        - 0x20: encry_write (need encryption to write)
 *        - 0x40: sc_read (need secure connections to read)
 *        - 0x80: sc_write (need secure connections to write)
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_set_security_permission(uint16_t serv_handle, uint16_t attr_handle,
											  uint8_t security_permissions);

/**
 * @brief aci_gatt_set_desc_value
 * this command sets the value of the descriptor specified by char_desc_handle.
 *
 * @param serv_handle handle of the service which contains the characteristic
 *        descriptor
 * @param char_handle handle of the characteristic which contains the
 *        descriptor
 * @param char_desc_handle handle of the descriptor whose value has to be set
 * @param val_offset offset from which the descriptor value has to be updated
 * @param char_desc_value_length length of the descriptor value
 * @param char_desc_value descriptor value
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_set_desc_value(uint16_t serv_handle, uint16_t char_handle,
									 uint16_t char_desc_handle, uint16_t val_offset,
									 uint8_t char_desc_value_length,
									 const uint8_t *char_desc_value);

/**
 * @brief aci_gatt_read_handle_value
 * reads the value of the attribute handle specified from the local gatt
 * database.
 *
 * @param attr_handle handle of the attribute to read
 * @param offset offset from which the value needs to be read
 * @param value_length_requested maximum number of octets to be returned as
 *        attribute value
 * @param[out] length length of the attribute value
 * @param[out] value_length length in octets of the value parameter
 * @param[out] value attribute value
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_read_handle_value(uint16_t attr_handle, uint16_t offset,
										uint16_t value_length_requested, uint16_t *length,
										uint16_t *value_length, uint8_t *value);

/**
 * @brief aci_gatt_update_char_value_ext
 * this command is a more flexible version of aci_gatt_update_char_value to
 * support update of long attribute up to 512 bytes and indicate selectively
 * the generation of indication/notification.
 * the description notes for the aci_gatt_update_char_value command also apply
 * here.
 *
 * @param conn_handle_to_notify specifies the client(s) to be notified.
 *        values:
 *        - 0x0000: notify all subscribed clients on their unenhanced att
 *          bearer
 *        - 0x0001 ... 0x0eff: notify one client on the specified unenhanced
 *          att bearer (the parameter is the connection handle)
 *        - 0xea00 ... 0xea3f: notify one client on the specified enhanced att
 *          bearer (the lsb-byte of the parameter is the connection-oriented
 *          channel index)
 * @param service_handle handle of service to which the characteristic belongs
 * @param char_handle handle of the characteristic declaration
 * @param update_type allow notification or indication generation, if enabled
 *        in the client characteristic configuration descriptor
 *        flags:
 *        - 0x00: do not notify
 *        - 0x01: notification
 *        - 0x02: indication
 * @param char_length total length of the characteristic value.
 *        in case of a variable size characteristic, this field specifies the
 *        new length of the characteristic value after the update; in case of
 *        fixed length characteristic this field is ignored.
 * @param value_offset the offset from which the attribute value has to be
 *        updated.
 * @param value_length length of the value parameter in octets.
 *        this value must not exceed (ble_cmd_max_param_len - 12) i.e. 243 for
 *        ble_cmd_max_param_len default value.
 * @param value updated characteristic value
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_update_char_value_ext(uint16_t conn_handle_to_notify, uint16_t service_handle,
											uint16_t char_handle, uint8_t update_type,
											uint16_t char_length, uint16_t value_offset,
											uint8_t value_length, const uint8_t *value);

/**
 * @brief aci_gatt_set_access_permission
 * this command sets the access permission for the attribute handle specified.
 *
 * @param serv_handle handle of the service which contains the attribute whose
 *        permission has to be modified
 * @param attr_handle handle of the attribute whose permission has to be
 *        modified
 * @param access_permissions access permission
 *        flags:
 *        - 0x00: none
 *        - 0x01: read
 *        - 0x02: write
 *        - 0x04: write_wo_resp
 *        - 0x08: signed_write
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_set_access_permission(uint16_t serv_handle, uint16_t attr_handle,
											uint8_t access_permissions);

/**
 * @brief aci_gatt_store_db
 * this command forces the saving of the gatt database for all active
 * connections. note that, by default, the gatt database is saved per active
 * connection at the time of disconnection.
 *
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_store_db(void);

/**
 * @brief aci_gatt_send_mult_notification
 * this command sends a multiple handle value notification over the att bearer
 * specified in parameter. the handles provided as parameters must be the
 * handles of the characteristic declarations.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param number_of_handles number of handles in the following table
 *        values:
 *        - 0x02 ... 0x7e
 * @param handle_entry see @ref handle_entry_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_send_mult_notification(uint16_t connection_handle, uint8_t number_of_handles,
											 const handle_entry_t *handle_entry);

/**
 * @brief aci_gatt_read_multiple_var_char_value
 * starts a procedure to read multiple variable length characteristic values
 * from a server.
 * the command must specify the handles of the characteristic values to be
 * read.
 * when the procedure is completed, a aci_gatt_proc_complete_event event is
 * generated. before procedure completion the response packets are given
 * through aci_att_read_multiple_resp_event event.
 *
 * @param connection_handle specifies the att bearer for which the command
 *        applies.
 *        values:
 *        - 0x0000 ... 0x0eff: unenhanced att bearer (the parameter is the
 *          connection handle)
 *        - 0xea00 ... 0xea3f: enhanced att bearer (the lsb-byte of the
 *          parameter is the connection-oriented channel index)
 * @param number_of_handles number of handles in the following table
 *        values:
 *        - 0x02 ... 0x7e
 * @param handle_entry see @ref handle_entry_t
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_read_multiple_var_char_value(uint16_t connection_handle,
												   uint8_t number_of_handles,
												   const handle_entry_t *handle_entry);

/**
 * @brief aci_gatt_write_without_resp_ext
 * this command is used on the client side to write an attribute value to the
 * server. it can replace aci_gatt_write_without_resp or
 * aci_gatt_signed_write_without_resp when the length of the data to be written
 * exceeds the capacity supported by these commands. in this case, the data to
 * be written is not part of the command, but is stored in the "extra data"
 * buffer, which must be pre-filled by the application before calling this
 * command. the used portion of the "extra data" buffer can only be reused
 * after receiving hci_command_complete_event.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param attr_handle handle of the characteristic value to be written
 * @param signed_mode specifies the type of gatt write without response: signed
 *        or not.
 *        values:
 *        - 0x00: write without response
 *        - 0x01: signed write without response
 * @param data_length length of the data to be written.
 * @param data_pointer pointer to the data to be written (it is the offset in
 *        bytes from the start of the "extra data" buffer).
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_write_without_resp_ext(uint16_t connection_handle, uint16_t attr_handle,
											 uint8_t signed_mode, uint16_t data_length,
											 uint32_t data_pointer);

/**
 * @brief aci_gatt_write_with_resp_ext
 * this command is used on the client side to write an attribute value to the
 * server. it can replace aci_gatt_write_char_value,
 * aci_gatt_write_long_char_value or aci_gatt_write_char_reliable when the
 * length of the data to be written exceeds the capacity supported by these
 * commands. in this case, the data to be written is not part of the command,
 * but is stored in the "extra data" buffer, which must be pre-filled by the
 * application before calling this command. the used portion of the "extra
 * data" buffer can only be reused after receiving
 * aci_gatt_proc_complete_event.
 *
 * @param connection_handle connection handle for which the command applies.
 *        values:
 *        - 0x0000 ... 0x0eff
 * @param attr_handle handle of the characteristic value to be written
 * @param write_mode specifies the type of gatt write with response procedure.
 *        values:
 *        - 0x00: write characteristic value/descriptor
 *        - 0x01: write long characteristic value/descriptor
 *        - 0x02: reliable write characteristic value
 * @param val_offset offset at which the attribute has to be written
 * @param data_length length of the data to be written.
 * @param data_pointer pointer to the data to be written (it is the offset in
 *        bytes from the start of the "extra data" buffer).
 * @return value indicating success or error code.
 */
ble_status_t aci_gatt_write_with_resp_ext(uint16_t connection_handle, uint16_t attr_handle,
										  uint8_t write_mode, uint16_t val_offset,
										  uint16_t data_length, uint32_t data_pointer);

#endif /* BLE_GATT_ACI_H__ */
