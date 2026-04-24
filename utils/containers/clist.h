#ifndef UTILS_CLIST_H
#define UTILS_CLIST_H

#include "list.h"

typedef list_node_t clist_node_t;
typedef list_t clist_t;

void clist_init(clist_t *clist);
void clist_cycle(clist_t *clist, clist_node_t **head);
void clist_push_back(clist_t *clist, clist_node_t *back);
void clist_push_front(clist_t *clist, clist_node_t *front);
void clist_pop_front(clist_t *clist, clist_node_t **front);
void clist_pop_back(clist_t *clist, clist_node_t **back);
void clist_insert_after(clist_t *clist, clist_node_t *node, clist_node_t *new_node);
void clist_insert_before(clist_t *clist, clist_node_t *node, clist_node_t *new_node);
void clist_delete(clist_t *clist, clist_node_t *node);
void clist_clear(clist_t *clist);

#endif
