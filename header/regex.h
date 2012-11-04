/*
 * regex.h
 *
 *  Created on: 2012-10-30
 *      Author: winiex
 */

#ifndef REGEX_H_
#define REGEX_H_

typedef enum {
	ATERISK, BAR, QUESTION, PARENLEFT, PARENRIGHT, CONCAT
} REGEX_OPER_T;

void insert_cat_char(char* regex_infix, char* regex_infix_with_cat_char);

void regex_infix_to_postfix(char* regex_infix, char* regex_postfix);

static int is_char_or_number(char a_char);

static int get_operator_precedence(char oper);

#endif /* REGEX_H_ */
