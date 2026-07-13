#include "kernel/critical.h"
#include "kernel/task/task_kernel.h"
#include "lib/status.h"
#include "lib/utils.h"
#include "semaphore.h"

void kernel_semaphore_binary_init(semaphore_t *semaphore)
{
    semaphore->max_count = 1;
    semaphore->count = 1;
    list_init(&semaphore->wait_queue);
}

void kernel_semaphore_counting_init(semaphore_t *semaphore, uint32_t max_count)
{
    semaphore->max_count = max_count;
    semaphore->count = max_count;
    list_init(&semaphore->wait_queue);
}

bw_status_t kernel_semaphore_take(semaphore_t *semaphore, uint32_t timeout_ms)
{
    KERNEL_ENTER_CRITICAL();
    if (semaphore->count > 0)
    {
        semaphore->count--;
        KERNEL_EXIT_CRITICAL();
        return STATUS_OK;
    }

    kernel_task_wait_on_queue(&semaphore->wait_queue);
    kernel_task_set_delay(timeout_ms);
    KERNEL_EXIT_CRITICAL();

    kernel_task_yield();

    KERNEL_ENTER_CRITICAL();
    bw_status_t exit_status = get_task_tcb()->exit_status;
    get_task_tcb()->exit_status = STATUS_OK;

    KERNEL_EXIT_CRITICAL();
    return exit_status;
}

void kernel_semaphore_give(semaphore_t *semaphore)
{
    KERNEL_ENTER_CRITICAL();

    list_node_t *node = semaphore->wait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        kernel_task_wake_from_queue(&semaphore->wait_queue, node);
        kernel_task_remove_delay(&tcb->delay_node);
        tcb->exit_status = STATUS_OK;

        KERNEL_EXIT_CRITICAL();
        kernel_task_yield_if_higher();
        return;
    }

    if (semaphore->count < semaphore->max_count)
    {
        semaphore->count++;
    }

    KERNEL_EXIT_CRITICAL();
}

void kernel_semaphore_give_from_isr(semaphore_t *semaphore)
{
    kernel_semaphore_give(semaphore);
}
