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
** $Id: static.c,v 1.13 2007-12-06 02:25:00 xwyan Exp $
**
** static.c: The Static control demo program.
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
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define IDC_STATIC1     101
#define IDC_STATIC2     102
#define IDC_STATIC3     103
#define IDC_STATIC4     104

static HWND hMainWnd = HWND_INVALID;

static int ControlTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
        {
            CreateWindow (CTRL_STATIC, 
                            "This is a simple static control.", 
                            WS_CHILD | SS_NOTIFY | SS_SIMPLE | WS_VISIBLE | WS_THINFRAME | WS_BORDER,
                            IDC_STATIC1, 
                            10, 10, 180, 20, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "This is a left-aligned static control (auto-wrap).", 
                            WS_CHILD | SS_NOTIFY | SS_LEFT | WS_VISIBLE | WS_BORDER,
                            IDC_STATIC2, 
                            10, 40, 180, 45, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "This is a right-aligned static control (auto-wrap).", 
                            WS_CHILD | SS_NOTIFY | SS_RIGHT | WS_VISIBLE | WS_BORDER | WS_THINFRAME,
                            IDC_STATIC3, 
                            10, 90, 180, 45, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "This is a center-aligned static control (auto-wrap).", 
                            WS_CHILD | SS_NOTIFY | SS_CENTER | WS_VISIBLE | WS_BORDER,
                            IDC_STATIC4, 
                            10, 140, 180, 45, hWnd, 0);

            CreateWindow (CTRL_STATIC, 
                            "An icon static control: ", 
                            WS_CHILD | SS_RIGHT | WS_VISIBLE,
                            IDC_STATIC, 
                            250, 40, 150, 20, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "", 
                            WS_CHILD | SS_ICON | WS_VISIBLE,
                            IDC_STATIC, 
                            420, 20, 50, 50, hWnd, (DWORD)GetLargeSystemIcon (IDI_INFORMATION));

            CreateWindow (CTRL_STATIC, 
                            "A bitmap static control: ", 
                            WS_CHILD | SS_RIGHT | WS_VISIBLE,
                            IDC_STATIC, 
                            250, 100, 150, 20, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "", 
                            WS_CHILD | SS_ICON | WS_VISIBLE,
                            IDC_STATIC, 
                            420, 80, 50, 50, hWnd, (DWORD)GetLargeSystemIcon (IDI_APPLICATION));

            CreateWindow (CTRL_STATIC, 
                            "A bitmap static control (center): ", 
                            WS_CHILD | SS_RIGHT | WS_VISIBLE,
                            IDC_STATIC, 
                            250, 160, 150, 20, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "", 
                            WS_CHILD | SS_ICON | SS_REALSIZEIMAGE | SS_CENTERIMAGE | WS_VISIBLE,
                            IDC_STATIC, 
                            420, 140, 50, 50, hWnd, (DWORD)GetLargeSystemIcon (IDI_STOP));

            CreateWindow (CTRL_STATIC, 
                            "A black box: ", 
                            WS_CHILD |  WS_VISIBLE,
                            IDC_STATIC, 
                            10, 200, 100, 20, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "", 
                            WS_CHILD | SS_BLACKRECT | WS_VISIBLE,
                            IDC_STATIC, 
                            110, 200, 50, 20, hWnd, 0);

            CreateWindow (CTRL_STATIC, 
                            "A gray box: ", 
                            WS_CHILD | WS_VISIBLE,
                            IDC_STATIC, 
                            10, 230, 100, 20, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "", 
                            WS_CHILD | SS_GRAYRECT | WS_VISIBLE,
                            IDC_STATIC, 
                            110, 230, 50, 20, hWnd, 0);
            
            CreateWindow (CTRL_STATIC, 
                            "A white box: ", 
                            WS_CHILD | WS_VISIBLE ,
                            IDC_STATIC, 
                            10, 260, 100, 20, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "", 
                            WS_CHILD | SS_WHITERECT | WS_VISIBLE,
                            IDC_STATIC, 
                            110, 260, 50, 20, hWnd, 0);
            
            CreateWindow (CTRL_STATIC, 
                            "A black frame: ", 
                            WS_CHILD | WS_VISIBLE,
                            IDC_STATIC, 
                            170, 200, 90, 20, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "", 
                            WS_CHILD | SS_BLACKFRAME | WS_VISIBLE,
                            IDC_STATIC, 
                            260, 200, 50, 20, hWnd, 0);

            CreateWindow (CTRL_STATIC, 
                            "A gray frame: ", 
                            WS_CHILD | WS_VISIBLE,
                            IDC_STATIC, 
                            170, 230, 100, 20, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "", 
                            WS_CHILD | SS_GRAYFRAME | WS_VISIBLE,
                            IDC_STATIC, 
                            260, 230, 50, 20, hWnd, 0);
            
            CreateWindow (CTRL_STATIC, 
                            "A white frame: ", 
                            WS_CHILD | WS_VISIBLE,
                            IDC_STATIC, 
                            170, 260, 100, 20, hWnd, 0);
            CreateWindow (CTRL_STATIC, 
                            "", 
                            WS_CHILD | SS_WHITEFRAME | WS_VISIBLE,
                            IDC_STATIC, 
                            260, 260, 50, 20, hWnd, 0);
            
            CreateWindow (CTRL_STATIC, 
                            "A Group Box", 
                            WS_CHILD | SS_GROUPBOX | WS_VISIBLE,
                            IDC_STATIC, 
                            350, 190, 160, 90, hWnd, 0);
            
            CreateWindow (CTRL_STATIC, 
                            "SS_LEFTNOWORDWRAP: "
                            "\tTabs are expanded, but words are not wrapped. "
                            "Text that extends past the end of a line is clipped.", 
                            WS_CHILD | SS_LEFTNOWORDWRAP | WS_VISIBLE | WS_BORDER,
                            IDC_STATIC, 
                            10, 290, 500, 30, hWnd, 0);

            CreateWindow (CTRL_BUTTON, 
                            "Close", 
                            WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 
                            IDCANCEL, 
                            450, 330, 60, 25, hWnd, 0);
        }
        break;

        case MSG_COMMAND:
            if (wParam == IDCANCEL)
                PostMessage (hWnd, MSG_CLOSE, 0, 0L);
            break;

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
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "Static controls";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlTestWinProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 520;
    pCreateInfo->by = 390;
    pCreateInfo->iBkColor = GetWindowElementPixel (HWND_NULL, WE_MAINC_THREED_BODY); 
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

void static_demo (HWND hwnd)
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

