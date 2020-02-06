# ECS-150-Project-002
## User-Level Thread Library

### Phase 1:
Implementing the queue was quite simple after our usage of linked lists in the
previous assignment. The only difference in this assignment was that we used a
*doubly* linked list, in order to effectively implement the queue. The code we
wrote in phase one was mostly a straightforward implementation of the specifics
given in the assignment.
[We used this resource to understand doubly linked lists](https://gist.github.com/mycodeschool/7429492?fbclid=IwAR24iiE892i0ILBeKzcquCkKj8n5u3_x6Kc_2wLRPX_NyDm7jrq_ZhfhM5A)
[And this website to understand the linked list implementation of a queue](https://gist.github.com/mycodeschool/7429492?fbclid=IwAR24iiE892i0ILBeKzcquCkKj8n5u3_x6Kc_2wLRPX_NyDm7jrq_ZhfhM5A)

The `createNewNode` function takes a data pointer, allocates memory for the
node struct, initializes the next attribute to `NULL` and the data attribute to
the data input. The return value is a pointer to the new node.

The `queue_create` function takes no input, but works in a similar way. It
allocates memory for the queue struct, initializes the head and tail attributes
to `NULL` and the length attribute to 0, and returns a pointer to the queue.

The `queue_destroy` function simply checks if the queue is empty, and if it is,
it frees the memory of the queue struct.

The `queue_enqueue` function takes a queue and data to be queued. It first calls
`createNewNode` on the data, then takes the new node and adds it to the given
queue. If the queue was previously empty, it assigns the data to the head, and
otherwise to the tail. It also increments the length attribute of the queue.

The `queue_dequeue` function takes a queue and double pointer to a variable
to store the data that is popped from the queue. It simply takes the value in
head attribute of the queue, storing it in the passed pointer, and changes the
head of the queue to the next node in the queue by accessing the head node next
attribute. The queue length attribute is also decremented. If the dequeue has
popped off the last item in the queue, the tail is set to `NULL`.

The `queue_delete` function takes a queue and data, iterates through the queue
and searches for the first node that has a data attribute that matches the data
input. It then removes that node, linking the node before it to the node after
it, then frees the node.

The `queue_iterate` function takes a queue, a function, an argument, and a
double pointer to a variable to store the returned data. The function
iterates through all elements of the queue, calling the given function with the
data of the current node in the queue as the first argument, and the argument
passed to `queue_iterate` as the second argument. If the function returns a 1,
the iteration stops, and the pointer to the variable is set to the pointer of
the data attribute of the current node.

The `queue_length` function takes a queue struct and simply returns the value
of its length attribute.

### Phase 2:
The `checkTID` function was created for usage in a `queue_iterate` call. It
returns a 1 or 0 representing whether the thread's `TID` equals the given `TID`.

The function `threadInitialization` takes a function and pointer to an argument
as inputs. First, it creates the `readyQueue`, `blockedQueue`, and
`zombieQueue`. It then initializes a `mainThread`: it allocates space for the
`threadContext`, sets the `TID` to the `threadCount`, sets the `parentTID` to
-1, sets the `stackPointer` to the pointer returned by the *given* function to
allocate the stack, and sets the `threadState` to RUNNING. It then sets the
`runningThread` to this newly initialized `mainThread` and initializes the
context using the *given* function that takes the function and argument that
were passed into `threadInitialization` as an input.

Implementing `uthread_yield` was quite straightforward. We simply dequeue the
next available thread from the `readyQueue`, set it as our `runningThread`, and
set the `runningThread` state attribute as RUNNING. We then take the old
thread and store it in the `blockedQueue` or `readyQueue` depending on the
`threadState`. Finally, we context switch from the old thread into the new one.

Implementing `uthread_self` was *incredibly* simple, just a one liner. We
return the `TID` of the (global) `runningThread`.

The function `uthread_create` takes a function and argument as inputs. It first
calls `threadInitialization` if no threads exist, allocates space for a thread
struct, then in the struct: increments the `threadCount`, sets the state to
READY, initializes the `stackPointer`. Finally, it enqueues this new thread.

The implementation of `uthread_exit` was also quite simple. The function takes
in a return value. We first take the `runningThread` and set the `retValue`
attribute as given and the `threadState` to a zombie state. We then check the
`blockedQueue` to find the parent of our current thread, and if it was blocked,
we return it to a ready state and queue. We then context switch into the next
ready thread.
### Phase 3:

### Phase 4:

### Testing:
To test the queue, we created multiple functions to test various functionalities
that we were expected to have. We assert that the return values are what we
expect, to ensure that the functions are correct. We also check the resulting
queue/node to ensure that the tested function has worked as expected.
### Final Notes:
