#ifndef RTOS_MQUEUE_H
#define RTOS_MQUEUE_H

#include "utils/status.h"
#include "utils/containers/list.h"
#include "utils/containers/queue.h"
#include <stddef.h>

typedef struct 
{
    queue_t container;
    list_t swait_queue; // send wait queue
    list_t rwait_queue; // receive wait queue
} mqueue_t;

void rtos_mqueue_init(mqueue_t *mqueue, void *buf, uint16_t length, uint16_t element_size);
void rtos_mqueue_peek(mqueue_t *mqueue, void **data);
bw_status_t rtos_mqueue_send(mqueue_t *mqueue, void *data, uint32_t timeout_ms);
bw_status_t rtos_mqueue_receive(mqueue_t *mqueue, void *data, uint32_t timeout_ms);
void rtos_mqueue_overwrite(mqueue_t *mqueue, void *data);

#endif
