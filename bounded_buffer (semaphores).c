/****************************************************************************
FILE    : pcbuffer.c
SUBJECT : Implementation of a bounded buffer module using semaphores.
AUTHOR  : (C) Copyright 2010 by Peter C. Chapin <PChapin@vtc.vsc.edu>

****************************************************************************/

#include "pcbuffer.h"

void pcbuffer_init( pcbuffer_t *p )
{
    pthread_mutex_init( &p->lock, NULL );
    sem_init( &p->used, 0, 0 );
    sem_init( &p->free, 0, PCBUFFER_SIZE );
    p->next_in = p->next_out = 0;
}


void pcbuffer_destroy( pcbuffer_t *p )
{
    pthread_mutex_destroy( &p->lock );
    sem_destroy( &p->used );
    sem_destroy( &p->free );
}


void pcbuffer_push( pcbuffer_t *p, void *incoming )
{
    sem_wait( &p->free );
    pthread_mutex_lock( &p->lock );
    p->buffer[p->next_in] = incoming;
    p->next_in++;
    if( p->next_in >= PCBUFFER_SIZE ) p->next_in = 0;
    pthread_mutex_unlock( &p->lock );
    sem_post( &p->used );
}


void *pcbuffer_pop( pcbuffer_t *p )
{
    void *return_value;

    sem_wait( &p->used );
    pthread_mutex_lock( &p->lock );
    return_value = p->buffer[p->next_out];
    p->next_out++;
    if( p->next_out >= PCBUFFER_SIZE ) p->next_out = 0;
    pthread_mutex_unlock( &p->lock );
    sem_post( &p->free );

    return return_value;
}
