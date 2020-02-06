# ECS-150-Project-002
## User-Level Thread Library

### Phase 1:
Implementing the queue was quite simple after our usage of linked lists in the
previous assignment. The only difference in this assignment was that we used a
*doubly* linked list, in order to effectively implement the queue. The code we
wrote in phase one was mostly a straightforward implementation of the specifics
given in the assignment.

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

Implementing `uthread_self` was *incredibly* simple, just a one liner. We
return the `TID` of the (global) `runningThread`.

The implementation of `uthread_exit` was also quite simple. We first set
the `retval` and set the `threadState` to a zombie state. We then check the
`blockedQueue` for the parent of our current thread, and if it was blocked, we
return it to a ready state and queue. We then context switch into the next
ready thread.
### Phase 3:

### Phase 4:

### Testing:

### Final Notes:
