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
#include "nfa.h"

void test_consctuct_nfa();

int main(void) {

	char* regex = "(aaaa|aaaa)";
	NFA_TP nfa;
	NFA_STATE_TP state;
	LIST_TP list;
	LIST_TP list_move_to;

	nfa_regex_to_nfa(regex, &nfa);
	state = nfa->nfa_start;

	list = nfa_state_closure(state);
	list_move_to = nfa_state_set_move(list, 'a');

	return EXIT_SUCCESS;
}

void test_consctuct_nfa() {
	char nfa_char_a = 'a', nfa_char_b = 'b';
	NFA_TP char_nfa_a, char_nfa_b, or_nfa, closure_nfa;

	nfa_construct_char_nfa(nfa_char_a, &char_nfa_a);
	nfa_construct_char_nfa(nfa_char_b, &char_nfa_b);

	nfa_construct_metachar_nfa('|', &or_nfa, char_nfa_a, char_nfa_b);

	nfa_construct_metachar_nfa('*', &closure_nfa, or_nfa, NULL);
}
