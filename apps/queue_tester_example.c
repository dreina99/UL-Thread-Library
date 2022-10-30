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


/* queue_create */
void test_create(void)
{
	TEST_ASSERT(queue_create() != NULL);
}

/* queue_iterate, queue_length, queue_delete, queue_enqueue */
void test_iterator(void)
{
	void iterator_inc(queue_t q, void *data)
	{
		int *a = (int*)data;

		if (*a == 42)
			queue_delete(q, data);
		else
			*a += 1;
	}

    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);
}

/* queue_dequeue */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}


void printQ(queue_t q, void *data)
{
	if(q){}
	int *a = (int*) data;

	printf("%d\n", *a);
}

/* queue_delete on empty queue */
void test_delete_empty(void)
{
	queue_t q;
	int b = 5;
	int *a = &b;
	q = queue_create();
	TEST_ASSERT(queue_delete(q, a) == -1);
}

/* queue_destroy on NULL queue */
void test_destroy_null(void)
{
    queue_t q;
    q = NULL;
	TEST_ASSERT(queue_destroy(q) == -1);
}

/* queue_destroy on non-empty queue */
void test_destroy_nonempty(void)
{
    queue_t q;
    q = queue_create();
	int a = 42;
	queue_enqueue(q, &a);
	TEST_ASSERT(queue_destroy(q) == -1);
}

/* queue_delete on single node queue */
void test_delete_single(void)
{
	queue_t q;
	int a = 42, *ptr;
	ptr = &a;
	q = queue_create();
	queue_enqueue(q, &a);
	TEST_ASSERT(queue_delete(q, ptr) == 0);
}

/* queue_iterate on NULL func and NULL queue */
void test_iterate_null(void)
{
	queue_t q;
	q = NULL;
	TEST_ASSERT(queue_iterate(q, printQ) == -1);
	
	q = queue_create();
	queue_func_t func = NULL;
	TEST_ASSERT(queue_iterate(q, func) == -1);
}

void test_dequeue_null(void)
{
	queue_t q;
	void *ptr;
	q = queue_create();

	TEST_ASSERT(queue_dequeue(q, &ptr) == -1);
}

int main(void)
{
	test_create();
	test_queue_simple();
    test_iterator();
	test_delete_empty();
    test_destroy_null();
    test_destroy_nonempty();
	test_delete_single();
	test_iterate_null();
	test_dequeue_null();
	return 0;
}
