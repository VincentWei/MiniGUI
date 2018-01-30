/*
 * \file threadx_semaphore.h
 * \author Yan Xiaowei <xwyan@minigui.org>
 * \date 2005/01/11
 * 
 * \brief This is the POSIX semaphore implementation in order to 
 *        run MiniGUI on ThreadX.
 *
 \verbatim

    This header contains all the definitions needed to support
    POSIX semaphore under ThreadX. The reader is referred to the POSIX
    standard or equivalent documentation for details of the
    functionality contained herein.
 
    We do this work in order to run MiniGUI on ThreadX.

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2005~2018, Beijing FMSoft Technologies Co., Ltd.

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
 * $Id: threadx_semaphore.h 11349 2009-03-02 05:00:43Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
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
