/*
 * nfa.c
 *
 *  Created on: 2012-10-30
 *      Author: winiex
 */

#include "winiex_nfa.h"

void nfa_regex_to_nfa(char* regex, NFA_TP* nfa_p) {
	int regex_memory_size = -1;
	char* regex_infix_with_cat;
	char* regex_postfix;
	LIST_TP char_set;

	//首先获取 regex 的 postfix 形式 regex_postfix
	regex_memory_size = regex_measure_regex_memory_size(regex);
	regex_infix_with_cat = (char *) malloc(regex_memory_size * sizeof(char));
	regex_postfix = (char *) malloc(regex_memory_size * sizeof(char));
	regex_insert_cat_char(regex, regex_infix_with_cat);
	regex_infix_to_postfix(regex_infix_with_cat, regex_postfix);

	//创建 nfa
	nfa_regex_postfix_to_nfa(regex_postfix, nfa_p);

	char_set = regex_get_char_set(regex);
	(*nfa_p)->char_set = char_set;

	free(regex_infix_with_cat);
	free(regex_postfix);
}

void nfa_regex_postfix_to_nfa(char* regex_postfix, NFA_TP* nfa_p) {
	char current_char;
	int counter = 0, str_length;
	NFA_TP char_nfa, meta_nfa, nfa1, nfa2;
	STACK_TP nfa_stack;

	str_length = strlen(regex_postfix);
	stack_init(&nfa_stack);

	for (; counter < str_length; counter++) {
		current_char = regex_postfix[counter];

		if (regex_is_char_or_number(current_char)) {
			nfa_construct_char_nfa(current_char, &char_nfa);
			stack_push_pointer(nfa_stack, char_nfa);
		} else {
			switch (current_char) {
			case '|':
				nfa1 = (NFA_TP) stack_pop_pointer(nfa_stack);
				nfa2 = (NFA_TP) stack_pop_pointer(nfa_stack);
				nfa_construct_metachar_nfa('|', &meta_nfa, nfa1, nfa2);
				stack_push_pointer(nfa_stack, meta_nfa);
				break;
			case '*':
				nfa1 = (NFA_TP) stack_pop_pointer(nfa_stack);
				nfa_construct_metachar_nfa('*', &meta_nfa, nfa1, NULL );
				stack_push_pointer(nfa_stack, meta_nfa);
				break;
			case '?':
				nfa1 = (NFA_TP) stack_pop_pointer(nfa_stack);
				nfa_construct_metachar_nfa('?', &meta_nfa, nfa1, NULL );
				stack_push_pointer(nfa_stack, meta_nfa);
				break;
			case '&':
				nfa1 = (NFA_TP) stack_pop_pointer(nfa_stack);
				nfa2 = (NFA_TP) stack_pop_pointer(nfa_stack);
				nfa_construct_metachar_nfa('&', &meta_nfa, nfa1, nfa2);
				stack_push_pointer(nfa_stack, meta_nfa);
				break;
			case '+':
				nfa1 = (NFA_TP) stack_pop_pointer(nfa_stack);
				nfa_construct_metachar_nfa('+', &meta_nfa, nfa1, NULL );
				stack_push_pointer(nfa_stack, meta_nfa);
				break;
			}
		}
	}

	(*nfa_p) = (NFA_TP) stack_pop_pointer(nfa_stack);
	stack_destroy(&nfa_stack);
}

void nfa_construct_char_nfa(char char_elem, NFA_TP* nfa_p) {
	NFA_STATE_TP start_state, match_state;
	NFA_TRANS_TP trans_char_elem;
	NFA_TP nfa;

	nfa = (NFA_TP) malloc(sizeof(NFA_T));

	start_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));
	match_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));

	start_state->state_type = NFA_STATE_TYPE_START;
	start_state->trans_in_head = NULL;

	match_state->state_type = NFA_STATE_TYPE_MATCH;
	match_state->trans_out_head = NULL;

	//为 start_state 构建 trans_out_head
	trans_char_elem = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));
	trans_char_elem->state_from = start_state;
	trans_char_elem->state_to = match_state;
	trans_char_elem->trans_char = char_elem;
	trans_char_elem->trans_next = NULL;
	trans_char_elem->trans_prior = NULL;

	start_state->trans_out_head = trans_char_elem;

	//为 match_state 构建 trans_in_head
	trans_char_elem = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));
	trans_char_elem->state_from = start_state;
	trans_char_elem->state_to = match_state;
	trans_char_elem->trans_char = char_elem;
	trans_char_elem->trans_next = NULL;
	trans_char_elem->trans_prior = NULL;

	match_state->trans_in_head = trans_char_elem;

	//接下来构建 nfa_reform_result 的 nfa_states_head
	start_state->state_prior = NULL;
	match_state->state_prior = start_state;
	start_state->state_next = match_state;
	match_state->state_next = NULL;

	nfa->inner_nfas = NULL;
	nfa->nfa_prior = NULL;
	nfa->nfa_next = NULL;
	nfa->nfa_states_head = start_state;
	nfa->nfa_start = start_state;
	nfa->nfa_match = match_state;
	nfa->nfa_type = NFA_TYPE_CHAR;
	nfa->state_count = 2;

	(*nfa_p) = nfa;

	//处理各个 state 的 state_code
	start_state->state_code = 0;
	match_state->state_code = 1;
}

void nfa_construct_metachar_nfa(char meta_char_elem, NFA_TP* meta_nfa,
		NFA_TP nfa1, NFA_TP nfa2) {
	NFA_TP nfa = (*meta_nfa) = (NFA_TP) malloc(sizeof(NFA_T));

	switch (meta_char_elem) {
	case '|':
		nfa_reform_or(nfa, nfa1, nfa2);
		break;
	case '*':
		nfa_reform_closure(nfa, nfa1);
		break;
	case '?':
		nfa_reform_one_or_none(nfa, nfa1);
		break;
	case '&':
		nfa_reform_cat(nfa, nfa1, nfa2);
		break;
	case '+':
		nfa_reform_one_or_more(nfa, nfa1);
		break;
	default:
		fprintf(stderr, "meta_char_elem WRONG!");
		break;
	}
}

LIST_TP nfa_state_closure(NFA_STATE_TP state) {
	return nfa_state_closure_by_char(state, NULL_CHAR);
}

LIST_TP nfa_state_closure_by_char(NFA_STATE_TP state, char char_param) {
	NFA_STATE_TP state_cursor1, state_cursor2;
	NFA_TRANS_TP trans_cursor;
	QUEUE_TP state_queue;
	LIST_TP state_result_list;
	int state_visited[1000], counter;

	for (counter = 0; counter < 1000; counter++) {
		state_visited[counter] = 0;
	}

	list_init(&state_result_list);
	queue_init(&state_queue);

	//使用广度优先遍历的方式来找到 state 经过空转换能够达到的所有状态
	queue_enqueue_pointer(state_queue, state);
	list_add_pointer(state_result_list, state);
	state_visited[state->state_code] = 1;

	while (!queue_is_empty(state_queue)) {
		state_cursor1 = (NFA_STATE_TP) queue_dequeue_pointer(state_queue);
		trans_cursor = state_cursor1->trans_out_head;
		state_visited[state_cursor1->state_code] = 1;

		for (; trans_cursor != NULL ; trans_cursor = trans_cursor->trans_next) {
			if (trans_cursor->trans_char == char_param
					&& !state_visited[trans_cursor->state_to->state_code]) {
				state_cursor2 = trans_cursor->state_to;
				queue_enqueue_pointer(state_queue, state_cursor2);
				list_add_pointer(state_result_list, state_cursor2);
				state_visited[state_cursor2->state_code] = 1;
			}
		}
	}

	return state_result_list;
}

LIST_TP nfa_state_set_closure(LIST_TP state_set) {
	NFA_STATE_TP state_cursor;
	LIST_TP list_result, list_cursor, list_cat_cursor;
	LIST_ELEM_TP list_elem_cursor;

	list_elem_cursor = state_set->list_head;
	state_cursor = (NFA_STATE_TP) list_elem_cursor->pointer_elem;
	list_cat_cursor = nfa_state_closure(state_cursor);

	list_elem_cursor = list_elem_cursor->next;

	for (; list_elem_cursor != NULL ; list_elem_cursor =
			list_elem_cursor->next) {

		state_cursor = (NFA_STATE_TP) list_elem_cursor->pointer_elem;
		list_cursor = nfa_state_closure(state_cursor);

		list_cat_cursor->list_tail->next = list_cursor->list_head;
		list_cursor->list_head->prior = list_cat_cursor->list_tail;

		list_cat_cursor = list_cat(list_cat_cursor, list_cursor);

		// 这里的状态会存在重复的，需要做一个集合去除重复元素的函数
		if (list_cat_cursor != NULL && list_cat_cursor->list_size != 0) {
			nfa_remove_nfa_states_duplicates(list_cat_cursor);
		}
	}

	list_result = list_cat_cursor;

	return list_result;
}

LIST_TP nfa_state_move(NFA_STATE_TP state, char move_char) {
	LIST_TP move_to_states_list = NULL;
	NFA_TRANS_TP trans_cursor;

	list_init(&move_to_states_list);

	trans_cursor = state->trans_out_head;

	for (; trans_cursor != NULL ; trans_cursor = trans_cursor->trans_next) {
		if (trans_cursor->trans_char == move_char) {
			list_add_pointer(move_to_states_list, trans_cursor->state_to);
		}
	}

	return move_to_states_list;
}

LIST_TP nfa_state_set_move(LIST_TP state_set, char move_char) {
	LIST_TP list_result, list_cursor, list_cat_cursor;
	LIST_ELEM_TP list_elem_cursor;
	NFA_STATE_TP state_cursor;

	list_elem_cursor = state_set->list_head;
	state_cursor = (NFA_STATE_TP) list_elem_cursor->pointer_elem;
	list_cat_cursor = nfa_state_move(state_cursor, move_char);

	list_elem_cursor = list_elem_cursor->next;

	for (; list_elem_cursor != NULL ; list_elem_cursor =
			list_elem_cursor->next) {
		state_cursor = (NFA_STATE_TP) list_elem_cursor->pointer_elem;
		list_cursor = nfa_state_move(state_cursor, move_char);

		list_cat_cursor = list_cat(list_cat_cursor, list_cursor);

		// 这里的状态会存在重复的，需要做一个集合去除重复元素的函数
		if (list_cat_cursor != NULL && list_cat_cursor->list_size != 0) {
			nfa_remove_nfa_states_duplicates(list_cat_cursor);
		}
	}

	list_result = list_cat_cursor;

	return list_result;
}

void nfa_destroy(NFA_TP* nfa_p) {

}

static void nfa_remove_nfa_states_duplicates(LIST_TP states_list) {
	LIST_ELEM_TP current_elem, inner_elem;
	NFA_STATE_TP current_state, inner_state;
	int current_state_code;

	current_elem = states_list->list_head;

	for (; current_elem != NULL ; current_elem = current_elem->next) {
		current_state = (NFA_STATE_TP) current_elem->pointer_elem;
		current_state_code = current_state->state_code;

		for (inner_elem = current_elem->next; inner_elem != NULL ; inner_elem =
				inner_elem->next) {
			inner_state = (NFA_STATE_TP) inner_elem->pointer_elem;
			if (inner_state->state_code == current_state_code) {
				list_delete_elem(states_list, inner_elem);
			}
		}
	}
}

static void nfa_reform_or(NFA_TP nfa_reform_result, NFA_TP nfa1, NFA_TP nfa2) {
	int state_code_counter;
	NFA_STATE_TP result_start_state, result_match_state, state_cursor;
	NFA_TRANS_TP result_start_to_nfa1_trans, result_start_to_nfa2_trans,
			nfa1_match_to_result_match_trans, nfa2_match_to_result_match_trans;

	result_start_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));
	result_match_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));

	result_start_state->state_type = NFA_STATE_TYPE_START;
	result_start_state->trans_in_head = NULL;

	result_match_state->state_type = NFA_STATE_TYPE_MATCH;
	result_match_state->trans_out_head = NULL;

	//为 result_start_state 构建 trans_out_head
	result_start_to_nfa1_trans = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));
	result_start_to_nfa1_trans->state_from = result_start_state;
	result_start_to_nfa1_trans->state_to = nfa1->nfa_start;
	result_start_to_nfa1_trans->trans_char = NULL_CHAR;
	result_start_to_nfa1_trans->trans_prior = NULL;
	result_start_to_nfa1_trans->trans_next = result_start_to_nfa2_trans =
			(NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));

	result_start_to_nfa2_trans->trans_prior = result_start_to_nfa1_trans;
	result_start_to_nfa2_trans->trans_next = NULL;
	result_start_to_nfa2_trans->state_from = result_start_state;
	result_start_to_nfa2_trans->state_to = nfa2->nfa_start;
	result_start_to_nfa2_trans->trans_char = NULL_CHAR;

	result_start_state->trans_out_head = result_start_to_nfa1_trans;

	//为 nfa1->nfa_start 构建 trans_in_head
	result_start_to_nfa1_trans = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));
	result_start_to_nfa1_trans->state_from = result_start_state;
	result_start_to_nfa1_trans->state_to = nfa1->nfa_start;
	result_start_to_nfa1_trans->trans_char = NULL_CHAR;
	result_start_to_nfa1_trans->trans_prior = NULL;
	result_start_to_nfa1_trans->trans_next = NULL;

	nfa1->nfa_start->trans_in_head = result_start_to_nfa1_trans;

	//为 nfa2->start_state 构建 trans_in_head
	result_start_to_nfa2_trans = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));
	result_start_to_nfa2_trans->state_from = result_start_state;
	result_start_to_nfa2_trans->state_to = nfa2->nfa_start;
	result_start_to_nfa2_trans->trans_char = NULL_CHAR;
	result_start_to_nfa2_trans->trans_prior = NULL;
	result_start_to_nfa2_trans->trans_next = NULL;

	nfa2->nfa_start->trans_in_head = result_start_to_nfa2_trans;

	//为 nfa1->nfa_match 构建 trans_out_head
	nfa1_match_to_result_match_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	nfa1_match_to_result_match_trans->state_from = nfa1->nfa_match;
	nfa1_match_to_result_match_trans->state_to = result_match_state;
	nfa1_match_to_result_match_trans->trans_char = NULL_CHAR;
	nfa1_match_to_result_match_trans->trans_prior = NULL;
	nfa1_match_to_result_match_trans->trans_next = NULL;

	nfa1->nfa_match->trans_out_head = nfa1_match_to_result_match_trans;

	//为 nfa2->nfa_match 构建 trans_out_head
	nfa2_match_to_result_match_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	nfa2_match_to_result_match_trans->state_from = nfa2->nfa_match;
	nfa2_match_to_result_match_trans->state_to = result_match_state;
	nfa2_match_to_result_match_trans->trans_char = NULL_CHAR;
	nfa2_match_to_result_match_trans->trans_prior = NULL;
	nfa2_match_to_result_match_trans->trans_next = NULL;

	nfa2->nfa_match->trans_out_head = nfa2_match_to_result_match_trans;

	//为 result_match_state 构建 trans_in_head
	nfa1_match_to_result_match_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	nfa1_match_to_result_match_trans->state_from = nfa1->nfa_match;
	nfa1_match_to_result_match_trans->state_to = result_match_state;
	nfa1_match_to_result_match_trans->trans_char = NULL_CHAR;
	nfa1_match_to_result_match_trans->trans_prior = NULL;
	nfa1_match_to_result_match_trans->trans_next =
			nfa2_match_to_result_match_trans = (NFA_TRANS_TP) malloc(
					sizeof(NFA_TRANS_T));

	nfa2_match_to_result_match_trans->state_from = nfa2->nfa_match;
	nfa2_match_to_result_match_trans->state_to = result_match_state;
	nfa2_match_to_result_match_trans->trans_char = NULL_CHAR;
	nfa2_match_to_result_match_trans->trans_prior =
			nfa1_match_to_result_match_trans;
	nfa2_match_to_result_match_trans->trans_next = NULL;

	result_match_state->trans_in_head = nfa1_match_to_result_match_trans;

	//接下来构建 nfa_reform_result 的 nfa_states_head
	result_start_state->state_prior = NULL;
	result_start_state->state_next = nfa1->nfa_start;
	nfa1->nfa_start->state_prior = result_start_state;

	nfa1->nfa_match->state_next = nfa2->nfa_start;
	nfa2->nfa_start->state_prior = nfa1->nfa_match;

	nfa2->nfa_match->state_next = result_match_state;
	result_match_state->state_prior = nfa2->nfa_match;
	result_match_state->state_next = NULL;

	//构建本 NFA 内部的 NFA 链表
	nfa1->nfa_prior = NULL;
	nfa1->nfa_next = nfa2;
	nfa2->nfa_prior = nfa1;
	nfa2->nfa_next = NULL;

	//一切就绪，开始构建新组建的 NFA
	nfa_reform_result->inner_nfas = nfa1;
	nfa_reform_result->nfa_prior = NULL;
	nfa_reform_result->nfa_next = NULL;
	nfa_reform_result->nfa_start = result_start_state;
	nfa_reform_result->nfa_match = result_match_state;
	nfa_reform_result->nfa_states_head = result_start_state;
	nfa_reform_result->nfa_type = NFA_TYPE_OR;
	nfa_reform_result->state_count = nfa1->state_count + nfa2->state_count + 2;

	//处理各个 state 的 state_code
	state_cursor = nfa_reform_result->nfa_states_head;
	state_code_counter = 0;
	for (; state_cursor != NULL ; state_cursor = state_cursor->state_next) {
		state_cursor->state_code = state_code_counter;
		state_code_counter++;
	}
}

static void nfa_reform_closure(NFA_TP nfa_reform_result, NFA_TP nfa) {
	int state_code_counter;
	NFA_STATE_TP result_start_state, result_match_state, state_cursor;
	NFA_TRANS_TP result_start_to_inner_nfa_trans,
			inner_nfa_match_to_start_trans, result_inner_nfa_to_match_trans,
			result_start_to_match_trans;

	result_start_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));
	result_match_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));

	result_start_state->state_type = NFA_STATE_TYPE_START;
	result_start_state->trans_in_head = NULL;

	result_match_state->state_type = NFA_STATE_TYPE_MATCH;
	result_match_state->trans_out_head = NULL;

	//为 result_start_state 构建 trans_out_head
	result_start_to_inner_nfa_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	result_start_to_inner_nfa_trans->state_from = result_start_state;
	result_start_to_inner_nfa_trans->state_to = nfa->nfa_start;
	result_start_to_inner_nfa_trans->trans_char = NULL_CHAR;
	result_start_to_inner_nfa_trans->trans_prior = NULL;
	result_start_to_inner_nfa_trans->trans_next = result_start_to_match_trans =
			(NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));

	result_start_to_match_trans->trans_prior = result_start_to_inner_nfa_trans;
	result_start_to_match_trans->trans_next = NULL;
	result_start_to_match_trans->state_from = result_start_state;
	result_start_to_match_trans->state_to = result_match_state;
	result_start_to_match_trans->trans_char = NULL_CHAR;

	result_start_state->trans_out_head = result_start_to_inner_nfa_trans;

	//为 nfa->nfa_start 构建 trans_in_head
	inner_nfa_match_to_start_trans = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));
	inner_nfa_match_to_start_trans->state_from = nfa->nfa_match;
	inner_nfa_match_to_start_trans->state_to = nfa->nfa_start;
	inner_nfa_match_to_start_trans->trans_char = NULL_CHAR;
	inner_nfa_match_to_start_trans->trans_prior = NULL;
	inner_nfa_match_to_start_trans->trans_next =
			result_start_to_inner_nfa_trans = (NFA_TRANS_TP) malloc(
					sizeof(NFA_TRANS_T));

	result_start_to_inner_nfa_trans->trans_prior =
			inner_nfa_match_to_start_trans;
	result_start_to_inner_nfa_trans->trans_next = NULL;
	result_start_to_inner_nfa_trans->trans_char = NULL_CHAR;
	result_start_to_inner_nfa_trans->state_from = result_start_state;
	result_start_to_inner_nfa_trans->state_to = nfa->nfa_start;

	nfa->nfa_start->trans_in_head = inner_nfa_match_to_start_trans;

	//为 nfa->nfa_match 构建 trans_out_head
	inner_nfa_match_to_start_trans = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));
	inner_nfa_match_to_start_trans->state_from = nfa->nfa_match;
	inner_nfa_match_to_start_trans->state_to = nfa->nfa_start;
	inner_nfa_match_to_start_trans->trans_char = NULL_CHAR;
	inner_nfa_match_to_start_trans->trans_prior = NULL;
	inner_nfa_match_to_start_trans->trans_next =
			result_inner_nfa_to_match_trans = (NFA_TRANS_TP) malloc(
					sizeof(NFA_TRANS_T));

	result_inner_nfa_to_match_trans->trans_prior =
			inner_nfa_match_to_start_trans;
	result_inner_nfa_to_match_trans->trans_next = NULL;
	result_inner_nfa_to_match_trans->state_from = nfa->nfa_match;
	result_inner_nfa_to_match_trans->state_to = result_match_state;
	result_inner_nfa_to_match_trans->trans_char = NULL_CHAR;

	nfa->nfa_match->trans_out_head = inner_nfa_match_to_start_trans;

	//为 result_match_state 构建 trans_in_head
	result_start_to_match_trans = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));
	result_start_to_match_trans->state_from = result_start_state;
	result_start_to_match_trans->state_to = result_match_state;
	result_start_to_match_trans->trans_char = NULL_CHAR;
	result_start_to_match_trans->trans_prior = NULL;
	result_start_to_match_trans->trans_next = result_inner_nfa_to_match_trans =
			(NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));

	result_inner_nfa_to_match_trans->trans_prior = result_start_to_match_trans;
	result_inner_nfa_to_match_trans->trans_next = NULL;
	result_inner_nfa_to_match_trans->state_from = nfa->nfa_match;
	result_inner_nfa_to_match_trans->state_to = result_match_state;
	result_inner_nfa_to_match_trans->trans_char = NULL_CHAR;

	result_match_state->trans_in_head = result_start_to_match_trans;

	//接下来构建 nfa_reform_result 的 nfa_states_head
	result_start_state->state_prior = NULL;
	result_start_state->state_next = nfa->nfa_start;
	nfa->nfa_start->state_prior = result_start_state;
	nfa->nfa_match->state_next = result_match_state;
	result_match_state->state_prior = nfa->nfa_match;
	result_match_state->state_next = NULL;

	//一切准备就绪，开始构建闭包 NFA
	nfa_reform_result->inner_nfas = nfa;
	nfa_reform_result->nfa_prior = NULL;
	nfa_reform_result->nfa_next = NULL;
	nfa_reform_result->nfa_start = result_start_state;
	nfa_reform_result->nfa_match = result_match_state;
	nfa_reform_result->nfa_states_head = result_start_state;
	nfa_reform_result->nfa_type = NFA_TYPE_CLOSURE;
	nfa_reform_result->state_count = nfa->state_count + 2;

	//处理各个 state 的 state_code
	state_cursor = nfa_reform_result->nfa_states_head;
	state_code_counter = 0;
	for (; state_cursor != NULL ; state_cursor = state_cursor->state_next) {
		state_cursor->state_code = state_code_counter;
		state_code_counter++;
	}
}

static void nfa_reform_one_or_none(NFA_TP nfa_reform_result, NFA_TP nfa) {
	int state_code_counter;
	NFA_STATE_TP result_start_state, result_match_state, state_cursor;
	NFA_TRANS_TP result_start_to_inner_nfa_trans, result_start_to_match_trans,
			result_inner_nfa_to_match_trans;

	result_start_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));
	result_match_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));

	result_start_state->state_type = NFA_STATE_TYPE_START;
	result_start_state->trans_in_head = NULL;

	result_match_state->state_type = NFA_STATE_TYPE_MATCH;
	result_match_state->trans_out_head = NULL;

	//为 result_start_state 构建 trans_out_head
	result_start_to_inner_nfa_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	result_start_to_inner_nfa_trans->state_from = result_start_state;
	result_start_to_inner_nfa_trans->state_to = nfa->nfa_start;
	result_start_to_inner_nfa_trans->trans_char = NULL_CHAR;
	result_start_to_inner_nfa_trans->trans_prior = NULL;
	result_start_to_inner_nfa_trans->trans_next = result_start_to_match_trans =
			(NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));

	result_start_to_match_trans->trans_prior = result_start_to_inner_nfa_trans;
	result_start_to_match_trans->trans_next = NULL;
	result_start_to_match_trans->state_from = result_start_state;
	result_start_to_match_trans->state_to = result_match_state;
	result_start_to_match_trans->trans_char = NULL_CHAR;

	result_start_state->trans_out_head = result_start_to_inner_nfa_trans;

	//为 nfa->nfa_start 构建 trans_in_head
	result_start_to_inner_nfa_trans->state_from = result_start_state;
	result_start_to_inner_nfa_trans->state_to = nfa->nfa_start;
	result_start_to_inner_nfa_trans->trans_char = NULL_CHAR;
	result_start_to_inner_nfa_trans->trans_prior = NULL;
	result_start_to_inner_nfa_trans->trans_next = NULL;

	nfa->nfa_start->trans_in_head = result_start_to_inner_nfa_trans;

	//为 result_match_state 构建 trans_in_head
	result_inner_nfa_to_match_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	result_inner_nfa_to_match_trans->state_from = nfa->nfa_match;
	result_inner_nfa_to_match_trans->state_to = result_match_state;
	result_inner_nfa_to_match_trans->trans_char = NULL_CHAR;
	result_inner_nfa_to_match_trans->trans_prior = NULL;
	result_inner_nfa_to_match_trans->trans_next = result_start_to_match_trans =
			(NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));

	result_start_to_match_trans->trans_prior = result_inner_nfa_to_match_trans;
	result_start_to_match_trans->trans_next = NULL;
	result_start_to_match_trans->state_from = result_start_state;
	result_start_to_match_trans->state_to = result_match_state;
	result_start_to_match_trans->trans_char = NULL_CHAR;

	result_match_state->trans_in_head = result_inner_nfa_to_match_trans;

	//为 nfa->nfa_match 构建 trans_out_head
	result_inner_nfa_to_match_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	result_inner_nfa_to_match_trans->state_from = nfa->nfa_match;
	result_inner_nfa_to_match_trans->state_to = result_match_state;
	result_inner_nfa_to_match_trans->trans_char = NULL_CHAR;
	result_inner_nfa_to_match_trans->trans_prior = NULL;
	result_inner_nfa_to_match_trans->trans_next = NULL;

	nfa->nfa_match->trans_out_head = result_inner_nfa_to_match_trans;

	//接下来构建 nfa_reform_result 的 nfa_states_head
	result_start_state->state_prior = NULL;
	result_start_state->state_next = nfa->nfa_start;
	nfa->nfa_start->state_prior = result_start_state;
	nfa->nfa_match->state_next = result_match_state;
	result_match_state->state_prior = nfa->nfa_match;
	result_match_state->state_next = NULL;

	//一切准备就绪，开始构建 nfa_reform_result
	nfa_reform_result->inner_nfas = nfa;
	nfa_reform_result->nfa_prior = NULL;
	nfa_reform_result->nfa_next = NULL;
	nfa_reform_result->nfa_start = result_start_state;
	nfa_reform_result->nfa_match = result_match_state;
	nfa_reform_result->nfa_states_head = result_start_state;
	nfa_reform_result->nfa_type = NFA_TYPE_ONE_OR_NONE;
	nfa_reform_result->state_count = nfa->state_count + 2;

	//处理各个 state 的 state_code
	state_cursor = nfa_reform_result->nfa_states_head;
	state_code_counter = 0;
	for (; state_cursor != NULL ; state_cursor = state_cursor->state_next) {
		state_cursor->state_code = state_code_counter;
		state_code_counter++;
	}
}

static void nfa_reform_cat(NFA_TP nfa_reform_result, NFA_TP nfa1, NFA_TP nfa2) {
	int state_code_counter;
	NFA_STATE_TP result_start_state, result_match_state, state_cursor;
	NFA_TRANS_TP result_start_to_inner_nfa1_trans,
			nfa1_match_to_nfa2_start_trans, inner_nfa2_to_result_match_trans;

	result_start_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));
	result_match_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));

	result_start_state->state_type = NFA_STATE_TYPE_START;
	result_start_state->trans_in_head = NULL;

	result_match_state->state_type = NFA_STATE_TYPE_MATCH;
	result_match_state->trans_out_head = NULL;

	//为 result_start_state 构建 trans_out_head
	result_start_to_inner_nfa1_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	result_start_to_inner_nfa1_trans->state_from = result_start_state;
	result_start_to_inner_nfa1_trans->state_to = nfa1->nfa_start;
	result_start_to_inner_nfa1_trans->trans_char = NULL_CHAR;
	result_start_to_inner_nfa1_trans->trans_prior = NULL;
	result_start_to_inner_nfa1_trans->trans_next = NULL;

	result_start_state->trans_out_head = result_start_to_inner_nfa1_trans;

	//为 nfa1->nfa_start 构建 trans_in_head
	result_start_to_inner_nfa1_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	result_start_to_inner_nfa1_trans->state_from = result_start_state;
	result_start_to_inner_nfa1_trans->state_to = nfa1->nfa_start;
	result_start_to_inner_nfa1_trans->trans_char = NULL_CHAR;
	result_start_to_inner_nfa1_trans->trans_prior = NULL;
	result_start_to_inner_nfa1_trans->trans_next = NULL;

	nfa1->nfa_start->trans_in_head = result_start_to_inner_nfa1_trans;

	//为 nfa1->nfa_match 构建 trans_out_head
	nfa1_match_to_nfa2_start_trans = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));
	nfa1_match_to_nfa2_start_trans->trans_prior = NULL;
	nfa1_match_to_nfa2_start_trans->trans_next = NULL;
	nfa1_match_to_nfa2_start_trans->state_from = nfa1->nfa_match;
	nfa1_match_to_nfa2_start_trans->state_to = nfa2->nfa_start;
	nfa1_match_to_nfa2_start_trans->trans_char = NULL_CHAR;

	nfa1->nfa_match->trans_out_head = nfa1_match_to_nfa2_start_trans;

	//为 nfa1->nfa_start 构建 trans_in_head
	nfa1_match_to_nfa2_start_trans = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));
	nfa1_match_to_nfa2_start_trans->trans_prior = NULL;
	nfa1_match_to_nfa2_start_trans->trans_next = NULL;
	nfa1_match_to_nfa2_start_trans->state_from = nfa1->nfa_match;
	nfa1_match_to_nfa2_start_trans->state_to = nfa2->nfa_start;
	nfa1_match_to_nfa2_start_trans->trans_char = NULL_CHAR;

	nfa1->nfa_start->trans_in_head = nfa1_match_to_nfa2_start_trans;

	//为 nfa2->nfa_match 构建 trans_out_head
	inner_nfa2_to_result_match_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	inner_nfa2_to_result_match_trans->trans_prior = NULL;
	inner_nfa2_to_result_match_trans->trans_next = NULL;
	inner_nfa2_to_result_match_trans->trans_char = NULL_CHAR;
	inner_nfa2_to_result_match_trans->state_from = nfa2->nfa_match;
	inner_nfa2_to_result_match_trans->state_to = result_match_state;

	nfa2->nfa_match->trans_out_head = inner_nfa2_to_result_match_trans;

	//为 result_match_state 构建 trans_in_state
	inner_nfa2_to_result_match_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	inner_nfa2_to_result_match_trans->trans_prior = NULL;
	inner_nfa2_to_result_match_trans->trans_next = NULL;
	inner_nfa2_to_result_match_trans->trans_char = NULL_CHAR;
	inner_nfa2_to_result_match_trans->state_from = nfa2->nfa_match;
	inner_nfa2_to_result_match_trans->state_to = result_match_state;

	result_match_state->trans_in_head = inner_nfa2_to_result_match_trans;

	//接下来构建 nfa_reform_result 的 nfa_states_head
	result_start_state->state_prior = NULL;
	result_start_state->state_next = nfa1->nfa_start;
	nfa1->nfa_start->state_prior = result_start_state;

	nfa1->nfa_match->state_next = nfa2->nfa_start;
	nfa2->nfa_start->state_prior = nfa1->nfa_match;

	result_match_state->state_prior = nfa2->nfa_match;
	nfa2->nfa_match->state_next = result_match_state;
	result_match_state->state_next = NULL;

	//接下来构建 nfa_reform_result 的 inner_nfas
	nfa1->nfa_prior = NULL;
	nfa1->nfa_next = nfa2;
	nfa1->nfa_prior = nfa1;
	nfa2->nfa_next = NULL;

	//一切准备就绪，开始构建 nfa_reform_result
	nfa_reform_result->inner_nfas = nfa1;
	nfa_reform_result->nfa_match = result_match_state;
	nfa_reform_result->nfa_start = result_start_state;
	nfa_reform_result->nfa_next = NULL;
	nfa_reform_result->nfa_prior = NULL;
	nfa_reform_result->nfa_states_head = result_start_state;
	nfa_reform_result->nfa_type = NFA_TYPE_CAT;
	nfa_reform_result->state_count = nfa1->state_count + nfa2->state_count + 2;

	//处理各个 state 的 state_code
	state_cursor = nfa_reform_result->nfa_states_head;
	state_code_counter = 0;
	for (; state_cursor != NULL ; state_cursor = state_cursor->state_next) {
		state_cursor->state_code = state_code_counter;
		state_code_counter++;
	}
}

static void nfa_reform_one_or_more(NFA_TP nfa_reform_result, NFA_TP nfa) {
	int state_code_counter;
	NFA_STATE_TP result_start_state, result_match_state, state_cursor;
	NFA_TRANS_TP result_start_to_inner_nfa_start_trans,
			inner_nfa_match_to_start_trans,
			inner_nfa_match_to_result_match_trans;

	result_start_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));
	result_match_state = (NFA_STATE_TP) malloc(sizeof(NFA_STATE_T));

	result_start_state->trans_in_head = NULL;
	result_start_state->state_type = NFA_STATE_TYPE_START;

	result_match_state->trans_out_head = NULL;
	result_match_state->state_type = NFA_STATE_TYPE_MATCH;

	//为 result_start_state 构建 trans_out_head
	result_start_to_inner_nfa_start_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	result_start_to_inner_nfa_start_trans->state_from = result_start_state;
	result_start_to_inner_nfa_start_trans->state_to = nfa->nfa_start;
	result_start_to_inner_nfa_start_trans->trans_char = NULL_CHAR;
	result_start_to_inner_nfa_start_trans->trans_next = NULL;
	result_start_to_inner_nfa_start_trans->trans_prior = NULL;

	result_start_state->trans_out_head = result_start_to_inner_nfa_start_trans;

	//为 nfa->nfa_start 构建 trans_in_head
	result_start_to_inner_nfa_start_trans = (NFA_TRANS_TP) malloc(
			sizeof(NFA_TRANS_T));
	result_start_to_inner_nfa_start_trans->state_from = result_start_state;
	result_start_to_inner_nfa_start_trans->state_to = nfa->nfa_start;
	result_start_to_inner_nfa_start_trans->trans_char = NULL_CHAR;
	result_start_to_inner_nfa_start_trans->trans_next = NULL;
	result_start_to_inner_nfa_start_trans->trans_prior =
			inner_nfa_match_to_start_trans = (NFA_TRANS_TP) malloc(
					sizeof(NFA_TRANS_T));

	inner_nfa_match_to_start_trans->state_from = nfa->nfa_match;
	inner_nfa_match_to_start_trans->state_to = nfa->nfa_start;
	inner_nfa_match_to_start_trans->trans_char = NULL_CHAR;
	inner_nfa_match_to_start_trans->trans_prior =
			result_start_to_inner_nfa_start_trans;
	inner_nfa_match_to_start_trans->trans_next = NULL;

	nfa->nfa_start->trans_in_head = result_start_to_inner_nfa_start_trans;

	//为 nfa->nfa_match 构建 trans_out_head
	inner_nfa_match_to_start_trans = (NFA_TRANS_TP) malloc(sizeof(NFA_TRANS_T));

	inner_nfa_match_to_start_trans->state_from = nfa->nfa_match;
	inner_nfa_match_to_start_trans->state_to = nfa->nfa_start;
	inner_nfa_match_to_start_trans->trans_char = NULL_CHAR;
	inner_nfa_match_to_start_trans->trans_prior = NULL;
	inner_nfa_match_to_start_trans->trans_next =
			inner_nfa_match_to_result_match_trans = (NFA_TRANS_TP) malloc(
					sizeof(NFA_TRANS_T));

	inner_nfa_match_to_result_match_trans->state_from = nfa->nfa_match;
	inner_nfa_match_to_result_match_trans->state_to = result_match_state;
	inner_nfa_match_to_result_match_trans->trans_char = NULL_CHAR;
	inner_nfa_match_to_result_match_trans->trans_prior =
			inner_nfa_match_to_start_trans;
	inner_nfa_match_to_result_match_trans->trans_next = NULL;

	nfa->nfa_match->trans_out_head = inner_nfa_match_to_start_trans;

	//为 result_match_state 构建 trans_in_head
	inner_nfa_match_to_result_match_trans->state_from = nfa->nfa_match;
	inner_nfa_match_to_result_match_trans->state_to = result_match_state;
	inner_nfa_match_to_result_match_trans->trans_char = NULL_CHAR;
	inner_nfa_match_to_result_match_trans->trans_prior = NULL;
	inner_nfa_match_to_result_match_trans->trans_next = NULL;

	result_match_state->trans_in_head = inner_nfa_match_to_start_trans;

	//为 nfa_reform_result 构建 nfa_states_head
	result_start_state->state_prior = NULL;
	result_start_state->state_next = nfa->nfa_start;
	nfa->nfa_start->state_prior = result_start_state;
	nfa->nfa_match->state_next = result_match_state;
	result_match_state->state_prior = nfa->nfa_match;
	result_match_state->state_next = NULL;

	//一切准备就绪，开始构建 nfa_reform_result
	nfa_reform_result->inner_nfas = nfa;
	nfa_reform_result->nfa_match = result_match_state;
	nfa_reform_result->nfa_start = result_start_state;
	nfa_reform_result->nfa_next = NULL;
	nfa_reform_result->nfa_prior = NULL;
	nfa_reform_result->nfa_states_head = result_start_state;
	nfa_reform_result->nfa_type = NFA_TYPE_ONE_OR_MORE;
	nfa_reform_result->state_count = nfa->state_count + 2;

	//处理各个 state 的 state_code
	state_cursor = nfa_reform_result->nfa_states_head;
	state_code_counter = 0;
	for (; state_cursor != NULL ; state_cursor = state_cursor->state_next) {
		state_cursor->state_code = state_code_counter;
		state_code_counter++;
	}
}
