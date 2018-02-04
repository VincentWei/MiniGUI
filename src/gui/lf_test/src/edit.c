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
** $Id: edit.c,v 1.23 2007-12-06 02:24:59 xwyan Exp $
**
** edit.c: The Edit control demo program.
**
** Copyright (C) 2001 ~ 2002 Wei Yongming.
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Create date: 2001/11/01
*/

/*
**  This source is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public
**  License as published by the Free Software Foundation; either
**  version 2 of the License, or (at your option) any later version.
**
**  This software is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public
**  License along with this library; if not, write to the Free
**  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
**  MA 02111-1307, USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

static HWND hMainWnd = HWND_INVALID;

#define IDC_CTRL1     100
#define IDC_CTRL2     110
#define IDC_CTRL3     120
#define IDC_CTRL4     130
#define IDC_CTRL5     140
#define IDC_CTRL6     150
#define IDC_CTRL7     160

static int ControlTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:

            CreateWindow (CTRL_STATIC,
                        "Simple edit box with fixed font:",
                        WS_CHILD | WS_VISIBLE | SS_SIMPLE, 
                        IDC_STATIC, 
                        10, 15, 300, 20, hWnd, 0);
            CreateWindow (CTRL_EDIT, 
                        "This is a simple edit box. It uses fixed system font to display text.", 
                        WS_CHILD | WS_VISIBLE | WS_BORDER, 
                        IDC_CTRL1, 
                        10, 40, 325, 24, hWnd, 0);

            CreateWindow (CTRL_STATIC,
                        "Single-line edit box using variable-width font:",
                        WS_CHILD | WS_VISIBLE | SS_SIMPLE, 
                        IDC_STATIC, 
                        10, 75, 300, 20, hWnd, 0);
            CreateWindow (CTRL_SLEDIT, 
                        "This is a single line edit box. It uses fixed system font to display text", 
                        WS_CHILD | WS_BORDER | WS_VISIBLE, 
                        IDC_CTRL2, 
                        10, 100, 325, 24, hWnd, 0);

            CreateWindow (CTRL_STATIC,
                        "Multiple-line edit box using variable-width font:",
                        WS_CHILD | WS_VISIBLE | SS_SIMPLE, 
                        IDC_STATIC, 
                        10, 135, 280, 20, hWnd, 0);
            CreateWindow (CTRL_MLEDIT, 
                        "飞漫软件－嵌入式图形系统及浏览器技术领导厂商！\n \n \n ", 
                        WS_CHILD | WS_BORDER | WS_VSCROLL | WS_VISIBLE, 
                        IDC_CTRL3, 
                        10, 160, 325, 50, hWnd, 0);

            CreateWindow (CTRL_STATIC,
                        "Multiple-line edit box (auto-wrap):",
                        WS_CHILD | WS_VISIBLE | SS_SIMPLE, 
                        IDC_STATIC, 
                        350, 20, 280, 20, hWnd, 0);
            CreateWindow (CTRL_MLEDIT, 
                        "飞漫软件－嵌入式图形系统及浏览器技术领导厂商！\n \n \n ", 
                        WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL |ES_AUTOWRAP, 
                        IDC_CTRL3 + 1, 
                        350, 40, 230,170, hWnd, 0);

            CreateWindow (CTRL_STATIC,
                        "Password edit box:",
                        WS_CHILD | WS_VISIBLE | SS_SIMPLE, 
                        IDC_STATIC, 
                        10, 225, 100, 20, hWnd, 0);
            CreateWindow (CTRL_SLEDIT, 
                        "Password", 
                        WS_CHILD | WS_BORDER | ES_PASSWORD | WS_VISIBLE, 
                        IDC_CTRL4, 
                        10, 250, 100, 24, hWnd, 0);

            CreateWindow (CTRL_STATIC,
                        "Typed chars to upper case:",
                        WS_CHILD | WS_VISIBLE | SS_SIMPLE, 
                        IDC_STATIC, 
                        120, 225, 100, 20, hWnd, 0);
            CreateWindow (CTRL_EDIT, 
                        "", 
                        WS_CHILD | WS_BORDER | ES_UPPERCASE | WS_VISIBLE, 
                        IDC_CTRL4 + 1, 
                        120, 250, 100, 24, hWnd, 0);

            CreateWindow (CTRL_STATIC,
                        "Typed chars to lower case:",
                        WS_CHILD | WS_VISIBLE | SS_SIMPLE, 
                        IDC_STATIC, 
                        230, 225, 100, 20, hWnd, 0);
            CreateWindow (CTRL_EDIT, 
                        "", 
                        WS_CHILD | WS_BORDER | ES_LOWERCASE | WS_VISIBLE, 
                        IDC_CTRL4 + 2, 
                        230, 250, 100, 24, hWnd, 0);

            CreateWindow (CTRL_STATIC,
                        "Read only edit box:",
                        WS_CHILD | WS_VISIBLE | SS_SIMPLE, 
                        IDC_STATIC, 
                        10, 285, 300, 20, hWnd, 0);
            CreateWindow (CTRL_SLEDIT, 
                        "You can not edit the text in this edit box.",
                        WS_CHILD | WS_BORDER | ES_READONLY | WS_VISIBLE, 
                        IDC_CTRL5, 
                        10, 310, 325, 24, hWnd, 0);

            CreateWindow (CTRL_STATIC,
                        "Limited edit box:",
                        WS_CHILD | WS_VISIBLE | SS_SIMPLE, 
                        IDC_STATIC, 
                        10, 345, 300, 20, hWnd, 0);
            CreateWindow (CTRL_SLEDIT, 
                        "You can only input 100 characters in this box.", 
                        WS_CHILD | WS_BORDER | WS_VISIBLE | ES_UPPERCASE | ES_BASELINE | ES_AUTOWRAP | ES_LEFT | ES_NOHIDESEL | ES_TITLE | ES_AUTOSELECT,
                        IDC_CTRL5 + 1, 
                        10, 370, 325, 24, hWnd, 0);
            SendDlgItemMessage (hWnd, IDC_CTRL5 + 1, EM_LIMITTEXT, 100, 0);

            CreateWindow (CTRL_BUTTON, 
                        "Close", 
                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
                        IDCANCEL, 
                        520, 370, 60, 24, hWnd, 0);

            CreateWindow (CTRL_SLEDIT, 
                        "Right-aligned text",
                        WS_CHILD | WS_BORDER | ES_RIGHT | WS_VISIBLE, 
                        IDC_CTRL6, 
                        350, 250, 230, 24, hWnd, 0);

            CreateWindow (CTRL_SLEDIT, 
                        "Center-aligned text",
                        WS_CHILD | WS_BORDER | ES_CENTER | WS_VISIBLE, 
                        IDC_CTRL7, 
                        350, 310, 230, 24, hWnd, 0);
        break;

        case MSG_COMMAND:
        {
            int id   = LOWORD(wParam);
            int code = HIWORD(wParam);
            char buffer [256];
            HDC hdc;

            if (wParam == IDCANCEL) {
                PostMessage (hWnd, MSG_CLOSE, 0, 0);
                return 0;
            }

            sprintf (buffer, "ID: %d, Code: %x", id, code);

            hdc = GetClientDC (hWnd);
            TextOut (hdc, 0, 0, buffer);
            
            switch (id)
            {
                case IDC_CTRL1:
                break;
            
                case IDC_CTRL2:
                break;
                
                case IDC_CTRL3:
                break;
                
                case IDC_CTRL4:
                    if (code == EN_CHANGE) {
                        char buffer [256];
                        GetWindowText ((HWND)lParam, buffer, 255);
                    }
                break;
                
                default:
                break;
            }

            ReleaseDC (hdc);
            break;
        }
        
	case MSG_DESTROY:
            DestroyAllControls (hWnd);
            hMainWnd = HWND_INVALID;
	    return 0;

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            MainWindowCleanup (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitCreateInfo(PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_IMECOMPOSE;
    pCreateInfo->spCaption = "Edit controls" ;
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlTestWinProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 600;
    pCreateInfo->by = 450;
    pCreateInfo->iBkColor = GetWindowElementColor (WE_MAINC_THREED_BODY);
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

void edit_demo (HWND hwnd)
{
    MAINWINCREATE CreateInfo;

    if (hMainWnd != HWND_INVALID) {
        ShowWindow (hMainWnd, SW_SHOWNORMAL);
        return;
    }

    InitCreateInfo (&CreateInfo);
    CreateInfo.hHosting = hwnd;

    hMainWnd = CreateMainWindow (&CreateInfo);
}

