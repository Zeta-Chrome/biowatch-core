#include "rtos.h"
#include "critical.h"
#include "logger.h"
#include "lptim/lptim.h"
#include "stm32wb55xx.h"
#include "task/task.h"
#include "utils.h"

void rtos_scheduler_tick();

static void sleep_task(void *user_data)
{
    (void)user_data;
    while (1);
}

void rtos_init(rtos_conf_t *conf)
{
    NVIC_SetPriority(PendSV_IRQn, 15);
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
    NVIC_EnableIRQ(PendSV_IRQn);
    rtos_scheduler_tick();
}
