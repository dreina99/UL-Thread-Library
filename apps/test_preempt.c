/*
 * Preemption test
 *
 * Tests the preemption of various threads. Uses a delay() function to control order of 
 * execution. Thread1 enqueues thread2 and then enters a delay which will preempt before
 * printing. Thread2 enqueues thread3, prints, and exits, yielding to thread3. Thread3  
 * also uses a delay() function to force a preempted context switch to thread1. These 
 * preemptions will continually force context switches until the delays finish, leading 
 * to the output.
 *
 * thread2
 * thread1
 * thread3
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <uthread.h>

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

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
	{
        now = clock();
	}

	return;
}

void thread3(void* arg)
{
	(void)arg;
	delay(1000);
	printf("thread3\n");
}

void thread2(void* arg)
{
	(void)arg;
	uthread_create(thread3, NULL);
	printf("thread2\n");
}

void thread1(void* arg)
{
	(void)arg;
	uthread_create(thread2, NULL);
   	delay(1000);
	printf("thread1\n");
}

int main(void)
{
	uthread_run(true, thread1, NULL);
	return 0;
}
