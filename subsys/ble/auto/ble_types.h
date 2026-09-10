/*****************************************************************************
 * @file    ble_types.h
 * @brief   STM32WB BLE command/event types
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

#ifndef BLE_TYPES_H__
#define BLE_TYPES_H__

#include <stdint.h>
#include "cmsis_gcc.h"

#define BLE_CMD_MAX_PARAM_LEN 255
#define BLE_EVT_MAX_PARAM_LEN 255

/* Type used for function return value */
typedef uint8_t ble_status_t;

/* Definition of Host_Nb_Of_Completed_Pkt_Pair_t */
typedef __PACKED_STRUCT
{
	/**
   * Connection_Handle[i].
   * Values:
   * - 0x0000 ... 0x0EFF
   */
	uint16_t connection_handle;
	/**
   * The number of HCI Data Packets [i] that have been completed for the
   * associated Connection_Handle since the previous time the event was
   * returned.
   * Values:
   * - 0x0000 ... 0xFFFF
   */
	uint16_t host_num_of_completed_packets;
}
host_nb_of_completed_pkt_pair_t;

/* Definition of Adv_Set_t */
typedef __PACKED_STRUCT
{
	/**
   * Used to identify an advertising set.
   * Values:
   * - 0x00 ... 0xEF
   */
	uint8_t advertising_handle;
	/**
   * Duration of advertising set.
   * Time = N * 10 ms.
   * Values:
   * - 0x0000 (0 ms) : No advertising duration.
   * - 0x0001 (10 ms)  ... 0xFFFF (655350 ms) : Advertising duration
   */
	uint16_t duration;
	/**
   * Maximum number of advertising events.
   * Values:
   * - 0x00: No maximum number of advertising events
   * - 0x01 ... 0xFF: Maximum number of extended advertising events the
   *   Controller shall attempt to send prior to terminating the extended
   *   advertising
   */
	uint8_t max_extended_advertising_events;
}
adv_set_t;

/* Definition of Scan_Param_Phy_t */
typedef __PACKED_STRUCT
{
	/**
   * Passive or active scanning. With passive scanning, no scan request PDUs
   * are sent.
   * Values:
   * - 0x00: Passive scanning
   * - 0x01: Active scanning
   */
	uint8_t scan_type;
	/**
   * Time interval from when the Controller started its last scan until it
   * begins the subsequent scan on the primary advertising physical channel.
   * Time = N * 0.625 ms.
   * Values:
   * - 0x0004 (2.500 ms)  ... 0x5DC0 (15000.000 ms) : only supported range
   */
	uint16_t scan_interval;
	/**
   * Duration of the scan on the primary advertising physical channel.
   * Time = N * 0.625 ms.
   * Values:
   * - 0x0004 (2.500 ms)  ... 0x5DC0 (15000.000 ms) : only supported range
   */
	uint16_t scan_window;
}
scan_param_phy_t;

/* Definition of Init_Param_Phy_t */
typedef __PACKED_STRUCT
{
	/**
   * Time interval from when the Controller started its last scan until it
   * begins the subsequent scan on the primary advertising physical channel.
   * Time = N * 0.625 ms.
   * Values:
   * - 0x0004 (2.500 ms)  ... 0x5DC0 (15000.000 ms) : only supported range
   */
	uint16_t scan_interval;
	/**
   * Duration of the scan on the primary advertising physical channel.
   * Time = N * 0.625 ms.
   * Values:
   * - 0x0004 (2.500 ms)  ... 0x5DC0 (15000.000 ms) : only supported range
   */
	uint16_t scan_window;
	/**
   * Minimum value for the connection event interval.
   * Time = N * 1.25 ms.
   * Values:
   * - 0x0006 (7.50 ms)  ... 0x0C80 (4000.00 ms)
   */
	uint16_t conn_interval_min;
	/**
   * Maximum value for the connection event interval.
   * Time = N * 1.25 ms.
   * Values:
   * - 0x0006 (7.50 ms)  ... 0x0C80 (4000.00 ms)
   */
	uint16_t conn_interval_max;
	/**
   * Maximum Peripheral latency for the connection in number of connection
   * events.
   * Values:
   * - 0x0000 ... 0x01F3
   */
	uint16_t conn_latency;
	/**
   * Supervision timeout for the LE Link.
   * It shall be a multiple of 10 ms and larger than (1 +
   * connPeripheralLatency) * connInterval * 2.
   * Time = N * 10 ms.
   * Values:
   * - 0x000A (100 ms)  ... 0x0C80 (32000 ms)
   */
	uint16_t supervision_timeout;
	/**
   * Information parameter about the minimum length of connection needed for
   * this LE connection.
   * Time = N * 0.625 ms.
   * Values:
   * - 0x0000 (0.000 ms)  ... 0xFFFF (40959.375 ms)
   */
	uint16_t min_ce_length;
	/**
   * Information parameter about the maximum length of connection needed for
   * this LE connection.
   * Time = N * 0.625 ms.
   * Values:
   * - 0x0000 (0.000 ms)  ... 0xFFFF (40959.375 ms)
   */
	uint16_t max_ce_length;
}
init_param_phy_t;

/* Definition of Peer_Entry_t */
typedef __PACKED_STRUCT
{
	/**
   * Address type.
   * Values:
   * - 0x00: Public Device Address
   * - 0x01: Random Device Address
   */
	uint8_t peer_address_type;
	/**
   * Public Device Address or Random Device Address.
   */
	uint8_t peer_address[6];
}
peer_entry_t;

/* Definition of Bonded_Device_Entry_t */
typedef __PACKED_STRUCT
{
	/**
   * Address type.
   * Values:
   * - 0x00: Public Device Address
   * - 0x01: Random Device Address
   */
	uint8_t address_type;
	/**
   * Public Device Address or Random Device Address.
   */
	uint8_t address[6];
}
bonded_device_entry_t;

/* Definition of List_Entry_t */
typedef __PACKED_STRUCT
{
	/**
   * Address type.
   * Values:
   * - 0x00: Public Device Address
   * - 0x01: Random Device Address
   */
	uint8_t address_type;
	/**
   * Public Device Address or Random Device Address.
   */
	uint8_t address[6];
}
list_entry_t;

/* Definition of Service_UUID_t */
typedef __PACKED_UNION
{
	/**
   * 16-bit UUID
   */
	uint16_t service_uuid_16;
	/**
   * 128-bit UUID
   */
	uint8_t service_uuid_128[16];
}
service_uuid_t;

/* Definition of Include_UUID_t */
typedef __PACKED_UNION
{
	/**
   * 16-bit UUID
   */
	uint16_t include_uuid_16;
	/**
   * 128-bit UUID
   */
	uint8_t include_uuid_128[16];
}
include_uuid_t;

/* Definition of Char_UUID_t */
typedef __PACKED_UNION
{
	/**
   * 16-bit UUID
   */
	uint16_t char_uuid_16;
	/**
   * 128-bit UUID
   */
	uint8_t char_uuid_128[16];
}
char_uuid_t;

/* Definition of Char_Desc_Uuid_t */
typedef __PACKED_UNION
{
	/**
   * 16-bit UUID
   */
	uint16_t char_uuid_16;
	/**
   * 128-bit UUID
   */
	uint8_t char_uuid_128[16];
}
char_desc_uuid_t;

/* Definition of UUID_t */
typedef __PACKED_UNION
{
	/**
   * 16-bit UUID
   */
	uint16_t uuid_16;
	/**
   * 128-bit UUID
   */
	uint8_t uuid_128[16];
}
uuid_t;

/* Definition of Handle_Entry_t */
typedef __PACKED_STRUCT
{
	/**
   * Attribute handle
   */
	uint16_t handle;
}
handle_entry_t;

/* Definition of Handle_Packets_Pair_Entry_t */
typedef __PACKED_STRUCT
{
	/**
   * Connection handle
   */
	uint16_t connection_handle;
	/**
   * The number of HCI Data Packets that have been completed (transmitted or
   * flushed) for the associated Connection_Handle since the previous time the
   * event was returned.
   */
	uint16_t hc_num_of_completed_packets;
}
handle_packets_pair_entry_t;

/* Definition of Advertising_Report_t */
typedef __PACKED_STRUCT
{
	/**
   * Type of advertising report event:
   * ADV_IND: Connectable undirected advertising',
   * ADV_DIRECT_IND: Connectable directed advertising,
   * ADV_SCAN_IND: Scannable undirected advertising,
   * ADV_NONCONN_IND: Non connectable undirected advertising,
   * SCAN_RSP: Scan response.
   * Values:
   * - 0x00: ADV_IND
   * - 0x01: ADV_DIRECT_IND
   * - 0x02: ADV_SCAN_IND
   * - 0x03: ADV_NONCONN_IND
   * - 0x04: SCAN_RSP
   */
	uint8_t event_type;
	/**
   * Address type.
   * Values:
   * - 0x00: Public Device Address
   * - 0x01: Random Device Address
   * - 0x02: Public Identity Address (corresponds to the Resolved Private
   *   Address)
   * - 0x03: Random (static) Identity Address (corresponds to the Resolved
   *   Private Address)
   */
	uint8_t address_type;
	/**
   * Public Device Address or Random Device Address of the device to be
   * connected.
   */
	uint8_t address[6];
	/**
   * Length of the Data field for each device which responded.
   * Values:
   * - 0 ... 31
   */
	uint8_t length_data;
	/**
   * Octets of advertising or scan response data formatted as defined in Core
   * Specification [Vol 3, Part C, 11].
   */
	const uint8_t *data;
	/**
   * RSSI (signed integer).
   * Units: dBm.
   * Values:
   * - 127: RSSI not available
   * - -127 ... 20
   */
	uint8_t rssi;
}
advertising_report_t;

/* Definition of Direct_Advertising_Report_t */
typedef __PACKED_STRUCT
{
	/**
   * Advertising type
   * Values:
   * - 0x01: Connectable directed advertising (ADV_DIRECT_IND)
   */
	uint8_t event_type;
	/**
   * Address type.
   * Values:
   * - 0x00: Public Device Address
   * - 0x01: Random Device Address
   * - 0x02: Public Identity Address (corresponds to the Resolved Private
   *   Address)
   * - 0x03: Random (static) Identity Address (corresponds to the Resolved
   *   Private Address)
   */
	uint8_t address_type;
	/**
   * Public Device Address, Random Device Address, Public Identity Address or
   * Random (static) Identity Address of the advertising device.
   */
	uint8_t address[6];
	/**
   * 0x01 Random Device Address
   * Values:
   * - 0x01: Random Device Address
   */
	uint8_t direct_address_type;
	/**
   * Random Device Address
   */
	uint8_t direct_address[6];
	/**
   * RSSI (signed integer).
   * Units: dBm.
   * Values:
   * - 127: RSSI not available
   * - -127 ... 20
   */
	uint8_t rssi;
}
direct_advertising_report_t;

/* Definition of Attribute_Group_Handle_Pair_t */
typedef __PACKED_STRUCT
{
	/**
   * Found Attribute handle
   */
	uint16_t found_attribute_handle;
	/**
   * Group End handle
   */
	uint16_t group_end_handle;
}
attribute_group_handle_pair_t;

/* Definition of Handle_Item_t */
typedef __PACKED_STRUCT
{
	uint16_t handle;
}
handle_item_t;

/* Internal types used by process functions */

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t reason;
}
hci_disconnect_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_disconnect_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
hci_read_remote_version_information_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_read_remote_version_information_rp0;

typedef __PACKED_STRUCT
{
	uint8_t event_mask[8];
}
hci_set_event_mask_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_set_event_mask_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_reset_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t type;
}
hci_read_transmit_power_level_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t transmit_power_level;
}
hci_read_transmit_power_level_rp0;

typedef __PACKED_STRUCT
{
	uint8_t flow_control_enable;
}
hci_set_controller_to_host_flow_control_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_set_controller_to_host_flow_control_rp0;

typedef __PACKED_STRUCT
{
	uint16_t host_acl_data_packet_length;
	uint8_t host_synchronous_data_packet_length;
	uint16_t host_total_num_acl_data_packets;
	uint16_t host_total_num_synchronous_data_packets;
}
hci_host_buffer_size_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_host_buffer_size_rp0;

typedef __PACKED_STRUCT
{
	uint8_t number_of_handles;
	host_nb_of_completed_pkt_pair_t
		host_nb_of_completed_pkt_pair[(BLE_CMD_MAX_PARAM_LEN - 1) /
									  sizeof(host_nb_of_completed_pkt_pair_t)];
}
hci_host_number_of_completed_packets_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_host_number_of_completed_packets_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t hci_version;
	uint16_t hci_subversion;
	uint8_t lmp_version;
	uint16_t company_identifier;
	uint16_t lmp_subversion;
}
hci_read_local_version_information_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t supported_commands[64];
}
hci_read_local_supported_commands_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t lmp_features[8];
}
hci_read_local_supported_features_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t bd_addr[6];
}
hci_read_bd_addr_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
hci_read_rssi_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t rssi;
}
hci_read_rssi_rp0;

typedef __PACKED_STRUCT
{
	uint8_t le_event_mask[8];
}
hci_le_set_event_mask_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_event_mask_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t hc_le_acl_data_packet_length;
	uint8_t hc_total_num_le_acl_data_packets;
}
hci_le_read_buffer_size_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t le_features[8];
}
hci_le_read_local_supported_features_page_0_rp0;

typedef __PACKED_STRUCT
{
	uint8_t random_address[6];
}
hci_le_set_random_address_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_random_address_rp0;

typedef __PACKED_STRUCT
{
	uint16_t advertising_interval_min;
	uint16_t advertising_interval_max;
	uint8_t advertising_type;
	uint8_t own_address_type;
	uint8_t peer_address_type;
	uint8_t peer_address[6];
	uint8_t advertising_channel_map;
	uint8_t advertising_filter_policy;
}
hci_le_set_advertising_parameters_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_advertising_parameters_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t transmit_power_level;
}
hci_le_read_advertising_physical_channel_tx_power_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_data_length;
	uint8_t advertising_data[31];
}
hci_le_set_advertising_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_advertising_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t scan_response_data_length;
	uint8_t scan_response_data[31];
}
hci_le_set_scan_response_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_scan_response_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_enable;
}
hci_le_set_advertising_enable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_advertising_enable_rp0;

typedef __PACKED_STRUCT
{
	uint8_t le_scan_type;
	uint16_t le_scan_interval;
	uint16_t le_scan_window;
	uint8_t own_address_type;
	uint8_t scanning_filter_policy;
}
hci_le_set_scan_parameters_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_scan_parameters_rp0;

typedef __PACKED_STRUCT
{
	uint8_t le_scan_enable;
	uint8_t filter_duplicates;
}
hci_le_set_scan_enable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_scan_enable_rp0;

typedef __PACKED_STRUCT
{
	uint16_t le_scan_interval;
	uint16_t le_scan_window;
	uint8_t initiator_filter_policy;
	uint8_t peer_address_type;
	uint8_t peer_address[6];
	uint8_t own_address_type;
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint16_t minimum_ce_length;
	uint16_t maximum_ce_length;
}
hci_le_create_connection_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_create_connection_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_create_connection_cancel_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t filter_accept_list_size;
}
hci_le_read_filter_accept_list_size_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_clear_filter_accept_list_rp0;

typedef __PACKED_STRUCT
{
	uint8_t address_type;
	uint8_t address[6];
}
hci_le_add_device_to_filter_accept_list_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_add_device_to_filter_accept_list_rp0;

typedef __PACKED_STRUCT
{
	uint8_t address_type;
	uint8_t address[6];
}
hci_le_remove_device_from_filter_accept_list_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_remove_device_from_filter_accept_list_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint16_t minimum_ce_length;
	uint16_t maximum_ce_length;
}
hci_le_connection_update_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_connection_update_rp0;

typedef __PACKED_STRUCT
{
	uint8_t le_channel_map[5];
}
hci_le_set_host_channel_classification_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_host_channel_classification_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
hci_le_read_channel_map_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t le_channel_map[5];
}
hci_le_read_channel_map_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
hci_le_read_remote_features_page_0_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_read_remote_features_page_0_rp0;

typedef __PACKED_STRUCT
{
	uint8_t key[16];
	uint8_t plaintext_data[16];
}
hci_le_encrypt_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t encrypted_data[16];
}
hci_le_encrypt_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t random_number[8];
}
hci_le_rand_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t random_number[8];
	uint16_t encrypted_diversifier;
	uint8_t long_term_key[16];
}
hci_le_enable_encryption_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_enable_encryption_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t long_term_key[16];
}
hci_le_long_term_key_request_reply_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
}
hci_le_long_term_key_request_reply_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
hci_le_long_term_key_request_negative_reply_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
}
hci_le_long_term_key_request_negative_reply_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t le_states[8];
}
hci_le_read_supported_states_rp0;

typedef __PACKED_STRUCT
{
	uint8_t rx_frequency;
}
hci_le_receiver_test_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_receiver_test_rp0;

typedef __PACKED_STRUCT
{
	uint8_t tx_frequency;
	uint8_t length_of_test_data;
	uint8_t packet_payload;
}
hci_le_transmitter_test_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_transmitter_test_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t number_of_packets;
}
hci_le_test_end_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t txoctets;
	uint16_t txtime;
}
hci_le_set_data_length_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
}
hci_le_set_data_length_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t suggestedmaxtxoctets;
	uint16_t suggestedmaxtxtime;
}
hci_le_read_suggested_default_data_length_rp0;

typedef __PACKED_STRUCT
{
	uint16_t suggestedmaxtxoctets;
	uint16_t suggestedmaxtxtime;
}
hci_le_write_suggested_default_data_length_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_write_suggested_default_data_length_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_read_local_p256_public_key_rp0;

typedef __PACKED_STRUCT
{
	uint8_t remote_p256_public_key[64];
}
hci_le_generate_dhkey_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_generate_dhkey_rp0;

typedef __PACKED_STRUCT
{
	uint8_t peer_identity_address_type;
	uint8_t peer_identity_address[6];
	uint8_t peer_irk[16];
	uint8_t local_irk[16];
}
hci_le_add_device_to_resolving_list_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_add_device_to_resolving_list_rp0;

typedef __PACKED_STRUCT
{
	uint8_t peer_identity_address_type;
	uint8_t peer_identity_address[6];
}
hci_le_remove_device_from_resolving_list_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_remove_device_from_resolving_list_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_clear_resolving_list_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t resolving_list_size;
}
hci_le_read_resolving_list_size_rp0;

typedef __PACKED_STRUCT
{
	uint8_t peer_identity_address_type;
	uint8_t peer_identity_address[6];
}
hci_le_read_peer_resolvable_address_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t peer_resolvable_address[6];
}
hci_le_read_peer_resolvable_address_rp0;

typedef __PACKED_STRUCT
{
	uint8_t peer_identity_address_type;
	uint8_t peer_identity_address[6];
}
hci_le_read_local_resolvable_address_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t local_resolvable_address[6];
}
hci_le_read_local_resolvable_address_rp0;

typedef __PACKED_STRUCT
{
	uint8_t address_resolution_enable;
}
hci_le_set_address_resolution_enable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_address_resolution_enable_rp0;

typedef __PACKED_STRUCT
{
	uint16_t rpa_timeout;
}
hci_le_set_resolvable_private_address_timeout_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_resolvable_private_address_timeout_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t supportedmaxtxoctets;
	uint16_t supportedmaxtxtime;
	uint16_t supportedmaxrxoctets;
	uint16_t supportedmaxrxtime;
}
hci_le_read_maximum_data_length_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
hci_le_read_phy_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t tx_phy;
	uint8_t rx_phy;
}
hci_le_read_phy_rp0;

typedef __PACKED_STRUCT
{
	uint8_t all_phys;
	uint8_t tx_phys;
	uint8_t rx_phys;
}
hci_le_set_default_phy_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_default_phy_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t all_phys;
	uint8_t tx_phys;
	uint8_t rx_phys;
	uint16_t phy_options;
}
hci_le_set_phy_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_phy_rp0;

typedef __PACKED_STRUCT
{
	uint8_t rx_frequency;
	uint8_t phy;
	uint8_t modulation_index;
}
hci_le_receiver_test_v2_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_receiver_test_v2_rp0;

typedef __PACKED_STRUCT
{
	uint8_t tx_frequency;
	uint8_t length_of_test_data;
	uint8_t packet_payload;
	uint8_t phy;
}
hci_le_transmitter_test_v2_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_transmitter_test_v2_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
	uint8_t random_address[6];
}
hci_le_set_advertising_set_random_address_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_advertising_set_random_address_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
	uint16_t adv_event_properties;
	uint8_t primary_adv_interval_min[3];
	uint8_t primary_adv_interval_max[3];
	uint8_t primary_adv_channel_map;
	uint8_t own_address_type;
	uint8_t peer_address_type;
	uint8_t peer_address[6];
	uint8_t adv_filter_policy;
	uint8_t adv_tx_power;
	uint8_t primary_adv_phy;
	uint8_t secondary_adv_max_skip;
	uint8_t secondary_adv_phy;
	uint8_t adv_sid;
	uint8_t scan_req_notification_enable;
}
hci_le_set_extended_advertising_parameters_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t selected_tx_power;
}
hci_le_set_extended_advertising_parameters_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
	uint8_t operation;
	uint8_t fragment_preference;
	uint8_t advertising_data_length;
	uint8_t advertising_data[BLE_CMD_MAX_PARAM_LEN - 4];
}
hci_le_set_extended_advertising_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_extended_advertising_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
	uint8_t operation;
	uint8_t fragment_preference;
	uint8_t scan_response_data_length;
	uint8_t scan_response_data[BLE_CMD_MAX_PARAM_LEN - 4];
}
hci_le_set_extended_scan_response_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_extended_scan_response_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t enable;
	uint8_t num_sets;
	adv_set_t adv_set[(BLE_CMD_MAX_PARAM_LEN - 2) / sizeof(adv_set_t)];
}
hci_le_set_extended_advertising_enable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_extended_advertising_enable_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t max_advertising_data_length;
}
hci_le_read_maximum_advertising_data_length_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t num_supported_advertising_sets;
}
hci_le_read_number_of_supported_advertising_sets_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
}
hci_le_remove_advertising_set_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_remove_advertising_set_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_clear_advertising_sets_rp0;

typedef __PACKED_STRUCT
{
	uint8_t own_address_type;
	uint8_t scanning_filter_policy;
	uint8_t scanning_phys;
	scan_param_phy_t scan_param_phy[2];
}
hci_le_set_extended_scan_parameters_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_extended_scan_parameters_rp0;

typedef __PACKED_STRUCT
{
	uint8_t enable;
	uint8_t filter_duplicates;
	uint16_t duration;
	uint16_t period;
}
hci_le_set_extended_scan_enable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_extended_scan_enable_rp0;

typedef __PACKED_STRUCT
{
	uint8_t initiator_filter_policy;
	uint8_t own_address_type;
	uint8_t peer_address_type;
	uint8_t peer_address[6];
	uint8_t initiating_phys;
	init_param_phy_t init_param_phy[3];
}
hci_le_extended_create_connection_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_extended_create_connection_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t min_tx_power;
	uint8_t max_tx_power;
}
hci_le_read_transmit_power_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t rf_tx_path_compensation;
	uint16_t rf_rx_path_compensation;
}
hci_le_read_rf_path_compensation_rp0;

typedef __PACKED_STRUCT
{
	uint16_t rf_tx_path_compensation;
	uint16_t rf_rx_path_compensation;
}
hci_le_write_rf_path_compensation_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_write_rf_path_compensation_rp0;

typedef __PACKED_STRUCT
{
	uint8_t peer_identity_address_type;
	uint8_t peer_identity_address[6];
	uint8_t privacy_mode;
}
hci_le_set_privacy_mode_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_privacy_mode_rp0;

typedef __PACKED_STRUCT
{
	uint8_t remote_p256_public_key[64];
	uint8_t key_type;
}
hci_le_generate_dhkey_v2_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_generate_dhkey_v2_rp0;

typedef __PACKED_STRUCT
{
	uint16_t rpa_timeout_min;
	uint16_t rpa_timeout_max;
}
hci_le_set_resolvable_private_address_timeout_v2_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
hci_le_set_resolvable_private_address_timeout_v2_rp0;

typedef __PACKED_STRUCT
{
	uint8_t offset;
	uint8_t length;
	uint8_t value[BLE_CMD_MAX_PARAM_LEN - 2];
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
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 3) - 2];
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
	uint8_t in_data[BLE_CMD_MAX_PARAM_LEN - 27];
}
aci_hal_ead_encrypt_decrypt_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t out_data_length;
	uint8_t out_data[(BLE_EVT_MAX_PARAM_LEN - 3) - 3];
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

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_non_discoverable_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_type;
	uint16_t advertising_interval_min;
	uint16_t advertising_interval_max;
	uint8_t own_address_type;
	uint8_t advertising_filter_policy;
	uint8_t local_name_length;
	uint8_t local_name[BLE_CMD_MAX_PARAM_LEN - 13];
}
aci_gap_set_limited_discoverable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t service_uuid_length;
	uint8_t service_uuid_list[BLE_CMD_MAX_PARAM_LEN - 13];
}
aci_gap_set_limited_discoverable_cp1;

typedef __PACKED_STRUCT
{
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
}
aci_gap_set_limited_discoverable_cp2;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_limited_discoverable_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_type;
	uint16_t advertising_interval_min;
	uint16_t advertising_interval_max;
	uint8_t own_address_type;
	uint8_t advertising_filter_policy;
	uint8_t local_name_length;
	uint8_t local_name[BLE_CMD_MAX_PARAM_LEN - 13];
}
aci_gap_set_discoverable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t service_uuid_length;
	uint8_t service_uuid_list[BLE_CMD_MAX_PARAM_LEN - 13];
}
aci_gap_set_discoverable_cp1;

typedef __PACKED_STRUCT
{
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
}
aci_gap_set_discoverable_cp2;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_discoverable_rp0;

typedef __PACKED_STRUCT
{
	uint8_t own_address_type;
	uint8_t directed_advertising_type;
	uint8_t direct_address_type;
	uint8_t direct_address[6];
	uint16_t advertising_interval_min;
	uint16_t advertising_interval_max;
}
aci_gap_set_direct_connectable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_direct_connectable_rp0;

typedef __PACKED_STRUCT
{
	uint8_t io_capability;
}
aci_gap_set_io_capability_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_io_capability_rp0;

typedef __PACKED_STRUCT
{
	uint8_t bonding_mode;
	uint8_t mitm_mode;
	uint8_t sc_support;
	uint8_t keypress_notification_support;
	uint8_t min_encryption_key_size;
	uint8_t max_encryption_key_size;
	uint8_t use_fixed_pin;
	uint32_t fixed_pin;
	uint8_t identity_address_type;
}
aci_gap_set_authentication_requirement_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_authentication_requirement_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t authorization_enable;
}
aci_gap_set_authorization_requirement_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_authorization_requirement_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint32_t pass_key;
}
aci_gap_pass_key_resp_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_pass_key_resp_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t authorize;
}
aci_gap_authorization_resp_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_authorization_resp_rp0;

typedef __PACKED_STRUCT
{
	uint8_t role;
	uint8_t privacy_enabled;
	uint8_t device_name_char_len;
}
aci_gap_init_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t service_handle;
	uint16_t dev_name_char_handle;
	uint16_t appearance_char_handle;
}
aci_gap_init_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_event_type;
	uint8_t own_address_type;
}
aci_gap_set_non_connectable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_non_connectable_rp0;

typedef __PACKED_STRUCT
{
	uint16_t advertising_interval_min;
	uint16_t advertising_interval_max;
	uint8_t own_address_type;
	uint8_t adv_filter_policy;
}
aci_gap_set_undirected_connectable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_undirected_connectable_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gap_peripheral_security_req_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_peripheral_security_req_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advdatalen;
	uint8_t advdata[BLE_CMD_MAX_PARAM_LEN - 1];
}
aci_gap_update_adv_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_update_adv_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t adtype;
}
aci_gap_delete_ad_type_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_delete_ad_type_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gap_get_security_level_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t security_mode;
	uint8_t security_level;
}
aci_gap_get_security_level_rp0;

typedef __PACKED_STRUCT
{
	uint16_t gap_evt_mask;
}
aci_gap_set_event_mask_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_event_mask_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_configure_filter_accept_list_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t reason;
}
aci_gap_terminate_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_terminate_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_clear_security_db_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gap_allow_rebond_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_allow_rebond_rp0;

typedef __PACKED_STRUCT
{
	uint16_t le_scan_interval;
	uint16_t le_scan_window;
	uint8_t own_address_type;
	uint8_t filter_duplicates;
}
aci_gap_start_limited_discovery_proc_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_start_limited_discovery_proc_rp0;

typedef __PACKED_STRUCT
{
	uint16_t le_scan_interval;
	uint16_t le_scan_window;
	uint8_t own_address_type;
	uint8_t filter_duplicates;
}
aci_gap_start_general_discovery_proc_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_start_general_discovery_proc_rp0;

typedef __PACKED_STRUCT
{
	uint16_t le_scan_interval;
	uint16_t le_scan_window;
	uint8_t own_address_type;
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint16_t minimum_ce_length;
	uint16_t maximum_ce_length;
	uint8_t num_of_peer_entries;
	peer_entry_t peer_entry[(BLE_CMD_MAX_PARAM_LEN - 18) / sizeof(peer_entry_t)];
}
aci_gap_start_auto_connection_establish_proc_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_start_auto_connection_establish_proc_rp0;

typedef __PACKED_STRUCT
{
	uint8_t le_scan_type;
	uint16_t le_scan_interval;
	uint16_t le_scan_window;
	uint8_t own_address_type;
	uint8_t scanning_filter_policy;
	uint8_t filter_duplicates;
}
aci_gap_start_general_connection_establish_proc_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_start_general_connection_establish_proc_rp0;

typedef __PACKED_STRUCT
{
	uint8_t le_scan_type;
	uint16_t le_scan_interval;
	uint16_t le_scan_window;
	uint8_t own_address_type;
	uint8_t scanning_filter_policy;
	uint8_t filter_duplicates;
	uint8_t num_of_peer_entries;
	peer_entry_t peer_entry[(BLE_CMD_MAX_PARAM_LEN - 9) / sizeof(peer_entry_t)];
}
aci_gap_start_selective_connection_establish_proc_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_start_selective_connection_establish_proc_rp0;

typedef __PACKED_STRUCT
{
	uint16_t le_scan_interval;
	uint16_t le_scan_window;
	uint8_t peer_address_type;
	uint8_t peer_address[6];
	uint8_t own_address_type;
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint16_t minimum_ce_length;
	uint16_t maximum_ce_length;
}
aci_gap_create_connection_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_create_connection_rp0;

typedef __PACKED_STRUCT
{
	uint8_t procedure_code;
}
aci_gap_terminate_gap_proc_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_terminate_gap_proc_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint16_t minimum_ce_length;
	uint16_t maximum_ce_length;
}
aci_gap_start_connection_update_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_start_connection_update_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t force_rebond;
}
aci_gap_send_pairing_req_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_send_pairing_req_rp0;

typedef __PACKED_STRUCT
{
	uint16_t advertising_interval_min;
	uint16_t advertising_interval_max;
	uint8_t advertising_type;
	uint8_t own_address_type;
	uint8_t adv_data_length;
	uint8_t adv_data[BLE_CMD_MAX_PARAM_LEN - 8];
}
aci_gap_set_broadcast_mode_cp0;

typedef __PACKED_STRUCT
{
	uint8_t num_of_peer_entries;
	peer_entry_t peer_entry[(BLE_CMD_MAX_PARAM_LEN - 8) / sizeof(peer_entry_t)];
}
aci_gap_set_broadcast_mode_cp1;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_broadcast_mode_rp0;

typedef __PACKED_STRUCT
{
	uint16_t le_scan_interval;
	uint16_t le_scan_window;
	uint8_t le_scan_type;
	uint8_t own_address_type;
	uint8_t filter_duplicates;
	uint8_t scanning_filter_policy;
}
aci_gap_start_observation_proc_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_start_observation_proc_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t num_of_addresses;
	bonded_device_entry_t
		bonded_device_entry[((BLE_EVT_MAX_PARAM_LEN - 3) - 2) / sizeof(bonded_device_entry_t)];
}
aci_gap_get_bonded_devices_rp0;

typedef __PACKED_STRUCT
{
	uint8_t peer_address_type;
	uint8_t peer_address[6];
}
aci_gap_check_bonded_device_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t id_address_type;
	uint8_t id_address[6];
}
aci_gap_check_bonded_device_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t confirm_yes_no;
}
aci_gap_numeric_comparison_value_confirm_yesno_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_numeric_comparison_value_confirm_yesno_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t input_type;
}
aci_gap_passkey_input_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_passkey_input_rp0;

typedef __PACKED_STRUCT
{
	uint8_t oob_data_type;
}
aci_gap_get_oob_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t address_type;
	uint8_t address[6];
	uint8_t oob_data_type;
	uint8_t oob_data_len;
	uint8_t oob_data[16];
}
aci_gap_get_oob_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t device_type;
	uint8_t address_type;
	uint8_t address[6];
	uint8_t oob_data_type;
	uint8_t oob_data_len;
	uint8_t oob_data[16];
}
aci_gap_set_oob_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_set_oob_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t peer_identity_address_type;
	uint8_t peer_identity_address[6];
}
aci_gap_remove_bonded_device_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_remove_bonded_device_rp0;

typedef __PACKED_STRUCT
{
	uint8_t num_of_list_entries;
	list_entry_t list_entry[(BLE_CMD_MAX_PARAM_LEN - 2) / sizeof(list_entry_t)];
}
aci_gap_add_devices_to_list_cp0;

typedef __PACKED_STRUCT
{
	uint8_t mode;
}
aci_gap_add_devices_to_list_cp1;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_add_devices_to_list_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t accept;
}
aci_gap_pairing_request_reply_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_pairing_request_reply_rp0;

typedef __PACKED_STRUCT
{
	uint16_t adv_interval_min;
	uint16_t adv_interval_max;
	uint8_t adv_channel_map;
	uint8_t own_address_type;
	uint8_t own_address[6];
	uint8_t pa_level;
}
aci_gap_additional_beacon_start_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_additional_beacon_start_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_additional_beacon_stop_rp0;

typedef __PACKED_STRUCT
{
	uint8_t adv_data_length;
	uint8_t adv_data[BLE_CMD_MAX_PARAM_LEN - 1];
}
aci_gap_additional_beacon_set_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_additional_beacon_set_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t adv_mode;
	uint8_t advertising_handle;
	uint16_t adv_event_properties;
	uint32_t primary_adv_interval_min;
	uint32_t primary_adv_interval_max;
	uint8_t primary_adv_channel_map;
	uint8_t own_address_type;
	uint8_t peer_address_type;
	uint8_t peer_address[6];
	uint8_t adv_filter_policy;
	uint8_t adv_tx_power;
	uint8_t secondary_adv_max_skip;
	uint8_t secondary_adv_phy;
	uint8_t adv_sid;
	uint8_t scan_req_notification_enable;
}
aci_gap_adv_set_configuration_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_adv_set_configuration_rp0;

typedef __PACKED_STRUCT
{
	uint8_t enable;
	uint8_t num_sets;
	adv_set_t adv_set[(BLE_CMD_MAX_PARAM_LEN - 2) / sizeof(adv_set_t)];
}
aci_gap_adv_set_enable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_adv_set_enable_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
	uint8_t operation;
	uint8_t fragment_preference;
	uint8_t advertising_data_length;
	uint8_t advertising_data[BLE_CMD_MAX_PARAM_LEN - 4];
}
aci_gap_adv_set_adv_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_adv_set_adv_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
	uint8_t operation;
	uint8_t fragment_preference;
	uint8_t scan_response_data_length;
	uint8_t scan_response_data[BLE_CMD_MAX_PARAM_LEN - 4];
}
aci_gap_adv_set_scan_resp_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_adv_set_scan_resp_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
}
aci_gap_adv_remove_set_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_adv_remove_set_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_adv_clear_sets_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
	uint8_t random_address[6];
}
aci_gap_adv_set_random_address_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_adv_set_random_address_rp0;

typedef __PACKED_STRUCT
{
	uint8_t scan_mode;
	uint8_t procedure;
	uint8_t own_address_type;
	uint8_t filter_duplicates;
	uint16_t duration;
	uint16_t period;
	uint8_t scanning_filter_policy;
	uint8_t scanning_phys;
	scan_param_phy_t scan_param_phy[2];
}
aci_gap_ext_start_scan_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_ext_start_scan_rp0;

typedef __PACKED_STRUCT
{
	uint8_t initiating_mode;
	uint8_t procedure;
	uint8_t own_address_type;
	uint8_t peer_address_type;
	uint8_t peer_address[6];
	uint8_t advertising_handle;
	uint8_t subevent;
	uint8_t initiator_filter_policy;
	uint8_t initiating_phys;
	init_param_phy_t init_param_phy[3];
}
aci_gap_ext_create_connection_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gap_ext_create_connection_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_init_rp0;

typedef __PACKED_STRUCT
{
	uint8_t service_uuid_type;
	service_uuid_t service_uuid;
}
aci_gatt_add_service_cp0;

typedef __PACKED_STRUCT
{
	uint8_t service_type;
	uint8_t max_attribute_records;
}
aci_gatt_add_service_cp1;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t service_handle;
}
aci_gatt_add_service_rp0;

typedef __PACKED_STRUCT
{
	uint16_t service_handle;
	uint16_t include_start_handle;
	uint16_t include_end_handle;
	uint8_t include_uuid_type;
	include_uuid_t include_uuid;
}
aci_gatt_include_service_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t include_handle;
}
aci_gatt_include_service_rp0;

typedef __PACKED_STRUCT
{
	uint16_t service_handle;
	uint8_t char_uuid_type;
	char_uuid_t char_uuid;
}
aci_gatt_add_char_cp0;

typedef __PACKED_STRUCT
{
	uint16_t char_value_length;
	uint8_t char_properties;
	uint8_t security_permissions;
	uint8_t gatt_evt_mask;
	uint8_t enc_key_size;
	uint8_t is_variable;
}
aci_gatt_add_char_cp1;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t char_handle;
}
aci_gatt_add_char_rp0;

typedef __PACKED_STRUCT
{
	uint16_t service_handle;
	uint16_t char_handle;
	uint8_t char_desc_uuid_type;
	char_desc_uuid_t char_desc_uuid;
}
aci_gatt_add_char_desc_cp0;

typedef __PACKED_STRUCT
{
	uint8_t char_desc_value_max_len;
	uint8_t char_desc_value_length;
	uint8_t char_desc_value[BLE_CMD_MAX_PARAM_LEN - 12];
}
aci_gatt_add_char_desc_cp1;

typedef __PACKED_STRUCT
{
	uint8_t security_permissions;
	uint8_t access_permissions;
	uint8_t gatt_evt_mask;
	uint8_t enc_key_size;
	uint8_t is_variable;
}
aci_gatt_add_char_desc_cp2;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t char_desc_handle;
}
aci_gatt_add_char_desc_rp0;

typedef __PACKED_STRUCT
{
	uint16_t service_handle;
	uint16_t char_handle;
	uint8_t val_offset;
	uint8_t char_value_length;
	uint8_t char_value[BLE_CMD_MAX_PARAM_LEN - 6];
}
aci_gatt_update_char_value_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_update_char_value_rp0;

typedef __PACKED_STRUCT
{
	uint16_t serv_handle;
	uint16_t char_handle;
}
aci_gatt_del_char_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_del_char_rp0;

typedef __PACKED_STRUCT
{
	uint16_t serv_handle;
}
aci_gatt_del_service_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_del_service_rp0;

typedef __PACKED_STRUCT
{
	uint16_t serv_handle;
	uint16_t include_handle;
}
aci_gatt_del_include_service_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_del_include_service_rp0;

typedef __PACKED_STRUCT
{
	uint32_t gatt_evt_mask;
}
aci_gatt_set_event_mask_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_set_event_mask_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gatt_exchange_config_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_exchange_config_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t start_handle;
	uint16_t end_handle;
}
aci_att_find_info_req_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_att_find_info_req_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t start_handle;
	uint16_t end_handle;
	uint16_t uuid;
	uint8_t attribute_val_length;
	uint8_t attribute_val[BLE_CMD_MAX_PARAM_LEN - 9];
}
aci_att_find_by_type_value_req_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_att_find_by_type_value_req_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t start_handle;
	uint16_t end_handle;
	uint8_t uuid_type;
	uuid_t uuid;
}
aci_att_read_by_type_req_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_att_read_by_type_req_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t start_handle;
	uint16_t end_handle;
	uint8_t uuid_type;
	uuid_t uuid;
}
aci_att_read_by_group_type_req_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_att_read_by_group_type_req_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint16_t val_offset;
	uint8_t attribute_val_length;
	uint8_t attribute_val[BLE_CMD_MAX_PARAM_LEN - 7];
}
aci_att_prepare_write_req_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_att_prepare_write_req_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t execute;
}
aci_att_execute_write_req_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_att_execute_write_req_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gatt_disc_all_primary_services_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_disc_all_primary_services_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t uuid_type;
	uuid_t uuid;
}
aci_gatt_disc_primary_service_by_uuid_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_disc_primary_service_by_uuid_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t start_handle;
	uint16_t end_handle;
}
aci_gatt_find_included_services_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_find_included_services_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t start_handle;
	uint16_t end_handle;
}
aci_gatt_disc_all_char_of_service_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_disc_all_char_of_service_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t start_handle;
	uint16_t end_handle;
	uint8_t uuid_type;
	uuid_t uuid;
}
aci_gatt_disc_char_by_uuid_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_disc_char_by_uuid_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t char_handle;
	uint16_t end_handle;
}
aci_gatt_disc_all_char_desc_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_disc_all_char_desc_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
}
aci_gatt_read_char_value_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_read_char_value_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t start_handle;
	uint16_t end_handle;
	uint8_t uuid_type;
	uuid_t uuid;
}
aci_gatt_read_using_char_uuid_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_read_using_char_uuid_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint16_t val_offset;
}
aci_gatt_read_long_char_value_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_read_long_char_value_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t number_of_handles;
	handle_entry_t handle_entry[(BLE_CMD_MAX_PARAM_LEN - 3) / sizeof(handle_entry_t)];
}
aci_gatt_read_multiple_char_value_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_read_multiple_char_value_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint8_t attribute_val_length;
	uint8_t attribute_val[BLE_CMD_MAX_PARAM_LEN - 5];
}
aci_gatt_write_char_value_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_write_char_value_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint16_t val_offset;
	uint8_t attribute_val_length;
	uint8_t attribute_val[BLE_CMD_MAX_PARAM_LEN - 7];
}
aci_gatt_write_long_char_value_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_write_long_char_value_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint16_t val_offset;
	uint8_t attribute_val_length;
	uint8_t attribute_val[BLE_CMD_MAX_PARAM_LEN - 7];
}
aci_gatt_write_char_reliable_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_write_char_reliable_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint8_t attribute_val_length;
	uint8_t attribute_val[BLE_CMD_MAX_PARAM_LEN - 5];
}
aci_gatt_write_without_resp_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_write_without_resp_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint8_t attribute_val_length;
	uint8_t attribute_val[BLE_CMD_MAX_PARAM_LEN - 5];
}
aci_gatt_signed_write_without_resp_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_signed_write_without_resp_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gatt_confirm_indication_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_confirm_indication_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint8_t write_status;
	uint8_t error_code;
	uint8_t attribute_val_length;
	uint8_t attribute_val[BLE_CMD_MAX_PARAM_LEN - 7];
}
aci_gatt_permit_write_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_permit_write_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t read_status;
	uint8_t error_code;
	uint16_t attr_handle;
}
aci_gatt_permit_read_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_permit_read_rp0;

typedef __PACKED_STRUCT
{
	uint16_t serv_handle;
	uint16_t attr_handle;
	uint8_t security_permissions;
}
aci_gatt_set_security_permission_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_set_security_permission_rp0;

typedef __PACKED_STRUCT
{
	uint16_t serv_handle;
	uint16_t char_handle;
	uint16_t char_desc_handle;
	uint16_t val_offset;
	uint8_t char_desc_value_length;
	uint8_t char_desc_value[BLE_CMD_MAX_PARAM_LEN - 9];
}
aci_gatt_set_desc_value_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_set_desc_value_rp0;

typedef __PACKED_STRUCT
{
	uint16_t attr_handle;
	uint16_t offset;
	uint16_t value_length_requested;
}
aci_gatt_read_handle_value_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t length;
	uint16_t value_length;
	uint8_t value[(BLE_EVT_MAX_PARAM_LEN - 3) - 5];
}
aci_gatt_read_handle_value_rp0;

typedef __PACKED_STRUCT
{
	uint16_t conn_handle_to_notify;
	uint16_t service_handle;
	uint16_t char_handle;
	uint8_t update_type;
	uint16_t char_length;
	uint16_t value_offset;
	uint8_t value_length;
	uint8_t value[BLE_CMD_MAX_PARAM_LEN - 12];
}
aci_gatt_update_char_value_ext_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_update_char_value_ext_rp0;

typedef __PACKED_STRUCT
{
	uint16_t serv_handle;
	uint16_t attr_handle;
	uint8_t access_permissions;
}
aci_gatt_set_access_permission_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_set_access_permission_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_store_db_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t number_of_handles;
	handle_entry_t handle_entry[(BLE_CMD_MAX_PARAM_LEN - 3) / sizeof(handle_entry_t)];
}
aci_gatt_send_mult_notification_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_send_mult_notification_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t number_of_handles;
	handle_entry_t handle_entry[(BLE_CMD_MAX_PARAM_LEN - 3) / sizeof(handle_entry_t)];
}
aci_gatt_read_multiple_var_char_value_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_read_multiple_var_char_value_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint8_t signed_mode;
	uint16_t data_length;
	uint32_t data_pointer;
}
aci_gatt_write_without_resp_ext_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_write_without_resp_ext_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint8_t write_mode;
	uint16_t val_offset;
	uint16_t data_length;
	uint32_t data_pointer;
}
aci_gatt_write_with_resp_ext_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_gatt_write_with_resp_ext_rp0;

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
	uint8_t channel_index_list[(BLE_EVT_MAX_PARAM_LEN - 3) - 2];
}
aci_l2cap_coc_connect_confirm_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t mtu;
	uint16_t mps;
	uint8_t channel_number;
	uint8_t channel_index_list[BLE_CMD_MAX_PARAM_LEN - 7];
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
	uint8_t data[BLE_CMD_MAX_PARAM_LEN - 3];
}
aci_l2cap_coc_tx_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
}
aci_l2cap_coc_tx_data_rp0;

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
	uint8_t value[BLE_CMD_MAX_PARAM_LEN - 2];
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
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 3) - 2];
}
aci_read_config_data_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t reason;
}
hci_disconnection_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t encryption_enabled;
}
hci_encryption_change_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t version;
	uint16_t manufacturer_name;
	uint16_t subversion;
}
hci_read_remote_version_information_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t hardware_code;
}
hci_hardware_error_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t number_of_handles;
	handle_packets_pair_entry_t handle_packets_pair_entry[(BLE_EVT_MAX_PARAM_LEN - 1) /
														  sizeof(handle_packets_pair_entry_t)];
}
hci_number_of_completed_packets_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
}
hci_encryption_key_refresh_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t role;
	uint8_t peer_address_type;
	uint8_t peer_address[6];
	uint16_t conn_interval;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint8_t central_clock_accuracy;
}
hci_le_connection_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t num_reports;
	advertising_report_t
		advertising_report[((BLE_EVT_MAX_PARAM_LEN - 1) - 1) / sizeof(advertising_report_t)];
}
hci_le_advertising_report_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint16_t conn_interval;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
}
hci_le_connection_update_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t le_features[8];
}
hci_le_read_remote_features_page_0_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t random_number[8];
	uint16_t encrypted_diversifier;
}
hci_le_long_term_key_request_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t maxtxoctets;
	uint16_t maxtxtime;
	uint16_t maxrxoctets;
	uint16_t maxrxtime;
}
hci_le_data_length_change_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t local_p256_public_key[64];
}
hci_le_read_local_p256_public_key_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t dhkey[32];
}
hci_le_generate_dhkey_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t role;
	uint8_t peer_address_type;
	uint8_t peer_address[6];
	uint8_t local_resolvable_private_address[6];
	uint8_t peer_resolvable_private_address[6];
	uint16_t conn_interval;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint8_t central_clock_accuracy;
}
hci_le_enhanced_connection_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t num_reports;
	direct_advertising_report_t direct_advertising_report[((BLE_EVT_MAX_PARAM_LEN - 1) - 1) /
														  sizeof(direct_advertising_report_t)];
}
hci_le_directed_advertising_report_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t connection_handle;
	uint8_t tx_phy;
	uint8_t rx_phy;
}
hci_le_phy_update_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t num_reports;
	uint16_t event_type;
	uint8_t address_type;
	uint8_t address[6];
	uint8_t primary_phy;
	uint8_t secondary_phy;
	uint8_t advertising_sid;
	uint8_t tx_power;
	uint8_t rssi;
	uint16_t periodic_adv_interval;
	uint8_t direct_address_type;
	uint8_t direct_address[6];
	uint8_t data_length;
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 1) - 25];
}
hci_le_extended_advertising_report_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint8_t advertising_handle;
	uint16_t connection_handle;
	uint8_t num_completed_ext_adv_events;
}
hci_le_advertising_set_terminated_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
	uint8_t scanner_address_type;
	uint8_t scanner_address[6];
}
hci_le_scan_request_received_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t channel_selection_algorithm;
}
hci_le_channel_selection_algorithm_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t warning_type;
	uint8_t data_length;
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 2) - 2];
}
aci_warning_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t status;
	uint8_t reason;
}
aci_gap_pairing_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gap_pass_key_req_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gap_authorization_req_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gap_bond_lost_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t procedure_code;
	uint8_t status;
	uint8_t data_length;
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 2) - 3];
}
aci_gap_proc_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gap_addr_not_resolved_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint32_t numeric_value;
}
aci_gap_numeric_comparison_value_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t notification_type;
}
aci_gap_keypress_notification_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t bonded;
	uint8_t auth_req;
}
aci_gap_pairing_request_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t result;
}
aci_l2cap_connection_update_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t data_length;
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 2) - 3];
}
aci_l2cap_proc_timeout_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t identifier;
	uint16_t l2cap_length;
	uint16_t interval_min;
	uint16_t interval_max;
	uint16_t latency;
	uint16_t timeout_multiplier;
}
aci_l2cap_connection_update_req_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t identifier;
	uint16_t reason;
	uint8_t data_length;
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 2) - 6];
}
aci_l2cap_command_reject_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t spsm;
	uint16_t mtu;
	uint16_t mps;
	uint16_t initial_credits;
	uint8_t channel_number;
}
aci_l2cap_coc_connect_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t mtu;
	uint16_t mps;
	uint16_t initial_credits;
	uint16_t result;
	uint8_t channel_number;
	uint8_t channel_index_list[(BLE_EVT_MAX_PARAM_LEN - 2) - 11];
}
aci_l2cap_coc_connect_confirm_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t mtu;
	uint16_t mps;
	uint8_t channel_number;
	uint8_t channel_index_list[(BLE_EVT_MAX_PARAM_LEN - 2) - 7];
}
aci_l2cap_coc_reconf_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t result;
}
aci_l2cap_coc_reconf_confirm_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t channel_index;
}
aci_l2cap_coc_disconnect_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t channel_index;
	uint16_t credits;
}
aci_l2cap_coc_flow_control_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t channel_index;
	uint16_t length;
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 2) - 3];
}
aci_l2cap_coc_rx_data_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attr_handle;
	uint16_t offset;
	uint16_t attr_data_length;
	uint8_t attr_data[(BLE_EVT_MAX_PARAM_LEN - 2) - 8];
}
aci_gatt_attribute_modified_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gatt_proc_timeout_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t server_rx_mtu;
}
aci_att_exchange_mtu_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t format;
	uint8_t event_data_length;
	uint8_t handle_uuid_pair[(BLE_EVT_MAX_PARAM_LEN - 2) - 4];
}
aci_att_find_info_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t num_of_handle_pair;
	attribute_group_handle_pair_t attribute_group_handle_pair[((BLE_EVT_MAX_PARAM_LEN - 2) - 3) /
															  sizeof(attribute_group_handle_pair_t)];
}
aci_att_find_by_type_value_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t handle_value_pair_length;
	uint8_t data_length;
	uint8_t handle_value_pair_data[(BLE_EVT_MAX_PARAM_LEN - 2) - 4];
}
aci_att_read_by_type_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t event_data_length;
	uint8_t attribute_value[(BLE_EVT_MAX_PARAM_LEN - 2) - 3];
}
aci_att_read_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t event_data_length;
	uint8_t attribute_value[(BLE_EVT_MAX_PARAM_LEN - 2) - 3];
}
aci_att_read_blob_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t event_data_length;
	uint8_t set_of_values[(BLE_EVT_MAX_PARAM_LEN - 2) - 3];
}
aci_att_read_multiple_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t attribute_data_length;
	uint8_t data_length;
	uint8_t attribute_data_list[(BLE_EVT_MAX_PARAM_LEN - 2) - 4];
}
aci_att_read_by_group_type_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attribute_handle;
	uint16_t offset;
	uint8_t part_attribute_value_length;
	uint8_t part_attribute_value[(BLE_EVT_MAX_PARAM_LEN - 2) - 7];
}
aci_att_prepare_write_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_att_exec_write_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attribute_handle;
	uint8_t attribute_value_length;
	uint8_t attribute_value[(BLE_EVT_MAX_PARAM_LEN - 2) - 5];
}
aci_gatt_indication_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attribute_handle;
	uint8_t attribute_value_length;
	uint8_t attribute_value[(BLE_EVT_MAX_PARAM_LEN - 2) - 5];
}
aci_gatt_notification_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t error_code;
}
aci_gatt_proc_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t req_opcode;
	uint16_t attribute_handle;
	uint8_t error_code;
}
aci_gatt_error_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attribute_handle;
	uint8_t attribute_value_length;
	uint8_t attribute_value[(BLE_EVT_MAX_PARAM_LEN - 2) - 5];
}
aci_gatt_disc_read_char_by_uuid_resp_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attribute_handle;
	uint8_t data_length;
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 2) - 5];
}
aci_gatt_write_permit_req_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attribute_handle;
	uint16_t offset;
}
aci_gatt_read_permit_req_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t number_of_handles;
	handle_item_t handle_item[((BLE_EVT_MAX_PARAM_LEN - 2) - 3) / sizeof(handle_item_t)];
}
aci_gatt_read_multi_permit_req_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t available_buffers;
}
aci_gatt_tx_pool_available_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
}
aci_gatt_server_confirmation_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attribute_handle;
	uint16_t offset;
	uint8_t data_length;
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 2) - 7];
}
aci_gatt_prepare_write_permit_req_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint8_t channel_index;
	uint8_t eab_state;
	uint16_t mtu;
}
aci_gatt_eatt_bearer_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t offset;
	uint16_t data_length;
	uint8_t data[(BLE_EVT_MAX_PARAM_LEN - 2) - 6];
}
aci_gatt_mult_notification_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t attr_handle;
}
aci_gatt_notification_complete_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t offset;
	uint16_t event_data_length;
	uint8_t attribute_value[(BLE_EVT_MAX_PARAM_LEN - 2) - 6];
}
aci_gatt_read_ext_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attribute_handle;
	uint16_t offset;
	uint16_t attribute_value_length;
	uint8_t attribute_value[(BLE_EVT_MAX_PARAM_LEN - 2) - 8];
}
aci_gatt_indication_ext_event_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t attribute_handle;
	uint16_t offset;
	uint16_t attribute_value_length;
	uint8_t attribute_value[(BLE_EVT_MAX_PARAM_LEN - 2) - 8];
}
aci_gatt_notification_ext_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t last_state;
	uint8_t next_state;
	uint32_t next_state_systime;
	uint8_t last_state_slot;
	uint8_t next_state_slot;
}
aci_hal_end_of_radio_activity_event_rp0;

typedef __PACKED_STRUCT
{
	uint8_t rssi;
	uint8_t peer_address_type;
	uint8_t peer_address[6];
}
aci_hal_scan_req_report_event_rp0;

#endif /* BLE_TYPES_H__ */
