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

struct node {
	void* next;
	void* data;
};

struct queue {
	struct node* head;
	struct node* tail;
};

queue_t threadQ;
int flag;

struct uthread_tcb {
	uthread_ctx_t* threadCtx;
	char* stackPointer;
	int state;
	int threadNum;
};

uthread_ctx_t ctx[1];

struct uthread_tcb *uthread_current(void)
{
	return threadQ->head->data;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	threadQ = queue_create();
	
	/* Store idle thread */
	struct uthread_tcb* idleThread = malloc(sizeof(struct uthread_tcb));
	idleThread->threadCtx = &ctx[0];
	idleThread->state = READY;
	
	/* Create and ctx_switch to initial thread */
	uthread_create(func, arg);
	struct uthread_tcb* initThread = threadQ->head->data;
	initThread->state = RUNNING;
	uthread_ctx_switch(&ctx[0], initThread->threadCtx);

	/* Begin infinite loop, break when no more threads ready to run */
	struct uthread_tcb* currThread = threadQ->head->data;

	while(1)
	{
		void* temp;
		queue_dequeue(threadQ, &temp);
		currThread = temp;

		/* If no more threads to schedule, break */
		if(queue_length(threadQ) == 0)
		{
			break;
		}

		/* If currThread finished, free allocated memory */
		if(currThread->state == EXITED)
		{
			uthread_ctx_destroy_stack(currThread->stackPointer);
			free(currThread);

			/* ctx_switch to new head, if new head is blocked yield */
			struct uthread_tcb* newHead = threadQ->head->data;
			if(newHead->state == BLOCKED)
			{
				flag = 1;
				uthread_yield();
				continue;
			}
			uthread_ctx_switch(&ctx[0], newHead->threadCtx);
		}

		/* If we are in preemptive mode */
		if(preempt)
		{
			uthread_yield();
			struct uthread_tcb* nextThread = threadQ->head->data;
			currThread = nextThread;
		}
	}

	/* Destroy the empty queue */
	queue_destroy(threadQ);
	
	return 0;
}

void printQ(queue_t q, void *data)
{
        if(q){}
        struct uthread_tcb* curNode = data;

        printf("state: %d\n", curNode->state);
}

void uthread_yield(void)
{
	void* temp; 
	queue_dequeue(threadQ, &temp);
	struct uthread_tcb* yieldingThread = temp;
	struct uthread_tcb* newHead = threadQ->head->data;
	
	/* If yieldingThread hasn't finished, change to ready and re-enqueue */
	if(yieldingThread->state == RUNNING || yieldingThread->state == READY)
	{
		yieldingThread->state = READY;
		queue_enqueue(threadQ, yieldingThread);
	}

	/* If yieldingThread is blocked, just re-enqueue */
	if(yieldingThread->state == BLOCKED)
	{
		queue_enqueue(threadQ, yieldingThread);
	}

	/* If new head is blocked, call yield() again */
	if(newHead->state == BLOCKED)
	{
		uthread_yield();
	}
	
	newHead->state = RUNNING;
	
	if(flag)
	{
		flag = 0;
		uthread_ctx_switch(&ctx[0], newHead->threadCtx);
		return;
	}

	uthread_ctx_switch(yieldingThread->threadCtx, newHead->threadCtx);
}

void uthread_exit(void)
{
	if(uthread_current() == NULL)
	{
		exit(0);
	}

	struct uthread_tcb* currThread = uthread_current();
	
	if(currThread->state == RUNNING || currThread->state == READY)
	{
		currThread->state = EXITED;
	}
	
	uthread_ctx_switch(currThread->threadCtx, &ctx[0]);
	
	exit(0);
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* create new tcb */
	struct uthread_tcb* newThread = malloc(sizeof(struct uthread_tcb));
	newThread->threadCtx = malloc(sizeof(uthread_ctx_t));
	newThread->stackPointer = uthread_ctx_alloc_stack();
	newThread->state = READY;
		
	if(uthread_ctx_init(newThread->threadCtx, newThread->stackPointer, func, arg) || newThread == NULL)
	{
		return -1;
	}
	
	queue_enqueue(threadQ, newThread);
	newThread->threadNum = queue_length(threadQ);

	return 0;
}

void uthread_block(void)
{
	struct uthread_tcb* head = uthread_current();
	head->state = BLOCKED;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = READY;
}