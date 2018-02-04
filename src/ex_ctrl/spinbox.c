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
** spinbox.c: the SpinBox control
**
** Create date: 2001/3/27
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGCTRL_SPINBOX
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"

#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "ctrl/spinbox.h"
#include "spinbox_impl.h"

typedef struct SpinData {
    SPININFO spinfo;
    HWND hTarget;
    int canup, candown;
} SPINDATA;
typedef SPINDATA* PSPINDATA;

static void 
DrawSpinBoxVert (HWND hwnd, HDC hdc, 
        SPININFO* spinfo , SPINDATA* psd, DWORD dwStyle)
{
    BOOL        flag = !(dwStyle & SPS_AUTOSCROLL);
    RECT        rect;
    DWORD       color; 
    WINDOWINFO  *info;
 
    info = (WINDOWINFO*)GetWindowInfo (hwnd);
    color = GetWindowElementAttr (hwnd, WE_FGC_WINDOW);  

    if ((dwStyle & SPS_TYPE_MASK) == SPS_TYPE_NORMAL
            || (dwStyle & SPS_TYPE_MASK) == SPS_TYPE_UPARROW) {

        GetClientRect (hwnd, &rect); 
        rect.bottom = rect.bottom >> 1;

        if (spinfo->cur > spinfo->min || (flag && psd->canup)) { 
            info->we_rdr->draw_arrow (hwnd, hdc, &rect, color, 
                    LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_UP);
        }
        else { 
            /* Draw disabled vert up arrow button */
            info->we_rdr->draw_arrow (hwnd, hdc, 
                &rect, color, 
                LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_UP | LFRDR_ARROW_NOFILL);
        }
    }

    if ((dwStyle & SPS_TYPE_MASK) == SPS_TYPE_NORMAL
            || (dwStyle & SPS_TYPE_MASK) == SPS_TYPE_DOWNARROW) {

        GetClientRect (hwnd, &rect);
        rect.top = rect.bottom >> 1;

        if (spinfo->cur < spinfo->max || (flag && psd->candown)) { 
            //Draw enabled vert down arrow button
            info->we_rdr->draw_arrow (hwnd, hdc, &rect, color, 
                    LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_DOWN);
        }
        else { 
            /* Draw disabled vert down arrow button */
            info->we_rdr->draw_arrow (hwnd, hdc, &rect, color, 
                LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_DOWN | LFRDR_ARROW_NOFILL);
        }
    }
}

static void 
DrawSpinBoxHorz (HWND hwnd, HDC hdc, 
        SPININFO* spinfo, SPINDATA* psd, DWORD dwStyle)
{
    BOOL flag = !(dwStyle & SPS_AUTOSCROLL);
    RECT rect;
    WINDOWINFO  *info;
    DWORD color; 
 
    info = (WINDOWINFO*)GetWindowInfo (hwnd);
    color = GetWindowElementAttr (hwnd, WE_FGC_WINDOW);  

    if ((dwStyle & SPS_TYPE_MASK) == SPS_TYPE_NORMAL
            || (dwStyle & SPS_TYPE_MASK) == SPS_TYPE_UPARROW) {

        GetClientRect (hwnd, &rect);
        rect.right = rect.right >> 1;

        if (spinfo->cur > spinfo->min || (flag && psd->canup)) {
            info->we_rdr->draw_arrow (hwnd, hdc, &rect, color, 
                    LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_LEFT);
        }
        else {
            info->we_rdr->draw_arrow (hwnd, hdc, &rect, color, 
                LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_LEFT | LFRDR_ARROW_NOFILL);
        }
    }

    if ((dwStyle & SPS_TYPE_MASK) == SPS_TYPE_NORMAL
            || (dwStyle & SPS_TYPE_MASK) == SPS_TYPE_DOWNARROW) {

        GetClientRect (hwnd, &rect);
        rect.left = rect.right >> 1;

        if (spinfo->cur < spinfo->max || (flag && psd->candown)) {
            info->we_rdr->draw_arrow (hwnd, hdc, &rect, color, 
                    LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_RIGHT);
        }
        else {
            info->we_rdr->draw_arrow (hwnd, hdc, &rect, color , 
               LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_RIGHT | LFRDR_ARROW_NOFILL);
        }
    }
}

static void 
OnUpArrow (HWND hWnd, PSPINDATA pData, 
        int id, DWORD dwStyle, DWORD wParam)
{
    if ((dwStyle & SPS_AUTOSCROLL)) {
        if (pData->spinfo.cur > pData->spinfo.min ) {
            pData->spinfo.cur --;
            PostMessage (pData->hTarget, MSG_KEYDOWN, 
                    SCANCODE_CURSORBLOCKUP, (wParam|KS_SPINPOST));
            PostMessage (pData->hTarget, MSG_KEYUP, 
                    SCANCODE_CURSORBLOCKUP, (wParam|KS_SPINPOST));
            if ( pData->spinfo.cur <= pData->spinfo.min ) {
                NotifyParent ( hWnd, id, SPN_REACHMIN);
            }
        }
    } else {
        if (pData->canup) {
            pData->spinfo.cur --;
            PostMessage (pData->hTarget, MSG_KEYDOWN, 
                    SCANCODE_CURSORBLOCKUP, wParam|KS_SPINPOST);
            PostMessage (pData->hTarget, MSG_KEYUP, 
                    SCANCODE_CURSORBLOCKUP, wParam|KS_SPINPOST);
            if ( pData->spinfo.cur <= pData->spinfo.min ) {
                NotifyParent ( hWnd, id, SPN_REACHMIN);
            }
        }
    }
}

static void 
OnDownArrow (HWND hWnd, PSPINDATA pData, int id, DWORD dwStyle, DWORD wParam)
{
    if ((dwStyle & SPS_AUTOSCROLL)) {
        if (pData->spinfo.cur < pData->spinfo.max) {
            pData->spinfo.cur ++;
            PostMessage (pData->hTarget, MSG_KEYDOWN, 
                    SCANCODE_CURSORBLOCKDOWN, wParam|KS_SPINPOST);
            PostMessage (pData->hTarget, MSG_KEYUP, 
                    SCANCODE_CURSORBLOCKDOWN, wParam|KS_SPINPOST);
            if ( pData->spinfo.cur >= pData->spinfo.max) {
                NotifyParent ( hWnd, id, SPN_REACHMAX);
            }
        }
    } else {
        if (pData->candown) {
            pData->spinfo.cur ++;
            PostMessage (pData->hTarget, MSG_KEYDOWN, 
                    SCANCODE_CURSORBLOCKDOWN, wParam|KS_SPINPOST);
            PostMessage (pData->hTarget, MSG_KEYUP, 
                    SCANCODE_CURSORBLOCKDOWN, wParam|KS_SPINPOST);
            if ( pData->spinfo.cur >= pData->spinfo.max) {
                NotifyParent ( hWnd, id, SPN_REACHMAX);
            }
        }
    }
}

static LRESULT
SpinCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PSPINDATA pData = (PSPINDATA) GetWindowAdditionalData2 (hWnd);

    switch (message) {   
        case MSG_CREATE:
            pData = (PSPINDATA) calloc (1, sizeof (SPINDATA));
            if (!pData) {
                return -1;
            }
            pData->canup = 1;
            pData->candown =  1;
            pData->hTarget = GetParent ( hWnd );
            SetWindowAdditionalData2 (hWnd, (DWORD)pData);
        break;

        case MSG_DESTROY:
            free (pData);
        break;

        case MSG_SIZECHANGING:
        {
            const RECT* rcExpect = (const RECT*)wParam;
            RECT* rcResult = (RECT*)lParam;

            CopyRect (rcResult, rcExpect);
            return 0;
        }

        case MSG_SIZECHANGED:
        {
            RECT* rcWin = (RECT*)wParam;
            RECT* rcClient = (RECT*)lParam;
            *rcClient = *rcWin;
            return 1;
        }

        case SPM_SETINFO: 
        {
            PSPININFO spinfo = (PSPININFO) lParam;
            if ( !spinfo )
                return -1;

            if (!(GetWindowStyle (hWnd) & SPS_AUTOSCROLL)) {
                memcpy ( &(pData->spinfo), spinfo, sizeof (SPININFO) );
            } else {
                if ( (spinfo->max >= spinfo->min &&
                        spinfo->max >= spinfo->cur &&
                        spinfo->cur >= spinfo->min )) {
                    memcpy ( &(pData->spinfo), spinfo, sizeof (SPININFO) );
                } else {
                    return -1;
                }
            }
            InvalidateRect ( hWnd, NULL, FALSE );
            return 0;
        }

        case SPM_GETINFO:
        {
            PSPININFO spinfo = (PSPININFO) lParam;
            if ( !spinfo )
                return -1;
            memcpy ( spinfo, &(pData->spinfo), sizeof (SPININFO) );
            return 0;
        }

        case SPM_SETCUR:
        {
            if (!(GetWindowStyle (hWnd) & SPS_AUTOSCROLL)) {
                pData->spinfo.cur = wParam;
            }
            else{
                if ( wParam > pData->spinfo.max || wParam < pData->spinfo.min)
                    return -1;
                pData->spinfo.cur = wParam;
            }
            InvalidateRect (hWnd, NULL, FALSE );
            return 0;
        }

        case SPM_GETCUR:
            return pData->spinfo.cur;

        case SPM_ENABLEUP:
            pData->canup = 1;
            InvalidateRect ( hWnd, NULL, FALSE );
            return 0;

        case SPM_ENABLEDOWN:
            pData->candown = 1;
            InvalidateRect ( hWnd, NULL, FALSE );
            return 0;

        case SPM_DISABLEUP:
            pData->canup = 0;
            InvalidateRect ( hWnd, NULL, FALSE );
            return 0;

        case SPM_DISABLEDOWN:
            pData->candown = 0;
            InvalidateRect ( hWnd, NULL, FALSE );
            return 0;

        case SPM_SETTARGET:
            pData->hTarget = (HWND) lParam;
            return 0;

        case SPM_GETTARGET:
            return (LRESULT)pData->hTarget;

        case MSG_NCPAINT:
            return 0;

        case MSG_PAINT:
        {
            DWORD dwStyle = GetWindowStyle (hWnd);
            HDC hdc = BeginPaint (hWnd);

            SetBkMode (hdc, BM_TRANSPARENT);

            if (dwStyle & SPS_HORIZONTAL)
                DrawSpinBoxHorz (hWnd, hdc, &pData->spinfo, pData, dwStyle);
            else
                DrawSpinBoxVert (hWnd, hdc, &pData->spinfo, pData, dwStyle);

            EndPaint (hWnd, hdc);
            return 0;
        }

        case MSG_LBUTTONDOWN:
        {
            int     posx = LOWORD (lParam);
            int     posy = HIWORD (lParam);
            int     id = GetDlgCtrlID (hWnd);
            RECT    rect;
            DWORD   dwStyle = GetWindowStyle (hWnd);

            GetClientRect (hWnd, &rect); 

            if ((dwStyle & SPS_TYPE_MASK) == SPS_TYPE_UPARROW) {
                OnUpArrow (hWnd, pData, id, dwStyle, wParam);
            }
            else if ((dwStyle & SPS_TYPE_MASK) == SPS_TYPE_DOWNARROW) {
                OnDownArrow (hWnd, pData, id, dwStyle, wParam);
            }
            else if ( ((dwStyle & SPS_HORIZONTAL) && (posx <= RECTW(rect)>>1))
                || (!(dwStyle & SPS_HORIZONTAL) && (posy <= RECTH(rect)>>1)) ) {
                OnUpArrow (hWnd, pData, id, dwStyle, wParam);
            }
            else if ( ((dwStyle & SPS_HORIZONTAL) && (posx >= (RECTW(rect)>>1) + 2))
                || (!(dwStyle & SPS_HORIZONTAL) && (posy >= (RECTH(rect)>>1) + 2))) {
                OnDownArrow (hWnd, pData, id, dwStyle, wParam);
            }

            InvalidateRect ( hWnd, NULL, FALSE );

            if (GetCapture () == hWnd)
                break;

            SetCapture (hWnd);
            SetAutoRepeatMessage (hWnd, message, wParam, lParam);
            break;
        }
            
        case MSG_LBUTTONUP:
            SetAutoRepeatMessage (0, 0, 0, 0);
            ReleaseCapture ();
            break;

        default:
            break;
    }

    return DefaultControlProc (hWnd, message, wParam, lParam);
}

BOOL RegisterSpinControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_SPINBOX;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = SpinCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#endif /* _MGCTRL_SPINBOX */

