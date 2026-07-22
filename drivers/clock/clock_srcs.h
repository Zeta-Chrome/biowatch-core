#ifndef DRIVER_CLOCK_SRCS_H
#define DRIVER_CLOCK_SRCS_H

#include "clock.h"

void clock_enable_msi(enum clock_msi_range range);
void clock_disable_msi();

void clock_enable_hsi();
void clock_disable_hsi();

void clock_enable_hse();
void clock_disable_hse();

void clock_enable_pll();
void clock_disable_pll();

void clock_enable_lse();
void clock_disable_lse();

#endif
