/*
 * checkTID test
 *
 * Tests that checkTID works properly
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread1(void* arg)
{
	return 0;
}

int main(void)
{
	uthread_t tid;

	tid = uthread_create(thread1, NULL);
	if (checkTID(thread1,tid)==FALSE){
		printf("ERROR: CheckTID RETURNING INCORRECT TRUTH VALUE");
	}
	uthread_join(tid, NULL);
	return 0;
}
