#include "semaphore.h"
#include "critical.h"
#include "status.h"
#include "task/task_kernel.h"
#include "utils.h"

void rtos_bsemaphore_init(semaphore_t *semaphore)
{
    semaphore->max_count = 1;
    semaphore->count = 1;
    list_init(&semaphore->wait_queue);
}

void rtos_csemaphore_init(semaphore_t *semaphore, uint32_t max_count)
{
    semaphore->max_count = max_count;
    semaphore->count = max_count;
    list_init(&semaphore->wait_queue);
}

bw_status_t rtos_semaphore_take(semaphore_t *semaphore, uint32_t timeout_ms)
{
    RTOS_ENTER_CRITICAL();
    if (semaphore->count > 0)
    {
        semaphore->count--;
        RTOS_EXIT_CRITICAL();
        return STATUS_OK;
    }

    rtos_task_wait_on_queue(&semaphore->wait_queue);
    rtos_task_set_delay(timeout_ms);
    RTOS_EXIT_CRITICAL();

    rtos_task_yield();

    RTOS_ENTER_CRITICAL();
    bw_status_t exit_status = get_task_tcb()->exit_status;
    get_task_tcb()->exit_status = STATUS_OK;

    if (exit_status != STATUS_OK)
    {
        list_delete_node(&semaphore->wait_queue, &get_task_tcb()->state_node);
        RTOS_EXIT_CRITICAL();
        return exit_status;
    }
    RTOS_EXIT_CRITICAL();

    return exit_status;
}

void rtos_semaphore_give(semaphore_t *semaphore)
{
    RTOS_ENTER_CRITICAL();

    list_node_t *node = semaphore->wait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        rtos_task_wake_from_queue(&semaphore->wait_queue, node);
        rtos_task_remove_delay(&tcb->delay_node);
        tcb->exit_status = STATUS_OK;

        RTOS_EXIT_CRITICAL();
        rtos_task_yield_if_higher();
        return;
    }

    if (semaphore->count < semaphore->max_count)
    {
        semaphore->count++;
    }

    RTOS_EXIT_CRITICAL();
}

void rtos_semaphore_give_from_isr(semaphore_t *semaphore)
{
    rtos_semaphore_give(semaphore);
}
