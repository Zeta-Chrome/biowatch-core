/*****************************************************************************
 * @file    ble_gen_aci.c
 * @brief   STM32WB BLE API (gen_aci)
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

#include "ble_gen_aci.h"
#include "subsys/ble/tl/hci_tl.h"
#include "subsys/ble/ble_defs.h"
#include <string.h>

ble_status_t aci_reset(uint8_t mode, uint32_t options)
{
	struct hci_request rq;
	uint8_t cmd_buffer[BLE_CMD_MAX_PARAM_LEN];
	aci_reset_cp0 *cp0 = (aci_reset_cp0 *)(cmd_buffer);
	ble_status_t status = 0;
	int index_input = 0;
	cp0->mode = mode;
	index_input += 1;
	cp0->options = options;
	index_input += 4;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x300;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;
	if (hci_send_req(&rq) == STATUS_TIMEOUT)
		return BLE_STATUS_TIMEOUT;
	return status;
}

ble_status_t aci_get_information(uint32_t *version, uint32_t *options, uint32_t *debug_info)
{
	struct hci_request rq;
	aci_get_information_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x301;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);
	if (hci_send_req(&rq) == STATUS_TIMEOUT)
		return BLE_STATUS_TIMEOUT;
	if (resp.status)
		return resp.status;
	memcpy((void *)version, (const void *)resp.version, 8);
	*options = resp.options;
	memcpy((void *)debug_info, (const void *)resp.debug_info, 12);
	return BLE_STATUS_SUCCESS;
}

ble_status_t aci_write_config_data(uint8_t offset, uint8_t length, const uint8_t *value)
{
	struct hci_request rq;
	uint8_t cmd_buffer[BLE_CMD_MAX_PARAM_LEN];
	aci_write_config_data_cp0 *cp0 = (aci_write_config_data_cp0 *)(cmd_buffer);
	ble_status_t status = 0;
	int index_input = 0;
	cp0->offset = offset;
	index_input += 1;
	cp0->length = length;
	index_input += 1;
	memcpy((void *)&cp0->value, (const void *)value, length);
	index_input += length;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x302;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &status;
	rq.ret_len = 1;
	if (hci_send_req(&rq) == STATUS_TIMEOUT)
		return BLE_STATUS_TIMEOUT;
	return status;
}

ble_status_t aci_read_config_data(uint8_t offset, uint8_t *data_length, uint8_t *data)
{
	struct hci_request rq;
	uint8_t cmd_buffer[BLE_CMD_MAX_PARAM_LEN];
	aci_read_config_data_cp0 *cp0 = (aci_read_config_data_cp0 *)(cmd_buffer);
	aci_read_config_data_rp0 resp;
	memset(&resp, 0, sizeof(resp));
	int index_input = 0;
	cp0->offset = offset;
	index_input += 1;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = 0x3f;
	rq.ocf = 0x303;
	rq.cmd_param = cmd_buffer;
	rq.cmd_len = index_input;
	rq.ret_param = &resp;
	rq.ret_len = sizeof(resp);
	if (hci_send_req(&rq) == STATUS_TIMEOUT)
		return BLE_STATUS_TIMEOUT;
	if (resp.status)
		return resp.status;
	*data_length = resp.data_length;
	memcpy((void *)data, (const void *)resp.data, *data_length);
	return BLE_STATUS_SUCCESS;
}
