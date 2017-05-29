/*
 * \file nucleus_semaphore.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2005/01/11
 * 
 * Description:
 *
 *  This header contains all the definitions needed to support
 *  POSIX semaphore under Nucleus. The reader is referred to the POSIX
 *  standard or equivalent documentation for details of the
 *  functionality contained herein.
 *
 \verbatim

    Copyright (C) 2005~2009 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

    This header contains all the definitions needed to support
    POSIX semaphore under Nucleus. The reader is referred to the POSIX
    standard or equivalent documentation for details of the
    functionality contained herein.
 
    We do this work in order to run MiniGUI on Nucleus.

 \endverbatim
 */

/*
 * $Id: nucleus_semaphore.h 11349 2009-03-02 05:00:43Z weiym $
 *
 * This is the POSIX semaphore implementation in order to run MiniGUI on Nucleus.
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *              pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *      Copyright (C) 2005~2009 Feynman Software.
 */

#ifndef FMNU_SEMAPHORE_H
#define FMNU_SEMAPHORE_H

#include <stddef.h>

#include "nucleus.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#ifndef NUPTH_LEN_NAME
#define NUPTH_LEN_NAME              12
#endif

typedef struct _sem_t
{
	NU_SEMAPHORE  nu_sem;
	char		  name [NUPTH_LEN_NAME];
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

#endif /* FMNU_SEMAPHORE_H */

/* End of pthread.h */
