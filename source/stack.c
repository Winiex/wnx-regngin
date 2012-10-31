/*
 * stack.c
 *
 *  Created on: 2012-10-30
 *      Author: winiex
 */

#include <stdlib.h>
#include "stack.h"

stack_p stack_init() {
	stack_p stack = (stack_p) malloc(sizeof(stack));

	//初始的栈的可用单元大小为20，但是初始的栈大小为0
	stack->head = (stack_elem_p) malloc(20 * sizeof(stack_elem));
	stack->top = stack->head - 1;
	stack->depth = 0;
	stack->size = 20;

	if (stack->head == NULL ) {
		return NULL ;
	}

	return stack;
}

void stack_push(stack_p stack, int to_push) {
	stack_elem_p elem_to_push;

	//如果栈空间已经满，则重新分配20个栈单元空间
	if (stack->depth == stack->size) {
		//TODO realloc 会不会 free 之前的内存呢？没有的话需要我们自己手动 free 一下。
		stack->head = (stack_elem_p) realloc(stack->head, stack->size + 20);
		stack->top = stack->head + stack->size;
		stack->size += 20;
	}

	elem_to_push = stack->top + 1;
	elem_to_push->elem = to_push;
	stack->top = elem_to_push;

	stack->depth += 1;
}

stack_elem_p stack_pop(stack_p stack) {
	stack_elem_p elem_to_pop;

	if (stack->depth == 0) {
		return NULL ;
	}

	elem_to_pop = stack->top;
	stack->top = stack->top - 1;
	stack->depth -= 1;

	return elem_to_pop;
}

int stack_pop_int(stack_p stack) {
	stack_elem_p elem_pop = stack_pop(stack);

	if (elem_pop == NULL ) {
		exit(-1);
	}

	return elem_pop->elem;
}

stack_elem_p stack_top(stack_p stack) {
	return stack->top;
}

int stack_top_int(stack_p stack) {
	stack_elem_p elem_top = stack_top(stack);

	if (elem_top == NULL ) {
		exit(-1);
	}

	return elem_top->elem;
}

int stack_depth(stack_p stack) {
	return stack->depth;
}

int stack_is_empty(stack_p stack) {
	if (stack->depth == 0) {
		return 1;
	} else {
		return 0;
	}
}

void stack_destroy(stack_p stack) {
	free(stack->head);
	free(stack);
}
