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
** timer.c: The Timer module for MiniGUI-Threads.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999/04/21
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "misc.h"
#include "timer.h"

#ifdef _MGRM_PROCESSES
#include "ourhdr.h"
#include "client.h"
#include "sharedres.h"
#endif

#if defined(_DEBUG) && defined(_MGRM_PROCESSES)
#define TIMER_ERR_SYS(text)         err_sys (text)
#else
#define TIMER_ERR_SYS(text)
#endif

DWORD __mg_timer_counter = 0;
static TIMER *timerstr[DEF_NR_TIMERS];

#ifdef _MGRM_THREADS
/* lock for protecting timerstr */
static pthread_mutex_t timerLock;
#define TIMER_LOCK()   pthread_mutex_lock(&timerLock)
#define TIMER_UNLOCK() pthread_mutex_unlock(&timerLock)
#else
#define TIMER_LOCK()
#define TIMER_UNLOCK()
#endif

#ifdef __LINUX__
#   define _MG_USE_BETTER_TIMER
#endif /* __LINUX__ */

#ifdef _MG_USE_BETTER_TIMER
#include <sys/times.h>
static clock_t g_timer_started;
static clock_t g_last_tick;
#endif

/* timer action for minigui timers */
static void __mg_timer_action (void *data)
{
#ifdef _MGRM_PROCESSES
    SHAREDRES_TIMER_COUNTER += 1;
#else

#if defined(__uClinux__) && defined(_MGRM_STANDALONE)
    __mg_timer_counter += 10;
#else
#   ifdef _MG_USE_BETTER_TIMER
    __mg_timer_counter = times(NULL) - g_timer_started;
#   else /* _MG_USE_BETTER_TIMER */
    __mg_timer_counter ++;
#   endif /* _MG_USE_BETTER_TIMER */
#endif

#endif

#ifdef _MGRM_THREADS
    /* alert desktop */
    AlertDesktopTimerEvent ();
#endif
}

/* timer entry for thread version */
#ifdef _MGRM_THREADS

#ifdef __AOS__

#include "os_api.h"

static OS_TIMER_ID __mg_os_timer = 0;

#else /* __AOS__ */
static void* TimerEntry (void* data)
{
    sem_post ((sem_t*)data);

#ifdef _MG_USE_BETTER_TIMER
    g_timer_started = times(NULL);
    g_last_tick = 0;
#endif /* _MG_USE_BETTER_TIMER */

    while (__mg_quiting_stage > _MG_QUITING_STAGE_TIMER) {
        __mg_os_time_delay (10);
        __mg_timer_action (NULL);
    }

    /* printf("quit from TimerEntry()\n"); */
    return NULL;
}
#endif /* !__AOS__ */

int __mg_timer_init (void)
{
    if (!mg_InitTimer ()) {
        fprintf (stderr, "KERNEL>timer: Init Timer failure.\n");
        return -1;
    }

#ifdef __AOS__
    __mg_os_timer = tp_os_timer_create ("mgtimer", __mg_timer_action, 
                                         NULL, AOS_TIMER_TICKT, 
                                         OS_AUTO_ACTIVATE | OS_AUTO_LOAD);
#else /* NOT __AOS__ */
    {
        sem_t wait;
        sem_init (&wait, 0, 0);
        pthread_create (&__mg_timer, NULL, TimerEntry, &wait);
        sem_wait (&wait);
        sem_destroy (&wait);
    }
#endif /* __AOS__ */

    return 0;
}

#else /* for MiniGUI-Processes and MiniGUI-Standalone */

#ifdef __NOUNIX__

BOOL mg_InstallIntervalTimer (void)
{
    return TRUE;
}

BOOL mg_UninstallIntervalTimer (void)
{
    return TRUE;
}

#else

#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

static struct sigaction old_alarm_handler;
static struct itimerval old_timer;

BOOL mg_InstallIntervalTimer (void)
{
    struct itimerval timerv;
    struct sigaction siga;
    
    sigaction (SIGALRM, NULL, &old_alarm_handler);

    siga = old_alarm_handler;
    siga.sa_handler = (void(*)(int))__mg_timer_action;
#ifndef _MGRM_STANDALONE
    siga.sa_flags = 0;
#else
    siga.sa_flags = SA_RESTART;
#endif
    sigaction (SIGALRM, &siga, NULL);

    timerv.it_interval.tv_sec = 0;
#if defined(__uClinux__) && defined(_MGRM_STANDALONE)
    timerv.it_interval.tv_usec = 100000;        /* 100 ms */
#else
    timerv.it_interval.tv_usec = 10000;         /* 10 ms */
#endif
    timerv.it_value = timerv.it_interval;

    if (setitimer (ITIMER_REAL, &timerv, &old_timer) == -1) {
        TIMER_ERR_SYS ("setitimer call failed!\n");
        return FALSE;
    }

    return TRUE;
}

BOOL mg_UninstallIntervalTimer (void)
{
    if (setitimer (ITIMER_REAL, &old_timer, 0) == -1) {
        TIMER_ERR_SYS ("setitimer call failed!\n");
        return FALSE;
    }

    if (sigaction (SIGALRM, &old_alarm_handler, NULL) == -1) {
        TIMER_ERR_SYS ("sigaction call failed!\n");
        return FALSE;
    }

    return TRUE;
}

#endif /* __NOUNIX__ */
#endif /* !_MGRM_THREADS */

BOOL mg_InitTimer (void)
{
#ifdef _MGRM_THREADS
    pthread_mutex_init (&timerLock, NULL);
    __mg_timer_counter = 0;
#else
    mg_InstallIntervalTimer ();
#endif

    return TRUE;
}

void mg_TerminateTimer (void)
{
    int i;

#ifdef __AOS__
    tp_os_timer_delete (__mg_os_timer);
#endif

#ifndef _MGRM_THREADS
    mg_UninstallIntervalTimer ();
#else
#   ifdef __WINBOND_SWLINUX__
    pthread_detach (__mg_timer); /* XXX: Can we pthread_join()? */
#   else
    pthread_join (__mg_timer, NULL);
#   endif /* __WINBOND_SWLINUX__ */
#endif

    for (i = 0; i < DEF_NR_TIMERS; i++) {
        if (timerstr[i] != NULL)
            free ( timerstr[i] );
        timerstr[i] = NULL;
    }

#ifdef _MGRM_THREADS
    pthread_mutex_destroy (&timerLock);
#endif
}

/************************* Functions run in desktop thread *******************/
void mg_dispatch_timer_message (DWORD inter)
{
    int i;

#ifdef _MG_USE_BETTER_TIMER
    clock_t now = __mg_timer_counter;

    inter = now - g_last_tick;
    g_last_tick = now;
#endif /* _MG_USE_BETTER_TIMER */

    TIMER_LOCK ();

    for (i=0; i<DEF_NR_TIMERS; i++) {
        if (timerstr[i] && timerstr[i]->msg_queue) {
            timerstr[i]->count += inter;
            if (timerstr[i]->count >= timerstr[i]->speed) {
#ifdef _MGRM_PROCESSES
                timerstr[i]->tick_count = SHAREDRES_TIMER_COUNTER;
#else
                timerstr[i]->tick_count = __mg_timer_counter;
#endif
                /* setting timer flag is simple, we do not need to lock msgq,
                   or else we may encounter dead lock here */ 
                SetMsgQueueTimerFlag (timerstr[i]->msg_queue, i);
                timerstr[i]->count -= timerstr[i]->speed;
            }
        }
    }

    TIMER_UNLOCK ();
}

/* thread safe, no mutex */
TIMER* __mg_get_timer (int slot)
{
    if (slot < 0 || slot >= DEF_NR_TIMERS)
        return NULL;

    return timerstr[slot];
}

#if 0
int __mg_get_timer_slot (HWND hWnd, int id)
{
    int i;
    int slot = -1;

    TIMER_LOCK ();
    for (i = 0; i < DEF_NR_TIMERS; i++) {
        if (timerstr[i] != NULL) {
            if (timerstr[i]->hWnd == hWnd && timerstr[i]->id == id) {
                slot = i;
                break;
            }
        }
    }

    TIMER_UNLOCK ();
    return slot;
}

void __mg_move_timer_last (TIMER* timer, int slot)
{
    if (slot < 0 || slot >= DEF_NR_TIMERS)
        return;

    TIMER_LOCK ();

    if (timer && timer->msg_queue) {
        /* The following code is already called in message.c...
         * timer->tick_count = 0;
         * timer->msg_queue->TimerMask &= ~(0x01 << slot);
         */

        if (slot != (DEF_NR_TIMERS - 1)) {
            TIMER* t;

            if (timer->msg_queue->TimerMask & (0x01 << (DEF_NR_TIMERS -1))) {
                timer->msg_queue->TimerMask |= (0x01 << slot);
                timer->msg_queue->TimerMask &= ~(0x01 << (DEF_NR_TIMERS -1));
            }

            t = timerstr [DEF_NR_TIMERS - 1];
            timerstr [DEF_NR_TIMERS - 1] = timerstr [slot];
            timerstr [slot] = t;
        }
    }

    TIMER_UNLOCK ();
    return;
}
#endif

BOOL GUIAPI SetTimerEx (HWND hWnd, LINT id, DWORD speed, 
                TIMERPROC timer_proc)
{
    int i;
    int slot = -1;
    PMSGQUEUE pMsgQueue;

    if (id == 0) {
        _MG_PRINTF ("KERNEL>Timer: bad identifier (%ld).\n", id);
        return FALSE;
    }

    if (speed == 0) {
        speed = 1;
    }

#ifdef _MGRM_THREADS
    if (!(pMsgQueue = GetMsgQueueThisThread ())) {
        _MG_PRINTF ("KERNEL>Timer: Not a GUI thread.\n");
        return FALSE;
    }
#else
    pMsgQueue = __mg_dsk_msg_queue;
#endif

    TIMER_LOCK ();

    /* Is there an empty timer slot? */
    for (i=0; i<DEF_NR_TIMERS; i++) {
        if (timerstr[i] == NULL) {
            if (slot < 0)
                slot = i;
        }
        else if (timerstr[i]->hWnd == hWnd && timerstr[i]->id == id) {
            _MG_PRINTF ("KERNEL>Timer: duplicated call to SetTimerEx (%p, %ld).\n", hWnd, id);
            goto badret;
        }
    }

    if (slot < 0 || slot == DEF_NR_TIMERS) {
        _MG_PRINTF ("KERNEL>Timer: No more slot for new timer (total: %d)\n", DEF_NR_TIMERS);
        goto badret;
    }

    timerstr[slot] = malloc (sizeof (TIMER));

    timerstr[slot]->speed = speed;
    timerstr[slot]->hWnd = hWnd;
    timerstr[slot]->id = id;
    timerstr[slot]->count = 0;
    timerstr[slot]->proc = timer_proc;
    timerstr[slot]->tick_count = 0;
    timerstr[slot]->msg_queue = pMsgQueue;

#ifdef _MGRM_PROCESSES
    if (!mgIsServer)
        __mg_set_select_timeout (USEC_10MS * speed);
#endif

    TIMER_UNLOCK ();
    return TRUE;
    
badret:
    TIMER_UNLOCK ();
    return FALSE;
}

#ifdef _MGRM_PROCESSES
static void reset_select_timeout (void)
{
    int i;

    unsigned int speed = 0;
    for (i=0; i<DEF_NR_TIMERS; i++) {
        if (timerstr[i]) {
            if (speed == 0 || timerstr[i]->speed < speed)
                speed = timerstr[i]->speed;
        }
    }
    __mg_set_select_timeout (USEC_10MS * speed);
}

#endif

void __mg_remove_timer (TIMER* timer, int slot)
{
    if (slot < 0 || slot >= DEF_NR_TIMERS)
        return;

    TIMER_LOCK ();

    if (timer && timer->msg_queue && timerstr[slot] == timer) {
        /* The following code is already moved to message.c...
         * timer->msg_queue->TimerMask &= ~(0x01 << slot);
         */
        free (timerstr[slot]);
        timerstr [slot] = NULL;

#ifdef _MGRM_PROCESSES
        if (!mgIsServer)
            reset_select_timeout ();
#endif
    }


    TIMER_UNLOCK ();
    return;
}

void mg_remove_timers_by_msg_queue (const MSGQUEUE* msg_que)
{
    int i;

    TIMER_LOCK ();

    for (i=0; i<DEF_NR_TIMERS; i++) {
        if (timerstr [i] && timerstr[i]->msg_queue == msg_que) {
            free (timerstr[i]);
            timerstr [i] = NULL;
        }
    }

    TIMER_UNLOCK ();
}

/* If id == 0, clear all timers of the window */
int GUIAPI KillTimer (HWND hWnd, LINT id)
{
    int i;
    PMSGQUEUE pMsgQueue;
    int killed = 0;

#ifdef _MGRM_THREADS
    if (!(pMsgQueue = GetMsgQueueThisThread ()))
        return 0;
#else
    pMsgQueue = __mg_dsk_msg_queue;
#endif

    TIMER_LOCK ();

    for (i = 0; i < DEF_NR_TIMERS; i++) {
        if ((timerstr [i] && timerstr [i]->hWnd == hWnd) && 
                        (id == 0 || timerstr [i]->id == id)) {
            RemoveMsgQueueTimerFlag (pMsgQueue, i);
            free (timerstr[i]);
            timerstr [i] = NULL;
            killed ++;

            if (id) break;
        }
    }

#ifdef _MGRM_PROCESSES
    if (!mgIsServer && killed)
        reset_select_timeout ();
#endif

    TIMER_UNLOCK ();
    return killed;
}

BOOL GUIAPI ResetTimerEx (HWND hWnd, LINT id, DWORD speed, 
                TIMERPROC timer_proc)
{
    int i;
    PMSGQUEUE pMsgQueue;

    if (id == 0)
        return FALSE;

#ifdef _MGRM_THREADS
    if (!(pMsgQueue = GetMsgQueueThisThread ()))
        return FALSE;
#else
    pMsgQueue = __mg_dsk_msg_queue;
#endif

    TIMER_LOCK ();

    for (i = 0; i < DEF_NR_TIMERS; i++) {
      if (timerstr[i] && timerstr[i]->hWnd == hWnd && timerstr[i]->id == id) {
        /* Should clear old timer flags */
        RemoveMsgQueueTimerFlag (pMsgQueue, i);
        timerstr[i]->speed = speed;
        timerstr[i]->count = 0;
        if (timer_proc != (TIMERPROC)INV_PTR)
            timerstr[i]->proc = timer_proc;
        timerstr[i]->tick_count = 0;

        TIMER_UNLOCK ();
        return TRUE;
      }
    }

    TIMER_UNLOCK ();
    return FALSE;
}

BOOL GUIAPI IsTimerInstalled (HWND hWnd, LINT id)
{
    int i;

    if (id == 0)
        return FALSE;

    TIMER_LOCK ();

    for (i = 0; i < DEF_NR_TIMERS; i++) {
        if ( timerstr[i] != NULL ) {
            if ( timerstr[i]->hWnd == hWnd && timerstr[i]->id == id) {
                TIMER_UNLOCK ();
                return TRUE;
            }
        }
    }

    TIMER_UNLOCK ();
    return FALSE;
}

/* no lock is ok */
BOOL GUIAPI HaveFreeTimer (void)
{
    int i;

    for (i=0; i<DEF_NR_TIMERS; i++) {
        if (timerstr[i] == NULL)
            return TRUE;
    }

    return FALSE;
}

DWORD GUIAPI GetTickCount (void)
{
#ifdef _MGRM_PROCESSES
    return SHAREDRES_TIMER_COUNTER;
#else
    return __mg_timer_counter;
#endif
}

