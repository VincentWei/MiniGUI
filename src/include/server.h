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
** server.h: routines for server.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_SERVER_H
    #define GUI_SERVER_H

#define LOCKFILE    "/var/tmp/mginit"
#define CS_PATH     "/var/tmp/minigui" /* well-known name */ 

#define THRES_LIVE  500                /* 5 seconds */
#define TO_SOCKIO   100                /* 1 seconds */

#define DEF_NR_GLOBALS              16
#define MAX_NR_LAYERS               64

#define IPC_KEY_BASE                0x464D4700

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef MINIGUI_H
int serv_listen (const char* name);
int serv_accept (int listenfd, pid_t *pidptr, uid_t *uidptr);
#endif

/* defined in kernel/sharedres.c. */
void __mg_delete_sharedres_sem (void);

/* defined in kernel/layer.c. */
void __mg_delete_zi_sem (void);

int __mg_init_layers (void);
void __mg_cleanup_layers (void);

BOOL __mg_is_valid_layer (MG_Layer* layer);

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
intptr_t __mg_do_zorder_operation (int cli, const ZORDEROPINFO* info);
intptr_t __mg_do_zorder_maskrect_operation (int cli, const ZORDERMASKRECTOPINFO* info);
int __mg_remove_all_znodes_of_client (int cli);

int __mg_handle_normal_mouse_move (const ZORDERINFO* zi, int x, int y);
int __mg_get_znode_at_point (const ZORDERINFO* zi, int x, int y, HWND* hwnd);
    
HWND __mg_do_reghook_operation (int cli, const REGHOOKINFO* info);
int __mg_handle_mouse_hook (int message, WPARAM wParam, LPARAM lParam);

int __mg_do_change_topmost_layer (void);

MG_Layer* __mg_find_layer_by_name (const char* name);

int __mg_do_drag_drop_window (int msg, int x, int y);

void __mg_check_dirty_znode (int cli);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_SERVER_H */

