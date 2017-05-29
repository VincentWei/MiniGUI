/*
** $Id$
**
** bmpf-test.c: Bitmap Font unit test.
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
#include <minigui/fixedmath.h>


MYBITMAP mybmp;
RGB *pal;

static int GlyphMyBitmapWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {

        case MSG_CREATE:
            pal = (RGB *) malloc (256 * sizeof(RGB));
            LoadMyBitmap (&mybmp, pal, "11.png");
            break;

        case MSG_PAINT:
            {
                hdc = BeginPaint (hWnd);
                FillBoxWithMyBitmap (hdc, 10, 10, &mybmp,pal);
		//PaintImageFromFile (hdc, 10, 60, "11.png");

                EndPaint (hWnd, hdc);
            }
            return 0;

        case MSG_KEYDOWN: {
                              break;
                          }

        case MSG_CLOSE:
                          DestroyMainWindow (hWnd);
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
    JoinLayer(NAME_DEF_LAYER , "fillboxwithmybitmap" , 0 , 0);
#endif
    pal = (RGB *)malloc (256 * sizeof (RGB));
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Glyph FillBoxWithMyBitmap test";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = GlyphMyBitmapWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    //CreateInfo.iBkColor = COLOR_blue;
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
    free (pal);
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

