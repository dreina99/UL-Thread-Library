/*
 * Preemption stop test:
 * Tests preempt_stop() on various threads.
 * Uses a delay() function to control order of execution.
 *
 * Sequence:
 * Thread1 enqueues thread2, restores previous configurations, then enters a delay of 5 seconds.
 * If preempt_stop() is working correctly, thread1 should not call our custom signal handler
 * which calls uthread_yield() but rather wait the full delay of 5 seconds, print, and then exit.
 * Thread2 is then scheduled to run and it prints then exits, finishing the program. 
 *
 * Output:
 * thread1
 * thread3
 * thread2
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <uthread.h>

#include "../libuthread/private.h"

/**
 * @brief Simulate a delay for preemption
 *
 * @cite https://c-for-dummies.com/blog/?p=69
 * @param milliseconds Amount of time to delay in milliseconds
 * @return none
 */
void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds * (CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
	{
        now = clock();
	}

	return;
}

void thread2(void *arg)
{
	(void)arg;
	printf("thread2\n");
}

void thread1(void *arg)
{
	(void)arg;
	uthread_create(thread2, NULL);
	preempt_stop();
   	delay(5000);
	printf("thread1\n");
}

int main(void)
{
	uthread_run(true, thread1, NULL);
	return 0;
}