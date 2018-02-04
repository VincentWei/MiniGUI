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
 * nr_edit.c
 * wangjian
 * 2008-03-03.
 */

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>


static int NrEditProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    int i;
    static MYBITMAP mybmp;
    static RGB *pal;
    HWND edit;
    switch (message) 
    {
        case MSG_CREATE:
            {
                pal = (RGB *)malloc (256 * sizeof (RGB));
                LoadMyBitmap (&mybmp, pal, "edit.bmp");
                for(i = 0; i < 256; ++i)
                {
                    if(pal[i].r == 0xFD && pal[i].g == 0xFF &&  pal[i].b == 0xFC)
                    {
                        mybmp.transparent = i;
                        break;
                    }
                }
                
                edit = CreateWindowEx(CTRL_MLEDIT, "",
                        WS_VISIBLE |  WS_BORDER | ES_BASELINE 
                        | ES_AUTOWRAP | ES_NOHIDESEL , 
                        WS_EX_NONE,
                        0,
                        ((480-mybmp.w)>>1), ((380-mybmp.h)>>1), mybmp.w, mybmp.h, 
                        hWnd, 0);
               SendMessage (hWnd, MSG_KEYDOWN, SCANCODE_INSERT, 0L);
         
#if 1
                if (!SetWindowMask(edit, &mybmp))
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
    JoinLayer(NAME_DEF_LAYER , "nr_edit" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION; 
    CreateInfo.dwExStyle = WS_EX_TROUNDCNS | WS_EX_BROUNDCNS;
    //CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "non-regular edit demo ...";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = NrEditProc;
    CreateInfo.lx = 10;
    CreateInfo.ty = 10;
    CreateInfo.rx = 510;
    CreateInfo.by = 410;
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

