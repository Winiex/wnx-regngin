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
#include "winiex_nfa.h"
#include "winiex_dfa.h"
#include "winiex_regex.h"

#define RIGHT_PARAM_COUNT 2

int main(int argc, char* argv[]) {
	char* regex;
	NFA_TP nfa;
	DFA_TP dfa;

	if (argc != RIGHT_PARAM_COUNT) {
		fprintf(stderr, "WRONG PARAMATER");
		return EXIT_FAILURE;
	}

	regex = argv[1];

	if (!regex_validate_regex_passed(regex)) {
		fprintf(stderr, "WRONG REGULAR EXPRESSION");
		return EXIT_FAILURE;
	}

	printf("The regular expression is:\n%s\n\n", regex);

	nfa_regex_to_nfa(regex, &nfa);
	dfa_construct_from_nfa(nfa, &dfa);
	dfa_print_matrix_style(dfa);

	return EXIT_SUCCESS;
}
