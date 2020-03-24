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
** server.c: routines for server.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/12/20
**
** NOTE: The idea comes from sample code in APUE.
*/

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/times.h>
#include <sys/poll.h>

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
#include "sockio.h"
#include "client.h"
#include "server.h"
#include "sharedres.h"
#include "drawsemop.h"
#include "timer.h"
#include "license.h"

extern DWORD __mg_tick_counter;

ON_NEW_DEL_CLIENT OnNewDelClient = NULL;

#if 0
/* Since 5.0.0, use RegisterEventHookFunc to implement SetServerEventHook */
static SRVEVTHOOK srv_evt_hook = NULL;

SRVEVTHOOK GUIAPI SetServerEventHook (SRVEVTHOOK SrvEvtHook)
{
    SRVEVTHOOK old_hook = srv_evt_hook;

    srv_evt_hook = SrvEvtHook;

    return old_hook;
}
#endif /* deprecated code; moved to window.c */

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

    ke->status = 0L;
    me->status = 0L;
    lwe.status = 0L;

    if (!kernel_GetLWEvent (event, &lwe))
        return;

    Msg.time = __mg_tick_counter;
    if (lwe.type == LWETYPE_TIMEOUT) {
        Msg.message = MSG_TIMEOUT;
        Msg.wParam = (WPARAM)lwe.count;
        Msg.lParam = 0;

        // Since 5.0.0, we do not genenrate MSG_TIMEOUT message any more.
        // kernel_QueueMessage (msg_que, &Msg);
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

#if 0   /* Since 5.0.0 */
        if (__mg_do_drag_drop_window (Msg.message, 0, 0))
            return;

        if (!(srv_evt_hook && srv_evt_hook (&Msg))) {
            kernel_QueueMessage (msg_que, &Msg);
        }
#else   /* deprecated code */
        if (__mg_check_hook_func (HOOK_EVENT_KEY, &Msg) == HOOK_GOON) {
            kernel_QueueMessage (msg_que, &Msg);
        }
#endif  /* use __mg_check_hook_func instead */
    }
    else if (lwe.type == LWETYPE_MOUSE) {
#if 0   /* Since 5.0.0: move to desktop */
        static int down_client = -1;
        static int down_by;
        int cur_client = __mg_get_znode_at_point (__mg_zorder_info,
                        me->x, me->y, NULL);
#endif  /* moved to desktop */

        switch (me->event) {
        case ME_MOVED:
            Msg.message = MSG_MOUSEMOVE;
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
        case ME_MIDDLEDOWN:
            Msg.message = MSG_MBUTTONDOWN;
            break;
        case ME_MIDDLEUP:
            Msg.message = MSG_MBUTTONUP;
            break;
        case ME_MIDDLEDBLCLICK:
            Msg.message = MSG_MBUTTONDBLCLK;
            break;
        }

        Msg.wParam = me->status;
        Msg.lParam = MAKELONG (me->x, me->y);

        if (__mg_check_hook_func (HOOK_EVENT_MOUSE, &Msg) == HOOK_GOON) {
            kernel_QueueMessage (msg_que, &Msg);
        }

#if 0   /* Since 5.0.0: move to desktop */
        if (__mg_do_drag_drop_window (Msg.message, me->x, me->y)) {
            down_client = -1;
            return;
        }

        switch (Msg.message) {
        case MSG_LBUTTONDOWN:
        case MSG_RBUTTONDOWN:
            if (cur_client >= 0 && down_client == -1) {
                down_client = cur_client;
                down_by = Msg.message;
            }
            break;
        }

        if (cur_client == -1)
            SetDefaultCursor (GetSystemCursor (IDC_ARROW));

        if (__mg_handle_mouse_hook (Msg.message,
                                Msg.wParam, Msg.lParam) == HOOK_STOP)
            return;

        if (!(srv_evt_hook && srv_evt_hook (&Msg))) {
            int target_client = cur_client;

            if (down_client < 0) {
                if (!__mg_captured_wnd && me->event == ME_MOVED) {
                    int cli = __mg_handle_normal_mouse_move (__mg_zorder_info,
                                    me->x, me->y);

                    if (cli > 0)
                        target_client = cli;
                }
            }
            else
                target_client = down_client;

            if (target_client > 0) {
                Msg.hwnd = 0;
#ifdef _MG_CONFIG_FAST_MOUSEMOVE
                if (Msg.message == MSG_MOUSEMOVE) {
                    LOCK_MOUSEMOVE_SEM();
                    if (SHAREDRES_MOUSEMOVECLIENT > 0 &&
                            SHAREDRES_MOUSEMOVECLIENT != target_client) {
                        _DBG_PRINTF ("drop a mouse move message, old_client=%d, target_client=%d\n",
                                SHAREDRES_MOUSEMOVECLIENT, target_client);
                    }
                    SHAREDRES_MOUSEMOVECLIENT = target_client;
                    ++ SHAREDRES_MOUSEMOVESERIAL;
                    UNLOCK_MOUSEMOVE_SEM();
                }
                else
#endif
                {
                    Send2Client (&Msg, target_client);
                }
            }
            else
                kernel_QueueMessage (msg_que, &Msg);
        }

        if (Msg.message == MSG_LBUTTONUP && down_by == MSG_LBUTTONDOWN) {
            down_client = -1;
        }
        if (Msg.message == MSG_RBUTTONUP && down_by == MSG_RBUTTONDOWN) {
            down_client = -1;
        }
#endif  /* moved to desktop */
    }
}

static int listenfd;
static int maxi;

BOOL GUIAPI ServerStartup (int nr_globals,
                int def_nr_topmosts, int def_nr_normals)
{
    if (!mgIsServer)
        return FALSE;

    if (nr_globals <= 0) nr_globals = DEF_NR_GLOBALS;
    if (def_nr_topmosts <= 0) def_nr_topmosts = DEF_NR_TOPMOSTS;
    if (def_nr_normals <= 0) def_nr_normals = DEF_NR_NORMALS;

    nr_globals = (nr_globals + 7) & ~0x07;
    nr_globals -= NR_FIXED_ZNODES;
    def_nr_topmosts = (def_nr_topmosts + 7) & ~0x07;
    def_nr_normals = (def_nr_normals + 7) & ~0x07;

    SHAREDRES_NR_GLOBALS = nr_globals;
    SHAREDRES_DEF_NR_TOPMOSTS = def_nr_topmosts;
    SHAREDRES_DEF_NR_NORMALS = def_nr_normals;

    if (__mg_init_layers () == -1) {
        _ERR_PRINTF ("mginit: failed to initialize layers\n");
        return FALSE;
    }

    SHAREDRES_NR_LAYSERS = 1;
    mgTopmostLayer = mgLayers;
    SHAREDRES_TOPMOST_LAYER = (GHANDLE)mgTopmostLayer;

    /* reserve the first client slot for the server */
    __mg_client_id = __mg_client_add (0, getpid (), 0);
    mgClients[0].layer = mgTopmostLayer;

    /* obtain fd to listen for client requests on */
    if ((listenfd = serv_listen (CS_PATH)) < 0) {
        _ERR_PRINTF ("mginit: failed to open socket (%s) to listen: %m\n",
                CS_PATH);
        goto fail;
    }

    FD_SET (listenfd, &__mg_dsk_msg_queue->rfdset);
    __mg_dsk_msg_queue->maxfd = listenfd;
    maxi = -1;

    mg_InitTimer (TRUE);

    __mg_start_server_desktop ();

    __mg_screensaver_create();

#ifdef _MGSCHEMA_COMPOSITING
    if (!mg_InitCompositor ()) {
        _ERR_PRINTF ("mginit: failed to initialize compositor!\n");
        goto fail;
    }
    atexit (mg_TerminateCompositor);
#endif

    return TRUE;

fail:
    __mg_cleanup_layers ();
    return FALSE;
}

void server_ServerCleanup (void)
{
    __mg_cleanup_layers ();

    mg_TerminateTimer (TRUE);

    unlink (CS_PATH);
}

BOOL server_IdleHandler4Server (PMSGQUEUE msg_queue, BOOL wait)
{
    int    i, evt, clifd, nread;
    pid_t  pid;
    uid_t  uid;
    struct timeval sel_timeout = {0, 10000};
    struct timeval sel_timeout_nd = {0, 0};
    fd_set rset, wset, eset;
    fd_set* wsetptr = NULL;
    fd_set* esetptr = NULL;
    EXTRA_INPUT_EVENT extra;    // Since 4.0.0; for extra input events
    int nevts = 0;              // Since 5.0.0; for timer and fd events

#if 0   /* deprecated code */
    /* since 5.0.0, use __mg_update_tick_count instead */
    if (__mg_tick_counter != SHAREDRES_TIMER_COUNTER) {
        __mg_tick_counter = SHAREDRES_TIMER_COUNTER;
        AlertDesktopTimerEvent ();
    }
#endif  /* deprecated code */

    /* rset gets modified each time around */
    rset = __mg_dsk_msg_queue->rfdset;
    if (__mg_dsk_msg_queue->nr_wfds) {
        wset = __mg_dsk_msg_queue->wfdset;
        wsetptr = &wset;
    }
    if (__mg_dsk_msg_queue->nr_efds) {
        eset = __mg_dsk_msg_queue->efdset;
        esetptr = &eset;
    }

#ifdef _MGHAVE_CURSOR
    /* This call change the cursor position actually.
     * Under sharedfb schema, if the cursor has been hide by GDI function of
     * clients, this call will re-show the cursor.
     */
    kernel_ReShowCursor ();
#endif

#ifdef _MGSCHEMA_COMPOSITING
    __mg_composite_dirty_znodes ();
#endif

    extra.params_mask = 0;
    if ((evt = IAL_WaitEvent (__mg_dsk_msg_queue->maxfd, &rset, wsetptr, esetptr,
                wait?&sel_timeout:&sel_timeout_nd, &extra)) < 0) {

        /* It is time to check event again. */
        if (errno == EINTR) {
            if (wait)
                ParseEvent (msg_queue, 0);
            return FALSE;
        }
#ifdef _DEBUG
        __mg_err_msg ("select error on server");
#endif
    }

    if (FD_ISSET (listenfd, &rset)) {
        /* accept new client request */
        if ( (clifd = serv_accept (listenfd, &pid, &uid)) < 0) {
#ifdef _DEBUG
            __mg_err_msg ("serv_accept error: %d", clifd);
#endif
            return TRUE;
        }

        if ((i = __mg_client_add (clifd, pid, uid)) == -1) {
            /* can not accept this client */
            close (clifd);
            return TRUE;
        }
        if (OnNewDelClient) OnNewDelClient (LCO_NEW_CLIENT, i);

        FD_SET (clifd, &__mg_dsk_msg_queue->rfdset);
        if (clifd > __mg_dsk_msg_queue->maxfd)
            __mg_dsk_msg_queue->maxfd = clifd;  /* max fd for select() */
        if (i > maxi)
            maxi = i;       /* max index in client[] array */
#ifdef _DEBUG
        __mg_err_msg ("new connection: uid %d, fd %d", uid, clifd);
#endif
        return TRUE;
    }

    for (i = 0; i <= maxi; i++) {    /* go through client[] array */
        if ( (clifd = mgClients[i].fd) < 0)
            continue;
        if (FD_ISSET (clifd, &rset)) {
            int req_id;

            /* read request id from client */
            if ((nread = sock_read (clifd, &req_id, sizeof (int)))
                            == SOCKERR_IO) {
#ifdef _DEBUG
                __mg_err_msg ("server: read error on fd %d", clifd);
#endif
                if (OnNewDelClient) OnNewDelClient (LCO_DEL_CLIENT, i);
                __mg_remove_client (i, clifd);
            }
            else if (nread == SOCKERR_CLOSED) {
                if (OnNewDelClient) OnNewDelClient (LCO_DEL_CLIENT, i);
                __mg_remove_client (i, clifd);
            }
            else    /* process client's rquest */
                __mg_handle_request (clifd, req_id, i);
        }
    }

    /* handle intput event (mouse/touch-screen or keyboard) */
    if (evt & IAL_MOUSEEVENT) {
        ParseEvent (msg_queue, IAL_MOUSEEVENT);
        nevts++;
    }
    if (evt & IAL_KEYEVENT) {
        ParseEvent (msg_queue, IAL_KEYEVENT);
        nevts++;
    }
    if (evt & IAL_EVENT_EXTRA) {
        MSG msg;
        msg.hwnd = HWND_DESKTOP;
        msg.message = extra.event;
        msg.wParam = extra.wparam;
        msg.lParam = extra.lparam;
        msg.time = __mg_tick_counter;
        if (extra.params_mask) {
            // packed multiple sub events
            int i, n = 0;
            for (i = 0; i < NR_PACKED_SUB_EVENTS; i++) {
                if (extra.params_mask & (1 << i)) {
                    msg.wParam = extra.wparams[i];
                    msg.lParam = extra.lparams[i];
                    if (__mg_check_hook_func (HOOK_EVENT_EXTRA, &msg) ==
                            HOOK_GOON) {
                        kernel_QueueMessage (msg_queue, &msg);
                        nevts++;
                    }
                    n++;
                }
            }

            if (n > 0) {
                msg.message = MSG_EXIN_END_CHANGES;
                msg.wParam = n;
                msg.lParam = 0;
                if (__mg_check_hook_func (HOOK_EVENT_EXTRA, &msg) ==
                        HOOK_GOON) {
                    kernel_QueueMessage (msg_queue, &msg);
                    nevts++;
                }
            }
        }
        else {
            if (__mg_check_hook_func (HOOK_EVENT_EXTRA, &msg) == HOOK_GOON) {
                kernel_QueueMessage (msg_queue, &msg);
                nevts++;
            }
        }
    }
    else if (evt == 0) {
        ParseEvent (msg_queue, 0);
    }

    /* Since 5.0.0: Always check timers */
    nevts += __mg_check_expired_timers (msg_queue,
            SHAREDRES_TIMER_COUNTER - msg_queue->old_tick_count);
    msg_queue->old_tick_count = SHAREDRES_TIMER_COUNTER;

    /* go through registered listening fds */
    nevts += __mg_kernel_check_listen_fds (msg_queue, &rset, wsetptr, esetptr);
    return (nevts > 0);
}

