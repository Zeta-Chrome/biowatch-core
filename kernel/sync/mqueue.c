#include "kernel/critical.h"
#include "kernel/task/task.h"
#include "kernel/task/task_kernel.h"
#include "lib/containers/list.h"
#include "lib/status.h"
#include "mqueue.h"
#include <string.h>

void kernel_mqueue_init(mqueue_t *mqueue, void *buf, uint16_t length, uint16_t element_size)
{
    queue_init(&mqueue->container, buf, element_size, length);
    list_init(&mqueue->swait_queue);
    list_init(&mqueue->rwait_queue);
}

void kernel_mqueue_peek(mqueue_t *mqueue, void **data)
{
    KERNEL_ENTER_CRITICAL();
    queue_peek(&mqueue->container, data);
    KERNEL_EXIT_CRITICAL();
}

bw_status_t kernel_mqueue_send(mqueue_t *mqueue, void *data, uint32_t timeout_ms)
{
    KERNEL_ENTER_CRITICAL();
    list_node_t *node = mqueue->rwait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        memcpy(tcb->p_msg_data, data, mqueue->container.element_size);

        kernel_task_wake_from_queue(&mqueue->rwait_queue, node);
        kernel_task_remove_delay(&tcb->delay_node);
        tcb->p_msg_data = NULL;
        tcb->exit_status = STATUS_OK;

        KERNEL_EXIT_CRITICAL();
        kernel_task_yield_if_higher();
        return STATUS_OK;
    }

    node = mqueue->swait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        if (queue_push(&mqueue->container, tcb->p_msg_data))
        {
            kernel_task_wake_from_queue(&mqueue->swait_queue, node);
            kernel_task_remove_delay(&tcb->delay_node);
            tcb->p_msg_data = NULL;
            tcb->exit_status = STATUS_OK;
        }
    }
    else if (queue_push(&mqueue->container, data)) // if not full return
    {
        KERNEL_EXIT_CRITICAL();
        return STATUS_OK;
    }

    // Save the data pointer for future push
    get_task_tcb()->p_msg_data = data;

    // block until queue has empty space
    kernel_task_wait_on_queue(&mqueue->swait_queue);
    kernel_task_set_delay(timeout_ms);
    KERNEL_EXIT_CRITICAL();

    kernel_task_yield();

    KERNEL_ENTER_CRITICAL();
    bw_status_t exit_status = get_task_tcb()->exit_status;
    get_task_tcb()->exit_status = STATUS_OK;

    KERNEL_EXIT_CRITICAL();
    return exit_status;
}

bw_status_t kernel_mqueue_receive(mqueue_t *mqueue, void *data, uint32_t timeout_ms)
{
    KERNEL_ENTER_CRITICAL();
    list_node_t *node = mqueue->swait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        memcpy(data, tcb->p_msg_data, mqueue->container.element_size);

        kernel_task_wake_from_queue(&mqueue->swait_queue, node);
        kernel_task_remove_delay(&tcb->delay_node);
        tcb->p_msg_data = NULL;
        tcb->exit_status = STATUS_OK;

        KERNEL_EXIT_CRITICAL();
        kernel_task_yield_if_higher();
        return STATUS_OK;
    }

    node = mqueue->rwait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        if (queue_pop(&mqueue->container, tcb->p_msg_data))
        {
            kernel_task_wake_from_queue(&mqueue->rwait_queue, node);
            kernel_task_remove_delay(&tcb->delay_node);
            tcb->p_msg_data = NULL;
            tcb->exit_status = STATUS_OK;
        }
    }
    else if (queue_pop(&mqueue->container, data))
    {
        KERNEL_EXIT_CRITICAL();
        return STATUS_OK;
    }

    // Save the data pointer for future push
    get_task_tcb()->p_msg_data = data;

    // block until queue has an element
    kernel_task_wait_on_queue(&mqueue->rwait_queue);
    kernel_task_set_delay(timeout_ms);
    KERNEL_EXIT_CRITICAL();

    kernel_task_yield();

    KERNEL_ENTER_CRITICAL();
    bw_status_t exit_status = get_task_tcb()->exit_status;
    get_task_tcb()->exit_status = STATUS_OK;

    KERNEL_EXIT_CRITICAL();
    return exit_status;
}

void kernel_mqueue_overwrite(mqueue_t *mqueue, void *data)
{
    KERNEL_ENTER_CRITICAL();
    ring_push(&mqueue->container, data);
    KERNEL_EXIT_CRITICAL();
}
