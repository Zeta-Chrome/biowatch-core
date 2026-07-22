#ifndef KERNEL_MUTEX_H
#define KERNEL_MUTEX_H

#include "kernel/task/task.h"
#include "lib/containers/list.h"
#include <stdint.h>

struct mutex {
	struct list wait_queue;
	struct tcb *owner_task;
	uint8_t original_prio;
	uint32_t lock_count;
};

void kernel_mutex_init(struct mutex *mutex);
enum bw_status kernel_mutex_lock(struct mutex *mutex, uint32_t timeout_ms);
enum bw_status kernel_mutex_unlock(struct mutex *mutex);

#endif
