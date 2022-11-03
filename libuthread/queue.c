#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "private.h"

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


/**
 * @brief Gets the length of a queue parameter
 *
 * @param queue The queue for which to find the length
 * @return Returns integer length of queue, -1 if queue is
 * NULL
 */
int queue_length(queue_t queue)
{
	/* Check if queue exists */
	if(queue == NULL)
	{
		return -1;
	}

	/* Iterate from head of queue */
	int length = 0;
	struct node *curNode = queue->head;

	while(curNode != NULL)
	{
		length++;
		curNode = curNode->next;
	}

	return length;
}

/**
 * @brief Creates a struct queue data type, initializes head
 * and tail to NULL
 *
 * @param none
 * @return Returns a struct queue address
 */
queue_t queue_create(void)
{
	queue_t q = malloc(sizeof(struct queue));

	if(q == NULL)
	{
		return NULL;
	}

	q->head = NULL;
	q->tail = NULL;
	return q;
}

/**
 * @brief Destroys an empty @queue
 *
 * @param queue The queue for which to destroy
 * @return Returns 0 if @queue was freed successfully,
 * returns -1 if @queue is NULL or not empty
 */
int queue_destroy(queue_t queue)
{
	/* If queue is NULL or not empty */
	if(queue == NULL || queue_length(queue) != 0)
	{
		return -1;
	}

	free(queue);
	return 0;
}

/**
 * @brief Enqueues the address of @data into @queue
 *
 * @param queue The queue for which to enqueue data
 * @param data Address of data to enqueue
 * @return Returns -1 if @queue or @data is NULL,
 * returns 0 if enqueue was successful
 */
int queue_enqueue(queue_t queue, void *data)
{
	/* Check for NULL queue or data*/
	if(queue == NULL || data == NULL)
	{
		return -1;
	}

	/* Create new node to enqueue */
	struct node *newNode = malloc(sizeof(struct node));
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

/**
 * @brief Dequeues the head of @queue and assigns its
 * data to @data
 *
 * @param queue The queue to be popped
 * @param data Address of data pointer to be assigned
 * @return Returns -1 if @queue, @data is NULL, or
 * length of queue is 0, returns 0 if dequeue was successful
 */
int queue_dequeue(queue_t queue, void **data)
{
	/* fail case */
	if((queue == NULL) || !queue_length(queue) || data == NULL)
	{
		return -1;
	}

	/* save head node */
	struct node *front = queue->head;

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

/**
 * @brief Deletes the item in @queue equal to @data,
 * updates @queue accordingly.
 *
 * @param queue The queue to delete from
 * @param data Data to find and delete
 * @return Returns -1 if @queue, @data is NULL, or
 * data is not found, returns 0 if delete was successful
 */
int queue_delete(queue_t queue, void *data)
{
	/* Check for NULL queue or data*/
	if(queue == NULL || data == NULL)
	{
		return -1;
	}

	struct node *curr = queue->head;
	struct node *currPrev = NULL;

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
			return 0;
		}

		/* update nodes */
		currPrev = curr;
		curr = curr->next;
	}

	return -1;
}

/**
 * @brief Calls given callback function @func on each item
 * within @queue
 *
 * @param queue The queue to iterate through
 * @param func Function to be called on each queue item
 * @return Returns -1 if @queue, @func is NULL,
 * returns 0 if iterate was successful
 */
int queue_iterate(queue_t queue, queue_func_t func)
{
	if(queue == NULL || func == NULL)
	{
		return -1;
	}

	struct node *curNode = queue->head;

	while(curNode != NULL)
	{
		func(queue, curNode->data);
		curNode = curNode->next;
	}

	return 0;
}
