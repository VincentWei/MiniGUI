/*
** $Id: ucos2_pprivate.h 7400 2007-08-20 05:57:07Z weiym $
** 
** ucos2_pprivate.h: types header for pthread implementation for uC/OS-II.
**
** MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, pSOS, ThreadX, 
** OSE, and Nucleus, version 2.0.x/1.6.x.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
** All rights reserved.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-02
*/

#ifndef _UCOSII_PPRIVATE_H
#define _UCOSII_PPRIVATE_H

//=============================================================================
// Thread control data structure

// Per-thread information needed by POSIX
// This is pointed to by the NR_KERNEL_THREADS_DATA_POSIX entry of the
// per-thread data.

typedef struct
{
    unsigned int        state:4,                // Thread state
                        cancelstate:2,          // Cancel state of thread
                        canceltype:2,           // Cancel type of thread
                        cancelpending:1,        // pending cancel flag
                        freestack:1;            // stack malloced, must be freed

    pthread_t           id;                     // My thread ID
    pthread_attr_t      attr;                   // Current thread attributes
    void                *retval;                // return value
    void                *(*start_routine)(void *); // start routine
    void                *start_arg;             // argument to start routine
    OS_EVENT            *joiner;                // joining threads wait here
    int                 nr_joined;              // number of joined threads.
    void                *stackmem;              // base of stack memory area
                                                // only valid if freestack == true

    struct pthread_cleanup_buffer *cancelbuffer; // stack of cleanup buffers

    // Per-thread data table pointer
    void                **thread_data;
    
} pthread_info;


// Values for the state field. These are solely concerned with the
// states visible to POSIX. 
// Note: numerical order here is important, do not rearrange.

#define PTHREAD_STATE_FREE      0       // This structure is free for reuse
#define PTHREAD_STATE_DETACHED  1       // The thread is running but detached
#define PTHREAD_STATE_RUNNING   2       // The thread is running and will wait
                                        // to join when it exits
#define PTHREAD_STATE_JOIN      3       // The thread has exited and is waiting
                                        // to be joined
#define PTHREAD_STATE_EXITED    4       // The thread has exited and is ready to
                                        // be reaped
#define PTHREAD_TESTCANCEL() pthread_testcancel()

//-----------------------------------------------------------------------------
// Priority translation.
// uC/OS-II priorities run from 0 as the highest to 64 as the lowest. POSIX priorities
// run in the opposite direction. The following macros translate between the two
// priority ranges.

#define PTHREAD_UCOSII_PRIORITY(pri) (UCOSII_THREAD_LOWEST_PRIORITY-(pri))

#define PTHREAD_POSIX_PRIORITY(pri) (UCOSII_THREAD_LOWEST_PRIORITY-(pri))

//-----------------------------------------------------------------------------
// Global data structures

//-----------------------------------------------------------------------------
// Functions exported by pthread.c to the other parts.

int ucos2_posix_pthread_init (void);

#endif /* _UCOSII_PPRIVATE_H */

