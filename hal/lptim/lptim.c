#include "lptim.h"
#include "assert.h"
#include "reg.h"
#include "stm32wb55xx.h"
#include "systick/systick.h"

typedef enum
{
    LPTIM_TYPE_PERIOD,
    LPTIM_TYPE_ONCE
} lptim_type_t;

static struct
{
    lptim_type_t type;
    union
    {
        uint32_t rem_ticks;
        struct
        {
            uint16_t ms;
            uint16_t rem_mticks;
        } period;
    };
    uint16_t last_tick_goal;
    lptim_callback_t callback;
    void *user_data;
} g_handle;

static void lptim_clock_init()
{
    // Disable the write protection for RTC
    reg_set_mask(&PWR->CR1, PWR_CR1_DBP_Msk);

    // Enable LSE clock
    reg_set_mask(&RCC->BDCR, RCC_BDCR_LSEON_Msk);
    while (reg_get_bit(&RCC->BDCR, RCC_BDCR_LSERDY_Pos) != 1);

    reg_set_field(&RCC->CCIPR, RCC_CCIPR_LPTIM1SEL_Pos, 2, 0x3);  // Use LSE clock
    reg_set_mask(&RCC->APB1ENR1, RCC_APB1ENR1_LPTIM1EN_Msk);

    // Enable write protection
    reg_clear_mask(&PWR->CR1, PWR_CR1_DBP_Msk);
}

void hal_lptim_init(lptim_conf_t *conf)
{
    BW_ASSERT(conf->priority < 60, "Invalid priority %d (Expected 0-15)", conf->priority);
    g_handle.callback = conf->callback;
    g_handle.user_data = conf->user_data;

    lptim_clock_init();

    reg_clear_mask(&LPTIM1->CFGR, LPTIM_CFGR_CKSEL_Msk);      // Clocked by LSE
    reg_clear_mask(&LPTIM1->CFGR, LPTIM_CFGR_COUNTMODE_Msk);  // Clocked by LSE
    reg_clear_mask(&LPTIM1->CFGR, LPTIM_CFGR_TRIGEN);

    NVIC_SetPriority(LPTIM1_IRQn, conf->priority);
    NVIC_EnableIRQ(LPTIM1_IRQn);

    reg_set_mask(&LPTIM1->IER, LPTIM_IER_CMPMIE_Msk);
    reg_set_mask(&LPTIM1->CR, LPTIM_CR_ENABLE_Msk);
}

static inline void lptim_write_cmp(uint16_t ticks)
{
    LPTIM1->ICR = LPTIM_ICR_CMPOKCF_Msk;
    LPTIM1->CMP = ticks;
    while (!(LPTIM1->ISR & LPTIM_ISR_CMPOK_Msk));
}

static inline void lptim_set_period()
{
    uint32_t mticks = (g_handle.period.ms << 15) + g_handle.period.rem_mticks;
    lptim_write_cmp(mticks / 1000);
    g_handle.period.rem_mticks = mticks % 1000;
}

static inline void lptim_set_once()
{
    uint16_t ticks = g_handle.rem_ticks > 0xFFFF ? 0xFFFF : g_handle.rem_ticks;
    lptim_write_cmp(ticks);
    g_handle.rem_ticks -= ticks;
}

void hal_lptim_isr()
{
    LPTIM1->ICR = LPTIM_ICR_CMPMCF_Msk;

    if (g_handle.type == LPTIM_TYPE_PERIOD)
    {
        g_handle.callback(g_handle.user_data);
        lptim_set_period();
    }
    else if (g_handle.type == LPTIM_TYPE_ONCE)
    {
        if (g_handle.rem_ticks > 0)
        {
            lptim_set_once();
            return;
        }
        reg_clear_mask(&LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);
        g_handle.callback(g_handle.user_data);
    }
}

void hal_lptim_trigger_period(uint16_t ms)
{
    BW_ASSERT(ms < 2000, "Out of range ms: %d (Expected 0-1999)", ms);

    reg_clear_mask(&LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);

    g_handle.type = LPTIM_TYPE_PERIOD;
    g_handle.last_tick_goal = 0;
    g_handle.period.ms = ms;

    lptim_set_period();

    LPTIM1->ICR = LPTIM_ICR_CMPMCF_Msk;

    // Start counting
    reg_set_mask(&LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);
}

void hal_lptim_trigger_once(uint16_t ms)
{
    reg_clear_mask(&LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);

    g_handle.type = LPTIM_TYPE_ONCE;
    g_handle.last_tick_goal = 0;
    g_handle.rem_ticks = (ms << 15) / 1000;

    lptim_set_once();

    LPTIM1->ICR = LPTIM_ICR_CMPMCF_Msk;

    // Start counting
    reg_set_mask(&LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);
}

void hal_lptim_deinit()
{
    reg_clear_mask(&LPTIM1->CR, LPTIM_CR_ENABLE_Msk);
    reg_clear_mask(&LPTIM1->IER, LPTIM_IER_ARRMIE_Msk | LPTIM_IER_CMPMIE_Msk);
}
