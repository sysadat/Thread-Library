#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>
#include <preempt.h>

int thread1(void* arg)
{
	while(1) {
		printf("At thread1\n");
	}
}

int main (void)
{
	int ret;
	uthread_t tid;

	tid = uthread_create(thread1, NULL);
	uthread_join(tid, &ret);
	return 0;
}
