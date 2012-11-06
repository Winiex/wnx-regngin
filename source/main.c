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

static void test_consctuct_nfa();

int main(void) {

	char* regex = "(a|b*)";
	char* regex2 = "(a|b*)(cd|a)";
	char* regex3 = "a(a|b*)cd(ac*|d)";
	NFA_TP nfa;
	nfa_regex_to_nfa(regex3, &nfa);

	return EXIT_SUCCESS;
}

static void test_consctuct_nfa() {
	char nfa_char_a = 'a', nfa_char_b = 'b';
	NFA_TP char_nfa_a, char_nfa_b, or_nfa, closure_nfa;

	nfa_construct_char_nfa(nfa_char_a, &char_nfa_a);
	nfa_construct_char_nfa(nfa_char_b, &char_nfa_b);

	nfa_construct_metachar_nfa('|', &or_nfa, char_nfa_a, char_nfa_b);

	nfa_construct_metachar_nfa('*', &closure_nfa, or_nfa, NULL);
}
