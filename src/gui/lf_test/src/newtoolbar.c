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
** $Id: newtoolbar.c,v 1.15 2007-12-03 03:39:57 xwyan Exp $
**
** newtoolbar.c: The NewToolBar control demo program.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Create date: 2003/5/15
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
#ifndef __NOUNIX__
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>


#define LEN_TEXT    50
#define LEN_TIP     50

#define IDC_CTRL_BUTTON       100
#define IDC_CTRL_NEWTOOLBAR_1 110
#define IDC_CTRL_NEWTOOLBAR_2 120
#define IDC_CTRL_NEWTOOLBAR_3 130
#define IDC_CTRL_NEWTOOLBAR_4 140

#define IDC_NTB_ONE           140
#define IDC_NTB_TWO           150
#define IDC_NTB_THREE         160
#define IDC_NTB_FOUR          170


static HWND hMainWnd = HWND_INVALID;
RECT rect={0,330,500,400};
static void my_hotspot_proc (HWND hwnd, int id, const RECT* cell, int x, int y)
{
        HDC hdc;
        char buff [254];

        Ping ();
        memset (buff, 0, sizeof (buff));
        hdc = GetClientDC (GetParent (hwnd));
        InvalidateRect(hwnd,&rect,TRUE);
        memset(buff,0,sizeof(buff));
        sprintf (buff, "Hotpot of button %d of NewToolBar %d clicked.", id, GetDlgCtrlID (hwnd));
        TextOut (hdc, 0, 330, buff);
        ReleaseDC (hdc);
}

BITMAP bitmap1, bitmap2, bitmap3, bitmap4;

static void create_newtoolbars (HWND hWnd)
{
    HWND ntb1, ntb2, ntb3, ntb4;
    NTBINFO ntb_info;
    NTBITEMINFO ntbii;
    gal_pixel pixel;
    RECT hotspot = {16, 16, 32, 32};

    ntb_info.nr_cells = 4;
    ntb_info.w_cell  = 0;
    ntb_info.h_cell  = 0;
    ntb_info.nr_cols = 0;
    ntb_info.image = &bitmap1;

    ntb1 = CreateWindow (CTRL_NEWTOOLBAR,
                    "",
                    WS_CHILD | WS_VISIBLE, 
                    IDC_CTRL_NEWTOOLBAR_1,
                    0, 10, 1024, 0,
                    hWnd,
                    (DWORD) &ntb_info);
    pixel = GetPixelInBitmap (&bitmap1, 0, 0);
    SetWindowBkColor (ntb1, pixel);
    InvalidateRect (ntb1, NULL, TRUE);

    ntb_info.nr_cells = 4;
    ntb_info.w_cell  = 0;
    ntb_info.h_cell  = 0;
    ntb_info.nr_cols = 0;
    ntb_info.image = &bitmap2;
    ntb2 = CreateWindow (CTRL_NEWTOOLBAR,
                    "",
                    WS_CHILD | WS_VISIBLE | NTBS_WITHTEXT,
                    IDC_CTRL_NEWTOOLBAR_2,
                    0, 80, 1024, 0,
                    hWnd,(DWORD) &ntb_info);
    pixel = GetPixelInBitmap (&bitmap2, 0, 0);
    SetWindowBkColor (ntb2, pixel);
    InvalidateRect (ntb2, NULL, TRUE);

    ntb_info.nr_cells = 4;
    ntb_info.w_cell  = 0;
    ntb_info.h_cell  = 0;
    ntb_info.nr_cols = 0;
    ntb_info.image = &bitmap3;
    ntb3 = CreateWindow (CTRL_NEWTOOLBAR, "",
                    WS_CHILD | WS_VISIBLE
                    | NTBS_WITHTEXT | NTBS_TEXTRIGHT | NTBS_DRAWSEPARATOR,
                    IDC_CTRL_NEWTOOLBAR_3,
                    0, 170, 1024, 0,
                    hWnd,(DWORD) &ntb_info);
    pixel = GetPixelInBitmap (&bitmap3, 0, 0);
    SetWindowBkColor (ntb3, pixel);
    InvalidateRect (ntb3, NULL, TRUE);

    ntb_info.nr_cells = 4;
    ntb_info.w_cell  = 0;
    ntb_info.h_cell  = 0;
    ntb_info.nr_cols = 0;
    ntb_info.image = &bitmap4;
    ntb4 = CreateWindow (CTRL_NEWTOOLBAR, "",
                    WS_CHILD | WS_VISIBLE | NTBS_WITHTEXT | NTBS_TEXTRIGHT | NTBS_DRAWSTATES,
                    IDC_CTRL_NEWTOOLBAR_4,
                    0, 245, 1024, 0,
                    hWnd,(DWORD) &ntb_info);
    pixel = GetPixelInBitmap (&bitmap4, 0, 0);
    SetWindowBkColor (ntb4, pixel);
    InvalidateRect (ntb4, NULL, TRUE);

    memset (&ntbii, 0, sizeof (ntbii));
    ntbii.flags = NTBIF_PUSHBUTTON;
    ntbii.id = IDC_NTB_ONE;
    ntbii.bmp_cell = 0;
    SendMessage (ntb1, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_PUSHBUTTON;
    ntbii.id = IDC_NTB_TWO;
    ntbii.bmp_cell = 2;
    SendMessage(ntb1, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_PUSHBUTTON | NTBIF_DISABLED;
    ntbii.id = IDC_NTB_THREE;
    ntbii.bmp_cell = 1;
    SendMessage (ntb1, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_SEPARATOR;
    ntbii.id = 0;
    ntbii.bmp_cell = 0;
    ntbii.text = NULL;
    SendMessage (ntb1, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_HOTSPOTBUTTON;
    ntbii.id = IDC_NTB_FOUR;
    ntbii.bmp_cell = 3;
    ntbii.rc_hotspot = hotspot;
    ntbii.hotspot_proc = my_hotspot_proc;
    SendMessage (ntb1, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    memset (&ntbii, 0, sizeof (ntbii));
    ntbii.flags = NTBIF_PUSHBUTTON;
    ntbii.id = IDC_NTB_ONE;
    ntbii.bmp_cell = 0;
    ntbii.text = "Up";
    SendMessage (ntb2, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_PUSHBUTTON;
    ntbii.id = IDC_NTB_TWO;
    ntbii.bmp_cell = 2;
    ntbii.text = "Prev";
    SendMessage (ntb2, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_PUSHBUTTON | NTBIF_DISABLED;
    ntbii.id = IDC_NTB_THREE;
    ntbii.bmp_cell = 1;
    ntbii.text = "Next";
    SendMessage (ntb2, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_SEPARATOR;
    ntbii.id = 0;
    ntbii.bmp_cell = 0;
    ntbii.text = NULL;
    SendMessage (ntb2, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_HOTSPOTBUTTON;
    ntbii.id = IDC_NTB_FOUR;
    ntbii.bmp_cell = 3;
    ntbii.rc_hotspot = hotspot;
    ntbii.hotspot_proc = my_hotspot_proc;
    ntbii.text = "Menu";
    SendMessage (ntb2, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    memset (&ntbii, 0, sizeof (ntbii));
    ntbii.flags = NTBIF_PUSHBUTTON;
    ntbii.id = IDC_NTB_ONE;
    ntbii.bmp_cell = 0;
    ntbii.text = "Up";
    SendMessage (ntb3, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_PUSHBUTTON;
    ntbii.id = IDC_NTB_TWO;
    ntbii.bmp_cell = 1;
    ntbii.text = "Prev";
    SendMessage (ntb3, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_PUSHBUTTON;
    ntbii.id = IDC_NTB_THREE;
    ntbii.bmp_cell = 2;
    ntbii.text = "Next";
    SendMessage (ntb3, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_SEPARATOR;
    ntbii.id = 0;
    ntbii.bmp_cell = 0;
    ntbii.text = NULL;
    SendMessage (ntb3, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_HOTSPOTBUTTON | NTBIF_DISABLED;
    ntbii.id = IDC_NTB_FOUR;
    ntbii.bmp_cell = 3;
    ntbii.rc_hotspot = hotspot;
    ntbii.hotspot_proc = my_hotspot_proc;
    ntbii.text = "Menu";
    SendMessage (ntb3, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    memset (&ntbii, 0, sizeof (ntbii));
    ntbii.flags = NTBIF_PUSHBUTTON | NTBIF_DISABLED;
    ntbii.id = IDC_NTB_ONE;
    ntbii.bmp_cell = 0;
    ntbii.text = "Up";
    SendMessage (ntb4, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_PUSHBUTTON;
    ntbii.id = IDC_NTB_TWO;
    ntbii.bmp_cell = 1;
    ntbii.text = "Prev";
    SendMessage (ntb4, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_PUSHBUTTON;
    ntbii.id = IDC_NTB_THREE;
    ntbii.bmp_cell = 2;
    ntbii.text = "Next";
    SendMessage (ntb4, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_SEPARATOR;
    ntbii.id = 0;
    ntbii.bmp_cell = 0;
    ntbii.text = NULL;
    SendMessage (ntb4, NTBM_ADDITEM, 0, (LPARAM)&ntbii);

    ntbii.flags = NTBIF_HOTSPOTBUTTON;
    ntbii.id = IDC_NTB_FOUR;
    ntbii.bmp_cell = 3;
    ntbii.rc_hotspot = hotspot;
    ntbii.hotspot_proc = my_hotspot_proc;
    ntbii.text = "Menu";
    SendMessage (ntb4, NTBM_ADDITEM, 0, (LPARAM)&ntbii);
}

static int ControlTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_CREATE:
        if (LoadBitmap (HDC_SCREEN, &bitmap1, "res/new1.jpg"))
            return -1;

        if (LoadBitmap (HDC_SCREEN, &bitmap2, "res/new4.jpg"))
            return -1;

        if (LoadBitmap (HDC_SCREEN, &bitmap3, "res/new3.jpg"))
            return -1;

        if (LoadBitmap (HDC_SCREEN, &bitmap4, "res/new2.jpg"))
            return -1;

        CreateWindow (CTRL_BUTTON, "Close",
                        WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, IDC_CTRL_BUTTON, 
                        400, 320, 60, 25, hWnd,0);
        create_newtoolbars (hWnd);
        break;

    case MSG_COMMAND:
    {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);
        HDC hdc;
        char buff [254];

        if (wParam == IDC_CTRL_BUTTON) {
            PostMessage (hWnd, MSG_CLOSE, 0, 0);
            break;
        }

        memset (buff, 0, sizeof (buff));
        hdc = GetClientDC (hWnd);

        InvalidateRect(hWnd,&rect,TRUE);
        memset(buff,0,sizeof(buff));
        sprintf (buff, "Button %d of NewToolBar %d clicked.", code, id);
        TextOut (hdc, 0, 330, buff);
        ReleaseDC (hdc);
        break;
    }

    case MSG_DESTROY:
        UnloadBitmap (&bitmap1);
        UnloadBitmap (&bitmap2);
        UnloadBitmap (&bitmap3);
        UnloadBitmap (&bitmap4);
        DestroyAllControls (hWnd);
        hMainWnd = HWND_INVALID;
	return 0;

    case MSG_CLOSE:
        DestroyMainWindow (hWnd);
        MainWindowCleanup (hWnd);
        return 0;
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

static void InitCreateInfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "NewToolBar controls";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlTestWinProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 480;
    pCreateInfo->by = 380;
    pCreateInfo->iBkColor = PIXEL_lightgray;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

void newtoolbar_demo (HWND hwnd)
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

