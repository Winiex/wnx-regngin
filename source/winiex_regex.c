/*
 * regex.c
 *
 *  Created on: 2012-10-30
 *      Author: winiex
 */

#include "winiex_regex.h"

LIST_TP regex_get_char_set(char *regex) {
	LIST_TP char_set;
	LIST_ELEM_TP list_cursor, inner_cursor;
	int regex_length, counter, current_char;

	regex_length = strlen(regex);
	list_init(&char_set);

	for (counter = 0; counter < regex_length; counter++) {
		if (regex_is_char_or_number(regex[counter])) {
			list_add_int(char_set, regex[counter]);
		}
	}

	list_cursor = char_set->list_head;

	for (; list_cursor != NULL ; list_cursor = list_cursor->next) {
		current_char = list_cursor->int_elem;
		for (inner_cursor = list_cursor->next; inner_cursor != NULL ;
				inner_cursor = inner_cursor->next) {
			if (inner_cursor->int_elem == current_char) {
				list_delete_elem(char_set, inner_cursor);
			}
		}
	}

	return char_set;
}

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
			stack_push_int(buffer_stack, cursor_char);
			break;

		case ')':
			while ((buffer_elem = stack_pop_elem(buffer_stack)) != NULL ) {
				if (buffer_elem->int_elem != '(') {
					regex_postfix[postfix_fill_in_index] =
							(char) buffer_elem->int_elem;
					postfix_fill_in_index++;
				}
			}
			break;

		default:
			if (cursor_char == '|' || cursor_char == '&' || cursor_char == '?'
					|| cursor_char == '*' || cursor_char == '+') {

				current_char_precedence = regex_get_operator_precedence(
						cursor_char);

				while ((buffer_elem = stack_top_elem(buffer_stack)) != NULL ) {
					top_char_precedence = regex_get_operator_precedence(
							(char) buffer_elem->int_elem);
					if (top_char_precedence >= current_char_precedence) {
						buffer_elem = stack_pop_elem(buffer_stack);
						regex_postfix[postfix_fill_in_index] =
								(char) buffer_elem->int_elem;
						postfix_fill_in_index++;
					} else {
						break;
					}
				}
				stack_push_int(buffer_stack, cursor_char);

			} else {
				//当不是以上的符号的时候，就是普通的字符，直接输出
				regex_postfix[postfix_fill_in_index] = cursor_char;
				postfix_fill_in_index++;
			}
			break;
		}
	}

	while ((buffer_elem = stack_pop_elem(buffer_stack)) != NULL ) {
		regex_postfix[postfix_fill_in_index] = (char) buffer_elem->int_elem;
		postfix_fill_in_index++;
	}

	regex_postfix[postfix_fill_in_index] = '\0';

	stack_destroy(&buffer_stack);
}

void regex_insert_cat_char(char* regex_infix, char* regex_infix_with_cat_char) {
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

		if ((regex_is_char_or_number(former_char)
				&& regex_is_char_or_number(current_char))
				|| (former_char == ')' && regex_is_char_or_number(current_char))
				|| (regex_is_char_or_number(former_char) && current_char == '(')
				|| (former_char == ')' && current_char == '(')) {
			regex_infix_with_cat_char[cat_char_index] = '&';
			cat_char_index++;
			regex_infix_with_cat_char[cat_char_index] = current_char;
			cat_char_index++;
		} else if ((former_char == '*' && regex_is_char_or_number(current_char))
				|| (former_char == '+' && regex_is_char_or_number(current_char))
				|| (former_char == '?' && regex_is_char_or_number(current_char))) {
			regex_infix_with_cat_char[cat_char_index] = '&';
			cat_char_index++;
			regex_infix_with_cat_char[cat_char_index] = current_char;
			cat_char_index++;
		} else if ((former_char == '*' && current_char == '(')
				|| (former_char == '+' && current_char == '(')
				|| (former_char == '?' && current_char == '(')) {
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

int regex_measure_regex_memory_size(char* regex) {
	int result = -1, counter = 0, str_length = -1;
	char former_char, current_char;

	result = str_length = strlen(regex);

	for (; counter < str_length; counter++) {
		if (counter == 0) {
			continue;
		}
		current_char = regex[counter];
		former_char = regex[counter - 1];

		if (regex_is_char_or_number(current_char)
				&& regex_is_char_or_number(former_char)) {
			result++;
		}
	}

	//别忘了 '\0' 字符
	return result + 1;
}

int regex_is_char_or_number(char a_char) {
	if ((a_char >= 'a' && a_char <= 'z') || (a_char >= 'A' && a_char <= 'Z')
			|| (a_char >= '0' && a_char <= '9')) {
		return 1;
	}
	return 0;
}

static int regex_get_operator_precedence(char oper) {
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
