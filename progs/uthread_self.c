/*
 * Test to see if we join with a thread with itself
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

uthread_t tid[2];

int thread1 (void *arg)
{
	int joinCheck = 0;
	int ret;
	joinCheck = (uthread_join(uthread_self(), &ret));
	if (joinCheck != -1) {
		printf("Error, uthread joined a thread with itself\n");
	}
	printf("Correctly refused to join self\n");
	return 1;
}
int main(void)
{
	int ret;
	tid[0] = uthread_create(thread1, &ret);
	uthread_yield();
	return 0;
}
