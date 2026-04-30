#ifndef RTOS_EVENT_H
#define RTOS_EVENT_H

#include "utils/status.h"
#include "utils/containers/list.h"
#include <stdint.h>

typedef struct
{
    uint32_t event_flags;
    list_t wait_queue;
} event_t;

void rtos_event_init(event_t *event);
bw_status_t rtos_event_wait(event_t *event, uint32_t event_flags, uint32_t *events_received,
                            bool clear_on_exit, bool wait_for_all, uint32_t timeout_ms);
void rtos_event_set(event_t *event, uint32_t event_flags);
void rtos_event_clear(event_t *event, uint32_t event_flags);

#endif
