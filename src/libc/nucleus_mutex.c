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
** nucleus_mutex.c: Implementation of the POSIX pthread mutex functions
**                   for Nucleus.
**
** Create Date: 2005-03-03
*/

#include "mgconfig.h"

#if defined (__NUCLEUS__) && defined (_MGUSE_OWN_PTHREAD)

#include <nucleus.h>

#include "common.h"
#include "nucleus_pprivate.h"

#define NU_INHERIT      0
#define NU_NO_INHERIT   1

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
    
    attr->protocol = NU_NO_INHERIT;
    
    PTHREAD_RETURN (0);
}

//set priority inherit protocol
int pthread_mutexattr_setprotocol (pthread_mutexattr_t *attr, int protocol)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    if (protocol != NU_NO_INHERIT && protocol != NU_INHERIT)
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
    UINT status;
        
    // Set up the attributes we are going to use
    if (mutex_attr == NULL)
        pthread_mutexattr_init (&use_attr);
    else
        use_attr = *mutex_attr;

    __nupth_get_name (mutex->name, NAME_TYPE_MUTEX, 1);
    

    //create mutex
    status = NU_Create_Semaphore ((NU_SEMAPHORE *)&mutex->nu_mutex, mutex->name, 
                                  1, NU_PRIORITY);

    if (status == NU_SUCCESS) 
        PTHREAD_RETURN (0);
   
    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Destroy mutex.

int pthread_mutex_destroy (pthread_mutex_t *mutex)
{
    UINT status;

    PTHREAD_ENTRY ();
    PTHREAD_CHECK (mutex);

    /*
    if (mutex->nu_mutex.nu_mutex_ownership_count > 0)
        PTHREAD_RETURN (EBUSY);
    */

    status = NU_Delete_Semaphore ((NU_SEMAPHORE *)&mutex->nu_mutex);
    
    if (status == NU_SUCCESS)
        PTHREAD_RETURN (0);
        
    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Lock mutex, waiting for it if necessary.

int pthread_mutex_lock (pthread_mutex_t *mutex)
{
    UINT status;

    PTHREAD_ENTRY ();

    PTHREAD_CHECK (mutex);

    status = NU_Obtain_Semaphore ((NU_SEMAPHORE *)&mutex->nu_mutex, NU_SUSPEND);
    
    if (status == NU_SUCCESS)
        PTHREAD_RETURN (0);
        
    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Try to lock mutex.

int pthread_mutex_trylock (pthread_mutex_t *mutex)
{
    int ret;
    UINT status;
    
    PTHREAD_ENTRY ();

    PTHREAD_CHECK (mutex);

    status = NU_Obtain_Semaphore ((NU_SEMAPHORE *)&mutex->nu_mutex, NU_NO_SUSPEND);
    switch (status) {
    case NU_SUCCESS:
        ret = 0;
        break;

    case NU_UNAVAILABLE:
        ret = EBUSY;
        break;

    default:
        ret = EINVAL;
        break;
    }

    PTHREAD_RETURN (ret);
}


//-----------------------------------------------------------------------------
// Unlock mutex.

int pthread_mutex_unlock (pthread_mutex_t *mutex)
{
    UINT status;

    PTHREAD_ENTRY ();
    PTHREAD_CHECK (mutex);
    
    status = NU_Release_Semaphore ((NU_SEMAPHORE *)&mutex->nu_mutex);
    
    if (status == NU_SUCCESS)
        PTHREAD_RETURN (0);
    
    PTHREAD_RETURN (EINVAL);
}

#endif /* __NUCLEUS__ && _MGUSE_OWN_PTHREAD */

