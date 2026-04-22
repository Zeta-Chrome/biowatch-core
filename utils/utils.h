#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include "stm32wb55xx.h"
#include "status.h"
#include "logger.h"
#include "assert.h"

#define NULL ((void *)0)
static inline void bw_error_handler()
{
    __disable_irq();
    __asm volatile("bkpt #0");
    while (1);
}

#endif
