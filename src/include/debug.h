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
** debug.h: Some common debug helpers.
**
** Create date: 2020/02/12
*/

#ifndef GUI_DEBUG_H
    #define GUI_DEBUG_H

#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "internals.h"
#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

static inline void
dump_region (const CLIPRGN* rgn, const char* name)
{
    _WRN_PRINTF("rcBound of %s (%p): (%d, %d, %d, %d); size (%d x %d)\n",
            name, rgn,
            rgn->rcBound.left, rgn->rcBound.top,
            rgn->rcBound.right, rgn->rcBound.bottom,
            RECTW(rgn->rcBound), RECTH(rgn->rcBound));
}

static inline void
dump_rect (const RECT* rect, const char* name)
{
    _WRN_PRINTF("rect of %s: (%d, %d, %d, %d); size (%d x %d)\n", name,
            rect->left, rect->top, rect->right, rect->bottom,
            RECTWP(rect), RECTHP(rect));
}

static inline void
dump_window (HWND hwnd, const char* name)
{
    PMAINWIN main_win = (PMAINWIN)hwnd;

    if (hwnd == HWND_NULL) {
        _WRN_PRINTF("window for %s: (nil)\n", name);
        return;
    }
    else if (hwnd == HWND_INVALID) {
        _WRN_PRINTF("window for %s: (invalid)\n", name);
        return;
    }
    else if (main_win->DataType != TYPE_HWND) {
        _WRN_PRINTF("window for %s: not a window (datatype: %d)\n", name,
                (int)main_win->DataType);
        return;
    }

    switch (main_win->WinType) {
    case TYPE_MAINWIN:
        _WRN_PRINTF("A main window for %s: "
                "caption(%s), id(%ld), positon(%d, %d, %d, %d)\n",
                name, main_win->spCaption, main_win->id,
                main_win->left, main_win->top,
                main_win->right, main_win->bottom);
        break;

    case TYPE_VIRTWIN:
        _WRN_PRINTF("A virtual window for %s: "
                "caption(%s), id(%ld)\n",
                name, main_win->spCaption, main_win->id);
        break;

    case TYPE_CONTROL:
        _WRN_PRINTF("A control for %s: "
                "caption(%s), id(%ld), positon(%d, %d, %d, %d)\n",
                name, main_win->spCaption, main_win->id,
                main_win->left, main_win->top,
                main_win->right, main_win->bottom);
        break;

    case TYPE_ROOTWIN:
        _WRN_PRINTF("A root window for %s: "
                "caption(%s), id(%ld), positon(%d, %d, %d, %d)\n",
                name, main_win->spCaption, main_win->id, 
                main_win->left, main_win->top,
                main_win->right, main_win->bottom);
        break;

    default:
        _WRN_PRINTF("A unknown window for %s: %p\n",
                name, main_win);
        break;
    }
}

static inline void
dump_message (const MSG* msg, const char* name)
{
#ifdef _MGHAVE_MSG_STRING
    _WRN_PRINTF ("Message(%s) for %s: %u (%s), Wnd: %p, wP: %p, lP: %p.\n",
            msg->pAdd?"SYNC":"NORM",
            name, msg->message, Message2Str (msg->message),
            msg->hwnd, (PVOID)msg->wParam, (PVOID)msg->lParam);
#else
    _WRN_PRINTF ("Message(%S) for %s: %u, Wnd: %p, wP: %p, lP: %p.\n",
            msg->pAdd?"SYNC":"NORM",
            name, msg->message,
            msg->hwnd, (PVOID)msg->wParam, (PVOID)msg->lParam);
#endif
}

static inline void
dump_message_with_retval (const MSG* msg, LRESULT retval,
        const char* name)
{
#ifdef _MGHAVE_MSG_STRING
    _WRN_PRINTF ("Message(%s) for %s done: %u (%s), Wnd: %p, retval (%p).\n",
            msg->pAdd?"SYNC":"NORM",
            name, msg->message, Message2Str (msg->message),
            msg->hwnd, (PVOID)retval);
#else
    _WRN_PRINTF ("Message(%S) for %s: %u, Wnd: %p, retval (%p).\n",
            msg->pAdd?"SYNC":"NORM",
            name, msg->message,
            msg->hwnd, (PVOID)retval);
#endif
}

static inline void
dump_mouse_message (UINT message, int location, int x, int y,
        const char* name)
{
#ifdef _MGHAVE_MSG_STRING
    _WRN_PRINTF ("Mouse message for %s: %u (%s), location(%d), x(%d), y(%d).\n",
            name, message, Message2Str (message),
            location, x, y);
#else
    _WRN_PRINTF ("Mouse message for %s: %u, location(%d), x(%d), y(%d).\n",
            name, message, location, x, y);
#endif
}

static inline void
dump_message_queue (const MSGQUEUE* msg_queue, const char* name)
{
    int i, nr;

    _WRN_PRINTF ("Message queue for %s: Root Window (%p), nrWindows (%d), "
            "nrTimers (%d), nrListenFD (%d):\n",
            name,
            msg_queue->pRootMainWin ?
                msg_queue->pRootMainWin->spCaption : "NULL",
            msg_queue->nrWindows, msg_queue->nr_timers, msg_queue->nr_fd_slots);

    nr = 0;
    for (i = 0; i < DEF_NR_TIMERS; i++) {
        if (msg_queue->timer_slots[i]) {
            _WRN_PRINTF ("  Timer #%d: hWnd (%p), id (%ld)\n",
                    i, msg_queue->timer_slots[i]->hWnd,
                    msg_queue->timer_slots[i]->id);
            nr++;
        }
    }

    if (nr != msg_queue->nr_timers) {
        _ERR_PRINTF ("nrTimers in message queue does not match counted %d\n",
                nr);
    }

    nr = 0;
    for (i = 0; i < msg_queue->nr_fd_slots; i++) {
        if (msg_queue->fd_slots[i]) {
            _WRN_PRINTF ("  Listened FD #%d: type (%d), fd (%d), hwnd (%p)\n",
                    i, msg_queue->fd_slots[i]->type,
                    msg_queue->fd_slots[i]->fd, msg_queue->fd_slots[i]->hwnd);
            nr++;
        }
    }

    _WRN_PRINTF ("----End of message queue for %s\n", name);
}
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_DEBUG_H */

