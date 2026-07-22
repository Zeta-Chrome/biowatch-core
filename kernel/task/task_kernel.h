#ifndef KERNEL_TASK_KERNAL_H
#define KERNEL_TASK_KERNAL_H

#include "kernel/task/task.h"
#include "lib/containers/list.h"

void kernel_task_add_to_ready(struct list_node *node);
void kernel_task_remove_from_ready(struct list_node *node);
void kernel_task_set_delay(uint32_t ms);
void kernel_task_remove_delay(struct list_node *node);
void kernel_task_wait_on_queue(struct list *wait_queue);
void kernel_task_wake_from_queue(struct list *wait_queue, struct list_node *node);
void kernel_task_yield_if_higher();
struct tcb *get_task_tcb();

#endif
