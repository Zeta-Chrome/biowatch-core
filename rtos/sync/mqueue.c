#include "mqueue.h"
#include "containers/list.h"
#include "status.h"
#include "task/task.h"
#include "task/task_kernel.h"
#include "critical.h"
#include <string.h>

void rtos_mqueue_init(mqueue_t *mqueue, void *buf, uint16_t length, uint16_t element_size)
{
    queue_init(&mqueue->container, buf, element_size, length);
    list_init(&mqueue->swait_queue);
    list_init(&mqueue->rwait_queue);
}

void rtos_mqueue_peek(mqueue_t *mqueue, void **data)
{
    RTOS_ENTER_CRITICAL();
    queue_peek(&mqueue->container, data);
    RTOS_EXIT_CRITICAL();
}

bw_status_t rtos_mqueue_send(mqueue_t *mqueue, void *data, uint32_t timeout_ms)
{
    RTOS_ENTER_CRITICAL();
    list_node_t *node = mqueue->rwait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        memcpy(tcb->p_msg_data, data, mqueue->container.element_size);

        rtos_task_wake_from_queue(&mqueue->rwait_queue, node);
        rtos_task_remove_delay(&tcb->delay_node);
        tcb->p_msg_data = NULL;
        tcb->exit_status = STATUS_OK;

        RTOS_EXIT_CRITICAL();
        rtos_task_yield_if_higher();
        return STATUS_OK;
    }

    node = mqueue->swait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        if (queue_push(&mqueue->container, tcb->p_msg_data))
        {
            rtos_task_wake_from_queue(&mqueue->swait_queue, node);
            rtos_task_remove_delay(&tcb->delay_node);
            tcb->p_msg_data = NULL;
            tcb->exit_status = STATUS_OK;
        }
    }
    else if (queue_push(&mqueue->container, data))  // if not full return
    {
        RTOS_EXIT_CRITICAL();
        return STATUS_OK;
    }

    // Save the data pointer for future push
    get_task_tcb()->p_msg_data = data;

    // block until queue has empty space
    rtos_task_wait_on_queue(&mqueue->swait_queue);
    rtos_task_set_delay(timeout_ms);
    RTOS_EXIT_CRITICAL();

    rtos_task_yield();

    RTOS_ENTER_CRITICAL();
    bw_status_t exit_status = get_task_tcb()->exit_status;
    get_task_tcb()->exit_status = STATUS_OK;

    if (exit_status != STATUS_OK)
    {
        list_delete_node(&mqueue->swait_queue, &get_task_tcb()->state_node);
        RTOS_EXIT_CRITICAL();
        return exit_status;
    }
    RTOS_EXIT_CRITICAL();

    return STATUS_OK;
}

bw_status_t rtos_mqueue_receive(mqueue_t *mqueue, void *data, uint32_t timeout_ms)
{
    RTOS_ENTER_CRITICAL();
    list_node_t *node = mqueue->swait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        memcpy(data, tcb->p_msg_data, mqueue->container.element_size);

        rtos_task_wake_from_queue(&mqueue->swait_queue, node);
        rtos_task_remove_delay(&tcb->delay_node);
        tcb->p_msg_data = NULL;
        tcb->exit_status = STATUS_OK;

        RTOS_EXIT_CRITICAL();
        rtos_task_yield_if_higher();
        return STATUS_OK;
    }

    node = mqueue->rwait_queue.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        if (queue_pop(&mqueue->container, tcb->p_msg_data))
        {
            rtos_task_wake_from_queue(&mqueue->rwait_queue, node);
            rtos_task_remove_delay(&tcb->delay_node);
            tcb->p_msg_data = NULL;
            tcb->exit_status = STATUS_OK;
        }
    }
    else if (queue_pop(&mqueue->container, data))
    {
        RTOS_EXIT_CRITICAL();
        return STATUS_OK;
    }

    // Save the data pointer for future push
    get_task_tcb()->p_msg_data = data;

    // block until queue has an element
    rtos_task_wait_on_queue(&mqueue->rwait_queue);
    rtos_task_set_delay(timeout_ms);
    RTOS_EXIT_CRITICAL();

    rtos_task_yield();

    RTOS_ENTER_CRITICAL();
    bw_status_t exit_status = get_task_tcb()->exit_status;
    get_task_tcb()->exit_status = STATUS_OK;

    if (exit_status != STATUS_OK)
    {
        list_delete_node(&mqueue->swait_queue, &get_task_tcb()->state_node);
        RTOS_EXIT_CRITICAL();
        return exit_status;
    }
    RTOS_EXIT_CRITICAL();

    return exit_status;
}

void rtos_mqueue_overwrite(mqueue_t *mqueue, void *data)
{
    RTOS_ENTER_CRITICAL();
    ring_push(&mqueue->container, data);
    RTOS_EXIT_CRITICAL();
}
