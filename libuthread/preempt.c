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
 */
#define HZ 100
#define HZ_TO_MS 1000000

sigset_t signalSetter;
struct itimerval timerInterval;
struct sigaction signalAction;

// Handler function to yield current thread
static void sigvtalrmHandler(int signum)
{
	uthread_yield();
	if (0) {
		printf("signum is: %d\n", signum);
	}
}

// Diasable preempt by blocking SIGVTALRM signals
void preempt_disable(void)
{
	sigprocmask(SIG_BLOCK, &signalSetter, NULL);
}

// Enable preempt by unblock SIGVTALRM signals
void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, &signalSetter, NULL);
}

/* Signal handler, which is a timer interrupt handler, will force current
 * process to yield */
void preempt_start(void)
{
	/* Set up handler so if we receive a SIGVTALRM alarm,
	 * we will force the runningThread to yield */
	memset(&signalAction, 0, sizeof(signalAction));
	signalAction.sa_handler = &sigvtalrmHandler;
	sigaction(SIGVTALRM, &signalAction, NULL);

	/* Set up timer interal by dividing the frequency of preemption we want
	 * by the amount of Hz in micro seconds*/
	timerInterval.it_interval.tv_usec = HZ_TO_MS / HZ;
	timerInterval.it_interval.tv_sec = 0;
	timerInterval.it_value.tv_usec = HZ_TO_MS / HZ;;
	timerInterval.it_value.tv_sec = 0;

	/* Initialize timer, which will fire an alarm at 100hz */
	setitimer(ITIMER_VIRTUAL, &timerInterval, NULL);
}
