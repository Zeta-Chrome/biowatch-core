#include "kernel.h"
#include "subsys/lpm/lpm.h"
#include "timer.h"
#include "lib/logger.h"
#include "lib/utils.h"
#include "stm32wb55xx.h"
#include "task/task.h"

static kernel_idle_hook_t g_idle_hook;

void kernel_task_tick(uint32_t ms);

static void idle_task(void *user_data)
{
	(void)user_data;
	while (1) {
		g_idle_hook(user_data);
	}
}

void kernel_init(struct kernel_conf *conf)
{
	lpm_disable_mode(LPM_MODE_SLEEP, "KERNEL");
	enum bw_status status = kernel_mem_init(conf->pool_confs);
	if (status != STATUS_OK) {
		BW_LOG("Exhausted task stack space of %zu", TASK_POOL_SIZE);
		bw_error_handler();
	}

	g_idle_hook = conf->idle_hook;

	kernel_task_init();
	kernel_task_create(idle_task, "_IDLE_TASK", MAX_TASK_PRIORITY, conf->idle_task_size,
					   conf->idle_data, NULL);
	NVIC_SetPriority(PendSV_IRQn, 15);
}

void kernel_start()
{
	lpm_enable_mode(LPM_MODE_SLEEP, "KERNEL");
	kernel_timer_init();
	kernel_task_tick(0);
}
