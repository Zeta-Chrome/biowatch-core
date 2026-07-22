#ifndef UTILS_RING_H
#define UTILS_RING_H

#include "stdbool.h"
#include <stdint.h>

struct ring {
	uint8_t *data;
	uint16_t element_size;
	uint16_t capacity;
	uint16_t head;
	uint16_t tail;
	uint16_t count;
};

void ring_init(struct ring *ring, void *data, uint16_t element_size, uint16_t capacity);
bool ring_peek(struct ring *ring, void **data);
void ring_back(struct ring *ring, void **data);
void ring_push(struct ring *ring, void *data);
bool ring_pop(struct ring *ring, void *out);
bool is_ring_empty(struct ring *ring);

#endif
