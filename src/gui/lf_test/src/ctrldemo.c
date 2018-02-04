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
** $Id: main.c,v 1.22 2007-12-06 02:24:59 xwyan Exp $
**
** main.c: The main entry of control demo program.
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
#include <sys/wait.h>
#endif
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>

#include "ctrls.h"
#include "resource.h"

#define DEFAULT_WIDTH   800
#define DEFAULT_HEIGHT  600

#define INFO_DEBUG
#include "my_debug.h"
static HMENU createpmenuabout (void)
{
    HMENU hmnu;
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)"About...";
    hmnu = CreatePopupMenu (&mii);
    
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING ;
    mii.state       = 0;
    mii.id          = IDM_ABOUT_THIS;
    mii.typedata    = (DWORD)"About MiniGUI Controls...";
    InsertMenuItem(hmnu, 0, TRUE, &mii);

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING ;
    mii.state       = 0;
    mii.id          = IDM_ABOUT;
    mii.typedata    = (DWORD)"About MiniGUI...";
    InsertMenuItem(hmnu, 1, TRUE, &mii);

    return hmnu;
}
static HMENU createpmenu_renderer (void)
{
    HMENU hmnu;
    MENUITEMINFO mii;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)"Renderer";
    hmnu = CreatePopupMenu (&mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_CLASSIC;
    mii.typedata    = (DWORD)"classic";
    InsertMenuItem(hmnu, 0, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_FLAT;
    mii.typedata    = (DWORD)"flat";
    InsertMenuItem(hmnu, 1, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_FASHION;
    mii.typedata    = (DWORD)"fashion";
    InsertMenuItem(hmnu, 2, TRUE, &mii);
    return hmnu;
}

static HMENU createpmenufile (void)
{
    HMENU hmnu;
    MENUITEMINFO mii;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)"File";
    hmnu = CreatePopupMenu (&mii);
    
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_NEW;
    mii.typedata    = (DWORD)"New";
    InsertMenuItem(hmnu, 0, TRUE, &mii);
    
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_OPEN;
    mii.typedata    = (DWORD)"Open...";
    InsertMenuItem(hmnu, 1, TRUE, &mii);
    
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_SAVE;
    mii.typedata    = (DWORD)"Save";
    InsertMenuItem(hmnu, 2, TRUE, &mii);
    
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_SAVEAS;
    mii.typedata    = (DWORD)"Save As...";
    InsertMenuItem(hmnu, 3, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_CLOSE;
    mii.typedata    = (DWORD)"Close";
    InsertMenuItem(hmnu, 4, TRUE, &mii);
    
    mii.type        = MFT_SEPARATOR;
    mii.state       = 0;
    mii.id          = 0;
    mii.typedata    = 0;
    InsertMenuItem(hmnu, 5, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_EXIT;
    mii.typedata    = (DWORD)"Exit";
    InsertMenuItem(hmnu, 6, TRUE, &mii);

    return hmnu;
}

static HMENU createpmenuobject (void)
{
    HMENU hmnu;
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)"Controls";
    hmnu = CreatePopupMenu (&mii);
    
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_STATIC;
    mii.typedata    = (DWORD)"Static control";
    InsertMenuItem(hmnu, 0, TRUE, &mii);
     
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_BUTTON;
    mii.typedata    = (DWORD)"Button control";
    InsertMenuItem(hmnu, 1, TRUE, &mii);
     
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_MENUBUTTON;
    mii.typedata    = (DWORD)"MenuButton control";
    InsertMenuItem(hmnu, 2, TRUE, &mii);
     
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_EDIT;
    mii.typedata    = (DWORD)"Edit control";
    InsertMenuItem(hmnu, 3, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_LISTBOX;
    mii.typedata    = (DWORD)"ListBox control";
    InsertMenuItem(hmnu, 4, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_PROGRESSBAR;
    mii.typedata    = (DWORD)"ProgressBar control";
    InsertMenuItem(hmnu, 5, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_COMBOBOX;
    mii.typedata    = (DWORD)"ComboBox control";
    InsertMenuItem(hmnu, 6, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_TOOLBAR;
    mii.typedata    = (DWORD)"NewToolBar control";
    InsertMenuItem(hmnu, 7, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_TRACKBAR;
    mii.typedata    = (DWORD)"TrackBar control";
    InsertMenuItem(hmnu, 8, TRUE, &mii);

    mii.type        = MFT_SEPARATOR;
    mii.state       = 0;
    mii.id          = 0;
    mii.typedata    = 0;
    InsertMenuItem(hmnu, 9, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_LISTVIEW;
    mii.typedata    = (DWORD)"ListView control";
    InsertMenuItem(hmnu, 10, TRUE, &mii);
    
    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_TREEVIEW;
    mii.typedata    = (DWORD)"TreeView control";
    InsertMenuItem(hmnu, 12, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_MONTHCALENDAR;
    mii.typedata    = (DWORD)"MonthCalendar control";
    InsertMenuItem(hmnu, 13, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_SPINBOX;
    mii.typedata    = (DWORD)"SpinBox control";
    InsertMenuItem(hmnu, 14, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_COOLBAR;
    mii.typedata    = (DWORD)"CoolBar control";
    InsertMenuItem(hmnu, 15, TRUE, &mii);

    mii.type        = MFT_SEPARATOR;
    mii.state       = 0;
    mii.id          = 0;
    mii.typedata    = 0;
    InsertMenuItem(hmnu, 16, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_SUBCLASS;
    mii.typedata    = (DWORD)"Control Subclass";
    InsertMenuItem(hmnu, 17, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = IDM_TIMEEDIT;
    mii.typedata    = (DWORD)"Time Editor";
    InsertMenuItem(hmnu, 18, TRUE, &mii);

    return hmnu;
}

static HMENU createmenu (void)
{
    HMENU hmnu;
    MENUITEMINFO mii;

    hmnu = CreateMenu();

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 100;
    mii.typedata    = (DWORD)"File";
    mii.hsubmenu    = createpmenufile ();

    InsertMenuItem(hmnu, 0, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = 110;
    mii.typedata    = (DWORD)"Controls";
    mii.hsubmenu    = createpmenuobject ();
    InsertMenuItem(hmnu, 2, TRUE, &mii);
    
    mii.type        = MFT_STRING;
    mii.id          = 120;
    mii.typedata    = (DWORD)"Renderer";
    mii.hsubmenu    = createpmenu_renderer ();
    InsertMenuItem(hmnu, 3, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = 130;
    mii.typedata    = (DWORD)"About";
    mii.hsubmenu    = createpmenuabout ();
    InsertMenuItem(hmnu, 4, TRUE, &mii);
                   
    return hmnu;
}

static int CtrlTestWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message) {
        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            TabbedTextOut (hdc, 0, 0, 
                                 "\nPlease choose a command from menu...");
            EndPaint (hWnd, hdc);
            return 0;

        case MSG_COMMAND:
        switch (wParam) 
        {
            case IDM_CLASSIC:
                SetDefaultWindowElementRenderer ("classic");
                DesktopUpdateAllWindow ();
                break;
            case IDM_FLAT:
                SetDefaultWindowElementRenderer ("flat");
                DesktopUpdateAllWindow ();
                break;
            case IDM_FASHION:
                SetDefaultWindowElementRenderer ("fashion");
                DesktopUpdateAllWindow ();
                break;
            case IDM_NEW:
            case IDM_OPEN:
            case IDM_SAVE:
            case IDM_SAVEAS:
                break;

            case IDM_CLOSE:
            case IDM_EXIT:
                SendMessage (hWnd, MSG_CLOSE, 0, 0L);
                break;

            case IDM_STATIC:
                static_demo (hWnd);
                break;
            case IDM_BUTTON:
                button_demo (hWnd);
                break;
            case IDM_MENUBUTTON:
                menubutton_demo (hWnd);
                break;
            case IDM_EDIT:
                edit_demo (hWnd);
                break;
            case IDM_LISTBOX:
                listbox_demo (hWnd);
                break;
            case IDM_PROGRESSBAR:
                progressbar_demo (hWnd);
                break;
            case IDM_COMBOBOX:
                combobox_demo (hWnd);
                break;
            case IDM_TOOLBAR:
                newtoolbar_demo(hWnd);
                break;
            case IDM_TRACKBAR:
                trackbar_demo (hWnd);
                break;

            case IDM_LISTVIEW:
                listview_demo (hWnd);
                break;

            case IDM_TREEVIEW:
                treeview_demo (hWnd);
                break;
            case IDM_MONTHCALENDAR:
                monthcalendar_demo (hWnd);
                break;
            case IDM_SPINBOX:
                spinbox_demo (hWnd);
                break;
            case IDM_COOLBAR:
                coolbar_demo (hWnd);
                break;

            case IDM_SUBCLASS:
                subclass_demo (hWnd);
                break;

            case IDM_TIMEEDIT:
                timeeditor (hWnd);
                break;

            case IDM_ABOUT_THIS:
                MessageBox (hWnd, 
                    "MiniGUI Controls\n"
                    "Copyright (C) 2003 ~ 2007 Feynman Software.\n",
                    "About MiniGUI Controls",
                    MB_OK | MB_ICONINFORMATION | MB_BASEDONPARENT);
                break;
            
            case IDM_ABOUT:
#ifdef _MGMISC_ABOUTDLG
#ifdef _MGRM_THREADS
                OpenAboutDialog ();
#else
                OpenAboutDialog (hWnd);
#endif
#endif
                break;
        }
        break;

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
        return 0;
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

static void InitCreateInfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_BORDER | WS_CAPTION | WS_VISIBLE;
    pCreateInfo->dwExStyle = 0;
    pCreateInfo->spCaption = "MiniGUI Controls";
    pCreateInfo->hMenu = createmenu();
    pCreateInfo->hCursor = GetSystemCursor (IDC_ARROW);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = CtrlTestWinProc;
    pCreateInfo->lx = 200;
    pCreateInfo->ty = 100;
    pCreateInfo->rx = pCreateInfo->lx + 400;
    pCreateInfo->by = pCreateInfo->ty + 100;
    pCreateInfo->iBkColor = PIXEL_lightwhite; 
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

BITMAP bmp_bkgnd;
int MiniGUIMain (int args, const char* arg[])
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;

#ifdef _MGRM_PROCESSES
    int i;
    const char* layer = NULL;

    for (i = 1; i < args; i++) {
        if (strcmp (arg[i], "-layer") == 0) {
            layer = arg[i + 1];
            break;
        }
    }

    GetLayerInfo (layer, NULL, NULL, NULL);

    if (JoinLayer (layer, arg[0], 0, 0) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        return 1;
    }
#endif

    if (LoadBitmap (HDC_SCREEN, &bmp_bkgnd, "bkgnd.jpg"))
         return 1;

    InitCreateInfo (&CreateInfo);

    hMainWnd = CreateMainWindow (&CreateInfo);
    if (hMainWnd == HWND_INVALID)
        return 3;

    while (GetMessage (&Msg, hMainWnd)) {
        TranslateMessage (&Msg);
        DispatchMessage (&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    UnloadBitmap (&bmp_bkgnd);
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

