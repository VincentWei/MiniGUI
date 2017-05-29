/*
 * normalwin.c
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


static int NormalWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static int left = 0;
    static int top = 0;
    
    switch (message) 
    {
        case MSG_CREATE:
            {
                SetTimer(hWnd, 101, 50);
                return 0;
            }
        case MSG_TIMER:
            {
                if (wParam != 101)
                    return 0;
                left += 20;
                if(left >= 340) left = 0;
                top += 10;
                if(top >= 280) top = 0;
                MoveWindow(hWnd, left, top, 300, 200, TRUE);
                break;
            }
        
        case MSG_PAINT:
            {
                hdc = BeginPaint (hWnd);

                TextOut(hdc, 20, 20, "I am moveing...");

                EndPaint (hWnd, hdc);
                return 0;
            }

        case MSG_CLOSE:
            {
                KillTimer (hWnd, 101);
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
    JoinLayer(NAME_DEF_LAYER , "normal" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "normal window ...";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = NormalWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 300;
    CreateInfo.by = 200;
    CreateInfo.iBkColor = PIXEL_yellow;
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

