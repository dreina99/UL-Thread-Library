#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/queue.h"

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

struct node {
	void* next;
	void* data;
};

struct queue {
	struct node* head;
	struct node* tail;
};

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}


/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

static void iterator_inc(queue_t q, void *data)
{
    int *a = (int*)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

void printQ(queue_t q, void *data)
{
	int *a = (int*) data;

	printf("%d\n", *a);
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

	
    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    //queue_iterate(q, printQ);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);
}

void test_delete_empty(void)
{
	queue_t q;
	int b = 5;
	int *a = &b;
	q = queue_create();
	TEST_ASSERT(queue_delete(q, a) == -1);
}

void test_destroy_empty(void)
{
    queue_t q;
    q = queue_create();
	TEST_ASSERT(queue_destroy(q) == -1);
}

int main(void)
{
	test_create();
	test_queue_simple();
    test_iterator();
    test_delete_empty();
    test_destroy_empty();
	return 0;
}
