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
** scrollbar.c: the scrollbar control.
**
** Create date: 2008/01/18
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGCTRL_SCROLLBAR
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/scrollbar.h"
#include "ctrl/ctrlhelper.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "ctrlmisc.h"

#ifdef WIN32
static __inline void DPRINTF(const char* fmt, ...)
{
}
#else
#define DPRINTF(...) 
#endif

static int get_shaft_len (HWND hwnd)
{
    int shaft_len;
    const WINDOWINFO* winfo = GetWindowInfo(hwnd);
    RECT rc;

    if (winfo->dwStyle & SBS_NOSHAFT)
    {
        shaft_len = 0;
        return 0;
    }

    GetClientRect (hwnd, &rc);

    DPRINTF ("shaft len, window rect (%i, %i, %i, %i)",
            rc.left, rc.top, rc.right, rc.bottom);

    if (winfo->dwStyle & SBS_HORZ){
        shaft_len = RECTW (rc);
        if (!(winfo->dwStyle & SBS_NOARROW))
        {
            DPRINTF ("arrow height len == %i", RECTH (rc));
            shaft_len -= (RECTH (rc) << 1);
        }
    }
    else {
        shaft_len = RECTH (rc);
        if (!(winfo->dwStyle & SBS_NOARROW))
        {
            DPRINTF ("arrow width len == %i", RECTW (rc));
            shaft_len -= (RECTW (rc) << 1);
        }
    }

    if (shaft_len < 0)
        shaft_len = 0;
    DPRINTF ("shaft len = %i\n", shaft_len);
    
    return shaft_len;
}

static void recalc_thumb_start (HWND hwnd, PSCROLLBARDATA data)
{
    int bar_start;
    div_t divt;
    int move_range;
    const WINDOWINFO* winfo = GetWindowInfo(hwnd);

    if (winfo->dwStyle & SBS_NOSHAFT)
    {
        bar_start = data->arrowLen;
    }
    else
    {
        if (data->minPos == data->maxPos)
        {
            bar_start = data->arrowLen; 
        }
        else
        {
            move_range = get_shaft_len (hwnd) - data->barLen;
            divt = div (move_range, data->maxPos - data->minPos);
            bar_start = (data->curPos - data->minPos) * divt.quot 
                + (data->curPos - data->minPos)* divt.rem 
                /(data->maxPos - data->minPos); 
            DPRINTF ("shaft_len [%i], move_range [%i]"
                    "arrowLen [%i], bar_start [%i], "
                    "bar_len [%i]\n",
                    get_shaft_len (hwnd), move_range, 
                    data->arrowLen, bar_start,
                    data->barLen);
        }
    }
    data->barStart = bar_start;
    
    return;
}

static void recalc_thumb_len (HWND hwnd, PSCROLLBARDATA data)
{
    int barlen;
    div_t divt;
    int shaft_len;
    const WINDOWINFO* winfo = GetWindowInfo(hwnd);

    if (winfo->dwStyle & SBS_NOSHAFT)
    {
        barlen = 0;
    }
    else if (winfo->dwStyle & SBS_FIXEDBARLEN)
    {
        barlen = data->arrowLen;
    }
    else
    {
        shaft_len = get_shaft_len(hwnd);
        divt = div (shaft_len, data->maxPos - data->minPos + 1);
        barlen = data->pageStep * divt.quot 
            + data->pageStep * divt.rem / (data->maxPos - data->minPos + 1);
    }

    data->barLen = barlen;
    
    return;
}

static int init_sbdata (HWND hwnd, PSCROLLBARDATA pdata, RECT * scRect)
{
    DWORD win_style = GetWindowStyle (hwnd);

    if (!GetWindowRect (hwnd, scRect))
    {
        _MG_PRINTF ("ScrollBar Rect error!\n");
        return -1;
    }

    if ((win_style & SBS_NOSHAFT) && (win_style & SBS_NOARROW))
    {
        _MG_PRINTF ("ScrollBar styles error!\n");
        return -1;
    }
    if (!(win_style & SBS_HORZ))
        win_style |= SBS_VERT;

    if (win_style & SBS_HORZ)
    {
        if (win_style & SBS_BOTTOMALIGN)
            scRect->top = scRect->bottom 
                //- GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR) - 1;
                - GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR);
        else if (win_style & SBS_TOPALIGN)
            scRect->bottom = scRect->top 
                //+ GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR) + 1;
                + GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR);
    }
    else
    {
        if (win_style & SBS_LEFTALIGN)
            scRect->right = scRect->left 
                //+ GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR) + 1;
                + GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR);
        else if (win_style & SBS_RIGHTALIGN)
            scRect->left = scRect->right 
                //- GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR) - 1;
                - GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR);
    }

    if (win_style & SBS_NOARROW)
        pdata->arrowLen = 0;
    else if (win_style & SBS_HORZ)
        pdata->arrowLen = RECTHP(scRect);
    else
        pdata->arrowLen = RECTWP(scRect);

    pdata->minPos    = 0;
    pdata->maxPos    = 10;
    pdata->curPos    = 0;
    pdata->pageStep  = 11;
    pdata->barStart  = (win_style & SBS_HORZ) ? scRect->left : scRect->top;
    pdata->barStart  += pdata->arrowLen;
    pdata->barLen    = 10;
    pdata->track_pos = 0;  
    //recalc_thumb_len (hwnd, pdata);
    
    if(win_style & WS_DISABLED)
        pdata->status = SBS_DISABLED_LTUP & SBS_DISABLED_BTDN;
    else
        pdata->status = 0;
    
    return 0; 
}

static int get_mouse_pos(HWND hwnd, PSCROLLBARDATA data, int x, int y)
{
    int mouse_pos = 0;
    RECT scRect;
    DWORD win_style = GetWindowStyle (hwnd);

    GetClientRect (hwnd, &scRect);

    if (win_style & SBS_HORZ)
    {
        if (x > 0 && x < data->arrowLen)
            mouse_pos = HT_SB_LEFTARROW;
        else if(x < RECTW(scRect) && x > RECTW(scRect) - data->arrowLen)
            mouse_pos = HT_SB_RIGHTARROW;
        else if(x > (data->barStart + data->arrowLen) 
                && x < (data->barStart + data->barLen + data->arrowLen))
            mouse_pos = HT_SB_HTHUMB;
        else if(x < (data->barStart + data->arrowLen) && x > data->arrowLen)
            mouse_pos = HT_SB_LEFTSPACE;
        else if(x < (RECTW(scRect) - data->arrowLen) 
                && x > (data->barStart + data->barLen + data->arrowLen))
            mouse_pos = HT_SB_RIGHTSPACE;
    }
    else
    {
        if (y > 0 && y < data->arrowLen)
            mouse_pos = HT_SB_UPARROW;
        else if(y < RECTH(scRect) && y > RECTH(scRect) - data->arrowLen)
            mouse_pos = HT_SB_DOWNARROW;
        else if(y > data->barStart + data->arrowLen 
                && y < data->barStart + data->barLen + data->arrowLen)
            mouse_pos = HT_SB_VTHUMB;
        else if(y < data->barStart + data->arrowLen && y > data->arrowLen)
            mouse_pos = HT_SB_UPSPACE;
        else if(y < RECTH(scRect) - data->arrowLen 
                && y > data->barStart + data->barLen + data->arrowLen)
            mouse_pos = HT_SB_DOWNSPACE;
    }
    
    return mouse_pos;
}

int track_thumb (HWND hwnd, PSCROLLBARDATA data, int x, int y)
{
    int barStart, curPos;  
    int thumb_move_range;
    int mouse_pos;
    const WINDOWINFO* winfo;
        
    if (!data)
    {
        _MG_PRINTF ("addtional data2 is NULL\n");
        return -1; 
    }

    if (data->status & SBS_PRESSED_THUMB)
    {
        winfo = GetWindowInfo(hwnd);
            
        thumb_move_range = get_shaft_len (hwnd) - data->barLen;
        if (thumb_move_range == 0)
            return -1;

        mouse_pos = (winfo->dwStyle & SBS_HORZ ? x : y );

        if ( ((winfo->dwStyle & SBS_DISABLED_LTUP) &&
              (mouse_pos < data->track_pos)) 
          || ((winfo->dwStyle & SBS_DISABLED_BTDN) && 
              (mouse_pos > data->track_pos)))
            return -1; 
            
        DPRINTF ("old track_pos [%i] mousePos [ ] barStart [%i] cusPos [%i]\n",
                data->track_pos, data->barStart, data->curPos);

        barStart = data->barStart + mouse_pos - data->track_pos;

        if (barStart <= 0)
        {
            barStart = 0;
            data->status |= SBS_DISABLED_LTUP;
        }
        else if (barStart >= thumb_move_range)
        {
            barStart = thumb_move_range;
            data->status |= SBS_DISABLED_BTDN;
        }
        else
        {
            data->status &= ~(SBS_DISABLED_LTUP | SBS_DISABLED_BTDN);
        }

        curPos = (data->maxPos - data->minPos) * barStart 
            / thumb_move_range + data->minPos;
            
        if (data->curPos <= data->minPos)
            data->curPos = data->minPos;

        if (data->curPos >= data->maxPos)
            data->curPos = data->maxPos;
            
        if (barStart == data->barStart)
        {   
            return -1; 
        }   
        else
        {
            data->track_pos += barStart - data->barStart;
            data->barStart = barStart;
            data->curPos = curPos;
        }   

        DPRINTF ("new track_pos [%i] mousePos [%i] barStart [%i] cusPos [%i]\n",
                data->track_pos, mouse_pos, data->barStart, data->curPos);
    }   
    return 0;
}

static LRESULT ScrollBarCtrlProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PSCROLLBARDATA data;
    PCONTROL pCtrl;

    switch (message) 
    {
        case MSG_CREATE:
            {
                RECT rc;
                /* create the main data construction for the control */
                if (!(data = (PSCROLLBARDATA) malloc (sizeof (SCROLLBARDATA)))) 
                {
                    _MG_PRINTF ("Create ScrollBar control failure!\n");
                    return -1;
                }
                memset (data, 0 , sizeof (SCROLLBARDATA));

                /* initialization */
                if(0 != init_sbdata(hwnd, data, &rc))
                {
                    free(data);
                    _MG_PRINTF ("Create ScrollBar control failure!\n");
                    return -1;
                }

                /* keep the data construction on the control add data*/
                SetWindowAdditionalData2 (hwnd, (DWORD)data);

                MoveWindow (hwnd, rc.left, rc.top, 
                        RECTW (rc), RECTH (rc), TRUE);
                break;
            }

        case MSG_DESTROY:
            {
                /* release the scrollbar data construction */
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);
                free(data);
                /** destry caret */
                DestroyCaret (hwnd);
                break;
            }

        case MSG_SIZECHANGED:
            {
                RECT * rc_expect;
                DWORD win_style;

		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);

                if (!data)
                {
                    _MG_PRINTF ("addition data2 is NULL \n");
                    return 1;
                }

                rc_expect = (PRECT)lParam;

                win_style = GetWindowStyle (hwnd);

                if (win_style & SBS_HORZ)
                {
                    if (win_style & SBS_BOTTOMALIGN)
                        rc_expect->top = rc_expect->bottom 
                            - GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR);
                    else if (win_style & SBS_TOPALIGN)
                        rc_expect->bottom = rc_expect->top 
                            + GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR);
                }
                else
                {
                    if (win_style & SBS_LEFTALIGN)
                        rc_expect->right = rc_expect->left 
                            + GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR) ;
                    else if (win_style & SBS_RIGHTALIGN)
                        rc_expect->left = rc_expect->right 
                            - GetWindowElementAttr (hwnd, WE_METRICS_SCROLLBAR) ;
                }

                if (win_style & SBS_NOARROW)
                    data->arrowLen = 0;
                else if (win_style & SBS_HORZ)
                    data->arrowLen = RECTHP(rc_expect);
                else
                    data->arrowLen = RECTWP(rc_expect);

                DPRINTF ("MSG_SIZECHANGED: rc_expect = "
                        "(%i, %i, %i, %i), old barLen = %i\n",
                        rc_expect->left, rc_expect->top,
                        rc_expect->right, rc_expect->bottom,
                        data->barLen);

                return 1;
            }

        case MSG_CSIZECHANGED:
            {
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);
                pCtrl = (PCONTROL)hwnd;
                pCtrl->left = pCtrl->cl;
                pCtrl->top = pCtrl->ct;
                pCtrl->right = pCtrl->cr;
                pCtrl->bottom = pCtrl->cb;
                recalc_thumb_len (hwnd, data);
                recalc_thumb_start (hwnd, data);
                break;
            }

        case MSG_MOUSEMOVE:
            {
                int mouse_pos = 0;
                int x_pos = LOSWORD (lParam);
                int y_pos = HISWORD (lParam);
                const WINDOWINFO* winfo = NULL;
                HDC hdc;
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);
                winfo = GetWindowInfo(hwnd);
                hdc = GetClientDC (hwnd);

                if (hwnd == GetCapture ())
                {
                    ScreenToClient (hwnd, &x_pos, &y_pos);
                }
                mouse_pos = get_mouse_pos (hwnd, data, x_pos, y_pos);

                /** tracking thumb */
                if (data->status & SBS_PRESSED_THUMB)
                {
                    if (0 != track_thumb (hwnd, data, x_pos, y_pos))
                    {
                        ReleaseDC (hdc);
                        return 0;
                    }

                    data->status &= ~SBS_HILITE_THUMB;

                    // DK: Make the background does't redraw.
                    InvalidateRect(hwnd, NULL, FALSE);

                    if (winfo->dwStyle & SBS_NOTNOTIFYPARENT)
                    {
                        SendNotifyMessage (GetParent (hwnd),
                            (winfo->dwStyle & SBS_HORZ) ?
                            MSG_HSCROLL : MSG_VSCROLL, 
                            SB_THUMBTRACK, data->curPos);
                    }
                    else
                    {
                        pCtrl = (PCONTROL) hwnd;
                        NotifyParent (hwnd, pCtrl->id, SB_THUMBTRACK);
                    }
                    ReleaseDC (hdc);
                    return 0;
                }

                /** draw hilite or normal thumb */
                /* Mouse cursor hover on thumb button.*/
                if (mouse_pos == HT_SB_HTHUMB || mouse_pos == HT_SB_VTHUMB)
                {
                    if (!(data->status & SBS_HILITE_THUMB))
                    {
                        /* Make thumb button hilite.*/
                        data->status |= SBS_HILITE_THUMB;
                        winfo->we_rdr->draw_scrollbar (hwnd, hdc, mouse_pos); 
                    }
                }
                else
                {
                    if (data->status & SBS_PRESSED_THUMB ||
                        data->status & SBS_HILITE_THUMB)
                    {
                        data->status &= ~(SBS_PRESSED_THUMB
                                | SBS_HILITE_THUMB);
                        winfo->we_rdr->draw_scrollbar (hwnd, hdc, 
                            (winfo->dwStyle & SBS_HORZ) ? 
                            HT_SB_HTHUMB : HT_SB_VTHUMB);
                    }
                }

                /** draw left or up arrow if it is not disabled state*/
                if (!(data->status & SBS_DISABLED_LTUP))
                {
                    if (mouse_pos == HT_SB_LEFTARROW ||
                        mouse_pos == HT_SB_UPARROW)
                    {
                        if (!(data->status & SBS_HILITE_LTUP))
                        {
                            data->status |= SBS_HILITE_LTUP;
                            winfo->we_rdr->draw_scrollbar (hwnd, hdc, mouse_pos); 
                        }
                    }
                    else
                    {
                        if (data->status & SBS_PRESSED_LTUP ||
                            data->status & SBS_HILITE_LTUP)
                        {
                            data->status &= ~(SBS_HILITE_LTUP 
                                    | SBS_HILITE_LTUP);
                            winfo->we_rdr->draw_scrollbar (hwnd, hdc, 
                                (winfo->dwStyle & SBS_HORZ) ? 
                                HT_SB_LEFTARROW : HT_SB_UPARROW);
                        } 
                    }
                }

                /** draw right or down arrow if it is not disabled state*/
                if (!(data->status & SBS_DISABLED_BTDN))
                {
                    if (mouse_pos == HT_SB_RIGHTARROW ||
                        mouse_pos == HT_SB_DOWNARROW)
                    {
                        if (!(data->status & SBS_HILITE_BTDN))
                        {
                            data->status |= SBS_HILITE_BTDN;
                            winfo->we_rdr->draw_scrollbar (hwnd, hdc, mouse_pos); 
                        }
                    }
                    else
                    {
                        if (data->status & SBS_PRESSED_BTDN ||
                            data->status & SBS_HILITE_BTDN)
                        {
                            data->status &= ~(SBS_HILITE_BTDN
                                    | SBS_HILITE_BTDN);

                            winfo->we_rdr->draw_scrollbar (hwnd, hdc, 
                                (winfo->dwStyle & SBS_HORZ) ? 
                                HT_SB_RIGHTARROW : HT_SB_DOWNARROW);
                        } 
                    }
                }
                ReleaseDC (hdc);
                return 0;
            }

        case MSG_MOUSEMOVEIN:
            {
                BOOL in_out = (BOOL)wParam;
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);
                if (!in_out)
                {
                    DPRINTF ("<<<<<<<<<<<<MOVEOUT<<<<<<<<<<<<<\n");
                
                    //if (!(data->status & SBS_PRESSED_THUMB))
                    {
                        ReleaseCapture ();
                    }

                    data->status &= ~(SBS_HILITE_LTUP 
                            | SBS_HILITE_BTDN  | SBS_HILITE_THUMB); 
/*                     GetWindowRect (hwnd, &rc);
 *                     GetClientRect (hwnd, &crc);
 *                     DPRINTF ("win rect(%i, %i, %i, %i)\n",
 *                             rc.left, rc.top, rc.right, rc.bottom);
 *                     DPRINTF ("client rect(%i, %i, %i, %i)\n",
 *                             crc.left, crc.top, crc.right, crc.bottom);
 */

                    // DK: Make the background does't redraw.
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                return 0;
            }

        case MSG_LBUTTONDOWN:
            {
                int mouse_pos; 
                int x_pos = LOSWORD(lParam); 
                int y_pos = HISWORD(lParam); 
                int notification_code = 0;
                const WINDOWINFO* winfo;
                int which_scroll;
                if (hwnd != GetCapture ())
                {
                    SetCapture (hwnd);
                }
               
                winfo = GetWindowInfo(hwnd);
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);

                mouse_pos = get_mouse_pos (hwnd, data, x_pos, y_pos);

                if (mouse_pos == HT_SB_LEFTARROW ||
                    mouse_pos == HT_SB_UPARROW)
                {
                    if ((data->status & SBS_DISABLED_LTUP))
                        return 0;

                    if (winfo->dwStyle & SBS_HORZ)
                        notification_code = SB_LINELEFT;
                    else
                        notification_code = SB_LINEUP;
                    data->status |= SBS_PRESSED_LTUP;
                }
                else if (mouse_pos == HT_SB_RIGHTARROW ||
                         mouse_pos == HT_SB_DOWNARROW)
                {
                    if ((data->status & SBS_DISABLED_BTDN))
                        return 0;

                    if (winfo->dwStyle & SBS_HORZ)
                        notification_code = SB_LINERIGHT;
                    else
                        notification_code = SB_LINEDOWN;
                    data->status |= SBS_PRESSED_BTDN;
                }
                else if (mouse_pos == HT_SB_HTHUMB ||
                         mouse_pos == HT_SB_VTHUMB )
                {
                    if (winfo->dwStyle & SBS_HORZ)
                    {
                        data->track_pos = x_pos;
                    }
                    else 
                    {
                        data->track_pos = y_pos;
                        DPRINTF ("track_pos ---- begin %i\n", y_pos);
                    }

                    if (data->pageStep < data->maxPos - data->minPos + 1)
                        data->status |= SBS_PRESSED_THUMB;
                }
                else if (mouse_pos == HT_SB_LEFTSPACE ||
                         mouse_pos == HT_SB_UPSPACE)
                {
                    if (data->status & SBS_DISABLED_LTUP)
                        return 0;

                    if (winfo->dwStyle & SBS_HORZ)
                        notification_code = SB_PAGELEFT;
                    else
                        notification_code = SB_PAGEUP;
                }   
                else if (mouse_pos == HT_SB_RIGHTSPACE ||
                         mouse_pos == HT_SB_DOWNSPACE)
                {
                    if (data->status & SBS_DISABLED_BTDN)
                        return 0;

                    if (winfo->dwStyle & SBS_HORZ)
                        notification_code = SB_PAGERIGHT;
                    else
                        notification_code = SB_PAGEDOWN;
                }
                else
                {
                    return 0;
                }

                if(notification_code != 0)
                {
                    which_scroll = (winfo->dwStyle & SBS_HORZ) ? 
                        MSG_HSCROLL : MSG_VSCROLL;

                    if (winfo->dwStyle & SBS_NOTNOTIFYPARENT)
                    {
                        SendNotifyMessage (GetParent(hwnd), which_scroll, 
                                notification_code, 0);

                        SetAutoRepeatMessage (GetParent(hwnd), which_scroll,
                                notification_code, 0);
                    }
                    else
                    {
                        pCtrl = (PCONTROL) hwnd;
                        NotifyParent (hwnd, pCtrl->id, notification_code);

                        SetAutoRepeatMessage (GetParent(hwnd), 
                                MSG_COMMAND,
                                (WPARAM) MAKELONG 
                                (pCtrl->id, notification_code),
                                (LPARAM)hwnd);
                    }
                }
                
                // DK: Make the background does't redraw.
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

        case MSG_LBUTTONUP:
            {
                int x_pos = LOSWORD (lParam);
                int y_pos = HISWORD (lParam);
                const WINDOWINFO* winfo = GetWindowInfo(hwnd);
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);

                ScreenToClient (hwnd, &x_pos, &y_pos);
                track_thumb (hwnd, data, x_pos, y_pos);

                SetAutoRepeatMessage (0,0,0,0);
                ReleaseCapture ();

                data->status &= ~SBS_PRESSED_LTUP;
                data->status &= ~SBS_PRESSED_BTDN;

                if (data->status & SBS_PRESSED_THUMB) 
                {
                    data->status &= ~SBS_PRESSED_THUMB;

                    if (winfo->dwStyle & SBS_NOTNOTIFYPARENT)
                    {
                        SendNotifyMessage (GetParent (hwnd), 
                                ((GetWindowInfo(hwnd))->dwStyle & SBS_HORZ) ?  
                                MSG_HSCROLL : MSG_VSCROLL,
                                SB_THUMBPOSITION, data->curPos);
                    }
                    else
                    {
                        pCtrl = (PCONTROL) hwnd;
                        NotifyParent (hwnd, 
                                pCtrl->id, SB_THUMBPOSITION);
                    }
                }

                // DK: Make the background does't redraw.
                InvalidateRect(hwnd, NULL, FALSE);

                return 0;
            }

        case MSG_KEYDOWN:
            {
                int scancode = (int)wParam;
                int notification_code = 0;
                const WINDOWINFO* winfo = GetWindowInfo(hwnd);
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);

                switch (scancode) 
                {
                    case SCANCODE_PAGEUP:
                        {
                            if(data->status & SBS_DISABLED_LTUP)
                                return 0;

                            if (winfo->dwStyle & SBS_HORZ)
                                notification_code = SB_PAGELEFT;
                            else
                                notification_code = SB_PAGEUP;
                            break;
                        }
                        
                    case SCANCODE_PAGEDOWN:
                        {
                            if(data->status & SBS_DISABLED_BTDN)
                                return 0;

                            if (winfo->dwStyle & SBS_HORZ)
                                notification_code = SB_PAGERIGHT;
                            else
                                notification_code = SB_PAGEDOWN;
                            break;
                        }

                    case SCANCODE_CURSORBLOCKUP: 
                        {
                            if ((winfo->dwStyle & SBS_HORZ) 
                                    || (data->status & SBS_DISABLED_LTUP))
                                return 0;
                            notification_code = SB_LINEUP;
                            break;
                        }

                    case SCANCODE_CURSORBLOCKLEFT:
                        {
                            if (winfo->dwStyle & SBS_VERT
                                    || (data->status & SBS_DISABLED_LTUP))
                                return 0;
                            notification_code = SB_LINELEFT;
                            break;
                        }

                    case SCANCODE_CURSORBLOCKDOWN:
                        {
                            if (winfo->dwStyle & SBS_HORZ
                                    || (data->status & SBS_DISABLED_BTDN))
                                return 0;
                            notification_code = SB_LINEDOWN;
                            break;
                        }

                    case SCANCODE_CURSORBLOCKRIGHT:
                        {
                            if (winfo->dwStyle & SBS_VERT
                                    || (data->status & SBS_DISABLED_BTDN))
                                return 0;
                            notification_code = SB_LINERIGHT;
                            break;
                        }

                    case SCANCODE_HOME:
                        {
                            if (data->curPos == data->minPos)
                                return 0;

                            data->curPos = data->minPos;
                            data->status |= SBS_DISABLED_LTUP;

                            if (data->curPos == data->maxPos)
                                data->status |= SBS_DISABLED_BTDN;
                            else
                                data->status &= ~SBS_DISABLED_BTDN;

                            recalc_thumb_start (hwnd, data);
                            notification_code = SB_TOP;
                            break;
                        }

                    case SCANCODE_END:
                        {
                            if (data->curPos == data->maxPos)
                                return 0;

                            data->curPos = data->maxPos;
                            data->status |= SBS_DISABLED_BTDN;

                            if (data->curPos == data->minPos)
                                data->status |= SBS_DISABLED_LTUP;
                            else
                                data->status &= ~SBS_DISABLED_LTUP;

                            recalc_thumb_start (hwnd, data);
                            notification_code = SB_BOTTOM;
                            break;
                        }

                    default:
                        return 0;
                }

                if(notification_code != 0)
                {
                    if (winfo->dwStyle & SBS_NOTNOTIFYPARENT)
                    {
                        SendNotifyMessage (GetParent(hwnd), 
                                (winfo->dwStyle & SBS_HORZ) ? 
                                MSG_HSCROLL : MSG_VSCROLL, 
                                notification_code, 0);
                    }
                    else
                    {
                        pCtrl = (PCONTROL) hwnd;
                        NotifyParent (hwnd, 
                                pCtrl->id, notification_code);
                    }
                    // DK: Make the background does't redraw.
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                return 0;
            }

        case MSG_SETFOCUS:
            {
                CreateCaret (hwnd, 0, 0, 0);
                break;
            }

        case MSG_KILLFOCUS:
            {
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);
                data->status &= ~SBS_HILITE_THUMB;
                // DK: Make the background does't redraw.
                InvalidateRect(hwnd, NULL, FALSE);
                DestroyCaret (hwnd);
                break;
            }

        case MSG_CARETBLINK:
            {
                PCARETINFO caret;
                const WINDOWINFO* winfo;
                int status;

                pCtrl = (PCONTROL)hwnd;
                caret = pCtrl->pCaretInfo;
                winfo = GetWindowInfo(hwnd);
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);
                status = SBS_HILITE_THUMB;

                if (!data || !caret)
                    return -1; 

                if (!caret->fShow) 
                {
                    caret->fShow = TRUE;
                    if (winfo->dwStyle & SBS_HORZ)
                    {
                        if (!(data->status & status))
                        {
                            data->status |= status;
                            // DK: Make the background does't redraw.
                            InvalidateRect(hwnd, NULL, FALSE);
                        }
                    }
                    else
                    {
                        if (!(data->status & status))
                        {
                            data->status |= status;
                            // DK: Make the background does't redraw.
                            InvalidateRect(hwnd, NULL, FALSE);
                        }
                    }
                }
                else
                {
                    caret->fShow = FALSE;
                    if (winfo->dwStyle & SBS_HORZ)
                    {
                        if (data->status & status)
                        {
                            data->status &= ~status;
                            // DK: Make the background does't redraw.
                            InvalidateRect(hwnd, NULL, FALSE);
                        }
                    }
                    else
                    {
                        if (data->status & status)
                        {
                            data->status &= ~status;
                            // DK: Make the background does't redraw.
                            InvalidateRect(hwnd, NULL, FALSE);
                        }
                    }
                }
                return 0;
            }

        case MSG_PAINT:
            {
                const WINDOWINFO *winfo = GetWindowInfo(hwnd);
                HDC hdc = BeginPaint(hwnd);
                
                winfo->we_rdr->draw_scrollbar(hwnd, hdc, 
                        (winfo->dwStyle & SBS_HORZ) ? 
                        HT_HSCROLL : HT_VSCROLL);

                EndPaint(hwnd, hdc);
                return 0;
            }

        case SBM_GETPOS:
            {
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);
                return data->curPos;
            }

        case SBM_SETPOS:
            {
                int pos = (int)wParam;
                BOOL redraw = (BOOL)lParam;
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);

                if (pos == data->curPos)
                    return 0;

                if(pos < data->minPos) 
                {
                    pos = data->minPos;
                } 

                if (pos > data->maxPos)
                {
                    pos = data->maxPos;
                }

                data->curPos = pos;

                if (data->curPos == data->minPos)
                {
                    data->status |= SBS_DISABLED_LTUP;
                    SetAutoRepeatMessage (0,0,0,0);
                }
                else
                    data->status &= ~SBS_DISABLED_LTUP;

                if (data->curPos == data->maxPos)
                {
                    data->status |= SBS_DISABLED_BTDN;
                    SetAutoRepeatMessage (0,0,0,0);
                }
                else
                    data->status &= ~SBS_DISABLED_BTDN;

                recalc_thumb_start (hwnd, data);

                if(redraw)
                {
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                return 0;
            }

        case SBM_GETSCROLLINFO:
            {
                PSCROLLINFO info = (PSCROLLINFO) wParam;
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);

                if (info->fMask & SIF_RANGE)
                {
                    info->nMin = data->minPos;
                    info->nMax = data->maxPos;
                }
                if (info->fMask & SIF_PAGE)
                    info->nPage = data->pageStep;
                if (info->fMask & SIF_POS)
                    info->nPos = data->curPos;
                
                return 0;
            }

        case SBM_SETSCROLLINFO:
            {
                PSCROLLINFO info = (PSCROLLINFO) wParam;
                BOOL redraw = (BOOL)lParam;
                int new_nPos, new_nMin, new_nMax;
                UINT new_nPage;
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);

                new_nPos  = data->curPos;
                new_nMin  = data->minPos;
                new_nMax  = data->maxPos;
                new_nPage = data->pageStep;

                if (info->fMask & SIF_RANGE)
                {
                    if (info->nMax < info->nMin)
                    {
                        _MG_PRINTF ( "Error range\n");
                        return -1;
                    }
                    new_nMin = info->nMin;
                    new_nMax = info->nMax;
                }

                if (info->fMask & SIF_PAGE)
                {
                    if (info->nPage > new_nMax - new_nMin + 1
                        || info->nPage <= 0)
                    {
                        _MG_PRINTF ( "Error page step\n");
                        return -1;
                    }
                    new_nPage = info->nPage;
                }
                else
                {
                    if (data->pageStep > new_nMax - new_nMin + 1
                        || data->pageStep <= 0)
                    {
                        return -1;
                    }
                }

                if (info->fMask & SIF_POS)
                {
                    if (info->nPos < new_nMin ||
                        info->nPos > new_nMax)
                    {
                        _MG_PRINTF ( "Error postion to set");
                        return -1;
                    }
                    new_nPos = info->nPos;
                }
                else
                {
                    if (data->curPos < new_nMin ||
                        data->curPos > new_nMax)
                    {
                        return -1;
                    }
                }

                data->minPos   = new_nMin;
                data->maxPos   = new_nMax;
                data->pageStep = new_nPage;
                data->curPos   = new_nPos;

                if (data->curPos == data->minPos)
                    data->status |= SBS_DISABLED_LTUP;
                else
                    data->status &= ~SBS_DISABLED_LTUP;

                if (data->curPos == data->maxPos)
                    data->status |= SBS_DISABLED_BTDN;
                else
                    data->status &= ~SBS_DISABLED_BTDN;

                recalc_thumb_len (hwnd, data);
                recalc_thumb_start (hwnd, data);

                if(redraw)
                {
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                return 0;
            }

        case SBM_GETRANGE:
            {
                int *min = (int *)wParam;
                int *max = (int *)lParam;
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);
                
                *min = data->minPos;
                *max = data->maxPos;
                
                return 0;
            }

        case SBM_SETRANGE:
            {
                int min = (int) wParam;
                int max = (int) lParam;
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);
                
                if(max < data->curPos || min > data->curPos)
                {
                    _MG_PRINTF ( "Error range to set\n");
                    return -1;
                } 
                data->minPos = min;
                data->maxPos = max;

                if (data->curPos == data->minPos)
                    data->status |= SBS_DISABLED_LTUP;
                else
                    data->status &= ~SBS_DISABLED_LTUP;

                if (data->curPos == data->maxPos)
                    data->status |= SBS_DISABLED_BTDN;
                else
                    data->status &= ~SBS_DISABLED_BTDN;
                
                recalc_thumb_len (hwnd, data);
                recalc_thumb_start (hwnd, data);
                return 0;
            }

        case SBM_SETRANGEREDRAW:
            {
                int min = (int) wParam;
                int max = (int) lParam;
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);
                
                if(max < data->curPos || min > data->curPos)
                {
                    _MG_PRINTF ( "Error range to set\n");
                    return -1;
                } 
                data->minPos = min;
                data->maxPos = max;
                    
                if (data->curPos == data->minPos)
                    data->status |= SBS_DISABLED_LTUP;
                else
                    data->status &= ~SBS_DISABLED_LTUP;

                if (data->curPos == data->maxPos)
                    data->status |= SBS_DISABLED_BTDN;
                else
                    data->status &= ~SBS_DISABLED_BTDN;
                
                recalc_thumb_len (hwnd, data);
                recalc_thumb_start (hwnd, data);
                // DK: Make the background does't redraw.
                InvalidateRect(hwnd, NULL, FALSE);
                
                return 0;
            }

        case SBM_ENABLE_ARROW:
            {
                int which = (int)wParam;
                BOOL enable = (BOOL)lParam;
		        data = (PSCROLLBARDATA) GetWindowAdditionalData2 (hwnd);

                if (which & SB_ARROW_LTUP)
                {
                    if(enable)
                        data->status &= ~SBS_DISABLED_LTUP;
                    else
                        data->status |= SBS_DISABLED_LTUP;
                }
                if (which & SB_ARROW_BTDN)
                {
                    if(enable)
                        data->status &= ~SBS_DISABLED_BTDN;
                    else
                        data->status |= SBS_DISABLED_BTDN;
                }

                return 0;
            }

        default:
            break;
    }
    return DefaultControlProc (hwnd, message, wParam, lParam);
}

BOOL RegisterScrollBarControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_SCROLLBAR;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = 
        GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = ScrollBarCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#endif/* _MGCTRL_SCROLLBAR */

