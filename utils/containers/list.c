#include "list.h"
#include "utils.h"

void list_init(list_t *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

void list_push_back(list_t *list, list_node_t *back)
{
    back->next = NULL;
    back->prev = list->tail;

    if (list->tail != NULL)
    {
        list->tail->next = back;
    }
    else
    {
        list->head = back;
    }

    list->tail = back;
    list->count++;
}

void list_push_front(list_t *list, list_node_t *front)
{
    front->next = list->head;
    front->prev = NULL;

    if (list->head != NULL)
    {
        list->head->prev = front;
    }
    else
    {
        list->tail = front;
    }

    list->head = front;
    list->count++;
}

void list_pop_front(list_t *list, list_node_t **front)
{
    if (front != NULL)
    {
        *front = list->head;
    }

    if (list->head == NULL)
    {
        return;
    }

    list_delete(list, list->head);
}

void list_pop_back(list_t *list, list_node_t **back)
{
    if (back != NULL)
    {
        *back = list->tail;
    }
    if (list->tail == NULL)
    {
        return;
    }

    list->tail = list->tail->prev;
    if (list->tail == NULL)
    {
        list->head = NULL;
    }
    else
    {
        list->tail->next = NULL;
    }
    list->count--;
}

void list_insert_after(list_t *list, list_node_t *node, list_node_t *new_node)
{
#ifdef DEBUG
    list_node_t *curr = list->head;
    for (int i = 0; i < list->count; i++)
    {
        if (new_node == curr)
        {
            BW_LOG("New node present within the list could cause corruption");
            bw_error_handler();
        }
        curr = curr->next;
    }
#endif

    new_node->next = node->next;
    new_node->prev = node;

    if (node->next != NULL)
    {
        node->next->prev = new_node;
    }
    node->next = new_node;

    if (list->tail == node)
    {
        list->tail = new_node;
    }
    list->count++;
}

void list_insert_before(list_t *list, list_node_t *node, list_node_t *new_node)
{
#ifdef DEBUG
    list_node_t *curr = list->head;
    for (int i = 0; i < list->count; i++)
    {
        if (new_node == curr)
        {
            BW_LOG("New node present within the list could cause corruption");
            bw_error_handler();
        }
        curr = curr->next;
    }
#endif

    new_node->next = node;
    new_node->prev = node->prev;

    if (node->prev != NULL)
    {
        node->prev->next = new_node;
    }
    node->prev = new_node;

    if (list->head == node)
    {
        list->head = new_node;
    }
    list->count++;
}

void list_delete(list_t *list, list_node_t *node)
{
#ifdef DEBUG
    bool found = false;
    list_node_t *curr = list->head;
    for (int i = 0; i < list->count; i++)
    {
        if (node == curr)
        {
            found = true;
        }
        curr = curr->next;
    }
    if (!found)
    {
        BW_LOG("Delete node not present within the list could cause corruption");
        bw_error_handler();
    }
#endif

    // Unlink for circular list 
    list->head->prev = NULL;
    list->tail->next = NULL;

    if (node->next != NULL)
    {
        node->next->prev = node->prev;
    }
    else 
    {
        list->tail = node->prev;
    }

    if (node->prev != NULL)
    {
        node->prev->next = node->next;
    }
    else 
    {
        list->head = node->next;
    }

    list->count--;
}

void list_clear(list_t *list)
{
    list_init(list);
}
