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
** menu.h: the head file of menu module.
**
** Create date: 1999/04/08
**
** Used abbreviations:
**  Menu: mnu
**  Popup: ppp
**  Identifier: id
**  Mnemonic: mnic
**  Normal: nml
**  Item: itm
*/

#ifndef GUI_MENU_H
    #define GUI_MENU_H

typedef struct _MENUITEM {
    short               category;
    short               type;
    UINT                mnutype;
    UINT                mnustate;
    LINT                id;
    PBITMAP             uncheckedbmp;
    PBITMAP             checkedbmp;
    DWORD               itemdata;
    DWORD               typedata;
    int                 h;
    struct _MENUITEM*   submenu;
    struct _MENUITEM*   next;
} MENUITEM;
typedef MENUITEM* PMENUITEM;

typedef struct _MENUBAR
{
    short               category;       // category of data.
    short               type;           // type of data.
    HWND                hwnd;           // owner.
    LINT                hilitedItem;    // hilited item.
    PMENUITEM           head;           // head of menu item list
} MENUBAR;
typedef MENUBAR* PMENUBAR;

#ifdef _MGRM_PROCESSES
#   undef _MENU_SAVE_BOX
#else
#   undef _MENU_SAVE_BOX
#endif

typedef struct _TRACKMENUINFO
{
    RECT                    rc;

    /*add*/
    RECT                    top_scroll_rc;
    RECT                    bottom_scroll_rc;
    RECT                    show_rc;
    RECT                    selected_rc;

    PMENUITEM               pstart_show_mi;
    PMENUITEM               before_pstart_mi;
    /*add end*/

    PMENUITEM               pmi;
    PMENUBAR                pmb;

    /*add*/
    int                     draw_bottom_flag; 
    int                     draw_top_flag; 
    int                     mouse_leave_flag;
    /*add end*/

    int                     barPos;
    PMENUITEM               philite;
    HWND                    hwnd;
    UINT                    flags;

#ifdef _MENU_SAVE_BOX
    BITMAP                  savedbox;
#endif

    unsigned int idx_znode;     /* idex of znode. */

    struct _TRACKMENUINFO*  prev;
    struct _TRACKMENUINFO*  next;
} TRACKMENUINFO;

/* consts for block data heaps */
#if defined (__NOUNIX__) || defined (__uClinux__)
    #define SIZE_MI_HEAP   8
    #define SIZE_MB_HEAP   1
    #define SIZE_TMI_HEAP  2
#else
  #ifndef _MGRM_THREADS
    #define SIZE_MI_HEAP   16
    #define SIZE_MB_HEAP   2
    #define SIZE_TMI_HEAP  4
  #else
    #define SIZE_MI_HEAP   512
    #define SIZE_MB_HEAP   64
    #define SIZE_TMI_HEAP  16
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _DEBUG
void DumpMenu (HMENU hmnu);
#endif  /* _DEBUG */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_MENU_H

