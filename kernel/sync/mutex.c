#include "kernel/critical.h"
#include "kernel/task/task_kernel.h"
#include "lib/utils.h"
#include "mutex.h"
#include <stddef.h>

void kernel_mutex_init(struct mutex *mutex)
{
	mutex->owner_task = NULL;
	mutex->lock_count = 0;
	list_init(&mutex->wait_queue);
}

enum bw_status kernel_mutex_lock(struct mutex *mutex, uint32_t timeout_ms)
{
	KERNEL_ENTER_CRITICAL();
	struct tcb *tcb = get_task_tcb();
	if (mutex->lock_count == 0) {
		mutex->owner_task = tcb;
		mutex->original_prio = tcb->priority;
		mutex->lock_count++;
		KERNEL_EXIT_CRITICAL();
		return STATUS_OK;
	}

	if (mutex->owner_task == tcb) {
		mutex->lock_count++;
		KERNEL_EXIT_CRITICAL();
		return STATUS_OK;
	}

	if (tcb->priority < mutex->original_prio) // Priority inheritance
		mutex->owner_task->priority = tcb->priority;

	kernel_task_wait_on_queue(&mutex->wait_queue);
	kernel_task_set_delay(timeout_ms);
	KERNEL_EXIT_CRITICAL();

	kernel_task_yield();

	KERNEL_ENTER_CRITICAL();
	enum bw_status exit_status = get_task_tcb()->exit_status;
	get_task_tcb()->exit_status = STATUS_OK;

	KERNEL_EXIT_CRITICAL();
	return exit_status;
}

enum bw_status kernel_mutex_unlock(struct mutex *mutex)
{
	if (get_task_tcb() != mutex->owner_task)
		return STATUS_MUTEX_NOT_OWNER;

	KERNEL_ENTER_CRITICAL();
	if (mutex->lock_count > 0) {
		mutex->lock_count--;
		if (mutex->lock_count > 0) {
			KERNEL_EXIT_CRITICAL();
			return STATUS_OK;
		}
	}

	if (mutex->owner_task->priority != mutex->original_prio) {
		kernel_task_remove_from_ready(&mutex->owner_task->state_node);
		mutex->owner_task->priority = mutex->original_prio;
		kernel_task_add_to_ready(&mutex->owner_task->state_node);
	}
	mutex->owner_task = NULL;

	struct list_node *node = mutex->wait_queue.head;
	if (node != NULL) {
		struct tcb *tcb = node->data;

		kernel_task_wake_from_queue(&mutex->wait_queue, node);
		kernel_task_remove_delay(&tcb->delay_node);
		mutex->owner_task = tcb;
		mutex->original_prio = tcb->priority;
		tcb->exit_status = STATUS_OK;

		KERNEL_EXIT_CRITICAL();
		kernel_task_yield_if_higher();
		return STATUS_OK;
	}

	KERNEL_EXIT_CRITICAL();
	return STATUS_OK;
}
