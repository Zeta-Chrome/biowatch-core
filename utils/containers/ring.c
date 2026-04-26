#include "ring.h"
#include <string.h>

void ring_init(ring_t *ring, void *data, uint16_t element_size, uint16_t capacity)
{
    ring->data = data;
    ring->element_size = element_size;
    ring->capacity = capacity;
    ring->head = 0;
    ring->tail = 0;
    ring->count = 0;
}

bool ring_peek(ring_t *ring, void *data)
{
    if (ring->head == ring->tail)
    {
        return false;
    }

    void *src = ring->data + ring->head * ring->element_size;
    memcpy(data, src, ring->element_size);

    return true;
}

void ring_push(ring_t *ring, void *data)
{
    void *dest = ring->data + ring->tail * ring->element_size;
    memcpy(dest, data, ring->element_size);

    ring->tail = (ring->tail + 1) % ring->capacity;
    if (ring->count >= ring->capacity)
    {
        ring->head = (ring->head + 1) % ring->capacity; // overwrite old data
    }
    else
    {
        ring->count++;
    }
}

bool ring_pop(ring_t *ring, void *out)
{
    if (ring->head == ring->tail)
    {
        return false;
    }

    void *src = ring->data + ring->head * ring->element_size;
    memcpy(out, src, ring->element_size);

    ring->head = (ring->head + 1) % ring->capacity;
    ring->count--;

    return true;
}
