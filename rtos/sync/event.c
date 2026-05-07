#include "event.h"
#include "critical.h"
#include "status.h"
#include "task/task.h"
#include "task/task_kernel.h"
#include "utils.h"

void rtos_event_init(event_t *event)
{
    event->event_flags = 0;
    list_init(&event->wait_queue);
}

static bool event_check_flags(event_t *event, tcb_t *tcb)
{
    if ((tcb->event_wait_all && ((event->event_flags & tcb->event_flags) == tcb->event_flags)) ||
        (!tcb->event_wait_all && (event->event_flags & tcb->event_flags)))
    {
        tcb->events_received = event->event_flags & tcb->event_flags;
        return true;
    }

    return false;
}

bw_status_t rtos_event_wait(event_t *event, uint32_t event_flags, uint32_t *events_received,
                            bool clear_on_exit, bool wait_for_all, uint32_t timeout_ms)
{
    RTOS_ENTER_CRITICAL();
    tcb_t *tcb = get_task_tcb();
    tcb->event_flags = event_flags;
    tcb->event_clear_exit = clear_on_exit;
    tcb->event_wait_all = wait_for_all;

    if (event_check_flags(event, tcb))
    {
        if (tcb->event_clear_exit)
        {
            event->event_flags &= ~tcb->events_received;
        }
        tcb->event_clear_exit = false;
        tcb->event_wait_all = false;
        if (events_received != NULL)
        {
            *events_received = get_task_tcb()->events_received;
        }
        RTOS_EXIT_CRITICAL();
        return STATUS_OK;
    }
    
    if (timeout_ms == 0)
    {
        RTOS_EXIT_CRITICAL();
        return STATUS_TIMEOUT;
    }

    rtos_task_wait_on_queue(&event->wait_queue);
    rtos_task_set_delay(timeout_ms);
    RTOS_EXIT_CRITICAL();

    rtos_task_yield();

    RTOS_ENTER_CRITICAL();
    bw_status_t exit_status = get_task_tcb()->exit_status;
    get_task_tcb()->exit_status = STATUS_OK;

    if (exit_status != STATUS_OK)
    {
        list_delete_node(&event->wait_queue, &get_task_tcb()->state_node);
        RTOS_EXIT_CRITICAL();
        return exit_status;
    }
    else if (events_received != NULL)
    {
        *events_received = get_task_tcb()->events_received;
    }
    RTOS_EXIT_CRITICAL();

    return STATUS_OK;
}

void rtos_event_set(event_t *event, uint32_t event_flags)
{
    RTOS_ENTER_CRITICAL();
    event->event_flags |= event_flags;

    bool clear_on_exit = false;
    uint32_t clear_mask = 0;

    list_node_t *node = event->wait_queue.head;
    tcb_t *tcb;
    while (node != NULL)
    {
        tcb = node->data;
        if (event_check_flags(event, tcb))
        {
            if (tcb->event_clear_exit)
            {
                clear_on_exit = true;
                clear_mask |= tcb->events_received;
            }
            rtos_task_wake_from_queue(&event->wait_queue, node);
            rtos_task_remove_delay(&tcb->delay_node);
            tcb->exit_status = STATUS_OK;
        }

        // cannot do node = node->next ,cause this node is removed and placed in another list
        node = event->wait_queue.head;
    }

    if (clear_on_exit)
    {
        event->event_flags &= ~clear_mask;
    }
    RTOS_EXIT_CRITICAL();
    rtos_task_yield_if_higher();
}

void rtos_event_set_from_isr(event_t *event, uint32_t event_flags)
{
    rtos_event_set(event, event_flags);
}

void rtos_event_clear(event_t *event, uint32_t event_flags)
{
    RTOS_ENTER_CRITICAL();
    event->event_flags &= ~event_flags;
    RTOS_EXIT_CRITICAL();
}

void rtos_event_clear_from_isr(event_t *event, uint32_t event_flags)
{
    rtos_event_clear(event, event_flags);
}
