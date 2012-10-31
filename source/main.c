/*
 ============================================================================
 Name        : WinRegexEngine.c
 Author      : winiex
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

int main(void) {

	stack_p stack = stack_init();

	stack_push(stack, 1);
	stack_push(stack, 2);
	stack_push(stack, 3);
	stack_push(stack, 4);
	stack_push(stack, 5);
	stack_push(stack, 6);

	while (stack->depth != 0) {
		printf("elem:%d stack size:%d\n", stack_pop_int(stack), stack->depth);
	}

	return EXIT_SUCCESS;
}
