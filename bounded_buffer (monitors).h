/****************************************************************************
FILE    : bounded_buffer.h
SUBJECT : Interface to a bounded buffer module using monitors.
AUTHOR  : (C) Copyright 2010 by Peter C. Chapin <PChapin@vtc.vsc.edu>

****************************************************************************/

#ifndef BOUNDED_BUFFER_H
#define BOUNDED_BUFFER_H

#include <pthread.h>

#define BOUNDED_BUFFER_SIZE 8

// This is our bounded buffer type.
typedef struct {
    void *buffer[BOUNDED_BUFFER_SIZE];
    pthread_mutex_t lock;
    pthread_cond_t  not_full;
    pthread_cond_t  not_empty;
    int             next_in;   // Next available slot.
    int             next_out;  // Oldest used slot.
    int             count;
    // We need a separate count member. The condition next_in == next_out could mean an empty
    // buffer or a full buffer; that case must be disambiguated.
} bounded_buffer_t;

void  bounded_buffer_init( bounded_buffer_t * );
void  bounded_buffer_destroy( bounded_buffer_t * );
void  bounded_buffer_push( bounded_buffer_t *, void * );
void *bounded_buffer_pop( bounded_buffer_t * );

#endif
