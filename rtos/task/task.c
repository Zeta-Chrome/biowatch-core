#include "task.h"
#include <stdbool.h>
#include <string.h>
#include "status.h"
#include "task/mem.h"
#include "utils.h"
#include "utils/logger.h"
#include "hal/reg.h"
#include "stm32wb55xx.h"
#include "core_cm4.h"

#define MAX_TASKS 32

tcb_t *g_current_task = NULL;
static tcb_t g_task_table[MAX_TASKS];

void task_stack_init(uint32_t **sp, uint32_t *pc, uint32_t *user_data);

void rtos_task_init()
{

}

bw_status_t rtos_task_create(task_func_t task_ptr, const char *name, uint8_t priority,
                             uint32_t stack_depth, void *user_data, task_handle_t *handle)
{
    static task_handle_t next_handle = 0;

    tcb_t *tcb = &g_task_table[next_handle];
    strncpy(tcb->name, name, 16);
    tcb->state = TASK_STATE_READY;
    tcb->handle = next_handle;
    tcb->priority = priority;
    tcb->user_data = user_data;
    tcb->stack.size = stack_depth;
    bw_status_t status = rtos_mem_alloc(&tcb->stack);
    if (status != STATUS_OK)
    {
        return status;
    }

    task_stack_init(&tcb->stack.stack_ptr, (uint32_t *)task_ptr, (uint32_t *)user_data);
    if (handle)
    {
        *handle = tcb->handle;
    }
    next_handle++;

    return STATUS_OK;
}

void rtos_scheduler_tick()
{
    reg_set_mask(&SCB->ICSR, SCB_ICSR_PENDSVSET_Msk);
}

void task_scheduler()
{
    if (g_current_task != NULL && rtos_check_mem_sanity(&g_current_task->stack) == STATUS_STACK_OVR)
    {
        BW_LOG("[ERROR] Stack overflow detected in %s", g_current_task->name);
        bw_error_handler();
    }

    static uint8_t idx = 0;
    g_current_task = &g_task_table[idx];
    idx = (idx + 1) % 3;
}

void task_exit()
{
    BW_LOG("Task: %s has exited", g_current_task->name);
    bw_error_handler();
}
