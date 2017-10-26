/**
 * \file ucos2_semaphore.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2004/02/03
 * 
 * semaphore.h: This header contains the POSIX semaphore definitions 
 *              needed to support MiniGUI under uC/OS-II. 
 *
 \verbatim

    Copyright (C) 2004~2012 FMSoft.

    All rights reserved by FMSoft (http://www.fmsoft.cn).

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/*
 * $Id: ucos2_semaphore.h 11349 2009-03-02 05:00:43Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2004-2012 FMSoft.
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

