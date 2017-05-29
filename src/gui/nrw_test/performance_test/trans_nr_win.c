/*
 * trans_nr_win.c
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


static int TransNonRegularWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
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
    JoinLayer(NAME_DEF_LAYER , "trans_non_regular" , 0 , 0);
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
    
    CreateInfo.dwStyle =  WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_TRANSPARENT;
    CreateInfo.spCaption = "transparent non-regular window ...";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = TransNonRegularWinProc;
    CreateInfo.lx = 400;
    CreateInfo.ty = 50;
    CreateInfo.rx = 700;
    CreateInfo.by = 250;
    CreateInfo.iBkColor = PIXEL_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
#if 1 
    if (!SetWindowMask(hMainWnd, &mybmp))
        return -1;
    InvalidateRect(hMainWnd, NULL, TRUE);
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

