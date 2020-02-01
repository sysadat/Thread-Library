#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "../libuthread/queue.c"

#define TEST_ASSERT(assert)			\
do {						\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {				\
		printf("PASS\n");		\
	} else	{				\
		printf("FAIL\n");		\
		exit(1);			\
	}					\
} while(0)

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

/* The first unit test simply checks that creating a queue succeeds */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	queue_t q;
	q = queue_create();
	TEST_ASSERT(q != NULL);

}

void test_destroy(void)
{
	queue_t q;
	int data = 3;

	//Check to see if it can destroy an empty queue
	q = queue_create();
	TEST_ASSERT(queue_destroy(q) == 0);

	//Check to see that it will return -1 if given a non-empty queue
	q = queue_create();
	queue_enqueue(q, &data);
	TEST_ASSERT(queue_destroy(q) == -1);

	//Check to see that it will return -1 if given a NULL queue
	TEST_ASSERT(queue_destroy(NULL) == -1);
}

void test_enqueue(void)
{
	queue_t q;
	int data1 = 1;
	int data2 = 2;
	int data3 = 3;
	q = queue_create();

	// Check that it returns 0 if data was enqueued properly
	TEST_ASSERT(queue_enqueue(q, &data1) == 0);
	// check if the value in the queue is the value we enqueued
	TEST_ASSERT(q -> tail -> data == &data1);
	// Check that it returns 0 if data was enqueued properly
	TEST_ASSERT(queue_enqueue(q, &data2) == 0);
	// check if the value in the queue is the value we enqueued
	TEST_ASSERT(q -> tail -> data== &data2);
	// Check that it returns 0 if data was enqueued properly
	TEST_ASSERT(queue_enqueue(q, &data3) == 0);
	// check that the values in the queue are correct
	TEST_ASSERT(q -> tail -> data == &data3);
	// check that the values in the queue are correct
	TEST_ASSERT(q -> head -> data == &data1);
	// check that the length of the queue is correct
	TEST_ASSERT(q -> length == 3);

	// Check if returns -1 if data is null
	TEST_ASSERT(queue_enqueue(q, NULL) == -1);
	// Check if returns -1 if queue is null
	TEST_ASSERT(queue_enqueue(NULL, &data1) == -1);
}

void test_dequeue(void)
{
	queue_t q;
	int data1 = 1;
	int data2 = 2;
	int data3 = 3;
	int *ptr;
	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);

	// Check that it returns 0 if data was dequeued properly
	TEST_ASSERT(queue_dequeue(q, (void**)&ptr)==0);
	// Check that it dequeued the correct (oldest) data
	TEST_ASSERT(ptr == &data1);
	// Check that it dequeued the correct (oldest) data
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data2);
	//check if length is being updated correctly
	TEST_ASSERT(q -> length == 1);
	// Check that it dequeued the correct (oldest) data
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data3);
	// Check that it returns -1 if there is nothing left to dequeue
	TEST_ASSERT(queue_dequeue(q, (void**)&ptr)==-1);

	// Check if returns -1 if data is null
	TEST_ASSERT(queue_dequeue(q, NULL) == -1);
	// Check if returns -1 if queue is null
	TEST_ASSERT(queue_dequeue(NULL, (void**)&ptr) == -1);
}
void test_delete(void)
{
	queue_t q;
	int data1 = 1;
	int data2 = 2;
	int data3 = 3;
	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);

	// Check that it returns 0 if data was deleted properly
	TEST_ASSERT(queue_delete(q, &data2) == 0);
	// check that the length of the queue is correct
	TEST_ASSERT(q -> length == 2);
	// check if delete worked properly
	TEST_ASSERT(q -> head -> data == &data1);
	// check if delete worked properly
	TEST_ASSERT(q -> tail -> data == &data3);
	// Check that it returns -1 if data was not found (it was deleted)
	TEST_ASSERT(queue_delete(q, &data2) == -1);
	// check that the length was not changed
	TEST_ASSERT(q -> length == 2);

	// Check if queue_delete returns -1 if the data or queue is NULL
	TEST_ASSERT(queue_delete(q, NULL) == -1);
	TEST_ASSERT(queue_delete(NULL, &data1) == -1);
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
	TEST_ASSERT(data[0] == 2);

	/* Find and get the item which is equal to value '5' */
	ptr = NULL;
	queue_iterate(q, find_item, (void*)5, (void**)&ptr);
	TEST_ASSERT(ptr != NULL);
	TEST_ASSERT(*ptr == 5);
	TEST_ASSERT(ptr == &data[3]);

	//check if -1 is returned if the queue or function is NULL
	TEST_ASSERT(queue_iterate(q, NULL, (void*)5, (void**)&ptr)==-1);
	TEST_ASSERT(queue_iterate(NULL, find_item, (void*)5, (void**)&ptr)==-1);

}

void test_length(void)
{
	queue_t q;
	int testLength;
	int data = 3, *ptr;

	// Check length of a created, but empty queue
	q = queue_create();
	testLength = queue_length(q);
	TEST_ASSERT(testLength >= 0);

	// Check length of a queue after enqueue
	queue_enqueue(q, &data);
	testLength = queue_length(q);
	TEST_ASSERT(testLength == 1);

	// Check length of a queue after enqueue
	queue_enqueue(q, &data);
	testLength = queue_length(q);
	TEST_ASSERT(testLength == 2);

	// Check length of a queue after dequeue
	queue_dequeue(q, (void**)&ptr);
	testLength = queue_length(q);
	TEST_ASSERT(testLength == 1);

	//check to make sure -1 is returned if the queue is NULL
	TEST_ASSERT(queue_length(NULL)== -1);
}

/*queue_simple checks the result of a simple enqueue/dequeue scenario*/
void test_queue_simple(void)
{
	queue_t q;
	int data = 3, *ptr;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}
void test_elaborate(void)
{
	queue_t q;
	int data1 = 1;
	int data2 = 2;
	int data3 = 3;
	int *ptr;

	q = queue_create();

	TEST_ASSERT(queue_enqueue(q, &data1) == 0);
	TEST_ASSERT(queue_enqueue(q, &data2) == 0);
	TEST_ASSERT(queue_enqueue(q, &data3) == 0);
	TEST_ASSERT(queue_iterate(q, inc_item,(void*)1, NULL) == 0);
	TEST_ASSERT(queue_delete(q, &data2) == 0);
	TEST_ASSERT(data1 == 2);
	TEST_ASSERT(data2 == 3);
	TEST_ASSERT(data3 == 4);
	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == 0);
	TEST_ASSERT(ptr == &data1);
	TEST_ASSERT(queue_length(q) == 1);
	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == 0);
	TEST_ASSERT(ptr == &data3);
	TEST_ASSERT(queue_destroy(q) == 0);
}

void test_print(void)
{
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (int i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
		queue_enqueue(q, &data[i]);
	}
	Node *current = q -> head;
	while(current != NULL) {
		printf("Item is: %d\n", *(int *)current -> data);
		current = current -> next;
	}
}

int main (void)
{
	test_create();
	test_destroy();
	test_enqueue();
	test_dequeue();
	test_delete();
	test_iterator();
	test_length();
	test_queue_simple();
	test_elaborate();
	test_print();
	return 0;
}
