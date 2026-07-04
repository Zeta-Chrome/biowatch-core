#ifndef HAL_IPCC_H
#define HAL_IPCC_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*ipcc_callback_t)(void *);

typedef struct
{
    ipcc_callback_t callback;
    void *user_data;
} ipcc_handle_t;

void hal_ipcc_init(uint8_t tx_prio, uint8_t rx_prio);
bool hal_ipcc_is_tx_channel_occupied(uint8_t channel);
void hal_ipcc_tx(uint8_t channel, ipcc_handle_t *tx_handle);
void hal_ipcc_tx_masked(uint8_t channel, ipcc_handle_t *tx_handle);
bool hal_ipcc_is_rx_channel_occupied(uint8_t channel);
void hal_ipcc_rx(uint8_t channel, ipcc_handle_t *rx_handle);
void hal_ipcc_deinit();

#endif
