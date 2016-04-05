/****************************************************************************
FILE          : barrier.h
SUBJECT       : Interface to the barrier abstract data type.
PROGRAMMER    : (C) Copyright 2010 by Peter C. Chapin <pcc482719@gmail.com>

****************************************************************************/

#ifndef BARRIER_H
#define BARRIER_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  all_released;
    pthread_cond_t  not_enough;
    int max;
    int count;
    int releasing;
    int wait_needed;
} barrier_t;

void barrier_init( barrier_t *b, int limit );
void barrier_destroy( barrier_t *b );
void barrier_wait( barrier_t *b );

#endif
