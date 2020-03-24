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
** client.c: routines for client.
**
** The idea comes from sample code in APUE.
** Thank Mr. Richard Stevens for his perfect work.
**
** Create date: 2000/12/20
*/

#include <sys/types.h>
#include <sys/shm.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "gal.h"
#include "cliprect.h"
#include "internals.h"
#include "timer.h"
#include "ourhdr.h"
#include "client.h"
#include "server.h"
#include "sockio.h"
#include "sharedres.h"
#include "drawsemop.h"
#include "cursor.h"

int __mg_client_id;

static int conn_fd = -1;         /* connected socket */

ON_LOCK_CLIENT_REQ  OnLockClientReq = NULL;
ON_TRYLOCK_CLIENT_REQ  OnTrylockClientReq = NULL;
ON_UNLOCK_CLIENT_REQ  OnUnlockClientReq = NULL;

#define TIMEOUT_START_REPEAT    30
#define TIMEOUT_REPEAT          5

#if 0   /* deprecated code */
/* set the timer to 10ms can make the client respond more faster */

static struct timeval my_timeout;
void __mg_set_select_timeout (unsigned int usec)
{
    my_timeout.tv_sec = 0;

#if 0
    if (usec > 0) {
        if (usec > USEC_10MS * TIMEOUT_START_REPEAT)
            my_timeout.tv_usec = USEC_10MS * TIMEOUT_REPEAT;
        else if (usec > USEC_10MS * TIMEOUT_REPEAT)
            my_timeout.tv_usec = USEC_10MS * TIMEOUT_REPEAT >> 1;
        else
            my_timeout.tv_usec = USEC_10MS;
    }
    else {
        my_timeout.tv_usec = USEC_10MS * TIMEOUT_START_REPEAT;
    }
#endif
    my_timeout.tv_usec = USEC_10MS;
}
#endif  /* deprecated code */

BOOL client_ClientStartup (void)
{
    struct sigaction siga;

    /* connect to server */
    if ((conn_fd = cli_conn (CS_PATH, 'a')) < 0)
        return FALSE;

    FD_SET (conn_fd, &__mg_dsk_msg_queue->rfdset);
    __mg_dsk_msg_queue->maxfd = conn_fd;

    // __mg_set_select_timeout (0);

    /* ignore the SIGPIPE signal */
    siga.sa_handler = SIG_IGN;
    siga.sa_flags = 0;
    memset (&siga.sa_mask, 0, sizeof (sigset_t));
    sigaction (SIGPIPE, &siga, NULL);

    return TRUE;
}

void client_ClientCleanup (void)
{
    close (conn_fd);
    conn_fd = -1;
}

int GUIAPI GetSockFD2Server (void)
{
    if (mgIsServer)
        return -1;

    return conn_fd;
}

#include "debug.h"

static void process_socket_message (MSG *msg)
{
    if (msg->message == MSG_UPDATECLIWIN &&
            __mg_client_check_hwnd (msg->hwnd, __mg_client_id)) {
        __mg_update_window (msg->hwnd,
                LOSWORD(msg->wParam), HISWORD(msg->wParam),
                LOSWORD(msg->lParam), HISWORD(msg->lParam));
    }
    else {
        if (msg->hwnd == HWND_NULL) {
            msg->hwnd = HWND_DESKTOP;
            //dump_message (msg, __func__);
            QueueDeskMessage (msg);
        }
        else if (__mg_client_check_hwnd (msg->hwnd, __mg_client_id)) {
            QueueDeskMessage (msg);
        }
    }
}

int GUIAPI ClientRequestEx2 (const REQUEST* request,
                const void* ex_data, size_t ex_data_len, int fd_to_send,
                void* result, size_t len_rslt, int* fd_received)
{
    int n;

    if (mgIsServer)
        return -1;

/*
#ifdef _MGGAL_MLSHADOW
    if ((request->id != REQID_MLSHADOW_CLIREQ)
            && (__mg_client_id == 0 && request->id != REQID_JOINLAYER)) {
#elif defined(_MGGAL_NEXUS)
    if ((request->id != REQID_NEXUS_HWSURFACE)
            && (__mg_client_id == 0 && request->id != REQID_JOINLAYER)) {
#elif defined(_MGSCHEMA_COMPOSITING)
    if (__mg_client_id == 0 && request->id != REQID_JOINLAYER &&
            request->id != REQID_GETSHAREDSURFACE) {
#else
    if (__mg_client_id == 0 && request->id != REQID_JOINLAYER) {
#endif
*/
    if (__mg_client_id == 0 && (request->id & REQMASK_JOINLAYERFIRST)) {
        _ERR_PRINTF ("CLIENT: please call JoinLayer first.\n");
        exit (255);
        return -1;
    }

    if (OnLockClientReq && OnUnlockClientReq)
        OnLockClientReq();

#if 0
    {
        size_t len_data;

        n = sock_write (conn_fd, &request->id, sizeof (int));
        if (n < 0) goto sock_error;

        if (ex_data_len <= 0 || ex_data == NULL) {
            ex_data = NULL;
            ex_data_len = 0;
        }

        len_data = request->len_data + ex_data_len;
        n = sock_write (conn_fd, &len_data, sizeof (size_t));
        if (n < 0) goto sock_error;

        n = sock_write (conn_fd, request->data, request->len_data);
        if (n < 0) goto sock_error;

        if (ex_data_len > 0) {
            n = sock_write (conn_fd, ex_data, ex_data_len);
            if (n < 0) goto sock_error;
        }
    }
#else /* use sendmsg */
    {
        struct iovec    iov[2];
        struct msghdr   msg;
        struct cmsghdr  *cmsg = NULL;

        n = sock_write (conn_fd, &request->id, sizeof (int));
        if (n < 0) goto sock_error;

        n = sock_write (conn_fd, &request->len_data, sizeof (size_t));
        if (n < 0) goto sock_error;

        n = sock_write (conn_fd, &ex_data_len, sizeof (size_t));
        if (n < 0) goto sock_error;

        iov[0].iov_base = (void*)request->data;
        iov[0].iov_len  = request->len_data;

        iov[1].iov_base = (void*)ex_data;
        iov[1].iov_len  = ex_data_len;

        msg.msg_iov     = iov;
        msg.msg_iovlen  = 2;
        msg.msg_name    = NULL;
        msg.msg_namelen = 0;

        if (fd_to_send >= 0) {
            cmsg = alloca (CMSG_LEN (sizeof (int)));

            cmsg->cmsg_level    = SOL_SOCKET;
            cmsg->cmsg_type     = SCM_RIGHTS;
            cmsg->cmsg_len      = CMSG_LEN (sizeof (int));
            memcpy (CMSG_DATA (cmsg), &fd_to_send, sizeof (int));

            msg.msg_control     = cmsg;
            msg.msg_controllen  = CMSG_LEN (sizeof (int));
        }
        else {
            msg.msg_control    = NULL;
            msg.msg_controllen = 0;
        }

        if ((n = sock_sendmsg (conn_fd, &msg, 0)) < 0)
            goto sock_error;
    }
#endif

    if (result == NULL || len_rslt == 0) {
        if (OnLockClientReq && OnUnlockClientReq)
            OnUnlockClientReq();
        return 0;
    }

    {
        MSG msg;

        do {
            n = sock_read (conn_fd, &msg, sizeof (MSG));
            if (n < 0) {
                goto sock_error;
            }

            if (msg.hwnd == HWND_INVALID) {
                break;
            }
            else {
                process_socket_message(&msg);
            }
        } while (TRUE);
    }

#if 0
    {
        n = sock_read (conn_fd, result, len_rslt);
        if (n < 0) goto sock_error;
    }
#else /* use recvmsg */
    {
        struct iovec    iov[1];
        struct msghdr   msg;
        struct cmsghdr  *cmsg = NULL;

        /* receive the real reply result */
        iov[0].iov_base = result;
        iov[0].iov_len  = len_rslt;

        msg.msg_iov     = iov;
        msg.msg_iovlen  = 1;
        msg.msg_name    = NULL;
        msg.msg_namelen = 0;

        if (fd_received) {
            *fd_received = -1;

            cmsg = alloca (CMSG_LEN (sizeof (int)));
            msg.msg_control     = cmsg;
            msg.msg_controllen  = CMSG_LEN (sizeof (int));
        }
        else {
            msg.msg_control     = NULL;
            msg.msg_controllen  = 0;
        }

        if ((n = sock_recvmsg (conn_fd, &msg, 0)) < 0) {
            goto sock_error;
        }

        if (fd_received && msg.msg_controllen == CMSG_LEN (sizeof (int))) {
            memcpy (fd_received, CMSG_DATA(cmsg), sizeof (int));
        }

        if (fd_received && *fd_received == -1) {
            _WRN_PRINTF ("Received an invalid file descriptor.\n");
        }
    }
#endif

    if (OnLockClientReq && OnUnlockClientReq)
        OnUnlockClientReq();

    return 0;

sock_error:
    if (n == SOCKERR_IO) {
        if (OnLockClientReq && OnUnlockClientReq)
            OnUnlockClientReq();
    }
    else if (n == SOCKERR_CLOSED) {
        if (OnLockClientReq && OnUnlockClientReq)
            OnUnlockClientReq();
        exit (255);
    }

    return -1;
}

static void check_live (void)
{
    REQUEST req;

#if 0   /* deprecated code */
    /* Since 5.0.0, call __mg_update_tick_count instead */
    if (__mg_tick_counter != SHAREDRES_TIMER_COUNTER) {
        AlertDesktopTimerEvent ();
        __mg_tick_counter = SHAREDRES_TIMER_COUNTER;
    }
#endif  /* deprecated code */

    __mg_update_tick_count (NULL);

    /* Tell server that I am live */
    req.id = REQID_IAMLIVE;
    req.data = &__mg_tick_counter;
    req.len_data = sizeof (unsigned int);
    ClientRequest (&req, NULL, 0);
}

BOOL client_IdleHandler4Client (PMSGQUEUE msg_queue, BOOL wait)
{
    static DWORD old_timer;
    static DWORD repeat_timeout = TIMEOUT_START_REPEAT;
    fd_set rset, wset, eset;
    fd_set* wsetptr = NULL;
    fd_set* esetptr = NULL;
    int n, nread;
    struct timeval sel_timeout;
    MSG Msg;

    check_live ();

    /* rset gets modified each time around */
    rset = msg_queue->rfdset;
    if (msg_queue->nr_wfds) {
        wset = msg_queue->wfdset;
        wsetptr = &wset;
    }
    if (msg_queue->nr_efds) {
        eset = msg_queue->efdset;
        esetptr = &eset;
    }

    if (wait) {
        sel_timeout.tv_sec = 0;
        sel_timeout.tv_usec = USEC_10MS;
    }
    else {  /* check fd only: for HavePendingMessage function */
        sel_timeout.tv_sec = 0;
        sel_timeout.tv_usec = 0;
    }

    if ((n = select (msg_queue->maxfd + 1,
            &rset, wsetptr, esetptr, &sel_timeout)) < 0) {
        if (errno == EINTR) {
            /* no event */
            return FALSE;
        }
        __mg_err_sys ("client: select error");
    }
    else if (n == 0) {
        static int old_mouse_x = -1, old_mouse_y = -1, old_buttons = -1;
        static unsigned int old_mouse_move_serial = 0xdeadbeef;
        int flag = 0, mouse_x = -1, mouse_y = -1, buttons = -1;

        check_live ();
        if (wait && (__mg_tick_counter - old_timer) >= repeat_timeout) {
            Msg.hwnd = HWND_DESKTOP;
            Msg.message = MSG_TIMEOUT;
            Msg.wParam = (WPARAM)__mg_tick_counter;
            Msg.lParam = 0;
            Msg.time = __mg_tick_counter;
            // Since 5.0.0, we do not genenrate MSG_TIMEOUT message any more.
            // kernel_QueueMessage (msg_queue, &Msg);

            old_timer = __mg_tick_counter;
            repeat_timeout = TIMEOUT_REPEAT;
        }

        LOCK_MOUSEMOVE_SEM();
        if (SHAREDRES_MOUSEMOVECLIENT == __mg_client_id
                && SHAREDRES_MOUSEMOVESERIAL != old_mouse_move_serial) {
            mouse_x = SHAREDRES_MOUSEX;
            mouse_y = SHAREDRES_MOUSEY;
            buttons = SHAREDRES_SHIFTSTATUS;
            flag = 1;
        }
        old_mouse_move_serial = SHAREDRES_MOUSEMOVESERIAL;
        UNLOCK_MOUSEMOVE_SEM();

        if (flag && (mouse_x != old_mouse_x || mouse_y != old_mouse_y ||
                    buttons != old_buttons)) {
            MSG msg;
            old_mouse_x = mouse_x;
            old_mouse_y = mouse_y;
            old_buttons = buttons;

            msg.hwnd = HWND_DESKTOP;
            msg.message = MSG_MOUSEMOVE;
            msg.wParam = buttons;
            msg.lParam = MAKELONG(mouse_x, mouse_y);
            kernel_QueueMessage (msg_queue, &msg);
            n++;

            //dump_message (&msg, __func__);
        }
    }

    /* Since 5.0.0: always check timer */
    n += __mg_check_expired_timers (msg_queue,
            SHAREDRES_TIMER_COUNTER - msg_queue->old_tick_count);
    msg_queue->old_tick_count = SHAREDRES_TIMER_COUNTER;

    if (FD_ISSET (conn_fd, &rset) &&
        (!OnTrylockClientReq || !OnUnlockClientReq ||
         (OnTrylockClientReq && OnUnlockClientReq &&
            !OnTrylockClientReq()))) {

        if ((nread = sock_read (conn_fd, &Msg, sizeof (MSG))) < 0) {
            if (OnTrylockClientReq && OnUnlockClientReq)
                OnUnlockClientReq();
            __mg_err_sys ("client: read error on fd %d", conn_fd);
        }
        else if (nread == 0) {
            if (OnTrylockClientReq && OnUnlockClientReq)
                OnUnlockClientReq();
            __mg_err_sys ("client: server closed");
            close (conn_fd);
        }
        else {           /* process event from server */
            if (OnTrylockClientReq && OnUnlockClientReq)
                OnUnlockClientReq();
            process_socket_message (&Msg);
        }
    }

    /* go through registered listen fds */
    n += __mg_kernel_check_listen_fds (msg_queue, &rset, wsetptr, esetptr);

    check_live ();

    if (n > 0) {
        old_timer = __mg_tick_counter;
        repeat_timeout = TIMEOUT_START_REPEAT;
    }

    return (n > 0);
}

GHANDLE GUIAPI JoinLayer (const char* layer_name, const char* client_name,
               int max_nr_topmosts, int max_nr_normals)
{
    GHANDLE layer_handle = INV_LAYER_HANDLE;
    REQUEST req;
    JOINLAYERINFO info;
    JOINEDCLIENTINFO joined_info;

    if (mgIsServer)
        return layer_handle;

    if (__mg_client_id != 0) /* Already joined, return the handle */
        return GetLayerInfo (NAME_SELF_LAYER, NULL, NULL, NULL);

    if (layer_name) {
        strncpy (info.layer_name, layer_name, LEN_LAYER_NAME);
        info.layer_name [LEN_LAYER_NAME] = '\0';
    }
    else {
        /* treat as the topmost layer */
        info.layer_name [0] = '\0';
    }

    if (client_name) {
        strncpy (info.client_name, client_name, LEN_CLIENT_NAME);
        info.client_name [LEN_CLIENT_NAME] = '\0';
    }
    else
        info.client_name [0] = '\0';

    if (max_nr_topmosts <= 0) max_nr_topmosts = DEF_NR_TOPMOSTS;
    if (max_nr_normals <= 0) max_nr_normals = DEF_NR_NORMALS;

    info.max_nr_topmosts = (max_nr_topmosts + 7) & ~0x07;
    info.max_nr_normals = (max_nr_normals + 7) & ~0x07;

    req.id = REQID_JOINLAYER;
    req.data = &info;
    req.len_data = sizeof (JOINLAYERINFO);

    if (ClientRequest (&req, &joined_info, sizeof (JOINEDCLIENTINFO)) < 0)
        goto ret;

    __mg_client_id = joined_info.cli_id;

    layer_handle = joined_info.layer;

    if (layer_handle != INV_LAYER_HANDLE) {
        ZORDERINFO* zi;

        __mg_layer = layer_handle;

        zi  = (ZORDERINFO*) shmat (joined_info.zo_shmid, 0, SHM_RDONLY);
        if (zi == (void*)-1) {
            return INV_LAYER_HANDLE;
        }

        __mg_zorder_info = zi;
    }

    __mg_tick_counter = SHAREDRES_TIMER_COUNTER;

    __mg_start_client_desktop ();

ret:
    return layer_handle;
}

GHANDLE GUIAPI GetLayerInfo (const char* layer_name,
                int* nr_clients, BOOL* is_topmost, int* cli_active)
{
    if (mgIsServer)
        return INV_LAYER_HANDLE;

    if (layer_name) {
        LAYERINFO info;
        REQUEST req;

        req.id = REQID_LAYERINFO;
        req.data = layer_name;
        req.len_data = strlen (layer_name) + 1;

        if (ClientRequest (&req, &info, sizeof (LAYERINFO)) < 0)
            goto ret;

        if (info.handle) {
            if (nr_clients) *nr_clients = info.nr_clients;
            if (is_topmost) *is_topmost = info.is_topmost;
            if (cli_active) *cli_active = info.cli_active;
        }

        return info.handle;
    }

ret:
    return INV_LAYER_HANDLE;
}

BOOL GUIAPI SetTopmostLayer (BOOL handle_name,
                GHANDLE handle, const char* name)
{
    BOOL ret = FALSE;
    LAYEROPINFO info;
    REQUEST req;

    if (mgIsServer)
        return FALSE;

    if (!handle_name && name == NULL)
        return FALSE;

    if (handle_name && handle == INV_LAYER_HANDLE)
        return FALSE;

    info.id_op = ID_LAYEROP_SETTOP;
    info.handle_name = handle_name;

    if (handle_name) {
        info.layer.handle = handle;
    }
    else {
        strncpy (info.layer.name, name, LEN_LAYER_NAME);
        info.layer.name [LEN_LAYER_NAME] = '\0';
    }

    req.id = REQID_LAYEROP;
    req.data = &info;
    req.len_data = sizeof (LAYEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (BOOL)) < 0)
        ret = FALSE;

    return ret;
}

BOOL GUIAPI DeleteLayer (BOOL handle_name,
                GHANDLE handle, const char* name)
{
    BOOL ret = FALSE;
    LAYEROPINFO info;
    REQUEST req;

    if (mgIsServer)
        return FALSE;

    if (!handle_name && name == NULL)
        return FALSE;

    if (handle_name && handle == INV_LAYER_HANDLE)
        return FALSE;

    info.id_op = ID_LAYEROP_DELETE;
    info.handle_name = handle_name;

    if (handle_name) {
        info.layer.handle = handle;
    }
    else {
        strncpy (info.layer.name, name, LEN_LAYER_NAME);
        info.layer.name [LEN_LAYER_NAME] = '\0';
    }

    req.id = REQID_LAYEROP;
    req.data = &info;
    req.len_data = sizeof (LAYEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (BOOL)) < 0)
        ret = FALSE;

    return ret;
}

/* Since 5.0.0 */
BOOL __mg_client_on_layer_changed (GHANDLE layer_handle, int zi_shmid)
{
    if (layer_handle != INV_LAYER_HANDLE) {
        ZORDERINFO* zi;

        __mg_layer = INV_LAYER_HANDLE;
        __mg_zorder_info = NULL;

        if (shmdt (__mg_zorder_info) < 0) {
            _ERR_PRINTF ("Failed to detach from the shared zorder info: %m\n");
            return FALSE;
        }

        zi  = (ZORDERINFO*) shmat (zi_shmid, 0, SHM_RDONLY);
        if (zi == (void*)-1) {
            _ERR_PRINTF ("Failed to attach to the shared zorder info: %m\n");
            return FALSE;
        }

        __mg_layer = layer_handle;
        __mg_zorder_info = zi;
        return TRUE;
    }

    return FALSE;
}

/* Since 5.0.0 */
BOOL GUIAPI MoveToLayer (BOOL handle_name, GHANDLE handle, const char* name)
{
    MOVETOLAYERINFO info;
    MOVEDCLIENTINFO moved_info;
    REQUEST req;

    if (mgIsServer)
        return FALSE;

    if (!handle_name && name == NULL)
        return FALSE;

    if (handle_name && handle == INV_LAYER_HANDLE)
        return FALSE;

    info.handle_name = handle_name;
    if (handle_name) {
        info.layer.handle = handle;
    }
    else {
        strncpy (info.layer.name, name, LEN_LAYER_NAME);
        info.layer.name [LEN_LAYER_NAME] = '\0';
    }

    req.id = REQID_MOVETOLAYER;
    req.data = &info;
    req.len_data = sizeof (MOVETOLAYERINFO);

    if (ClientRequest (&req, &moved_info, sizeof (MOVEDCLIENTINFO)) < 0)
        return FALSE;

    return __mg_client_on_layer_changed (moved_info.layer, moved_info.zo_shmid);
}

