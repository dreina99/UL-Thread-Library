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

struct node {
	void* next;
	void* data;
};

struct queue {
	struct node* head;
	struct node* tail;
};

queue_t threadQ;

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
	int flag = 1;

	threadQ = queue_create();
	
	/* Store idle thread */
	struct uthread_tcb* idleThread = malloc(sizeof(struct uthread_tcb));
	idleThread->threadCtx = &ctx[0];
	idleThread->state = READY;
	
	/* Create and ctx_switch to initial thread */
	uthread_create(func, arg);
	struct uthread_tcb* initThread = threadQ->head->data;
	initThread->state = RUNNING;


	/* Begin infinite loop, break when no more threads ready to run */
	struct uthread_tcb* currThread = threadQ->head->data;
	
	while(1)
	{	
		if(flag)
		{
			flag = 0;
			uthread_ctx_switch(&ctx[0], initThread->threadCtx);
		}

		void* temp;
		queue_dequeue(threadQ, &temp);
		currThread = temp;

		if(queue_length(threadQ) == 0)
		{
			break;
		}

		if(currThread->state == EXITED)
		{
			uthread_ctx_destroy_stack(currThread->stackPointer);
			free(currThread);
			struct uthread_tcb* newHead = threadQ->head->data;
			uthread_ctx_switch(&ctx[0], newHead->threadCtx);
		}


		if(preempt)
		{
			uthread_yield();
			struct uthread_tcb* nextThread = threadQ->head->data;
			currThread = nextThread;
		}
	}
		
	return 0;
}

void uthread_yield(void)
{
	void* temp; 
	queue_dequeue(threadQ, &temp);
	struct uthread_tcb* yieldingThread = temp;
	struct uthread_tcb* newHead = threadQ->head->data;
	
	if(yieldingThread->state == RUNNING)
	{
		yieldingThread->state = READY;
		queue_enqueue(threadQ, yieldingThread);
	}

	newHead->state = RUNNING;
	uthread_ctx_switch(yieldingThread->threadCtx, newHead->threadCtx);
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	if(uthread_current() == NULL)
	{
		exit(0);
	}

	struct uthread_tcb* currThread = uthread_current();
	
	if(currThread->state == RUNNING || currThread->state == READY)
	{
		currThread->state = EXITED;
	}

	struct uthread_tcb* currProcess = threadQ->head->data;
	uthread_ctx_switch(currProcess->threadCtx, &ctx[0]);
	
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

// void uthread_block(void)
// {
// 	/* TODO Phase 4 */
// }

// void uthread_unblock(struct uthread_tcb *uthread)
// {
// 	/* TODO Phase 4 */
// }