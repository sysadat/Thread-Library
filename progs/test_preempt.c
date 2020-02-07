/*
 * Preempt test
 *
 * Created a two infinite printing thread with no yeilds or exits
 * if we can break out from either thread, it means preemption works
*/

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>
#include <preempt.h>

int thread1(void* arg)
{
	while (1) {
		printf("Thread1\n");
	}
	return 0;
}

int main(void)
{
	uthread_create(thread1, NULL);
	while (1) {
		printf("Main thread\n");
	}
	return 0;
}
