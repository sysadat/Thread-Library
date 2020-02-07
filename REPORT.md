# ECS-150-Project-002
## User-Level Thread Library

### Phase 1:
Implementing the queue was quite simple after our experience using linked lists
in the previous assignment. The only difference in this assignment was that we
used a *doubly* linked list, in order to effectively implement the queue. The
code we wrote in phase one was mostly a straightforward implementation of the
specifics given in the assignment.
[We used this resource to understand doubly linked lists](https://gist.github.com/mycodeschool/7429492?fbclid=IwAR24iiE892i0ILBeKzcquCkKj8n5u3_x6Kc_2wLRPX_NyDm7jrq_ZhfhM5A)
[And this website to understand the linked list implementation of a queue](https://gist.github.com/mycodeschool/7429492?fbclid=IwAR24iiE892i0ILBeKzcquCkKj8n5u3_x6Kc_2wLRPX_NyDm7jrq_ZhfhM5A)

The `createNewNode` function takes a data pointer, allocates memory for the
node struct, initializes the `next` attribute to `NULL` and the `data`
attribute to the data input. The return value is a pointer to the new node.

The `queue_create` function takes no input, but works in a similar way. It
allocates memory for the queue struct, initializes the `head` and `tail`
attributes to `NULL` and the length attribute to 0, and returns a pointer to
the queue.

The `queue_destroy` function simply checks if the queue is empty, and if it is,
it frees the memory of the queue struct.

The `queue_enqueue` function takes a queue and data to be queued. It first calls
`createNewNode` with the data as an argument, then adds the new node to the
given queue. If the queue was previously empty, it assigns the node to the
head, and otherwise to the tail. It also increments the `length` attribute of
the queue.

The `queue_dequeue` function takes a queue and double pointer to a variable
for storing the data that will be popped from the queue. It simply takes the
value in the `head` attribute of the queue, storing it in the passed pointer,
and changes the head of the queue to the next node in the queue by accessing the
head node's `next` attribute. The queue `length` attribute is then decremented.
If the dequeue has popped off the last item in the queue, the tail is set to
`NULL`.

The `queue_delete` function takes a queue and some data. It iterates through
the queue, searching for the first node that has a data attribute matching the
data input. It then removes that node, linking the node before it to the node
after it, then frees the removed node.

The `queue_iterate` function takes a queue, a function, an argument, and a
double pointer to a variable for storing the returned data. The function
iterates through all elements of the queue, calling the given function with the
data of the current node in the queue as the first argument, and the argument
passed to `queue_iterate` as the second argument. If the function returns a 1,
the iteration stops, and the pointer to the variable (given as an input) is set
to the pointer of the `data` attribute of the current node. This essentially
just returns the data in the first node that causes the function to return a 1.

The `queue_length` function takes a queue struct and simply returns the value
of its `length` attribute.

### Phase 2:
The `checkTID` function was created for usage in a `queue_iterate` call. It
returns a 1 or 0 representing whether the thread's `TID` equals the given `TID`.

The function `threadInitialization` takes a function and pointer to an argument
as inputs. First, it creates the `readyQueue`, `blockedQueue`, and
`zombieQueue`. It then initializes a `mainThread`: it allocates space for the
`threadContext`, sets the `TID` to the `threadCount`, sets the `parentTID` to
-1, sets the `stackPointer` to the pointer returned by the *given* function to
allocate the stack, and sets the `threadState` to `RUNNING`. It then sets the
`runningThread` to this newly initialized `mainThread` and initializes the
context using the *given* function which takes the function and argument that
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
`READY`, and initializes the `stackPointer`. Finally, it enqueues this new
thread in the `readyQueue`.

The implementation of `uthread_exit` was also quite simple. The function takes
in a return value. We first take the `runningThread` and set the `retValue`
attribute as given and the `threadState` to a zombie state. We then check the
`blockedQueue` to find the parent of our current thread, and if it was blocked,
we return it to a ready state and add it to the `readyQueue`. We then context
switch into the next available thread.

### Phase 3:
The entirety of phase 3 was a single function `uthread_join`. For inputs, the
function takes a `TID` and a pointer to a variable to store the return value.
First, we check to ensure the following: we are not joining main, the `TID` is
not the same as the `runningThread`, and the `TID` was properly passed in.
Next, we use `queue_iterate` to find the given `TID` in our `readyQueue`. If
found, this thread is a child of the `runningThread`, and we now need to join
the parent and child. We first check if the child is being joined elsewhere,
and stop if yes. We set the `runningThread` to a `BLOCKED` state, set the
parent of the child to the `runningThread`, and set the `joinCheck` attribute
to `TRUE` to prevent it from being joined elsewhere. Finally we call
`uthread_yield` and save the return value as the `retValue` attribute of the
`runningThread`. If the `TID` was not found in the `readyQueue`, our next step
is to check the `blockedQueue`, and perform the same exact process if the
`TID` is found. At this point, if we have still not found the `TID`, we
*similarly* check the `zombieQueue`. If the `TID` is found in the
`zombieQueue`, we need to collect its resources and free it. To do this, we
delete the stack that the child belongs to, free the child thread, and delete
it from the `zombieQueue`. Finally, we store the return value as the `retValue`
attribute of the `runningThread`.

### Phase 4:
In the beginning of the file, we define the macro `INTERVAL` as the ratio of
the number of milliseconds in a second (1,000,000) to the given frequency (Hz).

The function `sigvtalrmHandler` is simply a call to `uthread_yield`, for use in
`preempt_start`.

Implementing `preempt_disable` and `preempt_enable` were both one line
`sigprocmask` calls to set the `signalSetter` to `SIG_BLOCK` or `SIG_UNBLOCK`.

`preempt_start` sets up preemption for use. It first initializes the
`signalSetter`, adding `SIGVTALRM` to it. Then, it sets a `sigaction` to call
`sigvtalrmHandler` in the case that a `SIGVTALRM` is received. It then calls
`setitimer` to initialize the `ITIMER_VIRTUAL` attributes: it sets the timer
interval to the computed `INTERVAL` and initializes the timer to 0.

### Testing:
To test the queue, we created multiple functions to test various functionalities
that we were expected to have. We check that the return values are what we
expect, to verify that the functions work properly. To further verify our
results, we check the resulting queue/node (when possible) to ensure that the
tested function has worked as expected. We also have a "NULL" test that
tests all our functions with `NULL` inputs to ensure errors are handled
properly.

To test `uthread_join` we made two nested threads, each with a print to
identify the order at which they were executed to see that joining works as
intended. We also verify the return values of each thread to make ensure that a
join call returns the correct value.

Similarly, we checked the order of the print values in the test `uthread_self`.

A simple test was `uthread_self`, where we attempted to join our own thread and
expected a error.

Another simple test was `uthread_exist`, where we attempted to join a non
existent thread and expected a error.

Another test was `uthread_multiThread`, where we kept adding threads and made
sure that nothing broke.

To test preemption, we created an infinite loop in main and one in a thread
with no yields, exits, or returns. After a certain amount of time, the print
statement changes to the other one, indicating an interruption. Also, the
entire executable stops after a while and prints "Virtual Timer Expired". As
we used no yields or joins, these interruptions prove our preemption works as
expected.
