#ifndef BLE_HCI_LE_H
#define BLE_HCI_LE_H

#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"
#include <stdint.h>

/* Injected explicit structural configurations from ble_types */
typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
	uint16_t duration;
	uint8_t max_extended_advertising_events;
}
adv_set_t;

typedef __PACKED_STRUCT
{
	uint8_t scan_type;
	uint16_t scan_interval;
	uint16_t scan_window;
}
scan_param_phy_t;

typedef __PACKED_STRUCT
{
	uint16_t scan_interval;
	uint16_t scan_window;
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint16_t min_ce_length;
	uint16_t max_ce_length;
}
init_param_phy_t;

/* Pulled Command and Return Parameter Payload Structs */
typedef __PACKED_STRUCT
{
	uint8_t le_event_mask[8];
}
hci_le_set_event_mask_cp0;

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
	uint8_t scan_response_data_length;
	uint8_t scan_response_data[31];
}
hci_le_set_scan_response_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_enable;
}
hci_le_set_advertising_enable_cp0;

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
	uint8_t le_scan_enable;
	uint8_t filter_duplicates;
}
hci_le_set_scan_enable_cp0;

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
	uint8_t filter_accept_list_size;
}
hci_le_read_filter_accept_list_size_rp0;

typedef __PACKED_STRUCT
{
	uint8_t address_type;
	uint8_t address[6];
}
hci_le_add_device_to_filter_accept_list_cp0;

typedef __PACKED_STRUCT
{
	uint8_t address_type;
	uint8_t address[6];
}
hci_le_remove_device_from_filter_accept_list_cp0;

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
	uint8_t le_channel_map[5];
}
hci_le_set_host_channel_classification_cp0;

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
	uint8_t tx_frequency;
	uint8_t length_of_test_data;
	uint8_t packet_payload;
}
hci_le_transmitter_test_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t number_of_packets;
}
hci_le_test_end_rp0;

typedef __PACKED_STRUCT
{
	uint16_t connection_handle;
	uint16_t tx_octets;
	uint16_t tx_time;
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
	uint16_t suggested_max_tx_octets;
	uint16_t suggested_max_tx_time;
}
hci_le_read_suggested_default_data_length_rp0;

typedef __PACKED_STRUCT
{
	uint16_t suggested_max_tx_octets;
	uint16_t suggested_max_tx_time;
}
hci_le_write_suggested_default_data_length_cp0;

typedef __PACKED_STRUCT
{
	uint8_t remote_p256_public_key[64];
}
hci_le_generate_dhkey_cp0;

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
	uint8_t peer_identity_address_type;
	uint8_t peer_identity_address[6];
}
hci_le_remove_device_from_resolving_list_cp0;

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
	uint16_t rpa_timeout;
}
hci_le_set_resolvable_private_address_timeout_cp0;

typedef __PACKED_STRUCT
{
	uint8_t status;
	uint16_t supported_max_tx_octets;
	uint16_t supported_max_tx_time;
	uint16_t supported_max_rx_octets;
	uint16_t supported_max_rx_time;
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
	uint16_t connection_handle;
	uint8_t all_phys;
	uint8_t tx_phys;
	uint8_t rx_phys;
	uint16_t phy_options;
}
hci_le_set_phy_cp0;

typedef __PACKED_STRUCT
{
	uint8_t rx_frequency;
	uint8_t phy;
	uint8_t modulation_index;
}
hci_le_receiver_test_v2_cp0;

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
	uint8_t advertising_handle;
	uint8_t random_address[6];
}
hci_le_set_advertising_set_random_address_cp0;

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
	uint8_t advertising_data[HCI_COMMAND_MAX_PARAM_LEN - 4];
}
hci_le_set_extended_advertising_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t advertising_handle;
	uint8_t operation;
	uint8_t fragment_preference;
	uint8_t scan_response_data_length;
	uint8_t scan_response_data[HCI_COMMAND_MAX_PARAM_LEN - 4];
}
hci_le_set_extended_scan_response_data_cp0;

typedef __PACKED_STRUCT
{
	uint8_t enable;
	uint8_t num_sets;
	adv_set_t adv_set[(HCI_COMMAND_MAX_PARAM_LEN - 2) / sizeof(adv_set_t)];
}
hci_le_set_extended_advertising_enable_cp0;

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
	uint8_t own_address_type;
	uint8_t scanning_filter_policy;
	uint8_t scanning_phys;
	scan_param_phy_t scan_param_phy[2];
}
hci_le_set_extended_scan_parameters_cp0;

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
	uint8_t peer_identity_address_type;
	uint8_t peer_identity_address[6];
	uint8_t privacy_mode;
}
hci_le_set_privacy_mode_cp0;

typedef __PACKED_STRUCT
{
	uint8_t remote_p256_public_key[64];
	uint8_t key_type;
}
hci_le_generate_dhkey_v2_cp0;

typedef __PACKED_STRUCT
{
	uint16_t rpa_timeout_min;
	uint16_t rpa_timeout_max;
}
hci_le_set_resolvable_private_address_timeout_v2_cp0;

/* API Declarations */
enum ble_status hci_le_set_event_mask(const uint8_t *le_event_mask);
enum ble_status hci_le_read_buffer_size(uint16_t *hc_le_acl_data_packet_length,
										uint8_t *hc_total_num_le_acl_data_packets);
enum ble_status hci_le_read_local_supported_features_page_0(uint8_t *le_features);
enum ble_status hci_le_set_random_address(const uint8_t *random_address);
enum ble_status hci_le_set_advertising_parameters(
	uint16_t advertising_interval_min, uint16_t advertising_interval_max, uint8_t advertising_type,
	uint8_t own_address_type, uint8_t peer_address_type, const uint8_t *peer_address,
	uint8_t advertising_channel_map, uint8_t advertising_filter_policy);
enum ble_status hci_le_read_advertising_physical_channel_tx_power(uint8_t *transmit_power_level);
enum ble_status hci_le_set_advertising_data(uint8_t advertising_data_length,
											const uint8_t *advertising_data);
enum ble_status hci_le_set_scan_response_data(uint8_t scan_response_data_length,
											  const uint8_t *scan_response_data);
enum ble_status hci_le_set_advertising_enable(uint8_t advertising_enable);
enum ble_status hci_le_set_scan_parameters(uint8_t le_scan_type, uint16_t le_scan_interval,
										   uint16_t le_scan_window, uint8_t own_address_type,
										   uint8_t scanning_filter_policy);
enum ble_status hci_le_set_scan_enable(uint8_t le_scan_enable, uint8_t filter_duplicates);
enum ble_status hci_le_create_connection(uint16_t le_scan_interval, uint16_t le_scan_window,
										 uint8_t initiator_filter_policy, uint8_t peer_address_type,
										 const uint8_t *peer_address, uint8_t own_address_type,
										 uint16_t conn_interval_min, uint16_t conn_interval_max,
										 uint16_t conn_latency, uint16_t supervision_timeout,
										 uint16_t minimum_ce_length, uint16_t maximum_ce_length);
enum ble_status hci_le_create_connection_cancel(void);
enum ble_status hci_le_read_filter_accept_list_size(uint8_t *filter_accept_list_size);
enum ble_status hci_le_clear_filter_accept_list(void);
enum ble_status hci_le_add_device_to_filter_accept_list(uint8_t address_type,
														const uint8_t *address);
enum ble_status hci_le_remove_device_from_filter_accept_list(uint8_t address_type,
															 const uint8_t *address);
enum ble_status hci_le_connection_update(uint16_t connection_handle, uint16_t conn_interval_min,
										 uint16_t conn_interval_max, uint16_t conn_latency,
										 uint16_t supervision_timeout, uint16_t minimum_ce_length,
										 uint16_t maximum_ce_length);
enum ble_status hci_le_set_host_channel_classification(const uint8_t *le_channel_map);
enum ble_status hci_le_read_channel_map(uint16_t connection_handle, uint8_t *le_channel_map);
enum ble_status hci_le_read_remote_features_page_0(uint16_t connection_handle);
enum ble_status hci_le_encrypt(const uint8_t *key, const uint8_t *plaintext_data,
							   uint8_t *encrypted_data);
enum ble_status hci_le_rand(uint8_t *random_number);
enum ble_status hci_le_enable_encryption(uint16_t connection_handle, const uint8_t *random_number,
										 uint16_t encrypted_diversifier,
										 const uint8_t *long_term_key);
enum ble_status hci_le_long_term_key_request_reply(uint16_t connection_handle,
												   const uint8_t *long_term_key);
enum ble_status hci_le_long_term_key_request_negative_reply(uint16_t connection_handle);
enum ble_status hci_le_read_supported_states(uint8_t *le_states);
enum ble_status hci_le_receiver_test(uint8_t rx_frequency);
enum ble_status hci_le_transmitter_test(uint8_t tx_frequency, uint8_t length_of_test_data,
										uint8_t packet_payload);
enum ble_status hci_le_test_end(uint16_t *number_of_packets);
enum ble_status hci_le_set_data_length(uint16_t connection_handle, uint16_t tx_octets,
									   uint16_t tx_time);
enum ble_status hci_le_read_suggested_default_data_length(uint16_t *suggested_max_tx_octets,
														  uint16_t *suggested_max_tx_time);
enum ble_status hci_le_write_suggested_default_data_length(uint16_t suggested_max_tx_octets,
														   uint16_t suggested_max_tx_time);
enum ble_status hci_le_read_local_p256_public_key(void);
enum ble_status hci_le_generate_dhkey(const uint8_t *remote_p256_public_key);
enum ble_status hci_le_add_device_to_resolving_list(uint8_t peer_identity_address_type,
													const uint8_t *peer_identity_address,
													const uint8_t *peer_irk,
													const uint8_t *local_irk);
enum ble_status hci_le_remove_device_from_resolving_list(uint8_t peer_identity_address_type,
														 const uint8_t *peer_identity_address);
enum ble_status hci_le_clear_resolving_list(void);
enum ble_status hci_le_read_resolving_list_size(uint8_t *resolving_list_size);
enum ble_status hci_le_read_peer_resolvable_address(uint8_t peer_identity_address_type,
													const uint8_t *peer_identity_address,
													uint8_t *peer_resolvable_address);
enum ble_status hci_le_read_local_resolvable_address(uint8_t peer_identity_address_type,
													 const uint8_t *peer_identity_address,
													 uint8_t *local_resolvable_address);
enum ble_status hci_le_set_address_resolution_enable(uint8_t address_resolution_enable);
enum ble_status hci_le_set_resolvable_private_address_timeout(uint16_t rpa_timeout);
enum ble_status hci_le_read_maximum_data_length(uint16_t *supported_max_tx_octets,
												uint16_t *supported_max_tx_time,
												uint16_t *supported_max_rx_octets,
												uint16_t *supported_max_rx_time);
enum ble_status hci_le_read_phy(uint16_t connection_handle, uint8_t *tx_phy, uint8_t *rx_phy);
enum ble_status hci_le_set_default_phy(uint8_t all_phys, uint8_t tx_phys, uint8_t rx_phys);
enum ble_status hci_le_set_phy(uint16_t connection_handle, uint8_t all_phys, uint8_t tx_phys,
							   uint8_t rx_phys, uint16_t phy_options);
enum ble_status hci_le_receiver_test_v2(uint8_t rx_frequency, uint8_t phy,
										uint8_t modulation_index);
enum ble_status hci_le_transmitter_test_v2(uint8_t tx_frequency, uint8_t length_of_test_data,
										   uint8_t packet_payload, uint8_t phy);
enum ble_status hci_le_set_advertising_set_random_address(uint8_t advertising_handle,
														  const uint8_t *random_address);
enum ble_status hci_le_set_extended_advertising_parameters(
	uint8_t advertising_handle, uint16_t adv_event_properties,
	const uint8_t *primary_adv_interval_min, const uint8_t *primary_adv_interval_max,
	uint8_t primary_adv_channel_map, uint8_t own_address_type, uint8_t peer_address_type,
	const uint8_t *peer_address, uint8_t adv_filter_policy, uint8_t adv_tx_power,
	uint8_t primary_adv_phy, uint8_t secondary_adv_max_skip, uint8_t secondary_adv_phy,
	uint8_t adv_sid, uint8_t scan_req_notification_enable, uint8_t *selected_tx_power);
enum ble_status hci_le_set_extended_advertising_data(uint8_t advertising_handle, uint8_t operation,
													 uint8_t fragment_preference,
													 uint8_t advertising_data_length,
													 const uint8_t *advertising_data);
enum ble_status hci_le_set_extended_scan_response_data(uint8_t advertising_handle,
													   uint8_t operation,
													   uint8_t fragment_preference,
													   uint8_t scan_response_data_length,
													   const uint8_t *scan_response_data);
enum ble_status hci_le_set_extended_advertising_enable(uint8_t enable, uint8_t num_sets,
													   const adv_set_t *adv_set);
enum ble_status hci_le_read_maximum_advertising_data_length(uint16_t *max_advertising_data_length);
enum ble_status
hci_le_read_number_of_supported_advertising_sets(uint8_t *num_supported_advertising_sets);
enum ble_status hci_le_remove_advertising_set(uint8_t advertising_handle);
enum ble_status hci_le_clear_advertising_sets(void);
enum ble_status hci_le_set_extended_scan_parameters(uint8_t own_address_type,
													uint8_t scanning_filter_policy,
													uint8_t scanning_phys,
													const scan_param_phy_t *scan_param_phy);
enum ble_status hci_le_set_extended_scan_enable(uint8_t enable, uint8_t filter_duplicates,
												uint16_t duration, uint16_t period);
enum ble_status
hci_le_extended_create_connection(uint8_t initiator_filter_policy, uint8_t own_address_type,
								  uint8_t peer_address_type, const uint8_t *peer_address,
								  uint8_t initiating_phys, const init_param_phy_t *init_param_phy);
enum ble_status hci_le_read_transmit_power(uint8_t *min_tx_power, uint8_t *max_tx_power);
enum ble_status hci_le_read_rf_path_compensation(uint16_t *rf_tx_path_compensation,
												 uint16_t *rf_rx_path_compensation);
enum ble_status hci_le_write_rf_path_compensation(uint16_t rf_tx_path_compensation,
												  uint16_t rf_rx_path_compensation);
enum ble_status hci_le_set_privacy_mode(uint8_t peer_identity_address_type,
										const uint8_t *peer_identity_address, uint8_t privacy_mode);
enum ble_status hci_le_generate_dhkey_v2(const uint8_t *remote_p256_public_key, uint8_t key_type);
enum ble_status hci_le_set_resolvable_private_address_timeout_v2(uint16_t rpa_timeout_min,
																 uint16_t rpa_timeout_max);

#endif /* BLE_HCI_LE_H */
