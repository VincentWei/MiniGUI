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
** sysvipc_sem.c: implementation of pthread semaphore function on SysV IPC.
**
** Current maintainer: Wei Yongming
**
** Create date: 2005-01-05
*/

#include <errno.h>
#include <limits.h>

#include "common.h"

#ifdef _USE_SEM_ON_SYSVIPC

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>

#include "sysvipc_private.h"

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
_MACRO_START                                    \
    int __retval = 0;                           \
    if( err != 0 ) __retval = -1, errno = err;  \
    return __retval;                            \
_MACRO_END

// -------------------------------------------------------------------------
// Initialize semaphore to value.
// pshared is not supported under uClinux.

int sem_init  (sem_t *sem, int pshared, unsigned int value)
{
    union semun sunion;

    SEMA_ENTRY();

    if (value > SYSVIPC_SEM_VALUE_MAX)
        SEMA_RETURN (EINVAL)

    sem->semid = _sysvipc_pth_sem_set->semid;
    _sysvipc_pth_lock (_SYSVIPC_PTH_NUM_SEM_SET);
    sem->sem_num = _sysvipc_pth_get_free_sem (_sysvipc_pth_sem_set);
    _sysvipc_pth_unlock (_SYSVIPC_PTH_NUM_SEM_SET);

    if (sem->sem_num >= 0) {
        sunion.val = value;
        semctl (sem->semid, sem->sem_num, SETVAL, sunion);
        SEMA_RETURN (0)
    }

    SEMA_RETURN (ENOMEM)
}

// -------------------------------------------------------------------------
// Destroy the semaphore.

int sem_destroy  (sem_t *sem)
{
    int ret;
    union semun sunion;

    SEMA_ENTRY();

    ret = semctl (sem->semid, sem->sem_num, GETZCNT, sunion);
    if (ret > 0)
        SEMA_RETURN (EBUSY)
    else if (ret == 0) {
        _sysvipc_pth_lock (_SYSVIPC_PTH_NUM_SEM_SET);
        _sysvipc_pth_free_sem (_sysvipc_pth_sem_set, sem->sem_num);
        _sysvipc_pth_unlock (_SYSVIPC_PTH_NUM_SEM_SET);
        SEMA_RETURN(0);
    }

    SEMA_RETURN (EINVAL)
}

// -------------------------------------------------------------------------
// Decrement value if >0 or wait for a post.

int sem_wait  (sem_t *sem)
{
    struct sembuf sb;
    int retval = 0;

    SEMA_ENTRY ();

    // check for cancellation first.
    pthread_testcancel ();

again:
    sb.sem_num = sem->sem_num;
    sb.sem_op = -1;
    sb.sem_flg = 0;

    if (semop (sem->semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
        if (errno == EINVAL)
            retval = EINVAL;
    }

    // check if we were woken because we were being cancelled
    pthread_testcancel ();

    SEMA_RETURN (retval);
}

// -------------------------------------------------------------------------
// Decrement value if >0, return -1 if not.

int sem_trywait  (sem_t *sem)
{
    int ret;
    union semun sunion;

    SEMA_ENTRY();

    ret = semctl (sem->semid, sem->sem_num, GETVAL, sunion);
    if (ret < 0)
        SEMA_RETURN (EINVAL);
    if (ret == 0)
        SEMA_RETURN(EAGAIN);
    
    SEMA_RETURN(0);
}

// -------------------------------------------------------------------------
// Increment value and wake a waiter if one is present.

int sem_post  (sem_t *sem)
{
    struct sembuf sb;

again:
    sb.sem_num = sem->sem_num;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    if (semop (sem->semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
        if (errno == EINVAL)
            SEMA_RETURN(EINVAL);
    }

    SEMA_RETURN(0);
}
    

// -------------------------------------------------------------------------
// Get current value

int sem_getvalue  (sem_t *sem, int *sval)
{
    int ret;
    union semun sunion;

    SEMA_ENTRY();

    ret = semctl (sem->semid, sem->sem_num, GETVAL, sunion);
    if (ret < 0)
        SEMA_RETURN (EINVAL);

    *sval = ret;
    SEMA_RETURN (0);
}

#endif /* _USE_SEM_ON_SYSVIPC */

