#ifndef SUBSYS_LPM_H
#define SUBSYS_LPM_H

#include "drivers/clock/clock.h"
#include "drivers/pwr/pwr.h"

enum lpm_mode {
	LPM_MODE_RUN,
	LPM_MODE_SLEEP,
	LPM_MODE_LP_SLEEP,
	LPM_MODE_STOP,
	LPM_MODE_STANDBY,
	LPM_MODE_SHUTDOWN,
	LPM_MODE_COUNT
};

static inline struct clock_conf clock_conf_performance()
{
	return (struct clock_conf){ .src = CLOCK_SRC_PLL,
								.pllr = { .src = CLOCK_SRC_HSE, .mul = 6, .div = 1, .rdiv = 3 },
								.hpre = CLOCK_HPRE_1,
								.ppre1 = CLOCK_PPRE_4,
								.ppre2 = CLOCK_PPRE_1 };
}

static inline struct clock_conf clock_conf_lp_sleep()
{
	return (struct clock_conf){ .src = CLOCK_SRC_MSI,
								.msi_range = CLOCK_MSI_RANGE_1M,
								.hpre = CLOCK_HPRE_1,
								.ppre1 = CLOCK_PPRE_1,
								.ppre2 = CLOCK_PPRE_1 };
}

static inline struct clock_conf clock_conf_stop()
{
	return (struct clock_conf){ .src = CLOCK_SRC_MSI,
								.msi_range = CLOCK_MSI_RANGE_48M,
								.hpre = CLOCK_HPRE_1,
								.ppre1 = CLOCK_PPRE_2,
								.ppre2 = CLOCK_PPRE_1 };
}

void lpm_enable_mode(enum lpm_mode mode, const char *name);
void lpm_disable_mode(enum lpm_mode mode, const char *name);
void lpm_enter_mode();
void lpm_enter_standby(enum pwr_wkup wkup_pin, enum pwr_wkup_edge edge);
void lpm_enter_shutdown(enum pwr_wkup wkup_pin, enum pwr_wkup_edge edge);

#endif
