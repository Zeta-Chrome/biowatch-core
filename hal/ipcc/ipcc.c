#include "ipcc.h"
#include "assert.h"
#include "reg.h"
#include "stm32wb55xx.h"

static ipcc_handle_t *g_tx_handles[6];
static ipcc_handle_t *g_rx_handles[6];

void hal_ipcc_init(uint8_t tx_prio, uint8_t rx_prio)
{
    reg_set_mask(&IPCC->C1CR, IPCC_C1CR_RXOIE_Msk | IPCC_C1CR_TXFIE_Msk);
    NVIC_SetPriority(IPCC_C1_TX_IRQn, tx_prio);
    NVIC_EnableIRQ(IPCC_C1_TX_IRQn);
    NVIC_SetPriority(IPCC_C1_RX_IRQn, rx_prio);
    NVIC_EnableIRQ(IPCC_C1_RX_IRQn);
}

void hal_ipcc_set_occupied(uint8_t channel, ipcc_handle_t *tx_handle)
{
    BW_ASSERT(channel <= 6 && channel >= 1, "Channel out of range %d (Expected 1-6)", channel);
    g_tx_handles[channel - 1] = tx_handle;
    reg_set_bit(&IPCC->C1SCR, IPCC_C1SCR_CH1S_Pos + channel - 1);
    reg_clear_bit(&IPCC->C1MR, IPCC_C1MR_CH1FM_Pos + channel - 1);
}

void hal_ipcc_set_free(uint8_t channel, ipcc_handle_t *rx_handle)
{
    BW_ASSERT(channel <= 6 && channel >= 1, "Channel out of range %d (Expected 1-6)", channel);
    g_rx_handles[channel - 1] = rx_handle;
    reg_set_bit(&IPCC->C1SCR, IPCC_C1SCR_CH1C_Pos + channel - 1);
    reg_clear_bit(&IPCC->C1MR, IPCC_C1MR_CH1OM_Pos + channel - 1);
}

void hal_ipcc_tx_isr()
{
    uint8_t idx;
    uint8_t mask = (~IPCC->C1TOC2SR) & (~(IPCC->C1MR >> 16)) & 0x3F;
    while (mask != 0)
    {
        idx = __CLZ(__RBIT(mask));
        mask = mask & (mask - 1);
        reg_set_bit(&IPCC->C1MR, IPCC_C1MR_CH1FM_Pos + idx);
        g_tx_handles[idx]->callback(g_tx_handles[idx]->callback);
    }
}

void hal_ipcc_rx_isr() 
{
    uint8_t idx;
    uint8_t mask = IPCC->C1TOC2SR & (~IPCC->C1MR) & 0x3F;
    while (mask != 0)
    {
        idx = __CLZ(__RBIT(mask));
        reg_set_bit(&IPCC->C1MR, IPCC_C1MR_CH1OM_Pos + idx);
        g_rx_handles[idx]->callback(g_rx_handles[idx]->callback);
    }
}

void hal_ipcc_deinit()
{
    reg_clear_mask(&IPCC->C1CR, IPCC_C1CR_RXOIE_Msk | IPCC_C1CR_TXFIE_Msk);
    NVIC_DisableIRQ(IPCC_C1_TX_IRQn);
    NVIC_DisableIRQ(IPCC_C1_RX_IRQn);
}
