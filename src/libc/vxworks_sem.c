/*
** $Id: vxworks_sem.c 11129 2008-11-11 06:24:26Z weiym $
**
** vxworks_sem.c: Implementation of the POSIX semaphore functions
**         for VxWorks.
**
** Copyright (C) 2005 ~ 2008 Feynman Software.
** All rights reserved.
**
** Create Date: 2005-09-21
*/

#include "mgconfig.h"

#if defined (__VXWORKS__) && defined (_MGUSE_OWN_PTHREAD) && defined (_MGUSE_OWN_SEMAPHORE)

#include <limits.h>
#include <semLib.h>

#include "common.h"
#include "vxworks_pprivate.h"

// -------------------------------------------------------------------------
//macro definitions

#define _MACRO_START   do {
#define _MACRO_END     } while (0);

#define SEMA_ENTRY()                        \
_MACRO_START                                \
    if (sem == NULL) {                      \
        errno = EINVAL;                     \
        return -1;                          \
    }                                       \
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
    SEMA_ENTRY ();
    
    if (pshared) {
        fprintf (stderr, "pshared argument is not supported!");
        SEMA_RETURN (ENOSYS);
    }

    if (value > SEM_VALUE_MAX)
        SEMA_RETURN (EINVAL);
    
    sem->semid = semCCreate (SEM_Q_FIFO, value);

    if (sem->semid) {
        sem->value = value;
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

    status = semDelete (sem->semid);
    
    if (status == OK)
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
    
    status = semTake (sem->semid, WAIT_FOREVER);

    if (status != OK)
        ret = EINVAL;
    else
        if (sem->value > 0) sem->value --;

    // check if we were woken because we were being cancelled
    pthread_testcancel ();

    SEMA_RETURN (ret);
}

// -------------------------------------------------------------------------
// Decrement value if >0, return -1 if not.

int sem_trywait  (sem_t *sem)
{
    int ret;
    STATUS status;

    SEMA_ENTRY ();
    
    status = semTake (sem->semid, NO_WAIT);
    switch (status) {
    case OK:
        ret = 0;
        if (sem->value > 0) sem->value --;
        break;

    case ERROR:
        if (errno == S_objLib_OBJ_TIMEOUT || errno == S_objLib_OBJ_UNAVAILABLE)
            ret = EAGAIN;
        else
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

    status = semGive (sem->semid);
    
    if (status != OK)
        ret = EINVAL;
    else
        sem->value ++;

    SEMA_RETURN (ret);
}
    

// -------------------------------------------------------------------------
// Get current value

int sem_getvalue  (sem_t *sem, int *sval)
{
    int ret = 0;

    SEMA_ENTRY ();
    
    *sval = sem->value;

    SEMA_RETURN (ret);
}

#endif /* __VXWORKS__ && _MGUSE_OWN_PTHREAD && _MGUSE_OWN_SEMAPHORE */

