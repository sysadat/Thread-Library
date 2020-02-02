#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

// The Thread States
#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define ZOMBIE 3

typedef struct{
	uthread_t TID;
	uthread_ctx_t threadContext;
	void *stackPointer;
	unsigned short threadState;
} TCB;

// Globals
int numThread = 0;
queue_t readyQueue;
queue_t blockedQueue;
queue_t zombieQueue;
TCB *mainThread;
TCB *runningThread;

int threadInitialization(void)
{
	int contextInitCheck = 0;
	readyQueue = queue_create();
	blockedQueue = queue_create();
	zombieQueue = queue_create();
	if (readyQueue == NULL || blockedQueue == NULL|| zombieQueue == NULL) {
		return -1;
	}
	mainThread = (TCB*)malloc(sizeof(TCB));
	mainThread -> TID = numThread++;
	mainThread -> stackPointer = uthread_ctx_alloc_stack();
	if (!mainThread -> stackPointer) {
		return -1;
	}
	mainThread -> threadState = RUNNING;
	runningThread = mainThread;
	contextInitCheck = uthread_ctx_init(&runningThread -> threadContext, runningThread -> stackPointer, func, arg);
	if (contextInitCheck == -1) {
		return -1;
	}
	return 0;
}
void uthread_yield(void)
{
	// Keep track of current thread
	TCB *prevThread = runningThread;

	// Get new ready thread and set it to running
	queue_dequeue(readyQueue, (void **)&runningThread);
	runningThread -> threadState = RUNNING;

	// Store previous thread in a proper queue
	if (prevThread -> threadState == BLOCKED) {
		queue_enqueue(blockedQueue, prevThread);
	} else {
		prevThread -> threadState = READY;
		queue_enqueue(readyQueue, prevThread);
	}

	// The current thread is now running and then initiate the yield
	uthread_ctx_switch(&prevThread -> threadContext, &runningThread -> threadContext);
}

uthread_t uthread_self(void)
{
	return runningThread -> TID;
}

int uthread_create(uthread_func_t func, void *arg)
{
	int initCheck, contextInitCheck = 0;
	// If called for the first time, initialize the thread
	if (!numThread) {
		initCheck = threadInitialization();
		if (initCheck == -1) {
			return -1;
		}
	}
	TCB *newThread = (TCB*)malloc(sizeof(TCB));
	if (!newThread) {
		return -1;
	}
	newThread -> TID = numThread++;
	if (numThread >= USHRT_MAX) {
		return -1;
	}
	newThread -> threadState = READY;
	newThread -> stackPointer = uthread_ctx_alloc_stack();
	if (!newThread -> stackPointer) {
		return -1;
	}
	contextInitCheck = uthread_ctx_init(&newThread -> threadContext, newThread -> stackPointer, func, arg);
	if (contextInitCheck == -1) {
		return -1;
	}
	queue_enqueue(readyQueue, newThread);
	return newThread -> TID;
}

void uthread_exit(int retval)
{
	/* TODO Phase 2 */
}

int uthread_join(uthread_t tid, int *retval)
{
	if (!tid) {
		return -1;
	}
	while (1) {
		if (!queue_length(readyQueue)) {
			break;
		} else {
			uthread_yield();
		}
	}
	return 0;
	/* TODO Phase 3 */
}
