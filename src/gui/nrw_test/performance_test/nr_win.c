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
 * nr_win.c
 * wangjian
 * 2008-02-19.
 */

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>


static int NonRegularWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message) 
    {
        case MSG_CREATE:
            {
                CreateWindowEx(CTRL_BUTTON, "Normal",
                        WS_VISIBLE | BS_DEFPUSHBUTTON, 
                        WS_EX_NONE,
                        0,
                        0, 10, 80, 30, 
                        hWnd, 0);
                CreateWindowEx(CTRL_BUTTON, "Tansarent",
                        WS_VISIBLE | BS_DEFPUSHBUTTON, 
                        WS_EX_TRANSPARENT,
                        0,
                        100, 10, 80, 30, 
                        hWnd, 0);


                return 0;
            }
        
        case MSG_PAINT:
            {
                hdc = BeginPaint (hWnd);

                EndPaint (hWnd, hdc);
                return 0;
            }

        case MSG_CLOSE:
            {
                DestroyMainWindow (hWnd);
                PostQuitMessage (hWnd);
                return 0;
            }
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    int i;
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
    MYBITMAP mybmp;
    RGB *pal;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "non regular" , 0 , 0);
#endif

    pal = (RGB *)malloc (256 * sizeof (RGB));
    LoadMyBitmap (&mybmp, pal, "11.bmp");
    for(i = 0; i < 256; ++i)
    {
        if(pal[i].r == 0xFC && pal[i].g == 0xFF &&  pal[i].b == 0xFB)
        {
            mybmp.transparent = i;
            break;
        }
    }
    
    //CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwStyle = WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "non-regular window ...";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = NonRegularWinProc;
    CreateInfo.lx = 50;
    CreateInfo.ty = 50;
    CreateInfo.rx = 350;
    CreateInfo.by = 250;
    CreateInfo.iBkColor = PIXEL_cyan;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
#if 1
    if (!SetWindowMask(hMainWnd, &mybmp))
        return -1;
#endif
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow (hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    UnloadMyBitmap(&mybmp);
    free(pal);
    MainWindowThreadCleanup (hMainWnd);
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

