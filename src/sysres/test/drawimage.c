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

static char* image_names [] = {
    "MiniGUI256.bmp", 
#ifdef _MGLF_RDR_TINY
    "bg_tiny.bmp",
#endif
    "classic_check_button.bmp", 
    "classic_radio_button.bmp", 
#ifdef _MGLF_RDR_FASHION
    "fashion_check_btn.bmp", 
    "fashion_radio_btn.bmp", 
#endif
#if defined (_MGLF_RDR_FLAT) || defined (_MGLF_RDR_TINY)
    "flat_check_button.bmp", 
    "flat_check_button.bmp", 
#endif
    "shurufa-flat.bmp",

#ifdef _MGLF_RDR_SKIN
    "skin_arrows.gif", 
    "skin_arrows_shell.bmp", 
    "skin_bborder.bmp", 
    "skin_bkgnd.bmp", 
    "skin_caption.gif", 
    "skin_checkbtn.bmp",  
    "skin_cpn_btn.gif", 
    "skin_header.bmp", 
    "skin_lborder.bmp",  
    "skin_pb_hchunk.bmp", 
    "skin_pb_htrack.gif", 
    "skin_pb_vchunk.bmp", 
    "skin_pb_vtrack.bmp", 
    "skin_pushbtn.gif", 
    "skin_radiobtn.bmp",  
    "skin_rborder.bmp", 
    "skin_sb_arrows.bmp", 
    "skin_sb_hshaft.bmp", 
    "skin_sb_hthumb.bmp", 
    "skin_sb_vshaft.bmp", 
    "skin_sb_vthumb.bmp", 
    "skin_tab.bmp", 
    "skin_tb_horz.gif", 
    "skin_tb_vert.gif", 
    "skin_tborder.bmp", 
    "skin_tbslider_h.bmp", 
    "skin_tbslider_v.bmp", 
    "skin_tree.bmp", 

#endif
    "test_error.bmp"


};

static PBITMAP bmps[TABLESIZE(image_names)];
static int left = 0, top = 0;

static int DrawiconWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
    case MSG_CREATE: {
        int i = 0;
        for (i = 0; i < TABLESIZE(image_names); i++) {
            bmps[i] = LoadBitmapFromRes (HDC_SCREEN, image_names[i]);
            if (!bmps[i])
                fprintf (stderr, "Load image failure!");

        }
        break;
    }

    case MSG_PAINT: {
        int i;
        hdc = BeginPaint(hWnd);
        for (i = 0; i < TABLESIZE(image_names); i++) {
            if (bmps[i] != 0){
                if (left+bmps[i]->bmWidth > g_rcScr.right) {
                    left = 0;
                    top += bmps[i]->bmHeight; 
                }
                
                FillBoxWithBitmap(hdc, left , top, 0, 0, bmps[i]);
                left+=bmps[i]->bmWidth;
            }
            
        }
        EndPaint(hWnd, hdc);
        return 0;
    }
    case MSG_CLOSE: {
        int i = 0;
        for (i = 0; i < TABLESIZE(image_names); i++)
        {
            UnloadBitmap(bmps[i]);
            free (bmps[i]);
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
    JoinLayer(NAME_DEF_LAYER , "test-image" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Test of image res";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = DrawiconWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 1024;
    CreateInfo.by = 768;
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

