/*
 * Tests an excessive amount of threads
 *
 *
 * thread1
 * thread2
 * thread3
 * thread4
 * thread5
 * thread6
 * thread7
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread5(void* arg)
{
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 0;
}

int thread4(void* arg)
{
	uthread_create(thread5, NULL);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 0;
}
int thread3(void* arg)
{
	uthread_create(thread4, NULL);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	uthread_yield();
	return 0;
}

int thread2(void* arg)
{
	uthread_create(thread3, NULL);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	uthread_yield();
	uthread_yield();
	return 0;
}

int thread1(void* arg)
{
	uthread_create(thread2, NULL);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	uthread_yield();
	uthread_yield();
	uthread_yield();
	return 0;
}

int main(void)
{
	uthread_join(uthread_create(thread1, NULL), NULL);
	return 0;
}
