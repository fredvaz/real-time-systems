/**
 *
 * @brief Using the POSIX functionalities measure the execution time of each of the functions.
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
#include <sys/types.h>
#include <sched.h>
#include <pthread.h>

#include "func.h"


// Alias of the POSIX structure for a time value in nanoseconds.
typedef struct timespec timespec_t;

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

int main()
{
    timespec_t start, end;

    cpu_set_t mask;

    CPU_ZERO( &mask );      // This function is used for initializing a CPU set (a data structure representing a set of CPUs
    CPU_SET( 0, &mask );    // Sets code to run in CPU Core. 

    // Set the CPU affinity for a task.
    if ( sched_setaffinity( getpid(), sizeof(mask), &mask ) != 0 )
        printf( "Error on sched_setaffinity!\n" );

    // 
    
    clock_gettime( CLOCK_MONOTONIC, &start );
    f1( 1, 2 );
    clock_gettime( CLOCK_MONOTONIC, &end );

    printf( "Execution time for f1(): %LF ms\n", time2ms( time_diff( start, end ) ) );

    //

    clock_gettime( CLOCK_MONOTONIC, &start );
    f2( 1, 2 );
    clock_gettime( CLOCK_MONOTONIC, &end );

    printf( "Execution time for f2(): %LF ms\n", time2ms( time_diff( start, end ) ) );

    //
    
    clock_gettime( CLOCK_MONOTONIC, &start );
    f3( 1, 2 );
    clock_gettime( CLOCK_MONOTONIC, &end );

    printf( "Execution time for f3(): %LF ms\n", time2ms( time_diff( start, end ) ) );

    
    return 0;
}
