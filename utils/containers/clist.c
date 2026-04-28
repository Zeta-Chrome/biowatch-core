#include "clist.h"

void clist_init(clist_t *clist)
{
    list_init(clist);
}

void clist_cycle(clist_t *clist, clist_node_t **head)
{
    *head = clist->head;
    clist->head = clist->head->next;
    clist->tail = clist->tail->next;
}

void clist_push_back(clist_t *clist, clist_node_t *back)
{
    list_push_back(clist, back);
    clist->tail->next = clist->head;
    clist->head->prev = clist->tail;
}

void clist_push_front(clist_t *clist, clist_node_t *front)
{
    list_push_front(clist, front);
    clist->tail->next = clist->head;
    clist->head->prev = clist->tail;
}

void clist_pop_front(clist_t *clist, clist_node_t **front)
{
    list_pop_front(clist, front);
    if (clist->count == 0)
    {
        return;
    }
    clist->tail->next = clist->head;
    clist->head->prev = clist->tail;
}

void clist_pop_back(clist_t *clist, clist_node_t **back)
{
    list_pop_back(clist, back);
    if (clist->count == 0)
    {
        return;
    }
    clist->tail->next = clist->head;
    clist->head->prev = clist->tail;
}

void clist_insert_after(clist_t *clist, clist_node_t *node, clist_node_t *new_node)
{
    list_insert_after(clist, node, new_node);
}

void clist_insert_before(clist_t *clist, clist_node_t *node, clist_node_t *new_node)
{
    list_insert_before(clist, node, new_node);
}

void clist_delete_node(clist_t *clist, clist_node_t *node)
{
    list_delete_node(clist, node);
    if (clist->count == 0)
    {
        return;
    }
    clist->tail->next = clist->head;
    clist->head->prev = clist->tail;
}

void clist_clear(clist_t *clist)
{
    list_clear(clist);
}
