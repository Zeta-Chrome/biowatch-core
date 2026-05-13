#ifndef HAL_CLOCK_SRCS_H
#define HAL_CLOCK_SRCS_H

#include "clock.h"

void hal_clock_enable_msi(clock_msi_range_t range);
void hal_clock_disable_msi();

void hal_clock_enable_hsi();
void hal_clock_disable_hsi();

void hal_clock_enable_hse();
void hal_clock_disable_lse();

void hal_clock_enable_pll();
void hal_clock_disable_pll();

void hal_clock_enable_lse();
void hal_clock_disable_lse();

#endif
