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

int main(void) {

	char* regex = "baa|a";
	NFA_TP nfa;
	DFA_TP dfa;

	nfa_regex_to_nfa(regex, &nfa);
	dfa_construct_from_nfa(nfa, &dfa);

	return EXIT_SUCCESS;
}
