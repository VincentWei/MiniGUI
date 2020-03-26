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
** desktop-procs.c: The desktop procedures for MiniGUI-Processes
**
** Current maintainer: Wei Yongming.
**
** Derived from original desktop-lite.c (2005/08/16)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"

#ifdef _MGRM_PROCESSES

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "menu.h"
#include "timer.h"
#include "dc.h"
#include "icon.h"
#include "sharedres.h"
#include "misc.h"
#include "ourhdr.h"
#include "client.h"
#include "server.h"
#include "drawsemop.h"
#include "devfont.h"

/******************************* global data *********************************/
PMSGQUEUE __mg_dsk_msg_queue;

GHANDLE __mg_layer;

static void lock_zi_for_read (const ZORDERINFO* zi)
{
    struct sembuf sb;

    if (mgIsServer) return;

again:
    sb.sem_num = zi->zi_semnum;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    if (semop (zi->zi_semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }

}

static void unlock_zi_for_read (const ZORDERINFO* zi)
{
    struct sembuf sb;

    if (mgIsServer) return;

again:
    sb.sem_num = zi->zi_semnum;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    if (semop (zi->zi_semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

static int do_drag_drop_window (int msg, int x, int y);
static void lock_zi_for_change (const ZORDERINFO* zi)
{
    int clients = 0;
    struct sembuf sb;

    clients = MAX_NR_ZNODES (zi);

#ifndef _MGSCHEMA_COMPOSITING
    /* Cancel the current drag and drop operation */
    do_drag_drop_window (MSG_IDLE, 0, 0);
#endif

again:
    sb.sem_num = zi->zi_semnum;
    sb.sem_op = -clients;
    sb.sem_flg = SEM_UNDO;

    if (semop (zi->zi_semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

static void unlock_zi_for_change (const ZORDERINFO* zi)
{
    int clients = 0;
    struct sembuf sb;
    clients = MAX_NR_ZNODES (zi);

again:
    sb.sem_num = zi->zi_semnum;
    sb.sem_op = clients;
    sb.sem_flg = SEM_UNDO;

    if (semop (zi->zi_semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

static inline void* get_zi_from_client (int cli)
{
    return (((cli>0)?mgClients[cli].layer:mgTopmostLayer)->zorder_info);
}

static inline MG_Layer* get_layer_from_client (int cli)
{
    return ((cli>0)?mgClients[cli].layer:mgTopmostLayer);
}

/***************** Initialization/Termination routines ***********************/
#include "desktop.c"

static BOOL InitWndManagementInfo (void)
{
    __mg_captured_wnd = 0;

    __mg_ime_wnd = 0;

#ifdef _MGHAVE_MENU
    sg_ptmi = NULL;
#endif

    sg_hCaretWnd = 0;
    return TRUE;
}

static LRESULT DesktopWinProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void init_desktop_win (void)
{
    static MAINWIN desktop_win;
    PMAINWIN pDesktopWin;

    LICENSE_SET_MESSAGE_OFFSET();

    pDesktopWin = &desktop_win;

    pDesktopWin->pMsgQueue          = __mg_dsk_msg_queue;
    pDesktopWin->MainWindowProc     = DesktopWinProc;

    pDesktopWin->DataType           = TYPE_HWND;
    pDesktopWin->WinType            = TYPE_ROOTWIN;

    pDesktopWin->pLogFont           = GetSystemFont (SYSLOGFONT_WCHAR_DEF);
    if (mgIsServer)
        pDesktopWin->spCaption      = "THE DESKTOP OF THE SERVER";
    else
        pDesktopWin->spCaption = "THE DESKTOP OF CLIENT";

    if (mgIsServer) {
#ifndef _MGSCHEMA_COMPOSITING
        pDesktopWin->pGCRInfo       = &sg_ScrGCRInfo;
#endif
        pDesktopWin->idx_znode      = 0;
    }

    pDesktopWin->pMainWin           = pDesktopWin;
    pDesktopWin->we_rdr             = __mg_def_renderer;

    __mg_hwnd_desktop = (HWND)pDesktopWin;
    __mg_dsk_win  = pDesktopWin;
    __mg_dsk_msg_queue->pRootMainWin = __mg_dsk_win;
}

BOOL mg_InitDesktop (void)
{
#if 0   /* move to init-lite.c */
    /* Since 5.0.0: allocate message queue for desktop thread */
    if (!(__mg_dsk_msg_queue = mg_AllocMsgQueueForThisThread ()) ) {
        _WRN_PRINTF ("failed to allocate message queue\n");
        return FALSE;
    }
#endif  /* moved code */

#ifndef _MGSCHEMA_COMPOSITING
    /* Init heap of clipping rects. */
    InitFreeClipRectList (&sg_FreeClipRectList, SIZE_CLIPRECTHEAP);
#endif

    /* Init heap of invalid rects.*/
    InitFreeClipRectList (&sg_FreeInvRectList, SIZE_INVRECTHEAP);

    // Init Window Management information.
    if (!InitWndManagementInfo ())
        return FALSE;

    init_desktop_win ();

    return TRUE;
}

void mg_TerminateDesktop (void)
{
    /* Since 5.0.0: message queue for desktop thread was dynamically allocated */
    mg_FreeMsgQueueForThisThread ();
    __mg_dsk_msg_queue = NULL;

#ifndef _MGSCHEMA_COMPOSITING
    DestroyFreeClipRectList (&sg_FreeClipRectList);
#endif
    DestroyFreeClipRectList (&sg_FreeInvRectList);
}

/********************** Common routines for ZORDERINFO ***********************/
#ifndef _MGSCHEMA_COMPOSITING
static int update_client_window_rgn (int cli, HWND hwnd)
{
    int ret = 0;
    CLIPRECT* crc = sg_UpdateRgn.head;

    if (!mgIsServer)
        return -1;

    if (!hwnd) {
        return 0;
    }

    if (cli == 0) {
        while (crc) {
            __mg_update_window (hwnd, crc->rc.left, crc->rc.top,
                            crc->rc.right, crc->rc.bottom);

            crc = crc->next;
        }
    }
    else {
        MSG msg = {hwnd, MSG_UPDATECLIWIN, 0, 0, __mg_tick_counter};

        while (crc) {
            msg.wParam = MAKELONG (crc->rc.left, crc->rc.top);
            msg.lParam = MAKELONG (crc->rc.right, crc->rc.bottom);

            ret = __mg_send2client (&msg, mgClients + cli);
            if (ret < 0) {
                _WRN_PRINTF ("__mg_send2client failed\n");
                break;
            }

            crc = crc->next;
        }
    }

    if (ret < 0)
        return -1;

    return 0;
}
#endif /* not defined _MGSCHEMA_COMPOSITING */

/*********************** Client-side routines ********************************/

void __mg_start_client_desktop (void)
{
    SendMessage (HWND_DESKTOP, MSG_STARTSESSION, 0, 0);
}

static intptr_t cliAllocZOrderNode (PMAINWIN pWin, const COMPOSITINGINFO* ct_info)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;
    /* Since 5.0.0, use extra data of request to pass the caption */
    const char* caption;

    info.id_op = ID_ZOOP_ALLOC;
    info.flags = get_znode_flags_from_style (pWin);
    info.hwnd = (HWND)pWin;
    info.main_win = (HWND)pWin->pMainWin;

    if (pWin->spCaption) {
        caption = pWin->spCaption;
#if 0 /* Since 5.0.0, use extra data of request to pass the caption */
        if (strlen (pWin->spCaption) <= MAX_CAPTION_LEN) {
            strcpy (info.caption, pWin->spCaption);
        } else {
            memcpy (info.caption, pWin->spCaption, MAX_CAPTION_LEN);
            info.caption[MAX_CAPTION_LEN] = '\0';
        }
#endif
    }
    else {
        caption = "<No caption>";
    }

    dskGetWindowRectInScreen (pWin, &info.rc);

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

#ifdef _MGSCHEMA_COMPOSITING
    assert (pWin->surf && pWin->surf->shared_header);

    info.surf_flags = pWin->surf->flags;
    info.surf_size = pWin->surf->shared_header->map_size;

    if (ct_info) {
        info.ct = ct_info->type;
        info.ct_arg = ct_info->arg;
    }
    else {
        info.ct = CT_OPAQUE;
        info.ct_arg = 0;
    }

    if (ClientRequestEx2 (&req, caption, strlen(caption) + 1,
            pWin->surf->shared_header->fd,
            &ret, sizeof (intptr_t), NULL) < 0)
        return -1;
#else   /* ndef _MGSCHEMA_COMPOSITING */
    if (ClientRequestEx (&req, caption, strlen(caption) + 1,
            &ret, sizeof (intptr_t)) < 0)
        return -1;
#endif  /* ndef _MGSCHEMA_COMPOSITING */

    return ret;
}

static intptr_t cliFreeZOrderNode (PMAINWIN pWin)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_FREE;
    info.idx_znode = pWin->idx_znode;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}

static intptr_t cliMove2Top (PMAINWIN pWin)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_MOVE2TOP;
    info.idx_znode = pWin->idx_znode;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}

static intptr_t cliShowWindow (PMAINWIN pWin)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_SHOW;
    info.idx_znode = pWin->idx_znode;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}

static intptr_t cliHideWindow (PMAINWIN pWin)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_HIDE;
    info.idx_znode = pWin->idx_znode;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}

static intptr_t cliMoveWindow (PMAINWIN pWin, const RECT* rcWin, int fd)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_MOVEWIN;
    info.idx_znode = pWin->idx_znode;
    info.rc = *rcWin;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

#ifdef _MGSCHEMA_COMPOSITING
    info.surf_flags = pWin->surf->flags;
    info.surf_size = pWin->surf->shared_header->map_size;
#endif

    if (ClientRequestEx2 (&req, NULL, 0, fd,
                &ret, sizeof (intptr_t), NULL) < 0)
        return -1;

    return ret;
}

static intptr_t cliSetActiveWindow (PMAINWIN pWin)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_SETACTIVE;
    info.idx_znode = pWin?pWin->idx_znode:0;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}

#ifdef _MGHAVE_MENU
static intptr_t cliStartTrackPopupMenu (PTRACKMENUINFO ptmi)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_START_TRACKMENU;
    info.rc = ptmi->rc;
    info.hwnd = (HWND)ptmi;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

#ifdef _MGSCHEMA_COMPOSITING
    {
        GAL_Surface* surf;

        assert (ptmi->dc != HDC_INVALID);

        surf = GetSurfaceFromDC (ptmi->dc);
        assert (surf->shared_header);

        info.surf_flags = surf->flags;
        info.surf_size = surf->shared_header->map_size;
        if (ClientRequestEx2 (&req, NULL, 0, surf->shared_header->fd,
                &ret, sizeof (intptr_t), NULL) < 0)
            return -1;
    }
#else   /* ndef _MGSCHEMA_COMPOSITING */
    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;
#endif  /* ndef _MGSCHEMA_COMPOSITING */

    return ret;
}

static intptr_t cliEndTrackPopupMenu (PTRACKMENUINFO ptmi)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_END_TRACKMENU;
    info.idx_znode = ptmi->idx_znode;
    info.hwnd = 0;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}

static intptr_t cliForceCloseMenu (void)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_CLOSEMENU;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}
#endif /* defined _MGHAVE_MENU */

static intptr_t cliEnableWindow (PMAINWIN pWin, int flags)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_ENABLEWINDOW;
    info.idx_znode = pWin?pWin->idx_znode:0;
    info.flags = flags;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}

static intptr_t cliStartDragWindow (PMAINWIN pWin, const DRAGINFO* drag_info)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_STARTDRAG;
    info.idx_znode = pWin->idx_znode;
    info.location = drag_info->location;
    info.rc.left = drag_info->init_x;
    info.rc.top = drag_info->init_y;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}

static intptr_t cliCancelDragWindow (PMAINWIN pWin)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_CANCELDRAG;
    info.idx_znode = pWin->idx_znode;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}

static intptr_t cliChangeCaption (PMAINWIN pWin)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;
    const char* caption;

    info.id_op = ID_ZOOP_CHANGECAPTION;
    info.idx_znode = pWin->idx_znode;

#if 0 /* Since 5.0.0, use extra data to pass the caption */
    if (strlen (pWin->spCaption) <= MAX_CAPTION_LEN) {
        strcpy (info.caption, pWin->spCaption);
    } else {
        memcpy (info.caption, pWin->spCaption, MAX_CAPTION_LEN);
        info.caption[MAX_CAPTION_LEN] = '\0';
    }
#else
    if (pWin->spCaption) {
        caption = pWin->spCaption;
    }
    else {
        caption = "<No caption>";
    }
#endif

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);
    if ((ret = ClientRequestEx (&req, caption, strlen (caption) + 1,
                &ret, sizeof (intptr_t))) < 0) {
        return -1;
    }

    return ret;
}

/* Since 5.0.0 */
static intptr_t cliSetMainWinAlwaysTop (PMAINWIN pWin, BOOL fSet)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_SETALWAYSTOP;
    info.idx_znode = pWin->idx_znode;
    info.flags = fSet;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}

#ifdef _MGSCHEMA_COMPOSITING
static intptr_t cliSetMainWinCompositing (PMAINWIN pWin,
        const COMPOSITINGINFO* my_info)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_SETCOMPOSITING;
    info.idx_znode = pWin->idx_znode;
    info.ct = my_info->type;
    info.ct_arg = my_info->arg;

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

    return ret;
}
#endif /* defined _MGSCHEMA_COMPOSTING */

/*********************** Server-side routines ********************************/

void __mg_start_server_desktop (void)
{
#ifndef _MGSCHEMA_COMPOSITING
    RECT rcScr = GetScreenRect ();

    InitClipRgn (&sg_ScrGCRInfo.crgn, &sg_FreeClipRectList);
    SetClipRgn (&sg_ScrGCRInfo.crgn, &rcScr);
    sg_ScrGCRInfo.age = 0;
    sg_ScrGCRInfo.old_zi_age = 0;

    InitClipRgn (&sg_UpdateRgn, &sg_FreeClipRectList);
    MAKE_REGION_INFINITE(&sg_UpdateRgn);
#endif /* not defiend _MGSCHEMA_COMPOSITING */

    SendMessage (HWND_DESKTOP, MSG_STARTSESSION, 0, 0);
    SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
}

static intptr_t cliAllocZOrderMaskRect (HWND pWin, const RECT4MASK* rc, int nr_rc)
{
    intptr_t ret;
    REQUEST req;
    ZORDERMASKRECTOPINFO info;

    if (!rc || !nr_rc)
        return -1;

    info.id_op = ID_ZOOP_MASKRECT_SET;
    info.idx_znode = ((PMAINWIN)pWin)->idx_znode;
    info.flags = get_znode_flags_from_style((PMAINWIN)pWin);
    info.nr_maskrect = nr_rc;

    req.id = REQID_ZORDERMASKRECTOP;
    req.data = &info;
    req.len_data = sizeof (info);

    if ((ret = ClientRequestEx (&req, rc, sizeof(RECT4MASK)*nr_rc,
                &ret, sizeof (intptr_t))) < 0) {
        return -1;
    }

    return ret;
}

static intptr_t cliFreeZOrderMaskRect (PMAINWIN pWin)
{
    intptr_t ret;
    REQUEST req;
    ZORDERMASKRECTOPINFO info;

    info.id_op = ID_ZOOP_MASKRECT_FREE;
    info.idx_znode = pWin->idx_znode;
    info.flags = get_znode_flags_from_style((PMAINWIN)pWin);

    req.id = REQID_ZORDERMASKRECTOP;
    req.data = &info;
    req.len_data = sizeof(ZORDERMASKRECTOPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0) {
        return -1;
    }
    return ret;
}

static inline int srvAllocZOrderMaskRect (int cli, int idx_znode,
        int flags, const RECT4MASK *rc, const int nr_rc)
{
    return AllocZOrderMaskRect (cli, idx_znode, flags, rc, nr_rc);
}

static inline int srvFreeZOrderMaskRect (int cli, int idx_znode)
{
    return FreeZOrderMaskRect (cli, idx_znode);
}

ON_ZNODE_OPERATION OnZNodeOperation;

static int srvAllocZOrderNode (int cli, HWND hwnd, HWND main_win,
                DWORD flags, const RECT *rc, const char *caption,
                Uint32 surf_flags, size_t surf_size, int fd,
                int ct, DWORD ct_arg)
{
    int free_slot;
    HDC memdc = HDC_INVALID;

#ifdef _MGSCHEMA_COMPOSITING
    GAL_Surface *surf = NULL;

    if (cli == 0) {
        PMAINWIN pwin = (PMAINWIN)hwnd;
        surf = pwin->surf;

        // prevent to free this surface when deleting memdc for znode.
        surf->refcount++;
    }
    else if (fd >= 0) {
        surf = GAL_AttachSharedRGBSurface (fd, surf_size, surf_flags, TRUE);
        close (fd);
    }
    else {
        _ERR_PRINTF("KERNEL: not server but fd for shared surface is invalid\n");
        return -1;
    }

    if (surf) {
        memdc = CreateMemDCFromSurface (surf);
        if (memdc == HDC_INVALID) {
            if (cli > 0) {
                GAL_FreeSurface (surf);
            }

            _ERR_PRINTF("KERNEL: failed to create memory dc for znode\n");
            return -1;
        }
    }
    else {
        _ERR_PRINTF("KERNEL: invalid surface or failed to attach to surface\n");
        return -1;
    }
#endif /* def _MGSCHEMA_COMPOSITING */

    free_slot = AllocZOrderNode (cli, hwnd,
                    main_win, flags, rc, caption, memdc, ct, ct_arg);

    if ((free_slot != -1) && OnZNodeOperation)
        OnZNodeOperation (ZNOP_ALLOCATE, cli, free_slot);

    return free_slot;
}

#ifdef _MGSCHEMA_SHAREDFB
/* only work for _MGSCHEMA_SHAREDFB */
static BOOL _cb_update_cli_znode (void* context,
                const ZORDERINFO* zi, ZORDERNODE* znode)
{
    RECT rcInv;
    RECT rcScr = GetScreenRect ();
    int cli = (int)(intptr_t)context;

    if (znode->cli == cli && znode->flags & ZOF_VISIBLE && znode->hwnd) {
       MSG msg = {znode->hwnd, MSG_UPDATECLIWIN, 0, 0, __mg_tick_counter};

        if (!IsRectEmpty(&znode->dirty_rc)) {

            if (IntersectRect(&rcScr, &znode->rc, &rcScr) &&
                    IntersectRect(&rcInv, &znode->dirty_rc, &rcScr)) {
                rcInv = znode->dirty_rc;
                msg.wParam = MAKELONG (rcInv.left, rcInv.top);
                msg.lParam = MAKELONG (rcInv.right, rcInv.bottom);

                _DBG_PRINTF ("Send MSG_UPDATECLIWIN to client window (%s): "
                        "%d, %d, %d x %d\n",
                        znode->caption,
                        rcInv.left, rcInv.top,
                        rcInv.right - rcInv.left,
                        rcInv.bottom - rcInv.top);

                __mg_send2client (&msg, mgClients + znode->cli);
            }

            SetRectEmpty (&znode->dirty_rc);
            return TRUE;
        }
    }

    return FALSE;
}

void __mg_check_dirty_znode (int cli)
{
    ZORDERINFO* zi = (ZORDERINFO *)get_zi_from_client (cli);

    do_for_all_znodes ((void*)(intptr_t)cli, zi,
                    _cb_update_cli_znode, ZT_ALL & ~ZT_GLOBAL);

    mgClients [cli].has_dirty = FALSE;
}
#endif /* defined _MGSCHEMA_SHAREDFB */

static intptr_t srvSetActiveWindow (int cli, int idx_znode)
{
    HWND hRet = dskSetActiveZOrderNode (cli, idx_znode);

    if ((hRet != HWND_INVALID) && OnZNodeOperation)
        OnZNodeOperation (ZNOP_SETACTIVE, cli, idx_znode);

    return (intptr_t)hRet;
}

static int srvFreeZOrderNode (int cli, int idx_znode)
{
    HDC memdc = HDC_INVALID;
    int ret = FreeZOrderNode (cli, idx_znode, &memdc);

    if (!ret && OnZNodeOperation)
        OnZNodeOperation (ZNOP_FREE, cli, idx_znode);

#ifdef _MGSCHEMA_COMPOSITING
    if (memdc != HDC_INVALID) {
        DeleteMemDC (memdc);
    }
#endif  /* def _MGSCHEMA_COMPOSITING */

    return ret;
}

static int srvMove2Top (int cli, int idx_znode)
{
    int ret = dskMove2Top (cli, idx_znode);

    if (!ret && OnZNodeOperation)
        OnZNodeOperation (ZNOP_MOVE2TOP, cli, idx_znode);

    return ret;
}

static int srvShowWindow (int cli, int idx_znode)
{
    int ret = dskShowWindow (cli, idx_znode);

    if (!ret && OnZNodeOperation)
        OnZNodeOperation (ZNOP_SHOW, cli, idx_znode);

    return ret;
}

static int srvHideWindow (int cli, int idx_znode)
{
    int ret = dskHideWindow (cli, idx_znode);

    if (!ret && OnZNodeOperation)
        OnZNodeOperation (ZNOP_HIDE, cli, idx_znode);

    return ret;
}

static int srvMoveWindow (int cli, int idx_znode, const RECT* rcWin,
       HWND hwnd, Uint32 surf_flags, size_t surf_size, int fd)
{
    HDC memdc = HDC_INVALID;
    int ret;

#ifdef _MGSCHEMA_COMPOSITING
    GAL_Surface *surf = NULL;

    if (fd >= 0) {  // surface resized
        if (cli == 0) {
            PMAINWIN pwin = (PMAINWIN)hwnd;
            surf = pwin->surf;
            // prevent to free this surface when deleting memdc for znode.
            surf->refcount++;
        }
        else {
            surf = GAL_AttachSharedRGBSurface (fd, surf_size, surf_flags, TRUE);
            close (fd);
        }

        if (surf) {
            memdc = CreateMemDCFromSurface (surf);
            if (memdc == HDC_INVALID) {
                if (cli > 0) {
                    GAL_FreeSurface (surf);
                }

                _ERR_PRINTF("KERNEL: failed to create new memory dc for znode\n");
                return -1;
            }
        }
        else {
            _ERR_PRINTF("KERNEL: failed to attach to shared surface\n");
            return -1;
        }
    }

#endif /* def _MGSCHEMA_COMPOSITING */

    ret = dskMoveWindow (cli, idx_znode, memdc, rcWin);
    if (ret == 0 && OnZNodeOperation)
        OnZNodeOperation (ZNOP_MOVEWIN, cli, idx_znode);

    return ret;
}

static int srvEnableWindow (int cli, int idx_znode, int flags)
{
    int ret = dskEnableZOrderNode (cli, idx_znode, flags);

    if (!ret && OnZNodeOperation) {
        if (flags)
            OnZNodeOperation (ZNOP_ENABLEWINDOW, cli, idx_znode);
        else
            OnZNodeOperation (ZNOP_DISABLEWINDOW, cli, idx_znode);
    }

    return ret;
}

typedef struct _DRAGDROPINFO {
    int cli, idx_znode;
    HWND hwnd;
    ZORDERINFO* zi;
    RECT rc;

    int location;
    int last_x, last_y;
} DRAGDROPINFO;

static DRAGDROPINFO _dd_info = {-1};

static int srvStartDragWindow (int cli, int idx_znode,
                int location, int init_x, int init_y)
{
    ZORDERINFO* zi = get_zorder_info(cli);
    ZORDERNODE* nodes;
#ifndef _MGSCHEMA_COMPOSITING
    RECT rcScr = GetScreenRect ();
#endif

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    if (_dd_info.cli >= 0)
        return -1;

    nodes = GET_ZORDERNODE(zi);

#ifndef _MGSCHEMA_COMPOSITING
    lock_zi_for_change (zi);
#endif

    _dd_info.cli = cli;
    _dd_info.idx_znode = idx_znode;
    _dd_info.hwnd = nodes [idx_znode].hwnd;
    _dd_info.zi = zi;
    _dd_info.rc = nodes [idx_znode].rc;

    _dd_info.location = location;
    _dd_info.last_x = init_x;
    _dd_info.last_y = init_y;

    switch (location) {
        case HT_CAPTION:
            //SetDefaultCursor (GetSystemCursor (IDC_MOVE));
            SetCursor (GetSystemCursor (IDC_MOVE));
            break;
        case HT_BORDER_TOP:
        case HT_BORDER_BOTTOM:
            //SetDefaultCursor (GetSystemCursor (IDC_SIZENS));
            SetCursor (GetSystemCursor (IDC_SIZENS));
            break;
        case HT_BORDER_LEFT:
        case HT_BORDER_RIGHT:
            //SetDefaultCursor (GetSystemCursor (IDC_SIZEWE));
            SetCursor (GetSystemCursor (IDC_SIZEWE));
            break;
        case HT_CORNER_TL:
        case HT_CORNER_BR:
            //SetDefaultCursor (GetSystemCursor (IDC_SIZENWSE));
            SetCursor (GetSystemCursor (IDC_SIZENWSE));
            break;
        case HT_CORNER_BL:
        case HT_CORNER_TR:
            //SetDefaultCursor (GetSystemCursor (IDC_SIZENESW));
            SetCursor (GetSystemCursor (IDC_SIZENESW));
            break;
        default:
            _WRN_PRINTF ("bad location\n");
            break;
    }

#ifndef _MGSCHEMA_COMPOSITING
    SetPenColor (HDC_SCREEN_SYS, PIXEL_lightwhite);
    SelectClipRect (HDC_SCREEN_SYS, &rcScr);
    do_for_all_znodes (NULL, zi, _cb_exclude_rc, ZT_GLOBAL);
    FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);
#endif

    if (OnZNodeOperation)
        OnZNodeOperation (ZNOP_STARTDRAG, cli, idx_znode);

    return 0;
}

static int srvCancelDragWindow (int cli, int idx_znode)
{
    ZORDERINFO* zi = get_zorder_info(cli);
#ifndef _MGSCHEMA_COMPOSITING
    RECT rcScr = GetScreenRect ();
#endif

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    if (_dd_info.cli == -1
                    || _dd_info.cli != cli
                    || _dd_info.idx_znode != idx_znode)
        return -1;

    _dd_info.cli = -1;
#ifndef _MGSCHEMA_COMPOSITING
    unlock_zi_for_change (zi);
    SelectClipRect (HDC_SCREEN_SYS, &rcScr);
    //SetDefaultCursor (GetSystemCursor (IDC_ARROW));
#endif

    if (OnZNodeOperation)
        OnZNodeOperation (ZNOP_CANCELDRAG, cli, idx_znode);

    return 0;
}

/* Since 5.0.0: make static */
static int do_drag_drop_window (int msg, int x, int y)
{
    HWND hwnd;
#ifndef _MGSCHEMA_COMPOSITING
    RECT rcScr = GetScreenRect ();
#endif

    if (_dd_info.cli < 0)
        return 0;

    if (msg == MSG_MOUSEMOVE) {
#ifndef _MGSCHEMA_COMPOSITING
        SetPenColor (HDC_SCREEN_SYS, PIXEL_lightwhite);
        FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);
#else
        MSG msg = {_dd_info.hwnd, MSG_WINDOWDROPPED, 0, 0, __mg_tick_counter};
#endif

        switch (_dd_info.location) {
        case HT_CAPTION:
            OffsetRect (&_dd_info.rc,
                            x - _dd_info.last_x,
                            y - _dd_info.last_y);
            break;

        case HT_BORDER_TOP:
            _dd_info.rc.top += y - _dd_info.last_y;
            break;

        case HT_BORDER_BOTTOM:
            _dd_info.rc.bottom += y - _dd_info.last_y;
            break;

        case HT_BORDER_LEFT:
            _dd_info.rc.left += x - _dd_info.last_x;
            break;

        case HT_BORDER_RIGHT:
            _dd_info.rc.right += x - _dd_info.last_x;
            break;

        case HT_CORNER_TL:
            _dd_info.rc.left += x - _dd_info.last_x;
            _dd_info.rc.top += y - _dd_info.last_y;
            break;

        case HT_CORNER_TR:
            _dd_info.rc.right += x - _dd_info.last_x;
            _dd_info.rc.top += y - _dd_info.last_y;
            break;

        case HT_CORNER_BL:
            _dd_info.rc.left += x - _dd_info.last_x;
            _dd_info.rc.bottom += y - _dd_info.last_y;
            break;

        case HT_CORNER_BR:
            _dd_info.rc.right += x - _dd_info.last_x;
            _dd_info.rc.bottom += y - _dd_info.last_y;
            break;

        default:
            _WRN_PRINTF ("bad location\n");
            break;
        }

#ifndef _MGSCHEMA_COMPOSITING
        FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);
        SyncUpdateDC (HDC_SCREEN_SYS);
#else   /* not defined _MGSCHEMA_COMPOSITING */

        msg.wParam = MAKELONG (_dd_info.rc.left, _dd_info.rc.top);
        msg.lParam = MAKELONG (_dd_info.rc.right, _dd_info.rc.bottom);
        /* post MSG_WINDOWDROPPED to the target window */
        if (_dd_info.cli == 0) {
            PostMessage (_dd_info.hwnd, MSG_WINDOWDROPPED,
                            msg.wParam, msg.lParam);
        }
        else {
            mgClients [_dd_info.cli].last_live_time = __mg_tick_counter;
            __mg_send2client (&msg, mgClients + _dd_info.cli);
        }
#endif  /* defined _MGSCHEMA_COMPOSITING */
        _dd_info.last_x = x;
        _dd_info.last_y = y;
    }
    else {
        MSG msg = {_dd_info.hwnd, MSG_WINDOWDROPPED, 0, 0, __mg_tick_counter};

        msg.wParam = MAKELONG (_dd_info.rc.left, _dd_info.rc.top);
        msg.lParam = MAKELONG (_dd_info.rc.right, _dd_info.rc.bottom);

#ifndef _MGSCHEMA_COMPOSITING
        SetPenColor (HDC_SCREEN_SYS, PIXEL_lightwhite);
        FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);
        SyncUpdateDC (HDC_SCREEN_SYS);
#endif  /* not defined _MGSCHEMA_COMPOSITING */

        /* post MSG_WINDOWDROPPED to the target window */
        if (_dd_info.cli == 0) {
            PostMessage (_dd_info.hwnd, MSG_WINDOWDROPPED,
                            msg.wParam, msg.lParam);
        }
        else {
            mgClients [_dd_info.cli].last_live_time = __mg_tick_counter;
            __mg_send2client (&msg, mgClients + _dd_info.cli);
        }

#ifndef _MGSCHEMA_COMPOSITING
        unlock_zi_for_change (_dd_info.zi);
        SelectClipRect (HDC_SCREEN_SYS, &rcScr);
#endif

        __mg_get_znode_at_point (__mg_zorder_info, x, y, &hwnd);
        if (_dd_info.hwnd != hwnd)
            SetDefaultCursor (GetSystemCursor (IDC_ARROW));
        _dd_info.cli = -1;
    }

    return 1;
}

static int srvChangeCaption (int cli, int idx_znode, const char *caption)
{
    ZORDERINFO* zi = get_zorder_info(cli);
    ZORDERNODE* nodes;

    nodes = GET_ZORDERNODE(zi);

    if (caption && idx_znode > 0) {
#if 0 /* Since 5.0.0, use strdup to duplicate the caption */
        PLOGFONT menufont ;
        int fit_chars, pos_chars[MAX_CAPTION_LEN];
        int caplen;
        menufont = GetSystemFont (SYSLOGFONT_MENU);
        caplen = strlen(caption);

        if (caplen < 32) {
            strcpy (nodes[idx_znode].caption, caption);
        } else {
            get_text_char_pos (menufont, caption, caplen,
                    (32 - 3), /* '...' = 3*/ &fit_chars, pos_chars);
            memcpy(nodes[idx_znode].caption,
                   caption, pos_chars[fit_chars-1]);
            strcpy ((nodes[idx_znode].caption + pos_chars[fit_chars-1]),
                    "...");
        }
#else
        if (nodes[idx_znode].caption)
            free (nodes[idx_znode].caption);

        nodes[idx_znode].caption = strdup (caption);
#endif
    }

    if (OnZNodeOperation)
        OnZNodeOperation (ZNOP_CHANGECAPTION, cli, idx_znode);

    return 0;
}

/* Since 5.0.0 */
static inline int srvSetZNodeAlwaysTop (int cli, int idx_znode, BOOL fSet)
{
    return dskSetZNodeAlwaysTop (cli, idx_znode, fSet);
}

#ifdef _MGSCHEMA_COMPOSITING
static inline int srvSetZNodeCompositing (int cli, int idx_znode,
        int type, DWORD arg)
{
    int ret;

    ret = dskSetZNodeCompositing (cli, idx_znode, type, arg);
    if (ret == 0 && OnZNodeOperation) {
        OnZNodeOperation (ZNOP_COMPOSITINGCHANGED, cli, idx_znode);
    }

    return ret;
}
#endif /* _MGSCHEMA_COMPOSITING */

intptr_t __mg_do_zorder_maskrect_operation (int cli,
        const ZORDERMASKRECTOPINFO* info)
{
    intptr_t ret = -1;
#ifdef _MGSCHEMA_COMPOSITING
    RECT rc_org_bound;
    get_znode_mask_bound (cli, info->idx_znode, &rc_org_bound);
#endif

    switch (info->id_op) {
    case ID_ZOOP_MASKRECT_SET:
        ret = srvAllocZOrderMaskRect (cli, info->idx_znode,
                    info->flags, info->rc,
                    info->nr_maskrect);
        break;
    case ID_ZOOP_MASKRECT_FREE:
        ret = srvFreeZOrderMaskRect (cli, info->idx_znode);
        break;
    }

    /* Since 5.0.0 */
    if (ret == 0) {
        if (OnZNodeOperation)
            OnZNodeOperation (ZNOP_REGIONCHANGED, cli, info->idx_znode);

#ifdef _MGSCHEMA_COMPOSITING
        DO_COMPSOR_OP_ARGS (on_changed_rgn,
                get_layer_from_client (cli), info->idx_znode, &rc_org_bound);
#endif
    }
    return ret;
}

intptr_t __mg_do_zorder_operation (int cli, const ZORDEROPINFO* info,
        const char* caption, int fd)
{
    intptr_t ret = -1;

    switch (info->id_op) {
        case ID_ZOOP_ALLOC:
#ifdef _MGSCHEMA_COMPOSITING
            ret = srvAllocZOrderNode (cli, info->hwnd, info->main_win,
                        info->flags, &info->rc, caption,
                        info->surf_flags, info->surf_size, fd,
                        info->ct, info->ct_arg);
#else
            ret = srvAllocZOrderNode (cli, info->hwnd, info->main_win,
                        info->flags, &info->rc, caption,
                        0, 0, fd, CT_OPAQUE, 0);
#endif
            break;
        case ID_ZOOP_FREE:
            ret = srvFreeZOrderNode (cli, info->idx_znode);
            break;
        case ID_ZOOP_MOVE2TOP:
            ret = srvMove2Top (cli, info->idx_znode);
            break;
        case ID_ZOOP_SHOW:
            ret = srvShowWindow (cli, info->idx_znode);
            break;
        case ID_ZOOP_HIDE:
            ret = srvHideWindow (cli, info->idx_znode);
            break;
        case ID_ZOOP_MOVEWIN:
#ifdef _MGSCHEMA_COMPOSITING
            ret = srvMoveWindow (cli, info->idx_znode, &info->rc,
                    info->hwnd, info->surf_flags, info->surf_size, fd);
#else
            ret = srvMoveWindow (cli, info->idx_znode, &info->rc,
                    info->hwnd, 0, 0, 0);
#endif
            break;
        case ID_ZOOP_SETACTIVE:
            ret = srvSetActiveWindow (cli, info->idx_znode);
            break;
#ifdef _MGHAVE_MENU
        case ID_ZOOP_START_TRACKMENU:
#ifdef _MGSCHEMA_COMPOSITING
            ret = srvStartTrackPopupMenu (cli, &info->rc, info->hwnd,
                        info->surf_flags, info->surf_size, fd);
#else
            ret = srvStartTrackPopupMenu (cli, &info->rc, info->hwnd,
                        0, 0, fd);
#endif
            break;
        case ID_ZOOP_END_TRACKMENU:
            ret = srvEndTrackPopupMenu (cli, info->idx_znode);
            break;
        case ID_ZOOP_CLOSEMENU:
            ret = srvForceCloseMenu (cli);
            break;
#endif
        case ID_ZOOP_ENABLEWINDOW:
            ret = srvEnableWindow (cli, info->idx_znode, info->flags);
            break;
        case ID_ZOOP_STARTDRAG:
            ret = srvStartDragWindow (cli, info->idx_znode, info->location,
                            info->rc.left, info->rc.top);
            break;
        case ID_ZOOP_CANCELDRAG:
            ret = srvCancelDragWindow (cli, info->idx_znode);
            break;
        case ID_ZOOP_CHANGECAPTION:
            srvChangeCaption (cli, info->idx_znode, caption);
            break;
        case ID_ZOOP_SETALWAYSTOP:
            srvSetZNodeAlwaysTop (cli, info->idx_znode, info->flags);
            break;
#ifdef _MGSCHEMA_COMPOSITING
        case ID_ZOOP_SETCOMPOSITING:
            srvSetZNodeCompositing (cli, info->idx_znode,
                            info->ct, info->ct_arg);
            break;
#endif
        default:
            break;
    }

#if 0
    {
        ZORDERINFO* zi = get_zi_from_client (cli);
        ZORDERNODE* nodes;

        nodes = GET_ZORDERNODE(zi);

        dump_zorder_list (zi, nodes);
    }
#endif

    return ret;
}

#ifndef _MGSCHEMA_COMPOSITING
static BOOL _cb_intersect_rc_no_cli (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    ZORDERNODE* del_node = (ZORDERNODE*)context;

    if (node->cli != del_node->cli &&
                    node->flags & ZOF_VISIBLE &&
                    //SubtractClipRect (&sg_UpdateRgn, &node->rc)) {
                    subtract_rgn_by_node(&sg_UpdateRgn, zi, node)) {
        node->age ++;
        node->flags |= ZOF_IF_REFERENCE;
        return TRUE;
    }

    return FALSE;
}

static BOOL _cb_update_rc_nocli (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    int cli = (int)(intptr_t)context;

    if (node->flags & ZOF_VISIBLE && node->cli != cli &&
                    //SubtractClipRect (&sg_UpdateRgn, &node->rc)) {
                    subtract_rgn_by_node(&sg_UpdateRgn, zi, node)) {
        node->age ++;
        node->flags |= ZOF_IF_REFERENCE;
        return TRUE;
    }

    return FALSE;
}
#endif /* not defined _MGSCHEMA_COMPOSITING */

int __mg_remove_all_znodes_of_client (int cli)
{
    ZORDERINFO* zi = get_zorder_info(cli);
    ZORDERNODE* nodes;
    int level, slot, old_active;
#ifndef _MGSCHEMA_COMPOSITING
    int slot2;
    RECT rcScr = GetScreenRect();
#else
    MG_Layer* layer = get_layer_from_client (cli);
#endif
    RECT rc_bound = {0, 0, 0, 0};

    nodes = GET_ZORDERNODE(zi);

    if (zi->cli_trackmenu == cli) {
        int i;
        ZORDERNODE* menu_nodes = GET_MENUNODE(zi);
        /* lock zi for change */
        lock_zi_for_change (zi);

        for (i = 0; i < zi->nr_popupmenus; i++) {
            GetBoundRect (&rc_bound, &rc_bound, &menu_nodes [i].rc);
        }

#ifndef _MGSCHEMA_COMPOSITING
        SetClipRgn (&sg_UpdateRgn, &rc_bound);

        /* check influenced window zorder nodes */
        do_for_all_znodes ((void*)(intptr_t)cli,
                zi, _cb_update_rc_nocli, ZT_ALL);

        if (SubtractClipRect (&sg_UpdateRgn, &rcScr)) {
            nodes [0].age ++;
            nodes [0].flags |= ZOF_IF_REFERENCE;
        }

#else   /* defined _MGSCHEMA_COMPOSITING */
        for (i = (zi->nr_popupmenus - 1); i >= 0; i--) {
            DO_COMPSOR_OP_ARGS (on_hiding_ppp, i);
            DeleteMemDC (nodes [i].mem_dc);
        }
#endif  /* defined _MGSCHEMA_COMPOSITING */

        zi->cli_trackmenu = -1;
        zi->nr_popupmenus = 0;

        /* unlock zi for change */
        unlock_zi_for_change (zi);
#ifdef _MGSCHEMA_COMPOSITING
        DO_COMPSOR_OP_ARGS (on_closed_menu, &rc_bound);
#endif
    }

    /* lock zi for change */
    lock_zi_for_change (zi);

    /* Since 5.0.0: handle all levels in a loop */
    for (level = 0; level < NR_ZORDER_LEVELS; level++) {
        slot = zi->first_in_levels[level];
        for (; slot > 0; slot = nodes [slot].next) {
            if (nodes [slot].cli == cli) {
                DWORD flags = nodes [slot].flags;
                if (flags & ZOF_VISIBLE) {
#ifndef _MGSCHEMA_COMPOSITING
                    int al; // affected level

                    SetClipRgn (&sg_UpdateRgn, &nodes [slot].rc);
                    GetBoundRect (&rc_bound, &rc_bound, &nodes [slot].rc);

                    slot2 = nodes [slot].next;
                    for (; slot2 > 0; slot2 = nodes [slot2].next) {
                        if (nodes [slot2].cli != cli &&
                                nodes [slot2].flags & ZOF_VISIBLE &&
                                subtract_rgn_by_node(&sg_UpdateRgn,
                                    zi, &nodes [slot2])) {

                            nodes [slot2].age ++;
                            nodes [slot2].flags |= ZOF_IF_REFERENCE;
                        }
                    }

                    for (al = level + 1; al < NR_ZORDER_LEVELS; al++) {
                        do_for_all_znodes (nodes + slot, zi,
                                        _cb_intersect_rc_no_cli,
                                        _zts_for_level[al]);
                    }

                    if (!(nodes [0].flags & ZOF_IF_REFERENCE) &&
                                    SubtractClipRect (&sg_UpdateRgn, &rcScr)) {
                        nodes [0].age ++;
                        nodes [0].flags |= ZOF_IF_REFERENCE;
                    }
#else   /* defined _MGSCHEMA_COMPOSITING */
                    DO_COMPSOR_OP_ARGS (on_hiding_win, layer, slot);
#endif  /* defined _MGSCHEMA_COMPOSITING */
                }

                unchain_znode ((unsigned char *)(zi+1), nodes, slot);
                clean_znode_maskrect (zi, nodes, slot);

                if (zi->first_in_levels[level] == slot) {
                    zi->first_in_levels[level] = nodes [slot].next;
                }
                zi->nr_nodes_in_levels[level]--;

#ifdef _MGSCHEMA_COMPOSITING
                if (flags & ZOF_VISIBLE) {
                    DO_COMPSOR_OP_ARGS (on_dirty_screen, layer,
                            flags & ZOF_TYPE_FLAG_MASK, &nodes [slot].rc);
                }
#endif  /* defined _MGSCHEMA_COMPOSITING */
            }
        }
    }

    old_active = zi->active_win;
    if (nodes [old_active].cli == cli)
        zi->active_win = 0; /* set the active_win to desktop temp */

    /* unlock zi for change  */
    unlock_zi_for_change (zi);

#ifndef _MGSCHEMA_COMPOSITING
    /* update all znode if it's dirty */
    do_for_all_znodes (&rc_bound, zi, _cb_update_znode, ZT_ALL);

    if (nodes [0].flags & ZOF_IF_REFERENCE) {
        SendMessage (HWND_DESKTOP,
                        MSG_ERASEDESKTOP, 0, (WPARAM)&rc_bound);
        nodes [0].flags &= ~ZOF_IF_REFERENCE;
    }
#endif  /* not defined _MGSCHEMA_COMPOSITING */

    /* if active_win belongs to the client, change it */
    if (nodes [old_active].cli == cli) {
        int next_active = get_next_activable_mainwin (zi, 0);
        srvSetActiveWindow (nodes [next_active].cli, next_active);
    }

    return 0;
}

int __mg_do_change_topmost_layer (void)
{
    ZORDERINFO* old_zorder_info = __mg_zorder_info;
    ZORDERNODE *new_nodes, *old_nodes;
    unsigned char *old_use_bmp, *new_use_bmp;
#ifndef _MGSCHEMA_COMPOSITING
    int i;
#endif

#ifdef _MGHAVE_MENU
    srvForceCloseMenu (0);
#endif

    old_use_bmp = (unsigned char*)old_zorder_info + sizeof (ZORDERINFO);
    old_nodes = GET_ZORDERNODE(old_zorder_info);

    __mg_zorder_info = mgTopmostLayer->zorder_info;

    /* copy globals to new layer */
    lock_zi_for_change (__mg_zorder_info);
    __mg_zorder_info->nr_globals = old_zorder_info->nr_globals;
    __mg_zorder_info->first_global = old_zorder_info->first_global;

    if (old_zorder_info->active_win < __mg_zorder_info->nr_globals) {
        __mg_zorder_info->active_win = old_zorder_info->active_win;
    }

    new_use_bmp = (unsigned char*)__mg_zorder_info + sizeof (ZORDERINFO);
    new_nodes = GET_ZORDERNODE(__mg_zorder_info);
    memcpy (new_use_bmp, old_use_bmp, LEN_USAGE_BMP_SPECIAL (old_zorder_info));
    memcpy (new_nodes, old_nodes,
            sizeof(ZORDERNODE) * MAX_NR_SPECIAL_ZNODES (old_zorder_info));

#ifndef _MGSCHEMA_COMPOSITING
    for (i = old_zorder_info->max_nr_tooltips;
            i <= MAX_NR_SPECIAL_ZNODES (old_zorder_info); i++) {
        new_nodes [i].age = old_nodes [i].age + 1;
    }
    new_nodes [0].age = old_nodes [0].age + 1;
#endif /* not defined _MGSCHEMA_COMPOSITING */

    unlock_zi_for_change (__mg_zorder_info);

    /* Since 5.0.0: check clients which created special znodes */
    {
        int slot;
        int cli_moving[3] = { 0 }, nr_clients = 0, i;
        MG_Layer* dst_layer = mgTopmostLayer;
        MSG msg = { HWND_DESKTOP,
                MSG_LAYERCHANGED, (WPARAM)dst_layer,
                (LPARAM)dst_layer->zorder_shmid, __mg_tick_counter };

        slot = old_zorder_info->first_tooltip;
        if (old_nodes[slot].cli > 0) {
            cli_moving[0] = old_nodes[slot].cli;
            nr_clients++;
        }

        slot = old_zorder_info->first_screenlock;
        if (old_nodes[slot].cli > 0) {
            for (i = 0; i < nr_clients; i++) {
                if (cli_moving[i] == old_nodes[slot].cli)
                    break;
            }

            if (i < nr_clients) {
                cli_moving[nr_clients] = old_nodes[slot].cli;
                nr_clients++;
            }
        }

        slot = old_zorder_info->first_docker;
        if (old_nodes[slot].cli > 0) {
            for (i = 0; i < nr_clients; i++) {
                if (cli_moving[i] == old_nodes[slot].cli)
                    break;
            }

            if (i < nr_clients) {
                cli_moving[nr_clients] = old_nodes[slot].cli;
                nr_clients++;
            }
        }

        slot = old_zorder_info->first_launcher;
        if (old_nodes[slot].cli > 0) {
            for (i = 0; i < nr_clients; i++) {
                if (cli_moving[i] == old_nodes[slot].cli)
                    break;
            }

            if (i < nr_clients) {
                cli_moving[nr_clients] = old_nodes[slot].cli;
                nr_clients++;
            }
        }

        for (i = 0; i < nr_clients; i++) {
            __mg_move_client_to_layer (mgClients + cli_moving[i],
                mgTopmostLayer);

            __mg_send2client (&msg, mgClients + cli_moving[i]);
        }
    }

    return 0;
}

static HWND dskGetActiveWindow (int* cli)
{
    int active_cli = -1;
    HWND active = HWND_NULL;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    lock_zi_for_read (__mg_zorder_info);
    if (__mg_zorder_info->active_win) {
        active_cli = nodes [__mg_zorder_info->active_win].cli;
        if (active_cli == __mg_client_id)
            active = nodes [__mg_zorder_info->active_win].hwnd;
        else
            active = HWND_OTHERPROC;

    }

    unlock_zi_for_read (__mg_zorder_info);

    if (cli) *cli = active_cli;

    return active;
}

/*
 * Set the active main window.
 */
static HWND dskSetActiveWindow (PMAINWIN pWin)
{
    int old_cli;
    HWND old;

    old = dskGetActiveWindow (&old_cli);

    if ((old_cli == __mg_client_id && pWin == (PMAINWIN)old) ||
                    (pWin && (pWin->dwExStyle & WS_EX_TOOLWINDOW)))
        return old;

    if (mgIsServer)
        old = (HWND) srvSetActiveWindow (0, pWin?pWin->idx_znode:0);
    else
        old = (HWND) cliSetActiveWindow (pWin);

    return old;
}

/*
 * This funciton add the new main window to the z-order list.
 * If new main window is a visible window,
 * this new main window becomes the active window.
 *
 * Return 0 if OK, else -1;
 */
static int dskAddNewMainWindow (PMAINWIN pWin, const COMPOSITINGINFO* ct_info)
{
    RECT rcWin;

    if (mgIsServer) {
        memcpy (&rcWin, &pWin->left, sizeof (RECT));
        pWin->idx_znode = srvAllocZOrderNode (0, (HWND)pWin,
                (HWND)pWin->pMainWin,
                get_znode_flags_from_style (pWin),
                &rcWin, pWin->spCaption, 0, 0, -1,
                ct_info->type, ct_info->arg);
    }
    else
        pWin->idx_znode = cliAllocZOrderNode (pWin, ct_info);

    if (pWin->idx_znode <= 0)
        return -1;

    /* Since 5.0.0: handle window style if failed to allocate znode for fixed ones */
    if (pWin->dwExStyle & WS_EX_WINTYPE_MASK) {
        ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

        pWin->dwExStyle &= ~WS_EX_WINTYPE_MASK;
        switch (nodes [pWin->idx_znode].flags & ZOF_TYPE_MASK) {
        case ZOF_TYPE_TOOLTIP:
            pWin->dwExStyle |= WS_EX_WINTYPE_TOOLTIP;
            break;
        case ZOF_TYPE_GLOBAL:
            pWin->dwExStyle |= WS_EX_WINTYPE_GLOBAL;
            break;
        case ZOF_TYPE_SCREENLOCK:
            pWin->dwExStyle |= WS_EX_WINTYPE_SCREENLOCK;
            break;
        case ZOF_TYPE_DOCKER:
            pWin->dwExStyle |= WS_EX_WINTYPE_DOCKER;
            break;
        case ZOF_TYPE_HIGHER:
            pWin->dwExStyle |= WS_EX_WINTYPE_HIGHER;
            pWin->dwExStyle |= WS_EX_TOPMOST;
            break;
        case ZOF_TYPE_NORMAL:
            pWin->dwExStyle |= WS_EX_WINTYPE_NORMAL;
            break;
        case ZOF_TYPE_LAUNCHER:
            pWin->dwExStyle |= WS_EX_WINTYPE_LAUNCHER;
            break;
        default:
            assert(0);
            break;
        }
    }

#if 0   /* move to window.c since 5.0.0 */
    /* Handle main window hosting. */
    if (pWin->pHosting)
        dskAddNewHostedMainWindow (pWin->pHosting, pWin);
#endif  /* moved code */

    /* Init Global Clip Region info. */
    dskInitGCRInfo (pWin);

    /* Init Invalid Region info. */
    dskInitInvRgn (pWin);

    /* houhh 20081128, create secondary window dc before
     * InvalidateRect, erase_bk will used this. */
    //pWin->dwExStyle = pWin->dwExStyle | WS_EX_AUTOSECONDARYDC;
    if (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC)
        pWin->secondaryDC = CreateSecondaryDC ((HWND)pWin);

    if (pWin->secondaryDC == HDC_INVALID) {
        /* remove the flag of WS_EX_AUTOSECONDARYDC */
        pWin->dwExStyle = pWin->dwExStyle | WS_EX_AUTOSECONDARYDC;
        pWin->secondaryDC = 0;
    }

    /* Create private client dc. */
    if (pWin->dwExStyle & WS_EX_USEPRIVATECDC) {
#if 0   /* deprecated code */
        if (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC)
            pWin->privCDC = GetSecondarySubDC (pWin->secondaryDC,
                    (HWND)pWin, TRUE);
        else
            pWin->privCDC = CreatePrivateClientDC ((HWND)pWin);
#else   /* deprecated code */
        /* since 5.0.0, always call CreatePrivateClientDC */
        pWin->privCDC = CreatePrivateClientDC ((HWND)pWin);
#endif
    }
    else
        pWin->privCDC = 0;

    /* show and active this main window. */
    if (pWin->dwStyle & WS_VISIBLE) {

        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);

        SendNotifyMessage ((HWND)pWin, MSG_SHOWWINDOW, SW_SHOWNORMAL, 0);

        InvalidateRect ((HWND)pWin, NULL, TRUE);

        dskSetActiveWindow (pWin);
    }

    return 0;
}

/*
 * This function removes a main window from z-order list.
 */
static void dskRemoveMainWindow (PMAINWIN pWin)
{
    if (mgIsServer) {
        srvFreeZOrderMaskRect (0, pWin->idx_znode);
        srvFreeZOrderNode (0, pWin->idx_znode);
    }
    else {
        cliFreeZOrderMaskRect (pWin);
        cliFreeZOrderNode (pWin);
    }

#if 0   /* move to window.c since 5.0.0 */
    /* Handle main window hosting. */
    if (pWin->pHosting)
        dskRemoveHostedMainWindow (pWin->pHosting, pWin);
#endif  /* moved code */

    if ((pWin->dwExStyle & WS_EX_AUTOSECONDARYDC) && pWin->secondaryDC) {
#if 0   /* deprecated code */
        DeleteSecondaryDC ((HWND)pWin);
#else   /* deprecated code */
        __mg_delete_secondary_dc (pWin);
#endif
        pWin->update_secdc = NULL;
    }

    if (pWin->privCDC) {
#if 0   /* deprecated code */
        if (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC) {
            ReleaseSecondarySubDC (pWin->privCDC);
        }
        else {
            if (pWin->secondaryDC)
                ReleaseSecondarySubDC (pWin->privCDC);
            else
                DeletePrivateDC (pWin->privCDC);
        }
#else   /* deprecated code */
        /* since 5.0.0, always call DeletePrivateDC */
        DeletePrivateDC (pWin->privCDC);
#endif
        pWin->privCDC = 0;
    }

    /* since 5.0.0, reset auto-repeat message if target window is the
       main window being removed or it is contained in this main window */
    if (checkAndGetMainWinIfWindow (sg_msgAutoRepeat.hwnd) == pWin) {
        sg_msgAutoRepeat.hwnd = 0;
    }

    if (checkAndGetMainWinIfWindow (sg_hCaretWnd) == pWin) {
        sg_hCaretWnd = 0;
    }
}

/* Note: no global control for special main windows */
static void dskHideGlobalControl (PMAINWIN pWin, int reason, LPARAM lParam)
{
    int first = 0;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    lock_zi_for_read (__mg_zorder_info);
    switch (nodes [pWin->idx_znode].flags & ZOF_TYPE_MASK) {
        case ZOF_TYPE_GLOBAL:
            first = __mg_zorder_info->first_global;
            break;
        case ZOF_TYPE_HIGHER:
            first = __mg_zorder_info->first_topmost;
            break;
        case ZOF_TYPE_NORMAL:
            first = __mg_zorder_info->first_normal;
            break;
        default:
            break;
    }
    unlock_zi_for_read (__mg_zorder_info);

    if (first > 0 && nodes [first].cli == __mg_client_id &&
            (nodes [first].flags & ZOF_TF_CONTROL) &&
            (nodes [first].flags & ZOF_VISIBLE)) {

        RECT rc = nodes [first].rc;
        PMAINWIN pCurTop = (PMAINWIN)nodes [first].hwnd;

        pCurTop->dwStyle &= ~WS_VISIBLE;
        cliHideWindow (pCurTop);
        dskSetPrimitiveChildren (pCurTop, FALSE);
        SendNotifyMessage (pCurTop->hParent,
                        MSG_CHILDHIDDEN, reason, lParam);

        dskScreenToClient (pCurTop->pMainWin, &rc, &rc);
        InvalidateRect ((HWND)pCurTop->pMainWin, &rc, TRUE);
    }
}

static void dskMoveToTopMost (PMAINWIN pWin, int reason, LPARAM lParam)
{
    if (!pWin) return;

    if (dskIsTopMost (pWin) && (pWin->dwStyle & WS_VISIBLE)) {
        return;
    }

    /* hide the possible visible global control */
    dskHideGlobalControl (pWin, reason, lParam);

    if (mgIsServer)
        srvMove2Top (0, pWin->idx_znode);
    else
        cliMove2Top (pWin);

    /* activate this main window. */
    if (!(pWin->dwStyle & WS_VISIBLE)) {
        pWin->dwStyle |= WS_VISIBLE;

        if (mgIsServer)
            srvShowWindow (0, pWin->idx_znode);
        else
            cliShowWindow (pWin);

        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);

        InvalidateRect ((HWND)pWin, NULL, TRUE);
    }
    else {
        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
    }

    if (reason != RCTM_SHOWCTRL)
        dskSetActiveWindow (pWin);
}

/*
 * Shows/Activates a main window.
 */
static void dskShowMainWindow (PMAINWIN pWin, BOOL bActive)
{
    if (pWin->dwStyle & WS_VISIBLE)
        return;

    pWin->dwStyle |= WS_VISIBLE;

    if (mgIsServer)
        srvShowWindow (0, pWin->idx_znode);
    else
        cliShowWindow (pWin);

    SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);

    InvalidateRect ((HWND)pWin, NULL, TRUE);

    if (bActive)
        dskSetActiveWindow (pWin);

    return;
}

/*
 * Hides a main window
 */
static void dskHideMainWindow (PMAINWIN pWin)
{
    if (!(pWin->dwStyle & WS_VISIBLE))
        return;

    pWin->dwStyle &= ~WS_VISIBLE;

    if (mgIsServer)
        srvHideWindow (0, pWin->idx_znode);
    else
        cliHideWindow (pWin);
}

#ifdef _MGSCHEMA_COMPOSITING
/* return values:
   >= 0: window surface resized
   -1: no need to resize surface
   -2: failed to resize surface
 */
static int resize_window_surface (PMAINWIN pWin, const RECT* prcResult)
{
    int nw = RECTWP(prcResult);
    int nh = RECTHP(prcResult);

    if (pWin->surf->w < nw || pWin->surf->h < nh) {
        GAL_Surface* new_surf;

        new_surf = GAL_CreateSurfaceForZNodeAs (pWin->surf, nw, nh);
        if (new_surf == NULL) {
            _WRN_PRINTF ("failed to resize surface nw (%d), nh (%d)\n", nw, nh);
            return -2;
        }

        GAL_SetClipRect (new_surf, NULL);
        GAL_SetColorKey (pWin->surf, 0, 0); // disable color key
        GAL_SetAlpha (pWin->surf, 0, 0);    // disable alpha
        GAL_BlitSurface (pWin->surf, NULL, new_surf, NULL);
        memcpy (new_surf->dirty_info, pWin->surf->dirty_info,
                sizeof (GAL_DirtyInfo));
        GAL_FreeSurface (pWin->surf);

        pWin->surf = new_surf;
        if (pWin->surf->shared_header)
            return pWin->surf->shared_header->fd;
        else
            return 0;
    }

    return -1;
}
#else   /* defined _MGSCHEMA_COMPOSITING */
static inline int resize_window_surface (PMAINWIN pWin, const RECT* prcExpect)
{
    return -1;
}
#endif  /* not defined _MGSCHEMA_COMPOSITING */

static int dskMoveGlobalControl (PMAINWIN pCtrl, RECT* prcExpect)
{
    RECT newWinRect, rcResult;
    int fd, ret;

    SendAsyncMessage ((HWND)pCtrl, MSG_CHANGESIZE,
                    (WPARAM)(prcExpect), (LPARAM)(&rcResult));
    dskClientToScreen ((PMAINWIN)(pCtrl->hParent), &rcResult, &newWinRect);

    if ((fd = resize_window_surface (pCtrl, &newWinRect)) == -2) {
        return -1;
    }

    if (mgIsServer)
        ret = srvMoveWindow (0, pCtrl->idx_znode, &newWinRect,
                (HWND)pCtrl, 0, 0, 0);
    else
        ret = cliMoveWindow (pCtrl, &newWinRect, fd);

#ifdef _MGSCHEMA_COMPOSITING
    if (fd >= 0) {
        __mg_update_dc_on_surface_changed ((HWND)pCtrl, pCtrl->surf);
    }
#endif

    if (ret == 0 && pCtrl->dwStyle & WS_VISIBLE) {
        SendAsyncMessage ((HWND)pCtrl, MSG_NCPAINT, 0, 0);
        InvalidateRect ((HWND)pCtrl, NULL, TRUE);
    }

#ifdef _MGSCHEMA_COMPOSITING
    if (pCtrl->surf->shared_header && pCtrl->surf->shared_header->fd >= 0) {
        close (pCtrl->surf->shared_header->fd);
        pCtrl->surf->shared_header->fd = -1;
    }
#endif

    return ret;
}

static int dskMoveMainWindow (PMAINWIN pWin, RECT* prcExpect)
{
    RECT rcResult;
    int fd, ret;

    SendAsyncMessage ((HWND)pWin, MSG_CHANGESIZE,
                    (WPARAM)(prcExpect), (LPARAM)(&rcResult));

    if ((fd = resize_window_surface (pWin, &rcResult)) == -2)
        return -1;

    if (mgIsServer)
        ret = srvMoveWindow (0, pWin->idx_znode, &rcResult,
                (HWND)pWin, 0, 0, 0);
    else
        ret = cliMoveWindow (pWin, &rcResult, fd);

#ifdef _MGSCHEMA_COMPOSITING
    if (fd >= 0) {
        __mg_update_dc_on_surface_changed ((HWND)pWin, pWin->surf);
    }

    if (pWin->surf->shared_header && pWin->surf->shared_header->fd >= 0) {
        close (pWin->surf->shared_header->fd);
        pWin->surf->shared_header->fd = -1;
    }
#endif

    return ret;
}

#ifdef _MGHAVE_MENU
/*
 * The callback procedure of tracking menu.
 * It is defined in Menu module.
 */
int PopupMenuTrackProc (PTRACKMENUINFO ptmi,
                int message, WPARAM wParam, LPARAM lParam);

static void dskForceCloseMenu (void)
{
    ZORDERINFO* zi = __mg_zorder_info;

    if (zi->cli_trackmenu < 0)
        return;

    if (mgIsServer)
        srvForceCloseMenu (0);
    else
        cliForceCloseMenu ();
}

static int dskStartTrackPopupMenu (PTRACKMENUINFO ptmi)
{
    PTRACKMENUINFO plast;

    if (__mg_zorder_info->cli_trackmenu != -1 &&
                    __mg_zorder_info->cli_trackmenu != __mg_client_id) {
        dskForceCloseMenu ();
    }

    if (sg_ptmi) {
        plast = sg_ptmi;
        while (plast->next) {
            plast = plast->next;
        }

        plast->next = ptmi;
        ptmi->prev = plast;
        ptmi->next = NULL;
    }
    else {
        sg_ptmi = ptmi;
        ptmi->next = NULL;
        ptmi->prev = NULL;
    }

    if (PopupMenuTrackProc (ptmi, MSG_INITMENU, 0, 0))
        return -1;

    if (mgIsServer)
        ptmi->idx_znode = srvStartTrackPopupMenu (0, &ptmi->rc, (HWND)ptmi,
                0, 0, -1);
    else
        ptmi->idx_znode = cliStartTrackPopupMenu (ptmi);

    if (ptmi->idx_znode < 0) {
        if (sg_ptmi == ptmi) {
            sg_ptmi = NULL;
        }
        else {
            plast = sg_ptmi;
            while (plast->next) {
                plast = plast->next;
            }
            plast->prev->next = NULL;
            plast = plast->prev;
        }

        return -1;
    }

    PopupMenuTrackProc (ptmi, MSG_SHOWMENU, 0, 0);
    return 0;
}

static int dskEndTrackPopupMenu (PTRACKMENUINFO ptmi)
{
    PTRACKMENUINFO plast = NULL;
    RECT rc;
#ifndef _MGSCHEMA_COMPOSITING
    RECT rcScr = GetScreenRect ();
#endif

    if (sg_ptmi == ptmi) {
        sg_ptmi = NULL;
    }
    else {
        plast = sg_ptmi;
        while (plast->next) {
            plast = plast->next;
        }
        plast->prev->next = NULL;
        plast = plast->prev;
    }

    if (__mg_zorder_info->cli_trackmenu == __mg_client_id) {
        if (mgIsServer)
            srvEndTrackPopupMenu (0, ptmi->idx_znode);
        else
            cliEndTrackPopupMenu (ptmi);
    }

    PopupMenuTrackProc (ptmi, MSG_HIDEMENU, 0, 0);

    PopupMenuTrackProc (ptmi, MSG_ENDTRACKMENU, 0, 0);

    rc = ptmi->rc;
    ptmi = sg_ptmi;
    while (ptmi) {
        if (DoesIntersect (&rc, &ptmi->rc)) {
#ifndef _MGSCHEMA_COMPOSITING
            SelectClipRect (HDC_SCREEN_SYS, &rc);
#endif
            PopupMenuTrackProc (ptmi, MSG_SHOWMENU, 0, 0);
        }
        ptmi = ptmi->next;
    }

#ifndef _MGSCHEMA_COMPOSITING
    SelectClipRect (HDC_SCREEN_SYS, &rcScr);
#endif

    return 0;
}
#endif /* defined _MGHAVE_MENU */

static void dskEnableWindow (PMAINWIN pWin, int flags)
{
    if (!mgIsServer)
        cliEnableWindow (pWin, flags);
    else
        srvEnableWindow (0, pWin?pWin->idx_znode:0, flags);

    if ( (!(pWin->dwStyle & WS_DISABLED) && !flags)
            || ((pWin->dwStyle & WS_DISABLED) && flags) ) {
        if (flags)
            pWin->dwStyle &= ~WS_DISABLED;
        else
            pWin->dwStyle |=  WS_DISABLED;

        if (pWin->dwStyle & WS_DISABLED) {
            if (__mg_captured_wnd &&
                checkAndGetMainWinIfWindow (__mg_captured_wnd) == pWin)
                __mg_captured_wnd = 0;

            if (dskGetActiveWindow (NULL) == (HWND)pWin) {
                dskSetActiveWindow (NULL);
                return;
            }
        }

        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
    }
}

static inline HWND dskGetCaptureWindow (void)
{
    return __mg_captured_wnd;
}

static inline HWND dskSetCaptureWindow (PMAINWIN pWin)
{
    HWND old;

    old = __mg_captured_wnd;
    __mg_captured_wnd = (HWND)pWin;

    return old;
}

static HWND dskGetNextMainWindow (PMAINWIN pWin)
{
    ZORDERINFO* zi = __mg_zorder_info;
    ZORDERNODE* nodes = GET_ZORDERNODE(zi);
    HWND hwnd = HWND_NULL;
    int from;

    if (pWin) {
        from = pWin->idx_znode;
    }
    else {
        from = 0;
    }
 
    lock_zi_for_read (zi);

    from = __kernel_get_next_znode (zi, from);
    while (from > 0) {
        hwnd = nodes[from].hwnd;

        if ((nodes[from].cli == __mg_client_id) &&
                (pWin = checkAndGetMainWinIfMainWin (hwnd)) &&
                !(pWin->dwExStyle & WS_EX_CTRLASMAINWIN)) {
            break;
        }

        from = __kernel_get_next_znode (zi, from);
    }

    unlock_zi_for_read (zi);
    return hwnd;
}

static int dskStartDragWindow (PMAINWIN pWin, const DRAGINFO* drag_info)
{
    int ret;

    if (!(pWin->dwStyle & WS_VISIBLE))
        return -1;

    if (mgIsServer)
        ret = srvStartDragWindow (0, pWin->idx_znode, drag_info->location,
                        drag_info->init_x, drag_info->init_y);
    else
        ret = cliStartDragWindow (pWin, drag_info);

    return ret;
}

static int dskCancelDragWindow (PMAINWIN pWin)
{
    int ret;

    if (!(pWin->dwStyle & WS_VISIBLE))
        return -1;

    if (mgIsServer)
        ret = srvCancelDragWindow (0, pWin->idx_znode);
    else
        ret = cliCancelDragWindow (pWin);

    return ret;
}

static int dskChangeCaption (PMAINWIN pWin)
{
    if (mgIsServer) {
        return srvChangeCaption (0, pWin->idx_znode, pWin->spCaption);
    }
    else
        return cliChangeCaption (pWin);
}

/* Since 5.0.0 */
static BOOL dskSetMainWinAlwaysTop (PMAINWIN pWin, BOOL fSet)
{
    if ((pWin->dwStyle & WS_ALWAYSTOP) && fSet)
        return TRUE;

    if (!(pWin->dwStyle & WS_ALWAYSTOP) && !fSet)
        return TRUE;

    if (mgIsServer) {
        if (srvSetZNodeAlwaysTop (0, pWin->idx_znode, fSet))
            return FALSE;
    }
    else {
        if (cliSetMainWinAlwaysTop (pWin, fSet))
            return FALSE;
    }

    if (fSet) {
        pWin->dwStyle |= WS_ALWAYSTOP;
    }
    else {
        pWin->dwStyle &= ~WS_ALWAYSTOP;
    }

    return TRUE;
}

/* Since 5.0.0 */
static int dskSetWindowMask (HWND pWin, const WINMASKINFO* mask_info)
{
    if (!mask_info->rcs || !mask_info->nr_rcs)
        return -1;

    if (mgIsServer) {
        /* XXX: free first */
        srvFreeZOrderMaskRect (0, ((PMAINWIN)pWin)->idx_znode);
        return srvAllocZOrderMaskRect (0,
                ((PMAINWIN)pWin)->idx_znode,
                get_znode_flags_from_style((PMAINWIN)pWin),
                mask_info->rcs, mask_info->nr_rcs);
    }
    else {
        cliFreeZOrderMaskRect ((PMAINWIN)pWin);
        return cliAllocZOrderMaskRect (pWin, mask_info->rcs, mask_info->nr_rcs);
    }
}

/* Since 5.0.0 */
BOOL __mg_move_client_to_layer (MG_Client* client, MG_Layer* dst_layer)
{
    ZORDERINFO *src_zi, *dst_zi;
    ZORDERNODE *src_nodes;
    int cli = client - mgClients;
    int next, from = 0;
    int nr_moving = 0, nr_free;

    src_zi = (ZORDERINFO*)client->layer->zorder_info;
    src_nodes = GET_ZORDERNODE(src_zi);

    /* number of general znodes of the client */
    do {
        if ((next = __kernel_get_next_znode (src_zi, from)) <= 0)
            break;

        if (src_nodes [next].cli == cli) {
            DWORD type = src_nodes [next].flags & ZOF_TYPE_MASK;
            if (IS_TYPE_GENERAL (type)) {   // do not count special znodes
                nr_moving++;
            }
        }

        from = next;
    } while (1);

    /* check free slots */
    dst_zi = (ZORDERINFO*)dst_layer->zorder_info;
    nr_free = MAX_NR_GENERAL_ZNODES (dst_zi);
    nr_free -= NR_GENERAL_ZNODES(dst_zi);
    if (nr_free < nr_moving) {
        _WRN_PRINTF ("Not enough free slots to move the client: "
            "nr_moving (%d), nr_free (%d)\n", nr_moving, nr_free);
        return FALSE;
    }

    /* nr znodes of the client */
    do {
        if ((next = __kernel_get_next_znode (src_zi, from)) <= 0)
            break;

        if (src_nodes [next].cli == cli) {
            AllocZOrderNodeEx (dst_zi, cli,
                            src_nodes [next].hwnd,
                            src_nodes [next].main_win,
                            src_nodes [next].flags,
                            &src_nodes [next].rc,
                            src_nodes [next].caption,
#ifdef _MGSCHEMA_COMPOSITING
                            src_nodes [next].mem_dc,
                            src_nodes [next].ct,
                            src_nodes [next].ct_arg
#else
                            HDC_INVALID, CT_OPAQUE, 0
#endif
                );
            FreeZOrderNodeEx (src_zi, next, NULL);
        }

        from = next;
    } while (1);

    /* Set layer of the client */
    client->layer = dst_layer;
    return TRUE;
}

#ifdef _MGSCHEMA_COMPOSITING
/* Since 5.0.0 */
static BOOL dskSetMainWinCompositing (PMAINWIN pWin, const COMPOSITINGINFO* info)
{
    if (mgIsServer) {
        if (srvSetZNodeCompositing (0, pWin->idx_znode, info->type, info->arg))
            return FALSE;
    }
    else if (cliSetMainWinCompositing (pWin, info)) {
        return FALSE;
    }

    return TRUE;
}
#endif /* defined _MGSCHEMA_COMPOSITING */

static LRESULT dskWindowMessageHandler (UINT message,
        PMAINWIN pWin, LPARAM lParam)
{
    switch (message) {
    case MSG_ADDNEWMAINWIN:
        return dskAddNewMainWindow (pWin, (const COMPOSITINGINFO *)lParam);

    case MSG_REMOVEMAINWIN:
        dskRemoveMainWindow (pWin);
        break;

    case MSG_MOVETOTOPMOST:
        dskMoveToTopMost (pWin, RCTM_MESSAGE, 0);
        break;

    case MSG_SHOWMAINWIN:
        dskShowMainWindow (pWin, TRUE);
        break;

    case MSG_HIDEMAINWIN:
        dskHideMainWindow (pWin);
        break;

    case MSG_MOVEMAINWIN:
        if (pWin->WinType == TYPE_CONTROL)
            dskMoveGlobalControl (pWin, (RECT*)lParam);
        else
            dskMoveMainWindow (pWin, (RECT*)lParam);
        break;

    case MSG_GETACTIVEMAIN:
        return (LRESULT)dskGetActiveWindow (NULL);

    case MSG_SETACTIVEMAIN:
        return (LRESULT)dskSetActiveWindow (pWin);

    case MSG_GETCAPTURE:
        return (LRESULT)dskGetCaptureWindow ();

    case MSG_SETCAPTURE:
        return (LRESULT)dskSetCaptureWindow (pWin);

#ifdef _MGHAVE_MENU
    case MSG_TRACKPOPUPMENU:
        return dskStartTrackPopupMenu ((PTRACKMENUINFO)lParam);

    case MSG_ENDTRACKMENU:
        return dskEndTrackPopupMenu ((PTRACKMENUINFO)lParam);

    case MSG_CLOSEMENU:
        dskCloseMenu ();
        break;
#endif

    case MSG_SCROLLMAINWIN:
        dskScrollMainWindow (pWin, (PSCROLLWINDOWINFO)lParam);
        break;

    case MSG_CARET_CREATE:
        sg_hCaretWnd = (HWND)pWin;
        sg_uCaretBTime = pWin->pCaretInfo->uTime;
        return 0;

    case MSG_CARET_DESTROY:
        sg_hCaretWnd = 0;
        return 0;

    case MSG_ENABLEMAINWIN:
        dskEnableWindow (pWin, lParam);
        break;

    case MSG_ISENABLED:
        return !(pWin->dwStyle & WS_DISABLED);

    case MSG_SETWINCURSOR: {
        HCURSOR old = pWin->hCursor;

        pWin->hCursor = (HCURSOR)lParam;
        return (LRESULT)old;
    }

    case MSG_GETNEXTMAINWIN:
        return (LRESULT)dskGetNextMainWindow (pWin);

    case MSG_SHOWGLOBALCTRL: {
        dskMoveGlobalControl (pWin, (RECT*)&(pWin->left));
        dskMoveToTopMost (pWin, RCTM_SHOWCTRL, 0);
        dskSetPrimitiveChildren (pWin, TRUE);
        break;
    }

    case MSG_HIDEGLOBALCTRL:
        dskHideMainWindow (pWin);
        dskSetPrimitiveChildren (pWin, FALSE);
        break;

    case MSG_STARTDRAGWIN:
        return dskStartDragWindow (pWin, (DRAGINFO*)lParam);

    case MSG_CANCELDRAGWIN:
        return dskCancelDragWindow (pWin);

    case MSG_CHANGECAPTION:
        return dskChangeCaption (pWin);

    /* Since 5.0.0 */
    case MSG_SETALWAYSTOP:
        return dskSetMainWinAlwaysTop (pWin, (BOOL)lParam);

    /* Since 5.0.0 */
    case MSG_SETWINDOWMASK:
        return dskSetWindowMask (pWin, (WINMASKINFO*)lParam);

#ifdef _MGSCHEMA_COMPOSITING
    /* Since 5.0.0 */
    case MSG_SETCOMPOSITING:
        return dskSetMainWinCompositing (pWin, (const COMPOSITINGINFO*)lParam);
#endif

    /* Since 5.0.0 */
    case MSG_DUMPZORDER:
         dskDumpZOrder (__mg_zorder_info);
         break;

    /* Since 5.0.0 */
    case MSG_SETAUTOREPEAT:
        sg_msgAutoRepeat = *(const MSG*)lParam;
        break;
   }

   return 0;
}

/*********************** Hook support ****************************************/
int __mg_do_reghook_operation (int cli, const REGHOOKINFO* info)
{
    int retval = -1;

    switch (info->id_op) {
    case HOOKOP_ID_REGISTER:
        retval = dskRegisterHookWin (cli, info->hwnd, info->flags);
        break;

    case HOOKOP_ID_UNREGISTER:
        retval = dskUnregisterHookWin (cli, info->hwnd);
        break;

    default:
        break;
    }

    return retval;
}

static int cliRegisterHookWin (HWND hwnd, DWORD flags)
{
    int retval;
    REGHOOKINFO info;
    REQUEST req;

    assert (!mgIsServer);

    info.id_op = HOOKOP_ID_REGISTER;
    info.hwnd = hwnd;
    info.flags = flags;

    req.id = REQID_REGISTERHOOK;
    req.data = &info;
    req.len_data = sizeof (REGHOOKINFO);
    if (ClientRequest (&req, &retval, sizeof (int)) < 0)
        return -1;

    return retval;
}

static int cliUnregisterHookWin (HWND hwnd)
{
    int retval = 0;
    REGHOOKINFO info;
    REQUEST req;

    assert (!mgIsServer);

    info.id_op = HOOKOP_ID_UNREGISTER;
    info.hwnd = hwnd;
    info.flags = 0;

    req.id = REQID_REGISTERHOOK;
    req.data = &info;
    req.len_data = sizeof (REGHOOKINFO);
    if (ClientRequest (&req, &retval, sizeof (int)) < 0)
        return -1;

    return retval;
}

static int cliCalculateDefaultPosition (CALCPOSINFO* info)
{
    RECT retrc;
    REQUEST req;

    assert (!mgIsServer);

    req.id = REQID_CALCPOSITION;
    req.data = info;
    req.len_data = sizeof (CALCPOSINFO);
    if (ClientRequest (&req, &retrc, sizeof (RECT)) < 0)
        return -1;

    info->rc = retrc;
    return 0;
}

#if 0   /* Since 5.0.0, deprecated */
static HOOKINFO keyhook = {0, HWND_NULL, 0};
static HOOKINFO mousehook = {0, HWND_NULL, 0};

static int srvHandleKeyHook (int message, WPARAM wParam, LPARAM lParam)
{
    MSG msg;

    if (keyhook.cli <= 0 || keyhook.hwnd == HWND_NULL)
        return HOOK_GOON;

    msg.hwnd = keyhook.hwnd;
    msg.message = message;
    msg.wParam = wParam;
    msg.lParam = lParam;

    Send2Client (&msg, keyhook.cli);
    return keyhook.flag;
}

int __mg_handle_mouse_hook (int message, WPARAM wParam, LPARAM lParam)
{
    MSG msg;
    if (mousehook.cli <= 0 || mousehook.hwnd == HWND_NULL)
        return HOOK_GOON;

    msg.hwnd = mousehook.hwnd;
    msg.message = message;
    msg.wParam = wParam;
    msg.lParam = lParam;

    Send2Client (&msg, mousehook.cli);

    return mousehook.flag;
}
#endif  /* deprecated code */

/* used by client to check the validation of a hwnd */
BOOL __mg_client_check_hwnd (HWND hwnd, int cli)
{
    ZORDERINFO* zi = __mg_zorder_info;
    ZORDERNODE* nodes;
    int level, slot;

    nodes = GET_ZORDERNODE(zi);

    lock_zi_for_read (zi);

#if 0   /* deprecated code */
    /* Since 5.0.0 */
    static int fixed_slots [] = { ZNIDX_SCREENLOCK, ZNIDX_DOCKER,
        ZNIDX_LAUNCHER };
    for (slot = 0; slot < TABLESIZE(fixed_slots); slot++) {
        if (nodes[fixed_slots[slot]].cli == cli &&
                nodes[fixed_slots[slot]].hwnd == hwnd)
            goto ret_true;
    }

    slot = zi->first_normal;
    for (; slot > 0; slot = nodes [slot].next) {
        if (cli == nodes [slot].cli && hwnd == nodes [slot].hwnd) {
            goto ret_true;
        }
    }
#endif  /* deprecated code */

    /* ignore global ones for client */

    for (level = 0; level < NR_ZORDER_LEVELS; level++) {
        if (level == ZLIDX_GLOBAL)
            continue;

        slot = zi->first_in_levels[level];
        for (; slot > 0; slot = nodes [slot].next) {
            if (cli == nodes [slot].cli && hwnd == nodes [slot].hwnd) {
                goto ret_true;
            }
        }
    }

    unlock_zi_for_read (zi);
    return FALSE;

ret_true:
    unlock_zi_for_read (zi);
    return TRUE;
}

/*
 * Key message handling.
 *
 * handle_special_key
 * srvKeyMessageHandler
 */
static int handle_special_key (int scancode)
{
    switch (scancode) {
    case SCANCODE_BACKSPACE:
        ExitGUISafely (-1);
        return 0;
    }

    return 0;
}

static int srvKeyMessageHandler (int message, int scancode, DWORD status)
{
    int next_node;
    static int mg_altdown = 0;
    static int mg_modal = 0;

    if ((message == MSG_KEYDOWN) && (status & KS_ALT) && (status & KS_CTRL))
        return handle_special_key (scancode);

    if (scancode == SCANCODE_LEFTALT || scancode == SCANCODE_RIGHTALT) {
        if (message == MSG_KEYDOWN) {
            mg_altdown = 1;
            return 0;
        }
        else {
            mg_altdown = 0;
            if (mg_modal == 1) {
                mg_modal = 0;
                return 0;
            }
        }
    }

    if (mg_altdown) {
        if (message == MSG_KEYDOWN) {
            if (scancode == SCANCODE_TAB) {
                mg_modal = 1;
#ifdef _MGHAVE_MENU
                srvForceCloseMenu (0);
#endif
                next_node = get_next_activable_mainwin (__mg_zorder_info,
                                    __mg_zorder_info->active_win);
                if (next_node) {
                    srvMove2Top (0, next_node);
                    srvSetActiveWindow (0, next_node);
                }
                return 0;
            }
            else if (scancode == SCANCODE_ESCAPE) {

                mg_modal = 1;

                if (__mg_zorder_info->active_win) {
                    __mg_post_msg_by_znode (__mg_zorder_info,
                                    __mg_zorder_info->active_win,
                                    MSG_CLOSE, 0, 0);
                    return 0;
                }
            }
        }
        else if (mg_modal == 1)
            return 0;
    }

    if (scancode == SCANCODE_LEFTALT
             || scancode == SCANCODE_RIGHTALT || mg_altdown) {
        if (message == MSG_KEYDOWN)
            message = MSG_SYSKEYDOWN;
        else {
            message = MSG_SYSKEYUP;
            mg_altdown = 0;
        }
    }

#if 0   /* Since 5.0.0, deprecated */
    if (srvHandleKeyHook (message,
                            (WPARAM)scancode, (LPARAM)status) == HOOK_STOP)
        return 0;
#endif  /* Since 5.0.0, do not handle event hook here */

    if (__mg_ime_wnd) {
        PostMessage (__mg_ime_wnd,
                        message, (WPARAM)scancode, (LPARAM)status);
        return 0;
    }

    if (__mg_zorder_info->active_win) {
        __mg_post_msg_by_znode (__mg_zorder_info,
                        __mg_zorder_info->active_win, message,
                        (WPARAM)scancode, (LPARAM)status);
    }
    else {
        SendMessage (HWND_DESKTOP, MSG_DT_KEYOFF + message,
                        (WPARAM)scancode, (LPARAM)status);
    }

    return 0;
}

int __mg_get_znode_at_point (const ZORDERINFO* zi, int x, int y, HWND* hwnd)
{
    ZORDERNODE* nodes = GET_ZORDERNODE(zi);
    int slot;

    if (zi->cli_trackmenu >= 0)
        return -1;

    slot = get_znode_at_point (zi, nodes, x, y);
    if (slot == 0)
        return -1;

    if (hwnd) *hwnd = nodes [slot].hwnd;
    return nodes [slot].cli;
}

/* since 5.0.0: make static */
static int handle_normal_mouse_move (const ZORDERINFO* zi, int x, int y)
{
    ZORDERNODE* nodes = GET_ZORDERNODE(zi);

    static int old_slot, old_cli;
    static HWND old_hwnd;
    int cur_slot;
    int cur_cli = 0;

    if (zi->cli_trackmenu >= 0)
        return zi->cli_trackmenu;

    cur_slot = get_znode_at_point (zi, nodes, x, y);

    if (old_slot != cur_slot) {
        if (old_slot > 0 && nodes [old_slot].cli == old_cli &&
                    nodes [old_slot].hwnd == old_hwnd) {
            post_msg_by_znode_p (zi, nodes + old_slot,
                        MSG_MOUSEMOVEIN, FALSE, 0);

            post_msg_by_znode_p (zi, nodes + old_slot,
                        MSG_NCMOUSEMOVE, HT_OUT, MAKELONG (x, y));
        }

        if (cur_slot > 0) {
            HWND hwnd;

            if (nodes [cur_slot].flags & ZOF_DISABLED) {
                HCURSOR def_cursor = GetDefaultCursor ();
                if (def_cursor)
                    SetCursor (def_cursor);
            }
            else {
                cur_cli = nodes [cur_slot].cli;
            }

            if (nodes [cur_slot].cli == old_cli)
                hwnd = old_hwnd;
            else
                hwnd = HWND_OTHERPROC;

            post_msg_by_znode_p (zi, nodes + cur_slot,
                            MSG_MOUSEMOVEIN, TRUE, (LPARAM)hwnd);
        }
        else
            SetCursor (GetSystemCursor (IDC_ARROW));
    }
    else if (cur_slot > 0) {
        if (nodes [cur_slot].flags & ZOF_DISABLED) {
            HCURSOR def_cursor = GetDefaultCursor ();
            if (def_cursor)
                SetCursor (def_cursor);
        }
        else {
            cur_cli = nodes [cur_slot].cli;
        }
    }

    old_slot = cur_slot;
    if (cur_slot > 0) {
        old_cli = nodes [cur_slot].cli;
        old_hwnd = nodes [cur_slot].hwnd;
    }
    else {
        old_cli = -1;
        old_hwnd = 0;
    }

    return cur_cli;
}

PMAINWIN gui_GetMainWindowPtrUnderPoint (int x, int y)
{
    HWND hwnd;

    if (__mg_get_znode_at_point (__mg_zorder_info, x, y, &hwnd)
                    == __mg_client_id) {
        return (PMAINWIN)hwnd;
    }

    return NULL;
}

/* defined in ../gui/window.c */
extern void __mg_reset_mainwin_capture_info (PCONTROL ctrl);

static PMAINWIN mgs_captured_main_win = (void*)HWND_INVALID;
static int mgs_captured_by;

void __mg_reset_desktop_capture_info (PMAINWIN pWin)
{
    if (pWin == mgs_captured_main_win) {
        mgs_captured_main_win = (void*)HWND_INVALID;
        mgs_captured_by = 0;
    }

    if ((HWND)pWin == __mg_captured_wnd)
        __mg_captured_wnd = 0;

    __mg_reset_mainwin_capture_info ((PCONTROL)pWin);
}

static int check_capture (int message)
{
    if (mgs_captured_main_win != (void*)HWND_INVALID
                    && mgs_captured_main_win != NULL) {

        switch (message) {
        case MSG_LBUTTONDOWN:
        case MSG_RBUTTONDOWN:
        case MSG_MBUTTONDOWN:
            if (mgs_captured_by != message)
                return 1;
            break;

        case MSG_LBUTTONUP:
            if (mgs_captured_by != MSG_LBUTTONDOWN)
                return 1;
            break;

        case MSG_RBUTTONUP:
            if (mgs_captured_by != MSG_RBUTTONDOWN)
                return 1;
            break;

        case MSG_MBUTTONUP:
            if (mgs_captured_by != MSG_MBUTTONDOWN)
                return 1;
            break;
        }
    }

    return 0;
}

/* Since 5.0.0; return non zero to STOP handling the message */
static int srvPreMouseMessageHandler (UINT message, WPARAM flags, int x, int y)
{
    static int down_client = -1;
    static int down_by;
    int target_client;
    int cur_client;
    int retval;
    WPARAM wparam = flags;
    LPARAM lparam = MAKELONG (x, y);
    MSG Msg = { HWND_DESKTOP, message, wparam, lparam };

    if (dskPreMouseMessageHandler (message, wparam, lparam))
        return HOOK_STOP;

    if (do_drag_drop_window (Msg.message, x, y)) {
        down_client = -1;
        return HOOK_STOP;
    }

    cur_client = __mg_get_znode_at_point (__mg_zorder_info, x, y, NULL);
    switch (Msg.message) {
    case MSG_LBUTTONDOWN:
    case MSG_RBUTTONDOWN:
    case MSG_MBUTTONDOWN:
        if (cur_client >= 0 && down_client == -1) {
            down_client = cur_client;
            down_by = Msg.message;
        }
        break;
    }

    if (cur_client == -1)
        SetDefaultCursor (GetSystemCursor (IDC_ARROW));

    target_client = cur_client;
    if (down_client < 0) {
        if (!__mg_captured_wnd && message == MSG_MOUSEMOVE) {
            int cli = handle_normal_mouse_move (__mg_zorder_info, x, y);

            if (cli > 0)
                target_client = cli;
        }
    }
    else
        target_client = down_client;

    retval = HOOK_GOON;
    if (target_client > 0) {
        Msg.hwnd = HWND_NULL;
        if (Msg.message == MSG_MOUSEMOVE) {
            LOCK_MOUSEMOVE_SEM();
            SHAREDRES_MOUSEMOVECLIENT = target_client;
            SHAREDRES_MOUSEMOVESERIAL++;
            UNLOCK_MOUSEMOVE_SEM();
        }
        else {
            Send2Client (&Msg, target_client);
        }

        retval = HOOK_STOP;
    }

    if (Msg.message == MSG_LBUTTONUP && down_by == MSG_LBUTTONDOWN) {
        down_client = -1;
    }
    else if (Msg.message == MSG_RBUTTONUP && down_by == MSG_RBUTTONDOWN) {
        down_client = -1;
    }
    else if (Msg.message == MSG_MBUTTONUP && down_by == MSG_MBUTTONDOWN) {
        down_client = -1;
    }

    return retval;
}

static int dskMouseMessageHandler (int message, WPARAM flags, int x, int y)
{
    PMAINWIN pUnderPointer;
    PMAINWIN pCtrlPtrIn;
    int CapHitCode = HT_UNKNOWN;
    int UndHitCode = HT_UNKNOWN;
    int cx = 0, cy = 0;

    if (__mg_captured_wnd) {
        PostMessage (__mg_captured_wnd,
            message, flags | KS_CAPTURED, MAKELONG (x, y));
        return 0;
    }

    if (mgs_captured_main_win != (void*)HWND_INVALID
                    && mgs_captured_main_win != NULL) {
        CapHitCode = SendAsyncMessage((HWND)mgs_captured_main_win,
                        MSG_HITTEST, (WPARAM)x, (LPARAM)y);
    }

    pCtrlPtrIn = gui_GetMainWindowPtrUnderPoint (x, y);

    if (pCtrlPtrIn && pCtrlPtrIn->WinType == TYPE_CONTROL) {
        pUnderPointer = pCtrlPtrIn->pMainWin;
        UndHitCode = HT_CLIENT;
        cx = x - pUnderPointer->cl;
        cy = y - pUnderPointer->ct;
    }
    else {
        pUnderPointer = pCtrlPtrIn;
        pCtrlPtrIn = NULL;

        if (pUnderPointer) {
            UndHitCode = SendAsyncMessage((HWND)pUnderPointer, MSG_HITTEST,
                                        (WPARAM)x, (LPARAM)y);
            cx = x - pUnderPointer->cl;
            cy = y - pUnderPointer->ct;
        }
    }

    switch (message) {
    case MSG_MOUSEMOVE:
        if (mgs_captured_main_win != (void *)HWND_INVALID) {
            if (mgs_captured_main_win)
                PostMessage((HWND)mgs_captured_main_win, MSG_NCMOUSEMOVE,
                            CapHitCode, MAKELONG (x, y));
            else
                PostMessage(HWND_DESKTOP, MSG_DT_MOUSEMOVE,
                            pUnderPointer == NULL, MAKELONG (x, y));
            break;
        }

        if (pUnderPointer) {
            HCURSOR def_cursor = GetDefaultCursor ();
            if (UndHitCode == HT_CLIENT) {
                if (def_cursor)
                    SetCursor (def_cursor);
                PostMessage ((HWND)pUnderPointer, MSG_SETCURSOR,
                        UndHitCode, MAKELONG (cx, cy));
                PostMessage((HWND)pUnderPointer, MSG_NCMOUSEMOVE,
                        UndHitCode, MAKELONG (x, y));
                PostMessage((HWND)pUnderPointer, MSG_MOUSEMOVE,
                        flags, MAKELONG (cx, cy));
            }
            else {
                if (def_cursor)
                    SetCursor (def_cursor);
                PostMessage ((HWND)pUnderPointer, MSG_NCSETCURSOR,
                        UndHitCode, MAKELONG (x, y));
                PostMessage((HWND)pUnderPointer, MSG_NCMOUSEMOVE,
                        UndHitCode, MAKELONG (x, y));
            }
        }
        break;

    case MSG_LBUTTONDOWN:
    case MSG_RBUTTONDOWN:
    case MSG_MBUTTONDOWN:
        if (check_capture (message)) /* ignore the event */
            break;

        if (pUnderPointer) {
#ifdef _MGHAVE_MENU
            dskForceCloseMenu ();
#endif

            if (pUnderPointer->dwStyle & WS_DISABLED) {
                Ping ();
                break;
            }

            if (pCtrlPtrIn == NULL) {
                if (!dskIsTopMost (pUnderPointer)) {
                    dskMoveToTopMost (pUnderPointer,
                                    RCTM_CLICK, MAKELONG (x, y));
                }

                if (pUnderPointer !=
                        (PMAINWIN)dskSetActiveWindow (pUnderPointer))
                    PostMessage ((HWND) pUnderPointer,
                            MSG_MOUSEACTIVE, UndHitCode, 0);
            }

            if (UndHitCode != HT_CLIENT) {
                if (UndHitCode & HT_NEEDCAPTURE) {
                    mgs_captured_main_win = pUnderPointer;
                    mgs_captured_by = message;
                }
                else
                    mgs_captured_main_win = (void*)HWND_INVALID;

                PostMessage ((HWND)pUnderPointer, message + MSG_NCMOUSEOFF,
                        UndHitCode, MAKELONG (x, y));
            }
            else {
                PostMessage ((HWND)pUnderPointer, message,
                    flags, MAKELONG(cx, cy));
                mgs_captured_main_win = (void*)HWND_INVALID;
            }
        }
        else {
            dskSetActiveWindow (NULL);
            mgs_captured_main_win = NULL;
            PostMessage (HWND_DESKTOP, message + MSG_DT_MOUSEOFF,
                        flags, MAKELONG (x, y));
        }
        break;

    case MSG_LBUTTONUP:
    case MSG_RBUTTONUP:
    case MSG_MBUTTONUP:
        if (check_capture (message)) /* ignore the event */
            break;

        if (mgs_captured_main_win != (void*)HWND_INVALID) {
            if (mgs_captured_main_win)
                PostMessage ((HWND)mgs_captured_main_win,
                    message + MSG_NCMOUSEOFF,
                    CapHitCode, MAKELONG (x, y));
            else if (!pUnderPointer)
                PostMessage (HWND_DESKTOP, message + MSG_DT_MOUSEOFF,
                    flags, MAKELONG (x, y));

           // mgs_captured_main_win = NULL;
            mgs_captured_main_win = (void*)HWND_INVALID;
            break;
        }
        else {
            if (pUnderPointer) {
                if (pUnderPointer->dwStyle & WS_DISABLED) {
                    break;
                }

                if (UndHitCode == HT_CLIENT) {
                    PostMessage((HWND)pUnderPointer, message,
                        flags, MAKELONG (cx, cy));
                }
                else {
                    PostMessage((HWND)pUnderPointer,
                        message + MSG_NCMOUSEOFF,
                        UndHitCode, MAKELONG (x, y));
                }
            }
            else
                PostMessage (HWND_DESKTOP, message + MSG_DT_MOUSEOFF,
                    flags, MAKELONG (x, y));
        }
        break;

    case MSG_LBUTTONDBLCLK:
    case MSG_RBUTTONDBLCLK:
    case MSG_MBUTTONDBLCLK:
        if (pUnderPointer) {
            if (pUnderPointer->dwStyle & WS_DISABLED) {
                Ping ();
                break;
            }

            if (UndHitCode == HT_CLIENT)
                PostMessage((HWND)pUnderPointer, message,
                    flags, MAKELONG(cx, cy));
            else
                PostMessage((HWND)pUnderPointer, message + MSG_NCMOUSEOFF,
                    UndHitCode, MAKELONG (x, y));
        }
        else {
            PostMessage(HWND_DESKTOP, message + MSG_DT_MOUSEOFF,
                    flags, MAKELONG (x, y));
        }

        break;
    }

    return 0;
}

/***********************************************************
 * Common message handling for the server and all clients.
 **********************************************************/

static int dskOnNewCtrlInstance (PCONTROL pParent, PCONTROL pNewCtrl)
{
    PCONTROL pFirstCtrl, pLastCtrl;

    if (pNewCtrl->dwExStyle & WS_EX_CTRLASMAINWIN) {
        RECT rcWin;

        dskGetWindowRectInScreen ((PMAINWIN)pNewCtrl, &rcWin);

        /* Add to z-order. */
        if (mgIsServer)
            pNewCtrl->idx_znode = srvAllocZOrderNode (0,
                            (HWND)pNewCtrl, (HWND)pNewCtrl->pMainWin,
                            get_znode_flags_from_style ((PMAINWIN)pNewCtrl),
                            &rcWin, pNewCtrl->spCaption, 0, 0, -1, CT_OPAQUE, 0);
        else
            pNewCtrl->idx_znode = cliAllocZOrderNode ((PMAINWIN)pNewCtrl, NULL);

        if (pNewCtrl->idx_znode <= 0)
            return -1;

        /* Init Global Clip Region info. */
        dskInitGCRInfo ((PMAINWIN)pNewCtrl);
    }

    pFirstCtrl = pParent->children;

    pNewCtrl->next = NULL;

    if (!pFirstCtrl) {
        pParent->children = pNewCtrl;
        pNewCtrl->prev = NULL;
    }
    else {
        pLastCtrl = pFirstCtrl;

        while (pLastCtrl->next)
            pLastCtrl = pLastCtrl->next;

        pLastCtrl->next = pNewCtrl;
        pNewCtrl->prev = pLastCtrl;
    }

    dskInitInvRgn ((PMAINWIN)pNewCtrl);

    pNewCtrl->pcci->nUseCount ++;

    return 0;
}

static int dskOnRemoveCtrlInstance (PCONTROL pParent, PCONTROL pCtrl)
{
    PCONTROL pFirstCtrl;
    BOOL fFound = FALSE;

    pFirstCtrl = pParent->children;

    if (!pFirstCtrl)
        return -1;
    else {
        if (pFirstCtrl == pCtrl) {
            pParent->children = pCtrl->next;
            if (pCtrl->next)
                pCtrl->next->prev = NULL;
            fFound = TRUE;
        }
        else {
            while (pFirstCtrl->next) {
                if (pFirstCtrl->next == pCtrl) {
                    pFirstCtrl->next = pCtrl->next;
                    if (pFirstCtrl->next)
                        pFirstCtrl->next->prev = pCtrl->prev;
                    fFound = TRUE;
                    break;
                }

                pFirstCtrl = pFirstCtrl->next;
            }
        }
    }

    /* remove from z-order */
    if (pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN) {
        if (mgIsServer)
            srvFreeZOrderNode (0, pCtrl->idx_znode);
        else
            cliFreeZOrderNode ((PMAINWIN)pCtrl);
    }

    if (fFound) {
        pCtrl->pcci->nUseCount --;
        return 0;
    }

    if ((HWND)pCtrl == __mg_captured_wnd) {
        /* force release the capture */
        __mg_captured_wnd = 0;
    }

    /* Since 5.0.0 */
    if ((HWND)pCtrl == sg_hCaretWnd) {
        sg_hCaretWnd = 0;
    }

    if ((HWND)pCtrl == sg_msgAutoRepeat.hwnd) {
        sg_msgAutoRepeat.hwnd = 0;
    }

    return -1;
}

/***********************************************************
 * Session message handling for the server.
 **********************************************************/

#define IDM_REDRAWBG    MINID_RESERVED
#define IDM_CLOSEALLWIN (MINID_RESERVED + 1)
#define IDM_ENDSESSION  (MINID_RESERVED + 2)

#define IDM_FIRSTWINDOW (MINID_RESERVED + 101)

#define IDM_SWITCH_LAYER (MINID_RESERVED + 201)

#define IDM_DELETE_LAYER (MINID_RESERVED + 301)

#ifdef _MGHAVE_MENU
static HMENU srvCreateWindowSubMenu (void)
{
    HMENU hmnu;
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_WINDOW);

    hmnu = CreatePopupMenu (&mii);
    return hmnu;
}

static HMENU srvCreateLayerSubMenu (BOOL flag)
{
    HMENU hmnu;
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    =
        flag ? (DWORD)GetSysText(IDS_MGST_SWITCHLAYER)
             : (DWORD)GetSysText(IDS_MGST_DELLAYER);

    hmnu = CreatePopupMenu (&mii);
    return hmnu;
}

static HMENU srvCreateDesktopMenu (void)
{
    HMENU hmnu;
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_START);

    hmnu = CreatePopupMenu (&mii);

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = IDM_REDRAWBG;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_REFRESH);
    InsertMenuItem (hmnu, 0, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_CLOSEALLWIN;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_CLOSEALLWIN);
    InsertMenuItem (hmnu, 1, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_ENDSESSION;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_ENDSESSION);
    InsertMenuItem (hmnu, 2, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_WINDOW);
    mii.hsubmenu     = srvCreateWindowSubMenu();
    InsertMenuItem (hmnu, 3, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_SWITCHLAYER);
    mii.hsubmenu     = srvCreateLayerSubMenu(TRUE);
    InsertMenuItem (hmnu, 4, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_DELLAYER);
    mii.hsubmenu     = srvCreateLayerSubMenu(FALSE);
    InsertMenuItem (hmnu, 5, TRUE, &mii);

    mii.type        = MFT_SEPARATOR;
    mii.id          = 0;
    mii.typedata    = 0;
    mii.hsubmenu    = 0;
    InsertMenuItem (hmnu, 6, TRUE, &mii);

    return hmnu;
}

typedef struct _UPDATA_DSKMENU_INFO
{
    MENUITEMINFO mii;
    HMENU menu;
    int id;
    int pos;
} UPDATA_DSKMENU_INFO;

static BOOL _cb_update_dskmenu (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    UPDATA_DSKMENU_INFO* info = (UPDATA_DSKMENU_INFO*) context;

    if (node->flags & ZOF_TF_MAINWIN) {
        if (node->flags & ZOF_VISIBLE && !(node->flags & ZOF_DISABLED))
            info->mii.state       = MFS_ENABLED;
        else
            info->mii.state       = MFS_DISABLED;
    }
    else
        return FALSE;

    info->mii.id              = info->id;
    info->mii.typedata        = (DWORD)(node->caption);
    info->mii.itemdata        = (DWORD)node;
    InsertMenuItem (info->menu, info->pos, TRUE, &info->mii);

    info->id++;
    info->pos++;
    return TRUE;
}
#endif /* defined _MGHAVE_MENU */

static BOOL _cb_close_mainwin (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    if (node->flags & ZOF_TF_MAINWIN) {
        post_msg_by_znode_p (zi, node, MSG_CLOSE, 0, 0);
    }
    return TRUE;
}

/* this function can be called only by the server */
static int nr_of_all_znodes (void)
{
    MG_Layer* layer = mgLayers;
    int count = 0;

    while (layer) {
        ZORDERINFO* zi;
        int level;

        zi = layer->zorder_info;
        for (level = 0; level < NR_ZORDER_LEVELS; level++) {
            count += zi->nr_nodes_in_levels[level];
        }

        layer = layer->next;
    }

    return count;
}

#define SERVER_HAS_NO_MAINWINDOW() (nr_of_all_znodes() == 0)
#define CLIENT_HAS_NO_MAINWINDOW() (znodes_of_this_client() == 0)

static int srvDesktopCommand (int id)
{
    if (id == IDM_REDRAWBG) {
        SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
    }
    else if (id == IDM_CLOSEALLWIN) {
        lock_zi_for_read (__mg_zorder_info);
        do_for_all_znodes (NULL,
                    __mg_zorder_info, _cb_close_mainwin, ZT_ALL);
        unlock_zi_for_read (__mg_zorder_info);
    }
    else if (id == IDM_ENDSESSION) {
        SendMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
    }
#ifdef _MGHAVE_MENU
    else if (id >= IDM_FIRSTWINDOW && id < IDM_SWITCH_LAYER) {
        HMENU win_menu;
        MENUITEMINFO mii = {MIIM_DATA};
        ZORDERNODE* node;

        win_menu = GetSubMenu (sg_DesktopMenu, 3);
        if (GetMenuItemInfo (win_menu, id, MF_BYCOMMAND, &mii) == 0) {
            node = (ZORDERNODE*)mii.itemdata;
            if (node && (node->flags & ZOF_TF_MAINWIN)
                            && !(node->flags & ZOF_DISABLED)) {
                ZORDERNODE * win_nodes =
                        (ZORDERNODE*) ((char*)(__mg_zorder_info + 1) +
                                       __mg_zorder_info->size_usage_bmp);
                win_nodes += DEF_NR_POPUPMENUS;

                srvMove2Top (node->cli, node - win_nodes);
                srvSetActiveWindow (node->cli, node - win_nodes);
            }
        }
    }
    else if (id >= IDM_SWITCH_LAYER && id < IDM_DELETE_LAYER) {
        HMENU win_menu;
        MENUITEMINFO mii = {MIIM_DATA};

        win_menu = GetSubMenu (sg_DesktopMenu, 4);
        if (GetMenuItemInfo (win_menu, id, MF_BYCOMMAND, &mii) == 0) {
            ServerSetTopmostLayer ((MG_Layer*) mii.itemdata);
        }
    }
    else if (id >= IDM_DELETE_LAYER) {
        HMENU win_menu;
        MENUITEMINFO mii = {MIIM_DATA};

        win_menu = GetSubMenu (sg_DesktopMenu, 5);
        if (GetMenuItemInfo (win_menu, id, MF_BYCOMMAND, &mii) == 0) {
            ServerDeleteLayer ((MG_Layer*) mii.itemdata);
        }
    }
#endif /* defined _MGHAVE_MENU */

    return 0;
}

static int srvRegisterIMEWnd (HWND hwnd)
{
    if (!mgIsServer)
        return ERR_INV_HWND;

    if (__mg_ime_wnd != 0)
        return ERR_IME_TOOMUCHIMEWND;

    if (!checkAndGetMainWinIfMainWin (hwnd))
        return ERR_INV_HWND;

    __mg_ime_wnd = hwnd;

    SendNotifyMessage (__mg_ime_wnd, MSG_IME_CLOSE, 0, 0);

    return ERR_OK;
}

static int srvUnregisterIMEWnd (HWND hwnd)
{
    if (__mg_ime_wnd != hwnd)
        return ERR_IME_NOSUCHIMEWND;

    __mg_ime_wnd = 0;

    return ERR_OK;
}

/***********************************************************
 * Session message handling for the clients.
 **********************************************************/
static BOOL _cb_count_znodes (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    if (node->cli == __mg_client_id) {
        return TRUE;
    }

    return FALSE;
}

static int znodes_of_this_client (void)
{
    int count = 0;

    lock_zi_for_read (__mg_zorder_info);
    count = do_for_all_znodes (NULL, __mg_zorder_info,
                    _cb_count_znodes, ZT_ALL);
    unlock_zi_for_read (__mg_zorder_info);

    return count;
}

static int cliSesseionMessageHandler (int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_STARTSESSION:
        __mg_init_local_sys_text ();
        break;

    case MSG_REINITSESSION:
        if (wParam)
            __mg_init_local_sys_text ();
        break;

    case MSG_ENDSESSION:
        if (CLIENT_HAS_NO_MAINWINDOW()) {
            PostQuitMessage (HWND_DESKTOP);
            return 1;
        }
        break;

    default:
        break;
#if 0
    case MSG_ERASEDESKTOP:
    case MSG_DT_KEYDOWN:
    case MSG_DT_CHAR:
    case MSG_DT_KEYUP:
    case MSG_DT_SYSKEYDOWN:
    case MSG_DT_SYSCHAR:
    case MSG_DT_SYSKEYUP:
    case MSG_DT_LBUTTONDOWN:
    case MSG_DT_LBUTTONUP:
    case MSG_DT_LBUTTONDBLCLK:
    case MSG_DT_MOUSEMOVE:
    case MSG_DT_RBUTTONDOWN:
    case MSG_DT_RBUTTONDBLCLK:
    case MSG_DT_RBUTTONUP:
        break;
#endif
    }

    return 0;
}

/***********************************************************
 * Handler for MSG_PAINT for the server and all clients.
 **********************************************************/

typedef struct _REFRESH_INFO
{
    const RECT* invrc;
    BOOL is_empty_invrc;
} REFRESH_INFO;

static BOOL _cb_refresh_znode (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    PMAINWIN pTemp;
    REFRESH_INFO* info = (REFRESH_INFO*) context;

    if (node->cli == __mg_client_id) {
        pTemp = (PMAINWIN)node->hwnd;

        if (pTemp && pTemp->WinType != TYPE_CONTROL
                        && pTemp->dwStyle & WS_VISIBLE) {
            if (info->is_empty_invrc) {
                SendAsyncMessage ((HWND)pTemp, MSG_NCPAINT, 0, 0);
                InvalidateRect ((HWND)pTemp, NULL, TRUE);
            }
            else {
                RECT rcTemp, rcInv;
                if (IntersectRect (&rcTemp,
                            (RECT*)(&pTemp->left), info->invrc)) {
                    dskScreenToWindow (pTemp, &rcTemp, &rcInv);
                    SendAsyncMessage ((HWND)pTemp,
                                    MSG_NCPAINT, 0, (LPARAM)(&rcInv));
                    dskScreenToClient (pTemp, &rcTemp, &rcInv);
                    InvalidateRect ((HWND)pTemp, &rcInv, TRUE);
                }
            }
        }

        return TRUE;
    }

    return FALSE;
}

static void dskRefreshAllWindow (const RECT* invrc)
{
    REFRESH_INFO info = {invrc, FALSE};

    if (invrc->top == invrc->bottom || invrc->left == invrc->right)
        info.is_empty_invrc = TRUE;

    if (mgIsServer)
        SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0,
                            (LPARAM)(info.is_empty_invrc?0:&invrc));

    lock_zi_for_read (__mg_zorder_info);
    do_for_all_znodes (&info, __mg_zorder_info, _cb_refresh_znode, ZT_ALL);
    unlock_zi_for_read (__mg_zorder_info);
}

#ifdef _MGMISC_SAVESCREEN
static void srvSaveScreen (BOOL active)
{
    RECT rcActive;
    HWND hwndActive = 0;
    int cliActive = -1;

    static int n = 1;
    char buffer[20];

    if (active) {
        ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

        if (__mg_zorder_info->active_win) {
            cliActive = nodes [__mg_zorder_info->active_win].cli;
            hwndActive = nodes [__mg_zorder_info->active_win].hwnd;
            rcActive = nodes [__mg_zorder_info->active_win].rc;
        }
    }

    if (cliActive == -1) {
        cliActive = 0;
        hwndActive = 0;
        rcActive = GetScreenRect();
    }

    sprintf (buffer, "%d-%p-%d.bmp", cliActive, hwndActive, n);
    if (SaveScreenRectContent (&rcActive, buffer)) {
        Ping ();
        n ++;
    }
}
#endif /* _MGMISC_SAVESCREEN */

static BOOL _cb_paint_mainwin (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    if (node->flags & ZOF_TF_MAINWIN) {
        post_msg_by_znode_p (zi, node, MSG_PAINT, 0, 0);
    }
    return TRUE;
}

void GUIAPI DesktopUpdateAllWindow(void)
{
    SendMessage(HWND_DESKTOP, MSG_PAINT, 0L, 0L);

    lock_zi_for_read (__mg_zorder_info);
    do_for_all_znodes (NULL, __mg_zorder_info, _cb_paint_mainwin, ZT_ALL);
    unlock_zi_for_read (__mg_zorder_info);
}

#ifdef _MGHAVE_MENU
static void srvUpdateDesktopMenu (void)
{
    UPDATA_DSKMENU_INFO info;
    int level, nCount, count, iPos, sep_pos;
    MG_Layer* layer;

    info.menu = GetSubMenu (sg_DesktopMenu, 3);

    nCount = GetMenuItemCount (info.menu);
    for (iPos = nCount; iPos > 0; iPos --)
        DeleteMenu (info.menu, iPos - 1, MF_BYPOSITION);

    memset (&info.mii, 0, sizeof (MENUITEMINFO));
    info.mii.type = MFT_STRING;
    info.id = IDM_FIRSTWINDOW;
    info.pos = 0;

    sep_pos = 0;
    for (level = 0; level < NR_ZORDER_LEVELS; level++) {

        if (sep_pos > 0 && level < (NR_ZORDER_LEVELS - 1)) {
            info.mii.type            = MFT_SEPARATOR;
            info.mii.state           = 0;
            info.mii.id              = 0;
            info.mii.typedata        = 0;
            InsertMenuItem (info.menu, sep_pos, TRUE, &info.mii);

            info.pos ++;
        }

        info.mii.type = MFT_STRING;
        count = do_for_all_znodes (&info,
                __mg_zorder_info, _cb_update_dskmenu, _zts_for_level[level]);
        if (count > 0)
            sep_pos = info.pos;
        else
            sep_pos = 0;
    }

    info.menu = GetSubMenu (sg_DesktopMenu, 4);
    nCount = GetMenuItemCount (info.menu);
    for (iPos = nCount; iPos > 0; iPos --)
        DeleteMenu (info.menu, iPos - 1, MF_BYPOSITION);

    info.mii.type = MFT_STRING;
    info.id = IDM_SWITCH_LAYER;
    info.pos = 0;
    layer = mgLayers;
    while (layer) {
        info.mii.id              = info.id;
        info.mii.typedata        = (DWORD)layer->name;
        info.mii.itemdata        = (DWORD)layer;
        if (mgTopmostLayer == layer)
            info.mii.state       = MFS_DISABLED;
        else
            info.mii.state       = MFS_ENABLED;

        InsertMenuItem (info.menu, info.pos, TRUE, &info.mii);

        info.pos ++;
        info.id ++;

        layer = layer->next;
    }

    info.menu = GetSubMenu (sg_DesktopMenu, 5);
    nCount = GetMenuItemCount (info.menu);
    for (iPos = nCount; iPos > 0; iPos --)
        DeleteMenu (info.menu, iPos - 1, MF_BYPOSITION);

    info.mii.type = MFT_STRING;
    info.id = IDM_DELETE_LAYER;
    info.pos = 0;
    layer = mgLayers;
    while (layer) {
        info.mii.id              = info.id;
        info.mii.typedata        = (DWORD)layer->name;
        info.mii.itemdata        = (DWORD)layer;
        if (mgTopmostLayer == layer)
            info.mii.state       = MFS_DISABLED;
        else
            info.mii.state       = MFS_ENABLED;

        InsertMenuItem (info.menu, info.pos, TRUE, &info.mii);

        info.pos ++;
        info.id ++;

        layer = layer->next;
    }

    nCount = GetMenuItemCount (sg_DesktopMenu);
    for (iPos = nCount; iPos > 7; iPos --)
        DeleteMenu (sg_DesktopMenu, iPos - 1, MF_BYPOSITION);

    dsk_ops->customize_desktop_menu (dt_context, sg_DesktopMenu, 7);
}
#endif /* defined _MGHAVE_MENU */

#include "debug.h"

static int srvSesseionMessageHandler (int message, WPARAM wParam, LPARAM lParam)
{
    static HDC hDesktopDC;

    switch (message) {
    case MSG_STARTSESSION:
        __mg_init_local_sys_text ();
        hDesktopDC = GetDC (HWND_DESKTOP);

        if (dsk_ops->init)
            dt_context = dsk_ops->init (hDesktopDC);
#ifdef _MGHAVE_MENU
        sg_DesktopMenu = srvCreateDesktopMenu ();
#endif
        break;

    case MSG_REINITSESSION:
        if (wParam)
            __mg_init_local_sys_text ();

#ifdef _MGHAVE_MENU
        sg_DesktopMenu = srvCreateDesktopMenu ();
#endif
        SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
        break;

    case MSG_REINITDESKOPS:
        if (dsk_ops->init) {
            dt_context = dsk_ops->init (hDesktopDC);
            SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
        }
        break;

    case MSG_ENDSESSION:
        if (SERVER_HAS_NO_MAINWINDOW()) {
            __mg_screensaver_destroy();

            if (hDesktopDC) {
                ReleaseDC (hDesktopDC);
                hDesktopDC = 0;
            }

#ifdef _MGHAVE_MENU
            if (sg_DesktopMenu) {
                DestroyMenu (sg_DesktopMenu);
                sg_DesktopMenu = 0;
            }
#endif

            if(dsk_ops->deinit) {
                dsk_ops->deinit(dt_context);
            }

            PostQuitMessage (HWND_DESKTOP);
            return 1;
        }
        break;

    case MSG_ERASEDESKTOP:
        if (dsk_ops->paint_desktop)
            dsk_ops->paint_desktop(dt_context, hDesktopDC, (PRECT)lParam);
        break;

    case MSG_DT_KEYLONGPRESS:
    case MSG_DT_KEYALWAYSPRESS:
    case MSG_DT_KEYDOWN:
    case MSG_DT_CHAR:
    case MSG_DT_KEYUP:
    case MSG_DT_SYSKEYDOWN:
    case MSG_DT_SYSCHAR:
    case MSG_DT_SYSKEYUP:
        if(dsk_ops->keyboard_handler)
            dsk_ops->keyboard_handler(dt_context, message, wParam, lParam);
        break;

#ifdef _MGHAVE_MENU
    case MSG_DT_RBUTTONUP:
        if (sg_DesktopMenu)
            srvUpdateDesktopMenu ();
#endif
    case MSG_DT_LBUTTONDOWN:
    case MSG_DT_LBUTTONUP:
    case MSG_DT_LBUTTONDBLCLK:
    case MSG_DT_MOUSEMOVE:
    case MSG_DT_RBUTTONDOWN:
    case MSG_DT_RBUTTONDBLCLK:
    case MSG_DT_MBUTTONDOWN:
    case MSG_DT_MBUTTONUP:
    case MSG_DT_MBUTTONDBLCLK:
        if(dsk_ops->mouse_handler)
            dsk_ops->mouse_handler(dt_context, message, wParam, lParam);
        break;
    }

    return 0;
}

/* Since 5.0.0, we handle caret blink and auto repeat message here */
static void dskOnTimer (void)
{
    static UINT uCounter = 0;
    static UINT blink_counter = 0;

    if (sg_msgAutoRepeat.hwnd != 0) {
        PostMessage (sg_msgAutoRepeat.hwnd, sg_msgAutoRepeat.message,
                sg_msgAutoRepeat.wParam, sg_msgAutoRepeat.lParam);
    }

    if (__mg_tick_counter < (blink_counter + 10))
        return;

    uCounter += (__mg_tick_counter - blink_counter) * 10;
    blink_counter = __mg_tick_counter;

    if (sg_hCaretWnd != 0
            && (HWND)checkAndGetMainWinIfWindow (sg_hCaretWnd)
                    == dskGetActiveWindow (NULL)
            && uCounter >= sg_uCaretBTime) {
        PostMessage (sg_hCaretWnd, MSG_CARETBLINK, 0, 0);
        uCounter = 0;
    }
}


static LRESULT DesktopWinProc (HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam)
{
    WPARAM flags;
    int x, y;

    if (message >= MSG_FIRSTWINDOWMSG && message <= MSG_LASTWINDOWMSG) {
        return dskWindowMessageHandler (message, (PMAINWIN)wParam, lParam);
    }
    // VW: Since 4.0.0 for extra input messages.
    else if (message >= MSG_FIRSTEXTRAINPUTMSG &&
            message <= MSG_LASTEXTRAINPUTMSG) {
        // Since 5.0.0; moved from server.c
        if (mgIsServer && dskPreExtraMessageHandler (message, wParam, lParam))
            return 0;

        if (mgIsServer) {

            if (do_drag_drop_window (message, 0, 0))
                return 0;

            if (__mg_zorder_info->active_win) {
                __mg_post_msg_by_znode (__mg_zorder_info,
                        __mg_zorder_info->active_win,
                        message, wParam, lParam);
            }
        }
    }
    else if (message >= MSG_FIRSTKEYMSG && message <= MSG_LASTKEYMSG) {
        // Since 5.0.0; moved from server.c
        if (mgIsServer && dskPreKeyMessageHandler (message, wParam, lParam))
            return 0;

        if (mgIsServer) {

            if (do_drag_drop_window (message, 0, 0))
                return 0;

            if (wParam == SCANCODE_PRINTSCREEN && message == MSG_KEYDOWN) {
#ifdef _MGMISC_SAVESCREEN
                srvSaveScreen (lParam & KS_CTRL);
#endif
            }
#ifdef _MGHAVE_MENU
            else if (wParam == SCANCODE_ESCAPE && lParam & KS_CTRL) {
                srvUpdateDesktopMenu ();
                TrackPopupMenu (sg_DesktopMenu, TPM_DEFAULT,
                        0, g_rcScr.bottom, HWND_DESKTOP);
            }
#endif

            if (__mg_zorder_info->cli_trackmenu > 0) {
                MSG msg = {0, message, wParam, lParam, __mg_tick_counter};
                __mg_send2client (&msg, mgClients +
                                __mg_zorder_info->cli_trackmenu);
            }
#ifdef _MGHAVE_MENU
            else if (sg_ptmi)
                PopupMenuTrackProc (sg_ptmi, message, wParam, lParam);
#endif
            else
                srvKeyMessageHandler (message, (int)wParam, (DWORD)lParam);
        }
#ifdef _MGHAVE_MENU
        else if (sg_ptmi)
            PopupMenuTrackProc (sg_ptmi, message, wParam, lParam);
#endif

        return 0;
    }
    else if (message >= MSG_FIRSTMOUSEMSG && message <= MSG_LASTMOUSEMSG) {
        flags = wParam;

        x = LOSWORD (lParam);
        y = HISWORD (lParam);

        // Since 5.0.0; moved from server.c
        if (mgIsServer && srvPreMouseMessageHandler (message, flags, x, y))
            return 0;

        if (mgIsServer) {

            if (__mg_zorder_info->cli_trackmenu > 0 ) {

                MSG msg = {0, message, wParam, lParam, __mg_tick_counter};

                __mg_send2client (&msg, mgClients +
                                __mg_zorder_info->cli_trackmenu);
            }
#ifdef _MGHAVE_MENU
            else if (sg_ptmi) {
                if (PopupMenuTrackProc (sg_ptmi, message, x, y) == 0)
                    return 0;
                dskMouseMessageHandler (message, flags, x, y);
            }
#endif
            else
                dskMouseMessageHandler (message, flags, x, y);
        }
#ifdef _MGHAVE_MENU
        else if (sg_ptmi) {
            if (PopupMenuTrackProc (sg_ptmi, message, x, y) == 0)
                return 0;
            dskMouseMessageHandler (message, flags, x, y);
        }
#endif
        else
            dskMouseMessageHandler (message, flags, x, y);

        return 0;
    }
    else if (message == MSG_COMMAND) {
        if (wParam <= MAXID_RESERVED && wParam >= MINID_RESERVED) {
            return srvDesktopCommand ((int)wParam);
        }
        else {
            if (dsk_ops->desktop_menucmd_handler)
                dsk_ops->desktop_menucmd_handler(dt_context, (int)wParam);
            return 0;
        }
    }
    else if (message >= MSG_FIRSTSESSIONMSG && message <= MSG_LASTSESSIONMSG) {
        if (mgIsServer)
            return srvSesseionMessageHandler (message, wParam, lParam);
        else
            return cliSesseionMessageHandler (message, wParam, lParam);
    }

    switch (message) {
    case MSG_LAYERCHANGED: {
        if (mgIsServer) {
            _WRN_PRINTF("The server got a MSG_LAYERCHANGED message\n");
        }
        else {
            __mg_client_on_layer_changed ((GHANDLE)wParam, (int)lParam);
        }
        break;
    }

    case MSG_TIMEOUT:
        // Since 5.0.0: MSG_IDLE messages will be generated by PeekMessage.
        // BroadcastMessageInThisThread (MSG_IDLE, wParam, 0);
        break;

    case MSG_SRVNOTIFY: {
        MSG msg = {0, MSG_SRVNOTIFY, wParam, lParam};
        dskBroadcastMessage (&msg);
        break;
    }

    case MSG_PAINT: {
        RECT invrc;

        invrc.left = LOWORD(wParam);
        invrc.top = HIWORD(wParam);
        invrc.right = LOWORD(lParam);
        invrc.bottom = HIWORD(lParam);

        dskRefreshAllWindow (&invrc);
        break;
    }

    case MSG_REGISTERWNDCLASS:
        return gui_AddNewControlClass ((PWNDCLASS)lParam);

    case MSG_UNREGISTERWNDCLASS:
        return gui_DeleteControlClass ((const char*)lParam);

    case MSG_NEWCTRLINSTANCE:
        return dskOnNewCtrlInstance ((PCONTROL)wParam, (PCONTROL)lParam);

    case MSG_REMOVECTRLINSTANCE:
        return dskOnRemoveCtrlInstance ((PCONTROL)wParam, (PCONTROL)lParam);

    case MSG_GETCTRLCLASSINFO:
        return (LRESULT)gui_GetControlClassInfo ((const char*)lParam);

    case MSG_CTRLCLASSDATAOP:
        return (LRESULT)gui_ControlClassDataOp (wParam, (WNDCLASS*)lParam);

    /* Since 5.0.0 */
    case MSG_REGISTERHOOKFUNC:
        return (LRESULT)dskRegisterHookFunc ((int)wParam, (HOOKINFO*)lParam);

    /* Since 5.0.0 */
    case MSG_REGISTERHOOKWIN:
        if (mgIsServer) {
            return (LRESULT)dskRegisterHookWin (0, (HWND)wParam, (DWORD)lParam);
        }
        else {
            return (LRESULT)cliRegisterHookWin ((HWND)wParam, (DWORD)lParam);
        }
        break;

    /* Since 5.0.0 */
    case MSG_UNREGISTERHOOKWIN:
        if (mgIsServer) {
            return (LRESULT)dskUnregisterHookWin (0, (HWND)wParam);
        }
        else {
            return (LRESULT)cliUnregisterHookWin ((HWND)wParam);
        }
        break;

#ifdef _MGHAVE_VIRTUAL_WINDOW
    /* Since 5.0.0 */
    case MSG_MANAGE_MSGTHREAD:
        if (wParam == MSGTHREAD_SIGNIN)
            return dskRegisterMsgQueue ((MSGQUEUE*)lParam);
        return dskUnregisterMsgQueue ((MSGQUEUE*)lParam);
#endif

    /* Since 5.0.0 */
    case MSG_CALC_POSITION:
        if (mgIsServer) {
            dskCalculateDefaultPosition ((int)wParam, (CALCPOSINFO*)lParam);
            return 0;
        }
        else
            return cliCalculateDefaultPosition ((CALCPOSINFO*)lParam);
        break;

    case MSG_IME_REGISTER:
        if (mgIsServer)
            return srvRegisterIMEWnd ((HWND)wParam);
        break;

    case MSG_IME_UNREGISTER:
        if (mgIsServer)
            return srvUnregisterIMEWnd ((HWND)wParam);
        break;

    case MSG_IME_SETSTATUS:
        if (mgIsServer)
            return dskSetIMEStatus ((int)wParam, (int)lParam);
        break;

    case MSG_IME_GETSTATUS:
        if (mgIsServer)
            return dskGetIMEStatus ((int)wParam);
        break;

    case MSG_IME_SET_TARGET_INFO:
        if (mgIsServer)
            return dskSetIMETargetInfo ((IME_TARGET_INFO*)lParam);
        break;

    case MSG_IME_GET_TARGET_INFO:
        if (mgIsServer)
            return dskGetIMETargetInfo ((IME_TARGET_INFO*)lParam);
        break;

    case MSG_BROADCASTMSG:
        return dskBroadcastMessage ((PMSG)lParam);

    case MSG_TIMER:
#if 0   /* deprecated code */
    /* Since 5.0.0, the desktop only handles caret blinking in MSG_TIMER
       message, and the interval for this MSG_TIMER changes to about 0.05s. */
        if (__mg_quiting_stage < 0) {
            if (__mg_quiting_stage > _MG_QUITING_STAGE_FORCE &&
                    __mg_quiting_stage <= _MG_QUITING_STAGE_START) {
                __mg_quiting_stage--;
                /* printf("try to quit %d\n", __mg_quiting_stage); */
            }
            else if (__mg_quiting_stage <= _MG_QUITING_STAGE_FORCE) {
                /* printf("force to quit !!!\n"); */
            }

            if (__mg_quiting_stage > _MG_QUITING_STAGE_DESKTOP
                    && (mgIsServer ? SERVER_HAS_NO_MAINWINDOW() :
                        CLIENT_HAS_NO_MAINWINDOW())) {
                __mg_quiting_stage = _MG_QUITING_STAGE_DESKTOP;
            }
            else if (__mg_quiting_stage <= _MG_QUITING_STAGE_DESKTOP) {
                PostMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
            }
        }
#endif  /* deprecated code */

        dskOnTimer ();
        break;
    }

    return 0;
}

#ifdef _MGSCHEMA_COMPOSITING
struct _my_circle_context {
    int         status;     /* whether failed to allocate cliprect for region */
    int         r;          /* the radius of the round corner */
    RECT        rc;         /* the rectangle of the znode */
    DWORD       tw_flags;   /* the corner flags of the znode */
    DWORD       rgn_ops;    /* region operation */
    CLIPRGN*    dst_rgn;    /* the destination region */
};

static void cb_circle_corners (void* context, int x1, int x2, int y)
{
    struct _my_circle_context* ctxt = (struct _my_circle_context*)context;
    RECT rc;

    if (ctxt->status) // check status first
        return;

    if (y < 0) {
        if (ctxt->tw_flags & ZOF_TW_TROUNDCNS) {
            rc.left = ctxt->rc.left + x1 + ctxt->r;
            rc.right = ctxt->rc.right + x2 - ctxt->r;
            rc.top = ctxt->rc.top + y + ctxt->r;
            rc.bottom = rc.top + 1;
        }
        else {
            return;
        }
    }
    else if (y > 0) {
        if (ctxt->tw_flags & ZOF_TW_BROUNDCNS) {
            rc.left = ctxt->rc.left + x1 + ctxt->r;
            rc.right = ctxt->rc.right + x2 - ctxt->r;
            rc.top = ctxt->rc.bottom + y - ctxt->r;
            rc.bottom = rc.top + 1;
        }
        else {
            return;
        }
    }
    else {
        rc.left = ctxt->rc.left + x1 + ctxt->r;
        rc.right = ctxt->rc.right + x2 - ctxt->r;
        rc.top = ctxt->rc.bottom - ctxt->r;
        rc.bottom = rc.top + 1;
    }

    // check out of bound
    {
        RECT eff_rc = ctxt->rc;
        if (!IntersectRect (&rc, &eff_rc, &rc))
            return;
    }

    if (!(ctxt->rgn_ops & RGN_OP_FLAG_ABS)) {
        OffsetRect (&rc, -ctxt->rc.left, -ctxt->rc.top);
    }

    if ((ctxt->rgn_ops & RGN_OP_MASK) == RGN_OP_EXCLUDE) {
        if (!SubtractClipRect (ctxt->dst_rgn, &rc))
            ctxt->status = -1;
    }
    else {
        _DBG_PRINTF("add new rect: %d, %d, %d, %d for (%d, %d) to (%d, %d)\n",
                rc.left, rc.top, rc.right, rc.bottom, x1, y, x2, y);
        if (!AddClipRect (ctxt->dst_rgn, &rc))
            ctxt->status = -1;
    }
}
#endif /* _MGSCHEMA_COMPOSITING */


BOOL GUIAPI ServerGetWinZNodeRegion (MG_Layer* layer, int idx_znode,
                DWORD rgn_ops, CLIPRGN* dst_rgn)
{
    RECT rc;
    MASKRECT *maskrect;
    ZORDERNODE* nodes;
    ZORDERINFO* zi;
    int idx, nr_mask_rects;

    if (!mgIsServer || idx_znode <= 0)
        return FALSE;

    if (layer) {
        zi = mgTopmostLayer->zorder_info;
    }
    else {
        if (!__mg_is_valid_layer (layer))
            return FALSE;
        zi = layer->zorder_info;
    }

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return FALSE;
    }

    if ((rgn_ops & RGN_OP_MASK) == RGN_OP_SET) {
        EmptyClipRgn (dst_rgn);
        rgn_ops &= ~RGN_OP_MASK;
        rgn_ops |= RGN_OP_INCLUDE;
    }

    /* lock zi for read */
    lock_zi_for_read (zi);

    nr_mask_rects = 0;
    nodes = GET_ZORDERNODE(zi);
    maskrect = GET_MASKRECT(zi);
    idx = nodes [idx_znode].idx_mask_rect;
    while (idx) {
        rc.left = maskrect->left;
        rc.top = maskrect->top;
        rc.right = maskrect->left;
        rc.bottom = maskrect->bottom;

        if (rgn_ops & RGN_OP_FLAG_ABS) {
            OffsetRect (&rc, nodes[idx_znode].rc.left, nodes[idx_znode].rc.top);
        }

        if ((rgn_ops & RGN_OP_MASK) == RGN_OP_EXCLUDE) {
            if (!SubtractClipRect (dst_rgn, &rc)) {
                nr_mask_rects = -1;
                goto __mg_err_ret;
            }
        }
        else {
            if (!AddClipRect (dst_rgn, &rc)) {
                nr_mask_rects = -1;
                goto __mg_err_ret;
            }
        }

        idx = maskrect->next;
        nr_mask_rects ++;
    }

    if (nr_mask_rects == 0) {
        rc = nodes[idx_znode].rc;
        if (!(rgn_ops & RGN_OP_FLAG_ABS)) {
            OffsetRect (&rc, -rc.left, -rc.top);
        }

#ifdef _MGSCHEMA_COMPOSITING
        /* Since 5.0.0, we count in the round corners here */
        if (nodes[idx_znode].flags & ZOF_TW_FLAG_MASK) {
            struct _my_circle_context ctxt = { 0, RADIUS_WINDOW_CORNERS, };

            if (RECTW(rc) >= (RADIUS_WINDOW_CORNERS << 1) &&
                    RECTH(rc) >= (RADIUS_WINDOW_CORNERS << 1)) {
                ctxt.status = 0;
                ctxt.r = RADIUS_WINDOW_CORNERS;
                ctxt.rc = rc;
                /* both top and bottom corners for popup menu */
                ctxt.tw_flags = nodes[idx_znode].flags & ZOF_TW_FLAG_MASK;    
                ctxt.rgn_ops = rgn_ops;
                ctxt.dst_rgn = dst_rgn;

                CircleGenerator (&ctxt, 0, 0, RADIUS_WINDOW_CORNERS,
                        cb_circle_corners);

                if (ctxt.status) {
                    nr_mask_rects = -1;
                    goto __mg_err_ret;
                }

                if (nodes[idx_znode].flags & ZOF_TW_TROUNDCNS) {
                    rc.top += RADIUS_WINDOW_CORNERS;
                }
                if (nodes[idx_znode].flags & ZOF_TW_BROUNDCNS) {
                    rc.bottom -= RADIUS_WINDOW_CORNERS;
                }
            }
        }
#endif /* _MGSCHEMA_COMPOSITING */

        if ((rgn_ops & RGN_OP_MASK) == RGN_OP_EXCLUDE) {
            if (!SubtractClipRect (dst_rgn, &rc)) {
                nr_mask_rects = -1;
                goto __mg_err_ret;
            }
        }
        else {
            if (!AddClipRect (dst_rgn, &rc)) {
                nr_mask_rects = -1;
                goto __mg_err_ret;
            }
        }
    }

__mg_err_ret:
    /* unlock zi for read */
    unlock_zi_for_read (zi);

    return nr_mask_rects >= 0;
}

BOOL GUIAPI ServerGetPopupMenuZNodeRegion (int idx_znode,
                DWORD rgn_ops, CLIPRGN* dst_rgn)
{
    RECT rc;
    ZORDERNODE* nodes;
    ZORDERINFO* zi;
    int nr_mask_rects = 0;

    if (!mgIsServer || idx_znode < 0)
        return FALSE;

    zi = mgTopmostLayer->zorder_info;
    if (idx_znode >= zi->nr_popupmenus)
        return FALSE;

    if ((rgn_ops & RGN_OP_MASK) == RGN_OP_SET) {
        EmptyClipRgn (dst_rgn);
        rgn_ops &= ~RGN_OP_MASK;
        rgn_ops |= RGN_OP_INCLUDE;
    }

    /* lock zi for read */
    lock_zi_for_read (zi);

    nodes = GET_MENUNODE(zi);
    rc = nodes[idx_znode].rc;
    if (!(rgn_ops & RGN_OP_FLAG_ABS)) {
        OffsetRect (&rc, -rc.left, -rc.top);
    }

#ifdef _MGSCHEMA_COMPOSITING
    {
        struct _my_circle_context ctxt = { 0, RADIUS_POPUPMENU_CORNERS, };

        if (RECTW(rc) >= (RADIUS_POPUPMENU_CORNERS << 1) &&
                RECTH(rc) >= (RADIUS_POPUPMENU_CORNERS << 1)) {
            ctxt.status = 0;
            ctxt.r = RADIUS_POPUPMENU_CORNERS;
            ctxt.rc = rc;
            /* both top and bottom corners for popup menu */
            ctxt.tw_flags = ZOF_TW_TROUNDCNS | ZOF_TW_BROUNDCNS;    
            ctxt.rgn_ops = rgn_ops;
            ctxt.dst_rgn = dst_rgn;

            CircleGenerator (&ctxt, 0, 0, RADIUS_POPUPMENU_CORNERS,
                    cb_circle_corners);

            if (ctxt.status) {
                nr_mask_rects = -1;
                goto __mg_err_ret;
            }

            rc.top += RADIUS_POPUPMENU_CORNERS;
            rc.bottom -= RADIUS_POPUPMENU_CORNERS;
        }
    }
#endif /* _MGSCHEMA_COMPOSITING */

    if ((rgn_ops & RGN_OP_MASK) == RGN_OP_EXCLUDE) {
        if (!SubtractClipRect (dst_rgn, &rc)) {
            nr_mask_rects = -1;
            goto __mg_err_ret;
        }
    }
    else {
        if (!AddClipRect (dst_rgn, &rc)) {
            nr_mask_rects = -1;
            goto __mg_err_ret;
        }
    }

__mg_err_ret:
    /* unlock zi for read */
    unlock_zi_for_read (zi);

    return nr_mask_rects >= 0;
}

#endif /* defined _MGRM_PROCESSES */

