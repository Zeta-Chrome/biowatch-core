#ifndef KERNEL_KERNEL_H
#define KERNEL_KERNEL_H

#include "kernel/sync/event.h" // IWYU pragma: export
#include "kernel/sync/mqueue.h" // IWYU pragma: export
#include "kernel/sync/mutex.h" // IWYU pragma: export
#include "kernel/sync/semaphore.h" // IWYU pragma: export
#include "kernel/task/mem.h"
#include "kernel/task/task.h" // IWYU pragma: export

typedef void (*kernel_idle_hook_t)(void *);

struct kernel_conf {
	struct kernel_pool_conf pool_confs[MEM_BLOCK_COUNT];
	kernel_idle_hook_t idle_hook;
	void *idle_data;
};

void kernel_init(struct kernel_conf *conf);
void kernel_start();

#endif
