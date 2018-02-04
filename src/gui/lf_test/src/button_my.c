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
** $Id: button.c,v 1.17 2007-12-06 02:24:59 xwyan Exp $
**
** button.c: The Button control demo program.
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

#define IDC_BUTTON          100
#define IDC_CHECKBOX        200
#define IDC_RADIOBUTTON     300

static char* bn_str [100];

#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (1, 2, 6)
static void my_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    printf ("From my_notif_proc: ID: %d, Code %s\n", id, bn_str[nc]);
}

#endif

static int ControlTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HWND hwnd;
    switch (message) {
        case MSG_CREATE:
            CreateWindow (CTRL_BUTTON,
                          "Push Button", 
                          WS_CHILD | BS_PUSHBUTTON | BS_CHECKED | WS_VISIBLE, 
                          IDC_BUTTON, 
                          10, 10, 80, 40, hWnd, 0);

            CreateWindow (CTRL_BUTTON, 
                          "Multiple Lines Push Button", 
                          WS_CHILD | BS_PUSHBUTTON | BS_MULTLINE | WS_VISIBLE, 
                          IDC_BUTTON + 1, 
                          110, 10, 80, 40, hWnd, 0);

            CreateWindow (CTRL_BUTTON, 
                          "Normal check box", 
                          WS_CHILD | BS_CHECKBOX | BS_CHECKED | WS_VISIBLE, 
                          IDC_BUTTON + 2, 
                          220, 10, 150, 40, hWnd, 0);

            CreateWindow (CTRL_BUTTON, 
                          "Normal radio button", 
                          WS_CHILD | BS_RADIOBUTTON | WS_VISIBLE, 
                          IDC_BUTTON + 3, 
                          390, 10, 150, 40, hWnd, 0);

            hwnd = CreateWindow (CTRL_BUTTON, 
                          "Auto 3-state check box", 
                          WS_CHILD | BS_AUTO3STATE | BS_PUSHLIKE|WS_VISIBLE | BS_NOTIFY, 
                          IDC_CHECKBOX, 
                          10, 60, 150, 30, hWnd, 0);

            CreateWindow (CTRL_BUTTON, 
                          "Auto check box on left", 
                          WS_CHILD | BS_AUTOCHECKBOX | BS_LEFTTEXT | BS_RIGHT | WS_VISIBLE, 
                          IDC_CHECKBOX + 1, 
                          190, 60, 150, 30, hWnd, 0);

            CreateWindow (CTRL_BUTTON, 
                          "Push-like 3state box", 
                          WS_CHILD | BS_AUTO3STATE | BS_PUSHLIKE | WS_VISIBLE, 
                          IDC_CHECKBOX + 2, 
                          390, 65, 110, 20, hWnd, 0);

            CreateWindow (CTRL_STATIC, 
                          "A Group Box", 
                          WS_CHILD | SS_GROUPBOX | WS_VISIBLE,
                          IDC_STATIC, 
                          10, 100, 180, 140, hWnd, 0);
            CreateWindow (CTRL_BUTTON, 
                          "Auto Radio Button 1", 
                          WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP,
                          IDC_RADIOBUTTON,
                          20, 120, 130, 30, hWnd, 0);
            CreateWindow (CTRL_BUTTON, 
                          "Auto Radio Button 2", 
                          WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE,
                          IDC_RADIOBUTTON + 1,
                          20, 160, 130, 30, hWnd, 0);
            CreateWindow (CTRL_BUTTON, 
                          "Auto Radio Button 3", 
                          WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE,
                          IDC_RADIOBUTTON + 2,
                          20, 200, 130, 30, hWnd, 0);

            CreateWindow (CTRL_STATIC, 
                          "A Group Box", 
                          WS_CHILD | SS_GROUPBOX | WS_VISIBLE,
                          IDC_STATIC, 
                          220, 100, 170, 140, hWnd, 0);
            CreateWindow (CTRL_BUTTON, 
                          "Auto Radio Button 1", 
                          WS_CHILD | BS_AUTORADIOBUTTON | BS_LEFTTEXT | BS_RIGHT | WS_VISIBLE | WS_GROUP,
                          IDC_RADIOBUTTON + 3,
                          230, 120, 140, 30, hWnd, 0);
            CreateWindow (CTRL_BUTTON, 
                          "Auto Radio Button 2", 
                          WS_CHILD | BS_AUTORADIOBUTTON | BS_LEFTTEXT | BS_RIGHT | WS_VISIBLE,
                          IDC_RADIOBUTTON + 4,
                          230, 160, 140, 30, hWnd, 0);
            CreateWindow (CTRL_BUTTON, 
                          "Auto Radio Button 3", 
                          WS_CHILD | BS_AUTORADIOBUTTON | BS_LEFTTEXT | BS_RIGHT | WS_VISIBLE,
                          IDC_RADIOBUTTON + 5,
                          230, 200, 140, 30, hWnd, 0);

            CreateWindow (CTRL_BUTTON, 
                          "Push-like Radio 1", 
                          WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_VISIBLE | WS_GROUP,
                          IDC_RADIOBUTTON + 3,
                          10, 260, 100, 30, hWnd, 0);
            CreateWindow (CTRL_BUTTON, 
                          "Push-like Radio 2", 
                          WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_VISIBLE,
                          IDC_RADIOBUTTON + 4,
                          120, 260, 100, 30, hWnd, 0);
            CreateWindow (CTRL_BUTTON, 
                          "Push-like Radio 3", 
                          WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_VISIBLE,
                          IDC_RADIOBUTTON + 5,
                          230, 260, 100, 30, hWnd, 0);

            CreateWindow (CTRL_BUTTON, 
                          "Close", 
                          WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON |BS_REALSIZEIMAGE | BS_NOTIFY | WS_VISIBLE, 
                          IDC_BUTTON + 4, 
                          340, 260, 100, 30, hWnd, (DWORD) GetLargeSystemIcon (IDI_APPLICATION));

            CreateWindow (CTRL_BUTTON, 
                          "Close", 
                          WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 
                          IDCANCEL, 
                          450, 260, 60, 30, hWnd, 0);


#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (1, 2, 6)
            SetNotificationCallback (hwnd, my_notif_proc);
#endif

        break;

        case MSG_KEYUP:
        {
            switch (wParam)
            {
                case SCANCODE_E:
                    SendMessage(GetDlgItem(hWnd, IDC_CHECKBOX), BM_ENABLE, 1, 0);
                    break;
                case SCANCODE_D:
                    SendMessage(GetDlgItem(hWnd, IDC_CHECKBOX), BM_ENABLE, 0, 0);
                    break;
                case SCANCODE_R:
                    SendMessage(GetDlgItem(hWnd, IDC_RADIOBUTTON), BM_ENABLE, 1, 0);
                    break;
                case SCANCODE_F:
                    SendMessage(GetDlgItem(hWnd, IDC_RADIOBUTTON), BM_ENABLE, 0, 0);
                    break;
                case SCANCODE_Q:
                    SendMessage(GetDlgItem(hWnd, IDC_BUTTON), BM_ENABLE, 1, 0);
                    break;
                case SCANCODE_A:
                    SendMessage(GetDlgItem(hWnd, IDC_BUTTON), BM_ENABLE, 0, 0);
                    break;
                default:
                    break;
            }
        }

        case MSG_COMMAND:
        {
            if (wParam == IDCANCEL) {
                PostMessage (hWnd, MSG_CLOSE, 0, 0);
            }
        }
        break;
       
        case MSG_DESTROY:
            DestroyAllControls (hWnd);
        return 0;

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            MainWindowCleanup (hWnd);
            return 0;
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

static void InitCreateInfo(PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "Button controls";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(1);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlTestWinProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 520;
    pCreateInfo->by = 350;
    pCreateInfo->iBkColor = GetWindowElementColor (WE_MAINC_THREED_BODY);
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

int button_demo (HWND hWnd)
{

    MSG Msg;
    HWND hMainWnd;

    bn_str [BN_CLICKED       ] = "BN_CLICKED       ";
    bn_str [BN_DOUBLECLICKED ] = "BN_DOUBLECLICKED ";
    bn_str [BN_DBLCLK        ] = "BN_DBLCLK        ";
    bn_str [BN_HILITE        ] = "BN_HILITE        ";
    bn_str [BN_UNHILITE      ] = "BN_UNHILITE      ";
    bn_str [BN_PUSHED        ] = "BN_PUSHED        ";
    bn_str [BN_UNPUSHED      ] = "BN_UNPUSHED      ";
    bn_str [BN_DISABLE       ] = "BN_DISABLE       ";
    bn_str [BN_ENABLE        ] = "BN_ENABLE        ";
    bn_str [BN_SETFOCUS      ] = "BN_SETFOCUS      ";
    bn_str [BN_KILLFOCUS     ] = "BN_KILLFOCUS     ";
    bn_str [BN_PAINT         ] = "BN_PAINT         ";

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif

    MAINWINCREATE CreateInfo;

    InitCreateInfo (&CreateInfo);
    CreateInfo.hHosting= hWnd;

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    return 0;
}

