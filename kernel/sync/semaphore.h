#ifndef KERNEL_SEMAPHORE_H
#define KERNEL_SEMAPHORE_H

#include "lib/containers/list.h"
#include "lib/status.h"
#include <stdint.h>

struct semaphore {
	uint32_t max_count;
	uint32_t count;
	struct list wait_queue; // take wait queue
};

void kernel_semaphore_binary_init(struct semaphore *semaphore); // binary semaphore
void kernel_semaphore_counting_init(struct semaphore *semaphore,
									uint32_t max_count); // counting semaphore
enum bw_status kernel_semaphore_take(struct semaphore *semaphore, uint32_t timeout_ms);
void kernel_semaphore_give(struct semaphore *semaphore);
void kernel_semaphore_give_from_isr(struct semaphore *semaphore);

#endif
