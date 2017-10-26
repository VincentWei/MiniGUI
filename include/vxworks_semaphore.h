/*
 * \file vxworks_semaphore.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2005/01/11
 * 
 * Description:
 *
 *  This header contains all the definitions needed to support
 *  POSIX semaphore under ThreadX. The reader is referred to the POSIX
 *  standard or equivalent documentation for details of the
 *  functionality contained herein.
 *
 \verbatim

    Copyright (C) 2005~2012 FMSoft.

    All rights reserved by FMSoft (http://www.fmsoft.cn).

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

    This header contains all the definitions needed to support
    POSIX semaphore under ThreadX. The reader is referred to the POSIX
    standard or equivalent documentation for details of the
    functionality contained herein.
 
    We do this work in order to run MiniGUI on ThreadX.

 \endverbatim
 */

/*
 * $Id: vxworks_semaphore.h 13674 2010-12-06 06:45:01Z wanzheng $
 *
 * This is the POSIX semaphore implementation in order to run MiniGUI on ThreadX.
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2005-2012 FMSoft.
 */

#ifndef FMVX_SEMAPHORE_H
#define FMVX_SEMAPHORE_H

#ifdef _MGUSE_OWN_SEMAPHORE
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _sem_t
{
    SEM_ID          semid;
    unsigned int    value;
} sem_t;

#define SEM_VALUE_MAX	UINT_MAX

/*-----------------------------------------------------------------------------
** Semaphore functions
*/

/* Initialize semaphore to value. */
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

#endif /* FMVX_SEMAPHORE_H */
#endif

/* End of pthread.h */
