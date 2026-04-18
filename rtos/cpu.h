#include <stdint.h>
#include "cmsis_gcc.h"

#define MAX_IRQ_PRIORITY (5 << 4)

static uint32_t g_critical_nesting = 0;

static inline void rtos_enter_critical()
{
    __set_BASEPRI(MAX_IRQ_PRIORITY);
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
