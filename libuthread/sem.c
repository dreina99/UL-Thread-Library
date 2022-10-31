#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "private.h"
#include "queue.h"
#include "sem.h"
#include "uthread.h"


struct node 
{
	void* next;
	void* data;
};

struct queue 
{
	struct node* head;
	struct node* tail;
};

struct uthread_tcb 
{
	uthread_ctx_t* threadCtx;
	char* stackPointer;
	int state;
	int threadNum;
};

struct semaphore 
{
	int count;
	queue_t blockedQ;
};

extern queue_t threadQ;

sem_t sem_create(size_t count)
{
	sem_t sem = malloc(sizeof(struct semaphore));

	if(sem == NULL)
	{
		return NULL;
	}

	sem->count = count;
	sem->blockedQ = queue_create();
	
	return sem;
}

int sem_destroy(sem_t sem)
{
	if(queue_length(sem->blockedQ) || sem == NULL)
	{
		return -1;
	}

	queue_destroy(sem->blockedQ);
	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{
	if(sem == NULL)
	{
		return -1;
	}

	int initFlag = 1;

	while(sem->count == 0)
	{	
		/* On first iteration */
		if(initFlag)
		{
			initFlag = 0;

			/* Add to blocked queue */
			queue_enqueue(sem->blockedQ, uthread_current());
		}

		/* Block until semaphore becomes available */
		uthread_block();
	}

	/* Once it becomes available take it i.e. decrement semaphore's count */
	sem->count -= 1;

	return 0;
}

int sem_up(sem_t sem)
{	
	if(sem == NULL)
	{
		return -1;
	}

	/* Release the lock */
	sem->count += 1;

	/* 'Wake up' first thread in blockedQ */
	if(queue_length(sem->blockedQ))
	{
		void* pop;
		queue_dequeue(sem->blockedQ, &pop);

		struct uthread_tcb* wakingThread = pop;
		uthread_unblock(wakingThread);
	}

	return 0;
}

