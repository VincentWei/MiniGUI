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
** threadx_mutex.c: This file contains the implementation of the POSIX 
**      pthread mutex functions for ThreadX.
**
** Author: Yan Xiaowei
**
** Create Date: 2005-01-11
*/

#include "mgconfig.h"

#if defined (__THREADX__) && defined (_MGUSE_OWN_PTHREAD)

#include "tx_api.h"

#include "common.h"

#include "threadx_pprivate.h"

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
    
    attr->protocol = TX_NO_INHERIT;
    
    PTHREAD_RETURN (0);
}

//set priority inherit protocol
int pthread_mutexattr_setprotocol (pthread_mutexattr_t *attr, int protocol)
{
	PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    if (protocol != TX_NO_INHERIT && protocol != TX_INHERIT)
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

	__txpth_get_name (mutex->name, NAME_TYPE_MUTEX, 1);
	

    //create mutex
    status = tx_mutex_create ((TX_MUTEX*)&mutex->tx_mutex, mutex->name, use_attr.protocol);

    if (status == TX_SUCCESS) 
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

    if (mutex->tx_mutex.tx_mutex_ownership_count > 0)
        PTHREAD_RETURN (EBUSY);

	status = tx_mutex_delete ((TX_MUTEX*)&mutex->tx_mutex);
	
   	if (!status)
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

	status = tx_mutex_get ((TX_MUTEX*)&mutex->tx_mutex, TX_WAIT_FOREVER);
	
    if (status == TX_SUCCESS)
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

	status = tx_mutex_get ((TX_MUTEX*)&mutex->tx_mutex, TX_NO_WAIT);
    switch (status) {
    case TX_SUCCESS:
        ret = 0;
        break;

    case TX_NOT_AVAILABLE:
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
    
    status = tx_mutex_put ((TX_MUTEX*)&mutex->tx_mutex);
	
	if (status == TX_SUCCESS)
		PTHREAD_RETURN (0);
	
    PTHREAD_RETURN (EINVAL);
}

#endif /* __THREADX__ && _MGUSE_OWN_PTHREAD */

