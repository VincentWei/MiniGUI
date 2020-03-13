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
#include "ctrlclass.h"
#include "timer.h"
#include "newgal.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

static inline void
dump_region (const CLIPRGN* rgn, const char* name)
{
    _MG_PRINTF("rcBound of %s (%p): (%d, %d, %d, %d); size (%d x %d)\n",
            name, rgn,
            rgn->rcBound.left, rgn->rcBound.top,
            rgn->rcBound.right, rgn->rcBound.bottom,
            RECTW(rgn->rcBound), RECTH(rgn->rcBound));
}

static inline void
dump_rect (const RECT* rect, const char* name)
{
    _MG_PRINTF("rect of %s: (%d, %d, %d, %d); size (%d x %d)\n", name,
            rect->left, rect->top, rect->right, rect->bottom,
            RECTWP(rect), RECTHP(rect));
}

static inline void
dump_gal_rect (const GAL_Rect* rect, const char* name)
{
    _MG_PRINTF("rect of %s: (%d, %d), (%d X %d)\n", name,
            rect->x, rect->y, rect->w, rect->h);
}

static inline void
dump_window (HWND hwnd, const char* name)
{
    PMAINWIN main_win = (PMAINWIN)hwnd;

    if (hwnd == HWND_NULL) {
        _MG_PRINTF("window for %s: (nil)\n", name);
        return;
    }
    else if (hwnd == HWND_INVALID) {
        _MG_PRINTF("window for %s: (invalid)\n", name);
        return;
    }
    else if (main_win->DataType != TYPE_HWND) {
        _MG_PRINTF("window for %s: not a window (datatype: %d)\n", name,
                (int)main_win->DataType);
        return;
    }

    switch (main_win->WinType) {
    case TYPE_MAINWIN:
        _MG_PRINTF("A main window for %s: "
                "caption(%s), id(%ld), positon(%d, %d, %d, %d)\n",
                name, main_win->spCaption, main_win->id,
                main_win->left, main_win->top,
                main_win->right, main_win->bottom);
        break;

    case TYPE_VIRTWIN:
        _MG_PRINTF("A virtual window for %s: "
                "caption(%s), id(%ld)\n",
                name, main_win->spCaption, main_win->id);
        break;

    case TYPE_CONTROL:
        _MG_PRINTF("A control for %s: "
                "caption(%s), id(%ld), positon(%d, %d, %d, %d)\n",
                name, main_win->spCaption, main_win->id,
                main_win->left, main_win->top,
                main_win->right, main_win->bottom);
        break;

    case TYPE_ROOTWIN:
        _MG_PRINTF("A root window for %s: "
                "caption(%s), id(%ld), positon(%d, %d, %d, %d)\n",
                name, main_win->spCaption, main_win->id, 
                main_win->left, main_win->top,
                main_win->right, main_win->bottom);
        break;

    default:
        _MG_PRINTF("A unknown window for %s: %p\n",
                name, main_win);
        break;
    }
}

static inline void
dump_message (const MSG* msg, const char* name)
{
#ifdef _MGHAVE_MSG_STRING
    _MG_PRINTF ("Message(%s) for %s: %u (%s), Wnd: %p, wP: %p, lP: %p.\n",
#ifdef _MGHAVE_VIRTUAL_WINDOW
            msg->pSyncMsg?"SYNC":"NORM",
#else
            "NORM",
#endif
            name, msg->message, Message2Str (msg->message),
            msg->hwnd, (PVOID)msg->wParam, (PVOID)msg->lParam);
#else
    _MG_PRINTF ("Message(%s) for %s: %u, Wnd: %p, wP: %p, lP: %p.\n",
#ifdef _MGHAVE_VIRTUAL_WINDOW
            msg->pSyncMsg?"SYNC":"NORM",
#else
            "NORM",
#endif
            name, msg->message,
            msg->hwnd, (PVOID)msg->wParam, (PVOID)msg->lParam);
#endif
}

static inline void
dump_message_with_retval (const MSG* msg, LRESULT retval, const char* name)
{
#ifdef _MGHAVE_MSG_STRING
    _MG_PRINTF ("Message(%s) for %s done: %u (%s), Wnd: %p, retval (%p).\n",
#ifdef _MGHAVE_VIRTUAL_WINDOW
            msg->pSyncMsg?"SYNC":"NORM",
#else
            "NORM",
#endif
            name, msg->message, Message2Str (msg->message),
            msg->hwnd, (PVOID)retval);
#else
    _MG_PRINTF ("Message(%s) for %s: %u, Wnd: %p, retval (%p).\n",
#ifdef _MGHAVE_VIRTUAL_WINDOW
            msg->pSyncMsg?"SYNC":"NORM",
#else
            "NORM",
#endif
            name, msg->message,
            msg->hwnd, (PVOID)retval);
#endif
}

static inline void
dump_mouse_message (UINT message, int location, int x, int y, const char* name)
{
#ifdef _MGHAVE_MSG_STRING
    _MG_PRINTF ("Mouse message for %s: %u (%s), location(%d), x(%d), y(%d).\n",
            name, message, Message2Str (message),
            location, x, y);
#else
    _MG_PRINTF ("Mouse message for %s: %u, location(%d), x(%d), y(%d).\n",
            name, message, location, x, y);
#endif
}

static inline void
dump_message_queue (const MSGQUEUE* msg_queue, const char* name)
{
    int i, nr;

    _MG_PRINTF ("Message queue for %s: Root Window (%p), nrWindows (%d), "
            "nrTimers (%d), nrListenFD (%d):\n",
            name,
            msg_queue->pRootMainWin ?
                msg_queue->pRootMainWin->spCaption : "NULL",
            msg_queue->nrWindows, msg_queue->nr_timers, msg_queue->nr_fd_slots);

    nr = 0;
    for (i = 0; i < DEF_NR_TIMERS; i++) {
        if (msg_queue->timer_slots[i]) {
            _MG_PRINTF ("  Timer #%d: hWnd (%p), id (%ld)\n",
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
            _MG_PRINTF ("  Listened FD #%d: type (%d), fd (%d), hwnd (%p)\n",
                    i, msg_queue->fd_slots[i]->type,
                    msg_queue->fd_slots[i]->fd, msg_queue->fd_slots[i]->hwnd);
            nr++;
        }
    }

    _MG_PRINTF ("----End of message queue for %s\n", name);
}

static inline void
dump_window_details (HWND hwnd, const char* name)
{
    PMAINWIN main_wnd = (PMAINWIN)hwnd;

    _MG_PRINTF ("Start info of handle (%p) for %s", hwnd, name);

    if (main_wnd->DataType == TYPE_WINTODEL) {
        _WRN_PRINTF ("The handle (%p) is a %s window to be deleted!\n",
                hwnd, (main_wnd->WinType == TYPE_MAINWIN) ? "main" : "virtual");
        return;
    }
    else if (main_wnd->DataType != TYPE_HWND) {
        _ERR_PRINTF ("The handle (%p) is not a window!\n", hwnd);
        return;
    }

    if (main_wnd->WinType == TYPE_MAINWIN ||
            main_wnd->WinType == TYPE_ROOTWIN) {
        PCONTROL control = (PCONTROL)hwnd;

        _MG_PRINTF ("The handle (*%p) is a main Windown", hwnd);

        _MG_PRINTF ("Rect        -- (%d, %d, %d, %d)\n",
                        main_wnd->left, main_wnd->top,
                        main_wnd->right, main_wnd->bottom);
        _MG_PRINTF ("Client Rect -- (%d, %d, %d, %d)\n",
                        main_wnd->cl, main_wnd->ct, main_wnd->cr, main_wnd->cb);

        _MG_PRINTF ("Style       -- %lx\n", main_wnd->dwStyle);
        _MG_PRINTF ("ExStyle     -- %lx\n", main_wnd->dwExStyle);

        _MG_PRINTF ("AddData     -- %lx\n", main_wnd->dwAddData);
        _MG_PRINTF ("AddData2    -- %lx\n", main_wnd->dwAddData2);

        _MG_PRINTF ("WinProc     -- %p\n",  main_wnd->MainWindowProc);
        _MG_PRINTF ("NotifProc   -- %p\n",  main_wnd->NotifProc);
        _MG_PRINTF ("Caption     -- %s\n",  main_wnd->spCaption);
        _MG_PRINTF ("ID          -- %ld\n", main_wnd->id);

        _MG_PRINTF ("FirstChild  -- %p\n",  main_wnd->hFirstChild);

        control = (PCONTROL)main_wnd->hFirstChild;
        while (control) {
            _MG_PRINTF ("    Child   -- %p (%ld); class: %s (%d)\n",
                    control, control->id,
                    control->pcci->name, control->pcci->nUseCount);
            control = control->next;
        }

        _MG_PRINTF ("ActiveChild -- %p\n",  main_wnd->hActiveChild);
        _MG_PRINTF ("Hosting     -- %p\n",  main_wnd->pHosting);
        _MG_PRINTF ("FirstHosted -- %p\n",  main_wnd->pFirstHosted);
        _MG_PRINTF ("NextHosted  -- %p\n",  main_wnd->pNextHosted);
        _MG_PRINTF ("BkColor     -- %x\n",  main_wnd->iBkColor);
        _MG_PRINTF ("Menu        -- %p\n",  main_wnd->hMenu);
        _MG_PRINTF ("Accel       -- %p\n",  main_wnd->hAccel);
        _MG_PRINTF ("Cursor      -- %p\n",  main_wnd->hCursor);
        _MG_PRINTF ("Icon        -- %p\n",  main_wnd->hIcon);
        _MG_PRINTF ("SysMenu     -- %p\n",  main_wnd->hSysMenu);
        _MG_PRINTF ("MsgQueue    -- %p\n",  main_wnd->pMsgQueue);
    }
    else if (main_wnd->WinType == TYPE_CONTROL) {
        PCONTROL control = (PCONTROL)hwnd;

        _MG_PRINTF ("The handle (%p) is a control:\n", hwnd);

        _MG_PRINTF ("Rect        -- (%d, %d, %d, %d)\n",
                        control->left, control->top,
                        control->right, control->bottom);
        _MG_PRINTF ("Client Rect -- (%d, %d, %d, %d)\n",
                        control->cl, control->ct, control->cr, control->cb);

        _MG_PRINTF ("Style       -- %lx\n", control->dwStyle);
        _MG_PRINTF ("ExStyle     -- %lx\n", control->dwExStyle);

        _MG_PRINTF ("PrivCDC     -- %p\n",  control->privCDC);

        _MG_PRINTF ("AddData     -- %lx\n", control->dwAddData);
        _MG_PRINTF ("AddData2    -- %lx\n", control->dwAddData2);

        _MG_PRINTF ("WinProc     -- %p\n",  control->ControlProc);
        _MG_PRINTF ("NotifProc   -- %p\n",  control->NotifProc);

        _MG_PRINTF ("Caption     -- %s\n",  control->spCaption);
        _MG_PRINTF ("ID          -- %ld\n", control->id);

        _MG_PRINTF ("FirstChild  -- %p\n",  control->children);
        _MG_PRINTF ("ActiveChild -- %p\n",  control->active);
        _MG_PRINTF ("Parent      -- %p\n",  control->pParent);
        _MG_PRINTF ("Next        -- %p\n",  control->next);

        control = (PCONTROL)control->children;
        while (control) {
            _MG_PRINTF ("    Child   -- %p (%ld); class: %s (%d)\n",
                    control, control->id,
                    control->pcci->name, control->pcci->nUseCount);
            control = control->next;
        }
    }
#ifdef _MGHAVE_VIRTUAL_WINDOW
    else if (main_wnd->WinType == TYPE_VIRTWIN) {
        PVIRTWIN virt_wnd = (PVIRTWIN)hwnd;

        _MG_PRINTF ("The handle (*%p) is a virtual Window", hwnd);

        _MG_PRINTF ("AddData     -- %lx\n", virt_wnd->dwAddData);
        _MG_PRINTF ("AddData2    -- %lx\n", virt_wnd->dwAddData2);

        _MG_PRINTF ("WinProc     -- %p\n",  virt_wnd->WndProc);
        _MG_PRINTF ("NotifProc   -- %p\n",  virt_wnd->NotifProc);
        _MG_PRINTF ("Caption     -- %s\n",  virt_wnd->spCaption);
        _MG_PRINTF ("ID          -- %ld\n", virt_wnd->id);

        _MG_PRINTF ("Hosting     -- %p\n",  virt_wnd->pHosting);
        _MG_PRINTF ("FirstHosted -- %p\n",  virt_wnd->pFirstHosted);
        _MG_PRINTF ("NextHosted  -- %p\n",  virt_wnd->pNextHosted);

        _MG_PRINTF ("MsgQueue    -- %p\n",  virt_wnd->pMsgQueue);
    }
#endif  /* defined  _MGHAVE_VIRTUAL_WINDOW */
    else {
        _ERR_PRINTF ("The handle (%p) has a bad window type: %d!\n", hwnd,
                (int)main_wnd->WinType);
    }

    _MG_PRINTF ("End of info for handle (%p) of %s", hwnd, name);
}

static inline void
dump_surface (GAL_Surface* surface, const char* name)
{
    _MG_PRINTF ("Surface RGBA masks for %s: %x, %x, %x, %x\n", name,
            surface->format->Rmask,
            surface->format->Gmask,
            surface->format->Bmask,
            surface->format->Amask);
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_DEBUG_H */

