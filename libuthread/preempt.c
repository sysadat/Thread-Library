#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 * Macros allow quick changes to the frequency without modifying the rest of the code
 */
#define HZ 100
#define HZ_TO_MS 1000000
#define INTERVAL HZ_TO_MS / HZ

// Global variable
sigset_t signalSetter;

// Handler function to yield current thread
static void sigvtalrmHandler(int signum)
{
	uthread_yield();
	// we may get an error if signum is unused, so we check for one:
	if (0) {
		printf("signum is: %d\n", signum);
	}
}

// Diasable preempt by blocking SIGVTALRM signals
void preempt_disable(void)
{
	sigprocmask(SIG_BLOCK, &signalSetter, NULL);
}

// Enable preempt by unblocking SIGVTALRM signals
void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, &signalSetter, NULL);
}

// Create a signal handler, which is a timer interrupt handler, that will force the current process to yield
void preempt_start(void)
{
	struct itimerval timerInterval;
	struct sigaction signalAction;

	 /* Initialize the signal to empty and then add the signal to the set. Idea from Professor's Wednesday Slides */
	sigemptyset(&signalSetter);
	sigaddset(&signalSetter, SIGVTALRM);

	/* Set up handler so if we receive a SIGVTALRM alarm, we will force the runningThread to yield */
	signalAction.sa_handler = &sigvtalrmHandler;
	sigaction(SIGVTALRM, &signalAction, NULL);

	/* Set up timer interal by dividing the frequency of preemption we want in Hz by the amount of micro seconds in a Hz*/
	timerInterval.it_interval.tv_usec = INTERVAL;
	timerInterval.it_interval.tv_sec = 0;
	timerInterval.it_value.tv_usec = INTERVAL;
	timerInterval.it_value.tv_sec = 0;

	/* Initialize timer to fire an alarm every 100hz */
	setitimer(ITIMER_VIRTUAL, &timerInterval, NULL);
}
