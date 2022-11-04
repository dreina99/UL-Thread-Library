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

The `next` member holds the address to the next node in the linked list, while
the `data` member holds the address of any data type since its a `void*`. In our
use-case, the data member will hold the address of a thread's TCB which we will
define later.

As for the Queue API functions, we implemented them as specified by the
documentation in `queue.h`.

### *Testing*

We created unit tests for every function defined in our Queue API.
Our main goal was to reach 100% coverage in `queue.c`, but we also made sure to
test for corner cases such as dequeueing or deleting the last node in the linked
list.

We also created several integration tests to make sure that the functions were
working simultaneously.

<br>

## **Uthread API**

The Uthread API is implemented using a queue of `struct uthread_tcb` that are
continuously updated as threads are created, yielded, and exited. Our `struct
uthread_tcb` hold three important pieces of information:

* `uthread_ctx_t *threadCtx`
* `char *stackPointer`
* `int state`

`threadCtx` is used to manage the process context while the stack pointer keeps
track of the thread's allocated stack. The state variable is assigned a macro,
either `RUNNING`, `READY`, `BLOCKED` or `EXITED`. This internal state variable
allows the program to perform certain actions when a thread is in each state.

The lifecycle of a multithreaded process always begins in `uthread_run()` where
we save the idle thread, then create and switch to the new context. Once the
first thread is created, the process will only return to `uthread_run()` once a
thread has exited. `uthread_run()` manages exited threads within a while loop.
This loop either deallocates the memory for exited threads or breaks the loop if
there are no threads to be scheduled. In the while loop, we always pop the first
thread's tcb in the thread queue. We can do this because we only return to
uthread_run once a thread has exited, so we are guaranteed that the first thread
in the queue will have the state `EXITED`, in which case it should be removed
from the queue.

It is important to note that `uthread_run()` only handles threads that are
exited and all thread to thread context switches are handled completely in
`uthread_yield()`. This design choice reflects our emphasis on simplicity as
`uthread_yield()` only needs to dequeue the thread queue, change states, and
switch between thread contexts.

`uthread_create()` and `uthread_exit()` are fairly straightforward functions.
The first allocates memory for a thread and saves it into our thread queue,
while the latter only sets a thread state and returns to `uthread_run()` to
handle memory deallocation.

`uthread_block()` and `uthread_unblock()` are used in conjunction with our
Semaphore API. `uthread_block()` is only called when a thread calls `sem_down()`
on a semaphore with no remaining resources. `uthread_unblock()` is called when a
semaphore gains resources and a thread needs to be unblocked and added back into
the queue.

### *Testing*

All testing for this phase was completeed with the provided programs in /apps

<br>

## **Semaphore API**

Our semaphore struct contains two data members:

* `int count`
* `queue_t blockedQ`

`count` represents the number of resources available in the semaphore for
threads and `blockedQ` is a queue that holds all of the blocked threads that are
'asleep,' waiting for the semaphore's resources to become available. We decided
to keep `blockedQ` strictly in `sem.c`  as it was simpler and created less
clutter in `uthread.c`.

`int sem_down(sem_t sem)` handles removing a resource from the semaphore and
blocks threads that call semaphores with 0 resources available. Threads that
call `sem_down()` when the semaphore has 0 resources are continuously blocked in
a while loop so that they stay asleep and do not claim the semaphore's resources
at the wrong time. We used a flag to enqueue threads into `blockedQ` only on the
thread's entrance into the while loop, so that multiple instances of the same
thread do not get added.

`int sem_up(sem_t sem)` handles freeing a semaphore's resource and unblocks the
first waiting thread in the `blockedQ` if there are threads waiting. This
happens by dequeueing the first element in the `blockedQ` and calling
`uthread_unblock()`, which will change the thread's state and add it back into
`uthread.c`'s `threadQ` so it can be scheduled as normal.

### *Testing*

Along with the provided test files, we created our own file named
`sem_corner.c` which tests the corner case where a thread takes a semaphore's
resource before a thread in the blocked queue can be awoken. For this test, we
added three threads to `threadQ` and blocked the first thread. When switching to
the second thread we freed the semaphore. However, the third thread takes this
resource before thread 1 wakes up, so thread 1 is left idling and never gets to
run its print statement.

<br>

## **Preemption API**

To implement preemption we followed these steps:

1. Create a sigaction that listens for `SIGVTALRM`
2. Define the signal handler function
3. Create a virtual timer that raises `SIGVTALRM` every 100Hz
4. Restore previous timer and action configurations

Of these steps, the only noteworthy parts are restoring the timer and action
configurations and our signal handler function.

Our signal handler function `void preempt_handler(int signum)` executes once a
`SIGVTALRM` is raised and simply calls `uthread_yield()`.

To restore the original timer configuration and previous action associated with
`SIGVTALRM`, we stored their previous configrations in global variables `struct
sigaction oldHandler` and `struct itimerval oldValue`.

 We did this by using the third parameter of both `sigaction()` and
 `setitimer()` when calling them initially to store the previous action and
 timer configurations.

But the previous configurations are not actually restored until we call
`preempt_stop()` which executes:

* `sigaction(SIGVTALRM, &oldHandler, NULL)`
* `setitimer(timerType, &oldValue, NULL)`

### *Testing*

To test our Preemption API, we devised three test cases that should generate the
correct output if our preemption implementation is sound.

To help us test preemption, we created a helper function `void delay(int
milliseconds)`. We call this function in our test threads to simulate a thread
'hogging' the CPU for too long.

#### *test_preempt.c*

Tests the preemption of various threads using `delay()` to control the order of
execution. The sequence of this tester is as follows:

1. `thread1` enqueues `thread2` and then enters a delay for 2 seconds.  
2. Preempt to `thread2`.  
3. `thread2` enqueues `thread3` and then enters a delay for 1 second.  
4. Preempt to `thread3`
5. `thread3` then prints, exits, and context switches back to `thread1`.  
6. `thread1` and `thread2` preempt to one another until thread2 finishes its 1 second delay.
7. `thread2` prints, exits, and context switches to `thread1`
8. `thread1` finshes its delay, prints, and exits.
9. Finishing the program with output in the order: `thread3`, `thread2`,
`thread1`

<br>

#### *test_preempt_disable.c*

Tests `preempt_disable()` and `preempt_enable()` over the execution of various
threads using `delay()` to control the order of execution. We included
`private.h` for testing purposes, normally user code would not be able to
disable or enable preemption. The sequence of this tester is as follows:

1. `thread1` enqueues `thread2`, calls `preempt_disable()`, and enters a delay
of 5 seconds.  
2. Since preempts were disabled, `thread1` will wait the full 5 second delay without getting preempted, print, and then exit.  
3. `thread2` is then scheduled to run, it enqueues `thread3`, calls `preempt_enable()`, and then enters a delay for 1 second.  
4. Since preempts were enabled, `thread2` will get preempted.  
5. Preempt to `thread3`.  
6. `thread3` prints and exits.  
7. `thread2` is scheduled to run, prints, and exits.
8. Finishing the program with output in the order: `thread1`, `thread3`, `thread2`

<br>

#### *test_preempt_stop.c*

Tests `preempt_stop()` over the execution of various threads using `delay()` to
control the order of execution. We included `private.h` for testing purposes,
normally user code would not be able to stop preemption. The sequence of this
tester is as follows:

1. `thread1` enqueues `thread2`, calls `preempt_stop()`, then enters a delay of
5 seconds.  
2. Since we restored action and timer configurations, preemption will not call our custom signal handler.  
3. `thread1` waits the full 5 second delay, prints, and then exits.  
4. `thread2` is scheduled to run, it prints and then exits.  
5. Finishing the program with output in the order: `thread1`, `thread2`
