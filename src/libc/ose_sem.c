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
** ose_sem.c: This file contains the implementation of the POSIX 
**          semaphore functions for OSE.
**
** Author: Qingzhou Long
**
** Create Date: 2006-05-19
*/

#include "mgconfig.h"

#if defined (__OSE__) 

#include <limits.h>
#include "ose.h"
#include "errno.h"
#include "common.h"

// -------------------------------------------------------------------------
//macro definitions

#define _MACRO_START   do {
#define _MACRO_END     } while (0);

#define SEMA_ENTRY()                        \
_MACRO_START                                \
    if (sem == NULL) {						\
    	errno = EINVAL;        				\
    	return -1;                          \
    }										\
_MACRO_END

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

int sem_init (sem_t *sem, int pshared, unsigned int value)
{
    UINT status;
   
    SEMA_ENTRY ();
    
    if (pshared) {
        printf ("pshared argument is not supported! \n");
        SEMA_RETURN (ENOSYS);
    } 

    if (value > SEM_VALUE_MAX)
        SEMA_RETURN (EINVAL);
    
    //create semaphore
    sem->ose_sem = create_sem(value);

    if (sem->ose_sem != NULL) {
        SEMA_RETURN (0);
    }
        
    SEMA_RETURN (ENOMEM);
}

// -------------------------------------------------------------------------
// Destroy the semaphore.

int sem_destroy  (sem_t *sem)
{
    SEMA_ENTRY();

    kill_sem (sem->ose_sem);
    
    SEMA_RETURN (0);
}

// -------------------------------------------------------------------------
// Decrement value if >0 or wait for a post.

int sem_wait  (sem_t *sem)
{
    SEMA_ENTRY ();

    // check for cancellation first.
    pthread_testcancel ();
    
    wait_sem (sem->ose_sem);

    // check if we were woken because we were being cancelled
    pthread_testcancel ();

    SEMA_RETURN (0);
}

// -------------------------------------------------------------------------
// Decrement value if >0, return -1 if not.

//FIXME
int sem_trywait  (sem_t *sem)
{
    int ret, val;

    SEMA_ENTRY ();
    val = get_sem (sem->ose_sem);
    if (val)
    {
        ret = 0;
        wait_sem (sem->ose_sem);
    }
    else
        ret = EAGAIN;
    SEMA_RETURN (ret);
}

// -------------------------------------------------------------------------
// Increment value and wake a waiter if one is present.

int sem_post  (sem_t *sem)
{
    SEMA_ENTRY ();

    signal_sem (sem->ose_sem);

    SEMA_RETURN (0);
}
    

// -------------------------------------------------------------------------
// Get current value

int sem_getvalue  (sem_t *sem, int *sval)
{
    SEMA_ENTRY ();
    
    *sval = get_sem (sem->ose_sem);

    SEMA_RETURN (0);
}

#endif /* __THREADX__ && _MGUSE_OWN_PTHREAD */

