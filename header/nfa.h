/*
 * nfa.h
 *
 *  Created on: 2012-10-30
 *      Author: winiex
 */

#ifndef NFA_H_
#define NFA_H_

struct NFA_STATE;
struct NFA_TRANS;
struct NFA;

typedef struct NFA_STATE NFA_STATE_T;
typedef struct NFA_STATE* NFA_STATE_TP;

typedef struct NFA_TRANS NFA_TRANS_T;
typedef struct NFA_TRANS* NFA_TRANS_TP;

typedef struct NFA NFA_T;
typedef struct NFA* NFA_TP;

typedef enum {
	START, MATCH, TRANS
} NFA_STATE_TYPE_T;

struct NFA_STATE {
	int state_code;
	NFA_STATE_TYPE_T type;
	NFA_TRANS_TP trans;
};

struct NFA_TRNAS {
	NFA_STATE_TP from;
	NFA_STATE_TP to;
	int trans_code;
};

struct NFA {
	NFA_STATE_TP states;
	NFA_STATE_TP start;
	NFA_STATE_TP match;
	int state_count;
};

void nfa_regex_to_nfa(char* regex, NFA_TP* nfa);

void nfa_regex_postfix_to_nfa(char* regex_post, NFA_TP * nfa);

void nfa_construct_char_nfa(char )

#endif /* NFA_H_ */
