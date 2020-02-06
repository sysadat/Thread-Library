/*
 * Thread creation and join test
 *
 * Tests the creation of multiples threads and if join
 * recieves the correct return value.
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread2(void* arg)
{
	printf("thread%d\n", uthread_self());
	return 2;
}

int thread1(void* arg)
{
	int ret;
	uthread_t tid;

	printf("thread%d\n", uthread_self());
	tid = uthread_create(thread2, NULL);
	uthread_join(tid, &ret);
	if (ret != 2){
		printf("ERROR: WRONG RETURN VALUE\n");
	}
	return 1;
}

int main(void)
{
	int ret;
	uthread_t tid;

	tid = uthread_create(thread1, NULL)
	uthread_join(tid , &ret);
	if (ret != 1){
		printf("ERROR: WRONG RETURN VALUE\n");
	}
	return 0;
}
