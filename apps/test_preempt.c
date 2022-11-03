/*
 * Preemption test:
 * Tests the preemption of various threads. Uses a delay() function to control order of execution.
 *
 * Sequence:
 * Thread1 enqueues thread2 and then enters a delay for 2 seconds which will preempt to thread2.
 * Thread2 enqueues thread3 and then enters a delay for 1 second which will preempt to thread3.    
 * Thread3 then prints and exits and context switches to thread1. 
 * Thread1 will then resume its delay() causing it to once again preempt to thread2.
 * This back and forth preemption continues until thread2 finishes its delay, prints,   
 * and then exits, context switching to thread1.  
 * Thread1 will then print and exit, finishing the program.
 *
 * Output:
 * thread3
 * thread2
 * thread1
 *
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
	delay(1000);
	printf("thread2\n");
}

void thread1(void *arg)
{
	(void)arg;
	uthread_create(thread2, NULL);
   	delay(2000);
	printf("thread1\n");
}

int main(void)
{
	uthread_run(true, thread1, NULL);
	return 0;
}