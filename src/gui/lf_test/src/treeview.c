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
** $Id: treeview.c,v 1.11 2007-12-06 02:25:00 xwyan Exp $
**
** treeview.c: The TreeView control demo program.
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
#include <minigui/mgext.h>

#define IDC_CTRL1     100
#define IDC_CTRL2     110
#define IDC_CTRL3     120
#define IDC_CTRL4     130
#define IDC_CTRL5     140
#define IDC_CTRL6     150

static HWND hMainWnd = HWND_INVALID;

static int ControlTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    static HWND hChildWnd1,hChildWnd2,hChildWnd3,hChildWnd4,hChildWnd5,hChildWnd6;
    static TVITEMINFO tvItemInfo;

    switch (message) {
        case MSG_CREATE:
        {
            int i = 0;
            GHANDLE item;
            hChildWnd1 = CreateWindow (CTRL_TREEVIEW, "", 
                    WS_VISIBLE | WS_VSCROLL | WS_HSCROLL
                    | WS_BORDER | TVS_SORT, 
                    IDC_CTRL1, 
                    10, 20, 320, 150, hWnd, 0);
            hChildWnd2 = CreateWindow ("Edit",
                    "Add a child to current node.",
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    IDC_CTRL2,
                    10, 190, 250, 25, hWnd, 0);
            hChildWnd3 = CreateWindow ("Edit",
                    "Find a node in treeview.",
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    IDC_CTRL3,
                    10, 230, 250, 25, hWnd, 0);
            hChildWnd4 = CreateWindow ("button",
                    "Add",
                    WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                    IDC_CTRL4,
                    270, 190, 60, 20, hWnd, 0);
            hChildWnd5 = CreateWindow ("button",
                    "Del",
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    IDC_CTRL5,
                    270, 280, 60, 20, hWnd, 0);
            hChildWnd6 = CreateWindow ("button",
                    "Find",
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    IDC_CTRL6,
                    270, 230, 60, 20, hWnd, 0);
            
            tvItemInfo.text = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
            item = SendMessage (hChildWnd1, TVM_ADDITEM, 0, (LPARAM) &tvItemInfo);
            
            tvItemInfo.text = "b";
            for (i = 0; i <= 10; i++)
                SendMessage (hChildWnd1, TVM_ADDITEM, 0, (LPARAM) &tvItemInfo);
            
            tvItemInfo.text = "ccccccccccc";
            SendMessage (hChildWnd1, TVM_ADDITEM, 0, (LPARAM) &tvItemInfo);

            tvItemInfo.text = "a.a";
            SendMessage (hChildWnd1, TVM_ADDITEM, (WPARAM) item, (LPARAM) &tvItemInfo);
            tvItemInfo.text = "a.b";
            SendMessage (hChildWnd1, TVM_ADDITEM, (WPARAM) item, (LPARAM) &tvItemInfo);
        }
        break;

        case MSG_COMMAND:
        {
            int id   = LOWORD(wParam);
            //int code = HIWORD(wParam);
            char buffer[255];
            GHANDLE item;

            switch (id) {
            case IDC_CTRL4:
                SendMessage (hChildWnd2, MSG_GETTEXT, 99, (LPARAM) buffer);
                if (buffer[0] != '\0') {
                    tvItemInfo.text = buffer;
                    item = SendMessage (hChildWnd1, TVM_GETSELITEM, 0, 0);
                    SendMessage (hChildWnd1, TVM_ADDITEM, (WPARAM)item, (LPARAM) &tvItemInfo);
                }
                break;
                        
                case IDC_CTRL5:
                    SendMessage (hChildWnd1, TVM_DELTREE, 0, 0);
                break;
                
                case IDC_CTRL6:
                {
                    SendMessage (hChildWnd3, MSG_GETTEXT, 99, (LPARAM) buffer);
                    if (buffer[0] != '\0') {
                        item = SendMessage (hChildWnd1, TVM_SEARCHITEM, 0, (LPARAM) buffer);
                        if (item > 0) {
                            SendMessage (hChildWnd1, TVM_SETSELITEM, (WPARAM)item, 0L);
                        }
                    }
                }
                break;
                
                default:
                break;
            }
        }
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
    pCreateInfo->dwExStyle = WS_EX_IMECOMPOSE;
    pCreateInfo->spCaption = "TreeView controls" ;
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlTestWinProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 400;
    pCreateInfo->by = 300;
    pCreateInfo->iBkColor = GetWindowElementColor (WE_MAINC_THREED_BODY);
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

void treeview_demo (HWND hwnd)
{
    MAINWINCREATE CreateInfo;

    if (hMainWnd != HWND_INVALID) {
        ShowWindow (hMainWnd, SW_SHOWNORMAL);
        return;
    }

    InitCreateInfo(&CreateInfo);
    CreateInfo.hHosting = hwnd;

    hMainWnd = CreateMainWindow(&CreateInfo);
}

