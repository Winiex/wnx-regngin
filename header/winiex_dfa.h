/*
 * dfa.h
 *
 *  Created on: 2012-11-06
 *      Author: winiex
 */

#ifndef WINIEX_DFA_H_
#define WINIEX_DFA_H_

#include "winiex_nfa.h"
#include "winiex_queue.h"
#include "winiex_list.h"

struct DFA_STATE;
struct DFA_TRANS;
struct DFA;

typedef struct DFA_STATE DFA_STATE_T;
typedef struct DFA_STATE* DFA_STATE_TP;

typedef struct DFA_TRANS DFA_TRANS_T;
typedef struct DFA_TRANS* DFA_TRANS_TP;

typedef struct DFA DFA_T;
typedef struct DFA* DFA_TP;

typedef enum DFA_STATE_TYPE {
	DFA_STATE_TYPE_START, DFA_STATE_TYPE_TRANS, DFA_STATE_TYPE_MATCH
} DFA_STATE_TYPE_T;


struct DFA_STATE {
	int state_code;
	int nfa_states_count;
	DFA_STATE_TYPE_T state_type;
	LIST_TP nfa_states_p;
	DFA_TRANS_TP trans_in_head;
	DFA_TRANS_TP trans_out_head;
	DFA_STATE_TP state_prior;
	DFA_STATE_TP state_next;
};

struct DFA_TRANS {
	char trans_char;
	DFA_STATE_TP state_from;
	DFA_STATE_TP state_to;
	DFA_TRANS_TP trans_prior;
	DFA_TRANS_TP trans_next;
};

struct DFA {
	int state_count;
	DFA_STATE_TP dfa_states_head;
	DFA_STATE_TP dfa_start;
	DFA_STATE_TP dfa_match;
	LIST_TP char_set;
};

void dfa_construct_from_nfa(NFA_TP nfa, DFA_TP* dfa_p);

void dfa_print_matrix_style(DFA_TP dfa);

static int dfa_states_equal(LIST_TP list1, LIST_TP list2);

static void dfa_print_blank_by_state_code(int state_code);

#endif
