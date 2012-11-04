/*
 * regex.c
 *
 *  Created on: 2012-10-30
 *      Author: winiex
 */

#include <string.h>
#include "regex.h"
#include "stack.h"

void regex_infix_to_postfix(char* regex_infix, char* regex_postfix) {
	int counter, postfix_fill_in_index = 0;
	int top_char_precedence, current_char_precedence;
	int str_length;
	char cursor_char;
	STACK_TP buffer_stack;
	STACK_ELEM_TP buffer_elem;

	str_length = strlen(regex_infix);
	stack_init(&buffer_stack);

	for (counter = 0; counter < str_length; counter++) {
		cursor_char = regex_infix[counter];

		switch (cursor_char) {

		case '(':
			stack_push(buffer_stack, cursor_char);
			break;

		case ')':
			while ((buffer_elem = stack_pop(buffer_stack)) != NULL ) {
				if (buffer_elem->elem != '(') {
					regex_postfix[postfix_fill_in_index] =
							(char) buffer_elem->elem;
					postfix_fill_in_index++;
				}
			}
			break;

		default:
			if (cursor_char == '|' || cursor_char == '&' || cursor_char == '?'
					|| cursor_char == '*' || cursor_char == '+') {

				current_char_precedence = get_operator_precedence(cursor_char);

				while ((buffer_elem = stack_top(buffer_stack)) != NULL ) {
					top_char_precedence = get_operator_precedence(
							(char) buffer_elem->elem);
					if (top_char_precedence >= current_char_precedence) {
						buffer_elem = stack_pop(buffer_stack);
						regex_postfix[postfix_fill_in_index] =
								(char) buffer_elem->elem;
						postfix_fill_in_index++;
					} else {
						break;
					}
				}
				stack_push(buffer_stack, cursor_char);

			} else {
				//当不是以上的符号的时候，就是普通的字符，直接输出
				regex_postfix[postfix_fill_in_index] = cursor_char;
				postfix_fill_in_index++;
			}
			break;
		}
	}

	while ((buffer_elem = stack_pop(buffer_stack)) != NULL ) {
		regex_postfix[postfix_fill_in_index] = (char) buffer_elem->elem;
		postfix_fill_in_index++;
	}

	regex_postfix[postfix_fill_in_index] = '\0';
}

void insert_cat_char(char* regex_infix, char* regex_infix_with_cat_char) {
	char current_char, former_char;
	int str_length, counter, cat_char_index = 0;

	str_length = strlen(regex_infix);

	for (counter = 0; counter < str_length; counter++) {
		if (counter == 0) {
			regex_infix_with_cat_char[cat_char_index] = regex_infix[counter];
			cat_char_index++;
			continue;
		}

		former_char = regex_infix[counter - 1];
		current_char = regex_infix[counter];

		if (is_char_or_number(former_char) && is_char_or_number(current_char)) {
			regex_infix_with_cat_char[cat_char_index] = '&';
			cat_char_index++;
			regex_infix_with_cat_char[cat_char_index] = current_char;
			cat_char_index++;
		} else {
			regex_infix_with_cat_char[cat_char_index] = current_char;
			cat_char_index++;
		}
	}

	regex_infix_with_cat_char[cat_char_index] = '\0';
}

static int is_char_or_number(char a_char) {
	if ((a_char >= 'a' && a_char <= 'z') || (a_char >= 'A' && a_char <= 'Z')
			|| (a_char >= '0' && a_char <= '9')) {
		return 1;
	}
	return 0;
}

static int get_operator_precedence(char oper) {
	int precedence = -1;

	switch (oper) {
	case '(':
		precedence = 1;
		break;
	case '|':
		precedence = 2;
		break;
	case '&':
		precedence = 3;
		break;
	case '?':
		precedence = 4;
		break;
	case '*':
		precedence = 4;
		break;
	case '+':
		precedence = 4;
		break;
	}

	return precedence;
}
