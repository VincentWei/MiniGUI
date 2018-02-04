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

#define IDC_BUTTON 100

static BITMAP brush_bmp;
static STIPPLE stipple;

static int status = -1;

static int HelloWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {

        case MSG_CREATE:
            //CreateWindow (CTRL_BUTTON,
              //            "Button",
                //          WS_CHILD | BS_PUSHBUTTON | BS_CHECKED | WS_VISIBLE,
                  //        IDC_BUTTON,
                    //      10, 150, 50, 30, hWnd, 0);
            break;


        case MSG_LBUTTONDOWN:
            status = 0;
            RECT rc;
            rc.left = 0;
            rc.right = 50;
            rc.top = 0;
            rc.bottom = 10;
            fprintf (stderr, "lbuttondown down.\n");
            InvalidateRect (hWnd, NULL, TRUE);
        break;

        case MSG_RBUTTONDOWN:
            status = 1;
            InvalidateRect (hWnd, NULL, TRUE);
            fprintf (stderr, "rbuttondown down.\n");
            break;

        case MSG_LBUTTONUP:
            status = 0;
            ScrollWindow(hWnd, 10, 0, NULL, NULL);
            fprintf (stderr, "lbuttondown release.\n");
            InvalidateRect (hWnd, NULL, TRUE);
        break;

        case MSG_KEYDOWN:
            fprintf (stderr, "MGS_KEYDOWN: key %d\n", LOWORD(wParam));
            break;          

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            if (status == 0)
                TextOut (hdc, 0, 0, "LBUTTONDOWN occurs.");
            else if (status == 1)
                TextOut (hdc, 0, 0, "RBUTTONDOWN occurs.");


            SetBrushColor (hdc, RGB2Pixel(hdc, 0x00, 0x00, 0xFF));
            /*
            FillBox (hdc, 50, 50, 30, 30);

            SetBrushColor (hdc, RGB2Pixel (hdc, 0x00, 0xFF, 0x00));
            FillBox (hdc, 90, 50, 30, 30);

            SetBrushColor (hdc, RGB2Pixel (hdc, 0xFF, 0x00, 0x00));
            FillBox (hdc, 130, 50, 30, 30);

            SetBrushColor (hdc, RGB2Pixel (hdc, 0xFF, 0xFF, 0xFF));
            FillBox (hdc, 50, 90, 30, 30);

            SetBrushColor (hdc, RGB2Pixel (hdc, 0x00, 0x00, 0x00));
            FillBox (hdc, 90, 90, 30, 30);
            */

            //SetPenType(hdc, PT_DOUBLE_DASH);

            SetPenColor(hdc, COLOR_red);

            SetBrushInfo(hdc, &brush_bmp, &stipple);
            SetBrushType(hdc, BT_STIPPLED);

            SetPenType(hdc, PT_DOUBLE_DASH);
            SetPenDashes(hdc, 0, "\0\40", 2);
            SetPenWidth(hdc, 10);
            SetPenJoinStyle(hdc, PT_JOIN_ROUND);
            SetPenCapStyle(hdc, PT_CAP_PROJECTING);

            LineEx(hdc, 10,10, 100,10);
            LineEx(hdc, 100,10, 100,20);
            LineEx(hdc, 20,20, 50,50);
            //SetBrushColor(hdc, COLOR_red);

            RECT border_rc;
            SetRect(&border_rc, 60, 60, 400,400);
           // IncludeClipRect(hdc, &border_rc);
            SetRect(&border_rc, 60, 60, 400,400);
            ExcludeClipRect(hdc, &border_rc);
            RoundRect(hdc, 60,60, 400,400, 0,0);
          //  Rectangle(hdc, 70, 70, 410, 410);
               
            EndPaint (hWnd, hdc);
            return 0;

        case MSG_MOUSEMOVE:
            {
                int x_pos = LOSWORD (lParam);
                int y_pos = HISWORD (lParam);
                printf("mouse in <%d, %d>\n", x_pos, y_pos);
                break;
            }

        case MSG_CLOSE:
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
    LoadBitmap(HDC_SCREEN, &brush_bmp, "fall_night.jpg");
    stipple.width = 8;
    stipple.height = 8;
    stipple.pitch = 1;
    stipple.size = 8;
    stipple.bits = "\x81\x42\x24\x18\x18\x24\x42\x81";


#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif

    fprintf (stderr, "MiniGUIMain is run. \n");
    CreateInfo.dwStyle = WS_VISIBLE | WS_MAXIMIZEBOX| WS_MINIMIZEBOX|WS_THINFRAME | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "hello";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = HelloWinProc;
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

    /*
    TextOut(HDC_SCREEN, 0, 0, "fdfda");
    while (GetMessage(&Msg, HWND_DESKTOP)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    // */

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

