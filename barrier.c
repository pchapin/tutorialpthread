/****************************************************************************
FILE          : barrier.c
SUBJECT       : Implementation of the barrier abstract type.
PROGRAMMER    : (C) Copyright 2010 by Peter C. Chapin <pcc482719@gmail.com>

****************************************************************************/

#include "barrier.h"

void barrier_init( barrier_t *b, int limit )
{
    pthread_mutex_init( &b->lock, NULL );
    pthread_cond_init( &b->not_enough, NULL );
    pthread_cond_init( &b->all_released, NULL );
    if( limit < 1 ) limit = 1;
    b->max         = limit;
    b->count       = 0;
    b->releasing   = 0;
    b->wait_needed = 0;
}

void barrier_destroy( barrier_t *b )
{
    pthread_cond_destroy( &b->not_enough );
    pthread_cond_destroy( &b->all_released );
    pthread_mutex_destroy( &b->lock );
}

void barrier_wait( barrier_t *b )
{
    pthread_mutex_lock( &b->lock );

    // If the previous batch of threads is releasing, wait until they are all released.
    while( b->releasing ) pthread_cond_wait( &b->all_released, &b->lock );

    // One more thread on the barrier.
    ++b->count;

    // If we've reached the limit, start the releasing process.
    if( b->count == b->max ) {
        b->releasing   = 1;
        b->wait_needed = 0;
        pthread_cond_broadcast( &b->not_enough );
        --b->count;
    }
    else {
        // We are not at the limit; we need to wait.
        b->wait_needed = 1;
        while( b->wait_needed ) pthread_cond_wait( &b->not_enough, &b->lock );
        --b->count;

        // If we are the last thread out, turn off the releasing process and let others in.
        if( b->count == 0 ) {
            b->releasing = 0;
            pthread_cond_broadcast( &b->all_released );
        }
    }
    pthread_mutex_unlock( &b->lock );
}
