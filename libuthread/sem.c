#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct node {
	void* next;
	void* data;
};

struct queue {
	struct node* head;
	struct node* tail;
};

struct uthread_tcb {
	uthread_ctx_t* threadCtx;
	char* stackPointer;
	int state;
	int threadNum;
};

struct semaphore {
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

	/* If the lock is taken, block this thread */
	if(sem->count == 0)
	{
		queue_enqueue(sem->blockedQ, threadQ->head->data);
		uthread_block();
		return 0;
	}

	/* If it's unlocked, decrement semaphore's count */
	sem->count--;

	return 0;
}

int sem_up(sem_t sem)
{	
	if(sem == NULL)
	{
		return -1;
	}

	/* If releasing the lock, unblock first waiting thread */
	if(sem->count == 0)
	{
		/* Unblock first in blockedQ if it isn't empty */
		if(queue_length(sem->blockedQ))
		{
			void* temp;
			queue_dequeue(sem->blockedQ, &temp);
			struct uthread_tcb* head = temp;
			uthread_unblock(head);
		}
	}

	/* Release the lock */
	sem->count++;

	return 0;
}

