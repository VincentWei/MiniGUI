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
** $Id: listview.c,v 1.36 2007-12-06 02:24:59 xwyan Exp $
**
** listview.c: The ListView control demo program.
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
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>


#define IDC_LISTVIEW    10

#define IDC_CTRL1       20
#define IDC_CTRL2       30
#define IDC_CTRL3       40
#define IDC_CTRL4       50
#define IDC_EDIT        60
#define IDC_CTRL6       70
#define IDC_CTRL7       80
#define IDC_CTRL8       90
#define IDC_CTRL9       100
#define IDC_CTRL10      110
#define IDC_CTRL11      120
#define IDC_CTRL12      130

#define IDM_FILE        200

static HWND hMainWnd = HWND_INVALID;
static HWND hChildWnd1;
static HMENU hRightMenu;

static void create_ctrls (HWND hWnd)
{
        CreateWindow ("button", "Delete the selected item",
                                 WS_CHILD
                                 | BS_PUSHBUTTON
                                 | WS_VISIBLE,
                                 IDC_CTRL1, 10, 260, 130, 20, hWnd, 0);
        CreateWindow ("button", "Clear all item",
                                 WS_CHILD
                                 | BS_PUSHBUTTON
                                 | WS_VISIBLE,
                                 IDC_CTRL2, 150, 260, 100, 20, hWnd, 0);
        CreateWindow ("button", "select item",
                                 WS_CHILD
                                 | BS_PUSHBUTTON
                                 | WS_VISIBLE,
                                 IDC_CTRL3, 260, 260, 100, 20, hWnd, 0);
        CreateWindow ("button", "show item",
                                 WS_CHILD
                                 | BS_PUSHBUTTON
                                 | WS_VISIBLE,
                                 IDC_CTRL4, 370, 260, 100, 20, hWnd, 0);
        CreateWindow ("edit", "",
                                 WS_CHILD
                                 | WS_VISIBLE,
                                 IDC_EDIT, 150, 230, 120, 20, hWnd, 0);
        CreateWindow ("button", "del column",
                                 WS_CHILD
                                 | WS_VISIBLE,
                                 IDC_CTRL6, 10, 290, 130, 20, hWnd, 0);
        CreateWindow ("button", "set column head",
                                 WS_CHILD
                                 | WS_VISIBLE,
                                 IDC_CTRL7, 150, 290, 100, 20, hWnd, 0);
        CreateWindow ("button", "sorting the items",
                                 WS_CHILD
                                 | WS_VISIBLE,
                                 IDC_CTRL8, 260, 290, 100, 20, hWnd, 0);
        CreateWindow ("button", "find item",
                                 WS_CHILD
                                 | WS_VISIBLE,
                                 IDC_CTRL9, 370, 290, 100, 20, hWnd, 0);
}

static int compare_time (HLVITEM nItem1, HLVITEM nItem2, PLVSORTDATA sortData)
{
        DWORD data1, data2;
        struct stat stat1, stat2;

        data1 = SendMessage (hChildWnd1, LVM_GETITEMADDDATA, 0, nItem1);
        data2 = SendMessage (hChildWnd1, LVM_GETITEMADDDATA, 0, nItem2);

        stat ( (char*)data1, &stat1 );
        stat ( (char*)data2, &stat2 );

        return (stat1.st_mtime - stat2.st_mtime);
}

static int compare_size (HLVITEM nItem1, HLVITEM nItem2, PLVSORTDATA sortData)
{
        DWORD data1, data2;
        struct stat stat1, stat2;
        int size1, size2;

        data1 = SendMessage (hChildWnd1, LVM_GETITEMADDDATA, 0, nItem1);
        data2 = SendMessage (hChildWnd1, LVM_GETITEMADDDATA, 0, nItem2);

        stat ( (char*)data1, &stat1 );
        stat ( (char*)data2, &stat2 );

        if (S_ISREG(stat1.st_mode))
            size1 = stat1.st_size;
        else
            size1 = 0;

        if (S_ISREG(stat2.st_mode))
            size2 = stat2.st_size;
        else
            size2 = 0;

        return (size1 - size2);
}

static HMENU create_rightbutton_menu (void)
{
    int i;
    HMENU hMenu;
    MENUITEMINFO mii;
    char *msg[] = {
        "open",
        "copy",
        "delete",
        "rename",
        "properties"
    };

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)"File";

    hMenu = CreatePopupMenu (&mii);

    for ( i = 0; i < 5; i ++ ) {
        memset ( &mii, 0, sizeof (MENUITEMINFO) );
        mii.type = MFT_STRING;
        mii.id = IDM_FILE+ i;
        mii.state = 0;
        mii.typedata= (DWORD) msg[i]; 
        InsertMenuItem ( hMenu, i, TRUE, &mii );
    }

    return hMenu;
    //return StripPopupHead(hMenu);
}

static void
lv_notify_process (HWND hwnd, int id, int code, DWORD addData)
{
    if (code == LVN_KEYDOWN) {
        PLVNM_KEYDOWN down;
        int key;
        
        down = (PLVNM_KEYDOWN)addData;
        key = LOWORD(down->wParam);

        if (key == SCANCODE_REMOVE) {
            HLVITEM hlvi;
            hlvi = SendMessage (hwnd, LVM_GETSELECTEDITEM, 0, 0);
            if (hlvi) {
                if( MessageBox (hMainWnd, "are you really want to delete this file?", 
                            "warning", MB_YESNO) == IDYES) {
                    // not really delete yet.
                    SendMessage (hwnd, LVM_DELITEM, 0, hlvi);
                }
            }
        }
        if (key == SCANCODE_ENTER) {
        }
    }
    if (code == LVN_ITEMRUP) {
        PLVNM_ITEMRUP up;
        int x, y;
        
        up = (PLVNM_ITEMRUP)addData;
        x = LOSWORD(up->lParam);
        y = HISWORD(up->lParam);

        ClientToScreen (hChildWnd1, &x, &y);

        TrackPopupMenu (GetPopupSubMenu (hRightMenu), TPM_LEFTALIGN | TPM_LEFTBUTTON , 
            x, y, hMainWnd);
    }
    if (code == LVN_ITEMDBCLK) {
        HLVITEM hlvi = SendMessage (hwnd, LVM_GETSELECTEDITEM, 0, 0);
        if (hlvi > 0) {
                if( MessageBox (hMainWnd, "Are you really want to open this file?", 
                            "Question", MB_YESNO) == IDYES) {
                    MessageBox (hMainWnd, "Me too.", "Sorry", MB_OK);
                }
        }
    }
}

extern BITMAP bmp_bkgnd;

static int
ControlTestWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    static BITMAP folder;

    switch (message)
    {

    case MSG_CREATE:
    {
        int i = 0;
        LVSUBITEM subdata;
        LVITEM item;
        LVCOLUMN s1;

        DIR*    dir;
        struct  dirent* dir_ent;
        struct  stat my_stat;

        create_ctrls (hWnd);
        hRightMenu = create_rightbutton_menu ();

        LoadBitmapFromFile (HDC_SCREEN, &folder, "./res/folder.bmp");

        hChildWnd1 = CreateWindowEx (CTRL_LISTVIEW, "List View",
                                   WS_CHILD | WS_VISIBLE | WS_VSCROLL |
                                   WS_HSCROLL | WS_BORDER, WS_EX_NONE, IDC_LISTVIEW, 20, 20, 450,
                                   200, hWnd, 0);

        SetNotificationCallback (hChildWnd1, lv_notify_process);

        s1.nCols = 0;
        s1.pszHeadText = "File name";
        s1.width = 100;
        s1.pfnCompare = NULL;
        s1.colFlags = 0;
        SendMessage (hChildWnd1, LVM_ADDCOLUMN, 0, (LPARAM) & s1);

        s1.nCols = 1;
        s1.pszHeadText = "File size";
        s1.width = 100;
        s1.pfnCompare = compare_size;
        s1.colFlags = LVCF_RIGHTALIGN | LVHF_CENTERALIGN;
        SendMessage (hChildWnd1, LVM_ADDCOLUMN, 0, (LPARAM) & s1);

        s1.nCols = 2;
        s1.pszHeadText = "Category";
        s1.width = 80;
        s1.pfnCompare = NULL;
        s1.colFlags = 0;
        SendMessage (hChildWnd1, LVM_ADDCOLUMN, 0, (LPARAM) & s1);

        s1.nCols = 3;
        s1.pszHeadText = "Last modified time";
        s1.width = 180;
        s1.pfnCompare = compare_time;
        s1.colFlags = LVCF_CENTERALIGN;
        SendMessage (hChildWnd1, LVM_ADDCOLUMN, 0, (LPARAM) & s1);

        dir = opendir ("./");


        while ( (dir_ent = readdir ( dir )) != NULL )
        {
            if (stat (dir_ent->d_name, &my_stat) < 0 ){
                continue;
            }

            if ( !( S_ISDIR(my_stat.st_mode) || S_ISREG(my_stat.st_mode) ) ) 
                continue;

            item.itemData = (DWORD) strdup(dir_ent->d_name);
            item.nItem = i;
            item.nItemHeight = 28;
            SendMessage (hChildWnd1, LVM_ADDITEM, 0, (LPARAM) &item);

            subdata.nItem = i;

            subdata.subItem = 0;
            subdata.flags = 0;
            subdata.pszText = dir_ent->d_name;
            if (S_ISDIR(my_stat.st_mode)) {
                subdata.flags = LVFLAG_BITMAP;
                subdata.image = (DWORD)&folder;
                subdata.nTextColor = PIXEL_blue;
            }
            else {
                subdata.nTextColor = PIXEL_black;
            }

            SendMessage (hChildWnd1, LVM_FILLSUBITEM, 0, (LPARAM) & subdata);

            subdata.flags = 0;
            subdata.image = 0;

            {
            int fsize = 0;
            char buffer[50];
            if (S_ISREG(my_stat.st_mode))
                fsize = my_stat.st_size;
            sprintf (buffer, "%d", fsize);
            subdata.pszText = buffer;
            subdata.nTextColor = PIXEL_black;
            subdata.subItem = 1;
            SendMessage (hChildWnd1, LVM_FILLSUBITEM, 0, (LPARAM) & subdata);
            }

            subdata.subItem = 2;
            if (S_ISDIR(my_stat.st_mode)) {
                subdata.pszText = "directory";
            }
            else if (S_ISREG(my_stat.st_mode)) {
                subdata.pszText = "regular file";
            }
            subdata.nTextColor = PIXEL_black;
            SendMessage (hChildWnd1, LVM_FILLSUBITEM, 0, (LPARAM) & subdata);

            subdata.subItem = 3;
            subdata.pszText = ctime(&my_stat.st_mtime);
            subdata.pszText[strlen(subdata.pszText)-1] = 0;
            subdata.nTextColor = PIXEL_black;
            SendMessage (hChildWnd1, LVM_FILLSUBITEM, 0, (LPARAM) & subdata);

            i++;
        }

        closedir (dir);

      }
      break;

    case MSG_COMMAND:
    {
        int id = LOWORD (wParam);
        //int code = HIWORD (wParam);
        int nSel;
        char buffer[101];

        GetWindowText (GetDlgItem(hWnd, IDC_EDIT), buffer, 100);
        nSel = atoi(buffer);

        switch (id)
        {
        case IDC_CTRL1:
        {
            HLVITEM hlvi;
            hlvi = SendMessage (hChildWnd1, LVM_GETSELECTEDITEM, 0, 0);
            SendMessage (hChildWnd1, LVM_DELITEM, 0, (LPARAM)hlvi );
            break;
        }
        case IDC_CTRL2:
            SendMessage (hChildWnd1, LVM_DELALLITEM, 0, 0);
            break;
        case IDC_CTRL3:
            SendMessage (hChildWnd1, LVM_SELECTITEM, nSel, 0);
            break;
        case IDC_CTRL4:
            SendMessage (hChildWnd1, LVM_SELECTITEM, nSel, 0);
            SendMessage (hChildWnd1, LVM_SHOWITEM, nSel, 0);
            break;
        case IDC_CTRL6:
            SendMessage (hChildWnd1, LVM_DELCOLUMN, nSel, 0);
            break;
        case IDC_CTRL7:
        {
            LVCOLUMN pcol;
            pcol.width = 100;
            pcol.nCols = 2;
            pcol.pszHeadText = buffer;
            pcol.nTextMax = strlen (buffer);
            SendMessage (hChildWnd1, LVM_SETCOLUMN, 0, (LPARAM)&pcol);
            break;
        }
        case IDC_CTRL8:
            SendMessage (hChildWnd1, LVM_SORTITEMS, 0, (LPARAM)compare_size);
            break;
        case IDC_CTRL9:
        {
            LVFINDINFO find_info;
            HLVITEM hlvi;
            char buffer1[100];
            char *text[1];

            strcpy (buffer1, "listview.c");
            text[0] = buffer1;

            find_info.iStart = 0;
            find_info.flags = LVFF_TEXT;
            find_info.nCols = 0;
            find_info.pszInfo = text;
            hlvi = SendMessage (hChildWnd1, LVM_FINDITEM, 0, (LPARAM)&find_info);
            SendMessage (hChildWnd1, LVM_CHOOSEITEM, 0, hlvi);
            break;
        }
        case IDM_FILE:
		case IDM_FILE+1:
		case IDM_FILE+2:
		case IDM_FILE+3:
        case IDM_FILE+4:
            MessageBox (hMainWnd, "In Construction ...", "Sorry", MB_OK);
            break;
        } //end switch

        break;
    }

    
    case MSG_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        const RECT* clip = (const RECT*) lParam;
        BOOL fGetDC = FALSE;
        RECT rcTemp;
                
        if (hdc == 0) {
            hdc = GetClientDC (hWnd);
            fGetDC = TRUE;
        }       
                
        if (clip) {
            rcTemp = *clip;
            ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
            ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
            IncludeClipRect (hdc, &rcTemp);
        }

        FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bmp_bkgnd);

        if (fGetDC)
            ReleaseDC (hdc);
        return 0;
    }

    case MSG_DESTROY:
        UnloadBitmap (&folder);
        DestroyMenu (hRightMenu);
        DestroyAllControls (hWnd);
        hMainWnd = HWND_INVALID;
        return 0;

    case MSG_CLOSE:
        DestroyMainWindow (hWnd);
        MainWindowCleanup (hWnd);
        return 0;

    } //end switch

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

static void
InitCreateInfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX
                           | WS_MAXIMIZEBOX | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "ListView control";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor (1);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlTestWinProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 500;
    pCreateInfo->by = 350;
    pCreateInfo->iBkColor = GetWindowElementColor (WE_MAINC_THREED_BODY);
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

void
listview_demo (HWND hwnd)
{
    MAINWINCREATE CreateInfo;

    InitCreateInfo (&CreateInfo);
    CreateInfo.hHosting = hwnd;

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd != HWND_INVALID)
    {
        ShowWindow (hMainWnd, SW_SHOWNORMAL);
        return;
    }
}
