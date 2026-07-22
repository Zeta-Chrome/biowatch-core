#include "event.h"
#include "kernel/critical.h"
#include "kernel/task/task.h"
#include "kernel/task/task_kernel.h"
#include "lib/status.h"
#include "lib/utils.h"
#include <stddef.h>

void kernel_event_init(struct event *event)
{
	event->event_flags = 0;
	list_init(&event->wait_queue);
}

static bool event_check_flags(struct event *event, struct tcb *tcb)
{
	if ((tcb->event_wait_all && ((event->event_flags & tcb->event_flags) == tcb->event_flags)) ||
		(!tcb->event_wait_all && (event->event_flags & tcb->event_flags))) {
		tcb->events_received = event->event_flags & tcb->event_flags;
		return true;
	}

	return false;
}

enum bw_status kernel_event_wait(struct event *event, uint32_t event_flags,
								 uint32_t *events_received, bool clear_on_exit, bool wait_for_all,
								 uint32_t timeout_ms)
{
	KERNEL_ENTER_CRITICAL();
	struct tcb *tcb = get_task_tcb();
	tcb->event_flags = event_flags;
	tcb->event_clear_exit = clear_on_exit;
	tcb->event_wait_all = wait_for_all;

	if (event_check_flags(event, tcb)) {
		if (tcb->event_clear_exit)
			event->event_flags &= ~tcb->events_received;

		tcb->event_clear_exit = false;
		tcb->event_wait_all = false;
		if (events_received != NULL)
			*events_received = get_task_tcb()->events_received;

		KERNEL_EXIT_CRITICAL();
		return STATUS_OK;
	}

	if (timeout_ms == 0) {
		KERNEL_EXIT_CRITICAL();
		return STATUS_TIMEOUT;
	}

	kernel_task_wait_on_queue(&event->wait_queue);
	kernel_task_set_delay(timeout_ms);
	KERNEL_EXIT_CRITICAL();

	kernel_task_yield();

	KERNEL_ENTER_CRITICAL();
	enum bw_status exit_status = get_task_tcb()->exit_status;
	get_task_tcb()->exit_status = STATUS_OK;

	if (exit_status == STATUS_OK && events_received != NULL)
		*events_received = get_task_tcb()->events_received;

	KERNEL_EXIT_CRITICAL();

	return exit_status;
}

void kernel_event_set(struct event *event, uint32_t event_flags)
{
	KERNEL_ENTER_CRITICAL();
	event->event_flags |= event_flags;

	bool clear_on_exit = false;
	uint32_t clear_mask = 0;

	struct list_node *node = event->wait_queue.head;
	struct list_node *next;
	struct tcb *tcb;
	while (node != NULL) {
		tcb = node->data;
		next = node->next;
		if (event_check_flags(event, tcb)) {
			if (tcb->event_clear_exit) {
				clear_on_exit = true;
				clear_mask |= tcb->events_received;
			}
			kernel_task_wake_from_queue(&event->wait_queue, node);
			kernel_task_remove_delay(&tcb->delay_node);
			tcb->exit_status = STATUS_OK;
		}

		// cannot do node = node->next here, cause this node is removed and placed in another list
		node = next;
	}

	if (clear_on_exit)
		event->event_flags &= ~clear_mask;

	KERNEL_EXIT_CRITICAL();
	kernel_task_yield_if_higher();
}

void kernel_event_set_from_isr(struct event *event, uint32_t event_flags)
{
	kernel_event_set(event, event_flags);
}

void kernel_event_clear(struct event *event, uint32_t event_flags)
{
	KERNEL_ENTER_CRITICAL();
	event->event_flags &= ~event_flags;
	KERNEL_EXIT_CRITICAL();
}

void kernel_event_clear_from_isr(struct event *event, uint32_t event_flags)
{
	kernel_event_clear(event, event_flags);
}
