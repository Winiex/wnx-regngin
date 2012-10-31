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

void regex_infix_to_postfix(char* regex_infix, char* regex_postfix);

#endif /* REGEX_H_ */
