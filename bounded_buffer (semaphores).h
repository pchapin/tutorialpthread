/****************************************************************************
FILE    : bounded_buffer.h
SUBJECT : Interface to a bounded buffer module using semaphores.
AUTHOR  : (C) Copyright 2010 by Peter C. Chapin <PChapin@vtc.vsc.edu>

****************************************************************************/

#ifndef PCBUFFER_H
#define PCBUFFER_H

#include <pthread.h>
#include <semaphore.h>

#define PCBUFFER_SIZE 8

// This is our producer/consumer buffer type.
typedef struct {
    void *buffer[PCBUFFER_SIZE];
    pthread_mutex_t lock;
    sem_t           used;      // Use POSIX semaphores here.
    sem_t           free;      // ...
    int             next_in;   // Next available slot.
    int             next_out;  // Oldest used slot.
} pcbuffer_t;

void  pcbuffer_init( pcbuffer_t * );
void  pcbuffer_destroy( pcbuffer_t * );
void  pcbuffer_push( pcbuffer_t *, void * );
void *pcbuffer_pop( pcbuffer_t * );

#endif
