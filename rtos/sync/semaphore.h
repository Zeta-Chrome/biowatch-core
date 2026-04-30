#ifndef RTOS_SEMAPHORE_H
#define RTOS_SEMAPHORE_H

#include "status.h"
#include "utils/containers/list.h"

typedef struct 
{
    uint32_t max_count;
    uint32_t count;
    list_t wait_queue; // take wait queue
} semaphore_t;

void rtos_semaphore_binary_init(semaphore_t* semaphore); // binary semaphore
void rtos_semaphore_counting_init(semaphore_t* semaphore, uint32_t max_count); // counting semaphore
bw_status_t rtos_semaphore_take(semaphore_t* semaphore, uint32_t timeout_ms);
void rtos_semaphore_give(semaphore_t* semaphore);

#endif
