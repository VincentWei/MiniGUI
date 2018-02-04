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
static void del_global_res (int cli, void* key)
{
    MG_Client* client = mgClients + cli;
    struct GlobalRes *global_res = client->global_res, *next;

    while (global_res) {
        next = global_res->next;

        if (global_res->key == key) {
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

int GUIAPI ServerSendReply (int clifd, const void* reply, int len)
{
    MSG reply_msg = {HWND_INVALID, 0};
    if (!mgIsServer)
        return SOCKERR_IO;

    /* send a reply message to indicate this is a reply of request */
    if (sock_write (clifd, &reply_msg, sizeof (MSG)) < 0)
        return SOCKERR_IO;

    if (sock_write (clifd, reply, len) < 0)
        return SOCKERR_IO;
    return SOCKERR_OK;
}

static int load_cursor (int cli, int clifd, void* buff, size_t len)
{
    HCURSOR hcsr;

    hcsr = LoadCursorFromFile (buff);

#ifdef _MGHAVE_CURSOR
    if (hcsr) {
        add_global_res (cli, (void*) hcsr, 
                        (void*)hcsr, (ReleaseProc)DestroyCursor);
    }
#endif
    return ServerSendReply (clifd, &hcsr, sizeof (HCURSOR));
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
        add_global_res (cli, (void*) hcsr, (void*)hcsr, NULL);
    }
#endif
    return ServerSendReply (clifd, &hcsr, sizeof (HCURSOR));
}

static int copy_cursor (int cli, int clifd, void* buff, size_t len)
{
    HCURSOR hcsr = CopyCursor ((HCURSOR)*(intptr_t*)buff);
#ifdef _MGHAVE_CURSOR
    if (hcsr) {
        add_global_res (cli, (void*) hcsr, (void*)hcsr, NULL);
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
    del_global_res (cli, (void*)hcsr);

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
            ServerSetTopmostLayer (layer);
            ret_value = TRUE;
            break;
    }

ret:
    return ServerSendReply (clifd, &ret_value, sizeof (BOOL));
}

static int zorder_op (int cli, int clifd, void* buff, size_t len)
{
    intptr_t ret_value;
    ZORDEROPINFO* info = (ZORDEROPINFO*)buff;

    ret_value = __mg_do_zorder_operation (cli, info);

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
extern int Sigma8654_ServerOnGetSurface(REQ_SIGMA8654_GETSURFACE *request, REP_SIGMA8654_GETSURFACE *reply);
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
extern int Nexus_ServerOnGetSurface(REQ_NEXUS_GETSURFACE *request, REP_NEXUS_GETSURFACE *reply);
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
    HWND ret_value;

    info = (REGHOOKINFO*)buff;
    ret_value = __mg_do_reghook_operation (cli, info);

    return ServerSendReply (clifd, &ret_value, sizeof (HWND));
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

void release_HWS (REQ_HWSURFACE* allocated)
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

            add_global_res (cli, allocated->bucket, 
                            allocated, (ReleaseProc)release_HWS);
        }
    }
    else {
       del_global_res (cli, request->bucket);
    }

    return ServerSendReply (clifd, &reply, sizeof (REP_HWSURFACE));
}

#ifdef _MGHAVE_CLIPBOARD
extern int clipboard_op (int cli, int clifd, void* buff, size_t len);
#endif

static REQ_HANDLER handlers [MAX_REQID] =
{
    load_cursor,
    create_cursor,
    destroy_cursor,
    clip_cursor,
    get_clip_cursor,
    set_cursor,
    get_current_cursor,
    show_cursor,
    set_cursor_pos,
    layer_info,
    join_layer,
    layer_op,
    zorder_op,
    im_live,
    open_ime_wnd,
    set_ime_stat,
    get_ime_stat,
    register_hook,
    req_hw_surface,
#ifdef _MGHAVE_CLIPBOARD
    clipboard_op,
#else
    NULL,
#endif
#ifdef _MGGAL_MLSHADOW
    handle_mlshadow_req,
#else
    NULL,
#endif
    change_zorder_maskrect,
#ifdef _MGGAL_NEXUS
    nexus_client_get_surface,
#else
    NULL,
#endif
#ifdef _MGGAL_SIGMA8654
    sigma8654_client_get_surface,
#else
    NULL,
#endif
    get_ime_targetinfo,
    set_ime_targetinfo,
    copy_cursor,
};

BOOL GUIAPI RegisterRequestHandler (int req_id, REQ_HANDLER your_handler)
{
    if (req_id <= MAX_SYS_REQID || req_id > MAX_REQID)
        return FALSE;

    handlers [req_id - 1] = your_handler;
    return TRUE;
}

REQ_HANDLER GUIAPI GetRequestHandler (int req_id)
{
    if (req_id <= 0 || req_id > MAX_REQID)
        return NULL;

    return handlers [req_id - 1];
}

static char _request_data_buff [1024];

int __mg_handle_request (int clifd, int req_id, int cli)
{
    int n;
    char* buff;
    size_t len_data;

    if ((n = sock_read (clifd, &len_data, sizeof (size_t))) == SOCKERR_IO)
        return SOCKERR_IO;
    else if (n == SOCKERR_CLOSED) {
        goto error;
    }

    if (len_data <= sizeof (_request_data_buff)) {
        buff = _request_data_buff;
    }
    else {
        buff = malloc (len_data);
        if (buff == NULL)
            return SOCKERR_INVARG;
    }

    if ((n = sock_read (clifd, buff, len_data)) == SOCKERR_IO)
        return SOCKERR_IO;
    else if (n == SOCKERR_CLOSED) {
        goto error;
    }

    if (req_id > MAX_REQID || req_id <= 0 || handlers [req_id - 1] == NULL)
        return SOCKERR_INVARG;

    n = handlers [req_id - 1] (cli, clifd, buff, len_data);

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


