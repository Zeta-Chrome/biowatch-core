#ifndef DRIVER_IPCC_H
#define DRIVER_IPCC_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*ipcc_callback_t)(void *);

struct ipcc_handle {
	ipcc_callback_t callback;
	void *user_data;
};

void ipcc_init(uint8_t tx_prio, uint8_t rx_prio);
bool ipcc_is_tx_channel_occupied(uint8_t channel);
void ipcc_tx(uint8_t channel, struct ipcc_handle *tx_handle);
void ipcc_tx_masked(uint8_t channel, struct ipcc_handle *tx_handle);
bool ipcc_is_rx_channel_occupied(uint8_t channel);
void ipcc_rx(uint8_t channel, struct ipcc_handle *rx_handle);
void ipcc_deinit();

#endif
