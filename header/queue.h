/*
 * queue.h
 *
 *  Created on: 2012-11-7
 *      Author: winiex
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#define QUEUE_SIZE 1000

struct queue_elem {
	int int_elem;
	void* pointer_elem;
};

typedef struct queue_elem QUEUE_ELEM_T;
typedef struct queue_elem* QUEUE_ELEM_TP;

struct queue {
	QUEUE_ELEM_T queue[QUEUE_SIZE];
	int queue_head, queue_tail;
	int length;
	int size;
};

typedef struct queue QUEUE_T;
typedef struct queue* QUEUE_TP;

void queue_init(QUEUE_TP* queue);

void queue_enqueue_int(QUEUE_TP queue, int int_elem);

void queue_enqueue_pointer(QUEUE_TP queue, void* pointer_elem);

void queue_enqueue_elem(QUEUE_TP queue, QUEUE_ELEM_TP elem);

int queue_dequeue_int(QUEUE_TP queue);

void* queue_dequeue_pointer(QUEUE_TP queue);

QUEUE_ELEM_TP queue_dequeue_elem(QUEUE_TP queue);

int queue_front_int(QUEUE_TP queue);

void* queue_front_pointer(QUEUE_TP queue);

QUEUE_ELEM_TP queue_front_elem(QUEUE_TP queue);

int queue_is_empty(QUEUE_TP queue);

int queue_is_full(QUEUE_TP queue);

int queue_length(QUEUE_TP queue);

void queue_destroy(QUEUE_TP* queue);

#endif /* QUEUE_H_ */
