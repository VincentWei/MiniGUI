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
 ** window.c: The Window module.
 **
 ** Create date: 1999.04.19
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "menu.h"
#include "ctrlclass.h"
#include "element.h"
#include "dc.h"
#ifdef _MGRM_PROCESSES
#include "client.h"
#endif

#include "license.h"
#ifdef _MG_ENABLE_LICENSE
#   include "../sysres/license/c_files/04_progressbar-bk.dat.c"
#endif
#ifndef __TARGET_UNKNOWN__
unsigned int g_license_processor_id;
#endif

#ifdef _MGHAVE_MENU
/* function defined in menu module. */
void DrawMenuBarHelper (const MAINWIN *pWin, HDC hdc, const RECT* pClipRect);
#endif

/* functions defined in caret module. */
BOOL BlinkCaret (HWND hWnd);
void GetCaretBitmaps (PCARETINFO pCaretInfo);

static void RecalcScrollInfo (PMAINWIN pWin, BOOL bIsHBar);
/* this message will auto-repeat when MSG_IDLE received */
static MSG sg_repeat_msg = {0, 0, 0, 0};

void GUIAPI SetAutoRepeatMessage (HWND hwnd, UINT msg, 
        WPARAM wParam, LPARAM lParam)
{
    PMAINWIN pWin = (PMAINWIN)hwnd;

    /* if hwnd is zero, disable repeate message */
    if (hwnd == 0) {
        sg_repeat_msg.hwnd = 0;
        sg_repeat_msg.message  = 0;
        sg_repeat_msg.wParam = 0;
        sg_repeat_msg.lParam = 0;
        return;
    }
    else if (hwnd == HWND_INVALID || pWin->DataType != TYPE_HWND)
        return;

    sg_repeat_msg.hwnd = hwnd;
    sg_repeat_msg.message  = msg;
    sg_repeat_msg.wParam = wParam;
    sg_repeat_msg.lParam = lParam;
}

static void RecalcClientArea (HWND hWnd)
{
    PMAINWIN pWin = (PMAINWIN)hWnd;
    RECT rcWin, rcClient;
    int win_w, win_h, dc_w, dc_h;
    PDC secondary_pdc;

    memcpy (&rcWin, &pWin->left, sizeof (RECT));
    memcpy (&rcClient, &pWin->cl, sizeof (RECT));

    if (SendAsyncMessage (hWnd, MSG_SIZECHANGED, 
                (WPARAM)&rcWin, (LPARAM)&rcClient))
        memcpy (&pWin->cl, &rcClient, sizeof(RECT));

    if ((pWin->WinType == TYPE_MAINWIN) && pWin->secondaryDC) {
        win_w = RECTWP((const RECT*)&pWin->left);
        win_h = RECTHP((const RECT*)&pWin->left);
        secondary_pdc = dc_HDC2PDC (pWin->secondaryDC);
        dc_w = RECTW (secondary_pdc->DevRC);
        dc_h = RECTH (secondary_pdc->DevRC);

        if ((win_w > dc_w) || (win_h > dc_h)) {
            DeleteSecondaryDC (hWnd);
            pWin->secondaryDC = CreateSecondaryDC ((HWND)pWin);

            if (pWin->secondaryDC == HDC_INVALID) {
                /* remove the flag of WS_EX_AUTOSECONDARYDC */
                pWin->dwExStyle = pWin->dwExStyle | WS_EX_AUTOSECONDARYDC;
                pWin->secondaryDC = 0;
            }

            /* update the privCDC of pWin and child window. */
            if (pWin->dwExStyle & WS_EX_USEPRIVATECDC) {
                PCONTROL pNext;
                PCONTROL pCtrl = (PCONTROL)(pWin->hFirstChild);
                while (pCtrl) {
                    pNext = pCtrl->next;
                    if (pCtrl->dwExStyle & WS_EX_USEPRIVATECDC) {
                        ReleaseSecondarySubDC (pCtrl->privCDC);
                        pCtrl->privCDC = GetSecondarySubDC (pWin->secondaryDC, 
                                (HWND)pCtrl, TRUE);
                    }
                    pCtrl = pNext;
                }
                ReleaseSecondarySubDC (pWin->privCDC);
                pWin->privCDC = GetSecondarySubDC (pWin->secondaryDC, 
                        (HWND)pWin, TRUE);
            }
        }
    } 

    SendNotifyMessage (hWnd, MSG_CSIZECHANGED, 
            pWin->cr - pWin->cl, pWin->cb - pWin->ct);
}

static PCONTROL wndMouseInWhichControl (PMAINWIN pWin, int x, int y, 
        int* UndHitCode)
{
    PCONTROL pCtrl;
    int hitcode;
    RECT rect;
    unsigned short idx;
    MASKRECT *maskrect;

    pCtrl = (PCONTROL)pWin->hPrimitive;
    if (pCtrl) {
        if (pCtrl->primitive) {
            if (UndHitCode)
                *UndHitCode = HT_CLIENT;
            return pCtrl;
        }
        else {
            hitcode = SendAsyncMessage ((HWND)pCtrl, MSG_HITTEST, 
                    (WPARAM)x, (LPARAM)y);
            if (hitcode != HT_OUT && hitcode != HT_TRANSPARENT) {
                if (UndHitCode)
                    *UndHitCode = hitcode;
                return pCtrl;
            }
        }
    }

    for (pCtrl = (PCONTROL)(pWin->hFirstChild); pCtrl && pCtrl->next; pCtrl = pCtrl->next);
    while (pCtrl) {
        /* WS_EX_CRLASMAINWIN is also need to check.*/
        //if (pCtrl->dwStyle & WS_VISIBLE &&
        //        !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN))
        if (pCtrl->dwStyle & WS_VISIBLE) {
            maskrect = pCtrl->mask_rects;
            if (!maskrect) {
                rect.left   = pCtrl->left;
                rect.top    = pCtrl->top;
                rect.right  = pCtrl->right;
                rect.bottom = pCtrl->bottom;
                if (PtInRect (&rect, x, y)) {
                    hitcode = SendAsyncMessage((HWND)pCtrl, MSG_HITTEST, 
                            (WPARAM)x, (LPARAM)y);
                    if (hitcode != HT_OUT && hitcode != HT_TRANSPARENT) {
                        if (UndHitCode) {
                            *UndHitCode = hitcode;
                        }
                        return pCtrl;
                    }
                }
            }
            else {
                idx = 0;
                do {
                    rect.left   = (maskrect + idx)->left + pCtrl->left;
                    rect.top    = (maskrect + idx)->top + pCtrl->top;
                    rect.right  = (maskrect + idx)->right + pCtrl->left;
                    rect.bottom = (maskrect + idx)->bottom + pCtrl->top;
                    if (PtInRect (&rect, x, y)) {
                        hitcode = SendAsyncMessage((HWND)pCtrl, MSG_HITTEST, 
                                (WPARAM)x, (LPARAM)y);
                        if (hitcode != HT_OUT && hitcode != HT_TRANSPARENT) {
                            if (UndHitCode) {
                                *UndHitCode = hitcode;
                            }
                            return pCtrl;
                        }

                        /** ignore this control and check the next */
                        break;
                    }

                    idx = (maskrect + idx)->next;
                } while (idx != 0);
            }
        }
        pCtrl = pCtrl->prev;
    }

    return NULL;
}

HWND GUIAPI GetWindowUnderCursor (void)
{
    POINT pt;
    PCONTROL pCtrl, pchild;
    int cx, cy;

    GetCursorPos (&pt);

    pCtrl = (PCONTROL)gui_GetMainWindowPtrUnderPoint (pt.x, pt.y);
    if (!pCtrl) return 0;

    do {
        cx = pt.x; cy = pt.y;
        ScreenToClient ((HWND)pCtrl, &cx, &cy);
        pchild = wndMouseInWhichControl ((PMAINWIN)pCtrl, cx, cy, NULL);
        if (!pchild) break;
        pCtrl = pchild;
    } while (1);

    return (HWND)pCtrl;
}

HWND GUIAPI WindowFromPointEx (POINT pt, BOOL bRecursion)
{
    HWND hChild; 
    HWND hParent = (HWND)gui_GetMainWindowPtrUnderPoint (pt.x, pt.y);

    if (hParent && bRecursion) {
        ScreenToClient(hParent, &pt.x, &pt.y);
        hChild = ChildWindowFromPointEx(hParent, pt, 
                CWP_SKIPINVISIBLE | CWP_SKIPDISABLED);

        while (hChild != HWND_NULL) {
            ClientToScreen(hParent, &pt.x, &pt.y);
            ScreenToClient(hChild, &pt.x, &pt.y);

            hParent = hChild;
            hChild = ChildWindowFromPointEx(hParent, pt, 
                    CWP_SKIPINVISIBLE | CWP_SKIPDISABLED);
            //TODO:not support static control
        }

        return IsWindow(hChild)?hChild:hParent;
    }
    else
         return hParent;

    return HWND_NULL;
}

//should found the above(last created) control, not previous created window.
HWND GUIAPI ChildWindowFromPointEx (HWND hParent, POINT pt, UINT uFlags)
{
    PMAINWIN pWin;
    PCONTROL pCtrl; 
    HWND pRet = HWND_NULL;
    RECT rect;
    unsigned short idx;
    MASKRECT *maskrect;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hParent), HWND_NULL);
    pWin = MG_GET_WINDOW_PTR (hParent);

    pCtrl = (PCONTROL)(pWin->hFirstChild);
    while (pCtrl) {
        if ((uFlags & CWP_SKIPINVISIBLE) && !(pCtrl->dwStyle & WS_VISIBLE)) {
            pCtrl = pCtrl->next;
            continue;
        }
        if ((uFlags & CWP_SKIPDISABLED) && (pCtrl->dwStyle & WS_DISABLED)) {
            pCtrl = pCtrl->next;
            continue;
        }
        if ((uFlags & CWP_SKIPTRANSPARENT) && (pCtrl->dwExStyle & WS_EX_TRANSPARENT)) {
            pCtrl = pCtrl->next;
            continue;
        }

        maskrect = pCtrl->mask_rects;
        if (!maskrect) {
            rect.left   = pCtrl->left;
            rect.top    = pCtrl->top;
            rect.right  = pCtrl->right;
            rect.bottom = pCtrl->bottom;
            if (PtInRect (&rect, pt.x, pt.y)) {
                pRet = (HWND)pCtrl;
                pCtrl = pCtrl->next;
                continue;
            }
        }
        else {
            idx = 0;
            do {
                rect.left   = (maskrect + idx)->left + pCtrl->left;
                rect.top    = (maskrect + idx)->top + pCtrl->top;
                rect.right  = (maskrect + idx)->right + pCtrl->left;
                rect.bottom = (maskrect + idx)->bottom + pCtrl->top;
                if (PtInRect (&rect, pt.x, pt.y)) {
                    pRet = (HWND)pCtrl;
                    break;
                }

                idx = (maskrect + idx)->next;
            } while (idx != 0);
        }

        pCtrl = pCtrl->next;
    }

    return pRet;
}

/* NOTE:
 ** this function is CONTROL mouse messages handler,
 ** can automaticly capture mouse depend on HITTEST code.
 */

static PCONTROL __mgs_captured_ctrl = NULL;

void __mg_reset_mainwin_capture_info (PCONTROL ctrl)
{
    if ((ctrl == NULL || ctrl == __mgs_captured_ctrl)
            || (__mgs_captured_ctrl 
                && __mgs_captured_ctrl->pMainWin == (PMAINWIN)ctrl)) {
        POINT mousePos;

        ReleaseCapture ();
        __mgs_captured_ctrl = NULL;

        GetCursorPos (&mousePos);
        PostMessage (HWND_DESKTOP, MSG_MOUSEMOVE, 
                0, MAKELONG (mousePos.x, mousePos.y));
    }
}

static LRESULT DefaultDTMouseMsgHandler (PMAINWIN pWin, UINT message, 
        WPARAM flags, int x, int y)
{
    int hc_mainwin = HT_UNKNOWN;
    int cx, cy;
    PCONTROL pCtrl = NULL;

    if (pWin->dwStyle & WS_DISABLED)
        return 0;

    cx = x - pWin->cl;
    cy = y - pWin->ct;

    /* houhh 20081023, BUGFIX for can not click combox's listbox 
     * out of Mainwin.*/
    pCtrl = wndMouseInWhichControl (pWin, cx, cy, &hc_mainwin);
    if (pCtrl == NULL) {
        hc_mainwin = SendAsyncMessage((HWND)pWin, MSG_HITTEST, 
                    (WPARAM)x, (LPARAM)y);
        pCtrl = (PCONTROL)pWin;
    }
    else {
        hc_mainwin = HT_CLIENT;
    }

    /* houhh 20080804, if click on  scrollbar of mainwin's control, then
     * need to change hc_mainwin, else click on scrollbar of mainwin, 
     * hc_mainwin can not change to HT_CLIENT.*/

    if (pCtrl && pCtrl->WinType == TYPE_CONTROL &&
            (hc_mainwin >= HT_SB_LEFTARROW &&
             hc_mainwin <= HT_SB_VTHUMB))
        hc_mainwin = HT_CLIENT;

    switch (message) {
        case MSG_DT_MOUSEMOVE:
            if (hc_mainwin == HT_CLIENT)
            {
                PostMessage ((HWND)pWin, MSG_SETCURSOR, 
                        0, MAKELONG (cx, cy));
            }else{
                PostMessage ((HWND)pWin, MSG_NCSETCURSOR, 
                        hc_mainwin, MAKELONG (cx, cy));
            }
        case MSG_DT_LBUTTONDBLCLK:
        case MSG_DT_RBUTTONDBLCLK:
            if (hc_mainwin == HT_CLIENT) {
                PostMessage((HWND)pWin, 
                        message + (MSG_NCMOUSEOFF - MSG_DT_MOUSEOFF),
                        hc_mainwin, MAKELONG (x, y));
                PostMessage((HWND)pWin, 
                        message - MSG_DT_MOUSEOFF,
                        flags, MAKELONG (cx, cy));
            }
            else {
                PostMessage((HWND)pWin, 
                        message + (MSG_NCMOUSEOFF - MSG_DT_MOUSEOFF),
                        hc_mainwin, MAKELONG (x, y));
            }
            break;

        case MSG_DT_LBUTTONDOWN:
        case MSG_DT_RBUTTONDOWN:
            if (hc_mainwin != HT_CLIENT) {
                PostMessage ((HWND)pWin, 
                        message + (MSG_NCMOUSEOFF - MSG_DT_MOUSEOFF),
                        hc_mainwin, MAKELONG (x, y));
            }
            else 
            {
                PostMessage((HWND)pWin,
                        message - MSG_DT_MOUSEOFF,
                        flags, MAKELONG(cx, cy));
            }
            break;

        case MSG_DT_LBUTTONUP:
        case MSG_DT_RBUTTONUP:
            if (hc_mainwin == HT_CLIENT) {
                PostMessage((HWND)pWin, 
                        message - MSG_DT_MOUSEOFF,
                        flags, MAKELONG(cx, cy));
            }
            else {
                PostMessage ((HWND)pWin, 
                        message + (MSG_NCMOUSEOFF - MSG_DT_MOUSEOFF),
                        hc_mainwin, MAKELONG (x, y));
            }
            break;

    }

    return 0;
}

static LRESULT DefaultMouseMsgHandler (PMAINWIN pWin, UINT message, 
        WPARAM flags, int x, int y)
{
    static PMAINWIN __mgs_captured_win = NULL;
    PCONTROL pUnderPointer;
    int CapHitCode = HT_UNKNOWN;
    int UndHitCode = HT_UNKNOWN;
    int cx = 0, cy = 0;

    if (__mgs_captured_ctrl) {
        /* convert to parent window's client coordinates. */
        ScreenToClient ((HWND)__mgs_captured_win, &x, &y);

        CapHitCode = SendAsyncMessage((HWND)__mgs_captured_ctrl, MSG_HITTEST,
                (WPARAM)x, (LPARAM)y);

        pUnderPointer = NULL;
    }
    else {
        pUnderPointer = wndMouseInWhichControl (pWin, x, y, &UndHitCode);
        if (pUnderPointer && (pUnderPointer->dwStyle & WS_DISABLED))
            pUnderPointer = NULL;


        if (pUnderPointer) {
            cx = x - pUnderPointer->cl;
            cy = y - pUnderPointer->ct;
        }
    }

    switch (message) {
        case MSG_MOUSEMOVE:
            if (__mgs_captured_ctrl)
                PostMessage((HWND)__mgs_captured_ctrl, MSG_NCMOUSEMOVE, 
                        CapHitCode, MAKELONG (x, y));
            else {
                if (pWin->hOldUnderPointer != (HWND)pUnderPointer) {
                    if (pWin->hOldUnderPointer) {
                        PostMessage ((HWND)pWin->hOldUnderPointer,
                                MSG_MOUSEMOVEIN, FALSE, (LPARAM)pUnderPointer);
                        PostMessage ((HWND)pWin->hOldUnderPointer,
                                MSG_NCMOUSEMOVE, HT_OUT, MAKELONG (x, y));
                    }

                    if (pUnderPointer)
                        PostMessage ((HWND)pUnderPointer, MSG_MOUSEMOVEIN, 
                                TRUE, (LPARAM)pWin->hOldUnderPointer);

                    pWin->hOldUnderPointer = (HWND)pUnderPointer;
                }

                if (pUnderPointer == NULL) {
                    pWin->hOldUnderPointer = 0;
                    break;
                }

                if (pUnderPointer->dwStyle & WS_DISABLED) {
                    SetCursor (GetSystemCursor (IDC_ARROW));
                    break;
                }

                if (UndHitCode == HT_CLIENT) {
                    PostMessage ((HWND)pUnderPointer,
                            MSG_SETCURSOR, 0, MAKELONG (cx, cy));

                    PostMessage((HWND)pUnderPointer, MSG_NCMOUSEMOVE, 
                            UndHitCode, MAKELONG (x, y));
                    PostMessage((HWND)pUnderPointer, MSG_MOUSEMOVE, 
                            flags, MAKELONG (cx, cy));
                }
                else
                {
                    PostMessage((HWND)pUnderPointer, MSG_NCSETCURSOR, 
                            UndHitCode, MAKELONG (x, y));
                    PostMessage((HWND)pUnderPointer, MSG_NCMOUSEMOVE, 
                            UndHitCode, MAKELONG (x, y));
                }
            }
            break;

        case MSG_LBUTTONDOWN:
        case MSG_RBUTTONDOWN:
            if (pUnderPointer) {
                if (pUnderPointer->dwStyle & WS_DISABLED) {
                    Ping ();
                    break;
                }

                SendNotifyMessage ((HWND) pUnderPointer, 
                        MSG_MOUSEACTIVE, UndHitCode, 0);

                if (UndHitCode != HT_CLIENT) {
                    if (UndHitCode & HT_NEEDCAPTURE
                            && message == MSG_LBUTTONDOWN) {
                        SetCapture ((HWND)pWin);
                        __mgs_captured_win = pWin;
                        __mgs_captured_ctrl = pUnderPointer;
                    }
                    else
                        __mgs_captured_ctrl = NULL;

                    PostMessage ((HWND)pUnderPointer, message + MSG_NCMOUSEOFF,
                            UndHitCode, MAKELONG (x, y));
                }
                else if (__mgs_captured_ctrl == NULL) {
                    PostMessage((HWND)pUnderPointer, message, 
                            flags, MAKELONG(cx, cy));
                }
            }
            else if (pWin->hActiveChild) {
                SendNotifyMessage (pWin->hActiveChild, 
                        MSG_MOUSEACTIVE, HT_OUT, 0);
                PostMessage (pWin->hActiveChild, message + MSG_NCMOUSEOFF, 
                        HT_OUT, MAKELONG(x, y));
            }
            break;

        case MSG_RBUTTONUP:
        case MSG_LBUTTONUP:
            if (__mgs_captured_ctrl && message == MSG_LBUTTONUP) {
                PostMessage ((HWND)__mgs_captured_ctrl, 
                        message + MSG_NCMOUSEOFF, 
                        CapHitCode, MAKELONG (x, y));
                ReleaseCapture ();
                __mgs_captured_win = NULL;
                __mgs_captured_ctrl = NULL;
            }
            else if (pUnderPointer) {
                if (pUnderPointer->dwStyle & WS_DISABLED) {
                    break;
                }

                if (UndHitCode == HT_CLIENT)
                    PostMessage((HWND)pUnderPointer, message, 
                            flags, MAKELONG (cx, cy));
                else
                    PostMessage((HWND)pUnderPointer, message + MSG_NCMOUSEOFF, 
                            UndHitCode, MAKELONG (x, y));
            }
            else if (pWin->hActiveChild) {
                PostMessage(pWin->hActiveChild, message + MSG_NCMOUSEOFF, 
                        HT_OUT, MAKELONG(x, y));
            }
            break;

        case MSG_LBUTTONDBLCLK:
        case MSG_RBUTTONDBLCLK:
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
                if (pWin->hActiveChild) {
                    PostMessage(pWin->hActiveChild, message + MSG_NCMOUSEOFF, 
                            HT_OUT, MAKELONG(x, y));
                }
            }
            break;

    }

    return 0;
}

BOOL wndGetVScrollBarRect (const MAINWIN* pWin, 
        RECT* rcVBar)
{
    if (pWin->dwStyle & WS_VSCROLL) {
        if (!GetWindowInfo((HWND)pWin)->we_rdr->calc_we_area ( 
                    (HWND)pWin, HT_VSCROLL, rcVBar))
            return TRUE;
    }

    return FALSE;
}

BOOL wndGetHScrollBarRect (const MAINWIN* pWin, 
        RECT* rcHBar)
{
    if (pWin->dwStyle & WS_HSCROLL) {
        if (!GetWindowInfo((HWND)pWin)->we_rdr->calc_we_area ( 
                    (HWND)pWin, HT_HSCROLL, rcHBar))
            return TRUE;
    }

    return FALSE;

}

/* rc: position relative to secondary_dc, so need transform 
 * it  relative to real_dc(screen).*/
void update_secondary_dc (PMAINWIN pWin, HDC secondary_dc, 
        HDC real_dc, const RECT* rc, DWORD flags)
{
    PDC pdc_main;
    RECT wnd_rc, clip_rc, client_rc;

    if (!dc_IsVisible((PDC)secondary_dc))
        return;
    if (!pWin->pMainWin->secondaryDC) {
        return;
    }

    pdc_main  = dc_HDC2PDC(real_dc);

    /* calculate offset postion. */
    if (flags == HT_CLIENT){
        gui_WndClientRect ((HWND)pWin, &wnd_rc);
    }
    else {
        gui_WndRect ((HWND)pWin, &wnd_rc);
    }

    /* 1. clip_rc.left/clip_rc.top is real_rc update in the screen (coordinate relative
     *    to screen). 
     * 2. clip_rc.left/clip_rc.top need translate rc to relative real_dc's DevRC(BitBlt). 
     * */
    clip_rc.left = wnd_rc.left + rc->left - pdc_main->DevRC.left;
    clip_rc.top = wnd_rc.top  + rc->top  - pdc_main->DevRC.top;
    clip_rc.right  = clip_rc.left + RECTWP(rc);
    clip_rc.bottom = clip_rc.top  + RECTHP(rc);

    IncludeClipRect (real_dc, &clip_rc);

    /* exclude client rect. get non-client region.*/
    if (flags == HT_BORDER) {
        CopyRect (&client_rc, (const RECT*)&pWin->cl);
        /* houhh 20090804, should be convert client_rc to relative pMainWin.*/
        if (IsControl((HWND)pWin)) {
            ClientToWindow((HWND)pWin->pMainWin, &client_rc.left, &client_rc.top);
            ClientToWindow((HWND)pWin->pMainWin, &client_rc.right, &client_rc.bottom);
        }
        else
            OffsetRect (&client_rc, -pdc_main->DevRC.left, -pdc_main->DevRC.top);
        ExcludeClipRect (real_dc, &client_rc);
    }

    /* exclude tranparent client rect.*/
    if (flags == HT_CLIENT) {
        PCONTROL child;
        RECT rc = pWin->pMainWin->update_rc;
        child = (PCONTROL)pWin->hFirstChild;
        while (child) {
            //if ((child->dwStyle & WS_VISIBLE) && (child->dwExStyle & WS_EX_TRANSPARENT)
            if ((child->dwStyle & WS_VISIBLE)
                    && DoesIntersect ((const RECT*)&child->left, &rc)) {
                RECT rcTmp;
                gui_WndClientRect ((HWND)child, &rcTmp);
                ScreenToClient ((HWND)pWin->pMainWin, 
                        &rcTmp.left, &rcTmp.top);
                ScreenToClient ((HWND)pWin->pMainWin, 
                        &rcTmp.right, &rcTmp.bottom);
                ExcludeClipRect (real_dc, &rcTmp);
            }
            child = child->next;
        }
    }

    if (pWin->pMainWin->update_secdc == ON_UPDSECDC_DEFAULT)
        BitBlt (secondary_dc, rc->left, rc->top, 
                RECTWP(rc), RECTHP(rc), 
                real_dc, clip_rc.left, clip_rc.top, 0);
    else if (pWin->pMainWin->update_secdc != ON_UPDSECDC_DONOTHING) {
        if (flags == HT_CLIENT){
            RECT main_update_rc;
            SetRect(&main_update_rc, clip_rc.left, clip_rc.top, 
                    clip_rc.right, clip_rc.bottom);
            ClientToWindow((HWND)pWin->pMainWin, 
                    &main_update_rc.left, &main_update_rc.top);
            ClientToWindow((HWND)pWin->pMainWin, 
                    &main_update_rc.right, &main_update_rc.bottom);
            pWin->pMainWin->update_secdc ((HWND)pWin, 
                    secondary_dc, real_dc, rc, &clip_rc, &main_update_rc);
        }
        else {
            pWin->pMainWin->update_secdc ((HWND)pWin, 
                    secondary_dc, real_dc, rc, &clip_rc, &clip_rc);
        }
    }
}

static void draw_secondary_nc_area (PMAINWIN pWin, 
        const WINDOW_ELEMENT_RENDERER* rdr, 
        HDC hdc, DWORD flags)
{
    RECT rc;
    HDC real_dc = 0;

    do {
#if 0
        /* houhh 20081012, the WS_EX_CTRLASMAINWIN style window have itself's pdc->pGCRInfo
         * information, so the real_dc's global clip info should use itselft's. */
        if (pWin->dwExStyle & WS_EX_CTRLASMAINWIN)
            real_dc = GetDC ((HWND)pWin);
        else
            real_dc = GetDC ((HWND)pWin->pMainWin);
#else
        real_dc = GetDC ((HWND)pWin->pMainWin);
#endif

        if (rdr->calc_we_area ((HWND)pWin, flags, &rc))
            break;

        if (!IsRectEmpty(&rc)){
            update_secondary_dc (pWin, hdc, real_dc, &rc, flags);
        }
    } while (FALSE);

    if (real_dc)
        ReleaseDC (real_dc);
}

static void ShrinkScrollbarClipRect(HWND hWnd, HDC hdc, const WINDOWINFO *_info, int downCode)
{
    static HWND hWnd_old;
    static LFSCROLLBARINFO vscroll_old;
    static LFSCROLLBARINFO hscroll_old;
    WINDOWINFO *info = (WINDOWINFO *)_info;
    RECT rc_bound, rc_new, rc_old;
    LFSCROLLBARINFO *p_scrollbar_info_old, *p_scrollbar_info_new;
    LFSCROLLBARINFO scrollbar_info_bak;
    int which;

    if (hWnd != hWnd_old) {
        hWnd_old = hWnd;
        memset(&vscroll_old, 0, sizeof(vscroll_old));
        memset(&hscroll_old, 0, sizeof(hscroll_old));
        return;
    }

    if (downCode == HT_VSCROLL) {
        p_scrollbar_info_old = &vscroll_old;
        p_scrollbar_info_new = &info->vscroll;
        which = HT_SB_VTHUMB;
    }else if (downCode == HT_HSCROLL) {
        p_scrollbar_info_old = &hscroll_old;
        p_scrollbar_info_new = &info->hscroll;
        which = HT_SB_HTHUMB;
    }else{
        return;
    }

    if (1
            && p_scrollbar_info_old->minPos == p_scrollbar_info_new->minPos
            && p_scrollbar_info_old->maxPos == p_scrollbar_info_new->maxPos
            && p_scrollbar_info_old->curPos != p_scrollbar_info_new->curPos /* Yes, != */
            && p_scrollbar_info_old->pageStep == p_scrollbar_info_new->pageStep
            && p_scrollbar_info_old->arrowLen == p_scrollbar_info_new->arrowLen
            /* barStart */
            && p_scrollbar_info_old->barLen == p_scrollbar_info_new->barLen
            && p_scrollbar_info_old->status == p_scrollbar_info_new->status) {
        info->we_rdr->calc_we_area(hWnd, which, &rc_new);

        memcpy(&scrollbar_info_bak, p_scrollbar_info_new, sizeof(scrollbar_info_bak));
        memcpy(p_scrollbar_info_new, p_scrollbar_info_old, sizeof(scrollbar_info_bak));
        info->we_rdr->calc_we_area(hWnd, which, &rc_old);
        memcpy(p_scrollbar_info_new, &scrollbar_info_bak, sizeof(scrollbar_info_bak));

        GetBoundRect(&rc_bound, &rc_new, &rc_old);

        /* [humingming.2010.9.7] for problem fashion render draw scrollbar's thumb framework bound.
         * why add 1 pixel? don't answer me, try this code with MiniGUI:
         *
         * SetRect (&rc_bound, 10, 10, 10+50, 10+50);
         * SetPenColor (hdc, PIXEL_blue);
         * SetBrushColor (hdc, PIXEL_red);
         *
         * MoveTo (hdc, rc_bound.left, rc_bound.top);
         * LineTo (hdc, rc_bound.right, rc_bound.top);
         * LineTo (hdc, rc_bound.right, rc_bound.bottom); 
         * LineTo (hdc, rc_bound.left, rc_bound.bottom); 
         * LineTo (hdc, rc_bound.left, rc_bound.top);
         *
         * ClipRectIntersect(hdc, &rc_bound);
         * FillBox (hdc, 0, 0, 500, 400);
         *
         * you check bound of the box's right and bottom carefully, and you will konw it. -_-||
         */
        rc_bound.right += 1;
        rc_bound.bottom += 1;

#if 0 /* to show the clip rect */
        SetBrushColor(hdc, RGB2Pixel(hdc, 0xff, 0, 0));
        FillBox(hdc, rc_bound.left, rc_bound.top, RECTW(rc_bound), RECTH(rc_bound));
        usleep(100);
#endif
        ClipRectIntersect(hdc, &rc_bound);
    }

    memcpy(p_scrollbar_info_old, p_scrollbar_info_new, sizeof(scrollbar_info_bak));
}

#define wndDrawNCButton(pWin, downCode, status) wndDrawNCButtonEx(pWin, 0, downCode, status)

static BOOL wndDrawNCButtonEx (PMAINWIN pWin, HDC hdc, int downCode, int status)
{
    BOOL fGetDC = FALSE;
    const WINDOWINFO  *info = GetWindowInfo ((HWND)pWin);
    static int _status =
        SBS_PRESSED_LTUP | SBS_PRESSED_BTDN | SBS_PRESSED_THUMB |
        SBS_HILITE_LTUP | SBS_HILITE_BTDN | SBS_HILITE_THUMB;

	if(hdc == 0)
	{
		hdc = get_valid_dc (pWin, FALSE);
		fGetDC = TRUE;
	}

    switch (downCode) {
        case HT_ICON:
        case HT_MAXBUTTON:
        case HT_MINBUTTON:
        case HT_CLOSEBUTTON:
            {
                BOOL is_active = TRUE;
                if (pWin->WinType == TYPE_MAINWIN)
                    is_active = !(pWin->dwStyle & WS_DISABLED) &&
                        (GetActiveWindow() == (HWND)pWin);
                else 
                    is_active = !(pWin->dwStyle & WS_DISABLED) &&
                        (((PCONTROL)pWin)->pParent->active == (PCONTROL)pWin);

                if (is_active) 
                    info->we_rdr->draw_caption_button ((HWND)pWin, 
                            hdc, downCode, LFRDR_BTN_STATUS_NORMAL|status);
                else
                    info->we_rdr->draw_caption_button ((HWND)pWin, 
                            hdc, downCode, LFRDR_BTN_STATUS_INACTIVE|status);

                if (pWin->pMainWin->secondaryDC) {
                    draw_secondary_nc_area (pWin, info->we_rdr, hdc, downCode);
                }
                release_valid_dc (pWin, hdc);
                return TRUE;
            }

        case HT_SB_UPARROW:
            {
                if (pWin->vscroll.curPos == pWin->vscroll.minPos)
                {
                    pWin->vscroll.status |= SBS_DISABLED_LTUP; 
                }
                else
                { 
                    pWin->vscroll.status &= ~SBS_DISABLED_LTUP; 
                }

                if (status == LFRDR_BTN_STATUS_PRESSED)
                    pWin->vscroll.status |= SBS_PRESSED_LTUP;
                else if (status == LFRDR_BTN_STATUS_HILITE)
                    pWin->vscroll.status |= SBS_HILITE_LTUP;

                if (pWin->vscroll.curPos + pWin->vscroll.pageStep - 1 
                        >= pWin->vscroll.maxPos)
                    pWin->vscroll.status |= SBS_DISABLED_BTDN; 
                else
                    pWin->vscroll.status &= ~SBS_DISABLED_BTDN; 

                break;
            }

        case HT_SB_DOWNARROW:
            {
                if (pWin->vscroll.curPos + pWin->vscroll.pageStep - 1 
                        >= pWin->vscroll.maxPos)
                {
                    pWin->vscroll.status |= SBS_DISABLED_BTDN; 
                }
                else
                {
                    pWin->vscroll.status &= ~SBS_DISABLED_BTDN; 
                }

                if (status == LFRDR_BTN_STATUS_PRESSED)
                    pWin->vscroll.status |= SBS_PRESSED_BTDN;
                else if (status == LFRDR_BTN_STATUS_HILITE)
                    pWin->vscroll.status |= SBS_HILITE_BTDN;

                if (pWin->vscroll.curPos == pWin->vscroll.minPos)
                    pWin->vscroll.status |= SBS_DISABLED_LTUP; 
                else
                    pWin->vscroll.status &= ~SBS_DISABLED_LTUP; 

                break;
            }

        case HT_SB_LEFTARROW:
            {
                if (pWin->hscroll.curPos == pWin->hscroll.minPos)
                {
                    pWin->hscroll.status |= SBS_DISABLED_LTUP; 
                }
                else
                {
                    pWin->hscroll.status &= ~SBS_DISABLED_LTUP; 
                }

                if (status == LFRDR_BTN_STATUS_PRESSED)
                    pWin->hscroll.status |= SBS_PRESSED_LTUP;
                else if (status == LFRDR_BTN_STATUS_HILITE)
                    pWin->hscroll.status |= SBS_HILITE_LTUP;

                if (pWin->hscroll.curPos + pWin->hscroll.pageStep - 1 
                        >= pWin->hscroll.maxPos)
                    pWin->hscroll.status |= SBS_DISABLED_BTDN; 
                else
                    pWin->hscroll.status &= ~SBS_DISABLED_BTDN; 

                break;
            }

        case HT_SB_RIGHTARROW:
            {
                if (pWin->hscroll.curPos + pWin->hscroll.pageStep - 1 
                        >= pWin->hscroll.maxPos)
                {
                    pWin->hscroll.status |= SBS_DISABLED_BTDN; 
                }
                else
                {
                    pWin->hscroll.status &= ~SBS_DISABLED_BTDN; 
                }

                if (status == LFRDR_BTN_STATUS_PRESSED)
                    pWin->hscroll.status |= SBS_PRESSED_BTDN;
                else if (status == LFRDR_BTN_STATUS_HILITE)
                    pWin->hscroll.status |= SBS_HILITE_BTDN;

                if (pWin->hscroll.curPos == pWin->hscroll.minPos)
                    pWin->hscroll.status |= SBS_DISABLED_LTUP; 
                else
                    pWin->hscroll.status &= ~SBS_DISABLED_LTUP; 

                break;
            }

        case HT_SB_HTHUMB:
            {
                if (status == LFRDR_BTN_STATUS_HILITE)
                    pWin->hscroll.status |= SBS_HILITE_THUMB;

                if (pWin->hscroll.curPos == pWin->hscroll.minPos)
                    pWin->hscroll.status |= SBS_DISABLED_LTUP; 
                else
                    pWin->hscroll.status &= ~SBS_DISABLED_LTUP;

                if (pWin->hscroll.curPos + pWin->hscroll.pageStep - 1 
                        >= pWin->hscroll.maxPos)
                    pWin->hscroll.status |= SBS_DISABLED_BTDN; 
                else
                    pWin->hscroll.status &= ~SBS_DISABLED_BTDN; 

                if (pWin->hscroll.pageStep < pWin->hscroll.maxPos 
                        - pWin->hscroll.minPos + 1)
                {
                    if (status == LFRDR_BTN_STATUS_PRESSED)
                        pWin->hscroll.status |= SBS_PRESSED_THUMB;
                }

                break;
            }

        case HT_SB_VTHUMB:
            {
                if (status == LFRDR_BTN_STATUS_HILITE)
                    pWin->vscroll.status |= SBS_HILITE_THUMB;

                if (pWin->vscroll.curPos == pWin->vscroll.minPos)
                    pWin->vscroll.status |= SBS_DISABLED_LTUP; 
                else
                    pWin->vscroll.status &= ~SBS_DISABLED_LTUP;

                if (pWin->vscroll.curPos + pWin->vscroll.pageStep - 1 
                        >= pWin->vscroll.maxPos)
                    pWin->vscroll.status |= SBS_DISABLED_BTDN; 
                else
                    pWin->vscroll.status &= ~SBS_DISABLED_BTDN; 

                if (pWin->vscroll.pageStep < pWin->vscroll.maxPos 
                        - pWin->vscroll.minPos + 1)
                {
                    if (status == LFRDR_BTN_STATUS_PRESSED)
                        pWin->vscroll.status |= SBS_PRESSED_THUMB;
                }

                break;
            }

        case HT_SB_LEFTSPACE:
        case HT_SB_RIGHTSPACE:
        case HT_HSCROLL:
            {
                if (pWin->hscroll.curPos == pWin->hscroll.minPos)
                    pWin->hscroll.status |= SBS_DISABLED_LTUP; 
                else
                    pWin->hscroll.status &= ~SBS_DISABLED_LTUP;

                if (pWin->hscroll.curPos + pWin->hscroll.pageStep - 1 
                        >= pWin->hscroll.maxPos)
                    pWin->hscroll.status |= SBS_DISABLED_BTDN; 
                else
                    pWin->hscroll.status &= ~SBS_DISABLED_BTDN; 
                break;
            }

        case HT_SB_UPSPACE:
        case HT_SB_DOWNSPACE:
        case HT_VSCROLL:
            {
                if (pWin->vscroll.curPos == pWin->vscroll.minPos)
                    pWin->vscroll.status |= SBS_DISABLED_LTUP; 
                else
                    pWin->vscroll.status &= ~SBS_DISABLED_LTUP;

                if (pWin->vscroll.curPos + pWin->vscroll.pageStep - 1 
                        >= pWin->vscroll.maxPos)
                    pWin->vscroll.status |= SBS_DISABLED_BTDN; 
                else
                    pWin->vscroll.status &= ~SBS_DISABLED_BTDN; 

                break;
            }
        default:
            release_valid_dc (pWin, hdc);
            return FALSE;
    }

    if (! pWin->pMainWin->secondaryDC) {
        ShrinkScrollbarClipRect((HWND)pWin, hdc, info, downCode);
    }

    info->we_rdr->draw_scrollbar ((HWND)pWin, hdc, downCode);

    if (pWin->pMainWin->secondaryDC) {
        draw_secondary_nc_area (pWin, info->we_rdr, hdc, downCode);
    }

	if(fGetDC)
	    release_valid_dc (pWin, hdc);

    pWin->hscroll.status &= ~_status;
    pWin->vscroll.status &= ~_status;
 
    return TRUE;
}

#ifdef _MGHAVE_MENU
int MenuBarHitTest (HWND hwnd, int x, int y);

static void wndTrackMenuBarOnMouseMove(PMAINWIN pWin, int message, 
        int location, int x, int y)
{
    /** do nothing in LFRDR */
    return;
}
#endif

static int set_hilite_sbpos (int location)
{
    switch (location) 
    {
        case HT_SB_UPARROW:
        case HT_SB_DOWNARROW:
        case HT_SB_LEFTARROW:
        case HT_SB_RIGHTARROW:
        case HT_SB_HTHUMB:
        case HT_SB_VTHUMB:
            return location;
        default:
            return HT_SB_UNKNOWN;
    }
}

#define CALC_THUMB_HPOS(newBarStart, rcBar, scroll) \
    ((rcBar.right - rcBar.left - scroll.barLen) > 0 ? ((newBarStart) * (scroll.maxPos - scroll.minPos - scroll.pageStep + 1) \
        / (rcBar.right - rcBar.left - scroll.barLen) + scroll.minPos) : 0)

static int wndHandleHScrollBar (PMAINWIN pWin, 
        int message, int location, int x, int y)
{
    static int downPos = HT_SB_UNKNOWN;
    static int movePos = HT_SB_UNKNOWN;
    static int hilitePos = HT_SB_UNKNOWN;
    static int sbCode = 0;
    static int oldBarStart;
    static int oldThumbPos;
    static int oldx;
    int newBarStart;
    int newThumbPos = 0;
    RECT rcBar;

    if (!((location & HT_SB_MASK) == HT_SB_MASK && location < HT_SB_VMASK)
            && downPos == HT_SB_UNKNOWN && hilitePos == HT_SB_UNKNOWN)
        return 0;

    if (pWin->hscroll.status & SBS_HIDE || 
            pWin->hscroll.status & SBS_DISABLED)
        return -1;

    wndGetHScrollBarRect (pWin, &rcBar);
    rcBar.left += GetWindowElementAttr ((HWND)pWin, WE_METRICS_SCROLLBAR);
    rcBar.right -= GetWindowElementAttr ((HWND)pWin, WE_METRICS_SCROLLBAR);

    switch( message )
    {
        case MSG_NCLBUTTONDOWN:
            oldBarStart = pWin->hscroll.barStart;
            oldThumbPos = pWin->hscroll.curPos;
            oldx = x;

            downPos = location;
            movePos = location;
            hilitePos = HT_SB_UNKNOWN;

            wndDrawNCButton (pWin, location, LFRDR_BTN_STATUS_PRESSED);

            if (location == HT_SB_LEFTARROW) {
                if (pWin->hscroll.curPos == pWin->hscroll.minPos)
                    break;

                sbCode = SB_LINELEFT;
            }
            else if (location == HT_SB_RIGHTARROW) {
                if (pWin->hscroll.curPos + pWin->hscroll.pageStep
                        >= pWin->hscroll.maxPos)
                    break;

                sbCode = SB_LINERIGHT;
            }
            else if (location == HT_SB_LEFTSPACE) {
                if (pWin->hscroll.curPos == pWin->hscroll.minPos)
                    break;

                sbCode = SB_PAGELEFT;
            }
            else if (location == HT_SB_RIGHTSPACE) {
                if (pWin->hscroll.curPos == pWin->hscroll.maxPos)
                    break;

                sbCode = SB_PAGERIGHT;
            }
            else if (location == HT_SB_HTHUMB) {
                sbCode = SB_THUMBTRACK;
                break;
            }

            if (location != HT_SB_UNKNOWN) {
                SendNotifyMessage ((HWND)pWin, MSG_HSCROLL, sbCode, x);
                SetAutoRepeatMessage ((HWND)pWin, MSG_HSCROLL, sbCode, x);
            }
            break;

        case MSG_NCLBUTTONUP:
            if (sbCode == SB_THUMBTRACK 
                    && downPos == HT_SB_HTHUMB) {
                newBarStart = oldBarStart + x - oldx;
                if (RECTW(rcBar) != 0)
                    newThumbPos = CALC_THUMB_HPOS(newBarStart, rcBar, pWin->hscroll);

                pWin->hscroll.status &= ~SBS_PRESSED_LTUP;
                pWin->hscroll.status &= ~SBS_PRESSED_BTDN;

                if (newThumbPos < pWin->hscroll.minPos)
                    newThumbPos = pWin->hscroll.minPos;
                if (newThumbPos > pWin->hscroll.maxPos)
                    newThumbPos = pWin->hscroll.maxPos;
                if (newBarStart != oldBarStart)
                    SendNotifyMessage ((HWND)pWin,
                            MSG_HSCROLL, SB_THUMBPOSITION, newThumbPos);

                //wndDrawNCButton (pWin, downPos, LFRDR_BTN_STATUS_NORMAL);
                wndDrawNCButton (pWin, HT_HSCROLL, 0);

                downPos = HT_SB_UNKNOWN;
                movePos = HT_SB_UNKNOWN;
                hilitePos = HT_SB_UNKNOWN;
                return -1;
            }

            if (downPos != HT_SB_UNKNOWN) {
                //wndDrawNCButton (pWin, downPos, LFRDR_BTN_STATUS_NORMAL);
                wndDrawNCButton (pWin, HT_HSCROLL, 0);
                SendNotifyMessage ((HWND)pWin, MSG_HSCROLL, SB_ENDSCROLL, 0);
                /* cancel repeat message */
                SetAutoRepeatMessage (0, 0, 0, 0);
            }

            downPos = HT_SB_UNKNOWN;
            movePos = HT_SB_UNKNOWN;
            hilitePos = HT_SB_UNKNOWN;
            return 0;

        case MSG_NCMOUSEMOVE:
            if (sbCode == SB_THUMBTRACK
                    && downPos == HT_SB_HTHUMB) {
                newBarStart = oldBarStart + x - oldx;
                if (RECTW(rcBar) != 0)
                    newThumbPos = CALC_THUMB_HPOS(newBarStart, rcBar, pWin->hscroll);

                if (newThumbPos < pWin->hscroll.minPos)
                    newThumbPos = pWin->hscroll.minPos;
                if (newThumbPos > pWin->hscroll.maxPos)
                    newThumbPos = pWin->hscroll.maxPos;
                if (newThumbPos != oldThumbPos) {
                    if (pWin->hscroll.pageStep < pWin->hscroll.maxPos
                            - pWin->hscroll.minPos + 1)
                    {
                        SendNotifyMessage ((HWND)pWin,
                                MSG_HSCROLL, SB_THUMBTRACK, newThumbPos);
                        oldThumbPos = newThumbPos;
                    }
                }
            }

            if (downPos != HT_SB_UNKNOWN) { 
                if (movePos == downPos && location != downPos)
                    wndDrawNCButton (pWin, downPos, LFRDR_BTN_STATUS_NORMAL);
                else if (movePos != downPos && location == downPos)
                    wndDrawNCButton (pWin, downPos, LFRDR_BTN_STATUS_PRESSED);
                movePos = location;
            }
            else {
                PostMessage((HWND)pWin, MSG_NCSETCURSOR, location, MAKELONG (x, y));
                if (hilitePos != location) {
                    if (hilitePos != HT_SB_UNKNOWN)
                        wndDrawNCButton (pWin, hilitePos, LFRDR_BTN_STATUS_NORMAL);

                    if (location != HT_SB_UNKNOWN 
                         && ((location & HT_SB_MASK) == HT_SB_MASK && location < HT_SB_VMASK))
                        wndDrawNCButton (pWin, location, LFRDR_BTN_STATUS_HILITE);
                }

                if ((location & HT_SB_MASK) == HT_SB_MASK && location < HT_SB_VMASK)
                    hilitePos = set_hilite_sbpos (location);
                else 
                    hilitePos = HT_SB_UNKNOWN;
            }

            return 0;
    }

    return 1;
}

#define CALC_THUMB_VPOS(newBarStart, rcBar, scroll) \
    ((rcBar.bottom - rcBar.top - scroll.barLen) > 0 ? ((newBarStart) * (scroll.maxPos - scroll.minPos - scroll.pageStep + 1) \
        / (rcBar.bottom - rcBar.top - scroll.barLen) + scroll.minPos) : 0)

static int wndHandleVScrollBar (PMAINWIN pWin, 
        int message, int location, int x, int y)
{
    static int downPos = HT_SB_UNKNOWN;
    static int movePos = HT_SB_UNKNOWN;
    static int hilitePos = HT_SB_UNKNOWN;
    static int sbCode = 0;
    static int oldBarStart;
    static int oldThumbPos;
    static int oldy;
    int newBarStart;
    int newThumbPos = 0;
    RECT rcBar;

    if ((location & HT_SB_VMASK) != HT_SB_VMASK 
            && downPos == HT_SB_UNKNOWN 
            && hilitePos == HT_SB_UNKNOWN)
        return 0;

    if (pWin->vscroll.status & SBS_HIDE || 
            pWin->vscroll.status & SBS_DISABLED)
        return -1;

    wndGetVScrollBarRect (pWin, &rcBar);
    rcBar.top += GetWindowElementAttr ((HWND)pWin, WE_METRICS_SCROLLBAR);
    rcBar.bottom -= GetWindowElementAttr ((HWND)pWin, WE_METRICS_SCROLLBAR);

    switch (message)
    {
        case MSG_NCLBUTTONDOWN:
            oldBarStart = pWin->vscroll.barStart;
            oldThumbPos = pWin->vscroll.curPos;
            oldy = y;
            downPos = location;
            movePos = location;
            hilitePos = HT_SB_UNKNOWN;

            wndDrawNCButton (pWin, location, LFRDR_BTN_STATUS_PRESSED);

            if (location == HT_SB_UPARROW) {
                if (pWin->vscroll.curPos == pWin->vscroll.minPos)
                    break;
                sbCode = SB_LINEUP;
            }
            else if (location == HT_SB_DOWNARROW) {
                if (pWin->vscroll.curPos + pWin->vscroll.pageStep - 1
                        >= pWin->vscroll.maxPos)
                    break;
                sbCode = SB_LINEDOWN;
            }
            else if (location == HT_SB_UPSPACE) {
                if (pWin->vscroll.curPos == pWin->vscroll.minPos)
                    break;
                sbCode = SB_PAGEUP;
            }
            else if (location == HT_SB_DOWNSPACE) {
                if (pWin->vscroll.curPos >= pWin->vscroll.maxPos)
                    break;
                sbCode = SB_PAGEDOWN;
            }
            else if (location == HT_SB_VTHUMB) {
                sbCode = SB_THUMBTRACK;
                break;
            }

            if (location != HT_SB_UNKNOWN) {
                SendNotifyMessage ((HWND)pWin, MSG_VSCROLL, sbCode, y);
                SetAutoRepeatMessage ((HWND)pWin, MSG_VSCROLL, sbCode, y);
            }
            break;

        case MSG_NCLBUTTONUP:
            if (sbCode == SB_THUMBTRACK 
                    && downPos == HT_SB_VTHUMB) {
                newBarStart = oldBarStart + y - oldy;
                if (RECTH(rcBar) != 0)
                    newThumbPos = CALC_THUMB_VPOS(newBarStart, rcBar, pWin->vscroll);

                pWin->vscroll.status &= ~SBS_PRESSED_LTUP;
                pWin->vscroll.status &= ~SBS_PRESSED_BTDN;

                if (newThumbPos < pWin->vscroll.minPos)
                    newThumbPos = pWin->vscroll.minPos;
                if (newThumbPos > pWin->vscroll.maxPos)
                    newThumbPos = pWin->vscroll.maxPos;
                if (newBarStart != oldBarStart) {
                    SendNotifyMessage ((HWND)pWin,
                            MSG_VSCROLL, SB_THUMBPOSITION, newThumbPos);
                }

                //    wndDrawNCButton (pWin, downPos, LFRDR_BTN_STATUS_NORMAL);
                wndDrawNCButton (pWin, HT_VSCROLL, 0);

                downPos = HT_SB_UNKNOWN;
                movePos = HT_SB_UNKNOWN;
                hilitePos = HT_SB_UNKNOWN;
                return -1;
            }

            if (downPos != HT_SB_UNKNOWN) {
                //wndDrawNCButton (pWin, downPos, LFRDR_BTN_STATUS_NORMAL);
                wndDrawNCButton (pWin, HT_VSCROLL, 0);
                SendNotifyMessage ((HWND)pWin, MSG_VSCROLL, SB_ENDSCROLL, 0);
                /* cancel repeat message */
                SetAutoRepeatMessage (0, 0, 0, 0);
            }

            downPos = HT_SB_UNKNOWN;
            movePos = HT_SB_UNKNOWN;
            hilitePos = HT_SB_UNKNOWN;
            return -1;

        case MSG_NCMOUSEMOVE:
            if (sbCode == SB_THUMBTRACK
                    && downPos == HT_SB_VTHUMB) {
                newBarStart = oldBarStart + y - oldy;
                if (RECTH(rcBar) != 0)
                    newThumbPos = CALC_THUMB_VPOS(newBarStart, rcBar, pWin->vscroll);

                if (newThumbPos < pWin->vscroll.minPos)
                    newThumbPos = pWin->vscroll.minPos;
                if (newThumbPos >  (1 + pWin->vscroll.maxPos - pWin->vscroll.pageStep))
                    newThumbPos = pWin->vscroll.maxPos - pWin->vscroll.pageStep + 1;
                if (newThumbPos != oldThumbPos) {
                    if (pWin->vscroll.pageStep < pWin->vscroll.maxPos
                            - pWin->vscroll.minPos + 1)
                    {
                        SendNotifyMessage ((HWND)pWin,
                                MSG_VSCROLL, SB_THUMBTRACK, newThumbPos);
                        oldThumbPos = newThumbPos;
                    }
                }
            }

            if (downPos != HT_SB_UNKNOWN) {
                if (movePos == downPos && location != downPos)
                    wndDrawNCButton (pWin, downPos, LFRDR_BTN_STATUS_NORMAL);
                else if (movePos != downPos && location == downPos)
                    wndDrawNCButton (pWin, downPos, LFRDR_BTN_STATUS_PRESSED);
                movePos = location;
            }
            else {
                PostMessage((HWND)pWin, MSG_NCSETCURSOR, location, MAKELONG (x, y));

                if (hilitePos != location) {
                    if (hilitePos != HT_SB_UNKNOWN)
                        wndDrawNCButton (pWin, hilitePos, LFRDR_BTN_STATUS_NORMAL);

                    if ((location != HT_SB_UNKNOWN)
                         && ((location & HT_SB_VMASK) == HT_SB_VMASK))
                        wndDrawNCButton (pWin, location, LFRDR_BTN_STATUS_HILITE);
                }
                if ((location & HT_SB_VMASK) == HT_SB_VMASK)
                    hilitePos = set_hilite_sbpos (location);
                else
                    hilitePos = HT_SB_UNKNOWN;
            }

            break;
    }

    return 1;
}

static void wndHandleCustomHotspot (PMAINWIN pWin, 
        int message, int location, int x, int y)
{
    const WINDOW_ELEMENT_RENDERER *rdr;
    static int downPos = HT_UNKNOWN;
    static int movePos = HT_UNKNOWN;
    static int hilitePos = HT_UNKNOWN;
    RECT    rc;
    HDC     hdc;

    rdr = GetWindowInfo ((HWND)pWin)->we_rdr;

    if (rdr->calc_we_area ((HWND)pWin, location, &rc))
        return;

    switch (message)
    {
        case MSG_NCLBUTTONDOWN:
            downPos = location;
            movePos = location;
            hilitePos = HT_UNKNOWN;

            hdc = get_valid_dc (pWin, FALSE);
            if (rdr->draw_custom_hotspot)
                rdr->draw_custom_hotspot ((HWND)pWin, hdc, location, 
                        LFRDR_BTN_STATUS_PRESSED);
            if (pWin->pMainWin->secondaryDC) {
                draw_secondary_nc_area (pWin, rdr, hdc, location);
            }
            release_valid_dc (pWin, hdc);

            if (rdr->on_click_hotspot)
                rdr->on_click_hotspot ((HWND)pWin, location);
            break;

        case MSG_NCLBUTTONUP:
            hdc = get_valid_dc (pWin, FALSE);
            if (rdr->draw_custom_hotspot && (downPos != HT_UNKNOWN)) {
                rdr->draw_custom_hotspot ((HWND)pWin, hdc, location, 
                        LFRDR_BTN_STATUS_NORMAL);
            }
            if (pWin->pMainWin->secondaryDC) {
                draw_secondary_nc_area (pWin, rdr, hdc, location);
            }
            release_valid_dc (pWin, hdc);

            downPos = HT_UNKNOWN;
            movePos = HT_UNKNOWN;
            hilitePos = HT_UNKNOWN;
            break;

        case MSG_NCMOUSEMOVE:
            hdc = get_valid_dc (pWin, FALSE);
            if (downPos != HT_UNKNOWN) {
                if (rdr->draw_custom_hotspot) {
                    if (movePos == downPos && location != downPos)
                        rdr->draw_custom_hotspot ((HWND)pWin, hdc, location, 
                                LFRDR_BTN_STATUS_NORMAL);
                    else if (movePos != downPos && location == downPos)
                        rdr->draw_custom_hotspot ((HWND)pWin, hdc, location, 
                                LFRDR_BTN_STATUS_PRESSED);
                }
                movePos = location;
            }
            else {
                PostMessage((HWND)pWin, 
                        MSG_NCSETCURSOR, location, MAKELONG (x, y));

                if (rdr->draw_custom_hotspot && (hilitePos != location)) {
                    if (hilitePos != HT_UNKNOWN)
                        rdr->draw_custom_hotspot ((HWND)pWin, hdc, location, 
                                LFRDR_BTN_STATUS_NORMAL);

                    if (location != HT_UNKNOWN)
                        rdr->draw_custom_hotspot ((HWND)pWin, hdc, location, 
                                LFRDR_BTN_STATUS_HILITE);
                }
                hilitePos = location;
            }
            if (pWin->pMainWin->secondaryDC) {
                draw_secondary_nc_area (pWin, rdr, hdc, location);
            }
            release_valid_dc (pWin, hdc);

            break;
    }
}

/* this function is CONTROL safe. */
static LRESULT DefaultNCMouseMsgHandler(PMAINWIN pWin, UINT message, 
        int location, int x, int y)
{
    static int downCode = HT_UNKNOWN;
    static int moveCode = HT_UNKNOWN;
    static int hiliteCode = HT_UNKNOWN;

#ifdef _MGHAVE_MENU
    int barItem;
#endif

    /*for user hittest process*/
    if (location & HT_USER_MASK) {
        wndHandleCustomHotspot (pWin, message, location, x, y);
        return 0;
    }

#ifdef _MGHAVE_MENU
    if (pWin->WinType == TYPE_MAINWIN && message == MSG_NCMOUSEMOVE)
        wndTrackMenuBarOnMouseMove (pWin, message, location, x, y);
#endif

    /*process scrollbar*/
    if (pWin->dwStyle & WS_HSCROLL) {
        switch (wndHandleHScrollBar (pWin, message, location, x, y)) {
            case 1:
                return 0;
            case -1:
                downCode = HT_UNKNOWN;
                moveCode = HT_UNKNOWN;
                return 0;
            /* hscroll do not response this message.*/
            case 0:
                break;
        }
    }

    if (pWin->dwStyle & WS_VSCROLL) {
        switch (wndHandleVScrollBar (pWin, message, location, x, y)) {
            case 1:
                return 0;
            case -1:
                downCode = HT_UNKNOWN;
                moveCode = HT_UNKNOWN;
                return 0;
        }
    }

    switch (message)
    {
        case MSG_NCLBUTTONDOWN:

#ifdef _MGHAVE_MENU
            if (location == HT_MENUBAR) {
                barItem = MenuBarHitTest ((HWND)pWin, x, y);
                if (barItem >= 0)
                    TrackMenuBar ((HWND)pWin, barItem);

                return 0;
            }
            else 
#endif
                if (location & HT_DRAGGABLE && !(pWin->dwExStyle & WS_EX_NOTDRAGGABLE)) {
                    DRAGINFO drag_info;

                    drag_info.location = location;
                    drag_info.init_x = x;
                    drag_info.init_y = y;
                    SendMessage (HWND_DESKTOP, MSG_STARTDRAGWIN, 
                            (WPARAM)pWin, (LPARAM)&drag_info);
                }
                else {
                    downCode = location;
                    moveCode = location;
                    hiliteCode = HT_UNKNOWN;

                    wndDrawNCButton (pWin, downCode, LFRDR_BTN_STATUS_PRESSED);
                }
            break;

        case MSG_NCMOUSEMOVE:
            if (pWin->hOldUnderPointer && location == HT_OUT) {
                PostMessage (pWin->hOldUnderPointer,
                        MSG_MOUSEMOVEIN, FALSE, 0);
                PostMessage (pWin->hOldUnderPointer,
                        MSG_NCMOUSEMOVE, HT_OUT, MAKELONG (x, y));
                pWin->hOldUnderPointer = 0;
            }

            if (downCode != HT_UNKNOWN) {
                if (moveCode == downCode && location != downCode) {
                    wndDrawNCButton (pWin, downCode, LFRDR_BTN_STATUS_NORMAL);
                    moveCode = location;
                }
                else if (moveCode != downCode && location == downCode) {
                    wndDrawNCButton (pWin, downCode, LFRDR_BTN_STATUS_PRESSED);
                    moveCode = location;
                }
            }
            else {
                /*only move process*/
                if (hiliteCode != location) {
                    if (hiliteCode != HT_SB_UNKNOWN)
                        wndDrawNCButton (pWin, hiliteCode, LFRDR_BTN_STATUS_NORMAL);

                    wndDrawNCButton (pWin, location, LFRDR_BTN_STATUS_HILITE);
                }
                hiliteCode = location;
            }

            if (location != HT_CLIENT && downCode == HT_UNKNOWN) {
                PostMessage((HWND)pWin, MSG_NCSETCURSOR, 
                        location, MAKELONG (x, y));
            }
            break;

        case MSG_NCLBUTTONUP:
            if (downCode & HT_DRAGGABLE && !(pWin->dwExStyle & WS_EX_NOTDRAGGABLE)) {
                SendMessage (HWND_DESKTOP, MSG_CANCELDRAGWIN, (WPARAM)pWin, 0L);
            }
            else if (location == downCode) {
                wndDrawNCButton (pWin, downCode, LFRDR_BTN_STATUS_HILITE);
                switch (location) {
                    case HT_CLOSEBUTTON:
                        SendNotifyMessage ((HWND)pWin, MSG_CLOSE, 0, 0);
                        break;

                    case HT_MAXBUTTON:
                        SendNotifyMessage ((HWND)pWin, MSG_MAXIMIZE, 0, 0);
                        break;

                    case HT_MINBUTTON:
                        SendNotifyMessage ((HWND)pWin, MSG_MINIMIZE, 0, 0);
                        break;

#ifdef _MGHAVE_MENU
                    case HT_ICON:
                        if (pWin->hSysMenu)
                            TrackPopupMenu (pWin->hSysMenu, 
                                    TPM_SYSCMD, x, y, (HWND)pWin);
                        break;
#endif

                    case HT_CAPTION:
                        break;

                }
            }
            downCode = HT_UNKNOWN;
            moveCode = HT_UNKNOWN;
            hiliteCode = HT_UNKNOWN;
            break;

#ifdef _MGHAVE_MENU
        case MSG_NCRBUTTONUP:
            if (location == HT_CAPTION && pWin->hSysMenu)
                TrackPopupMenu (pWin->hSysMenu, TPM_SYSCMD, x, y, (HWND)pWin);
            break;
#endif

        case MSG_NCLBUTTONDBLCLK:
            if (location == HT_ICON)
                SendNotifyMessage ((HWND)pWin, MSG_CLOSE, 0, 0);
            /*            else if (location == HT_CAPTION) */
            /*                SendNotifyMessage ((HWND)pWin, MSG_MAXIMIZE, 0, 0); */
            break;

            /*        case MSG_NCRBUTTONDOWN: */
            /*        case MSG_NCRBUTTONDBLCLK: */
            /*            break; */

    }

    return 0;
}

static LRESULT DefaultKeyMsgHandler (PMAINWIN pWin, UINT message, 
        WPARAM wParam, LPARAM lParam)
{
    /*
     ** NOTE:
     ** Currently only handle one message.
     ** Should handle fowllowing messages:
     **
     ** MSG_KEYDOWN,
     ** MSG_KEYUP,
     ** MSG_CHAR,
     ** MSG_SYSKEYDOWN,
     ** MSG_SYSCHAR.
     */
    if (message == MSG_KEYDOWN 
            || message == MSG_KEYUP 
            || message == MSG_KEYLONGPRESS 
            || message == MSG_KEYALWAYSPRESS 
            || message == MSG_CHAR) {
        if (pWin->hActiveChild
                && !(pWin->dwStyle & WS_DISABLED)) {
            /* DK[07/05/10]Fix Bug4798, Check the control if has WS_DISABLED property. */
            PCONTROL pChild = (PCONTROL) pWin->hActiveChild;
            if (!(pChild->dwStyle & WS_DISABLED))
                SendMessage (pWin->hActiveChild, message, wParam, lParam);
        }
    }
#ifdef _MGHAVE_MENU
    else if (message == MSG_SYSKEYUP) {
        if (pWin->WinType == TYPE_MAINWIN
                && !(pWin->dwStyle & WS_DISABLED))
            TrackMenuBar ((HWND)pWin, 0);
    }
#endif

    return 0;
}

static LRESULT DefaultCreateMsgHandler(PMAINWIN pWin, UINT message, 
        WPARAM wParam, LPARAM lParam)
{

    /*
     ** NOTE:
     ** Currently does nothing.
     ** Should handle fowllowing messages:
     **
     ** MSG_NCCREATE,
     ** MSG_CREATE,
     ** MSG_INITPANES,
     ** MSG_DESTROYPANES,
     ** MSG_DESTROY,
     ** MSG_NCDESTROY.
     */
    return 0;
}

static void RecalcScrollInfo (PMAINWIN pWin, BOOL bIsHBar)
{
    PLFSCROLLBARINFO pSBar;
    const WINDOWINFO  *info = GetWindowInfo ((HWND)pWin);

    if (bIsHBar)
        pSBar = &pWin->hscroll;
    else
        pSBar = &pWin->vscroll;

    if (pSBar->minPos == pSBar->maxPos) {
        ShowScrollBar((HWND)pWin, bIsHBar, FALSE);
        return;
    }

    info->we_rdr->calc_thumb_area ((HWND)pWin, !bIsHBar, pSBar);
}

/* This function is CONTROL safe. */
static void 
OnChangeSize (PMAINWIN pWin, PRECT pDestRect, PRECT pResultRect)
{
    RECT cli_rc;
    const WINDOWINFO *info = GetWindowInfo ((HWND)pWin);
    int w, h;

    if (pDestRect) {
        int minWidth = 0, minHeight = 0;

        memcpy(&pWin->left, pDestRect, sizeof(RECT));

        minWidth = 
            info->we_rdr->calc_we_metrics ((HWND)pWin, NULL, 
                    LFRDR_METRICS_MINWIN_WIDTH);

        minHeight = 
            info->we_rdr->calc_we_metrics ((HWND)pWin, NULL, 
                    LFRDR_METRICS_MINWIN_HEIGHT);

        if ((minHeight > (pWin->bottom - pWin->top)) 
                && (pWin->bottom != pWin->top))
            pWin->bottom = pWin->top + minHeight;

        if ((pWin->right < (pWin->left + minWidth)) 
                && (pWin->right != pWin->left))
            pWin->right = pWin->left + minWidth;

        if (pResultRect)
            memcpy (pResultRect, &pWin->left, sizeof(RECT));
    }

    memcpy (&pWin->cl, &pWin->left, sizeof(RECT));

    info->we_rdr->calc_we_area ((HWND)pWin, HT_CLIENT, &cli_rc);
    pWin->cl += cli_rc.left;
    pWin->ct += cli_rc.top;

    if ((w = RECTW(cli_rc)) > 0)
        pWin->cr = pWin->cl + w;
    else
        pWin->cr = pWin->cl;

    if ((h = RECTH(cli_rc)) > 0)
        pWin->cb = pWin->ct + h;
    else
        pWin->cb = pWin->ct;

    if (pWin->dwStyle & WS_HSCROLL && !(pWin->hscroll.status & SBS_HIDE)) {
        RecalcScrollInfo (pWin, TRUE);
    }

    if (pWin->dwStyle & WS_VSCROLL && !(pWin->vscroll.status & SBS_HIDE)) {
        RecalcScrollInfo (pWin, FALSE);
    }
}

static void OnQueryNCRect(PMAINWIN pWin, PRECT pRC)
{
    memcpy(pRC, &pWin->left, sizeof(RECT));
}

static void OnQueryClientArea(PMAINWIN pWin, PRECT pRC)
{
    memcpy(pRC, &pWin->cl, sizeof(RECT));
}

    static void 
calc_metrics_cli2win (const WINDOW_ELEMENT_RENDERER *rdr, 
        int dwStyle, int win_type, int* width, int* height, 
        BOOL hasMenu)
{
    int iBorder = 0;
    LFRDR_WINSTYLEINFO info;

    if (!width && !height)
        return;

    info.dwStyle = dwStyle;
    info.winType = win_type;

    if (dwStyle & WS_BORDER || dwStyle & WS_THINFRAME
            || dwStyle & WS_THICKFRAME) {
        iBorder = 
            rdr->calc_we_metrics (HWND_NULL, &info, LFRDR_METRICS_BORDER);
    }

    if (width)
    {
        *width += iBorder <<1;

        if (dwStyle & WS_VSCROLL) {
            *width += 
                rdr->calc_we_metrics (HWND_NULL, &info, LFRDR_METRICS_VSCROLL_W);
        }
    }

    if (height)
    {
        *height += iBorder <<1;

        if (dwStyle & WS_CAPTION) {
            *height += 
                rdr->calc_we_metrics (HWND_NULL, &info, LFRDR_METRICS_CAPTION_H);
        }

        if (dwStyle & WS_HSCROLL) {
            *height += 
                rdr->calc_we_metrics (HWND_NULL, &info, LFRDR_METRICS_VSCROLL_W);
        }

        if (hasMenu)
            *height += 
                rdr->calc_we_metrics (HWND_NULL, &info, LFRDR_METRICS_MENU_H);
    }
}

int GUIAPI ClientWidthToWindowWidthEx (DWORD dwStyle, int win_type, int cw)
{
    int width = cw;
    const WINDOW_ELEMENT_RENDERER* rdr = 
        GetDefaultWindowElementRenderer ();

    calc_metrics_cli2win (rdr, dwStyle, win_type, &width, 
            NULL, FALSE);

    return width;
}

int GUIAPI ClientHeightToWindowHeightEx (DWORD dwStyle, int win_type, 
        int ch, BOOL hasMenu)
{
    int height = ch;
    const WINDOW_ELEMENT_RENDERER* rdr = 
        GetDefaultWindowElementRenderer ();

    calc_metrics_cli2win (rdr, dwStyle, win_type, NULL, 
            &height, hasMenu);

    return height;
}


BOOL GUIAPI AdjustWindowRectEx (RECT* pRect, DWORD dwStyle, 
                BOOL bMenu, DWORD dwExStyle)
{
    int cw, ch;
    int ww, wh; 
    int off_x, off_y;
    int minw, minh;
    //for minimum window 
    LFRDR_WINSTYLEINFO style_info;
    WINDOW_ELEMENT_RENDERER* rdr;

    if (pRect == NULL)
        return FALSE;

    cw = RECTWP (pRect);
    ch = RECTHP (pRect);

    ww = ClientWidthToWindowWidthEx (dwStyle, LFRDR_WINTYPE_MAINWIN, cw);
    wh = ClientHeightToWindowHeightEx (dwStyle, LFRDR_WINTYPE_MAINWIN, ch, bMenu);

    off_x = (ww - cw) >> 1;
    off_y = (wh - ch) >> 1;

    pRect->left -= off_x;
    pRect->right += off_x + ((ww - cw) % 2);
    pRect->top -= off_y;
    pRect->bottom += off_y + ((wh - ch) % 2);

    //limit the minimum window size
    rdr = (WINDOW_ELEMENT_RENDERER*)GetDefaultWindowElementRenderer ();
    if (rdr) {
        style_info.dwStyle = dwStyle;
        style_info.winType = LFRDR_WINTYPE_MAINWIN;
        minw = rdr->calc_we_metrics(0, &style_info, LFRDR_METRICS_MINWIN_WIDTH);
        minh = rdr->calc_we_metrics(0, &style_info, LFRDR_METRICS_MINWIN_HEIGHT);

        if (ww < minw) {
            pRect->right += minw - ww;
        }
        if (wh < minh) {
            pRect->bottom += minh - wh;
        }
    }

    return TRUE;
}

/* this function is CONTROL safe. */
static int HittestOnNClient (PMAINWIN pWin, int x, int y)
{
    const WINDOWINFO *info = GetWindowInfo ((HWND)pWin);
    return info->we_rdr->hit_test ((HWND)pWin, x, y);
}


/************************** internal functions *********************************/
void gui_WndRect(HWND hWnd, PRECT prc)
{
    PCONTROL pParent;
    PCONTROL pCtrl;

    pParent = pCtrl = (PCONTROL) hWnd;

    if (hWnd == HWND_DESKTOP) {
        *prc = g_rcScr;
        return;
    }

    prc->left = pCtrl->left;
    prc->top  = pCtrl->top;
    prc->right = pCtrl->right;
    prc->bottom = pCtrl->bottom;

    /* dongjunjie 2009/7/9 main window and control as main window
     * don't need offset
     * */
    if (pCtrl->WinType == TYPE_MAINWIN )
        return ;

    while ((pParent = pParent->pParent)) {
        prc->left += pParent->cl;
        prc->top  += pParent->ct;
        prc->right += pParent->cl;
        prc->bottom += pParent->ct;
    }
}

void gui_WndClientRect(HWND hWnd, PRECT prc)
{
    PCONTROL pCtrl;
    PCONTROL pParent;
    pParent = pCtrl = (PCONTROL) hWnd;

    if (hWnd == HWND_DESKTOP) {
        *prc = g_rcScr;
        return;
    }

    prc->left = pCtrl->cl;
    prc->top  = pCtrl->ct;
    prc->right = pCtrl->cr;
    prc->bottom = pCtrl->cb;
    while ((pParent = pParent->pParent)) {
        prc->left += pParent->cl;
        prc->top  += pParent->ct;
        prc->right += pParent->cl;
        prc->bottom += pParent->ct;
    }
}

extern HWND __mg_ime_wnd;
void gui_open_ime_window (PMAINWIN pWin, BOOL open_close, HWND rev_hwnd)
{
#ifndef _MGRM_PROCESSES
    if (__mg_ime_wnd && pWin) {
        int edit_status;

        if (pWin->pMainWin && ((pWin->pMainWin->dwExStyle & WS_EX_TOOLWINDOW)
                    || ((HWND)(pWin->pMainWin) == __mg_ime_wnd)))
            return;

        edit_status = SendAsyncMessage ((HWND)pWin, MSG_DOESNEEDIME, 0, 0L);
        if ((edit_status == IME_WINDOW_TYPE_EDITABLE
                    || edit_status == IME_WINDOW_TYPE_PASSWORD)
                && open_close) {
            SendNotifyMessage (__mg_ime_wnd, MSG_IME_OPEN, 0, 0);
        } else {
            SendNotifyMessage (__mg_ime_wnd, MSG_IME_CLOSE, 0, 0);
        }
    }
#else
    if (!mgIsServer && pWin) {
        BOOL edit_status;
        REQUEST req;
        BOOL open = FALSE;

        if (pWin->pMainWin && (pWin->pMainWin->dwExStyle & WS_EX_TOOLWINDOW))
            return;

        edit_status = SendAsyncMessage ((HWND)pWin, MSG_DOESNEEDIME, 0, 0L);

        req.id = REQID_OPENIMEWND;
        req.data = &open;
        req.len_data = sizeof (BOOL);
        if ((edit_status == IME_WINDOW_TYPE_EDITABLE
                    || edit_status == IME_WINDOW_TYPE_PASSWORD)
                && open_close) {
            open = TRUE;
        }
        ClientRequest (&req, NULL, 0);
    }
#endif
}

static LRESULT DefaultPostMsgHandler(PMAINWIN pWin, UINT message,
        WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case MSG_SETCURSOR:
            /*
             ** NOTE:
             ** this message is only implemented for main window.
             ** for CONTROL, must handle this message and should NOT 
             ** call default window procedure
             ** when handle MSG_SETCURSOR.
             */
            if (wndMouseInWhichControl (pWin, LOSWORD(lParam), HISWORD(lParam), 
                        NULL))
                break;

            if (pWin->hCursor)
                SetCursor(pWin->hCursor);
            break;

        case MSG_NCSETCURSOR:
            /*
             ** NOTE:
             ** this message is only implemented for main window.
             ** for CONTROL, must handle this message and should NOT 
             ** call default window procedure
             ** when handle MSG_SETCURSOR.
             */
            switch (wParam) {
                case HT_BORDER_TOP:
                case HT_BORDER_BOTTOM:
                    SetCursor (GetSystemCursor (IDC_SIZENS));
                    break;
                case HT_BORDER_LEFT:
                case HT_BORDER_RIGHT:
                    SetCursor (GetSystemCursor (IDC_SIZEWE));
                    break;
                case HT_CORNER_TL:
                case HT_CORNER_BR:
                    SetCursor (GetSystemCursor (IDC_SIZENWSE));
                    break;
                case HT_CORNER_BL:
                case HT_CORNER_TR:
                    SetCursor (GetSystemCursor (IDC_SIZENESW));
                    break;
                default:
                    SetCursor (GetSystemCursor (IDC_ARROW));
                    break;
            }
            break;

        case MSG_HITTEST:
            if (PtInRect((PRECT)(&(pWin->cl)), (int)wParam, (int)lParam))
                return HT_CLIENT;
            else {
                CONTROL * pCtrl;
                if(pWin->WinType == TYPE_MAINWIN && ( pCtrl= (CONTROL*)(pWin->hFirstChildAsMainWin)) != NULL){
                    int x = (int)wParam;
                    int y = (int)lParam;
                    ScreenToClient((HWND)pCtrl->pParent,&x,&y);

                    while(pCtrl){
                        if((pCtrl->dwStyle&WS_VISIBLE) && PtInRect((PRECT)&(pCtrl->cl),x, y))
                        {
                            return HT_CLIENT;
                        }
                        pCtrl = pCtrl->next_ctrl_as_main;
                    }
                }
                return HittestOnNClient (pWin, 
                            (int)wParam, (int)lParam);
            }
        break;

        case MSG_CHANGESIZE:
            OnChangeSize (pWin, (PRECT)wParam, (PRECT)lParam);
            RecalcClientArea ((HWND)pWin);
            break;

        case MSG_SIZECHANGING:
            memcpy ((PRECT)lParam, (PRECT)wParam, sizeof (RECT));
            break;

        case MSG_QUERYNCRECT:
            OnQueryNCRect(pWin, (PRECT)lParam);
            break;

        case MSG_QUERYCLIENTAREA:
            OnQueryClientArea(pWin, (PRECT)lParam);
            break;

        case MSG_SETFOCUS:
        case MSG_KILLFOCUS:
            if (pWin->hActiveChild && !lParam)
                SendNotifyMessage (pWin->hActiveChild, message, 0, lParam);
            break;

        case MSG_MOUSEACTIVE:
            if (pWin->WinType == TYPE_CONTROL 
                    && !(pWin->dwStyle & WS_DISABLED)) {

                if (wParam != HT_OUT) {
                    PCONTROL pCtrl = (PCONTROL)pWin;
                    PCONTROL old_active = pCtrl->pParent->active;

                    if (old_active != (PCONTROL)pWin) {
                        if (old_active) {
                            SendNotifyMessage ((HWND)old_active, 
                                    MSG_ACTIVE, FALSE, wParam);
                            SendNotifyMessage ((HWND)old_active, 
                                    MSG_KILLFOCUS, (WPARAM)pWin, 0);
                        }

                        pCtrl->pParent->active = (PCONTROL)pWin;

                        SendNotifyMessage ((HWND)pWin, MSG_ACTIVE, TRUE, 0);
                        SendNotifyMessage ((HWND)pWin, 
                                MSG_SETFOCUS, (WPARAM)old_active, 0);
                    }
                }
            }
            break;

        case MSG_WINDOWDROPPED:
            {
                RECT rc;
                rc.left = LOSWORD (wParam);
                rc.top = HISWORD (wParam);
                rc.right = LOSWORD (lParam);
                rc.bottom = HISWORD (lParam);

                MoveWindow ((HWND)pWin, rc.left, rc.top, 
                        RECTW(rc), RECTH(rc), FALSE);
            }
            break;
    }

    return 0;
}

static void wndEraseBackground(const PMAINWIN pWin, 
        HDC hdc, const RECT* pClipRect)
{
    RECT rcTemp;
    BOOL fGetDC = FALSE;
    const WINDOW_ELEMENT_RENDERER *rdr = 
        GetWindowInfo ((HWND)pWin)->we_rdr;

    if (hdc == 0) {
        hdc = get_valid_dc ((PMAINWIN)pWin, TRUE);
        fGetDC = TRUE;
    }

    if (pClipRect) {
        rcTemp = *pClipRect;
#if 0
        if (pWin->WinType == TYPE_MAINWIN) {
            ScreenToClient ((HWND)pWin, &rcTemp.left, &rcTemp.top);
            ScreenToClient ((HWND)pWin, &rcTemp.right, &rcTemp.bottom);
        }
#endif
    }
    else {
        rcTemp.left = rcTemp.top = 0;
        rcTemp.right = pWin->cr - pWin->cl;
        rcTemp.bottom = pWin->cb - pWin->ct;
    }


    if (rdr->erase_background)
        rdr->erase_background ((HWND)pWin, hdc, &rcTemp);

    /* do secondaryDC update when EndPaint().*/
#if 0
    if (pWin->pMainWin->secondaryDC) {
        HDC real_dc = HDC_INVALID;
        real_dc = GetClientDC ((HWND)pWin->pMainWin);
        update_secondary_dc ((PMAINWIN)pWin, hdc, real_dc, &rcTemp, HT_CLIENT);
        ReleaseDC (real_dc);
    }
#endif

    if (fGetDC) {
        release_valid_dc ((PMAINWIN)pWin, hdc);
    }
}

/* this function is CONTROL safe. */
static void wndDrawNCFrame(MAINWIN* pWin, HDC hdc, const RECT* prcInvalid)
{
    BOOL fGetDC = FALSE;
    const WINDOW_ELEMENT_RENDERER *rdr;
    BOOL is_active = TRUE;

    /* to avoid some unexpected event... */
    if (!MG_IS_WINDOW ((HWND)pWin))
        return;

    if (hdc == 0) {
        hdc = get_valid_dc (pWin, FALSE);
        fGetDC = TRUE;
    }

    if (prcInvalid)
        ClipRectIntersect (hdc, prcInvalid);

    rdr = GetWindowInfo ((HWND)pWin)->we_rdr;

    /*fix bug 3099: GetActiveWindow can not use in control.*/
    if (pWin->WinType == TYPE_MAINWIN)
        is_active = !(pWin->dwStyle & WS_DISABLED) &&
                     (GetActiveWindow() == (HWND)pWin);
    else {
        /*
        is_active = !(pWin->dwStyle & WS_DISABLED) &&
                     (((PCONTROL)pWin)->pParent->active == (PCONTROL)pWin);
        */
    }

    RecalcScrollInfo (pWin, TRUE);
    RecalcScrollInfo (pWin, FALSE);

    rdr->draw_caption ((HWND)pWin, hdc, is_active);
    if (is_active)
        rdr->draw_caption_button ((HWND)pWin, hdc, 0, LFRDR_BTN_STATUS_NORMAL);
    else
        rdr->draw_caption_button ((HWND)pWin, hdc, 0, LFRDR_BTN_STATUS_INACTIVE);
    
    /*dongjunjie 09/07/09
     * If Border is Zero, but MainWindow style have caption
     * we should notify the secondaryDC
     * */
    if (pWin->pMainWin->secondaryDC) {
        draw_secondary_nc_area (pWin, rdr, hdc, HT_CAPTION);
    }

    if (!(pWin->hscroll.status & SBS_HIDE)) {
        wndDrawNCButtonEx (pWin, hdc, HT_HSCROLL, 0);
    }

    if (!(pWin->vscroll.status & SBS_HIDE)) {
        wndDrawNCButtonEx (pWin, hdc, HT_VSCROLL, 0);
    }

    if (rdr->draw_custom_hotspot)
        rdr->draw_custom_hotspot ((HWND)pWin, hdc, 0, 0);

    rdr->draw_border ((HWND)pWin, hdc, is_active);

    if (pWin->pMainWin->secondaryDC) {
        draw_secondary_nc_area (pWin, rdr, hdc, HT_BORDER);
    }

#ifdef _MGHAVE_MENU
    if (pWin->WinType == TYPE_MAINWIN) {
        DrawMenuBarHelper (pWin, hdc, prcInvalid);
        if (pWin->secondaryDC) {
            draw_secondary_nc_area (pWin, rdr, hdc, HT_MENUBAR);
        }
    }
#endif

    if (fGetDC)
        release_valid_dc (pWin, hdc);
}

/* this function is CONTROL safe. */
static void wndActiveMainWindow (PMAINWIN pWin, BOOL fActive)
{
    HDC hdc;
    const WINDOWINFO  *wnd_info;

    hdc = get_valid_dc (pWin, FALSE);
    wnd_info = GetWindowInfo ((HWND)pWin);

    wnd_info->we_rdr->draw_caption ((HWND)pWin, hdc, fActive);
    if (fActive)
        wnd_info->we_rdr->draw_caption_button ((HWND)pWin, hdc, 0, LFRDR_BTN_STATUS_NORMAL);
    else
        wnd_info->we_rdr->draw_caption_button ((HWND)pWin, hdc, 0, LFRDR_BTN_STATUS_INACTIVE);

    if (wnd_info->we_rdr->draw_custom_hotspot)
        wnd_info->we_rdr->draw_custom_hotspot ((HWND)pWin, hdc, 0, 0);

    wnd_info->we_rdr->draw_border ((HWND)pWin, hdc, fActive);
    if (pWin->pMainWin->secondaryDC) {
        draw_secondary_nc_area (pWin, wnd_info->we_rdr, hdc, HT_BORDER);
    }

    release_valid_dc (pWin, hdc);
}

static LRESULT DefaultPaintMsgHandler(PMAINWIN pWin, UINT message,
        WPARAM wParam, LPARAM lParam)
{
    switch( message )
    {
        case MSG_NCPAINT:
            {
                wndDrawNCFrame (pWin, (HDC)wParam, (const RECT*)lParam);
                break;
            }

        case MSG_ERASEBKGND:
            if (pWin->WinType == TYPE_CONTROL &&
                    pWin->dwExStyle & WS_EX_TRANSPARENT)
                break;
            wndEraseBackground (pWin, (HDC)wParam, (const RECT*)lParam);
            break;

        case MSG_NCACTIVATE:
            wndActiveMainWindow (pWin, (BOOL)wParam);
            break;

        case MSG_SYNCPAINT:
            wndActiveMainWindow (pWin, (BOOL)wParam);
            UpdateWindow ((HWND)pWin, TRUE);
            break;

        case MSG_PAINT:
            {
                HDC hdc = BeginPaint ((HWND)pWin);
                EndPaint ((HWND)pWin, hdc);
            }
            break;
    }

    return 0;
}

static LRESULT DefaultControlMsgHandler(PMAINWIN pWin, UINT message,
        WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    int len;

    switch( message )
    {
        case MSG_ENABLE:
            if ( (!(pWin->dwStyle & WS_DISABLED) && !wParam)
                    || ((pWin->dwStyle & WS_DISABLED) && wParam) ) {
                if (wParam)
                    pWin->dwStyle &= ~WS_DISABLED;
                else
                    pWin->dwStyle |=  WS_DISABLED;
            }
            break;

        case MSG_SYSCOMMAND:
            if (wParam == SC_CLOSE)
                SendNotifyMessage ((HWND)pWin, MSG_CLOSE, 0, 0);
            break;

        case MSG_GETTEXTLENGTH:
            if (pWin->spCaption)
                return strlen (pWin->spCaption);
            else
                return 0;

        case MSG_GETTEXT:
            if (pWin->spCaption) {
                char* buffer = (char*)lParam;

                len = MIN (strlen (pWin->spCaption), wParam);
                memcpy (buffer, pWin->spCaption, len);
                buffer [len] = '\0';
                return len;
            }
            else
                return 0;
            break;

        case MSG_FONTCHANGED:
            UpdateWindow ((HWND)pWin, TRUE);
            break;

        case MSG_SETTEXT:
            {
                RECT  rc;
                const WINDOWINFO *info;
                /*
                 ** NOTE:
                 ** this message is only implemented for main window.
                 ** for CONTROL, must handle this message and should NOT 
                 ** call default window procedure
                 ** when handle MSG_SETTEXT.
                 */
                if (pWin->WinType == TYPE_CONTROL)
                    return 0;

                FreeFixStr (pWin->spCaption);
                len = strlen ((char*)lParam);
                pWin->spCaption = FixStrAlloc (len);
                if (len > 0)
                    strcpy (pWin->spCaption, (char*)lParam);

#ifdef _MGRM_PROCESSES
                SendMessage (HWND_DESKTOP, MSG_CHANGECAPTION, (WPARAM) pWin, 0L);
#endif
                hdc = get_valid_dc (pWin, FALSE);
                info = GetWindowInfo ((HWND)pWin);
                SetRectEmpty (&rc);

                if (pWin->dwStyle & WS_MINIMIZEBOX) {
                    info->we_rdr->calc_we_area ((HWND)pWin, HT_MINBUTTON, &rc);
                    --rc.left;
                    --rc.top;
                    ExcludeClipRect (hdc, &rc);
                }

                if (pWin->dwStyle & WS_MAXIMIZEBOX) {
                    info->we_rdr->calc_we_area ((HWND)pWin, HT_MAXBUTTON, &rc);
                    --rc.left;
                    --rc.top;
                    ExcludeClipRect (hdc, &rc);
                }

                if (!(pWin->dwExStyle & WS_EX_NOCLOSEBOX)) {
                    info->we_rdr->calc_we_area ((HWND)pWin, HT_CLOSEBUTTON, &rc);
                    --rc.left;
                    --rc.top;
                    ExcludeClipRect (hdc, &rc);
                }

                /*Draw caption text information*/
                info->we_rdr->draw_caption ((HWND)pWin, 
                        hdc, GetForegroundWindow () == (HWND)pWin);
                if (pWin->pMainWin->secondaryDC) {
                    draw_secondary_nc_area (pWin, info->we_rdr, hdc, HT_CAPTION);
                }
                /*It maybe need to delete. */
#if 0
                if (info->we_rdr->draw_custom_hotspot)
                    info->we_rdr->draw_custom_hotspot ((HWND)pWin, hdc, 0, 0);
#endif
                release_valid_dc (pWin, hdc);
                break;
            }
    }

    return 0;
}

static LRESULT DefaultSessionMsgHandler(PMAINWIN pWin, UINT message,
        WPARAM wParam, LPARAM lParam)
{

    /*
     ** NOTE:
     ** Currently does nothing, should handle fowllowing messages:
     **
     ** MSG_QUERYENDSESSION:
     ** MSG_ENDSESSION:
     */

    return 0;
}

static LRESULT DefaultSystemMsgHandler(PMAINWIN pWin, UINT message, 
        WPARAM wParam, LPARAM lParam)
{

    /*
     ** NOTE:
     ** Currently does nothing, should handle following messages:
     **
     ** MSG_IDLE, MSG_CARETBLINK:
     */

    if (message == MSG_IDLE) {
        if (pWin == gui_GetMainWindowPtrOfControl (sg_repeat_msg.hwnd)) {
            SendNotifyMessage (sg_repeat_msg.hwnd, 
                    sg_repeat_msg.message, 
                    sg_repeat_msg.wParam, sg_repeat_msg.lParam);
        }
    }
    else if (message == MSG_CARETBLINK && pWin->dwStyle & WS_VISIBLE) {
        BlinkCaret ((HWND)pWin);
    }
    /* houhh 20090619. */
#ifdef _MGRM_PROCESSES
    else if (message == MSG_UPDATECLIWIN) {
        __mg_update_window ((HWND)pWin,
                LOSWORD(wParam), HISWORD(wParam),
                LOSWORD(lParam), HISWORD(lParam));
    }
#endif
    return 0;
}

/*
 ** NOTE:
 ** This default main window call-back procedure
 ** also implemented for control.
 */
LRESULT PreDefMainWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PMAINWIN pWin = (PMAINWIN)hWnd;

    if (message > MSG_DT_MOUSEOFF && message <= MSG_DT_RBUTTONDBLCLK)
        return DefaultDTMouseMsgHandler(pWin, message, 
                wParam, LOSWORD (lParam), HISWORD (lParam));
    else if (message >= MSG_FIRSTMOUSEMSG && message <= MSG_NCMOUSEOFF)
        return DefaultMouseMsgHandler(pWin, message, 
                wParam, LOSWORD (lParam), HISWORD (lParam));
    else if (message > MSG_NCMOUSEOFF && message <= MSG_LASTMOUSEMSG)
        return DefaultNCMouseMsgHandler(pWin, message, 
                (int)wParam, LOSWORD (lParam), HISWORD (lParam));
    else if (message >= MSG_FIRSTKEYMSG && message <= MSG_LASTKEYMSG)
        return DefaultKeyMsgHandler(pWin, message, wParam, lParam);
    else if (message >= MSG_FIRSTPOSTMSG && message <= MSG_LASTPOSTMSG)
        return DefaultPostMsgHandler(pWin, message, wParam, lParam);
    else if (message >= MSG_FIRSTCREATEMSG && message <= MSG_LASTCREATEMSG) 
        return DefaultCreateMsgHandler(pWin, message, wParam, lParam);
    else if (message >= MSG_FIRSTPAINTMSG && message <= MSG_LASTPAINTMSG) 
        return DefaultPaintMsgHandler(pWin, message, wParam, lParam);
    else if (message >= MSG_FIRSTSESSIONMSG && message <= MSG_LASTSESSIONMSG) 
        return DefaultSessionMsgHandler(pWin, message, wParam, lParam);
    else if (message >= MSG_FIRSTCONTROLMSG && message <= MSG_LASTCONTROLMSG) 
        return DefaultControlMsgHandler(pWin, message, wParam, lParam);
    else if (message >= MSG_FIRSTSYSTEMMSG && message <= MSG_LASTSYSTEMMSG) 
        return DefaultSystemMsgHandler(pWin, message, wParam, lParam);
#if (defined(_MG_ENABLE_SCREENSAVER) || defined(_MG_ENABLE_WATERMARK)) && defined(_MGRM_THREADS)
    else if (message == MSG_CANCELSCREENSAVER) {
        screensaver_hide();
        SendNotifyMessage (HWND_DESKTOP, MSG_PAINT, 0, 0);
    }
#endif

    return 0;
}

LRESULT PreDefControlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == MSG_SETTEXT)
        return 0;
    else if (message == MSG_SETCURSOR) {
        if (GetWindowExStyle (hWnd) & WS_EX_USEPARENTCURSOR)
            return 0;

        SetCursor (GetWindowCursor (hWnd));
        return 0;
    }
    else if (message == MSG_NCSETCURSOR) {
        SetCursor (GetSystemCursor (IDC_ARROW));
        return 0;
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

LRESULT DefaultWindowProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (IsMainWindow(hWnd)) {
        return DefaultMainWinProc (hWnd, message, wParam, lParam);
    }
    else if (IsControl(hWnd)) {
        return DefaultControlProc (hWnd, message, wParam, lParam);
    }
    else if (IsDialog(hWnd)) {
        return DefaultDialogProc (hWnd, message, wParam, lParam);
    }
    return 0;
}

/************************* GUI calls support ********************************/

static HWND set_focus_helper (HWND hWnd)
{
    PMAINWIN pWin;
    PMAINWIN old_active;

    if (IsMainWindow (hWnd))
        return HWND_INVALID;

    pWin = (PMAINWIN) GetParent (hWnd);
    old_active = (PMAINWIN)pWin->hActiveChild;
    if (old_active != (PMAINWIN)hWnd) {

        if (old_active)
            SendNotifyMessage ((HWND)old_active, MSG_KILLFOCUS, (WPARAM)hWnd, 1);

        pWin->hActiveChild = hWnd;
        SendNotifyMessage (hWnd, MSG_SETFOCUS, (WPARAM)old_active, 1);
    }

    return pWin->hActiveChild;
}

HWND GUIAPI SetFocusChild (HWND hWnd)
{
    HWND hOldActive;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), HWND_INVALID);

    if ((hOldActive = set_focus_helper (hWnd)) != HWND_INVALID) {
        do {
            hWnd = GetParent (hWnd);
        } while (set_focus_helper (hWnd) != HWND_INVALID);
    }

    return hOldActive;
}

HWND GUIAPI GetFocusChild (HWND hParent)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hParent), HWND_INVALID);
    pWin = MG_GET_WINDOW_PTR (hParent);
    return pWin->hActiveChild;
}

HWND GUIAPI SetNullFocus (HWND hParent)
{
    PMAINWIN pWin;
    HWND hOldActive;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hParent), HWND_INVALID);
    pWin = MG_GET_WINDOW_PTR (hParent);

    hOldActive = pWin->hActiveChild;
    if (hOldActive)
        SendNotifyMessage (hOldActive, MSG_KILLFOCUS, 0, 0);
    pWin->hActiveChild = 0;

    return hOldActive;
}

/* NOTE: this function support ONLY main window. */
HWND GUIAPI SetActiveWindow (HWND hMainWnd)
{
    if (!MG_IS_NORMAL_WINDOW(hMainWnd) || !IsMainWindow (hMainWnd))
        return HWND_INVALID;

    return (HWND)SendMessage (HWND_DESKTOP, 
            MSG_SETACTIVEMAIN, (WPARAM)hMainWnd, 0);
}

/* NOTE: this function support ONLY main window. */
HWND GUIAPI GetActiveWindow (void)
{
    return (HWND)SendMessage (HWND_DESKTOP, MSG_GETACTIVEMAIN, 0, 0);
}

HWND GUIAPI SetCapture (HWND hWnd)
{
    PMAINWIN pWin = (PMAINWIN)hWnd;

    if (hWnd == HWND_INVALID || (pWin && pWin->DataType != TYPE_HWND))
        return HWND_INVALID;

    /* if hWnd is HWND_DESKTOP or zero, release capture */
    if (hWnd == HWND_DESKTOP) hWnd = 0;

    return (HWND) SendMessage (HWND_DESKTOP, MSG_SETCAPTURE, (WPARAM)hWnd, 0);
}


HWND GUIAPI GetCapture (void)
{
    return (HWND)SendMessage (HWND_DESKTOP, MSG_GETCAPTURE, 0, 0);
}

void GUIAPI ReleaseCapture (void)
{
    SendMessage (HWND_DESKTOP, MSG_SETCAPTURE, 0, 0);
}

/*************************** Main window and thread **************************/

/* get main window pointer from a handle */
PMAINWIN gui_CheckAndGetMainWindowPtr (HWND hWnd)
{
    MG_CHECK_RET (MG_IS_NORMAL_MAIN_WINDOW(hWnd), NULL);
    return MG_GET_WINDOW_PTR (hWnd);
}

PMAINWIN gui_GetMainWindowPtrOfControl (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), NULL);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->pMainWin;
}

BOOL GUIAPI IsWindow (HWND hWnd)
{
    return MG_IS_NORMAL_WINDOW (hWnd);
}

BOOL GUIAPI IsMainWindow (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);

    pWin = MG_GET_WINDOW_PTR(hWnd);

    return (pWin->WinType == TYPE_MAINWIN);
}

BOOL GUIAPI IsControl (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);

    pWin = MG_GET_WINDOW_PTR(hWnd);

    return (pWin->WinType == TYPE_CONTROL);
}

BOOL GUIAPI IsDialog (HWND hWnd)
{
    return (BOOL)SendAsyncMessage (hWnd, MSG_ISDIALOG, 0, 0);
}

HWND GUIAPI GetMainWindowHandle (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_WINDOW(hWnd), HWND_INVALID);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return (HWND)pWin->pMainWin;
}

HWND GUIAPI GetParent (HWND hWnd)
{
    PCONTROL pChildWin = (PCONTROL)hWnd;

    MG_CHECK_RET (MG_IS_WINDOW(hWnd), HWND_INVALID);

    return (HWND)pChildWin->pParent;
}

HWND GUIAPI GetHosting (HWND hWnd)
{
    PMAINWIN pWin;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hWnd)))
        return HWND_INVALID;

    if (pWin->pHosting == NULL)
        return HWND_DESKTOP;

    return (HWND)(pWin->pHosting);
}

HWND GUIAPI GetFirstHosted (HWND hWnd)
{
    PMAINWIN pWin;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hWnd)))
        return HWND_INVALID;

    return (HWND)(pWin->pFirstHosted);
}

HWND GUIAPI GetNextHosted (HWND hHosting, HWND hHosted)
{
    PMAINWIN pWin;
    PMAINWIN pHosted;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hHosting)))
        return HWND_INVALID;

    if (hHosted == 0) {
        return GetFirstHosted(hHosting);
    }

    if (!(pHosted = gui_CheckAndGetMainWindowPtr (hHosted)))
        return HWND_INVALID;

    if (pHosted->pHosting != pWin)
        return HWND_INVALID;

    return (HWND)(pHosted->pNextHosted);
}

HWND GUIAPI GetNextChild (HWND hWnd, HWND hChild)
{
    PCONTROL pControl, pChild;

    if (hChild == HWND_INVALID) return HWND_INVALID;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), HWND_INVALID);
    pControl = MG_GET_CONTROL_PTR(hWnd);

    pChild = (PCONTROL)hChild;

    if (pChild == NULL) {
        return (HWND)pControl->children;
    }
    else if (pControl != pChild->pParent) {
        return HWND_INVALID;
    }

    return (HWND)pChild->next;
}

HWND GUIAPI GetNextMainWindow (HWND hMainWnd)
{
    PMAINWIN pMainWin;

    if (hMainWnd == HWND_DESKTOP || hMainWnd == 0)
        pMainWin = NULL;
    else if (!(pMainWin = gui_CheckAndGetMainWindowPtr (hMainWnd)))
        return HWND_INVALID;

    return (HWND) SendMessage (HWND_DESKTOP, 
            MSG_GETNEXTMAINWIN, (WPARAM)pMainWin, 0L);
}

/*
 * TODO: implement ScrollWindowEx completely.
 */
int GUIAPI ScrollWindowEx (HWND hWnd, int dx, int dy,
                const RECT *prcScroll, const RECT *prcClip,
                PCLIPRGN pRgnUpdate, PRECT prcUpdate, UINT flags)
{
    SCROLLWINDOWINFO swi;
    RECT rcClient, rcScroll;
    BOOL fCaret;
    PMAINWIN pWin;

    if ((dx == 0 && dy == 0))
        return -1;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), -1);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    GetClientRect (hWnd, &rcClient);
    if (prcScroll)
        IntersectRect (&rcScroll, prcScroll, &rcClient);
    else
        rcScroll = rcClient;

    /*
     * BUGFIX: if we scrolled all area, the all area must
     * be set as invalidate (dongjunjie 2010/7/28)
     */
    if((dx != 0 && abs(dx) >= RECTW(rcScroll))
        || (dy != 0 && abs(dy) >= RECTH(rcScroll)))
    {
        InvalidateRect(hWnd, &rcScroll, TRUE);
        goto UPDATERGIN;
    }

    swi.iOffx = dx;
    swi.iOffy = dy;
    swi.rc1   = &rcScroll;
    swi.rc2   = prcClip;

    /* hide caret */
    fCaret = HideCaretEx (hWnd, FALSE);

    /* Modify the children's position before scrolling */
    if (flags & SW_SCROLLCHILDREN) {
        PCONTROL child;
        child = ((PCONTROL)hWnd)->children;
        while (child) {

            if ((prcClip == NULL) || IsCovered ((const RECT*)&child->left, prcClip)) {
                child->left += dx; child->top += dy;
                child->right += dx; child->bottom += dy;
                child->cl += dx; child->ct += dy;
                child->cr += dx; child->cb += dy;
            }

            child = child->next;
        }
    }

    SendMessage (HWND_DESKTOP, MSG_SCROLLMAINWIN, (WPARAM)hWnd, (LPARAM)(&swi));

    /* show caret */
    if (fCaret) ShowCaretEx (hWnd, FALSE);

UPDATERGIN:
    /* FIXME: we use the invalid region of the window as the update region directly. */
    if (pRgnUpdate) {
        /* copy from window's invalidate region */
        RECT rcClient;
        RECT rc;
        CopyRegion (pRgnUpdate, &pWin->InvRgn.rgn);
        /* FIXME: I have not the best idea to do this */
        GetClientRect(hWnd, &rcClient);
        rc = rcClient;    
        if(dx > 0)
            rc.right = rc.left + dx + 1;
        else if(dx < 0)
            rc.left = rc.right + dx - 1;
        if(dx != 0)
            AddClipRect(pRgnUpdate, &rc);
        rc = rcClient;
        if(dy > 0)
            rc.bottom = rc.top + dy + 1;
        else if(dy < 0)
            rc.top = rc.bottom + dy - 1;
        if(dy != 0)
            AddClipRect(pRgnUpdate, &rc);
    }
    if (prcUpdate) {
        if(pRgnUpdate)
            *prcUpdate = pRgnUpdate->rcBound;
        else if(dx != 0 && dy != 0)
            GetClientRect(hWnd, prcUpdate);
        else if(dx != 0)
        {
            GetClientRect(hWnd, prcUpdate);
            if(dx > 0)
                prcUpdate->right = prcUpdate->left + dx + 1;
            else
                prcUpdate->left = prcUpdate->right + dx - 1;
        }
        else if(dy != 0)
        {
            GetClientRect(hWnd, prcUpdate);
            if(dy > 0)
                prcUpdate->bottom = prcUpdate->top + dx + 1;
            else
                prcUpdate->top = prcUpdate->bottom + dx - 1;
        }
        else
            *prcUpdate = pWin->InvRgn.rgn.rcBound;
    }

    return pWin->InvRgn.rgn.type;
}

static PLFSCROLLBARINFO wndGetScrollBar (MAINWIN* pWin, int iSBar)
{
    if (iSBar == SB_HORZ) {
        if (pWin->dwStyle & WS_HSCROLL)
            return &pWin->hscroll;
    }
    else if (iSBar == SB_VERT) {
        if (pWin->dwStyle & WS_VSCROLL)
            return &pWin->vscroll;
    }

    return NULL;
}

BOOL GUIAPI EnableScrollBar (HWND hWnd, int iSBar, BOOL bEnable)
{
    PLFSCROLLBARINFO pSBar;
    PMAINWIN pWin;
    BOOL bPrevState;
    RECT rcBar;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if ( !(pSBar = wndGetScrollBar (pWin, iSBar)) )
        return FALSE;

    bPrevState = !(pSBar->status & SBS_DISABLED);

    if (bEnable && !bPrevState)
        pSBar->status &= ~SBS_DISABLED;
    else if (!bEnable && bPrevState)
        pSBar->status |= SBS_DISABLED;
    else
        return FALSE;

    if (iSBar == SB_VERT)
        wndGetVScrollBarRect (pWin, &rcBar);
    else
        wndGetHScrollBarRect (pWin, &rcBar);

    SendAsyncMessage (hWnd, MSG_NCPAINT, 0, (LPARAM)(&rcBar));

    return TRUE;
}

BOOL GUIAPI GetScrollPos (HWND hWnd, int iSBar, int* pPos)
{
    PLFSCROLLBARINFO pSBar;
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if ( !(pSBar = wndGetScrollBar (pWin, iSBar)) )
        return FALSE;

    *pPos = pSBar->curPos;
    return TRUE;
}

BOOL GUIAPI GetScrollRange (HWND hWnd, int iSBar, int* pMinPos, int* pMaxPos)
{
    PLFSCROLLBARINFO pSBar;
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if ( !(pSBar = wndGetScrollBar (pWin, iSBar)) )
        return FALSE;

    *pMinPos = pSBar->minPos;
    *pMaxPos = pSBar->maxPos;
    return TRUE;
}

BOOL GUIAPI SetScrollPos (HWND hWnd, int iSBar, int iNewPos)
{
    PLFSCROLLBARINFO pSBar;
    PMAINWIN pWin;
    //const WINDOWINFO  *info = GetWindowInfo (hWnd);

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if ( !(pSBar = wndGetScrollBar (pWin, iSBar)) )
        return FALSE;

    if (iNewPos == pSBar->curPos)
        return TRUE;

    if (iNewPos < pSBar->minPos)
        pSBar->curPos = pSBar->minPos;
    else
        pSBar->curPos = iNewPos;

    {
        int max = pSBar->maxPos;
        max -= ((pSBar->pageStep - 1) > 0)?(pSBar->pageStep - 1):0;

        if (pSBar->curPos > max)
            pSBar->curPos = max;
    }

    RecalcScrollInfo (pWin, iSBar == SB_HORZ);

    if(iSBar == SB_VERT)
    {
        //        if (!(pWin->vscroll.status & SBS_HIDE ||
        //             pWin->vscroll.status & SBS_DISABLED))
        wndDrawNCButton (pWin, HT_VSCROLL, 0);
    }
    else
    {
        //        if (!(pWin->hscroll.status & SBS_HIDE ||
        //             pWin->hscroll.status & SBS_DISABLED))
        wndDrawNCButton (pWin, HT_HSCROLL, 0);
    }

    return TRUE;
}

BOOL GUIAPI SetScrollRange (HWND hWnd, int iSBar, int iMinPos, int iMaxPos)
{
    PLFSCROLLBARINFO pSBar;
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if ( !(pSBar = wndGetScrollBar (pWin, iSBar)) )
        return FALSE;

    if (iMinPos == pSBar->minPos && iMaxPos == pSBar->maxPos)
        return TRUE;

    pSBar->minPos = (iMinPos < iMaxPos)?iMinPos:iMaxPos;
    pSBar->maxPos = (iMinPos > iMaxPos)?iMinPos:iMaxPos;

    /* validate parameters. */
    if (pSBar->curPos < pSBar->minPos)
        pSBar->curPos = pSBar->minPos;

    if (pSBar->pageStep <= 0)
        pSBar->pageStep = 0;
    else if (pSBar->pageStep > (pSBar->maxPos - pSBar->minPos + 1))
        pSBar->pageStep = pSBar->maxPos - pSBar->minPos + 1;

    {
        int max = pSBar->maxPos;
        max -= ((pSBar->pageStep - 1) > 0)?(pSBar->pageStep - 1):0;

        if (pSBar->curPos > max)
            pSBar->curPos = max;
    }

    RecalcScrollInfo (pWin, iSBar == SB_HORZ);

    if(iSBar == SB_VERT)
    {
        //        if (!(pWin->vscroll.status & SBS_HIDE ||
        //             pWin->vscroll.status & SBS_DISABLED))
        wndDrawNCButton (pWin, HT_VSCROLL, 0);
    }
    else
    {
        //        if (!(pWin->hscroll.status & SBS_HIDE ||
        //             pWin->hscroll.status & SBS_DISABLED))
        wndDrawNCButton (pWin, HT_HSCROLL, 0);
    }
    return TRUE;
}

int wndScrollBarSliderStartPos (MAINWIN *pWin, int iSBar)
{
    int start = 0; 
    int sb = GetWindowElementAttr ((HWND)pWin, WE_METRICS_SCROLLBAR);

    if (iSBar == SB_HORZ) {
        RECT rcHBar;

        wndGetHScrollBarRect (pWin, &rcHBar);
        start = rcHBar.left +
            pWin->hscroll.barStart + sb;
    }
    else if (iSBar == SB_VERT) {
        RECT rcVBar;

        wndGetVScrollBarRect (pWin, &rcVBar);
        start = rcVBar.top + 
            pWin->vscroll.barStart + sb;

        if (start + pWin->vscroll.barLen > rcVBar.bottom)
            start = rcVBar.bottom - pWin->vscroll.barLen;
    }

    return start;
}

BOOL GUIAPI SetScrollInfo (HWND hWnd, int iSBar, 
        const SCROLLINFO* lpsi, BOOL fRedraw)
{
    PLFSCROLLBARINFO pSBar;
    PMAINWIN pWin;
    RECT rcBar;
    DWORD changed_mask = 0;
    int sb;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if ( !(pSBar = wndGetScrollBar (pWin, iSBar)) )
        return FALSE;

    if (lpsi->fMask & SIF_RANGE) {
        if (pSBar->minPos != lpsi->nMin || pSBar->maxPos != lpsi->nMax) {
            changed_mask |= SIF_RANGE;
        }
    }

    if (lpsi->fMask & SIF_POS && pSBar->curPos != lpsi->nPos) {
        changed_mask |= SIF_POS;
    }

    if (lpsi->fMask & SIF_PAGE && pSBar->pageStep != lpsi->nPage) {
        changed_mask |= SIF_PAGE;
    }

    if (changed_mask == 0)
        return TRUE;
    else if (changed_mask == SIF_POS)
        return SetScrollPos (hWnd, iSBar, lpsi->nPos);

    if (changed_mask & SIF_RANGE) {
        pSBar->minPos = (lpsi->nMin < lpsi->nMax)?lpsi->nMin:lpsi->nMax;
        pSBar->maxPos = (lpsi->nMin < lpsi->nMax)?lpsi->nMax:lpsi->nMin;
    }

    if (changed_mask & SIF_POS)
        pSBar->curPos = lpsi->nPos;

    if (changed_mask & SIF_PAGE)
        pSBar->pageStep = lpsi->nPage;

    /* validate parameters. */
    if (pSBar->curPos < pSBar->minPos)
        pSBar->curPos = pSBar->minPos;

    if (pSBar->pageStep <= 0)
        pSBar->pageStep = 0;
    else if (pSBar->pageStep > (pSBar->maxPos - pSBar->minPos + 1))
        pSBar->pageStep = pSBar->maxPos - pSBar->minPos + 1;

    {
        int max = pSBar->maxPos;
        max -= ((pSBar->pageStep - 1) > 0)?(pSBar->pageStep - 1):0;

        if (pSBar->curPos > max)
            pSBar->curPos = max;
    }

    if (fRedraw) {
        HDC hdc;

        if (iSBar == SB_VERT)
            wndGetVScrollBarRect (pWin, &rcBar);
        else
            wndGetHScrollBarRect (pWin, &rcBar);

        RecalcScrollInfo (pWin, iSBar == SB_HORZ);

        sb = GetWindowElementAttr ((HWND)pWin, WE_METRICS_SCROLLBAR);
        if (iSBar == SB_VERT) {
            rcBar.top += sb;
            rcBar.bottom -= sb;
        }
        else {
            rcBar.left += sb;
            rcBar.right -= sb;
        }

        hdc = GetDC (hWnd);
        ClipRectIntersect (hdc, &rcBar);

        if(iSBar == SB_VERT)
        {
            //            if (!(pWin->vscroll.status & SBS_HIDE ||
            //                 pWin->vscroll.status & SBS_DISABLED))
            wndDrawNCButton (pWin, HT_VSCROLL, 0);
        }
        else
        {
            //            if (!(pWin->hscroll.status & SBS_HIDE ||
            //                 pWin->hscroll.status & SBS_DISABLED))
            wndDrawNCButton (pWin, HT_HSCROLL, 0);
        }

        ReleaseDC (hdc);
    }

    return TRUE;
}

BOOL GUIAPI GetScrollInfo (HWND hWnd, int iSBar, PSCROLLINFO lpsi)
{
    PLFSCROLLBARINFO pSBar;
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if (!(pSBar = wndGetScrollBar (pWin, iSBar)))
        return FALSE;

    if (lpsi->fMask & SIF_RANGE) {
        lpsi->nMin = pSBar->minPos;
        lpsi->nMax = pSBar->maxPos;
    }

    if (lpsi->fMask & SIF_POS) {
        lpsi->nPos = pSBar->curPos;
    }

    if (lpsi->fMask & SIF_PAGE)
        lpsi->nPage = pSBar->pageStep;

    return TRUE;
}

BOOL GUIAPI ShowScrollBar (HWND hWnd, int iSBar, BOOL bShow)
{
    // 
    PLFSCROLLBARINFO pSBar;
    PMAINWIN pWin;
    BOOL bPrevState;
    RECT rcBar;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if ( !(pSBar = wndGetScrollBar (pWin, iSBar)) )
    {
        return FALSE;
    }

    bPrevState = !(pSBar->status & SBS_HIDE);

    if( (bPrevState && bShow) || (!bPrevState && !bShow))
    {
        return FALSE;
    }

    //TO AVOID THE DEAD LOOP
    if (!bPrevState 
        && ((pSBar->minPos == pSBar->maxPos) 
            || (pSBar->pageStep >= (pSBar->maxPos - pSBar->minPos)))) 
    {
        return FALSE;
    }

    if (iSBar == SB_VERT)
        wndGetVScrollBarRect (pWin, &rcBar);
    else
        wndGetHScrollBarRect (pWin, &rcBar);

    //HEAR, status Changed,
    if(bShow)
        pSBar->status &= ~SBS_HIDE;
    else
        pSBar->status |= SBS_HIDE;

    //Hear , MSG_CHANGESIZE would call OnChangeSize
    //OnChangeSize May be modify pSBar->status as 100
    //( pSBar->status|=SBS_HIDE), when
    // pSBar's pageStep >= max -min
    // And Then, RecalcClientArea would be called,
    // RecalcClientArea will send MSG_CHANGESIZE,
    // In some controls, it will process MSG_CHANGESIZE
    // and call ShowScrollBar too.
    // Becareful the MSG_CHANGESIZE !!!
    SendAsyncMessage (hWnd, MSG_CHANGESIZE, 0, 0);

    /* houhh 20081006, pSbar->status |= SBS_HIDE, Get rcBar is error.
     */
#if 0
    if (iSBar == SB_VERT)
        wndGetVScrollBarRect (pWin, &rcBar);
    else
        wndGetHScrollBarRect (pWin, &rcBar);
#endif

    InflateRect (&rcBar, 1, 1);

    if (bShow) {
        SendAsyncMessage (hWnd, MSG_NCPAINT, 0, 0);
    }
    else {
        /* tranlate the rcBar coordinate relative to client area. */
        RECT cli_rc;
        const WINDOWINFO *info = GetWindowInfo (hWnd);
        info->we_rdr->calc_we_area (hWnd, HT_CLIENT, &cli_rc);
        SendAsyncMessage (hWnd, MSG_NCPAINT, 0, 0);
        rcBar.left   -= cli_rc.left;
        rcBar.top    -= cli_rc.top;
        rcBar.right  -= cli_rc.left;
        rcBar.bottom -= cli_rc.top;
        InvalidateRect (hWnd, &rcBar, TRUE);
    }

    return TRUE;
}

/*************************** Main window creation ****************************/
#ifdef _MGRM_THREADS
int GUIAPI CreateThreadForMainWindow (pthread_t* thread,
        pthread_attr_t* attr,
        void * (*start_routine)(void *),
        void * arg)
{
    pthread_attr_t new_attr;
    int ret;

    pthread_attr_init (&new_attr);
    if (attr == NULL) {
        attr = &new_attr;
        pthread_attr_setdetachstate (attr, PTHREAD_CREATE_DETACHED);
#ifdef __NOUNIX__
        /* hope 16KB is enough for MiniGUI */
        pthread_attr_setstacksize (attr, 16 * 1024);
#endif
    }

    ret = pthread_create(thread, attr, start_routine, arg);

    pthread_attr_destroy (&new_attr);

    return ret;
}

int GUIAPI WaitMainWindowClose (HWND hWnd, void** returnval)
{
    pthread_t th;

    if (!(gui_CheckAndGetMainWindowPtr(hWnd)))
        return -1;

    th = GetMainWinThread (hWnd);

    return pthread_join (th, returnval);
}
#endif

void GUIAPI MainWindowThreadCleanup (HWND hMainWnd)
{
    PMAINWIN pWin = (PMAINWIN)hMainWnd;

    _MG_PRINTF ("GUI>Window: MainWindowThreadCleanup called: %p (%s)\n", pWin, pWin->spCaption);

    if (!MG_IS_DESTROYED_WINDOW (hMainWnd)) {
        _MG_PRINTF ("GUI>Window: Unexpected calling of "
                "(MainWindowThreadCleanup); Window (%p) "
                "not destroyed yet!\n", hMainWnd);
        return;
    }

#ifdef _MGRM_THREADS
    if (pWin->pHosting == NULL) {
        mg_FreeMsgQueueThisThread ();
        _MG_PRINTF ("GUI>Window: Message queure is freed: %p (%s)\n", pWin, pWin->spCaption);
    }
#endif

#ifdef __THREADX__ 
    /* to avoid threadx keep pWin's value,which will lead to wrong way */
    memset (pWin, 0xcc, sizeof(MAINWIN));
#endif

    free (pWin);
}

#ifdef __TARGET_FMSOFT__
/* To handle main window offset */
int __mg_mainwin_offset_x;
int __mg_mainwin_offset_y;
#endif

static void set_window_renderer (PMAINWIN pWin, const char* werdr_name)
{
    const WINDOW_ELEMENT_RENDERER* renderer = 
        GetWindowRendererFromName (werdr_name);

    if (renderer)
        pWin->we_rdr = (WINDOW_ELEMENT_RENDERER*)renderer;
    else
        pWin->we_rdr = __mg_def_renderer;

    if (pWin->we_rdr)                               
        ++pWin->we_rdr->refcount;
}

/*
 * ResetMenuSize:
 * reset menu height
 *
 * Author : NuohuaZhou
 * Data   : 2007-11-22
 */
static void ResetMenuSize (HWND hwnd)
{
    /** font size of renderer */
    int font_size;

    /** min menu size of renderer*/
    int menu_height_min;

    /** expect menu size of renderer*/
    int menu_height_expect;

    font_size = ((PLOGFONT)GetWindowElementAttr (hwnd, WE_FONT_MENU))->size;
    menu_height_min = font_size + (LFRDR_MENUITEMOFFY_MIN << 1);
    menu_height_expect = GetWindowElementAttr (hwnd, WE_METRICS_MENU);

    /** reset menu height */
    if (menu_height_expect < menu_height_min) {
        SetWindowElementAttr (hwnd, WE_METRICS_MENU, menu_height_min);
    }
}

HWND GUIAPI CreateMainWindowEx (PMAINWINCREATE pCreateInfo, 
        const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs,
        const char* window_name, const char* layer_name)
{
    // 
    PMAINWIN pWin;

    if (pCreateInfo == NULL) {

        return HWND_INVALID;
    }

    if (!(pWin = calloc(1, sizeof(MAINWIN)))) {

        return HWND_INVALID;
    }

#ifdef _MGRM_THREADS
    if (pCreateInfo->hHosting == HWND_DESKTOP || pCreateInfo->hHosting == 0) {
        /*
         ** Create thread infomation and message queue for this new main window.
         */
        if ((pWin->pMessages = GetMsgQueueThisThread ()) == NULL) {
            if (!(pWin->pMessages = mg_InitMsgQueueThisThread ()) ) {
                free (pWin);

                return HWND_INVALID;
            }
            pWin->pMessages->pRootMainWin = pWin;
        }
        else {
            /* Already have a top level main window, in case of user have set 
               a wrong hosting window */
            pWin->pHosting = pWin->pMessages->pRootMainWin;
        }
    }
    else {
        pWin->pMessages = GetMsgQueueThisThread ();
        if (pWin->pMessages != kernel_GetMsgQueue (pCreateInfo->hHosting) ||
                pWin->pMessages == NULL) {
            free (pWin);

            return HWND_INVALID;
        }
    }

    if (pWin->pHosting == NULL)
        pWin->pHosting = gui_GetMainWindowPtrOfControl (pCreateInfo->hHosting);
    /* leave the pHosting is NULL for the first window of this thread. */
#else
    pWin->pHosting = gui_GetMainWindowPtrOfControl (pCreateInfo->hHosting);
    if (pWin->pHosting == NULL)
        pWin->pHosting = __mg_dsk_win;

    pWin->pMessages = __mg_dsk_msg_queue;
#endif

    pWin->pMainWin      = pWin;
    pWin->hParent       = 0;
    pWin->pFirstHosted  = NULL;
    pWin->pNextHosted   = NULL;
    pWin->DataType      = TYPE_HWND;
    pWin->WinType       = TYPE_MAINWIN;

#ifdef _MGRM_THREADS
    pWin->th            = pthread_self();
#endif

    pWin->hFirstChild   = 0;
    pWin->hActiveChild  = 0;
    pWin->hOldUnderPointer = 0;
    pWin->hPrimitive    = 0;

    pWin->NotifProc     = NULL;

    pWin->dwStyle       = pCreateInfo->dwStyle;
    pWin->dwExStyle     = pCreateInfo->dwExStyle;

#ifdef _MGHAVE_MENU
    pWin->hMenu         = pCreateInfo->hMenu;
#else
    pWin->hMenu         = 0;
#endif
    pWin->hCursor       = pCreateInfo->hCursor;
    pWin->hIcon         = pCreateInfo->hIcon;

#ifdef _MGHAVE_MENU
    if ((pWin->dwStyle & WS_CAPTION) && (pWin->dwStyle & WS_SYSMENU))
        pWin->hSysMenu= CreateSystemMenu ((HWND)pWin, pWin->dwStyle);
    else
#endif
        pWin->hSysMenu = 0;

    pWin->spCaption    = FixStrAlloc (strlen (pCreateInfo->spCaption));
    if (pCreateInfo->spCaption [0])
        strcpy (pWin->spCaption, pCreateInfo->spCaption);

    pWin->MainWindowProc = pCreateInfo->MainWindowProc;
    pWin->iBkColor    = pCreateInfo->iBkColor;

    pWin->pCaretInfo = NULL;

    pWin->dwAddData   = pCreateInfo->dwAddData;
    pWin->dwAddData2  = 0;
    pWin->secondaryDC = 0;

    /* Scroll bar */
    if (pWin->dwStyle & WS_VSCROLL) {
        pWin->vscroll.minPos = 0;
        pWin->vscroll.maxPos = 100;
        pWin->vscroll.curPos = 0;
        pWin->vscroll.pageStep = 101;
        pWin->vscroll.barStart = 0;
        pWin->vscroll.barLen = 10;
        pWin->vscroll.status = SBS_NORMAL;
    }
    else
        pWin->vscroll.status = SBS_HIDE | SBS_DISABLED;

    if (pWin->dwStyle & WS_HSCROLL) {
        pWin->hscroll.minPos = 0;
        pWin->hscroll.maxPos = 100;
        pWin->hscroll.curPos = 0;
        pWin->hscroll.pageStep = 101;
        pWin->hscroll.barStart = 0;
        pWin->hscroll.barLen = 10;
        pWin->hscroll.status = SBS_NORMAL;
    }
    else
        pWin->hscroll.status = SBS_HIDE | SBS_DISABLED;

    /** perfer to use parent renderer */
    if (pWin->dwExStyle & WS_EX_USEPARENTRDR) {
        if (((PMAINWIN)pCreateInfo->hHosting)->we_rdr) {
            pWin->we_rdr = ((PMAINWIN)pCreateInfo->hHosting)->we_rdr;
            ++pWin->we_rdr->refcount;
        }
        else {
            return HWND_INVALID;
        }
    }
    else {
        /** set window renderer */
        set_window_renderer (pWin, werdr_name);
    }

    /** set window element data */
    while (we_attrs && we_attrs->we_attr_id != -1) {
        // append_window_element_data (pWin, 
        //       we_attrs->we_attr_id, we_attrs->we_attr);
        DWORD _old;
        set_window_element_data ((HWND)pWin, 
                we_attrs->we_attr_id, we_attrs->we_attr, &_old);
        ++we_attrs;
    }

    /** prefer to parent font */
    if (pWin->dwExStyle & WS_EX_USEPARENTFONT)
        pWin->pLogFont = __mg_dsk_win->pLogFont;
    else {
        pWin->pLogFont = GetSystemFont (SYSLOGFONT_WCHAR_DEF);
    }

    if (SendMessage ((HWND)pWin, MSG_NCCREATE, 0, (LPARAM)pCreateInfo))
        goto err;

    /** reset menu size */
    ResetMenuSize ((HWND)pWin);

#ifdef __TARGET_FMSOFT__
    pCreateInfo->lx += __mg_mainwin_offset_x;
    pCreateInfo->rx += __mg_mainwin_offset_x;
    pCreateInfo->ty += __mg_mainwin_offset_y;
    pCreateInfo->by += __mg_mainwin_offset_y;
#endif

    SendMessage ((HWND)pWin, MSG_SIZECHANGING, 
            (WPARAM)&pCreateInfo->lx, (LPARAM)&pWin->left);
    SendMessage ((HWND)pWin, MSG_CHANGESIZE, (WPARAM)&pWin->left, 0);

    pWin->pGCRInfo = &pWin->GCRInfo;

    if (SendMessage (HWND_DESKTOP, MSG_ADDNEWMAINWIN, (WPARAM) pWin, 0) < 0)
        goto err;

    /* houhh20081127, Move these code into dskAddNewMainWindow().*/
#if 0
    /* Create secondary window dc. */
    if (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC)
        pWin->secondaryDC = CreateSecondaryDC ((HWND)pWin);
#endif

    /* 
     * We should add the new main window in system and then
     * SendMessage MSG_CREATE for application to create
     * child windows.
     */
    if (SendMessage ((HWND)pWin, MSG_CREATE, 0, (LPARAM)pCreateInfo)) {
        SendMessage(HWND_DESKTOP, MSG_REMOVEMAINWIN, (WPARAM)pWin, 0);
        goto err;
    }

#if 0
    /* Create private client dc. */
    if (pWin->dwExStyle & WS_EX_USEPRIVATECDC) {
        if (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC)
            pWin->privCDC = GetSecondarySubDC (pWin->secondaryDC, (HWND)pWin, TRUE);
        else
            pWin->privCDC = CreatePrivateClientDC ((HWND)pWin);
    }
    else
        pWin->privCDC = 0;
#endif

#ifndef _MGRM_PROCESSES
    screensaver_create();
#endif

    return (HWND)pWin;

err:
#ifdef _MGRM_THREADS
    if (pWin->pMessages && pWin->pHosting == NULL) {
        mg_FreeMsgQueueThisThread ();
    }
#endif

    if (pWin->secondaryDC) DeleteSecondaryDC ((HWND)pWin);
    free (pWin);


    return HWND_INVALID;
}

BOOL GUIAPI DestroyMainWindow (HWND hWnd)
{
    PMAINWIN pWin;
    PMAINWIN head, next;    /* hosted window list. */

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hWnd))) return FALSE;

    if (SendMessage (hWnd, MSG_DESTROY, 0, 0))
        return FALSE;

    /* destroy all controls of this window */
    DestroyAllControls (hWnd);

    /* destroy all hosted main windows and dialogs here. */
    head = pWin->pFirstHosted;
    while (head) {
        next = head->pNextHosted;

        if (IsDialog((HWND)head)) {
            EndDialog ((HWND)head, IDCANCEL);
        }
        else {
            if (DestroyMainWindow ((HWND)head))
                MainWindowCleanup ((HWND)head);
        }

        head = next;
    }

    /* kill all timers of this window */
    KillTimer (hWnd, 0);

    SendMessage(HWND_DESKTOP, MSG_REMOVEMAINWIN, (WPARAM)hWnd, 0);

    if (sg_repeat_msg.hwnd == hWnd)
        sg_repeat_msg.hwnd = 0;

    /* make the window to be invalid for PeekMessageEx, PostMessage etc */
    pWin->DataType = TYPE_WINTODEL;

    ThrowAwayMessages (hWnd);

    /* houhh 20081127, move these code to .*/
#if 0
    if ((pWin->dwExStyle & WS_EX_AUTOSECONDARYDC) && pWin->secondaryDC) {
        DeleteSecondaryDC (hWnd);
        pWin->update_secdc = NULL;
    }

    if (pWin->privCDC) {
        if (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC) {
            ReleaseSecondarySubDC (pWin->privCDC);
        }
        else
            DeletePrivateDC (pWin->privCDC);
        pWin->privCDC = 0;
    }
#endif

    if (pWin->spCaption) {
        FreeFixStr (pWin->spCaption);
        pWin->spCaption = NULL;
    }

#ifdef _MGHAVE_MENU
    if (pWin->hMenu) {
        DestroyMenu (pWin->hMenu);
        pWin->hMenu = 0;
    }

    if (pWin->hSysMenu) {
        DestroyMenu (pWin->hSysMenu);
        pWin->hSysMenu = 0;
    }
#endif

    EmptyClipRgn (&pWin->pGCRInfo->crgn);
    EmptyClipRgn (&pWin->InvRgn.rgn);

    free_window_element_data (hWnd);
    --pWin->we_rdr->refcount;

#ifdef _MGRM_THREADS
    pthread_mutex_destroy (&pWin->pGCRInfo->lock);
    pthread_mutex_destroy (&pWin->InvRgn.lock);
#endif

    return TRUE;
}

/*************************** Main window creation ****************************/
void GUIAPI UpdateWindow (HWND hWnd, BOOL fErase)
{
    MG_CHECK (MG_IS_NORMAL_WINDOW(hWnd));

    if (fErase)
        SendAsyncMessage (hWnd, MSG_CHANGESIZE, 0, 0);

    SendAsyncMessage (hWnd, MSG_NCPAINT, 0, 0);
    if (fErase)
        InvalidateRect (hWnd, NULL, TRUE);
    else
        InvalidateRect (hWnd, NULL, FALSE);

#if 0
#if 0
    MSG Msg;
    HWND hMainWnd;

    /* MSG_PAINT Must Use PostMessage. Becuse Only PostMessage can give a correct order
     * when processing MSG_PAINT message
     *
     * the Order of MSG_PAINT:
     *     * The Parent Window Must Be Painted before Children
     *     * The Previously created window must be Painted before afterwards created
     *
     * */
    if (hWnd != HWND_DESKTOP) {
        PMAINWIN pWin;

        pWin = (PMAINWIN) hWnd;
        SendMessage (hWnd, MSG_PAINT, 0, (LPARAM)(&pWin->InvRgn.rgn));
    }
    else
        SendMessage (hWnd, MSG_PAINT, 0, 0);
        
#else
    hMainWnd = GetMainWindowHandle(hWnd);
    while(PeekMessageEx(&Msg, hMainWnd, MSG_PAINT, MSG_PAINT + 1 , FALSE, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
#endif
#else /* a new implementation only check the window and its children. */
    {
        PMAINWIN pWin, winStartToUpdate;

        winStartToUpdate = pWin = (PMAINWIN) hWnd;
        while (pWin != pWin->pMainWin) {
            if ((pWin->dwExStyle & WS_EX_TRANSPARENT)) {
                winStartToUpdate = (PMAINWIN) pWin->hParent;
            }
            pWin = (PMAINWIN) pWin->hParent;
        }

        while ((hWnd = kernel_CheckInvalidRegion (winStartToUpdate))) {
            pWin = (PMAINWIN) hWnd;
            SendMessage (hWnd, MSG_PAINT, 0, (LPARAM)(&pWin->InvRgn.rgn));
        }
    }
#endif
}

MG_EXPORT void GUIAPI UpdateInvalidRect (HWND hWnd, BOOL bErase);
void GUIAPI UpdateInvalidRect (HWND hWnd, BOOL fErase)
{
    MG_CHECK (MG_IS_NORMAL_WINDOW(hWnd));

    if (fErase)
        SendAsyncMessage (hWnd, MSG_CHANGESIZE, 0, 0);

    SendAsyncMessage (hWnd, MSG_NCPAINT, 0, 0);

    /* a new implementation only check the window and its children. */
    {
        PMAINWIN pWin, winStartToUpdate;

        winStartToUpdate = pWin = (PMAINWIN) hWnd;
        while (pWin != pWin->pMainWin) {
            if ((pWin->dwExStyle & WS_EX_TRANSPARENT)) {
                winStartToUpdate = (PMAINWIN) pWin->hParent;
            }
            pWin = (PMAINWIN) pWin->hParent;
        }

        while ((hWnd = kernel_CheckInvalidRegion (winStartToUpdate))) {
            pWin = (PMAINWIN) hWnd;
            SendMessage (hWnd, MSG_PAINT, 0, (LPARAM)(&pWin->InvRgn.rgn));
        }
    }
}

/*
 ** this function show window in behavious by specified iCmdShow.
 ** if the window was previously visible, the return value is nonzero.
 ** if the window was previously hiddedn, the return value is zero.
 */
BOOL GUIAPI ShowWindow (HWND hWnd, int iCmdShow)
{
    // 
    //PrintInvRgn((PMAINWIN)hWnd, 0);
    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);

    if (IsMainWindow (hWnd)) {
        switch (iCmdShow)
        {
            case SW_SHOWNORMAL:
                SendMessage (HWND_DESKTOP, 
                        MSG_MOVETOTOPMOST, (WPARAM)hWnd, 0);
                break;

            case SW_SHOW:
                SendMessage (HWND_DESKTOP, 
                        MSG_SHOWMAINWIN, (WPARAM)hWnd, 0);
                break;

            case SW_HIDE:
                SendMessage (HWND_DESKTOP, 
                        MSG_HIDEMAINWIN, (WPARAM)hWnd, 0);
                break;
        }
    }
    else {
        PCONTROL pControl;

        pControl = (PCONTROL)hWnd;

        if (pControl->dwExStyle & WS_EX_CTRLASMAINWIN) {
            if (iCmdShow == SW_SHOW)
                SendMessage (HWND_DESKTOP, 
                        MSG_SHOWGLOBALCTRL, (WPARAM)hWnd, iCmdShow);
            else if (iCmdShow == SW_HIDE)
                SendMessage (HWND_DESKTOP, 
                        MSG_HIDEGLOBALCTRL, (WPARAM)hWnd, iCmdShow);
            else {

                return FALSE;
            }
        }
        else {
            switch (iCmdShow) {
                case SW_SHOWNORMAL:
                case SW_SHOW:
                    if (!(pControl->dwStyle & WS_VISIBLE)) {
                        pControl->dwStyle |= WS_VISIBLE;

                        SendAsyncMessage (hWnd, MSG_NCPAINT, 0, 0);
                        InvalidateRect (hWnd, NULL, TRUE);
                    }
                    break;

                case SW_HIDE:
                    if (pControl->dwStyle & WS_VISIBLE) {

                        pControl->dwStyle &= ~WS_VISIBLE;
                        InvalidateRect ((HWND)(pControl->pParent), 
                                (RECT*)(&pControl->left), TRUE);
                    }
                    break;
            }
        }

        if (iCmdShow == SW_HIDE && pControl->pParent->active == pControl) {
            SendNotifyMessage (hWnd, MSG_KILLFOCUS, 0, 0);
            pControl->pParent->active = NULL;
        }
    }

    SendNotifyMessage (hWnd, MSG_SHOWWINDOW, (WPARAM)iCmdShow, 0);

    return TRUE;
}

BOOL GUIAPI EnableWindow (HWND hWnd, BOOL fEnable)
{
    BOOL fOldStatus;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);

    if (IsMainWindow (hWnd)) {
        fOldStatus = SendMessage (HWND_DESKTOP, MSG_ENABLEMAINWIN,
                (WPARAM)hWnd, (LPARAM)fEnable);
    }
    else {
        PCONTROL pControl;

        pControl = (PCONTROL)hWnd;

        fOldStatus = !(pControl->dwStyle & WS_DISABLED);
    }

    SendNotifyMessage (hWnd, MSG_ENABLE, fEnable, 0);

    return fOldStatus;
}

BOOL GUIAPI IsWindowEnabled (HWND hWnd)
{
    PMAINWIN pWin = (PMAINWIN)hWnd;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);

    return !(pWin->dwStyle & WS_DISABLED);
}

void GUIAPI ScreenToClient (HWND hWnd, int* x, int* y)
{
    PCONTROL pParent;
    PCONTROL pCtrl;

    MG_CHECK (MG_IS_NORMAL_WINDOW(hWnd));
    pParent = pCtrl = MG_GET_CONTROL_PTR (hWnd);

    *x -= pCtrl->cl;
    *y -= pCtrl->ct;
    while ((pParent = pParent->pParent)) {
        *x -= pParent->cl;
        *y -= pParent->ct;
    }
}

void GUIAPI ClientToScreen(HWND hWnd, int* x, int* y)
{
    PCONTROL pParent;
    PCONTROL pCtrl;

    MG_CHECK (MG_IS_NORMAL_WINDOW(hWnd));
    pParent = pCtrl = MG_GET_CONTROL_PTR (hWnd);

    *x += pCtrl->cl;
    *y += pCtrl->ct;
    while ((pParent = pParent->pParent)) {
        *x += pParent->cl;
        *y += pParent->ct;
    }
}

void GUIAPI ScreenToWindow (HWND hWnd, int* x, int* y)
{
    PCONTROL pParent;
    PCONTROL pCtrl;

    MG_CHECK (MG_IS_NORMAL_WINDOW(hWnd));
    pParent = pCtrl = MG_GET_CONTROL_PTR (hWnd);

    *x -= pCtrl->left;
    *y -= pCtrl->top;
    while ((pParent = pParent->pParent)) {
        *x -= pParent->left;
        *y -= pParent->top;
    }
}

void GUIAPI WindowToClient (HWND hWnd, int* x, int* y)
{
    PCONTROL pCtrl;

    MG_CHECK (MG_IS_NORMAL_WINDOW(hWnd));
    pCtrl = MG_GET_CONTROL_PTR (hWnd);

    *x -= pCtrl->cl - pCtrl->left;
    *y -= pCtrl->ct - pCtrl->top;
}

void GUIAPI ClientToWindow (HWND hWnd, int* x, int* y)
{
    PCONTROL pCtrl;

    MG_CHECK (MG_IS_NORMAL_WINDOW(hWnd));
    pCtrl = MG_GET_CONTROL_PTR (hWnd);

    *x += pCtrl->cl - pCtrl->left;
    *y += pCtrl->ct - pCtrl->top;
}

void GUIAPI WindowToScreen (HWND hWnd, int* x, int* y)
{
    PCONTROL pParent;
    PCONTROL pCtrl;

    MG_CHECK (MG_IS_NORMAL_WINDOW(hWnd));
    pParent = pCtrl = MG_GET_CONTROL_PTR (hWnd);

    *x += pCtrl->left;
    *y += pCtrl->top;
    while ((pParent = pParent->pParent)) {
        *x += pParent->left;
        *y += pParent->top;
    }
}

BOOL GUIAPI GetClientRect (HWND hWnd, PRECT prc)
{
    PMAINWIN pWin = (PMAINWIN)hWnd;

    if (hWnd == HWND_DESKTOP) {
        *prc = g_rcScr;
        return TRUE;
    }
    else if (hWnd == HWND_INVALID || pWin->DataType != TYPE_HWND)
        return FALSE;

    prc->left = prc->top = 0;
    prc->right = pWin->cr - pWin->cl;
    prc->bottom = pWin->cb - pWin->ct;
    return TRUE;
}

/******************** main window and control styles support *****************/
gal_pixel GUIAPI GetWindowBkColor (HWND hWnd)
{
    PMAINWIN pWin = (PMAINWIN)hWnd;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), PIXEL_invalid);

    return pWin->iBkColor;
}

gal_pixel GUIAPI SetWindowBkColor (HWND hWnd, gal_pixel new_bkcolor)
{
    gal_pixel old_bkcolor;
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), PIXEL_invalid);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    old_bkcolor = pWin->iBkColor;
    pWin->iBkColor = new_bkcolor;
    return old_bkcolor;
}

PLOGFONT GUIAPI GetWindowFont (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), NULL);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->pLogFont;
}

PLOGFONT GUIAPI SetWindowFont (HWND hWnd, PLOGFONT pLogFont)
{
    PLOGFONT old_logfont;
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), NULL);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if (pLogFont == NULL)
        pLogFont = GetSystemFont (SYSLOGFONT_WCHAR_DEF);

    if (SendMessage (hWnd, MSG_FONTCHANGING, 0, (LPARAM)pLogFont))
        return NULL;

    old_logfont = pWin->pLogFont;
    pWin->pLogFont = pLogFont;
    SendNotifyMessage (hWnd, MSG_FONTCHANGED, 0, 0);

    return old_logfont;
}

HCURSOR GUIAPI GetWindowCursor (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), 0);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->hCursor;
}

HCURSOR GUIAPI SetWindowCursor (HWND hWnd, HCURSOR hNewCursor)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), 0);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if (pWin->WinType == TYPE_MAINWIN)
        return (HCURSOR)SendMessage (HWND_DESKTOP, 
                MSG_SETWINCURSOR, (WPARAM)hWnd, (LPARAM)hNewCursor);
    else if (pWin->WinType == TYPE_CONTROL) {
        HCURSOR old = pWin->hCursor;
        pWin->hCursor = hNewCursor;
        return old;
    }

    return 0;
}

DWORD GetWindowStyle (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), 0);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->dwStyle;
}

BOOL GUIAPI ExcludeWindowStyle (HWND hWnd, DWORD dwStyle)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    pWin->dwStyle &= ~dwStyle;
    return TRUE;
}

BOOL GUIAPI IncludeWindowStyle (HWND hWnd, DWORD dwStyle)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    pWin->dwStyle |= dwStyle;
    return TRUE;
}

DWORD GetWindowExStyle (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), 0);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->dwExStyle;
}

BOOL GUIAPI ExcludeWindowExStyle (HWND hWnd, DWORD dwStyle)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    pWin->dwExStyle &= ~dwStyle;
    return TRUE;
}

BOOL GUIAPI IncludeWindowExStyle (HWND hWnd, DWORD dwStyle)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    pWin->dwExStyle |= dwStyle;
    return TRUE;
}

DWORD GUIAPI GetWindowAdditionalData (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), 0);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->dwAddData;
}

DWORD GUIAPI SetWindowAdditionalData (HWND hWnd, DWORD newData)
{
    DWORD    oldOne;
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), 0L);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    oldOne = pWin->dwAddData;
    pWin->dwAddData = newData;
    return oldOne;
}

DWORD GUIAPI GetWindowAdditionalData2 (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), 0L);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->dwAddData2;
}

DWORD GUIAPI SetWindowAdditionalData2 (HWND hWnd, DWORD newData)
{
    DWORD    oldOne;
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), 0L);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    oldOne = pWin->dwAddData2;
    pWin->dwAddData2 = newData;
    return oldOne;
}

DWORD GUIAPI GetWindowClassAdditionalData (HWND hWnd)
{
    PMAINWIN pWin;
    PCONTROL pCtrl;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), 0);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if (pWin->WinType == TYPE_CONTROL) {
        pCtrl = MG_GET_CONTROL_PTR(hWnd);
        return pCtrl->pcci->dwAddData;
    }

    return 0;
}

DWORD GUIAPI SetWindowClassAdditionalData (HWND hWnd, DWORD newData)
{
    PMAINWIN pWin;
    PCONTROL pCtrl;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), 0);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if (pWin->WinType == TYPE_CONTROL) {
        DWORD oldOne;

        pCtrl = MG_GET_CONTROL_PTR(hWnd);
        oldOne = pCtrl->pcci->dwAddData;
        pCtrl->pcci->dwAddData = newData;
        return oldOne;
    }

    return 0L;
}

const char* GUIAPI GetClassName (HWND hWnd)
{
    PMAINWIN pWin;
    PCONTROL pCtrl;

    pWin = (PMAINWIN)hWnd;

    if (!MG_IS_WINDOW(hWnd))
        return NULL;
    else if (hWnd == HWND_DESKTOP)
        return ROOTWINCLASSNAME;
    else if (pWin->WinType == TYPE_MAINWIN)
        return MAINWINCLASSNAME;
    else if (pWin->WinType == TYPE_CONTROL) {
        pCtrl = (PCONTROL)hWnd;
        return pCtrl->pcci->name;
    }

    return NULL;
}

BOOL GUIAPI IsWindowVisible (HWND hWnd)
{
    PMAINWIN pMainWin;
    PCONTROL pCtrl;

    if ((pMainWin = gui_CheckAndGetMainWindowPtr (hWnd))) {
        return pMainWin->dwStyle & WS_VISIBLE;
    }
    else if (IsControl (hWnd)) {
        pCtrl = (PCONTROL)hWnd;
        while (pCtrl) {
            if (!(pCtrl->dwStyle & WS_VISIBLE))
                return FALSE;

            pCtrl = pCtrl->pParent;
        }
    }

    return TRUE;
}

BOOL GUIAPI GetWindowRect (HWND hWnd, PRECT prc)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    prc->left = pWin->left;
    prc->top = pWin->top;
    prc->right = pWin->right;
    prc->bottom = pWin->bottom;
    return TRUE;
}

WNDPROC GUIAPI GetWindowCallbackProc (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), NULL);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->MainWindowProc;
}

WNDPROC GUIAPI SetWindowCallbackProc (HWND hWnd, WNDPROC newProc)
{
    PMAINWIN pWin;
    WNDPROC old_proc;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), NULL);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    old_proc = pWin->MainWindowProc;
    if (newProc)
        pWin->MainWindowProc = newProc;
    return old_proc;
}

const char* GUIAPI GetWindowCaption (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), NULL);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->spCaption;
}

BOOL GUIAPI SetWindowCaption (HWND hWnd, const char* spCaption)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if (pWin->WinType == TYPE_MAINWIN) {
        return SetWindowText (hWnd, spCaption);
    }
    else if (pWin->WinType == TYPE_CONTROL) {
        PCONTROL pCtrl;
        pCtrl = (PCONTROL)hWnd;
        if (pCtrl->spCaption) {
            FreeFixStr (pCtrl->spCaption);
            pCtrl->spCaption = NULL;
        }

        if (spCaption) {
            pCtrl->spCaption = FixStrAlloc (strlen (spCaption));
            if (spCaption [0])
                strcpy (pCtrl->spCaption, spCaption);
        }

        return TRUE;
    }

    return FALSE;
}

int GUIAPI GetWindowTextLength (HWND hWnd)
{
    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), -1);

    return SendMessage (hWnd, MSG_GETTEXTLENGTH, 0, 0);
}

int GUIAPI GetWindowText (HWND hWnd, char* spString, int nMaxLen)
{
    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), -1);

    return SendMessage (hWnd, MSG_GETTEXT, (WPARAM)nMaxLen, (LPARAM)spString);
}

BOOL GUIAPI SetWindowText (HWND hWnd, const char* spString)
{
    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);

    return (SendMessage (hWnd, MSG_SETTEXT, 0, (LPARAM)spString) == 0);
}

extern BLOCKHEAP __mg_FreeClipRectList;
/* NOTE: This function is control safe */
BOOL GUIAPI MoveWindow (HWND hWnd, int x, int y, int w, int h, BOOL fPaint)
{
    RECT rcWindow;
    RECT rcExpect, rcResult;
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    SetRect (&rcExpect, x, y, x + w, y + h);

    GetWindowRect (hWnd, &rcWindow);
    SendMessage (hWnd, MSG_SIZECHANGING, 
            (WPARAM)(&rcExpect), (LPARAM)(&rcResult));

    if (EqualRect (&rcWindow, &rcResult))
    {
        return FALSE;
    }

    if (IsMainWindow (hWnd) || (pWin->dwExStyle & WS_EX_CTRLASMAINWIN)) {
        SendMessage (HWND_DESKTOP, MSG_MOVEMAINWIN, 
                (WPARAM)hWnd, (LPARAM)(&rcResult));

        if (RECTH (rcWindow) != RECTH (rcResult)
                || RECTW (rcWindow) != RECTW (rcResult))
            fPaint = TRUE;
    }
    else {
        PCONTROL pParent;
        PCONTROL pCtrl;

        pCtrl = (PCONTROL)hWnd;
        pParent = pCtrl->pParent;

        rcExpect = rcResult;
        SendMessage (hWnd, MSG_CHANGESIZE, 
                (WPARAM)(&rcExpect), (LPARAM)(&rcResult));

        if (IsWindowVisible (hWnd)
                && (pParent->InvRgn.frozen == 0)) {

#if 1
            InvalidateRect((HWND)pParent, &rcWindow, TRUE);
            InvalidateRect((HWND)pParent, &rcResult, TRUE);
#elif 0
            if (pCtrl->dwExStyle & WS_EX_TRANSPARENT)
                InvalidateRect (hWnd, NULL, TRUE);
            else {
                HDC hdc;
                hdc = get_valid_dc ((PMAINWIN)pParent, TRUE);
                BitBlt (hdc, pCtrl->left, pCtrl->top, 
                        pCtrl->right - pCtrl->left,
                        pCtrl->bottom - pCtrl->top,
                        hdc, rcResult.left, rcResult.top, 0);
                release_valid_dc ((PMAINWIN)pParent, hdc);
            }
            /* set to invisible temporarily. */
            /* FIXME: need more optimization. */
            pCtrl->dwStyle &= ~WS_VISIBLE;
            InvalidateRect ((HWND)pParent, &rcWindow, TRUE);
            rcExpect.left = rcResult.left;
            rcExpect.top = rcResult.top;
            rcExpect.right = rcResult.left + RECTW (rcWindow);
            rcExpect.bottom = rcResult.top + RECTH (rcWindow);
            InvalidateRect ((HWND)pParent, &rcExpect, TRUE);
            pCtrl->dwStyle |= WS_VISIBLE;
#else
            if (pCtrl->dwExStyle & WS_EX_TRANSPARENT) {
                InvalidateRect ((HWND)pParent, &rcWindow, TRUE);
                InvalidateRect (hWnd, NULL, TRUE);
            }
            else {
                if (DoesIntersect ((const RECT*)&rcWindow, (const RECT*)&pCtrl->cl)) {
                    CLIPRGN rgn;
                    PCLIPRECT  p = NULL;
                    InitClipRgn (&rgn, &__mg_FreeClipRectList);
                    AddClipRect (&rgn, &rcWindow);
                    SubtractClipRect (&rgn, (const RECT*)&pCtrl->cl);
                    p = rgn.head;
                    while(p) {
                        InvalidateRect((HWND)pParent, &p->rc, TRUE);
                        p = p->next;
                    }
                    EmptyClipRgn (&rgn);
                }
                else {
                    InvalidateRect ((HWND)pParent, &rcWindow, TRUE);
                }
                if (RECTH (rcWindow) != RECTH (rcResult)
                        || RECTW (rcWindow) != RECTW (rcResult))
                {
                    fPaint = TRUE;
                }
                else {
                    PCONTROL pChild ;
                    RECT     rc;
                    BOOL     flag = TRUE;

                    for (pChild = pParent->children; pChild; pChild = pChild->next) {
                        if (pChild == pCtrl || !(pChild->dwStyle & WS_VISIBLE)
                                || (pChild->dwExStyle & WS_EX_CTRLASMAINWIN))
                            continue;

                        if(IntersectRect(&rc, &rcWindow, (const RECT*)&pChild->cl)) {
                            flag = FALSE;
                            break;
                        }
                    }
                    if (!flag) {
                        InvalidateRect ((HWND)pParent, &rcResult, FALSE);
                    }
                    else {
                        HDC hdc;
                        hdc = get_valid_dc ((PMAINWIN)pParent, TRUE);
                        BitBlt (hdc, rcWindow.left, rcWindow.top, 
                                RECTW(rcWindow), RECTH(rcWindow),
                                hdc, rcResult.left, rcResult.top, 0);
                        release_valid_dc ((PMAINWIN)pParent, hdc);

                        for(pChild = pCtrl->next; pChild; pChild = pChild->next) {
                            if(!(pChild->dwStyle & WS_VISIBLE)
                                    || (pChild->dwExStyle & WS_EX_CTRLASMAINWIN))
                                continue;

                            if(IntersectRect(&rc, &rcResult, (const RECT*)&pChild->cl)) {
                                OffsetRect(&rc, - pChild->left, - pChild->top);
                                InvalidateRect((HWND)pChild, &rc, TRUE);
                            }
                        }
                    }
                }
            }
#endif
        }
    }

    if ((RECTW (rcWindow) != w ) || (RECTH (rcWindow) != h))
        SendAsyncMessage (hWnd, MSG_NCPAINT, 0, 0);
    if (fPaint) {
        InvalidateRect (hWnd, NULL, TRUE);
    }

    return TRUE;
}

/*************************** Paint support ***********************************/

#define WIRM_PARENT  0x01
#define WIRM_PREV_SIBLING 0x02
#define WIRM_NEXT_SIBLING 0x04
#define WIRM_CHILDREN 0x08
#define WIRM_SIBLING  (WIRM_PREV_SIBLING|WIRM_NEXT_SIBLING)
#define WIRM_ALL      (WIRM_PARENT | WIRM_CHILDREN | WIRM_SIBLING)
static BOOL _wndInvalidateRect(HWND hWnd, const RECT* prc, BOOL bEraseBkgnd, int mark)
{
    PCONTROL pCtrl;
    RECT     rcClient;
    RECT     rcInv;
    RECT     rcTemp;
    PINVRGN  pInvRgn;    


    pCtrl = (PCONTROL)hWnd;


    //invalidate itself
    rcClient.left = rcClient.top = 0;
    rcClient.right = pCtrl->cr - pCtrl->cl;
    rcClient.bottom = pCtrl->cb - pCtrl->ct;

    pInvRgn = &pCtrl->InvRgn;
    
    if(!pInvRgn->frozen)
    {
        PCONTROL pNext;
#ifdef _MGRM_THREADS
        pthread_mutex_lock(&pInvRgn->lock);
#endif
        if (bEraseBkgnd) {
            pCtrl->Flags |= WF_ERASEBKGND;
        }

        if(prc) {
            rcInv = *prc;
            NormalizeRect(&rcInv);
            if(IntersectRect(&rcInv, &rcInv, &rcClient))
                AddClipRect(&pInvRgn->rgn, &rcInv);
        }
        else {
            rcInv = rcClient;
            SetClipRgn(&pInvRgn->rgn, &rcInv);
        }

        rcTemp = rcInv;
        OffsetRect(&rcTemp, pCtrl->cl, pCtrl->ct);

        //subtract from next sibling controls
        if(pCtrl->WinType == TYPE_CONTROL /*&& ( mark & WIRM_NEXT_SIBLING)*/)
        {
            for(pNext = pCtrl->next; pNext; pNext = pNext->next)
            {
                RECT rc;
                if((pNext->dwStyle & WS_VISIBLE)
                    && !(pNext->dwExStyle & WS_EX_TRANSPARENT)
                    && IntersectRect(&rc, &rcTemp, (const RECT*)&pNext->left))
                {
                    OffsetRect(&rc, - pCtrl->cl, - pCtrl->ct );
                    SubtractClipRect(&pInvRgn->rgn, &rc);
                }
            }
        }
#ifdef _MGRM_THREADS
        pthread_mutex_unlock(&pInvRgn->lock);
#endif
    }
    else
    {
        return FALSE;
    }

    //invalidate parent 
    if( (mark & WIRM_PARENT) 
            && (pCtrl->dwExStyle & WS_EX_TRANSPARENT)
            && pCtrl->WinType != TYPE_MAINWIN
            && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN))
    {
        /* houhh 20100426, invalide rect for parent must be valid. */
        if (rcTemp.top < rcTemp.bottom && rcTemp.left < rcTemp.right) {
            _wndInvalidateRect((HWND)pCtrl->pParent, &rcTemp, bEraseBkgnd, WIRM_PARENT|WIRM_SIBLING);
        }
    }

    //invalidate sibling
    if(pCtrl->WinType==TYPE_CONTROL 
        && (pCtrl->dwExStyle & WS_EX_TRANSPARENT)
        && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)
        && (mark & WIRM_PREV_SIBLING))
    {
        PCONTROL   pPrev;
        for(pPrev = pCtrl->prev; pPrev; pPrev = pPrev->prev)
        {
            RECT rc;

            if(!(pPrev->dwStyle & WS_VISIBLE))
                continue;

            if(IntersectRect(&rc, &rcTemp, (const RECT*)&pPrev->cl))
            {
                OffsetRect(&rc,  - pPrev->cl, - pPrev->ct);
                _wndInvalidateRect((HWND)pPrev, &rc, bEraseBkgnd, WIRM_CHILDREN);
            }
        }
    }

    if(pCtrl->WinType==TYPE_CONTROL 
        && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)
        && (mark & WIRM_NEXT_SIBLING))
    {
        PCONTROL pNext;
        for(pNext = pCtrl->next; pNext; pNext = pNext->next)
        {
            RECT rc;
            if(!(pNext->dwStyle & WS_VISIBLE)
                || !(pNext->dwExStyle & WS_EX_TRANSPARENT))
                continue;

            if(IntersectRect(&rc, &rcTemp, (const RECT*)&pNext->cl))
            {
                OffsetRect(&rc, - pNext->cl, - pNext->ct);
                _wndInvalidateRect((HWND)pNext, &rc, bEraseBkgnd, WIRM_CHILDREN);
            }
        }
    }

    
    //check mark for invalidate children
    if(mark & WIRM_CHILDREN)
    {
        PCONTROL pChild ;
        RECT     rc;

        
        for(pChild = pCtrl->children; pChild; pChild = pChild->next)
        {
            if(!(pChild->dwStyle & WS_VISIBLE)
                || (pChild->dwExStyle & WS_EX_CTRLASMAINWIN))
                continue;
            
            if(IntersectRect(&rc, &rcInv, (const RECT*)&pChild->cl))
            {
                OffsetRect(&rc, - pChild->cl, - pChild->ct);
                _wndInvalidateRect((HWND)pChild, &rc, bEraseBkgnd, WIRM_CHILDREN);
            }
            if(IntersectRect(&rc, &rcInv, (const RECT*)&pChild->left))
            {
                OffsetRect(&rc, - pChild->left, - pChild->top);
                SendAsyncMessage((HWND)pChild, MSG_NCPAINT, 0, (LPARAM)&rc);
            }
        }
    }

    return TRUE;

}

BOOL GUIAPI InvalidateRect (HWND hWnd, const RECT* prc, BOOL bEraseBkgnd)
{
    BOOL retval;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);

    retval = _wndInvalidateRect(hWnd, prc, bEraseBkgnd, WIRM_ALL);
    PostMessage (hWnd, MSG_PAINT, 0, (LPARAM)prc);

    return retval;
}

/* TODO: Optimize */
BOOL GUIAPI InvalidateRegion (HWND hWnd, const CLIPRGN* pRgn, BOOL bErase)
{
    const CLIPRECT* crc = pRgn->head;

    while (crc) {
        if (!InvalidateRect (hWnd, &crc->rc,  bErase))
            return FALSE;
        crc = crc->next;
    }

    return TRUE;
}

BOOL GUIAPI ValidateRect (HWND hWnd, const RECT* rect)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if (rect) {
        return SubtractClipRect (&pWin->InvRgn.rgn, rect);
    }
    else {
        EmptyClipRgn (&pWin->InvRgn.rgn);
    }

    return TRUE;
}

BOOL GUIAPI ValidateRegion (HWND hWnd, const CLIPRGN* pRgn)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if (pRgn) {
        return SubtractRegion (&pWin->InvRgn.rgn, &pWin->InvRgn.rgn, pRgn);
    }
    else {
        EmptyClipRgn (&pWin->InvRgn.rgn);
    }

    return TRUE;
}

BOOL GUIAPI GetUpdateRect (HWND hWnd, RECT* update_rect)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    *update_rect = pWin->InvRgn.rgn.rcBound;
    return TRUE;
}

int GUIAPI GetUpdateRegion (HWND hWnd, PCLIPRGN pRgn)
{
    PMAINWIN pWin;

    MG_CHECK_RET (pRgn != NULL, -1);
    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), -1);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    CopyRegion (pRgn, &pWin->InvRgn.rgn);
    return pRgn->type;
}

HDC GUIAPI BeginPaint (HWND hWnd)
{
    PMAINWIN pWin;
    PINVRGN pInvRgn;
    HDC hdc;
    RECT rcInv;
    PCONTROL child;
    BOOL fEraseBk;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), HDC_INVALID);
    pWin = MG_GET_WINDOW_PTR (hWnd);


    if (pWin->pCaretInfo && pWin->pCaretInfo->fBlink) {
        HideCaretEx (hWnd, FALSE);
        pWin->pCaretInfo->fBlink = TRUE;
    }

    hdc = get_valid_dc (pWin, TRUE);

#if 0
    /* FIXME: why do this? */
    if (((PMAINWIN)hWnd)->dwExStyle & WS_EX_TRANSPARENT)
        SetBkMode (hdc, BM_TRANSPARENT);
#endif

    pInvRgn = &pWin->InvRgn;

#ifdef _MGRM_THREADS
    pthread_mutex_lock (&pInvRgn->lock);
#endif
    pInvRgn->frozen ++;

    fEraseBk = pWin->Flags & WF_ERASEBKGND;
    pWin->Flags = (pWin->Flags & ~WF_ERASEBKGND);

    SelectClipRegion (hdc, &pInvRgn->rgn);
    if (pWin->pMainWin->secondaryDC) {
        CopyRect (&pWin->pMainWin->update_rc, &(dc_HDC2PDC(hdc))->ecrgn.rcBound);
        coor_SP2DP((PDC)hdc, &pWin->pMainWin->update_rc.left, 
                &pWin->pMainWin->update_rc.top);
        coor_SP2DP((PDC)hdc, &pWin->pMainWin->update_rc.right,
                &pWin->pMainWin->update_rc.bottom);
    }
    else {
        CopyRect (&pWin->pMainWin->update_rc, &pInvRgn->rgn.rcBound);
    }
    rcInv = pInvRgn->rgn.rcBound;

    EmptyClipRgn (&pInvRgn->rgn);

    pInvRgn->frozen --;
#ifdef _MGRM_THREADS
    pthread_mutex_unlock (&pInvRgn->lock);
#endif

    if(!(pWin->dwStyle & WS_VISIBLE))
        return hdc;

    /* exclude the children area from the client */
    /*    if (pWin->dwExStyle & WS_EX_CLIPCHILDREN)*/ {
        child = (PCONTROL)pWin->hFirstChild;
        while (child) {
            if ((child->dwStyle & WS_VISIBLE) && !(child->dwExStyle & WS_EX_TRANSPARENT)
                    && DoesIntersect ((const RECT*)&child->left, &rcInv)) {
                ExcludeClipRect (hdc, (const RECT*)&child->left);
            }
            child = child->next;
        }
    }
    //erase bkgnd
    if(fEraseBk 
            && !(pWin->WinType == TYPE_CONTROL
                    && !(pWin->dwExStyle & WS_EX_CTRLASMAINWIN)
                    && (pWin->dwExStyle & WS_EX_TRANSPARENT)
                )
        )
        SendAsyncMessage(hWnd, MSG_ERASEBKGND, (WPARAM)hdc,0); //(LPARAM)&rcInv);

    //repaint NC area of tranparent children 
    for(child = (PCONTROL)pWin->hFirstChild; child; child = child->next)
    {
        RECT rcTemp;
        if(!(pWin->dwExStyle & WS_EX_TRANSPARENT)
            && (!(child->dwStyle & WS_VISIBLE)
                || !(child->dwExStyle & WS_EX_TRANSPARENT)
                || ( child->dwExStyle & WS_EX_CTRLASMAINWIN)))
            continue;

        if(IntersectRect(&rcTemp, &rcInv, (const RECT*)&child->left))
        {
            //draw NC
            OffsetRect(&rcTemp, -child->left, -child->top);
            SendAsyncMessage((HWND)child, MSG_NCPAINT, (WPARAM)0, (LPARAM)&rcTemp);
        }
        
    }

    return hdc;
}

void GUIAPI EndPaint (HWND hWnd, HDC hdc)
{
    PMAINWIN pWin;

    MG_CHECK (MG_IS_NORMAL_WINDOW(hWnd));
    pWin = MG_GET_WINDOW_PTR (hWnd);

    if (pWin->pMainWin->secondaryDC) {
        if (!IsRectEmpty(&pWin->pMainWin->update_rc)) {
            HDC real_dc = HDC_INVALID;
            real_dc = GetClientDC ((HWND)pWin->pMainWin);

            update_secondary_dc (pWin, hdc, real_dc, 
                    &pWin->pMainWin->update_rc, HT_CLIENT);
            ReleaseDC (real_dc);
        }
    }

    release_valid_dc (pWin, hdc);

    /* privCDC is not need Release, but need clear lcrgn.*/
    if (pWin->dwExStyle & WS_EX_USEPRIVATECDC)
        SelectClipRect (pWin->privCDC, NULL);

    /* show caret */
#if 0
    if (pWin->pCaretInfo) {
        GetCaretBitmaps (pWin->pCaretInfo);
        if (pWin->pCaretInfo->fBlink) {
            HDC caret_dc = GetClientDC (hWnd);
            pWin->pCaretInfo->fShow = TRUE;
            pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pXored;
            FillBoxWithBitmap (caret_dc,
                    pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                    &pWin->pCaretInfo->caret_bmp);
            ReleaseDC (caret_dc);
        }
    }
#else
    if (pWin->pCaretInfo && pWin->pCaretInfo->fBlink) {
        pWin->pCaretInfo->fBlink = FALSE;
        ShowCaretEx (hWnd, FALSE);
    }
#endif
}

BOOL RegisterWindowClass (PWNDCLASS pWndClass)
{
    if (pWndClass == NULL)
        return FALSE;

    return SendMessage (HWND_DESKTOP, 
            MSG_REGISTERWNDCLASS, 0, (LPARAM)pWndClass) == ERR_OK;
}

BOOL UnregisterWindowClass (const char* szClassName)
{
    if (szClassName == NULL)
        return FALSE;

    return SendMessage (HWND_DESKTOP, 
            MSG_UNREGISTERWNDCLASS, 0, (LPARAM)szClassName) == ERR_OK;
}

BOOL GUIAPI GetWindowClassInfo (PWNDCLASS pWndClass)
{
    if (pWndClass == NULL || pWndClass->spClassName == NULL)
        return FALSE;

    return SendMessage (HWND_DESKTOP, 
            MSG_CTRLCLASSDATAOP, CCDOP_GETCCI, (LPARAM)pWndClass) == ERR_OK;
}

BOOL GUIAPI SetWindowClassInfo (const WNDCLASS* pWndClass)
{
    if (pWndClass == NULL || pWndClass->spClassName == NULL)
        return FALSE;

    return SendMessage (HWND_DESKTOP, 
            MSG_CTRLCLASSDATAOP, CCDOP_SETCCI, (LPARAM)pWndClass) == ERR_OK;
}

static void set_control_renderer (PCONTROL pCtrl, const char* werdr_name)
{
    const WINDOW_ELEMENT_RENDERER* renderer = 
        GetWindowRendererFromName (werdr_name);

    if (renderer)
        pCtrl->we_rdr = (WINDOW_ELEMENT_RENDERER*)renderer;
    else
        pCtrl->we_rdr = __mg_def_renderer;

    if (pCtrl->we_rdr)
        ++pCtrl->we_rdr->refcount;
}

HWND GUIAPI CreateWindowEx2 (const char* spClassName, 
        const char* spCaption, DWORD dwStyle, DWORD dwExStyle, 
        LINT id, int x, int y, int w, int h, HWND hParentWnd, 
        const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs, 
        DWORD dwAddData)
{
    PMAINWIN pMainWin;
    PCTRLCLASSINFO cci;
    PCONTROL pNewCtrl;
    RECT rcExpect;

    if (!(pMainWin = gui_GetMainWindowPtrOfControl (hParentWnd)))
        return HWND_INVALID;

    cci = (PCTRLCLASSINFO)SendMessage (HWND_DESKTOP, 
            MSG_GETCTRLCLASSINFO, 0, (LPARAM)spClassName);

    if (!cci) return HWND_INVALID;

    pNewCtrl = calloc (1, sizeof (CONTROL));

    if (!pNewCtrl) return HWND_INVALID;

    pNewCtrl->DataType = TYPE_HWND;
    pNewCtrl->WinType  = TYPE_CONTROL;

    pNewCtrl->left     = x;
    pNewCtrl->top      = y;
    pNewCtrl->right    = x + w;
    pNewCtrl->bottom   = y + h;

    memcpy (&pNewCtrl->cl, &pNewCtrl->left, sizeof (RECT));
    memcpy (&rcExpect, &pNewCtrl->left, sizeof (RECT));

    if (spCaption) {
        int len = strlen (spCaption);

        pNewCtrl->spCaption = FixStrAlloc (len);
        if (len > 0)
            strcpy (pNewCtrl->spCaption, spCaption);
    }
    else
        pNewCtrl->spCaption = "";

    pNewCtrl->dwStyle   = dwStyle | WS_CHILD | cci->dwStyle;
    pNewCtrl->dwExStyle = dwExStyle | cci->dwExStyle;

    pNewCtrl->iBkColor  = cci->iBkColor;
    pNewCtrl->hCursor   = cci->hCursor;
    pNewCtrl->hMenu     = 0;
    pNewCtrl->hAccel    = 0;
    pNewCtrl->hIcon     = 0;
    pNewCtrl->hSysMenu  = 0;
    pNewCtrl->id        = id;
    pNewCtrl->pCaretInfo = NULL;

    pNewCtrl->dwAddData = dwAddData;
    pNewCtrl->dwAddData2 = 0;

    pNewCtrl->ControlProc = cci->ControlProc;

    /* Scroll bar */
    if (pNewCtrl->dwStyle & WS_VSCROLL) {
        pNewCtrl->vscroll.minPos = 0;
        pNewCtrl->vscroll.maxPos = 100;
        pNewCtrl->vscroll.curPos = 0;
        pNewCtrl->vscroll.pageStep = 101;
        pNewCtrl->vscroll.barStart = 0;
        pNewCtrl->vscroll.barLen = 10;
        pNewCtrl->vscroll.status = SBS_NORMAL;
    }
    else
        pNewCtrl->vscroll.status = SBS_HIDE | SBS_DISABLED;

    if (pNewCtrl->dwStyle & WS_HSCROLL) {
        pNewCtrl->hscroll.minPos = 0;
        pNewCtrl->hscroll.maxPos = 100;
        pNewCtrl->hscroll.curPos = 0;
        pNewCtrl->hscroll.pageStep = 101;
        pNewCtrl->hscroll.barStart = 0;
        pNewCtrl->hscroll.barLen = 10;
        pNewCtrl->hscroll.status = SBS_NORMAL;
    }
    else
        pNewCtrl->hscroll.status = SBS_HIDE | SBS_DISABLED;

    /** prefer to use parent renderer */
    if (pNewCtrl->dwExStyle & WS_EX_USEPARENTRDR)
    {
        if (((PCONTROL)hParentWnd)->we_rdr)
        {
            /** only get render from parent */
            pNewCtrl->we_rdr = ((PCONTROL)hParentWnd)->we_rdr;
            ++pNewCtrl->we_rdr->refcount;
        }
        else
            return HWND_INVALID;
    }
    else
    {
        /** set window renderer */
        set_control_renderer (pNewCtrl, werdr_name);
    }

    /** set window element data */
    while (we_attrs && we_attrs->we_attr_id != -1)
    {
        //append_window_element_data (pNewCtrl, 
           //     we_attrs->we_attr_id, we_attrs->we_attr);
        DWORD _old;
        set_window_element_data ((HWND)pNewCtrl, 
                we_attrs->we_attr_id, we_attrs->we_attr, &_old);
        ++we_attrs;
    }

    /** prefer to parent font */
    if (pNewCtrl->dwExStyle & WS_EX_USEPARENTFONT)
        pNewCtrl->pLogFont = ((PCONTROL)hParentWnd)->pLogFont;
    else
        pNewCtrl->pLogFont = GetSystemFont (SYSLOGFONT_CONTROL);

    /* reserve by humingming 2010.7.6 */
    /*if (spCaption) {
        int len = __mg_strlen (pNewCtrl->pLogFont, spCaption)+2;

        pNewCtrl->spCaption = FixStrAlloc (len);
        if (len > 0)
            //strcpy (pNewCtrl->spCaption, spCaption);
            memcpy (pNewCtrl->spCaption, spCaption, len);
    }
    else
        pNewCtrl->spCaption = "";*/

    pNewCtrl->children = NULL;
    pNewCtrl->active   = NULL;
    pNewCtrl->old_under_pointer = NULL;
    pNewCtrl->primitive = NULL;

    pNewCtrl->notif_proc = NULL;

    pNewCtrl->pMainWin = (PMAINWIN)pMainWin;
    pNewCtrl->pParent  = (PCONTROL)hParentWnd;
    pNewCtrl->next     = NULL;

    pNewCtrl->pcci     = cci;

    if (dwExStyle & WS_EX_CTRLASMAINWIN) {
        if ( !(pNewCtrl->pGCRInfo = malloc (sizeof (GCRINFO))) ) {
            goto error;
        }
    }
    else {
        pNewCtrl->pGCRInfo = pMainWin->pGCRInfo;
        pNewCtrl->idx_znode = pMainWin->idx_znode;
    }

    if (SendMessage (HWND_DESKTOP, 
                MSG_NEWCTRLINSTANCE, 
                (WPARAM)hParentWnd, (LPARAM)pNewCtrl) < 0)
        goto error;


    if (cci->dwStyle & CS_OWNDC)
        pNewCtrl->dwExStyle |= WS_EX_USEPRIVATECDC;

    if (pNewCtrl->dwExStyle & WS_EX_USEPRIVATECDC) {
        if (pMainWin->dwExStyle & WS_EX_AUTOSECONDARYDC)
            pNewCtrl->privCDC = GetSecondarySubDC (pMainWin->secondaryDC,
                    (HWND)pNewCtrl, TRUE);
        else
            pNewCtrl->privCDC = CreatePrivateClientDC ((HWND)pNewCtrl);
    }
    else
        pNewCtrl->privCDC = 0;


    if (SendMessage ((HWND)pNewCtrl, MSG_NCCREATE, 0, (LPARAM)dwAddData)) {
        goto error;
    }

    if (SendMessage ((HWND)pNewCtrl, MSG_CREATE, 
                (WPARAM)hParentWnd, (LPARAM)dwAddData)) {
        SendMessage (HWND_DESKTOP, 
                MSG_REMOVECTRLINSTANCE, (WPARAM)hParentWnd, (LPARAM)pNewCtrl);
        goto error;
    }

    //set ctrl as main
    if(pNewCtrl->dwExStyle & WS_EX_CTRLASMAINWIN){
        pNewCtrl->next_ctrl_as_main = (PCONTROL)pNewCtrl->pMainWin->hFirstChildAsMainWin;
        pMainWin->hFirstChildAsMainWin = (HWND) pNewCtrl;
    }
    
    SendMessage ((HWND)pNewCtrl, MSG_SIZECHANGING, 
            (WPARAM)&rcExpect, (LPARAM)&pNewCtrl->left);

    SendMessage ((HWND)pNewCtrl, MSG_CHANGESIZE, 
            (WPARAM)(&pNewCtrl->left), 0);

    /* houhh 20080820, not need to call UpdateWindow() here. */
    if (pNewCtrl->pParent->dwStyle & WS_VISIBLE 
            && pNewCtrl->dwStyle & WS_VISIBLE) {
#if 1
        InvalidateRect ((HWND)pNewCtrl, NULL, TRUE);
        SendAsyncMessage ((HWND)pNewCtrl, MSG_NCPAINT, 0, 0);
#else
        UpdateWindow ((HWND)pNewCtrl, TRUE);
#endif
    }

    return (HWND)pNewCtrl;

error:
    if (dwExStyle & WS_EX_CTRLASMAINWIN) {
        if (pNewCtrl->pGCRInfo) free (pNewCtrl->pGCRInfo);
    }
    free (pNewCtrl);

    return HWND_INVALID;
}

BOOL GUIAPI DestroyWindow (HWND hWnd)
{
    // 
    PCONTROL pCtrl;
    PCONTROL pParent;

    if (!IsControl (hWnd)) return FALSE;

    if (SendMessage (hWnd, MSG_DESTROY, 0, 0))
    {

        return FALSE;
    }

    /* destroy all controls of this window */
    DestroyAllControls (hWnd);

    /* kill all timers of this window */
    KillTimer (hWnd, 0);

    pCtrl = (PCONTROL)hWnd;
    pParent = pCtrl->pParent;
    if (pParent->active == (PCONTROL) hWnd)
        pParent->active = NULL;
    if (pParent->old_under_pointer == (PCONTROL) hWnd)
        pParent->old_under_pointer = NULL;
    if (pParent->primitive == (PCONTROL) hWnd)
        pParent->primitive = NULL;

    if (SendMessage (HWND_DESKTOP, 
                MSG_REMOVECTRLINSTANCE, (WPARAM)pParent, (LPARAM)pCtrl))
    {

        return FALSE;
    }

    __mg_reset_mainwin_capture_info (pCtrl);

    pCtrl->dwStyle &= ~WS_VISIBLE;
    if (IsWindowVisible ((HWND) pParent))
        InvalidateRect ((HWND) pParent, (PRECT)(&pCtrl->left), TRUE);

    if (pCtrl->privCDC) {
        if (pCtrl->pMainWin->dwExStyle & WS_EX_AUTOSECONDARYDC) {
            ReleaseSecondarySubDC (pCtrl->privCDC);
        }
        else {
            if (pCtrl->pMainWin->secondaryDC)
                ReleaseSecondarySubDC (pCtrl->privCDC);
            else
                DeletePrivateDC (pCtrl->privCDC);
        }
        pCtrl->privCDC = 0;
    }

    if (sg_repeat_msg.hwnd == hWnd)
        sg_repeat_msg.hwnd = 0;

    ThrowAwayMessages (hWnd);

    if (pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN) {
        PMAINWIN pMainWin;
        EmptyClipRgn (&pCtrl->pGCRInfo->crgn);
#ifdef _MGRM_THREADS
        pthread_mutex_destroy (&pCtrl->pGCRInfo->lock);
#endif
        free (pCtrl->pGCRInfo);

        pMainWin = pCtrl->pMainWin;
        if ((PCONTROL) pMainWin->hFirstChildAsMainWin == pCtrl)
            pMainWin->hFirstChildAsMainWin = (HWND)pCtrl->next_ctrl_as_main;
        else {
            PCONTROL tmp = (PCONTROL) pMainWin->hFirstChildAsMainWin;
            BOOL found = FALSE;

            while(tmp) {
                if (tmp->next_ctrl_as_main == pCtrl) {
                    found = TRUE;
                    break;
                }

                tmp = tmp->next_ctrl_as_main; 
            }
            if (found)
                tmp->next_ctrl_as_main = pCtrl->next_ctrl_as_main;
        }
    }
    EmptyClipRgn (&pCtrl->InvRgn.rgn);
#ifdef _MGRM_THREADS
    pthread_mutex_destroy (&pCtrl->InvRgn.lock);
#endif

    if (pCtrl->spCaption)
        FreeFixStr (pCtrl->spCaption);

    free_window_element_data (hWnd);
    --pCtrl->we_rdr->refcount;

    free (pCtrl);

    return TRUE;
}

NOTIFPROC GUIAPI SetNotificationCallback (HWND hwnd, NOTIFPROC notif_proc)
{
    NOTIFPROC old_proc;
    PCONTROL control = (PCONTROL)hwnd;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hwnd), NULL);

    old_proc = control->notif_proc;

    control->notif_proc = notif_proc;

    return old_proc;
}

NOTIFPROC GUIAPI GetNotificationCallback (HWND hwnd)
{
    PCONTROL control = (PCONTROL)hwnd;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hwnd), NULL);

    return control->notif_proc;
}

/****************************** Hooks support ********************************/
#ifndef _MGRM_PROCESSES
MSGHOOK GUIAPI RegisterKeyMsgHook (void* context, MSGHOOK hook)
{
    return (MSGHOOK)SendMessage (HWND_DESKTOP, 
            MSG_REGISTERKEYHOOK, (WPARAM)context, (LPARAM)hook);
}

MSGHOOK GUIAPI RegisterMouseMsgHook (void* context, MSGHOOK hook)
{
    return (MSGHOOK)SendMessage (HWND_DESKTOP, 
            MSG_REGISTERMOUSEHOOK, (WPARAM)context, (LPARAM)hook);
}

#else

/*
 * REQID_REGKEYHOOK        0x0016
 */
HWND GUIAPI RegisterKeyHookWindow (HWND hwnd, DWORD flag)
{
    HWND old_hwnd = HWND_NULL;

    if (!mgIsServer) {

        REGHOOKINFO info;
        REQUEST req;

        info.id_op = ID_REG_KEY;
        info.hwnd = hwnd;
        info.flag = flag;

        req.id = REQID_REGISTERHOOK;
        req.data = &info;
        req.len_data = sizeof (REGHOOKINFO);
        ClientRequest (&req, &old_hwnd, sizeof (HWND));
    }

    return old_hwnd;
}

HWND GUIAPI RegisterMouseHookWindow (HWND hwnd, DWORD flag)
{
    HWND old_hwnd = HWND_NULL;
    if (!mgIsServer) {

        REGHOOKINFO info;
        REQUEST req;

        info.id_op = ID_REG_MOUSE;
        info.hwnd = hwnd;
        info.flag = flag;

        req.id = REQID_REGISTERHOOK;
        req.data = &info;
        req.len_data = sizeof (REGHOOKINFO);
        ClientRequest (&req, &old_hwnd, sizeof (HWND));
    }

    return old_hwnd;
}
#endif

/**************************** IME support ************************************/
int GUIAPI RegisterIMEWindow (HWND hWnd)
{
#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
        MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), -1);

        return SendMessage (HWND_DESKTOP, MSG_IME_REGISTER, (WPARAM)hWnd, 0);
    }else
        return ERR_INV_HWND;
#else
    return SendMessage (HWND_DESKTOP, MSG_IME_REGISTER, (WPARAM)hWnd, 0);
#endif
}

int GUIAPI UnregisterIMEWindow (HWND hWnd)
{
#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
        return SendMessage (HWND_DESKTOP, MSG_IME_UNREGISTER, (WPARAM)hWnd, 0);
    }else
        return ERR_IME_NOSUCHIMEWND;
#else
    return SendMessage (HWND_DESKTOP, MSG_IME_UNREGISTER, (WPARAM)hWnd, 0);
#endif
}

int GUIAPI GetIMEStatus (int StatusCode)
{
#ifdef _MGRM_PROCESSES
    if (!mgIsServer) {
        REQUEST req;
        int ret;

        req.id = REQID_GETIMESTAT;
        req.data = &StatusCode;
        req.len_data = sizeof(int); 

        ClientRequest (&req, &ret, sizeof(int));
        return ret;
    } else
        return SendMessage (HWND_DESKTOP, 
                MSG_IME_GETSTATUS, (WPARAM)StatusCode, 0);
#else
    return SendMessage (HWND_DESKTOP, 
            MSG_IME_GETSTATUS, (WPARAM)StatusCode, 0);
#endif
}

int GUIAPI SetIMEStatus (int StatusCode, int Value)
{
#ifdef _MGRM_PROCESSES
    if (!mgIsServer) {
        REQUEST req;
        unsigned int que_data;
        int ret;

        que_data = (StatusCode<<16)|Value;

        req.id = REQID_SETIMESTAT;
        req.data = &que_data;
        req.len_data = sizeof(int); 

        ClientRequest (&req, &ret, sizeof(int));
        return ret;
    } else
        return SendMessage (HWND_DESKTOP, 
                MSG_IME_SETSTATUS, (WPARAM)StatusCode, Value);
#else
    return SendMessage (HWND_DESKTOP, 
            MSG_IME_SETSTATUS, (WPARAM)StatusCode, Value);
#endif
}

int GUIAPI SetIMETargetInfo (const IME_TARGET_INFO *info)
{
    int ret = 0;
#ifdef _MGRM_PROCESSES
    if (!mgIsServer) {
        REQUEST req;

        req.id = REQID_SETIMEPOS;
        req.data = info;
        req.len_data = sizeof(IME_TARGET_INFO); 

        ClientRequest (&req, &ret, sizeof(int));
        return ret;
    } else
        return SendMessage (HWND_DESKTOP, 
                MSG_IME_SET_TARGET_INFO, 0, (LPARAM)info);
#else
    ret = SendMessage (HWND_DESKTOP,
            MSG_IME_SET_TARGET_INFO, 0, (LPARAM)info);
    return ret;
#endif
}

int GUIAPI GetIMEPos (POINT *pt)
{
    IME_TARGET_INFO info;
    int ret = GetIMETargetInfo (&info);
    *pt = info.ptCaret;
    return ret;
}

int GUIAPI SetIMEPos (const POINT* pt)
{
    IME_TARGET_INFO info;
    memset (&info, 0, sizeof(IME_TARGET_INFO));
    info.ptCaret = *pt;
    return SetIMETargetInfo (&info);
}

int GUIAPI GetIMETargetInfo (IME_TARGET_INFO *info)
{
#ifdef _MGRM_PROCESSES
    if (!mgIsServer) {
        REQUEST req;

        req.id = REQID_GETIMEPOS;
        req.data = 0;
        req.len_data = 0;

        return ClientRequest (&req, info, sizeof (IME_TARGET_INFO));
    }
    else {
        return SendMessage (HWND_DESKTOP, 
                MSG_IME_GET_TARGET_INFO, 0, (LPARAM)info);
    }
#else
    return SendMessage (HWND_DESKTOP, 
            MSG_IME_GET_TARGET_INFO, 0, (LPARAM)info);
#endif
}

HICON GetWindowIcon (HWND hWnd)
{
    PMAINWIN pWin;
    if (!(pWin = gui_CheckAndGetMainWindowPtr (hWnd)))
        return 0;

    return pWin->hIcon;
}

HICON SetWindowIcon (HWND hWnd, HICON hIcon, BOOL bRedraw)
{
    PMAINWIN pWin;
    HICON hOld;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hWnd)))
        return 0;

    hOld = pWin->hIcon;
    pWin->hIcon = hIcon;

    if (bRedraw) { /* redraw the whole window */
        UpdateWindow (hWnd, TRUE);
    }
    else {  /* draw caption only */
        HDC hdc;
        const WINDOW_ELEMENT_RENDERER* rdr;

        hdc = get_valid_dc (pWin, FALSE);
        rdr = GetWindowInfo ((HWND)pWin)->we_rdr;
        rdr ->draw_caption ((HWND)pWin, hdc, 
                !(pWin->dwStyle & WS_DISABLED) && (GetActiveWindow () == hWnd));

        if (pWin->pMainWin->secondaryDC) {
            draw_secondary_nc_area (pWin, rdr, hdc, HT_CAPTION);
        }
        release_valid_dc (pWin, hdc);
    }
    return hOld;
}

/**
 * CalcXYBannedRects:
 *   this function calculates x-y-banned rectangles of MYBITMAP 
 *   which is 8 bits per pixels.
 *
 * param hwnd           : the window handle
 * param mask           : MYBITMAP with 8 bits per pixels
 * param rect_size(out) : number of the x-y-banned rectangles
 *
 * return               : return NULL on failure, return the x-y-banned 
 *                        rectangles on success.
 *
 * note                 : You are responsible to free the x-y-banned rectangles.
 *
 * Author               : NuohuaZhou
 * Data                 : 2008-02-20
 */

#define XYBANNED_RECT_NR_INIT   10
#define XYBANNED_RECT_NR_INC    10

#define TYPE_MYBITMAP  1
#define TYPE_BITMAP    2

#include "../newgal/blit.h"

static RECT4MASK* CalcXYBannedRects (HDC hdc, const void* mask, int * rect_size, int type)
{
    Uint32  x, y;
    Uint32  pixel;
    POINT   section;
    RECT4MASK* xybanned_rects;
    BOOL    ismask, combined;
    int     max_index, max_calloced_nr;
    int     prev_pos_from, prev_pos_to, next_pos_from, next_pos_to;
    int     combined_index, i;
    int     w = 0, h = 0;
    BYTE*   bits = NULL;
    Uint32  transparent  = 0;
    unsigned sA = 0;
    unsigned sR, sG, sB;

    if (!mask) return NULL;

    if (type == TYPE_MYBITMAP){
        w = ((MYBITMAP*)mask)->w;
        h = ((MYBITMAP*)mask)->h;
        bits = ((MYBITMAP*)mask)->bits;
        transparent = ((MYBITMAP*)mask)->transparent;
        if (((MYBITMAP*)mask)->depth != 8) {
            return NULL;
        }
    }
    else {
        w = ((BITMAP*)mask)->bmWidth;
        h = ((BITMAP*)mask)->bmHeight;
        bits = ((BITMAP*)mask)->bmBits;
        if (!(dc_HDC2PDC(hdc))->surface->format) {
            return NULL;
        }
    }
    if (h <= 0) return NULL;

    if (rect_size)
        *rect_size = 0;

    max_calloced_nr = XYBANNED_RECT_NR_INIT;
    xybanned_rects  = (RECT4MASK *) calloc (max_calloced_nr, sizeof (RECT4MASK));
    if (!xybanned_rects) {
        return NULL;
    }

    max_index     = 0;
    next_pos_from = 0;
    next_pos_to   = max_index;

    for (y = 0; y < h; y++) {
        section.x        = 0;
        section.y        = 0;
        ismask           = FALSE;
        combined         = FALSE;
        prev_pos_from    = next_pos_from;
        prev_pos_to      = next_pos_to;

        for (x = 0; x <= w; x++) {
           if (type == TYPE_BITMAP) {
                GAL_PixelFormat* pixfmt = (dc_HDC2PDC(hdc))->surface->format;
                int bpp = ((BITMAP*)mask)->bmBytesPerPixel;
                BYTE* p = bits + (y * ((BITMAP*)mask)->bmPitch) + x * bpp;
                if (((BITMAP*)mask)->bmType & BMP_TYPE_ALPHA_MASK) {
                    BITMAP* bmp = (BITMAP*)mask;
                    int x = ((p - bmp->bmBits)%bmp->bmPitch);
                    int y = ((p - bmp->bmBits)/bmp->bmPitch);
                    int pitch = bmp->bmAlphaPitch;
                    sA = bmp->bmAlphaMask[y * pitch + x/bmp->bmBytesPerPixel];
                    DISEMBLE_RGB (p, bpp, pixfmt, pixel, sR, sG, sB); 
                }
                else {
                    DISEMBLE_RGBA (p, bpp, pixfmt, pixel, sR, sG, sB, sA);
                }
                sR ^= sG & sB; /* prevent unused-but-set-variable warning */
            }
            else {
                pixel = bits [y * ((MYBITMAP*)mask)->pitch + x];
            }
            
            if ((type == TYPE_MYBITMAP && pixel != transparent)
                    || (type == TYPE_BITMAP && sA != 0)) 
            {
                if (!ismask) {
                    section.x = x;
                    ismask = TRUE;
                }
            }
            
            if (((type == TYPE_MYBITMAP && pixel == transparent)
                        || (type == TYPE_BITMAP && sA == 0)
                        || x == w) && ismask) {
                ismask = FALSE;
                section.y = x;
                combined_index = -1;
                if (prev_pos_from <= max_index && prev_pos_to <= max_index) {
                    for (i = prev_pos_from; i <= prev_pos_to; ++i) {
                        if (y == xybanned_rects[i].bottom 
                                && section.x == xybanned_rects[i].left 
                                && section.y == xybanned_rects[i].right) {
                            ++xybanned_rects[i].bottom;
                            combined_index = i;
                            break;
                        }
                    }
                }
                if (-1 == combined_index) {
                    ++ max_index;
                    if (max_index >= max_calloced_nr) {
                        max_calloced_nr += XYBANNED_RECT_NR_INC;
                        xybanned_rects = (RECT4MASK *) realloc (xybanned_rects, 
                                max_calloced_nr * sizeof (RECT4MASK));
                        if (!xybanned_rects) {
                            return NULL;
                        }
                    }
                    xybanned_rects[max_index].left   = section.x;
                    xybanned_rects[max_index].top    = y;
                    xybanned_rects[max_index].right  = section.y;
                    xybanned_rects[max_index].bottom = y + 1;
                } else {
                    if (!combined) {
                        combined = TRUE;
                        next_pos_from = combined_index; 
                    } else {
                        if (combined_index < next_pos_from)
                            next_pos_from = combined_index;
                    }
                }
            }
        }// for (x = 0; x < w; ++x)
        
        if ((section.y == 0) && (section.x == 0)) {
            next_pos_to = next_pos_from = max_index + 1;
        } else {
            next_pos_to = max_index;
        }
    }// for (y = 1; y < h; ++y)

    if (rect_size)
        *rect_size = max_index + 1;

    return xybanned_rects;
}

/* API for non-rectangle window.*/
static int gui_GenerateMaskRect(HWND hWnd, RECT4MASK* rect, int rect_size)
{
    MASKRECT    *new_maskrect;
    PCONTROL    pCtrl;
    int         i, retval;

    if (!rect)
        return FALSE;

    if (IsControl (hWnd)) {
        /*
         * Because control without WS_EX_CTRLASMAINWIN has not 
         * the own Z order node, we should allocate memory for an 
         * array of rect_size for mask_rects.
         *
         * The prev of first mask_rects indicates the number of the array.
         * The next of last mask_rects is 0.
         * The other prev and next of mask_rects is the index of the array.
         */
        pCtrl = (PCONTROL) hWnd;

        if (!(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {

            if (pCtrl->mask_rects == NULL) {
                pCtrl->mask_rects = 
                    (MASKRECT *) calloc (rect_size, sizeof (MASKRECT));

                if (!pCtrl->mask_rects) {
                    _MG_PRINTF ("GUI>Window: Not enough memory!\n");
                    free (rect);
                    return FALSE;
                }
            }
            else {
                /* 
                 * when number of calloced mask_rects is not enough,
                 * free the old memory and calloc more.
                 */
                if (pCtrl->mask_rects->prev < rect_size) {

                    new_maskrect = 
                        (MASKRECT *) calloc (rect_size, sizeof (MASKRECT));
                    if (!new_maskrect) {
                        _MG_PRINTF ("GUI>Window: Not enough memory!\n");
                        free (rect);
                        return FALSE;
                    }
                    free (pCtrl->mask_rects);
                    pCtrl->mask_rects = new_maskrect;
                }
            }

            for (i = 0; i < rect_size; ++i)
            {
                (pCtrl->mask_rects + i)->left   = rect[i].left;
                (pCtrl->mask_rects + i)->top    = rect[i].top;
                (pCtrl->mask_rects + i)->right  = rect[i].right;
                (pCtrl->mask_rects + i)->bottom = rect[i].bottom;
                (pCtrl->mask_rects + i)->prev   = i;
                (pCtrl->mask_rects + i)->next   = i + 1;
            }

            /** the field [prev] is used to store number of mask_rects */
            pCtrl->mask_rects->prev = rect_size;

            /** mark the end with 0 */
            (pCtrl->mask_rects + rect_size - 1)->next = 0;

            free (rect);

            if (pCtrl->dwStyle & WS_VISIBLE) {
                ShowWindow (hWnd, SW_HIDE);
                ShowWindow (hWnd, SW_SHOW);
            }

            return TRUE;
        }
    }

    retval = kernel_change_z_order_mask_rect (hWnd, rect, rect_size);
    free (rect);

    pCtrl = (PCONTROL)hWnd;

    if (!retval && pCtrl->dwStyle & WS_VISIBLE) {
        MoveWindow (hWnd, pCtrl->left, pCtrl->top, 
                pCtrl->right - pCtrl->left - 1,
                pCtrl->bottom - pCtrl->top - 1, FALSE);
        MoveWindow (hWnd, pCtrl->left, pCtrl->top,
                pCtrl->right - pCtrl->left + 1,
                pCtrl->bottom - pCtrl->top + 1, FALSE);
    }

    return retval;
}

BOOL GUIAPI SetWindowMask (HWND hWnd, const MYBITMAP* mask)
{
    int         retval, rect_size;
    RECT4MASK   *rect;

    rect = CalcXYBannedRects (HDC_SCREEN, mask, &rect_size, TYPE_MYBITMAP);
    if (!rect)
        return FALSE;
    
    retval = gui_GenerateMaskRect(hWnd, rect, rect_size);
    return (retval == 0 ? TRUE : FALSE);
}

BOOL GUIAPI SetWindowMaskEx (HWND hWnd, HDC hdc, const BITMAP* mask)
{
    int         retval, rect_size;
    RECT4MASK   *rect;

    rect = CalcXYBannedRects (hdc, mask, &rect_size, TYPE_BITMAP);
    if (!rect)
        return FALSE;
    
    retval = gui_GenerateMaskRect(hWnd, rect, rect_size);
    return (retval == 0 ? TRUE : FALSE);
}

BOOL GUIAPI SetWindowRegion (HWND hWnd, const CLIPRGN * region)
{
    MASKRECT    *new_maskrect;
    PCLIPRECT   cliprc;
    RECT4MASK   *rect;
    PCONTROL    pCtrl;
    int         retval, rect_size, i;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);

    if (!region || IsEmptyClipRgn(region))
        return FALSE;

    /** number of rect */
    for (rect_size = 1, cliprc = region->head; 
            cliprc && cliprc != region->tail; rect_size++, cliprc=cliprc->next);

    if (IsControl (hWnd)) {
        /*
         * Because control without WS_EX_CTRLASMAINWIN has not 
         * the Z order node, we should allocate memory for an 
         * array of rect_size for mask_rects.
         *
         * The prev of first mask_rects indicates the number of the array.
         * The next of last mask_rects is 0.
         * The other prev and next of mask_rects is the index of the array.
         */
        pCtrl = (PCONTROL) hWnd;

        if (!(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {

            if (pCtrl->mask_rects == NULL) {
                pCtrl->mask_rects = 
                    (MASKRECT *) calloc (rect_size, sizeof (MASKRECT));

                if (!pCtrl->mask_rects) {
                    _MG_PRINTF ("GUI>Window: Not enough memory!\n");
                    return FALSE;
                }
            }
            else {
                /* 
                 * when number of calloced mask_rects is not enough,
                 * free the old memory and calloc more.
                 */
                if (pCtrl->mask_rects->prev < rect_size) {
                    new_maskrect = 
                        (MASKRECT *) calloc (rect_size, sizeof (MASKRECT));

                    if (!new_maskrect) {
                        _MG_PRINTF ("GUI>Window: Not enough memory!\n");
                        return FALSE;
                    }
                    free (pCtrl->mask_rects);
                    pCtrl->mask_rects = new_maskrect;
                }
            }

            cliprc = region->head;

            for (i = 0; i < rect_size; ++i) {
                (pCtrl->mask_rects + i)->left   = cliprc->rc.left;
                (pCtrl->mask_rects + i)->top    = cliprc->rc.top;
                (pCtrl->mask_rects + i)->right  = cliprc->rc.right;
                (pCtrl->mask_rects + i)->bottom = cliprc->rc.bottom;
                (pCtrl->mask_rects + i)->prev   = i;
                (pCtrl->mask_rects + i)->next   = i + 1;
                cliprc = cliprc->next;
            }

            /** the field [prev] is used to store number of mask_rects */
            pCtrl->mask_rects->prev = rect_size;

            /** mark the end with 0 */
            (pCtrl->mask_rects + rect_size - 1)->next = 0;

            if (pCtrl->dwStyle & WS_VISIBLE) {
                ShowWindow (hWnd, SW_HIDE);
                ShowWindow (hWnd, SW_SHOW);
            }
            return TRUE;
        }
    }

    rect = (RECT4MASK *) calloc(rect_size, sizeof(RECT4MASK));
    if(rect == NULL) {
        _MG_PRINTF ("GUI>Window: Not enough memory!\n");
        return FALSE;
    }

    cliprc = region->head;

    for (i = 0; i < rect_size; ++i) {
        (rect + i)->left    = cliprc->rc.left;
        (rect + i)->top     = cliprc->rc.top;
        (rect + i)->right   = cliprc->rc.right;
        (rect + i)->bottom  = cliprc->rc.bottom;
        cliprc = cliprc->next;
    }

    retval = kernel_change_z_order_mask_rect (hWnd, rect, rect_size);
    free (rect);

    pCtrl = (PCONTROL) hWnd;

    if (!retval && pCtrl->dwStyle & WS_VISIBLE) {
        MoveWindow (hWnd, pCtrl->left, pCtrl->top, 
                pCtrl->right - pCtrl->left - 1,
                pCtrl->bottom - pCtrl->top - 1, FALSE);
        MoveWindow (hWnd, pCtrl->left, pCtrl->top,
                pCtrl->right - pCtrl->left + 1,
                pCtrl->bottom - pCtrl->top + 1, FALSE);
    }

    return (retval == 0 ? TRUE : FALSE);
}

BOOL GUIAPI GetWindowRegion (HWND hWnd, CLIPRGN* region)
{
    PCONTROL    pCtrl;
    int         nr_rects, i;
    RECT        rc;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW(hWnd), FALSE);
    MG_CHECK_RET (region != NULL, FALSE);

    pCtrl = (PCONTROL) hWnd;
    if (IsControl (hWnd) && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
        /* the field [prev] is used to store number of mask_rects */
        nr_rects = pCtrl->mask_rects->prev;
        if (nr_rects == 0) {
            GetWindowRect (hWnd, &rc);
            OffsetRect (&rc, rc.left, rc.top);
            return SetClipRgn (region, &rc);
        }
        else {
            EmptyClipRgn (region);
            for (i = 0; i < nr_rects; i++) {
                rc.left = (pCtrl->mask_rects + i)->left;
                rc.top = (pCtrl->mask_rects + i)->top;
                rc.right = (pCtrl->mask_rects + i)->right;
                rc.bottom = (pCtrl->mask_rects + i)->bottom;
                if (!AddClipRect (region, &rc))
                    return FALSE;
            }
        }
    }
    else {
        return kernel_get_window_region (hWnd, region) > 0;
    }

    return TRUE;
}

#ifdef __TARGET_MSTUDIO__

int GUIAPI GetWindowZOrder(HWND hWnd)
{
    PCONTROL pCtrl, pCtrlTmp;
    int idx;
    if(!IsWindow(hWnd))
        return -1;

    if(IsMainWindow(hWnd))
        return 0;

    pCtrl = (PCONTROL)hWnd;

    pCtrlTmp = pCtrl->pParent->children;
    
    idx = 0;
    while(pCtrlTmp && pCtrl != pCtrlTmp)
    {
        idx ++;
        pCtrlTmp = pCtrlTmp->next;
    }

    return idx;
}

int GUIAPI SetWindowZOrder(HWND hWnd, int zorder)
{
    int idx;
    PCONTROL pCtrl, pCtrlTmp;
    if(!IsWindow(hWnd))
        return -1;

    if(IsMainWindow(hWnd))
        return 0;
    
    pCtrl = (PCONTROL)hWnd;

    pCtrlTmp = pCtrl->pParent->children;

    idx = 0;
    while(idx < zorder && pCtrlTmp && pCtrlTmp != pCtrl){
        idx ++;
        pCtrlTmp = pCtrlTmp->next;
    }

    if(idx < zorder) //Move Down
    {
        pCtrlTmp = pCtrlTmp->next;
        if(pCtrlTmp && pCtrl == pCtrl->pParent->children)
            pCtrl->pParent->children = pCtrl->next;
        while(idx < zorder && pCtrlTmp)
        {
            //switch pCtrl and pCtrlTmp
            pCtrl->next = pCtrlTmp->next;
            pCtrlTmp->prev = pCtrl->prev;
            pCtrl->prev = pCtrlTmp;
            pCtrlTmp->next = pCtrl;
            if(pCtrl->next)
                pCtrl->next->prev = pCtrl;
            if(pCtrlTmp->prev)
                pCtrlTmp->prev->next = pCtrlTmp;
            pCtrlTmp = pCtrl->next;
        }
    }
    else if(pCtrlTmp && pCtrlTmp != pCtrl) // Move Up
    {
        PCONTROL prev;
        do{
            prev = pCtrl->prev;
            if(prev == NULL)
                break;
            prev->next = pCtrl->next;
            pCtrl->prev = prev->prev;
            pCtrl->next = prev;
            prev->prev = pCtrl;
            if(pCtrl->prev)
                pCtrl->prev->next = pCtrl;
            if(prev->next)
                prev->next->prev = prev;
        }while(prev != pCtrlTmp);
        if(pCtrlTmp == pCtrl->pParent->children)
            pCtrl->pParent->children = pCtrl;

        if(prev == NULL)
            return 0;
    }

    return idx;
}

#endif

