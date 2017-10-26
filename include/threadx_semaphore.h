/*
 * \file threadx_semaphore.h
 * \author Yan Xiaowei <xwyan@minigui.org>
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

    Copyright (C) 2005-2012 FMSoft.

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
 * $Id: threadx_semaphore.h 11349 2009-03-02 05:00:43Z weiym $
 *
 * This is the POSIX semaphore implementation in order to run MiniGUI on ThreadX.
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2005-2012 FMSoft.
 */

#ifndef FMTX_SEMAPHORE_H
#define FMTX_SEMAPHORE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct TXPTH_SEMAPHORE_STRUCT
{
    unsigned long tx_semaphore_id;
    char* tx_semaphore_name;
    unsigned long tx_semaphore_count;
    void *tx_semaphore_suspension_list;
    unsigned long tx_semaphore_suspended_count;
    void *tx_semaphore_created_next;
    void *tx_semaphore_created_previous;
} TXPTH_SEMAPHORE;

typedef struct _sem_t
{
	TXPTH_SEMAPHORE  tx_sem;
	char		  name [TXPTH_LEN_NAME];
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

#endif /* FMTX_SEMAPHORE_H */

/* End of threadx_semaphore.h */
