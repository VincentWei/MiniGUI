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
** coolbar.c: the Coolbar Control module.
**
** Original author: Wang Jian.
**
** Create date: 2001-08-20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#ifdef _MGCTRL_COOLBAR
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
//#include "mywindows.h"

#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "ctrl/coolbar.h"
#include "coolbar_impl.h"

#define ITEMBARWIDTH    8

static LRESULT CoolBarCtrlProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL RegisterCoolBarControl (void)
{
    WNDCLASS WndClass;
        
    WndClass.spClassName = CTRL_COOLBAR;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_DESKTOP, WE_MAINC_THREED_BODY);
    WndClass.WinProc     = CoolBarCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

static COOLBARITEMDATA* GetCurTag (int posx, int posy, PCOOLBARCTRL pdata)
{
    COOLBARITEMDATA*  tmpdata;

    tmpdata = pdata->head;
    while (tmpdata) { 
        if (PtInRect (&tmpdata->RcTitle, posx, posy)
                && tmpdata->ItemType != TYPE_BARITEM) {
            return tmpdata;    
        }
        tmpdata = tmpdata->next;
    }
    return NULL;         
}

static COOLBARITEMDATA* GetCurSel (PCOOLBARCTRL pdata)
{
    COOLBARITEMDATA*  tmpdata;

    tmpdata = pdata->head;
    while (tmpdata) { 
        if (pdata->iSel == tmpdata->insPos
                || pdata->iMvOver == tmpdata->insPos)
            return tmpdata;    

        tmpdata = tmpdata->next;
    }

    return NULL;         
}

static int enable_item (PCOOLBARCTRL TbarData, int id, BOOL flag)
{
    COOLBARITEMDATA*  tmpdata;
   
    tmpdata = TbarData->head;
    while (tmpdata) { 
        if (tmpdata->id == id) {
            tmpdata->Disable = !flag;    
            return 0;
        }
        tmpdata = tmpdata->next;
    }

    return -1;
}

static void ShowCurItemHint (HWND hWnd, PCOOLBARCTRL cb, COOLBARITEMDATA* tmpdata)
{
    int x = tmpdata->hintx, y = tmpdata->hinty;
    PCOOLBARCTRL TbarData = (PCOOLBARCTRL) GetWindowAdditionalData2(hWnd);

    if (tmpdata->Hint [0] == '\0')
        return;

    ClientToScreen (hWnd, &x, &y);
    if (g_rcScr.bottom - y < y - g_rcScr.top) {
        y -= (GetSysCharHeight () + (2 << 1) + cb->ItemHeight);
    }

    if (TbarData->hToolTip == 0) {
        TbarData->hToolTip = CreateToolTipWin (hWnd, x, y, 1000, tmpdata->Hint);
    }
    else {
        ResetToolTipWin (TbarData->hToolTip, x, y, tmpdata->Hint);
    }
}

static void unhilight (HWND hwnd)
{
    COOLBARITEMDATA* pItemdata;
    PCOOLBARCTRL TbarData;
    
    TbarData = (PCOOLBARCTRL) GetWindowAdditionalData2(hwnd);

    if ((pItemdata = GetCurSel(TbarData))) {
        TbarData->iSel = -1;
        TbarData->iMvOver = -1;
        InvalidateRect (hwnd, &pItemdata->RcTitle, TRUE);
    }
}

static void draw_hilight_box (HWND hWnd, HDC hdc, COOLBARITEMDATA* item)
{
    int  l,r,t,b; 
    WINDOWINFO *info = (WINDOWINFO*)GetWindowInfo (hWnd);
    DWORD color;
    DWORD mainc = GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY);

    l = item->RcTitle.left;
    t = item->RcTitle.top;
    r = item->RcTitle.right - 1;
    b = item->RcTitle.bottom - 1;

    color = info->we_rdr->calc_3dbox_color (mainc, LFRDR_3DBOX_COLOR_DARKER);
    SetPenColor (hdc,
            RGBA2Pixel (hdc, GetRValue (color), GetGValue (color), 
                GetBValue (color), GetAValue (color)));
    MoveTo (hdc, l, t);
    LineTo (hdc, l, b);
    MoveTo (hdc, r - 1, t);
    LineTo (hdc, r - 1, b);

    color = info->we_rdr->calc_3dbox_color (mainc, LFRDR_3DBOX_COLOR_LIGHTER);
    SetPenColor (hdc,
            RGBA2Pixel (hdc, GetRValue (color), GetGValue (color), 
                GetBValue (color), GetAValue (color)));
    MoveTo (hdc, l + 1, t);
    LineTo (hdc, l + 1, b);
    MoveTo (hdc, r, t);
    LineTo (hdc, r, b);
}

static void hilight (HWND hWnd, PCOOLBARCTRL cb, COOLBARITEMDATA* item)
{
    HDC hdc;

    hdc = GetClientDC (hWnd);
    draw_hilight_box (hWnd, hdc, item);
    ReleaseDC (hdc);

    ShowCurItemHint (hWnd, cb, item);
}

static void DrawCoolBox (HWND hWnd, HDC hdc, PCOOLBARCTRL pdata)
{
    COOLBARITEMDATA* tmpdata;
    RECT rc;
    int l,t;
    WINDOWINFO *info = (WINDOWINFO*)GetWindowInfo (hWnd);
    DWORD color;
    DWORD mainc = GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY);
  
    GetClientRect (hWnd, &rc);

    if (pdata->BackBmp) {
        FillBoxWithBitmap (hdc, 0, 0, rc.right, rc.bottom, pdata->BackBmp);
    }

    color = info->we_rdr->calc_3dbox_color (mainc, LFRDR_3DBOX_COLOR_DARKEST);
    SetPenColor (hdc,
            RGBA2Pixel (hdc, GetRValue (color), GetGValue (color), 
                GetBValue (color), GetAValue (color)));
    MoveTo (hdc, 0, 0);
    LineTo (hdc, rc.right, 0);
    MoveTo (hdc, 0, rc.bottom - 2);
    LineTo (hdc, rc.right, rc.bottom - 2);

    color = info->we_rdr->calc_3dbox_color (mainc, LFRDR_3DBOX_COLOR_LIGHTEST);
    SetPenColor (hdc,
            RGBA2Pixel (hdc, GetRValue (color), GetGValue (color), 
                GetBValue (color), GetAValue (color)));
    MoveTo (hdc, 0, 1);
    LineTo (hdc, rc.right, 1);
    MoveTo (hdc, 0, rc.bottom - 1);
    LineTo (hdc, rc.right, rc.bottom - 1);

    tmpdata = pdata->head;
    while (tmpdata) {
        l = tmpdata->RcTitle.left;
        t = tmpdata->RcTitle.top;
      
        switch (tmpdata->ItemType) {
        case TYPE_BARITEM:
        {
            WINDOWINFO *info = (WINDOWINFO*)GetWindowInfo (hWnd);
            RECT rcTmp;
            rcTmp.left = l + 2;
            rcTmp.top = 4;
            rcTmp.right = l + 4;
            rcTmp.bottom = rc.bottom - 4;

            info->we_rdr->draw_3dbox (hdc, &rcTmp, 
                GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY),
                LFRDR_BTN_STATUS_PRESSED);
        }
            break;

        case TYPE_BMPITEM:
            FillBoxWithBitmap (hdc, l + 2, t + 2, 
                            pdata->ItemWidth, pdata->ItemHeight, tmpdata->Bmp);
            break;

        case TYPE_TEXTITEM:
        {
            SIZE size;
            int h;
            WINDOWINFO *info;
            RECT rc;

            if (tmpdata->Caption == NULL || tmpdata->Caption [0] == '\0')
                break;

            GetTextExtent (hdc, tmpdata->Caption, -1, &size);
            h = (pdata->ItemHeight - size.cy) / 2;

            SetBkMode (hdc, BM_TRANSPARENT);
            if (tmpdata->Disable) {
                info = (WINDOWINFO*)GetWindowInfo (hWnd);
                rc.left = l + 2;
                rc.top = t + h + 2;
                rc.right = rc.left + size.cx;
                rc.bottom = rc.top + size.cy;
                info->we_rdr->disabled_text_out (hWnd, hdc,
                    tmpdata->Caption, &rc, DT_SINGLELINE);
            }
            else {
                SetBkColor (hdc, GetWindowBkColor (hWnd));
                SetTextColor (hdc, PIXEL_black);
                TextOut (hdc, l+2, t + h + 2, tmpdata->Caption);
            }

            break;
        }

        default:
            break;
        }

        tmpdata = tmpdata->next;
    }

    if ((tmpdata = GetCurSel (pdata)) == NULL)
        return;

    draw_hilight_box (hWnd, hdc, tmpdata);
}

static void set_item_rect (HWND hwnd, PCOOLBARCTRL TbarData, COOLBARITEMDATA* ptemp)
{
    SIZE size;
    int w;

    if (TbarData->tail == NULL) 
        ptemp->RcTitle.left = 2;
    else
        ptemp->RcTitle.left = TbarData->tail->RcTitle.right;

    switch (ptemp->ItemType) {
    case TYPE_BARITEM:
        w = ITEMBARWIDTH;
        break;

    case TYPE_BMPITEM:
        w = TbarData->ItemWidth + 4;
        break;

    case TYPE_TEXTITEM:
        if (strlen (ptemp->Caption)) {
            HDC hdc;
            hdc = GetClientDC (hwnd);
            GetTextExtent (hdc, ptemp->Caption, -1, &size);
            ReleaseDC (hdc);
        }
        w = size.cx + 4;
        break;

    default:
        w = 0;
        break;
    }

    ptemp->RcTitle.right = ptemp->RcTitle.left + w;
    ptemp->RcTitle.top = 2;
    ptemp->RcTitle.bottom = ptemp->RcTitle.top + TbarData->ItemHeight + 4;
  
    ptemp->hintx = ptemp->RcTitle.left;
    ptemp->hinty = ptemp->RcTitle.bottom;
}

static LRESULT CoolBarCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC              hdc;
    PCOOLBARCTRL     TbarData;
    PCOOLBARITEMDATA pTbid;
        
    switch (message) {
        case MSG_CREATE:
        {
            DWORD data; 
            DWORD dwStyle;
            const char* caption;

            if ((TbarData = (COOLBARCTRL*) calloc (1, sizeof (COOLBARCTRL))) == NULL)
                return 1;

            TbarData->nCount = 0;
            TbarData->head = TbarData->tail = NULL;
            TbarData->BackBmp = NULL;
            TbarData->iSel = -1;
            TbarData->iMvOver = -1;
            TbarData->ShowHint = TRUE;
            TbarData->hToolTip = 0;

            ExcludeWindowStyle (hWnd, WS_BORDER);

            dwStyle = GetWindowStyle (hWnd);
            if (dwStyle & CBS_BMP_32X32) {
                TbarData->ItemWidth = 32;
                TbarData->ItemHeight = 32;
            }
            else if (dwStyle & CBS_BMP_CUSTOM) {
                data = GetWindowAdditionalData (hWnd);
                TbarData->ItemWidth = LOWORD (data);
                TbarData->ItemHeight = HIWORD (data);
            }
            else {
                TbarData->ItemWidth = 16;
                TbarData->ItemHeight = 16;
            }

            caption = GetWindowCaption (hWnd);
            if ((dwStyle & CBS_USEBKBMP) && caption [0]) {
                TbarData->BackBmp = (BITMAP*)calloc (1, sizeof (BITMAP));
                if (LoadBitmap (HDC_SCREEN, TbarData->BackBmp, caption) < 0) {
                    free (TbarData->BackBmp);
                    TbarData->BackBmp = NULL;
                    break;
                }
            }
            SetWindowAdditionalData2 (hWnd, (DWORD)TbarData);
        }
        break;

        case MSG_DESTROY:
        { 
            COOLBARITEMDATA* unloaddata, *tmp;
            TbarData = (PCOOLBARCTRL) GetWindowAdditionalData2(hWnd);
            if (TbarData->hToolTip != 0) {
                DestroyToolTipWin (TbarData->hToolTip);
                TbarData->hToolTip = 0;
            }

            if (TbarData->BackBmp) {
                UnloadBitmap (TbarData->BackBmp);
                free (TbarData->BackBmp);
            }

            unloaddata = TbarData->head;
            while (unloaddata) {
                tmp = unloaddata->next;
                free (unloaddata);
                unloaddata = tmp;
            }

            free (TbarData);
        }
        break;

        case MSG_SIZECHANGING:
        {
            const RECT* rcExpect = (const RECT*)wParam;
            RECT* rcResult = (RECT*)lParam;

            TbarData = (PCOOLBARCTRL) GetWindowAdditionalData2(hWnd);

            rcResult->left = rcExpect->left;
            rcResult->top = rcExpect->top;
            rcResult->right = rcExpect->right;
            rcResult->bottom = rcExpect->top + TbarData->ItemHeight + 8;
            return 0;
        }

		case MSG_SIZECHANGED:
		{
			RECT* rcWin = (RECT*)wParam;
			RECT* rcClient = (RECT*)lParam;
			*rcClient = *rcWin;
			return 1;
		}

        case MSG_NCPAINT:
            return 0;

        case MSG_PAINT:
        {
            TbarData = (PCOOLBARCTRL) GetWindowAdditionalData2(hWnd);
            hdc = BeginPaint (hWnd);
            DrawCoolBox (hWnd, hdc, TbarData);
            EndPaint (hWnd, hdc);
            return 0;
        }

        case CBM_ADDITEM:
        {
            COOLBARITEMINFO* TbarInfo = NULL;
            COOLBARITEMDATA* ptemp;
            RECT rc;

            TbarData = (PCOOLBARCTRL) GetWindowAdditionalData2 (hWnd);
            TbarInfo = (COOLBARITEMINFO*) lParam;

            if (!(ptemp = (COOLBARITEMDATA*)calloc (1, sizeof (COOLBARITEMDATA)))) {
                return -1;
            }

            GetClientRect (hWnd, &rc);

            ptemp->id = TbarInfo->id;
            ptemp->Disable = 0;
            ptemp->ItemType = TbarInfo->ItemType;

            if (TbarInfo->ItemHint) {
                strncpy (ptemp->Hint, TbarInfo->ItemHint, LEN_HINT);
                ptemp->Hint [LEN_HINT] = '\0';
            }
            else
                ptemp->Hint [0] = '\0';

            if (TbarInfo->Caption) {
                strncpy (ptemp->Caption, TbarInfo->Caption, LEN_TITLE);
                ptemp->Caption [LEN_TITLE] = '\0';
            }
            else
                ptemp->Caption [0] = '\0';
             
            ptemp->Bmp = TbarInfo->Bmp;

            set_item_rect (hWnd, TbarData, ptemp); 

            ptemp->next = NULL;
            if (TbarData->nCount == 0) {
                TbarData->head = TbarData->tail = ptemp;
            }
            else if (TbarData->nCount > 0) { 
                TbarData->tail->next = ptemp;
                TbarData->tail = ptemp;
            }
            ptemp->insPos = TbarData->nCount;
            TbarData->nCount++;

            InvalidateRect (hWnd, NULL, TRUE);
            return 0;
        }

        case CBM_ENABLE:
            TbarData = (PCOOLBARCTRL)GetWindowAdditionalData2 (hWnd);
            if (enable_item (TbarData, wParam, lParam))
                return -1;

            InvalidateRect (hWnd, NULL, TRUE);
            return 0;

        case MSG_LBUTTONDOWN:
        {         
             int posx, posy;
             TbarData=(PCOOLBARCTRL) GetWindowAdditionalData2(hWnd);

             posx = LOSWORD (lParam);
             posy = HISWORD (lParam);

             if (GetCapture () == hWnd)
                break;
             
             SetCapture (hWnd);
                         
             if ((pTbid = GetCurTag (posx, posy, TbarData)) == NULL)
                break; 
             
             TbarData->iSel = pTbid->insPos;
             break;
        }

        case MSG_LBUTTONUP:
        { 
            int x, y;
            TbarData = (PCOOLBARCTRL)GetWindowAdditionalData2(hWnd);
            x = LOSWORD(lParam);
            y = HISWORD(lParam);
                      
            if (GetCapture() != hWnd)
               break;
            ReleaseCapture ();

            ScreenToClient (hWnd, &x, &y);

            if ((pTbid = GetCurTag(x, y, TbarData)) == NULL) {
                break;
            }
          
            if (TbarData->iSel == pTbid->insPos && !pTbid->Disable)
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), pTbid->id);

            TbarData->iSel = -1;
            break;
        }

        case MSG_MOUSEMOVE:
        {  
            int x, y;
            TbarData = (PCOOLBARCTRL) GetWindowAdditionalData2(hWnd);
            x = LOSWORD(lParam);
            y = HISWORD(lParam);

            if (GetCapture() == hWnd)
                ScreenToClient (hWnd, &x, &y);
                
            if ((pTbid = GetCurTag (x, y, TbarData)) == NULL) {
                unhilight (hWnd);
                break;
            }

            if (TbarData->iMvOver == pTbid->insPos)
               break;

            unhilight (hWnd);
            TbarData->iMvOver = pTbid->insPos;
            hilight (hWnd, TbarData, pTbid);
            break;
        }
        
        case MSG_MOUSEMOVEIN:
            if (!wParam)
                unhilight (hWnd);   
            break;

        case MSG_NCLBUTTONDOWN:
        case MSG_KILLFOCUS:
            unhilight (hWnd);   
            break;
    }

    return DefaultControlProc (hWnd, message, wParam, lParam);
}

#endif /* _MGCTRL_COOLBAR */

