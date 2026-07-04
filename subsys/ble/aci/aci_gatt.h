/*****************************************************************************
 * @file    aci_gatt.h
 * @brief   Refactored STM32WB BLE GATT ACI Commands & Data Structures
 *****************************************************************************
 */

#ifndef BLE_ACI_GATT_H
#define BLE_ACI_GATT_H

#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"
#include <stdint.h>

/* Injected relevant structural types from ble_types.h */
typedef __PACKED_UNION
{
    uint16_t service_uuid_16;
    uint8_t service_uuid_128[16];
}
service_uuid_t;

typedef __PACKED_UNION
{
    uint16_t include_uuid_16;
    uint8_t include_uuid_128[16];
}
include_uuid_t;

typedef __PACKED_UNION
{
    uint16_t char_uuid_16;
    uint8_t char_uuid_128[16];
}
char_uuid_t;

typedef __PACKED_UNION
{
    uint16_t char_uuid_16;
    uint8_t char_uuid_128[16];
}
char_desc_uuid_t;

typedef __PACKED_UNION
{
    uint16_t uuid_16;
    uint8_t uuid_128[16];
}
uuid_t;

typedef __PACKED_STRUCT
{
    uint16_t handle;
}
handle_entry_t;

/* Pulled Serialization Command and Return Parameter Structs */
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
    uint8_t char_desc_value[HCI_COMMAND_MAX_PARAM_LEN - 12];
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
    uint8_t char_value[HCI_COMMAND_MAX_PARAM_LEN - 6];
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
    uint8_t attribute_val[HCI_COMMAND_MAX_PARAM_LEN - 9];
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
    uint8_t attribute_val[HCI_COMMAND_MAX_PARAM_LEN - 7];
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
    handle_entry_t handle_entry[(HCI_COMMAND_MAX_PARAM_LEN - 3) / sizeof(handle_entry_t)];
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
    uint8_t attribute_val[HCI_COMMAND_MAX_PARAM_LEN - 5];
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
    uint8_t attribute_val[HCI_COMMAND_MAX_PARAM_LEN - 7];
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
    uint8_t attribute_val[HCI_COMMAND_MAX_PARAM_LEN - 7];
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
    uint8_t attribute_val[HCI_COMMAND_MAX_PARAM_LEN - 5];
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
    uint8_t attribute_val[HCI_COMMAND_MAX_PARAM_LEN - 5];
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
    uint8_t attribute_val[HCI_COMMAND_MAX_PARAM_LEN - 7];
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
    uint8_t char_desc_value[HCI_COMMAND_MAX_PARAM_LEN - 9];
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
    uint8_t value[(HCI_EVENT_MAX_PARAM_LEN - 3) - 5];
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
    uint8_t value[HCI_COMMAND_MAX_PARAM_LEN - 12];
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
    handle_entry_t handle_entry[(HCI_COMMAND_MAX_PARAM_LEN - 3) / sizeof(handle_entry_t)];
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
    handle_entry_t handle_entry[(HCI_COMMAND_MAX_PARAM_LEN - 3) / sizeof(handle_entry_t)];
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

/* API Declarations */
ble_status_t aci_gatt_init(void);
ble_status_t aci_gatt_add_service(uint8_t service_uuid_type,
                                  const service_uuid_t *service_uuid,
                                  uint8_t service_type,
                                  uint8_t max_attribute_records,
                                  uint16_t *service_handle);
ble_status_t aci_gatt_include_service(uint16_t service_handle,
                                      uint16_t include_start_handle,
                                      uint16_t include_end_handle,
                                      uint8_t include_uuid_type,
                                      const include_uuid_t *include_uuid,
                                      uint16_t *include_handle);
ble_status_t aci_gatt_add_char(uint16_t service_handle,
                               uint8_t char_uuid_type,
                               const char_uuid_t *char_uuid,
                               uint16_t char_value_length,
                               uint8_t char_properties,
                               uint8_t security_permissions,
                               uint8_t gatt_evt_mask,
                               uint8_t enc_key_size,
                               uint8_t is_variable,
                               uint16_t *char_handle);
ble_status_t aci_gatt_add_char_desc(uint16_t service_handle,
                                    uint16_t char_handle,
                                    uint8_t char_desc_uuid_type,
                                    const char_desc_uuid_t *char_desc_uuid,
                                    uint8_t char_desc_value_max_len,
                                    uint8_t char_desc_value_length,
                                    const uint8_t *char_desc_value,
                                    uint8_t security_permissions,
                                    uint8_t access_permissions,
                                    uint8_t gatt_evt_mask,
                                    uint8_t enc_key_size,
                                    uint8_t is_variable,
                                    uint16_t *char_desc_handle);
ble_status_t aci_gatt_update_char_value(uint16_t service_handle,
                                        uint16_t char_handle,
                                        uint8_t val_offset,
                                        uint8_t char_value_length,
                                        const uint8_t *char_value);
ble_status_t aci_gatt_del_char(uint16_t serv_handle, uint16_t char_handle);
ble_status_t aci_gatt_del_service(uint16_t serv_handle);
ble_status_t aci_gatt_del_include_service(uint16_t serv_handle, uint16_t include_handle);
ble_status_t aci_gatt_set_event_mask(uint32_t gatt_evt_mask);
ble_status_t aci_gatt_exchange_config(uint16_t connection_handle);
ble_status_t aci_att_find_info_req(uint16_t connection_handle, uint16_t start_handle, uint16_t end_handle);
ble_status_t aci_att_find_by_type_value_req(uint16_t connection_handle,
                                            uint16_t start_handle,
                                            uint16_t end_handle,
                                            uint16_t uuid,
                                            uint8_t attribute_val_length,
                                            const uint8_t *attribute_val);
ble_status_t aci_att_read_by_type_req(uint16_t connection_handle,
                                      uint16_t start_handle,
                                      uint16_t end_handle,
                                      uint8_t uuid_type,
                                      const uuid_t *uuid);
ble_status_t aci_att_read_by_group_type_req(uint16_t connection_handle,
                                            uint16_t start_handle,
                                            uint16_t end_handle,
                                            uint8_t uuid_type,
                                            const uuid_t *uuid);
ble_status_t aci_att_prepare_write_req(uint16_t connection_handle,
                                       uint16_t attr_handle,
                                       uint16_t val_offset,
                                       uint8_t attribute_val_length,
                                       const uint8_t *attribute_val);
ble_status_t aci_att_execute_write_req(uint16_t connection_handle, uint8_t execute);
ble_status_t aci_gatt_disc_all_primary_services(uint16_t connection_handle);
ble_status_t
aci_gatt_disc_primary_service_by_uuid(uint16_t connection_handle, uint8_t uuid_type, const uuid_t *uuid);
ble_status_t
aci_gatt_find_included_services(uint16_t connection_handle, uint16_t start_handle, uint16_t end_handle);
ble_status_t
aci_gatt_disc_all_char_of_service(uint16_t connection_handle, uint16_t start_handle, uint16_t end_handle);
ble_status_t aci_gatt_disc_char_by_uuid(uint16_t connection_handle,
                                        uint16_t start_handle,
                                        uint16_t end_handle,
                                        uint8_t uuid_type,
                                        const uuid_t *uuid);
ble_status_t
aci_gatt_disc_all_char_desc(uint16_t connection_handle, uint16_t char_handle, uint16_t end_handle);
ble_status_t aci_gatt_read_char_value(uint16_t connection_handle, uint16_t attr_handle);
ble_status_t aci_gatt_read_using_char_uuid(uint16_t connection_handle,
                                           uint16_t start_handle,
                                           uint16_t end_handle,
                                           uint8_t uuid_type,
                                           const uuid_t *uuid);
ble_status_t
aci_gatt_read_long_char_value(uint16_t connection_handle, uint16_t attr_handle, uint16_t val_offset);
ble_status_t aci_gatt_read_multiple_char_value(uint16_t connection_handle,
                                               uint8_t number_of_handles,
                                               const handle_entry_t *handle_entry);
ble_status_t aci_gatt_write_char_value(uint16_t connection_handle,
                                       uint16_t attr_handle,
                                       uint8_t attribute_val_length,
                                       const uint8_t *attribute_val);
ble_status_t aci_gatt_write_long_char_value(uint16_t connection_handle,
                                            uint16_t attr_handle,
                                            uint16_t val_offset,
                                            uint8_t attribute_val_length,
                                            const uint8_t *attribute_val);
ble_status_t aci_gatt_write_char_reliable(uint16_t connection_handle,
                                          uint16_t attr_handle,
                                          uint16_t val_offset,
                                          uint8_t attribute_val_length,
                                          const uint8_t *attribute_val);
ble_status_t aci_gatt_write_without_resp(uint16_t connection_handle,
                                         uint16_t attr_handle,
                                         uint8_t attribute_val_length,
                                         const uint8_t *attribute_val);
ble_status_t aci_gatt_signed_write_without_resp(uint16_t connection_handle,
                                                uint16_t attr_handle,
                                                uint8_t attribute_val_length,
                                                const uint8_t *attribute_val);
ble_status_t aci_gatt_confirm_indication(uint16_t connection_handle);
ble_status_t aci_gatt_permit_write(uint16_t connection_handle,
                                   uint16_t attr_handle,
                                   uint8_t write_status,
                                   uint8_t error_code,
                                   uint8_t attribute_val_length,
                                   const uint8_t *attribute_val);
ble_status_t aci_gatt_permit_read(uint16_t connection_handle,
                                  uint8_t read_status,
                                  uint8_t error_code,
                                  uint16_t attr_handle);
ble_status_t
aci_gatt_set_security_permission(uint16_t serv_handle, uint16_t attr_handle, uint8_t security_permissions);
ble_status_t aci_gatt_set_desc_value(uint16_t serv_handle,
                                     uint16_t char_handle,
                                     uint16_t char_desc_handle,
                                     uint16_t val_offset,
                                     uint8_t char_desc_value_length,
                                     const uint8_t *char_desc_value);
ble_status_t aci_gatt_read_handle_value(uint16_t attr_handle,
                                        uint16_t offset,
                                        uint16_t value_length_requested,
                                        uint16_t *length,
                                        uint16_t *value_length,
                                        uint8_t *value);
ble_status_t aci_gatt_update_char_value_ext(uint16_t conn_handle_to_notify,
                                            uint16_t service_handle,
                                            uint16_t char_handle,
                                            uint8_t update_type,
                                            uint16_t char_length,
                                            uint16_t value_offset,
                                            uint8_t value_length,
                                            const uint8_t *value);
ble_status_t
aci_gatt_set_access_permission(uint16_t serv_handle, uint16_t attr_handle, uint8_t access_permissions);
ble_status_t aci_gatt_store_db(void);
ble_status_t aci_gatt_send_mult_notification(uint16_t connection_handle,
                                             uint8_t number_of_handles,
                                             const handle_entry_t *handle_entry);
ble_status_t aci_gatt_read_multiple_var_char_value(uint16_t connection_handle,
                                                   uint8_t number_of_handles,
                                                   const handle_entry_t *handle_entry);
ble_status_t aci_gatt_write_without_resp_ext(uint16_t connection_handle,
                                             uint16_t attr_handle,
                                             uint8_t signed_mode,
                                             uint16_t data_length,
                                             uint32_t data_pointer);
ble_status_t aci_gatt_write_with_resp_ext(uint16_t connection_handle,
                                          uint16_t attr_handle,
                                          uint8_t write_mode,
                                          uint16_t val_offset,
                                          uint16_t data_length,
                                          uint32_t data_pointer);

#endif /* BLE_ACI_GATT_H */
