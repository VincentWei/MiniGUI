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
** tiemr.h: the head file of Timer module.
**
** Create date: 1999/4/21
*/

#ifndef GUI_TIMER_H
    #define GUI_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define USEC_1S         1000000
#define USEC_10MS       10000

typedef struct _timer {
    HWND        hWnd;
    LINT        id;
    DWORD       speed;
    UINT_PTR    count;

    TIMERPROC   proc;
    UINT_PTR    tick_count;
    PMSGQUEUE   msg_queue;
} TIMER;
typedef TIMER* PTIMER;

BOOL mg_InitTimer (void);
void mg_TerminateTimer (void);
void mg_dispatch_timer_message (DWORD inter);
void mg_remove_timers_by_msg_queue (const MSGQUEUE* msg_que);

TIMER* __mg_get_timer (int slot);
void __mg_remove_timer (TIMER* timer, int slot);

static inline HWND __mg_get_timer_hwnd (int slot)
{
    TIMER* ptimer = __mg_get_timer (slot);
    if (ptimer)
        return ptimer->hWnd;

    return HWND_NULL;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_TIMER_H */

