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
** layer.c: maintain the layers in server.
** 
** Current maintainer: Wei Yongming.
**
** Create date: 2005/08/14
*/

#include <sys/types.h>
#include <signal.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "internals.h"

#include "ourhdr.h"
#include "sockio.h"
#include "client.h"
#include "server.h"
#include "sharedres.h"
#include "drawsemop.h"
#include "misc.h"

#define SEM_PARAM 0666
MG_Layer* mgLayers = NULL;
MG_Layer* mgTopmostLayer = NULL;

ON_CHANGE_LAYER OnChangeLayer = NULL;

#define CHANGE_TOPMOST_LAYER(layer)             \
    lock_draw_sem ();                           \
    SHAREDRES_TOPMOST_LAYER = (GHANDLE)layer;   \
    unlock_draw_sem ();

static int sg_nr_layers = 0;
static unsigned char sem_usage [(MAX_NR_LAYERS + 7)/8];

static BOOL do_alloc_layer (MG_Layer* layer, const char* name, 
                int nr_topmosts, int nr_normals)
{
    ZORDERINFO* zi;
    ZORDERNODE* znodes;
    void* maskrect_usage_bmp;

    layer->zorder_shmid = kernel_alloc_z_order_info (nr_topmosts, nr_normals);

    if (layer->zorder_shmid == -1)
        return FALSE;

    /* Attach to the share memory. */
    zi = shmat (layer->zorder_shmid, 0, 0);
    if (zi == (void*)-1) 
        return FALSE;

    if (shmctl (layer->zorder_shmid, IPC_RMID, NULL) < 0) 
        return FALSE;

    strcpy (layer->name, name);

    layer->zorder_info = zi;

    zi->size_usage_bmp = SIZE_USAGE_BMP;
    zi->size_maskrect_usage_bmp = SIZE_MASKRECT_USAGE_BMP;

    zi->max_nr_popupmenus = DEF_NR_POPUPMENUS;
    zi->max_nr_globals = SHAREDRES_NR_GLOBALS;
    zi->max_nr_topmosts = nr_topmosts;
    zi->max_nr_normals = nr_normals;

    zi->nr_popupmenus = 0;
    zi->nr_globals = 1;
    zi->nr_topmosts = 0;
    zi->nr_normals = 0;

    zi->first_global = 0;
    zi->first_topmost = 0;
    zi->first_normal = 0;

    zi->active_win = 0;

    zi->cli_trackmenu = -1;
    zi->ptmi_in_cli = HWND_INVALID;

    zi->zi_semid = SHAREDRES_SEMID_LAYER;

    /* find one unused semaphore slot here */
    zi->zi_semnum = __mg_lookfor_unused_slot (sem_usage, sizeof (sem_usage), 1);

    /* init the semaphore with semctl */
    {
        union semun arg;
        arg.val = zi->max_nr_popupmenus + zi->max_nr_globals
                    + zi->max_nr_topmosts + zi->max_nr_normals;
        semctl (zi->zi_semid, zi->zi_semnum, SETVAL, arg);
    }

    memset (zi + 1, 0xFF, SIZE_USAGE_BMP);
    /* get a unused mask rect slot. */
    maskrect_usage_bmp = GET_MASKRECT_USAGEBMP(zi);
    memset (maskrect_usage_bmp, 0xFF, zi->size_maskrect_usage_bmp);

    /* init z-order node for desktop */
    znodes = (ZORDERNODE*) ((char*)(zi + 1) + zi->size_usage_bmp + 
                    sizeof (ZORDERNODE) * DEF_NR_POPUPMENUS);

    znodes [0].flags = ZOF_TYPE_DESKTOP | ZOF_VISIBLE;
    znodes [0].rc = g_rcScr;
    znodes [0].age = 0;
    znodes [0].cli = 0;
    znodes [0].fortestinghwnd = HWND_DESKTOP;
    znodes [0].next = 0;
    znodes [0].prev = 0;
    /*for mask rect.*/
    znodes [0].idx_mask_rect = 0;

    __mg_slot_set_use ((unsigned char*)(zi + 1), 0);
    __mg_slot_set_use ((unsigned char*)(maskrect_usage_bmp), 0);

    sg_nr_layers ++;
    return TRUE;
}

static void do_free_layer (MG_Layer* layer)
{
    ZORDERINFO* zi = layer->zorder_info;

    /* free zorder_semid for reuse here... */
    __mg_slot_clear_use (sem_usage, zi->zi_semnum);
    kernel_free_z_order_info (zi);

    if (layer->prev)
        layer->prev->next = layer->next;
    if (layer->next)
        layer->next->prev = layer->prev;

    if (layer == mgLayers) {
        mgLayers = layer->next;
    }

    sg_nr_layers --;
}

BOOL GUIAPI ServerDeleteLayer (MG_Layer* layer)
{
    ZORDERINFO* zi = layer->zorder_info;
    MG_Client* client;

    if (layer == NULL || strcmp (layer->name, NAME_DEF_LAYER) == 0) {
        _MG_PRINTF ("SERVER: Can not delete the default layer.\n");
        return FALSE;
    }

    if (zi->nr_topmosts + zi->nr_normals + zi->nr_popupmenus != 0) {
        _MG_PRINTF ("SERVER: Some objects are still in this layer.\n");
        return FALSE;
    }

    if (layer != mgLayers)
        ServerSetTopmostLayer (mgLayers);
    else
        ServerSetTopmostLayer (layer->next);

    if (OnChangeLayer) OnChangeLayer (LCO_DEL_LAYER, layer, NULL);

    client = layer->cli_head;
    while (client) {

        MG_Client* next = client->next;

        __mg_remove_client (client - mgClients, client->fd);

        client = next;
    }

    do_free_layer (layer);
    return TRUE;
}

BOOL __mg_is_valid_layer (MG_Layer* layer)
{
    MG_Layer* myLayer;

    myLayer = mgLayers;
    while (myLayer) {
        if (layer == myLayer)
            return TRUE;

        myLayer = myLayer->next;
    }

    return FALSE;
}

inline static key_t get_sem_key (void)
{
    return (key_t)(IPC_KEY_BASE + 0x03);
}

void __mg_delete_zi_sem (void)
{
    union semun ignored;
    if (semctl (SHAREDRES_SEMID_LAYER, 0, IPC_RMID, ignored) < 0)
        goto error;

    return;

error:
    perror("remove semaphore");
}

int __mg_init_layers ()
{
    key_t sem_key;
    int semid;

    mgLayers = calloc (1, sizeof (MG_Layer));

    if (mgLayers == NULL)
        return -1;

    memset (sem_usage, 0xFF, sizeof (sem_usage));

    if ((sem_key = get_sem_key ()) == -1) {
        return -1;
    }

    semid = semget (sem_key, MAX_NR_LAYERS, SEM_PARAM | IPC_CREAT | IPC_EXCL); 
    if (semid == -1)
        return -1;
    atexit (__mg_delete_zi_sem);

    SHAREDRES_SEMID_LAYER = semid;

    /* allocate the first layer for the default layer. */
    if (!do_alloc_layer (mgLayers, NAME_DEF_LAYER, 
                    SHAREDRES_DEF_NR_TOPMOSTS, SHAREDRES_DEF_NR_NORMALS)) {

        free (mgLayers);
        mgLayers = NULL;
        return -1;
    }

    __mg_zorder_info = mgLayers->zorder_info;

    return semid;
}

void __mg_cleanup_layers (void)
{
    MG_Layer *layer = mgLayers, *next;

    while (layer) {
        next = layer->next;

        do_free_layer (layer);
        free (layer);

        layer = next;

    };

    mgLayers = NULL;
    mgTopmostLayer = NULL;
    __mg_zorder_info = NULL;
}

/* allocate a layer slot from the layer pool for the new layer */
static MG_Layer* alloc_layer (const char* layer_name,
                int nr_topmosts, int nr_normals)
{
    MG_Layer* new_layer;

    if (sg_nr_layers >= MAX_NR_LAYERS) {
        return NULL;
    }

    if (!(new_layer = calloc (1, sizeof (MG_Layer))))
        return NULL;

    if (!do_alloc_layer (new_layer, layer_name, nr_topmosts, nr_normals)) {
        free (new_layer);
        return NULL;
    }

    _MG_PRINTF ("SERVER: Create a new layer: %s\n", layer_name);

    strcpy (new_layer->name, layer_name);
    new_layer->cli_head = NULL;
    new_layer->cli_active = NULL;

    /* chain it */
    if (mgLayers)
        mgLayers->prev = new_layer;
    new_layer->prev = NULL;
    new_layer->next = mgLayers;
    mgLayers = new_layer;

    /* Notify that a new layer created. */
    if (OnChangeLayer)
        OnChangeLayer (LCO_NEW_LAYER, new_layer, NULL);

    mgTopmostLayer = new_layer;

    _MG_PRINTF ("SERVER: alloc_layer, mgTopmostLayer->zi = %p\n", 
            mgTopmostLayer->zorder_info);

    /* Topmost layer changed */
    CHANGE_TOPMOST_LAYER(mgTopmostLayer);

    __mg_do_change_topmost_layer ();
    _MG_PRINTF ("SERVER: alloc_layer, mgTopmostLayer->zi = %p\n", 
            mgTopmostLayer->zorder_info);

    /* Notify that a new topmost layer have been set. */
    if (OnChangeLayer) 
        OnChangeLayer (LCO_TOPMOST_CHANGED, mgTopmostLayer, NULL);

    PostMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);

    return new_layer;
}

/* Retrives the layer by the name. */
MG_Layer* __mg_find_layer_by_name (const char* name)
{
    MG_Layer* layer = mgLayers;

    if (name == NULL || name [0] == '\0')
        return NULL;

    while (layer) {
        if (strncmp (layer->name, name, LEN_LAYER_NAME) == 0)
            return layer;

        layer = layer->next;
    }

    return NULL;
}

/* get information of a layer, the server side of GetLayerInfo */
void __mg_get_layer_info (int cli, const char* layer_name, LAYERINFO* info)
{
    MG_Layer* layer;
    MG_Client* client;

    memset (info, 0, sizeof (LAYERINFO));
    info->cli_active = -1;

    if (layer_name [0]) {
        layer = __mg_find_layer_by_name (layer_name);
        if (layer == NULL) {
            info->handle = INV_LAYER_HANDLE;
            return;
        }
    }
    else
        layer = mgClients[cli].layer;

    info->handle = (GHANDLE)layer;
    if (info->handle) {

        client = layer->cli_head;
        while (client) {
            info->nr_clients ++;

            client = client->next;
        }

        if (mgTopmostLayer == layer)
            info->is_topmost = TRUE;
        info->cli_active = layer->cli_active - mgClients;
    }
}

static void do_client_join_layer (int cli, 
                const JOINLAYERINFO* info, JOINEDCLIENTINFO* joined_info)
{
    MG_Layer* layer = (MG_Layer*)(joined_info->layer);
    MG_Client* new_client = mgClients + cli;

    if (new_client->layer == layer) {   /* duplicated calling of JoinLayer */
        joined_info->cli_id = cli;
        joined_info->zo_shmid = layer->zorder_shmid;
        return;
    }

    _MG_PRINTF ("SERVER: Join a client (%s) to layer %s\n", 
                info->client_name, layer->name);

    strcpy (new_client->name, info->client_name);
    
    new_client->layer = layer;

    /* Notify that a new client joined to this layer. */
    if (OnChangeLayer)
        OnChangeLayer (LCO_JOIN_CLIENT, layer, new_client);

    new_client->prev = NULL;
    new_client->next = layer->cli_head;
    if (layer->cli_head)
        layer->cli_head->prev = new_client;
    layer->cli_head = new_client;

    joined_info->cli_id = cli;
    joined_info->zo_shmid = layer->zorder_shmid;
}

/* Join a client to a layer, the server side of JoinLayer */
void __mg_client_join_layer (int cli, 
                const JOINLAYERINFO* info, JOINEDCLIENTINFO* joined_info)
{
    MG_Layer* layer;

    if ((layer = __mg_find_layer_by_name (info->layer_name)) == NULL) {
        layer = alloc_layer (info->layer_name, 
            (info->max_nr_topmosts <= 0)?SHAREDRES_DEF_NR_TOPMOSTS:
                    info->max_nr_topmosts,
            (info->max_nr_normals<= 0)?SHAREDRES_DEF_NR_NORMALS:
                    info->max_nr_normals);
    }

    joined_info->layer = (GHANDLE)layer;
    do_client_join_layer (cli, info, joined_info);
}

BOOL GUIAPI ServerSetTopmostLayer (MG_Layer* layer)
{
    MG_Client* client;
    unsigned int IsPaint = 0;

    if (!mgIsServer)
        return FALSE;

    if (layer == mgTopmostLayer)
        return FALSE;

    if (mgTopmostLayer && mgTopmostLayer->cli_head)
        IsPaint = 1;

    mgTopmostLayer = layer;
    CHANGE_TOPMOST_LAYER (layer);

    __mg_do_change_topmost_layer ();

    client = layer->cli_head;
    while (client) {
        MSG msg = {0, MSG_PAINT, 0, 0, __mg_timer_counter};

        __mg_send2client (&msg, client);

        client = client->next;
    }

    /* Notify that a new topmost layer have been set. */
    if (OnChangeLayer)
        OnChangeLayer (LCO_TOPMOST_CHANGED, mgTopmostLayer, NULL);

    if (IsPaint)
    {
        SendMessage (HWND_DESKTOP, MSG_PAINT, 0, 0);
    }

    return TRUE;
}

MG_Layer* GUIAPI ServerCreateLayer (const char* layer_name, 
                int max_nr_topmosts, int max_nr_normals)
{
    MG_Layer* new_layer;

    if (layer_name == NULL || layer_name [0] == '\0')
        return NULL;

    new_layer = __mg_find_layer_by_name (layer_name);

    if (new_layer)
        return new_layer;

    if (sg_nr_layers >= MAX_NR_LAYERS) {
        return NULL;
    }

    if (!(new_layer = calloc (1, sizeof (MG_Layer))))
        return NULL;

    if (max_nr_topmosts <= 0) max_nr_topmosts = DEF_NR_TOPMOSTS;
    if (max_nr_normals <= 0) max_nr_normals = DEF_NR_NORMALS;
    max_nr_topmosts = (max_nr_topmosts + 7) & ~0x07;
    max_nr_normals = (max_nr_normals + 7) & ~0x07;

    if (!do_alloc_layer (new_layer, layer_name, max_nr_topmosts, max_nr_normals)) {
        free (new_layer);
        return NULL;
    }

    strcpy (new_layer->name, layer_name);
    new_layer->cli_head = NULL;
    new_layer->cli_active = NULL;

    /* chain it */
    if (mgLayers)
        mgLayers->prev = new_layer;
    new_layer->prev = NULL;
    new_layer->next = mgLayers;
    mgLayers = new_layer;

    /* Notify that a new layer created. */
    if (OnChangeLayer)
        OnChangeLayer (LCO_NEW_LAYER, new_layer, NULL);

    return new_layer;
}

void GUIAPI DisableClientsOutput (void)
{
    if (!mgIsServer)
        return;

    CHANGE_TOPMOST_LAYER(NULL);
}

void GUIAPI UpdateTopmostLayer (const RECT* dirty_rc)
{
    MSG msg = {0, MSG_PAINT, 0, 0, __mg_timer_counter};

    if (!mgIsServer)
        return;

    CHANGE_TOPMOST_LAYER(mgTopmostLayer);

    if (dirty_rc) {
        RECT eff_rc;
        IntersectRect (&eff_rc, dirty_rc, &g_rcScr);
        msg.wParam = MAKELONG (eff_rc.left, eff_rc.top);
        msg.lParam = MAKELONG (eff_rc.right, eff_rc.bottom);
    }

    Send2Client (&msg, CLIENTS_TOPMOST);

    PostMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
    SendMessage (HWND_DESKTOP, MSG_PAINT, 0, 0);
}

int GUIAPI ServerGetNextZNode (MG_Layer* layer, int idx_znode, int* cli)
{
    ZORDERINFO* zi;
    ZORDERNODE* nodes;
    int next = 0;
    DWORD type;

    if (layer == NULL)
        layer = mgTopmostLayer;

    if (!__mg_is_valid_layer (layer))
        return -1;

    zi = (ZORDERINFO*)layer->zorder_info;
    if (idx_znode > zi->max_nr_globals 
            + zi->max_nr_topmosts + zi->max_nr_normals) {
        return -1;
    }

    nodes = (ZORDERNODE*) ((char*)(zi + 1) + zi->size_usage_bmp +
                    sizeof (ZORDERNODE) * DEF_NR_POPUPMENUS);

    if (idx_znode <= 0) {
        next = zi->first_global;
        if (next == 0)
            next = zi->first_topmost;
        if (next == 0)
            next = zi->first_normal;

        if (next > 0 && cli) {
            *cli = nodes [next].cli;
        }

        return next;
    }

    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;
    if (type != ZOF_TYPE_GLOBAL && type != ZOF_TYPE_TOPMOST
            && type != ZOF_TYPE_NORMAL)
        return -1;

    if (idx_znode > 0) {
        next = nodes [idx_znode].next;
    }

    if (next > 0 && cli) {
        *cli = nodes [next].cli;
        return next;
    }

    switch (nodes [idx_znode].flags & ZOF_TYPE_MASK) {
        case ZOF_TYPE_GLOBAL:
            next = zi->first_topmost;
            if (next == 0)
                next = zi->first_normal;
            break;

        case ZOF_TYPE_TOPMOST:
            next = zi->first_normal;
            break;

        case ZOF_TYPE_NORMAL:
            break;
        default:
            return -1;
    }

    if (next > 0 && cli) {
        *cli = nodes [next].cli;
    }

    return next;
}

BOOL GUIAPI ServerGetZNodeInfo (MG_Layer* layer, int idx_znode,
                ZNODEINFO* znode_info)
{
    ZORDERINFO* zi;
    ZORDERNODE* nodes;

    if (znode_info == NULL)
        return FALSE;

    if (layer == NULL)
        layer = mgTopmostLayer;

    if (!__mg_is_valid_layer (layer))
        return FALSE;

    zi = (ZORDERINFO*)layer->zorder_info;
    if (idx_znode > zi->max_nr_globals 
            + zi->max_nr_topmosts + zi->max_nr_normals) {
        return FALSE;
    }

    nodes = (ZORDERNODE*) ((char*)(zi + 1) + zi->size_usage_bmp +
                    sizeof (ZORDERNODE) * DEF_NR_POPUPMENUS);
    znode_info->type = (nodes [idx_znode].flags & ZOF_TYPE_FLAG_MASK);
    znode_info->flags = (nodes [idx_znode].flags & ZOF_FLAG_MASK);
    znode_info->caption = nodes [idx_znode].caption;
    znode_info->rc = nodes [idx_znode].rc;
    znode_info->cli = nodes [idx_znode].cli;
    znode_info->hwnd = nodes [idx_znode].fortestinghwnd;
    znode_info->main_win = nodes [idx_znode].main_win;

    return TRUE;
}

BOOL GUIAPI ServerDoZNodeOperation (MG_Layer* layer,
                int idx_znode, int op_code, void* op_data, BOOL notify)
{
    ZORDERINFO* zi;
    ZORDERNODE* nodes;
    DWORD type;
    ZORDEROPINFO info;

    if (layer == NULL)
        layer = mgTopmostLayer;

    if (!__mg_is_valid_layer (layer))
        return FALSE;

    zi = (ZORDERINFO*)layer->zorder_info;
    if (idx_znode > zi->max_nr_globals 
            + zi->max_nr_topmosts + zi->max_nr_normals) {
        return FALSE;
    }

    nodes = (ZORDERNODE*) ((char*)(zi + 1) + zi->size_usage_bmp +
                    sizeof (ZORDERNODE) * DEF_NR_POPUPMENUS);
    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;
    if (type != ZOF_TYPE_GLOBAL && type != ZOF_TYPE_TOPMOST
            && type != ZOF_TYPE_NORMAL)
        return FALSE;

    switch (op_code) {
        case ZNOP_MOVE2TOP:
            info.id_op = ID_ZOOP_MOVE2TOP;
            info.idx_znode = idx_znode;
            break;
        case ZNOP_SETACTIVE:
            info.id_op = ID_ZOOP_SETACTIVE;
            info.idx_znode = idx_znode;
            break;
        default:
            return FALSE;
    }

    __mg_do_zorder_operation (nodes[idx_znode].cli, &info);

    return TRUE;
}

