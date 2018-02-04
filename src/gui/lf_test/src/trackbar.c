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
** $Id: trackbar.c,v 1.10 2007-12-06 02:25:00 xwyan Exp $
**
** trackbar.c: The TrackBar control demo program.
**
** Copyright (C) 2001 ~ 2002 Wei Yongming.
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Create date: 2001/11/01
*/

/*
**  This source is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public
**  License as published by the Free Software Foundation; either
**  version 2 of the License, or (at your option) any later version.
**
**  This software is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public
**  License along with this library; if not, write to the Free
**  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
**  MA 02111-1307, USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define IDC_TRACKBAR1     100
#define IDC_TRACKBAR2     101

static HWND hMainWnd = HWND_INVALID;
static HWND hTrackWnd1, hTrackWnd2, hShowWnd;
static int MaxLength = 1000;
static int CurrentPos = 0;
static char CurPosBuffer[16];

static void TrackbarChanged(int code, HWND ctrl)
{
	switch (code)
	{
	    case TBN_REACHMAX:
	    	CurrentPos = MaxLength;
	    	sprintf(CurPosBuffer, "%d", CurrentPos);
            SendMessage (hShowWnd,  MSG_SETTEXT, 0, (LPARAM)CurPosBuffer);
	    break;

	    case TBN_REACHMIN:
	    	CurrentPos = 0;
	    	sprintf(CurPosBuffer, "%d", CurrentPos);
            SendMessage (hShowWnd,  MSG_SETTEXT, 0, (LPARAM)CurPosBuffer);
	    break;

	    case TBN_CHANGE:
	    	CurrentPos = SendMessage (hTrackWnd1, TBM_GETPOS, 0, 0);
	    	sprintf(CurPosBuffer, "%d", CurrentPos);
            SendMessage (hShowWnd,  MSG_SETTEXT, 0, (LPARAM)CurPosBuffer);
	    break;

	    default:
	    break;
	}
}

static int ControlTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            hShowWnd = CreateWindow (CTRL_STATIC, 
                              "", 
                              WS_VISIBLE, 
                              IDC_STATIC, 
                              10, 20, 60, 20, hWnd, 0);

            hTrackWnd1 = CreateWindow (CTRL_TRACKBAR, 
                              "", 
                              WS_VISIBLE | TBS_NOTIFY, 
                              IDC_TRACKBAR1, 
                              10, 40, 180, 50, hWnd, 0);
            SendMessage (hTrackWnd1, TBM_SETRANGE, 0, 1000);
            SendMessage (hTrackWnd1, TBM_SETLINESIZE, 1, 0);
            SendMessage (hTrackWnd1, TBM_SETPAGESIZE, 10, 0);
            SendMessage (hTrackWnd1, TBM_SETTICKFREQ, 1000, 0);

            hTrackWnd2 = CreateWindow (CTRL_TRACKBAR, 
                              "", 
                              WS_VISIBLE | TBS_NOTIFY | TBS_TIP, 
                              IDC_TRACKBAR1 + 1, 
                              200, 40, 180, 50, hWnd, 0);
            SendMessage (hTrackWnd2, TBM_SETTIP, (WPARAM)"Min", (LPARAM)"Max");

            hTrackWnd2 = CreateWindow (CTRL_TRACKBAR, 
                              "", 
                              WS_VISIBLE | TBS_NOTIFY | TBS_VERTICAL, 
                              IDC_TRACKBAR1 + 2, 
                              10, 100, 50, 200, hWnd, 0);
            CreateWindow (CTRL_TRACKBAR, 
                              "", 
                              WS_VISIBLE | TBS_NOTIFY | TBS_VERTICAL | TBS_NOTICK, 
                              IDC_TRACKBAR1 + 3, 
                              200, 100, 50, 200, hWnd, 0);

            SendMessage (hTrackWnd2, TBM_SETRANGE, 0, 10000);
            SendMessage (hTrackWnd2, TBM_SETLINESIZE, 1, 0);
            SendMessage (hTrackWnd2, TBM_SETPAGESIZE, 10, 0);
            SendMessage (hTrackWnd2, TBM_SETTICKFREQ, 10000, 0);

            CurrentPos = 0;
            sprintf (CurPosBuffer, "%d", CurrentPos);
            SendMessage (hShowWnd,  MSG_SETTEXT, 0, (LPARAM)CurPosBuffer);
        break;

        case MSG_COMMAND:
        {
            int id   = LOWORD(wParam);
            int code = HIWORD(wParam);
            HWND ctrl = (HWND)(lParam);
            char buffer [256];
            HDC hdc;

            sprintf (buffer, "ID: %d, Code: %x", id, code);

            hdc = GetClientDC (hWnd);
            TextOut (hdc, 0, 0, buffer);
            ReleaseDC (hdc);

            if (id == IDC_TRACKBAR1)
                TrackbarChanged(code, ctrl);
        }
        break;

	case MSG_DESTROY:
            DestroyAllControls (hWnd);
            hMainWnd = HWND_INVALID;
	    return 0;

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            MainWindowCleanup (hWnd);
            return 0;

        default:
            break;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitCreateInfo(PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "TrackBar controls" ;
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(IDC_ARROW);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlTestWinProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 460;
    pCreateInfo->by = 380;
    pCreateInfo->iBkColor = GetWindowElementColor (WE_MAINC_THREED_BODY);
    pCreateInfo->dwAddData = 0;
    pCreateInfo->dwReserved = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

void trackbar_demo (HWND hwnd)
{
    MAINWINCREATE CreateInfo;

    if (hMainWnd != HWND_INVALID) {
        ShowWindow (hMainWnd, SW_SHOWNORMAL);
        return;
    }

    InitCreateInfo (&CreateInfo);
    CreateInfo.hHosting = hwnd;

    hMainWnd = CreateMainWindow (&CreateInfo);
}

