/****************************************************************************
FILE    : sema.c
SUBJECT : Implementation of a semaphore abstract type.
AUTHOR  : (C) Copyright 2008 by Peter C. Chapin <PChapin@vtc.vsc.edu>

****************************************************************************/

#include "sema.h"

// Implementation of my semaphore type.

void semaphore_init( semaphore_t *s, int initial_count )
{
    if( initial_count < 0 ) initial_count = 0;

    s->raw_count = initial_count;
    pthread_mutex_init( &s->lock, 0 );
    pthread_cond_init( &s->non_zero, 0 );
}


void semaphore_destroy( semaphore_t *s )
{
    pthread_mutex_destroy( &s->lock );
    pthread_cond_destroy( &s->non_zero );
}


void semaphore_up( semaphore_t *s )
{
    pthread_mutex_lock( &s->lock );
    s->raw_count++;
    pthread_mutex_unlock( &s->lock );
    pthread_cond_signal( &s->non_zero );
}


void semaphore_down( semaphore_t *s )
{
    pthread_mutex_lock( &s->lock );
    while( s->raw_count == 0 )
        pthread_cond_wait( &s->non_zero, &s->lock );

    s->raw_count--;
    pthread_mutex_unlock( &s->lock );
}

