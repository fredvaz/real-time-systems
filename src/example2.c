/**
 *
 * @brief Write the code of an application that, during some seconds, maintains the three tasks jointly
 * running using RMPO. This application should show if the tasks have met the temporal deadlines.
 * All the tasks 1 to 3 should be periodically activated with periods of 0.1 [s], 0.2 [s] and 0.3 [s],
 * respectively.
 *
 * @author fredvaz
 * @date 2024-04-26
 * @version 0.1
 * 
 */

#define _GNU_SOURCE     // The MACRO _GNU_SOURCE has to be defined to enable additional features provided by the GNU C library such as pthread.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "func.h"


// Activation Periods
#define PERIOD1_MS 100
#define PERIOD2_MS 200
#define PERIOD3_MS 300

// Number of Threads
#define N_THREADS 3

// Alias of the POSIX structure for a time value in nanoseconds.
typedef struct timespec timespec_t;

// Alias of the Data structure to describe a process' schedulability.
typedef struct sched_param sched_param_t;


void* task_1( void * arg )
{
    printf( "Hello! I am thread 1!\n" );
}

void* task_2( void * arg )
{
    printf( "Hello! I am thread 2!\n" );
}

void* task_3( void * arg )
{
    printf( "Hello! I am thread 3!\n" );
}


int main()
{
    timespec_t activation_time[N_THREADS], worse_response_time[3] = { {0,0}, {0,0}, {0,0} };

    cpu_set_t mask;

    pthread_t thr[N_THREADS];

    pthread_attr_t attr[N_THREADS];

    sched_param_t priorities[3];

    // Set all activation to 2 seconds after the current time.
    timespec_t delay; delay.tv_sec = 2; delay.tv_nsec = 0;

    void * tasks[N_THREADS] = { task_1, task_2, task_3 };

    
    // Lock the memory of the process
    mlockall( MCL_CURRENT | MCL_FUTURE );

    CPU_ZERO( &mask );      // This function is used for initializing a CPU set (a data structure representing a set of CPUs.
    CPU_SET( 0, &mask );    // Sets code to run in CPU Core. 


    // Set the main thread to lowest priority.
    pthread_t self = pthread_self();

    pthread_setschedprio( self, sched_get_priority_min( SCHED_FIFO ) );

    clock_gettime( CLOCK_MONOTONIC, &activation_time[0] );

    // Add a delay of 2 secons for all at the current time.
    for ( int i = 1; i < N_THREADS; i++ )
    {
        activation_time[i] = activation_time[0];
        activation_time[i].tv_sec += delay.tv_sec;
    }
    activation_time[0].tv_sec += delay.tv_sec;


    // For each thread change the scheduler to FIFO and set the priorities (in RMPO).

    for ( int i = 0; i < N_THREADS; i++ )
    {
        pthread_attr_init( &attr[i] );                                          // Set threads parameters.
        pthread_attr_setaffinity_np( &attr[i], sizeof(mask), &mask );           // Set threads affinity, preventing biased times.
        pthread_attr_setinheritsched( &attr[i], PTHREAD_EXPLICIT_SCHED );       // Set explicit scheduling.
        pthread_attr_setschedpolicy( &attr[i], SCHED_FIFO );                    // Set policy type to FIFO.

        priorities[i].sched_priority = sched_get_priority_max( SCHED_FIFO );    // Get the priority of each thread.
        pthread_attr_setschedparam( &attr[i], &priorities[i] );                    // Set to the values specified in the buffer.
    }

    // Create all threads
    printf( "Starting threads, please wait...\n" );

    for ( int i = 0; i < N_THREADS; i++ )
    {
        if ( pthread_create( &thr[i], &attr[i], tasks[i], NULL ) < 0 )
            exit( 0 );
    }

    // It will sleep for 5 seconds and then cancel all threads
    while ( 1 )
    {
        printf( "Sleeping for 5 seconds, please wait...\n" );
        sleep( 5 );
        
        printf( "Cancelling threads, please wait...\n" );
        for ( int i = 0; i < N_THREADS; i++ )
            pthread_cancel( thr[i] );

        return 0;
    }
}
