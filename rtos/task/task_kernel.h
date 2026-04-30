#ifndef RTOS_TASK_KERNAL_H
#define RTOS_TASK_KERNAL_H

#include "task/task.h"
#include "utils/containers/list.h"

void rtos_task_add_to_ready(list_node_t *node);
void rtos_task_remove_from_ready(list_node_t *node);
void rtos_task_set_delay(uint32_t ms);
void rtos_task_remove_delay(list_node_t *node);
void rtos_task_wait_on_queue(list_t *wait_queue);
void rtos_task_wake_from_queue(list_t *wait_queue, list_node_t *node);
void rtos_task_yield_if_higher();
tcb_t* get_task_tcb();

#endif
