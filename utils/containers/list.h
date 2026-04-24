#ifndef UTILS_LIST_H
#define UTILS_LIST_H

#include <stdbool.h>
#include <stdint.h>

typedef struct list_node
{
    void *data;
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

typedef struct 
{
    list_node_t *head;
    list_node_t *tail;
    uint16_t count;
} list_t;

void list_init(list_t *list);
void list_push_back(list_t *list, list_node_t *back);
void list_push_front(list_t *list, list_node_t *front);
void list_pop_front(list_t *list, list_node_t **front);
void list_pop_back(list_t *list, list_node_t **back);
void list_insert_after(list_t *list, list_node_t *node, list_node_t *new_node);
void list_insert_before(list_t *list, list_node_t *node, list_node_t *new_node);
void list_delete(list_t *list, list_node_t *node);
void list_clear(list_t *list);

#endif
