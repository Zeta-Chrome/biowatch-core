#include "timer.h"
#include "critical.h"
#include "lib/containers/list.h"
#include <stddef.h>

static struct list g_timer_queue;

void kernel_timer_init()
{
	list_init(&g_timer_queue);
}

void kernel_timer_register(struct kernel_timer *timer)
{
	timer->node.data = timer;
	timer->rem_ticks = timer->ticks;
	struct list_node *node = g_timer_queue.head;
	struct kernel_timer *t;

	KERNEL_ENTER_CRITICAL();

	while (node != NULL) {
		t = node->data;
		if (t->rem_ticks > timer->rem_ticks) {
			t->rem_ticks -= timer->rem_ticks;
			break;
		}
		timer->rem_ticks -= t->rem_ticks;
		node = node->next;
	}

	if (node == NULL)
		list_push_back(&g_timer_queue, &timer->node);
	else
		list_insert_before(&g_timer_queue, node, &timer->node);

	timer->active = true;
	KERNEL_EXIT_CRITICAL();
}

void kernel_timer_tick()
{
	struct list_node *node = g_timer_queue.head;

	KERNEL_ENTER_CRITICAL();
	if (node != NULL) {
		struct kernel_timer *t = node->data;
		t->rem_ticks--;
		while (t->rem_ticks == 0) {
			t->active = false;
			list_pop_front(&g_timer_queue, NULL);
			if (t->callback)
				t->callback(t->user_data);

			if (t->type == KERNEL_TIMER_PERIODIC)
				kernel_timer_register(t);

			node = g_timer_queue.head;
			if (node == NULL)
				break;

			t = node->data;
		}
	}

	KERNEL_EXIT_CRITICAL();
}

void kernel_timer_start(struct kernel_timer *timer)
{
	KERNEL_ENTER_CRITICAL();

	kernel_timer_stop(timer); // Stops the timer if active
	kernel_timer_register(timer);

	KERNEL_EXIT_CRITICAL();
}

void kernel_timer_stop(struct kernel_timer *timer)
{
	KERNEL_ENTER_CRITICAL();

	if (timer->active) {
		list_delete_node(&g_timer_queue, &timer->node);
		timer->rem_ticks = 0;
		timer->active = false;
	}

	KERNEL_EXIT_CRITICAL();
}
