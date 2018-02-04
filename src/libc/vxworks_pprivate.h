/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/*
** vxworks_ pprivate.h: The private header for POSIX Thread implementation 
**      on VxWorks.
**
** Create Date: 2005-09-21
*/

#ifndef VXPTH_PPRIVATE_H
#define VXPTH_PPRIVATE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* -------------------------------------- */
#define VXPTH_LEN_NAME          12
#define PTHREAD_STACK_DEFAULT   (1024*4)

#define UINT    unsigned int
#define ULONG   unsigned int

/*
#define ENOMEM      200
#define EINVAL      201
#define EBUSY       202
#define EAGAIN      203
#define ESRCH       204
#define EDEADLK     205
#define ENOTSUP     206
#define ENOSYS      207
*/

/* -------------------------------------- */

#define VXPTHNUM_POSIX_PTHREAD_THREADS_MAX      64
#define VXPTHNUM_LIBC_MAIN_DEFAULT_STACK_SIZE   (1024 * 4)

#define VXPTH_THREAD_MIN_PRIORITY               31
#define VXPTH_THREAD_MAX_PRIORITY               10

#define VXPTH_THREAD_DEF_PRIORITY               20
#define VXPTH_THREAD_DEF_PREEMPT_THRESHOLD      20
#define VXPTH_THREAD_DEF_TIME_SLICE             10

#define VXPTH_POSIX_MAIN_DEF_PRIORITY           21
#define VXPTH_POSIX_MAIN_DEF_PREEMPT_THRESHOLD  21
#define VXPTH_POSIX_MAIN_DEF_TIME_SLICE         10

//=============================================================================
// Thread control data structure

// Per-thread information needed by POSIX
// This is pointed to by the tx_thread_name entry of the thread control block of a VX_THREAD.

typedef struct
{
    // The following is space for the VxWorks thread object that underlies
    // this POSIX thread. It is allocated like this to avoid constructing
    // it on startup.
    WIND_TCB            tcb;
    int                 task_id;                // VxWorks task id
    SEM_ID              joiner;                 // joining threads wait here

    char                name [VXPTH_LEN_NAME];  // Thread name
    unsigned int        state:4,                // Thread state
                        cancelstate:2,          // Cancel state of thread
                        canceltype:2,           // Cancel type of thread
                        cancelpending:1,        // pending cancel flag
                        freestack:1;            // stack malloced, must be freed

    int                 pth_errno;              // the per-thread errno location

    pthread_t           id;                     // My thread ID
    pthread_attr_t      attr;                   // Current thread attributes
    void                *retval;                // return value
    void                *(*start_routine)(void *); // start routine
    void                *start_arg;             // argument to start routine
    void                *stackmem;              // base of stack memory area
                                                // only valid if freestack == true

    struct pthread_cleanup_buffer *cancelbuffer; // stack of cleanup buffers

    // Per-thread data table pointer
    void                *thread_data[PTHREAD_KEYS_MAX];
} pthread_info;


// Values for the state field. These are solely concerned with the
// states visible to POSIX. The thread's run state is stored in the
// eCos thread object.
// Note: numerical order here is important, do not rearrange.

#define PTHREAD_STATE_FREE      0       // This structure is free for reuse
#define PTHREAD_STATE_DETACHED  1       // The thread is running but detached
#define PTHREAD_STATE_RUNNING   2       // The thread is running and will wait
                                        // to join when it exits
#define PTHREAD_STATE_JOIN      3       // The thread has exited and is waiting
                                        // to be joined
#define PTHREAD_STATE_EXITED    4       // The thread has exited and is ready to
                                        // be reaped
//-----------------------------------------------------------------------------
// Internal definitions
#define _REPORT_RETVAL(err)
#define _REPORT_RETURN()

#define _MACRO_START   do {
#define _MACRO_END     } while (0);

// Handle entry to a pthread package function. 
#define PTHREAD_ENTRY()

// Handle entry to a pthread package function with no args. 
#define PTHREAD_ENTRY_VOID()

// Do a pthread package defined return. This requires the error code to be
// returned as the result of the function. This also gives us a place to
// put any generic tidyup handling needed for things like signal delivery
// and cancellation.
#define PTHREAD_RETURN(err)                  \
_MACRO_START                                 \
    _REPORT_RETVAL (err);                    \
    return err;                              \
_MACRO_END

// A void variant of the above.
#define PTHREAD_RETURN_VOID                  \
_MACRO_START                                 \
    _REPORT_RETURN();                        \
    return;                                  \
_MACRO_END

#if 0
#define VXPTH_ASSERT(cond, info)    if (!(cond)) SerialOutputString (info)
#define VXPTH_FAIL(info)            SerialOutputString (info)
#else
#define VXPTH_ASSERT(cond, info)
#define VXPTH_FAIL(info)
#endif

// Check that a pointer passed in as an argument is valid and return
// EINVAL if it is not. This should be used to check pointers that are
// required to be valid. Pointers that may optionally be NULL should
// be checked within the function.
#define PTHREAD_CHECK(ptr) if( (ptr) == NULL ) PTHREAD_RETURN(EINVAL);

#define PTHREAD_TESTCANCEL() pthread_testcancel()

//-----------------------------------------------------------------------------
// Priority translation.
// VxWorks priorities run from 0 as the highest to 31 as the lowest. POSIX priorities
// run in the opposite direction. The following macros translate between the two
// priority ranges.

#define PTHREAD_VX_PRIORITY(pri) (VXPTH_THREAD_MIN_PRIORITY-(pri))

#define PTHREAD_POSIX_PRIORITY(pri) (VXPTH_THREAD_MIN_PRIORITY-(pri))

//-----------------------------------------------------------------------------
// Global data structures

// Mutex for locking access to pthread_info structures

//-----------------------------------------------------------------------------
// Functions exported by pthread.c to the other parts of the POSIX subsystem.

//pthread_info *pthread_self_info (void);

pthread_info *pthread_info_id (pthread_t id);

#define NAME_TYPE_THREAD        't'
#define NAME_TYPE_MUTEX         'm'
#define NAME_TYPE_SEM           's'
#define NAME_TYPE_EVENTFLAGS    'e'

void __vxpth_get_name (char *name, char type, int lock);

int pthread_create (pthread_t *thread,
                    const pthread_attr_t *attr,
                    void *(*start_routine) (void *),
                    void *arg);
void pthread_exit (void *retval);
void pthread_testcancel (void);

int pthread_attr_init (pthread_attr_t *attr);
int pthread_attr_setinheritsched (pthread_attr_t *attr, int inherit);
int pthread_attr_setstackaddr (pthread_attr_t *attr, void *stackaddr);
int pthread_attr_setstacksize (pthread_attr_t *attr, size_t stacksize);
int pthread_setschedparam (pthread_t thread_id, int policy, const struct sched_param *param);
int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy);
int pthread_attr_setschedparam (pthread_attr_t *attr, const struct sched_param *param);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* !VXPTH_PPRIVATE_H */

