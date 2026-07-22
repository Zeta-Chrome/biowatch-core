#ifndef KERNEL_TASK_H
#define KERNEL_TASK_H

#include "lib/containers/list.h"
#include "mem.h"
#include <stdint.h>

#define MAX_TASKS 32
#define MAX_TASK_PRIORITY 15
#define TASK_NAME_LEN 16
#define MAX_TIMEOUT UINT32_MAX

typedef uint8_t *task_handle_t;
typedef void (*task_func_t)(void *user_data);

enum task_state { TASK_STATE_FREE, TASK_STATE_READY, TASK_STATE_BLOCKED, TASK_STATE_SUSPENDED };

enum notify_action {
	NOTIFY_ACTION_NONE,
	NOTIFY_ACTION_SET_BITS,
	NOTIFY_ACTION_INCREMENT,
	NOTIFY_ACTION_OVERWRITE
};

struct tcb {
	struct task_stack stack;
	char name[TASK_NAME_LEN];
	uint8_t idx;
	enum task_state state;
	uint8_t priority; // 0 - 15
	struct list *p_state_queue;
	struct list *p_delay_queue;
	struct list_node state_node; // in ready queue, free queue or some wait queue
	struct list_node delay_node; // in delay queue
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
	enum bw_status exit_status;
};

void kernel_task_init();
void kernel_task_create(task_func_t task_ptr, const char *name, uint8_t priority,
						uint32_t stack_depth, void *p_user_data, task_handle_t *handle);
enum bw_status kernel_task_notify_wait(uint32_t clear_entry_mask, uint32_t clear_exit_mask,
									   uint32_t *p_notification, uint32_t timeout_ms);
void kernel_task_notify(task_handle_t handle, uint32_t value, enum notify_action action);
void kernel_task_notify_from_isr(task_handle_t handle, uint32_t value, enum notify_action action);
bool kernel_task_notify_clear(task_handle_t handle);
bool kernel_task_notify_clear_from_isr(task_handle_t handle);
void kernel_task_delay(uint32_t ms);
void kernel_task_yield();
void kernel_task_delete(task_handle_t handle);

#endif
