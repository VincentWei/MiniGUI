/*
** $Id: sysvipc_mutex.c 7400 2007-08-20 05:57:07Z weiym $
** 
** sysvipc_mutex.c: implementation of pthread mutex function on SysV IPC.
**
** Copyright (C) 2005 ~ 2007 Feynman Software
** All rights reserved.
**
** Current maintainer: Wei Yongming
**
** Create date: 2005-01-05
*/

#include <stdio.h>
#include <errno.h>

#include "common.h"

#ifdef _USE_MUTEX_ON_SYSVIPC

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sched.h>

#include "sysvipc_private.h"

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

    attr->kind = SYSVIPC_PTHREAD_MUTEX_FAST_NP;
    
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

// Set the type of attribute object
int pthread_mutexattr_settype (pthread_mutexattr_t *attr, int kind)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if (kind < SYSVIPC_PTHREAD_MUTEX_FAST_NP || kind > SYSVIPC_PTHREAD_MUTEX_ERRORCHECK_NP)
        PTHREAD_RETURN (EINVAL);

    attr->kind = kind;
    
    PTHREAD_RETURN(0);
}

// Get the type of attribute object
int pthread_mutexattr_gettype (const pthread_mutexattr_t *attr, int *kind)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    *kind = attr->kind;
    
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
    PTHREAD_ENTRY ();

    PTHREAD_CHECK (mutex);

    // Set up the attributes we are going to use
    if (mutex_attr == NULL)
        pthread_mutexattr_init (&use_attr);
    else 
        use_attr = *mutex_attr;
    
    mutex->semid = _sysvipc_pth_mutex_set->semid;
    _sysvipc_pth_lock (_SYSVIPC_PTH_NUM_MUTEX_SET);
    mutex->sem_num = _sysvipc_pth_get_free_sem (_sysvipc_pth_mutex_set);
    _sysvipc_pth_unlock (_SYSVIPC_PTH_NUM_MUTEX_SET);

    if (mutex->sem_num < 0) {
        PTHREAD_RETURN (ENOMEM)
    }
    else {
        union semun sunion;

        sunion.val = 1;
        semctl (mutex->semid, mutex->sem_num, SETVAL, sunion);

        mutex->kind = use_attr.kind;
        if (mutex->kind == SYSVIPC_PTHREAD_MUTEX_RECURSIVE_NP) {
            mutex->locked_times = 0;
        }
    }

    PTHREAD_RETURN (0)
}

//-----------------------------------------------------------------------------
// Destroy mutex.

int pthread_mutex_destroy (pthread_mutex_t *mutex)
{
    int ret;
    int errval = EINVAL;
    union semun sunion;

    PTHREAD_ENTRY ();

    PTHREAD_CHECK (mutex);

    ret = semctl (mutex->semid, mutex->sem_num, GETZCNT, sunion);
    if (ret > 0) {
        errval = EBUSY;
    }
    else if (ret == 0) {
        _sysvipc_pth_lock (_SYSVIPC_PTH_NUM_MUTEX_SET);
        _sysvipc_pth_free_sem (_sysvipc_pth_mutex_set, mutex->sem_num);
        _sysvipc_pth_unlock (_SYSVIPC_PTH_NUM_MUTEX_SET);
        errval = 0;
    }

    PTHREAD_RETURN (errval);
}

//-----------------------------------------------------------------------------
// Lock mutex, waiting for it if necessary.

int pthread_mutex_lock (pthread_mutex_t *mutex)
{
    pid_t pid;
    int value;
    union semun sunion;
    
    PTHREAD_ENTRY ();

    PTHREAD_CHECK (mutex);

    if (mutex->semid <= 0) {
        if (pthread_mutex_init (mutex, NULL) < 0)
            PTHREAD_RETURN (EINVAL)
    }

    switch (mutex->kind) {
    case SYSVIPC_PTHREAD_MUTEX_FAST_NP:
        break;

    case SYSVIPC_PTHREAD_MUTEX_RECURSIVE_NP:
        pid = (pid_t) semctl (mutex->semid, mutex->sem_num, GETPID, sunion);
        value = semctl (mutex->semid, mutex->sem_num, GETVAL, sunion);
        if (value == 0 && pid == getpid ()) {
            mutex->locked_times ++;
            PTHREAD_RETURN (0);
        }
        break;

    case SYSVIPC_PTHREAD_MUTEX_ERRORCHECK_NP:
        pid = (pid_t) semctl (mutex->semid, mutex->sem_num, GETPID, sunion);
        value = semctl (mutex->semid, mutex->sem_num, GETVAL, sunion);
        if (value == 0 && pid == getpid ()) {
            PTHREAD_RETURN (EDEADLK);
        }
        break;
    }

    _sysvipc_pth_sem_op (mutex->semid, mutex->sem_num, -1);

    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Try to lock mutex.

int pthread_mutex_trylock (pthread_mutex_t *mutex)
{
    pid_t pid;
    int value;
    union semun sunion;

    PTHREAD_ENTRY ();

    PTHREAD_CHECK (mutex);
    
    value = semctl (mutex->semid, mutex->sem_num, GETVAL, sunion);
    if (value == 0)
        PTHREAD_RETURN (EBUSY);
    
    pthread_mutex_lock (mutex);

    PTHREAD_RETURN (0);
}


//-----------------------------------------------------------------------------
// Unlock mutex.

int pthread_mutex_unlock (pthread_mutex_t *mutex)
{
    pid_t pid;
    int value;
    union semun sunion;

    PTHREAD_ENTRY ();

    PTHREAD_CHECK (mutex);
    
    switch (mutex->kind) {
    case SYSVIPC_PTHREAD_MUTEX_FAST_NP:
        break;

    case SYSVIPC_PTHREAD_MUTEX_RECURSIVE_NP:
        if (mutex->locked_times > 0)
            mutex->locked_times --;

        if (mutex->locked_times > 0)
            PTHREAD_RETURN (0);
        break;

    case SYSVIPC_PTHREAD_MUTEX_ERRORCHECK_NP:
        pid = (pid_t) semctl (mutex->semid, mutex->sem_num, GETPID, sunion);
        value = semctl (mutex->semid, mutex->sem_num, GETVAL, sunion);
        if (value != 0 || pid != getpid ()) {
            PTHREAD_RETURN (EPERM);
        }
        break;
    }

    _sysvipc_pth_sem_op (mutex->semid, mutex->sem_num, 1);
    PTHREAD_RETURN (0);
}

#endif /* _USE_MUTEX_ON_SYSVIPC */

