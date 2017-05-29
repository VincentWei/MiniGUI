/*
** $Id: client.c 10660 2008-08-14 09:30:39Z weiym $
** 
** client.c: maintain the clients in server.
** 
** Copyright (C) 2003 ~ 2008 Feynman Software
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/12/20
**
** NOTE: The idea comes from sample code in APUE.
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

#define    NALLOC       4        /* #Client structs to alloc/realloc for */

extern unsigned int __mg_timer_counter;

MG_Client* mgClients = NULL;
int mgClientSize = 0;

#define off_pointer(p,off)                  \
        do {                                \
            if (p) {                        \
                unsigned int tmp;           \
                tmp = (unsigned int)p;      \
                tmp += off;                 \
                p = (void*)tmp;             \
            }                               \
        } while (0);

static void offset_pointers (MG_Client* new_head, int size, int off)
{
    int i;
    MG_Layer* layer;

    if (off == 0) return;

    for (i = 0; i < size; i++) {

        off_pointer (new_head [i].next, off);
        off_pointer (new_head [i].prev, off);
    }

    layer = mgLayers;
    while (layer) {
        off_pointer (layer->cli_head, off);
        off_pointer (layer->cli_active, off);

        layer = layer->next;
    }
}

/* alloc more entries in the client[] array */
static BOOL client_alloc (void)
{
    int i;

    if (mgClients == NULL)
        mgClients = malloc (NALLOC * sizeof(MG_Client));
    else {
        char* new_head = realloc (mgClients, 
                        (mgClientSize + NALLOC) * sizeof(MG_Client));

        if (new_head) {
            offset_pointers ((MG_Client*) new_head, mgClientSize, 
                            new_head - (char*)mgClients);
        }

        mgClients = (MG_Client*) new_head;
    }

    if (mgClients == NULL)
        return FALSE;

    /* have to initialize the new entries */
    for (i = mgClientSize; i < mgClientSize + NALLOC; i++) {
        memset (mgClients + i, 0, sizeof (MG_Client));
        mgClients[i].fd        = -1;    /* fd of -1 means entry available */
    }

    mgClientSize += NALLOC;
    return TRUE;
}

/* 
 * Called by IdleHandler4Server() when connection request 
 * from a new client arrives 
 */
int __mg_client_add (int fd, pid_t pid, uid_t uid)
{
    int i;

    if (mgClients == NULL)        /* first time we're called */
        if (!client_alloc ())
            return -1;
again:
    for (i = 0; i < mgClientSize; i++) {
        if (mgClients[i].fd == -1) {    /* find an available entry */
            mgClients[i].fd = fd;
            mgClients[i].pid = pid;
            mgClients[i].uid = uid;
            mgClients[i].has_dirty = FALSE;
            mgClients[i].last_live_time = __mg_timer_counter;
            mgClients[i].layer = NULL;
            return (i);    /* return index in client[] array */
        }
    }

    /* client array full, time to realloc for more */
    if (!client_alloc ())
        return -1;

    goto again;        /* and search again (will work this time) */
}

/* Called by IdleHandler4Server() when we're done with a client */
void __mg_client_del (int cli)
{
    MG_Client* deleting = mgClients + cli;
    MG_Layer* layer = deleting->layer;

    _MG_PRINTF ("SERVER: Remove a client: %s\n", deleting->name);

    if (layer == NULL)
        goto ret;

    if (deleting->global_res)
        __mg_release_global_res (cli);
    __mg_remove_all_znodes_of_client (cli);

    if (deleting->next)
        deleting->next->prev = deleting->prev;
    if (deleting->prev) 
        deleting->prev->next = deleting->next;

    if (deleting == layer->cli_head) {
        layer->cli_head = deleting->next;
#if 0
        if (layer->cli_head == NULL) {
            if (__mg_free_layer (layer)) {
                SendMessage (HWND_DESKTOP, MSG_TOPMOSTCHANGED, 0, 0);
                PostMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
            }
            PostMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
            goto ret;
        }
#endif
    }

    if (deleting == layer->cli_active) {
        layer->cli_active = deleting->next;
        if (layer->cli_active == NULL) {
            layer->cli_active = layer->cli_head;
        }
    }

ret:
    memset (deleting, 0, sizeof (MG_Client));
    deleting->fd = -1;
}

void __mg_remove_client (int cli, int clifd)
{
#ifdef _DEBUG
    err_msg ("client closed: uid %d, fd %d", mgClients [cli].uid, clifd);
#endif
    __mg_client_del (cli);    /* client has closed conn */
    FD_CLR (clifd, &mg_rfdset);
    close (clifd);
}

int __mg_send2client (MSG* msg, MG_Client* client)
{
    int ret;

    if (__mg_timer_counter < (client->last_live_time + THRES_LIVE)) {
        ret = sock_write_t (client->fd, msg, sizeof (MSG), TO_SOCKIO);

        switch (ret) {
        case SOCKERR_TIMEOUT:
            client->last_live_time = 0;
            break;
#if 0
        case SOCKERR_CLOSED:
            {
                int cli = client - mgClients;
                if (OnNewDelClient) OnNewDelClient (LCO_DEL_CLIENT, cli);
                __mg_remove_client (cli, client->fd);
            }
            break;
#endif
        }

        return ret;
    }
    else
        return SOCKERR_OK;
}

void __mg_set_active_client (MG_Client* client)
{
    MG_Layer* layer;

    if (client)
        layer = client->layer;
    else
        layer = mgTopmostLayer;

    if (layer == NULL || layer->cli_active == client)
        return;

    if (layer->cli_active) {
        MSG msg = {0, MSG_SETFOCUS, 0, 0, __mg_timer_counter};

        __mg_send2client (&msg, layer->cli_active);
    }

    /* Notify that active client changed. */
    if (OnChangeLayer)
        OnChangeLayer (LCO_ACTIVE_CHANGED, layer, client);
    layer->cli_active = client;
}

/* send message to client(s) */
int GUIAPI Send2Client (MSG* msg, int cli)
{
	
    int i, n;
    MG_Client* client;

    if (!mgIsServer)
        return -1;

    if (cli >= 0 && cli < mgClientSize && mgClients [cli].fd != -1) {
        return __mg_send2client (msg, mgClients + cli);
    }
    /* send to active client */
    else if (cli == CLIENT_ACTIVE) {
        if (mgTopmostLayer && mgTopmostLayer->cli_active) {
            return __mg_send2client (msg, mgTopmostLayer->cli_active);
        }
    }
    /* send to topmost clients */
    else if (cli == CLIENTS_TOPMOST) {
        if (mgTopmostLayer) {
            client = mgTopmostLayer->cli_head;
            while (client) {
                if ((n = __mg_send2client (msg, client)) < 0)
                    return n;
                client = client->next;
            }
        }
    }
    /* send to all clients except topmost client */
    else if (cli == CLIENTS_EXCEPT_TOPMOST) {
        client = mgClients;
        for (i = 0; i < mgClientSize; i++, client++) {
            if (client->layer != mgTopmostLayer) {
                if ((n = __mg_send2client (msg, client)) < 0)
                    return n;
            }
        }
    }
    /* send to all clients */
    else if (cli == CLIENTS_ALL) {
        client = mgClients;
        for (i = 0; i < mgClientSize; i++, client++) {
            if ((n = __mg_send2client (msg, client)) < 0)
                return n;
        }
    }
    else
        return SOCKERR_INVARG;

    return SOCKERR_OK;
}

BOOL GUIAPI Send2TopMostClients (int iMsg, WPARAM wParam, LPARAM lParam)
{
    MSG msg = {0, iMsg, wParam, lParam, __mg_timer_counter};

    if (!mgIsServer)
        return FALSE;

    if (Send2Client (&msg, CLIENTS_TOPMOST) < 0)
        return FALSE;

    return TRUE;
}

BOOL GUIAPI Send2ActiveWindow (const MG_Layer* layer, 
                int iMsg, WPARAM wParam, LPARAM lParam)
{
    int active_win;

    if (!mgIsServer || NULL == layer)
        return FALSE;

    active_win = ((ZORDERINFO*)layer->zorder_info)->active_win;
    if (active_win <= 0)
        return FALSE;

    if (__mg_post_msg_by_znode (layer->zorder_info, 
                            active_win, iMsg, wParam, lParam) > 0)
        return TRUE;

    return FALSE;
}

BOOL GUIAPI SetTopmostClient (int cli)
{
    if (!mgIsServer)
        return FALSE;

    if (cli < 0 || cli >= mgClientSize || mgClients [cli].fd == -1)
        return FALSE;
                 
    return ServerSetTopmostLayer (mgClients [cli].layer);
}

int GUIAPI GetClientByPID (int pid)
{
    int i;

    if (!mgIsServer)
        return -1;

    for (i = 0; i < mgClientSize; i++) {
        if (mgClients[i].pid == pid) {
            if (mgClients[i].fd == -1)
                return -1;
            return i;
        }
    }

    return -1;
}

