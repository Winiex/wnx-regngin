/*
 * dfa.c
 *
 *  Created on: 2012-11-6
 *      Author: winiex
 */

#include "winiex_dfa.h"

void dfa_construct_from_nfa(NFA_TP nfa, DFA_TP* dfa_p) {
	DFA_STATE_TP dfa_state_start, dfa_state_match, dfa_state_cursor,
			new_state_cursor, dfa_state_cmp_cursor, dfa_state_equal = NULL,
			current_state, state_code_cursor;
	LIST_TP nfa_state_closure_list, nfa_state_move, dfa_state_cmp_list_cursor;
	LIST_ELEM_TP trans_char_cursor, find_match_state_cursor;
	DFA_TRANS_TP trans_cursor;
	NFA_STATE_TP find_match_nfa_state_cursor;
	QUEUE_TP state_set_queue;
	int state_counter = 0, state_code_counter;
	char trans_char;

	queue_init(&state_set_queue);

	nfa_state_closure_list = nfa_state_closure(nfa->nfa_start);
	dfa_state_start = dfa_state_cursor = (DFA_STATE_TP) malloc(
			sizeof(DFA_STATE_T));
	state_counter++;

	dfa_state_cursor->nfa_states_p = nfa_state_closure_list;
	dfa_state_cursor->nfa_states_count = nfa_state_closure_list->list_size;
	dfa_state_cursor->state_type = DFA_STATE_TYPE_START;
	dfa_state_cursor->state_prior = NULL;
	dfa_state_cursor->state_next = NULL;
	dfa_state_cursor->trans_in_head = NULL;

	queue_enqueue_pointer(state_set_queue, dfa_state_cursor);

	while (!queue_is_empty(state_set_queue)) {
		current_state = (DFA_STATE_TP) queue_dequeue_pointer(state_set_queue);
		trans_char_cursor = nfa->char_set->list_head;

		find_match_state_cursor = current_state->nfa_states_p->list_head;

		//如果 current_state 的 state 中包含 nfa 的 nfa_match，那么 current_state 就是构建的
		//dfa 的 dfa_match
		for (; find_match_state_cursor != NULL ; find_match_state_cursor =
				find_match_state_cursor->next) {
			find_match_nfa_state_cursor =
					(NFA_STATE_TP) find_match_state_cursor->pointer_elem;
			if (find_match_nfa_state_cursor->state_code
					== nfa->nfa_match->state_code) {
				dfa_state_match = current_state;
				break;
			}
		}

		if (dfa_state_match == current_state) {
			current_state->state_type = DFA_STATE_TYPE_MATCH;
		}

		for (; trans_char_cursor != NULL ;
				trans_char_cursor = trans_char_cursor->next) {

			trans_char = (char) trans_char_cursor->int_elem;

			nfa_state_move = nfa_state_set_move(current_state->nfa_states_p,
					trans_char);

			if (nfa_state_move->list_size == 0) {
				continue;
			}

			nfa_state_move = nfa_state_set_closure(nfa_state_move);

			dfa_state_cmp_cursor = dfa_state_start;

			for (; dfa_state_cmp_cursor != NULL ; dfa_state_cmp_cursor =
					dfa_state_cmp_cursor->state_next) {
				dfa_state_cmp_list_cursor = dfa_state_cmp_cursor->nfa_states_p;
				if (dfa_states_equal(dfa_state_cmp_list_cursor,
						nfa_state_move)) {
					dfa_state_equal = dfa_state_cmp_cursor;
				}
			}

			if (dfa_state_equal == NULL ) {
				//说明该状态不存在
				new_state_cursor = (DFA_STATE_TP) malloc(sizeof(DFA_STATE_T));

				new_state_cursor->nfa_states_p = nfa_state_move;
				new_state_cursor->nfa_states_count = nfa_state_move->list_size;
				new_state_cursor->state_type = DFA_STATE_TYPE_TRANS;
				new_state_cursor->state_next = NULL;
				new_state_cursor->state_prior = dfa_state_cursor;

				dfa_state_cursor->state_next = new_state_cursor;
				dfa_state_cursor = new_state_cursor;

				trans_cursor = (DFA_TRANS_TP) malloc(sizeof(DFA_TRANS_T));
				trans_cursor->state_from = current_state;
				trans_cursor->state_to = dfa_state_cursor;
				trans_cursor->trans_char = trans_char;
				trans_cursor->trans_next = current_state->trans_out_head;
				trans_cursor->trans_prior = NULL;

				if (current_state->trans_out_head != NULL ) {
					current_state->trans_out_head->trans_prior = trans_cursor;
				}

				current_state->trans_out_head = trans_cursor;

				trans_cursor = (DFA_TRANS_TP) malloc(sizeof(DFA_TRANS_T));
				trans_cursor->state_from = current_state;
				trans_cursor->state_to = dfa_state_cursor;
				trans_cursor->trans_char = trans_char;
				trans_cursor->trans_next = dfa_state_cursor->trans_out_head;
				trans_cursor->trans_prior = NULL;

				if (dfa_state_cursor->trans_out_head != NULL ) {
					dfa_state_cursor->trans_out_head->trans_prior =
							trans_cursor;
				}

				dfa_state_cursor->trans_in_head = trans_cursor;
				state_counter++;
				queue_enqueue_pointer(state_set_queue, dfa_state_cursor);
			} else {
				//说明该状态已经存在
				trans_cursor = (DFA_TRANS_TP) malloc(sizeof(DFA_TRANS_T));
				trans_cursor->state_from = current_state;
				trans_cursor->state_to = dfa_state_equal;
				trans_cursor->trans_char = trans_char;
				trans_cursor->trans_next = current_state->trans_out_head;
				trans_cursor->trans_prior = NULL;

				if (current_state->trans_out_head != NULL ) {
					current_state->trans_out_head->trans_prior = trans_cursor;
				}

				current_state->trans_out_head = trans_cursor;

				trans_cursor = (DFA_TRANS_TP) malloc(sizeof(DFA_TRANS_T));
				trans_cursor->state_from = current_state;
				trans_cursor->state_to = dfa_state_equal;
				trans_cursor->trans_char = trans_char;
				trans_cursor->trans_next = dfa_state_cursor->trans_out_head;
				trans_cursor->trans_prior = NULL;

				if (dfa_state_cursor->trans_out_head != NULL ) {
					dfa_state_cursor->trans_out_head->trans_prior =
							trans_cursor;
				}

				dfa_state_equal->trans_in_head = trans_cursor;
			}
		}
	}

	//处理 state_code
	state_code_cursor = dfa_state_start;
	for (state_code_counter = 0; state_code_cursor != NULL ; state_code_cursor =
			state_code_cursor->state_next) {
		state_code_cursor->state_code = state_code_counter;
		state_code_counter++;
	}

	(*dfa_p) = (DFA_TP) malloc(sizeof(DFA_T));
	(*dfa_p)->dfa_match = dfa_state_match;
	(*dfa_p)->dfa_start = dfa_state_start;
	(*dfa_p)->dfa_states_head = dfa_state_start;
	(*dfa_p)->state_count = state_counter;
	(*dfa_p)->char_set = nfa->char_set;
}

static int dfa_states_equal(LIST_TP list1, LIST_TP list2) {
	LIST_ELEM_TP list1_cursor, list2_cursor;
	NFA_STATE_TP list1_state_cursor, list2_state_cursor;
	int result = 1;

	if (list1->list_size != list2->list_size) {
		return 0;
	}

	for (list1_cursor = list1->list_head, list2_cursor = list2->list_head;
			list1_cursor != NULL && list2_cursor != NULL ;) {
		list1_state_cursor = (NFA_STATE_TP) list1_cursor->pointer_elem;
		list2_state_cursor = (NFA_STATE_TP) list2_cursor->pointer_elem;

		if (list1_state_cursor->state_code != list2_state_cursor->state_code) {
			result = 0;
			break;
		}

		list1_cursor = list1_cursor->next;
		list2_cursor = list2_cursor->next;
	}

	return result;
}

void dfa_print_matrix_style(DFA_TP dfa) {
	DFA_STATE_TP dfa_state_cursor;
	NFA_STATE_TP nfa_state_of_dfa_state;
	LIST_ELEM_TP dfa_state_nfa_state_list_elem_cursor, char_set_cursor;
	DFA_TRANS_TP trans_cursor;
	int counter, char_set_length = dfa->char_set->list_size, print_bool;

	//先打印 DFA 中各个状态包含了哪些 NFA 中的状态
	printf("States of the DFA:\n");
	dfa_state_cursor = dfa->dfa_states_head;

	for (; dfa_state_cursor != NULL ;
			dfa_state_cursor = dfa_state_cursor->state_next) {
		printf("%d = { ", dfa_state_cursor->state_code);
		dfa_state_nfa_state_list_elem_cursor =
				dfa_state_cursor->nfa_states_p->list_head;

		for (; dfa_state_nfa_state_list_elem_cursor != NULL ;
				dfa_state_nfa_state_list_elem_cursor =
						dfa_state_nfa_state_list_elem_cursor->next) {
			nfa_state_of_dfa_state =
					(NFA_STATE_TP) dfa_state_nfa_state_list_elem_cursor->pointer_elem;
			printf("%d ", nfa_state_of_dfa_state->state_code);
		}
		printf("}\n");
	}

	//接下来打印状态转换表
	printf("\nThe state transition table of the DFA:\n");
	dfa_state_cursor = dfa->dfa_states_head;

	printf("+--------");
	for (counter = 0; counter < char_set_length; counter++) {
		printf("--------");
	}
	printf("+\n");

	char_set_cursor = dfa->char_set->list_head;

	printf("|");
	printf("        ");
	for (; char_set_cursor != NULL ; char_set_cursor = char_set_cursor->next) {
		putchar((char) char_set_cursor->int_elem);
		printf("       ");
	}
	printf("|\n");

	for (; dfa_state_cursor != NULL ;
			dfa_state_cursor = dfa_state_cursor->state_next) {
		printf("|%d", dfa_state_cursor->state_code);
		//根据 state_code 的量级来打印不同的空格数目从而好对齐
		dfa_print_blank_by_state_code(dfa_state_cursor->state_code);

		for (char_set_cursor = dfa->char_set->list_head;
				char_set_cursor != NULL ;
				char_set_cursor = char_set_cursor->next) {
			trans_cursor = dfa_state_cursor->trans_out_head;
			print_bool = 0;

			for (; trans_cursor != NULL ;
					trans_cursor = trans_cursor->trans_next) {
				if (trans_cursor->trans_char
						== ((char) char_set_cursor->int_elem)) {
					printf("%d", trans_cursor->state_to->state_code);
					dfa_print_blank_by_state_code(
							trans_cursor->state_to->state_code);
					print_bool = 1;
				}
			}

			if (print_bool == 0) {
				printf("NULL    ");
			}
		}
		printf("|\n");
	}

	printf("+--------");
	for (counter = 0; counter < char_set_length; counter++) {
		printf("--------");
	}
	printf("+\n");
}

static void dfa_print_blank_by_state_code(int state_code) {
	if (state_code < 10) {
		printf("       ");
	} else if (state_code < 100) {
		printf("      ");
	} else if (state_code < 1000) {
		printf("     ");
	} else if (state_code < 10000) {
		printf("    ");
	} else if (state_code < 100000) {
		printf("   ");
	} else if (state_code < 1000000) {
		printf("  ");
	} else if (state_code < 10000000) {
		printf(" ");
	}
}
