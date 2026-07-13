#ifndef KERNEL_MUTEX_H
#define KERNEL_MUTEX_H

#include "kernel/task/task.h"
#include "lib/containers/list.h"
#include <stdint.h>

typedef struct
{
    list_t wait_queue;
    tcb_t *owner_task;
    uint8_t original_prio;
    uint32_t lock_count;
} mutex_t;

void kernel_mutex_init(mutex_t *mutex);
bw_status_t kernel_mutex_lock(mutex_t *mutex, uint32_t timeout_ms);
bw_status_t kernel_mutex_unlock(mutex_t *mutex);

#endif
