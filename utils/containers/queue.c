#include "queue.h"
#include "containers/ring.h"

void queue_init(queue_t *queue, void *data, uint16_t element_size, uint16_t capacity)
{
    ring_init(queue, data, element_size, capacity); 
}

bool queue_peek(queue_t* queue, void *data)
{
    return ring_peek(queue, data);
}

bool queue_push(queue_t *queue, void *data)
{
    if (is_queue_full(queue))
    {
        return false;
    }

    ring_push(queue, data);
    return true;
}

bool queue_pop(queue_t *queue, void *out)
{
    return ring_pop(queue, out); 
}

bool is_queue_empty(queue_t *queue)
{
    return is_ring_empty(queue);
}

bool is_queue_full(queue_t *queue)
{
    if (queue->count >= queue->capacity)
    {
        return true;
    }

    return false; 
}

