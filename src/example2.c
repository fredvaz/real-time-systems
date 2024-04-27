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

typedef struct
{
    timespec_t activation_time;
    timespec_t response_time;
}thread_args_t;


// Covert the time from nanoseconds to miliseconds. 
long double time2ms( struct timespec t )
{
    return (long double)(t.tv_sec * (long double)1E3 + t.tv_nsec / (long double)1E6);
}


// Get the time difference between the start and end.
timespec_t time_diff( timespec_t start, timespec_t end )
{
    timespec_t delta_time;

    if ( (end.tv_nsec - start.tv_nsec) < 0 )                // 
    {
        delta_time.tv_sec = end.tv_sec - start.tv_sec - 1;
        delta_time.tv_nsec = 1E9 + end.tv_nsec - start.tv_nsec;
    }
    else                                                    // Default case
    {
        delta_time.tv_sec = end.tv_sec - start.tv_sec;
        delta_time.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return delta_time;
}


timespec_t time_sum( timespec_t t2, timespec_t t1 ) {

  timespec_t delta_time;

  delta_time.tv_nsec = t2.tv_nsec + t1.tv_nsec;
  delta_time.tv_sec = t2.tv_sec + t1.tv_sec;

  if ( delta_time.tv_nsec > (int)(1E9) - 1 )
  {
    delta_time.tv_nsec = delta_time.tv_nsec % (int)(1E9);
    delta_time.tv_sec = delta_time.tv_sec + 1;
  }
  return delta_time;
}


void * task_1( thread_args_t * task_args )
{
    clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &(task_args->activation_time), NULL );       // Wait for all tasks activate at the same time.

    timespec_t dt, temp; dt.tv_sec = 0, dt.tv_nsec = (PERIOD1_MS) * 1E6;                          // Create timespec for activation period.

    while ( 1 )
    {
        timespec_t run_time;

        // printf( "Thread 1!\n" );
        
        f1( 1, 2 );

        clock_gettime( CLOCK_MONOTONIC, &run_time );                                              // Get the Execution time.

        task_args->response_time = time_diff( task_args->activation_time, run_time );             // Calculate responde time.

        // temp = time_diff( )

        task_args->activation_time = time_sum( task_args->activation_time, dt );                  // Increment activation time.

        clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &(task_args->activation_time), NULL );   // Sleep until next activation time is reached.
    }
}


void * task_2( thread_args_t * task_args )
{
    clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &(task_args->activation_time), NULL );       // Wait for all tasks activate at the same time.

    timespec_t dt, temp; dt.tv_sec = 0, dt.tv_nsec = (PERIOD2_MS) * 1E6;                          // Create timespec for activation period.

    while ( 1 )
    {
        timespec_t run_time;

        // printf( "Thread 2!\n" );
        
        f2( 1, 2 );

        clock_gettime( CLOCK_MONOTONIC, &run_time );                                              // Get the Execution time.

        task_args->response_time = time_diff( task_args->activation_time, run_time );             // Calculate responde time.

        // temp = time_diff( )

        task_args->activation_time = time_sum( task_args->activation_time, dt );                  // Increment activation time.

        clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &(task_args->activation_time), NULL );   // Sleep until next activation time is reached.
    }
}


void * task_3( thread_args_t * task_args )
{
    clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &(task_args->activation_time), NULL );       // Wait for all tasks activate at the same time.

    timespec_t dt, temp; dt.tv_sec = 0, dt.tv_nsec = (PERIOD3_MS) * 1E6;                          // Create timespec for activation period.

    while ( 1 )
    {
        timespec_t run_time;

        // printf( "Thread 3!\n" );
        
        f3( 1, 2 );
        
        clock_gettime( CLOCK_MONOTONIC, &run_time );                                              // Get the Execution time.

        task_args->response_time = time_diff( task_args->activation_time, run_time );             // Calculate responde time.

        // temp = time_diff( )

        task_args->activation_time = time_sum( task_args->activation_time, dt );                  // Increment activation time.

        clock_nanosleep( CLOCK_MONOTONIC, TIMER_ABSTIME, &(task_args->activation_time), NULL );   // Sleep until next activation time is reached.
    }
}


int main()
{
    cpu_set_t       mask;

    pthread_t       thr[N_THREADS];

    pthread_attr_t  attr[N_THREADS];

    sched_param_t   priorities[N_THREADS];

    thread_args_t   task_args[N_THREADS];
    
    void            * tasks[N_THREADS] = { &task_1, &task_2, &task_3 };

    timespec_t      delay; delay.tv_sec = 2; delay.tv_nsec = 0;    // Set all activation to 2 seconds after the current time.

    
    // Lock the memory of the process
    mlockall( MCL_CURRENT | MCL_FUTURE );

    CPU_ZERO( &mask );      // This function is used for initializing a CPU set (a data structure representing a set of CPUs.
    CPU_SET( 0, &mask );    // Sets code to run in CPU Core. 


    // Set the main thread to lowest priority.
    pthread_t self = pthread_self();

    pthread_setschedprio( self, sched_get_priority_min( SCHED_FIFO ) );

    clock_gettime( CLOCK_MONOTONIC, &(task_args->activation_time) );

    // Add a delay of 2 secons for all at the current time.
    for ( int i = 1; i < N_THREADS; i++ )
    {        
        task_args[i].activation_time = task_args->activation_time;
        task_args[i].activation_time.tv_sec += delay.tv_sec;
    }
    task_args[0].activation_time.tv_sec += delay.tv_sec;


    // For each thread change the scheduler to FIFO and set the priorities (in RMPO).

    for ( int i = 0; i < N_THREADS; i++ )
    {
        pthread_attr_init( &attr[i] );                                          // Set threads parameters.
        pthread_attr_setaffinity_np( &attr[i], sizeof(mask), &mask );           // Set threads affinity, preventing biased times.
        pthread_attr_setinheritsched( &attr[i], PTHREAD_EXPLICIT_SCHED );       // Set explicit scheduling.
        pthread_attr_setschedpolicy( &attr[i], SCHED_FIFO );                    // Set policy type to FIFO.

        priorities[i].sched_priority = sched_get_priority_max( SCHED_FIFO );    // Get the priority of each thread.
        pthread_attr_setschedparam( &attr[i], &priorities[i] );                 // Set to the values specified in the buffer.
    }

    // Create all threads.
    printf( "Starting threads, please wait...\n" );

    for ( int i = 0; i < N_THREADS; i++ )
    {
        if ( pthread_create( &thr[i], &attr[i], tasks[i], &(task_args[i]) ) < 0 )
            exit( 0 );
    }

    // It will sleep for 5 seconds and then cancel all threads.
    while ( 1 )
    {
        printf( "Sleeping for 5 seconds, please wait...\n" );
        sleep( 5 );

        // Wait for threads to finish (if needed)
        // for (int i = 0; i < N_THREADS; ++i)
        //     pthread_join(thr[i], NULL);


        printf( "Cancelling threads, please wait...\n" );
        for ( int i = 0; i < N_THREADS; i++ )
            pthread_cancel( thr[ i ] );


        for ( int i = 0; i < N_THREADS; i++ )
            printf( "Task %d execution time: %LF ms\n", i, time2ms( task_args[i].response_time ) );
        
        return 0;
    }
}
