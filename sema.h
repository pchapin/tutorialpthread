/****************************************************************************
FILE    : sema.h
SUBJECT : Interface to semaphore abstract type.
AUTHOR  : (C) Copyright 2012 by Peter C. Chapin <PChapin@vtc.vsc.edu>

This header file specifies the interface to a semphore abstract type.
****************************************************************************/

#ifndef SEMA_H
#define SEMA_H

#include <pthread.h>

// This is our semaphore type.
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  non_zero;
    int             raw_count;
} semaphore_t;

void semaphore_init( semaphore_t *s, int initial_count );
void semaphore_destroy( semaphore_t *s );
void semaphore_up( semaphore_t *s );
void semaphore_down( semaphore_t *s );

#endif
