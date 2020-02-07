/*
 * Preempt test
 *
 * Created a thread with an infinite loop and joins, and checks to see if
 * if we can break out from that thread
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
