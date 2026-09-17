#include "lpm.h"
#include "cmsis_gcc.h"
#include "drivers/lptim/lptim.h"
#include "drivers/pwr/pwr.h"
#include "drivers/rtc/rtc.h"
#include "kernel/critical.h"
#include "kernel/timer.h"
#include "lib/logger.h"
#include "lib/utils.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
	SLEEP_TIMER_NONE,
	SLEEP_TIMER_LPTIM,
	SLEEP_TIMER_RTC,
} sleep_timer_t;

static volatile sleep_timer_t g_active_sleep_timer = SLEEP_TIMER_NONE;
static uint32_t g_sleep_t0_ms;
static volatile uint64_t g_total_sleep_ms;
static uint32_t g_lp_mode_locks[LPM_MODE_COUNT];

void kernel_timer_tick(uint32_t ticks);

void lpm_enable_mode(enum lpm_mode mode, const char *name)
{
	(void)name;
	uint32_t key = KERNEL_ENTER_CRITICAL();
	if (g_lp_mode_locks[mode] > 0)
		g_lp_mode_locks[mode]--;
	KERNEL_EXIT_CRITICAL(key);
}

void lpm_disable_mode(enum lpm_mode mode, const char *name)
{
	(void)name;
	uint32_t key = KERNEL_ENTER_CRITICAL();
	g_lp_mode_locks[mode]++;
	KERNEL_EXIT_CRITICAL(key);
}

static enum lpm_mode select_mode(uint64_t deadline_ms)
{
	if (deadline_ms == 0)
		return LPM_MODE_RUN;

	for (int i = LPM_MODE_SLEEP; i <= LPM_MODE_STOP; i++) {
		if (g_lp_mode_locks[i] > 0)
			return i - 1;

		if (deadline_ms <= 2)
			return LPM_MODE_SLEEP;
	}
	return LPM_MODE_STOP;
}

static void wake_on_deadline(uint32_t deadline_ms)
{
	if (deadline_ms < 2000) {
		g_active_sleep_timer = SLEEP_TIMER_LPTIM;
		lptim_trigger_once(deadline_ms, NULL); // Wakes from WFI
	} else {
		g_active_sleep_timer = SLEEP_TIMER_RTC;
		g_sleep_t0_ms = rtc_get_time_ms();
		rtc_enable_wut(deadline_ms / 1000, NULL, HIGHEST_IRQ_PRIO); // Wakes from WFI
		lptim_stop_trigger();
	}
}

static void resume_on_wake()
{
	uint32_t elapsed_ms = 0;

	switch (g_active_sleep_timer) {
	case SLEEP_TIMER_LPTIM:
		elapsed_ms = lptim_get_elapsed_ms();
		break;

	case SLEEP_TIMER_RTC:
		elapsed_ms = rtc_get_time_ms() - g_sleep_t0_ms;
		rtc_disable_wut();
		break;

	case SLEEP_TIMER_NONE:
	default:
		return;
	}

	if (elapsed_ms > 0) {
		g_total_sleep_ms += elapsed_ms;
		kernel_timer_tick(elapsed_ms);
	}

	kernel_timer_resume();
}

void lpm_enter_mode()
{
	struct clock_conf clock_conf;

	__disable_irq();
	uint64_t deadline_ms = kernel_timer_deadline_ms();
	enum lpm_mode mode = select_mode(deadline_ms);
	if (mode == LPM_MODE_RUN) {
		__enable_irq();
		return;
	}
	wake_on_deadline(deadline_ms);
	SET_FIELD(RCC->AHB1SMENR, RCC_AHB1SMENR_SRAM1SMEN_Msk);
	SET_FIELD(RCC->AHB3SMENR, RCC_AHB3SMENR_SRAM2SMEN_Msk);

	switch (mode) {
	case LPM_MODE_STOP:
		clock_conf = clock_conf_stop();
		clock_reconfigure(&clock_conf);
		pwr_enter_stop(PWR_STOP_MODE_STOP2);
		clock_conf = clock_conf_performance();
		clock_reconfigure(&clock_conf);
		break;
	case LPM_MODE_LP_SLEEP:
		clock_conf = clock_conf_lp_sleep();
		clock_reconfigure(&clock_conf);
		pwr_enter_lp_sleep();
		pwr_exit_lp_sleep();
		clock_conf = clock_conf_performance();
		clock_reconfigure(&clock_conf);
		break;
	case LPM_MODE_SLEEP:
		pwr_enter_sleep();
		break;
	default:
		break;
	}
	resume_on_wake();
	uint64_t uptime_ms = kernel_timer_ms();
	__enable_irq();

	uint16_t percentx100 = 0;
	if (uptime_ms > 0)
		percentx100 = (uint64_t)(g_total_sleep_ms * 10000) / uptime_ms;
	BW_LOG("Mode: %d - Idle percentage: %llu / %llu = %u.%u%%\n", mode, g_total_sleep_ms, uptime_ms,
		   percentx100 / 100, percentx100 % 100);
}

void lpm_enter_standby(enum pwr_wkup wkup_pin, enum pwr_wkup_edge edge)
{
	if (g_lp_mode_locks[LPM_MODE_STANDBY] > 0)
		return;

	pwr_enable_wkup(wkup_pin, edge);
	pwr_enter_standby();
}

void lpm_enter_shutdown(enum pwr_wkup wkup_pin, enum pwr_wkup_edge edge)
{
	if (g_lp_mode_locks[LPM_MODE_SHUTDOWN] > 0)
		return;

	pwr_enable_wkup(wkup_pin, edge);
	pwr_enter_shutdown();
}
