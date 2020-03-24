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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** desktop-comm.c: The Desktop module.
**
** Current maintainer: Wei Yongming.
**
*/

/* only for MiniGUI-Threads or MiniGUI-Standalone */
#include "desktop.c"

#if defined(_MGRM_THREADS) || defined(_MGRM_STANDALONE)

static LRESULT DesktopWinProc (HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam);

static void init_desktop_win (void)
{
    static MAINWIN sg_desktop_win;
    PMAINWIN pDesktopWin;

    LICENSE_SET_MESSAGE_OFFSET();

    pDesktopWin = &sg_desktop_win;

    pDesktopWin->pMsgQueue         = __mg_dsk_msg_queue;
    pDesktopWin->MainWindowProc    = DesktopWinProc;

    pDesktopWin->DataType          = TYPE_HWND;
    pDesktopWin->WinType           = TYPE_ROOTWIN;

    pDesktopWin->pLogFont          = GetSystemFont (SYSLOGFONT_WCHAR_DEF);
    pDesktopWin->spCaption         = "THE DESKTOP WINDOW";

    pDesktopWin->pGCRInfo          = &sg_ScrGCRInfo;
    pDesktopWin->idx_znode         = 0;

    pDesktopWin->pMainWin          = pDesktopWin;
    pDesktopWin->we_rdr            = __mg_def_renderer;

    __mg_hwnd_desktop = (HWND)pDesktopWin;
    __mg_dsk_win  = pDesktopWin;

    __mg_dsk_msg_queue->pRootMainWin = __mg_dsk_win;
}

static PMAINWIN dskGetActiveWindow (void)
{
    ZORDERNODE* nodes;

    nodes = GET_ZORDERNODE(__mg_zorder_info);

    return (PMAINWIN)nodes[__mg_zorder_info->active_win].hwnd;
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
                &rc, pWin->spCaption, HDC_INVALID,
                CT_OPAQUE, 0);
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
        return -1;
    }

    /* Since 5.0.0: handle window style if failed to allocate znode
       for special main window */
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
    // Handle main window hosting.
    if (pWin->pHosting)
        dskAddNewHostedMainWindow (pWin->pHosting, pWin);
#endif  /* moved code */

    // Init Global Clip Region info.
    dskInitGCRInfo (pWin);

    // Init Invalid Region info.
    dskInitInvRgn (pWin);

    /* houhh 20081128, create secondary window dc before
     * InvalidateRect, erase_bk will used this. */
    if (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC) {
        pWin->secondaryDC = CreateSecondaryDC ((HWND)pWin);
        pWin->update_secdc = ON_UPDSECDC_DEFAULT;
    }

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
#else
        /* since 5.0.0, always call CreatePrivateClientDC */
        pWin->privCDC = CreatePrivateClientDC ((HWND)pWin);
#endif
    }
    else
        pWin->privCDC = 0;

    // show and active this main window.
    if (pWin->dwStyle & WS_VISIBLE) {
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
#if 0 /* no global ones */
        case ZOF_TYPE_GLOBAL:
            first = __mg_zorder_info->first_global;
            break;
#endif
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

    if (first > 0 && !(nodes [first].flags & ZOF_TF_MAINWIN)
                    && (nodes [first].flags & ZOF_VISIBLE)) {

        RECT rc = nodes [first].rc;
        PMAINWIN pCurTop = (PMAINWIN) nodes [first].hwnd;

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

    if (!(pWin->dwStyle & WS_VISIBLE)) {
        pWin->dwStyle |= WS_VISIBLE;

        dskShowWindow (0, pWin->idx_znode);

        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
        InvalidateRect ((HWND)pWin, NULL, TRUE);
    }
    else {
        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
    }

    // activate this main window.
    if (reason != RCTM_SHOWCTRL)
        dskSetActiveWindow (pWin);
}

/* Since 5.0.0 */
static BOOL dskSetMainWindowAlwaysTop (PMAINWIN pWin, BOOL fSet)
{
    if ((pWin->dwStyle & WS_ALWAYSTOP) && fSet)
        return TRUE;

    if (!(pWin->dwStyle & WS_ALWAYSTOP) && !fSet)
        return TRUE;

    if (dskSetZNodeAlwaysTop (0, pWin->idx_znode, fSet))
        return FALSE;

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
    FreeZOrderMaskRect (0, ((PMAINWIN)pWin)->idx_znode);
    return AllocZOrderMaskRect (0, ((PMAINWIN)pWin)->idx_znode,
            get_znode_flags_from_style ((PMAINWIN)pWin),
            mask_info->rcs, mask_info->nr_rcs);
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
    FreeZOrderNode (0, pWin->idx_znode, NULL);

#if 0   /* move to window.c since 5.0.0 */
    // Handle main window hosting.
    if (pWin->pHosting)
        dskRemoveHostedMainWindow (pWin->pHosting, pWin);
#endif  /* moved code */

    /* houhh 20081127.*/
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

    if (PopupMenuTrackProc (ptmi, MSG_INITMENU, 0, 0))
        return -1;

    ptmi->idx_znode = srvStartTrackPopupMenu (0, &ptmi->rc, (HWND)ptmi,
            0, 0, -1);

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
    RECT rcScr = GetScreenRect();

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
    SelectClipRect (HDC_SCREEN_SYS, &rcScr);

    return 0;
}

#endif /* defiend _MGHAVE_MENU */

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
            if (__mg_captured_wnd &&
                checkAndGetMainWinIfWindow (__mg_captured_wnd) == pWin)
                __mg_captured_wnd = 0;

            if (dskGetActiveWindow () == pWin) {
                dskSetActiveWindow (NULL);
                return;
            }
        }

        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
    }
}

static int dskMoveMainWindow (PMAINWIN pWin, const RECT* prcExpect)
{
    RECT rcResult;

    SendAsyncMessage ((HWND)pWin, MSG_CHANGESIZE,
                    (WPARAM)(prcExpect), (LPARAM)(&rcResult));

    return dskMoveWindow (0, pWin->idx_znode, HDC_INVALID, &rcResult);
}

static int dskMoveGlobalControl (PMAINWIN pCtrl, RECT* prcExpect)
{
    RECT newWinRect, rcResult;
    int ret;

    SendAsyncMessage ((HWND)pCtrl, MSG_CHANGESIZE,
                    (WPARAM)(prcExpect), (LPARAM)(&rcResult));
    dskClientToScreen ((PMAINWIN)(pCtrl->hParent), prcExpect, &newWinRect);

    ret = dskMoveWindow (0, pCtrl->idx_znode, HDC_INVALID, &newWinRect);

    if (ret == 0 && pCtrl->dwStyle & WS_VISIBLE) {
        SendAsyncMessage ((HWND)pCtrl, MSG_NCPAINT, 0, 0);
        InvalidateRect ((HWND)pCtrl, NULL, TRUE);
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
        return (PMAINWIN)(nodes[slot].hwnd);
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
                    get_next_activable_mainwin (__mg_zorder_info,
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

#if 0 /* since 5.0.0, deprecated code */

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

#else   /* since 5.0.0, deprecated code */

#ifndef _MGRM_THREADS
    if (__mg_ime_wnd &&
            message != MSG_SYSKEYDOWN && message != MSG_SYSKEYUP) {
        PostMessage (__mg_ime_wnd, message, (WPARAM)scancode, (LPARAM)status);
        return 0;
    }
#else
    else if (__mg_ime_wnd && __mg_zorder_info->active_win) {
        PostMessage (__mg_ime_wnd,
                message, (WPARAM)scancode, (LPARAM)status);
        return 0;
    }
#endif

    if (__mg_zorder_info->active_win) {
        __mg_post_msg_by_znode (__mg_zorder_info,
                __mg_zorder_info->active_win,
                message, (WPARAM)scancode, (LPARAM)status);
    }
    else {
        SendMessage (HWND_DESKTOP, MSG_DT_KEYOFF + message,
                (WPARAM)scancode, (LPARAM)status);
    }

#endif  /* since 5.0.0: do not handle hook function here */

    return 0;
}

static PMAINWIN _mgs_button_down_main_window = NULL;
static PMAINWIN _mgs_old_under_p = NULL;

#define DOWN_BUTTON_NONE        0x0000
#define DOWN_BUTTON_LEFT        0x0001
#define DOWN_BUTTON_RIGHT       0x0002
#define DOWN_BUTTON_MIDDLE      0x0004
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

    if ((HWND)pWin == __mg_captured_wnd)
        __mg_captured_wnd = 0;

    __mg_reset_mainwin_capture_info ((PCONTROL)pWin);
}

static HWND DesktopSetCapture (HWND hwnd)
{
    HWND hTemp;

    _mgs_old_under_p = NULL;
    _mgs_button_down_main_window = checkAndGetMainWinIfWindow (hwnd);
    _mgs_down_buttons = DOWN_BUTTON_NONE;

    hTemp = __mg_captured_wnd;
    __mg_captured_wnd = hwnd;
    return hTemp;
}

static LRESULT MouseMessageHandler (UINT message, WPARAM flags, int x, int y)
{
    PMAINWIN pUnderPointer;
    PMAINWIN pCtrlPtrIn;

    if (__mg_captured_wnd) {
        PostMessage (__mg_captured_wnd, message,
                        flags | KS_CAPTURED, MAKELONG (x, y));
        return 0;
    }

    pCtrlPtrIn = gui_GetMainWindowPtrUnderPoint (x, y);

#if 0   /* since 5.0.0, do not handle hook function here */
    if (dskHandleMouseHooks ((HWND)pCtrlPtrIn,
                            message, flags, MAKELONG (x, y)) == HOOK_STOP)
        return 0;
#endif  /* since 5.0.0, do not handle hook function here */

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
    case MSG_MBUTTONDOWN:
        if (_mgs_button_down_main_window) {
            PostMessage ((HWND)_mgs_button_down_main_window,
                            message + MSG_DT_MOUSEOFF,
                            flags, MAKELONG (x, y));
            if (message == MSG_LBUTTONDOWN)
                _mgs_down_buttons |= DOWN_BUTTON_LEFT;
            else if (message == MSG_RBUTTONDOWN)
                _mgs_down_buttons |= DOWN_BUTTON_RIGHT;
            else
                _mgs_down_buttons |= DOWN_BUTTON_MIDDLE;
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
            else if (message == MSG_RBUTTONDOWN)
                _mgs_down_buttons = DOWN_BUTTON_RIGHT;
            else
                _mgs_down_buttons = DOWN_BUTTON_MIDDLE;
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
    case MSG_MBUTTONUP:
        if (_mgs_down_buttons == DOWN_BUTTON_LEFT &&
                        message != MSG_LBUTTONUP) {
            break;
        }
        if (_mgs_down_buttons == DOWN_BUTTON_RIGHT &&
                        message != MSG_RBUTTONUP) {
            break;
        }
        if (_mgs_down_buttons == DOWN_BUTTON_MIDDLE &&
                        message != MSG_MBUTTONUP) {
            break;
        }

        if (_mgs_button_down_main_window) {
            PostMessage ((HWND)_mgs_button_down_main_window,
                            message + MSG_DT_MOUSEOFF,
                            flags, MAKELONG (x, y));
            if (message == MSG_LBUTTONUP)
                _mgs_down_buttons &= ~DOWN_BUTTON_LEFT;
            else if (message == MSG_RBUTTONUP)
                _mgs_down_buttons &= ~DOWN_BUTTON_RIGHT;
            else
                _mgs_down_buttons &= ~DOWN_BUTTON_MIDDLE;

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
    case MSG_MBUTTONDBLCLK:
        if (_mgs_button_down_main_window) {
            PostMessage ((HWND)_mgs_button_down_main_window,
                            message + MSG_DT_MOUSEOFF,
                            flags, MAKELONG (x, y));
            if (message == MSG_LBUTTONDBLCLK)
                _mgs_down_buttons |= DOWN_BUTTON_LEFT;
            else if (message == MSG_RBUTTONDBLCLK)
                _mgs_down_buttons |= DOWN_BUTTON_RIGHT;
            else
                _mgs_down_buttons |= DOWN_BUTTON_MIDDLE;
        }
        else if (pUnderPointer) {
            PostMessage ((HWND)pUnderPointer,
                            message + MSG_DT_MOUSEOFF,
                            flags, MAKELONG (x, y));
            _mgs_button_down_main_window = pUnderPointer;
            if (message == MSG_LBUTTONDBLCLK)
                _mgs_down_buttons = DOWN_BUTTON_LEFT;
            else if (message == MSG_RBUTTONDBLCLK)
                _mgs_down_buttons = DOWN_BUTTON_RIGHT;
            else
                _mgs_down_buttons = DOWN_BUTTON_MIDDLE;
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

#else   /* not define _MGRM_THREADS */

static void lock_zorder_info (void)
{
    PMAINWIN pWin = NULL;
    int level, slot;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    for (level = 0; level < NR_ZORDER_LEVELS; level++) {
        slot = __mg_zorder_info->first_in_levels[level];
        for (; slot > 0; slot = nodes[slot].next) {
            pWin = (PMAINWIN)(nodes[slot].hwnd);
            if (pWin)
                pthread_mutex_lock (&pWin->pGCRInfo->lock);
        }
    }

#if 0   /* deprected code */
    static int fixed_slots [] = { ZNIDX_SCREENLOCK, ZNIDX_DOCKER,
        ZNIDX_LAUNCHER };

    /* Since 5.0.0 */
    for (slot = 0; slot < TABLESIZE(fixed_slots); slot++) {
        pWin = (PMAINWIN)(nodes[fixed_slots[slot]].hwnd);
        if (pWin)
            pthread_mutex_lock (&pWin->pGCRInfo->lock);
    }

    slot = __mg_zorder_info->first_topmost;
    for (; slot > 0; slot = nodes[slot].next) {
        pWin = (PMAINWIN)(nodes[slot].hwnd);
        if (pWin)
            pthread_mutex_lock (&pWin->pGCRInfo->lock);
    }

    slot = __mg_zorder_info->first_normal;
    for (; slot > 0; slot = nodes[slot].next) {
        pWin = (PMAINWIN)(nodes[slot].hwnd);
        if (pWin)
            pthread_mutex_lock (&pWin->pGCRInfo->lock);
    }
#endif  /* deprected code */
}

static void unlock_zorder_info (void)
{
    PMAINWIN pWin;
    int level, slot;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    for (level = 0; level < NR_ZORDER_LEVELS; level++) {
        slot = __mg_zorder_info->first_in_levels[level];
        for (; slot > 0; slot = nodes[slot].next) {
            pWin = (PMAINWIN)(nodes[slot].hwnd);
            if (pWin)
                pthread_mutex_unlock (&pWin->pGCRInfo->lock);
        }
    }
}

#endif  /* define _MGRM_THREADS */

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
        _WRN_PRINTF ("bad location\n");
        break;
    }

    SetPenColor (HDC_SCREEN_SYS, PIXEL_lightwhite);
    FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);
    SyncUpdateDC (HDC_SCREEN_SYS);
    return 0;
}

static int dskCancelDragWindow (PMAINWIN pWin)
{
    RECT rcScr = GetScreenRect();

    if (!(pWin->dwStyle & WS_VISIBLE))
        return -1;

    if (_dd_info.hwnd == 0 || _dd_info.hwnd != (HWND)pWin)
        return -1;

    _dd_info.hwnd = (HWND)-1;
    unlock_zorder_info ();
    SelectClipRect (HDC_SCREEN_SYS, &rcScr);
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
        SyncUpdateDC (HDC_SCREEN_SYS);

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

        FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);
        SyncUpdateDC (HDC_SCREEN_SYS);

        _dd_info.last_x = x;
        _dd_info.last_y = y;
    }
    else {
        RECT rcScr = GetScreenRect();

        SetPenColor (HDC_SCREEN_SYS, PIXEL_lightwhite);
        FocusRect (HDC_SCREEN_SYS, _dd_info.rc.left, _dd_info.rc.top,
                _dd_info.rc.right, _dd_info.rc.bottom);
        SyncUpdateDC (HDC_SCREEN_SYS);

        /* post MSG_WINDOWDROPPED to the target window */
        SendNotifyMessage (_dd_info.hwnd, MSG_WINDOWDROPPED,
                          MAKELONG (_dd_info.rc.left, _dd_info.rc.top),
                          MAKELONG (_dd_info.rc.right, _dd_info.rc.bottom));
        unlock_zorder_info ();
        SelectClipRect (HDC_SCREEN_SYS, &rcScr);

        if (_dd_info.hwnd != (HWND)gui_GetMainWindowPtrUnderPoint (x, y))
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
            lRet = (LRESULT)dskMoveGlobalControl (pWin, (RECT*)lParam);
        else {
            lRet = (LRESULT)dskMoveMainWindow (pWin, (RECT*)lParam);
        }
        return lRet;

    case MSG_GETACTIVEMAIN:
        return (LRESULT)dskGetActiveWindow();

    case MSG_SETACTIVEMAIN:
        lRet = (LRESULT)dskSetActiveWindow (pWin);
        return lRet;

    case MSG_GETCAPTURE:
        return (LRESULT)__mg_captured_wnd;

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

    case MSG_SETWINCURSOR: {
        HCURSOR old = pWin->hCursor;

        pWin->hCursor = (HCURSOR)lParam;
        return (LRESULT)old;
    }

    case MSG_GETNEXTMAINWIN: {
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

            if ((pWin = checkAndGetMainWinIfMainWin (hwnd)) &&
                    !(pWin->dwExStyle & WS_EX_CTRLASMAINWIN)) {
                break;
            }

            from = __kernel_get_next_znode (zi, from);
        }

        unlock_zi_for_read (zi);
        return (LRESULT)hwnd;
    }

    case MSG_DUMPZORDER:
         dskDumpZOrder (__mg_zorder_info);
         break;

    /* Since 5.0.0 */
    case MSG_SETAUTOREPEAT:
        sg_msgAutoRepeat = *(const MSG*)lParam;
        break;

    /* Since 5.0.0 */
    case MSG_SETALWAYSTOP:
        return dskSetMainWindowAlwaysTop(pWin, (BOOL)lParam);

    /* Since 5.0.0 */
    case MSG_SETWINDOWMASK:
        return dskSetWindowMask (pWin, (WINMASKINFO*)lParam);

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
    int level, slot;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    hWinMenu = GetSubMenu (hDesktopMenu, 3);
    nCount = GetMenuItemCount (hWinMenu);

    for (iPos = nCount; iPos > 0; iPos--)
        DeleteMenu (hWinMenu, iPos - 1, MF_BYPOSITION);

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;

    id = IDM_FIRSTWINDOW;
    iPos = 0;

    for (level = 0; level < NR_ZORDER_LEVELS; level++) {
        BOOL inserted;

        inserted = FALSE;
        slot = __mg_zorder_info->first_in_levels[level];
        for (; slot > 0; slot = nodes[slot].next) {
            pWin = (PMAINWIN)(nodes[slot].hwnd);
            if (pWin && pWin->WinType == TYPE_MAINWIN) {
                if (nodes[slot].flags & ZOF_VISIBLE &&
                        !(nodes[slot].flags & ZOF_DISABLED))
                    mii.state       = MFS_ENABLED;
                else
                    mii.state       = MFS_DISABLED;
            }
            else {
                continue;
            }

            mii.type            = MFT_STRING;
            mii.id              = id;
            mii.typedata        = (DWORD)pWin->spCaption;
            mii.itemdata        = (DWORD)pWin;
            InsertMenuItem (hWinMenu, iPos, TRUE, &mii);

            id++;
            iPos++;

            inserted = TRUE;
        }

        if (inserted && level < (NR_ZORDER_LEVELS - 1) &&
                __mg_zorder_info->first_in_levels[level+1] > 0) {
            mii.type            = MFT_SEPARATOR;
            mii.state           = 0;
            mii.id              = 0;
            mii.typedata        = 0;
            InsertMenuItem(hWinMenu, iPos, TRUE, &mii);
            iPos ++;
        }
    }

    nCount = GetMenuItemCount (hDesktopMenu);
    for (iPos = nCount; iPos > 5; iPos --)
        DeleteMenu (hDesktopMenu, iPos - 1, MF_BYPOSITION);

    dsk_ops->customize_desktop_menu (dt_context, hDesktopMenu, 5);
}
#endif

static void close_all_main_window (void)
{
    int level, slot;
    PMAINWIN pWin;
    ZORDERNODE* nodes = GET_ZORDERNODE(__mg_zorder_info);

    for (level = 0; level < NR_ZORDER_LEVELS; level++) {
        slot = __mg_zorder_info->first_in_levels[level];
        for (; slot > 0; slot = nodes[slot].next) {
            pWin = (PMAINWIN)(nodes[slot].hwnd);
            if (pWin && (pWin->WinType == TYPE_MAINWIN))
                SendNotifyMessage ((HWND)pWin, MSG_CLOSE, 0, 0);
        }
    }
}

static int dskDesktopCommand (HMENU hDesktopMenu, int id)
{
    if (id == IDM_REDRAWBG)
        SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
    else if (id == IDM_CLOSEALLWIN) {
        close_all_main_window ();
    }
    else if (id == IDM_ENDSESSION) {
#ifdef _MGHAVE_VIRTUAL_WINDOW
        //close_all_main_window ();
        post_quit_to_all_message_threads (TRUE);
#else
        SendNotifyMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
#endif  /* defined _MGHAVE_VIRTUAL_WINDOW */
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
            _WRN_PRINTF ("Failed to allocate znode for control.\n");
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
        FreeZOrderNode (0, pCtrl->idx_znode, NULL);
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

    pTemp = (PMAINWIN)node->hwnd;

    if (pTemp
            && ((pTemp->WinType == TYPE_CONTROL &&
                    (pTemp->dwExStyle & WS_EX_CTRLASMAINWIN))
                || pTemp->WinType != TYPE_CONTROL)
            && pTemp->dwStyle & WS_VISIBLE) {
        if (info->is_empty_invrc) {
            SendAsyncMessage ((HWND)pTemp, MSG_NCPAINT, 0, 0);
            InvalidateRect ((HWND)pTemp, NULL, TRUE);
        }
        else {
            RECT rcTemp, rcInv, rcWin;

            if (pTemp->WinType == TYPE_CONTROL &&
                    (pTemp->dwExStyle & WS_EX_CTRLASMAINWIN)){
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

    if (!checkAndGetMainWinIfMainWin (hwnd))
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

void GUIAPI DesktopUpdateAllWindow(void)
{
    SendMessage(HWND_DESKTOP, MSG_PAINT, 0, 0);
}

static void dskOnTimer (void)
{
    static DWORD uCounter = 0;
    static DWORD blink_counter = 0;

    if (sg_msgAutoRepeat.hwnd != 0) {
        PostMessage (sg_msgAutoRepeat.hwnd, sg_msgAutoRepeat.message,
                sg_msgAutoRepeat.wParam, sg_msgAutoRepeat.lParam);
    }

    if (__mg_tick_counter < (blink_counter + 10))
        return;

    uCounter += (__mg_tick_counter - blink_counter) * 10;
    blink_counter = __mg_tick_counter;

    if (sg_hCaretWnd != 0 &&
            checkAndGetMainWinIfWindow (sg_hCaretWnd) ==
                dskGetActiveWindow() &&
            uCounter >= sg_uCaretBTime) {
        PostMessage (sg_hCaretWnd, MSG_CARETBLINK, 0, 0);
        uCounter = 0;
    }
}

static LRESULT DesktopWinProc (HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam)
{
    static HDC hDesktopDC;
    WPARAM flags;
    int x, y;
    PMAINWIN active_mainwnd = dskGetActiveWindow();

    if (message >= MSG_FIRSTWINDOWMSG && message <= MSG_LASTWINDOWMSG)
        return WindowMessageHandler (message, (PMAINWIN)wParam, lParam);

    if (message == MSG_CHAR && active_mainwnd) {
        PostMessage ((HWND)active_mainwnd, message, wParam, lParam);
    }
    // VW: Since 4.0.0 for extra input messages.
    else if (message >= MSG_FIRSTEXTRAINPUTMSG &&
            message <= MSG_LASTEXTRAINPUTMSG) {

        if (dskPreExtraMessageHandler (message, wParam, lParam))
            return 0;

        if (do_drag_drop_window (message, 0, 0))
            return 0;

        PostMessage ((HWND)active_mainwnd, message, wParam, lParam);
    }
    else if (message >= MSG_FIRSTKEYMSG && message <= MSG_LASTKEYMSG) {

        if (dskPreKeyMessageHandler (message, wParam, lParam))
            return 0;

        if (do_drag_drop_window (message, 0, 0))
            return 0;

        if (wParam == SCANCODE_PRINTSCREEN && message == MSG_KEYDOWN) {
#ifdef _MGMISC_SAVESCREEN
            static int n = 1;
            char buffer[64]; // VM: 20 is too small for file name

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

        if (dskPreMouseMessageHandler (message, wParam, lParam))
            return 0;

        flags = wParam;

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
        else {
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
        if (dsk_ops->init)
            dt_context = dsk_ops->init (hDesktopDC);
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

    case MSG_REINITDESKOPS:
        if (dsk_ops->init) {
            dt_context = dsk_ops->init (hDesktopDC);
            SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
        }
        break;

    case MSG_ENDSESSION:
        __mg_screensaver_destroy ();

        if (hDesktopDC)
            ReleaseDC (hDesktopDC);
        hDesktopDC = 0;

#ifdef _MGHAVE_MENU
        if (sg_DesktopMenu) {
            DestroyMenu (sg_DesktopMenu);
            sg_DesktopMenu = 0;
        }
#endif
        if (dsk_ops->deinit)
            dsk_ops->deinit (dt_context);

        PostQuitMessage (HWND_DESKTOP);
        return 1;

    case MSG_ERASEDESKTOP:
        if (dsk_ops->paint_desktop)
            dsk_ops->paint_desktop(dt_context, hDesktopDC, (PRECT)lParam);
        break;

#ifndef _MGRM_THREADS
    case MSG_SETFOCUS: {
        HWND active_win = (HWND)dskGetActiveWindow();
        if (active_win) {
            SendNotifyMessage ((HWND)active_win, MSG_SETFOCUS, 0, 0);
        }
        break;
    }
#endif

    case MSG_PAINT: {
        RECT invrc;

        invrc.left = LOWORD(wParam);
        invrc.top = HIWORD(wParam);
        invrc.right = LOWORD(lParam);
        invrc.bottom = HIWORD(lParam);

        dskRefreshAllWindow (&invrc);
        break;
    }

    case MSG_BROADCASTMSG:
        return dskBroadcastMessage ((PMSG)lParam);

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
        return (LRESULT)dskRegisterHookWin (0, (HWND)wParam, (DWORD)lParam);

    /* Since 5.0.0 */
    case MSG_UNREGISTERHOOKWIN:
        return (LRESULT)dskUnregisterHookWin (0, (HWND)wParam);

#ifdef _MGHAVE_VIRTUAL_WINDOW
    /* Since 5.0.0 */
    case MSG_MANAGE_MSGTHREAD:
        if (wParam == MSGTHREAD_SIGNIN)
            return dskRegisterMsgQueue ((MSGQUEUE*)lParam);
        return dskUnregisterMsgQueue ((MSGQUEUE*)lParam);
#endif

    /* Since 5.0.0 */
    case MSG_CALC_POSITION:
        dskCalculateDefaultPosition (0, (CALCPOSINFO*)lParam);
        return 0;

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

#if 0
    case MSG_SRVNOTIFY:
        BroadcastMessage (MSG_SRVNOTIFY, wParam, lParam);
        break;
#endif

    case MSG_TIMEOUT:
        // Since 5.0.0: MSG_IDLE messages will be generated by PeekMessage.
        // BroadcastMessageInThisThread (MSG_IDLE, wParam, 0);
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
    case MSG_DT_MBUTTONDOWN:
    case MSG_DT_MBUTTONDBLCLK:
    case MSG_DT_MBUTTONUP:
        if(dsk_ops->mouse_handler)
            dsk_ops->mouse_handler(dt_context, message, wParam, lParam);
        break;

    /* Since 5.0.0, the desktop only handles caret blinking in MSG_TIMER
       message, and the interval for this MSG_TIMER changes to about 0.05s. */
    case MSG_TIMER:
        dskOnTimer ();
        break;

    default:
        break;
    }

    return 0;
}

#endif  /* defined(_MGRM_THREADS) || defined(_MGRM_STANDALONE) */

