#include "task.h"
#include <stdbool.h>
#include <string.h>
#include "critical.h"
#include "status.h"
#include "systick/systick.h"
#include "task/mem.h"
#include "utils.h"
#include "utils/logger.h"
#include "hal/reg.h"
#include "containers/clist.h"
#include "containers/list.h"

typedef uint16_t prio_msk_t;

static struct
{
    list_node_t task_nodes[MAX_TASKS];
    clist_t ready_list[MAX_TASK_PRIORITY + 1];
    list_t free_list;
    list_t delay_list;
    list_t ewait_list;
} g_task_manager;
static tcb_t g_task_table[MAX_TASKS];
static prio_msk_t g_priority_mask = 0;
tcb_t *g_current_task = NULL;
tcb_t *g_next_task = NULL;

void task_stack_init(uint32_t **sp, uint32_t *pc, uint32_t *user_data);

void rtos_task_init()
{
    for (int i = 0; i <= MAX_TASK_PRIORITY; i++)
    {
        clist_init(&g_task_manager.ready_list[i]);
    }
    list_init(&g_task_manager.free_list);
    list_init(&g_task_manager.delay_list);
    list_init(&g_task_manager.ewait_list);

    for (int i = 0; i < MAX_TASKS; i++)
    {
        g_task_manager.task_nodes[i].data = &g_task_table[i];
        list_push_back(&g_task_manager.free_list, &g_task_manager.task_nodes[i]);
    }
}

void rtos_task_create(task_func_t task_ptr, const char *name, uint8_t priority, uint32_t stack_depth,
                      void *user_data, task_handle_t *handle)
{
    BW_ASSERT(priority < 16, "Invalid priority : %d, (Expected range (0-15))");

    list_node_t *node;
    list_pop_front(&g_task_manager.free_list, &node);
    if (node == NULL)
    {
        BW_LOG("Exceeded the maximum task count : %d", MAX_TASKS);
        bw_error_handler();
    }
    clist_push_back(&g_task_manager.ready_list[priority], node);
    g_priority_mask |= 1 << priority;

    tcb_t *tcb = node->data;
    strncpy(tcb->name, name, 16);
    tcb->state = TASK_STATE_READY;
    tcb->handle = node;
    tcb->priority = priority;
    tcb->user_data = user_data;
    tcb->stack.size = stack_depth;
    bw_status_t status = rtos_mem_alloc(&tcb->stack);
    if (status != STATUS_OK)
    {
        BW_LOG("Ran out of memory blocks");
        bw_error_handler();
    }

    task_stack_init(&tcb->stack.stack_ptr, (uint32_t *)task_ptr, (uint32_t *)user_data);
    if (handle)
    {
        *handle = tcb->handle;
    }
}

void rtos_task_delay(uint32_t ms)
{
    RTOS_ENTER_CRITICAL();

    tcb_t *tcb;
    list_node_t *node = g_task_manager.delay_list.head;
    while (node != NULL)
    {
        tcb = node->data;
        if (tcb->delay_ticks > ms)
        {
            break;
        }
        ms -= tcb->delay_ticks;
        node = node->next;
    }

    g_current_task->delay_ticks = ms;
    clist_delete(&g_task_manager.ready_list[g_current_task->priority], g_current_task->handle);
    if (g_task_manager.ready_list[g_current_task->priority].count == 0)
    {
        g_priority_mask &= ~(1 << g_current_task->priority);
    }

    if (node == NULL)
    {
        list_push_back(&g_task_manager.delay_list, g_current_task->handle);
    }
    else
    {
        list_insert_before(&g_task_manager.delay_list, node, g_current_task->handle);
    }

    RTOS_EXIT_CRITICAL();

    reg_set_mask(&SCB->ICSR, SCB_ICSR_PENDSVSET_Msk);
}

void rtos_scheduler_tick(void *data)
{
    (void)data;
    RTOS_ENTER_CRITICAL();

    list_node_t *node = g_task_manager.delay_list.head;
    if (node != NULL)
    {
        tcb_t *tcb = node->data;
        tcb->delay_ticks--;
        while (tcb->delay_ticks == 0)
        {
            list_pop_front(&g_task_manager.delay_list, NULL);
            clist_push_back(&g_task_manager.ready_list[tcb->priority], node);
            tcb->state = TASK_STATE_READY;
            g_priority_mask |= 1 << tcb->priority;

            node = g_task_manager.delay_list.head;
            if (node == NULL)
            {
                break;
            }
            tcb = node->data;
        }
    }
    
    RTOS_EXIT_CRITICAL();

    reg_set_mask(&SCB->ICSR, SCB_ICSR_PENDSVSET_Msk);
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
    clist_cycle(&g_task_manager.ready_list[highest_prio], &head);
    if (head == NULL)
    {
        g_next_task = NULL;
        return;
    }
    g_next_task = (tcb_t *)head->data;
}

void task_exit()
{
    BW_LOG("Task: %s has exited", g_current_task->name);
    bw_error_handler();
}
