/*
 * Preemption enable/disable test:
 * Tests the preempt_disable() and preempt_enable() of various threads.
 * Uses a delay() function to control order of execution.
 *
 * Sequence:
 * Thread1 enqueues thread2, disables preempts, and enters a delay of 5 seconds.
 * If preempt_disable() is working correctly, thread1 should not preempt and 
 * wait the full delay of 5 seconds, printing, and then exiting.
 * Thread2 is then scheduled to run and it enqueues thread3, re-enable preempts, and then
 * enters a delay of 1 second. 
 * If preempt_enable() is working correctly, thread2 should be preempted and then yield to thread3.
 * Thread3 prints and exits, context switching back to thread2. Thread2 then prints and exits,
 * finishing the program.
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

void thread3(void *arg)
{
	(void)arg;
	printf("thread3\n");
}

void thread2(void *arg)
{
	(void)arg;
	uthread_create(thread3, NULL);
	preempt_enable();
	delay(1000);
	printf("thread2\n");
}

void thread1(void *arg)
{
	(void)arg;
	uthread_create(thread2, NULL);
	preempt_disable();
   	delay(5000);
	printf("thread1\n");
}

int main(void)
{
	uthread_run(true, thread1, NULL);
	return 0;
}
