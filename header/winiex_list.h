/*
 * list.h
 *
 *  Created on: 2012-11-7
 *      Author: winiex
 */

#ifndef WINIEX_LIST_H_
#define WINIEX_LIST_H_

#include <stdlib.h>

struct list_elem {
	int int_elem;
	void* pointer_elem;
	struct list_elem* prior;
	struct list_elem* next;
};

typedef struct list_elem LIST_ELEM_T;
typedef struct list_elem* LIST_ELEM_TP;

struct list {
	LIST_ELEM_TP list_head;
	LIST_ELEM_TP list_tail;
	int list_size;
};

typedef struct list LIST_T;
typedef struct list* LIST_TP;

void list_init(LIST_TP* list);

void list_add_int(LIST_TP list, int int_add);

void list_add_pointer(LIST_TP list, void* pointer_add);

void list_add_elem(LIST_TP list, LIST_ELEM_TP elem_add);

void list_delete_elem(LIST_TP list, LIST_ELEM_TP elem_delete);

LIST_TP list_cat(LIST_TP list1, LIST_TP list2);

void list_destroy(LIST_TP* list);

#endif /* LIST_H_ */
