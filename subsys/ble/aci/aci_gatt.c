#include "aci_gatt.h"
#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"
#include <string.h>

enum ble_status aci_gatt_init(void)
{
	struct hci_request rq;
	enum ble_status status = 0;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x101;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_add_service(uint8_t service_uuid_type, const service_uuid_t *service_uuid,
									 uint8_t service_type, uint8_t max_attribute_records,
									 uint16_t *service_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_add_service_cp0 *cp0 = (aci_gatt_add_service_cp0 *)(cmd_buffer);
	aci_gatt_add_service_cp1 *cp1 =
		(aci_gatt_add_service_cp1 *)(cmd_buffer + 1 +
									 (service_uuid_type == 1 ? 2 :
															   (service_uuid_type == 2 ? 16 : 0)));
	aci_gatt_add_service_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	cp0->service_uuid_type = service_uuid_type;
	index_input += 1;
	{
		uint8_t size;
		switch (service_uuid_type) {
		case 1:
			size = 2;
			break;
		case 2:
			size = 16;
			break;
		default:
			return BLE_STATUS_ERROR;
		}
		memcpy((void *)&cp0->service_uuid, (const void *)service_uuid, size);
		index_input += size;
		{
			cp1->service_type = service_type;
		}
		index_input += 1;
		{
			cp1->max_attribute_records = max_attribute_records;
		}
		index_input += 1;
	}
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x102;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	if (resp.status)
		return resp.status;
	*service_handle = resp.service_handle;
	return BLE_STATUS_SUCCESS;
}

enum ble_status aci_gatt_include_service(uint16_t service_handle, uint16_t include_start_handle,
										 uint16_t include_end_handle, uint8_t include_uuid_type,
										 const include_uuid_t *include_uuid,
										 uint16_t *include_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_include_service_cp0 *cp0 = (aci_gatt_include_service_cp0 *)(cmd_buffer);
	aci_gatt_include_service_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	int uuid_size = (include_uuid_type == 2) ? 16 : 2;
	cp0->service_handle = service_handle;
	index_input += 2;
	cp0->include_start_handle = include_start_handle;
	index_input += 2;
	cp0->include_end_handle = include_end_handle;
	index_input += 2;
	cp0->include_uuid_type = include_uuid_type;
	index_input += 1;
	memcpy((void *)&cp0->include_uuid, (const void *)include_uuid, uuid_size);
	index_input += uuid_size;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x103;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	if (resp.status)
		return resp.status;
	*include_handle = resp.include_handle;
	return BLE_STATUS_SUCCESS;
}

enum ble_status aci_gatt_add_char(uint16_t service_handle, uint8_t char_uuid_type,
								  const char_uuid_t *char_uuid, uint16_t char_value_length,
								  uint8_t char_properties, uint8_t security_permissions,
								  uint8_t gatt_evt_mask, uint8_t enc_key_size, uint8_t is_variable,
								  uint16_t *char_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_add_char_cp0 *cp0 = (aci_gatt_add_char_cp0 *)(cmd_buffer);
	aci_gatt_add_char_cp1 *cp1 =
		(aci_gatt_add_char_cp1 *)(cmd_buffer + 2 + 1 +
								  (char_uuid_type == 1 ? 2 : (char_uuid_type == 2 ? 16 : 0)));
	aci_gatt_add_char_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	cp0->service_handle = service_handle;
	index_input += 2;
	cp0->char_uuid_type = char_uuid_type;
	index_input += 1;
	{
		uint8_t size;
		switch (char_uuid_type) {
		case 1:
			size = 2;
			break;
		case 2:
			size = 16;
			break;
		default:
			return BLE_STATUS_ERROR;
		}
		memcpy((void *)&cp0->char_uuid, (const void *)char_uuid, size);
		index_input += size;
		{
			cp1->char_value_length = char_value_length;
		}
		index_input += 2;
		{
			cp1->char_properties = char_properties;
		}
		index_input += 1;
		{
			cp1->security_permissions = security_permissions;
		}
		index_input += 1;
		{
			cp1->gatt_evt_mask = gatt_evt_mask;
		}
		index_input += 1;
		{
			cp1->enc_key_size = enc_key_size;
		}
		index_input += 1;
		{
			cp1->is_variable = is_variable;
		}
		index_input += 1;
	}
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x104;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	if (resp.status)
		return resp.status;
	*char_handle = resp.char_handle;
	return BLE_STATUS_SUCCESS;
}

enum ble_status
aci_gatt_add_char_desc(uint16_t service_handle, uint16_t char_handle, uint8_t char_desc_uuid_type,
					   const char_desc_uuid_t *char_desc_uuid, uint8_t char_desc_value_max_len,
					   uint8_t char_desc_value_length, const uint8_t *char_desc_value,
					   uint8_t security_permissions, uint8_t access_permissions,
					   uint8_t gatt_evt_mask, uint8_t enc_key_size, uint8_t is_variable,
					   uint16_t *char_desc_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_add_char_desc_cp0 *cp0 = (aci_gatt_add_char_desc_cp0 *)(cmd_buffer);
	aci_gatt_add_char_desc_cp1 *cp1 =
		(aci_gatt_add_char_desc_cp1 *)(cmd_buffer + 2 + 2 + 1 +
									   (char_desc_uuid_type == 1 ?
											2 :
											(char_desc_uuid_type == 2 ? 16 : 0)));
	aci_gatt_add_char_desc_cp2 *cp2 =
		(aci_gatt_add_char_desc_cp2 *)(cmd_buffer + 2 + 2 + 1 +
									   (char_desc_uuid_type == 1 ?
											2 :
											(char_desc_uuid_type == 2 ? 16 : 0)) +
									   1 + 1 + char_desc_value_length * (sizeof(uint8_t)));
	aci_gatt_add_char_desc_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	cp0->service_handle = service_handle;
	index_input += 2;
	cp0->char_handle = char_handle;
	index_input += 2;
	cp0->char_desc_uuid_type = char_desc_uuid_type;
	index_input += 1;
	{
		uint8_t size;
		switch (char_desc_uuid_type) {
		case 1:
			size = 2;
			break;
		case 2:
			size = 16;
			break;
		default:
			return BLE_STATUS_ERROR;
		}
		memcpy((void *)&cp0->char_desc_uuid, (const void *)char_desc_uuid, size);
		index_input += size;
		{
			cp1->char_desc_value_max_len = char_desc_value_max_len;
		}
		index_input += 1;
		{
			cp1->char_desc_value_length = char_desc_value_length;
		}
		index_input += 1;
		memcpy((void *)&cp1->char_desc_value, (const void *)char_desc_value,
			   char_desc_value_length);
		index_input += char_desc_value_length;
		{
			cp2->security_permissions = security_permissions;
		}
		index_input += 1;
		{
			cp2->access_permissions = access_permissions;
		}
		index_input += 1;
		{
			cp2->gatt_evt_mask = gatt_evt_mask;
		}
		index_input += 1;
		{
			cp2->enc_key_size = enc_key_size;
		}
		index_input += 1;
		{
			cp2->is_variable = is_variable;
		}
		index_input += 1;
	}
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x105;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	if (resp.status)
		return resp.status;
	*char_desc_handle = resp.char_desc_handle;
	return BLE_STATUS_SUCCESS;
}

enum ble_status aci_gatt_update_char_value(uint16_t service_handle, uint16_t char_handle,
										   uint8_t val_offset, uint8_t char_value_length,
										   const uint8_t *char_value)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_update_char_value_cp0 *cp0 = (aci_gatt_update_char_value_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->service_handle = service_handle;
	index_input += 2;
	cp0->char_handle = char_handle;
	index_input += 2;
	cp0->val_offset = val_offset;
	index_input += 1;
	cp0->char_value_length = char_value_length;
	index_input += 1;
	memcpy((void *)&cp0->char_value, (const void *)char_value, char_value_length);
	index_input += char_value_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x106;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_del_char(uint16_t serv_handle, uint16_t char_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_del_char_cp0 *cp0 = (aci_gatt_del_char_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->serv_handle = serv_handle;
	index_input += 2;
	cp0->char_handle = char_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x107;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_del_service(uint16_t serv_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_del_service_cp0 *cp0 = (aci_gatt_del_service_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->serv_handle = serv_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x108;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_del_include_service(uint16_t serv_handle, uint16_t include_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_del_include_service_cp0 *cp0 = (aci_gatt_del_include_service_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->serv_handle = serv_handle;
	index_input += 2;
	cp0->include_handle = include_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x109;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_set_event_mask(uint32_t gatt_evt_mask)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_set_event_mask_cp0 *cp0 = (aci_gatt_set_event_mask_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->gatt_evt_mask = gatt_evt_mask;
	index_input += 4;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x10a;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_exchange_config(uint16_t connection_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_exchange_config_cp0 *cp0 = (aci_gatt_exchange_config_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x10b;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_att_find_info_req(uint16_t connection_handle, uint16_t start_handle,
									  uint16_t end_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_att_find_info_req_cp0 *cp0 = (aci_att_find_info_req_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->start_handle = start_handle;
	index_input += 2;
	cp0->end_handle = end_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x10c;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_att_find_by_type_value_req(uint16_t connection_handle, uint16_t start_handle,
											   uint16_t end_handle, uint16_t uuid,
											   uint8_t attribute_val_length,
											   const uint8_t *attribute_val)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_att_find_by_type_value_req_cp0 *cp0 = (aci_att_find_by_type_value_req_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->start_handle = start_handle;
	index_input += 2;
	cp0->end_handle = end_handle;
	index_input += 2;
	cp0->uuid = uuid;
	index_input += 2;
	cp0->attribute_val_length = attribute_val_length;
	index_input += 1;
	memcpy((void *)&cp0->attribute_val, (const void *)attribute_val, attribute_val_length);
	index_input += attribute_val_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x10d;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_att_read_by_type_req(uint16_t connection_handle, uint16_t start_handle,
										 uint16_t end_handle, uint8_t uuid_type, const uuid_t *uuid)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_att_read_by_type_req_cp0 *cp0 = (aci_att_read_by_type_req_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	int uuid_size = (uuid_type == 2) ? 16 : 2;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->start_handle = start_handle;
	index_input += 2;
	cp0->end_handle = end_handle;
	index_input += 2;
	cp0->uuid_type = uuid_type;
	index_input += 1;
	memcpy((void *)&cp0->uuid, (const void *)uuid, uuid_size);
	index_input += uuid_size;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x10e;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_att_read_by_group_type_req(uint16_t connection_handle, uint16_t start_handle,
											   uint16_t end_handle, uint8_t uuid_type,
											   const uuid_t *uuid)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_att_read_by_group_type_req_cp0 *cp0 = (aci_att_read_by_group_type_req_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	int uuid_size = (uuid_type == 2) ? 16 : 2;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->start_handle = start_handle;
	index_input += 2;
	cp0->end_handle = end_handle;
	index_input += 2;
	cp0->uuid_type = uuid_type;
	index_input += 1;
	memcpy((void *)&cp0->uuid, (const void *)uuid, uuid_size);
	index_input += uuid_size;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x10f;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_att_prepare_write_req(uint16_t connection_handle, uint16_t attr_handle,
										  uint16_t val_offset, uint8_t attribute_val_length,
										  const uint8_t *attribute_val)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_att_prepare_write_req_cp0 *cp0 = (aci_att_prepare_write_req_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->val_offset = val_offset;
	index_input += 2;
	cp0->attribute_val_length = attribute_val_length;
	index_input += 1;
	memcpy((void *)&cp0->attribute_val, (const void *)attribute_val, attribute_val_length);
	index_input += attribute_val_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x110;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_att_execute_write_req(uint16_t connection_handle, uint8_t execute)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_att_execute_write_req_cp0 *cp0 = (aci_att_execute_write_req_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->execute = execute;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x111;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_disc_all_primary_services(uint16_t connection_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_disc_all_primary_services_cp0 *cp0 =
		(aci_gatt_disc_all_primary_services_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x112;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_disc_primary_service_by_uuid(uint16_t connection_handle, uint8_t uuid_type,
													  const uuid_t *uuid)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_disc_primary_service_by_uuid_cp0 *cp0 =
		(aci_gatt_disc_primary_service_by_uuid_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	int uuid_size = (uuid_type == 2) ? 16 : 2;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->uuid_type = uuid_type;
	index_input += 1;
	memcpy((void *)&cp0->uuid, (const void *)uuid, uuid_size);
	index_input += uuid_size;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x113;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_find_included_services(uint16_t connection_handle, uint16_t start_handle,
												uint16_t end_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_find_included_services_cp0 *cp0 = (aci_gatt_find_included_services_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->start_handle = start_handle;
	index_input += 2;
	cp0->end_handle = end_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x114;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_disc_all_char_of_service(uint16_t connection_handle, uint16_t start_handle,
												  uint16_t end_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_disc_all_char_of_service_cp0 *cp0 =
		(aci_gatt_disc_all_char_of_service_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->start_handle = start_handle;
	index_input += 2;
	cp0->end_handle = end_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x115;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_disc_char_by_uuid(uint16_t connection_handle, uint16_t start_handle,
										   uint16_t end_handle, uint8_t uuid_type,
										   const uuid_t *uuid)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_disc_char_by_uuid_cp0 *cp0 = (aci_gatt_disc_char_by_uuid_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	int uuid_size = (uuid_type == 2) ? 16 : 2;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->start_handle = start_handle;
	index_input += 2;
	cp0->end_handle = end_handle;
	index_input += 2;
	cp0->uuid_type = uuid_type;
	index_input += 1;
	memcpy((void *)&cp0->uuid, (const void *)uuid, uuid_size);
	index_input += uuid_size;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x116;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_disc_all_char_desc(uint16_t connection_handle, uint16_t char_handle,
											uint16_t end_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_disc_all_char_desc_cp0 *cp0 = (aci_gatt_disc_all_char_desc_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->char_handle = char_handle;
	index_input += 2;
	cp0->end_handle = end_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x117;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_read_char_value(uint16_t connection_handle, uint16_t attr_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_read_char_value_cp0 *cp0 = (aci_gatt_read_char_value_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x118;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_read_using_char_uuid(uint16_t connection_handle, uint16_t start_handle,
											  uint16_t end_handle, uint8_t uuid_type,
											  const uuid_t *uuid)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_read_using_char_uuid_cp0 *cp0 = (aci_gatt_read_using_char_uuid_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	int uuid_size = (uuid_type == 2) ? 16 : 2;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->start_handle = start_handle;
	index_input += 2;
	cp0->end_handle = end_handle;
	index_input += 2;
	cp0->uuid_type = uuid_type;
	index_input += 1;
	memcpy((void *)&cp0->uuid, (const void *)uuid, uuid_size);
	index_input += uuid_size;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x119;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_read_long_char_value(uint16_t connection_handle, uint16_t attr_handle,
											  uint16_t val_offset)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_read_long_char_value_cp0 *cp0 = (aci_gatt_read_long_char_value_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->val_offset = val_offset;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x11a;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_read_multiple_char_value(uint16_t connection_handle,
												  uint8_t number_of_handles,
												  const handle_entry_t *handle_entry)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_read_multiple_char_value_cp0 *cp0 =
		(aci_gatt_read_multiple_char_value_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->number_of_handles = number_of_handles;
	index_input += 1;
	memcpy((void *)&cp0->handle_entry, (const void *)handle_entry,
		   number_of_handles * (sizeof(handle_entry_t)));
	index_input += number_of_handles * (sizeof(handle_entry_t));
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x11b;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_write_char_value(uint16_t connection_handle, uint16_t attr_handle,
										  uint8_t attribute_val_length,
										  const uint8_t *attribute_val)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_write_char_value_cp0 *cp0 = (aci_gatt_write_char_value_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->attribute_val_length = attribute_val_length;
	index_input += 1;
	memcpy((void *)&cp0->attribute_val, (const void *)attribute_val, attribute_val_length);
	index_input += attribute_val_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x11c;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_write_long_char_value(uint16_t connection_handle, uint16_t attr_handle,
											   uint16_t val_offset, uint8_t attribute_val_length,
											   const uint8_t *attribute_val)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_write_long_char_value_cp0 *cp0 = (aci_gatt_write_long_char_value_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->val_offset = val_offset;
	index_input += 2;
	cp0->attribute_val_length = attribute_val_length;
	index_input += 1;
	memcpy((void *)&cp0->attribute_val, (const void *)attribute_val, attribute_val_length);
	index_input += attribute_val_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x11d;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_write_char_reliable(uint16_t connection_handle, uint16_t attr_handle,
											 uint16_t val_offset, uint8_t attribute_val_length,
											 const uint8_t *attribute_val)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_write_char_reliable_cp0 *cp0 = (aci_gatt_write_char_reliable_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->val_offset = val_offset;
	index_input += 2;
	cp0->attribute_val_length = attribute_val_length;
	index_input += 1;
	memcpy((void *)&cp0->attribute_val, (const void *)attribute_val, attribute_val_length);
	index_input += attribute_val_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x11e;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_write_without_resp(uint16_t connection_handle, uint16_t attr_handle,
											uint8_t attribute_val_length,
											const uint8_t *attribute_val)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_write_without_resp_cp0 *cp0 = (aci_gatt_write_without_resp_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->attribute_val_length = attribute_val_length;
	index_input += 1;
	memcpy((void *)&cp0->attribute_val, (const void *)attribute_val, attribute_val_length);
	index_input += attribute_val_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x123;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_signed_write_without_resp(uint16_t connection_handle, uint16_t attr_handle,
												   uint8_t attribute_val_length,
												   const uint8_t *attribute_val)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_signed_write_without_resp_cp0 *cp0 =
		(aci_gatt_signed_write_without_resp_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->attribute_val_length = attribute_val_length;
	index_input += 1;
	memcpy((void *)&cp0->attribute_val, (const void *)attribute_val, attribute_val_length);
	index_input += attribute_val_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x124;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_confirm_indication(uint16_t connection_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_confirm_indication_cp0 *cp0 = (aci_gatt_confirm_indication_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x125;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_permit_write(uint16_t connection_handle, uint16_t attr_handle,
									  uint8_t write_status, uint8_t error_code,
									  uint8_t attribute_val_length, const uint8_t *attribute_val)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_permit_write_cp0 *cp0 = (aci_gatt_permit_write_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->write_status = write_status;
	index_input += 1;
	cp0->error_code = error_code;
	index_input += 1;
	cp0->attribute_val_length = attribute_val_length;
	index_input += 1;
	memcpy((void *)&cp0->attribute_val, (const void *)attribute_val, attribute_val_length);
	index_input += attribute_val_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x126;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_permit_read(uint16_t connection_handle, uint8_t read_status,
									 uint8_t error_code, uint16_t attr_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_permit_read_cp0 *cp0 = (aci_gatt_permit_read_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->read_status = read_status;
	index_input += 1;
	cp0->error_code = error_code;
	index_input += 1;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x127;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_set_security_permission(uint16_t serv_handle, uint16_t attr_handle,
												 uint8_t security_permissions)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_set_security_permission_cp0 *cp0 =
		(aci_gatt_set_security_permission_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->serv_handle = serv_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->security_permissions = security_permissions;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x128;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_set_desc_value(uint16_t serv_handle, uint16_t char_handle,
										uint16_t char_desc_handle, uint16_t val_offset,
										uint8_t char_desc_value_length,
										const uint8_t *char_desc_value)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_set_desc_value_cp0 *cp0 = (aci_gatt_set_desc_value_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->serv_handle = serv_handle;
	index_input += 2;
	cp0->char_handle = char_handle;
	index_input += 2;
	cp0->char_desc_handle = char_desc_handle;
	index_input += 2;
	cp0->val_offset = val_offset;
	index_input += 2;
	cp0->char_desc_value_length = char_desc_value_length;
	index_input += 1;
	memcpy((void *)&cp0->char_desc_value, (const void *)char_desc_value, char_desc_value_length);
	index_input += char_desc_value_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x129;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_read_handle_value(uint16_t attr_handle, uint16_t offset,
										   uint16_t value_length_requested, uint16_t *length,
										   uint16_t *value_length, uint8_t *value)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_read_handle_value_cp0 *cp0 = (aci_gatt_read_handle_value_cp0 *)(cmd_buffer);
	aci_gatt_read_handle_value_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->offset = offset;
	index_input += 2;
	cp0->value_length_requested = value_length_requested;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x12a;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	if (resp.status)
		return resp.status;
	*length = resp.length;
	*value_length = resp.value_length;
	memcpy((void *)value, (const void *)resp.value, *value_length);
	return BLE_STATUS_SUCCESS;
}

enum ble_status aci_gatt_update_char_value_ext(uint16_t conn_handle_to_notify,
											   uint16_t service_handle, uint16_t char_handle,
											   uint8_t update_type, uint16_t char_length,
											   uint16_t value_offset, uint8_t value_length,
											   const uint8_t *value)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_update_char_value_ext_cp0 *cp0 = (aci_gatt_update_char_value_ext_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->conn_handle_to_notify = conn_handle_to_notify;
	index_input += 2;
	cp0->service_handle = service_handle;
	index_input += 2;
	cp0->char_handle = char_handle;
	index_input += 2;
	cp0->update_type = update_type;
	index_input += 1;
	cp0->char_length = char_length;
	index_input += 2;
	cp0->value_offset = value_offset;
	index_input += 2;
	cp0->value_length = value_length;
	index_input += 1;
	memcpy((void *)&cp0->value, (const void *)value, value_length);
	index_input += value_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x12c;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_set_access_permission(uint16_t serv_handle, uint16_t attr_handle,
											   uint8_t access_permissions)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_set_access_permission_cp0 *cp0 = (aci_gatt_set_access_permission_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->serv_handle = serv_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->access_permissions = access_permissions;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x12e;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_store_db(void)
{
	struct hci_request rq;
	enum ble_status status = 0;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x130;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_send_mult_notification(uint16_t connection_handle,
												uint8_t number_of_handles,
												const handle_entry_t *handle_entry)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_send_mult_notification_cp0 *cp0 = (aci_gatt_send_mult_notification_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->number_of_handles = number_of_handles;
	index_input += 1;
	memcpy((void *)&cp0->handle_entry, (const void *)handle_entry,
		   number_of_handles * (sizeof(handle_entry_t)));
	index_input += number_of_handles * (sizeof(handle_entry_t));
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x131;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_read_multiple_var_char_value(uint16_t connection_handle,
													  uint8_t number_of_handles,
													  const handle_entry_t *handle_entry)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_read_multiple_var_char_value_cp0 *cp0 =
		(aci_gatt_read_multiple_var_char_value_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->number_of_handles = number_of_handles;
	index_input += 1;
	memcpy((void *)&cp0->handle_entry, (const void *)handle_entry,
		   number_of_handles * (sizeof(handle_entry_t)));
	index_input += number_of_handles * (sizeof(handle_entry_t));
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x132;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_write_without_resp_ext(uint16_t connection_handle, uint16_t attr_handle,
												uint8_t signed_mode, uint16_t data_length,
												uint32_t data_pointer)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_write_without_resp_ext_cp0 *cp0 = (aci_gatt_write_without_resp_ext_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->signed_mode = signed_mode;
	index_input += 1;
	cp0->data_length = data_length;
	index_input += 2;
	cp0->data_pointer = data_pointer;
	index_input += 4;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x140;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gatt_write_with_resp_ext(uint16_t connection_handle, uint16_t attr_handle,
											 uint8_t write_mode, uint16_t val_offset,
											 uint16_t data_length, uint32_t data_pointer)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gatt_write_with_resp_ext_cp0 *cp0 = (aci_gatt_write_with_resp_ext_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->attr_handle = attr_handle;
	index_input += 2;
	cp0->write_mode = write_mode;
	index_input += 1;
	cp0->val_offset = val_offset;
	index_input += 2;
	cp0->data_length = data_length;
	index_input += 2;
	cp0->data_pointer = data_pointer;
	index_input += 4;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x141;
	rq.event = 0x0F;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}
