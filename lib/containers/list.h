#ifndef UTILS_LIST_H
#define UTILS_LIST_H

#include <stdbool.h>
#include <stdint.h>

struct list_node {
	void *data;
	struct list_node *next;
	struct list_node *prev;
};

struct list {
	struct list_node *head;
	struct list_node *tail;
	uint16_t count;
};

void list_init(struct list *list);
void list_push_back(struct list *list, struct list_node *back);
void list_push_front(struct list *list, struct list_node *front);
void list_pop_front(struct list *list, struct list_node **front);
void list_pop_back(struct list *list, struct list_node **back);
void list_insert_after(struct list *list, struct list_node *node, struct list_node *new_node);
void list_insert_before(struct list *list, struct list_node *node, struct list_node *new_node);
void list_delete_node(struct list *list, struct list_node *node);
bool list_is_empty(struct list *list);
void list_clear(struct list *list);

#endif
