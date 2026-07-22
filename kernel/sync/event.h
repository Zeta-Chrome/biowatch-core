#ifndef KERNEL_EVENT_H
#define KERNEL_EVENT_H

#include "lib/containers/list.h"
#include "lib/status.h"
#include <stdint.h>

struct event {
	uint32_t event_flags;
	struct list wait_queue;
};

void kernel_event_init(struct event *event);
enum bw_status kernel_event_wait(struct event *event, uint32_t event_flags,
								 uint32_t *events_received, bool clear_on_exit, bool wait_for_all,
								 uint32_t timeout_ms);
void kernel_event_set(struct event *event, uint32_t event_flags);
void kernel_event_set_from_isr(struct event *event, uint32_t event_flags);
void kernel_event_clear(struct event *event, uint32_t event_flags);
void kernel_event_clear_from_isr(struct event *event, uint32_t event_flags);

#endif
