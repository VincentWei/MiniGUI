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
** server.h: routines for server.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_SERVER_H
    #define GUI_SERVER_H

#include "constants.h"

#ifdef _MGSCHEMA_COMPOSITING

#define DO_COMPSOR_OP(op)                                               \
do {                                                                    \
    CompositorCtxt* ctxt;                                               \
    const CompositorOps* ops = ServerSelectCompositor (NULL, &ctxt);    \
    if (ops && ops->op) {                                               \
        ops->op(ctxt);                                                  \
    }                                                                   \
} while (0)

#define DO_COMPSOR_OP_ARGS(op, ...)                                     \
do {                                                                    \
    CompositorCtxt* ctxt;                                               \
    const CompositorOps* ops = ServerSelectCompositor (NULL, &ctxt);    \
    if (ops && ops->op) {                                               \
        ops->op(ctxt, __VA_ARGS__);                                     \
    }                                                                   \
} while (0)

#else   /* defined _MGSCHEMA_COMPOSITING */

#define DO_COMPSOR_OP(op)                                               \
    do { } while (0)

#define DO_COMPSOR_OP_ARGS(op, ...)                                     \
    do { } while (0)

#endif  /* not defined _MGSCHEMA_COMPOSITING */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef MINIGUI_H
int serv_listen (const char* name);
int serv_accept (int listenfd, pid_t *pidptr, uid_t *uidptr);
#endif

/* defined in kernel/sharedres.c. */
void __mg_delete_sharedres_sem (void);

int __mg_init_layers (void);
void __mg_cleanup_layers (void);

static inline
BOOL __mg_is_valid_layer (const MG_Layer* layer) {
    MG_Layer* _layer = mgLayers;
    while (_layer) {
        if (_layer == layer)
            return TRUE;
        _layer = _layer->next;
    }

    return FALSE;
}

static inline
MG_Layer* __mg_get_layer_from_zi (const ZORDERINFO* zi) {
    MG_Layer* layer = mgLayers;
    while (layer) {
        if (layer->zorder_info == zi)
            return layer;
        layer = layer->next;
    }

    return NULL;
}

void __mg_get_layer_info (int cli, const char* layer_name, LAYERINFO* info);
void __mg_client_join_layer (int cli,
                const JOINLAYERINFO* info, JOINEDCLIENTINFO* joined_info);

int  __mg_client_add (int fd, pid_t pid, uid_t uid);
void __mg_client_del (int cli);

void __mg_release_global_res (int cli);

void __mg_remove_client (int cli, int clifd);

int __mg_handle_request (int clifd, int req_id, int cli);

int __mg_send2client (const MSG* msg, MG_Client* client);
void __mg_set_active_client (MG_Client* client);

void __mg_start_server_desktop (void);
int __mg_post_msg_by_znode (const ZORDERINFO* zi, int znode,
                int message, WPARAM wParam, LPARAM lParam);
intptr_t __mg_do_zorder_operation (int cli, const ZORDEROPINFO* info,
                const char* caption, int fd);
intptr_t __mg_do_zorder_maskrect_operation (int cli,
                const ZORDERMASKRECTOPINFO* info);
int __mg_remove_all_znodes_of_client (int cli);

int __mg_handle_normal_mouse_move (const ZORDERINFO* zi, int x, int y);
int __mg_get_znode_at_point (const ZORDERINFO* zi, int x, int y, HWND* hwnd);

int __mg_do_reghook_operation (int cli, const REGHOOKINFO* info);
int __mg_handle_mouse_hook (int message, WPARAM wParam, LPARAM lParam);

int __mg_do_change_topmost_layer (void);

MG_Layer* __mg_find_layer_by_name (const char* name);

int __mg_do_drag_drop_window (int msg, int x, int y);

#ifdef _MGSCHEMA_SHAREDFB
void __mg_check_dirty_znode (int cli);
#else
static inline void __mg_check_dirty_znode (int cli) {}
#endif

BOOL __mg_move_client_to_layer (MG_Client* client, MG_Layer* layer);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_SERVER_H */

