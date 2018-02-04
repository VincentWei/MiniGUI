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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define CURSOR_NUM 22
#define IDC_ERROR -1 
#define IDC_OVERFLOW 100

static int cursors [] = {
    IDC_ARROW,
    IDC_IBEAM,
    IDC_PENCIL,
    IDC_CROSS,
    IDC_MOVE,
    IDC_SIZENWSE,
    IDC_SIZENESW,
    IDC_SIZEWE,
    IDC_SIZENS,
    IDC_UPARROW,
    IDC_NONE,
    IDC_HELP,
    IDC_BUSY,
    IDC_WAIT,
    IDC_RARROW,
    IDC_COLOMN,
    IDC_ROW,
    IDC_DRAG,
    IDC_NODROP,
    IDC_HAND_POINT,
    IDC_HAND_SELECT,
    IDC_SPLIT_HORZ,
    IDC_SPLIT_VERT,
    IDC_ERROR,
    IDC_OVERFLOW
};

static char* cursor_names [] = {
    "ARROW",
    "IBEAM",
    "PENCIL",
    "CROSS",
    "MOVE",
    "SIZENWSE",
    "SIZENESW",
    "SIZEWE",
    "SIZENS",
    "UPARROW",
    "NONE",
    "HELP",
    "BUSY",
    "WAIT",
    "RARROW",
    "COLOMN",
    "ROW",
    "DRAG",
    "NODROP",
    "HAND_POINT",
    "HAND_SELECT",
    "SPLIT_HORZ",
    "SPLIT_VERT"
};

static int cursor_id = 0;
static HCURSOR hcursors [TABLESIZE(cursors)];

static int DrawiconWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{

    switch (message) {
    case MSG_CREATE: {
        int i;
        for (i = 0; i < TABLESIZE(cursors); i++)
            hcursors[i] = LoadCursorFromRes (cursors[i]);
        break;
    }

    case MSG_LBUTTONDOWN:
        if (++cursor_id>=TABLESIZE(cursors))
            cursor_id = 0;

        printf ("current cursor is: %s. \n", cursor_names[cursor_id]);
        break;

    case MSG_SETCURSOR: 
        SetCursor (hcursors[cursor_id]);
        return 0;

    case MSG_CLOSE: {
        int i;
        for (i = 0; i < TABLESIZE(cursors); i++)
            DestroyCursor (hcursors[i]);
        
        ;
    }

   }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int args, const char* arg[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "drawicon" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Draw icon";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(IDC_CROSS);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = DrawiconWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 800;
    CreateInfo.by = 600;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

