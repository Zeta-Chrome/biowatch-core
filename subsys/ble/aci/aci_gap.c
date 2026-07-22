#include "aci_gap.h"
#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"
#include <string.h>

enum ble_status aci_gap_set_non_discoverable(void)
{
	struct hci_request rq;
	enum ble_status status = 0;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x081;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_set_limited_discoverable(
	uint8_t advertising_type, uint16_t advertising_interval_min, uint16_t advertising_interval_max,
	uint8_t own_address_type, uint8_t advertising_filter_policy, uint8_t local_name_length,
	const uint8_t *local_name, uint8_t service_uuid_length, const uint8_t *service_uuid_list,
	uint16_t conn_interval_min, uint16_t conn_interval_max)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_limited_discoverable_cp0 *cp0 =
		(aci_gap_set_limited_discoverable_cp0 *)(cmd_buffer);
	aci_gap_set_limited_discoverable_cp1 *cp1 =
		(aci_gap_set_limited_discoverable_cp1 *)(cmd_buffer + 1 + 2 + 2 + 1 + 1 + 1 +
												 local_name_length * (sizeof(uint8_t)));
	aci_gap_set_limited_discoverable_cp2 *cp2 =
		(aci_gap_set_limited_discoverable_cp2 *)(cmd_buffer + 1 + 2 + 2 + 1 + 1 + 1 +
												 local_name_length * (sizeof(uint8_t)) + 1 +
												 service_uuid_length * (sizeof(uint8_t)));
	enum ble_status status = 0;
	int index_input = 0;
	cp0->advertising_type = advertising_type;
	index_input += 1;
	cp0->advertising_interval_min = advertising_interval_min;
	index_input += 2;
	cp0->advertising_interval_max = advertising_interval_max;
	index_input += 2;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->advertising_filter_policy = advertising_filter_policy;
	index_input += 1;
	cp0->local_name_length = local_name_length;
	index_input += 1;
	{
		memcpy((void *)&cp0->local_name, (const void *)local_name, local_name_length);
		index_input += local_name_length;
		{
			cp1->service_uuid_length = service_uuid_length;
		}
		index_input += 1;
		memcpy((void *)&cp1->service_uuid_list, (const void *)service_uuid_list,
			   service_uuid_length);
		index_input += service_uuid_length;
		{
			cp2->conn_interval_min = conn_interval_min;
		}
		index_input += 2;
		{
			cp2->conn_interval_max = conn_interval_max;
		}
		index_input += 2;
	}
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x082;
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

enum ble_status aci_gap_set_discoverable(
	uint8_t advertising_type, uint16_t advertising_interval_min, uint16_t advertising_interval_max,
	uint8_t own_address_type, uint8_t advertising_filter_policy, uint8_t local_name_length,
	const uint8_t *local_name, uint8_t service_uuid_length, const uint8_t *service_uuid_list,
	uint16_t conn_interval_min, uint16_t conn_interval_max)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_discoverable_cp0 *cp0 = (aci_gap_set_discoverable_cp0 *)(cmd_buffer);
	aci_gap_set_discoverable_cp1 *cp1 =
		(aci_gap_set_discoverable_cp1 *)(cmd_buffer + 1 + 2 + 2 + 1 + 1 + 1 +
										 local_name_length * (sizeof(uint8_t)));
	aci_gap_set_discoverable_cp2 *cp2 =
		(aci_gap_set_discoverable_cp2 *)(cmd_buffer + 1 + 2 + 2 + 1 + 1 + 1 +
										 local_name_length * (sizeof(uint8_t)) + 1 +
										 service_uuid_length * (sizeof(uint8_t)));
	enum ble_status status = 0;
	int index_input = 0;
	cp0->advertising_type = advertising_type;
	index_input += 1;
	cp0->advertising_interval_min = advertising_interval_min;
	index_input += 2;
	cp0->advertising_interval_max = advertising_interval_max;
	index_input += 2;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->advertising_filter_policy = advertising_filter_policy;
	index_input += 1;
	cp0->local_name_length = local_name_length;
	index_input += 1;
	{
		memcpy((void *)&cp0->local_name, (const void *)local_name, local_name_length);
		index_input += local_name_length;
		{
			cp1->service_uuid_length = service_uuid_length;
		}
		index_input += 1;
		memcpy((void *)&cp1->service_uuid_list, (const void *)service_uuid_list,
			   service_uuid_length);
		index_input += service_uuid_length;
		{
			cp2->conn_interval_min = conn_interval_min;
		}
		index_input += 2;
		{
			cp2->conn_interval_max = conn_interval_max;
		}
		index_input += 2;
	}
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x083;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status
aci_gap_set_direct_connectable(uint8_t own_address_type, uint8_t directed_advertising_type,
							   uint8_t direct_address_type, const uint8_t *direct_address,
							   uint16_t advertising_interval_min, uint16_t advertising_interval_max)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_direct_connectable_cp0 *cp0 = (aci_gap_set_direct_connectable_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->directed_advertising_type = directed_advertising_type;
	index_input += 1;
	cp0->direct_address_type = direct_address_type;
	index_input += 1;
	memcpy((void *)&cp0->direct_address, (const void *)direct_address, 6);
	index_input += 6;
	cp0->advertising_interval_min = advertising_interval_min;
	index_input += 2;
	cp0->advertising_interval_max = advertising_interval_max;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x084;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_set_io_capability(uint8_t io_capability)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_io_capability_cp0 *cp0 = (aci_gap_set_io_capability_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->io_capability = io_capability;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x085;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_set_authentication_requirement(uint8_t bonding_mode, uint8_t mitm_mode,
													   uint8_t sc_support,
													   uint8_t keypress_notification_support,
													   uint8_t min_encryption_key_size,
													   uint8_t max_encryption_key_size,
													   uint8_t use_fixed_pin, uint32_t fixed_pin,
													   uint8_t identity_address_type)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_authentication_requirement_cp0 *cp0 =
		(aci_gap_set_authentication_requirement_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->bonding_mode = bonding_mode;
	index_input += 1;
	cp0->mitm_mode = mitm_mode;
	index_input += 1;
	cp0->sc_support = sc_support;
	index_input += 1;
	cp0->keypress_notification_support = keypress_notification_support;
	index_input += 1;
	cp0->min_encryption_key_size = min_encryption_key_size;
	index_input += 1;
	cp0->max_encryption_key_size = max_encryption_key_size;
	index_input += 1;
	cp0->use_fixed_pin = use_fixed_pin;
	index_input += 1;
	cp0->fixed_pin = fixed_pin;
	index_input += 4;
	cp0->identity_address_type = identity_address_type;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x086;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_set_authorization_requirement(uint16_t connection_handle,
													  uint8_t authorization_enable)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_authorization_requirement_cp0 *cp0 =
		(aci_gap_set_authorization_requirement_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->authorization_enable = authorization_enable;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x087;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_pass_key_resp(uint16_t connection_handle, uint32_t pass_key)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_pass_key_resp_cp0 *cp0 = (aci_gap_pass_key_resp_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->pass_key = pass_key;
	index_input += 4;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x088;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_authorization_resp(uint16_t connection_handle, uint8_t authorize)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_authorization_resp_cp0 *cp0 = (aci_gap_authorization_resp_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->authorize = authorize;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x089;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_init(uint8_t role, uint8_t privacy_enabled, uint8_t device_name_char_len,
							 uint16_t *service_handle, uint16_t *dev_name_char_handle,
							 uint16_t *appearance_char_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_init_cp0 *cp0 = (aci_gap_init_cp0 *)(cmd_buffer);
	aci_gap_init_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	cp0->role = role;
	index_input += 1;
	cp0->privacy_enabled = privacy_enabled;
	index_input += 1;
	cp0->device_name_char_len = device_name_char_len;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x08a;
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
	*dev_name_char_handle = resp.dev_name_char_handle;
	*appearance_char_handle = resp.appearance_char_handle;
	return BLE_STATUS_SUCCESS;
}

enum ble_status aci_gap_set_non_connectable(uint8_t advertising_event_type,
											uint8_t own_address_type)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_non_connectable_cp0 *cp0 = (aci_gap_set_non_connectable_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->advertising_event_type = advertising_event_type;
	index_input += 1;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x08b;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_set_undirected_connectable(uint16_t advertising_interval_min,
												   uint16_t advertising_interval_max,
												   uint8_t own_address_type,
												   uint8_t adv_filter_policy)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_undirected_connectable_cp0 *cp0 =
		(aci_gap_set_undirected_connectable_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->advertising_interval_min = advertising_interval_min;
	index_input += 2;
	cp0->advertising_interval_max = advertising_interval_max;
	index_input += 2;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->adv_filter_policy = adv_filter_policy;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x08c;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_peripheral_security_req(uint16_t connection_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_peripheral_security_req_cp0 *cp0 = (aci_gap_peripheral_security_req_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x08d;
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

enum ble_status aci_gap_update_adv_data(uint8_t adv_data_len, const uint8_t *adv_data)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_update_adv_data_cp0 *cp0 = (aci_gap_update_adv_data_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->adv_data_len = adv_data_len;
	index_input += 1;
	memcpy((void *)&cp0->adv_data, (const void *)adv_data, adv_data_len);
	index_input += adv_data_len;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x08e;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_delete_ad_type(uint8_t ad_type)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_delete_ad_type_cp0 *cp0 = (aci_gap_delete_ad_type_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->ad_type = ad_type;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x08f;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_get_security_level(uint16_t connection_handle, uint8_t *security_mode,
										   uint8_t *security_level)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_get_security_level_cp0 *cp0 = (aci_gap_get_security_level_cp0 *)(cmd_buffer);
	aci_gap_get_security_level_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x090;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	if (resp.status)
		return resp.status;
	*security_mode = resp.security_mode;
	*security_level = resp.security_level;
	return BLE_STATUS_SUCCESS;
}

enum ble_status aci_gap_set_event_mask(uint16_t gap_evt_mask)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_event_mask_cp0 *cp0 = (aci_gap_set_event_mask_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->gap_evt_mask = gap_evt_mask;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x091;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_configure_filter_accept_list(void)
{
	struct hci_request rq;
	enum ble_status status = 0;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x092;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_terminate(uint16_t connection_handle, uint8_t reason)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_terminate_cp0 *cp0 = (aci_gap_terminate_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->reason = reason;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x093;
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

enum ble_status aci_gap_clear_security_db(void)
{
	struct hci_request rq;
	enum ble_status status = 0;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x094;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_allow_rebond(uint16_t connection_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_allow_rebond_cp0 *cp0 = (aci_gap_allow_rebond_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x095;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_start_limited_discovery_proc(uint16_t le_scan_interval,
													 uint16_t le_scan_window,
													 uint8_t own_address_type,
													 uint8_t filter_duplicates)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_start_limited_discovery_proc_cp0 *cp0 =
		(aci_gap_start_limited_discovery_proc_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->le_scan_interval = le_scan_interval;
	index_input += 2;
	cp0->le_scan_window = le_scan_window;
	index_input += 2;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->filter_duplicates = filter_duplicates;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x096;
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

enum ble_status aci_gap_start_general_discovery_proc(uint16_t le_scan_interval,
													 uint16_t le_scan_window,
													 uint8_t own_address_type,
													 uint8_t filter_duplicates)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_start_general_discovery_proc_cp0 *cp0 =
		(aci_gap_start_general_discovery_proc_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->le_scan_interval = le_scan_interval;
	index_input += 2;
	cp0->le_scan_window = le_scan_window;
	index_input += 2;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->filter_duplicates = filter_duplicates;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x097;
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

enum ble_status aci_gap_start_auto_connection_establish_proc(
	uint16_t le_scan_interval, uint16_t le_scan_window, uint8_t own_address_type,
	uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t conn_latency,
	uint16_t supervision_timeout, uint16_t minimum_ce_length, uint16_t maximum_ce_length,
	uint8_t num_of_peer_entries, const peer_entry_t *peer_entry)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_start_auto_connection_establish_proc_cp0 *cp0 =
		(aci_gap_start_auto_connection_establish_proc_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->le_scan_interval = le_scan_interval;
	index_input += 2;
	cp0->le_scan_window = le_scan_window;
	index_input += 2;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->conn_interval_min = conn_interval_min;
	index_input += 2;
	cp0->conn_interval_max = conn_interval_max;
	index_input += 2;
	cp0->conn_latency = conn_latency;
	index_input += 2;
	cp0->supervision_timeout = supervision_timeout;
	index_input += 2;
	cp0->minimum_ce_length = minimum_ce_length;
	index_input += 2;
	cp0->maximum_ce_length = maximum_ce_length;
	index_input += 2;
	cp0->num_of_peer_entries = num_of_peer_entries;
	index_input += 1;
	memcpy((void *)&cp0->peer_entry, (const void *)peer_entry,
		   num_of_peer_entries * (sizeof(peer_entry_t)));
	index_input += num_of_peer_entries * (sizeof(peer_entry_t));
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x099;
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

enum ble_status aci_gap_start_general_connection_establish_proc(
	uint8_t le_scan_type, uint16_t le_scan_interval, uint16_t le_scan_window,
	uint8_t own_address_type, uint8_t scanning_filter_policy, uint8_t filter_duplicates)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_start_general_connection_establish_proc_cp0 *cp0 =
		(aci_gap_start_general_connection_establish_proc_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->le_scan_type = le_scan_type;
	index_input += 1;
	cp0->le_scan_interval = le_scan_interval;
	index_input += 2;
	cp0->le_scan_window = le_scan_window;
	index_input += 2;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->scanning_filter_policy = scanning_filter_policy;
	index_input += 1;
	cp0->filter_duplicates = filter_duplicates;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x09a;
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

enum ble_status aci_gap_start_selective_connection_establish_proc(
	uint8_t le_scan_type, uint16_t le_scan_interval, uint16_t le_scan_window,
	uint8_t own_address_type, uint8_t scanning_filter_policy, uint8_t filter_duplicates,
	uint8_t num_of_peer_entries, const peer_entry_t *peer_entry)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_start_selective_connection_establish_proc_cp0 *cp0 =
		(aci_gap_start_selective_connection_establish_proc_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->le_scan_type = le_scan_type;
	index_input += 1;
	cp0->le_scan_interval = le_scan_interval;
	index_input += 2;
	cp0->le_scan_window = le_scan_window;
	index_input += 2;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->scanning_filter_policy = scanning_filter_policy;
	index_input += 1;
	cp0->filter_duplicates = filter_duplicates;
	index_input += 1;
	cp0->num_of_peer_entries = num_of_peer_entries;
	index_input += 1;
	memcpy((void *)&cp0->peer_entry, (const void *)peer_entry,
		   num_of_peer_entries * (sizeof(peer_entry_t)));
	index_input += num_of_peer_entries * (sizeof(peer_entry_t));
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x09b;
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

enum ble_status aci_gap_create_connection(uint16_t le_scan_interval, uint16_t le_scan_window,
										  uint8_t peer_address_type, const uint8_t *peer_address,
										  uint8_t own_address_type, uint16_t conn_interval_min,
										  uint16_t conn_interval_max, uint16_t conn_latency,
										  uint16_t supervision_timeout, uint16_t minimum_ce_length,
										  uint16_t maximum_ce_length)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_create_connection_cp0 *cp0 = (aci_gap_create_connection_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->le_scan_interval = le_scan_interval;
	index_input += 2;
	cp0->le_scan_window = le_scan_window;
	index_input += 2;
	cp0->peer_address_type = peer_address_type;
	index_input += 1;
	memcpy((void *)&cp0->peer_address, (const void *)peer_address, 6);
	index_input += 6;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->conn_interval_min = conn_interval_min;
	index_input += 2;
	cp0->conn_interval_max = conn_interval_max;
	index_input += 2;
	cp0->conn_latency = conn_latency;
	index_input += 2;
	cp0->supervision_timeout = supervision_timeout;
	index_input += 2;
	cp0->minimum_ce_length = minimum_ce_length;
	index_input += 2;
	cp0->maximum_ce_length = maximum_ce_length;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x09c;
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

enum ble_status aci_gap_terminate_gap_proc(uint8_t procedure_code)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_terminate_gap_proc_cp0 *cp0 = (aci_gap_terminate_gap_proc_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->procedure_code = procedure_code;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x09d;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_start_connection_update(uint16_t connection_handle,
												uint16_t conn_interval_min,
												uint16_t conn_interval_max, uint16_t conn_latency,
												uint16_t supervision_timeout,
												uint16_t minimum_ce_length,
												uint16_t maximum_ce_length)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_start_connection_update_cp0 *cp0 = (aci_gap_start_connection_update_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->conn_interval_min = conn_interval_min;
	index_input += 2;
	cp0->conn_interval_max = conn_interval_max;
	index_input += 2;
	cp0->conn_latency = conn_latency;
	index_input += 2;
	cp0->supervision_timeout = supervision_timeout;
	index_input += 2;
	cp0->minimum_ce_length = minimum_ce_length;
	index_input += 2;
	cp0->maximum_ce_length = maximum_ce_length;
	index_input += 2;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x09e;
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

enum ble_status aci_gap_send_pairing_req(uint16_t connection_handle, uint8_t force_rebond)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_send_pairing_req_cp0 *cp0 = (aci_gap_send_pairing_req_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->force_rebond = force_rebond;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x09f;
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

enum ble_status aci_gap_set_broadcast_mode(uint16_t advertising_interval_min,
										   uint16_t advertising_interval_max,
										   uint8_t advertising_type, uint8_t own_address_type,
										   uint8_t adv_data_length, const uint8_t *adv_data,
										   uint8_t num_of_peer_entries,
										   const peer_entry_t *peer_entry)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_broadcast_mode_cp0 *cp0 = (aci_gap_set_broadcast_mode_cp0 *)(cmd_buffer);
	aci_gap_set_broadcast_mode_cp1 *cp1 =
		(aci_gap_set_broadcast_mode_cp1 *)(cmd_buffer + 2 + 2 + 1 + 1 + 1 +
										   adv_data_length * (sizeof(uint8_t)));
	enum ble_status status = 0;
	int index_input = 0;
	cp0->advertising_interval_min = advertising_interval_min;
	index_input += 2;
	cp0->advertising_interval_max = advertising_interval_max;
	index_input += 2;
	cp0->advertising_type = advertising_type;
	index_input += 1;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->adv_data_length = adv_data_length;
	index_input += 1;
	{
		memcpy((void *)&cp0->adv_data, (const void *)adv_data, adv_data_length);
		index_input += adv_data_length;
		{
			cp1->num_of_peer_entries = num_of_peer_entries;
		}
		index_input += 1;
		memcpy((void *)&cp1->peer_entry, (const void *)peer_entry,
			   num_of_peer_entries * (sizeof(peer_entry_t)));
		index_input += num_of_peer_entries * (sizeof(peer_entry_t));
	}
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0a1;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_start_observation_proc(uint16_t le_scan_interval, uint16_t le_scan_window,
											   uint8_t le_scan_type, uint8_t own_address_type,
											   uint8_t filter_duplicates,
											   uint8_t scanning_filter_policy)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_start_observation_proc_cp0 *cp0 = (aci_gap_start_observation_proc_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->le_scan_interval = le_scan_interval;
	index_input += 2;
	cp0->le_scan_window = le_scan_window;
	index_input += 2;
	cp0->le_scan_type = le_scan_type;
	index_input += 1;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->filter_duplicates = filter_duplicates;
	index_input += 1;
	cp0->scanning_filter_policy = scanning_filter_policy;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0a2;
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

enum ble_status aci_gap_get_bonded_devices(uint8_t *num_of_addresses,
										   bonded_device_entry_t *bonded_device_entry)
{
	struct hci_request rq;
	aci_gap_get_bonded_devices_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0a3;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	if (resp.status)
		return resp.status;
	*num_of_addresses = resp.num_of_addresses;
	memcpy((void *)bonded_device_entry, (const void *)resp.bonded_device_entry,
		   *num_of_addresses * (sizeof(bonded_device_entry_t)));
	return BLE_STATUS_SUCCESS;
}

enum ble_status aci_gap_check_bonded_device(uint8_t peer_address_type, const uint8_t *peer_address,
											uint8_t *id_address_type, uint8_t *id_address)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_check_bonded_device_cp0 *cp0 = (aci_gap_check_bonded_device_cp0 *)(cmd_buffer);
	aci_gap_check_bonded_device_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	cp0->peer_address_type = peer_address_type;
	index_input += 1;
	memcpy((void *)&cp0->peer_address, (const void *)peer_address, 6);
	index_input += 6;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0a4;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	if (resp.status)
		return resp.status;
	*id_address_type = resp.id_address_type;
	memcpy((void *)id_address, (const void *)resp.id_address, 6);
	return BLE_STATUS_SUCCESS;
}

enum ble_status aci_gap_numeric_comparison_value_confirm_yesno(uint16_t connection_handle,
															   uint8_t confirm_yes_no)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_numeric_comparison_value_confirm_yesno_cp0 *cp0 =
		(aci_gap_numeric_comparison_value_confirm_yesno_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->confirm_yes_no = confirm_yes_no;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0a5;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_passkey_input(uint16_t connection_handle, uint8_t input_type)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_passkey_input_cp0 *cp0 = (aci_gap_passkey_input_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->input_type = input_type;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0a6;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_get_oob_data(uint8_t oob_data_type, uint8_t *address_type, uint8_t *address,
									 uint8_t *oob_data_len, uint8_t *oob_data)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_get_oob_data_cp0 *cp0 = (aci_gap_get_oob_data_cp0 *)(cmd_buffer);
	aci_gap_get_oob_data_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	cp0->oob_data_type = oob_data_type;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0a7;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	if (resp.status)
		return resp.status;
	*address_type = resp.address_type;
	memcpy((void *)address, (const void *)resp.address, 6);
	*oob_data_len = resp.oob_data_len;
	memcpy((void *)oob_data, (const void *)resp.oob_data, 16);
	return BLE_STATUS_SUCCESS;
}

enum ble_status aci_gap_set_oob_data(uint8_t device_type, uint8_t address_type,
									 const uint8_t *address, uint8_t oob_data_type,
									 uint8_t oob_data_len, const uint8_t *oob_data)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_set_oob_data_cp0 *cp0 = (aci_gap_set_oob_data_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	device_type = device_type;
	cp0->device_type = device_type;
	index_input += 1;
	cp0->address_type = address_type;
	index_input += 1;
	memcpy((void *)&cp0->address, (const void *)address, 6);
	index_input += 6;
	cp0->oob_data_type = oob_data_type;
	index_input += 1;
	cp0->oob_data_len = oob_data_len;
	index_input += 1;
	memcpy((void *)&cp0->oob_data, (const void *)oob_data, 16);
	index_input += 16;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0a8;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_remove_bonded_device(uint8_t peer_identity_address_type,
											 const uint8_t *peer_identity_address)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_remove_bonded_device_cp0 *cp0 = (aci_gap_remove_bonded_device_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->peer_identity_address_type = peer_identity_address_type;
	index_input += 1;
	memcpy((void *)&cp0->peer_identity_address, (const void *)peer_identity_address, 6);
	index_input += 6;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0aa;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_add_devices_to_list(uint8_t num_of_list_entries,
											const list_entry_t *list_entry, uint8_t mode)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_add_devices_to_list_cp0 *cp0 = (aci_gap_add_devices_to_list_cp0 *)(cmd_buffer);
	aci_gap_add_devices_to_list_cp1 *cp1 =
		(aci_gap_add_devices_to_list_cp1 *)(cmd_buffer + 1 +
											num_of_list_entries * (sizeof(list_entry_t)));
	enum ble_status status = 0;
	int index_input = 0;
	cp0->num_of_list_entries = num_of_list_entries;
	index_input += 1;
	{
		memcpy((void *)&cp0->list_entry, (const void *)list_entry,
			   num_of_list_entries * (sizeof(list_entry_t)));
		index_input += num_of_list_entries * (sizeof(list_entry_t));
		{
			cp1->mode = mode;
		}
		index_input += 1;
	}
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0ab;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_pairing_request_reply(uint16_t connection_handle, uint8_t accept)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_pairing_request_reply_cp0 *cp0 = (aci_gap_pairing_request_reply_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->accept = accept;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0ad;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_additional_beacon_start(uint16_t adv_interval_min,
												uint16_t adv_interval_max, uint8_t adv_channel_map,
												uint8_t own_address_type,
												const uint8_t *own_address, uint8_t pa_level)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_additional_beacon_start_cp0 *cp0 = (aci_gap_additional_beacon_start_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->adv_interval_min = adv_interval_min;
	index_input += 2;
	cp0->adv_interval_max = adv_interval_max;
	index_input += 2;
	cp0->adv_channel_map = adv_channel_map;
	index_input += 1;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	memcpy((void *)&cp0->own_address, (const void *)own_address, 6);
	index_input += 6;
	cp0->pa_level = pa_level;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0b0;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_additional_beacon_stop(void)
{
	struct hci_request rq;
	enum ble_status status = 0;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0b1;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_additional_beacon_set_data(uint8_t adv_data_length, const uint8_t *adv_data)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_additional_beacon_set_data_cp0 *cp0 =
		(aci_gap_additional_beacon_set_data_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->adv_data_length = adv_data_length;
	index_input += 1;
	memcpy((void *)&cp0->adv_data, (const void *)adv_data, adv_data_length);
	index_input += adv_data_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0b2;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_adv_set_configuration(
	uint8_t adv_mode, uint8_t advertising_handle, uint16_t adv_event_properties,
	uint32_t primary_adv_interval_min, uint32_t primary_adv_interval_max,
	uint8_t primary_adv_channel_map, uint8_t own_address_type, uint8_t peer_address_type,
	const uint8_t *peer_address, uint8_t adv_filter_policy, uint8_t adv_tx_power,
	uint8_t secondary_adv_max_skip, uint8_t secondary_adv_phy, uint8_t adv_sid,
	uint8_t scan_req_notification_enable)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_adv_set_configuration_cp0 *cp0 = (aci_gap_adv_set_configuration_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->adv_mode = adv_mode;
	index_input += 1;
	cp0->advertising_handle = advertising_handle;
	index_input += 1;
	cp0->adv_event_properties = adv_event_properties;
	index_input += 2;
	cp0->primary_adv_interval_min = primary_adv_interval_min;
	index_input += 4;
	cp0->primary_adv_interval_max = primary_adv_interval_max;
	index_input += 4;
	cp0->primary_adv_channel_map = primary_adv_channel_map;
	index_input += 1;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->peer_address_type = peer_address_type;
	index_input += 1;
	memcpy((void *)&cp0->peer_address, (const void *)peer_address, 6);
	index_input += 6;
	cp0->adv_filter_policy = adv_filter_policy;
	index_input += 1;
	cp0->adv_tx_power = adv_tx_power;
	index_input += 1;
	cp0->secondary_adv_max_skip = secondary_adv_max_skip;
	index_input += 1;
	cp0->secondary_adv_phy = secondary_adv_phy;
	index_input += 1;
	cp0->adv_sid = adv_sid;
	index_input += 1;
	cp0->scan_req_notification_enable = scan_req_notification_enable;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0c0;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_adv_set_enable(uint8_t enable, uint8_t num_sets, const adv_set_t *adv_set)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_adv_set_enable_cp0 *cp0 = (aci_gap_adv_set_enable_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->enable = enable;
	index_input += 1;
	cp0->num_sets = num_sets;
	index_input += 1;
	memcpy((void *)&cp0->adv_set, (const void *)adv_set, num_sets * (sizeof(adv_set_t)));
	index_input += num_sets * (sizeof(adv_set_t));
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0c1;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_adv_set_adv_data(uint8_t advertising_handle, uint8_t operation,
										 uint8_t fragment_preference,
										 uint8_t advertising_data_length,
										 const uint8_t *advertising_data)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_adv_set_adv_data_cp0 *cp0 = (aci_gap_adv_set_adv_data_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->advertising_handle = advertising_handle;
	index_input += 1;
	cp0->operation = operation;
	index_input += 1;
	cp0->fragment_preference = fragment_preference;
	index_input += 1;
	cp0->advertising_data_length = advertising_data_length;
	index_input += 1;
	memcpy((void *)&cp0->advertising_data, (const void *)advertising_data, advertising_data_length);
	index_input += advertising_data_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0c2;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_adv_set_scan_resp_data(uint8_t advertising_handle, uint8_t operation,
											   uint8_t fragment_preference,
											   uint8_t scan_response_data_length,
											   const uint8_t *scan_response_data)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_adv_set_scan_resp_data_cp0 *cp0 = (aci_gap_adv_set_scan_resp_data_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->advertising_handle = advertising_handle;
	index_input += 1;
	cp0->operation = operation;
	index_input += 1;
	cp0->fragment_preference = fragment_preference;
	index_input += 1;
	cp0->scan_response_data_length = scan_response_data_length;
	index_input += 1;
	memcpy((void *)&cp0->scan_response_data, (const void *)scan_response_data,
		   scan_response_data_length);
	index_input += scan_response_data_length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0c3;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_adv_remove_set(uint8_t advertising_handle)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_adv_remove_set_cp0 *cp0 = (aci_gap_adv_remove_set_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->advertising_handle = advertising_handle;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0c4;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_adv_clear_sets(void)
{
	struct hci_request rq;
	enum ble_status status = 0;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0c5;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_adv_set_random_address(uint8_t advertising_handle,
											   const uint8_t *random_address)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_adv_set_random_address_cp0 *cp0 = (aci_gap_adv_set_random_address_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->advertising_handle = advertising_handle;
	index_input += 1;
	memcpy((void *)&cp0->random_address, (const void *)random_address, 6);
	index_input += 6;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0c6;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_gap_ext_start_scan(uint8_t scan_mode, uint8_t procedure,
									   uint8_t own_address_type, uint8_t filter_duplicates,
									   uint16_t duration, uint16_t period,
									   uint8_t scanning_filter_policy, uint8_t scanning_phys,
									   const scan_param_phy_t *scan_param_phy)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_ext_start_scan_cp0 *cp0 = (aci_gap_ext_start_scan_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->scan_mode = scan_mode;
	index_input += 1;
	cp0->procedure = procedure;
	index_input += 1;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->filter_duplicates = filter_duplicates;
	index_input += 1;
	cp0->duration = duration;
	index_input += 2;
	cp0->period = period;
	index_input += 2;
	cp0->scanning_filter_policy = scanning_filter_policy;
	index_input += 1;
	cp0->scanning_phys = scanning_phys;
	index_input += 1;
	memcpy((void *)&cp0->scan_param_phy, (const void *)scan_param_phy, 10);
	index_input += 10;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0d0;
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

enum ble_status aci_gap_ext_create_connection(uint8_t initiating_mode, uint8_t procedure,
											  uint8_t own_address_type, uint8_t peer_address_type,
											  const uint8_t *peer_address,
											  uint8_t advertising_handle, uint8_t subevent,
											  uint8_t initiator_filter_policy,
											  uint8_t initiating_phys,
											  const init_param_phy_t *init_param_phy)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_gap_ext_create_connection_cp0 *cp0 = (aci_gap_ext_create_connection_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;
	cp0->initiating_mode = initiating_mode;
	index_input += 1;
	cp0->procedure = procedure;
	index_input += 1;
	cp0->own_address_type = own_address_type;
	index_input += 1;
	cp0->peer_address_type = peer_address_type;
	index_input += 1;
	memcpy((void *)&cp0->peer_address, (const void *)peer_address, 6);
	index_input += 6;
	cp0->advertising_handle = advertising_handle;
	index_input += 1;
	cp0->subevent = subevent;
	index_input += 1;
	cp0->initiator_filter_policy = initiator_filter_policy;
	index_input += 1;
	cp0->initiating_phys = initiating_phys;
	index_input += 1;
	memcpy((void *)&cp0->init_param_phy, (const void *)init_param_phy, 48);
	index_input += 48;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x0d1;
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
