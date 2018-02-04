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
** threadx_sem.c: This file contains the implementation of the POSIX 
**          semaphore functions for ThreadX.
**
** Author: Yan Xiaowei
**
** Create Date: 2005-01-11
*/

#include "mgconfig.h"

#if defined (__THREADX__) && defined (_MGUSE_OWN_PTHREAD)

#include <limits.h>
#include "tx_api.h"

#include "common.h"
#include "threadx_pprivate.h"

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
        fprintf (stderr, "pshared argument is not supported!");
        SEMA_RETURN (ENOSYS);
    } 

    if (value > SEM_VALUE_MAX)
        SEMA_RETURN (EINVAL);
    
    //obtain semaphore name    
    __txpth_get_name (sem->name, NAME_TYPE_SEM, 1);

    //create semaphore
    status = tx_semaphore_create ((TX_SEMAPHORE*)&sem->tx_sem, sem->name, value);

    if (status == TX_SUCCESS) {
        SEMA_RETURN (0);
    }
        
    SEMA_RETURN (ENOMEM);
}

// -------------------------------------------------------------------------
// Destroy the semaphore.

int sem_destroy  (sem_t *sem)
{
    UINT status;

    SEMA_ENTRY();

    if (sem->tx_sem.tx_semaphore_suspended_count > 0)
        SEMA_RETURN (EBUSY);

    status = tx_semaphore_delete ((TX_SEMAPHORE*)&sem->tx_sem);
    
    if (status == TX_SUCCESS)
        SEMA_RETURN (0);
        
    SEMA_RETURN (EINVAL);
}

// -------------------------------------------------------------------------
// Decrement value if >0 or wait for a post.

int sem_wait  (sem_t *sem)
{
    int ret = 0;
    UINT status;

    SEMA_ENTRY ();

    // check for cancellation first.
    pthread_testcancel ();
    
    status = tx_semaphore_get ((TX_SEMAPHORE*)&sem->tx_sem, TX_WAIT_FOREVER);

    if (status != TX_SUCCESS)
        ret = EINVAL;

    // check if we were woken because we were being cancelled
    pthread_testcancel ();

    SEMA_RETURN (ret);
}

// -------------------------------------------------------------------------
// Decrement value if >0, return -1 if not.

int sem_trywait  (sem_t *sem)
{
    int ret;

    SEMA_ENTRY ();
    
    switch (tx_semaphore_get ((TX_SEMAPHORE*)&sem->tx_sem, TX_NO_WAIT)) {
    case TX_SUCCESS:
        ret = 0;
        break;

    case TX_NO_INSTANCE:
        ret = EAGAIN;
        break;

    default:
        ret = EINVAL;
        break;
    }

    SEMA_RETURN (ret);
}

// -------------------------------------------------------------------------
// Increment value and wake a waiter if one is present.

int sem_post  (sem_t *sem)
{
    int ret = 0;
    UINT status;

    SEMA_ENTRY ();

    status = tx_semaphore_put ((TX_SEMAPHORE*)&sem->tx_sem);
    
    if (status != TX_SUCCESS)
        ret = EINVAL;

    SEMA_RETURN (ret);
}
    

// -------------------------------------------------------------------------
// Get current value

int sem_getvalue  (sem_t *sem, int *sval)
{
    int ret;
    UINT status;
    char *name;
    ULONG cur_value, susp_count;
    TX_THREAD *first_susp;
    TX_SEMAPHORE *next_sem;

    SEMA_ENTRY ();
    
    status = tx_semaphore_info_get ((TX_SEMAPHORE*)&sem->tx_sem, &name, 
                &cur_value, &first_susp, &susp_count, &next_sem);

    if (status == TX_SUCCESS) {
        *sval = cur_value;
        ret = 0;
    }
    else {
        ret = EINVAL;
    }

    SEMA_RETURN (ret);
}

#endif /* __THREADX__ && _MGUSE_OWN_PTHREAD */

