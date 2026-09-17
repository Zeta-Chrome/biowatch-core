#include "timer.h"
#include "critical.h"
#include "drivers/lptim/lptim.h"
#include "kernel/task/task.h"
#include "kernel/task/task_kernel.h"
#include "lib/containers/list.h"
#include "lib/utils.h"
#include <stddef.h>

static volatile uint64_t g_tick_ms = 0;
static struct list g_timer_queue;

void kernel_task_tick(uint32_t ticks);
void kernel_timer_tick(uint32_t ticks);

void kernel_timer_init()
{
#ifdef DEBUG
	SET_FIELD(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_LPTIM1_STOP);
#endif
	lptim_init(HIGHEST_IRQ_PRIO);
	lptim_trigger_period(1, kernel_timer_tick);
	list_init(&g_timer_queue);
}

uint64_t kernel_timer_ms()
{
	return g_tick_ms;
}

static void add_timer(struct kernel_timer *timer)
{
	uint32_t key = KERNEL_ENTER_CRITICAL();

	timer->active = true;
	if (timer->ticks == MAX_TIMEOUT) {
		KERNEL_EXIT_CRITICAL(key);
		return;
	}

	struct list_node *node;
	struct kernel_timer *t;

	node = g_timer_queue.head;
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

	KERNEL_EXIT_CRITICAL(key);
}

void kernel_timer_start(struct kernel_timer *timer)
{
	uint32_t key = KERNEL_ENTER_CRITICAL();
	kernel_timer_stop(timer); // Stops the timer if active
	timer->node.data = timer;
	timer->rem_ticks = timer->ticks;
	add_timer(timer);
	KERNEL_EXIT_CRITICAL(key);
}

static void remove_timer(struct kernel_timer *timer)
{
	uint32_t key = KERNEL_ENTER_CRITICAL();

	timer->active = false;

	struct list_node *node = g_timer_queue.head;
	bool found = false;
	while (node) {
		if (node == &timer->node) {
			found = true;
			break;
		}
		node = node->next;
	}

	if (timer->ticks < MAX_TIMEOUT) {
		node = timer->node.next;
		if (node) {
			struct kernel_timer *t = node->data;
			t->rem_ticks += timer->rem_ticks;
		}
	}

	if (found)
		list_delete_node(&g_timer_queue, &timer->node);

	KERNEL_EXIT_CRITICAL(key);
}

void kernel_timer_tick(uint32_t ticks)
{
	g_tick_ms += ticks;

	struct list expired_timers;
	list_init(&expired_timers);

	struct list_node *node = g_timer_queue.head;

	uint32_t key = KERNEL_ENTER_CRITICAL();
	if (node != NULL) {
		struct kernel_timer *t = node->data;
		t->rem_ticks = MAX((int)t->rem_ticks - (int)ticks, 0);
		while (t->rem_ticks == 0) {
			t->active = false;
			list_pop_front(&g_timer_queue, NULL);
			list_push_back(&expired_timers, &t->node);

			node = g_timer_queue.head;
			if (node == NULL)
				break;

			t = node->data;
		}
	}
	KERNEL_EXIT_CRITICAL(key);

	while (expired_timers.head != NULL) {
		list_pop_front(&expired_timers, &node);
		struct kernel_timer *t = node->data;
		t->callback(t->user_data);

		if (t->type == KERNEL_TIMER_PERIODIC)
			kernel_timer_start(t);
	}

	kernel_task_tick(ticks);
}

void kernel_timer_stop(struct kernel_timer *timer)
{
	if (!timer->active)
		return;

	uint32_t key = KERNEL_ENTER_CRITICAL();
	remove_timer(timer);
	timer->rem_ticks = 0;
	timer->active = false;
	KERNEL_EXIT_CRITICAL(key);
}

uint64_t kernel_timer_deadline_ms()
{
	uint32_t key = KERNEL_ENTER_CRITICAL();
	uint64_t deadline = kernel_task_deadline_ms();
	struct list_node *node = g_timer_queue.head;
	if (node) {
		struct kernel_timer *t = node->data;
		deadline = MIN(deadline, t->rem_ticks);
	}
	KERNEL_EXIT_CRITICAL(key);

	return deadline;
}

void kernel_timer_resume()
{
	lptim_trigger_period(1, kernel_timer_tick);
}
