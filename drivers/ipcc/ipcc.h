#ifndef DRIVER_IPCC_H
#define DRIVER_IPCC_H

#include <stdbool.h>
#include <stdint.h>

/**
 *********************************************************************************
 * IPCC CHANNELS
 *********************************************************************************
 */

/*  CPU1                                             CPU2
 *   |             (SYSTEM)                            |
 *   |----HW_IPCC_SYSTEM_CMD_RSP_CHANNEL-------------->|
 *   |                                                 |
 *   |<---HW_IPCC_SYSTEM_EVENT_CHANNEL-----------------|
 *   |                                                 |
 *   |            (ZIGBEE)                             |
 *   |----HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL------------>|
 *   |                                                 |
 *   |----HW_IPCC_ZIGBEE_CMD_CLI_CHANNEL-------------->|
 *   |                                                 |
 *   |<---HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL-------|
 *   |                                                 |
 *   |<---HW_IPCC_ZIGBEE_CLI_NOTIF_ACK_CHANNEL---------|
 *   |                                                 |
 *   |             (THREAD)                            |
 *   |----HW_IPCC_THREAD_OT_CMD_RSP_CHANNEL----------->|
 *   |                                                 |
 *   |----HW_IPCC_THREAD_CLI_CMD_CHANNEL-------------->|
 *   |                                                 |
 *   |<---HW_IPCC_THREAD_NOTIFICATION_ACK_CHANNEL------|
 *   |                                                 |
 *   |<---HW_IPCC_THREAD_CLI_NOTIFICATION_ACK_CHANNEL--|
 *   |                                                 |
 *   |             (BLE)                               |
 *   |----HW_IPCC_BLE_CMD_CHANNEL--------------------->|
 *   |                                                 |
 *   |----HW_IPCC_HCI_ACL_DATA_CHANNEL---------------->|
 *   |                                                 |
 *   |<---HW_IPCC_BLE_EVENT_CHANNEL--------------------|
 *   |                                                 |
 *   |             (BLE LLD)                           |
 *   |----HW_IPCC_BLE_LLD_CMD_CHANNEL----------------->|
 *   |                                                 |
 *   |<---HW_IPCC_BLE_LLD_RSP_CHANNEL------------------|
 *   |                                                 |
 *   |<---HW_IPCC_BLE_LLD_M0_CMD_CHANNEL---------------|
 *   |                                                 |
 *   |             (MAC)                               |
 *   |----HW_IPCC_MAC_802_15_4_CMD_RSP_CHANNEL-------->|
 *   |                                                 |
 *   |<---HW_IPCC_MAC_802_15_4_NOTIFICATION_ACK_CHANNEL|
 *   |                                                 |
 *   |             (BUFFER)                            |
 *   |----HW_IPCC_MM_RELEASE_BUFFER_CHANNE------------>|
 *   |                                                 |
 *   |             (TRACE)                             |
 *   |<----HW_IPCC_TRACES_CHANNEL----------------------|
 *   |                                                 |
 *
 *
 *
 */

/** CPU1 */
#define IPCC_BLE_CMD_CHANNEL 1
#define IPCC_SYSTEM_CMD_RSP_CHANNEL 2
#define IPCC_THREAD_OT_CMD_RSP_CHANNEL 3
#define IPCC_MM_RELEASE_BUFFER_CHANNEL 4
#define IPCC_THREAD_CLI_CMD_CHANNEL 5
#define IPCC_HCI_ACL_DATA_CHANNEL 6

/** CPU2 */
#define IPCC_BLE_EVENT_CHANNEL 1
#define IPCC_SYSTEM_EVENT_CHANNEL 2
#define IPCC_TRACES_CHANNEL 4

typedef void (*ipcc_callback_t)(void *);

struct ipcc_handle {
	ipcc_callback_t callback;
	void *user_data;
};

void ipcc_init(uint8_t tx_prio, uint8_t rx_prio);
void ipcc_enable();
bool ipcc_is_tx_channel_occupied(uint8_t channel);
void ipcc_tx(uint8_t channel, struct ipcc_handle *tx_handle);
void ipcc_tx_masked(uint8_t channel, struct ipcc_handle *tx_handle);
bool ipcc_is_rx_channel_occupied(uint8_t channel);
void ipcc_rx(uint8_t channel, struct ipcc_handle *rx_handle);
void ipcc_deinit();

#endif
