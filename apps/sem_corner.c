/*
 * Phase 2 semaphore corner case
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>
#include <sem.h>

sem_t sem1;

void threadC(void *arg)
{
	(void)arg;
    sem_down(sem1);
    printf("C\n");

}

void threadB(void *arg)
{
	(void)arg;  
    sem_up(sem1);
    printf("B\n");
}

void threadA(void *arg)
{
	(void)arg;

    uthread_create(threadB, NULL); 
    uthread_create(threadC, NULL);
    sem_down(sem1);
    printf("A\n");
}

int main(void)
{
	sem1 = sem_create(0);

	uthread_run(false, threadA, NULL);

	sem_destroy(sem1);
	
	return 0;
}
