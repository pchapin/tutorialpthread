/****************************************************************************
FILE          : pcbuffer_demo.c
LAST REVISION : 2008-09-15
SUBJECT       : Test program to exercise pcbuffer objects.
PROGRAMMER    : (C) Copyright 2008 by Peter Chapin

This program is very minimal; a large number of interesting cases are not
exercised (or at least not necessarily exercised). Using this program will
at least verify that the functions in pcbuffer.c compile and do something
useful without, for example, dumping core immediately. Building a more
complete test program would be nice. Maybe someday.

Please send comments or bug reports to

     Peter Chapin
     Computer Information Systems
     Vermont Technical College
     Randolph Center, VT 05061
     Peter.Chapin@vtc.vsc.edu
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include "pcbuffer.h"

#define OBJECT_COUNT 10000

pcbuffer my_buffer;

void *producer(void *arg)
{
  int   i;
  int  *p;

  for (i = 1; i <= OBJECT_COUNT; i++) {
    p = malloc(sizeof(int));
    *p = i;
    pcbuffer_push(&my_buffer, p);
    if (i % 1000 == 0) printf("P: %5d\n", i);
  }
  return NULL;
}


void *consumer(void *arg)
{
  int   i;
  int  *p;

  for (i = 1; i <= OBJECT_COUNT; i++) {
    p = pcbuffer_pop(&my_buffer);
    if (*p != i) {
      printf("C: Unexpected item from producer: %d\n", *p);
    }
    free(p);
    if (i % 1000 == 0) printf("C: %5d\n", i);
  }
  return NULL;
}


int main(void)
{
  pthread_t p_ID, c_ID;

  pcbuffer_init(&my_buffer);
  pthread_create(&p_ID, NULL, producer, NULL);
  pthread_create(&c_ID, NULL, consumer, NULL);

  // Let the threads do their thing.

  pthread_join(c_ID, NULL);
  pthread_join(p_ID, NULL);
  pcbuffer_destroy(&my_buffer);
  
  return 0;
}
