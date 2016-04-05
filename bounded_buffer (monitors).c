/****************************************************************************
FILE    : bounded_buffer.c
SUBJECT : Implementation of a bounded buffer module using monitors.
AUTHOR  : (C) Copyright 2010 by Peter C. Chapin <PChapin@vtc.vsc.edu>

****************************************************************************/

#include "bounded_buffer.h"

void bounded_buffer_init( bounded_buffer_t *p )
{
    pthread_mutex_init( &p->lock, NULL );
    pthread_cond_init( &p->not_full, NULL );
    pthread_cond_init( &p->not_empty, NULL );
    p->next_in  = 0;
    p->next_out = 0;
    p->count    = 0;
}


void bounded_buffer_destroy( bounded_buffer_t *p )
{
    pthread_mutex_destroy( &p->lock );
    pthread_cond_destroy( &p->not_full );
    pthread_cond_destroy( &p->not_empty );
}


void bounded_buffer_push( bounded_buffer_t *p, void *incoming )
{
    pthread_mutex_lock( &p->lock);
    while( p->count == BOUNDED_BUFFER_SIZE )
        pthread_cond_wait( &p->not_full, &p->lock );
    p->buffer[p->next_in] = incoming;
    p->next_in = (p->next_in + 1) % BOUNDED_BUFFER_SIZE;
    p->count++;
    pthread_cond_signal( &p->not_empty );
    pthread_mutex_unlock( &p->lock );
}


void *bounded_buffer_pop( bounded_buffer_t *p )
{
    void *return_value;

    pthread_mutex_lock( &p->lock );
    while( p->count == 0 )
        pthread_cond_wait( &p->not_empty, &p->lock );
    return_value = p->buffer[p->next_out];
    p->next_out = (p->next_out + 1) % BOUNDED_BUFFER_SIZE;
    p->count--;
    pthread_cond_signal( &p->not_full );
    pthread_mutex_unlock( &p->lock );

    return return_value;
}
