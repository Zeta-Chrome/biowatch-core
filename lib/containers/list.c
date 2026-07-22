#include "lib/utils.h"
#include "list.h"
#include <stddef.h>

void list_init(struct list *list)
{
	list->head = NULL;
	list->tail = NULL;
	list->count = 0;
}

void list_push_back(struct list *list, struct list_node *back)
{
	back->next = NULL;
	back->prev = list->tail;

	if (list->tail != NULL)
		list->tail->next = back;
	else
		list->head = back;

	list->tail = back;
	list->count++;
}

void list_push_front(struct list *list, struct list_node *front)
{
	front->next = list->head;
	front->prev = NULL;

	if (list->head != NULL)
		list->head->prev = front;
	else
		list->tail = front;

	list->head = front;
	list->count++;
}

void list_pop_front(struct list *list, struct list_node **front)
{
	if (front != NULL)
		*front = list->head;

	if (list->head == NULL)
		return;

	list_delete_node(list, list->head);
}

void list_pop_back(struct list *list, struct list_node **back)
{
	if (back != NULL)
		*back = list->tail;

	if (list->tail == NULL)
		return;

	list_delete_node(list, list->tail);
}

void list_insert_after(struct list *list, struct list_node *node, struct list_node *new_node)
{
#ifdef DEBUG
	struct list_node *curr = list->head;
	for (int i = 0; i < list->count; i++) {
		if (new_node == curr) {
			BW_LOG("New node present within the list could cause corruption");
			bw_error_handler();
		}
		curr = curr->next;
	}
#endif

	new_node->next = node->next;
	new_node->prev = node;

	if (node->next != NULL)
		node->next->prev = new_node;

	node->next = new_node;

	if (list->tail == node)
		list->tail = new_node;

	list->count++;
}

void list_insert_before(struct list *list, struct list_node *node, struct list_node *new_node)
{
#ifdef DEBUG
	struct list_node *curr = list->head;
	for (int i = 0; i < list->count; i++) {
		if (new_node == curr) {
			BW_LOG("New node present within the list could cause corruption");
			bw_error_handler();
		}
		curr = curr->next;
	}
#endif

	new_node->next = node;
	new_node->prev = node->prev;

	if (node->prev != NULL)
		node->prev->next = new_node;

	node->prev = new_node;

	if (list->head == node)
		list->head = new_node;

	list->count++;
}

void list_delete_node(struct list *list, struct list_node *node)
{
#ifdef DEBUG
	bool found = false;
	struct list_node *curr = list->head;
	for (int i = 0; i < list->count; i++) {
		if (node == curr)
			found = true;

		curr = curr->next;
	}
	if (!found) {
		BW_LOG("Delete node not present within the list could cause corruption");
		bw_error_handler();
	}
#endif

	if (node->next != NULL)
		node->next->prev = node->prev;
	else
		list->tail = node->prev;

	if (node->prev != NULL)
		node->prev->next = node->next;
	else
		list->head = node->next;

	list->count--;
}

bool list_is_empty(struct list *list)
{
	return (list->head == NULL) && (list->tail == NULL);
}

void list_clear(struct list *list)
{
	list_init(list);
}
