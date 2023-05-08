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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
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
#endif /* defined _MGRM_PROCESSES */

#if defined(_DEBUG) && defined(_MGRM_PROCESSES)
#define TIMER_ERR_SYS(text)         __mg_err_sys (text)
#else
#define TIMER_ERR_SYS(text)
#endif

DWORD __mg_tick_counter = 0;

/* update timer count for desktop thread */
void __mg_update_tick_count (void *data)
{
#if defined(_MGRM_PROCESSES)
    if (mgIsServer) {
        __mg_tick_counter = __mg_os_get_time_ticks ();
        SHAREDRES_TIMER_COUNTER = __mg_tick_counter;
    }
    else {
        __mg_tick_counter = SHAREDRES_TIMER_COUNTER;
    }
#else   /* defined _MGRM_PROCESSES */
    __mg_tick_counter = __mg_os_get_time_ticks ();
#endif  /* not defined _MGRM_PROCESSES */

    /* Since 5.0.0, the desktop only handles caret blinking in MSG_TIMEOUT
       message, and the interval for the timer of desktop changes to 0.05s. */
    if (__mg_tick_counter >
            __mg_dsk_msg_queue->last_ticks_desktop + DESKTOP_TIMER_INERTVAL) {
        __mg_dsk_msg_queue->dwState |= QS_DESKTIMER;
#ifdef _MGRM_THREADS    /* only wake up desktop for threads mode */
        POST_MSGQ (__mg_dsk_msg_queue);
#endif
        __mg_dsk_msg_queue->last_ticks_desktop = __mg_tick_counter;
    }

#if 0  /* deprecated code */
#if defined(_MGRM_PROCESSES)
    if (mgIsServer) {
        DWORD elapsed_ticks;

        /* Since 5.0.0, we use elapsed time in ms to count the ticks */
        elapsed_ticks = __mg_os_get_elapsed_ms ();
        elapsed_ticks = (elapsed_ticks + 5) / 10;

        __mg_tick_counter += elapsed_ticks;
        SHAREDRES_TIMER_COUNTER = __mg_tick_counter;
    }
    else {
        __mg_tick_counter = SHAREDRES_TIMER_COUNTER;
    }
#else   /* defined _MGRM_PROCESSES */
    DWORD elapsed_ticks;

    /* Since 5.0.0, we use elapsed time in ms to count the ticks */
    elapsed_ticks = __mg_os_get_elapsed_ms ();
    elapsed_ticks = (elapsed_ticks + 5) / 10;
    __mg_tick_counter += elapsed_ticks;
#endif  /* not defined _MGRM_PROCESSES */

    /* Since 5.0.0, the desktop only handles caret blinking in MSG_TIMEOUT
       message, and the interval for the timer of desktop changes to 0.05s. */
    if (__mg_tick_counter >
            __mg_dsk_msg_queue->last_ticks_desktop + DESKTOP_TIMER_INERTVAL) {
        __mg_dsk_msg_queue->dwState |= QS_DESKTIMER;
#ifdef _MGRM_THREADS    /* only wake up desktop for threads mode */
        POST_MSGQ (__mg_dsk_msg_queue);
#endif
        __mg_dsk_msg_queue->last_ticks_desktop = __mg_tick_counter;
    }
#endif  /* deprecated code */
}

#ifdef __NOUNIX__

static BOOL install_system_timer (void)
{
    return TRUE;
}

static BOOL unintall_system_timer (void)
{
    return TRUE;
}

#else   /* defined __NOUNIX__ */

#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

static struct sigaction old_alarm_handler;
static struct itimerval old_timer;

static BOOL install_system_timer (void)
{
    struct itimerval timerv;
    struct sigaction siga;

    sigaction (SIGALRM, NULL, &old_alarm_handler);

    siga = old_alarm_handler;
    siga.sa_handler = (void(*)(int))__mg_update_tick_count;
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

static BOOL unintall_system_timer (void)
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

#endif /* not defined __NOUNIX__ */

BOOL mg_InitTimer (BOOL use_sys_timer)
{
    __mg_tick_counter = 0;

    __mg_os_start_time();

    if (use_sys_timer) {
        install_system_timer ();
    }

    return TRUE;
}

void mg_TerminateTimer (BOOL use_sys_timer)
{
    if (use_sys_timer)
        unintall_system_timer ();
}

/************************* Functions run in message thread *******************/
int __mg_check_expired_timers (MSGQUEUE* msg_queue, DWORD inter)
{
    int nr = 0;

    if (inter == 0)
        return 0;

    if (msg_queue == NULL) {
        msg_queue = getMsgQueueForThisThread ();
    }

    if (msg_queue) {
        int i;
        TIMER** timer_slots = msg_queue->timer_slots;

        for (i = 0; i < DEF_NR_TIMERS; i++) {
            if (timer_slots[i]) {
                if (__mg_tick_counter >= timer_slots[i]->ticks_expected) {
                    /* setting timer flag is simple, we do not need to lock
                       msgq, or else we may encounter dead lock here */
                    msg_queue->expired_timer_mask |= (0x01UL << i);
                    POST_MSGQ (msg_queue);

                    timer_slots[i]->ticks_expected += timer_slots[i]->interv;
                    timer_slots[i]->ticks_fired = __mg_tick_counter;
                    nr++;
                }
#if 0   /* deprecated code */
                timer_slots[i]->count += inter;
                if (timer_slots[i]->count >= timer_slots[i]->interv) {
#ifdef _MGRM_PROCESSES
                    timer_slots[i]->ticks_current = SHAREDRES_TIMER_COUNTER;
#else
                    timer_slots[i]->ticks_current = __mg_tick_counter;
#endif
                    /* setting timer flag is simple, we do not need to lock msgq,
                       or else we may encounter dead lock here */
                    msg_queue->expired_timer_mask |= (0x01UL << i);
                    POST_MSGQ (msg_queue);
                    timer_slots[i]->count -= timer_slots[i]->interv;
                    nr++;
                }
#endif /* deprecated code */
            }
        }
    }
    else {
        _WRN_PRINTF ("called for non message thread\n");
    }

    return nr;
}

BOOL GUIAPI SetTimerEx (HWND hWnd, LINT id, DWORD interv,
                TIMERPROC timer_proc)
{
    int i;
    int slot = -1;
    TIMER** timer_slots;
    PMSGQUEUE pMsgQueue;

#ifndef _MGRM_THREADS
    /* Force to update tick count */
    GetTickCount();
#endif

    if (id == 0) {
        _WRN_PRINTF ("bad identifier (%ld).\n", id);
        return FALSE;
    }

    if (interv == 0) {
        interv = 1;
    }

    timer_slots = getTimerSlotsForThisThread (&pMsgQueue);
    if (MG_UNLIKELY (timer_slots == NULL)) {
        _WRN_PRINTF ("called for non message thread\n");
        goto badret;
    }

    /* Since 5.0.0: only check hWnd if timer_proc is NULL */
    if (MG_UNLIKELY (timer_proc == NULL &&
                !getMainWinIfWindowInThisThread (hWnd))) {
        _WRN_PRINTF ("called for a window not in current thread\n");
        assert (0);
        goto badret;
    }

    /* Is there an empty timer slot? */
    for (i = 0; i < DEF_NR_TIMERS; i++) {
        if (timer_slots[i] == NULL) {
            if (slot < 0)
                slot = i;
        }
        else if (timer_slots[i]->hWnd == hWnd && timer_slots[i]->id == id) {
            /* Since 5.0.0: we reset timer parameters for duplicated call of
               this function */
            timer_slots[i]->interv = interv;
            timer_slots[i]->ticks_expected = __mg_tick_counter + interv;
            timer_slots[i]->ticks_fired = 0;
            timer_slots[i]->proc = timer_proc;
            return TRUE;
        }
    }

    if (slot < 0 || slot == DEF_NR_TIMERS) {
        _WRN_PRINTF ("no more slot for new timer (total: %d)\n", DEF_NR_TIMERS);
        goto badret;
    }

    timer_slots[slot] = mg_slice_new (TIMER);

    timer_slots[slot]->hWnd = hWnd;
    timer_slots[slot]->id = id;
    timer_slots[slot]->interv = interv;
    timer_slots[slot]->ticks_expected = __mg_tick_counter + interv;
    timer_slots[slot]->ticks_fired = 0;
    timer_slots[slot]->proc = timer_proc;

    _DBG_PRINTF ("ticks_expected (%d): %lu, tick_counter: %lu\n",
            slot, timer_slots[slot]->ticks_expected, __mg_tick_counter);

    pMsgQueue->nr_timers++;

    return TRUE;

badret:
    return FALSE;
}

#if 0   /* deprected code */
#ifdef _MGRM_PROCESSES
static void reset_select_timeout (TIMER** timer_slots)
{
    int i;

    unsigned int interv = 0;
    for (i = 0; i < DEF_NR_TIMERS; i++) {
        if (timer_slots[i]) {
            if (interv == 0 || timer_slots[i]->interv < interv)
                interv = timer_slots[i]->interv;
        }
    }
    __mg_set_select_timeout (USEC_10MS * interv);
}
#endif
#endif  /* deprecated code */

void __mg_remove_timer (MSGQUEUE* msg_queue, int slot)
{
    TIMER** timer_slots;

    if (slot < 0 || slot >= DEF_NR_TIMERS)
        return;

    timer_slots = msg_queue->timer_slots;
    if (MG_LIKELY (timer_slots[slot])) {
        /* The following code is already moved to message.c...
         * timer->msg_queue->expired_timer_mask &= ~(0x01UL << slot);
         */
        mg_slice_delete (TIMER, timer_slots[slot]);
        timer_slots [slot] = NULL;
        msg_queue->nr_timers--;
    }
}

void __mg_remove_timers_by_msg_queue (MSGQUEUE* msg_queue)
{
    int i;
    TIMER** timer_slots = msg_queue->timer_slots;

    for (i = 0; i < DEF_NR_TIMERS; i++) {
        if (timer_slots [i]) {
            mg_slice_delete (TIMER, timer_slots[i]);
            timer_slots [i] = NULL;
        }
    }

    msg_queue->nr_timers--;
}

/* If id == 0, clear all timers of the window */
int GUIAPI KillTimer (HWND hWnd, LINT id)
{
    int i;
    int killed = 0;
    MSGQUEUE* msg_queue;

#ifndef _MGRM_THREADS
    /* Force to update tick count */
    GetTickCount();
#endif

    msg_queue = getMsgQueueForThisThread ();
    if (msg_queue) {

        TIMER** timer_slots =  msg_queue->timer_slots;
        for (i = 0; i < DEF_NR_TIMERS; i++) {
            if ((timer_slots [i] && timer_slots [i]->hWnd == hWnd) &&
                    (id == 0 || timer_slots [i]->id == id)) {
                msg_queue->expired_timer_mask &= ~(0x01UL << i);
                mg_slice_delete (TIMER, timer_slots[i]);
                timer_slots [i] = NULL;
                msg_queue->nr_timers--;
                killed ++;

                if (id)
                    break;
            }
        }
    }
    else {
        _WRN_PRINTF ("called for non message thread\n");
    }


    return killed;
}

BOOL GUIAPI ResetTimerEx (HWND hWnd, LINT id, DWORD interv,
                TIMERPROC timer_proc)
{
    int i;
    MSGQUEUE* msg_queue;

#ifndef _MGRM_THREADS
    /* Force to update tick count */
    GetTickCount();
#endif

    if (id == 0)
        return FALSE;

    msg_queue = getMsgQueueForThisThread ();
    if (MG_LIKELY (msg_queue)) {
        TIMER** timer_slots = msg_queue->timer_slots;

        for (i = 0; i < DEF_NR_TIMERS; i++) {
            if (timer_slots[i] &&
                    timer_slots[i]->hWnd == hWnd && timer_slots[i]->id == id) {
                /* Should clear old timer flags */
                msg_queue->expired_timer_mask &= ~(0x01UL << i);
                timer_slots[i]->interv = interv;
                timer_slots[i]->ticks_expected = __mg_tick_counter + interv;
                timer_slots[i]->ticks_fired = 0;
                if (timer_proc != (TIMERPROC)INV_PTR)
                    timer_slots[i]->proc = timer_proc;

                return TRUE;
            }
        }
    }
    else {
        _WRN_PRINTF ("called for non message thread\n");
    }

    return FALSE;
}

BOOL GUIAPI IsTimerInstalled (HWND hWnd, LINT id)
{
    int i;
    TIMER** timer_slots;

#ifndef _MGRM_THREADS
    /* Force to update tick count */
    GetTickCount();
#endif

    if (id == 0)
        return FALSE;

    timer_slots = getTimerSlotsForThisThread (NULL);
    if (timer_slots) {
        for (i = 0; i < DEF_NR_TIMERS; i++) {
            if (timer_slots[i] &&
                    timer_slots[i]->hWnd == hWnd &&
                    timer_slots[i]->id == id) {
                return TRUE;
            }
        }
    }
    else {
        _WRN_PRINTF ("called for non message thread\n");
    }

    return FALSE;
}

BOOL GUIAPI HaveFreeTimer (void)
{
    int i;
    TIMER** timer_slots;

#ifndef _MGRM_THREADS
    /* Force to update tick count */
    GetTickCount();
#endif

    timer_slots = getTimerSlotsForThisThread (NULL);
    if (timer_slots) {
        for (i = 0; i < DEF_NR_TIMERS; i++) {
            if (timer_slots[i] == NULL)
                return TRUE;
        }
    }
    else {
        _WRN_PRINTF ("called for non message thread\n");
    }

    return FALSE;
}

DWORD GUIAPI GetTickCount (void)
{
#if defined(_MGRM_PROCESSES)
    if (mgIsServer) {
        __mg_tick_counter = __mg_os_get_time_ticks ();
        SHAREDRES_TIMER_COUNTER = __mg_tick_counter;
    }
    else {
        __mg_tick_counter = SHAREDRES_TIMER_COUNTER;
    }
#else   /* defined _MGRM_PROCESSES */
    __mg_tick_counter = __mg_os_get_time_ticks ();
#endif  /* not defined _MGRM_PROCESSES */

    return __mg_tick_counter;
}

