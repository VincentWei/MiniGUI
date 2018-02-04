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
** pprivate.c: implementation of internal functions for uCPThreads
**
** Current maintainer: Wei Yongming
**
** Create date: 2005-01-05
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "common.h"

#if defined (_USE_MUTEX_ON_SYSVIPC) || defined (_USE_SEM_ON_SYSVIPC)

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sched.h>

#include "sysvipc_private.h"

// -------------------------------------------------------------------------
// Internal definitions

#define _MACRO_START   do {
#define _MACRO_END     } while (0);

/* Create a SysV semaphore set. */
int _sysvipc_pth_create_sem_set (sem_set_info* ssi, key_t sem_key, int lock_num)
{
    ssi->semid = semget (sem_key, SYSVIPC_PTH_DEFNR_SEMS, 0666 | IPC_CREAT | IPC_EXCL);
    if (ssi->semid < 0)
        return -1;

    ssi->lock_num = lock_num;
    memset (ssi->usage_bmp, 0xFF, SYSVIPC_PTH_LEN_BMP);
    return 0;
}

/* Destroy a SysV semapore set. */
int _sysvipc_pth_destroy_sem_set (sem_set_info* ssi)
{
    union semun ignored;
    if (semctl (ssi->semid, 0, IPC_RMID, ignored) < 0)
        return -1;

    return 0;
}

static int _lookfor_unused_slot (unsigned char* bitmap, int len_bmp, int set)
{
    int unused = 0;
    int i, j;

    for (i = 0; i < len_bmp; i++) {
        for (j = 0; j < 8; j++) {
            if (*bitmap & (0x80 >> j)) {
                if (set)
                    *bitmap &= (~(0x80 >> j));
                return unused + j;
            }
        }

        unused += 8;
        bitmap++;
    }

    return -1;
}

static void _slot_set_use (unsigned char* bitmap, int index)
{
    bitmap += index >> 3;
    *bitmap &= (~(0x80 >> (index % 8)));
}

static int _slot_clear_use (unsigned char* bitmap, int index)
{
    bitmap += index >> 3;
    if (*bitmap & (0x80 >> (index % 8)))
        return 0;

    *bitmap |= (0x80 >> (index % 8));
    return 1;
}

/* Get a free semaphore from the sem set. */
int _sysvipc_pth_get_free_sem (sem_set_info* ssi)
{
    return _lookfor_unused_slot (ssi->usage_bmp, SYSVIPC_PTH_LEN_BMP, 1);
}

/* Free a semaphore in the sem set. */
int _sysvipc_pth_free_sem (sem_set_info* ssi, int sem_num)
{
    return _slot_clear_use (ssi->usage_bmp, sem_num);
}

/* Down/up a semaphore uninterruptablly. */
void _sysvipc_pth_sem_op (int semid, int sem_num, int value)
{
    struct sembuf sb;

again:
    sb.sem_num = sem_num;
    sb.sem_op = value;
    sb.sem_flg = 0;

    if (semop (semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

int _sysvipc_pth_sys_semid;
sem_set_info* _sysvipc_pth_mutex_set;
sem_set_info* _sysvipc_pth_sem_set;
static sem_set_info __sysvipc_pth_mutex_set;
static sem_set_info __sysvipc_pth_sem_set;

int _sysvipc_mutex_sem_init (void)
{
    int semid;
    union semun sunion;

    semid = semget (KEY_SYS_SEM_SET, 1, 0);
    if (semid != -1)
        semctl (semid, 0, IPC_RMID);
    semid = semget (KEY_MUTEX_SET, 1, 0);
    if (semid != -1)
        semctl (semid, 0, IPC_RMID);
    semid = semget (KEY_SEM_SET, 1, 0);
    if (semid != -1)
        semctl (semid, 0, IPC_RMID);

    _sysvipc_pth_sys_semid = semget (KEY_SYS_SEM_SET, 2, 0666 | IPC_CREAT | IPC_EXCL);
    if (_sysvipc_pth_sys_semid == -1) {
        perror ("Can not create system semaphore set");
        goto error;
    }

    sunion.val = 1;
    semctl (_sysvipc_pth_sys_semid, _SYSVIPC_PTH_NUM_MUTEX_SET, SETVAL, sunion);
    semctl (_sysvipc_pth_sys_semid, _SYSVIPC_PTH_NUM_SEM_SET, SETVAL, sunion);

    _sysvipc_pth_mutex_set = &__sysvipc_pth_mutex_set;
    _sysvipc_pth_sem_set = &__sysvipc_pth_sem_set;

    if (_sysvipc_pth_create_sem_set (_sysvipc_pth_mutex_set, KEY_MUTEX_SET, _SYSVIPC_PTH_NUM_MUTEX_SET)) {
        perror ("Can not create _sysvipc_pth_mutex_set");
        goto error;
    }

    if (_sysvipc_pth_create_sem_set (_sysvipc_pth_sem_set, KEY_SEM_SET, _SYSVIPC_PTH_NUM_SEM_SET)) {
        perror ("Can not create _sysvipc_pth_sem_set");
        goto error;
    }

    return 0;

error:
    fprintf (stderr, "Mutex/Semaphore on SysV IPC: fatal error on _sysvipc_mutex_sem_init.\n");
    return -1;
}

int _sysvipc_mutex_sem_term (void)
{
    semctl (_sysvipc_pth_sys_semid, 0, IPC_RMID);
    semctl (__sysvipc_pth_mutex_set.semid, 0, IPC_RMID);
    semctl (__sysvipc_pth_sem_set.semid, 0, IPC_RMID);

    return 0;
}

#endif /* (_USE_MUTEX_ON_SYSVIPC) || defined (_USE_SEM_ON_SYSVIPC) */


