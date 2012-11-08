/*
 * dfa.h
 *
 *  Created on: 2012-11-06
 *      Author: winiex
 */

#ifndef NFA_H_
#define NFA_H_

#include "list.h"

struct dfa_state {
	int state_code;
	LIST_TP nfa_states_p;
	struct dfa_state* state_prior;
	struct dfa_state* state_next;
};

typedef struct dfa_state DFA_STATE_T;
typedef struct dfa_state* DFA_STATE_TP;

struct dfa_trans {
	char trans_char;
	DFA_STATE_TP state_from;
	DFA_STATE_TP state_to;
};

typedef struct dfa_trans DFA_TRANS_T;
typedef struct dfa_trans* DFA_TRANS_TP;

struct dfa {
	int state_count;
	DFA_STATE_TP dfa_states_head;
	DFA_STATE_TP dfa_start;
	DFA_STATE_TP dfa_match;
};

void dfa_construct_from_nfa(NFA_TP nfa);

#endif
