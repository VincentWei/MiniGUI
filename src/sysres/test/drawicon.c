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

#define _MGLF_RDR_FLAT 1

static char* icon_names [] = {
    "Icon_211.ico",
#ifdef _MGLF_RDR_FLAT
    "excalmatory-flat.ico", 
#endif
    "excalmatory.ico", 
#ifdef _MGLF_RDR_FLAT
    "failed-flat.ico",
#endif
    "failed.ico", 

#ifdef _MGLF_RDR_FLAT
    "fold-flat.ico", 
#endif
    "fold.ico", 

#ifdef _MGLF_RDR_FLAT
    "folder-flat.ico", 
#endif
    "folder.ico", 

#ifdef _MGLF_RDR_FLAT
    "form-flat.ico", 
#endif
    "form.ico", 
};

static HICON myicons_small [27], myicons_large[27];

static int DrawiconWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
    case MSG_CREATE: {
        int i = 0;
        for (i = 0; i < TABLESIZE(icon_names); i++) {
            myicons_small[i] = LoadIconFromRes (HDC_SCREEN, icon_names[i], 1);
            if (myicons_small[i] == 0)
                fprintf (stderr, "Load icon file failure!");

            myicons_large[i] = LoadIconFromRes(HDC_SCREEN, icon_names[i], 0);
            if (myicons_large[i] == 0)
                fprintf (stderr, "Load icon file failure!");
        }
        break;
    }
#define ICON_W  50
#define ICON_TOP_M 10
#define ICON_H  50


    case MSG_PAINT: {
        int i;
        hdc = BeginPaint(hWnd);
        for (i = 0; i < TABLESIZE(icon_names); i++) {
            if (myicons_small[i] != 0)
                DrawIcon(hdc, 0+i*ICON_W, ICON_TOP_M, 0, 0, myicons_small[i]);
            if (myicons_large[i] != 0)
                DrawIcon(hdc, 0+i*ICON_W, ICON_TOP_M+ICON_H, 0, 0, myicons_large[i]);
        }
        EndPaint(hWnd, hdc);
        return 0;
    }
    case MSG_CLOSE: {
        int i = 0;
        for (i = 0; i < TABLESIZE(icon_names); i++)
        {
            DestroyIcon(myicons_small[i]);
            DestroyIcon(myicons_large[i]);
        }
        DestroyMainWindow (hWnd);
        PostQuitMessage (hWnd);
        return 0;
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
    CreateInfo.hCursor = GetSystemCursor(0);
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

