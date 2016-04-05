/****************************************************************************
FILE          : rwlock.h
LAST REVISED  : 2007-11-22
SUBJECT       : Reader/writer locks.
PROGRAMMER    : (C) Copyright 2007 by Peter C. Chapin

This file contains the declaration of several functions that allow for
reader/writer locks. The function names should be fairly self-explanatory.

Please send comments or bug reports pertaining to this file to

     Peter C. Chapin
     Electrical and Computer Engineering Technology
     Vermont Technical College
     Randolph Center, VT 05061
     Peter.Chapin@vtc.vsc.edu
****************************************************************************/

#ifndef RWLOCK_H
#define RWLOCK_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_mutex_t wrt;
    int             readcount;
} rw_lock;

#ifdef __cplusplus
extern "C" {
#endif

void rw_init     ( rw_lock * );
void rw_destroy  ( rw_lock * );
void read_lock   ( rw_lock * );
void read_unlock ( rw_lock * );
void write_lock  ( rw_lock * );
void write_unlock( rw_lock * );

#ifdef __cplusplus
}
#endif

#endif
