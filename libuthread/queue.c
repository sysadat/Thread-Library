#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Inspired by https://gist.github.com/mycodeschool/7429492?fbclid=IwAR24iiE892i0ILBeKzcquCkKj8n5u3_x6Kc_2wLRPX_NyDm7jrq_ZhfhM5A */

/* Inspired by https://www.geeksforgeeks.org/queue-linked-list-implementation/*/

typedef struct Node {
	void *data;
	// struct Node *prev;
	struct Node *next;
} Node;

// A global pointer to the head node
// Node *head;
// head = NULL;

struct queue {
	int length;
	Node *head;
	Node *tail;
};

//initalizes a new node
Node createNewNode(void *data)
{
	Node *newNode = (Node*)malloc(sizeof(Node));
	if (!newNode) {
		// malloc failed
		return -1;
	}
	newNode -> data = data;
	newNode -> next = NULL;
	return newNode;
}

//initializes a queue
queue_t queue_create(void)
{
	struct queue *createdQueue = (struct queue*)malloc(sizeof(struct queue));
	if (!createdQueue) {
		return NULL;
	} else {
		createdQueue -> length = 0;
		createdQueue -> head = NULL;
		createdQueue -> tail = NULL;
		return createdQueue;
	}
}

//destroys and frees the queue
int queue_destroy(queue_t queue)
{
	if (!queue || queue -> length > 0) {
		//queue is NULL or queue is not empty
		return -1;
	} else {
		free(queue);
		return 0;
	}
}

// adds an item to the queue
int queue_enqueue(queue_t queue, void *data)
{
	if (!queue || !data) {
		//no queue or data to be queued
		return -1;
	}
	Node *newNode = createNewNode(data);
	if (!newNode) {
		//error creating new node
		return -1;
	}
	if (!queue -> tail) {
		queue -> head = newNode;
		queue -> tail = newNode;
		queue -> length += 1;
		return 0;
	}
	queue -> tail -> next = newNode;
	queue -> tail = newNode
	queue -> length += 1;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (!queue || !data || !queue -> head) {
		// error nothing to dequeue
		return -1;
	}
	Node *tempNode = queue -> head;
	*data = tempNode -> data;
	queue -> head = queue -> head -> next;

	if (!queue -> head) {
		queue -> tail = NULL;
	}
	queue -> length -= 1;
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
	if (!queue) {
		return -1;
	} else {
		return queue -> length;
	}
}
