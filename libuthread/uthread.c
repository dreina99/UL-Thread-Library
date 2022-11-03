#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

#define RUNNING 0
#define READY 1
#define EXITED 2
#define BLOCKED 3

struct node 
{
	void *next;
	void *data;
};

struct queue 
{
	struct node *head;
	struct node *tail;
};

struct uthread_tcb 
{
	uthread_ctx_t *threadCtx;
	char *stackPointer;
	int state;
};

/* Keep the queue of threads and idleThread context global */
queue_t threadQ;
uthread_ctx_t ctx[1];

/**
 * @brief queue_funct_t Helper function, prints out state's of given queue starting from head end
 *
 * @param q Queue whose state's we want printed
 * @param data The current node we are at in the queue
 * @return none
 */
void printQ(queue_t q, void *data)
{
        if(q){}
        struct uthread_tcb *curNode = data;

        printf("state: %d\n", curNode->state);
}

/**
 * @brief Get current running thread
 *
 * @param none
 * @return struct uthread_tcb of the current running thread 
 */
struct uthread_tcb *uthread_current(void)
{
	return threadQ->head->data;
}

/**
 * @brief Runs the multithreading library
 *
 * @param preempt Boolean value to enable preemption
 * @param func Function of the first thread to start
 * @param arg Arguments to be passed to the first thread
 * @return int - 0 in case of success, -1 in case of failure
 */
int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	threadQ = queue_create(); /* Initialize queue */

	/* Create and ctx_switch to initial thread */
	uthread_create(func, arg);
	struct uthread_tcb *initThread = threadQ->head->data;
	initThread->state = RUNNING;

	/* If we are in preemptive mode */
	if(preempt)
	{
		preempt_start(true);
	}

	uthread_ctx_switch(&ctx[0], initThread->threadCtx);

	/* Begin infinite loop, break when no more threads ready to run */
	struct uthread_tcb *currThread;

	while(1)
	{
		void *popped;

		preempt_disable();
		if(queue_dequeue(threadQ, &popped) == -1) /* If dequeue fails */
		{	
			break;
		}
		preempt_enable();

		currThread = popped;
		
		/* If currThread finished, free allocated memory */
		if(currThread->state == EXITED)
		{
			uthread_ctx_destroy_stack(currThread->stackPointer);
			free(currThread->threadCtx);

			/* If no more threads to schedule, break */
			if(queue_length(threadQ) == 0)
			{
				break;
			}

			/* ctx_switch to new head, if new head is blocked yield */
			struct uthread_tcb *newHead = threadQ->head->data;
			newHead->state = RUNNING;

			uthread_ctx_switch(&ctx[0], newHead->threadCtx);
		}
	}

	/* Destroy the queues */
	queue_destroy(threadQ);

	/* Restore timer and sigaction configurations */
	if(preempt)
	{
		preempt_stop();
	}

	return 0;
}

/**
 * @brief Current running thread can yield for other threads to execute 
 *
 * @param none
 * @return none
 */
void uthread_yield(void)
{
	/* If there is only one thread in the queue, then we have no threads to yield to */
	if(queue_length(threadQ) == 1)
	{
		return;
	}

	void *popped;

	preempt_disable(); 
	queue_dequeue(threadQ, &popped);
	preempt_enable();

	struct uthread_tcb *yieldingThread = popped;
	struct uthread_tcb *newHead = threadQ->head->data;

	/* If yieldingThread hasn't finished, change to ready and re-enqueue */
	if(yieldingThread->state == RUNNING || yieldingThread->state == READY)
	{
		yieldingThread->state = READY;

		preempt_disable();
		queue_enqueue(threadQ, yieldingThread);
		preempt_enable();
	}

	newHead->state = RUNNING;

	uthread_ctx_switch(yieldingThread->threadCtx, newHead->threadCtx);
}

/**
 * @brief Exit from the current running thread
 *
 * @param none
 * @return none
 */
void uthread_exit(void)
{
	if(uthread_current() == NULL)
	{
		exit(0);
	}

	struct uthread_tcb *currThread = uthread_current();
	
	if(currThread->state == RUNNING || currThread->state == READY)
	{
		currThread->state = EXITED;
	}
	
	uthread_ctx_switch(currThread->threadCtx, &ctx[0]);
	
	exit(0);
}

/**
 * @brief Create a new thread
 *
 * @param func Function to be executed by created thread
 * @param arg Arguments to be passed to the created thread
 * @return none
 */
int uthread_create(uthread_func_t func, void *arg)
{
	/* create new tcb */
	struct uthread_tcb *newThread = malloc(sizeof(struct uthread_tcb));
	newThread->threadCtx = malloc(sizeof(uthread_ctx_t));
	newThread->stackPointer = uthread_ctx_alloc_stack();
	newThread->state = READY;

	if(uthread_ctx_init(newThread->threadCtx, newThread->stackPointer, func, arg) || newThread == NULL)
	{
		return -1;
	}

	preempt_disable();
	queue_enqueue(threadQ, newThread);
	preempt_enable();

	return 0;
}

/**
 * @brief Block current running thread
 *
 * @param none
 * @return none
 */
void uthread_block(void)
{
	uthread_current()->state = BLOCKED;
	uthread_yield();
}

/**
 * @brief Unblock current running thread
 *
 * @param uthread TCB of thread we want to unblock
 * @return none
 */
void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = READY;

	preempt_disable();
	queue_enqueue(threadQ, uthread);
	preempt_enable();
}