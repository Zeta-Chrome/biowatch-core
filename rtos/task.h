#ifndef RTOS_TASK_H
#define RTOS_TASK_H

#include <stdint.h>

#define TASK_NAME_LEN 16

typedef uint32_t task_handle_t;
typedef void (*task_func_t)(void *user_data);

typedef enum
{
    TASK_STATE_DELETED,
    TASK_STATE_READY,
    TASK_STATE_RUNNING,
    TASK_STATE_BLOCKED,
    TASK_STATE_SUSPENDED
} task_state_t;

typedef struct
{
    uint32_t *top_ptr;
    uint32_t size;
} task_stack_t;

typedef struct
{
    char name[TASK_NAME_LEN];
    task_handle_t handle;
    task_state_t state;
    uint8_t priority;  // 4 - 15
    task_stack_t stack;
    void *user_data;
} tcb_t;

task_handle_t rtos_task_create(task_func_t task_ptr, const char *name, uint8_t priority, uint32_t stack_depth,
                               void *user_data);

#endif
