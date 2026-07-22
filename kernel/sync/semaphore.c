#include "kernel/critical.h"
#include "kernel/task/task_kernel.h"
#include "lib/status.h"
#include "lib/utils.h"
#include "semaphore.h"
#include <stddef.h>

void kernel_semaphore_binary_init(struct semaphore *semaphore)
{
	semaphore->max_count = 1;
	semaphore->count = 1;
	list_init(&semaphore->wait_queue);
}

void kernel_semaphore_counting_init(struct semaphore *semaphore, uint32_t max_count)
{
	semaphore->max_count = max_count;
	semaphore->count = max_count;
	list_init(&semaphore->wait_queue);
}

enum bw_status kernel_semaphore_take(struct semaphore *semaphore, uint32_t timeout_ms)
{
	KERNEL_ENTER_CRITICAL();
	if (semaphore->count > 0) {
		semaphore->count--;
		KERNEL_EXIT_CRITICAL();
		return STATUS_OK;
	}

	kernel_task_wait_on_queue(&semaphore->wait_queue);
	kernel_task_set_delay(timeout_ms);
	KERNEL_EXIT_CRITICAL();

	kernel_task_yield();

	KERNEL_ENTER_CRITICAL();
	enum bw_status exit_status = get_task_tcb()->exit_status;
	get_task_tcb()->exit_status = STATUS_OK;

	KERNEL_EXIT_CRITICAL();
	return exit_status;
}

void kernel_semaphore_give(struct semaphore *semaphore)
{
	KERNEL_ENTER_CRITICAL();

	struct list_node *node = semaphore->wait_queue.head;
	if (node != NULL) {
		struct tcb *tcb = node->data;
		kernel_task_wake_from_queue(&semaphore->wait_queue, node);
		kernel_task_remove_delay(&tcb->delay_node);
		tcb->exit_status = STATUS_OK;

		KERNEL_EXIT_CRITICAL();
		kernel_task_yield_if_higher();
		return;
	}

	if (semaphore->count < semaphore->max_count)
		semaphore->count++;

	KERNEL_EXIT_CRITICAL();
}

void kernel_semaphore_give_from_isr(struct semaphore *semaphore)
{
	kernel_semaphore_give(semaphore);
}
