#include "critical.h"
#include "mutex.h"
#include "task/task_kernel.h"
#include "utils.h"

void rtos_mutex_init(mutex_t *mutex)
{
    mutex->owner_task = NULL;
    mutex->lock_count = 0;
    list_init(&mutex->wait_queue);
}

bw_status_t rtos_mutex_lock(mutex_t *mutex, uint32_t timeout_ms)
{
    RTOS_ENTER_CRITICAL();
    tcb_t *tcb = get_task_tcb();
    if (mutex->lock_count == 0)
    {
        mutex->owner_task = tcb;
        mutex->original_prio = tcb->priority;
        mutex->lock_count++;
        RTOS_EXIT_CRITICAL();
        return STATUS_OK;
    }

    if (mutex->owner_task == tcb)
    {
        mutex->lock_count++;
        RTOS_EXIT_CRITICAL();
        return STATUS_OK;
    }

    if (tcb->priority < mutex->original_prio) // Priority inheritance
    {
        mutex->owner_task->priority = tcb->priority;
    }
    rtos_task_wait_on_queue(&mutex->wait_queue);
    rtos_task_set_delay(timeout_ms);
    RTOS_EXIT_CRITICAL();

    rtos_task_yield();

    RTOS_ENTER_CRITICAL();
    bw_status_t exit_status = get_task_tcb()->exit_status;
    get_task_tcb()->exit_status = STATUS_OK;

    RTOS_EXIT_CRITICAL();
    return exit_status;
}

bw_status_t rtos_mutex_unlock(mutex_t *mutex)
{
    if (get_task_tcb() != mutex->owner_task)
    {
        return STATUS_MUTEX_NOT_OWNER;
    }

    RTOS_ENTER_CRITICAL();
    if (mutex->lock_count > 0)
    {
        mutex->lock_count--;
        if (mutex->lock_count > 0)
        {
            RTOS_EXIT_CRITICAL();
            return STATUS_OK;
        }
    }

    if (mutex->owner_task->priority != mutex->original_prio)
    {
        rtos_task_remove_from_ready(&mutex->owner_task->state_node);
        mutex->owner_task->priority = mutex->original_prio;
        rtos_task_add_to_ready(&mutex->owner_task->state_node);
    }
    mutex->owner_task = NULL;

    list_node_t *node = mutex->wait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;

        rtos_task_wake_from_queue(&mutex->wait_queue, node);
        rtos_task_remove_delay(&tcb->delay_node);
        mutex->owner_task = tcb;
        mutex->original_prio = tcb->priority;
        tcb->exit_status = STATUS_OK;

        RTOS_EXIT_CRITICAL();
        rtos_task_yield_if_higher();
        return STATUS_OK;
    }

    RTOS_EXIT_CRITICAL();
    return STATUS_OK;
}
