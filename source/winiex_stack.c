/*
 * stack.c
 *
 *  Created on: 2012-10-30
 *      Author: winiex
 */

#include "winiex_stack.h"

void stack_init(STACK_TP* stack_p) {
	STACK_TP stack = (STACK_TP) malloc(sizeof(STACK_T));

	//初始的栈的可用单元大小为20，但是初始的栈大小为0
	stack->head = (STACK_ELEM_TP) malloc(20 * sizeof(STACK_ELEM_T));
	stack->top = stack->head - 1;
	stack->depth = 0;
	stack->size = 20;

	if (stack == NULL || stack->head == NULL ) {
		fprintf(stderr, "Init Stack Error!");
		return;
	}

	(*stack_p) = stack;
}

void stack_push_int(STACK_TP stack, int to_push) {
	STACK_ELEM_TP elem_to_push;

	//如果栈空间已经满，则重新分配20个栈单元空间
	if (stack->depth == stack->size) {
		//TODO realloc 会不会 free 之前的内存呢？没有的话需要我们自己手动 free 一下。
		stack->head = (STACK_ELEM_TP) realloc(stack->head, stack->size + 20);
		stack->top = stack->head + stack->size;
		stack->size += 20;
	}

	elem_to_push = stack->top + 1;
	elem_to_push->int_elem = to_push;
	elem_to_push->pointer_elem = NULL;
	stack->top = elem_to_push;

	stack->depth += 1;
}

void stack_push_pointer(STACK_TP stack, void* pointer) {
	STACK_ELEM_TP elem_to_push;

	//如果栈空间已经满，则重新分配20个栈单元空间
	if (stack->depth == stack->size) {
		//TODO realloc 会不会 free 之前的内存呢？没有的话需要我们自己手动 free 一下。
		stack->head = (STACK_ELEM_TP) realloc(stack->head, stack->size + 20);
		stack->top = stack->head + stack->size;
		stack->size += 20;
	}

	elem_to_push = stack->top + 1;
	elem_to_push->int_elem = 0;
	elem_to_push->pointer_elem = pointer;
	stack->top = elem_to_push;

	stack->depth += 1;
}

void stack_push_elem(STACK_TP stack, STACK_ELEM_TP elem) {
	STACK_ELEM_TP elem_to_push;

	//如果栈空间已经满，则重新分配20个栈单元空间
	if (stack->depth == stack->size) {
		//TODO realloc 会不会 free 之前的内存呢？没有的话需要我们自己手动 free 一下。
		stack->head = (STACK_ELEM_TP) realloc(stack->head, stack->size + 20);
		stack->top = stack->head + stack->size;
		stack->size += 20;
	}

	elem_to_push = stack->top + 1;
	elem_to_push->int_elem = elem->int_elem;
	elem_to_push->pointer_elem = elem->pointer_elem;
	stack->top = elem_to_push;

	stack->depth += 1;
}

int stack_pop_int(STACK_TP stack) {
	STACK_ELEM_TP elem_pop = stack_pop_elem(stack);

	if (elem_pop == NULL ) {
		fprintf(stderr, "Stack Pop Int Error!");
		return -1;
	}

	return elem_pop->int_elem;
}

void* stack_pop_pointer(STACK_TP stack) {
	STACK_ELEM_TP elem_pop = stack_pop_elem(stack);

	if (elem_pop == NULL ) {
		fprintf(stderr, "Stack Pop Pointer Error!");
		return NULL ;
	}

	return elem_pop->pointer_elem;
}

STACK_ELEM_TP stack_pop_elem(STACK_TP stack) {
	STACK_ELEM_TP elem_to_pop;

	if (stack->depth == 0) {
		return NULL ;
	}

	elem_to_pop = stack->top;
	stack->top = stack->top - 1;
	stack->depth -= 1;

	return elem_to_pop;
}

int stack_top_int(STACK_TP stack) {
	STACK_ELEM_TP elem_top = stack_top_elem(stack);

	if (elem_top == NULL ) {
		fprintf(stderr, "Stack Top Int Error!");
		return -1;
	}

	return elem_top->int_elem;
}

void* stack_top_pointer(STACK_TP stack) {
	STACK_ELEM_TP elem_top = stack_top_elem(stack);

	if (elem_top == NULL ) {
		fprintf(stderr, "Stack Pop Pointer Error!");
		return NULL;
	}

	return elem_top->pointer_elem;
}

STACK_ELEM_TP stack_top_elem(STACK_TP stack) {
	if (stack->depth == 0) {
		return NULL ;
	}
	return stack->top;
}

int stack_depth(STACK_TP stack) {
	return stack->depth;
}

int stack_is_empty(STACK_TP stack) {
	if (stack->depth == 0) {
		return 1;
	} else {
		return 0;
	}
}

void stack_destroy(STACK_TP* stack) {
	free((*stack)->head);
	free(*stack);
	(*stack) = NULL;
}
