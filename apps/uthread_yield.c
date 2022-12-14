/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>

void thread3(void *arg)
{
	(void)arg;

	uthread_yield(); // 5  thread2 -> thread1 -> thread3
	printf("thread3\n"); // 9
}

void thread2(void *arg)
{
	(void)arg;

	uthread_create(thread3, NULL); // 3 threadQ: thread2 -> thread1 -> thread3
	uthread_yield(); // 4 threadQ: thread1 -> thread3 -> thread2
	printf("thread2\n"); // 8
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL); // 1 threadQ: thread1 -> thread2
	uthread_yield(); // 2 
	printf("thread1\n"); // 6
	uthread_yield(); // 7 threadQ: thread3 -> thread2 -> thread1
}

int main(void)
{
	uthread_run(false, thread1, NULL);
	return 0;
}