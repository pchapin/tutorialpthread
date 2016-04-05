/****************************************************************************
FILE    : philosophers.c
SUBJECT : Solution to the dining philosophers problem.
AUTHOR  : (C) Copyright 2010 by Peter C. Chapin <pcc482719@gmail.com>

This solution follows the pseudo code in "Operating Systems Design and Implementation" third
edition by Andrew S. Tanenbaum and Albert S. Woodhull. Prentice Hall. Copyright 2006.
ISBN=0-13-142938-8, pages 89-92.

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N  5     // How many philosophers are dining tonight.

// The philosophers are arranged around a circular table. These macros compute the index of the
// philosopher on the left and right respectively of the given philosopher.
//
#define LEFT(philosopher_number)  ( ( philosopher_number + N - 1) % N )
#define RIGHT(philosopher_number) ( ( philosopher_number + 1 )    % N )

// The three things that philosophers do.
typedef enum { Thinking, Hungry, Eating } state_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
state_t         state_array[N];   // Keep track of what each philosopher is doing.
sem_t           coordinate[N];    // Coordinate access to forks.


// How a philosopher thinks.
void think( int philosopher_number )
{
    printf( "Philosopher %i is thinking...\n", philosopher_number );
    sleep( 10 );
}


// How a philosopher eats.
void eat( int philosopher_number )
{
    printf( "Philosopher %i is eating...\n", philosopher_number );
    sleep( 5 );
}


// Allow the given philosopher to eat if possible.
void test( int philosopher_number )
{
    if( state_array[philosopher_number]        == Hungry &&
        state_array[LEFT(philosopher_number)]  != Eating &&
        state_array[RIGHT(philosopher_number)] != Eating    ) {

        state_array[philosopher_number] = Eating;
        sem_post( &coordinate[philosopher_number] );
    }
}


// Attempt to obtain necessary resources for eating.
void take_forks( int philosopher_number )
{
    pthread_mutex_lock( &mutex );
    state_array[philosopher_number] = Hungry;
    test( philosopher_number );                  // I may or may not be eating after this.
    pthread_mutex_unlock( &mutex );
    sem_wait( &coordinate[philosopher_number] ); // Block if I'm not eating.
}


// Release resources.
void put_forks( int philosopher_number )
{
    pthread_mutex_lock( &mutex );
    state_array[philosopher_number] = Thinking;
    test( LEFT(philosopher_number) );            // Allow neighbor to eat if possible.
    test( RIGHT(philosopher_number) );           // Allow neighbor to eat if possible.
    pthread_mutex_unlock( &mutex );
}


// What a philosopher does.
void *philosopher(void *arg)
{
    // Find out my position around the table.
    int philosopher_number = *(int *)arg;
    free( arg );

    while( 1 ) {
        think( philosopher_number );
        take_forks( philosopher_number );
        eat( philosopher_number );
        put_forks( philosopher_number );
    }

    return NULL;
}


int main( void )
{
    int       philosopher_number;
    int       i;
    int      *arg;
    pthread_t threadIDs[N];

    // Initialize the semaphores.
    for( i = 0; i < N; ++i ) {
        sem_init( &coordinate[i], 0, 0 );
    }

    // Create the philosopher threads and assign each one to a table position.
    for( philosopher_number = 0; philosopher_number < N; ++philosopher_number ) {
        arg = (int *)malloc( sizeof( int ) );
       *arg = philosopher_number;
        pthread_create( &threadIDs[philosopher_number], NULL, philosopher, arg );
    }

    // Wait for the meal to finish.
    for( philosopher_number = 0; philosopher_number < N; ++philosopher_number ) {
        pthread_join( threadIDs[philosopher_number], NULL );
    }

    // Clean up the semaphores.
    for( i = 0; i < N; ++i ) {
        sem_destroy( &coordinate[i] );
    }
    return EXIT_SUCCESS;
}
