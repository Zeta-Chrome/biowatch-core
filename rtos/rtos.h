#ifndef RTOS_RTOS_H
#define RTOS_RTOS_H

#include "critical.h"
#include "hal/lptim/lptim.h"
#include "utils/logger.h"
#include "utils/status.h"
#include "stm32wb55xx.h"
#include "task/task.h"
#include "task/mem.h"
#include "utils/utils.h"

void rtos_scheduler_tick();

static void sleep_task(void *user_data)
{
    (void)user_data;
    while (1);
}

typedef struct 
{
    rtos_pool_conf_t pool_confs[MEM_BLOCK_COUNT];
} rtos_conf_t;

void rtos_init(rtos_conf_t *conf)
{
    NVIC_DisableIRQ(PendSV_IRQn); 

    bw_status_t status = rtos_mem_init(conf->pool_confs);
    if(status != STATUS_OK)
    {
        BW_LOG("Exhausted task stack space of %zu", TASK_POOL_SIZE);
        bw_error_handler();
    }
    rtos_task_init();
    rtos_task_create(sleep_task, "_SLEEP_TASK", MAX_TASK_PRIORITY, 32, NULL, NULL);

    lptim_conf_t lptim_conf = {.priority = HIGHEST_IRQ_PRIO + 1, .callback = rtos_scheduler_tick, NULL};
    hal_lptim_init(&lptim_conf);
    hal_lptim_trigger_period(1);
}

void rtos_run()
{
    NVIC_SetPriority(PendSV_IRQn, 15);
    NVIC_EnableIRQ(PendSV_IRQn);
    rtos_scheduler_tick();
}

#endif
