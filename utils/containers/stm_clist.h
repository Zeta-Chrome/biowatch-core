#ifndef STM_CLIST_H
#define STM_CLIST_H

#include "utils/utils.h"
#include <stdbool.h>

typedef PACKED_STRUCT stm_list_node
{
    struct stm_list_node *next;
    struct stm_list_node *prev;
}
stm_list_node_t;

void stm_list_init(stm_list_node_t *head);
bool stm_list_is_empty(stm_list_node_t *head);
void stm_list_insert_head(stm_list_node_t *head, stm_list_node_t *node);
void stm_list_insert_tail(stm_list_node_t *head, stm_list_node_t *node);
void stm_list_remove_node(stm_list_node_t *node);
void stm_list_remove_head(stm_list_node_t *head, stm_list_node_t **node);
void stm_list_remove_tail(stm_list_node_t *head, stm_list_node_t **node);
void stm_list_insert_after(stm_list_node_t *node, stm_list_node_t *ref);
void stm_list_insert_before(stm_list_node_t *node, stm_list_node_t *ref);
void stm_list_get_next(stm_list_node_t *ref, stm_list_node_t **node);
void stm_list_get_prev(stm_list_node_t *ref, stm_list_node_t **node);
int stm_list_get_size(stm_list_node_t *head);

#endif
