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
** ucos2_mutex.c: implementation of pthread mutex function under uC/OS-II.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-02
*/

#include "mgconfig.h"

#if defined(__UCOSII__) && defined(_MGUSE_OWN_PTHREAD)

#include <errno.h>

#include "os_cpu.h"
#include "os_cfg.h"
#include "ucos_ii.h"

#define _HAVE_TYPE_BYTE 1
#define _HAVE_TYPE_WORD 1
#define _HAVE_TYPE_LONG 1

#include "common.h"
#include "own_stdio.h"

//#define USE_UCOS2_MUTEX	1
#undef USE_UCOS2_MUTEX

// -------------------------------------------------------------------------
// Internal definitions

#define _MACRO_START   do {
#define _MACRO_END     } while (0);

#define PTHREAD_ENTRY()

#define PTHREAD_CHECK(arg)             \
_MACRO_START                           \
    if (arg == NULL) {                 \
        errno = EINVAL;                \
        return -1;                     \
    }                                  \
_MACRO_END

// Do a semaphore package defined return. This requires the error code
// to be placed in errno, and if it is non-zero, -1 returned as the
// result of the function. This also gives us a place to put any
// generic tidyup handling needed for things like signal delivery and
// cancellation.
#define PTHREAD_RETURN(err)                     \
_MACRO_START                                    \
    int __retval = 0;                           \
    if( err != 0 ) __retval = -1, errno = err;  \
    return __retval;                            \
_MACRO_END

//=============================================================================
// Mutexes

//-----------------------------------------------------------------------------
// Mutex attributes manipulation functions

//-----------------------------------------------------------------------------
// Initialize attribute object

int pthread_mutexattr_init ( pthread_mutexattr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    attr->prio = OS_LOWEST_PRIO;
    //attr->prio = MINIGUI_HIGHEST_MUTEXT_PRIO;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Destroy attribute object

int pthread_mutexattr_destroy ( pthread_mutexattr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    // Nothing to do here...
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Optional functions depending on priority inversion protection options.

int pthread_mutexattr_setpriority ( pthread_mutexattr_t *attr, int priority)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if (priority < 0 || priority >= OS_LOWEST_PRIO) {
        PTHREAD_RETURN(EINVAL);
    }

    attr->prio = (INT8U) priority;
    
    PTHREAD_RETURN(0);
}

// Get priority
int pthread_mutexattr_getpriority ( pthread_mutexattr_t *attr, int* priority)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    *priority = attr->prio;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Mutex functions

//-----------------------------------------------------------------------------
// Initialize mutex. If mutex_attr is NULL, use default attributes.

int pthread_mutex_init (pthread_mutex_t *mutex,
                                const pthread_mutexattr_t *mutex_attr)
{
    pthread_mutexattr_t use_attr;
    
#if 0
    PTHREAD_ENTRY();

    PTHREAD_CHECK( mutex );
#endif

    // Set up the attributes we are going to use
    if( mutex_attr == NULL )
        pthread_mutexattr_init( &use_attr );
    else use_attr = *mutex_attr;
    
#if USE_UCOS2_MUTEX
    {
        INT8U err;
        mutex->os_mutex = OSMutexCreate (use_attr.prio, &err);
        if (err == OS_NO_ERR) {
            PTHREAD_RETURN(0);
        }
    }
#else
    mutex->os_mutex = OSSemCreate (1);
    if (mutex->os_mutex)
        PTHREAD_RETURN(0);
#endif

    PTHREAD_RETURN(ENOMEM);
}

//-----------------------------------------------------------------------------
// Destroy mutex.

int pthread_mutex_destroy (pthread_mutex_t *mutex)
{
    INT8U err;
    int errval = EINVAL;

    PTHREAD_ENTRY();

    PTHREAD_CHECK( mutex );

#if USE_UCOS2_MUTEX
    OSMutexDel (mutex->os_mutex, OS_DEL_NO_PEND, &err);
#else
    OSSemDel (mutex->os_mutex, OS_DEL_NO_PEND, &err);
#endif
    switch (err) {
    case OS_ERR_TASK_WAITING:
        errval = EBUSY;
	break;
    case OS_NO_ERR:
        errval = 0;
	break;
    }

    PTHREAD_RETURN(errval);
}

//-----------------------------------------------------------------------------
// Lock mutex, waiting for it if necessary.

int pthread_mutex_lock (pthread_mutex_t *mutex)
{
    INT8U err;
    int retval = 0;

    PTHREAD_ENTRY();

    PTHREAD_CHECK( mutex );

#if USE_UCOS2_MUTEX
    OSMutexPend (mutex->os_mutex, 0, &err);
#else
    OSSemPend (mutex->os_mutex, 0, &err);
#endif
    if (err != OS_NO_ERR)
        retval = EINVAL;

    PTHREAD_RETURN(retval);
}

//-----------------------------------------------------------------------------
// Try to lock mutex.

int pthread_mutex_trylock (pthread_mutex_t *mutex)
{
    INT16U ret;
    
    PTHREAD_ENTRY();

    PTHREAD_CHECK( mutex );
    
#if USE_UCOS2_MUTEX
    {
        INT8U err;
        ret = OSMutexAccept (mutex->os_mutex, &err);
    }
#else
    ret = OSSemAccept (mutex->os_mutex);
#endif
    if (ret == 0)
        PTHREAD_RETURN(EBUSY);
    
    PTHREAD_RETURN(0);
}


//-----------------------------------------------------------------------------
// Unlock mutex.

int pthread_mutex_unlock (pthread_mutex_t *mutex)
{
    INT8U err;

    PTHREAD_ENTRY();

    PTHREAD_CHECK( mutex );
    
#if USE_UCOS2_MUTEX
    err = OSMutexPost (mutex->os_mutex);
#else
    err = OSSemPost (mutex->os_mutex);
#endif

    if (err != OS_NO_ERR)
        PTHREAD_RETURN(EINVAL);

    PTHREAD_RETURN(0);
}


#if 0 /* We do not implement pthread condition variable */
//=============================================================================
// Condition Variables

//-----------------------------------------------------------------------------
// Attribute manipulation functions
// We do not actually support any attributes at present, so these do nothing.

//-----------------------------------------------------------------------------
// Initialize condition variable attributes

int pthread_condattr_init (pthread_condattr_t *attr)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    // There are no condition variable attributes at present
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Destroy condition variable attributes

int pthread_condattr_destroy (pthread_condattr_t *attr)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    // nothing to do here...
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Condition variable functions

//-----------------------------------------------------------------------------
// Initialize condition variable.

int pthread_cond_init (pthread_cond_t *cond,
                               const pthread_condattr_t *attr)
{
    PTHREAD_ENTRY ();

    PTHREAD_CHECK (cond);

    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Destroy condition variable.

int pthread_cond_destroy (pthread_cond_t *cond)
{
    PTHREAD_ENTRY ();

    PTHREAD_CHECK (cond);

    // TODO:

    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Wake up one thread waiting for condition variable

int pthread_cond_signal (pthread_cond_t *cond)
{
    PTHREAD_ENTRY ();

    PTHREAD_CHECK (cond);

    // TODO:

    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Wake up all threads waiting for condition variable

int pthread_cond_broadcast (pthread_cond_t *cond)
{
    PTHREAD_ENTRY ();

    PTHREAD_CHECK (cond);

    // TODO:

    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Block on condition variable until signalled. The mutex is
// assumed to be locked before this call, will be unlocked
// during the wait, and will be re-locked on wakeup.

int pthread_cond_wait (pthread_cond_t *cond,
                               pthread_mutex_t *mutex)
{
    PTHREAD_ENTRY ();

    // check for cancellation first.
    PTHREAD_TESTCANCEL ();

    PTHREAD_CHECK (cond);
    PTHREAD_CHECK (mutex);    

    // check if we were woken because we were being cancelled
    PTHREAD_TESTCANCEL ();

    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Block on condition variable until signalled, or the timeout expires.

int pthread_cond_timedwait (pthread_cond_t *cond,
                                    pthread_mutex_t *mutex,
                                    const struct timespec *abstime)
{
    PTHREAD_ENTRY ();

    // check for cancellation first.
    PTHREAD_TESTCANCEL ();

    PTHREAD_CHECK (cond);
    PTHREAD_CHECK (mutex);    
    PTHREAD_CHECK (abstime);    

    // check if we were woken because we were being cancelled
    PTHREAD_TESTCANCEL ();

    if (get_wake_reason == TIMEOUT)
        PTHREAD_RETURN (ETIMEDOUT);
    else
        PTHREAD_RETURN (0);
}

#endif /* We do not implement pthread condition variable */

#endif /* __UCOSII__ && _MGUSE_OWN_PTHREAD */

