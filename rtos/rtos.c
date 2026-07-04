#include "critical.h"
#include "logger.h"
#include "rtos.h"
#include "stm32wb55xx.h"
#include "systick/systick.h"
#include "task/task.h"
#include "utils.h"

#ifndef DEBUG
#include "lptim/lptim.h"
#endif

static rtos_idle_hook_t g_idle_hook;

void rtos_scheduler_tick();

static void idle_task(void *user_data)
{
    (void)user_data;
    while (1)
    {
        RTOS_ENTER_CRITICAL();
        g_idle_hook(user_data);
        RTOS_EXIT_CRITICAL();
    }
}

void rtos_init(rtos_conf_t *conf)
{
    NVIC_SetPriority(PendSV_IRQn, 15);
    NVIC_DisableIRQ(PendSV_IRQn);

    bw_status_t status = rtos_mem_init(conf->pool_confs);
    if (status != STATUS_OK)
    {
        BW_LOG("Exhausted task stack space of %zu", TASK_POOL_SIZE);
        bw_error_handler();
    }

    g_idle_hook = conf->idle_hook;

    rtos_task_init();
    rtos_task_create(idle_task, "_SLEEP_TASK", MAX_TASK_PRIORITY, 32, conf->idle_data, NULL);

#ifdef DEBUG
    hal_systick_init(HIGHEST_IRQ_PRIO); // Systick timer is halted when breakpoint is hit
#else
    lptim_conf_t lptim_conf = {.priority = HIGHEST_IRQ_PRIO, .callback = rtos_scheduler_tick, NULL};
    hal_lptim_init(&lptim_conf);
    hal_lptim_trigger_period(1);
#endif
}

void rtos_run()
{
    NVIC_EnableIRQ(PendSV_IRQn);
    rtos_scheduler_tick();
}
