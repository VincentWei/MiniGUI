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
** trackbar.c: the TrackBar Control module.
**
** Create date: 2000/12/02
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGCTRL_TRACKBAR
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/trackbar.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "ctrlmisc.h"
#include "trackbar_impl.h"

#define GAP_TIP_SLIDER          12
static void TrackBarOnDraw (HWND hwnd, HDC hdc, TRACKBARDATA* pData, DWORD dwStyle)
{
    const WINDOW_ELEMENT_RENDERER* win_rdr;
    RECT    rc_client;

    win_rdr = GetWindowInfo(hwnd)->we_rdr;
    if (!win_rdr) {
        _MG_PRINTF ("CONTROL>TrackBar: window renderer is NULL.\n");
        return;
    }
    
    GetClientRect (hwnd, &rc_client);

    win_rdr->draw_trackbar (hwnd, hdc, (LFRDR_TRACKBARINFO *)pData); 

    /* draw the tip of trackbar. */
    if ((dwStyle & TBS_TIP) && !(dwStyle & TBS_VERTICAL)) {
        SIZE    text_ext;
        char    sPos[10];
        RECT    rc_bar, rc_border;
        int sliderh, EndTipLen, x, y, w, h;

        win_rdr->calc_trackbar_rect (hwnd, (LFRDR_TRACKBARINFO *)pData, 
                dwStyle, &rc_client, NULL, &rc_bar, &rc_border);
        sliderh = RECTH (rc_bar);

        x = rc_border.left;
        y = rc_border.top;
        w = RECTW (rc_border);
        h = RECTH (rc_border);

        SelectFont (hdc, (PLOGFONT)GetWindowElementAttr 
                (hwnd, WE_FONT_TOOLTIP));

        SetBkMode (hdc, BM_TRANSPARENT);
        SetBkColor (hdc, GetWindowBkColor (hwnd));
        SetTextColor (hdc, GetWindowElementPixel (hwnd, WE_FGC_THREED_BODY));

        TextOut (hdc, x + 1, y + (h>>1) - (sliderh>>1) - GAP_TIP_SLIDER, 
                            pData->sStartTip);

        GetTextExtent (hdc, pData->sEndTip, -1, &text_ext);
        EndTipLen = text_ext.cx + 4;
        TextOut (hdc, (EndTipLen > (w>>1) - 20 ? x + (w>>1) + 20 : x + w -EndTipLen), 
                        y + (h>>1) - (sliderh>>1) - GAP_TIP_SLIDER, pData->sEndTip); 
        sprintf (sPos, "%d", pData->nPos);
        GetTextExtent (hdc, sPos, -1, &text_ext);
        TextOut (hdc, x + ((w - text_ext.cx) >> 1), 
                        y + (h>>1) -(sliderh>>1) - GAP_TIP_SLIDER, sPos);
    }
}

static void TrackBarNormalizeParams (const CONTROL* pCtrl, TRACKBARDATA* pData, BOOL fNotify)
{
    if (pData->nPos >= pData->nMax) {
        if (fNotify) {
            NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_CHANGE);
            NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_REACHMAX);
        }
        pData->nPos = pData->nMax;
    }
    else if (pData->nPos <= pData->nMin) {
        if (fNotify) {
            NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_CHANGE);
            NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_REACHMIN);
        }
        pData->nPos = pData->nMin;
    }
    else if (pData->nPos < pData->nMax && pData->nPos > pData->nMin) {
        if (fNotify)
            NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_CHANGE);
    }
}

static void SetSliderPos (const CONTROL* pCtrl, int new_pos)
{
    TRACKBARDATA*   pData = (TRACKBARDATA *)pCtrl->dwAddData2;
    RECT            rc_client, old_slider, new_slider;

    if (new_pos > pData->nMax)
        new_pos = pData->nMax; 

    if (new_pos < pData->nMin)
        new_pos = pData->nMin; 

    if (pData->nPos == new_pos) 
        return;

    GetClientRect ((HWND)pCtrl, &rc_client);

    pCtrl->we_rdr->calc_trackbar_rect ((HWND)pCtrl, (LFRDR_TRACKBARINFO *)pData, 
                        pCtrl->dwStyle, &rc_client, NULL, &old_slider, NULL);

    pData->nPos = new_pos;
    TrackBarNormalizeParams (pCtrl, pData, pCtrl->dwStyle & TBS_NOTIFY);

    pCtrl->we_rdr->calc_trackbar_rect ((HWND)pCtrl, (LFRDR_TRACKBARINFO *)pData, 
                        pCtrl->dwStyle, &rc_client, NULL, &new_slider, NULL);

    if (pCtrl->dwStyle & TBS_TIP) {
        InvalidateRect ((HWND)pCtrl, NULL, TRUE);
    }
    else if (!EqualRect (&old_slider, &new_slider)) {
       InvalidateRect ((HWND)pCtrl, &old_slider, TRUE);
       InvalidateRect ((HWND)pCtrl, &new_slider, FALSE);
    }

}

static int NormalizeMousePos (HWND hwnd, TRACKBARDATA* pData, int mousepos)
{
    RECT    rcClient, rcBar;
    int     h;
    int     len, pos;
    const   WINDOW_ELEMENT_RENDERER* win_rdr;
    DWORD   dwStyle;

    GetClientRect (hwnd, &rcClient);
    h = RECTH (rcClient);

    dwStyle = GetWindowStyle (hwnd);
    win_rdr = GetWindowInfo(hwnd)->we_rdr;

    win_rdr->calc_trackbar_rect (hwnd, (LFRDR_TRACKBARINFO *)pData, 
                        dwStyle, &rcClient, NULL, &rcBar, NULL);

    if (dwStyle & TBS_VERTICAL) {
        int blank =  RECTH (rcBar)>>1;
        len = h - (blank<<1);
        if (mousepos > rcClient.bottom - blank)
            pos = 0;
        else if (mousepos < blank)
            pos = len;
        else
            pos = h - blank - mousepos;
    } else {
        int blank =  RECTW (rcBar)>>1;

        len = RECTW (rcClient) - (blank<<1);
        if (mousepos < blank)
            pos = 0;
        else if (mousepos > rcClient.right - blank)
            pos = len ;
        else
            pos = mousepos - blank;
    }

    return (int)((pData->nMax - pData->nMin) * pos / (float)len + 0.5) + pData->nMin;
}

static LRESULT TrackBarCtrlProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PCONTROL      pCtrl;
    TRACKBARDATA* pData;
    pCtrl = gui_Control (hwnd);
    
    switch (message)
    {
        case MSG_CREATE:
            if (!(pData = malloc (sizeof (TRACKBARDATA)))) {
                _MG_PRINTF ("CONTROL>TrackBar: Create control failure!\n");
                return -1;
            }
            pData->nMax = 10;
            pData->nMin = 0;
            pData->nPos = 0;
            pData->nLineSize = 1;
            pData->nPageSize = 5;
            strcpy (pData->sStartTip, "Start");
            strcpy (pData->sEndTip, "End");
            pData->nTickFreq = 1;
            pCtrl->dwAddData2 = (DWORD)pData;
        break;
    
        case MSG_DESTROY:
            free((void *)(pCtrl->dwAddData2));
        break;

        case MSG_NCPAINT:
            return 0;
       
        case MSG_GETTEXTLENGTH:
        case MSG_GETTEXT:
        case MSG_SETTEXT:
            return -1;

        case MSG_PAINT:
        {
            HDC hdc;

            hdc = BeginPaint (hwnd);
            TrackBarOnDraw (hwnd, hdc, (TRACKBARDATA *)pCtrl->dwAddData2, pCtrl->dwStyle);
            EndPaint (hwnd, hdc);
            return 0;
        }


        case TBM_SETRANGE:
        {
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;

            if (wParam == lParam)
                return -1;

            pData->nMin = MIN ((int)wParam, (int)lParam);
            pData->nMax = MAX ((int)wParam, (int)lParam);

            if (pData->nPos > pData->nMax)
                pData->nPos = pData->nMax;
            if (pData->nPos < pData->nMin)
                pData->nPos = pData->nMin;

            SendMessage (hwnd, TBM_SETPOS, pData->nPos, 0);
            return 0;
        }
        
        case TBM_GETMIN:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nMin;
     
        case TBM_GETMAX:    
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nMax;
    
        case TBM_SETMIN:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;

            if (wParam == pData->nMin || wParam >= pData->nMax)
                return -1;

            pData->nMin = wParam;
            if (pData->nPos < pData->nMin)
                pData->nPos = pData->nMin;
            SendMessage (hwnd, TBM_SETPOS, pData->nPos, 0);
            return 0;
    
        case TBM_SETMAX:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;

            if ((int)wParam == pData->nMax || (int)wParam <= pData->nMin)
                return -1;

            pData->nMax = wParam;
            if (pData->nPos > pData->nMax)
                pData->nPos = pData->nMax;
            SendMessage (hwnd, TBM_SETPOS, pData->nPos, 0);
            return 0;
        
        case TBM_SETLINESIZE:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            if (wParam > (pData->nMax - pData->nMin))
                return -1;
            pData->nLineSize = wParam;
            return 0;

        case TBM_GETLINESIZE:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nLineSize;
        
        case TBM_SETPAGESIZE:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            if (wParam > (pData->nMax - pData->nMin))
                return -1;
            pData->nPageSize = wParam;
            return 0;
        
        case TBM_GETPAGESIZE:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nPageSize;
    
        case TBM_SETPOS:
            SetSliderPos (pCtrl, wParam);
            return 0;
        
        case TBM_GETPOS:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nPos;
        
        case TBM_SETTICKFREQ:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            if (wParam > (pData->nMax - pData->nMin))
                wParam = pData->nMax - pData->nMin;
            pData->nTickFreq = wParam > 1 ? wParam : 1;
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;

        case TBM_GETTICKFREQ:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            return pData->nTickFreq;
    
        case TBM_SETTIP:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            if (wParam) 
                strncpy(pData->sStartTip, (char *) wParam, TBLEN_TIP);
            if (lParam)
                strncpy (pData->sEndTip, (char *) lParam, TBLEN_TIP);
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;

        case TBM_GETTIP:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;
            if (wParam)
                strcpy ((char *) wParam, pData->sStartTip);
            if (lParam)
                strcpy ((char *) lParam, pData->sEndTip);
            return 0;
        
        case MSG_SETFOCUS:
            if (pCtrl->dwStyle & TBS_FOCUS)
                break;
            pCtrl->dwStyle |= TBS_FOCUS;
            InvalidateRect (hwnd, NULL, TRUE);
            break;
    
        case MSG_KILLFOCUS:
            pCtrl->dwStyle &= ~TBS_FOCUS;
            InvalidateRect (hwnd, NULL, TRUE);
            break;
    
        case MSG_GETDLGCODE:
            return DLGC_WANTARROWS;

        case MSG_ENABLE:
            if (wParam && (pCtrl->dwStyle & WS_DISABLED))
                pCtrl->dwStyle &= ~WS_DISABLED;
            else if (!wParam && !(pCtrl->dwStyle & WS_DISABLED))
                pCtrl->dwStyle |= WS_DISABLED;
            else
                return 0;
            InvalidateRect (hwnd, NULL, TRUE);

            return 0;

        case MSG_KEYDOWN:
            pData = (TRACKBARDATA *)pCtrl->dwAddData2;

            if (pCtrl->dwStyle & WS_DISABLED)
                return 0;

            switch (LOWORD (wParam)) {
                case SCANCODE_CURSORBLOCKUP:
                case SCANCODE_CURSORBLOCKRIGHT:
                    SetSliderPos (pCtrl, pData->nPos + pData->nLineSize);
                break;

                case SCANCODE_CURSORBLOCKDOWN:
                case SCANCODE_CURSORBLOCKLEFT:
                    SetSliderPos (pCtrl, pData->nPos - pData->nLineSize);
                break;
            
                case SCANCODE_PAGEDOWN:
                    SetSliderPos (pCtrl, pData->nPos - pData->nPageSize);
                break;
            
                case SCANCODE_PAGEUP:
                    SetSliderPos (pCtrl, pData->nPos + pData->nPageSize);
                break;
            
                case SCANCODE_HOME:
                    pData->nPos = pData->nMin;
                    TrackBarNormalizeParams (pCtrl, pData, pCtrl->dwStyle & TBS_NOTIFY);
                    InvalidateRect (hwnd, NULL, TRUE);
                break;
            
                case SCANCODE_END:
                    pData->nPos = pData->nMax;
                    TrackBarNormalizeParams (pCtrl, pData, pCtrl->dwStyle & TBS_NOTIFY);
                    InvalidateRect (hwnd, NULL, TRUE);
                break;
            }
        break;

        case MSG_LBUTTONDOWN:
            if (GetCapture() != hwnd) {
                int mouseX = LOSWORD(lParam);
                int mouseY = HISWORD(lParam);
                
                /** set state to press slider */        
                pCtrl->dwStyle |=  LFRDR_TBS_PRESSED; 

                SetCapture (hwnd);

                NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_STARTDRAG);

                pData = (TRACKBARDATA *)pCtrl->dwAddData2;

                /** redraw slider, press it */
                SetSliderPos (pCtrl, NormalizeMousePos (hwnd, pData,
                            (pCtrl->dwStyle & TBS_VERTICAL)?mouseY:mouseX));
            }
            break;
                
        case MSG_MOUSEMOVE:
        {
            /** hilite slider */

            RECT rc_client;
            RECT rc_slider;
            int mouseX = LOSWORD(lParam);
            int mouseY = HISWORD(lParam);

            pData = (TRACKBARDATA *)pCtrl->dwAddData2;

            GetClientRect (hwnd, &rc_client);
            pCtrl->we_rdr->calc_trackbar_rect (hwnd, (LFRDR_TRACKBARINFO *)pData, 
                                pCtrl->dwStyle, &rc_client, NULL, &rc_slider, NULL);

            if (GetCapture() == hwnd)
            {
                ScreenToClient (hwnd, &mouseX, &mouseY);
                if (PtInRect (&rc_slider, mouseX, mouseY))
                    pCtrl->dwStyle |= LFRDR_TBS_HILITE; 
                else 
                    pCtrl->dwStyle &= ~LFRDR_TBS_HILITE; 
            }
            else
            {
                if (PtInRect (&rc_slider, mouseX, mouseY))
                {
                    /** avoid twinkle by valid redraw */
                    if (!(pCtrl->dwStyle & LFRDR_TBS_HILITE))
                    {
                        pCtrl->dwStyle |= LFRDR_TBS_HILITE; 
                        InvalidateRect (hwnd, NULL, TRUE);
                    }
                }
                else 
                {
                    /** avoid twinkle by valid redraw */
                    if (pCtrl->dwStyle & LFRDR_TBS_HILITE)
                    {
                        pCtrl->dwStyle &= ~LFRDR_TBS_HILITE; 
                        InvalidateRect (hwnd, NULL, TRUE);
                    }
                }
                break;
            }

            SetSliderPos (pCtrl, NormalizeMousePos (hwnd, pData,
                            (pCtrl->dwStyle & TBS_VERTICAL)?mouseY:mouseX));
        }
        break;

        case MSG_LBUTTONUP:
            if (GetCapture() == hwnd) {

                /** set state to press slider */        
                pCtrl->dwStyle &= ~LFRDR_TBS_PRESSED; 
                ReleaseCapture ();
                NotifyParent ((HWND)pCtrl, pCtrl->id, TBN_STOPDRAG);

                /** redraw slider, unpress it */
                InvalidateRect (hwnd, NULL, TRUE);
            }
            break;
    
        case MSG_FONTCHANGED:
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;

        default:
            break;    
    }
    
    return DefaultControlProc (hwnd, message, wParam, lParam);
}

BOOL RegisterTrackBarControl (void)
{
    WNDCLASS WndClass;
    WndClass.spClassName = "trackbar";
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_DESKTOP, WE_MAINC_THREED_BODY);
    WndClass.WinProc     = TrackBarCtrlProc;
    return AddNewControlClass (&WndClass) == ERR_OK;
}
#endif /* _MGCTRL_TRACKBAR */

