#ifndef RTOS_CRITICAL_H
#define RTOS_CRITICAL_H

#include <stdint.h>
#include "cmsis_gcc.h"

#define HIGHEST_IRQ_PRIO 3 
#define RTOS_ENTER_CRITICAL() rtos_enter_critical()
#define RTOS_EXIT_CRITICAL() rtos_exit_critical()

static uint32_t g_critical_nesting = 0;

static inline void rtos_enter_critical()
{
    __set_BASEPRI(HIGHEST_IRQ_PRIO << 4);
    g_critical_nesting++;
}

static inline void rtos_exit_critical()
{
    g_critical_nesting--;
    if (g_critical_nesting == 0)
    {
        __set_BASEPRI(0);
    }
}

#endif
