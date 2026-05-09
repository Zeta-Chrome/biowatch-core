#ifndef RTOS_CRITICAL_H
#define RTOS_CRITICAL_H

#include <stdint.h>
#include "cmsis_gcc.h"
#include "stm32wb55xx.h"

#define HIGHEST_IRQ_PRIO 3
#define RTOS_ENTER_CRITICAL() rtos_enter_critical()
#define RTOS_EXIT_CRITICAL() rtos_exit_critical()

static uint32_t g_critical_nesting = 0;
static uint32_t g_isr_mask_state = 0;

static inline void rtos_enter_critical()
{
    if (g_critical_nesting == 0)
    {
        g_isr_mask_state = __get_BASEPRI();
    }
    __set_BASEPRI(HIGHEST_IRQ_PRIO << (8 - __NVIC_PRIO_BITS));
    g_critical_nesting++;
}

static inline void rtos_exit_critical()
{
    g_critical_nesting--;
    if (g_critical_nesting == 0)
    {
        __set_BASEPRI(g_isr_mask_state);
    }
}

#endif
