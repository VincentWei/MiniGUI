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
** psos_sem.c: This file contains the implementation of the POSIX 
**          semaphore functions for pSOS.
**
** Author: Wei Yongming
**
** Create Date: 2007-05-16
*/

#include "mgconfig.h"

#if defined (__PSOS__) && defined (_MGUSE_OWN_PTHREAD)

#include <limits.h>
#include <stdio.h>

#include "common.h"
#include "psos_pprivate.h"

/* ------------------------------------------------------------------------- */
/*macro definitions */

#define _MACRO_START   do {
#define _MACRO_END     } while (0);

#define SEMA_ENTRY()                        \
_MACRO_START                                \
    if (sem == NULL) {						\
    	errno = EINVAL;        				\
    	return -1;                          \
    }										\
_MACRO_END

/* Do a semaphore package defined return. This requires the error code */
/* to be placed in errno, and if it is non-zero, -1 returned as the */
/* result of the function. This also gives us a place to put any */
/* generic tidyup handling needed for things like signal delivery and */
/* cancellation. */
#define SEMA_RETURN(err)                        \
_MACRO_START                                    \
    int __retval = 0;                           \
    if( err != 0 ) __retval = -1, errno = err;  \
    return __retval;                            \
_MACRO_END


/* ------------------------------------------------------------------------- */

/* Initialize semaphore to value. */

int sem_init (sem_t *sem, int pshared, unsigned int value)
{
    SEMA_ENTRY ();
    
    if (pshared) {
        fprintf (stderr, "pshared argument is not supported!");
        SEMA_RETURN (ENOSYS);
    } 

    if (value > SEM_VALUE_MAX)
        SEMA_RETURN (EINVAL);
    
    /* create os semaphore */
    if (sm_create (PSOSPTH_NAME_SEMAPHORE, value,
                SM_LOCAL | SM_FIFO | SM_UNBOUNDED, sem) == 0)
        SEMA_RETURN (0);

    SEMA_RETURN (ENOMEM);
}

/* ------------------------------------------------------------------------- */
/* Destroy the semaphore. */

int sem_destroy (sem_t *sem)
{
    struct sminfo sminfo;

    SEMA_ENTRY();

    if (sm_info (*sem, &sminfo) == 0) {
        if (sminfo.wqlen > 0)
            SEMA_RETURN (EBUSY);

        if (sm_delete (*sem) == 0)
            SEMA_RETURN (0);
    }
        
    SEMA_RETURN (EINVAL);
}

/* ------------------------------------------------------------------------- */
/* Decrement value if >0 or wait for a post. */

int sem_wait (sem_t *sem)
{
    int ret = 0;

    SEMA_ENTRY ();

    /* check for cancellation first. */
    pthread_testcancel ();
    
    if (sm_p (*sem, SM_WAIT, 0))
        ret = EINVAL;

    /* check if we were woken because we were being cancelled */
    pthread_testcancel ();

    SEMA_RETURN (ret);
}

/* ------------------------------------------------------------------------- */
/* Decrement value if >0, return -1 if not. */

int sem_trywait (sem_t *sem)
{
    int ret;

    SEMA_ENTRY ();
    
    switch (sm_p (*sem, SM_NOWAIT, 0)) {
    case 0:
        ret = 0;
        break;

    case ERR_NOSEM:
        ret = EAGAIN;
        break;

    default:
        ret = EINVAL;
        break;
    }

    SEMA_RETURN (ret);
}

/* ------------------------------------------------------------------------- */
/* Increment value and wake a waiter if one is present. */

int sem_post (sem_t *sem)
{
    int ret = 0;

    SEMA_ENTRY ();

    if (sm_v (*sem))
        ret = EINVAL;

    SEMA_RETURN (ret);
}
    

/* ------------------------------------------------------------------------- */
/* Get current value */

int sem_getvalue (sem_t *sem, int *sval)
{
    int ret;
    struct sminfo sminfo;

    SEMA_ENTRY();

    if (sm_info (*sem, &sminfo) == 0) {
        *sval = sminfo.count;
        ret = 0;
    }
    else {
        ret = EINVAL;
    }

    SEMA_RETURN (ret);
}

#endif /* __PSOS__ && _MGUSE_OWN_PTHREAD */

