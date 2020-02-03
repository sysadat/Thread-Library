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

// Thread States
#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define ZOMBIE 3

typedef struct{
	uthread_t TID;
	uthread_t parentTID;
	uthread_ctx_t threadContext;
	void *stackPointer;
	unsigned short threadState;
	int retValue;
} TCB;

// Globals
uthread_t threadCount = 0;
queue_t readyQueue;
queue_t blockedQueue;
queue_t zombieQueue;
TCB *mainThread;
TCB *runningThread;

// checks if a thread's TID is equal to a given tid
static int checkTID(void *thread, void *tid)
{
	TCB *givenThread = (TCB*)thread;
	uthread_t givenTid = (uthread_t)(long)tid;
	if (!givenThread || !givenTid) {
		return -1;
	} else {
		return (givenThread ->TID == givenTid);
	}
}
int threadInitialization(uthread_func_t func, void *arg)
{	//this needs comments
	int contextInitCheck = 0;
	readyQueue = queue_create();
	blockedQueue = queue_create();
	zombieQueue = queue_create();
	if (readyQueue == NULL || blockedQueue == NULL|| zombieQueue == NULL) {
		return -1;
	}
	mainThread = (TCB*)malloc(sizeof(TCB));
	if (!mainThread) {
		return -1;
	}
	mainThread -> TID = threadCount++;
	mainThread -> parentTID = -1;
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
	// save currently running thread
	TCB *oldThread = runningThread;

	// Get new ready thread and set it to running
	queue_dequeue(readyQueue, (void **)&runningThread);
	runningThread -> threadState = RUNNING;

	// Store previous thread in the proper queue
	if (oldThread -> threadState == BLOCKED) {
		queue_enqueue(blockedQueue, oldThread);
	} else {
		oldThread -> threadState = READY;
		queue_enqueue(readyQueue, oldThread);
	}

	// finally context switch into the new thread
	uthread_ctx_switch(&oldThread -> threadContext, &runningThread -> threadContext);
}

uthread_t uthread_self(void)
{
	return runningThread -> TID;
}

int uthread_create(uthread_func_t func, void *arg)
{//also needs comments
	int initCheck, contextInitCheck = 0;
	// If called for the first time, initialize the thread
	if (!threadCount) {
		initCheck = threadInitialization(func, arg);
		if (initCheck == -1) {
			return -1;
		}
	}
	TCB *newThread = (TCB*)malloc(sizeof(TCB));
	if (!newThread) {
		return -1;
	}
	newThread -> TID = threadCount++;
	if (threadCount >= USHRT_MAX) {
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
	} else {
		queue_enqueue(readyQueue, newThread);
		return newThread -> TID;
	}
}

void uthread_exit(int retval)
{
	runningThread -> threadState = ZOMBIE;
	runningThread -> retValue = retval;
	TCB *parent = NULL;
	queue_iterate(blockedQueue, checkTID, (void*)&runningThread -> parentTID, (void**)&parent);
	// if it has a parent, the parent will return to the readyQueue
	if (parent) {
		parent -> threadState = READY;
		parent -> retValue = retval;
		queue_delete(blockedQueue, parent);
		queue_enqueue(readyQueue, parent);
	// V I DONT THINK THIS ELSE SHOULD BE HERE!!!
	} else { // If no parent thread, it is a zombie.
		runningThread -> threadState = ZOMBIE;
		queue_enqueue(zombieQueue, runningThread);
	}
	TCB *oldThread = runningThread;
	queue_dequeue(readyQueue, (void**)&runningThread);
	uthread_ctx_switch(&oldThread -> threadContext, &runningThread -> threadContext);
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
	if (0) {
	printf("retval is: %p\n", retval);
	}
	return 0;
	/* TODO Phase 3 */
}
