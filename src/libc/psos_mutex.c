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
** mutex.c: This file contains the implementation of the POSIX 
**      pthread mutex functions for pSOS.
**
** Author: Wei Yongming
**
** Create Date: 2007-05-16
*/

#include "mgconfig.h"

#if defined (__PSOS__) && defined (_MGUSE_OWN_PTHREAD)

#include <psos.h>

#include "common.h"

#include "psos_pprivate.h"

/*=========================================================================== */
/* Mutexes */

/*--------------------------------------------------------------------------- */
/* Mutex attributes manipulation functions */

/*--------------------------------------------------------------------------- */
/* Initialize attribute object */

int pthread_mutexattr_init (pthread_mutexattr_t *attr)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    attr->type = PTHREAD_MUTEX_NORMAL;
    attr->protocol = PTHREAD_MUTEX_FIFO;
    attr->prio_ceiling = PSOSPTH_THREAD_DEF_PRIORITY;
    
    PTHREAD_RETURN (0);
}

/*--------------------------------------------------------------------------- */
/* Destroy attribute object */

int pthread_mutexattr_destroy ( pthread_mutexattr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    /* Nothing to do here... */
    
    PTHREAD_RETURN(0);
}

/* set mutex type */
int pthread_mutexattr_settype (pthread_mutexattr_t *attr, int type)
{
	PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    if (type != PTHREAD_MUTEX_NORMAL && type != PTHREAD_MUTEX_RECURSIVE)
        PTHREAD_RETURN (EINVAL);
        
    attr->type = type;
    
    PTHREAD_RETURN (0);
}

/* get mutex type */
int pthread_mutexattr_gettype (pthread_mutexattr_t *attr, int* type)
{
	PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    *type = attr->type;
    
    PTHREAD_RETURN (0);
}

/* set mutex protocol */
int pthread_mutexattr_setprotocol (pthread_mutexattr_t *attr, int protocol)
{
	PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    if (protocol != PTHREAD_MUTEX_FIFO
            && protocol != PTHREAD_PRIO_NONE
            && protocol != PTHREAD_PRIO_INHERIT
            && protocol != PTHREAD_PRIO_PROTECT)
        PTHREAD_RETURN (EINVAL);
        
    attr->protocol = protocol;
    
    PTHREAD_RETURN (0);
}

/* get mutex protocol */
int pthread_mutexattr_getprotocol (pthread_mutexattr_t *attr, int* protocol)
{
	PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    *protocol = attr->protocol;
    
    PTHREAD_RETURN (0);
}

/* Set the mutex prioceiling attribute in *ATTR to CEILING. */
int pthread_mutexattr_setprioceiling (pthread_mutexattr_t *attr, int ceiling)
{
	PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    if (ceiling < 1 || ceiling > 255)
        PTHREAD_RETURN (EINVAL);
        
    attr->prio_ceiling = ceiling;
    
    PTHREAD_RETURN (0);
}

/* Return in *CEILING the mutex prioceiling attribute in *ATTR. */
int pthread_mutexattr_getprioceiling (pthread_mutexattr_t *attr, int* ceiling)
{
	PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    *ceiling = attr->prio_ceiling;
    
    PTHREAD_RETURN (0);
}

/*--------------------------------------------------------------------------- */
/* Mutex functions */

/*--------------------------------------------------------------------------- */
/* Initialize mutex. If mutex_attr is NULL, use default attributes. */

int pthread_mutex_init (pthread_mutex_t *mutex,
                                const pthread_mutexattr_t *mutex_attr)
{
    pthread_mutexattr_t user_attr;
    unsigned long flags = MU_LOCAL;

    /* Set up the attributes we are going to use */
    if (mutex_attr == NULL)
        pthread_mutexattr_init (&user_attr);
    else
        user_attr = *mutex_attr;

    switch (user_attr.type) {
        case PTHREAD_MUTEX_NORMAL:
            flags |= MU_NORECURSIVE;
            break;
        case PTHREAD_MUTEX_RECURSIVE:
            flags |= MU_RECURSIVE;
            break;
        default:
            PTHREAD_RETURN (EINVAL);
    }

    switch (user_attr.protocol) {
        case PTHREAD_MUTEX_FIFO:
            flags |= MU_FIFO;
            break;
        case PTHREAD_PRIO_NONE:
            flags |= (MU_PRIOR | MU_PRIO_NONE);
            break;
        case PTHREAD_PRIO_INHERIT:
            flags |= (MU_PRIOR | MU_PRIO_INHERIT);
            break;
        case PTHREAD_PRIO_PROTECT:
            flags |= (MU_PRIOR | MU_PRIO_PROTECT);
            if (user_attr.prio_ceiling > 0 && user_attr.prio_ceiling < 256)
                break;
        default:
            PTHREAD_RETURN (EINVAL);
    }

    /* create os mutex */
    if (mu_create (PSOSPTH_NAME_MUTEX, flags, user_attr.prio_ceiling, mutex))
        PTHREAD_RETURN (ENOMEM);

    PTHREAD_RETURN (0);
}

/*--------------------------------------------------------------------------- */
/* Destroy mutex. */

int pthread_mutex_destroy (pthread_mutex_t *mutex)
{
    struct muinfo muinfo;

    PTHREAD_ENTRY ();
    PTHREAD_CHECK (mutex);

    if (mu_info (*mutex, &muinfo) == 0) {
        if (muinfo.wqlen > 0)
            PTHREAD_RETURN (EBUSY);

	    if (mu_delete (*mutex) == 0)
   		    PTHREAD_RETURN (0);
    }

    PTHREAD_RETURN (EINVAL);
}

/*--------------------------------------------------------------------------- */
/* Lock mutex, waiting for it if necessary. */

int pthread_mutex_lock (pthread_mutex_t *mutex)
{
    PTHREAD_ENTRY ();
    PTHREAD_CHECK (mutex);

    if (mu_lock (*mutex, MU_WAIT, 0) == 0)
		PTHREAD_RETURN (0);
	
    PTHREAD_RETURN (EINVAL);
}

/*--------------------------------------------------------------------------- */
/* Try to lock mutex. */

int pthread_mutex_trylock (pthread_mutex_t *mutex)
{
    int ret;
    
    PTHREAD_ENTRY ();
    PTHREAD_CHECK (mutex);

    switch (mu_lock (*mutex, MU_NOWAIT, 0)) {
    case 0:
        ret = 0;
        break;

    case ERR_MULOCKED:
        ret = EBUSY;
        break;

    default:
        ret = EINVAL;
        break;
    }

    PTHREAD_RETURN (ret);
}


/*--------------------------------------------------------------------------- */
/* Unlock mutex. */

int pthread_mutex_unlock (pthread_mutex_t *mutex)
{
    PTHREAD_ENTRY ();
    PTHREAD_CHECK (mutex);
    
    if (mu_unlock (*mutex) == 0)
		PTHREAD_RETURN (0);
	
    PTHREAD_RETURN (EINVAL);
}

/*  
 * Set the priority ceiling of MUTEX to CEILING, return 
 * old priority ceiling value in *OLD_CEILING
 */
int pthread_mutex_setprioceiling (pthread_mutex_t *mutex, int ceiling,
                int* old_ceiling)
{
    PTHREAD_ENTRY ();
    PTHREAD_CHECK (mutex);

    if (ceiling < 1 || ceiling > 255)
        PTHREAD_RETURN (EINVAL);

    if (mu_setceil (*mutex, ceiling, (unsigned long*)old_ceiling) == 0)
		PTHREAD_RETURN (0);
	
    PTHREAD_RETURN (EINVAL);
}

/* Get the priority ceiling of MUTEX. */
int pthread_mutex_getprioceiling (pthread_mutex_t *mutex, int* ceiling)
{
    PTHREAD_ENTRY ();
    PTHREAD_CHECK (mutex);

    if (ceiling == NULL)
        PTHREAD_RETURN (EINVAL);

    if (mu_setceil (*mutex, 0, (unsigned long*)ceiling) == 0)
		PTHREAD_RETURN (0);
	
    PTHREAD_RETURN (EINVAL);
}

#endif /* __PSOS__ && _MGUSE_OWN_PTHREAD */

