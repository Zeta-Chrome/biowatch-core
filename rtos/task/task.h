#ifndef RTOS_TASK_H
#define RTOS_TASK_H

#include <stdint.h>
#include "utils/containers/list.h"
#include "mem.h"

#define MAX_TASKS 32
#define MAX_TASK_PRIORITY 15
#define TASK_NAME_LEN 16

typedef list_node_t* task_handle_t;
typedef void (*task_func_t)(void *user_data);

typedef enum
{
    TASK_STATE_FREE,
    TASK_STATE_READY,
    TASK_STATE_BLOCKED,
    TASK_STATE_SUSPENDED
} task_state_t;

typedef struct
{
    task_stack_t stack;
    char name[TASK_NAME_LEN];
    task_handle_t handle;
    task_state_t state;
    uint32_t delay_ticks;
    uint8_t priority;  // 0 - 15
    void *user_data;
} tcb_t;

void rtos_task_init();
void rtos_task_create(task_func_t task_ptr, const char *name, uint8_t priority,
                               uint32_t stack_depth, void *user_data, task_handle_t *handle);
void rtos_task_delay(uint32_t ms);

#endif
