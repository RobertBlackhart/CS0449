Robert McDermot - rom66@pitt.edu

The original version of freader.c (without any mutex locking) did not output the expected 8000 characters for 2 reasons.

The first is that there needed to be a lock on the global variable numThreads.  This value was incremented each time a new thread started.  It controlled where in the buffer that thread should start writing.  If 2 threads read the value before either one modified it, they would stomp on each other's values while writing to the buffer.  This would produce output that would most likely leave part of the buffer blank.

The second reason that the expected output of freader.c was not initially produced was that the global variable bufCnt was not protected.  This variable told the main method how many characters had been written to the buffer so that it knew how many prints to do to the console.  If multiple threads accessed the variable before the others incremented its value, some of the increments would be lost (race condition).

The fix for both of these issues is to wrap the reads/writes of these variables in pthread_mutex_lock/unlock statements so that only 1 thread at a time could read/modify the variable thereby keeping them consistent with the state of the program.
