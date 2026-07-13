#ifndef KERNEL_MQUEUE_H
#define KERNEL_MQUEUE_H

#include "lib/containers/list.h"
#include "lib/containers/queue.h"
#include "lib/status.h"
#include <stddef.h>

typedef struct
{
    queue_t container;
    list_t swait_queue; // send wait queue
    list_t rwait_queue; // receive wait queue
} mqueue_t;

void kernel_mqueue_init(mqueue_t *mqueue, void *buf, uint16_t length, uint16_t element_size);
void kernel_mqueue_peek(mqueue_t *mqueue, void **data);
bw_status_t kernel_mqueue_send(mqueue_t *mqueue, void *data, uint32_t timeout_ms);
bw_status_t kernel_mqueue_receive(mqueue_t *mqueue, void *data, uint32_t timeout_ms);
void kernel_mqueue_overwrite(mqueue_t *mqueue, void *data);

#endif
