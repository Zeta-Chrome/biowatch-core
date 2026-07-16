#ifndef KERNEL_KERNEL_H
#define KERNEL_KERNEL_H

#include "kernel/sync/event.h"     // IWYU pragma: export
#include "kernel/sync/mqueue.h"    // IWYU pragma: export
#include "kernel/sync/mutex.h"     // IWYU pragma: export
#include "kernel/sync/semaphore.h" // IWYU pragma: export
#include "kernel/task/mem.h"
#include "kernel/task/task.h" // IWYU pragma: export

typedef void (*kernel_idle_hook_t)(void *);

typedef struct
{
    kernel_pool_conf_t pool_confs[MEM_BLOCK_COUNT];
    kernel_idle_hook_t idle_hook;
    void *idle_data;
} kernel_conf_t;

void kernel_init(kernel_conf_t *conf);
void kernel_start();

#endif
