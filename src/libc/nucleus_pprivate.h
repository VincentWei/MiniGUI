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
** nucleus_pprivate.h: The private header for POSIX Thread implementation 
**      on Nucleus.
**
** Create Date: 2005-03-03
*/

#ifndef NUPTH_PPRIVATE_H
#define NUPTH_PPRIVATE_H

#include <stddef.h>
#include <nucleus.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /* -------------------------------------- */
#define NUPTH_LEN_NAME      12
#define PTHREAD_STACK_DEFAULT   (1024*4)

#define UINT    unsigned int
#define ULONG   unsigned int

    /*
#   define ENOMEM      200
#   define EINVAL      201
#   define EBUSY       202
#   define EAGAIN      203
#   define ESRCH       204
#   define EDEADLK     205
#   define ENOTSUP     206
#   define ENOSYS      207
    */

typedef int (* PTH_ENTRY) (int argc, const char* argv []);
int nupth_posix_pthread_start (PTH_ENTRY pth_entry);
    /* -------------------------------------- */

#define NUPTHNUM_POSIX_PTHREAD_THREADS_MAX      64
#define NUPTHNUM_LIBC_MAIN_DEFAULT_STACK_SIZE   (1024 * 4)

#define NUPTH_THREAD_MIN_PRIORITY               31
#define NUPTH_THREAD_MAX_PRIORITY               10

#define NUPTH_THREAD_DEF_PRIORITY               20
#define NUPTH_THREAD_DEF_PREEMPT_THRESHOLD      20
#define NUPTH_THREAD_DEF_TIME_SLICE             10

#define NUPTH_POSIX_MAIN_DEF_PRIORITY           21
#define NUPTH_POSIX_MAIN_DEF_PREEMPT_THRESHOLD  21
#define NUPTH_POSIX_MAIN_DEF_TIME_SLICE         10

#define NUPTH_SIZE_BYTE_POOL        ((PTHREAD_STACK_DEFAULT + sizeof (pthread_info) + (1024*2)) * NUPTHNUM_POSIX_PTHREAD_THREADS_MAX)
#define NUPTH_NAME_BYTE_POOL        "BytePool4PThreads"

#define NUPTH_NAME_MUTEX            "Mutex4PThreads"

//=============================================================================
// Thread control data structure

// Per-thread information needed by POSIX
// This is pointed to by the tx_thread_name entry of the thread control block of a NU_THREAD.

typedef struct
{
    // The following is space for the Nucleus thread object that underlies
    // this POSIX thread. It is allocated like this to avoid constructing
    // it on startup.
    NU_TASK             thread_obj;

    char                name [NUPTH_LEN_NAME];  // Thread name
    unsigned int        state:4,                // Thread state
                        cancelstate:2,          // Cancel state of thread
                        canceltype:2,           // Cancel type of thread
                        cancelpending:1,        // pending cancel flag
                        freestack:1;            // stack malloced, must be freed

    int                 pth_errno;              // the per-thread errno location

    pthread_t           id;                     // My thread ID
    NU_TASK             *thread;                // pointer to Nucleus thread object
    pthread_attr_t      attr;                   // Current thread attributes
    void                *retval;                // return value
    void                *(*start_routine)(void *); // start routine
    void                *start_arg;             // argument to start routine
    NU_EVENT_GROUP      *joiner;               // joining threads wait here
    void                *stackmem;              // base of stack memory area
                                                // only valid if freestack == true

    struct pthread_cleanup_buffer *cancelbuffer; // stack of cleanup buffers

    // And the same for the joiner condition variable.
    NU_EVENT_GROUP      joiner_obj;
    char                joiner_name [NUPTH_LEN_NAME];

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
#define NUPTH_ASSERT(cond, info)    if (!(cond)) SerialOutputString (info)
#define NUPTH_FAIL(info)            SerialOutputString (info)
#else
#define NUPTH_ASSERT(cond, info)
#define NUPTH_FAIL(info)
#endif

// Check that a pointer passed in as an argument is valid and return
// EINVAL if it is not. This should be used to check pointers that are
// required to be valid. Pointers that may optionally be NULL should
// be checked within the function.
#define PTHREAD_CHECK(ptr) if( (ptr) == NULL ) PTHREAD_RETURN(EINVAL);

#define PTHREAD_TESTCANCEL() pthread_testcancel()

//-----------------------------------------------------------------------------
// Priority translation.
// Nucleus priorities run from 0 as the highest to 31 as the lowest. POSIX priorities
// run in the opposite direction. The following macros translate between the two
// priority ranges.

#define PTHREAD_NU_PRIORITY(pri) (NUPTH_THREAD_MIN_PRIORITY-(pri))

#define PTHREAD_POSIX_PRIORITY(pri) (NUPTH_THREAD_MIN_PRIORITY-(pri))

//-----------------------------------------------------------------------------
// Global data structures

// Mutex for locking access to pthread_info structures

//-----------------------------------------------------------------------------
// Functions exported by pthread.c to the other parts of the POSIX subsystem.

pthread_info *pthread_self_info (void);

pthread_info *pthread_info_id (pthread_t id);

#define NAME_TYPE_THREAD        't'
#define NAME_TYPE_MUTEX         'm'
#define NAME_TYPE_SEM           's'
#define NAME_TYPE_EVENTFLAGS    'e'

void __nupth_get_name (char *name, char type, int lock);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* !NUPTH_PPRIVATE_H */

