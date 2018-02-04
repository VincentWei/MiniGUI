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
** sysvipc_private.h: types header for pthread mutex and semaphore on SysV IPC.
**
** Current maintainer: Wei Yongming
**
** Create date: 2005-01-05
*/

#ifndef _SYSVIPC_PRIVATE_H
#define _SYSVIPC_PRIVATE_H

#if 0
union semun {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

//-----------------------------------------------------------------------------
// Global data structures

#define SYSVIPC_PTH_DEFNR_SEMS            64
#define SYSVIPC_PTH_LEN_BMP               (SYSVIPC_PTH_DEFNR_SEMS/8)

typedef struct _sem_set_info
{
    int lock_num;
    int semid;
    unsigned char usage_bmp [SYSVIPC_PTH_LEN_BMP];
} sem_set_info;

/* Create a SysV semaphore set. */
int _sysvipc_pth_create_sem_set (sem_set_info* ssi, key_t sem_key, int lock_num);

/* Destroy a SysV semapore set. */
int _sysvipc_pth_destroy_sem_set (sem_set_info* ssi);

/* Get a free semaphore from the sem set. */
int _sysvipc_pth_get_free_sem (sem_set_info* ssi);

/* Free a semaphore in the sem set. */
int _sysvipc_pth_free_sem (sem_set_info* ssi, int sem_num);

/* Operate a semaphore uninterruptablly. */
void _sysvipc_pth_sem_op (int semid, int sem_num, int value);

#define KEY_SYS_SEM_SET     0x464D4700
#define KEY_MUTEX_SET       0x464D4701
#define KEY_SEM_SET         0x464D4702

extern int _sysvipc_pth_sys_semid;
#define _SYSVIPC_PTH_NUM_MUTEX_SET      0
#define _SYSVIPC_PTH_NUM_SEM_SET        1

extern sem_set_info* _sysvipc_pth_mutex_set;
extern sem_set_info* _sysvipc_pth_sem_set;

/* lock the internal data. */
inline static void _sysvipc_pth_lock (int which)
{
    if (which >= 0)
    	_sysvipc_pth_sem_op (_sysvipc_pth_sys_semid, which, -1);
}

/* unlock the internal data. */
inline static void _sysvipc_pth_unlock (int which)
{
    if (which >= 0)
    	_sysvipc_pth_sem_op (_sysvipc_pth_sys_semid, which, 1);
}

#endif /* _SYSVIPC_PRIVATE_H */

