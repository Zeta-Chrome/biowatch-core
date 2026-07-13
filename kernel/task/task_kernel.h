#ifndef KERNEL_TASK_KERNAL_H
#define KERNEL_TASK_KERNAL_H

#include "kernel/task/task.h"
#include "lib/containers/list.h"

void kernel_task_add_to_ready(list_node_t *node);
void kernel_task_remove_from_ready(list_node_t *node);
void kernel_task_set_delay(uint32_t ms);
void kernel_task_remove_delay(list_node_t *node);
void kernel_task_wait_on_queue(list_t *wait_queue);
void kernel_task_wake_from_queue(list_t *wait_queue, list_node_t *node);
void kernel_task_yield_if_higher();
tcb_t *get_task_tcb();

#endif
