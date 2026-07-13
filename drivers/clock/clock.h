#ifndef DRIVER_CLOCK_H
#define DRIVER_CLOCK_H

#include "lib/status.h"
#include <stdint.h>

typedef enum
{
    CLOCK_SRC_MSI,
    CLOCK_SRC_HSI,
    CLOCK_SRC_HSE,
    CLOCK_SRC_PLL,
} clock_src_t;

typedef enum
{
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
} clock_msi_range_t;

typedef enum
{
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
} clock_hpre_t;

typedef enum
{
    CLOCK_PPRE_1,
    CLOCK_PPRE_2,
    CLOCK_PPRE_4,
    CLOCK_PPRE_8,
    CLOCK_PPRE_16,
} clock_ppre_t;

typedef struct
{
    clock_src_t src;
    uint8_t mul;
    uint8_t div;
    uint8_t rdiv;
} clock_pllr_conf_t;

typedef struct
{
    clock_src_t src;
    clock_pllr_conf_t pllr;
    clock_msi_range_t msi_range;
    clock_hpre_t hpre;
    clock_ppre_t ppre1;
    clock_ppre_t ppre2;
} clock_conf_t;

extern uint32_t SYSCLK_FREQ;
extern uint32_t HCLK1_FREQ;
extern uint32_t HCLK2_FREQ;
extern uint32_t HCLK4_FREQ;
extern uint32_t PCLK1_FREQ;
extern uint32_t PCLK2_FREQ;

static inline clock_conf_t clock_conf_performance()
{
    return (clock_conf_t){.src = CLOCK_SRC_PLL,
                          .pllr = {.src = CLOCK_SRC_HSE, .mul = 6, .div = 1, .rdiv = 3},
                          .hpre = CLOCK_HPRE_1,
                          .ppre1 = CLOCK_PPRE_4,
                          .ppre2 = CLOCK_PPRE_1};
}

static inline clock_conf_t clock_conf_lp_sleep()
{
    return (clock_conf_t){.src = CLOCK_SRC_MSI,
                          .msi_range = CLOCK_MSI_RANGE_1M,
                          .hpre = CLOCK_HPRE_1,
                          .ppre1 = CLOCK_PPRE_1,
                          .ppre2 = CLOCK_PPRE_1};
}

static inline clock_conf_t clock_conf_stop()
{
    return (clock_conf_t){.src = CLOCK_SRC_MSI,
                          .msi_range = CLOCK_MSI_RANGE_48M,
                          .hpre = CLOCK_HPRE_1,
                          .ppre1 = CLOCK_PPRE_2,
                          .ppre2 = CLOCK_PPRE_1};
}

bw_status_t clock_configure(clock_conf_t *conf);
bw_status_t clock_reconfigure(clock_conf_t *conf);

#endif
