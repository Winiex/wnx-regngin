/*
 * regex.h
 *
 *  Created on: 2012-10-30
 *      Author: winiex
 */

#ifndef WINIEX_REGEX_H_
#define WINIEX_REGEX_H_

#include <string.h>
#include "winiex_stack.h"
#include "winiex_list.h"

typedef enum {
	ATERISK, BAR, QUESTION, PARENLEFT, PARENRIGHT, CONCAT
} REGEX_OPER_T;

LIST_TP regex_get_char_set(char *regex);

void regex_insert_cat_char(char* regex_infix, char* regex_infix_with_cat_char);

void regex_infix_to_postfix(char* regex_infix, char* regex_postfix);

int regex_measure_regex_memory_size(char* regex);

int regex_is_char_or_number(char a_char);

static int regex_get_operator_precedence(char oper);

#endif /* REGEX_H_ */
