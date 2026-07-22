#ifndef KERNEL_MQUEUE_H
#define KERNEL_MQUEUE_H

#include "lib/containers/list.h"
#include "lib/containers/queue.h"
#include "lib/status.h"
#include <stddef.h>

struct mqueue {
	queue_t container;
	struct list swait_queue; // send wait queue
	struct list rwait_queue; // receive wait queue
};

void kernel_mqueue_init(struct mqueue *mqueue, void *buf, uint16_t length, uint16_t element_size);
void kernel_mqueue_peek(struct mqueue *mqueue, void **data);
enum bw_status kernel_mqueue_send(struct mqueue *mqueue, void *data, uint32_t timeout_ms);
enum bw_status kernel_mqueue_receive(struct mqueue *mqueue, void *data, uint32_t timeout_ms);
void kernel_mqueue_overwrite(struct mqueue *mqueue, void *data);

#endif
