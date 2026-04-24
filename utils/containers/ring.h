#ifndef UTILS_RING_H
#define UTILS_RING_H

#include "stdbool.h"
#include <stdint.h>

typedef struct
{
    uint8_t *data;
    uint16_t element_size;
    uint16_t capacity;
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} ring_t;

void ring_init(ring_t *ring, void *data, uint16_t element_size, uint16_t capacity);
void ring_push(ring_t *ring, void *data);
bool ring_pop(ring_t *ring, void *out);

#endif
