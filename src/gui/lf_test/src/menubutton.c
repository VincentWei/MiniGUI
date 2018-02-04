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
** $Id: menubutton.c,v 1.13 2007-12-06 02:24:59 xwyan Exp $
**
** menubutton.c: The MenuButton control demo program.
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

static HWND hMainWnd = HWND_INVALID;

#define IDC_MENUBUTTON 100

static void create_normal_menubutton (HWND hparent)
{
    HWND menubtn;
    MENUBUTTONITEM mbi;

    DIR*    dir;
    struct  dirent* dir_ent;
    struct  stat my_stat;

    menubtn = CreateWindowEx (CTRL_MENUBTN,
                        "Files in the current directory", 
#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (1, 5, 3)
                        WS_CHILD | WS_VISIBLE | MBS_ALIGNCENTER, 
#else
                        WS_CHILD | WS_VISIBLE, 
#endif
                        WS_EX_NONE,
                        IDC_MENUBUTTON,
                        10, 20, 280, 24, hparent, 0);

    mbi.text = NULL;
    mbi.bmp = NULL;
    mbi.data = 0;

    dir = opendir ("./");
    while ( (dir_ent = readdir ( dir )) != NULL ) {

        /* Assemble full path name. */
        if (stat (dir_ent->d_name, &my_stat) < 0 ){
           continue;
        }

        if (!S_ISDIR (my_stat.st_mode)) {
            mbi.text = dir_ent->d_name;
            SendMessage (menubtn, MBM_ADDITEM, 0, (LPARAM) &mbi);
        }
    }

    closedir (dir);
}

static void create_sorted_menubutton (HWND hparent)
{
    HWND menubtn;
    MENUBUTTONITEM mbi;

    DIR*    dir;
    struct  dirent* dir_ent;
    struct  stat my_stat;

    menubtn = CreateWindowEx (CTRL_MENUBTN,
                        "Files and directories in the current directory (sorted)", 
#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (1, 5, 3)
                        WS_CHILD | WS_VISIBLE | MBS_SORT | MBS_NOBUTTON | MBS_LEFTARROW, 
#else
                        WS_CHILD | WS_VISIBLE | MBS_SORT,
#endif
                        WS_EX_NONE,
                        IDC_MENUBUTTON + 1,
                        10, 60, 280, 24, hparent, 0);

    mbi.text = NULL;
    mbi.bmp = NULL;
    mbi.data = 0;

    dir = opendir ("./");
    while ( (dir_ent = readdir ( dir )) != NULL ) {

        /* Assemble full path name. */
        if (stat (dir_ent->d_name, &my_stat) < 0 ){
           continue;
        }

        mbi.text = dir_ent->d_name;
        if (S_ISDIR (my_stat.st_mode))
            SendMessage (menubtn, MBM_ADDITEM, 0, (LPARAM) &mbi);
        else
            SendMessage (menubtn, MBM_ADDITEM, -1, (LPARAM) &mbi);
    }

    closedir (dir);
}

static BITMAP bmp_folder;

static void create_bitmap_menubutton (HWND hparent)
{
    HWND menubtn;
    MENUBUTTONITEM mbi;

    DIR*    dir;
    struct  dirent* dir_ent;
    struct  stat my_stat;

    menubtn = CreateWindowEx (CTRL_MENUBTN,
                        "Files and directories in the current directory (sorted, bitmap)", 
#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (1, 5, 3)
                        WS_CHILD | WS_VISIBLE | MBS_SORT | MBS_LEFTARROW | MBS_ALIGNLEFT, 
#else
                        WS_CHILD | WS_VISIBLE | MBS_SORT,
#endif
                        WS_EX_NONE,
                        IDC_MENUBUTTON + 2,
                        10, 100, 280, 24, hparent, 0);

    mbi.text = NULL;
    mbi.bmp = NULL;
    mbi.data = 0;

    dir = opendir ("./");
    while ( (dir_ent = readdir ( dir )) != NULL ) {

        /* Assemble full path name. */
        if (stat (dir_ent->d_name, &my_stat) < 0 ){
           continue;
        }

        mbi.text = dir_ent->d_name;
        if (S_ISDIR (my_stat.st_mode)) {
            mbi.bmp = &bmp_folder;
            SendMessage (menubtn, MBM_ADDITEM, 0, (LPARAM) &mbi);
        }
        else {
            mbi.bmp = NULL;
            SendMessage (menubtn, MBM_ADDITEM, -1, (LPARAM) &mbi);
        }
    }

    closedir (dir);
}

static int ControlTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            create_normal_menubutton (hWnd);
            create_sorted_menubutton (hWnd);
            create_bitmap_menubutton (hWnd);

            CreateWindow (CTRL_BUTTON, 
                                "Close", 
                                WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 
                                IDCANCEL, 
                                230, 140, 60, 24, hWnd, 0);
            LoadBitmapFromFile (HDC_SCREEN, &bmp_folder, "./res/folder.bmp");
        break;

        case MSG_COMMAND:
        {
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);

            switch (id) {
            case IDC_MENUBUTTON:
            case IDC_MENUBUTTON + 1:
            case IDC_MENUBUTTON + 2:
                if (code == MBN_SELECTED) {
                    printf ("An item in the control %d is selected.\n", id);
                }
                else if (code == MBN_CHANGED) {
                    printf ("Selected item in the control %d has changed.\n", id);
                }
                break;
            
            case IDCANCEL:
                PostMessage (hWnd, MSG_CLOSE, 0, 0);
                break;
                
            default:
                break;
            }
        }
        break;
       
        case MSG_DESTROY:
            UnloadBitmap (&bmp_folder);
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

static void InitCreateInfo(PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "MenuButton controls";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(1);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlTestWinProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 300;
    pCreateInfo->by = 200;
    pCreateInfo->iBkColor = GetWindowElementColor (WE_MAINC_THREED_BODY);
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

void menubutton_demo (HWND hwnd)
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

