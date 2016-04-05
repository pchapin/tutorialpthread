/****************************************************************************
FILE          : attr_demo.c
LAST REVISION : October 4, 2002
SUBJECT       : Program to demonstrate thread attributes.
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


// This is the background thread function.
void *thread_function(void *arg)
{
  pthread_mutex_lock(&lock);
  printf("I'm in the thread function.\n");

  // I use the shared data here.
  sleep(10);

  pthread_mutex_unlock(&lock);
  return NULL;
}


// Main function sets up the thread and configures a number of attributes.
int main(void)
{
  pthread_attr_t attributes;  // Thread attributes object.
  pthread_t      threadID;    // Identifier for the subordinate thread.
  void          *result;      // Return value from the subordinate thread.
  int            rc;          // Return code from thread functions.

  pthread_mutex_init(&lock, NULL);

  // Set up the attribute object according to our desires.
  pthread_attr_init(&attributes);
  rc = pthread_attr_setinheritsched(&attributes, PTHREAD_EXPLICIT_SCHED);
  if (rc != 0) printf("pthread_attr_setinheritsched() failed!\n");
  rc = pthread_attr_setschedpolicy(&attributes, SCHED_FIFO);
  if (rc != 0) printf("pthread_attr_setschedpolicy() failed!\n");
  rc = pthread_attr_setscope(&attributes, PTHREAD_SCOPE_PROCESS);
  if (rc != 0) printf("pthread_attr_setscope() failed!\n");

  // Create the thread. Note that it has FIFO scheduling.
  rc = pthread_create(&threadID, &attributes, thread_function, NULL);
  if (rc != 0) {
    printf("pthread_create() failed!\n");
    switch (rc) {
      case EAGAIN:
        printf("Insufficient resources or resource limit reached.\n");
        break;
      case EINVAL:
        printf("Invalid thread attributes.\n");
        break;
      case ENOMEM:
        printf("Insufficient memory.\n");
        break;
      case EPERM:
        printf("Insufficient permission to create thread with specified attributes.\n");
        break;
    }
    return 1;
  }
  
  printf("Subordinate thread created. Waiting...\n");

  // Wait for it to end.
  pthread_join(threadID, &result);
  printf("Thread ended.\n");

  pthread_mutex_destroy(&lock);
  return 0;
}
