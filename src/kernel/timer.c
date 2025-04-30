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
** timer.c: The Timer module for MiniGUI.
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

/* update timer count for message and desktop thread */
DWORD __mg_update_tick_count (MSGQUEUE* msg_queue)
{
    DWORD ticks;

#if defined(_MGRM_PROCESSES)
    if (mgIsServer) {
        ticks = __mg_os_get_time_ticks ();
        SHAREDRES_TIMER_COUNTER = ticks;
    }
    else {
        ticks = SHAREDRES_TIMER_COUNTER;
    }
#else   /* defined _MGRM_PROCESSES */
    ticks = __mg_os_get_time_ticks ();
#endif  /* not defined _MGRM_PROCESSES */

    if (msg_queue && ticks != msg_queue->last_ticks) {
        /* Since 5.0.0, the desktop only handles caret blinking in MSG_TIMEOUT
           message, and the interval for the timer of desktop changes to 0.05s.
         */
        if (msg_queue == __mg_dsk_msg_queue) {
            if (ticks > __mg_dsk_msg_queue->last_ticks +
                    DESKTOP_TIMER_INERTVAL) {
                __mg_dsk_msg_queue->dwState |= QS_DESKTIMER;
#ifdef _MGRM_THREADS    /* only wake up desktop for threads mode */
                POST_MSGQ (__mg_dsk_msg_queue);
#endif
            }
        }

        msg_queue->last_ticks = ticks;
    }

    __mg_tick_counter = ticks;
    return ticks;
}

BOOL mg_InitTimer (BOOL use_sys_timer)
{
    __mg_tick_counter = 0;

    __mg_os_start_time();

    if (use_sys_timer) {
        // Since 5.0.14, do nothing.
        // install_system_timer ();
    }

    return TRUE;
}

void mg_TerminateTimer (BOOL use_sys_timer)
{
    if (use_sys_timer) {
        // Since 5.0.14, do nothing.
        // unintall_system_timer ();
    }
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

        __mg_update_tick_count (msg_queue);

        for (i = 0; i < DEF_NR_TIMERS; i++) {
            if (timer_slots[i]) {
                if (msg_queue->last_ticks >= timer_slots[i]->ticks_expected) {
                    /* setting timer flag is simple, we do not need to lock
                       msgq, or else we may encounter dead lock here */
                    msg_queue->expired_timer_mask |= (0x01UL << i);
                    POST_MSGQ (msg_queue);

                    timer_slots[i]->ticks_expected =
                        msg_queue->last_ticks + timer_slots[i]->interv;
                    timer_slots[i]->ticks_fired = msg_queue->last_ticks;
                    nr++;
                }
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
    PMSGQUEUE msg_queue;

    if (id == 0) {
        _WRN_PRINTF ("bad identifier (%ld).\n", id);
        return FALSE;
    }

    if (interv == 0) {
        interv = 1;
    }

    timer_slots = getTimerSlotsForThisThread (&msg_queue);
    if (MG_UNLIKELY (timer_slots == NULL)) {
        _WRN_PRINTF ("called for non message thread\n");
        goto badret;
    }

    __mg_update_tick_count (msg_queue);

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
            timer_slots[i]->ticks_expected = msg_queue->last_ticks + interv;
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
    timer_slots[slot]->ticks_expected = msg_queue->last_ticks + interv;
    timer_slots[slot]->ticks_fired = 0;
    timer_slots[slot]->proc = timer_proc;

    _DBG_PRINTF ("ticks_expected (%d): %lu, tick_counter: %lu\n",
            slot, timer_slots[slot]->ticks_expected, msg_queue->last_ticks);

    msg_queue->nr_timers++;

    return TRUE;

badret:
    return FALSE;
}

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

        __mg_update_tick_count (msg_queue);
        for (i = 0; i < DEF_NR_TIMERS; i++) {
            if (timer_slots[i] &&
                    timer_slots[i]->hWnd == hWnd && timer_slots[i]->id == id) {
                /* Should clear old timer flags */
                msg_queue->expired_timer_mask &= ~(0x01UL << i);
                timer_slots[i]->interv = interv;
                timer_slots[i]->ticks_expected = msg_queue->last_ticks + interv;
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
    return __mg_update_tick_count(getMsgQueueForThisThread ());
}

