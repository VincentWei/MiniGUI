/*
** $Id: vxworks_mutex.c 13674 2010-12-06 06:45:01Z wanzheng $
**
** vxworks_mutex.c: Implementation of the POSIX pthread mutex functions
**                   for VxWorks.
**
** Copyright (C) 2005 ~ 2007 Feynman Software.
** All rights reserved.
**
** Create Date: 2005-09-21
*/

#include "mgconfig.h"

#if defined (__VXWORKS__) && defined (_MGUSE_OWN_PTHREAD)

#include <semLib.h>
#include <objLib.h>

#include "common.h"
#include "vxworks_pprivate.h"

#define VX_INHERIT      0
#define VX_NO_INHERIT   1

//=============================================================================
// Mutexes

//-----------------------------------------------------------------------------
// Mutex attributes manipulation functions

//-----------------------------------------------------------------------------
// Initialize attribute object


int pthread_mutexattr_init ( pthread_mutexattr_t *attr)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    attr->protocol = VX_NO_INHERIT;
    
    PTHREAD_RETURN (0);
}

//set priority inherit protocol
int pthread_mutexattr_setprotocol (pthread_mutexattr_t *attr, int protocol)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    if (protocol != VX_NO_INHERIT && protocol != VX_INHERIT)
        PTHREAD_RETURN (EINVAL);
        
    attr->protocol = protocol;
    
    PTHREAD_RETURN (0);
}

//get priority inherit protocol
int pthread_mutexattr_getprotocol (pthread_mutexattr_t *attr, int* protocol)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    *protocol = attr->protocol;
    
    PTHREAD_RETURN (0);
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


// ----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Mutex functions

//-----------------------------------------------------------------------------
// Initialize mutex. If mutex_attr is NULL, use default attributes.

int pthread_mutex_init (pthread_mutex_t *mutex,
                        const pthread_mutexattr_t *mutex_attr)
{
    pthread_mutexattr_t use_attr;
        
    // Set up the attributes we are going to use
    if (mutex_attr == NULL)
        pthread_mutexattr_init (&use_attr);
    else
        use_attr = *mutex_attr;

    //create mutex
    mutex->vx_mutex = semBCreate (SEM_Q_PRIORITY, SEM_FULL);

    if (mutex->vx_mutex)
        PTHREAD_RETURN (0);
   
    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Destroy mutex.

int pthread_mutex_destroy (pthread_mutex_t *mutex)
{
    int status;

    PTHREAD_ENTRY ();
    PTHREAD_CHECK (mutex);

    status = semDelete (mutex->vx_mutex);
    
    if (status == OK)
        PTHREAD_RETURN (0);
        
    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Lock mutex, waiting for it if necessary.

int pthread_mutex_lock (pthread_mutex_t *mutex)
{
    STATUS status;

    PTHREAD_ENTRY ();

    PTHREAD_CHECK (mutex);

    status = semTake (mutex->vx_mutex, WAIT_FOREVER);
    
    if (status == OK)
        PTHREAD_RETURN (0);
        
    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Try to lock mutex.

int pthread_mutex_trylock (pthread_mutex_t *mutex)
{
    int ret;
    STATUS status;
    
    PTHREAD_ENTRY ();

    PTHREAD_CHECK (mutex);

    status = semTake (mutex->vx_mutex, NO_WAIT);

    switch (status) {
    case OK:
        ret = 0;
        break;

    case ERROR:
        //FIXME
        if (errno == S_objLib_OBJ_TIMEOUT || errno == S_objLib_OBJ_UNAVAILABLE)
            ret = EBUSY;
        else
            ret = EINVAL;
        break;
    }

    PTHREAD_RETURN (ret);
}


//-----------------------------------------------------------------------------
// Unlock mutex.

int pthread_mutex_unlock (pthread_mutex_t *mutex)
{
    STATUS status;

    PTHREAD_ENTRY ();
    PTHREAD_CHECK (mutex);
    
    status = semGive (mutex->vx_mutex);
    
    if (status == OK)
        PTHREAD_RETURN (0);
    
    PTHREAD_RETURN (EINVAL);
}

#endif /* __VXWORKS__ && _MGUSE_OWN_PTHREAD */

