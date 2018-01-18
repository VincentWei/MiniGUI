/*
** $Id: about.c 13674 2010-12-06 06:45:01Z wanzheng $
**
** about.c: the About Dialog module.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
** 
** Current maintainer: Wei Yongming.
**
** Create date: 1999/8/28
*/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#ifdef _MGMISC_ABOUTDLG

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"

static HWND sg_AboutWnd = 0;

static LRESULT AboutWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT    rcClient;

    switch (message) {
        case MSG_CREATE:
            sg_AboutWnd = hWnd;
            GetClientRect (hWnd, &rcClient);
            CreateWindow ("button", "Close", 
                                WS_CHILD | BS_DEFPUSHBUTTON | WS_VISIBLE, 
                                IDOK, 
                                (RECTW(rcClient) - 80)>>1, 
                                rcClient.bottom - 40, 
                                80, 24, hWnd, 0);
            break;
        
        case MSG_COMMAND:
            if (LOWORD (wParam) == IDOK && HIWORD (wParam) == BN_CLICKED)
                PostMessage (hWnd, MSG_CLOSE, 0, 0);
            break;
       
        case MSG_KEYDOWN:
            if (LOWORD (wParam) == SCANCODE_ESCAPE)
                PostMessage (hWnd, MSG_CLOSE, 0, 0);
            break;

        case MSG_PAINT:
        {
            HDC hdc;
            
            hdc = BeginPaint (hWnd);
            GetClientRect (hWnd, &rcClient);
            rcClient.top = 30;
            rcClient.bottom -= 50;
            rcClient.left = 10;
            rcClient.right -= 10;
            SetTextColor (hdc, PIXEL_black);
            SetBkMode (hdc, BM_TRANSPARENT);
            DrawText (hdc, 
                    "MiniGUI -- a mature cross-platform windowing system "
                    "and GUI support system for embedded or IoT devices.\n\n"
                    "Copyright (C) 2002 ~ 2018 FMSoft Co., Ltd.",
                    -1, &rcClient, DT_WORDBREAK | DT_CENTER);

            EndPaint (hWnd, hdc);
            return 0;
        }

        case MSG_CLOSE:
            sg_AboutWnd = 0;
            DestroyAllControls (hWnd);
#ifdef _MGRM_THREADS
            PostQuitMessage (hWnd);
#endif
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitAboutDialogCreateInfo (PMAINWINCREATE pCreateInfo, char* caption)
{
    sprintf (caption, "About MiniGUI (Ver %d.%d.%d)", 
                    MINIGUI_MAJOR_VERSION,
                    MINIGUI_MINOR_VERSION,
                    MINIGUI_MICRO_VERSION);

    pCreateInfo->dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE 
                                      | WS_BORDER;
    pCreateInfo->dwExStyle = WS_EX_TOPMOST;
    pCreateInfo->spCaption = caption;
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor (0);
    pCreateInfo->hIcon = GetSmallSystemIcon (0);
    pCreateInfo->MainWindowProc = AboutWinProc;
    pCreateInfo->lx = 10; 
    pCreateInfo->ty = 5;
    if (GetSysCharWidth () == 6) {
        pCreateInfo->rx = 260;
        pCreateInfo->by = 215;
    }
    else {
        pCreateInfo->rx = 340;
        pCreateInfo->by = 240;
    }
    pCreateInfo->iBkColor = PIXEL_lightwhite; 
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

#ifdef _MGRM_THREADS
static void* AboutDialogThread (void* data)
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;
    char caption [256];

    InitAboutDialogCreateInfo (&CreateInfo, caption);

    hMainWnd = CreateMainWindow(&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return NULL;

    ShowWindow (hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        DispatchMessage(&Msg);
    }

    DestroyMainWindow (hMainWnd);
    MainWindowThreadCleanup(hMainWnd);
    return NULL;
}

void GUIAPI OpenAboutDialog (void)
{
    pthread_t thread;
    
    if (sg_AboutWnd != 0) {
        ShowWindow (sg_AboutWnd, SW_SHOWNORMAL);
        return;
    }

    CreateThreadForMainWindow (&thread, NULL, AboutDialogThread, 0);
}

#else

HWND GUIAPI OpenAboutDialog (HWND hHosting)
{
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;
    char caption [256];

    if (sg_AboutWnd != 0) {
        ShowWindow (sg_AboutWnd, SW_SHOWNORMAL);
        return sg_AboutWnd;
    }

    InitAboutDialogCreateInfo (&CreateInfo, caption);
    CreateInfo.hHosting = hHosting;

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return HWND_INVALID;

    ShowWindow (hMainWnd, SW_SHOWNORMAL);
    return hMainWnd;
}

#endif /* _MGRM_THREADS */

#endif /* _MGMISC_ABOUTDLG */

