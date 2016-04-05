/****************************************************************************
FILE          : rwlock.c
LAST REVISED  : 2007-11-22
SUBJECT       : Reader/writer locks.
PROGRAMMER    : (C) Copyright 2007 by Peter C. Chapin

Please send comments or bug reports pertaining to this file to

     Peter C. Chapin
     Electrical and Computer Engineering Technology
     Vermont Technical College
     Randolph Center, VT 05061
     Peter.Chapin@vtc.vsc.edu
****************************************************************************/

#include "rwlock.h"

void rw_init( rw_lock *lock )
{
    pthread_mutex_init( &lock->mutex, NULL );
    pthread_mutex_init( &lock->wrt, NULL );
    lock->readcount = 0;
}


void rw_destroy( rw_lock *lock )
{
    pthread_mutex_destroy( &lock->mutex );
    pthread_mutex_destroy( &lock->wrt );
}


void read_lock( rw_lock *lock )
{
    pthread_mutex_lock( &lock->mutex );
    lock->readcount++;
    if( lock->readcount == 1 ) pthread_mutex_lock( &lock->wrt );
    pthread_mutex_unlock( &lock->mutex );
}


void read_unlock( rw_lock *lock )
{
    pthread_mutex_lock( &lock->mutex );
    lock->readcount--;
    if( lock->readcount == 0 ) pthread_mutex_unlock( &lock->wrt );
    pthread_mutex_unlock( &lock->mutex );
}


void write_lock( rw_lock *lock )
{
    pthread_mutex_lock( &lock->wrt );
}


void write_unlock( rw_lock *lock )
{
    pthread_mutex_unlock( &lock->wrt );
}

