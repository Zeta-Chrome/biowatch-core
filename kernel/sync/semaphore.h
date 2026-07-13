#ifndef KERNEL_SEMAPHORE_H
#define KERNEL_SEMAPHORE_H

#include "lib/containers/list.h"
#include "lib/status.h"
#include <stdint.h>

typedef struct
{
    uint32_t max_count;
    uint32_t count;
    list_t wait_queue; // take wait queue
} semaphore_t;

void kernel_semaphore_binary_init(semaphore_t *semaphore);                       // binary semaphore
void kernel_semaphore_counting_init(semaphore_t *semaphore, uint32_t max_count); // counting semaphore
bw_status_t kernel_semaphore_take(semaphore_t *semaphore, uint32_t timeout_ms);
void kernel_semaphore_give(semaphore_t *semaphore);
void kernel_semaphore_give_from_isr(semaphore_t *semaphore);

#endif
