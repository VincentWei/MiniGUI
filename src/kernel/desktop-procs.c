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

//#ifdef _MGRM_PROCESSES
#if defined _MGRM_PROCESSES && !defined _MGRM_STANDALONE

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

#ifndef _MG_ENABLE_SCREENSAVER
#   define SERVER_HAS_NO_MAINWINDOW() (nr_of_all_znodes() == 0)
#else
    /* The screensaver occupys one znode */
#   define SERVER_HAS_NO_MAINWINDOW() (nr_of_all_znodes() == 1)
#endif
#   define CLIENT_HAS_NO_MAINWINDOW() (znodes_of_this_client() == 0) 

/******************************* global data *********************************/
MSGQUEUE __mg_desktop_msg_queue;
PMSGQUEUE __mg_dsk_msg_queue = &__mg_desktop_msg_queue;

GHANDLE __mg_layer;

/* always be zero for clients. */
BOOL __mg_switch_away;

void lock_zi_for_read (const ZORDERINFO* zi)
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

void unlock_zi_for_read (const ZORDERINFO* zi)
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

void lock_zi_for_change (const ZORDERINFO* zi)
{
    int clients = 0;
    struct sembuf sb;

    clients = zi->max_nr_popupmenus + zi->max_nr_globals 
            + zi->max_nr_topmosts + zi->max_nr_normals;
    
    /* Cancel the current drag and drop operation */
    __mg_do_drag_drop_window (MSG_IDLE, 0, 0);

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

void unlock_zi_for_change (const ZORDERINFO* zi)
{
    int clients = 0;
    struct sembuf sb;
    clients = zi->max_nr_popupmenus + zi->max_nr_globals 
            + zi->max_nr_topmosts + zi->max_nr_normals;
    
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

inline void* get_zi_from_client(int cli) 
{
    return (((cli>0)?mgClients[cli].layer:mgTopmostLayer)->zorder_info);
}

/***************** Initialization/Termination routines ***********************/
#include "desktop.c"

static BOOL InitWndManagementInfo (void)
{
    __mg_capture_wnd = 0;

    __mg_ime_wnd = 0;

#ifdef _MGHAVE_MENU
    sg_ptmi = NULL;
#endif

    sg_hCaretWnd = 0;

    return TRUE;
}

static void TerminateSharedSysRes (void)
{
    return;
}

static void init_desktop_win (void)
{
    static MAINWIN desktop_win;
    PMAINWIN pDesktopWin;

    LICENSE_SET_MESSAGE_OFFSET();

    pDesktopWin = &desktop_win;

    pDesktopWin->pMessages         = __mg_dsk_msg_queue;
    pDesktopWin->MainWindowProc    = DesktopWinProc;

    pDesktopWin->DataType          = TYPE_HWND;
    pDesktopWin->WinType           = TYPE_ROOTWIN;

    pDesktopWin->pLogFont          = GetSystemFont (SYSLOGFONT_WCHAR_DEF);
    pDesktopWin->spCaption         = NULL;
    pDesktopWin->iBkColor          = 0;

    if (mgIsServer) {
        pDesktopWin->pGCRInfo      = &sg_ScrGCRInfo;
        pDesktopWin->idx_znode     = 0;
    }

    if (mgIsServer)
        pDesktopWin->spCaption = "THE DESKTOP OF THE SERVER";
    else
        pDesktopWin->spCaption = "THE VIRTUAL DESKTOP OF CLIENT";

    pDesktopWin->pMainWin          = pDesktopWin;
    pDesktopWin->we_rdr            = __mg_def_renderer; 

    __mg_hwnd_desktop = (HWND)pDesktopWin;
    __mg_dsk_win  = pDesktopWin;

}

BOOL mg_InitDesktop (void)
{
    /*
     * Init heap of clipping rects.
     */
    InitFreeClipRectList (&sg_FreeClipRectList, SIZE_CLIPRECTHEAP);

    /*
     * Init heap of invalid rects.
     */
    InitFreeClipRectList (&sg_FreeInvRectList, SIZE_INVRECTHEAP);

    // Init Window Management information.
    if (!InitWndManagementInfo ())
        return FALSE;

    init_desktop_win ();

    return TRUE;
}

void mg_TerminateDesktop (void)
{
    TerminateSharedSysRes ();

    DestroyFreeClipRectList (&sg_FreeClipRectList);
    DestroyFreeClipRectList (&sg_FreeInvRectList);
}

/********************** Common routines for ZORDERINFO ***********************/
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
        MSG msg = {hwnd, MSG_UPDATECLIWIN, 0, 0, __mg_timer_counter};

        while (crc) {
            msg.wParam = MAKELONG (crc->rc.left, crc->rc.top);
            msg.lParam = MAKELONG (crc->rc.right, crc->rc.bottom);

            ret = __mg_send2client (&msg, mgClients + cli);
            if (ret < 0)
                break;

            crc = crc->next;
        }
    }

    if (ret < 0)
        return -1;

    return 0;
}

/*********************** Client-side routines ********************************/

void __mg_start_client_desktop (void)
{
    SendMessage (HWND_DESKTOP, MSG_STARTSESSION, 0, 0);
}

static intptr_t cliAllocZOrderNode (PMAINWIN pWin)
{
    intptr_t ret;
    REQUEST req;
    ZORDEROPINFO info;

    info.id_op = ID_ZOOP_ALLOC;
    info.flags = get_znode_flags_from_style (pWin);
    info.hwnd = (HWND)pWin;
    info.main_win = (HWND)pWin->pMainWin;
    
    if (pWin->spCaption) {
        if (strlen (pWin->spCaption) <= MAX_CAPTION_LEN) {
            strcpy (info.caption, pWin->spCaption);
        } else {
            memcpy (info.caption, pWin->spCaption, MAX_CAPTION_LEN);
            info.caption[MAX_CAPTION_LEN] = '\0';
        }
    }
    
    dskGetWindowRectInScreen (pWin, &info.rc);

    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

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

static intptr_t cliMoveWindow (PMAINWIN pWin, const RECT* rcWin)
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

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
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

    if (ClientRequest (&req, &ret, sizeof (intptr_t)) < 0)
        return -1;

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
#endif

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

    info.id_op = ID_ZOOP_CHANGECAPTION;
    info.idx_znode = pWin->idx_znode;
    
    if (strlen (pWin->spCaption) <= MAX_CAPTION_LEN) {
        strcpy (info.caption, pWin->spCaption);
    } else {
        memcpy (info.caption, pWin->spCaption, MAX_CAPTION_LEN);
        info.caption[MAX_CAPTION_LEN] = '\0';
    }
    
    req.id = REQID_ZORDEROP;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);
    if ((ret = ClientRequest (&req, &ret, sizeof (intptr_t))) < 0) {
        return -1;
    }

    return ret;
}

/*********************** Server-side routines ********************************/

void __mg_start_server_desktop (void)
{
    InitClipRgn (&sg_ScrGCRInfo.crgn, &sg_FreeClipRectList);
    SetClipRgn (&sg_ScrGCRInfo.crgn, &g_rcScr);
    sg_ScrGCRInfo.age = 0;
    sg_ScrGCRInfo.old_zi_age = 0;

    InitClipRgn (&sg_UpdateRgn, &sg_FreeClipRectList);
    MAKE_REGION_INFINITE(&sg_UpdateRgn);

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

    if ((ret = ClientRequest (&req, &ret, sizeof (intptr_t))) < 0) {
        return -1;
    }
    return ret;
}

static int srvAllocZOrderMaskRect (int cli, int idx_znode, 
        int flags, const RECT4MASK *rc, const int nr_rc)
{
    return AllocZOrderMaskRect (cli, idx_znode, flags, rc, nr_rc);
}

static int srvFreeZOrderMaskRect (int cli, int idx_znode)
{
    return FreeZOrderMaskRect (cli, idx_znode);
}

int kernel_change_z_order_mask_rect (HWND pWin, const RECT4MASK* rc, int nr_rc)
{
    if (!rc || !nr_rc)
        return -1;

    if (mgIsServer) {
        return srvAllocZOrderMaskRect (0, 
                ((PMAINWIN)pWin)->idx_znode,
                get_znode_flags_from_style((PMAINWIN)pWin),
                rc, nr_rc);
    }
    else {
        cliFreeZOrderMaskRect ((PMAINWIN)pWin); 
        return cliAllocZOrderMaskRect (pWin, rc, nr_rc);
    }
}

ON_ZNODE_OPERATION OnZNodeOperation;

static int srvAllocZOrderNode (int cli, HWND hwnd, HWND main_win, 
                DWORD flags, const RECT *rc, const char *caption)
{
    int free_slot = AllocZOrderNode (cli, hwnd, 
                    main_win, flags, rc, caption);

    if ((free_slot != -1) && OnZNodeOperation)
        OnZNodeOperation (ZNOP_ALLOCATE, cli, free_slot);

    return free_slot;
}

static BOOL _cb_update_cli_znode (void* context, 
                const ZORDERINFO* zi, ZORDERNODE* znode)
{
    RECT rcInv;
    int cli = (int)(intptr_t)context;

    if (znode->cli == cli && znode->flags & ZOF_VISIBLE && znode->fortestinghwnd) {
       MSG msg = {znode->fortestinghwnd, MSG_UPDATECLIWIN, 0, 0, __mg_timer_counter};

        if (!IsRectEmpty(&(znode->dirty_rc))) {

            IntersectRect(&rcInv, &znode->dirty_rc, &g_rcScr);    

            msg.wParam = MAKELONG (rcInv.left, rcInv.top);
            msg.lParam = MAKELONG (rcInv.right, rcInv.bottom);

            SetRectEmpty (&(znode->dirty_rc));

            __mg_send2client (&msg, mgClients + znode->cli);
            return TRUE;
        }
    }

    return FALSE;
}

void __mg_check_dirty_znode (int cli)
{
    ZORDERINFO* zi = (ZORDERINFO *)get_zi_from_client (cli);

    do_for_all_znodes ((void*)(intptr_t)cli, zi, 
                    _cb_update_cli_znode, ZT_TOPMOST | ZT_NORMAL);

    mgClients [cli].has_dirty = FALSE;
}

static intptr_t srvSetActiveWindow (int cli, int idx_znode)
{
    HWND hRet = dskSetActiveZOrderNode (cli, idx_znode);

    if ((hRet != HWND_INVALID) && OnZNodeOperation)
        OnZNodeOperation (ZNOP_SETACTIVE, cli, idx_znode);

    return (intptr_t)hRet;
}

static int srvFreeZOrderNode (int cli, int idx_znode)
{
    int ret = FreeZOrderNode (cli, idx_znode);

    if (!ret && OnZNodeOperation)
        OnZNodeOperation (ZNOP_FREE, cli, idx_znode);

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

static int srvMoveWindow (int cli, int idx_znode, const RECT* rcWin)
{
    int ret = dskMoveWindow (cli, idx_znode, rcWin);

    if (!ret && OnZNodeOperation)
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
    ZORDERINFO* zi = _get_zorder_info(cli);
    ZORDERNODE* nodes;

    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals) ||
            idx_znode < 0) {
        return -1;
    }

    if (_dd_info.cli >= 0)
        return -1;

    nodes = GET_ZORDERNODE(zi);
    
    lock_zi_for_change (zi);
    
    _dd_info.cli = cli;
    _dd_info.idx_znode = idx_znode;
    _dd_info.hwnd = nodes [idx_znode].fortestinghwnd;
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
            _MG_PRINTF ("KERNEL>Desktop: Drag and drop window: bad location\n");
            break;
    }

    SetPenColor (HDC_SCREEN_SYS, PIXEL_lightwhite);
    SelectClipRect (HDC_SCREEN_SYS, &g_rcScr);
    do_for_all_znodes (NULL, zi, _cb_exclude_rc, ZT_GLOBAL);
    FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);

    if (OnZNodeOperation)
        OnZNodeOperation (ZNOP_STARTDRAG, cli, idx_znode);

    return 0;
}

static int srvCancelDragWindow (int cli, int idx_znode)
{
    ZORDERINFO* zi = _get_zorder_info(cli);

    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals) ||
            idx_znode < 0) {
        return -1;
    }

    if (_dd_info.cli == -1 
                    || _dd_info.cli != cli 
                    || _dd_info.idx_znode != idx_znode)
        return -1;

    _dd_info.cli = -1;
    unlock_zi_for_change (zi);
    SelectClipRect (HDC_SCREEN_SYS, &g_rcScr);
    //SetDefaultCursor (GetSystemCursor (IDC_ARROW));

    if (OnZNodeOperation)
        OnZNodeOperation (ZNOP_CANCELDRAG, cli, idx_znode);

    return 0;
}

int __mg_do_drag_drop_window (int msg, int x, int y)
{
    HWND hwnd;
    
    if (_dd_info.cli < 0)
        return 0;

    if (msg == MSG_MOUSEMOVE) {
        SetPenColor (HDC_SCREEN_SYS, PIXEL_lightwhite);
        FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);

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
                    _MG_PRINTF ("KERNEL>Desktop: __mg_do_drag_drop_window: bad location\n");
                    break;
        }

        FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);
                    
        _dd_info.last_x = x;
        _dd_info.last_y = y;
    }
    else {
        MSG msg = {_dd_info.hwnd, MSG_WINDOWDROPPED, 0, 0, __mg_timer_counter};

        msg.wParam = MAKELONG (_dd_info.rc.left, _dd_info.rc.top);
        msg.lParam = MAKELONG (_dd_info.rc.right, _dd_info.rc.bottom);

        SetPenColor (HDC_SCREEN_SYS, PIXEL_lightwhite);
        FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);

        /* post MSG_WINDOWDROPPED to the target window */
        if (_dd_info.cli == 0) {
            PostMessage (_dd_info.hwnd, MSG_WINDOWDROPPED,
                            msg.wParam, msg.lParam);
        }
        else {
            mgClients [_dd_info.cli].last_live_time = __mg_timer_counter;
            __mg_send2client (&msg, mgClients + _dd_info.cli);
        }

        unlock_zi_for_change (_dd_info.zi);
        SelectClipRect (HDC_SCREEN_SYS, &g_rcScr);
        __mg_get_znode_at_point (__mg_zorder_info, x, y, &hwnd);
        if(_dd_info.hwnd != hwnd)
            SetDefaultCursor (GetSystemCursor (IDC_ARROW));
        _dd_info.cli = -1;
    }

    return 1;
}

static int srvChangeCaption (int cli, int idx_znode, const char *caption)
{
    ZORDERINFO* zi = _get_zorder_info(cli);
    ZORDERNODE* nodes;

    nodes = GET_ZORDERNODE(zi);

    if (caption && idx_znode > 0) {
        PLOGFONT menufont ;
        int fit_chars, pos_chars[MAX_CAPTION_LEN];
        int caplen;
        menufont = GetSystemFont (SYSLOGFONT_MENU);
        caplen = strlen(caption);

        if (caplen < 32) {
            strcpy (nodes[idx_znode].caption, caption);
        } else {
            GetTextCharPos (menufont, caption, caplen, (32 - 3), /* '...' = 3*/
                                 &fit_chars, pos_chars);
            memcpy(nodes[idx_znode].caption, 
                   caption, pos_chars[fit_chars-1]);
            strcpy ((nodes[idx_znode].caption + pos_chars[fit_chars-1]), 
                    "...");
        }
    }

    if (OnZNodeOperation)
        OnZNodeOperation (ZNOP_CHANGECAPTION, cli, idx_znode);

    return 0;
}

intptr_t __mg_do_zorder_maskrect_operation (int cli,
        const ZORDERMASKRECTOPINFO* info)
{
    intptr_t ret = -1;
    
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
    return ret;
}

intptr_t __mg_do_zorder_operation (int cli, const ZORDEROPINFO* info)
{
    intptr_t ret = -1;
    
    switch (info->id_op) {
        case ID_ZOOP_ALLOC:
            ret = srvAllocZOrderNode (cli, info->hwnd, info->main_win, 
                            info->flags, &info->rc, info->caption);
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
            ret = srvMoveWindow (cli, info->idx_znode, &info->rc);
            break;
        case ID_ZOOP_SETACTIVE:
            ret = srvSetActiveWindow (cli, info->idx_znode);
            break;
#ifdef _MGHAVE_MENU
        case ID_ZOOP_START_TRACKMENU:
            ret = srvStartTrackPopupMenu (cli, &info->rc, info->hwnd);
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
            srvChangeCaption (cli, info->idx_znode, info->caption);
            break;
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

static BOOL _cb_intersect_rc_no_cli (void* context, 
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    ZORDERNODE* del_node = (ZORDERNODE*)context;

    if (node->cli != del_node->cli && 
                    node->flags & ZOF_VISIBLE && 
                    //SubtractClipRect (&sg_UpdateRgn, &node->rc)) {
                    subtract_rgn_by_node(&sg_UpdateRgn, zi, node)) {
        node->age ++;
        node->flags |= ZOF_REFERENCE;
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
        node->flags |= ZOF_REFERENCE;
        return TRUE;
    }

    return FALSE;
}

int __mg_remove_all_znodes_of_client (int cli)
{
    ZORDERINFO* zi = _get_zorder_info(cli);
    ZORDERNODE* nodes;
    int slot, slot2, old_active;
    RECT rc_bound = {0, 0, 0, 0};

    nodes = GET_ZORDERNODE(zi);
    
    if (zi->cli_trackmenu == cli) {
        int i;
        ZORDERNODE* menu_nodes;

        menu_nodes = GET_MENUNODE(zi);

        /* lock zi for change */
        lock_zi_for_change (zi);

        for (i = 0; i < zi->nr_popupmenus; i++) {
            GetBoundRect (&rc_bound, &rc_bound, &menu_nodes [i].rc);
        }

        SetClipRgn (&sg_UpdateRgn, &rc_bound);

        /* check influenced window zorder nodes */
        do_for_all_znodes ((void*)(intptr_t)cli, zi, _cb_update_rc_nocli, ZT_ALL);

        if (SubtractClipRect (&sg_UpdateRgn, &g_rcScr)) {
            nodes [0].age ++;
            nodes [0].flags |= ZOF_REFERENCE;
        }

        zi->cli_trackmenu = -1;
        zi->nr_popupmenus = 0;

        /* unlock zi for change */
        unlock_zi_for_change (zi);
    }

    /* lock zi for change */
    lock_zi_for_change (zi);

    /* handle topmosts */
    slot = zi->first_topmost;
    for (; slot > 0; slot = nodes [slot].next) {
        if (nodes [slot].cli == cli) {
            if (nodes [slot].flags & ZOF_VISIBLE) {
                SetClipRgn (&sg_UpdateRgn, &nodes [slot].rc);
                GetBoundRect (&rc_bound, &rc_bound, &nodes [slot].rc);

                slot2 = nodes [slot].next;
                for (; slot2 > 0; slot2 = nodes [slot2].next) {
                    if (nodes [slot2].cli != cli &&
                        nodes [slot2].flags & ZOF_VISIBLE && 
                        //SubtractClipRect (&sg_UpdateRgn, &nodes [slot2].rc)) {
                        subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes [slot2])) {

                        nodes [slot2].age ++;
                        nodes [slot2].flags |= ZOF_REFERENCE;
                    }
                }

                do_for_all_znodes (nodes + slot, zi, 
                                _cb_intersect_rc_no_cli, ZT_NORMAL);

                if (!(nodes [0].flags & ZOF_REFERENCE) &&
                                SubtractClipRect (&sg_UpdateRgn, &g_rcScr)) {
                    nodes [0].age ++;
                    nodes [0].flags |= ZOF_REFERENCE;
                }
            }

            unchain_znode ((unsigned char *)(zi+1), nodes, slot);
            clean_znode_maskrect (zi, nodes, slot);

            if (zi->first_topmost == slot) {
                zi->first_topmost = nodes [slot].next;
            }
            zi->nr_topmosts --;
        }
    }

    /* handle normals */
    slot = zi->first_normal;
    for (; slot > 0; slot = nodes [slot].next) {
        if (nodes [slot].cli == cli) {
            if (nodes [slot].flags & ZOF_VISIBLE) {
                SetClipRgn (&sg_UpdateRgn, &nodes [slot].rc);
                GetBoundRect (&rc_bound, &rc_bound, &nodes [slot].rc);

                slot2 = nodes [slot].next;
                for (; slot2 > 0; slot2 = nodes [slot2].next) {
                    if (nodes [slot2].cli != cli &&
                        nodes [slot2].flags & ZOF_VISIBLE && 
                        //SubtractClipRect (&sg_UpdateRgn, &nodes [slot2].rc)) {
                        subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes [slot2])) {

                        nodes [slot2].age ++;
                        nodes [slot2].flags |= ZOF_REFERENCE;
                    }
                }
                if (!(nodes [0].flags & ZOF_REFERENCE) &&
                        SubtractClipRect (&sg_UpdateRgn, &g_rcScr)) {
                    nodes [0].age ++;
                    nodes [0].flags |= ZOF_REFERENCE;
                }
            }

            unchain_znode ((unsigned char *)(zi+1), nodes, slot);
            clean_znode_maskrect (zi, nodes, slot);

            if (zi->first_normal == slot) {
                zi->first_normal = nodes [slot].next;
            }
            zi->nr_normals --;
        }
    }

    old_active = zi->active_win;
    if (nodes [old_active].cli == cli)
        zi->active_win = 0; /* set the active_win to desktop temp */

    /* unlock zi for change  */
    unlock_zi_for_change (zi);

    /* update all znode if it's dirty */
    do_for_all_znodes (&rc_bound, zi, _cb_update_znode, ZT_ALL);

    if (nodes [0].flags & ZOF_REFERENCE) {
        SendMessage (HWND_DESKTOP, 
                        MSG_ERASEDESKTOP, 0, (WPARAM)&rc_bound);
        nodes [0].flags &= ~ZOF_REFERENCE;
    }

    /* if active_win belongs to the client, change it */
    if (nodes [old_active].cli == cli) {
        int next_active = get_next_visible_mainwin (zi, 0);
        srvSetActiveWindow (nodes [next_active].cli, next_active);
    }

    return 0;
}

int __mg_do_change_topmost_layer (void)
{
    ZORDERINFO* old_zorder_info = __mg_zorder_info;
    ZORDERNODE *new_nodes, *old_nodes;
    unsigned char *old_use_bmp, *new_use_bmp;
    int i;

#ifdef _MGHAVE_MENU
    srvForceCloseMenu (0);
#endif

    old_use_bmp = (unsigned char*)old_zorder_info + sizeof (ZORDERINFO);

    old_nodes = GET_ZORDERNODE(old_zorder_info);

    __mg_zorder_info = mgTopmostLayer->zorder_info;

    lock_zi_for_change (__mg_zorder_info);
    __mg_zorder_info->nr_globals = old_zorder_info->nr_globals;
    __mg_zorder_info->first_global = old_zorder_info->first_global;

    if (old_zorder_info->active_win < __mg_zorder_info->nr_globals) {
        __mg_zorder_info->active_win = old_zorder_info->active_win;
    }

    new_use_bmp = (unsigned char*)__mg_zorder_info + sizeof (ZORDERINFO);

    new_nodes = GET_ZORDERNODE(__mg_zorder_info);

    memcpy (new_use_bmp, old_use_bmp, old_zorder_info->max_nr_globals/8);

    memcpy (new_nodes, old_nodes, 
                    (old_zorder_info->max_nr_globals)*sizeof(ZORDERNODE));

    for (i = old_zorder_info->max_nr_globals;
            i <= old_zorder_info->max_nr_globals + 
            old_zorder_info->max_nr_topmosts + 
            old_zorder_info->max_nr_normals; i++)
    {
        new_nodes [i].age = old_nodes [i].age + 1;
    }

    new_nodes [0].age = old_nodes [0].age + 1;
    unlock_zi_for_change (__mg_zorder_info);
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
            active = nodes [__mg_zorder_info->active_win].fortestinghwnd;
        else
            active = HWND_OTHERPROC;

    }

    unlock_zi_for_read (__mg_zorder_info);

    if (cli) *cli = active_cli;

    return active;
}

/*
 * Sets the active main window.
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
static int dskAddNewMainWindow (PMAINWIN pWin)
{
    RECT rcWin;

    if (mgIsServer) {
        memcpy (&rcWin, &pWin->left, sizeof (RECT));
        pWin->idx_znode = srvAllocZOrderNode (0, (HWND)pWin, (HWND)pWin->pMainWin,
                                              get_znode_flags_from_style (pWin), 
                                              &rcWin, pWin->spCaption);
    }
    else
        pWin->idx_znode = cliAllocZOrderNode (pWin);

    if (pWin->idx_znode <= 0)
        return -1;

    /* Handle main window hosting. */
    if (pWin->pHosting)
        dskAddNewHostedMainWindow (pWin->pHosting, pWin);

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
        if (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC)
            pWin->privCDC = GetSecondarySubDC (pWin->secondaryDC, (HWND)pWin, TRUE);
        else
            pWin->privCDC = CreatePrivateClientDC ((HWND)pWin);
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
        cliFreeZOrderMaskRect(pWin);
        cliFreeZOrderNode (pWin);
    }

    /* Handle main window hosting. */
    if (pWin->pHosting)
        dskRemoveHostedMainWindow (pWin->pHosting, pWin);

    if ((pWin->dwExStyle & WS_EX_AUTOSECONDARYDC) && pWin->secondaryDC) {
        DeleteSecondaryDC ((HWND)pWin);
        pWin->update_secdc = NULL;
    }

    if (pWin->privCDC) {
        if (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC) {
            ReleaseSecondarySubDC (pWin->privCDC);
        }
        else {
            if (pWin->secondaryDC)
                ReleaseSecondarySubDC (pWin->privCDC);
            else
                DeletePrivateDC (pWin->privCDC);
        }
        pWin->privCDC = 0;
    }
}

static void dskHideGlobalControl (PMAINWIN pWin, int reason, LPARAM lParam)
{
    int first = 0;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);
    
    lock_zi_for_read (__mg_zorder_info);
    switch (nodes [pWin->idx_znode].flags & ZOF_TYPE_MASK) {
        case ZOF_TYPE_GLOBAL:
            first = __mg_zorder_info->first_global;
            break;
        case ZOF_TYPE_TOPMOST:
            first = __mg_zorder_info->first_topmost;
            break;
        case ZOF_TYPE_NORMAL:
            first = __mg_zorder_info->first_normal;
            break;
        default:
            break;
    }
    unlock_zi_for_read (__mg_zorder_info);

    if (first > 0 && !(nodes [first].flags & ZOF_TF_MAINWIN)
                    && (nodes [first].flags & ZOF_VISIBLE)) {

        if (nodes [first].cli == __mg_client_id) {
            RECT rc = nodes [first].rc;
            PMAINWIN pCurTop = (PMAINWIN) nodes [first].fortestinghwnd;

            pCurTop->dwStyle &= ~WS_VISIBLE;
            cliHideWindow (pCurTop);
            dskSetPrimitiveChildren (pCurTop, FALSE);
            SendNotifyMessage (pCurTop->hParent, 
                            MSG_CHILDHIDDEN, reason, lParam);

            dskScreenToClient (pCurTop->pMainWin, &rc, &rc);
            InvalidateRect ((HWND)pCurTop->pMainWin, &rc, TRUE);
        }
    }
}

static void dskMoveToTopMost (PMAINWIN pWin, int reason, LPARAM lParam)
{
    if (!pWin) return;

    if (dskIsTopMost (pWin) && (pWin->dwStyle & WS_VISIBLE)) {
        return;
    }

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

static void dskMoveGlobalControl (PMAINWIN pCtrl, RECT* prcExpect)
{
    RECT newWinRect, rcResult;

    SendAsyncMessage ((HWND)pCtrl, MSG_CHANGESIZE, 
                    (WPARAM)(prcExpect), (LPARAM)(&rcResult));
    dskClientToScreen ((PMAINWIN)(pCtrl->hParent), &rcResult, &newWinRect);

    if (mgIsServer)
        srvMoveWindow (0, pCtrl->idx_znode, &newWinRect);
    else
        cliMoveWindow (pCtrl, &newWinRect);

    if (pCtrl->dwStyle & WS_VISIBLE) {
        SendAsyncMessage ((HWND)pCtrl, MSG_NCPAINT, 0, 0);
        InvalidateRect ((HWND)pCtrl, NULL, TRUE);
    }
}

static void dskMoveMainWindow (PMAINWIN pWin, RECT* prcExpect)
{
    RECT oldWinRect, rcResult;

    memcpy (&oldWinRect, &pWin->left, sizeof (RECT));
    SendAsyncMessage ((HWND)pWin, MSG_CHANGESIZE, 
                    (WPARAM)(prcExpect), (LPARAM)(&rcResult));

    if (mgIsServer)
        srvMoveWindow (0, pWin->idx_znode, &rcResult);
    else
        cliMoveWindow (pWin, &rcResult);
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

    PopupMenuTrackProc (ptmi, MSG_INITMENU, 0, 0);

    if (mgIsServer)
        ptmi->idx_znode = srvStartTrackPopupMenu (0, &ptmi->rc, (HWND)ptmi);
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
            SelectClipRect (HDC_SCREEN_SYS, &rc);
            PopupMenuTrackProc (ptmi, MSG_SHOWMENU, 0, 0);
        }
        ptmi = ptmi->next;
    }
    SelectClipRect (HDC_SCREEN_SYS, &g_rcScr);

    return 0;
}

#endif

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
            if (__mg_capture_wnd && 
                gui_GetMainWindowPtrOfControl (__mg_capture_wnd) == pWin) 
                __mg_capture_wnd = 0;

            if (dskGetActiveWindow (NULL) == (HWND)pWin) {
                dskSetActiveWindow (NULL);
                return;
            }
        }

        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
    }
}

#if 0
static int dskScrollMainWindow (PMAINWIN pWin, PSCROLLWINDOWINFO pswi)
{
    HDC hdc;
    RECT rcClient, rcScreen, rcInvalid;
    BOOL inved = FALSE;
    PCLIPRECT pcrc;
    PINVRGN pInvRgn;

    if (!(pWin->dwStyle & WS_VISIBLE))
        return 0;

    lock_zi_for_read (__mg_zorder_info);

    dskClientToScreen (pWin, pswi->rc1, &rcScreen);

    //BUGFIX: if the MainWindow is AutoSecondaryDC, the secondaryDC and
    //client dc would be diffirent, so we must get the scondaryDC,
    //the update to client dc (dongjunjie 2010/7/28)
    hdc = get_valid_dc(pWin, TRUE);

    pcrc = kernel_GetGCRgnInfo ((HWND)pWin)->crgn.head;
    while (pcrc) {
        RECT rcMove;

        if (!IntersectRect (&rcMove, &pcrc->rc, &rcScreen)) {
            pcrc = pcrc->next;
            continue;
        }

        dskScreenToClient (pWin, &rcMove, &rcMove);
        if (!IntersectRect (&rcMove, &rcMove, pswi->rc1)) {
            pcrc = pcrc->next;
            continue;
        }

        if (pWin->dwExStyle & WS_EX_TRANSPARENT) {
            /* set invalidate rect. */
            InvalidateRect ((HWND)pWin, &rcMove, TRUE);
            inved = TRUE;
        }
        else {
            dskRecalcRect (pWin, &rcMove);

            SelectClipRect (hdc, &rcMove);

            BitBlt (hdc, rcMove.left, rcMove.top, 
                    rcMove.right - rcMove.left,
                    rcMove.bottom - rcMove.top,
                    hdc, pswi->iOffx + rcMove.left, pswi->iOffy + rcMove.top, 0);
        }
        pcrc = pcrc->next;
    }
    //BUGFIXED: we must update the secondaryDC to clientDC, to ensure 
    //the secondaryDC and clientDC are same (dongjunjie 2010/07/08)
    if(pWin->pMainWin->secondaryDC){
        HDC real_dc = GetClientDC((HWND)pWin->pMainWin);
        update_secondary_dc(pWin, hdc, real_dc, pswi->rc1, HT_CLIENT);
        ReleaseDC (real_dc);
    }
    release_valid_dc(pWin, hdc);

    GetClientRect((HWND)pWin, &rcClient);

    pInvRgn = &pWin->InvRgn;
    if (!pInvRgn->frozen) {
#ifdef _MGRM_THREADS
        pthread_mutex_lock (&pInvRgn->lock);
#endif
        /*scroll whole screen, offset invalid region*/
        if (EqualRect (pswi->rc1, &rcClient)) 
            OffsetRegion (&(pInvRgn->rgn), pswi->iOffx, pswi->iOffy);
        else
            OffsetRegionEx (&(pInvRgn->rgn), &rcClient,
                pswi->rc1, pswi->iOffx, pswi->iOffy);
#ifdef _MGRM_THREADS
        pthread_mutex_unlock (&pInvRgn->lock);
#endif
    }

    pcrc = kernel_GetGCRgnInfo ((HWND)pWin)->crgn.head;
    while (pcrc) {
        BOOL bNeedInvalidate = FALSE;
        RECT rcMove;
        if (!IntersectRect (&rcMove, &pcrc->rc, &rcScreen)) {
            pcrc = pcrc->next;
            continue;
        }

        dskScreenToClient (pWin, &rcMove, &rcMove);

        dskRecalcRect (pWin, &rcMove);

        rcInvalid = rcMove;

        if (pswi->iOffx < 0) {
            rcInvalid.left = rcInvalid.right + pswi->iOffx;
            InvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            inved = TRUE;
        }
        else if (pswi->iOffx > 0) {
            rcInvalid.right = rcInvalid.left + pswi->iOffx;
            InvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            inved = TRUE;
        }
        
        /*
         * BUGFIXED: offx and offy would make the two diffrent areas invalidate
         * we should invalid both them (dongjunjie) 2010/07/30
         *  
         *                   content
         *  ---------------------------
         *  |//: offX                 |
         *  |//:                      |
         *  |//:       View           |
         *  |//:                      |
         *  |//:                      |
         *  |/////////////////////////| offY
         *  ---------------------------
         *  // - represent area need the Need invalidate
         *  see the to area must be invalidate
         */
        if(bNeedInvalidate)
        {
			InvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            rcInvalid = rcMove; //restore the invalidate area
            bNeedInvalidate = FALSE; //resotre the inved value
            inved = TRUE;
        }

        if (pswi->iOffy < 0) {
            rcInvalid.top = rcInvalid.bottom + pswi->iOffy;
            bNeedInvalidate = TRUE;
        }
        else if (pswi->iOffy > 0) {
            rcInvalid.bottom = rcInvalid.top + pswi->iOffy;
            bNeedInvalidate = TRUE;
        }

        if (bNeedInvalidate)
        {
			InvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            inved = TRUE;
        }

        pcrc = pcrc->next;
    }

    unlock_zi_for_read (__mg_zorder_info);

    if (inved)
        PostMessage ((HWND)pWin, MSG_PAINT, 0, 0);

    return 0;
}
#endif
static HWND dskGetCaptureWindow (void)
{
    return __mg_capture_wnd;
}

static HWND dskSetCaptureWindow (PMAINWIN pWin)
{
    HWND old;

    old = __mg_capture_wnd;
    __mg_capture_wnd = (HWND)pWin;

    return old;
}

static HWND dskGetNextMainWindow (PMAINWIN pWin)
{
    HWND hWnd = HWND_NULL;
    int slot;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);
    int last_type;

    if (pWin) {
        last_type = nodes[pWin->idx_znode].flags & ZOF_TYPE_MASK;
        slot = nodes[pWin->idx_znode].next;
    }else{
        last_type = ZOF_TYPE_GLOBAL;
        slot = __mg_zorder_info->first_global;
    }

    while (1) {
        if (slot <= 0) {
            if (last_type == ZOF_TYPE_GLOBAL) {
                last_type = ZOF_TYPE_TOPMOST;
                slot = __mg_zorder_info->first_topmost;
                continue;
            }else if (last_type == ZOF_TYPE_TOPMOST) {
                last_type = ZOF_TYPE_NORMAL;
                slot = __mg_zorder_info->first_normal;
                continue;
            }else{
                return HWND_NULL;
            }
        }

        hWnd = nodes[slot].fortestinghwnd;

        if (0
                || !hWnd
                || (nodes[slot].cli != __mg_client_id)
                || !(pWin = gui_CheckAndGetMainWindowPtr (hWnd))
                || (pWin->dwExStyle & WS_EX_CTRLASMAINWIN)) {
            slot = nodes[slot].next;
            continue;
        }else{
            return hWnd;
        }
    }
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

static LRESULT dskWindowMessageHandler (UINT message, PMAINWIN pWin, LPARAM lParam)
{
    switch (message) {
        case MSG_ADDNEWMAINWIN:
            return dskAddNewMainWindow (pWin);

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
        
        case MSG_SETWINCURSOR:
            {
                HCURSOR old = pWin->hCursor;

                pWin->hCursor = (HCURSOR)lParam;
                return (LRESULT)old;
            }

        case MSG_GETNEXTMAINWIN:
            return (LRESULT)dskGetNextMainWindow (pWin);

        case MSG_SHOWGLOBALCTRL:
            {
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
   }

   return 0;
}

/*********************** Hook support ****************************************/
typedef struct HookInfo
{
    int cli;
    HWND hwnd;
    DWORD flag;
} HOOKINFO;

static HOOKINFO keyhook = {0, HWND_NULL, 0};
static HOOKINFO mousehook = {0, HWND_NULL, 0};

HWND __mg_do_reghook_operation (int cli, const REGHOOKINFO* info)
{
    HWND ret = HWND_NULL;

    switch (info->id_op) {
        case ID_REG_KEY:
            ret = keyhook.hwnd;
            keyhook.cli = cli;
            keyhook.hwnd = info->hwnd;
            keyhook.flag = info->flag;
            break;

        case ID_REG_MOUSE:
            ret = mousehook.hwnd;
            mousehook.cli = cli;
            mousehook.hwnd = info->hwnd;
            mousehook.flag = info->flag;
            break;

        default:
            break;
    }
    return ret;
}

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

/* used by client to check the validation of a hwnd */
BOOL __mg_client_check_hwnd (HWND hwnd, int cli)
{
    ZORDERINFO* zi = __mg_zorder_info;
    ZORDERNODE* nodes;
    int slot;

    nodes = GET_ZORDERNODE(zi);

    lock_zi_for_read (zi);

    slot = zi->first_topmost;
    for (; slot > 0; slot = nodes [slot].next) {
        if (hwnd == nodes [slot].fortestinghwnd && cli == nodes [slot].cli) {
            goto ret_true;
        }
    }

    slot = zi->first_normal;
    for (; slot > 0; slot = nodes [slot].next) {
        if (hwnd == nodes [slot].fortestinghwnd && cli == nodes [slot].cli) {
            goto ret_true;
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

                next_node = get_next_visible_mainwin (__mg_zorder_info,
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

    if (srvHandleKeyHook (message, 
                            (WPARAM)scancode, (LPARAM)status) == HOOK_STOP)
        return 0;

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

    if (hwnd) *hwnd = nodes [slot].fortestinghwnd;
    return nodes [slot].cli;
}

int __mg_handle_normal_mouse_move (const ZORDERINFO* zi, int x, int y)
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
                    nodes [old_slot].fortestinghwnd == old_hwnd) {
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
        old_hwnd = nodes [cur_slot].fortestinghwnd;
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

    if ((HWND)pWin == __mg_capture_wnd)
        __mg_capture_wnd = 0;

    __mg_reset_mainwin_capture_info ((PCONTROL)pWin);
}

static int check_capture (int message)
{
    if (mgs_captured_main_win != (void*)HWND_INVALID
                    && mgs_captured_main_win != NULL) {

        switch (message) {
            case MSG_LBUTTONDOWN:
            case MSG_RBUTTONDOWN:
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
        }
    }

    return 0;
}

static int dskMouseMessageHandler (int message, WPARAM flags, int x, int y)
{
    PMAINWIN pUnderPointer;
    PMAINWIN pCtrlPtrIn;
    int CapHitCode = HT_UNKNOWN;
    int UndHitCode = HT_UNKNOWN;
    int cx = 0, cy = 0;

    if (__mg_capture_wnd) {
        PostMessage (__mg_capture_wnd, 
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
                    PostMessage((HWND)pUnderPointer, message, 
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
            if (pUnderPointer)
            {
                if (pUnderPointer->dwStyle & WS_DISABLED) {
                    Ping ();
                    break;
                }

                if(UndHitCode == HT_CLIENT)
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
                            &rcWin, pNewCtrl->spCaption);
        else
            pNewCtrl->idx_znode = cliAllocZOrderNode ((PMAINWIN)pNewCtrl);

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

    if ((HWND)pCtrl == __mg_capture_wnd)
        /* force release the capture */
        __mg_capture_wnd = 0;

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

    if (node->flags & ZOF_TF_MAINWIN
            && !(node->flags & ZOF_TF_TOPFOREVER)
       ) {
        if (node->flags & ZOF_VISIBLE)
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
#endif

static BOOL _cb_close_mainwin (void* context, 
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    if (node->flags & ZOF_TF_MAINWIN) {
        post_msg_by_znode_p (zi, node, MSG_CLOSE, 0, 0);
    }
    return TRUE;
}

static int nr_of_all_znodes (void)
{
    MG_Layer* layer = mgLayers;
    int count = 0;
    ZORDERINFO* zi;

    while (layer) {
        zi = layer->zorder_info;
        count += zi->nr_topmosts;
        count += zi->nr_normals;

        layer = layer->next;
    }

    if (__mg_zorder_info)
        count += (__mg_zorder_info->nr_globals - 1);

    return count;
}

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
        if ( SERVER_HAS_NO_MAINWINDOW() ){
            ExitGUISafely (-1);
        }
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
#endif

    return 0;
}


static int srvRegisterIMEWnd (HWND hwnd)
{
    if (!mgIsServer)
        return ERR_INV_HWND;

    if (__mg_ime_wnd != 0)
        return ERR_IME_TOOMUCHIMEWND;

    if (!gui_CheckAndGetMainWindowPtr (hwnd))
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
        pTemp = (PMAINWIN)node->fortestinghwnd;

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

/***********************************************************
 * Handler for MSG_TIMER for the server and all clients.
 **********************************************************/

static void dskOnTimer (void)
{
    static UINT uCounter = 0;
    static UINT blink_counter = 0;
    static UINT sg_old_counter = 0;

    if (sg_old_counter == 0)
        sg_old_counter = SHAREDRES_TIMER_COUNTER;

    mg_dispatch_timer_message (SHAREDRES_TIMER_COUNTER - sg_old_counter);
    sg_old_counter = SHAREDRES_TIMER_COUNTER;

    if (SHAREDRES_TIMER_COUNTER < (blink_counter + 10))
        return;

    uCounter += (SHAREDRES_TIMER_COUNTER - blink_counter) * 10;
    blink_counter = SHAREDRES_TIMER_COUNTER;

    if (sg_hCaretWnd != 0
            && (HWND)gui_GetMainWindowPtrOfControl (sg_hCaretWnd)
                    == dskGetActiveWindow (NULL) 
            && uCounter >= sg_uCaretBTime) {
        PostMessage (sg_hCaretWnd, MSG_CARETBLINK, 0, 0);
        uCounter = 0;
    }
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
            hwndActive = nodes [__mg_zorder_info->active_win].fortestinghwnd;
            rcActive = nodes [__mg_zorder_info->active_win].rc;
        }
    }

    if (cliActive == -1) {
        cliActive = 0;
        hwndActive = 0;
        rcActive = g_rcScr;
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
    int nCount, count, iPos;
    MG_Layer* layer;

    info.menu = GetSubMenu (sg_DesktopMenu, 3);

    nCount = GetMenuItemCount (info.menu);
    for (iPos = nCount; iPos > 0; iPos --)
        DeleteMenu (info.menu, iPos - 1, MF_BYPOSITION);
    
    memset (&info.mii, 0, sizeof (MENUITEMINFO));
    info.mii.type = MFT_STRING;
    info.id = IDM_FIRSTWINDOW;
    info.pos = 0;
    
    count = do_for_all_znodes (&info, 
                    __mg_zorder_info, _cb_update_dskmenu, ZT_GLOBAL);

    if (count) {
        info.mii.type            = MFT_SEPARATOR;
        info.mii.state           = 0;
        info.mii.id              = 0;
        info.mii.typedata        = 0;
        InsertMenuItem (info.menu, info.pos, TRUE, &info.mii);

        info.pos ++;
    }

    info.mii.type = MFT_STRING;
    count = do_for_all_znodes (&info, 
                    __mg_zorder_info, _cb_update_dskmenu, ZT_TOPMOST);

    if (count) {
        info.mii.type            = MFT_SEPARATOR;
        info.mii.state           = 0;
        info.mii.id              = 0;
        info.mii.typedata        = 0;
        InsertMenuItem (info.menu, info.pos, TRUE, &info.mii);

        info.pos ++;
        info.mii.type = MFT_STRING;
    }

    info.mii.type = MFT_STRING;
    count = do_for_all_znodes (&info, 
                    __mg_zorder_info, _cb_update_dskmenu, ZT_NORMAL);

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
#endif

static int srvSesseionMessageHandler (int message, WPARAM wParam, LPARAM lParam)
{
    static HDC hDesktopDC;

    switch (message) {
        case MSG_STARTSESSION:
            __mg_init_local_sys_text ();
            hDesktopDC = GetDC (HWND_DESKTOP);

            if(dsk_ops->init)
                dt_context = dsk_ops->init();
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

        case MSG_ENDSESSION:
            if (SERVER_HAS_NO_MAINWINDOW()) {
                screensaver_destroy();

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

            if(dsk_ops->paint_desktop)
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
                srvUpdateDesktopMenu ();
#endif
        case MSG_DT_LBUTTONDOWN:
        case MSG_DT_LBUTTONUP:
        case MSG_DT_LBUTTONDBLCLK:
        case MSG_DT_MOUSEMOVE:
        case MSG_DT_RBUTTONDOWN:
        case MSG_DT_RBUTTONDBLCLK:
            if(dsk_ops->mouse_handler)
                dsk_ops->mouse_handler(dt_context, message, wParam, lParam);        
        break;
    }

    return 0;
}


LRESULT DesktopWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int flags, x, y;

    if (message >= MSG_FIRSTWINDOWMSG && message <= MSG_LASTWINDOWMSG) {
        return dskWindowMessageHandler (message, (PMAINWIN)wParam, lParam);
    }
    else if (message >= MSG_FIRSTKEYMSG && message <= MSG_LASTKEYMSG) {
        if (mgIsServer) {
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

                MSG msg = {0, message, wParam, lParam, __mg_timer_counter};

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
        flags = (int)wParam;

        x = LOSWORD (lParam);
        y = HISWORD (lParam);

        if (mgIsServer) {
            if (__mg_zorder_info->cli_trackmenu > 0 ) {

                MSG msg = {0, message, wParam, lParam, __mg_timer_counter};

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
        else if (sg_ptmi)
            PopupMenuTrackProc (sg_ptmi, message, x, y);
#endif
        else
            dskMouseMessageHandler (message, flags, x, y);

        return 0;
    }
    else if (message == MSG_COMMAND) {
        if (wParam <= MAXID_RESERVED && wParam >= MINID_RESERVED)
        {
            return srvDesktopCommand ((int)wParam);
        }
        else
        {
            if(dsk_ops->desktop_menucmd_handler)
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
        case MSG_TIMEOUT:
            {
                MSG msg = {0, MSG_IDLE, wParam, 0};
                dskBroadcastMessage (&msg);
                break;
            }

        case MSG_SRVNOTIFY:
            {
                MSG msg = {0, MSG_SRVNOTIFY, wParam, lParam};
                dskBroadcastMessage (&msg);
                break;
            }

        case MSG_PAINT:
            {
                RECT invrc;

                invrc.left = LOWORD(wParam);
                invrc.top = HIWORD(wParam);
                invrc.right = LOWORD(lParam);
                invrc.bottom = HIWORD(lParam);

                dskRefreshAllWindow (&invrc);
                break;
            }

        case MSG_REGISTERWNDCLASS:
            return AddNewControlClass ((PWNDCLASS)lParam);

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
            if (__mg_quiting_stage < 0) {
                if (__mg_quiting_stage > _MG_QUITING_STAGE_FORCE && __mg_quiting_stage <= _MG_QUITING_STAGE_START) {
                    -- __mg_quiting_stage;
                    /* printf("try to quit %d\n", __mg_quiting_stage); */
                }else if (__mg_quiting_stage <= _MG_QUITING_STAGE_FORCE) {
                    /* printf("force to quit !!!\n"); */
                }

                if (__mg_quiting_stage > _MG_QUITING_STAGE_DESKTOP
                        && (mgIsServer ? SERVER_HAS_NO_MAINWINDOW() : CLIENT_HAS_NO_MAINWINDOW())) {
                    __mg_quiting_stage = _MG_QUITING_STAGE_DESKTOP;
                }else if (__mg_quiting_stage <= _MG_QUITING_STAGE_DESKTOP) {
                    PostMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
                }
            }

            dskOnTimer ();
            break;
    }

    return 0;
}

#endif
