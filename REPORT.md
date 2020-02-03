# ECS-150-Project-002
## User-Level Thread Library

### Important Global Functions:

### Phase 1:

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
### Phase 4:

### Phase 5:

### Final Notes:
