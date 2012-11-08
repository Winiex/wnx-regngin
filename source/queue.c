/*
 * queue.c
 *
 *  Created on: 2012-11-7
 *      Author: winiex
 */

#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

void queue_init(QUEUE_TP* queue) {
	(*queue) = (QUEUE_TP) malloc(sizeof(QUEUE_T));
	(*queue)->size = QUEUE_SIZE;
	(*queue)->length = 0;
	(*queue)->queue_head = 0;
	(*queue)->queue_tail = 0;
}

void queue_enqueue_int(QUEUE_TP queue, int int_elem) {
	QUEUE_ELEM_TP int_elem_enqueue;

	int_elem_enqueue = (QUEUE_ELEM_TP) malloc(sizeof(QUEUE_ELEM_T));

	int_elem_enqueue->int_elem = int_elem;
	int_elem_enqueue->pointer_elem = NULL;

	queue_enqueue_elem(queue, int_elem_enqueue);

	free(int_elem_enqueue);
}

void queue_enqueue_pointer(QUEUE_TP queue, void* pointer_elem) {
	QUEUE_ELEM_TP pointer_elem_enqueue;

	pointer_elem_enqueue = (QUEUE_ELEM_TP) malloc(sizeof(QUEUE_ELEM_T));

	pointer_elem_enqueue->int_elem = -1;
	pointer_elem_enqueue->pointer_elem = pointer_elem;

	queue_enqueue_elem(queue, pointer_elem_enqueue);

	free(pointer_elem_enqueue);
	pointer_elem_enqueue = NULL;
}

void queue_enqueue_elem(QUEUE_TP queue, QUEUE_ELEM_TP elem) {
	QUEUE_ELEM_TP elem_enqueue;

	if ((queue->queue_tail + 1) % QUEUE_SIZE == queue->queue_head) {
		sprintf(stderr, "Queue Full Error!");
		return;
	}

	elem_enqueue = &(queue->queue[queue->queue_tail]);
	elem_enqueue->int_elem = elem->int_elem;
	elem_enqueue->pointer_elem = elem->pointer_elem;

	queue->queue_tail = (queue->queue_tail + 1) % QUEUE_SIZE;

	queue->length += 1;
}

int queue_dequeue_int(QUEUE_TP queue) {
	QUEUE_ELEM_TP elem_dequeue = queue_dequeue_elem(queue);
	return elem_dequeue->int_elem;
}

void* queue_dequeue_pointer(QUEUE_TP queue) {
	QUEUE_ELEM_TP elem_dequeue = queue_dequeue_elem(queue);
	return elem_dequeue->pointer_elem;
}

QUEUE_ELEM_TP queue_dequeue_elem(QUEUE_TP queue) {
	QUEUE_ELEM_TP elem_dequeue;

	if (queue->queue_head == queue->queue_tail) {
		sprintf(stderr, "Queue Empty Error!");
		return NULL ;
	}

	elem_dequeue = &(queue->queue[queue->queue_head]);
	queue->queue_head = (queue->queue_head + 1) % QUEUE_SIZE;
	queue->length -= 1;

	return elem_dequeue;
}

int queue_front_int(QUEUE_TP queue) {
	QUEUE_ELEM_TP elem_front = queue_front_elem(queue);
	return elem_front->int_elem;
}

void* queue_front_pointer(QUEUE_TP queue) {
	QUEUE_ELEM_TP elem_front = queue_front_elem(queue);
	return elem_front->pointer_elem;
}

QUEUE_ELEM_TP queue_front_elem(QUEUE_TP queue) {
	QUEUE_ELEM_TP elem_front;

	if (queue->queue_head == queue->queue_tail) {
		sprintf(stderr, "Queue Empty Error!");
		return NULL ;
	}

	elem_front = &(queue->queue[queue->queue_head]);

	return elem_front;
}

int queue_is_empty(QUEUE_TP queue) {
	if (queue->queue_head == queue->queue_tail) {
		return 1;
	}
	return 0;
}

int queue_is_full(QUEUE_TP queue) {
	if (queue->queue_tail + 1 == queue->queue_head) {
		return 1;
	}
	return 0;
}

int queue_length(QUEUE_TP queue) {
	return queue->length;
}

void queue_destroy(QUEUE_TP* queue) {
	free(*queue);
	(*queue) = NULL;
}
