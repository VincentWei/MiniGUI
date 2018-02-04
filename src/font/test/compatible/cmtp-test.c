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
** $Id$
**
** cmpt-test.c: Bitmap Font dc compatible test.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2007/09/23
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#define BMP_FILE_DIGIT    "digits1.png"

static LOGFONT *logfont, *old ; 
static DEVFONT *dev_font;

static BITMAP digit_bmp;

static HDC mem_dc;

static int BmpfCmptTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {

        case MSG_CREATE:

            mem_dc = CreateMemDC(30, 15, 8, MEMDC_FLAG_SWSURFACE,
                                 0x000000e0, 0x0000001c, 0x00000003, 0x0000000f);
            
            if (LoadBitmap (mem_dc, &digit_bmp, BMP_FILE_DIGIT)) {
                fprintf (stderr, "Fail to load bitmap. \n");
                return 1;
            }


            dev_font = CreateBMPDevFont ("bmp-led-rrncnn-10-15-ISO8859-1", 
                           &digit_bmp, "0", 10, 10);

            logfont = CreateLogFont (FONT_TYPE_NAME_BITMAP_BMP, "led", 
                          "ISO8859-1",
                          FONT_WEIGHT_BOLD, FONT_SLANT_ITALIC, 
                          FONT_SETWIDTH_NORMAL, FONT_SPACING_CHARCELL, 
                          FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
                          10, 0);
        break;

        case MSG_PAINT:
            
            hdc = BeginPaint (hWnd);

#if 0            
            FillBoxWithBitmap (mem_dc, 0, 0, 10, 15, &digit_bmp);
#endif
            SelectFont (mem_dc, logfont);
            TextOut (mem_dc, 0, 0, "123");
            BitBlt(mem_dc, 0, 0, 0, 0, hdc, 30, 15, 0);

            EndPaint (hWnd, hdc);
            return 0;

        case MSG_CLOSE:
            UnloadBitmap (&digit_bmp);
 
            if (dev_font != NULL)
                DestroyBMPFont (dev_font);
            
            DestroyLogFont (logfont);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}


int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "bmpf compatible test" , 0 , 0);
#endif


    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "bmpf compatible test";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = BmpfCmptTestWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
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

#ifdef _MG_THREADS
#include <minigui/dti.c>
#endif

