///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** sem-manager->c: the System V semaphore manager module for MiniGUI.
**
** Create date: 2020-01-08
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"

#ifdef _MGRM_PROCESSES

#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ourhdr.h"
#include "misc.h"

struct _SemSetManager {
    int             semid;
    int             pre_created;
    int             nr_sems;
    int             len_use_bmp;
    BYTE*           use_bmp;
#ifdef _MGUSE_THREADS
    pthread_mutex_t lock;
#endif
};

SemSetManager* __mg_create_sem_set_manager (int semid, int nr_sems)
{
    SemSetManager* manager = NULL;

    if (nr_sems <= 0) {
        return NULL;
    }

    manager = calloc (sizeof (SemSetManager), 1);
    if (manager == NULL) {
        return NULL;
    }

    if (semid < 0) {
        /* create an anonymous semaphore set */
        manager->semid = semget (IPC_PRIVATE, nr_sems,
                0666 | IPC_CREAT | IPC_EXCL);
        if (manager->semid < 0) {
            _ERR_PRINTF ("SEMSETMANAGER: cannot create anonymous semaphore set.\n");
            goto failed;
        }
        manager->pre_created = 0;
    }
    else {
        manager->semid = semid;
        manager->pre_created = 1;
    }

    manager->len_use_bmp = (nr_sems + 7) >> 3;
    manager->use_bmp = malloc (manager->len_use_bmp);
    if (manager->use_bmp == NULL) {
        _ERR_PRINTF ("SEMSETMANAGER: cannot allocate use bitmap.\n");
        goto failed;
    }

    manager->nr_sems = nr_sems;
    memset (manager->use_bmp, 0xFF, manager->len_use_bmp);

#ifdef _MGUSE_THREADS
    pthread_mutex_init (&manager->lock, NULL);
#endif

    return manager;

failed:
    if (manager->use_bmp) {
        free (manager->use_bmp);
    }

    if (manager->semid >= 0 && manager->pre_created == 0) {
        union semun ignored;
        if (semctl (manager->semid, 0, IPC_RMID, ignored) < 0) {
            _WRN_PRINTF ("SEMSETMANAGER: failed to remove semaphore set.\n");
        }
    }

#ifdef _MGUSE_THREADS
    if (manager->nr_sems > 0)
        pthread_mutex_destroy (&manager->lock);
#endif

    return NULL;
}

/* use void* in order to use on_exit */
void __mg_delete_sem_set_manager (int code, void* data)
{
    SemSetManager* manager = (SemSetManager*)data;

    if (manager == NULL || manager->nr_sems == 0) {
        _ERR_PRINTF ("SEMSETMANAGER: bad manager or not initialized.\n");
        return;
    }

    free (manager->use_bmp);

    if (manager->semid >= 0 && manager->pre_created == 0) {
        union semun ignored;
        if (semctl (manager->semid, 0, IPC_RMID, ignored) < 0) {
            _WRN_PRINTF ("SEMSETMANAGER: failed to remove semaphore set.\n");
        }
    }

#ifdef _MGUSE_THREADS
    pthread_mutex_destroy (&manager->lock);
#endif

    free (manager);
}

int __mg_alloc_mutual_sem (SemSetManager* manager, int *semid)
{
    int n;
    union semun sunion;

#ifdef _MGUSE_THREADS
    pthread_mutex_lock (&manager->lock);
#endif

    n = __mg_lookfor_unused_slot (manager->use_bmp, manager->len_use_bmp, 1);
    if (n >= manager->nr_sems) {
        n = -1;
    }

    if (n >= 0 && semid) {
        *semid = manager->semid;
    }

    sunion.val = 1;
    semctl (manager->semid, n, SETVAL, sunion);

#ifdef _MGUSE_THREADS
    pthread_mutex_unlock (&manager->lock);
#endif

    return n;
}

int __mg_free_mutual_sem (SemSetManager* manager, int sem_num)
{
    if (sem_num < 0 || sem_num >= manager->nr_sems) {
        _DBG_PRINTF("Bad semaphore number: %d\n", sem_num);
        return -1;
    }

#ifdef _MGUSE_THREADS
    pthread_mutex_lock (&manager->lock);
#endif

    if (!__mg_slot_clear_use (manager->use_bmp, sem_num)) {
        _WRN_PRINTF("The semaphore is not marked as used: %d\n", sem_num);
        return -1;
    }

#ifdef _MGUSE_THREADS
    pthread_mutex_unlock (&manager->lock);
#endif

    return 0;
}

#endif /* _MGRM_PROCESSES */

