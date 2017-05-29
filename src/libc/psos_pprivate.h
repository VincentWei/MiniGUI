/*
** $Id: psos_pprivate.h 7400 2007-08-20 05:57:07Z weiym $
**
** psos_pprivate.h: The private header for POSIX PThreads implementation 
**      on pSOS. This is the POSIX PThreads implementation in order to run 
**      MiniGUI on pSOS.
**
** MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, pSOS, ThreadX, 
** OSE, and Nucleus, version 2.0.x/1.6.x.
**
** Copyright (C) 2007 Feynman Software.
** All rights reserved.
**
** Author: Wei Yongming
**
** Create Date: 2007-05-16
*/

#ifndef PSOSPTH_PPRIVATE_H
#define PSOSPTH_PPRIVATE_H

#include <stddef.h>

#include <psos.h>

#include "psos_pthread.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define PSOSPTHNUM_POSIX_PTHREAD_THREADS_MAX        64
#define PSOSPTHNUM_LIBC_MAIN_DEFAULT_STACK_SIZE     (1024 * 4)

#define PSOSPTH_THREAD_MIN_PRIORITY                 1
#define PSOSPTH_THREAD_MAX_PRIORITY                 239

#define PSOSPTH_THREAD_DEF_PRIORITY                 100
#define PSOSPTH_THREAD_DEF_TIME_SLICE               10

#define PSOSPTH_POSIX_MAIN_DEF_PRIORITY             100
#define PSOSPTH_POSIX_MAIN_DEF_TIME_SLICE           10

/* 
 * NOTE: This POSIX thread implementation on pSOS uses the task's 
 * numuber 7 notepad software register to hold the pointer to the
 * pthread_info structure of the task.
 *
 * t_setreg of pSOS+: 
 *  Registers 0 through 7 are for application use, and registers 8 
 *  through 15 are reserved for system use.
 */
#define PSOSPTH_SELFINFO_REGNUM         7

#define PSOSPTH_GLOBAL_MUTEX            "PTHL"
#define PSOSPTH_NAME_TASK               "FMTK"
#define PSOSPTH_NAME_MUTEX              "FMMU"
#define PSOSPTH_NAME_SEMAPHORE          "FMSE"

#define PSOSPTH_GLOBAL_MUTEX_CEILING    100

/*============================================================================*/
/* Thread control data structure */

/* 
 * Per-thread information needed by POSIX
 * This is pointed to by the tx_thread_name entry of the thread control block 
 * of a TX_THREAD.
 */

typedef struct
{
    /* The following is space for the pSOS thread object that underlies */
    /* this POSIX thread. It is allocated like this to avoid constructing */
    /* it on startup. */
    /* unsigned long       thread_obj; */

    unsigned int        state:4,            /* Thread state */
                        cancelstate:2,      /* Cancel state of thread */
                        canceltype:2,       /* Cancel type of thread */
                        cancelpending:1,    /* pending cancel flag */
                        freestack:1;        /* stack malloced, must be freed */

    pthread_t           id;                 /* my thread ID */
    unsigned long       psos_tid;           /* id of the pSOS thread object */
    pthread_attr_t      attr;               /* Current thread attributes */
    void                *retval;            /* return value */
    void                *(*start_routine)(void *); /* start routine */
    void                *start_arg;         /* argument to start routine */

    struct pthread_cleanup_buffer *cancelbuffer; /* stack of cleanup buffers */

    /* the joiner semaphore. */
    unsigned long       joiner;             /* joining threads wait here */
    int                 nr_joined;          /* number of joined threads. */

    /* Per-thread data table pointer */
    void                **thread_data;
} pthread_info;


/* 
 * Values for the state field. These are solely concerned with the
 * states visible to POSIX. The thread's run state is stored in the
 * pSOS thread object.
 *
 * Note: numerical order here is important, do not rearrange.
 */

#define PTHREAD_STATE_FREE      0   /* This structure is free for reuse */
#define PTHREAD_STATE_DETACHED  1   /* The thread is running but detached */
#define PTHREAD_STATE_RUNNING   2   /* The thread is running and will wait */
                                    /* to join when it exits */
#define PTHREAD_STATE_JOIN      3   /* The thread has exited and is waiting */
                                    /* to be joined */
#define PTHREAD_STATE_EXITED    4   /* The thread has exited and is ready to */
                                    /* be reaped */

/*--------------------------------------------------------------------------- */
/* Internal definitions */
#define _REPORT_RETVAL(err)
#define _REPORT_RETURN()

#define _MACRO_START   do {
#define _MACRO_END     } while (0);

/* Handle entry to a pthread package function.  */
#define PTHREAD_ENTRY()

/* Handle entry to a pthread package function with no args.  */
#define PTHREAD_ENTRY_VOID()

/* Do a pthread package defined return. This requires the error code to be */
/* returned as the result of the function. This also gives us a place to */
/* put any generic tidyup handling needed for things like signal delivery */
/* and cancellation. */
#define PTHREAD_RETURN(err)                  \
_MACRO_START                                 \
    _REPORT_RETVAL (err);                    \
    return err;                              \
_MACRO_END

/* A void variant of the above. */
#define PTHREAD_RETURN_VOID                  \
_MACRO_START                                 \
    _REPORT_RETURN();                        \
    return;                                  \
_MACRO_END

#if 0
#define PSOSPTH_ASSERT(cond, info)    if (!(cond)) SerialOutputString (info)
#define PSOSPTH_FAIL(info)            SerialOutputString (info)
#else
#define PSOSPTH_ASSERT(cond, info)
#define PSOSPTH_FAIL(info)
#endif

/* Check that a pointer passed in as an argument is valid and return */
/* EINVAL if it is not. This should be used to check pointers that are */
/* required to be valid. Pointers that may optionally be NULL should */
/* be checked within the function. */
#define PTHREAD_CHECK(ptr) if( (ptr) == NULL ) PTHREAD_RETURN(EINVAL);

#define PTHREAD_TESTCANCEL() pthread_testcancel()

/*--------------------------------------------------------------------------- */
/* Priority translation. */

/* pSOS priorities run from 255 as the highest to 0 as the lowest. 
 * Priority level 0 is reserved for the pSOS+ daemon task IDLE. 
 * Priority levels 240 - 255 are reserved for a variety of high priority 
 * pSOSystem daemon tasks. 
 *
 * The POSIX priorities run in the same direction. The following macros 
 * translate between the two priority ranges.
 */

#define PTHREAD_PSOS_PRIORITY(pri) (pri)

#define PTHREAD_POSIX_PRIORITY(pri) (pri)

/*--------------------------------------------------------------------------- */
/* Global data structures */

/* Mutex for locking access to pthread_info structures */

/*--------------------------------------------------------------------------- */
/* Functions exported by pthread.c to the other parts of the POSIX subsystem. */

pthread_info *pthread_self_info (void);

pthread_info *pthread_info_id (pthread_t id);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* !PSOSPTH_PPRIVATE_H */

/* End of psos_pprivate.h */

