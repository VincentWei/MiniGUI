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
** desktop-comm.c: The Desktop module.
**
** Current maintainer: Wei Yongming.
**
*/

/* only for MiniGUI-Threads or MiniGUI-Standalone */
#include "desktop.c"

#ifdef _MGRM_THREADS
extern int __mg_enter_terminategui;
#endif

#if defined(_MGRM_THREADS) || defined(_MGRM_STANDALONE)

int kernel_change_z_order_mask_rect (HWND pWin, const RECT4MASK* rc, int nr_rc)
{
    FreeZOrderMaskRect (0, ((PMAINWIN)pWin)->idx_znode);
    return AllocZOrderMaskRect (0, ((PMAINWIN)pWin)->idx_znode,
            get_znode_flags_from_style ((PMAINWIN)pWin), rc, nr_rc);
}

static void init_desktop_win (void)
{
    static MAINWIN sg_desktop_win;
    PMAINWIN pDesktopWin;

    LICENSE_SET_MESSAGE_OFFSET();

    pDesktopWin = &sg_desktop_win;

    pDesktopWin->pMessages         = __mg_dsk_msg_queue;
    pDesktopWin->MainWindowProc    = DesktopWinProc;

    pDesktopWin->DataType          = TYPE_HWND;
    pDesktopWin->WinType           = TYPE_ROOTWIN;
#ifdef _MGRM_THREADS
    pDesktopWin->th                = __mg_desktop;
#endif

    pDesktopWin->pLogFont          = GetSystemFont (SYSLOGFONT_WCHAR_DEF);
    pDesktopWin->spCaption         = "THE DESKTOP WINDOW";

    pDesktopWin->pGCRInfo          = &sg_ScrGCRInfo;
    pDesktopWin->idx_znode         = 0;

    pDesktopWin->pMainWin          = pDesktopWin;
    pDesktopWin->we_rdr            = __mg_def_renderer; 

    __mg_hwnd_desktop = (HWND)pDesktopWin;
    __mg_dsk_win  = pDesktopWin;
}

void mg_TerminateDesktop (void)
{
    if (__mg_dsk_msg_queue) {
        mg_DestroyMsgQueue (__mg_dsk_msg_queue);
#ifdef _MGRM_THREADS
        free (__mg_dsk_msg_queue);
        __mg_dsk_msg_queue = NULL;
#endif
    }

#ifdef _MGRM_THREADS
    pthread_mutex_destroy(&sg_ScrGCRInfo.lock);
#endif

    kernel_free_z_order_info (__mg_zorder_info);
    DestroyFreeClipRectList (&sg_FreeClipRectList);
    DestroyFreeClipRectList (&sg_FreeInvRectList);

    mg_TerminateSystemRes ();
	//dongjunjie avoid double free
	__mg_dsk_win = 0;
}

static PMAINWIN dskGetActiveWindow (void)
{
    ZORDERNODE* nodes;

    nodes = GET_ZORDERNODE(__mg_zorder_info);

    return (PMAINWIN)nodes[__mg_zorder_info->active_win].fortestinghwnd;
}

static PMAINWIN dskSetActiveWindow (PMAINWIN pWin)
{
    PMAINWIN pOldActive = dskGetActiveWindow();

    if (pWin == pOldActive) 
        return pOldActive;

    pOldActive = (PMAINWIN) dskSetActiveZOrderNode (0, pWin?pWin->idx_znode:0);

#ifndef _MGRM_PROCESSES /*for MiniGUI-Threads, not for MiniGUI-StandAlone*/
    if (pOldActive != (PMAINWIN)HWND_INVALID && __mg_ime_wnd)
        SendNotifyMessage (__mg_ime_wnd, MSG_IME_SETTARGET, (WPARAM)pWin, 0);
#endif

    return pOldActive;
}

/*
 * Shows/Activates a main window.
 *  This function called when a window was shown.
 */
static void dskShowMainWindow (PMAINWIN pWin, BOOL bActive)
{
    if (pWin->dwStyle & WS_VISIBLE)
        return;

    //dskUpdateGCRInfoOnShowMainWin (pWin);

    pWin->dwStyle |= WS_VISIBLE;

    dskShowWindow (0, pWin->idx_znode);

    SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);

    InvalidateRect ((HWND)pWin, NULL, TRUE);

#if 0
    if (pWin->dwExStyle & WS_EX_TOPMOST)
        return dskSetActiveWindow (pWin);

    // if the showing window cover the current active window
    // set this window as the active one. 
    //if (bActive && dskDoesCoverOther (pWin, dskGetActiveWindow()))
#endif
    if (bActive)
        dskSetActiveWindow (pWin); 

    return;
}


static int dskAllocZOrderNode (PMAINWIN pWin)
{
    RECT rc;

    dskGetWindowRectInScreen (pWin, &rc);

    return AllocZOrderNode (0, (HWND)pWin, 
                    (HWND)pWin->pMainWin, 
                    get_znode_flags_from_style (pWin), 
                    &rc, pWin->spCaption);
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
    pWin->pGCRInfo = &pWin->GCRInfo;

    // Update Z Order info.
    pWin->idx_znode = dskAllocZOrderNode (pWin);

    if (pWin->idx_znode <= 0) {
        _MG_PRINTF ("KERNEL>Desktop: Alloc zorder node for main window fail.\n");
        return -1;
    }

    // Handle main window hosting.
    if (pWin->pHosting)
        dskAddNewHostedMainWindow (pWin->pHosting, pWin);
    
    // Init Global Clip Region info.
    dskInitGCRInfo (pWin);

    // Init Invalid Region info.
    dskInitInvRgn (pWin);

    /* houhh 20081128, create secondary window dc before 
     * InvalidateRect, erase_bk will used this. */
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

    // show and active this main window.
    if ( pWin->dwStyle & WS_VISIBLE ) {

        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);

        SendNotifyMessage ((HWND)pWin, MSG_SHOWWINDOW, SW_SHOWNORMAL, 0);

        InvalidateRect ((HWND)pWin, NULL, TRUE);

        dskSetActiveWindow (pWin);
    }

    return 0;
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

        RECT rc = nodes [first].rc;
        PMAINWIN pCurTop = (PMAINWIN) nodes [first].fortestinghwnd;

        pCurTop->dwStyle &= ~WS_VISIBLE;
        dskHideWindow (0, pCurTop->idx_znode);
        dskSetPrimitiveChildren (pCurTop, FALSE);
        SendNotifyMessage (pCurTop->hParent, 
                        MSG_CHILDHIDDEN, reason, lParam);

        dskScreenToClient (pCurTop->pMainWin, &rc, &rc);
        InvalidateRect ((HWND)pCurTop->pMainWin, &rc, TRUE);
    }
}

static int update_client_window_rgn (int cli, HWND hwnd)
{
    CLIPRECT* crc = sg_UpdateRgn.head;

    if (hwnd) {
        while (crc) {
            __mg_update_window (hwnd, crc->rc.left, crc->rc.top, 
                    crc->rc.right, crc->rc.bottom);
            crc = crc->next;
        }
    }

    return 0;
}

static void dskMoveToTopMost (PMAINWIN pWin, int reason, LPARAM lParam)
{
    if (!pWin) return;

    if (dskIsTopMost (pWin) && (pWin->dwStyle & WS_VISIBLE))
        return;

    dskHideGlobalControl (pWin, reason, lParam);
    dskMove2Top (0, pWin->idx_znode);

    // activate this main window.
    if ( !(pWin->dwStyle & WS_VISIBLE) ) {
        pWin->dwStyle |= WS_VISIBLE;

        dskShowWindow (0, pWin->idx_znode);

        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
        InvalidateRect ((HWND)pWin, NULL, TRUE);
    }
    else {
        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
    }

    if (reason != RCTM_SHOWCTRL)
        dskSetActiveWindow (pWin);
}

static void dskHideMainWindow (PMAINWIN pWin)
{
    if (!(pWin->dwStyle & WS_VISIBLE))
        return;

    pWin->dwStyle &= ~WS_VISIBLE;

    dskHideWindow (0, pWin->idx_znode);
}

// When destroy a main win, all main win which is covered by
// this destroying main win will be redraw.
//
// Functions which lead to call this function:
//  DestroyWindow: destroy a visible window.
//
static void dskRemoveMainWindow (PMAINWIN pWin)
{
    FreeZOrderMaskRect (0, pWin->idx_znode);
    // Update window Z order list.
    FreeZOrderNode (0, pWin->idx_znode);

    // Handle main window hosting.
    if (pWin->pHosting)
        dskRemoveHostedMainWindow (pWin->pHosting, pWin);

    /* houhh 20081127.*/
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

#ifdef _MGHAVE_MENU

static void dskForceCloseMenu (void)
{
    ZORDERINFO* zi = __mg_zorder_info;

    if (zi->cli_trackmenu < 0)
        return;

    srvForceCloseMenu (0);
}

static int dskStartTrackPopupMenu(PTRACKMENUINFO ptmi)
{
    PTRACKMENUINFO plast;

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

    ptmi->idx_znode = srvStartTrackPopupMenu (0, &ptmi->rc, (HWND)ptmi);

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
    ZORDERINFO* zi = __mg_zorder_info;
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
    }

    if (zi->cli_trackmenu == 0) {
        srvEndTrackPopupMenu (0, ptmi->idx_znode);
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

    if (dskEnableZOrderNode (0, pWin?pWin->idx_znode:0, flags))
        return;

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

            if (dskGetActiveWindow () == pWin) {
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

    lock_zi_for_read (__mg_zorder_info);
    dskClientToScreen (pWin, pswi->rc1, &rcScreen);

    //BUGFIX: if the MainWindow is AutoSecondaryDC, the secondaryDC and
    //client dc would be diffirent, so we must get the scondaryDC,
    //the update to client dc (dongjunjie 2010/7/28)
    //hdc = GetClientDC ((HWND)pWin);
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
    //ReleaseDC (hdc);
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
            bNeedInvalidate = TRUE;
        }
        else if (pswi->iOffx > 0) {
            rcInvalid.right = rcInvalid.left + pswi->iOffx;
            bNeedInvalidate = TRUE;
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
    if (inved) {
        PostMessage ((HWND)pWin, MSG_PAINT, 0, 0);
    }

    return 0;
}
#endif
static void dskMoveMainWindow (PMAINWIN pWin, const RECT* prcExpect)
{
    RECT oldWinRect, rcResult;

    memcpy (&oldWinRect, &pWin->left, sizeof (RECT));
    SendAsyncMessage ((HWND)pWin, MSG_CHANGESIZE, 
                    (WPARAM)(prcExpect), (LPARAM)(&rcResult));

    dskMoveWindow (0, pWin->idx_znode, &rcResult);
}

static void dskMoveGlobalControl (PMAINWIN pCtrl, RECT* prcExpect)
{
    RECT newWinRect, rcResult;

    SendAsyncMessage ((HWND)pCtrl, MSG_CHANGESIZE, 
                    (WPARAM)(prcExpect), (LPARAM)(&rcResult));
    dskClientToScreen ((PMAINWIN)(pCtrl->hParent), prcExpect, &newWinRect);

    dskMoveWindow (0, pCtrl->idx_znode, &newWinRect);

    if (pCtrl->dwStyle & WS_VISIBLE) {
        SendAsyncMessage ((HWND)pCtrl, MSG_NCPAINT, 0, 0);
        InvalidateRect ((HWND)pCtrl, NULL, TRUE);
    }
}

/*********************** Hook support ****************************************/
static HOOKINFO keyhook;
static HOOKINFO mousehook;

static MSGHOOK dskRegisterKeyHook (void* context, MSGHOOK hook)
{
    MSGHOOK old_hook = keyhook.hook;

    keyhook.context = context;
    keyhook.hook = hook;
    return old_hook;
}

static MSGHOOK dskRegisterMouseHook (void* context, MSGHOOK hook)
{
    MSGHOOK old_hook = mousehook.hook;

    mousehook.context = context;
    mousehook.hook = hook;
    return old_hook;
}

static int dskHandleKeyHooks (HWND dst_wnd, UINT message, 
                WPARAM wParam, LPARAM lParam)
{
    int ret = HOOK_GOON;

    if (keyhook.hook) {
        ret = keyhook.hook (keyhook.context, dst_wnd, message, wParam, lParam);
    }

    return ret;
}

static int dskHandleMouseHooks (HWND dst_wnd, UINT message, 
                WPARAM wParam, LPARAM lParam)
{
    int ret = HOOK_GOON;

    if (mousehook.hook) {
        ret = mousehook.hook (mousehook.context, dst_wnd, message, wParam, lParam);
    }

    return ret;
}

/*********************** Desktop window support ******************************/
PMAINWIN gui_GetMainWindowPtrUnderPoint (int x, int y)
{
    int slot;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    slot = get_znode_at_point (__mg_zorder_info, nodes, x, y);

    if (slot == 0)
        return NULL;
    else
        return (PMAINWIN)(nodes[slot].fortestinghwnd);
}

static int HandleSpecialKey (int scancode)
{
    switch (scancode) {
    case SCANCODE_BACKSPACE:
        ExitGUISafely (-1);
        return 0;
    }

    return 0;
}

static LRESULT KeyMessageHandler (UINT message, int scancode, DWORD status)
{
    static int mg_altdown = 0;
    static int mg_modal = 0;
    int next_node;

    if ((message == MSG_KEYDOWN) && (status & KS_ALT) && (status & KS_CTRL))
        return HandleSpecialKey (scancode);
        
    if (scancode == SCANCODE_LEFTALT ||
        scancode == SCANCODE_RIGHTALT) {
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
            if( scancode == SCANCODE_TAB) {
            
                mg_modal = 1;
                
#ifdef _MGHAVE_MENU
                if (sg_ptmi)
                    dskForceCloseMenu ();
#endif
                next_node = 
                    get_next_visible_mainwin (__mg_zorder_info,
                            __mg_zorder_info->active_win);

                if (next_node) {
                    dskMove2Top (0, next_node);
                    dskSetActiveZOrderNode (0, next_node);
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
#ifndef _MGRM_THREADS
    if (__mg_ime_wnd
                    && message != MSG_SYSKEYDOWN && message != MSG_SYSKEYUP) {
        if (dskHandleKeyHooks (__mg_ime_wnd, 
                message, (WPARAM)scancode, (LPARAM)status) == HOOK_GOON)
            PostMessage (__mg_ime_wnd, 
                            message, (WPARAM)scancode, (LPARAM)status);
        return 0;
    }

#else
    else if (__mg_ime_wnd && __mg_zorder_info->active_win) {
        if (dskHandleKeyHooks (__mg_ime_wnd, 
                message, (WPARAM)scancode, (LPARAM)status) == HOOK_GOON)
            PostMessage (__mg_ime_wnd, 
                                message, (WPARAM)scancode, (LPARAM)status);

        return 0;
    }
#endif

    if (__mg_zorder_info->active_win) {
        if (dskHandleKeyHooks ((HWND)dskGetActiveWindow(), 
                    message, (WPARAM)scancode, (LPARAM)status) == HOOK_GOON) {
            __mg_post_msg_by_znode (__mg_zorder_info,
                    __mg_zorder_info->active_win, message, 
                    (WPARAM)scancode, (LPARAM)status);
        }
    }
    else {
        if (dskHandleKeyHooks (HWND_DESKTOP, message, 
                    (WPARAM)scancode, (LPARAM)status) == HOOK_GOON) {
            SendMessage (HWND_DESKTOP, MSG_DT_KEYOFF + message,
                    (WPARAM)scancode, (LPARAM)status);
        }
    }

    return 0;
}

static PMAINWIN _mgs_button_down_main_window = NULL;
static PMAINWIN _mgs_old_under_p = NULL;

#define DOWN_BUTTON_NONE        0x0000
#define DOWN_BUTTON_LEFT        0x0001
#define DOWN_BUTTON_RIGHT       0x0002
#define DOWN_BUTTON_ANY         0x000F

static DWORD _mgs_down_buttons = 0;


/* defined in ../gui/window.c */
extern void __mg_reset_mainwin_capture_info (PCONTROL ctrl);

void __mg_reset_desktop_capture_info (PMAINWIN pWin)
{
    _mgs_old_under_p = NULL;

    if (!__mg_ime_wnd || (HWND)pWin != __mg_ime_wnd) {
        _mgs_button_down_main_window = NULL;
        _mgs_down_buttons = DOWN_BUTTON_NONE;
    }

    if ((HWND)pWin == __mg_capture_wnd)
        __mg_capture_wnd = 0;

    __mg_reset_mainwin_capture_info ((PCONTROL)pWin);
}

static HWND DesktopSetCapture (HWND hwnd)
{
    HWND hTemp;

    _mgs_old_under_p = NULL;
    _mgs_button_down_main_window = gui_GetMainWindowPtrOfControl (hwnd);
    _mgs_down_buttons = DOWN_BUTTON_NONE;

    hTemp = __mg_capture_wnd;
    __mg_capture_wnd = hwnd;
    return hTemp;
}

static LRESULT MouseMessageHandler (UINT message, WPARAM flags, int x, int y)
{
    PMAINWIN pUnderPointer;
    PMAINWIN pCtrlPtrIn;

    if (__mg_capture_wnd) {
        PostMessage (__mg_capture_wnd, message, 
                        flags | KS_CAPTURED, MAKELONG (x, y));
        return 0;
    }

    pCtrlPtrIn = gui_GetMainWindowPtrUnderPoint (x, y);

    if (dskHandleMouseHooks ((HWND)pCtrlPtrIn, 
                            message, flags, MAKELONG (x, y)) == HOOK_STOP)
        return 0;
    
    if (pCtrlPtrIn && pCtrlPtrIn->WinType == TYPE_CONTROL) {
        pUnderPointer = pCtrlPtrIn->pMainWin;
    }
    else {
        pUnderPointer = pCtrlPtrIn;
        pCtrlPtrIn = NULL;
    }

    switch (message) {
        case MSG_MOUSEMOVE:
            if (_mgs_button_down_main_window) {
                PostMessage ((HWND)_mgs_button_down_main_window, 
                                message + MSG_DT_MOUSEOFF, 
                                flags, MAKELONG (x, y));
            }
            else {
                if (_mgs_old_under_p != pUnderPointer) {
                    if (_mgs_old_under_p) {
                        PostMessage ((HWND)_mgs_old_under_p,
                            MSG_MOUSEMOVEIN, FALSE, (LPARAM)pUnderPointer);
                        PostMessage ((HWND)_mgs_old_under_p,
                            MSG_NCMOUSEMOVE, HT_OUT, MAKELONG (x, y));
                    }
                    if (pUnderPointer)
                        PostMessage ((HWND)pUnderPointer,
                            MSG_MOUSEMOVEIN, TRUE, (LPARAM)_mgs_old_under_p);
                    else
                        SetCursor (GetSystemCursor (IDC_ARROW));

                    _mgs_old_under_p = pUnderPointer;
                }

                if (pUnderPointer) {
                    if (pUnderPointer->dwStyle & WS_DISABLED) {
                        HCURSOR def_cursor = GetDefaultCursor ();

                        if (def_cursor)
                            SetCursor (def_cursor);
                    }
                    else
                        PostMessage ((HWND)pUnderPointer, 
                                message + MSG_DT_MOUSEOFF, 
                                flags, MAKELONG (x, y));
                }
                else {
                    HCURSOR def_cursor = GetDefaultCursor ();

                    if (def_cursor)
                        SetCursor (def_cursor);
                    PostMessage (HWND_DESKTOP, MSG_DT_MOUSEMOVE,
                            flags, MAKELONG (x, y));
                }
            }
            break;

        case MSG_LBUTTONDOWN:
        case MSG_RBUTTONDOWN:
            if (_mgs_button_down_main_window) {
                PostMessage ((HWND)_mgs_button_down_main_window, 
                                message + MSG_DT_MOUSEOFF, 
                                flags, MAKELONG (x, y));
                if (message == MSG_LBUTTONDOWN)
                    _mgs_down_buttons |= DOWN_BUTTON_LEFT;
                else
                    _mgs_down_buttons |= DOWN_BUTTON_RIGHT;
            }
            else if (pUnderPointer) {
#ifdef _MGHAVE_MENU
                if (sg_ptmi)
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
                
                    if (pUnderPointer != dskSetActiveWindow (pUnderPointer))
                        SendNotifyMessage ((HWND) pUnderPointer, 
                                        MSG_MOUSEACTIVE, 0, 0);
                }


                PostMessage ((HWND)pUnderPointer, message + MSG_DT_MOUSEOFF,
                                flags, MAKELONG (x, y));

                _mgs_button_down_main_window = pUnderPointer;
                if (message == MSG_LBUTTONDOWN)
                    _mgs_down_buttons = DOWN_BUTTON_LEFT;
                else
                    _mgs_down_buttons = DOWN_BUTTON_RIGHT;
            }
            else {
                dskSetActiveWindow (NULL);
                PostMessage (HWND_DESKTOP, message + MSG_DT_MOUSEOFF,
                            flags, MAKELONG (x, y));
                _mgs_button_down_main_window = NULL;
                _mgs_down_buttons = DOWN_BUTTON_NONE;
            }
        break;

        case MSG_LBUTTONUP:
        case MSG_RBUTTONUP:
            if (_mgs_down_buttons == DOWN_BUTTON_LEFT &&
                            message == MSG_RBUTTONUP) {
                break;
            }
            if (_mgs_down_buttons == DOWN_BUTTON_RIGHT &&
                            message == MSG_LBUTTONUP) {
                break;
            }

            if (_mgs_button_down_main_window) {
                PostMessage ((HWND)_mgs_button_down_main_window, 
                                message + MSG_DT_MOUSEOFF, 
                                flags, MAKELONG (x, y));
                if (message == MSG_LBUTTONUP)
                    _mgs_down_buttons &= ~DOWN_BUTTON_LEFT;
                else 
                    _mgs_down_buttons &= ~DOWN_BUTTON_RIGHT;

                if (!(_mgs_down_buttons & DOWN_BUTTON_ANY)) {
                    _mgs_button_down_main_window = NULL;
                    _mgs_down_buttons = DOWN_BUTTON_NONE;
                }
            }
            else {
                /* fixed bug 4961: for deal with when _mgs_button_down_main_window was reset,
                 * but there real has a mouse up message. by humingming 2010.9.1
                 */
                if (pUnderPointer == NULL) {
                    PostMessage (HWND_DESKTOP,
                            message + MSG_DT_MOUSEOFF,
                            flags, MAKELONG (x, y));
                }
                else {
                    if ((pUnderPointer->dwStyle & WS_DISABLED)) {
                        break;
                    }
                    else {
                        PostMessage((HWND)pUnderPointer, 
                            message + MSG_DT_MOUSEOFF, 
                            flags, MAKELONG (x, y));
                    }
                }
            }
        break;
        
        case MSG_LBUTTONDBLCLK:
        case MSG_RBUTTONDBLCLK:
            if (_mgs_button_down_main_window) {
                PostMessage ((HWND)_mgs_button_down_main_window, 
                                message + MSG_DT_MOUSEOFF, 
                                flags, MAKELONG (x, y));
                if (message == MSG_LBUTTONDBLCLK)
                    _mgs_down_buttons |= DOWN_BUTTON_LEFT;
                else
                    _mgs_down_buttons |= DOWN_BUTTON_RIGHT;
            }
            else if (pUnderPointer) {
                PostMessage ((HWND)pUnderPointer, 
                                message + MSG_DT_MOUSEOFF, 
                                flags, MAKELONG (x, y));
                _mgs_button_down_main_window = pUnderPointer;
                if (message == MSG_LBUTTONDBLCLK)
                    _mgs_down_buttons = DOWN_BUTTON_LEFT;
                else
                    _mgs_down_buttons = DOWN_BUTTON_RIGHT;
            }
            else {
                PostMessage (HWND_DESKTOP, 
                            message + MSG_DT_MOUSEOFF, 
                            flags, MAKELONG (x, y));
                _mgs_button_down_main_window = NULL;
                _mgs_down_buttons = DOWN_BUTTON_NONE;
            }
            break;
    }

    return 0;
}

typedef struct _DRAGDROPINFO {
    HWND hwnd;
    RECT rc;

    int location;
    int last_x, last_y;
} DRAGDROPINFO;

static DRAGDROPINFO _dd_info;

#ifndef _MGRM_THREADS
inline static void lock_zorder_info (void) { }

inline static void unlock_zorder_info (void) { }
#else
static void lock_zorder_info (void)
{
    PMAINWIN pWin=NULL;
    int slot;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    slot = __mg_zorder_info->first_topmost;
    for (; slot > 0; slot = nodes[slot].next)
    {
        pWin = (PMAINWIN)(nodes[slot].fortestinghwnd);
        if (pWin)
            pthread_mutex_lock (&pWin->pGCRInfo->lock);
    }
    
    slot = __mg_zorder_info->first_normal;
    for (; slot > 0; slot = nodes[slot].next)
    {
        pWin = (PMAINWIN)(nodes[slot].fortestinghwnd);
        if (pWin)
            pthread_mutex_lock (&pWin->pGCRInfo->lock);
    }
}

static void unlock_zorder_info (void)
{
    PMAINWIN pWin;
    int slot;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    slot = __mg_zorder_info->first_topmost;
    for (; slot > 0; slot = nodes[slot].next)
    {
        pWin = (PMAINWIN)(nodes[slot].fortestinghwnd);
        if (pWin)
            pthread_mutex_unlock (&pWin->pGCRInfo->lock);
    }
    
    slot = __mg_zorder_info->first_normal;
    for (; slot > 0; slot = nodes[slot].next)
    {
        pWin = (PMAINWIN)(nodes[slot].fortestinghwnd);
        if (pWin)
            pthread_mutex_unlock (&pWin->pGCRInfo->lock);
    }
}

#endif

static int dskStartDragWindow (PMAINWIN pWin, const DRAGINFO* drag_info)
{
    if (!(pWin->dwStyle & WS_VISIBLE))
        return -1;

    if (_dd_info.hwnd != 0)
        return -1;

    _mgs_old_under_p = NULL;
    _mgs_button_down_main_window = NULL;
    _mgs_down_buttons = 0;

    lock_zorder_info ();

    _dd_info.hwnd       = (HWND)pWin;
    _dd_info.rc.left    = pWin->left;
    _dd_info.rc.top     = pWin->top;
    _dd_info.rc.right   = pWin->right;
    _dd_info.rc.bottom  = pWin->bottom;
    _dd_info.location   = drag_info->location;
    _dd_info.last_x     = drag_info->init_x;
    _dd_info.last_y     = drag_info->init_y;

    switch (_dd_info.location) {
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
    FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);
    return 0;
}

static int dskCancelDragWindow (PMAINWIN pWin)
{
    if (!(pWin->dwStyle & WS_VISIBLE))
        return -1;

    if (_dd_info.hwnd == 0 || _dd_info.hwnd != (HWND)pWin)
        return -1;

    _dd_info.hwnd = (HWND)-1;
    unlock_zorder_info ();
    SelectClipRect (HDC_SCREEN_SYS, &g_rcScr);
   // SetDefaultCursor (GetSystemCursor (IDC_ARROW));
    return 0;
}

static int do_drag_drop_window (UINT msg, int x, int y)
{
    if (_dd_info.hwnd == 0)
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
                    _MG_PRINTF ("KERNEL>Desktop: do_drag_drop_window: bad location\n");
                    break;
        }

        FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);
                    
        _dd_info.last_x = x;
        _dd_info.last_y = y;
    }
    else {
        SetPenColor (HDC_SCREEN_SYS, PIXEL_lightwhite);
        FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);

        /* post MSG_WINDOWDROPPED to the target window */
        SendNotifyMessage (_dd_info.hwnd, MSG_WINDOWDROPPED,
                          MAKELONG (_dd_info.rc.left, _dd_info.rc.top),
                          MAKELONG (_dd_info.rc.right, _dd_info.rc.bottom));         
        unlock_zorder_info ();
        SelectClipRect (HDC_SCREEN_SYS, &g_rcScr);
        
        if(_dd_info.hwnd != (HWND)gui_GetMainWindowPtrUnderPoint (x, y))
            SetDefaultCursor (GetSystemCursor (IDC_ARROW));
        _dd_info.hwnd = 0;
    }

    return 1;
}

static LRESULT WindowMessageHandler(UINT message, PMAINWIN pWin, LPARAM lParam)
{
    LRESULT lRet = 0;

    /* cancel the current drag and drop operation */
    do_drag_drop_window (message, 0, 0);

    switch (message) {
        case MSG_ADDNEWMAINWIN:
#ifdef _MGHAVE_MENU
            if (sg_ptmi)
                dskForceCloseMenu ();
#endif
            return dskAddNewMainWindow(pWin);

        case MSG_REMOVEMAINWIN:
#ifdef _MGHAVE_MENU
            if (sg_ptmi)
                dskForceCloseMenu ();
#endif
            dskRemoveMainWindow(pWin);
            __mg_reset_desktop_capture_info (pWin);
            return 0;

        case MSG_MOVETOTOPMOST:
#ifdef _MGHAVE_MENU
            if (sg_ptmi)
                dskForceCloseMenu ();
#endif
            dskMoveToTopMost(pWin, RCTM_MESSAGE, 0);
            return 0;

        case MSG_SHOWMAINWIN:
#ifdef _MGHAVE_MENU
            if (sg_ptmi)
                dskForceCloseMenu ();
#endif
            dskShowMainWindow(pWin, TRUE);
            return 0;

        case MSG_HIDEMAINWIN:
#ifdef _MGHAVE_MENU
            if (sg_ptmi)
                dskForceCloseMenu ();
#endif
            dskHideMainWindow (pWin);
            __mg_reset_desktop_capture_info (pWin); 
            return 0;

        case MSG_MOVEMAINWIN:
            if (pWin->WinType == TYPE_CONTROL)
                dskMoveGlobalControl (pWin, (RECT*)lParam);
            else {
                dskMoveMainWindow (pWin, (RECT*)lParam);
            }
            return 0;

        case MSG_GETACTIVEMAIN:
            return (LRESULT)dskGetActiveWindow();
        
        case MSG_SETACTIVEMAIN:
            lRet = (LRESULT)dskSetActiveWindow (pWin);
            return lRet;

        case MSG_GETCAPTURE:
            return (LRESULT)__mg_capture_wnd;

        case MSG_SETCAPTURE:
            return (LRESULT)DesktopSetCapture ((HWND)pWin);
 
#ifdef _MGHAVE_MENU
        case MSG_TRACKPOPUPMENU:
            return dskStartTrackPopupMenu((PTRACKMENUINFO)lParam);

        case MSG_ENDTRACKMENU:
            return dskEndTrackPopupMenu((PTRACKMENUINFO)lParam);

        case MSG_CLOSEMENU:
            return dskCloseMenu ();
#endif

        case MSG_SCROLLMAINWIN:
            lRet = dskScrollMainWindow (pWin, (PSCROLLWINDOWINFO)lParam);
            return lRet;

        case MSG_CARET_CREATE:
            sg_hCaretWnd = (HWND)pWin;
            sg_uCaretBTime = pWin->pCaretInfo->uTime;
            return 0;

        case MSG_CARET_DESTROY:
            sg_hCaretWnd = 0;
            return 0;

        case MSG_ENABLEMAINWIN:
            dskEnableWindow (pWin, lParam);
            return 0;
        
        case MSG_ISENABLED:
            return !(pWin->dwStyle & WS_DISABLED);
        
        case MSG_SETWINCURSOR:
        {
            HCURSOR old = pWin->hCursor;

            pWin->hCursor = (HCURSOR)lParam;
            return (LRESULT)old;
        }

        case MSG_GETNEXTMAINWIN:
        {
            HWND hWnd = HWND_NULL;
            int slot;
            ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);
            int last_type;

            if (pWin) {
                last_type = nodes[pWin->idx_znode].flags & ZOF_TYPE_MASK;
                slot = nodes[pWin->idx_znode].next;
            }else{
                last_type = ZOF_TYPE_TOPMOST;
                slot = __mg_zorder_info->first_topmost;
            }

            while (1) {
                if (slot <= 0) {
                    if (last_type == ZOF_TYPE_TOPMOST) {
                        last_type = ZOF_TYPE_NORMAL;
                        slot = __mg_zorder_info->first_normal;
                        continue;
                    }
                    else {
                        return (LRESULT)HWND_NULL;
                    }
                }

                hWnd = nodes[slot].fortestinghwnd;
                if (0
                        || !hWnd
                        || !(pWin = gui_CheckAndGetMainWindowPtr (hWnd))
                        || (pWin->dwExStyle & WS_EX_CTRLASMAINWIN)) {
                    slot = nodes[slot].next;
                    continue;
                }else{
                    return (LRESULT)hWnd;
                }
            }
            break;
        }

        case MSG_SHOWGLOBALCTRL:
#ifdef _MGHAVE_MENU
            if (sg_ptmi)
                dskForceCloseMenu ();
#endif
            dskMoveGlobalControl (pWin, (RECT*)&(pWin->left));
            dskMoveToTopMost (pWin, RCTM_SHOWCTRL, 0);
            dskSetPrimitiveChildren (pWin, TRUE);
            break;

        case MSG_HIDEGLOBALCTRL:
#ifdef _MGHAVE_MENU
            if (sg_ptmi)
                dskForceCloseMenu ();
#endif
            dskHideMainWindow (pWin);
            dskSetPrimitiveChildren (pWin, FALSE);
            break;

        case MSG_STARTDRAGWIN:
            return dskStartDragWindow (pWin, (DRAGINFO*)lParam);

        case MSG_CANCELDRAGWIN:
            return dskCancelDragWindow (pWin);
   }

   return lRet;
}

#define IDM_REDRAWBG    MINID_RESERVED
#define IDM_CLOSEALLWIN (MINID_RESERVED + 1)
#define IDM_ENDSESSION  (MINID_RESERVED + 2)

#define IDM_FIRSTWINDOW (MINID_RESERVED + 101)

#ifdef _MGHAVE_MENU
static HMENU dskCreateWindowSubMenu (void)
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

static HMENU dskCreateDesktopMenu (void)
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
    mii.hsubmenu     = dskCreateWindowSubMenu();
    InsertMenuItem (hmnu, 3, TRUE, &mii);
                        
    mii.type        = MFT_SEPARATOR;
    mii.id          = 0;
    mii.typedata    = 0;
    mii.hsubmenu    = 0;
    InsertMenuItem(hmnu, 4, TRUE, &mii);

    return hmnu;
}

static void dskUpdateDesktopMenu (HMENU hDesktopMenu)
{
    MENUITEMINFO mii;
    HMENU hWinMenu;
    int nCount, iPos;
    PMAINWIN    pWin;
    int id;
    int slot;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    hWinMenu = GetSubMenu (hDesktopMenu, 3);

    nCount = GetMenuItemCount (hWinMenu);

    for (iPos = nCount; iPos > 0; iPos --)
        DeleteMenu (hWinMenu, iPos - 1, MF_BYPOSITION);
    
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;

    id = IDM_FIRSTWINDOW;
    iPos = 0;
    
    slot = __mg_zorder_info->first_topmost;
    for (; slot > 0; slot = nodes[slot].next)
    {
        pWin = (PMAINWIN)(nodes[slot].fortestinghwnd);
        if (pWin && pWin->WinType == TYPE_MAINWIN && 
                !(nodes[slot].flags & ZOF_TF_TOPFOREVER)) {
            if (pWin->dwStyle & WS_VISIBLE)
                mii.state       = MFS_ENABLED;
            else
                mii.state       = MFS_DISABLED;
        } else {
            continue;
        }
            
        mii.id              = id;
        mii.typedata        = (DWORD)pWin->spCaption; 
        mii.itemdata        = (DWORD)pWin; 
        InsertMenuItem(hWinMenu, iPos, TRUE, &mii);

        id++;
        iPos++;
    }
    
    if (iPos != 0) {
        mii.type            = MFT_SEPARATOR;
        mii.state           = 0;
        mii.id              = 0;
        mii.typedata        = 0;
        InsertMenuItem(hWinMenu, iPos, TRUE, &mii);
        iPos ++;
        mii.type            = MFT_STRING;
    }

    slot = __mg_zorder_info->first_normal;
    for (; slot > 0; slot = nodes[slot].next)
    {
        pWin = (PMAINWIN)(nodes[slot].fortestinghwnd);
        if (pWin && pWin->WinType == TYPE_MAINWIN) {
            if (pWin->dwStyle & WS_VISIBLE)
                mii.state       = MFS_ENABLED;
            else
                mii.state       = MFS_DISABLED;
        }
        else {
            continue;
        }
            
        mii.id              = id;
        mii.typedata        = (DWORD)pWin->spCaption; 
        mii.itemdata        = (DWORD)pWin; 
        InsertMenuItem(hWinMenu, iPos, TRUE, &mii);

        id++;
        iPos++;
    }
    
    nCount = GetMenuItemCount (hDesktopMenu);
    for (iPos = nCount; iPos > 5; iPos --)
        DeleteMenu (hDesktopMenu, iPos - 1, MF_BYPOSITION);
        
    dsk_ops->customize_desktop_menu (dt_context, hDesktopMenu, 5);
}
#endif

static int dskDesktopCommand (HMENU hDesktopMenu, int id)
{
    int slot;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    if (id == IDM_REDRAWBG)
        SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
    else if (id == IDM_CLOSEALLWIN) {
        PMAINWIN pWin;
    
        slot = __mg_zorder_info->first_topmost;
        for (; slot > 0; slot = nodes[slot].next)
        {
            pWin = (PMAINWIN)(nodes[slot].fortestinghwnd);
            if (pWin && (pWin->WinType != TYPE_CONTROL)
#ifndef _MGRM_THREADS
                    && (pWin->pHosting == __mg_dsk_win)
#else
                    && (pWin->pHosting == NULL)
#endif
               )
				PostMessage ((HWND)pWin, MSG_CLOSE, 0, 0);
        }

        slot = __mg_zorder_info->first_normal;
        for (; slot > 0; slot = nodes[slot].next)
        {
            pWin = (PMAINWIN)(nodes[slot].fortestinghwnd);
            if (pWin && (pWin->WinType == TYPE_MAINWIN)
#ifndef _MGRM_THREADS
                    && (pWin->pHosting == __mg_dsk_win)
#else
                    && (pWin->pHosting == NULL)
#endif
               ) 
                    PostMessage ((HWND)pWin, MSG_CLOSE, 0, 0);
        }
    }
    else if (id == IDM_ENDSESSION) {
        PostMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
    }
#ifdef _MGHAVE_MENU
    else if (id >= IDM_FIRSTWINDOW) {

        HMENU win_menu;
        MENUITEMINFO mii = {MIIM_DATA};
        PMAINWIN pSelectedWin;

        win_menu = GetSubMenu (hDesktopMenu, 3);
        if (GetMenuItemInfo (win_menu, id, MF_BYCOMMAND, &mii) == 0) {
            pSelectedWin = (PMAINWIN)mii.itemdata;

            if (pSelectedWin 
                    && pSelectedWin->WinType == TYPE_MAINWIN
                    && !(pSelectedWin->dwStyle & WS_DISABLED)) {
                if (sg_ptmi)
                dskForceCloseMenu ();

                dskMoveToTopMost (pSelectedWin, RCTM_MESSAGE, 0);
                dskSetActiveWindow (pSelectedWin);
            }
        }
    }
#endif

    return 0;
}

static int dskOnNewCtrlInstance (PCONTROL pParent, PCONTROL pNewCtrl)
{
    PCONTROL pFirstCtrl, pLastCtrl;

    if (pNewCtrl->dwExStyle & WS_EX_CTRLASMAINWIN) {

        // Add to Z-Order list.
        pNewCtrl->idx_znode = dskAllocZOrderNode ((PMAINWIN)pNewCtrl);

        if (pNewCtrl->idx_znode <= 0) {
            _MG_PRINTF ("KERNEL>Desktop: Alloc zorder node for control fail.\n");
            return -1;
        }

        // Init Global Clip Region info.
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

    if (pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN) {
        FreeZOrderMaskRect (0, pCtrl->idx_znode);
        FreeZOrderNode (0, pCtrl->idx_znode);
    }

    if ((HWND)pCtrl == __mg_capture_wnd) {
        /* force release the capture */
        __mg_capture_wnd = 0;
    }

    if (fFound) {
        pCtrl->pcci->nUseCount --;
        return 0;
    }

    return -1;
}

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

    pTemp = (PMAINWIN)node->fortestinghwnd;

    if (pTemp
            && ((pTemp->WinType == TYPE_CONTROL && (pTemp->dwExStyle & WS_EX_CTRLASMAINWIN))
                || pTemp->WinType != TYPE_CONTROL)
            && pTemp->dwStyle & WS_VISIBLE) {
        if (info->is_empty_invrc) {
            SendAsyncMessage ((HWND)pTemp, MSG_NCPAINT, 0, 0);
            InvalidateRect ((HWND)pTemp, NULL, TRUE);
        }
        else {
            RECT rcTemp, rcInv, rcWin;

            if (pTemp->WinType == TYPE_CONTROL && (pTemp->dwExStyle & WS_EX_CTRLASMAINWIN)){
                dskGetWindowRectInScreen (pTemp, &rcWin);
            }
            else
                GetWindowRect((HWND)pTemp, &rcWin);

            if (IntersectRect (&rcTemp, 
                        &rcWin, info->invrc)) {
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

static void dskRefreshAllWindow (const RECT* invrc)
{
    REFRESH_INFO info = {invrc, FALSE};

    if (invrc->top == invrc->bottom || invrc->left == invrc->right)
        info.is_empty_invrc = TRUE;

#ifdef _MGHAVE_MENU
            dskForceCloseMenu ();
#endif

    SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 
                        (LPARAM)(info.is_empty_invrc?0:&invrc));

    lock_zi_for_read (__mg_zorder_info);
    do_for_all_znodes (&info, __mg_zorder_info, _cb_refresh_znode, ZT_ALL);
    unlock_zi_for_read (__mg_zorder_info);
}

static int dskRegisterIMEWnd (HWND hwnd)
{
    if (__mg_ime_wnd != 0)
        return ERR_IME_TOOMUCHIMEWND;

    if (!gui_CheckAndGetMainWindowPtr (hwnd))
        return ERR_INV_HWND;

    __mg_ime_wnd = hwnd;

    SendNotifyMessage (__mg_ime_wnd, MSG_IME_CLOSE, 0, 0);

#ifndef _MGRM_PROCESSES
    hwnd = (HWND)dskGetActiveWindow();
    if (__mg_ime_wnd != hwnd)
        SendNotifyMessage (__mg_ime_wnd, MSG_IME_SETTARGET, (WPARAM)hwnd, 0);
#endif

    return ERR_OK;
}

static int dskUnregisterIMEWnd (HWND hwnd)
{
    if (__mg_ime_wnd != hwnd)
        return ERR_IME_NOSUCHIMEWND;

    __mg_ime_wnd = 0;

    return ERR_OK;
}

#if 0
static int dskSetIMEStatus (int iIMEStatusCode, int Value)
{
    if (__mg_ime_wnd == 0)
        return ERR_IME_NOIMEWND;

    SendMessage (__mg_ime_wnd, 
        MSG_IME_SETSTATUS, (WPARAM)iIMEStatusCode, (LPARAM)Value);

    return ERR_OK;
}

static POINT pt = {0};
static int dskSetIMEPos (const POINT* p)
{
    if (__mg_ime_wnd == 0)
        return ERR_IME_NOIMEWND;

    pt = *p;
    //SendMessage (__mg_ime_wnd, 
    PostMessage (__mg_ime_wnd, 
        MSG_IME_SETPOS, (WPARAM)0, (LPARAM)&pt);

    return ERR_OK;
}

static int dskGetIMEPos (POINT* pt)
{
    if (__mg_ime_wnd == 0)
        return ERR_IME_NOIMEWND;

    SendMessage (__mg_ime_wnd, 
        MSG_IME_GETPOS, (WPARAM)0, (LPARAM)pt);

    return ERR_OK;
}

static int dskGetIMEStatus (int iIMEStatusCode)
{
    if (__mg_ime_wnd == 0)
        return ERR_IME_NOIMEWND;

    return SendMessage (__mg_ime_wnd, MSG_IME_GETSTATUS, iIMEStatusCode, 0);
}
#endif

void GUIAPI DesktopUpdateAllWindow(void)
{
	SendMessage(HWND_DESKTOP, MSG_PAINT, 0, 0);
}

#ifndef _MG_ENABLE_SCREENSAVER
#   define HAS_NO_MAINWINDOW() ((__mg_zorder_info->nr_normals + __mg_zorder_info->nr_topmosts) == 0) 
#else
    /* The screensaver occupys one znode */
#   define HAS_NO_MAINWINDOW() ((__mg_zorder_info->nr_normals + __mg_zorder_info->nr_topmosts) == 1) 
#endif

LRESULT DesktopWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC hDesktopDC;
    int flags, x, y;
    PMAINWIN active_mainwnd = dskGetActiveWindow();

    if (message >= MSG_FIRSTWINDOWMSG && message <= MSG_LASTWINDOWMSG)
        return WindowMessageHandler (message, (PMAINWIN)wParam, lParam);

    if (message == MSG_CHAR && active_mainwnd) {
        PostMessage ((HWND)active_mainwnd, message, wParam, lParam);
    }
    else if (message >= MSG_FIRSTKEYMSG && message <= MSG_LASTKEYMSG) {

        if (do_drag_drop_window (message, 0, 0))
            return 0;

        if (wParam == SCANCODE_PRINTSCREEN && message == MSG_KEYDOWN) {
#ifdef _MGMISC_SAVESCREEN
            static int n = 1;
            char buffer[20];

            sprintf (buffer, "%p-%d.bmp", (lParam & KS_CTRL)?
                                    (HWND)active_mainwnd:
                                    0, n);
            if (SaveMainWindowContent ((lParam & KS_CTRL)?
                                    (HWND)active_mainwnd:
                                    HWND_DESKTOP,
                                    buffer)) {
                Ping ();
                n ++;
            }
#endif
        }
#ifdef _MGHAVE_MENU
        else if (wParam == SCANCODE_ESCAPE && lParam & KS_CTRL) {
            dskUpdateDesktopMenu (sg_DesktopMenu);
            TrackPopupMenu (sg_DesktopMenu, TPM_DEFAULT, 
                0, g_rcScr.bottom, HWND_DESKTOP);
        }

        if (sg_ptmi) {
            if (PopupMenuTrackProc (sg_ptmi, message, wParam, lParam))
                return KeyMessageHandler (message, (int)wParam, (DWORD)lParam);
        }
        else
#endif
            return KeyMessageHandler (message, (int)wParam, (DWORD)lParam);
    }
        
    if (message >= MSG_FIRSTMOUSEMSG && message <= MSG_LASTMOUSEMSG) {

        flags = (int)wParam;

        x = LOSWORD (lParam);
        y = HISWORD (lParam);

        if (do_drag_drop_window (message, x, y))
            return 0;

#ifdef _MGHAVE_MENU
        if (sg_ptmi) {
            if (PopupMenuTrackProc (sg_ptmi, message, x, y))
                return MouseMessageHandler (message, flags, x, y);
        }
        else
#endif
            return MouseMessageHandler (message, flags, x, y);
    }
    
    if (message == MSG_COMMAND) {
      
        if (wParam <= MAXID_RESERVED && wParam >= MINID_RESERVED) {
#ifdef _MGHAVE_MENU
            dskDesktopCommand (sg_DesktopMenu, (int)wParam);
#endif
        }
        else
        {
            if(dsk_ops->desktop_menucmd_handler)
                dsk_ops->desktop_menucmd_handler(dt_context, (int)wParam);
        }

        return 0;
    }

    switch (message) {
        case MSG_STARTSESSION:
            __mg_init_local_sys_text ();
            hDesktopDC = GetDC (HWND_DESKTOP);

            dsk_ops = &def_dsk_ops;
            if(dsk_ops->init)
                dt_context = dsk_ops->init();
#ifdef _MGHAVE_MENU
			sg_DesktopMenu = dskCreateDesktopMenu ();
#endif

            break;

        case MSG_REINITSESSION:
            if (wParam)
                __mg_init_local_sys_text ();

#ifdef _MGHAVE_MENU
			if (sg_DesktopMenu) {
				DestroyMenu (sg_DesktopMenu);
				sg_DesktopMenu = 0;
			}

			sg_DesktopMenu = dskCreateDesktopMenu ();
#endif

            SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
        break;

        case MSG_ENDSESSION:
            if (
                    HAS_NO_MAINWINDOW()
#ifdef _MGRM_THREADS
                    && __mg_enter_terminategui
#endif
                    ) {
                screensaver_destroy();

                if (hDesktopDC) ReleaseDC (hDesktopDC);
                hDesktopDC = 0;

#ifdef _MGHAVE_MENU
                if (sg_DesktopMenu) {
                    DestroyMenu (sg_DesktopMenu);
                    sg_DesktopMenu = 0;
                }
#endif
                if(dsk_ops->deinit)
                    dsk_ops->deinit(dt_context);

                PostQuitMessage (HWND_DESKTOP);

                return 1;
            }
            break;

        case MSG_ERASEDESKTOP:

            if(dsk_ops->paint_desktop)
                dsk_ops->paint_desktop(dt_context, hDesktopDC, (PRECT)lParam);

            break;
#ifndef _MGRM_THREADS
        case MSG_SETFOCUS:
            {
                HWND active_win = (HWND)dskGetActiveWindow();
                if (active_win) {
                    SendNotifyMessage ((HWND)active_win, MSG_SETFOCUS, 0, 0);
                }
                break;
            }
#endif

        case MSG_PAINT:
        {
            RECT invrc;

            invrc.left = LOWORD(wParam);
            invrc.top = HIWORD(wParam);
            invrc.right = LOWORD(lParam);
            invrc.bottom = HIWORD(lParam);

            dskRefreshAllWindow (&invrc);
        }
        break;

        case MSG_BROADCASTMSG:
            return dskBroadcastMessage ((PMSG)lParam);

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
            
        case MSG_REGISTERKEYHOOK:
            return (LRESULT)dskRegisterKeyHook ((void*)wParam, 
                            (MSGHOOK)lParam);

        case MSG_REGISTERMOUSEHOOK:
            return (LRESULT)dskRegisterMouseHook ((void*)wParam, 
                            (MSGHOOK)lParam);

        case MSG_IME_REGISTER:
            return dskRegisterIMEWnd ((HWND)wParam);
            
        case MSG_IME_UNREGISTER:
            return dskUnregisterIMEWnd ((HWND)wParam);
            
        case MSG_IME_SETSTATUS:
            return dskSetIMEStatus ((int)wParam, (int)lParam);

        case MSG_IME_SET_TARGET_INFO:
            return dskSetIMETargetInfo ((IME_TARGET_INFO*)lParam);

        case MSG_IME_GET_TARGET_INFO:
            return dskGetIMETargetInfo ((IME_TARGET_INFO*)lParam);

        case MSG_IME_GETSTATUS:
            return dskGetIMEStatus ((int)wParam);
            
#ifndef _MGRM_THREADS
        case MSG_SRVNOTIFY:
            BroadcastMessage (MSG_SRVNOTIFY, wParam, lParam);
        break;
#endif
        case MSG_TIMEOUT:
            BroadcastMessage (MSG_IDLE, wParam, 0);
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
                dskUpdateDesktopMenu (sg_DesktopMenu);
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

        case MSG_TIMER:      // per 0.01s
        {
            static DWORD uCounter = 0;
#ifndef _MGRM_THREADS
            static DWORD blink_counter = 0;
            static DWORD sg_old_counter = 0;

            if (sg_old_counter == 0)
                sg_old_counter = __mg_timer_counter;

            mg_dispatch_timer_message (__mg_timer_counter - sg_old_counter);
            sg_old_counter = __mg_timer_counter;

            if (__mg_timer_counter < (blink_counter + 10))
                break;

            uCounter += (__mg_timer_counter - blink_counter) * 10;
            blink_counter = __mg_timer_counter;

#else
            if (__mg_quiting_stage < 0) {
                int slot;
                PMSGQUEUE pMsgQueue;
                ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);
                PMAINWIN pWin;

                /* XXX: wake up other theads */
                for (slot=__mg_zorder_info->first_topmost; slot > 0; slot = nodes[slot].next) {
                    pWin = (PMAINWIN)(nodes[slot].fortestinghwnd);
                    if (pWin && (pWin->WinType != TYPE_CONTROL) && (pWin->pHosting == NULL)) {
                        if ((pMsgQueue = kernel_GetMsgQueue((HWND)pWin))) {
                            POST_MSGQ(pMsgQueue);
                        }
                    }
                }
                for (slot = __mg_zorder_info->first_normal; slot > 0; slot = nodes[slot].next) {
                    pWin = (PMAINWIN)(nodes[slot].fortestinghwnd);
                    if (pWin && (pWin->WinType == TYPE_MAINWIN) && (pWin->pHosting == NULL)){
                        if ((pMsgQueue = kernel_GetMsgQueue((HWND)pWin))) {
                            POST_MSGQ(pMsgQueue);
                        }
                    }
                }

                if (__mg_quiting_stage > _MG_QUITING_STAGE_FORCE && __mg_quiting_stage <= _MG_QUITING_STAGE_START) {
                    -- __mg_quiting_stage;
                    /* printf("try to quit %d\n", __mg_quiting_stage); */
                }else if (__mg_quiting_stage <= _MG_QUITING_STAGE_FORCE) {
                    /* printf("force to quit !!!\n"); */
                }
                
                if (__mg_quiting_stage > _MG_QUITING_STAGE_DESKTOP
                        && HAS_NO_MAINWINDOW()
                        && __mg_enter_terminategui /* Let Desktop wait for MiniGUIMain() */
                        ) {
                    __mg_quiting_stage = _MG_QUITING_STAGE_DESKTOP;
                }else if (__mg_quiting_stage <= _MG_QUITING_STAGE_DESKTOP) {
                    PostMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
                }
            }

            mg_dispatch_timer_message (1);

            if (__mg_timer_counter % 10 != 0)
                break;

            uCounter += 100;
#endif
            if (sg_hCaretWnd != 0
                    && gui_GetMainWindowPtrOfControl (sg_hCaretWnd) == dskGetActiveWindow()
                    && uCounter >= sg_uCaretBTime) {
                PostMessage (sg_hCaretWnd, MSG_CARETBLINK, 0, 0);
                uCounter = 0;
            }
        }
        break;
    }

    return 0;
}
#endif

