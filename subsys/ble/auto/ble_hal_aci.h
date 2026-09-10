/*****************************************************************************
 * @file    ble_hal_aci.h
 * @brief   STM32WB BLE API (HAL_ACI)
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

#ifndef BLE_HAL_ACI_H__
#define BLE_HAL_ACI_H__

#include "ble_types.h"

/**
 * @brief aci_hal_write_config_data
 * this command writes a value to a configure data structure. it is useful to
 * setup directly some parameters for the ble stack.
 * refer to annex for details on the different parameters that can be
 * configured.
 * note: this command is an alias of aci_write_config_data.
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
ble_status_t aci_hal_write_config_data(uint8_t offset, uint8_t length, const uint8_t *value);

/**
 * @brief aci_hal_read_config_data
 * this command requests the value in the configure data structure. the number
 * of read bytes changes for different offset.
 * note: this command is an alias of aci_read_config_data.
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
ble_status_t aci_hal_read_config_data(uint8_t offset, uint8_t *data_length, uint8_t *data);

/**
 * @brief aci_hal_set_tx_power_level
 * this command sets the tx power level of the device. by controlling the pa
 * level, that determines the output power level (dbm) at the ic pin.
 * when the system starts up or reboots, the default tx power level is used,
 * which is the maximum value. once this command is given, the output power
 * changes instantly, regardless if there is ble communication going on or not.
 * for example, for debugging purpose, the device can be set to advertise all
 * the time. by using this command, one can then observe the evolution of the
 * tx signal strength.
 * the system keeps the last received tx power level from the command, i.e. the
 * 2nd command overwrites the previous tx power level. the new tx power level
 * remains until another aci_hal_set_tx_power_level command, or the system
 * reboots. however, note that the advertising extensions commands allow, per
 * advertising set, to override the value of tx power determined by
 * aci_hal_set_tx_power_level command (e.g. see aci_gap_adv_set_configuration).
 * refer to annex for the dbm corresponding values of pa_level parameter.
 *
 * @param en_high_power enable high power mode - deprecated and ignored
 *        values:
 *        - 0x00: standard power
 *        - 0x01: high power
 * @param pa_level power amplifier output level.
 *        values:
 *        - 0x00 ... 0x23
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_set_tx_power_level(uint8_t en_high_power, uint8_t pa_level);

/**
 * @brief aci_hal_le_tx_test_packet_number
 * this command returns the number of packets sent in direct test mode.
 * when the direct tx test is started, a 16-bit counter is used to count how
 * many packets have been transmitted.
 * this command can be used to check how many packets have been sent during the
 * direct tx test.
 * the counter starts from 0 and counts upwards. the counter can wrap and start
 * from 0 again. the counter is not cleared until the next direct tx test
 * starts.
 *
 * @param[out] number_of_packets number of packets sent during the last direct
 *        tx test.
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_le_tx_test_packet_number(uint32_t *number_of_packets);

/**
 * @brief aci_hal_tone_start
 * this command starts a carrier frequency, i.e. a tone, on a specific channel.
 * the frequency sine wave at the specific channel may be used for debugging
 * purpose only. the channel id is a parameter from 0x00 to 0x27 for the 40 ble
 * channels, e.g. 0x00 for 2.402 ghz, 0x01 for 2.404 ghz etc.
 * this command should not be used when normal ble activities are ongoing.
 * the tone should be stopped by aci_hal_tone_stop command.
 *
 * @param rf_channel ble channel id, from 0x00 to 0x27 meaning (2.402 +
 *        0.002*0xxx) ghz
 *        device will continuously emit 0s, that means that the tone will be at
 *        the channel center frequency minus the maximum frequency deviation
 *        (250 khz).
 *        values:
 *        - 0x00 ... 0x27
 * @param freq_offset frequency offset for tone channel
 *        values:
 *        - 0x00 ... 0xff
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_tone_start(uint8_t rf_channel, uint8_t freq_offset);

/**
 * @brief aci_hal_tone_stop
 * this command is used to stop the previously started aci_hal_tone_start
 * command.
 *
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_tone_stop(void);

/**
 * @brief aci_hal_get_link_status
 * this command returns the status of the 8 ble links managed by the device.
 *
 * @param[out] link_status array of link status (8 links). each link status is
 *        1 byte.
 *        values:
 *        - 0x00: idle
 *        - 0x01: advertising
 *        - 0x02: connected in peripheral role
 *        - 0x03: scanning
 *        - 0x04: reserved
 *        - 0x05: connected in central role
 *        - 0x06: tx test mode
 *        - 0x07: rx test mode
 *        - 0x81: advertising with additional beacon
 * @param[out] link_connection_handle array of connection handles (2 bytes) for
 *        8 links. valid only if the link status is "connected" (0x02 or 0x05)
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_get_link_status(uint8_t *link_status, uint16_t *link_connection_handle);

/**
 * @brief aci_hal_set_radio_activity_mask
 * this command set the bitmask associated to
 * aci_hal_end_of_radio_activity_event.
 * only the radio activities enabled in the mask will be reported to
 * application by aci_hal_end_of_radio_activity_event
 *
 * @param radio_activity_mask bitmask of radio events
 *        flags:
 *        - 0x0001: idle
 *        - 0x0002: advertising
 *        - 0x0004: peripheral connection
 *        - 0x0008: scanning
 *        - 0x0020: central connection
 *        - 0x0040: tx test mode
 *        - 0x0080: rx test mode
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_set_radio_activity_mask(uint16_t radio_activity_mask);

/**
 * @brief aci_hal_get_anchor_period
 * this command returns information about the anchor period to help application
 * in selecting slot timings when operating in multi-link scenarios.
 *
 * @param[out] anchor_period current anchor period.
 *        t = n * 0.625 ms.
 * @param[out] max_free_slot maximum available time that can be allocated for a
 *        new slot.
 *        t = n * 0.625 ms.
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_get_anchor_period(uint32_t *anchor_period, uint32_t *max_free_slot);

/**
 * @brief aci_hal_set_event_mask
 * this command is used to enable/disable the generation of hal events. if the
 * bit in the event_mask is set to a one, then the event associated with that
 * bit will be enabled.
 *
 * @param event_mask aci hal event mask. default: 0x00000000.
 *        flags:
 *        - 0x00000000: no events specified (default)
 *        - 0x00000001: aci_hal_scan_req_report_event
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_set_event_mask(uint32_t event_mask);

/**
 * @brief aci_hal_set_peripheral_latency
 * this command is used to disable/enable the peripheral latency feature during
 * a connection. note that, by default, the peripheral latency is enabled at
 * connection time.
 *
 * @param enable enable/disable peripheral latency.
 *        values:
 *        - 0x00: peripheral latency is disabled
 *        - 0x01: peripheral latency is enabled
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_set_peripheral_latency(uint8_t enable);

/**
 * @brief aci_hal_read_rssi
 * this command returns the value of the rssi.
 *
 * @param[out] rssi rssi (signed integer).
 *        units: dbm.
 *        values:
 *        - 127: rssi not available
 *        - -127 ... 20
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_read_rssi(uint8_t *rssi);

/**
 * @brief aci_hal_ead_encrypt_decrypt
 * this command encrypts or decrypts data following the encrypted advertising
 * data scheme.
 * when encryption mode is selected, in_data shall only contain the payload
 * field to encrypt. the command adds the randomizer and mic fields in the
 * result. the result data length (out_data_length) is equal to the input
 * length plus 9.
 * when decryption mode is selected, in_data shall contain the full encrypted
 * data (randomizer + payload + mic). the result data length (out_data_length)
 * is equal to the input length minus 9.
 * if the decryption fails, the returned status is ble_status_failed, otherwise
 * it is ble_status_success.
 * note: the in_data_length value must not exceed (ble_cmd_max_param_len - 27)
 * i.e. 228 for ble_cmd_max_param_len default value.
 *
 * @param mode ead operation mode: encryption or decryption.
 *        values:
 *        - 0x00: encryption
 *        - 0x01: decryption
 * @param key session key used for ead operation (in little endian format).
 * @param iv initialization vector used for ead operation (in little endian
 *        format).
 * @param in_data_length length of input data
 * @param in_data input data
 * @param[out] out_data_length length of result data
 * @param[out] out_data result data
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_ead_encrypt_decrypt(uint8_t mode, const uint8_t *key, const uint8_t *iv,
										 uint16_t in_data_length, const uint8_t *in_data,
										 uint16_t *out_data_length, uint8_t *out_data);

/**
 * @brief aci_hal_read_radio_reg
 * this command reads register value from the rf module.
 *
 * @param register_address address of the register to be read
 * @param[out] reg_val register value
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_read_radio_reg(uint8_t register_address, uint8_t *reg_val);

/**
 * @brief aci_hal_write_radio_reg
 * this command writes register value to the rf module.
 *
 * @param register_address address of the register to be written
 * @param register_value value to be written
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_write_radio_reg(uint8_t register_address, uint8_t register_value);

/**
 * @brief aci_hal_read_raw_rssi
 * this command returns the raw value of the rssi.
 *
 * @param[out] value raw rssi value
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_read_raw_rssi(uint8_t *value);

/**
 * @brief aci_hal_rx_start
 * this command does set up the rf to listen to a specific rf channel.
 *
 * @param rf_channel ble channel id, from 0x00 to 0x27 meaning (2.402 +
 *        0.002*0xxx) ghz
 *        device will continuously emit 0s, that means that the tone will be at
 *        the channel center frequency minus the maximum frequency deviation
 *        (250 khz).
 *        values:
 *        - 0x00 ... 0x27
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_rx_start(uint8_t rf_channel);

/**
 * @brief aci_hal_rx_stop
 * this command stops a previous aci_hal_rx_start command.
 *
 * @return value indicating success or error code.
 */
ble_status_t aci_hal_rx_stop(void);

#endif /* BLE_HAL_ACI_H__ */
