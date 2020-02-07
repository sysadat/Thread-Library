/*
 * checkTID test
 *
 * Tests that checkTID works properly
 */

#include <stdio.h>
#include <stdlib.h>

#include "uthread.h"
#include "../libuthread/uthread.c"

int thread1(void* arg)
{
	return 0;
}

int main(void)
{
	uthread_t tid;

	tid = uthread_create(thread1, NULL);
	if (checkTID((void*)&thread1, (void**)&tid) == 0){
		printf("ERROR: CheckTID RETURNING INCORRECT TRUTH VALUE\n");
	}
	uthread_join(tid, NULL);
	return 0;
}
