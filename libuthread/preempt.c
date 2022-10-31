#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

/* Handlers for virtual alarm */
struct sigaction newHandler;
struct sigaction oldHandler;
sigset_t ss;

/* Timers to raise periodic SIGVTALRM */
struct itimerval newValue;
struct itimerval oldValue;
int timerType;


/* Signal handler for SIGVTALRM */
void preempt_handler(int signum)
{
	signum = signum;
	// printf("Pre-empting...\n");
	// queue_iterate(threadQ, printQ3);
	uthread_yield(); /* Force currently running thread to yield */
}

void preempt_disable(void)
{
	/* Add SIGVTALRM to blocked signals */	
	sigaddset(&ss, SIGVTALRM);
	sigprocmask(SIG_BLOCK, &ss, NULL);
}

void preempt_enable(void)
{
	/* Remove SIGVTALRM from blocked signals */
	sigemptyset(&ss);
	sigaddset(&ss, SIGVTALRM);
	sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void preempt_start(bool preempt)
{
	/* If we don't want preemption, do nothing */
	if(!preempt)
	{
		return;
	}

	newHandler.sa_handler = preempt_handler; /* Add signal handler */
	sigemptyset(&newHandler.sa_mask); /* Initialize blocked signals */
	newHandler.sa_flags = 0; /* No special behavior for signal */
	sigaction(SIGVTALRM, &newHandler, &oldHandler); /* Initialize sigaction to listen for SIGVTALRM */

	/* Set interval timer type to decrement by process execution time */
	/* Raises SIGVTALRM after every interval */
	timerType = ITIMER_VIRTUAL;

	/* Set time to expire after 100Hz... */
	newValue.it_value.tv_sec = 0; 
	newValue.it_value.tv_usec = 100 * HZ;

	/* ...and every 100HZ after that */
	newValue.it_interval.tv_sec = 0;       
    newValue.it_interval.tv_usec = 100 * HZ;

	/* Start internal timer */
	setitimer(timerType, &newValue, &oldValue);
}

void preempt_stop(void)
{
	/* Restore previous sigaction and timer configurations */
	sigaction(SIGVTALRM, &oldHandler, NULL);
	setitimer(timerType, &oldValue, NULL);
}

