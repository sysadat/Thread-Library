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
	uthread_ctx_t *threadContext;
	void *stackPointer;
	unsigned short threadState;
	int retValue;
} TCB;

// Globals
uthread_t threadCount;
TCB *runningThread;
queue_t readyQueue;
queue_t blockedQueue;
queue_t zombieQueue;

// Checks if a thread's TID is equal to a given tid
static int checkTID(void *thread, void *tid)
{
	TCB *givenThread = (TCB*)thread;
	uthread_t givenTid = (uthread_t)(long)tid;
	if (!givenThread || !givenTid) {
		return -1;
	}
	return (givenThread ->TID == givenTid);
}

/* Function to be caled to initialize the library by registering the so-far
 * execution flow */
int threadInitialization(uthread_func_t func, void *arg)
{
	int contextInitCheck = 0;
	readyQueue = queue_create();
	blockedQueue = queue_create();
	zombieQueue = queue_create();
	// Verify queues were properly created
	if (readyQueue == NULL || blockedQueue == NULL|| zombieQueue == NULL) {
		return -1;
	}
	// Create the main thread, and initialize everything inside it
	TCB *mainThread = (TCB*)malloc(sizeof(TCB));
	if (!mainThread) {
		return -1;
	}
	mainThread -> threadContext = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	if (!mainThread -> threadContext) {
		return -1;
	}
	mainThread -> TID = threadCount++;
	mainThread -> parentTID = -1;
	mainThread -> stackPointer = uthread_ctx_alloc_stack();
	if (!mainThread -> stackPointer) {
		return -1;
	}
	// Set the currently runningThread as main and make it RUNNING
	mainThread -> threadState = RUNNING;
	runningThread = mainThread;
	contextInitCheck = uthread_ctx_init(runningThread -> threadContext, runningThread -> stackPointer, func, arg);
	if (contextInitCheck == -1) {
		return -1;
	}
	// Set up preemption after initialization
	preempt_start();
	return 0;
}
void uthread_yield(void)
{
	/* We are adding a new thread and are going to access the TCB and we
	*  dont want to be preempted */
	preempt_disable();
	// Store currently running thread
	TCB *oldThread = runningThread;

	// Get new ready thread and set it to running
	queue_dequeue(readyQueue, (void **)&runningThread);
	runningThread -> threadState = RUNNING;
	// Store previous thread in the proper queue
	if (oldThread -> threadState == BLOCKED) {
		queue_enqueue(blockedQueue, oldThread);
	} else if (oldThread -> threadState == RUNNING) {
		oldThread -> threadState = READY;
		queue_enqueue(readyQueue, oldThread);
	}
	// Finally context switch into the new thread
	uthread_ctx_switch(oldThread -> threadContext, runningThread -> threadContext);

	/* Preemption can be reenabled because the sensitive steps are done
	 * and these steps can be interrupted and resumed without affecting
	 * the shared data structures */
	preempt_enable();
}
uthread_t uthread_self(void)
{
	return runningThread -> TID;
}

int uthread_create(uthread_func_t func, void *arg)
{
	int initCheck, contextInitCheck = 0;
	// If called for the first time, initialize the thread
	if (!threadCount) {
		initCheck = threadInitialization(func, arg);
		if (initCheck == -1) {
			return -1;
		}
	}
	/* We are adding a new thread and are going to access the TCB and we
	*  dont want to be preempted */
	preempt_disable();
	// Create and initalize a new thread and set it to ready
	TCB *newThread = (TCB*)malloc(sizeof(TCB));
	if (!newThread) {
		return -1;
	}
	newThread -> threadContext = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	if (!newThread -> threadContext) {
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
	contextInitCheck = uthread_ctx_init(newThread -> threadContext, newThread -> stackPointer, func, arg);
	if (contextInitCheck == -1) {
		return -1;
	}
	/* Preemption can be reenabled because the sensitive steps are done
	 * and these steps can be interrupted and resumed without affecting
	 * the shared data structures */
	preempt_enable();
	queue_enqueue(readyQueue, newThread);
	return newThread -> TID;
}

void uthread_exit(int retval)
{
	/* We are manipulating a shared data structure, so we want to
	* disable preemption so that the process can be performed atomically */
	preempt_disable();
	runningThread -> threadState = ZOMBIE;
	runningThread -> retValue = retval;
	TCB *parent = NULL;
	preempt_enable();
	/* Preemption can be reenabled because the sensitive steps are done
	 * and these steps can be interrupted and resumed without affecting
	 * the shared data structures */

	/* Look through the blockedQueue to find the if the runningThread is
	 *  there and return result in parent */
	queue_iterate(blockedQueue, checkTID, (void*)&runningThread -> parentTID, (void**)&parent);

	/* We are manipulating a shared data structure, so we want to
	* disable preemption so that the process can be performed atomically */
	preempt_disable();

	// If it has a parent, the parent will return to the readyQueue
	if (parent) {
		parent -> threadState = READY;
		parent -> retValue = retval;
		queue_delete(blockedQueue, parent);
		queue_enqueue(readyQueue, parent);
	}
	// If no parent thread, it is a zombie.
	runningThread -> threadState = ZOMBIE;
	preempt_enable();
	queue_enqueue(zombieQueue, runningThread);

	/* Preemption can be reenabled because the sensitive steps are done
	 * and these steps can be interrupted and resumed without affecting
	 * the shared data structures */
	uthread_yield();
}
/*
 * uthread_join - Join a thread
 * @tid: TID of the thread to join
 * @retval: Address of an integer that will receive the return value
 *
 * This function makes the calling thread wait for the thread @tid to complete
 * and assign the return value of the finished thread to @retval (if @retval is
 * not NULL).
 *
 * A thread can be joined by only one other thread.
 *
 * Return: -1 if @tid is 0 (the 'main' thread cannot be joined), if @tid is the
 * TID of the calling thread, if thread @tid cannot be found, or if thread @tid
 * is already being joined. 0 otherwise.
 */
// Wait for threading system to terminate executing threads
int uthread_join(uthread_t tid, int *retval)
{
	/* Check to make sure you can't join main, the TID is not the same as
	* the calling thread*/
	if (!tid || tid == uthread_self()) {
		return -1;
	}
	printf("%p\n", retval);
	return 0;
	/* TODO Phase 3 */
}
