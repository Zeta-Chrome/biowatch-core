#include "aci_l2cap.h"
#include "subsys/ble/ble_types.h"
#include "subsys/ble/hci/hci.h"

enum ble_status aci_l2cap_connection_parameter_update_req(uint16_t connection_handle,
														  uint16_t conn_interval_min,
														  uint16_t conn_interval_max,
														  uint16_t latency,
														  uint16_t timeout_multiplier)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_l2cap_connection_parameter_update_req_cp0 *cp0 =
		(aci_l2cap_connection_parameter_update_req_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;

	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->conn_interval_min = conn_interval_min;
	index_input += 2;
	cp0->conn_interval_max = conn_interval_max;
	index_input += 2;
	cp0->latency = latency;
	index_input += 2;
	cp0->timeout_multiplier = timeout_multiplier;
	index_input += 2;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x181;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_l2cap_connection_parameter_update_resp(
	uint16_t connection_handle, uint16_t conn_interval_min, uint16_t conn_interval_max,
	uint16_t latency, uint16_t timeout_multiplier, uint16_t minimum_ce_length,
	uint16_t maximum_ce_length, uint8_t identifier, uint8_t accept)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_l2cap_connection_parameter_update_resp_cp0 *cp0 =
		(aci_l2cap_connection_parameter_update_resp_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;

	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->conn_interval_min = conn_interval_min;
	index_input += 2;
	cp0->conn_interval_max = conn_interval_max;
	index_input += 2;
	cp0->latency = latency;
	index_input += 2;
	cp0->timeout_multiplier = timeout_multiplier;
	index_input += 2;
	cp0->minimum_ce_length = minimum_ce_length;
	index_input += 2;
	cp0->maximum_ce_length = maximum_ce_length;
	index_input += 2;
	cp0->identifier = identifier;
	index_input += 1;
	cp0->accept = accept;
	index_input += 1;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x182;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_l2cap_coc_connect(uint16_t connection_handle, uint16_t spsm, uint16_t mtu,
									  uint16_t mps, uint16_t initial_credits,
									  uint8_t channel_number)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_l2cap_coc_connect_cp0 *cp0 = (aci_l2cap_coc_connect_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;

	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->spsm = spsm;
	index_input += 2;
	cp0->mtu = mtu;
	index_input += 2;
	cp0->mps = mps;
	index_input += 2;
	cp0->initial_credits = initial_credits;
	index_input += 2;
	cp0->channel_number = channel_number;
	index_input += 1;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x188;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_l2cap_coc_connect_confirm(uint16_t connection_handle, uint16_t mtu,
											  uint16_t mps, uint16_t initial_credits,
											  uint16_t result, uint8_t max_channel_number,
											  uint8_t *channel_number, uint8_t *channel_index_list)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_l2cap_coc_connect_confirm_cp0 *cp0 = (aci_l2cap_coc_connect_confirm_cp0 *)(cmd_buffer);
	aci_l2cap_coc_connect_confirm_rp0 resp;

	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->mtu = mtu;
	index_input += 2;
	cp0->mps = mps;
	index_input += 2;
	cp0->initial_credits = initial_credits;
	index_input += 2;
	cp0->result = result;
	index_input += 2;
	cp0->max_channel_number = max_channel_number;
	index_input += 1;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x189;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	if (resp.status)
		return resp.status;

	*channel_number = resp.channel_number;
	memcpy((void *)channel_index_list, (const void *)resp.channel_index_list, *channel_number);

	return BLE_STATUS_SUCCESS;
}

enum ble_status aci_l2cap_coc_reconf(uint16_t connection_handle, uint16_t mtu, uint16_t mps,
									 uint8_t channel_number, const uint8_t *channel_index_list)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_l2cap_coc_reconf_cp0 *cp0 = (aci_l2cap_coc_reconf_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;

	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->mtu = mtu;
	index_input += 2;
	cp0->mps = mps;
	index_input += 2;
	cp0->channel_number = channel_number;
	index_input += 1;
	memcpy((void *)&cp0->channel_index_list, (const void *)channel_index_list, channel_number);
	index_input += channel_number;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x18a;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_l2cap_coc_reconf_confirm(uint16_t connection_handle, uint16_t result)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_l2cap_coc_reconf_confirm_cp0 *cp0 = (aci_l2cap_coc_reconf_confirm_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;

	cp0->connection_handle = connection_handle;
	index_input += 2;
	cp0->result = result;
	index_input += 2;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x18b;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_l2cap_coc_disconnect(uint8_t channel_index)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_l2cap_coc_disconnect_cp0 *cp0 = (aci_l2cap_coc_disconnect_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;

	cp0->channel_index = channel_index;
	index_input += 1;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x18c;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_l2cap_coc_flow_control(uint8_t channel_index, uint16_t credits)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_l2cap_coc_flow_control_cp0 *cp0 = (aci_l2cap_coc_flow_control_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;

	cp0->channel_index = channel_index;
	index_input += 1;
	cp0->credits = credits;
	index_input += 2;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x18d;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}

enum ble_status aci_l2cap_coc_tx_data(uint8_t channel_index, uint16_t length, const uint8_t *data)
{
	struct hci_request rq;
	uint8_t cmd_buffer[HCI_COMMAND_MAX_PARAM_LEN];
	aci_l2cap_coc_tx_data_cp0 *cp0 = (aci_l2cap_coc_tx_data_cp0 *)(cmd_buffer);
	enum ble_status status = 0;
	int index_input = 0;

	cp0->channel_index = channel_index;
	index_input += 1;
	cp0->length = length;
	index_input += 2;
	memcpy((void *)&cp0->data, (const void *)data, length);
	index_input += length;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x18e;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;

	if (hci_send_req(&rq) == STATUS_TIMEOUT) {
		return BLE_STATUS_TIMEOUT;
	}

	return status;
}
