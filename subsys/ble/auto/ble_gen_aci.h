/*****************************************************************************
 * @file    ble_gen_aci.h
 * @brief   STM32WB BLE API (GEN_ACI)
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

#ifndef BLE_GEN_ACI_H__
#define BLE_GEN_ACI_H__

#include "ble_types.h"

/**
 * @brief aci_reset
 * this command resets the ble stack (host and le controller).
 *
 * @param mode aci reset mode.
 *        values:
 *        - 0x00: reset without ble stack options change
 *        - 0x01: reset with ble stack option changes
 * @param options new ble stack options to set at aci reset (a bit set to 1
 *        means that the corresponding optional feature is activated).
 *        flags:
 *        - 0x00000001: ll only mode
 *        - 0x00000002: no service change description
 *        - 0x00000004: device name is read-only
 *        - 0x00000008: support of extended advertising
 *        - 0x00000010: support of channel selection algorithm #2
 *        - 0x00000020: reduced gatt database in nvm
 *        - 0x00000040: support of gatt caching
 *        - 0x00000080: support of le power class 1 (flag not available in rcp
 *          mode)
 *        - 0x00000100: appearance is writable
 *        - 0x00000200: support of enhanced att
 * @return value indicating success or error code.
 */
ble_status_t aci_reset(uint8_t mode, uint32_t options);

/**
 * @brief aci_get_information
 * this command reads the local aci information.
 *
 * @param[out] version ble stack version.
 * @param[out] options current ble stack options (a bit set to 1 means that the
 *        corresponding optional feature is activated).
 *        flags:
 *        - 0x00000001: ll only mode
 *        - 0x00000002: no service change description
 *        - 0x00000004: device name is read-only
 *        - 0x00000008: support of extended advertising
 *        - 0x00000010: support of channel selection algorithm #2
 *        - 0x00000020: reduced gatt database in nvm
 *        - 0x00000040: support of gatt caching
 *        - 0x00000080: support of le power class 1 (flag not available in rcp
 *          mode)
 *        - 0x00000100: appearance is writable
 *        - 0x00000200: support of enhanced att
 * @param[out] debug_info ble stack debug information.
 * @return value indicating success or error code.
 */
ble_status_t aci_get_information(uint32_t *version, uint32_t *options, uint32_t *debug_info);

/**
 * @brief aci_write_config_data
 * this command writes a value to a configure data structure. it is useful to
 * setup directly some parameters for the ble stack.
 * refer to annex for details on the different parameters that can be
 * configured.
 *
 * @param offset offset of the element in the configuration data structure
 *        which has to be written.
 *        values:
 *        - 0x00: config_data_public_address_offset;
 *          bluetooth public address; 6 bytes
 *        - 0x08: config_data_er_offset;
 *          encryption root key; 16 bytes
 *        - 0x18: config_data_ir_offset;
 *          identity root key; 16 bytes
 *        - 0x2e: config_data_random_address_offset;
 *          static random address; 6 bytes
 *        - 0x34: config_data_gap_add_rec_nbr_offset;
 *          gap service additional record number; 1 byte
 *        - 0x35: config_data_sc_key_type_offset;
 *          secure connections key type; 1 byte
 *        - 0xb0: config_data_smp_mode_offset;
 *          smp mode; 1 byte
 *        - 0xc0: config_data_ll_scan_chan_map_offset;
 *          ll scan channel map; 1 byte
 *        - 0xc1: config_data_ll_bg_scan_mode_offset;
 *          ll background scan mode; 1 byte
 *        - 0xc3: config_data_ll_rpa_mode_offset;
 *          ll rpa mode; 1 byte
 *        - 0xd1: config_data_ll_max_data_ext_offset [only for full stack];
 *          ll maximum data length extension; 8 bytes
 * @param length length of data to be written
 * @param value data to be written
 * @return value indicating success or error code.
 */
ble_status_t aci_write_config_data(uint8_t offset, uint8_t length, const uint8_t *value);

/**
 * @brief aci_read_config_data
 * this command requests the value in the configure data structure. the number
 * of read bytes changes for different offset.
 *
 * @param offset offset of the element in the configuration data structure
 *        which has to be read.
 *        values:
 *        - 0x00: config_data_public_address_offset;
 *          bluetooth public address; 6 bytes
 *        - 0x08: config_data_er_offset;
 *          encryption root key used to derive ltk (legacy) and csrk; 16 bytes
 *        - 0x18: config_data_ir_offset
 *          identity root key used to derive dhk (legacy) and irk; 16 bytes
 *        - 0x2e: config_data_random_address_offset;
 *          static random address; 6 bytes
 * @param[out] data_length length of data in octets
 * @param[out] data data field associated with offset parameter
 * @return value indicating success or error code.
 */
ble_status_t aci_read_config_data(uint8_t offset, uint8_t *data_length, uint8_t *data);

#endif /* BLE_GEN_ACI_H__ */
