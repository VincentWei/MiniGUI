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
** about.c: the About Dialog module.
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
            DestroyMainWindow (hWnd);
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

