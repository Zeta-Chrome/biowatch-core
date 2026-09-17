#ifndef KERNEL_TIMER_H
#define KERNEL_TIMER_H

#include "lib/containers/list.h"
#include <stdint.h>

enum kernel_timer_type {
	KERNEL_TIMER_ONE_SHOT,
	KERNEL_TIMER_PERIODIC,
};

typedef void (*kernel_timer_callback)(void *user_data);

struct kernel_timer {
	enum kernel_timer_type type;
	uint64_t ticks;
	kernel_timer_callback callback;
	void *user_data;
	struct list_node node;
	uint64_t rem_ticks;
	bool active;
};

void kernel_timer_init();
uint64_t kernel_timer_ms();
void kernel_timer_start(struct kernel_timer *timer);
void kernel_timer_stop(struct kernel_timer *timer);
uint64_t kernel_timer_deadline_ms();
void kernel_timer_resume();

#endif
