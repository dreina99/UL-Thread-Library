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

Of these steps, the only noteworthy part was the creation of the virtual timer. 

To restore the original timer configuration and previous action associated with `SIGVTALRM`, we made sure to store their previous configrations in global variables `struct sigaction oldHandler` and `struct itimerval oldValue`.

 We did this by using the third parameter of both `sigaction()` and `setitimer()` when calling them initially to store the previous action and timer configurations. 

But the previous configurations are not actually restored until we call `preempt_stop()` which executes:

* `sigaction(SIGVTALRM, &oldHandler, NULL)`
* `setitimer(timerType, &oldValue, NULL)`


### *Testing*


<br>
