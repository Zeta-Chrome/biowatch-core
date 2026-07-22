#ifndef DRIVER_CLOCK_H
#define DRIVER_CLOCK_H

#include "lib/status.h"
#include <stdint.h>

enum clock_src {
	CLOCK_SRC_MSI,
	CLOCK_SRC_HSI,
	CLOCK_SRC_HSE,
	CLOCK_SRC_PLL,
};

enum clock_msi_range {
	CLOCK_MSI_RANGE_100K,
	CLOCK_MSI_RANGE_200K,
	CLOCK_MSI_RANGE_400K,
	CLOCK_MSI_RANGE_800K,
	CLOCK_MSI_RANGE_1M,
	CLOCK_MSI_RANGE_2M,
	CLOCK_MSI_RANGE_4M,
	CLOCK_MSI_RANGE_8M,
	CLOCK_MSI_RANGE_16M,
	CLOCK_MSI_RANGE_24M,
	CLOCK_MSI_RANGE_32M,
	CLOCK_MSI_RANGE_48M,
};

enum clock_hpre {
	CLOCK_HPRE_1,
	CLOCK_HPRE_3,
	CLOCK_HPRE_5,
	CLOCK_HPRE_6,
	CLOCK_HPRE_10,
	CLOCK_HPRE_32,
	CLOCK_HPRE_2,
	CLOCK_HPRE_4,
	CLOCK_HPRE_8,
	CLOCK_HPRE_16,
	CLOCK_HPRE_64,
	CLOCK_HPRE_128,
	CLOCK_HPRE_256,
	CLOCK_HPRE_512,
};

enum clock_ppre {
	CLOCK_PPRE_1,
	CLOCK_PPRE_2,
	CLOCK_PPRE_4,
	CLOCK_PPRE_8,
	CLOCK_PPRE_16,
};

struct clock_pllr_conf {
	enum clock_src src;
	uint8_t mul;
	uint8_t div;
	uint8_t rdiv;
};

struct clock_conf {
	enum clock_src src;
	struct clock_pllr_conf pllr;
	enum clock_msi_range msi_range;
	enum clock_hpre hpre;
	enum clock_ppre ppre1;
	enum clock_ppre ppre2;
};

extern uint32_t SYSCLK_FREQ;
extern uint32_t HCLK1_FREQ;
extern uint32_t HCLK2_FREQ;
extern uint32_t HCLK4_FREQ;
extern uint32_t PCLK1_FREQ;
extern uint32_t PCLK2_FREQ;

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

enum bw_status clock_configure(struct clock_conf *conf);
enum bw_status clock_reconfigure(struct clock_conf *conf);

#endif
