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
** cobmobox.c: the cobmobox control.
**
** Created by Jiao Libo at 2001/08/06
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGCTRL_COMBOBOX
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/edit.h"
#include "ctrl/listbox.h"
#include "ctrl/combobox.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "combobox_impl.h"
#include "ctrlmisc.h"

#define  LEN_SPINVALUE          50

#define  IDC_CEDIT              100
#define  IDC_CLISTBOX           101    

#define  INTER_EDIT_BUTTON      2 
#define  INTER_EDIT_LISTBOX     4 
#define  DEF_LISTHEIGHT         60

#define INC_BOX 1
#define DEC_BOX 2
static int mouse_old = 0;

static LRESULT ComboBoxCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL RegisterComboBoxControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_COMBOBOX;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = 
        GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = ComboBoxCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

static LRESULT AutoSpinEditBoxProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WNDPROC old_edit_proc;

    if (message == MSG_CHAR) {
        if (wParam != 127 && (wParam < '0' || wParam > '9'))
            return 0;
    }

    old_edit_proc = (WNDPROC) GetWindowAdditionalData (hWnd);
    return (*old_edit_proc) (hWnd, message, wParam, lParam);
}

static int DefCBProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

static void ComboBoxDrawSpinButton (HWND hwnd, HDC hdc , int which, int status)
{
    PCOMBOBOXDATA pData;
    PCONTROL pCtrl;
    const WINDOWINFO *win_info;
    DWORD fg_3d;
	BOOL  bGetDC = FALSE;

	if (hdc == 0) {
		bGetDC = TRUE;
		hdc = GetClientDC(hwnd);
	}
 
    pCtrl = gui_Control (hwnd); 
    win_info = GetWindowInfo(hwnd);
    pData = (PCOMBOBOXDATA)pCtrl->dwAddData2;

    /* not used var
    DWORD main_3d;
    main_3d = GetWindowElementAttr(hwnd, WE_MAINC_THREED_BODY);
    */
    fg_3d = GetWindowElementAttr(hwnd, WE_FGC_THREED_BODY);

    if(which == 0)
        status = LFRDR_BTN_STATUS_NORMAL;

    switch (pCtrl->dwStyle & CBS_TYPEMASK) { 
        case CBS_SIMPLE:
            break;
        case CBS_SPINLIST:
        case CBS_AUTOSPIN:
            if (pCtrl->dwStyle & CBS_SPINARROW_LEFTRIGHT) 
            {
                if(which == 0 || which == INC_BOX)
                {
                    SelectClipRect (hdc, &(pData->IncSpinBox));
                    win_info->we_rdr->draw_arrow(hwnd, hdc, &(pData->IncSpinBox), 
                            fg_3d, status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_RIGHT);
                }
                if(which == 0 || which == DEC_BOX)
                {
                    SelectClipRect (hdc, &(pData->DecSpinBox));
                    win_info->we_rdr->draw_arrow(hwnd, hdc, &(pData->DecSpinBox), 
                            fg_3d, status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_LEFT);
                }
            }
            else 
            {
                if(which == 0 || which == INC_BOX)
                {
                    SelectClipRect (hdc, &(pData->IncSpinBox));
                    win_info->we_rdr->draw_arrow(hwnd, hdc, &(pData->IncSpinBox), 
                            fg_3d, status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_UP);
                }
                if(which == 0 || which == DEC_BOX)
                {
                    SelectClipRect (hdc, &(pData->DecSpinBox));
                    win_info->we_rdr->draw_arrow(hwnd, hdc, &(pData->DecSpinBox), 
                            fg_3d, status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_DOWN);
                }
            }
            break;
        
        default:
            SelectClipRect (hdc, &(pData->IncSpinBox));
            win_info->we_rdr->draw_arrow(hwnd, hdc, &(pData->IncSpinBox), fg_3d,
                            status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_DOWN);
            break;
    }
	
	if(bGetDC)
	{
		ReleaseDC(hdc);
	}
    return ;
}

static void GetSpinBox (PCOMBOBOXDATA pData, DWORD dwStyle, 
                            const RECT* rect, RECT* edit_box)
{
    int w, h;
    if (dwStyle & CBS_SPINARROW_TOPBOTTOM) 
    {
        w = (rect->right - rect->left) > 16 ? 16 : (rect->right - rect->left);
        h = (rect->bottom - rect->top)>>1;
        h = h > (w>>1)? (w>>1) : h;

        edit_box->left = rect->left;
        edit_box->top = rect->top + h + INTER_EDIT_BUTTON;
        edit_box->right = rect->right;
        edit_box->bottom = rect->bottom - h - INTER_EDIT_BUTTON;

        pData->IncSpinBox.left = (rect->right - w)>>1;
        pData->IncSpinBox.right = pData->IncSpinBox.left + w;
        pData->IncSpinBox.top = rect->top;
        pData->IncSpinBox.bottom = rect->top + h;

        pData->DecSpinBox.left = pData->IncSpinBox.left;
        pData->DecSpinBox.right = pData->IncSpinBox.right;
        pData->DecSpinBox.top = rect->bottom - h;
        pData->DecSpinBox.bottom = rect->bottom;
    }
    else if (dwStyle & CBS_SPINARROW_LEFTRIGHT) 
    {
        h = rect->bottom - rect->top;
        w = h < 16 ? h : 16;

        edit_box->left = rect->left + (w>>1) + INTER_EDIT_BUTTON;
        edit_box->top = rect->top;
        edit_box->bottom = rect->bottom;
        edit_box->right = rect->right - (w>>1) - INTER_EDIT_BUTTON;

        pData->DecSpinBox.left = rect->left;
        pData->DecSpinBox.top = rect->top + ((h-w)>>1);
        pData->DecSpinBox.right = rect->left + (w>>1);
        pData->DecSpinBox.bottom = pData->DecSpinBox.top + w;

        pData->IncSpinBox.left = rect->right - (w>>1);
        pData->IncSpinBox.top = pData->DecSpinBox.top;
        pData->IncSpinBox.right = rect->right;
        pData->IncSpinBox.bottom = pData->DecSpinBox.bottom;
    }
    else //updown button on the right
    {
        h = rect->bottom - rect->top;
        w = h < 16 ? h : 16;

        edit_box->left = rect->left;
        edit_box->top = rect->top;
        edit_box->bottom = rect->bottom;
        edit_box->right = rect->right - w - INTER_EDIT_BUTTON;

        pData->IncSpinBox.left = rect->right - w;
        pData->IncSpinBox.top = rect->top + (h>>1) - (w>>1);
        pData->IncSpinBox.right = rect->right;
        pData->IncSpinBox.bottom = rect->top + (h>>1) - 1;

        pData->DecSpinBox.left = rect->right - w;
        pData->DecSpinBox.top = rect->top + (h>>1) + 1;
        pData->DecSpinBox.right =  rect->right;
        pData->DecSpinBox.bottom = rect->bottom - (h>>1) + (w >>1); 
    }
}

static void OnSizeChanged (PCONTROL pCtrl, DWORD dwStyle, const RECT* rcClient)
{
    PCOMBOBOXDATA pData;
    int  tmpx, tmpy;
    int  height, width;
    int w, h;
    RECT edit_box, rect;

    pData = (PCOMBOBOXDATA)pCtrl->dwAddData2;

    SetRect (&rect, 0, 0, RECTWP (rcClient), RECTHP (rcClient));
    width = rect.right - rect.left;

    switch (dwStyle & CBS_TYPEMASK) 
    {
        case CBS_SIMPLE:
            height = GetSysFontHeight (SYSLOGFONT_CONTROL) + MARGIN_EDIT_TOP 
                         + MARGIN_EDIT_BOTTOM + (WIDTH_EDIT_BORDER<<1);
            edit_box.left = rect.left;
            edit_box.top = rect.top;
            edit_box.right = rect.left + width;
            edit_box.bottom = rect.top + height;
           
            pData->ListBoxRect.left = rect.left;
            pData->ListBoxRect.right = rect.right - MARGIN_EDIT_RIGHT;
            pData->ListBoxRect.top = rect.top + height + INTER_EDIT_LISTBOX;
            pData->ListBoxRect.bottom = pData->ListBoxRect.top + pData->list_height;
            break;

        case CBS_SPINLIST:
            GetSpinBox (pData, dwStyle, &rect, &edit_box);
            pData->spin_pace = 1;
            pData->fastspin_pace = 5;
            break;

        case CBS_AUTOSPIN:
            GetSpinBox (pData, dwStyle, &rect, &edit_box);
            pData->spin_min = 0;
            pData->spin_max = 100;
            pData->spin_pace = 1;
            pData->fastspin_pace = 5;
            break;

        default:  // now, for CBS_DROPDOWNLIST
            h = RECTH(rect);
            w = h < 16 ? h : 16;

            edit_box.left = rect.left;
            edit_box.top = rect.top;
            edit_box.bottom = rect.bottom;
            edit_box.right = rect.right - w - INTER_EDIT_BUTTON;

            pData->DecSpinBox.left = rect.right - w;
            pData->DecSpinBox.top = rect.top + ((h-w)>>1);
            pData->DecSpinBox.right = rect.right;
            pData->DecSpinBox.bottom = pData->DecSpinBox.top + w;

            pData->IncSpinBox.left = pData->DecSpinBox.left;
            pData->IncSpinBox.top = pData->DecSpinBox.top;
            pData->IncSpinBox.right = pData->DecSpinBox.right;
            pData->IncSpinBox.bottom = pData->DecSpinBox.bottom;

            pData->ListBoxRect.left = rect.left;
            pData->ListBoxRect.right = rect.right;
            pData->ListBoxRect.top = rect.bottom;
            pData->ListBoxRect.bottom = rect.bottom + pData->list_height;
            break;
    }

    tmpx = pData->ListBoxRect.right;
    tmpy = pData->ListBoxRect.bottom;
    ClientToScreen ((HWND)pCtrl, &tmpx, &tmpy);
    if (tmpy > g_rcScr.bottom) {
        pData->ListBoxRect.left = rect.left;
        pData->ListBoxRect.right = rect.right;
        pData->ListBoxRect.top = rect.top - pData->list_height;
        pData->ListBoxRect.bottom = rect.top;
    }

    if (pData->EditControl) {
        ShowWindow(pData->EditControl, SW_SHOW);
        MoveWindow (pData->EditControl, edit_box.left, edit_box.top, 
                            RECTW(edit_box), RECTH(edit_box), TRUE);
    }

    if (pData->ListBoxControl) {
        MoveWindow (pData->ListBoxControl, 
                        pData->ListBoxRect.left, pData->ListBoxRect.top,
                        width, pData->list_height, TRUE);
    }
}

static LRESULT ComboBoxCtrlProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PCONTROL      pCtrl   = gui_Control (hwnd);
    PCOMBOBOXDATA pData   = (PCOMBOBOXDATA)pCtrl->dwAddData2;
    DWORD         dwStyle = pCtrl->dwStyle;
    
    switch (message) {
        case MSG_CREATE:
        {
            const char* edit_text = GetWindowCaption (hwnd);
            DWORD list_style, edit_style;

            if (!(pData = calloc (1, sizeof (COMBOBOXDATA)))) {
                _MG_PRINTF ("CONTROL>ComboBox: Create control failure!\n");
                return -1;
            }

            pData->wStateFlags = 0;
            pData->nListItems = 0;
            pCtrl->dwAddData2 = (DWORD)pData;
            
            list_style = WS_VSCROLL | LBS_NOTIFY | WS_THINFRAME;
            if ((dwStyle & CBS_TYPEMASK) == CBS_DROPDOWNLIST)
            list_style |= LBS_MOUSEFOLLOW;

            if (dwStyle & CBS_SORT) list_style |= LBS_SORT;

            edit_style = WS_CHILD;
            if (!(dwStyle & CBS_EDITNOBORDER)) edit_style |= WS_BORDER;
            if (dwStyle & CBS_EDITBASELINE) edit_style |= ES_BASELINE;
            if (dwStyle & CBS_UPPERCASE) edit_style |= ES_UPPERCASE;
            if (dwStyle & CBS_LOWERCASE) edit_style |= ES_LOWERCASE;
            if (dwStyle & CBS_READONLY) edit_style |= ES_READONLY;

            switch (dwStyle & CBS_TYPEMASK) {
            case CBS_SIMPLE:
                list_style |= WS_VISIBLE | WS_BORDER;
                pData->list_height = (pCtrl->dwAddData > 0) 
                                    ? pCtrl->dwAddData : DEF_LISTHEIGHT;
                break;

            case CBS_SPINLIST:
                pData->list_height = 0;
                pData->spin_pace = 1;
                pData->fastspin_pace = 5;
                break;

            case CBS_AUTOSPIN:
                pData->spin_min = 0;
                pData->spin_max = 100;
                pData->spin_pace = 1;
                pData->fastspin_pace = 5;
                pData->list_height = -1;
                break;

            default:
                pData->list_height = (pCtrl->dwAddData > 0) 
                                    ? pCtrl->dwAddData : DEF_LISTHEIGHT;
                break;
            }

            if (pCtrl->dwExStyle & WS_EX_TRANSPARENT)
                pData->EditControl = CreateWindowEx (CTRL_SLEDIT, 
                        edit_text, 
                        edit_style, WS_EX_TRANSPARENT, IDC_CEDIT,
                        0, 0, 10, 10,
                        hwnd, 0);
            else
                pData->EditControl = CreateWindow (CTRL_SLEDIT, 
                        edit_text, 
                        edit_style, IDC_CEDIT,
                        0, 0, 10, 10,
                        hwnd, 0);

            if ((dwStyle & CBS_TYPEMASK) == CBS_AUTOSPIN) {
                WNDPROC old_edit_proc = 
                    SetWindowCallbackProc (pData->EditControl, AutoSpinEditBoxProc);
                SetWindowAdditionalData (pData->EditControl, (DWORD)old_edit_proc);

                pData->str_format = strdup ("%d");
            }
            
            if (dwStyle & CBS_EDITNOBORDER)
                pCtrl->iBkColor = GetWindowBkColor(pData->EditControl);
            else
                pCtrl->iBkColor = GetWindowBkColor(GetParent(hwnd));

            if (pData->list_height >= 0) {
                DWORD ex_style = WS_EX_NONE;
                if ((dwStyle & CBS_TYPEMASK) == CBS_SIMPLE) {
					ex_style |= (pCtrl->dwExStyle & WS_EX_TRANSPARENT);
				}
				else{
                    ex_style |= WS_EX_CTRLASMAINWIN;
                }
                
                pData->ListBoxControl = CreateWindowEx (CTRL_LISTBOX,
                            "listbox", list_style, ex_style,
                            IDC_CLISTBOX, 0, 0, 0, 0, hwnd, 0);
            }
            else {
                pData->ListBoxControl = 0;
            }

            break;
        }

        case MSG_DESTROY:
            DestroyAllControls (hwnd);
            free (pData->str_format);
            free (pData);
        return 0;

        case MSG_SIZECHANGING:
        {
            const RECT* rcExpect = (const RECT*)wParam;
            RECT* rcResult = (RECT*)lParam;
            if ((dwStyle & CBS_TYPEMASK) == CBS_SIMPLE) {
                int height = GetSysFontHeight (SYSLOGFONT_CONTROL)
                        + MARGIN_EDIT_TOP + MARGIN_EDIT_BOTTOM 
                        + (WIDTH_EDIT_BORDER<<1);

                rcResult->left = rcExpect->left;
                rcResult->top = rcExpect->top;
                rcResult->right = rcExpect->right;
                rcResult->bottom = rcExpect->top + height;
                rcResult->bottom += pData->list_height + INTER_EDIT_LISTBOX*2;
                return 0;
            }
            break;
        }

        case MSG_SIZECHANGED:
            OnSizeChanged (pCtrl, dwStyle, (const RECT*)lParam);
            break;

        case MSG_CHILDHIDDEN: 
        { 
            int reason = wParam; 
            int x = LOSWORD(lParam); 
            int y = HISWORD(lParam); 

            if (reason == RCTM_CLICK)
                ScreenToClient (hwnd, &x, &y); 

            if (IS_SET (pData, CSF_HASDROPPED)) {
                CLEAR_STATE (pData, CSF_HASDROPPED);
                NotifyParent (hwnd, pCtrl->id, CBN_SELECTCANCEL);
                NotifyParent (hwnd, pCtrl->id, CBN_CLOSEUP);
            }

            if (((dwStyle & CBS_TYPEMASK) > CBS_AUTOSPIN) 
                    && PtInRect (&pData->IncSpinBox, x, y))
                SET_STATE (pData, CSF_HASDROPPED); 
            break; 
        } 

        case MSG_SETFOCUS:
            if (dwStyle & CBS_AUTOFOCUS || dwStyle & WS_TABSTOP)
                SetFocusChild (pData->EditControl);
            if (dwStyle & CBS_NOTIFY)
                NotifyParent (hwnd, pCtrl->id, CBN_SETFOCUS);
            break;

        case MSG_KILLFOCUS:
            if (((dwStyle & CBS_TYPEMASK) == CBS_DROPDOWNLIST) && 
                    IS_SET (pData, CSF_HASDROPPED)) { 
                ShowWindow (pData->ListBoxControl, SW_HIDE); 
                CLEAR_STATE (pData, CSF_HASDROPPED); 
                if (dwStyle & CBS_NOTIFY)
                {
                    NotifyParent (hwnd, pCtrl->id, CBN_SELECTCANCEL);
                    NotifyParent (hwnd, pCtrl->id, CBN_CLOSEUP);
                }
            } 
            if (dwStyle & CBS_NOTIFY)
                NotifyParent (hwnd, pCtrl->id, CBN_KILLFOCUS);
            break;

        case MSG_LBUTTONDOWN: 
        { 
            int x = LOSWORD(lParam); 
            int y = HISWORD(lParam); 
            BOOL inc = FALSE;

            if (PtInRect (&pData->IncSpinBox, x, y))
            {
                inc = TRUE;
                //SendMessage(hwnd, MSG_PAINT, 
                //        MAKELONG(INC_BOX, LFRDR_BTN_STATUS_PRESSED), 0);
                ComboBoxDrawSpinButton(hwnd, 0, INC_BOX, LFRDR_BTN_STATUS_PRESSED);
            }
            else if (PtInRect (&pData->DecSpinBox, x, y))
            {
                inc = FALSE;
                //SendMessage(hwnd, MSG_PAINT, 
                //        MAKELONG(DEC_BOX, LFRDR_BTN_STATUS_PRESSED), 0);
                ComboBoxDrawSpinButton(hwnd, 0, DEC_BOX, LFRDR_BTN_STATUS_PRESSED);
            }
            else
                break;

            switch (dwStyle & CBS_TYPEMASK) { 
                case CBS_SPINLIST:
                    SetCapture (hwnd);
                    SendMessage (hwnd, CB_SPIN, inc, 0);
                    SetAutoRepeatMessage (hwnd, CB_FASTSPIN, inc, 0);
                    break;
                case CBS_AUTOSPIN:
                    SetCapture (hwnd);
                    SendMessage (hwnd, CB_SPIN, !inc, 0);
                    SetAutoRepeatMessage (hwnd, CB_FASTSPIN, !inc, 0);
                    break;

                case CBS_DROPDOWNLIST:
                    if (IS_SET (pData, CSF_HASDROPPED)) {
                        CLEAR_STATE (pData, CSF_HASDROPPED); 
                    } 
                    else { 
                        ShowWindow (pData->ListBoxControl, SW_SHOW);
                        SetFocusChild (pData->ListBoxControl);
                        SendNotifyMessage (pData->ListBoxControl, MSG_SETFOCUS, 0,0);
                        SET_STATE (pData, CSF_HASDROPPED); 
                        if (dwStyle & CBS_NOTIFY)
                            NotifyParent (hwnd, pCtrl->id, CBN_DROPDOWN);
                    } 
                    break;
            } 

            break; 
        } 

        case MSG_LBUTTONUP: 
        {
            int x = LOSWORD(lParam); 
            int y = HISWORD(lParam); 
            
            if(!((dwStyle & CBS_TYPEMASK) == CBS_DROPDOWNLIST)) 
                ScreenToClient(hwnd, &x, &y);
            
            if (PtInRect (&pData->IncSpinBox, x, y))
            {
                //SendMessage(hwnd, MSG_PAINT, 
                //        MAKELONG(INC_BOX, LFRDR_BTN_STATUS_HILITE), 0);
                ComboBoxDrawSpinButton(hwnd, 0, INC_BOX, LFRDR_BTN_STATUS_HILITE);
                mouse_old = INC_BOX;
            }
            else if (PtInRect (&pData->DecSpinBox, x, y))
            {
                //SendMessage(hwnd, MSG_PAINT, 
                //        MAKELONG(DEC_BOX, LFRDR_BTN_STATUS_HILITE), 0);
				ComboBoxDrawSpinButton(hwnd, 0, DEC_BOX, LFRDR_BTN_STATUS_HILITE);
                mouse_old = DEC_BOX;
            }

            switch (dwStyle & CBS_TYPEMASK) 
            { 
                case CBS_SPINLIST:
                    if (IS_SET (pData, CSF_HASDROPPED)) {
                        SetFocusChild (pData->ListBoxControl);
                    } 
                case CBS_AUTOSPIN:
                    /* cancel repeat message */
                    SetAutoRepeatMessage (0, 0, 0, 0);
                    ReleaseCapture ();
            }
            NotifyParent (hwnd, pCtrl->id, CBN_CLICKED);
            break;
        }

        case MSG_KEYDOWN:
        {
            switch (dwStyle & CBS_TYPEMASK) {
            case CBS_SPINLIST:
            case CBS_AUTOSPIN:
                switch (LOWORD(wParam)) {
                case SCANCODE_CURSORBLOCKUP:
                case SCANCODE_CURSORBLOCKDOWN:
                    SendMessage (hwnd, CB_SPIN, 
                            (LOWORD(wParam) == SCANCODE_CURSORBLOCKDOWN), 0);
                    break;

                case SCANCODE_PAGEUP:
                case SCANCODE_PAGEDOWN:
                    SendMessage (hwnd, CB_FASTSPIN, 
                            (LOWORD(wParam) == SCANCODE_PAGEDOWN), 0);
                    break;
                }
                break;

            case CBS_DROPDOWNLIST:
                switch (LOWORD(wParam)) {
                case SCANCODE_ESCAPE:
                    if (IS_SET (pData, CSF_HASDROPPED)) {
                        ShowWindow (pData->ListBoxControl, SW_HIDE);
                        CLEAR_STATE (pData, CSF_HASDROPPED);
                        if (dwStyle & CBS_NOTIFY) {
                            NotifyParent (hwnd, pCtrl->id, CBN_SELECTCANCEL);
                            NotifyParent (hwnd, pCtrl->id, CBN_CLOSEUP);
                        }
                        SetFocusChild (pData->EditControl);
                    }
                    break;

                case SCANCODE_KEYPADENTER:
                case SCANCODE_ENTER:
                    if (IS_SET (pData, CSF_HASDROPPED)) {
                        ShowWindow (pData->ListBoxControl, SW_HIDE);
                        CLEAR_STATE (pData, CSF_HASDROPPED);
                        if (dwStyle & CBS_NOTIFY) {
                            NotifyParent (hwnd, pCtrl->id, CBN_SELECTOK);
                            NotifyParent (hwnd, pCtrl->id, CBN_CLOSEUP);
                        }
                        SetFocusChild (pData->EditControl);
                    }
                    break;

                case SCANCODE_CURSORBLOCKUP:
                case SCANCODE_PAGEUP:
                case SCANCODE_CURSORBLOCKDOWN:
                case SCANCODE_PAGEDOWN:
                    if (!IS_SET (pData, CSF_HASDROPPED)) {
                        ShowWindow (pData->ListBoxControl, SW_SHOW);
                        SetFocusChild (pData->ListBoxControl);
                        SET_STATE (pData, CSF_HASDROPPED);
                        if (dwStyle & CBS_NOTIFY)
                            NotifyParent (hwnd, pCtrl->id, CBN_DROPDOWN);
                    }
                    break;
                }
                break;
            }
        }

        case MSG_COMMAND:
        {
            int len;
            char *buffer;
            int sel;
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);

            switch (id) {
            case IDC_CLISTBOX:
            {
                if ((dwStyle & CBS_TYPEMASK) == CBS_DROPDOWNLIST && 
                                (code == LBN_CLICKED || code == LBN_KILLFOCUS)){
                    if (IS_SET (pData, CSF_HASDROPPED)) {
                        ShowWindow (pData->ListBoxControl, SW_HIDE);
                        CLEAR_STATE (pData, CSF_HASDROPPED);
                        if (dwStyle & CBS_NOTIFY) {
                            NotifyParent (hwnd, pCtrl->id, CBN_SELECTOK);
                            NotifyParent (hwnd, pCtrl->id, CBN_CLOSEUP);
                        }
                    }
                    break;
                }

                if (code == LBN_DBLCLK && (dwStyle & CBS_NOTIFY)) {
                    NotifyParent (hwnd, pCtrl->id, CBN_DBLCLK);
                    break;
                }

                if (code != LBN_SELCHANGE && dwStyle & CBS_READONLY)
                    break;

                if ((sel = SendMessage (pData->ListBoxControl, 
                                LB_GETCURSEL, 0, 0)) >= 0) {
                    len = SendMessage (pData->ListBoxControl, LB_GETTEXTLEN, sel, 0);
                    buffer = ALLOCATE_LOCAL (len + 1);
                    //buffer = FixStrAlloc (len + 1);
                    SendMessage (pData->ListBoxControl, LB_GETTEXT, 
                                    sel, (LPARAM)buffer);
                    SendMessage (pData->EditControl, MSG_SETTEXT, 0, (LPARAM)buffer);
                    DEALLOCATE_LOCAL(buffer);
                    //FreeFixStr (buffer);
                }

                if (dwStyle & CBS_NOTIFY)
                    NotifyParent (hwnd, pCtrl->id, CBN_SELCHANGE);
                break;
            } 

            case IDC_CEDIT:
                if (code == EN_DBLCLK && dwStyle & CBS_NOTIFY) {
                    NotifyParent (hwnd, pCtrl->id, CBN_DBLCLK);
                }
                else if (code == EN_CLICKED) {
                    NotifyParent (hwnd, pCtrl->id, CBN_CLICKED);
                }
                else if (code == EN_CHANGE) {
                    if (GetFocusChild(hwnd) == pData->EditControl) {
                        if (pData->ListBoxControl) {
							int len = GetWindowTextLength(pData->EditControl);
                            if ( len > 0) {
#ifdef HAVE_ALLOCA
                            	char *str = (char*)alloca(len+1);
#else
								char *str = (char*)malloc(len+1);
#endif
								if(str){
	                            	GetWindowText (pData->EditControl, str, len);
    	                            if ((sel = SendMessage (pData->ListBoxControl, 
        	                                        LB_FINDSTRING, 0, (LPARAM)str)) >= 0)
            	                        SendMessage (pData->ListBoxControl, 
                                 	           LB_SETCURSEL, sel, 0);
#ifndef HAVE_ALLOCA
									free(str);
#endif
                           		 }
							}
                        }
                    }

                    if (dwStyle & CBS_NOTIFY)
                        NotifyParent (hwnd, pCtrl->id, CBN_EDITCHANGE);
                 }
                 break;
            }
            break;
        }

        case MSG_GETDLGCODE:
            return DLGC_WANTARROWS | DLGC_WANTENTER;

        case MSG_FONTCHANGED:
            SetWindowFont (pData->EditControl, GetWindowFont (hwnd));
            if (pData->ListBoxControl)
                SetWindowFont (pData->ListBoxControl, GetWindowFont (hwnd));
            return 0;
        case MSG_MOUSEMOVEIN:
            mouse_old = 0;
            if(!(BOOL)wParam)
                //SendMessage(hwnd, MSG_PAINT, 0, 0);
				ComboBoxDrawSpinButton(hwnd, 0, 0, 0);
            break;
        case MSG_MOUSEMOVE:
            {
                int mouse = 0;
                int x_pos = LOSWORD (lParam);
                int y_pos = HISWORD (lParam);
                
                if(PtInRect(&pData->IncSpinBox, x_pos, y_pos))
                    mouse = INC_BOX; 
                else if(PtInRect(&pData->DecSpinBox, x_pos, y_pos))
                    mouse = DEC_BOX;
                else
                    mouse = 0;
                
                if(mouse != mouse_old)
                {
                    //SendMessage(hwnd, MSG_PAINT, 
                    //            MAKELONG(mouse, LFRDR_BTN_STATUS_HILITE), 0);
					ComboBoxDrawSpinButton(hwnd, 0, mouse, LFRDR_BTN_STATUS_HILITE);
                    mouse_old = mouse;
                }
                break;
            }
        case MSG_PAINT:
        {
            HDC hdc;
            hdc = BeginPaint (hwnd);
            ComboBoxDrawSpinButton (hwnd, hdc, 0, 0);
            EndPaint (hwnd, hdc);
            return 0;
        }

        case CB_GETCHILDREN:
        {
            HWND* wnd_edit = (HWND*)wParam;
            HWND* wnd_listbox = (HWND*)lParam;

            *wnd_edit = pData->EditControl;
            *wnd_listbox = pData->ListBoxControl;
            return 0;
        }

        /* when click the up/down button, 
         * spin the value of SIMPLE and AUTOSPIN combobox */
        case CB_SPIN:
            switch (dwStyle & CBS_TYPEMASK) {
            case CBS_SPINLIST:
                if (wParam)
                    SendMessage (pData->ListBoxControl, MSG_KEYDOWN, 
                            SCANCODE_CURSORBLOCKDOWN, 0);
                else
                    SendMessage (pData->ListBoxControl, MSG_KEYDOWN, 
                            SCANCODE_CURSORBLOCKUP, 0);
                break;

            case CBS_AUTOSPIN:
            {
                char buffer [LEN_SPINVALUE+1];
                int cur_value = 0;
                int new_value;

                GetWindowText (pData->EditControl, buffer, LEN_SPINVALUE);
#if 0
                cur_value = new_value = strtol (buffer, NULL, 0);
#else
                sscanf (buffer, pData->str_format, &cur_value);
                new_value = cur_value;
#endif
                if (wParam) {
                    new_value -= pData->spin_pace;
                }
                else {
                    new_value += pData->spin_pace;
                }

                if (dwStyle & CBS_AUTOLOOP) {
                    if (new_value > pData->spin_max)
                        new_value = pData->spin_min;
                    else if (new_value < pData->spin_min)
                        new_value = pData->spin_max;
                }
                else {
                    if (new_value > pData->spin_max)
                        new_value = pData->spin_max;
                    else if (new_value < pData->spin_min)
                        new_value = pData->spin_min;
                }

                if (new_value != cur_value) {
                    snprintf (buffer, LEN_SPINVALUE, pData->str_format, new_value);
                    SetWindowText (pData->EditControl, buffer);
                    if (dwStyle & CBS_NOTIFY)
                        NotifyParent (hwnd, pCtrl->id, CBN_EDITCHANGE);
                }
                break;
            }
            }
            break;

        case CB_FASTSPIN:
            switch (dwStyle & CBS_TYPEMASK) {
            case CBS_SPINLIST:
                if (wParam)
                    SendMessage (pData->ListBoxControl, MSG_KEYDOWN, 
                                    SCANCODE_PAGEDOWN, 0);
                else
                    SendMessage (pData->ListBoxControl, MSG_KEYDOWN, 
                                    SCANCODE_PAGEUP, 0);
                break;

            case CBS_AUTOSPIN:
            {
                char buffer [LEN_SPINVALUE+1];
                int cur_value, new_value;

                GetWindowText (pData->EditControl, buffer, LEN_SPINVALUE);
#if 0
                cur_value = new_value = strtol (buffer, NULL, 0);
#else
                sscanf (buffer, pData->str_format, &cur_value);
                new_value = cur_value;
#endif
                if (wParam) {
                    new_value -= pData->fastspin_pace;
                }
                else {
                    new_value += pData->fastspin_pace;
                }

                if (dwStyle & CBS_AUTOLOOP) {
                    if (new_value > pData->spin_max)
                        new_value = pData->spin_min;
                    else if (new_value < pData->spin_min)
                        new_value = pData->spin_max;
                }
                else {
                    if (new_value > pData->spin_max)
                        new_value = pData->spin_max;
                    else if (new_value < pData->spin_min)
                        new_value = pData->spin_min;
                }

                if (new_value != cur_value) {
                    snprintf (buffer, LEN_SPINVALUE, pData->str_format, new_value);
                    SetWindowText (pData->EditControl, buffer);
                    if (dwStyle & CBS_NOTIFY)
                        NotifyParent (hwnd, pCtrl->id, CBN_EDITCHANGE);
                }
                break;
            }
            }
            break;

        break;
    }
    
    if ((dwStyle & CBS_TYPEMASK) == CBS_AUTOSPIN) {
        switch (message) {
            case CB_SETSPINFORMAT:
            {
                free (pData->str_format);
                if ((pData->str_format = strdup ((const char*)lParam)))
                    return CB_OKAY;

                return CB_ERR;
            }

            case CB_SETSPINRANGE:
            {
                char buffer [LEN_SPINVALUE+1];
                int new_min = (int)wParam;
                int new_max = (int)lParam;
                int cur_value;

                if (new_min > new_max)
                    return CB_ERR;

                pData->spin_min = (int)wParam;
                pData->spin_max = (int)lParam;
                GetWindowText (pData->EditControl, buffer, LEN_SPINVALUE);

#if 0
                cur_value = strtol (buffer, NULL, 0);
#else
                sscanf (buffer, pData->str_format, &cur_value);
#endif
                if (cur_value < pData->spin_min) {
                    snprintf (buffer, LEN_SPINVALUE, 
                            pData->str_format, pData->spin_min);
                    SetWindowText (pData->EditControl, buffer);
                }
                else if (cur_value > pData->spin_max) {
                    snprintf (buffer, LEN_SPINVALUE, 
                            pData->str_format, pData->spin_max);
                    SetWindowText (pData->EditControl, buffer);
                }

                return CB_OKAY;
            }

            case CB_GETSPINRANGE:
            {
                int* spin_min = (int*)wParam;
                int* spin_max = (int*)lParam;

                if (spin_min) *spin_min = pData->spin_min;
                if (spin_max) *spin_max = pData->spin_max;
                return CB_OKAY;
            }

            case CB_SETSPINVALUE:
            {
                int new_value = (int)wParam;
                char buffer [LEN_SPINVALUE+1];

                if (new_value < pData->spin_min || new_value > pData->spin_max)
                    return CB_ERR;

                snprintf (buffer, LEN_SPINVALUE, pData->str_format, new_value);
                SetWindowText (pData->EditControl, buffer);
                return CB_OKAY;
            }

            case CB_GETSPINVALUE:
            {
                char buffer [LEN_SPINVALUE+1];
                int value;

                GetWindowText (pData->EditControl, buffer, LEN_SPINVALUE);
                sscanf (buffer, pData->str_format, &value);
                return value;
                // return strtol (buffer, NULL, 0);
            }

            case CB_SETSPINPACE:
            {
                int pace = (int)wParam;
                int fastpace = (int)lParam;

                if (pace != 0)
                    pData->spin_pace = pace;
                if (fastpace != 0)
                    pData->fastspin_pace = fastpace;

                return CB_OKAY;
            }

            case CB_GETSPINPACE:
            {
                int *pace = (int*)wParam;
                int *fastpace = (int*)lParam;

                if (pace)
                    *pace = pData->spin_pace;
                if (*fastpace)
                    *fastpace = pData->fastspin_pace;

                return CB_OKAY;
            }
        }
        return DefaultControlProc (hwnd, message, wParam, lParam);
    }
    else
        return DefCBProc (hwnd, message, wParam, lParam);
}

static int DefCBProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    PCOMBOBOXDATA pData;
    int len, index;
    char *selection;
    int  rc;

    pData = (COMBOBOXDATA *) gui_Control(hWnd)->dwAddData2;
    switch (message) {
        /* messages specific to the listbox control */
        case CB_SETSTRCMPFUNC:
            return SendMessage (pData->ListBoxControl, LB_SETSTRCMPFUNC, 
                                wParam, lParam);

        case CB_ADDSTRING:
            rc = SendMessage (pData->ListBoxControl, LB_ADDSTRING, 
                                wParam, lParam);
            if (rc >= 0)
                pData->nListItems++; 
            return rc;

        case CB_DELETESTRING:
            rc = SendMessage (pData->ListBoxControl, LB_DELETESTRING, 
                                wParam, lParam);
            if (rc == 0) {
                int idx;
                pData->nListItems--;
                if (pData->nListItems <= wParam)
                    idx = pData->nListItems - 1;
                else
                    idx = wParam;

                SendMessage (hWnd, CB_SETCURSEL, idx, 0);
            }
            return rc;

        case CB_DIR:
            return SendMessage (pData->ListBoxControl, LB_DIR, wParam, lParam);
            
        case CB_FINDSTRING:
            return SendMessage (pData->ListBoxControl, LB_FINDSTRING, 
                                    wParam, lParam);
            
        case CB_FINDSTRINGEXACT:
               return SendMessage (pData->ListBoxControl, LB_FINDSTRINGEXACT, 
                                    wParam, lParam);
            
        case CB_GETCOUNT:
            return SendMessage (pData->ListBoxControl, LB_GETCOUNT, 
                                    wParam, lParam);
            
        case CB_GETITEMADDDATA:
            return SendMessage (pData->ListBoxControl, LB_GETITEMADDDATA, 
                                    wParam, lParam);
            
        case CB_SETITEMADDDATA:
            return SendMessage (pData->ListBoxControl, LB_SETITEMADDDATA, 
                                    wParam, lParam);
            
        case CB_GETITEMHEIGHT:
            return SendMessage (pData->ListBoxControl, LB_GETITEMHEIGHT, 
                                    wParam, lParam);
            
        case CB_SETITEMHEIGHT:
            return SendMessage (pData->ListBoxControl, LB_SETITEMHEIGHT, 
                                    wParam, lParam);
            
        case CB_GETLBTEXT:
            return SendMessage (pData->ListBoxControl, LB_GETTEXT, 
                                    wParam, lParam);
            
        case CB_GETLBTEXTLEN:
            return SendMessage (pData->ListBoxControl, LB_GETTEXTLEN, 
                                    wParam, lParam);
            
        case CB_INSERTSTRING:
            rc = SendMessage (pData->ListBoxControl, LB_INSERTSTRING, 
                                    wParam, lParam);
            if (rc >= 0)
                pData->nListItems++; 
            return rc;
                
        case CB_GETCURSEL:
            return SendMessage (pData->ListBoxControl, LB_GETCURSEL, 
                                    wParam, lParam);

        case CB_SETCURSEL:
            if (SendMessage (pData->ListBoxControl, LB_SETCURSEL, 
                                wParam, lParam) == LB_ERR) {
                SetWindowText (pData->EditControl, "");
                return CB_ERR;
            }

            index = SendMessage (pData->ListBoxControl, LB_GETCURSEL, 
                                    wParam, lParam);
            len = SendMessage (pData->ListBoxControl, LB_GETTEXTLEN, index, 0);
            if (len <= 0) {
                SetWindowText (pData->EditControl, "");
                return CB_ERR;
            }

            selection = FixStrAlloc (len + 1);  
            SendMessage (pData->ListBoxControl, LB_GETTEXT,
                                (WPARAM)index, (LPARAM)selection);
            SetWindowText (pData->EditControl, selection);
            FreeFixStr (selection);
            break;

        /* messages specific to the edit control */
        case CB_GETEDITSEL:
            return SendMessage (pData->EditControl, EM_GETSEL, wParam, lParam);

        case CB_LIMITTEXT:
            return SendMessage (pData->EditControl, EM_LIMITTEXT, 
                                    wParam, lParam);

        case CB_SETEDITSEL:
            return SendMessage (pData->EditControl, EM_SETSEL, wParam, lParam);

        case MSG_GETTEXTLENGTH:
            return SendMessage (pData->EditControl, MSG_GETTEXTLENGTH, 
                                    wParam, lParam);

        case MSG_GETTEXT:
            return SendMessage (pData->EditControl, MSG_GETTEXT, wParam, lParam);

        case MSG_SETTEXT:
            return SendMessage (pData->EditControl, MSG_SETTEXT, wParam, lParam);

        /* messages handled by the combobox itself */
        case CB_GETDROPPEDCONTROLRECT:
            if (pData->ListBoxControl) {
                CopyRect ((PRECT)lParam, &pData->ListBoxRect);
                return 0;
            }
            return CB_ERR;

        case CB_GETDROPPEDSTATE:
            return IS_SET (pData, CSF_CAPTUREACTIVE);

        case CB_RESETCONTENT:
            SendMessage (pData->ListBoxControl, LB_RESETCONTENT, 0, 0);
            SetWindowText (pData->EditControl, "");
            return 0;

        case CB_SELECTSTRING:
            index = SendMessage (pData->ListBoxControl, LB_SELECTSTRING, 
                                    wParam, lParam);
            if (index == LB_ERR)
               return CB_ERR;

            len = SendMessage (pData->ListBoxControl, LB_GETTEXTLEN, index, 0);
            if (len <= 0)
               return CB_ERR;

            selection = FixStrAlloc (len + 1);  
            rc = SendMessage (pData->ListBoxControl, LB_GETTEXT, 
                                    (WPARAM)index, (LPARAM)selection);
            SetWindowText (pData->EditControl, selection);
            FreeFixStr (selection);
            break;
    }

    return DefaultControlProc (hWnd, message, wParam, lParam);
}

#endif /* _MGCTRL_COMBOBOX */

