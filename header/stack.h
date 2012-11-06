/*
 * stack.h
 *
 *  Created on: 2012-10-30
 *      Author: winiex
 */

#ifndef STACK_H_
#define STACK_H_

struct STACK_ELEM {
	int int_elem;
	void* pointer_elem;
};

typedef struct STACK_ELEM STACK_ELEM_T;
typedef struct STACK_ELEM* STACK_ELEM_TP;

//我将采用动态划分内存的方式来维护栈元素
struct STACK {
	STACK_ELEM_TP top;
	STACK_ELEM_TP head;
	int depth;
	int size;
};

typedef struct STACK STACK_T;
typedef struct STACK* STACK_TP;

void stack_init(STACK_TP* stack_p);

void stack_push_int(STACK_TP stack, int to_push);

void stack_push_pointer(STACK_TP stack, void* pointer);

void stack_push_elem(STACK_TP stack, STACK_ELEM_TP elem);

int stack_pop_int(STACK_TP stack);

void* stack_pop_pointer(STACK_TP stack);

STACK_ELEM_TP stack_pop_elem(STACK_TP stack);

int stack_top_int(STACK_TP stack);

void* stack_top_pointer(STACK_TP stack);

STACK_ELEM_TP stack_top_elem(STACK_TP stack);

int stack_depth(STACK_TP stack);

int stack_is_empty(STACK_TP stack);

void stack_destroy();

#endif /* STACK_H_ */
