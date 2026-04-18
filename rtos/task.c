#include "rtos/task.h"
#include <string.h>

#define MAX_TASKS 16
#define STACK_SIZE 1024

static tcb_t tcb_registry[MAX_TASKS];
static uint32_t stack_pool[MAX_TASKS][STACK_SIZE];

void task_stack_init(uint32_t *sp, uint32_t *pc);
void task_switch(uint32_t *sp);

task_handle_t rtos_task_create(task_func_t task_ptr, const char *name, uint8_t priority,
                               uint32_t stack_depth, void *user_data)
{
    static task_handle_t handle = 0;

    tcb_t *tcb = &tcb_registry[handle];
    strncpy(tcb->name, name, 16);
    tcb->state = TASK_STATE_READY;
    tcb->handle = handle;
    tcb->priority = priority;
    tcb->stack.top_ptr = stack_pool[handle];
    tcb->stack.size = stack_depth;
    tcb->user_data = user_data;

    // task_stack_init(tcb->stack.top_ptr, (uint32_t *)task_ptr);

    handle++;

    return tcb->handle;
}

void scheduler_tick()
{
    static uint8_t idx = 0;
    tcb_t *tcb = &tcb_registry[(idx + 1) % 2];
    // task_switch(tcb->stack.top_ptr);
}
