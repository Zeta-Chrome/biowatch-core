#include "task.h"
#include <stdbool.h>
#include <string.h>
#include "cmsis_gcc.h"
#include "critical.h"
#include "status.h"
#include "task/mem.h"
#include "utils.h"
#include "utils/logger.h"
#include "hal/reg.h"
#include "containers/clist.h"
#include "containers/list.h"

typedef uint16_t prio_msk_t;

static struct
{
    clist_t ready_queues[MAX_TASK_PRIORITY + 1];
    list_t free_queue;
    list_t delay_queue;
} g_task_manager;
static tcb_t g_task_table[MAX_TASKS];
static prio_msk_t g_priority_mask = 0;
tcb_t *g_current_task = NULL;
tcb_t *g_next_task = NULL;

void task_stack_init(uint32_t **sp, uint32_t *pc, uint32_t *p_usr_data);

void rtos_task_init()
{
    for (int i = 0; i <= MAX_TASK_PRIORITY; i++)
    {
        clist_init(&g_task_manager.ready_queues[i]);
    }
    list_init(&g_task_manager.free_queue);
    list_init(&g_task_manager.delay_queue);

    for (int i = 0; i < MAX_TASKS; i++)
    {
        g_task_table[i].idx = i;
        g_task_table[i].state_node.data = &g_task_table[i];
        g_task_table[i].delay_node.data = &g_task_table[i];
        g_task_table[i].p_state_queue = &g_task_manager.free_queue;
        g_task_table[i].p_delay_queue = NULL;
        list_push_back(&g_task_manager.free_queue, &g_task_table[i].state_node);
    }
}

void rtos_task_create(task_func_t task_ptr, const char *name, uint8_t priority, uint32_t stack_depth,
                      void *p_usr_data, task_handle_t *handle)
{
    BW_ASSERT(priority < 16, "Invalid priority : %d, (Expected range (0-15))");

    list_node_t *node;
    list_pop_front(&g_task_manager.free_queue, &node);
    if (node == NULL)
    {
        BW_LOG("Exceeded the maximum task count : %d", MAX_TASKS);
        bw_error_handler();
    }
    clist_push_back(&g_task_manager.ready_queues[priority], node);
    g_priority_mask |= 1 << priority;

    tcb_t *tcb = node->data;
    strncpy(tcb->name, name, 16);
    tcb->state = TASK_STATE_READY;
    tcb->priority = priority;
    tcb->p_usr_data = p_usr_data;
    tcb->stack.size = stack_depth;
    bw_status_t status = rtos_mem_alloc(&tcb->stack);
    if (status != STATUS_OK)
    {
        BW_LOG("Ran out of memory blocks");
        bw_error_handler();
    }

    task_stack_init(&tcb->stack.stack_ptr, (uint32_t *)task_ptr, (uint32_t *)p_usr_data);
    if (handle)
    {
        *handle = &tcb->idx;
    }
}

void rtos_task_add_to_ready(list_node_t *node)
{
    RTOS_ENTER_CRITICAL();
    tcb_t *tcb = node->data;
    clist_push_back(&g_task_manager.ready_queues[tcb->priority], node);

    tcb->state = TASK_STATE_READY;
    tcb->p_state_queue = &g_task_manager.ready_queues[tcb->priority];
    g_priority_mask |= 1 << tcb->priority;
    RTOS_EXIT_CRITICAL();
}

void rtos_task_remove_from_ready(list_node_t *node)
{
    RTOS_ENTER_CRITICAL();
    tcb_t *tcb = node->data;
    clist_delete_node(&g_task_manager.ready_queues[tcb->priority], node);
    if (g_task_manager.ready_queues[tcb->priority].count == 0)
    {
        g_priority_mask &= ~(1 << tcb->priority);
    }
    tcb->state = TASK_STATE_BLOCKED;
    RTOS_EXIT_CRITICAL();
}

void rtos_task_set_delay(uint32_t ms)
{
    if (ms == MAX_TIMEOUT)
    {
        g_current_task->delay_ticks = MAX_TIMEOUT;
        return;
    }

    tcb_t *tcb;
    list_node_t *node = g_task_manager.delay_queue.head;

    RTOS_ENTER_CRITICAL();
    while (node != NULL)
    {
        tcb = node->data;
        if (tcb->delay_ticks > ms)
        {
            tcb->delay_ticks -= ms;
            break;
        }
        ms -= tcb->delay_ticks;
        node = node->next;
    }

    if (node == NULL)
    {
        list_push_back(&g_task_manager.delay_queue, &g_current_task->delay_node);
    }
    else
    {
        list_insert_before(&g_task_manager.delay_queue, node, &g_current_task->delay_node);
    }

    g_current_task->delay_ticks = ms;
    g_current_task->p_delay_queue = &g_task_manager.delay_queue;

    RTOS_EXIT_CRITICAL();
}

void rtos_task_remove_delay(list_node_t *node)
{
    tcb_t *tcb = node->data;
    list_node_t *next_node = tcb->delay_node.next;

    if (tcb->delay_ticks == MAX_TIMEOUT)
    {
        tcb->delay_ticks = 0;
        return;
    }

    RTOS_ENTER_CRITICAL();
    if (next_node != NULL)
    {
        ((tcb_t *)next_node->data)->delay_ticks += tcb->delay_ticks;
    }
    tcb->delay_ticks = 0;
    tcb->p_delay_queue = NULL;
    list_delete_node(&g_task_manager.delay_queue, &tcb->delay_node);
    RTOS_EXIT_CRITICAL();
}

void rtos_task_wait_on_queue(list_t *wait_queue)
{
    RTOS_ENTER_CRITICAL();
    g_current_task->p_state_queue = wait_queue;
    rtos_task_remove_from_ready(&g_current_task->state_node);

    tcb_t *tcb;
    list_node_t *node = wait_queue->head;
    while (node != NULL)
    {
        tcb = node->data;
        if (tcb->priority > g_current_task->priority)
        {
            break;
        }
        node = node->next;
    }

    if (node == NULL)
    {
        list_push_back(wait_queue, &g_current_task->state_node);
    }
    else
    {
        list_insert_before(wait_queue, node, &g_current_task->state_node);
    }
    RTOS_EXIT_CRITICAL();
}

void rtos_task_yield()
{
    uint8_t highest_prio = __CLZ(__RBIT(g_priority_mask));
    if (g_current_task == g_task_manager.ready_queues[highest_prio].head->data &&
        g_task_manager.ready_queues[highest_prio].count == 1)
    {
        return;
    }
    reg_set_mask(&SCB->ICSR, SCB_ICSR_PENDSVSET_Msk);
}

void rtos_task_yield_if_higher()
{
    if (__CLZ(__RBIT(g_priority_mask)) > g_current_task->priority)
    {
        rtos_task_yield();
    }
}

void rtos_task_wake_from_queue(list_t *wait_queue, list_node_t *node)
{
    RTOS_ENTER_CRITICAL();
    list_delete_node(wait_queue, node);
    rtos_task_add_to_ready(node);
    RTOS_EXIT_CRITICAL();
}

bw_status_t rtos_task_notify_wait(uint32_t clear_entry_mask, uint32_t clear_exit_mask,
                                  uint32_t *p_notification, uint32_t timeout_ms)
{
    RTOS_ENTER_CRITICAL();
    g_current_task->notification_value &= ~clear_entry_mask;

    if (g_current_task->is_notified)
    {
        g_current_task->is_notified = false;
        if (p_notification != NULL)
        {
            *p_notification = g_current_task->notification_value;
        }
        g_current_task->notification_value &= ~clear_exit_mask;
        g_current_task->exit_status = STATUS_OK;

        RTOS_EXIT_CRITICAL();
        return STATUS_OK;
    }

    g_current_task->wait_on_notify = true;
    rtos_task_remove_from_ready(&g_current_task->state_node);
    rtos_task_set_delay(timeout_ms);
    RTOS_EXIT_CRITICAL();

    rtos_task_yield();

    RTOS_ENTER_CRITICAL();
    bw_status_t exit_status = g_current_task->exit_status;
    g_current_task->exit_status = STATUS_OK;

    if (exit_status == STATUS_OK)
    {
        if (p_notification != NULL)
        {
            *p_notification = g_current_task->notification_value;
        }
        g_current_task->notification_value &= ~clear_exit_mask;
    }
    RTOS_EXIT_CRITICAL();

    return exit_status;
}

void rtos_task_notify(task_handle_t handle, uint32_t value, notify_action_t action)
{
    tcb_t *tcb = handle != NULL ? &g_task_table[*handle] : g_current_task;

    RTOS_ENTER_CRITICAL();
    switch (action)
    {
    case NOTIFY_ACTION_NONE:
        break;

    case NOTIFY_ACTION_INCREMENT:
        tcb->notification_value++;
        break;

    case NOTIFY_ACTION_OVERWRITE:
        tcb->notification_value = value;
        break;

    case NOTIFY_ACTION_SET_BITS:
        tcb->notification_value |= value;
        break;
    }

    if (!tcb->wait_on_notify)
    {
        tcb->is_notified = true;
        tcb->exit_status = STATUS_OK;
        RTOS_EXIT_CRITICAL();
        return;
    }

    tcb->wait_on_notify = false;
    tcb->exit_status = STATUS_OK;
    rtos_task_add_to_ready(&tcb->state_node);
    rtos_task_remove_delay(&tcb->delay_node);
    RTOS_EXIT_CRITICAL();

    rtos_task_yield_if_higher();
}

void rtos_task_notify_from_isr(task_handle_t handle, uint32_t value, notify_action_t action)
{
    rtos_task_notify(handle, value, action);
}

bool rtos_task_notify_clear(task_handle_t handle)
{
    tcb_t *tcb = handle != NULL ? &g_task_table[*handle] : g_current_task;

    RTOS_ENTER_CRITICAL();

    bool was_notified = tcb->is_notified;
    tcb->is_notified = false;

    RTOS_EXIT_CRITICAL();

    return was_notified;
}

bool rtos_task_notify_clear_from_isr(task_handle_t handle)
{
    return rtos_task_notify_clear(handle);
}

void rtos_task_delay(uint32_t ms)
{
    RTOS_ENTER_CRITICAL();
    rtos_task_remove_from_ready(&g_current_task->state_node);
    rtos_task_set_delay(ms);
    RTOS_EXIT_CRITICAL();
    rtos_task_yield();
}

void rtos_scheduler_tick(void *data)
{
    (void)data;
    list_node_t *node = g_task_manager.delay_queue.head;

    RTOS_ENTER_CRITICAL();
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        tcb->delay_ticks--;
        while (tcb->delay_ticks == 0)
        {
            list_pop_front(&g_task_manager.delay_queue, NULL);
            rtos_task_add_to_ready(&tcb->state_node);
            tcb->exit_status = STATUS_TASK_TIMEOUT;

            node = g_task_manager.delay_queue.head;
            if (node == NULL)
            {
                break;
            }
            tcb = node->data;
        }
    }
    RTOS_EXIT_CRITICAL();

    rtos_task_yield();
}

void task_scheduler()
{
    if (g_current_task != NULL && rtos_check_mem_sanity(&g_current_task->stack) == STATUS_STACK_OVR)
    {
        BW_LOG("[ERROR] Stack overflow detected in %s", g_current_task->name);
        bw_error_handler();
    }

    uint8_t highest_prio = __CLZ(__RBIT(g_priority_mask));

    clist_node_t *head;

    RTOS_ENTER_CRITICAL();
    clist_cycle(&g_task_manager.ready_queues[highest_prio], &head);
    RTOS_EXIT_CRITICAL();

    if (head == NULL)
    {
        g_next_task = NULL;
        return;
    }
    g_next_task = (tcb_t *)head->data;
}

void rtos_task_delete(task_handle_t handle)
{
    RTOS_ENTER_CRITICAL();
    tcb_t *tcb = tcb = handle != NULL ? &g_task_table[*handle] : g_current_task;
    if (tcb->p_state_queue != NULL)
    {
        if (tcb->state == TASK_STATE_READY)
        {
            clist_delete_node(tcb->p_state_queue, &tcb->state_node);
            if (g_task_manager.ready_queues[g_current_task->priority].count == 0)
            {
                g_priority_mask &= ~(1 << tcb->priority);
            }
        }
        else
        {
            list_delete_node(tcb->p_state_queue, &tcb->state_node);
        }
    }

    if (tcb->p_state_queue != NULL)
    {
        list_delete_node(tcb->p_delay_queue, &tcb->delay_node);
    }

    list_push_back(&g_task_manager.free_queue, &tcb->state_node);
    tcb->state = TASK_STATE_FREE;
    tcb->p_state_queue = &g_task_manager.free_queue;
    tcb->p_delay_queue = NULL;
    tcb->delay_ticks = 0;
    tcb->event_flags = 0;
    tcb->event_wait_all = false;
    tcb->event_clear_exit = false;
    tcb->events_received = 0;
    tcb->notification_value = 0;
    tcb->is_notified = false;
    tcb->wait_on_notify = false;
    tcb->p_msg_data = NULL;
    tcb->exit_status = STATUS_OK;
    rtos_mem_dealloc(&tcb->stack);
    RTOS_EXIT_CRITICAL();

    if (handle == NULL)
    {
        rtos_task_yield();
    }
}

void task_exit()
{
    BW_LOG("Task: %s has exited", g_current_task->name);
    bw_error_handler();
}

tcb_t *get_task_tcb()
{
    return g_current_task;
}
