#include "drivers/clock/clock_srcs.h"
#include "lib/assert.h"
#include "lib/utils.h"
#include "lptim.h"
#include "stdbool.h"
#include "stm32wb55xx.h"

enum lptim_type { LPTIM_TYPE_PERIOD, LPTIM_TYPE_ONCE };

static struct {
	enum lptim_type type;
	uint16_t ms;
	uint32_t total_ticks;
	union {
		uint32_t rem_ticks;
		uint32_t err_milli_ticks;
	};
	lptim_callback_t callback;
} g_handle;

static void lptim_clock_init(void)
{
	SET_FIELD(PWR->CR1, PWR_CR1_DBP_Msk);
	clock_enable_lse();
	MODIFY_FIELD(RCC->CCIPR, RCC_CCIPR_LPTIM1SEL_Msk, RCC_CCIPR_LPTIM1SEL_Pos, 0x3);
	SET_FIELD(RCC->APB1ENR1, RCC_APB1ENR1_LPTIM1EN_Msk);
	CLEAR_FIELD(PWR->CR1, PWR_CR1_DBP_Msk);
}

void lptim_init(uint8_t priority)
{
	BW_ASSERT(priority < 16, "Invalid priority %d (Expected 0-15)", priority);

	lptim_clock_init();

	CLEAR_FIELD(LPTIM1->CFGR, LPTIM_CFGR_CKSEL_Msk);
	CLEAR_FIELD(LPTIM1->CFGR, LPTIM_CFGR_COUNTMODE_Msk);
	CLEAR_FIELD(LPTIM1->CFGR, LPTIM_CFGR_TRIGEN);

	NVIC_SetPriority(LPTIM1_IRQn, priority);
	NVIC_EnableIRQ(LPTIM1_IRQn);

	SET_FIELD(LPTIM1->CR, LPTIM_CR_ENABLE_Msk);
	SET_FIELD(LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);
}

static inline uint16_t lptim_read_cnt_safe(void)
{
	uint16_t c1, c2;
	do {
		c1 = (uint16_t)LPTIM1->CNT;
		c2 = (uint16_t)LPTIM1->CNT;
	} while (c1 != c2);
	return c1;
}

static inline void lptim_write_cmp(uint16_t target_cmp)
{
	WRITE_FIELD(LPTIM1->ICR, LPTIM_ICR_CMPOKCF_Msk);
	LPTIM1->CMP = target_cmp;
	while (!(LPTIM1->ISR & LPTIM_ISR_CMPOK_Msk))
		;
}

static inline void lptim_set_period(bool is_first)
{
	uint32_t milli_ticks = g_handle.total_ticks + g_handle.err_milli_ticks;
	uint16_t delta_ticks = (uint16_t)(milli_ticks / 1000);
	g_handle.err_milli_ticks = milli_ticks % 1000;

	uint16_t base = is_first ? lptim_read_cnt_safe() : (uint16_t)LPTIM1->CMP;
	lptim_write_cmp((uint16_t)(base + delta_ticks));
}

static inline void lptim_set_once(bool is_first)
{
	uint16_t ticks = (g_handle.rem_ticks > 0xFFFF) ? 0xFFFF : (uint16_t)g_handle.rem_ticks;
	uint16_t base = is_first ? lptim_read_cnt_safe() : (uint16_t)LPTIM1->CMP;

	lptim_write_cmp((uint16_t)(base + ticks));
	g_handle.rem_ticks -= ticks;
}

void lptim_isr(void)
{
	WRITE_FIELD(LPTIM1->ICR, LPTIM_ICR_CMPMCF_Msk);

	if (g_handle.type == LPTIM_TYPE_PERIOD) {
		if (g_handle.callback)
			g_handle.callback(g_handle.ms);
		lptim_set_period(false);
	} else if (g_handle.type == LPTIM_TYPE_ONCE) {
		if (g_handle.rem_ticks > 0) {
			lptim_set_once(false);
			return;
		}
		if (g_handle.callback)
			g_handle.callback(g_handle.ms);
		g_handle.ms = 0;
	}
}

void lptim_trigger_period(uint16_t ms, lptim_callback_t callback)
{
	BW_ASSERT(ms < 2000, "Out of range ms: %d (Expected 0-1999)", ms);

	g_handle.type = LPTIM_TYPE_PERIOD;
	g_handle.ms = ms;
	g_handle.total_ticks = (uint32_t)ms << 15;
	g_handle.err_milli_ticks = 0;
	g_handle.callback = callback;

	lptim_set_period(true);

	WRITE_FIELD(LPTIM1->ICR, LPTIM_ICR_CMPMCF_Msk);
	NVIC_ClearPendingIRQ(LPTIM1_IRQn);
	SET_FIELD(LPTIM1->IER, LPTIM_IER_CMPMIE_Msk);
}

void lptim_trigger_once(uint16_t ms, lptim_callback_t callback)
{
	g_handle.type = LPTIM_TYPE_ONCE;
	g_handle.ms = ms;
	g_handle.total_ticks = DIVC((uint32_t)ms << 15, 1000);
	g_handle.rem_ticks = g_handle.total_ticks;
	g_handle.callback = callback;

	lptim_set_once(true);

	WRITE_FIELD(LPTIM1->ICR, LPTIM_ICR_CMPMCF_Msk);
	NVIC_ClearPendingIRQ(LPTIM1_IRQn);
	SET_FIELD(LPTIM1->IER, LPTIM_IER_CMPMIE_Msk);
}

uint32_t lptim_get_elapsed_ms(void)
{
	if (g_handle.type != LPTIM_TYPE_ONCE || g_handle.total_ticks == 0)
		return 0;

	uint16_t ticks_left_in_chunk = (uint16_t)LPTIM1->CMP - lptim_read_cnt_safe();
	if ((LPTIM1->ISR & LPTIM_ISR_CMPM_Msk) || (ticks_left_in_chunk > 0xFFFE))
		ticks_left_in_chunk = 0;

	uint32_t total_rem_ticks = g_handle.rem_ticks + ticks_left_in_chunk;
	if (total_rem_ticks > g_handle.total_ticks)
		total_rem_ticks = g_handle.total_ticks;

	uint32_t elapsed_ticks = g_handle.total_ticks - total_rem_ticks;
	uint32_t elapsed_ms = (uint32_t)(((uint64_t)elapsed_ticks * 1000ULL) >> 15);

	return (elapsed_ms > g_handle.ms) ? g_handle.ms : elapsed_ms;
}

void lptim_stop_trigger(void)
{
	CLEAR_FIELD(LPTIM1->IER, LPTIM_IER_CMPMIE_Msk);
	WRITE_FIELD(LPTIM1->ICR, LPTIM_ICR_CMPMCF_Msk);
	NVIC_ClearPendingIRQ(LPTIM1_IRQn);
}

void lptim_deinit(void)
{
	CLEAR_FIELD(LPTIM1->CR, LPTIM_CR_ENABLE_Msk);
	CLEAR_FIELD(LPTIM1->IER, LPTIM_IER_CMPMIE_Msk);
	WRITE_FIELD(LPTIM1->ICR, LPTIM_ICR_CMPMCF_Msk);
	NVIC_ClearPendingIRQ(LPTIM1_IRQn);
}
