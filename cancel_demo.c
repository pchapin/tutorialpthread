/****************************************************************************
FILE          : cancel_demo.c
LAST REVISION : October 4, 2002
SUBJECT       : Program to demonstrate pthread cancellation.
PROGRAMMER    : (C) Copyright 2002 by Peter Chapin

Please send comments or bug reports to

     Peter Chapin
     P.O. Box 317
     Randolph Center, VT 05061
     pchapin@vtc.edu
****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock; // Synchronizes access to shared_data.
int shared_data;      // Imagine that this is much more complicated.


// This function unlocks the mutex pointed at by m. It is an example of
// a cancellation handler. Real cancellation handlers will probably need
// to clean up other resources besides just mutexes; the exact require-
// ments depend on the particular program. Obvious possibilities include
// open files and dynamically allocated memory.
//
void mutex_cleanup(void *m)
{
  pthread_mutex_t *p = (pthread_mutex_t *)m;
  printf("  Inside cancellation handler. Cleaning up elegantly...\n");
  pthread_mutex_unlock(p);
}

// This is the background thread function.
void *thread_function(void *arg)
{
  int old_state;
  int old_type;

  // These are the defaults. I put them here for educational purposes.
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &old_type);
  
  pthread_mutex_lock(&lock);
  pthread_cleanup_push(mutex_cleanup, &lock);

  // I use the shared data here. What if I get cancelled? The sleep call
  // below simulates a "long" system call (imagine that we are blocked
  // waiting for I/O that never happens).
  //
  sleep(120);

  pthread_cleanup_pop(0);
  pthread_mutex_unlock(&lock);
    // Note that pthread_cleanup_pop(1) pops the cancellation handler
    // and executes it as well, thus removing the need for an explicit
    // unlock operation here.

  return NULL;
}

// Main function sets up the thread and then cancels it (for fun).
int main(void)
{
  pthread_t  threadID;  // Identifier for the subordinate thread.
  void      *result;    // Return value from the subordinate thread.

  pthread_mutex_init(&lock, NULL);
  pthread_create(&threadID, NULL, thread_function, NULL);
  
  printf("Subordinate thread created. Waiting...\n");

  // In a real program we might use a timed condition variable to wait
  // for the subordinate thread to finish what it's doing.
  //
  sleep(10);

  printf("It's taking too long! Cancelling...\n");
  if (pthread_cancel(threadID) == ESRCH) {
    printf("Something went wrong... the thread appears to be already dead.\n");
  }
  printf("Done!\n");

  // Make sure the other thread has ended. This is particularly impor-
  // tant considering that pthread_cancel() might return before the
  // thread is actually cancelled. Thus we want to wait here so that we
  // know for sure that the cancellation handler ran.
  //
  pthread_join(threadID, &result);
  pthread_mutex_destroy(&lock);
  return 0;
}
