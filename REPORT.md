# **User-level Thread Library Implementation**

## **Implementation**
Our implementation of this program is broken down into the creation of four APIs:

1. Queue API
2. Uthread API
3. Semaphore API
4. Preemption API

<br>

## **Queue API**
We implemented our queue using a linked list, it uses a `struct node` containing members for:

* `node* next`
* `void* data`

The `next` member holds the address to the next node in the linked list, while the `data` member holds the address of any data type since its a `void *`. In our use-case, the data member will hold the address of a thread's TCB which we will define later.

### *Testing*


<br>


## **Uthread API**


### *Testing*


<br>


## **Semaphore API**


### *Testing*


<br>


## **Preemption API**


### *Testing*


<br>
