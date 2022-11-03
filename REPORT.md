# **User-level Thread Library Implementation**

## **Implementation**
Our implementation of this program is broken down into the creation of four APIs:

1. Queue API
2. Uthread API
3. Semaphore API
4. Preemption API

<br>

## **Queue API**
We decided to implement our queue using a linked list, each node in the list is a `struct node` containing members for:

* `node *next`
* `void *data`

The `next` member holds the address to the next node in the linked list, while the `data` member holds the address of any data type since its a `void*`. In our use-case, the data member will hold the address of a thread's TCB which we will define later.

As for the Queue API functions, we implemented them as specified by the documentation in `queue.h`.

### *Testing*
We created unit tests for every function defined in our Queue API. Our main goal was to reach 100% coverage in `queue.c`, but we also made sure to test for corner cases such as dequeueing or deleting the last node in the linked list.

We also created several integration tests to make sure that the functions were working simultaneously.

<br>

## **Uthread API**


### *Testing*


<br>


## **Semaphore API**


### *Testing*


<br>


## **Preemption API**
To implement preemption we followed these steps:

1. Create a sigaction that listens for `SIGVTALRM`
2. Define the signal handler function
3. Create a virtual timer that raises `SIGVTALRM` every 100Hz
4. Restore previous timer and action configurations

Of these steps, the only noteworthy parts are restoring the timer and action configurations and our signal handler function.

Our signal handler function `void preempt_handler(int signum)` executes once a `SIGVTALRM` is raised and simply calls `uthread_yield()`.

To restore the original timer configuration and previous action associated with `SIGVTALRM`, we stored their previous configrations in global variables `struct sigaction oldHandler` and `struct itimerval oldValue`.

 We did this by using the third parameter of both `sigaction()` and `setitimer()` when calling them initially to store the previous action and timer configurations.

But the previous configurations are not actually restored until we call `preempt_stop()` which executes:

* `sigaction(SIGVTALRM, &oldHandler, NULL)`
* `setitimer(timerType, &oldValue, NULL)`


### *Testing*
To test our Preemption API, we devised three test cases that should generate the correct output if our preemption implementation is sound.

To help us test preemption, we created a helper function `void delay(int milliseconds)`. We call this function in our test threads to simulate a thread 'hogging' the CPU for too long.

#### *test_preempt.c*
Tests the preemption of various threads using `delay()` to control the order of execution. The sequence of this tester is as follows:

1. `thread1` enqueues `thread2` and then enters a delay for 2 seconds.
2. Preempt to `thread2`.
3. `thread2` enqueues `thread3` and then enters a delay for 1 second.
4. Preempt to `thread3`
5. `thread3` then prints, exits, and context switches back to `thread1`.
6. `thread1` and `thread2` preempt to one another until thread2 finishes its 1 second delay.
7. `thread2` prints, exits, and context switches to `thread1`
8. `thread1` finshes its delay, prints, and exits.
9. Finishing the program with output in the order: `thread3`, `thread2`, `thread1`

<br>

#### *test_preempt_disable.c*
Tests `preempt_disable()` and `preempt_enable()` over the execution of various threads using `delay()` to control the order of execution. The sequence of this tester is as follows:

1. `thread1` enqueues `thread2`, calls `preempt_disable()`, and enters a delay of 5 seconds.
2. Since preempts were disabled, `thread1` will wait the full 5 second delay without getting preempted, print, and then exit.
3. `thread2` is then scheduled to run, it enqueues `thread3`, calls `preempt_enable()`, and then enters a delay for 1 second.
4. Since preempts were enabled, `thread2` will get preempted.
5. Preempt to `thread3`.
6. `thread3` prints and exits.
7. `thread2` is scheduled to run, prints and exits.
8. Finishing the program with output in the order: `thread1`, `thread3`, `thread2`

<br>

#### *test_preempt_stop.c*
Tests `preempt_stop()` over the execution various of threads using `delay()` to control the order of execution. The sequence of this tester is as follows:

1. `thread1` enqueues `thread2`, calls `preempt_stop()`, then enters a delay of 5 seconds.
2. Since we restored action and timer configurations, preemption will not call our custom signal handler.
3. `thread1` waits the full 5 second delay, prints, and then exits.
4. `thread2` is scheduled to run, it prints and then exits.
5. Finishing the program with output in the order: `thread1`, `thread2`

<br>
