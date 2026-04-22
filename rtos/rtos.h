#include <stdint.h>
#include "utils/status.h"
#include "stm32wb55xx.h"
#include "task/task.h"
#include "task/mem.h"
#include "cmsis_gcc.h"

#define NULL ((void *)0)
#define MAX_IRQ_PRIORITY (5 << 4)

void rtos_scheduler_tick();

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

static void sleep_task(void *user_data)
{
    (void)user_data;
    while (1);
}

typedef struct 
{
    rtos_pool_conf_t pool_confs[MEM_BLOCK_COUNT];
} rtos_conf_t;

bw_status_t rtos_init(rtos_conf_t *conf)
{
    NVIC_DisableIRQ(PendSV_IRQn);
    bw_status_t status = rtos_mem_init(conf->pool_confs);
    if(status != STATUS_OK)
    {
        return status;
    }
    rtos_task_create(sleep_task, "_SLEEP_TASK", 15, 32, NULL, NULL);

    return STATUS_OK;
}

void rtos_run()
{
    NVIC_SetPriority(PendSV_IRQn, 15);
    NVIC_EnableIRQ(PendSV_IRQn);
    rtos_scheduler_tick();
}
