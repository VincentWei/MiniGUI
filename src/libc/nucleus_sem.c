/*
** $Id: nucleus_sem.c 8944 2007-12-29 08:29:16Z xwyan $
**
** nucleus_sem.c: Implementation of the POSIX semaphore functions
**         for Nucleus.
**
** Copyright (C) 2005 ~ 2007 Feynman Software.
** All rights reserved.
**
** Create Date: 2005-03-03
*/

#include "mgconfig.h"

#if defined (__NUCLEUS__) && defined (_MGUSE_OWN_PTHREAD)

#include <limits.h>
#include <nucleus.h>

#include "common.h"
#include "nucleus_pprivate.h"

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
    UINT status;
   
    SEMA_ENTRY ();
    
    if (pshared) {
        fprintf (stderr, "pshared argument is not supported!");
        SEMA_RETURN (ENOSYS);
    }

    if (value > SEM_VALUE_MAX)
        SEMA_RETURN (EINVAL);
    
    //obtain semaphore name    
    __nupth_get_name (sem->name, NAME_TYPE_SEM, 1);

    //create semaphore
    status = NU_Create_Semaphore (&sem->nu_sem, sem->name, value,
                                   NU_PRIORITY);

    if (status == NU_SUCCESS) {
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

    status = NU_Delete_Semaphore (&sem->nu_sem);
    
    if (status == NU_SUCCESS)
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
    
    status = NU_Obtain_Semaphore (&sem->nu_sem, NU_SUSPEND);

    if (status != NU_SUCCESS)
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
    
    switch (NU_Obtain_Semaphore (&sem->nu_sem, NU_NO_SUSPEND)) {
    case NU_SUCCESS:
        ret = 0;
        break;

    case NU_UNAVAILABLE:
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

    status = NU_Release_Semaphore (&sem->nu_sem);
    
    if (status != NU_SUCCESS)
        ret = EINVAL;

    SEMA_RETURN (ret);
}
    

// -------------------------------------------------------------------------
// Get current value

int sem_getvalue  (sem_t *sem, int *sval)
{
    int ret;
    UINT status;
    char name[64];
    ULONG cur_value;
    OPTION suspend_type;
    UNSIGNED tasks_suspended;
    NU_TASK *first_task;

    SEMA_ENTRY ();
    
    status = NU_Semaphore_Information (&sem->nu_sem, name, 
                (UNSIGNED *)&cur_value, &suspend_type, &tasks_suspended, &first_task);

    if (status == NU_SUCCESS) {
        *sval = cur_value;
        ret = 0;
    }
    else {
        ret = EINVAL;
    }

    SEMA_RETURN (ret);
}

#endif /* __NUCLEUS__ && _MGUSE_OWN_PTHREAD */

