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
** ucos2_sem.c: implementation of pthread nonamed semaphore function 
**          under uC/OS-II.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-02
*/

#include "mgconfig.h"

#if defined(__UCOSII__) && defined(_MGUSE_OWN_PTHREAD)

#include <errno.h>
#include <limits.h>

#include "os_cpu.h"
#include "os_cfg.h"
#include "ucos_ii.h"

#define _HAVE_TYPE_BYTE 1
#define _HAVE_TYPE_WORD 1
#define _HAVE_TYPE_LONG 1

#include "common.h"
#include "own_stdio.h"

// -------------------------------------------------------------------------
// Internal definitions

#define _MACRO_START   do {
#define _MACRO_END     } while (0);

#define SEMA_ENTRY()

// Do a semaphore package defined return. This requires the error code
// to be placed in errno, and if it is non-zero, -1 returned as the
// result of the function. This also gives us a place to put any
// generic tidyup handling needed for things like signal delivery and
// cancellation.
#define SEMA_RETURN(err)                        \
_MACRO_START                                 \
    int __retval = 0;                           \
    if( err != 0 ) __retval = -1, errno = err;  \
    return __retval;                            \
_MACRO_END

// -------------------------------------------------------------------------
// Initialize semaphore to value.
// pshared is not supported under uC/OS-II.

int sem_init  (sem_t *sem, int pshared, unsigned int value)
{
    SEMA_ENTRY();

    if (value > SEM_VALUE_MAX)
        SEMA_RETURN (EINVAL)

    sem->os_sema = OSSemCreate (value);
    if (sem->os_sema)
        SEMA_RETURN (0)

    SEMA_RETURN (ENOMEM)
}

// -------------------------------------------------------------------------
// Destroy the semaphore.

int sem_destroy  (sem_t *sem)
{
    INT8U err;

    SEMA_ENTRY();

#if 0
    OS_SEM_DATA sem_data;
    // Check that the semaphore has no waiters
    if (OSSemQuery (sem->os_sema, &sem_data) == OS_NO_ERR) {
        if (sem_data->OSEvemtGrp)
            SEMA_RETURN (EBUSY);
    }
    else {
        SEMA_RETURN (EINVAL);
    }
#endif

    OSSemDel (sem->os_sema, OS_DEL_NO_PEND, &err);
    switch (err) {
    case OS_ERR_TASK_WAITING:
        SEMA_RETURN (EBUSY);
	break;
    case OS_NO_ERR:
        SEMA_RETURN(0);
	break;
    }

    SEMA_RETURN (EINVAL);
}

// -------------------------------------------------------------------------
// Decrement value if >0 or wait for a post.

int sem_wait  (sem_t *sem)
{
    INT8U err;
    int retval = 0;
    
    SEMA_ENTRY ();

    // check for cancellation first.
    pthread_testcancel ();

    OSSemPend (sem->os_sema, 0, &err);
    if (err != OS_NO_ERR)
        retval = EINVAL;
    
    // check if we were woken because we were being cancelled
    pthread_testcancel ();

    SEMA_RETURN (retval);
}

// -------------------------------------------------------------------------
// Decrement value if >0, return -1 if not.

int sem_trywait  (sem_t *sem)
{
    INT16U cnt;
    
    SEMA_ENTRY();

    cnt = OSSemAccept (sem->os_sema);
    if (cnt == 0)
        SEMA_RETURN(EAGAIN);
    
    SEMA_RETURN(0);
}

// -------------------------------------------------------------------------
// Increment value and wake a waiter if one is present.

int sem_post  (sem_t *sem)
{
    INT8U err;

    SEMA_ENTRY();

    err = OSSemPost (sem->os_sema);
    
    if (err != OS_NO_ERR)
        SEMA_RETURN(EINVAL);

    SEMA_RETURN(0);
}
    

// -------------------------------------------------------------------------
// Get current value

int sem_getvalue  (sem_t *sem, int *sval)
{
    OS_SEM_DATA sem_data;

    SEMA_ENTRY();

    if (OSSemQuery (sem->os_sema, &sem_data) == OS_NO_ERR) {
        *sval = sem_data.OSCnt;
    }
    else {
        SEMA_RETURN (EINVAL);
    }

    SEMA_RETURN (0);
}

#endif /* __UCOSII__ && _MGUSE_OWN_PTHREAD */

