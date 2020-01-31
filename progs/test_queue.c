#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "../libuthread/queue.c"
/* The first unit test simply checks that creating a queue succeeds, while the second checks a simple enqueue/dequeue scenario: */
void test_create(void)
{
	queue_t q;

	q = queue_create();
	assert(q != NULL);

}

void test_destroy(void)
{
	queue_t q;
	int checkDestroy;

	//Check to see if it can destroy a normal queue
	q = queue_create();
	checkDestroy = queue_destroy(q);
	assert(checkDestroy == 0);

	//Check to see that if failed to delete a non-empty queue
	int data = 3;
	queue_enqueue(q, &data);
	checkDestroy = queue_destroy(q);
	assert(checkDestroy == -1);
}

void test_enqueue(void)
{
	queue_t q;
	int data = 3, enqueueCheck = 0;

	q = queue_create();

	// Check if returns 0 if data was enqueued
	enqueueCheck = queue_enqueue(q, &data);
	assert(enqueueCheck == 0);

	// Check if returns -1 if data is null
	enqueueCheck = queue_enqueue(q, NULL);
	assert(enqueueCheck == -1);

}

// PUT THE TESTS HERE FOR DEQUEUE AND DELETE 

/* Callback function that increments items by a certain value */
static int inc_item(void *data, void *arg)
{
	int *a = (int*)data;
	int inc = (int)(long)arg;

	*a += inc;

	return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(void *data, void *arg)
{
	int *a = (int*)data;
	int match = (int)(long)arg;

	if (*a == match)
	return 1;

	return 0;
}

void test_iterator(void)
{
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int i;
	int *ptr;

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
	queue_enqueue(q, &data[i]);

	/* Add value '1' to every item of the queue */
	queue_iterate(q, inc_item, (void*)1, NULL);
	assert(data[0] == 2);

	/* Find and get the item which is equal to value '5' */
	ptr = NULL;
	queue_iterate(q, find_item, (void*)5, (void**)&ptr);
	assert(ptr != NULL);
	assert(*ptr == 5);
	assert(ptr == &data[3]);
}

void test_length(void)
{
	queue_t q;
	int testLength = 0;

	// Check length of a created, but empty queue
	q = queue_create();
	testLength = queue_length(q);
	assert(testLength >= 0);

	// Check length of a queue after enqueue
	int data = 3, *ptr;

	queue_enqueue(q, &data);
	testLength = queue_length(q);
	assert(testLength == 1);

	// Check length of a queue after dequeue

	queue_dequeue(q, (void**)&ptr);
	testLength = queue_length(q);
	assert(testLength == 0);
}

void test_queue_simple(void)
{
	queue_t q;
	int data = 3, *ptr;

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	assert(ptr == &data);
}

int main (void) {
	test_create();
	test_destroy();
	test_enqueue();
	test_iterator();
	test_length();
	test_queue_simple();
	return 0;
}
