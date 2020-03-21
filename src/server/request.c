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
** request.c: handle request of clients.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/12/21
**
** NOTE: The idea comes from sample code in APUE.
*/

#include <signal.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "internals.h"
#include "cursor.h"
#include "gal.h"

#include "ourhdr.h"
#include "sockio.h"
#include "client.h"
#include "server.h"
#include "sharedres.h"
#include "misc.h"

typedef void (* ReleaseProc) (void* );

struct GlobalRes
{
    struct GlobalRes* next;
    struct GlobalRes* prev;

    void* key;
    void* res;
    ReleaseProc release_proc;
};

static void add_global_res (int cli, void* key,
                void* res, ReleaseProc release_proc)
{
    MG_Client* client = mgClients + cli;
    struct GlobalRes *global_res = malloc (sizeof (struct GlobalRes));

    if (global_res) {
        global_res->key = key;
        global_res->res = res;
        global_res->release_proc = release_proc;

        if (client->global_res == NULL) {
            global_res->next = NULL;
            global_res->prev = NULL;
            client->global_res = global_res;
        }
        else {
            global_res->next = client->global_res;
            global_res->prev = NULL;
            global_res->next->prev = global_res;
            client->global_res = global_res;
        }
    }
}

#ifdef _MGGAL_MLSHADOW
extern int MLSHADOW_Server(void* request, void* reply);
extern void srvMLSHADOW_DelSurface(void *res);
#endif

static void del_global_res (int cli, void* key, void* res)
{
    MG_Client* client = mgClients + cli;
    struct GlobalRes *global_res = client->global_res, *next;

    while (global_res) {
        next = global_res->next;

        if (global_res->key == key && global_res->res == res) {
            if (global_res->release_proc)
                global_res->release_proc (global_res->res);

            if (global_res->prev)
                global_res->prev->next = global_res->next;
            if (global_res->next)
                global_res->next->prev = global_res->prev;

            if (global_res == client->global_res) {
                client->global_res = global_res->next;
            }

            free (global_res);
            break;
        }

        global_res = next;
    }
}

void __mg_release_global_res (int cli)
{
    MG_Client* client = mgClients + cli;
    struct GlobalRes *global_res = client->global_res, *next;

    while (global_res) {
        next = global_res->next;

        if (global_res->release_proc)
            global_res->release_proc (global_res->res);
        free (global_res);

        global_res = next;
    }
}

int GUIAPI ServerSendReplyEx (int clifd,
                const void* reply, int len, int fd_to_send)
{
    if (!mgIsServer)
        return SOCKERR_IO;

    {
        MSG msg = {HWND_INVALID, 0};

        /* send a reply message to indicate this is a reply of request */
        if (sock_write (clifd, &msg, sizeof (MSG)) < 0)
            return SOCKERR_IO;
    }

#if 0
    {
        if (sock_write (clifd, reply, len) < 0)
            return SOCKERR_IO;

        return SOCKERR_OK;
    }
#else /* use sendmsg */
    {
        struct iovec    iov[1];
        struct msghdr   msg;
        struct cmsghdr  *cmsg = NULL;

        iov[0].iov_base = (void*)reply;
        iov[0].iov_len  = len;

        msg.msg_iov     = iov;
        msg.msg_iovlen  = 1;
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

        return sock_sendmsg (clifd, &msg, 0);
    }
#endif
}

static int load_cursor (int cli, int clifd, void* buff, size_t len)
{
    HCURSOR hcsr;

    hcsr = LoadCursorFromFile (buff);

#ifdef _MGHAVE_CURSOR
    if (hcsr) {
        add_global_res (cli, hcsr, hcsr, (ReleaseProc)DestroyCursor);
    }
#endif
    return ServerSendReply (clifd, &hcsr, sizeof (HCURSOR));
}

#if IS_COMPOSITING_SCHEMA
static int load_cursor_png_file (int cli, int clifd, void* buff, size_t len)
{
    int hotspot[2];
    HCURSOR hcsr = 0;

    if (len <= (sizeof (hotspot) + 1)) // length of file name is 0.
        goto ret;

    memcpy (hotspot, buff, sizeof(hotspot));
    hcsr = LoadCursorFromPNGFile (buff + sizeof(hotspot), hotspot[0], hotspot[1]);

#ifdef _MGHAVE_CURSOR
    if (hcsr) {
        add_global_res (cli, hcsr, hcsr, (ReleaseProc)DestroyCursor);
    }
#endif

ret:
    return ServerSendReply (clifd, &hcsr, sizeof (HCURSOR));
}

static int load_cursor_png_mem (int cli, int clifd, void* buff, size_t len)
{
    int hotspot[2];
    HCURSOR hcsr;

    /* check whether has enough PNG data.
     * see: https://garethrees.org/2007/11/14/pngcrush/
     */
    if (len < (sizeof (hotspot) + 67))
        goto ret;

    memcpy (hotspot, buff, sizeof(hotspot));
    hcsr = LoadCursorFromPNGMem (buff + sizeof(hotspot), len - sizeof (hotspot),
            hotspot[0], hotspot[1]);

#ifdef _MGHAVE_CURSOR
    if (hcsr) {
        add_global_res (cli, hcsr, hcsr, (ReleaseProc)DestroyCursor);
    }
#endif

ret:
    return ServerSendReply (clifd, &hcsr, sizeof (HCURSOR));
}

static void my_release_sem_for_shared_surf (void* res)
{
    int sem_num = (int)(intptr_t)res;

    if (__mg_free_sem_for_shared_surf (sem_num)) {
        _WRN_PRINTF("Failed to call __mg_free_sem_for_shared_surf (%d)\n",
                        sem_num);
    }
}

static int alloc_sem_for_shared_surf (int cli, int clifd, void* buff, size_t len)
{
    int sem_num;

    sem_num = __mg_alloc_sem_for_shared_surf ();
    if (sem_num >= 0) {
        add_global_res (cli, (void*)alloc_sem_for_shared_surf,
                         (void*)(intptr_t)sem_num,
                         my_release_sem_for_shared_surf);
    }

    return ServerSendReply (clifd, &sem_num, sizeof (int));
}

static int free_sem_for_shared_surf (int cli, int clifd, void* buff, size_t len)
{
    int ret_value = 0;
    int sem_num;

    sem_num = *((int*)buff);
    del_global_res (cli, (void*)alloc_sem_for_shared_surf,
                    (void*)(intptr_t)sem_num);

    return ServerSendReply (clifd, &ret_value, sizeof (int));
}

#endif /* IS_COMPOSITING_SCHEMA */

static int move_to_layer (int cli, int clifd, void* buff, size_t len)
{
    MOVETOLAYERINFO* info = (MOVETOLAYERINFO*)buff;
    MOVEDCLIENTINFO moved_info = { INV_LAYER_HANDLE };
    MG_Layer* dst_layer;

    if (!info->handle_name) {
        if (info->layer.name[0] == '\0')
            dst_layer = mgClients [cli].layer;
        else
            dst_layer = __mg_find_layer_by_name (info->layer.name);
    }
    else
        dst_layer = (MG_Layer*)info->layer.handle;

    if (dst_layer == NULL)
        goto ret;

    if (!__mg_is_valid_layer (dst_layer))
        goto ret;

    if (__mg_move_client_to_layer (mgClients + cli, dst_layer)) {
        moved_info.layer = dst_layer;
        moved_info.zo_shmid = dst_layer->zorder_shmid;
    }

ret:
    return ServerSendReply (clifd, &moved_info, sizeof (MOVEDCLIENTINFO));
}

static int calc_position (int cli, int clifd, void* buff, size_t len)
{
    CALCPOSINFO* info = (CALCPOSINFO*)buff;

    SendMessage (HWND_DESKTOP, MSG_CALC_POSITION, (WPARAM)cli, (LPARAM)info);

    return ServerSendReply (clifd, &info->rc, sizeof (RECT));
}

static int create_cursor (int cli, int clifd, void* buff, size_t len)
{
    HCURSOR hcsr;
    int* tmp;
    BYTE* and_bits, *xor_bits;

    tmp = (int*)buff;
    and_bits = (BYTE*)(tmp + 6);
    xor_bits = and_bits + tmp [5];

    hcsr = CreateCursor (tmp [0], tmp [1], tmp [2], tmp [3],
                    and_bits, xor_bits, tmp [4]);

#ifdef _MGHAVE_CURSOR
    if (hcsr) {
        add_global_res (cli, hcsr, hcsr, NULL);
    }
#endif
    return ServerSendReply (clifd, &hcsr, sizeof (HCURSOR));
}

static int copy_cursor (int cli, int clifd, void* buff, size_t len)
{
    HCURSOR hcsr = CopyCursor ((HCURSOR)*(intptr_t*)buff);
#ifdef _MGHAVE_CURSOR
    if (hcsr) {
        add_global_res (cli, hcsr, hcsr, NULL);
    }
#endif
    return ServerSendReply (clifd, &hcsr, sizeof (HCURSOR));
}

static int destroy_cursor (int cli, int clifd, void* buff, size_t len)
{
    BOOL ret_value;
    HCURSOR hcsr;

    hcsr = *((HCURSOR*)buff);

    ret_value = DestroyCursor(hcsr);
    del_global_res (cli, hcsr, hcsr);

    return ServerSendReply (clifd, &ret_value, sizeof (BOOL));
}

static int clip_cursor (int cli, int clifd, void* buff, size_t len)
{
    RECT cliprc;

    memcpy (&cliprc, buff, sizeof (RECT));
    ClipCursor (&cliprc);

    return SOCKERR_OK;
}

static int get_clip_cursor (int cli, int clifd, void* buff, size_t len)
{
    RECT cliprc;

    GetClipCursor (&cliprc);

    return ServerSendReply (clifd, &cliprc, sizeof (RECT));
}

static int set_cursor (int cli, int clifd, void* buff, size_t len)
{
    HCURSOR hcsr;
    HCURSOR old;

    memcpy (&hcsr, buff, sizeof (HCURSOR));

    old = SetCursorEx (hcsr, FALSE);

    return ServerSendReply (clifd, &old, sizeof (HCURSOR));
}

static int get_current_cursor (int cli, int clifd, void* buff, size_t len)
{
    HCURSOR hcsr;

    hcsr = GetCurrentCursor ();

    return ServerSendReply (clifd, &hcsr, sizeof (HCURSOR));
}

#if 0
static int show_cursor_for_gdi (int cli, int clifd, void* buff, size_t len)
{
    RECT rc;
    BOOL show_hide;
    BOOL ret_value = TRUE;

    memcpy (&show_hide, buff, sizeof (BOOL));
    memcpy (&rc, buff + sizeof (BOOL), sizeof (RECT));

    ShowCursorForClientGDI (show_hide, &rc);

    return ServerSendReply (clifd, &ret_value, sizeof (BOOL));
}
#endif

static int show_cursor (int cli, int clifd, void* buff, size_t len)
{
    BOOL show_hide;
    int ret_value;

    memcpy (&show_hide, buff, sizeof (BOOL));
    ret_value = ShowCursor (show_hide);

    return ServerSendReply (clifd, &ret_value, sizeof (BOOL));
}

static int set_cursor_pos (int cli, int clifd, void* buff, size_t len)
{
    POINT pt;

    memcpy (&pt, buff, sizeof (POINT));

    SetCursorPos (pt.x, pt.y);

    return SOCKERR_OK;
}

static int layer_info (int cli, int clifd, void* buff, size_t len)
{
    LAYERINFO info;

    __mg_get_layer_info (cli, (const char*) buff, &info);

    return ServerSendReply (clifd, &info, sizeof (LAYERINFO));
}

static int join_layer (int cli, int clifd, void* buff, size_t len)
{
    static int nr_layers = 0, nr_clients = 0;
    JOINLAYERINFO* info;
    JOINEDCLIENTINFO joined_info = {INV_LAYER_HANDLE};

    info = (JOINLAYERINFO*) buff;

    if (info->layer_name [0] == '\0') {
        strcpy (info->layer_name, mgTopmostLayer->name);
        nr_layers ++;
    }
    if (info->client_name [0] == '\0') {
        sprintf (info->client_name, "Client-%d", nr_clients);
        nr_clients ++;
    }
    __mg_client_join_layer (cli, info, &joined_info);

    return ServerSendReply (clifd, &joined_info, sizeof (JOINEDCLIENTINFO));
}

static int layer_op (int cli, int clifd, void* buff, size_t len)
{
    BOOL ret_value = FALSE;
    LAYEROPINFO* info = (LAYEROPINFO*)buff;
    MG_Layer* layer;

    if (!info->handle_name) {
        if (info->layer.name[0] == '\0')
            layer = mgClients [cli].layer;
        else
            layer = __mg_find_layer_by_name (info->layer.name);
    }
    else
        layer = (MG_Layer*)info->layer.handle;

    if (layer == NULL)
        goto ret;

    if (!__mg_is_valid_layer (layer))
        goto ret;

    switch (info->id_op) {
        case ID_LAYEROP_DELETE:
            ret_value  = ServerDeleteLayer (layer);
            break;

        case ID_LAYEROP_SETTOP:
            ret_value = ServerSetTopmostLayer (layer);
            break;
    }

ret:
    return ServerSendReply (clifd, &ret_value, sizeof (BOOL));
}

/* Since 5.0.0: handle fd received for shared surface */
static int zorder_op (int cli, int clifd, void* buff, size_t len, int fd)
{
    intptr_t ret_value;
    ZORDEROPINFO* info = (ZORDEROPINFO*)buff;

    ret_value = __mg_do_zorder_operation (cli, info,
                buff + sizeof (ZORDEROPINFO), fd);

    return ServerSendReply (clifd, &ret_value, sizeof (intptr_t));
}

static int change_zorder_maskrect (int cli, int clifd, void* buff, size_t len)
{
    intptr_t ret_value;
    ZORDERMASKRECTOPINFO* info = (ZORDERMASKRECTOPINFO *)buff;
    info->rc = (RECT4MASK*)(buff + sizeof (ZORDERMASKRECTOPINFO));
    ret_value = __mg_do_zorder_maskrect_operation (cli, info);

    return ServerSendReply (clifd, &ret_value, sizeof (intptr_t));
}

#ifdef _MGGAL_SIGMA8654
extern int Sigma8654_ServerOnGetSurface(REQ_SIGMA8654_GETSURFACE *request,
            REP_SIGMA8654_GETSURFACE *reply);
static int sigma8654_client_get_surface(int cli, int clifd, void* buff, size_t len)
{
    REQ_SIGMA8654_GETSURFACE *request;
    REP_SIGMA8654_GETSURFACE reply;

    if (Sigma8654_ServerOnGetSurface(request, &reply) < 0)
    {
        fprintf(stderr, "Nexus_ServerOnGetSurface() failed\n");
        return -1;
    }
    return ServerSendReply (clifd, &reply, sizeof (reply));
}
#endif


#ifdef _MGGAL_NEXUS
extern int Nexus_ServerOnGetSurface(REQ_NEXUS_GETSURFACE *request,
            REP_NEXUS_GETSURFACE *reply);
static int nexus_client_get_surface(int cli, int clifd, void* buff, size_t len)
{
    REQ_NEXUS_GETSURFACE *request;
    REP_NEXUS_GETSURFACE reply;

    if (Nexus_ServerOnGetSurface(request, &reply) < 0)
    {
        fprintf(stderr, "Nexus_ServerOnGetSurface() failed\n");
        return -1;
    }
    return ServerSendReply (clifd, &reply, sizeof (reply));
}
#endif

static int im_live (int cli, int clifd, void* buff, size_t len)
{
    unsigned int time;

    memcpy (&time, buff, sizeof (unsigned int));
    mgClients [cli].last_live_time = time;

    return SOCKERR_OK;
}

extern HWND __mg_ime_wnd;

static int open_ime_wnd (int cli, int clifd, void* buff, size_t len)
{
    BOOL open;

    memcpy (&open, buff, sizeof (BOOL));
    if (__mg_ime_wnd) {
        if (open)
            SendNotifyMessage (__mg_ime_wnd, MSG_IME_OPEN, 0, 0);
        else
            SendNotifyMessage (__mg_ime_wnd, MSG_IME_CLOSE, 0, 0);
    }

    return SOCKERR_OK;
}

static int register_hook (int cli, int clifd, void* buff, size_t len)
{
    REGHOOKINFO* info;
    int ret_value;

    info = (REGHOOKINFO*)buff;
    ret_value = __mg_do_reghook_operation (cli, info);

    return ServerSendReply (clifd, &ret_value, sizeof (int));
}

static int set_ime_stat (int cli, int clifd, void* buff, size_t len)
{
    int ret, data;

    memcpy(&data, buff, sizeof(int));

    ret = SendMessage (HWND_DESKTOP, MSG_IME_SETSTATUS,
            HISWORD(data), LOSWORD(data));
    return ServerSendReply (clifd, &ret, sizeof (int));
}

static int get_ime_stat (int cli, int clifd, void* buff, size_t len)
{
    int ret, data;

    memcpy(&data, buff, sizeof(int));

    ret = SendMessage (HWND_DESKTOP, MSG_IME_GETSTATUS, (WPARAM)data, 0);
    return ServerSendReply (clifd, &ret , sizeof (int));
}

static int set_ime_targetinfo (int cli, int clifd, void* buff, size_t len)
{
    int ret;
    IME_TARGET_INFO data;

    memcpy (&data, buff, sizeof (IME_TARGET_INFO));

    ret = SendMessage (HWND_DESKTOP, MSG_IME_SET_TARGET_INFO,
            0, (LPARAM)&data);

    return ServerSendReply (clifd, &ret, sizeof (int));
}

static int get_ime_targetinfo (int cli, int clifd, void* buff, size_t len)
{
    IME_TARGET_INFO data;

    SendMessage (HWND_DESKTOP, MSG_IME_GET_TARGET_INFO, 0, (LPARAM)&data);

    return ServerSendReply (clifd, &data, sizeof (IME_TARGET_INFO));
}

#ifdef _MGGAL_MLSHADOW
static int handle_mlshadow_req (int cli, int clifd, void* buff, size_t len)
{
    int *op_id;

    op_id = buff;
    switch(*op_id) {
        case MLSOP_ID_GET_MASTERINFO:
            {
                MLSHADOW_REPLY_MASTER_INFO reply;
                MLSHADOW_Server(buff, &reply);
                ServerSendReply (clifd, &reply, sizeof(reply));
                break;
            }
        case MLSOP_ID_CREATE_SURFACE:
            {
                MLSHADOW_REPLY_SURFACE_CREATE reply;
                MLSHADOW_Server(buff, &reply);
                add_global_res (cli, (void*)reply.surface_key,
                        (void*)reply.surface_key, (ReleaseProc)srvMLSHADOW_DelSurface);
                ServerSendReply (clifd, &reply, sizeof(reply));
                break;
            }
        case MLSOP_ID_GET_SLAVEINFO:
            {
                MLSHADOW_REPLY_SLAVE_GETINFO reply;
                MLSHADOW_Server(buff, &reply);
                ServerSendReply (clifd, &reply, sizeof(reply));
                break;
            }
        case MLSOP_ID_SET_SLAVEINFO:
            {
                BOOL reply;
                MLSHADOW_Server(buff, &reply);
                ServerSendReply (clifd, &reply, sizeof(reply));
                break;
            }
        case MLSOP_ID_DESTROY_SLAVE:
            {
                BOOL reply;
                MLSHADOW_Server(buff, &reply);
                ServerSendReply (clifd, &reply, sizeof(reply));
                break;
            }
    }
    return 0;
}
#endif

#if IS_SHAREDFB_SCHEMA_PROCS
void release_hw_surface (REQ_HWSURFACE* allocated)
{
    /*[humingming./2010/11/24]: don't call GAL_RequestHWSurface,
     * this will casue double free or same other problem */
#if 0
    REP_HWSURFACE reply;

    GAL_RequestHWSurface (allocated, &reply);
#endif

    free (allocated);
}

static int req_hw_surface (int cli, int clifd, void* buff, size_t len)
{
    REQ_HWSURFACE* request = (REQ_HWSURFACE*) buff;
    REP_HWSURFACE reply;

    GAL_RequestHWSurface (request, &reply);

    if (request->bucket == NULL) {
        REQ_HWSURFACE* allocated;
        allocated = malloc (sizeof (REQ_HWSURFACE));
        if (allocated) {
            allocated->w = request->w;
            allocated->h = request->h;
            allocated->pitch = reply.pitch;
            allocated->offset = reply.offset;
            allocated->bucket = reply.bucket;

            add_global_res (cli, (void*)req_hw_surface,
                            allocated, (ReleaseProc)release_hw_surface);
        }
    }
    else {
       del_global_res (cli, (void*)req_hw_surface, request->bucket);
    }

    return ServerSendReply (clifd, &reply, sizeof (REP_HWSURFACE));
}
#endif /* IS_SHAREDFB_SCHEMA_PROCS */

#ifdef _MGHAVE_CLIPBOARD
extern int __mg_clipboard_op (int cli, int clifd, void* buff, size_t len);
#endif

#if IS_COMPOSITING_SCHEMA
/* get the fake screen surface (wallpaper pattern surface) */
static int get_shared_surface (int cli, int clifd, void* buff, size_t len)
{
    SHAREDSURFINFO info;

    assert (__gal_fake_screen);

    info.flags = __gal_fake_screen->flags;
    if (strcmp (buff, SYSSF_WALLPAPER_PATTER) == 0 &&
            __gal_fake_screen->shared_header) {
        info.size = __gal_fake_screen->shared_header->map_size;
        return ServerSendReplyEx (clifd, &info, sizeof (SHAREDSURFINFO),
                    __gal_fake_screen->shared_header->fd);
    }
    else {
        info.size = 0;
        return ServerSendReplyEx (clifd, &info, sizeof (SHAREDSURFINFO), -1);
    }
}
#else   /* IS_COMPOSITING_SCHEMA */
/* get the rendering surface */
static int get_shared_surface (int cli, int clifd, void* buff, size_t len)
{
    const char* name = buff;
    SHAREDSURFINFO info = { 0, 0 };

#ifdef _MGGAL_DRM
    extern int __drm_get_shared_screen_surface (const char*, SHAREDSURFINFO*);
    __drm_get_shared_screen_surface (name, &info);
#endif

    return ServerSendReply (clifd, &info, sizeof (SHAREDSURFINFO));
}
#endif  /* not IS_COMPOSITING_SCHEMA */

static int authenticate_client (int cli, int clifd, void* buff, size_t len)
{
    uint32_t magic;
    int auth_result = 1;

#ifdef _MGGAL_DRM
    extern int __drm_auth_client(int, uint32_t);
    magic = *(uint32_t*)buff;
    auth_result = __drm_auth_client (cli, magic);
#endif

    return ServerSendReply (clifd, &auth_result, sizeof (int));
}

static struct req_request {
    void* handler;
    int version;
} handlers [MAX_REQID] =
{
    { load_cursor, 0 },
    { create_cursor, 0 },
    { destroy_cursor, 0 },
    { clip_cursor, 0 },
    { get_clip_cursor, 0 },
    { set_cursor, 0 },
    { get_current_cursor, 0 },
    { show_cursor, 0 },
    { set_cursor_pos, 0 },
    { layer_info, 0 },
    { join_layer, 0 },
    { layer_op, 0 },
    { zorder_op, 1 },
    { im_live, 0 },
    { open_ime_wnd, 0 },
    { set_ime_stat, 0 },
    { get_ime_stat, 0 },
    { register_hook, 0 },
#if IS_SHAREDFB_SCHEMA_PROCS
    { req_hw_surface, 0 },
#else
    { NULL, 0 },
#endif
#ifdef _MGHAVE_CLIPBOARD
    { __mg_clipboard_op, 0 },
#else
    { NULL, 0 },
#endif
#ifdef _MGGAL_MLSHADOW
    { handle_mlshadow_req, 0 },
#else
    { NULL, 0 },
#endif
    { change_zorder_maskrect, 0 },
#ifdef _MGGAL_NEXUS
    { nexus_client_get_surface, 0 },
#else
    { NULL, 0 },
#endif
#ifdef _MGGAL_SIGMA8654
    { sigma8654_client_get_surface, 0 },
#else
    { NULL, 0 },
#endif
    { get_ime_targetinfo, 0 },
    { set_ime_targetinfo, 0 },
    { copy_cursor, 0 },
    { get_shared_surface, 0 },          // REQID_GETSHAREDSURFACE
#if IS_COMPOSITING_SCHEMA
    { load_cursor_png_file, 0 },        // REQID_LOADCURSOR_PNG
    { load_cursor_png_mem, 0 },         // REQID_LOADCURSOR_PNG_MEM
    { alloc_sem_for_shared_surf, 0 },   // REQID_ALLOC_SURF_SEM
    { free_sem_for_shared_surf, 0 },    // REQID_FREE_SURF_SEM
#else
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
#endif
    { move_to_layer, 0 },       // REQID_MOVETOLAYER
    { calc_position, 0 },       // REQID_CALCPOSITION
    { authenticate_client, 0 }, // REQID_AUTHCLIENT
};

BOOL GUIAPI RegisterRequestHandler (int req_id, REQ_HANDLER your_handler)
{
    if (req_id <= MAX_SYS_REQID || req_id > MAX_REQID)
        return FALSE;

    handlers[req_id - 1].handler = your_handler;
    handlers[req_id - 1].version = 0;
    return TRUE;
}

BOOL GUIAPI RegisterRequestHandlerV1 (int req_id, REQ_HANDLER_V1 your_handler)
{
    if (req_id <= MAX_SYS_REQID || req_id > MAX_REQID)
        return FALSE;

    handlers[req_id - 1].handler = your_handler;
    handlers[req_id - 1].version = 1;
    return TRUE;
}

REQ_HANDLER GUIAPI GetRequestHandler (int req_id)
{
    if (req_id <= 0 || req_id > MAX_REQID)
        return NULL;

    if (handlers[req_id - 1].version == 0)
        return handlers [req_id - 1].handler;

    return NULL;
}

REQ_HANDLER_V1 GUIAPI GetRequestHandlerV1 (int req_id)
{
    if (req_id <= 0 || req_id > MAX_REQID)
        return NULL;

    if (handlers[req_id - 1].version == 1)
        return handlers [req_id - 1].handler;

    return NULL;
}

void* GUIAPI GetRequestHandlerEx (int req_id, int* version)
{
    if (req_id <= 0 || req_id > MAX_REQID)
        return NULL;

    if (version) {
        *version = handlers [req_id - 1].version;
    }

    return handlers [req_id - 1].handler;
}

static char _request_data_buff [1024];

int __mg_handle_request (int clifd, int req_id, int cli)
{
    int n;
    char* buff;
    size_t req_data_len, ex_data_len;
    size_t len_data;
    int fd_received = -1;

    if ((n = sock_read (clifd, &req_data_len, sizeof (size_t))) == SOCKERR_IO)
        return SOCKERR_IO;
    else if (n == SOCKERR_CLOSED) {
        goto error;
    }

    if ((n = sock_read (clifd, &ex_data_len, sizeof (size_t))) == SOCKERR_IO)
        return SOCKERR_IO;
    else if (n == SOCKERR_CLOSED) {
        goto error;
    }

    len_data = req_data_len + ex_data_len;
    if (len_data <= sizeof (_request_data_buff)) {
        buff = _request_data_buff;
    }
    else {
        buff = malloc (len_data);
        if (buff == NULL)
            return SOCKERR_INVARG;
    }

#if 0
    if ((n = sock_read (clifd, buff, len_data)) == SOCKERR_IO)
        return SOCKERR_IO;
    else if (n == SOCKERR_CLOSED) {
        goto error;
    }
#else /* use recvmsg */
    {
        struct iovec    iov[2];
        struct msghdr   msg;
        struct cmsghdr  *cmsg = NULL;

        iov[0].iov_base = buff;
        iov[0].iov_len  = req_data_len;

        iov[1].iov_base = buff + req_data_len;
        iov[1].iov_len  = ex_data_len;

        msg.msg_iov     = iov;
        msg.msg_iovlen  = 2;
        msg.msg_name    = NULL;
        msg.msg_namelen = 0;

        cmsg = alloca (CMSG_LEN (sizeof (int)));
        msg.msg_control     = cmsg;
        msg.msg_controllen  = CMSG_LEN (sizeof (int));

        if ((n = sock_recvmsg (clifd, &msg, 0)) == SOCKERR_IO) {
            return SOCKERR_IO;
        }
        else if (n == SOCKERR_CLOSED) {
            goto error;
        }

        if (msg.msg_controllen == CMSG_LEN (sizeof (int))) {
            memcpy (&fd_received, CMSG_DATA(cmsg), sizeof (int));
        }
    }
#endif

    req_id &= ~REQMASK_FLAGS;
    if (req_id > MAX_REQID || req_id <= 0 ||
            handlers [req_id - 1].handler == NULL)
        return SOCKERR_INVARG;

    if (handlers [req_id - 1].version == 1) {
        REQ_HANDLER_V1 handler = handlers [req_id - 1].handler;
        n = handler (cli, clifd, buff, len_data, fd_received);
    }
    else {
        REQ_HANDLER handler = handlers [req_id - 1].handler;

        if (fd_received >= 0) {
            close (fd_received);
            _WRN_PRINTF ("A file descriptor received, but the request handler is version 0.\n");
        }

        n = handler (cli, clifd, buff, len_data);
    }

    if (len_data > sizeof (_request_data_buff))
        free (buff);

    if (n == SOCKERR_IO)
        goto error;

    if (req_id == REQID_IAMLIVE && mgClients [cli].has_dirty)
        __mg_check_dirty_znode (cli);

    return n;

error:
    __mg_remove_client (cli, clifd);
    return SOCKERR_CLOSED;
}

