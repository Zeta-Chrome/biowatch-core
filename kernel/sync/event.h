#ifndef KERNEL_EVENT_H
#define KERNEL_EVENT_H

#include "lib/containers/list.h"
#include "lib/status.h"
#include <stdint.h>

typedef struct
{
    uint32_t event_flags;
    list_t wait_queue;
} event_t;

void kernel_event_init(event_t *event);
bw_status_t kernel_event_wait(event_t *event, uint32_t event_flags, uint32_t *events_received, bool clear_on_exit,
                            bool wait_for_all, uint32_t timeout_ms);
void kernel_event_set(event_t *event, uint32_t event_flags);
void kernel_event_set_from_isr(event_t *event, uint32_t event_flags);
void kernel_event_clear(event_t *event, uint32_t event_flags);
void kernel_event_clear_from_isr(event_t *event, uint32_t event_flags);

#endif
