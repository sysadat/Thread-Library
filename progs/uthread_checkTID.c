/*
 * checkTID test
 *
 * Tests that checkTID works properly
 */

// TODO
#include <stdio.h>
#include <stdlib.h>

// #include "uthread.h"
#include "../libuthread/uthread.c"

int thread1(void* arg)
{
	printf("Here at thread1\n");
	return 0;
}

int main(void)
{
	uthread_t tid;

	tid = uthread_create(thread1, NULL);
	if (checkTID(&thread1, (void**)&tid) == 0){
		printf("ERROR: CheckTID RETURNING INCORRECT TRUTH VALUE\n");
	}
	uthread_join(tid, NULL);
	return 0;
}
