/*
 * stack.h
 *
 *  Created on: 2012-10-30
 *      Author: winiex
 */

#ifndef STACK_H_
#define STACK_H_

struct stack_elem {
	int elem;
};

typedef struct stack_elem stack_elem;
typedef struct stack_elem* stack_elem_p;

//我将采用动态划分内存的方式来维护栈元素
struct stack {
	stack_elem_p top;
	stack_elem_p head;
	int depth;
	int size;
};

typedef struct stack stack;
typedef struct stack* stack_p;

stack_p stack_init();

void stack_push(stack_p stack, int to_push);

stack_elem_p stack_pop(stack_p stack);

stack_elem_p stack_top(stack_p stack);

int stack_depth(stack_p stack);

int stack_is_empty(stack_p stack);

void stack_destroy();

#endif /* STACK_H_ */
