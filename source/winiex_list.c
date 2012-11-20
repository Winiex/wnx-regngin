/*
 * list.c
 *
 *  Created on: 2012-11-7
 *      Author: winiex
 */

#include "winiex_list.h"

void list_init(LIST_TP* list) {
	(*list) = (LIST_TP) malloc(sizeof(LIST_T));
	(*list)->list_head = NULL;
	(*list)->list_tail = NULL;
	(*list)->list_size = 0;
}

void list_add_int(LIST_TP list, int int_add) {
	LIST_ELEM_TP elem_add = (LIST_ELEM_TP) malloc(sizeof(LIST_ELEM_T));
	elem_add->int_elem = int_add;
	elem_add->pointer_elem = NULL;
	list_add_elem(list, elem_add);
}

void list_add_pointer(LIST_TP list, void* pointer_add) {
	LIST_ELEM_TP elem_add = (LIST_ELEM_TP) malloc(sizeof(LIST_ELEM_T));
	elem_add->int_elem = -1;
	elem_add->pointer_elem = pointer_add;
	list_add_elem(list, elem_add);
}

void list_add_elem(LIST_TP list, LIST_ELEM_TP elem_add) {
	LIST_ELEM_TP elem_cursor;

	if (list->list_head == NULL ) {
		list->list_head = elem_add;
		list->list_tail = elem_add;
		list->list_head->prior = NULL;
		list->list_tail->next = NULL;
		list->list_size = 1;
		return;
	}

	elem_cursor = list->list_tail;

	elem_cursor->next = elem_add;
	elem_add->prior = elem_cursor;
	elem_add->next = NULL;

	list->list_tail = elem_add;

	list->list_size += 1;

	list->list_head->prior = NULL;
	list->list_tail->next = NULL;
}

void list_delete_elem(LIST_TP list, LIST_ELEM_TP elem_delete) {
	if ((list->list_head == elem_delete) && (list->list_tail == elem_delete)) {
		list->list_head = list->list_tail = NULL;
	}

	if (elem_delete == list->list_head) {
		list->list_head = list->list_head->next;
		elem_delete->next = NULL;
		list->list_head->prior = NULL;
	} else if (elem_delete == list->list_tail) {
		list->list_tail = list->list_tail->prior;
		elem_delete->prior = NULL;
		list->list_tail->next = NULL;
	} else {
		elem_delete->prior->next = elem_delete->next;
		elem_delete->next->prior = elem_delete->prior;
	}

	list->list_size -= 1;
	free(elem_delete);
}

LIST_TP list_cat(LIST_TP list1, LIST_TP list2) {
	if (list1->list_size == 0 || list1 == NULL ) {
		return list2;
	}

	if (list2->list_size == 0 || list2 == NULL ) {
		return list1;
	}

	list1->list_head->prior = NULL;
	list1->list_tail->next = list2->list_head;
	list2->list_head->prior = list1->list_tail;
	list2->list_tail->next = NULL;

	list1->list_tail = list2->list_tail;
	list1->list_size += list2->list_size;

	return list1;
}

void list_destroy(LIST_TP* list) {
	LIST_ELEM_TP elem_cursor = (*list)->list_head;
	LIST_ELEM_TP to_free;

	while (elem_cursor != NULL ) {
		to_free = elem_cursor;
		elem_cursor = elem_cursor->next;
		free(elem_cursor);
	}

	free(*list);

	(*list) = NULL;
}
