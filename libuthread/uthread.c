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
};

struct uthread_tcb *uthread_current(void)
{
	return threadQ->head->data;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	threadQ = queue_create();
	if(preempt){}; /* dead code, remove after */
	uthread_create(func, arg);
	

	/* make idle thread */
	uthread_ctx_t ctx[1];
	struct uthread_tcb* idleThread = malloc(sizeof(struct uthread_tcb));
	idleThread->threadCtx = &ctx[0];
	idleThread->state = READY;

	struct uthread_tcb* newThread = threadQ->head->data;
	uthread_ctx_switch(&ctx[0], newThread->threadCtx);

	// /* Create initial thread */
	// uthread_create(func, arg);
	// struct uthread_tcb* temp = threadQ->head->data;
	// temp->state = RUNNING;
	
	// struct uthread_tcb* curr = threadQ->head->data;
	// void* toDelete;
	// while(1)
	// {
	// 	if(uthread_current() == NULL)
	// 	{
	// 		break;
	// 	}
	// 	else if(curr->state == EXITED)
	// 	{
	// 		queue_dequeue(threadQ, &toDelete);
	// 		struct uthread_tcb* temp = toDelete;
	// 		uthread_ctx_destroy_stack(temp->stackPointer);
	// 		free(toDelete);
	// 	}

	// 	uthread_yield();
	// 	struct uthread_tcb* temp2 = threadQ->head->data;
	// 	uthread_ctx_switch(curr->threadCtx, temp2->threadCtx);
	// 	curr = threadQ->head->data;
	// 	curr->state = RUNNING;
	// }

	/* switch back to idle thread */
	//uthread_ctx_switch(curr->threadCtx, idleThread->threadCtx);
	return 0;
}

void uthread_yield(void)
{
	void* yieldingThread; 
	queue_dequeue(threadQ, &yieldingThread);
	struct uthread_tcb* temp = yieldingThread;
	
	
	if(temp->state == RUNNING)
	{
		temp->state = READY;
		queue_enqueue(threadQ, temp);
	}

	
	else if (temp->state == EXITED)
	{
		uthread_ctx_destroy_stack(temp->stackPointer);
		free(temp);
	}
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	struct uthread_tcb* currThread = uthread_current();
	
	if(currThread->state == RUNNING || currThread->state == READY)
	{
		currThread->state = EXITED;
	}
	
	exit(0);
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* create new tcb */
	struct uthread_tcb* newThread = malloc(sizeof(struct uthread_tcb));
	newThread->threadCtx = malloc(sizeof(uthread_ctx_t));
	newThread->stackPointer = uthread_ctx_alloc_stack();
	

	if(newThread == NULL)
	{
		return -1;
	}

	/* init context*/
	if(uthread_ctx_init(newThread->threadCtx, newThread->stackPointer, func, arg) == -1)
	{
		return -1;
	}

	queue_enqueue(threadQ, newThread);
	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 4 */
}

// void uthread_unblock(struct uthread_tcb *uthread)
// {
// 	/* TODO Phase 4 */
// }