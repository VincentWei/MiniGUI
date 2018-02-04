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
#include "license.h"

extern DWORD __mg_timer_counter;

ON_NEW_DEL_CLIENT OnNewDelClient = NULL;

static SRVEVTHOOK srv_evt_hook = NULL;

SRVEVTHOOK GUIAPI SetServerEventHook (SRVEVTHOOK SrvEvtHook)
{
    SRVEVTHOOK old_hook = srv_evt_hook;

    if (!mgIsServer)
        return NULL;

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

    ke->status = 0L;
    me->status = 0L;
    lwe.status = 0L;

    if (!kernel_GetLWEvent (event, &lwe))
        return;

    Msg.time = __mg_timer_counter;
    if (lwe.type == LWETYPE_TIMEOUT) {
        Msg.message = MSG_TIMEOUT;
        Msg.wParam = (WPARAM)lwe.count;
        Msg.lParam = 0;

        /*
         * No need to send TIME_OUT message to the client
         * Send2Client (&Msg, CLIENT_ACTIVE);
         */
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

        if (__mg_do_drag_drop_window (Msg.message, 0, 0))
            return;

        if (!(srv_evt_hook && srv_evt_hook (&Msg))) {
            kernel_QueueMessage (msg_que, &Msg);
        }
    }
    else if (lwe.type == LWETYPE_MOUSE) {
        static int down_client = -1;
        static int down_by;
        int cur_client = __mg_get_znode_at_point (__mg_zorder_info, 
                        me->x, me->y, NULL);
        Msg.wParam = me->status;
        switch (me->event) {
        case ME_MOVED:
            Msg.message = MSG_MOUSEMOVE;
            break;
        case ME_LEFTDOWN:
            Msg.message = MSG_LBUTTONDOWN;
            break;
        case ME_RIGHTDOWN:
            Msg.message = MSG_RBUTTONDOWN;
            break;
        case ME_LEFTUP:
            Msg.message = MSG_LBUTTONUP;
            break;
        case ME_LEFTDBLCLICK:
            Msg.message = MSG_LBUTTONDBLCLK;
            break;
        case ME_RIGHTUP:
            Msg.message = MSG_RBUTTONUP;
            break;
        case ME_RIGHTDBLCLICK:
            Msg.message = MSG_RBUTTONDBLCLK;
            break;
        }

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

        Msg.lParam = MAKELONG (me->x, me->y);

        if (__mg_handle_mouse_hook (Msg.message, 
                                Msg.wParam, Msg.lParam) == HOOK_STOP)
            return;

        if (!(srv_evt_hook && srv_evt_hook (&Msg))) {
            int target_client = cur_client;

            if (down_client < 0) {
                if (!__mg_capture_wnd && me->event == ME_MOVED) {
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
                    lock_mousemove_sem();
                    {
                        if (SHAREDRES_MOUSEMOVECLIENT > 0 && SHAREDRES_MOUSEMOVECLIENT != target_client) {
                            printf("drop a mouse move message, old_client=%d, target_client=%d\n", SHAREDRES_MOUSEMOVECLIENT, target_client);
                        }
                    }
                    SHAREDRES_MOUSEMOVECLIENT = target_client;
                    ++ SHAREDRES_MOUSEMOVESERIAL;
                    unlock_mousemove_sem();
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
    def_nr_topmosts = (def_nr_topmosts + 7) & ~0x07;
    def_nr_normals = (def_nr_normals + 7) & ~0x07;

    SHAREDRES_NR_GLOBALS = nr_globals;
    SHAREDRES_DEF_NR_TOPMOSTS = def_nr_topmosts;
    SHAREDRES_DEF_NR_NORMALS = def_nr_normals;

    if (__mg_init_layers () == -1)
        return FALSE;

    SHAREDRES_NR_LAYSERS = 1;
    mgTopmostLayer = mgLayers;
    SHAREDRES_TOPMOST_LAYER = (GHANDLE)mgTopmostLayer;

    /* reserve the first client slot for the server */
    __mg_client_id = __mg_client_add (0, getpid (), 0);
    mgClients [0].layer = mgTopmostLayer;

    /* obtain fd to listen for client requests on */
    if ( (listenfd = serv_listen (CS_PATH)) < 0)
        goto fail;

    FD_ZERO (&mg_rfdset);
    FD_SET (listenfd, &mg_rfdset);
    mg_maxfd = listenfd;
    maxi = -1;

    mg_InitTimer ();

    __mg_start_server_desktop ();

    screensaver_create();

    return TRUE;

fail:
    __mg_cleanup_layers ();
    return FALSE;
}

void server_ServerCleanup (void)
{
    __mg_cleanup_layers ();

    mg_UninstallIntervalTimer ();

    unlink (CS_PATH);
}

BOOL server_IdleHandler4Server (PMSGQUEUE msg_queue)
{
    int    i, n, clifd, nread;
    pid_t  pid;
    uid_t  uid;
    struct timeval sel_timeout = {0, 0};
    fd_set rset, wset, eset;
    fd_set* wsetptr = NULL;
    fd_set* esetptr = NULL;

    if (__mg_timer_counter != SHAREDRES_TIMER_COUNTER) {
        __mg_timer_counter = SHAREDRES_TIMER_COUNTER;
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

#ifdef _MGHAVE_CURSOR
    /* if the cursor has been hide by GDI function of clients
     * this call will show the cursor
     */
    kernel_ReShowCursor ();
#endif

    if ( (n = IAL_WaitEvent (IAL_MOUSEEVENT | IAL_KEYEVENT, 
                mg_maxfd, &rset, wsetptr, esetptr, 
                msg_queue?NULL:(&sel_timeout))) < 0) {

        /* It is time to check event again. */
        if (errno == EINTR) {
            if (msg_queue)
                ParseEvent (msg_queue, 0);
            return FALSE;
        }
#ifdef _DEBUG
        err_msg ("select error on server");
#endif
    }

    if (FD_ISSET (listenfd, &rset)) {
        /* accept new client request */
        if ( (clifd = serv_accept (listenfd, &pid, &uid)) < 0) {
#ifdef _DEBUG
            err_msg ("serv_accept error: %d", clifd);
#endif
            return TRUE;
        }

        if ((i = __mg_client_add (clifd, pid, uid)) == -1) {
            /* can not accept this client */
            close (clifd);
            return TRUE;
        }
        if (OnNewDelClient) OnNewDelClient (LCO_NEW_CLIENT, i);

        FD_SET (clifd, &mg_rfdset);
        if (clifd > mg_maxfd)
            mg_maxfd = clifd;  /* max fd for select() */
        if (i > maxi)
            maxi = i;       /* max index in client[] array */
#ifdef _DEBUG
        err_msg ("new connection: uid %d, fd %d", uid, clifd);
#endif
        return TRUE;
    }

    for (i = 0; i <= maxi; i++) {    /* go through client[] array */
        if ( (clifd = mgClients [i].fd) < 0)
            continue;
        if (FD_ISSET (clifd, &rset)) {
            int req_id;

            /* read request id from client */
            if ( (nread = sock_read (clifd, &req_id, sizeof (int))) 
                            == SOCKERR_IO) {
#ifdef _DEBUG
                err_msg ("server: read error on fd %d", clifd);
#endif
                if (OnNewDelClient) OnNewDelClient (LCO_DEL_CLIENT, i);
                __mg_remove_client (i, clifd);
            }
            else if (nread == SOCKERR_CLOSED) {
                if (OnNewDelClient) OnNewDelClient (LCO_DEL_CLIENT, i);
                __mg_remove_client (i, clifd);
            } else            /* process client's rquest */
                __mg_handle_request (clifd, req_id, i);
        }
    }

    if (msg_queue == NULL)
        return (n > 0);

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

            if (temp && FD_ISSET (mg_listen_fds [i].fd, temp)) {
                Msg.hwnd = (HWND)mg_listen_fds [i].hwnd;
                Msg.wParam = MAKELONG (mg_listen_fds [i].fd, type);
                Msg.lParam = (LPARAM)mg_listen_fds [i].context;
                kernel_QueueMessage (msg_queue, &Msg);
            }
        }
    }

    return TRUE;
}

