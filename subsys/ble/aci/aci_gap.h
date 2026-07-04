#ifndef BLE_ACI_GAP_H
#define BLE_ACI_GAP_H

#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"
#include "subsys/ble/hci/hci_le.h"
#include <stdint.h>

/* GAP Roles
 */
#define GAP_PERIPHERAL_ROLE 0x01U
#define GAP_BROADCASTER_ROLE 0x02U
#define GAP_CENTRAL_ROLE 0x04U
#define GAP_OBSERVER_ROLE 0x08U

/* Pulled structural primitives used inside GAP commands */
typedef __PACKED_STRUCT
{
    uint8_t peer_address_type;
    uint8_t peer_address[6];
}
peer_entry_t;

typedef __PACKED_STRUCT
{
    uint8_t address_type;
    uint8_t address[6];
}
bonded_device_entry_t;

typedef __PACKED_STRUCT
{
    uint8_t address_type;
    uint8_t address[6];
}
list_entry_t;

/* Pulled Serialization Command and Return Parameter Structs */
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
    uint8_t local_name[HCI_COMMAND_MAX_PARAM_LEN - 13];
}
aci_gap_set_limited_discoverable_cp0;

typedef __PACKED_STRUCT
{
    uint8_t service_uuid_length;
    uint8_t service_uuid_list[HCI_COMMAND_MAX_PARAM_LEN - 13];
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
    uint8_t local_name[HCI_COMMAND_MAX_PARAM_LEN - 13];
}
aci_gap_set_discoverable_cp0;

typedef __PACKED_STRUCT
{
    uint8_t service_uuid_length;
    uint8_t service_uuid_list[HCI_COMMAND_MAX_PARAM_LEN - 13];
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
    uint8_t adv_data_len;
    uint8_t adv_data[HCI_COMMAND_MAX_PARAM_LEN - 1];
}
aci_gap_update_adv_data_cp0;

typedef __PACKED_STRUCT
{
    uint8_t status;
}
aci_gap_update_adv_data_rp0;

typedef __PACKED_STRUCT
{
    uint8_t ad_type;
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
    peer_entry_t peer_entry[(HCI_COMMAND_MAX_PARAM_LEN - 18) / sizeof(peer_entry_t)];
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
    peer_entry_t peer_entry[(HCI_COMMAND_MAX_PARAM_LEN - 9) / sizeof(peer_entry_t)];
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
    uint8_t adv_data[HCI_COMMAND_MAX_PARAM_LEN - 8];
}
aci_gap_set_broadcast_mode_cp0;

typedef __PACKED_STRUCT
{
    uint8_t num_of_peer_entries;
    peer_entry_t peer_entry[(HCI_COMMAND_MAX_PARAM_LEN - 8) / sizeof(peer_entry_t)];
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
        bonded_device_entry[((HCI_EVENT_MAX_PARAM_LEN - 3) - 2) / sizeof(bonded_device_entry_t)];
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
    list_entry_t list_entry[(HCI_COMMAND_MAX_PARAM_LEN - 2) / sizeof(list_entry_t)];
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
    uint8_t adv_data[HCI_COMMAND_MAX_PARAM_LEN - 1];
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
    adv_set_t adv_set[(HCI_COMMAND_MAX_PARAM_LEN - 2) / sizeof(adv_set_t)];
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
    uint8_t advertising_data[HCI_COMMAND_MAX_PARAM_LEN - 4];
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
    uint8_t scan_response_data[HCI_COMMAND_MAX_PARAM_LEN - 4];
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

/* API Declarations */
ble_status_t aci_gap_set_non_discoverable(void);
ble_status_t aci_gap_set_limited_discoverable(uint8_t advertising_type,
                                              uint16_t advertising_interval_min,
                                              uint16_t advertising_interval_max,
                                              uint8_t own_address_type,
                                              uint8_t advertising_filter_policy,
                                              uint8_t local_name_length,
                                              const uint8_t *local_name,
                                              uint8_t service_uuid_length,
                                              const uint8_t *service_uuid_list,
                                              uint16_t conn_interval_min,
                                              uint16_t conn_interval_max);
ble_status_t aci_gap_set_discoverable(uint8_t advertising_type,
                                      uint16_t advertising_interval_min,
                                      uint16_t advertising_interval_max,
                                      uint8_t own_address_type,
                                      uint8_t advertising_filter_policy,
                                      uint8_t local_name_length,
                                      const uint8_t *local_name,
                                      uint8_t service_uuid_length,
                                      const uint8_t *service_uuid_list,
                                      uint16_t conn_interval_min,
                                      uint16_t conn_interval_max);
ble_status_t aci_gap_set_direct_connectable(uint8_t own_address_type,
                                            uint8_t directed_advertising_type,
                                            uint8_t direct_address_type,
                                            const uint8_t *direct_address,
                                            uint16_t advertising_interval_min,
                                            uint16_t advertising_interval_max);
ble_status_t aci_gap_set_io_capability(uint8_t io_capability);
ble_status_t aci_gap_set_authentication_requirement(uint8_t bonding_mode,
                                                    uint8_t mitm_mode,
                                                    uint8_t sc_support,
                                                    uint8_t keypress_notification_support,
                                                    uint8_t min_encryption_key_size,
                                                    uint8_t max_encryption_key_size,
                                                    uint8_t use_fixed_pin,
                                                    uint32_t fixed_pin,
                                                    uint8_t identity_address_type);
ble_status_t aci_gap_set_authorization_requirement(uint16_t connection_handle, uint8_t authorization_enable);
ble_status_t aci_gap_pass_key_resp(uint16_t connection_handle, uint32_t pass_key);
ble_status_t aci_gap_authorization_resp(uint16_t connection_handle, uint8_t authorize);
ble_status_t aci_gap_init(uint8_t role,
                          uint8_t privacy_enabled,
                          uint8_t device_name_char_len,
                          uint16_t *service_handle,
                          uint16_t *dev_name_char_handle,
                          uint16_t *appearance_char_handle);
ble_status_t aci_gap_set_non_connectable(uint8_t advertising_event_type, uint8_t own_address_type);
ble_status_t aci_gap_set_undirected_connectable(uint16_t advertising_interval_min,
                                                uint16_t advertising_interval_max,
                                                uint8_t own_address_type,
                                                uint8_t adv_filter_policy);
ble_status_t aci_gap_peripheral_security_req(uint16_t connection_handle);
ble_status_t aci_gap_update_adv_data(uint8_t adv_data_len, const uint8_t *adv_data);
ble_status_t aci_gap_delete_ad_type(uint8_t ad_type);
ble_status_t
aci_gap_get_security_level(uint16_t connection_handle, uint8_t *security_mode, uint8_t *security_level);
ble_status_t aci_gap_set_event_mask(uint16_t gap_evt_mask);
ble_status_t aci_gap_configure_filter_accept_list(void);
ble_status_t aci_gap_terminate(uint16_t connection_handle, uint8_t reason);
ble_status_t aci_gap_clear_security_db(void);
ble_status_t aci_gap_allow_rebond(uint16_t connection_handle);
ble_status_t aci_gap_start_limited_discovery_proc(uint16_t le_scan_interval,
                                                  uint16_t le_scan_window,
                                                  uint8_t own_address_type,
                                                  uint8_t filter_duplicates);
ble_status_t aci_gap_start_general_discovery_proc(uint16_t le_scan_interval,
                                                  uint16_t le_scan_window,
                                                  uint8_t own_address_type,
                                                  uint8_t filter_duplicates);
ble_status_t aci_gap_start_auto_connection_establish_proc(uint16_t le_scan_interval,
                                                          uint16_t le_scan_window,
                                                          uint8_t own_address_type,
                                                          uint16_t conn_interval_min,
                                                          uint16_t conn_interval_max,
                                                          uint16_t conn_latency,
                                                          uint16_t supervision_timeout,
                                                          uint16_t minimum_ce_length,
                                                          uint16_t maximum_ce_length,
                                                          uint8_t num_of_peer_entries,
                                                          const peer_entry_t *peer_entry);
ble_status_t aci_gap_start_general_connection_establish_proc(uint8_t le_scan_type,
                                                             uint16_t le_scan_interval,
                                                             uint16_t le_scan_window,
                                                             uint8_t own_address_type,
                                                             uint8_t scanning_filter_policy,
                                                             uint8_t filter_duplicates);
ble_status_t aci_gap_start_selective_connection_establish_proc(uint8_t le_scan_type,
                                                               uint16_t le_scan_interval,
                                                               uint16_t le_scan_window,
                                                               uint8_t own_address_type,
                                                               uint8_t scanning_filter_policy,
                                                               uint8_t filter_duplicates,
                                                               uint8_t num_of_peer_entries,
                                                               const peer_entry_t *peer_entry);
ble_status_t aci_gap_create_connection(uint16_t le_scan_interval,
                                       uint16_t le_scan_window,
                                       uint8_t peer_address_type,
                                       const uint8_t *peer_address,
                                       uint8_t own_address_type,
                                       uint16_t conn_interval_min,
                                       uint16_t conn_interval_max,
                                       uint16_t conn_latency,
                                       uint16_t supervision_timeout,
                                       uint16_t minimum_ce_length,
                                       uint16_t maximum_ce_length);
ble_status_t aci_gap_terminate_gap_proc(uint8_t procedure_code);
ble_status_t aci_gap_start_connection_update(uint16_t connection_handle,
                                             uint16_t conn_interval_min,
                                             uint16_t conn_interval_max,
                                             uint16_t conn_latency,
                                             uint16_t supervision_timeout,
                                             uint16_t minimum_ce_length,
                                             uint16_t maximum_ce_length);
ble_status_t aci_gap_send_pairing_req(uint16_t connection_handle, uint8_t force_rebond);
ble_status_t aci_gap_set_broadcast_mode(uint16_t advertising_interval_min,
                                        uint16_t advertising_interval_max,
                                        uint8_t advertising_type,
                                        uint8_t own_address_type,
                                        uint8_t adv_data_length,
                                        const uint8_t *adv_data,
                                        uint8_t num_of_peer_entries,
                                        const peer_entry_t *peer_entry);
ble_status_t aci_gap_start_observation_proc(uint16_t le_scan_interval,
                                            uint16_t le_scan_window,
                                            uint8_t le_scan_type,
                                            uint8_t own_address_type,
                                            uint8_t filter_duplicates,
                                            uint8_t scanning_filter_policy);
ble_status_t aci_gap_get_bonded_devices(uint8_t *num_of_addresses,
                                        bonded_device_entry_t *bonded_device_entry);
ble_status_t aci_gap_check_bonded_device(uint8_t peer_address_type,
                                         const uint8_t *peer_address,
                                         uint8_t *id_address_type,
                                         uint8_t *id_address);
ble_status_t aci_gap_numeric_comparison_value_confirm_yesno(uint16_t connection_handle,
                                                            uint8_t confirm_yes_no);
ble_status_t aci_gap_passkey_input(uint16_t connection_handle, uint8_t input_type);
ble_status_t aci_gap_get_oob_data(uint8_t oob_data_type,
                                  uint8_t *address_type,
                                  uint8_t *address,
                                  uint8_t *oob_data_len,
                                  uint8_t *oob_data);
ble_status_t aci_gap_set_oob_data(uint8_t device_type,
                                  uint8_t address_type,
                                  const uint8_t *address,
                                  uint8_t oob_data_type,
                                  uint8_t oob_data_len,
                                  const uint8_t *oob_data);
ble_status_t aci_gap_remove_bonded_device(uint8_t peer_identity_address_type,
                                          const uint8_t *peer_identity_address);
ble_status_t
aci_gap_add_devices_to_list(uint8_t num_of_list_entries, const list_entry_t *list_entry, uint8_t mode);
ble_status_t aci_gap_pairing_request_reply(uint16_t connection_handle, uint8_t accept);
ble_status_t aci_gap_additional_beacon_start(uint16_t adv_interval_min,
                                             uint16_t adv_interval_max,
                                             uint8_t adv_channel_map,
                                             uint8_t own_address_type,
                                             const uint8_t *own_address,
                                             uint8_t pa_level);
ble_status_t aci_gap_additional_beacon_stop(void);
ble_status_t aci_gap_additional_beacon_set_data(uint8_t adv_data_length, const uint8_t *adv_data);
ble_status_t aci_gap_adv_set_configuration(uint8_t adv_mode,
                                           uint8_t advertising_handle,
                                           uint16_t adv_event_properties,
                                           uint32_t primary_adv_interval_min,
                                           uint32_t primary_adv_interval_max,
                                           uint8_t primary_adv_channel_map,
                                           uint8_t own_address_type,
                                           uint8_t peer_address_type,
                                           const uint8_t *peer_address,
                                           uint8_t adv_filter_policy,
                                           uint8_t adv_tx_power,
                                           uint8_t secondary_adv_max_skip,
                                           uint8_t secondary_adv_phy,
                                           uint8_t adv_sid,
                                           uint8_t scan_req_notification_enable);
ble_status_t aci_gap_adv_set_enable(uint8_t enable, uint8_t num_sets, const adv_set_t *adv_set);
ble_status_t aci_gap_adv_set_adv_data(uint8_t advertising_handle,
                                      uint8_t operation,
                                      uint8_t fragment_preference,
                                      uint8_t advertising_data_length,
                                      const uint8_t *advertising_data);
ble_status_t aci_gap_adv_set_scan_resp_data(uint8_t advertising_handle,
                                            uint8_t operation,
                                            uint8_t fragment_preference,
                                            uint8_t scan_response_data_length,
                                            const uint8_t *scan_response_data);
ble_status_t aci_gap_adv_remove_set(uint8_t advertising_handle);
ble_status_t aci_gap_adv_clear_sets(void);
ble_status_t aci_gap_adv_set_random_address(uint8_t advertising_handle, const uint8_t *random_address);
ble_status_t aci_gap_ext_start_scan(uint8_t scan_mode,
                                    uint8_t procedure,
                                    uint8_t own_address_type,
                                    uint8_t filter_duplicates,
                                    uint16_t duration,
                                    uint16_t period,
                                    uint8_t scanning_filter_policy,
                                    uint8_t scanning_phys,
                                    const scan_param_phy_t *scan_param_phy);
ble_status_t aci_gap_ext_create_connection(uint8_t initiating_mode,
                                           uint8_t procedure,
                                           uint8_t own_address_type,
                                           uint8_t peer_address_type,
                                           const uint8_t *peer_address,
                                           uint8_t advertising_handle,
                                           uint8_t subevent,
                                           uint8_t initiator_filter_policy,
                                           uint8_t initiating_phys,
                                           const init_param_phy_t *init_param_phy);

#endif /* BLE_BLE_GAP_ACI_H */
