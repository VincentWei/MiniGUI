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
** $Id: listbox.c,v 1.17 2007-12-06 02:24:59 xwyan Exp $
**
** listbox.c: The ListBox control demo program.
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
#ifndef __NOUNIX__
#include <unistd.h>
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define IDC_LISTBOX     100

#define IDC_CTRL2     110
#define IDC_CTRL3     120
#define IDC_CTRL4     130
#define IDC_CTRL5     140
#define IDC_CTRL6     150

static HWND hMainWnd = HWND_INVALID;

static void create_normal_listbox (HWND parent)
{
    HWND    hwnd;
    DIR*    dir;
    struct  dirent* dir_ent;

    CreateWindow (CTRL_STATIC, 
                "A normal listbox (sorted):", 
                WS_CHILD | SS_NOTIFY | SS_SIMPLE | WS_VISIBLE,
                IDC_STATIC, 
                10, 10, 200, 20, parent, 0);

    hwnd = CreateWindow (CTRL_LISTBOX,
                "Files in the current direcotory", 
                WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_SORT | LBS_MULTIPLESEL | WS_VSCROLL, 
                IDC_LISTBOX, 
                10, 40, 200, 100, parent, 0);

    dir = opendir ("./");
    while ( (dir_ent = readdir ( dir )) != NULL ) {
        SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM)dir_ent->d_name);
    }
    closedir (dir);
}

static void create_image_listbox (HWND parent)
{
    HWND    hwnd;
    DIR*    dir;
    struct  dirent* dir_ent;
    struct  stat my_stat;
    LISTBOXITEMINFO lbii;
    int h;

    CreateWindow (CTRL_STATIC, 
                "A listbox with LBS_USEICON style:", 
                WS_CHILD | SS_NOTIFY | SS_SIMPLE | WS_VISIBLE,
                IDC_STATIC, 
                10, 150, 180, 20, parent, 0);

    hwnd = CreateWindow (CTRL_LISTBOX,
                "Files in the current direcotory", 
                WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_USEICON | WS_VSCROLL, 
                IDC_LISTBOX + 1, 
                10, 180, 200, 100, parent, 0);

    GetIconSize (GetSmallSystemIcon (IDI_APPLICATION), NULL, &h);
    SendMessage (hwnd, LB_SETITEMHEIGHT, 0, h);

    dir = opendir ("./");
    while ( (dir_ent = readdir ( dir )) != NULL ) {

        if (stat (dir_ent->d_name, &my_stat) < 0 ){
           continue;
        }

        lbii.cmFlag = 0;
        lbii.string = dir_ent->d_name;
        if (S_ISDIR (my_stat.st_mode)) {
            lbii.hIcon = (DWORD)GetSmallSystemIcon (IDI_APPLICATION);
            SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM)&lbii);
        }
        else {
            lbii.hIcon = (DWORD)GetSmallSystemIcon (IDI_STOP);
            SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM)&lbii);
        }
    }
    closedir (dir);
}

static void create_multisel_listbox (HWND parent)
{
    HWND    hwnd;
    DIR*    dir;
    struct  dirent* dir_ent;
    struct  stat my_stat;
    LISTBOXITEMINFO lbii;
    int h;

    CreateWindow (CTRL_STATIC, 
                "A listbox with LBS_MULTIPLESEL style:", 
                WS_CHILD | SS_NOTIFY | SS_SIMPLE | WS_VISIBLE,
                IDC_STATIC, 
                220, 10, 200, 20, parent, 0);

    hwnd = CreateWindow (CTRL_LISTBOX,
                "Files in the current direcotory", 
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
                LBS_SORT | LBS_USEICON | LBS_MULTIPLESEL, 
                IDC_LISTBOX + 3, 
                220, 40, 200, 100, parent, 0);

    GetIconSize (GetSmallSystemIcon (IDI_APPLICATION), NULL, &h);

    dir = opendir ("./");
    while ( (dir_ent = readdir ( dir )) != NULL ) {

        if (stat (dir_ent->d_name, &my_stat) < 0 ){
           continue;
        }

        lbii.cmFlag = 0;
        lbii.string = dir_ent->d_name;
        if (S_ISDIR (my_stat.st_mode)) {
            lbii.hIcon = (DWORD)GetSmallSystemIcon (IDI_APPLICATION);
            SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM)&lbii);
        }
        else {
            lbii.hIcon = (DWORD)GetSmallSystemIcon (IDI_STOP);
            SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM)&lbii);
        }
    }
    closedir (dir);
}

static HICON icon1, icon2;

#define IDC_ITEMTEXT    300
#define IDC_ADDITEM     310
#define IDC_DELITEM     320
#define IDC_STRING      330
#define IDC_FIND        340

#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (1, 2, 6)
static void add_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        LISTBOXITEMINFO lbii;
        HWND parent = GetParent (hwnd);
        char buffer [100];

        SendDlgItemMessage (parent, IDC_ITEMTEXT, MSG_GETTEXT, 99, (LPARAM)buffer);
        if (buffer[0] != '\0') {
            int cur_sel;

            lbii.cmFlag = CMFLAG_BLANK;
            lbii.hIcon = icon2;
            lbii.string = buffer;
            
            cur_sel = SendDlgItemMessage (parent, IDC_LISTBOX + 2, LB_GETCURSEL, 0, 0);
            SendDlgItemMessage (parent, IDC_LISTBOX + 2, LB_INSERTSTRING, cur_sel, (LPARAM)&lbii);
        }
    }
}

static void del_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        int cur_sel;

        HWND parent = GetParent (hwnd);
        HWND listbox = GetDlgItem (parent , IDC_LISTBOX + 2);

        cur_sel = SendMessage (listbox, LB_GETCURSEL, 0, 0);
        if (cur_sel >= 0 && MessageBox (parent, 
                "Are you sure to delete this item?", 
                "Listbox", MB_YESNOCANCEL | MB_ICONQUESTION) == IDYES)
            SendMessage (listbox, LB_DELETESTRING, cur_sel, 0);
    }
}

static void find_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        char buffer [100];
        int cur_sel, found;

        HWND parent = GetParent (hwnd);
        HWND listbox = GetDlgItem (parent , IDC_LISTBOX + 2);

        SendDlgItemMessage (parent, IDC_STRING, MSG_GETTEXT, 99, (LPARAM)buffer);
        if (buffer[0] != '\0') {

            cur_sel = SendMessage (listbox, LB_GETCURSEL, 0, 0);
            if (cur_sel < 0)
                cur_sel = 0;

            found = SendMessage (listbox, LB_FINDSTRINGEXACT, cur_sel, (LPARAM)buffer);
            if (found >= 0)
                SendMessage (listbox, LB_SETCURSEL, found, 0);
        }
    }
}

static void listbox_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    printf ("From listbox_notif_proc: ID: %d, Code %d\n", id, nc);
}

#endif

static void create_checkbox_listbox (HWND parent)
{
    HWND hwnd;
    LISTBOXITEMINFO lbii;
    int height;

    CreateWindow (CTRL_EDIT,
                "Add me to the box bellow", 
                WS_CHILD | WS_VISIBLE | WS_BORDER, 
                IDC_ITEMTEXT, 
                220, 150, 190, 20, parent, 0);

    CreateWindow (CTRL_BUTTON,
                "Add", 
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
                IDC_ADDITEM, 
                430, 150, 60, 20, parent, 0);

    hwnd = CreateWindow (CTRL_LISTBOX,
                CTRL_LISTBOX,
                WS_CHILD | WS_VISIBLE | WS_BORDER | /* WS_VSCROLL | */
                LBS_NOTIFY | LBS_AUTOCHECKBOX | LBS_USEICON,
                IDC_LISTBOX + 2, 
                220, 180, 190, 100, parent, 0);

    CreateWindow (CTRL_BUTTON,
                "Delete", 
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                IDC_DELITEM,
                220, 290, 60, 20, parent, 0);
                              
    CreateWindow (CTRL_EDIT,
                NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER, 
                IDC_STRING, 
                290, 290, 120, 20, parent, 0);

    CreateWindow (CTRL_BUTTON,
                "Find", 
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
                IDC_FIND, 
                430, 290, 60, 20, parent, 0);

    GetIconSize (icon1, NULL, &height);
    SendMessage (hwnd, LB_SETITEMHEIGHT, 0, height);

    lbii.hIcon = icon1;
    lbii.cmFlag = CMFLAG_CHECKED;
    lbii.string = "One initially checked item";
    SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM)&lbii);

    lbii.cmFlag = CMFLAG_BLANK;
    lbii.string = "One initially not checked item";
    SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM)&lbii);

    lbii.cmFlag = CMFLAG_PARTCHECKED;
    lbii.string = "One initially part-checked item";
    SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM)&lbii);

#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (1, 2, 6)
    SetNotificationCallback (GetDlgItem (parent, IDC_ADDITEM), add_proc);
    SetNotificationCallback (GetDlgItem (parent, IDC_DELITEM), del_proc);
    SetNotificationCallback (GetDlgItem (parent, IDC_FIND), find_proc);
    SetNotificationCallback (GetDlgItem (parent, IDC_LISTBOX + 2), listbox_notif_proc);
#endif
}

static int ControlTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            icon1 = LoadIconFromFile (HDC_SCREEN, "res/audio.ico", 1);
            icon2 = LoadIconFromFile (HDC_SCREEN, "res/cddrive.ico", 1);
            create_normal_listbox (hWnd);
            create_image_listbox (hWnd);
            create_multisel_listbox (hWnd);
            create_checkbox_listbox (hWnd);

            CreateWindow (CTRL_BUTTON, 
                          "Close", 
                          WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 
                          IDCANCEL, 
                          430, 330, 60, 25, hWnd, 0);
        break;

        case MSG_COMMAND:
            if (wParam == IDCANCEL) {
                PostMessage (hWnd, MSG_CLOSE, 0, 0L);
            }
            break;

	case MSG_DESTROY:
            DestroyIcon (icon1);
            DestroyIcon (icon2);
            DestroyAllControls (hWnd);
            hMainWnd = HWND_INVALID;
	    return 0;

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            MainWindowThreadCleanup (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitCreateInfo(PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "ListBox controls";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor (IDC_ARROW);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlTestWinProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 500;
    pCreateInfo->by = 400;
    pCreateInfo->iBkColor = GetWindowElementColor (WE_MAINC_THREED_BODY); 
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

void listbox_demo (HWND hwnd)
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

