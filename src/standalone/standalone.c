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
** standalone.c: low-level routines for MiniGUI-Lite standalone version.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2003/08/15
*/

#include <string.h>
#include <errno.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <signal.h>
#include <time.h>
#include <limits.h>
#ifndef WIN32
#include <sys/poll.h>
#endif

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "internals.h"
#include "gal.h"
#include "ial.h"
#include "cursor.h"
#include "event.h"
#include "menu.h"
#include "ourhdr.h"

extern DWORD __mg_timer_counter;
static DWORD old_timer_counter = 0;

static SRVEVTHOOK srv_evt_hook = NULL;

SRVEVTHOOK GUIAPI SetServerEventHook (SRVEVTHOOK SrvEvtHook)
{
    SRVEVTHOOK old_hook = srv_evt_hook;

    srv_evt_hook = SrvEvtHook;

    return old_hook;
}

static void ParseEvent (PMSGQUEUE msg_que, int event)
{
    LWEVENT lwe;
    PMOUSEEVENT me;
    PKEYEVENT ke;
    MSG Msg;

    ke = &(lwe.data.ke);
    me = &(lwe.data.me);
    me->x = 0; me->y = 0;
    Msg.hwnd = HWND_DESKTOP;
    Msg.wParam = 0;
    Msg.lParam = 0;

    lwe.status = 0L;

    if (!kernel_GetLWEvent (event, &lwe))
        return;

    Msg.time = __mg_timer_counter;
    if (lwe.type == LWETYPE_TIMEOUT) {
        Msg.message = MSG_TIMEOUT;
        Msg.wParam = (WPARAM)lwe.count;
        Msg.lParam = 0;

        kernel_QueueMessage (msg_que, &Msg);
    }
    else if (lwe.type == LWETYPE_KEY) {
        Msg.wParam = ke->scancode;
        Msg.lParam = ke->status;
        if (ke->event == KE_KEYDOWN){
            Msg.message = MSG_KEYDOWN;
        }
        else if (ke->event == KE_KEYUP) {
            Msg.message = MSG_KEYUP;
        }
        else if (ke->event == KE_KEYLONGPRESS) {
            Msg.message = MSG_KEYLONGPRESS;
        }
        else if (ke->event == KE_KEYALWAYSPRESS) {
            Msg.message = MSG_KEYALWAYSPRESS;
        }

        if (!(srv_evt_hook && srv_evt_hook (&Msg))) {
            kernel_QueueMessage (msg_que, &Msg);
        }
    }
    else if (lwe.type == LWETYPE_MOUSE) {
        Msg.wParam = me->status;
        switch (me->event) {
        case ME_MOVED:
            Msg.message = MSG_MOUSEMOVE;
            SetCursor (GetSystemCursor (IDC_ARROW));
            break;
        case ME_LEFTDOWN:
            Msg.message = MSG_LBUTTONDOWN;
            break;
        case ME_LEFTUP:
            Msg.message = MSG_LBUTTONUP;
            break;
        case ME_LEFTDBLCLICK:
            Msg.message = MSG_LBUTTONDBLCLK;
            break;
        case ME_RIGHTDOWN:
            Msg.message = MSG_RBUTTONDOWN;
            break;
        case ME_RIGHTUP:
            Msg.message = MSG_RBUTTONUP;
            break;
        case ME_RIGHTDBLCLICK:
            Msg.message = MSG_RBUTTONDBLCLK;
            break;
        }

        Msg.lParam = MAKELONG (me->x, me->y);
        if (!(srv_evt_hook && srv_evt_hook (&Msg))) {
            kernel_QueueMessage (msg_que, &Msg);
        }
    }
}

BOOL GUIAPI salone_StandAloneStartup (void)
{
    mg_fd_zero (&mg_rfdset);
    mg_maxfd = 0;

#ifndef _MGGAL_BF533
    mg_InstallIntervalTimer ();
#endif

    return TRUE;
}

void salone_StandAloneCleanup (void)
{
#ifndef _MGGAL_BF533
    mg_UninstallIntervalTimer ();
#endif
}

BOOL minigui_idle (void)
{
    return salone_IdleHandler4StandAlone (__mg_dsk_msg_queue);
}

BOOL salone_IdleHandler4StandAlone (PMSGQUEUE msg_queue)
{
    int    i, n;
#ifdef __NOUNIX__
    struct timeval sel_timeout = {0, 10000};
#elif defined (_MGGAL_BF533)
    struct timeval sel_timeout = {0, 100000};
#endif
    struct timeval sel_timeout_nd = {0, 0};
    fd_set rset, wset, eset;
    fd_set* wsetptr = NULL;
    fd_set* esetptr = NULL;

    if (old_timer_counter != __mg_timer_counter) {
        old_timer_counter = __mg_timer_counter;
        SetDesktopTimerFlag ();
    }

    rset = mg_rfdset;        /* rset gets modified each time around */
    if (mg_wfdset) {
        wset = *mg_wfdset;
        wsetptr = &wset;
    }
    if (mg_efdset) {
        eset = *mg_efdset;
        esetptr = &eset;
    }

#ifdef __NOUNIX__
    n = IAL_WaitEvent (IAL_MOUSEEVENT | IAL_KEYEVENT, 
                mg_maxfd, &rset, wsetptr, esetptr, 
                msg_queue?&sel_timeout:&sel_timeout_nd);

    /* update __mg_timer_counter */
    if (msg_queue) {
        //__mg_timer_counter += 10;
		/**
		10 is too fast, the "repeat_threshold" is 5, use ++, repeate 5 times
		**/
		__mg_timer_counter++;
    }
#elif defined (_MGGAL_BF533)
    n = IAL_WaitEvent (IAL_MOUSEEVENT | IAL_KEYEVENT, 
                mg_maxfd, &rset, wsetptr, esetptr, 
                msg_queue?&sel_timeout:&sel_timeout_nd);

    /* update __mg_timer_counter */
    if (msg_queue) {
        __mg_timer_counter += 1;
    }
#else
    n = IAL_WaitEvent (IAL_MOUSEEVENT | IAL_KEYEVENT, 
                mg_maxfd, &rset, wsetptr, esetptr, 
                msg_queue?NULL:&sel_timeout_nd);
#endif

    if (msg_queue == NULL) msg_queue = __mg_dsk_msg_queue;

    if (n < 0) {

        /* It is time to check event again. */
        if (errno == EINTR) {
            //if (msg_queue)
                ParseEvent (msg_queue, 0);
        }
        return FALSE;
    }
/*
    else if (msg_queue == NULL)
        return (n > 0);
*/
    /* handle intput event (mouse/touch-screen or keyboard) */
    if (n & IAL_MOUSEEVENT) ParseEvent (msg_queue, IAL_MOUSEEVENT);
    if (n & IAL_KEYEVENT) ParseEvent (msg_queue, IAL_KEYEVENT);
    if (n == 0) ParseEvent (msg_queue, 0);

    /* go through registered listen fds */
    for (i = 0; i < MAX_NR_LISTEN_FD; i++) {
        MSG Msg;

        Msg.message = MSG_FDEVENT;

        if (mg_listen_fds [i].fd) {
            fd_set* temp = NULL;
            int type = mg_listen_fds [i].type;

            switch (type) {
            case POLLIN:
                temp = &rset;
                break;
            case POLLOUT:
                temp = wsetptr;
                break;
            case POLLERR:
                temp = esetptr;
                break;
            }

            if (temp && mg_fd_isset (mg_listen_fds [i].fd, temp)) {
                Msg.hwnd = (HWND)mg_listen_fds [i].hwnd;
                Msg.wParam = MAKELONG (mg_listen_fds [i].fd, type);
                Msg.lParam = (LPARAM)mg_listen_fds [i].context;
                kernel_QueueMessage (msg_queue, &Msg);
            }
        }
    }

    return (n > 0);
}

