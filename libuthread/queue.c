#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

struct node {
	void* next;
	void* data;
};

struct queue {
	struct node* head;
	struct node* tail;
};

int queue_length(queue_t queue)
{
	/* Iterate from head of queue */
	int length = 0;
	struct node* curNode = queue->head;
	
	while(curNode != NULL)
	{
		length++;
		curNode = curNode->next;
	}

	return length;
}

queue_t queue_create(void)
{
	struct queue* q = malloc(sizeof(struct queue));
	if(q == NULL) 
	{
		return NULL;
	}
	q->head = NULL;
	q->tail = NULL;
	return q;
}

int queue_destroy(queue_t queue)
{
	/* If queue is NULL */ 
	if((queue->head == NULL && queue->tail == NULL) || queue_length(queue) == 0)
	{
		return -1;
	}
	
	free(queue);
	return 0;
	
}

int queue_enqueue(queue_t queue, void *data)
{
	/* Check for NULL queue or data*/
	if(queue == NULL || data == NULL)
	{
		return -1;
	}

	/* Create new node to enqueue */
	struct node* newNode = malloc(sizeof(struct node));
	newNode->data = data;
	newNode->next = NULL;

	if(queue_length(queue) == 0) /* If queue is empty */
	{
		queue->tail = newNode;
		queue->head = newNode;
	}
	else /* Normal enqueue */
	{
		queue->tail->next = newNode;
		queue->tail = newNode;
	}
	
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* fail case */
	if((queue == NULL) || !queue_length(queue) || data == NULL)
	{
		return -1;
	}

	/* save head node */
	struct node* front = queue->head;
	
	/* if queue has one item, empty queue */
	if(queue_length(queue) == 1)
	{
		queue->head = NULL;
		queue->tail = NULL;
	}
	/* else assign head to next item */
	else
	{
		/* assign new head */
		queue->head = queue->head->next;
	}

	*data = front->data;
	free(front);
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	/* Check for NULL queue or data*/
	if(queue == NULL || data == NULL)
	{
		return -1;
	}

	
	
	struct node* curr = queue->head;
	struct node* currPrev = NULL;

	/* loop through list */
	while(curr)
	{
		if(*(int *)curr->data == *(int *)data) 
		{
			
			if(queue_length(queue) == 1)
			{
				queue->head = NULL;
				queue->tail = NULL;
			}
			/* if data found at head, assign new head*/
			else if(curr == queue->head)
			{
				queue->head = curr->next;
				printf("%d\n", *(int *)queue->head->data);
			}
			/* if data found at tail, set new tail */
			else if(curr == queue->tail)
			{
				currPrev->next = NULL;
				queue->tail = currPrev;
			}
			/* if data found in middle */
			else 
			{
				currPrev->next = curr->next;
			}
			//free(curr);
			return 0;
		}
		/* update nodes */
		currPrev = curr;
		curr = curr->next;
	}
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if(queue == NULL || func == NULL)
	{
		return -1;
	}

	struct node* curNode = queue->head;
	
	while(curNode != NULL)
	{
		func(queue, curNode->data);
		curNode = curNode->next;
	}

	return 0;
}