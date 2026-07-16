#include "critical.h"
#include "kernel.h"
#include "lib/logger.h"
#include "lib/utils.h"
#include "stm32wb55xx.h"
#include "task/task.h"

#ifndef DEBUG
#include "drivers/lptim/lptim.h"
#else
#include "drivers/systick/systick.h"
#endif

static kernel_idle_hook_t g_idle_hook;

void kernel_scheduler_tick();

static void idle_task(void *user_data)
{
    (void)user_data;
    while (1)
    {
        KERNEL_ENTER_CRITICAL();
        g_idle_hook(user_data);
        KERNEL_EXIT_CRITICAL();
    }
}

void kernel_init(kernel_conf_t *conf)
{
    NVIC_SetPriority(PendSV_IRQn, 15);
    NVIC_DisableIRQ(PendSV_IRQn);

    bw_status_t status = kernel_mem_init(conf->pool_confs);
    if (status != STATUS_OK)
    {
        BW_LOG("Exhausted task stack space of %zu", TASK_POOL_SIZE);
        bw_error_handler();
    }

    g_idle_hook = conf->idle_hook;

    kernel_task_init();
    kernel_task_create(idle_task, "_SLEEP_TASK", MAX_TASK_PRIORITY, 32, conf->idle_data, NULL);

#ifdef DEBUG
    systick_init(HIGHEST_IRQ_PRIO); // Systick timer is halted when breakpoint is hit
#else
    lptim_conf_t lptim_conf = {.priority = HIGHEST_IRQ_PRIO, .callback = kernel_scheduler_tick, NULL};
    lptim_init(&lptim_conf);
    lptim_trigger_period(1);
#endif
}

void kernel_start()
{
    NVIC_EnableIRQ(PendSV_IRQn);
    kernel_scheduler_tick();
}
