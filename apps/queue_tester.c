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


/* HELPER FUNCTIONS START */
/**
 * @brief Helper queue_func_t function to test queue_iterate()
 * with arithimetic functions
 */
void iterator_inc(queue_t q, void *data)
{
	int *a = (int*)data;

	if (*a == 42)
	{
		queue_delete(q, data);
	}
	else
	{
		*a += 1;
	}
}

/**
 * @brief Helper queue_func_t function to test queue_iterate()
 */
void printQ(queue_t q, void *data)
{
	if(q){}
	int *a = (int*) data;

	printf("%d\n", *a);
}
/* HELPER FUNCTIONS END */

/**
 * @brief Test queue_create()
 */
void test_create(void)
{
	TEST_ASSERT(queue_create() != NULL);
}

/**
 * @brief Test queue_iterate(), queue_length(), queue_delete() and queue_enqueue()
 */
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
	TEST_ASSERT(data[0] == 2);
	TEST_ASSERT(queue_length(q) == 9);
}

/**
 * @brief Test queue_dequeue() and queue_enqueue()
 */
void test_queue_simple(void)
{
	int data = 3;
	int *ptr;
	queue_t q;

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/**
 * @brief Test queue_delete() on empty queue
 */
void test_delete_empty(void)
{
	queue_t q;
	int b = 5;
	int *a = &b;
	q = queue_create();
	TEST_ASSERT(queue_delete(q, a) == -1);
}

/**
 * @brief Test queue_destroy() on NULL queue
 */
void test_destroy_null(void)
{
	queue_t q;
	q = NULL;
	TEST_ASSERT(queue_destroy(q) == -1);
}

/**
 * @brief Test queue_destroy() on non-empty queue
 */
void test_destroy_nonempty(void)
{
	queue_t q;
	q = queue_create();
	int a = 42;
	queue_enqueue(q, &a);
	TEST_ASSERT(queue_destroy(q) == -1);
}

/**
 * @brief Test queue_delete() on single node queue
 */
void test_delete_single(void)
{
	queue_t q;
	int a = 42;
	int *ptr;
	ptr = &a;
	q = queue_create();
	queue_enqueue(q, &a);
	TEST_ASSERT(queue_delete(q, ptr) == 0);
}

/**
 * @brief Test queue_iterate() on NULL params
 */
void test_iterate_null(void)
{
	queue_t q;
	q = NULL;
	TEST_ASSERT(queue_iterate(q, printQ) == -1);

	q = queue_create();
	queue_func_t func = NULL;
	TEST_ASSERT(queue_iterate(q, func) == -1);
}

/**
 * @brief Test all possible returns of queue_dequeue(queue_t queue, void **data)
 */
void test_dequeue(void)
{
	queue_t q1;
	queue_t q2;
	int data = 3;
	void *ptr;
	void **ptr2 = NULL;

	q1 = queue_create();
	q2 = NULL;

	/* empty queue */
	TEST_ASSERT(queue_dequeue(q1, &ptr) == -1);

	ptr = &data;
	/* null queue */
	TEST_ASSERT(queue_dequeue(q2, &ptr) == -1);

	queue_enqueue(q1, &data);
	/* successful dequeue */
	TEST_ASSERT(queue_dequeue(q1, &ptr) == 0);
	/* ptr set correctly */
	TEST_ASSERT(*(int*)ptr == 3);

	queue_enqueue(q1, &data);
	/* null data pointer */
	TEST_ASSERT(queue_dequeue(q1, ptr2) == -1);
}

/**
 * @brief Test all possibile returns for queue_length()
 */
void test_queue_length(void)
{
	queue_t q1;
	queue_t q2;
	queue_t q3;
	int data = 3;

	q1 = queue_create();
	q2 = queue_create();
	q3 = NULL;

	queue_enqueue(q1, &data);

	TEST_ASSERT(queue_length(q1) == 1);
	TEST_ASSERT(queue_length(q2) == 0);
	TEST_ASSERT(queue_length(q3) == -1);
}

/**
 * @brief Test all possible returns for queue_enqueue(queue_t queue, void *data)
 */
void test_enqueue(void)
{
	queue_t q1;
	queue_t q2;
	int d1 = 3;
	int *d2 = NULL;

	q1 = queue_create();
	q2 = NULL;

	/* successful enqueue */
	TEST_ASSERT(queue_enqueue(q1, &d1) == 0);

	/* null data */
	TEST_ASSERT(queue_enqueue(q1, d2) == -1);

	/* null queue, non-null data */
	TEST_ASSERT(queue_enqueue(q2, &d1) == -1);

	/* null queue, null data */
	TEST_ASSERT(queue_enqueue(q2, d2) == -1);
}

/**
 * @brief Test all possible returns for queue_delete(queue_t queue, void *data)
 */
void test_delete(void)
{
	queue_t q1;
	queue_t q2;
	int data = 3;
	int data2 = 4;
	void *ptr = NULL;

	q1 = queue_create();
	q2 = NULL;

	queue_enqueue(q1, &data);
	queue_enqueue(q1, &data2);

	/* null queue */
	TEST_ASSERT(queue_delete(q2, &data) == -1);

	/* null data */
	TEST_ASSERT(queue_delete(q1, ptr) == -1);

	/* successful delete */
	TEST_ASSERT(queue_delete(q1, &data) == 0);

	/* updates queue */
	TEST_ASSERT(queue_length(q1) == 1);

	/* data not found */
	TEST_ASSERT(queue_delete(q1, &data) == -1);

	/* successful delete */
	TEST_ASSERT(queue_delete(q1, &data2) == 0);

	/* empties queue */
	TEST_ASSERT(queue_length(q1) == 0);
}

int main(void)
{
	test_create();
	test_queue_simple();
	test_iterator();
	test_delete_empty();
	test_destroy_nonempty();
	test_delete_single();
	test_iterate_null();
	test_dequeue();
	test_queue_length();
	test_enqueue();
	test_delete();

	return 0;
}
