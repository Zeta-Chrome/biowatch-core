#include "queue.h"

void queue_init(queue_t *queue, void *data, uint16_t element_size, uint16_t capacity)
{
    ring_init(queue, data, element_size, capacity); 
}

bool queue_push(queue_t *queue, void *data)
{
    if (queue->count >= queue->capacity)
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
