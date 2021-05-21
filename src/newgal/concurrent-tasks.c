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
 *   Copyright (C) 2021, Beijing FMSoft Technologies Co., Ltd.
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
** concurrent-tasks.c:
**  This file implements the concurrent tasks by using POSIX threads.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2021/05/21
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "common.h"

#ifdef _MGRM_PROCESSES

#include "minigui.h"
#include "constants.h"
#include "newgal.h"

#include <pthread.h>
#include <semaphore.h>

#include "concurrent-tasks.h"

typedef void (*task_proc)(void* context, int loop_idx);

typedef struct _ConcurrentTasksInfo {
    pthread_t pths[_MGNR_CONCURRENT_TASKS];
    sem_t sem_loop;
    sem_t sem_lock;
    sem_t sem_sync;

    int         nr_loops;
    CCTaskProc  proc;
    void*       ctxt;
} ConcurrentTasksInfo;

static ConcurrentTasksInfo *cctasks_info;

static inline int cc_sem_wait (sem_t *sem)
{
try_again:
    if (sem_wait (sem) < 0) {
        if (errno == EINTR)
            goto try_again;
        else {
            _WRN_PRINTF ("Failed sem_wait: %s\n", strerror (errno));
            return -1;
        }
    }

    return 0;
}

static void* cc_task_entry (void* data)
{
    int task_idx = (int)(intptr_t)data;

    if (pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL))
        return NULL;

    do {
        int loop_idx;

        if (cc_sem_wait (&cctasks_info->sem_loop)) {
            _ERR_PRINTF ("CCTasks: thread %d failed on sem_wait\n", task_idx);
            return NULL;
        }

        cc_sem_wait (&cctasks_info->sem_lock);
        loop_idx = cctasks_info->nr_loops++;
        sem_post (&cctasks_info->sem_lock);

        pthread_testcancel ();

        cctasks_info->proc (cctasks_info->ctxt, loop_idx);

        if (sem_post (&cctasks_info->sem_sync)) {
            _ERR_PRINTF ("CCTasks: thread %d failed on sem_post\n", task_idx);
            return NULL;
        }

        pthread_testcancel ();

    } while (1);

    return NULL;
}

int concurrentTasks_SplitRect (GAL_Rect* rcs, const GAL_Rect* rc, int count)
{
    if (count == 1) {
        rcs[0] = *rc;
    }
    else if (count == 2) {
        rcs[0].x = rc->x;
        rcs[0].y = rc->y;
        rcs[0].w = rc->w;
        rcs[0].h = rc->h >> 1;

        rcs[1].x = rc->x;
        rcs[1].y = rcs[0].y + rcs[0].h;
        rcs[1].w = rc->w;
        rcs[1].h = rc->h - rcs[0].h;
    }
    else if (count == 4) {
        rcs[0].x = rc->x;
        rcs[0].y = rc->y;
        rcs[0].w = rc->w;
        rcs[0].h = rc->h >> 2;

        rcs[1].x = rc->x;
        rcs[1].y = rcs[0].y + rcs[0].h;
        rcs[1].w = rc->w;
        rcs[1].h = rc->h >> 2;

        rcs[2].x = rc->x;
        rcs[2].y = rcs[1].y + rcs[1].h;
        rcs[2].w = rc->w;
        rcs[2].h = rc->h >> 2;

        rcs[3].x = rc->x;
        rcs[3].y = rcs[2].y + rcs[2].h;
        rcs[3].w = rc->w;
        rcs[3].h = rc->h - rcs[0].h * 3;
    }
    else if (count == 8) {
        rcs[0].x = rc->x;
        rcs[0].y = rc->y;
        rcs[0].w = rc->w;
        rcs[0].h = rc->h >> 3;

        rcs[1].x = rc->x;
        rcs[1].y = rcs[0].y + rcs[0].h;
        rcs[1].w = rc->w;
        rcs[1].h = rc->h >> 3;

        rcs[2].x = rc->x;
        rcs[2].y = rcs[1].y + rcs[1].h;
        rcs[2].w = rc->w;
        rcs[2].h = rc->h >> 3;

        rcs[3].x = rc->x;
        rcs[3].y = rcs[2].y + rcs[2].h;
        rcs[3].w = rc->w;
        rcs[3].h = rc->h >> 3;

        rcs[4].x = rc->x;
        rcs[4].y = rcs[3].y + rcs[3].h;
        rcs[4].w = rc->w;
        rcs[4].h = rc->h >> 3;

        rcs[5].x = rc->x;
        rcs[5].y = rcs[4].y + rcs[4].h;
        rcs[5].w = rc->w;
        rcs[5].h = rc->h >> 3;

        rcs[6].x = rc->x;
        rcs[6].y = rcs[5].y + rcs[5].h;
        rcs[6].w = rc->w;
        rcs[6].h = rc->h >> 3;

        rcs[7].x = rc->x;
        rcs[7].y = rcs[6].y + rcs[6].h;
        rcs[7].w = rc->w;
        rcs[7].h = rc->h - rcs[0].h * 7;
    }
    else {
        return -1;
    }

    return 0;
}

int concurrentTasks_Do (void* context, CCTaskProc proc)
{
    int i;

    if (_MGNR_CONCURRENT_TASKS > 0) {
        cctasks_info->nr_loops = 1; // reserve 0 for the current thread
        cctasks_info->proc = proc;
        cctasks_info->ctxt = context;
    }

    // wake up the concurrent task threads
    for (i = 0; i < _MGNR_CONCURRENT_TASKS; i++) {
        sem_post (&cctasks_info->sem_loop);
    }

    proc (context, 0);

    // wait for finish of concurrent task threads
    for (i = 0; i < _MGNR_CONCURRENT_TASKS; i++) {
        if (cc_sem_wait (&cctasks_info->sem_sync) < 0)
            return -1;
    }

    return 0;
}

int concurrentTasks_Init (void)
{
    int i;

    if (_MGNR_CONCURRENT_TASKS <= 0)
        return 0;

    if ((cctasks_info = calloc (1, sizeof (ConcurrentTasksInfo))) == NULL) {
        _ERR_PRINTF ("CCTasks: failed to allocate memory\n");
        goto failed_sem_loop;
    }

    if (sem_init (&cctasks_info->sem_loop, 0, 0)) {
        _ERR_PRINTF ("CCTasks: failed to create loop semaphore: %s\n",
                strerror (errno));
        goto failed_sem_loop;
    }

    if (sem_init (&cctasks_info->sem_sync, 0, 0)) {
        _ERR_PRINTF ("CCTasks: failed to create sync semaphore: %s\n",
                strerror (errno));
        goto failed_sem_sync;
    }

    if (sem_init (&cctasks_info->sem_lock, 0, 1)) {
        _ERR_PRINTF ("CCTasks: failed to create lock semaphore: %s\n",
                strerror (errno));
        goto failed_sem_lock;
    }

    for (i = 0; i < _MGNR_CONCURRENT_TASKS; i++) {
        if (pthread_create (cctasks_info->pths + i, NULL, cc_task_entry,
                    (void*)(intptr_t)(i + 1))) {
            _ERR_PRINTF ("CCTasks: failed to create update thread (%d): %s\n",
                    i, strerror (errno));
            goto failed_threads;
        }
    }

    return 0;

failed_threads:
    sem_destroy (&cctasks_info->sem_lock);

failed_sem_lock:
    sem_destroy (&cctasks_info->sem_sync);

failed_sem_sync:
    sem_destroy (&cctasks_info->sem_loop);

failed_sem_loop:
    return -1;
}

int concurrentTasks_Term (void)
{
    int i;

    if (_MGNR_CONCURRENT_TASKS <= 0 && cctasks_info == NULL)
        return 0;

    for (i = 0; i < _MGNR_CONCURRENT_TASKS; i++) {
        _WRN_PRINTF ("Cancelling concurrent task thread: %d\n", i);
        /* send cancel request */
        pthread_cancel (cctasks_info->pths [i]);
        pthread_join (cctasks_info->pths [i], NULL);
    }

    sem_destroy (&cctasks_info->sem_sync);
    sem_destroy (&cctasks_info->sem_loop);
    sem_destroy (&cctasks_info->sem_lock);

    free (cctasks_info);
    return 0;
}

#endif  /* _MGRM_PROCESSES */

