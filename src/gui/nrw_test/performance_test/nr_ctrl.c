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
 * nrctrldemo.c
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

static char *cb_text[] = 
{
    "AAAAA",
    "BBBBB",
    "CCCCC",
    "DDDDD",
    "EEEEE",
    "FFFFF",
    "GGGGG",
    "HHHHH"
};

static int NrDemoProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    int i;
    static MYBITMAP mybmp;
    static RGB *pal;
    HWND btn1, btn2, cbb;
    switch (message) 
    {
        case MSG_CREATE:
            {
                pal = (RGB *)malloc (256 * sizeof (RGB));
                LoadMyBitmap (&mybmp, pal, "btn.bmp");
                for(i = 0; i < 256; ++i)
                {
                    if(pal[i].r == 0xFC && pal[i].g == 0xFF &&  pal[i].b == 0xFB)
                    {
                        mybmp.transparent = i;
                        break;
                    }
                }
                
                btn1 = CreateWindowEx(CTRL_BUTTON, "Normal",
                        WS_VISIBLE | BS_DEFPUSHBUTTON, 
                        WS_EX_TROUNDCNS,
                        0,
                        0, 10, mybmp.w, mybmp.h, 
                        hWnd, 0);
                btn2 = CreateWindowEx(CTRL_BUTTON, "Tansarent",
                        WS_VISIBLE | BS_DEFPUSHBUTTON, 
                        WS_EX_TRANSPARENT,
                        0,
                        mybmp.w+10, 10, mybmp.w, mybmp.h, 
                        hWnd, 0);
                cbb = CreateWindowEx(CTRL_COMBOBOX, "",
                        WS_VISIBLE | CBS_SIMPLE | CBS_NOTIFY,
                        WS_EX_TROUNDCNS,
                        0,
                        10, 110, 60, 25, 
                        hWnd, 80);
                for (i = 0; i < 7; i++) {
                    SendMessage(cbb, CB_ADDSTRING, 0, (LPARAM)cb_text[i]);
                }
#if 1
                if (!SetWindowMask(btn1, &mybmp))
                    return -1;
                if (!SetWindowMask(btn2, &mybmp))
                    return -1;
                if (!SetWindowMask(cbb, &mybmp))
                    return -1;
#endif


                return 0;
            }
        
        case MSG_CLOSE:
            {
                UnloadMyBitmap(&mybmp);
                free(pal);
                DestroyMainWindow (hWnd);
                PostQuitMessage (hWnd);
                return 0;
            }
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "nr demo" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION; 
    CreateInfo.dwExStyle = WS_EX_TROUNDCNS | WS_EX_BROUNDCNS;
    //CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "non-regular control demo ...";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = NrDemoProc;
    CreateInfo.lx = 10;
    CreateInfo.ty = 10;
    CreateInfo.rx = 310;
    CreateInfo.by = 310;
    CreateInfo.iBkColor = PIXEL_green;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow (hMainWnd, SW_SHOWNORMAL);

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

