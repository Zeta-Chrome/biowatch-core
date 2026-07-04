#include "assert.h"
#include "critical.h"
#include "ipcc.h"
#include "stm32wb55xx.h"
#include "utils.h"

static ipcc_handle_t *g_tx_handles[6];
static ipcc_handle_t *g_rx_handles[6];

void hal_ipcc_init(uint8_t tx_prio, uint8_t rx_prio)
{
    SET_FIELD(RCC->AHB3ENR, RCC_AHB3ENR_IPCCEN_Msk);
    __IO uint32_t tempreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_IPCCEN_Pos);
    (void)tempreg;

    SET_FIELD(IPCC->C1CR, IPCC_C1CR_RXOIE_Msk | IPCC_C1CR_TXFIE_Msk);
    IPCC->C1SCR = 0x0000003F;
    IPCC->C1MR = 0x003F003F;

    NVIC_SetPriority(IPCC_C1_TX_IRQn, tx_prio);
    NVIC_EnableIRQ(IPCC_C1_TX_IRQn);
    NVIC_SetPriority(IPCC_C1_RX_IRQn, rx_prio);
    NVIC_EnableIRQ(IPCC_C1_RX_IRQn);
}

bool hal_ipcc_is_tx_channel_occupied(uint8_t channel)
{
    BW_ASSERT(channel <= 6 && channel >= 1, "Channel out of range %d (Expected 1-6)", channel);
    return READ_BIT(IPCC->C1TOC2SR, IPCC_C1TOC2SR_CH1F_Pos + channel - 1);
}

void hal_ipcc_tx(uint8_t channel, ipcc_handle_t *tx_handle)
{
    BW_ASSERT(channel <= 6 && channel >= 1, "Channel out of range %d (Expected 1-6)", channel);
    g_tx_handles[channel - 1] = tx_handle;
    IPCC->C1SCR = 1UL << (IPCC_C1SCR_CH1S_Pos + channel - 1); // Set tx channel as occupied

    RTOS_ENTER_CRITICAL();
    CLEAR_BIT(IPCC->C1MR, IPCC_C1MR_CH1FM_Pos + channel - 1); // Unmask the tx free interrupt
    RTOS_EXIT_CRITICAL();
}

void hal_ipcc_tx_masked(uint8_t channel, ipcc_handle_t *tx_handle)
{
    BW_ASSERT(channel <= 6 && channel >= 1, "Channel out of range %d (Expected 1-6)", channel);
    g_tx_handles[channel - 1] = tx_handle;
    IPCC->C1SCR = 1UL << (IPCC_C1SCR_CH1S_Pos + channel - 1); // Set tx channel as occupied
}

bool hal_ipcc_is_rx_channel_occupied(uint8_t channel)
{
    BW_ASSERT(channel <= 6 && channel >= 1, "Channel out of range %d (Expected 1-6)", channel);
    return READ_BIT(IPCC->C2TOC1SR, IPCC_C2TOC1SR_CH1F_Pos + channel - 1);
}

void hal_ipcc_rx(uint8_t channel, ipcc_handle_t *rx_handle)
{
    BW_ASSERT(channel <= 6 && channel >= 1, "Channel out of range %d (Expected 1-6)", channel);
    g_rx_handles[channel - 1] = rx_handle;

    RTOS_ENTER_CRITICAL();
    CLEAR_BIT(IPCC->C1MR, IPCC_C1MR_CH1OM_Pos + channel - 1); // Unmask rx occupied interrupt
    RTOS_EXIT_CRITICAL();
}

void hal_ipcc_tx_isr()
{
    uint8_t idx;
    uint8_t mask = ~(IPCC->C1TOC2SR) & (~(IPCC->C1MR >> 16) & 0x3F);
    while (mask != 0)
    {
        idx = __CLZ(__RBIT(mask));

        RTOS_ENTER_CRITICAL();
        SET_BIT(IPCC->C1MR, IPCC_C1MR_CH1FM_Pos + idx);
        RTOS_EXIT_CRITICAL();

        if (g_tx_handles[idx] && g_tx_handles[idx]->callback)
        {
            g_tx_handles[idx]->callback(g_tx_handles[idx]->user_data);
        }
        mask = mask & (mask - 1);
    }
}

void hal_ipcc_rx_isr()
{
    uint8_t idx;
    uint8_t mask = IPCC->C2TOC1SR & (~IPCC->C1MR) & 0x3F;
    while (mask != 0)
    {
        idx = __CLZ(__RBIT(mask));
        if (g_rx_handles[idx] && g_rx_handles[idx]->callback)
        {
            g_rx_handles[idx]->callback(g_rx_handles[idx]->user_data);
        }
        WRITE_BIT(IPCC->C1SCR, IPCC_C1SCR_CH1C_Pos + idx);
        mask = mask & (mask - 1);
    }
}

void hal_ipcc_deinit()
{
    NVIC_DisableIRQ(IPCC_C1_TX_IRQn);
    NVIC_DisableIRQ(IPCC_C1_RX_IRQn);
    CLEAR_FIELD(IPCC->C1CR, IPCC_C1CR_RXOIE_Msk | IPCC_C1CR_TXFIE_Msk);
    CLEAR_FIELD(RCC->AHB3ENR, RCC_AHB3ENR_IPCCEN_Msk);
}
