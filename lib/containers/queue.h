#ifndef UTILS_QUEUE_H
#define UTILS_QUEUE_H

#include "lib/containers/ring.h"
#include <stdbool.h>

typedef ring_t queue_t;

void queue_init(queue_t *queue, void *data, uint16_t element_size, uint16_t capacity);
bool queue_peek(queue_t *queue, void **data);
void queue_back(queue_t *queue, void **data);
bool queue_push(queue_t *queue, void *data);
bool queue_pop(queue_t *queue, void *out);
bool is_queue_empty(queue_t *queue);
bool is_queue_full(queue_t *queue);

#endif
