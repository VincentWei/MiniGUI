/*
** $Id: sysvipc_private.h 7400 2007-08-20 05:57:07Z weiym $
** 
** sysvipc_private.h: types header for pthread mutex and semaphore on SysV IPC.
**
** Copyright (C) 2005 ~ 2007 Feynman Software
** All rights reserved.
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

