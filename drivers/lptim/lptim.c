#include "drivers/clock/clock_srcs.h"
#include "lib/assert.h"
#include "lib/utils.h"
#include "lptim.h"
#include "stm32wb55xx.h"

enum lptim_type { LPTIM_TYPE_PERIOD, LPTIM_TYPE_ONCE };

static struct {
	enum lptim_type type;
	union {
		uint32_t rem_ticks;
		struct {
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
	SET_FIELD(PWR->CR1, PWR_CR1_DBP_Msk);
	clock_enable_lse();
	MODIFY_FIELD(RCC->CCIPR, RCC_CCIPR_LPTIM1SEL_Msk, RCC_CCIPR_LPTIM1SEL_Pos, 0x3);
	SET_FIELD(RCC->APB1ENR1, RCC_APB1ENR1_LPTIM1EN_Msk);
	CLEAR_FIELD(PWR->CR1, PWR_CR1_DBP_Msk);
}

void lptim_init(struct lptim_conf *conf)
{
	BW_ASSERT(conf->priority < 60, "Invalid priority %d (Expected 0-15)", conf->priority);
	g_handle.callback = conf->callback;
	g_handle.user_data = conf->user_data;

	lptim_clock_init();

	CLEAR_FIELD(LPTIM1->CFGR, LPTIM_CFGR_CKSEL_Msk); // Clocked by LSE
	CLEAR_FIELD(LPTIM1->CFGR, LPTIM_CFGR_COUNTMODE_Msk); // Clocked by LSE
	CLEAR_FIELD(LPTIM1->CFGR, LPTIM_CFGR_TRIGEN);

	NVIC_SetPriority(LPTIM1_IRQn, conf->priority);
	NVIC_EnableIRQ(LPTIM1_IRQn);

	SET_FIELD(LPTIM1->IER, LPTIM_IER_CMPMIE_Msk);
	SET_FIELD(LPTIM1->CR, LPTIM_CR_ENABLE_Msk);
}

static inline void lptim_write_cmp(uint16_t ticks)
{
	WRITE_FIELD(LPTIM1->ICR, LPTIM_ICR_CMPOKCF_Msk);
	WRITE_FIELD(LPTIM1->CMP, ticks);
	while (!(LPTIM1->ISR & LPTIM_ISR_CMPOK_Msk))
		;
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

void lptim_isr()
{
	WRITE_FIELD(LPTIM1->ICR, LPTIM_ICR_CMPMCF_Msk);

	if (g_handle.type == LPTIM_TYPE_PERIOD) {
		g_handle.callback(g_handle.user_data);
		lptim_set_period();
	} else if (g_handle.type == LPTIM_TYPE_ONCE) {
		if (g_handle.rem_ticks > 0) {
			lptim_set_once();
			return;
		}
		CLEAR_FIELD(LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);
		g_handle.callback(g_handle.user_data);
	}
}

void lptim_trigger_period(uint16_t ms)
{
	BW_ASSERT(ms < 2000, "Out of range ms: %d (Expected 0-1999)", ms);

	CLEAR_FIELD(LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);

	g_handle.type = LPTIM_TYPE_PERIOD;
	g_handle.last_tick_goal = 0;
	g_handle.period.ms = ms;

	lptim_set_period();

	LPTIM1->ICR = LPTIM_ICR_CMPMCF_Msk;

	SET_FIELD(LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);
}

void lptim_trigger_once(uint16_t ms)
{
	CLEAR_FIELD(LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);

	g_handle.type = LPTIM_TYPE_ONCE;
	g_handle.last_tick_goal = 0;
	g_handle.rem_ticks = (ms << 15) / 1000;

	lptim_set_once();

	LPTIM1->ICR = LPTIM_ICR_CMPMCF_Msk;

	SET_FIELD(LPTIM1->CR, LPTIM_CR_CNTSTRT_Msk);
}

void lptim_deinit()
{
	CLEAR_FIELD(LPTIM1->CR, LPTIM_CR_ENABLE_Msk);
	CLEAR_FIELD(LPTIM1->IER, LPTIM_IER_ARRMIE_Msk | LPTIM_IER_CMPMIE_Msk);
}
