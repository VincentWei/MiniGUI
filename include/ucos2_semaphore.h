/**
 * \file ucos2_semaphore.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2004/02/03
 * 
 * \brief This header contains the POSIX semaphore definitions 
 *          needed to support MiniGUI under uC/OS-II. 
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: ucos2_semaphore.h 11349 2009-03-02 05:00:43Z weiym $
 *
 *          MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *          pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef UCOSII_SEMAPHORE_H
#define UCOSII_SEMAPHORE_H

#ifdef __UCOSII__

#define sem_t ucos2_sem_t

#define SEM_VALUE_MAX	USHRT_MAX

/*-----------------------------------------------------------------------------
** Semaphore object definition
*/

typedef struct
{
    void* os_sema;
} sem_t;

/*-----------------------------------------------------------------------------
** Semaphore functions
*/

/* Initialize semaphore to value. */
/* pshared is not supported under uC/OS-II. */
int sem_init  (sem_t *sem, int pshared, unsigned int value);

/* Destroy the semaphore. */
int sem_destroy  (sem_t *sem);

/* Decrement value if >0 or wait for a post. */
int sem_wait  (sem_t *sem);

/* Decrement value if >0, return -1 if not. */
int sem_trywait  (sem_t *sem);

/* Increment value and wake a waiter if one is present. */
int sem_post  (sem_t *sem);

/* Get current value */
int sem_getvalue  (sem_t *sem, int *sval);

/* We do not implement the named semaphore functions */
#if 0

/*-----------------------------------------------------------------------------
** Named semaphore functions
*/

/* Open an existing named semaphore, or create it. */
sem_t *sem_open  (const char *name, int oflag, ...);

/* Close descriptor for semaphore. */
int sem_close  (sem_t *sem);

/* Remove named semaphore */
int sem_unlink  (const char *name);

/*-----------------------------------------------------------------------------
** Special return value for sem_open()
*/

#define SEM_FAILED      ((sem_t *)NULL)

/* We do not implement the named semaphore functions */
#endif

#endif /* __UCOSII__ */
#endif /* UCOSII_SEMAPHORE_H */

