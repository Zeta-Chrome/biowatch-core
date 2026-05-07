#ifndef RTOS_TASK_H
#define RTOS_TASK_H

#include <stdint.h>
#include "utils/containers/list.h"
#include "mem.h"

#define MAX_TASKS 32
#define MAX_TASK_PRIORITY 15
#define TASK_NAME_LEN 16
#define MAX_TIMEOUT UINT32_MAX

typedef uint8_t *task_handle_t;
typedef void (*task_func_t)(void *user_data);

typedef enum
{
    TASK_STATE_FREE,
    TASK_STATE_READY,
    TASK_STATE_BLOCKED,
    TASK_STATE_SUSPENDED
} task_state_t;

typedef enum
{
    NOTIFY_ACTION_NONE,
    NOTIFY_ACTION_SET_BITS,
    NOTIFY_ACTION_INCREMENT,
    NOTIFY_ACTION_OVERWRITE
} notify_action_t;

typedef struct
{
    task_stack_t stack;
    char name[TASK_NAME_LEN];
    uint8_t idx;
    task_state_t state;
    uint8_t priority;  // 0 - 15
    list_t *p_state_queue;
    list_t *p_delay_queue;
    list_node_t state_node;  // in ready queue, free queue or some wait queue
    list_node_t delay_node;  // in delay queue
    uint32_t delay_ticks;
    uint32_t event_flags;
    bool event_clear_exit;
    bool event_wait_all;
    uint32_t events_received;
    uint32_t notification_value;
    bool wait_on_notify;
    bool is_notified;
    void *p_msg_data;
    void *p_usr_data;
    bw_status_t exit_status;
} tcb_t;

void rtos_task_init();
void rtos_task_create(task_func_t task_ptr, const char *name, uint8_t priority, uint32_t stack_depth,
                      void *p_user_data, task_handle_t *handle);
bw_status_t rtos_task_notify_wait(uint32_t clear_entry_mask, uint32_t clear_exit_mask,
                                  uint32_t *p_notification, uint32_t timeout_ms);
void rtos_task_notify(task_handle_t handle, uint32_t value, notify_action_t action);
void rtos_task_notify_from_isr(task_handle_t handle, uint32_t value, notify_action_t action);
bool rtos_task_notify_clear(task_handle_t handle);
bool rtos_task_notify_clear_from_isr(task_handle_t handle);
void rtos_task_delay(uint32_t ms);
void rtos_task_yield();
void rtos_task_delete(task_handle_t handle);

#endif
