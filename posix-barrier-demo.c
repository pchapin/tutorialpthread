
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_COUNT   10
#define MAX_LOOP_COUNT 1000000

pthread_barrier_t barrier;

void *thread_function( void *arg )
{
    int i;
    
    pthread_barrier_wait( &barrier );
    
    // Imagine doing something useful here.
    for( i = 0; i < MAX_LOOP_COUNT; ++i ) {
        ;
    }
    return NULL;
}


int main( void )
{
    int i;
    pthread_t IDs[THREAD_COUNT];
    
    // Initialize the barrier before anyone might try to use it.
    pthread_barrier_init( &barrier, NULL, THREAD_COUNT + 1 );
    
    for( i = 0; i < THREAD_COUNT; ++i ) {
        pthread_create( &IDs[i], NULL, thread_function, NULL );
    }
    pthread_barrier_wait( &barrier );
    
    // Imagine doing something useful here.
    printf( "The main thread is doing something useful!\n" );
    
    for( i = 0; i < THREAD_COUNT; ++i ) {
        pthread_join( IDs[i], NULL );
    }
    
    // Clean up the synchronization primitives. Nobody is using it here.
    pthread_barrier_destroy( &barrier );
    return EXIT_SUCCESS;
}
