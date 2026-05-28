#include "stm_clist.h"
#include "rtos/critical.h"

void stm_list_init(stm_list_node_t *head)
{
    head->next = head;
    head->prev = head;
}

bool stm_list_is_empty(stm_list_node_t *head)
{
    bool empty;
    RTOS_ENTER_CRITICAL();
    empty = (head->next == head);
    RTOS_EXIT_CRITICAL();
    return empty;
}

void stm_list_insert_head(stm_list_node_t *head, stm_list_node_t *node)
{
    RTOS_ENTER_CRITICAL();
    node->next       = head->next;
    node->prev       = head;
    head->next       = node;
    node->next->prev = node;
    RTOS_EXIT_CRITICAL();
}

void stm_list_insert_tail(stm_list_node_t *head, stm_list_node_t *node)
{
    RTOS_ENTER_CRITICAL();
    node->next       = head;
    node->prev       = head->prev;
    head->prev       = node;
    node->prev->next = node;
    RTOS_EXIT_CRITICAL();
}

void stm_list_remove_node(stm_list_node_t *node)
{
    RTOS_ENTER_CRITICAL();
    node->prev->next = node->next;
    node->next->prev = node->prev;
    RTOS_EXIT_CRITICAL();
}

void stm_list_remove_head(stm_list_node_t *head, stm_list_node_t **node)
{
    RTOS_ENTER_CRITICAL();
    *node = head->next;
    stm_list_remove_node(head->next);
    RTOS_EXIT_CRITICAL();
}

void stm_list_remove_tail(stm_list_node_t *head, stm_list_node_t **node)
{
    RTOS_ENTER_CRITICAL();
    *node = head->prev;
    stm_list_remove_node(head->prev);
    RTOS_EXIT_CRITICAL();
}

void stm_list_insert_after(stm_list_node_t *node, stm_list_node_t *ref)
{
    RTOS_ENTER_CRITICAL();
    node->next       = ref->next;
    node->prev       = ref;
    ref->next        = node;
    node->next->prev = node;
    RTOS_EXIT_CRITICAL();
}

void stm_list_insert_before(stm_list_node_t *node, stm_list_node_t *ref)
{
    RTOS_ENTER_CRITICAL();
    node->next       = ref;
    node->prev       = ref->prev;
    ref->prev        = node;
    node->prev->next = node;
    RTOS_EXIT_CRITICAL();
}

void stm_list_get_next(stm_list_node_t *ref, stm_list_node_t **node)
{
    RTOS_ENTER_CRITICAL();
    *node = ref->next;
    RTOS_EXIT_CRITICAL();
}

void stm_list_get_prev(stm_list_node_t *ref, stm_list_node_t **node)
{
    RTOS_ENTER_CRITICAL();
    *node = ref->prev;
    RTOS_EXIT_CRITICAL();
}

int stm_list_get_size(stm_list_node_t *head)
{
    int size = 0;
    stm_list_node_t *temp;
    RTOS_ENTER_CRITICAL();
    temp = head->next;
    while (temp != head)
    {
        size++;
        temp = temp->next;
    }
    RTOS_EXIT_CRITICAL();
    return size;
}
