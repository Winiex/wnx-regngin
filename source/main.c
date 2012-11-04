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
#include <string.h>
#include "regex.h"


int main(void) {
	char postfix[100];
	char* test = "((a|bcd)*)|b*";


	insert_cat_char(test, postfix);

	puts(postfix);

	return EXIT_SUCCESS;
}
