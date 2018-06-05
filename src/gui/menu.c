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
** menu.c: The Menu module.
**
** Create date: 1999.04.09
**
** Used abbreviations in this file:
**      Menu: mnu
**      Popup: ppp
**      Identifier: id
**      Mnemonic: mnic
**      Normal: nml
**      Item: itm
**      Modify records:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#ifdef _MGHAVE_MENU
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "blockheap.h"
#include "menu.h"

#define SHORTEN 0

/** constant length of menubar or menuitem */
#define LFRDR_MENUBAROFFX       8
#define LFRDR_INTERMENUITEMX    12
#define LFRDR_INTERMENUITEMY    2
#define LFRDR_MENUITEMOFFX      18
#define LFRDR_MENUTOPMARGIN     4
#define LFRDR_MENUBOTTOMMARGIN  4
#define LFRDR_MENULEFTMARGIN    4
#define LFRDR_MENURIGHTMARGIN   4
#define LFRDR_MENUITEMMINX      64
#define LFRDR_MENUSEPARATORY    4
#define LFRDR_MENUSEPARATORX    4

#define LFRDR_MENUBARITEMMARGIN 2

/** minimum menu pic height */
#define LFRDR_MENUPIC_MIN       6

/************************* MENUBAR allocation ********************************/
static BLOCKHEAP MBHeap;
inline static void InitFreeMBList (void)
{
    InitBlockDataHeap (&MBHeap, sizeof (MENUBAR), SIZE_MB_HEAP);
}

inline static PMENUBAR MenuBarAlloc (void)
{
    return (PMENUBAR) BlockDataAlloc (&MBHeap);
}

inline static void FreeMenuBar (PMENUBAR pmb)
{
    pmb->category = TYPE_UNDEFINED; 
    BlockDataFree (&MBHeap, pmb);
}

inline static void DestroyFreeMBList (void)
{
    DestroyBlockDataHeap (&MBHeap);
}

/************************* MENUITEM allocation *******************************/
static BLOCKHEAP MIHeap;
inline static void InitFreeMIList (void)
{
    InitBlockDataHeap (&MIHeap, sizeof (MENUITEM), SIZE_MI_HEAP);
}

inline static PMENUITEM MenuItemAlloc (void)
{
    return (PMENUITEM) BlockDataAlloc (&MIHeap);
}

inline static void FreeMenuItem (MENUITEM* pmi)
{
    pmi->category = TYPE_UNDEFINED; 
    BlockDataFree (&MIHeap, pmi);
}

inline static void DestroyFreeMIList (void)
{
    DestroyBlockDataHeap (&MIHeap);
}

/************************* TRACKMENUITEM heap *******************************/
static BLOCKHEAP TMIHeap;
inline static void InitFreeTMIList (void)
{
    InitBlockDataHeap (&TMIHeap, sizeof (TRACKMENUINFO), SIZE_TMI_HEAP);
}

inline static PTRACKMENUINFO TrackMenuInfoAlloc (void)
{
    return (PTRACKMENUINFO) BlockDataAlloc (&TMIHeap);
}

inline static void FreeTrackMenuInfo (TRACKMENUINFO* ptmi)
{
    BlockDataFree (&TMIHeap, ptmi);
}

inline static void DestroyFreeTMIList(void)
{
    DestroyBlockDataHeap (&TMIHeap);
}

/************************* Module initialization *****************************/

static BITMAP bmp_menuitem;

BOOL mg_InitMenu (void)
{
    InitFreeMBList ();
    InitFreeMIList ();
    InitFreeTMIList ();

    return InitBitmap (HDC_SCREEN_SYS, 16, 12, 0, NULL, &bmp_menuitem);
}

/************************* Module termination *******************************/
void mg_TerminateMenu (void)
{
    DestroyFreeTMIList ();
    DestroyFreeMBList ();
    DestroyFreeMIList ();

    UnloadBitmap (&bmp_menuitem);
}

/***************************** Menu creation *******************************/
#if 0
HMENU GUIAPI LoadMenuFromFile (const char* filename, int id)
{
    return 0;
}
#endif

HMENU GUIAPI CreateMenu (void)
{
    PMENUBAR pmb;

    if (!(pmb = MenuBarAlloc ()))
        return 0;
    
    pmb->category = TYPE_HMENU;
    pmb->type = TYPE_MENUBAR;
    pmb->head = NULL;

    return (HMENU)pmb;
}

HMENU GUIAPI CreatePopupMenu (PMENUITEMINFO pmii)
{
    PMENUITEM pmi;

#ifdef SHOW_BUG4491
    if (pmii->type == MFT_BITMAP || pmii->type == MFT_STRING) {
        if (pmii->typedata == 0)
            return 0;
    }
    else if (pmii->type == MFT_BMPSTRING) {
        if (pmii->typedata == 0 || pmii->uncheckedbmp == NULL)
            return 0;
    }
#else
    switch (pmii->type) {
    case MFT_BITMAP:
    case MFT_STRING:
    case MFT_RADIOCHECK:
    case MFT_MARKCHECK:
        if (pmii->typedata == 0)
            return 0;
        break;

    case MFT_BMPSTRING:
        if (pmii->typedata == 0 || pmii->uncheckedbmp == NULL)
            return 0;
        break;

    case MFT_SEPARATOR:
        break;

    default:
        return 0;
    }
#endif

    if (!(pmi = MenuItemAlloc ()))
        return 0;
   
    pmi->category = TYPE_HMENU;
    pmi->type = TYPE_PPPMENU;
    pmi->next = NULL;
    pmi->submenu = NULL;

    pmi->mnutype         = pmii->type;
    pmi->mnustate        = pmii->state;
    pmi->id              = pmii->id;
    pmi->uncheckedbmp    = pmii->uncheckedbmp;
    pmi->checkedbmp      = pmii->checkedbmp;
    pmi->itemdata        = pmii->itemdata;

    /* copy string. */
#ifdef SHOW_BUG4491
    if (pmii->type == MFT_STRING || pmii->type == MFT_BMPSTRING) {
        int len = strlen ((char*)pmii->typedata);
        pmi->typedata = (DWORD)FixStrAlloc (len);
        if (len > 0)
            strcpy ((char*)pmi->typedata, (char*)pmii->typedata);
    }
    else
        pmi->typedata = pmii->typedata;
#else
    switch (pmii->type) {
    case MFT_STRING:
    case MFT_RADIOCHECK:
    case MFT_MARKCHECK:
    case MFT_BMPSTRING:
    {
        int len = strlen ((char*)pmii->typedata);
        pmi->typedata = (DWORD)FixStrAlloc (len);
        if (len > 0)
            strcpy ((char*)pmi->typedata, (char*)pmii->typedata);
        break;
    }

    default:
        pmi->typedata = pmii->typedata;
        break;
    }
#endif

    if (pmi->mnutype == MFT_BITMAP) {
        if (pmi->uncheckedbmp == NULL)
            pmi->uncheckedbmp = (BITMAP*)pmi->typedata;
        if (pmi->checkedbmp == NULL)
            pmi->checkedbmp = pmi->uncheckedbmp;
    }
    else if (pmi->mnutype == MFT_BMPSTRING) {
        if (pmi->checkedbmp == NULL)
            pmi->checkedbmp = pmi->uncheckedbmp;
    }

    return (HMENU)pmi;
}

HMENU GUIAPI CreateSystemMenu (HWND hwnd, DWORD dwStyle)
{
    HMENU hmnu;
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_OPERATIONS);

    hmnu = CreatePopupMenu (&mii);

    memset (&mii, 0, sizeof(MENUITEMINFO));

    if (dwStyle & WS_MINIMIZEBOX) {
        mii.type        = MFT_STRING;
        mii.state       = 0;
        mii.id          = SC_MINIMIZE;
        mii.typedata    = (DWORD)GetSysText(IDS_MGST_MINIMIZE);
        InsertMenuItem(hmnu, 0, MF_BYPOSITION, &mii);
    }

    if (dwStyle & WS_MAXIMIZEBOX) {
        mii.type        = MFT_STRING;
        if (dwStyle & WS_MAXIMIZE)
            mii.state   = MFS_DISABLED;
        else
            mii.state   = 0;
        mii.id          = SC_MAXIMIZE;
        mii.typedata    = (DWORD)GetSysText(IDS_MGST_MAXIMIZE);
        InsertMenuItem(hmnu, 1, MF_BYPOSITION, &mii);

        mii.type        = MFT_STRING;
        if (dwStyle & WS_MAXIMIZE)
            mii.state   = 0;
        else
            mii.state   = MFS_DISABLED;
        mii.id          = SC_RESTORE;
        mii.typedata    = (DWORD)GetSysText(IDS_MGST_RESTORE);
        InsertMenuItem(hmnu, 2, MF_BYPOSITION, &mii);

        mii.type        = MFT_SEPARATOR;
        mii.state       = 0;
        mii.id          = 0;
        mii.typedata    = 0;
        InsertMenuItem(hmnu, 3, TRUE, &mii);
    }

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = SC_CLOSE;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_CLOSE);
    InsertMenuItem(hmnu, 4, MF_BYPOSITION, &mii);

    return hmnu;
}

static BOOL mnuInsertMenuItem (PMENUITEM pmi, PMENUITEM pnewmi,
                LINT item, BOOL flag)
{
    PMENUITEM ptmpmi;
    int index;
    
    if (flag) {
        // Insert this new menu item at the start.
        if (item == 0) {
            pnewmi->next = pmi;
            return TRUE;
        }
        
        index = 1;
        while (pmi->next) {

            if (index == item) {
                ptmpmi = pmi->next;
                pmi->next = pnewmi;
                pnewmi->next = ptmpmi;
                return FALSE;
            }
            
            index ++;
            pmi = pmi->next;
        }
    }
    else {
        if (item == pmi->id) {
            pnewmi->next = pmi;
            return TRUE;
        }
        
        while (pmi->next) {

            if (pmi->next->id == item) {
                ptmpmi = pmi->next;
                pmi->next = pnewmi;
                pnewmi->next = ptmpmi;
                return FALSE;
            }
            
            pmi = pmi->next;
        }
    }

    // append this new menu item at the end.
    pmi->next = pnewmi;

    return FALSE;
}

int GUIAPI InsertMenuItem (HMENU hmnu, LINT item, 
                            UINT flag, PMENUITEMINFO pmii)
{
    PMENUBAR pmb;
    PMENUITEM pnewmi, pmi;

#ifdef SHOW_BUG4491
    if (pmii->type == MFT_BITMAP || pmii->type == MFT_STRING) {
        if (pmii->typedata == 0)
            return ERR_INVALID_ARGS;
    }
    else if (pmii->type == MFT_BMPSTRING) {
        if (pmii->typedata == 0 || pmii->uncheckedbmp == NULL)
            return ERR_INVALID_ARGS;
    }
#else
    switch (pmii->type) {
    case MFT_BITMAP:
    case MFT_STRING:
    case MFT_RADIOCHECK:
    case MFT_MARKCHECK:
        if (pmii->typedata == 0)
            return ERR_INVALID_ARGS;
        break;

    case MFT_BMPSTRING:
        if (pmii->typedata == 0 || pmii->uncheckedbmp == NULL)
            return ERR_INVALID_ARGS;
        break;

    case MFT_SEPARATOR:
        break;

    default:
        return ERR_INVALID_ARGS;
    }
#endif

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE; 
    
    if (!(pnewmi = MenuItemAlloc ())) 
        return ERR_RES_ALLOCATION;

    pnewmi->category        = TYPE_HMENU;
    pnewmi->type            = TYPE_NMLMENU;
    pnewmi->mnutype         = pmii->type;
    pnewmi->mnustate        = pmii->state;
    pnewmi->id              = pmii->id;
    pnewmi->uncheckedbmp    = pmii->uncheckedbmp;
    pnewmi->checkedbmp      = pmii->checkedbmp;
    pnewmi->itemdata        = pmii->itemdata;
    pnewmi->next            = NULL;
    pnewmi->submenu         = (PMENUITEM)(pmii->hsubmenu);
    if (pnewmi->mnutype == MFT_SEPARATOR){
        pnewmi->submenu         = NULL;
    }

    /* copy string. */
#ifdef SHOW_BUG4491
    if (pmii->type == MFT_STRING || pmii->type == MFT_BMPSTRING) {
        int len = strlen ((char*)pmii->typedata);
        pnewmi->typedata = (DWORD)FixStrAlloc (len);
        if (len > 0)
            strcpy ((char*)pnewmi->typedata, (char*)pmii->typedata);
    }
    else
        pnewmi->typedata = pmii->typedata;
#else
    switch (pmii->type) {
    case MFT_STRING:
    case MFT_RADIOCHECK:
    case MFT_MARKCHECK:
    case MFT_BMPSTRING:
    {
        int len = strlen ((char*)pmii->typedata);
        pnewmi->typedata = (DWORD)FixStrAlloc (len);
        if (len > 0)
            strcpy ((char*)pnewmi->typedata, (char*)pmii->typedata);
        break;
    }

    default:
        pnewmi->typedata = pmii->typedata;
        break;
    }
#endif
    
    if (pnewmi->mnutype == MFT_BITMAP) {
        if (pnewmi->uncheckedbmp == NULL)
            pnewmi->uncheckedbmp = (BITMAP*)pnewmi->typedata;
        if (pnewmi->checkedbmp == NULL)
            pnewmi->checkedbmp = pnewmi->uncheckedbmp;
    }
    else if (pnewmi->mnutype == MFT_BMPSTRING) {
        if (pnewmi->checkedbmp == NULL)
            pnewmi->checkedbmp = pnewmi->uncheckedbmp;
    }

    if (pmb->type == TYPE_MENUBAR) {
        pmi = pmb->head;
        
        if (!pmi)
            pmb->head = pnewmi;
        else {
            if (mnuInsertMenuItem (pmb->head, pnewmi, item, flag))
                pmb->head = pnewmi;
        }
    }
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM)hmnu;

        if (!pmi->submenu)
            pmi->submenu = pnewmi;
        else {
            if (mnuInsertMenuItem (pmi->submenu, pnewmi, item, flag))
                pmi->submenu = pnewmi;
        }
    }
    else
        return ERR_INVALID_HMENU;

   return 0;
}

static void mnuDeleteMenuItem (PMENUITEM pmi)
{
    PMENUITEM ptmpmi;
    PMENUITEM psubmi;

    if (pmi->submenu) {
        psubmi = pmi->submenu;
        while (psubmi) {
            ptmpmi = psubmi->next;
            mnuDeleteMenuItem (psubmi);
            psubmi = ptmpmi;
        }
    }

#ifdef SHOW_BUG4491
    if (pmi->mnutype == MFT_STRING || pmi->mnutype == MFT_BMPSTRING)
        FreeFixStr ((void*)pmi->typedata);
#else
    switch (pmi->mnutype) {
    case MFT_STRING:
    case MFT_RADIOCHECK:
    case MFT_MARKCHECK:
    case MFT_BMPSTRING:
        FreeFixStr ((void*)pmi->typedata);
        break;
    }
#endif
        
    FreeMenuItem (pmi);
}

static PMENUITEM mnuRemoveMenuItem (PMENUITEM phead, LINT item, BOOL flag)
{
    int index;
    PMENUITEM pmi, ptmpmi;
    
    if (!phead) return NULL;
    
    if (flag) {
    
        if (item == 0) {
            pmi = phead->next;
            if (!phead->submenu) 
                mnuDeleteMenuItem (phead);
            return pmi;
        }
        
        pmi = phead;
        index = 1;
        while (pmi->next) {

            if (index == item) {
                ptmpmi = pmi->next;
                pmi->next = pmi->next->next;
                
                if (!ptmpmi->submenu)
                    mnuDeleteMenuItem (ptmpmi);

                return phead;
            }

            index ++;
            pmi = pmi->next;
        }
    }
    else {
        if (phead->id == item) {
            pmi = phead->next;
            if (!phead->submenu) 
                mnuDeleteMenuItem (phead);
            return pmi;
        }
        
        pmi = phead;
        while (pmi->next) {

            if (pmi->next->id == item) {
                ptmpmi = pmi->next;
                pmi->next = pmi->next->next;
                
                if (!ptmpmi->submenu)
                    mnuDeleteMenuItem (ptmpmi);

                return phead;
            }

            pmi = pmi->next;
        }
    }

    return phead;
}

int GUIAPI RemoveMenu (HMENU hmnu, LINT item, UINT flags)
{
    PMENUBAR pmb;
    PMENUITEM pmi;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE;
    
    if (pmb->type == TYPE_MENUBAR)
        pmb->head = mnuRemoveMenuItem (pmb->head, item, 
            flags & MF_BYPOSITION);
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi->submenu = mnuRemoveMenuItem (pmi->submenu, item, 
            flags & MF_BYPOSITION);
    }
    else
        return ERR_INVALID_HMENU;   /* this is a normal menu item. */

    return 0;
}

static PMENUITEM mnuDeleteMenu (PMENUITEM phead, LINT item, BOOL flag)
{
    int index;
    PMENUITEM pmi, ptmpmi;
    
    if (!phead) return NULL;
    
    if (flag) {
    
        if (item == 0) {
            pmi = phead->next;
            mnuDeleteMenuItem (phead);
            return pmi;
        }
        
        pmi = phead;
        index = 1;
        while (pmi->next) {

            if (index == item) {
                ptmpmi = pmi->next;
                pmi->next = pmi->next->next;
                mnuDeleteMenuItem (ptmpmi);
                return phead;
            }

            index ++;
            pmi = pmi->next;
        }
    }
    else {
        if (phead->id == item) {
            pmi = phead->next;
            mnuDeleteMenuItem (phead);
            return pmi;
        }
        
        pmi = phead;
        while (pmi->next) {

            if (pmi->next->id == item) {
                ptmpmi = pmi->next;
                pmi->next = pmi->next->next;
                mnuDeleteMenuItem (ptmpmi);
                return phead;
            }

            pmi = pmi->next;
        }
    }

    return phead;
}

int GUIAPI DeleteMenu (HMENU hmnu, LINT item, UINT flags)
{
    PMENUBAR pmb;
    PMENUITEM pmi;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE;
    
    if (pmb->type == TYPE_MENUBAR){
        pmb->head = mnuDeleteMenu (pmb->head, item, 
            flags & MF_BYPOSITION);

        SendMessage (HWND_DESKTOP, 
                        MSG_PAINT, 0, 0);
    }
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi->submenu = mnuDeleteMenu (pmi->submenu, item, 
            flags & MF_BYPOSITION);
    }
    else
        return ERR_INVALID_HMENU;   /* this is a normal menu item. */

    return 0;
}

int GUIAPI DestroyMenu (HMENU hmnu)
{
    PMENUBAR pmb;
    PMENUITEM pmi, ptmpmi;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE;
    
    if (pmb->type == TYPE_MENUBAR) {
        pmi = pmb->head;
        while (pmi) {
            ptmpmi = pmi->next;
            mnuDeleteMenuItem (pmi);
            pmi = ptmpmi;
        }

        FreeMenuBar (pmb);
    }
    else{
        mnuDeleteMenuItem ((PMENUITEM)hmnu);
    }

    return 0;
}

int GUIAPI IsMenu (HMENU hmnu)
{
    PMENUBAR pMenu = (PMENUBAR)hmnu;

    if (pMenu->category != TYPE_HMENU) return 0;

    return pMenu->type;
}

/*************************** Menu properties support *************************/
int GUIAPI GetMenuItemCount (HMENU hmnu)
{
    PMENUBAR pmb;
    PMENUITEM pmi;
    int count;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE;
    
    if (pmb->type == TYPE_MENUBAR)
        pmi = pmb->head;
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi = pmi->submenu;
    }
    else
        pmi = (PMENUITEM) hmnu;
    
    count = 0;
    while (pmi) {
        count ++;
        pmi = pmi->next;
    }
    
    return count;
}

static PMENUITEM mnuGetMenuItem (HMENU hmnu, LINT item, BOOL flag)
{
    PMENUBAR pmb;
    PMENUITEM pmi;
    int index;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return NULL; 
    
    if (pmb->type == TYPE_MENUBAR)
        pmi = pmb->head;
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi = pmi->submenu;
    }
    else
        pmi = (PMENUITEM) hmnu;

    if (flag) {
        index = 0;
        while (pmi) {
            if (index == item)
                return pmi;

            index ++;
            pmi = pmi->next;
        }
    }
    else {
        while (pmi) {
            if (pmi->id == item)
                return pmi;

            pmi = pmi->next;
        }
    }

    return NULL; 
}

LINT GUIAPI GetMenuItemID (HMENU hmnu, int pos)
{
    PMENUBAR pmb;
    PMENUITEM pmi;
    int index;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE;
    
    if (pmb->type == TYPE_MENUBAR)
        pmi = pmb->head;
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi = pmi->submenu;
    }
    else
        pmi = (PMENUITEM) hmnu;
    
    index = 0;
    while (pmi) {

        if (index == pos)
            return pmi->id;

        index ++;
        pmi = pmi->next;
    }
    
    return ERR_INVALID_POS;
}

HMENU GUIAPI GetPopupSubMenu (HMENU hpppmnu)
{
    PMENUITEM pmi;

    pmi = (PMENUITEM) hpppmnu;

    if (pmi->category != TYPE_HMENU)
        return 0;
    
    if (pmi->type != TYPE_PPPMENU)
        return 0;

    return (HMENU)(pmi->submenu);
}

HMENU GUIAPI StripPopupHead (HMENU hpppmnu)
{
    PMENUITEM pmi;
    HMENU hsubmenu;

    pmi = (PMENUITEM) hpppmnu;

    if (pmi->category != TYPE_HMENU)
        return 0;
    
    if (pmi->type != TYPE_PPPMENU)
        return 0;

    hsubmenu = (HMENU)(pmi->submenu);

#ifdef SHOW_BUG4491
    if (pmi->mnutype == MFT_STRING || pmi->mnutype == MFT_BMPSTRING)
        FreeFixStr ((void*)pmi->typedata);
#else
    switch (pmi->mnutype) {
    case MFT_STRING:
    case MFT_RADIOCHECK:
    case MFT_MARKCHECK:
    case MFT_BMPSTRING:
        FreeFixStr ((void*)pmi->typedata);
        break;
    }
#endif

    FreeMenuItem (pmi);

    return hsubmenu;
}

HMENU GUIAPI GetSubMenu (HMENU hmnu, int pos)
{
    PMENUBAR pmb;
    PMENUITEM pmi;
    int index;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return 0;
    
    if (pmb->type == TYPE_MENUBAR)
        pmi = pmb->head;
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi = pmi->submenu;
    }
    else
        return 0;   // this is a normal menu item. 
    
    index = 0;
    while (pmi) {

        if (index == pos)
            return (HMENU)(pmi->submenu);

        index ++;
        pmi = pmi->next;
    }
    
    return 0;
}

int GUIAPI GetMenuItemInfo (HMENU hmnu, LINT item, 
                            UINT flag, PMENUITEMINFO pmii)
{
    PMENUITEM pmi;

    if (!(pmi = mnuGetMenuItem (hmnu, item, flag & MF_BYPOSITION)))
        return ERR_INVALID_HMENU;

    if (pmii->mask & MIIM_CHECKMARKS) {
        pmii->checkedbmp   = pmi->checkedbmp;
        pmii->uncheckedbmp = pmi->uncheckedbmp;
    }

    if (pmii->mask & MIIM_DATA) {
        pmii->itemdata      = pmi->itemdata;
    }

    if (pmii->mask & MIIM_ID) {
        pmii->id            = pmi->id;
    }

    if (pmii->mask & MIIM_STATE) {
        pmii->state         = pmi->mnustate;
    }

    if (pmii->mask & MIIM_SUBMENU) {
        pmii->hsubmenu      = (HMENU)(pmi->submenu);
    }

    if (pmii->mask & MIIM_TYPE) {
        pmii->type          = pmi->mnutype;
#ifdef SHOW_BUG4491
        if (pmii->type == MFT_STRING)
            strncpy ((char*)(pmii->typedata), (char*)(pmi->typedata), 
                pmii->cch);
        else
            pmii->typedata  = pmi->typedata;
#else
        switch (pmii->type) {
        case MFT_STRING:
        case MFT_RADIOCHECK:
        case MFT_MARKCHECK:
        case MFT_BMPSTRING:
            strncpy ((char*)(pmii->typedata), (char*)(pmi->typedata), 
                pmii->cch);
            break;

        default:
            pmii->typedata  = pmi->typedata;
            break;
        }
#endif
    }

    return 0;
}

int GUIAPI SetMenuItemInfo (HMENU hmnu, LINT item, 
                            UINT flag, PMENUITEMINFO pmii)
{
    PMENUITEM pmi;

    if (!(pmi = mnuGetMenuItem (hmnu, item, flag & MF_BYPOSITION)))
        return ERR_INVALID_HMENU;

    if (pmii->mask & MIIM_CHECKMARKS) {
        if (pmi->mnutype == MFT_BMPSTRING && pmii->uncheckedbmp == NULL)
            return ERR_INVALID_ARGS;

        pmi->uncheckedbmp = pmii->uncheckedbmp;
        pmi->checkedbmp   = pmii->checkedbmp;
    }

    if (pmii->mask & MIIM_DATA) {
        pmi->itemdata      = pmii->itemdata;
    }

    if (pmii->mask & MIIM_ID) {
        pmi->id            = pmii->id;
    }

    if (pmii->mask & MIIM_STATE) {
        pmi->mnustate         = pmii->state;
    }

    if (pmii->mask & MIIM_SUBMENU) {
        pmi->submenu      = (PMENUITEM)(pmii->hsubmenu);
    }

    if (pmii->mask & MIIM_TYPE) {

#ifdef SHOW_BUG4491
        if (pmi->mnutype == MFT_STRING || pmi->mnutype == MFT_BMPSTRING)
            FreeFixStr ((char*)pmi->typedata);
#else
        switch (pmi->mnutype) {
        case MFT_STRING:
        case MFT_RADIOCHECK:
        case MFT_MARKCHECK:
        case MFT_BMPSTRING:
            if (pmi->typedata)
                FreeFixStr ((void*)pmi->typedata);
            break;
        }
#endif
        
        pmi->mnutype          = pmii->type;

#ifdef SHOW_BUG4491
        if ((pmi->mnutype == MFT_STRING 
                        || pmi->mnutype == MFT_BMPSTRING 
                        || pmi->mnutype == MFT_BITMAP)
                        && pmii->typedata == 0)
            return ERR_INVALID_ARGS;

        if (pmi->mnutype == MFT_STRING || pmi->mnutype == MFT_BMPSTRING) {
            int len = strlen ((char*)pmii->typedata);
            pmi->typedata = (DWORD)FixStrAlloc (len);
            if (len > 0)
                strcpy ((char*)pmi->typedata, (char*)pmii->typedata);
        }
        else
            pmi->typedata = pmii->typedata;
#else
        switch (pmii->type) {
        case MFT_STRING:
        case MFT_RADIOCHECK:
        case MFT_MARKCHECK:
        case MFT_BMPSTRING:
            {
				int len;
                if (pmii->typedata == 0) {
                    pmi->typedata = 0;
                    return ERR_INVALID_ARGS;
                }

                len = strlen ((char*)pmii->typedata);
                pmi->typedata = (DWORD)FixStrAlloc (len);
                if (len > 0)
                    strcpy ((char*)pmi->typedata, (char*)pmii->typedata);
                break;
            }

        case MFT_BITMAP:
            if (pmii->typedata == 0)
                return ERR_INVALID_ARGS;
            /* no break */

        default:
            pmi->typedata = pmii->typedata;
            break;
        }
#endif
    }

    if (pmi->mnutype == MFT_BITMAP) {
        if (pmi->uncheckedbmp == NULL)
            pmi->uncheckedbmp = (BITMAP*)pmi->typedata;
        if (pmi->checkedbmp == NULL)
            pmi->checkedbmp = pmi->uncheckedbmp;
    }
    else if (pmi->mnutype == MFT_BMPSTRING) {
        if (pmi->checkedbmp == NULL)
            pmi->checkedbmp = pmi->uncheckedbmp;
    }

    return 0;
}

UINT GUIAPI EnableMenuItem (HMENU hmnu, LINT item, UINT flags)
{
    PMENUITEM pmi;
    UINT prevstate = 0xFFFFFFFF; 

    if (!(pmi = mnuGetMenuItem (hmnu, item, flags & MF_BYPOSITION)))
        return prevstate;

    prevstate = 0x0000000F & pmi->mnustate;

    pmi->mnustate = (0xFFFFFFF0 & pmi->mnustate) | flags;

    return prevstate;
}

int GUIAPI CheckMenuRadioItem (HMENU hmnu, LINT first, LINT last, 
                            LINT checkitem, UINT flags)
{
    PMENUITEM pmi;
    int index;

    if (!(pmi = mnuGetMenuItem (hmnu, first, flags & MF_BYPOSITION)))
        return ERR_INVALID_HMENU;

    index = first;
    if (flags & MF_BYPOSITION) {
        while (pmi) {
            /* fix bug 4491 */
            if (pmi->mnutype == MFT_STRING)
                pmi->mnutype = MFT_RADIOCHECK;

            if (index == checkitem)
                pmi->mnustate = (0xFFFFFFF0 & pmi->mnustate) | MFS_CHECKED;
            else
                pmi->mnustate = (0xFFFFFFF0 & pmi->mnustate) | MFS_UNCHECKED;
            
            if (index == last)
                break;

            index ++;
            pmi = pmi->next;
        }
    }
    else {
        while (pmi) {
            /* fix bug 4491 */
            if (pmi->mnutype == MFT_STRING)
                pmi->mnutype = MFT_RADIOCHECK;

            if (pmi->id == checkitem)
                pmi->mnustate = (0xFFFFFFF0 & pmi->mnustate) | MFS_CHECKED;
            else
                pmi->mnustate = (0xFFFFFFF0 & pmi->mnustate) | MFS_UNCHECKED;
            
            if (pmi->id == last)
                break;

            pmi = pmi->next;
        }
    }

    return 0;
}

int GUIAPI SetMenuItemBitmaps (HMENU hmnu, LINT item, UINT flags, 
                            PBITMAP hBmpUnchecked, PBITMAP hBmpChecked)
{
    PMENUITEM pmi;

    if (!(pmi = mnuGetMenuItem (hmnu, item, flags & MF_BYPOSITION)))
        return ERR_INVALID_HMENU;

    if (pmi->mnutype == MFT_BMPSTRING && hBmpUnchecked == NULL)
            return ERR_INVALID_ARGS;

    pmi->checkedbmp = hBmpChecked;
    pmi->uncheckedbmp = hBmpUnchecked;

    if (pmi->mnutype == MFT_BITMAP) {
        if (pmi->uncheckedbmp == NULL)
            pmi->uncheckedbmp = (BITMAP*)pmi->typedata;
        if (pmi->checkedbmp == NULL)
            pmi->checkedbmp = pmi->uncheckedbmp;
    }
    else if (pmi->mnutype == MFT_BMPSTRING) {
        if (pmi->checkedbmp == NULL)
            pmi->checkedbmp = pmi->uncheckedbmp;
    }

    return 0;
}

#ifdef _DEBUG

void mnuDumpMenuItem (PMENUITEM pmi)
{
    PMENUITEM ptmpmi;
    
    printf ("Menu Item (0x%p) Information:\n", pmi);

    printf ("\tdata category:      %d\n", pmi->category);
    printf ("\tdata type:       %d\n", pmi->type);
    printf ("\tmenu type:       %#x\n", pmi->mnutype);
    printf ("\tmenu state:      %#x\n", pmi->mnustate);
    printf ("\tmenu id:         %ld\n", pmi->id);
    printf ("\tchecked bitmap:  0x%p\n", pmi->checkedbmp);
    printf ("\tunchecked bitmap:0x%p\n", pmi->uncheckedbmp);
    printf ("\titem data:       %p\n", (PVOID)pmi->itemdata);
    if (pmi->mnutype == MFT_STRING)
        printf ("\tstring:         %s\n", (char*)pmi->typedata);
    else
        printf ("\ttype data:      %p\n", (PVOID)pmi->typedata);
    printf ("\tnext item:       0x%p\n", pmi->next);
    
    if (pmi->submenu) {
        ptmpmi = pmi->submenu;
        while (ptmpmi) {
            mnuDumpMenuItem (ptmpmi);
            ptmpmi = ptmpmi->next;
        }
    }

    printf ("End of Info of Menu Item: 0x%p\n", pmi);
}

void DumpMenu (HMENU hmnu)
{
    PMENUBAR pmb;
    PMENUITEM pmi;

    pmb = (PMENUBAR) hmnu;
    if (pmb->category != TYPE_HMENU) {
        printf ("hmnu is not a valid menu handle.\n");
        return;
    }
    
    if (pmb->type == TYPE_MENUBAR) {
        printf ("hmnu is a menu bar.\n");
        
        pmi = pmb->head;
        while (pmi) {
            mnuDumpMenuItem (pmi);
            pmi = pmi->next;
        }
    }
    else if (pmb->type == TYPE_PPPMENU) {
        printf ("hmnu is a popup menu.\n");
        pmi = (PMENUITEM)hmnu;
        mnuDumpMenuItem (pmi);
    }
    else {
        printf ("hmnu is a normal menu item.\n");
        pmi = (PMENUITEM)hmnu;
        mnuDumpMenuItem (pmi);
    }
}
#endif  // _DEBUG

/***************************** Menu owner ***********************************/
// Global function defined in Desktop module.

HMENU GUIAPI SetMenu (HWND hwnd, HMENU hmnu)
{
    PMAINWIN pWin;
    HMENU hOld;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hwnd))) return 0;

    hOld = pWin->hMenu;
    pWin->hMenu = hmnu;

    if (hmnu == 0) {
        return 0;
    }

    ((PMENUBAR)hmnu)->hwnd = hwnd;

    DrawMenuBar (hwnd);

    return hOld;
}

HMENU GUIAPI GetMenu (HWND hwnd)
{
    PMAINWIN pWin;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hwnd))) return 0;

    return pWin->hMenu;
}

HMENU GUIAPI GetSystemMenu (HWND hwnd, BOOL flag)
{
    PMAINWIN pWin;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hwnd))) return 0;

    return pWin->hSysMenu;
}

/**************************** Menu drawing support ***************************/
static void mnuGetMenuBarItemSize (HWND hwnd, PMENUITEM pmi, SIZE* size)
{
    size->cy = ((PLOGFONT)GetWindowElementAttr (hwnd, WE_FONT_MENU))->size;

    /* fix bug #4491: use == instead of & for mnutype. */
    if (pmi->mnutype == MFT_BITMAP) {
        size->cx = pmi->checkedbmp->bmWidth;
    }
    else if (pmi->mnutype == MFT_SEPARATOR) {
        size->cx = LFRDR_MENUSEPARATORX; 
    }
    else if (pmi->mnutype == MFT_OWNERDRAW) {
    }
    else {
        SelectFont (HDC_SCREEN_SYS, (PLOGFONT)GetWindowElementAttr 
                (hwnd, WE_FONT_MENU));
        GetTextExtent (HDC_SCREEN_SYS, (char*) pmi->typedata, -1, size);
        if (pmi->mnutype == MFT_BMPSTRING) {
            size->cx += size->cy;
            size->cx += LFRDR_INTERMENUITEMX>>1;
        }
    }

    pmi->h = size->cx;
    return;
}

static void mnuDrawMenuBarItem (HWND hwnd, HDC hdc, PMENUITEM pmi, 
                int x, int y, int w, int h, int rect_y, int rect_h)
{
    PBITMAP bmp;
    PMAINWIN pWin;

    /** the menu item rect to be drawn*/
    RECT rect;
    int height;
    int inter;
    int old_mode;

    DWORD darker_dword;
    DWORD lighter_dword;
    DWORD bgc_dword;
    gal_pixel darker_pixel;
    gal_pixel lighter_pixel;

    pWin = (PMAINWIN) hwnd;
    if (NULL == pWin) 
        return;

    if (!pWin->we_rdr) {
        _MG_PRINTF ("GUI>Menu: LFRDR is NULL for window: %p\n", pWin);
        return;
    }

    height = ((PLOGFONT)GetWindowElementAttr (hwnd, WE_FONT_MENU))->size;

    inter = LFRDR_INTERMENUITEMX>>1;

    /** rect to be drawn */
    rect.left = x - inter;
    rect.top = rect_y + LFRDR_MENUBARITEMMARGIN;
    rect.right = rect.left + w + (inter << 1);
    rect.bottom = rect_y + rect_h - LFRDR_MENUBARITEMMARGIN;

    /** draw the rect, high priority for disabled item. */
    if (pmi->mnustate & MFS_DISABLED)
    {
        /** render a disabled item */
        SetTextColor(hdc, GetWindowElementPixelEx(hwnd, hdc, WE_FGC_DISABLED_ITEM));
        if (pmi->mnustate & MFS_HILITE)
            pWin->we_rdr->draw_disabled_menu_item (hwnd, hdc, &rect,
                    GetWindowElementAttr (hwnd, WE_BGC_DISABLED_ITEM));
        else
            pWin->we_rdr->draw_disabled_menu_item (hwnd, hdc, &rect,
                    GetWindowElementAttr (hwnd, WE_BGC_MENU));

    }
    else if (pmi->mnustate & MFS_HILITE)
    {
        /** render a hilite item */
        SetTextColor (hdc, GetWindowElementPixelEx 
                (hwnd, hdc, WE_FGC_HIGHLIGHT_ITEM));
        pWin->we_rdr->draw_hilite_menu_item (hwnd, hdc, &rect,
                GetWindowElementAttr (hwnd, WE_BGC_HIGHLIGHT_ITEM));
    }
    else
    {
        /** render a normal item as unhilite */
        SetTextColor (hdc, GetWindowElementPixelEx 
                (hwnd, hdc, WE_FGC_MENU));
        pWin->we_rdr->draw_normal_menu_item (hwnd, hdc, &rect,
                GetWindowElementAttr (hwnd, WE_MAINC_THREED_BODY));
    }

    /* fix bug #4491: use == instead of & for mnutype. */
    if (pmi->mnutype == MFT_BITMAP) {
    
        if (pmi->mnustate & MFS_CHECKED)
            bmp = pmi->checkedbmp;
        else
            bmp = pmi->uncheckedbmp;
            
        FillBoxWithBitmap (hdc, x, y, pmi->h, height, bmp);
    }
    else if (pmi->mnutype == MFT_SEPARATOR) {
        
        /** two vertical separator */

        bgc_dword = GetWindowElementAttr (hwnd, WE_BGC_MENU);
        darker_dword = pWin->we_rdr->calc_3dbox_color 
            (bgc_dword, LFRDR_3DBOX_COLOR_DARKEST);
        lighter_dword = pWin->we_rdr->calc_3dbox_color 
            (bgc_dword, LFRDR_3DBOX_COLOR_LIGHTEST);

        darker_pixel = DWORD2Pixel (hdc, darker_dword);
        lighter_pixel = DWORD2Pixel (hdc, lighter_dword);

        if (rect.top < rect.bottom - 2)
        {
            SetPenColor (hdc, darker_pixel); 
            MoveTo (hdc, 
                x + (pmi->h>>1), rect.top);
            LineTo (hdc, 
                x + (pmi->h>>1), rect.bottom - 2);

            SetPenColor (hdc, lighter_pixel);
            MoveTo (hdc, 
                x + (pmi->h>>1) + 1, rect.top);
            LineTo (hdc, 
                x + (pmi->h>>1) + 1, rect.bottom - 2);
        }
    }
    else if (pmi->mnutype == MFT_OWNERDRAW) {
    }
    else {
        if (pmi->mnutype == MFT_BMPSTRING) {
            if (pmi->mnustate & MFS_CHECKED)
                bmp = pmi->checkedbmp;
            else
                bmp = pmi->uncheckedbmp;
            
            FillBoxWithBitmap (hdc, x, y, height, height, bmp);
            x += bmp->bmWidth + inter;
        }

        SelectFont (hdc, (PLOGFONT)GetWindowElementAttr 
                (hwnd, WE_FONT_MENU));
        
        old_mode = SetBkMode (hdc, BM_TRANSPARENT);
        TextOut (hdc, x, y, (char *)pmi->typedata); 
        SetBkMode (hdc, old_mode);
    }
}

static int mnuGetNextMenuBarItem (PMENUBAR pmb, int pos)
{
    PMENUITEM pmi;
    int number;

    number = 0;
    pmi = pmb->head;
    while (pmi) {
        number ++;
        pmi = pmi->next;
    }

    number--;

    if (pos == number)
        return 0;
    else
        return pos + 1;
}

static int mnuGetPrevMenuBarItem (PMENUBAR pmb, int pos)
{
    PMENUITEM pmi;
    int number;

    number = 0;
    pmi = pmb->head;
    while (pmi) {
        number ++;
        pmi = pmi->next;
    }

    number--;

    if (pos == 0)
        return number;
    else
        return pos - 1;
}

/*
 * GetMenuOffy
 * get offset of font
 *
 * Author : zhounuohua<zhounuohua@minigui.com>
 * Data   : 2007-11-23
 */
static int GetMenuOffy (HWND hwnd, int menu_size)
{
    int font_size;
    int menu_size_min;
    int off;
    if (0 == menu_size)
    {
        menu_size = GetWindowElementAttr (hwnd, WE_METRICS_MENU);
    }
    font_size = ((PLOGFONT)GetWindowElementAttr (hwnd, WE_FONT_MENU))->size;
    menu_size_min = font_size + (LFRDR_MENUITEMOFFY_MIN << 1);
    if (menu_size > menu_size_min)
    {
        off = menu_size - font_size;
        return off >> 1;
    }
    return LFRDR_MENUITEMOFFY_MIN;
}

HMENU GUIAPI GetMenuBarItemRect (HWND hwnd, int pos, RECT* prc)
{
    PMAINWIN pWin;
    PMENUBAR pmb;
    PMENUITEM pmi;
    int x, y, inter, h;
    int count;
    SIZE size;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hwnd))) return 0;

    if (!pWin->hMenu) return 0;

    pmb = (PMENUBAR) (pWin->hMenu);
    if (pmb->category != TYPE_HMENU) return 0;
    if (pmb->type != TYPE_MENUBAR) return 0;

    x = pWin->cl - pWin->left;
    x += LFRDR_MENUBAROFFX;
    y = pWin->ct - pWin->top; 
    h = GetWindowElementAttr (hwnd, WE_METRICS_MENU);
    y -= h;

    inter = LFRDR_INTERMENUITEMX;

    count = 0;
    pmi = pmb->head;
    while (pmi) {

        mnuGetMenuBarItemSize (hwnd, pmi, &size);

        if (count == pos)
            break;

        x = x + inter + size.cx;
        count ++;
        
        pmi = pmi->next;
    }

    if (pmi == NULL) return 0;

    prc->left = x;
    prc->top  = y;
    prc->bottom = y + h;
    prc->right = x + size.cx;

    return (HMENU) pmi;
}

static BOOL mnuGetMenuBarRect (HWND hwnd, RECT* prc)
{
    PMAINWIN pWin;
    int h;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hwnd))) return FALSE;
    
    prc->left = pWin->cl - pWin->left;

    h = GetWindowElementAttr (hwnd, WE_METRICS_MENU);
    prc->top = pWin->ct - pWin->top - h; 

    prc->right = pWin->right - pWin->left - prc->left;
    prc->bottom = pWin->bottom - pWin->top - prc->top;

    return TRUE;
}

BOOL GUIAPI HiliteMenuBarItem (HWND hwnd, int pos, UINT flags)
{
    PMENUITEM pmi;
    RECT rcBar;
    RECT rc;
    HDC hdc;
    PMAINWIN pWin;

    int font_y;         /** top of font */
    int font_h;         /** height of font */
    int menu_size;      /** size of menu item */
    int menubar_offy;   /** offset of font */

    pWin = (PMAINWIN)hwnd;
    if (NULL == pWin) return FALSE;

    pmi = (PMENUITEM)GetMenuBarItemRect (hwnd, pos, &rc);

    if (!pmi) return FALSE;

    if (pmi->mnutype == MFT_SEPARATOR)
        return TRUE; 

    hdc = GetDC (hwnd);

    mnuGetMenuBarRect (hwnd, &rcBar);
    ClipRectIntersect (hdc, &rcBar);

    menu_size = GetWindowElementAttr (hwnd, WE_METRICS_MENU);
    menubar_offy = GetMenuOffy (hwnd, menu_size);

    font_y = pWin->ct - pWin->top;
    font_y -= menu_size;
    font_y += menubar_offy;

    font_h = menu_size - (menubar_offy << 1);
    if (font_h < 0)
        font_h = 0;

    switch (flags)
    {
        /** high priority for disabled item */
        case LFRDR_MENU_STATE_DISABLED:
            pmi->mnustate |= MFS_DISABLED;
            break;
        case LFRDR_MENU_STATE_HILITE:
            pmi->mnustate |= MFS_HILITE;
        break;
        case LFRDR_MENU_STATE_NORMAL:
        default:
            pmi->mnustate |= MFS_DEFAULT;
        break;
    }

    mnuDrawMenuBarItem (hwnd, hdc, pmi, rc.left, font_y, 
            RECTW (rc) , font_h, rc.top, RECTH (rc));

    /** reset draw state */
    pmi->mnustate &= ~(MFS_HILITE | MFS_DEFAULT);
    ReleaseDC (hdc);
    return TRUE;
}

int MenuBarHitTest (HWND hwnd, int mx, int my)
{
    PMAINWIN pWin;
    PMENUBAR pmb;
    PMENUITEM pmi;
    int x, y, inter, h;
    RECT rc;
    int count;
    SIZE size;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hwnd))) return -1;

    if (!pWin->hMenu) return -1;

    pmb = (PMENUBAR) (pWin->hMenu);
    if (pmb->category != TYPE_HMENU) return -1;
    if (pmb->type != TYPE_MENUBAR) return -1;

    x = pWin->cl;
    x += LFRDR_MENUBAROFFX;

    y = pWin->ct; 
    h = GetWindowElementAttr (hwnd, WE_METRICS_MENU);
    y -= h;

    inter = LFRDR_INTERMENUITEMX;

    rc.top  = y;
    rc.bottom = y + h;

    count = 0;
    pmi = pmb->head;
    while (pmi) {

        mnuGetMenuBarItemSize (hwnd, pmi, &size);
#if 0
        rc.left = x;
        rc.right = x + size.cx;
#else/*fix bug 3298, tangjb*/
        rc.left = x-(inter>>1);
        rc.right = x + size.cx + (inter>>1);
#endif
        if (PtInRect (&rc, mx, my))
            return count;

        x = x + inter + size.cx;
        count ++;
        
        pmi = pmi->next;
    }

    return -1;
}


void DrawMenuBarHelper (const MAINWIN* pWin, HDC hdc, const RECT* pClipRect)
{
    PMENUBAR pmb;
    PMENUITEM pmi;
    /** startx, starty, width, height of menubar*/
    int x, y, w, h;
    /** offset of font*/
    int menubar_offy;
    /** font size*/
    int menu_size;
    int inter;
    /** rect of menubar */
    RECT rc;
    /** HWND of MAINWIN */
    HWND hwnd;
    SIZE size;

    if (!pWin->hMenu) 
        return;

    hwnd = (HWND)pWin;
    pmb = (PMENUBAR) (pWin->hMenu);
    if (pmb->category != TYPE_HMENU) 
        return;

    x = pWin->cl - pWin->left;
    w = pWin->right - pWin->cl - (pWin->cl - pWin->left);

    menu_size = GetWindowElementAttr (hwnd, WE_METRICS_MENU);
    if (menu_size <= 0)
        return;

    menubar_offy = GetMenuOffy (hwnd, menu_size);

    h = menu_size;
    y = pWin->ct - pWin->top;
    y -= h;
    rc.left = x;
    rc.top = y;
    rc.right = x + w;
    rc.bottom = y + h;
    SelectClipRect (hdc, &rc);

    if (pClipRect)
        ClipRectIntersect (hdc, pClipRect);

    SetBrushColor (hdc, GetWindowElementPixelEx (hwnd, hdc, WE_MAINC_THREED_BODY));
    FillBox (hdc, x, y, w + 3, menu_size);

    x += LFRDR_MENUBAROFFX;
    y += menubar_offy;

    h -= menubar_offy << 1;
    if (h < 0)
        h = 0;

    inter = LFRDR_INTERMENUITEMX;
    pmi = pmb->head;
    while (pmi) {
        mnuGetMenuBarItemSize (hwnd, pmi, &size);
        mnuDrawMenuBarItem (hwnd, hdc, pmi,
               x, y, size.cx, h, rc.top, RECTH (rc));

        x = x + inter + size.cx;
        pmi = pmi->next;
    }
}

void GUIAPI DrawMenuBar (HWND hwnd)
{
    PMAINWIN pWin;
    HDC hdc;

    if (!(pWin = gui_CheckAndGetMainWindowPtr (hwnd))) return;

    if (!pWin->hMenu) return;
    
    hdc = GetDC (hwnd);
    DrawMenuBarHelper (pWin, hdc, NULL);
    ReleaseDC (hdc);
}

static void mnuGetPopupMenuExtent (PTRACKMENUINFO ptmi)
{
    PTRACKMENUINFO prevtmi;
    PMENUITEM pmi = ptmi->pmi;
    PMENUITEM psubmi;
    int w = 0, h = 0;
    int menu_size;
    int menu_font_size;
    int inter;
    int mioffx, minx;
    SIZE size;
    BOOL has_sub = FALSE;
    BOOL has_txt = FALSE;

    menu_font_size = ((PLOGFONT)GetWindowElementAttr 
        (ptmi->hwnd, WE_FONT_MENU))->size;
    menu_size = GetWindowElementAttr (ptmi->hwnd, WE_METRICS_MENU);
    inter = GetMenuOffy (ptmi->hwnd, menu_size);

    mioffx = LFRDR_MENUITEMOFFX;
    minx = LFRDR_MENUITEMMINX;
    
    ptmi->rc.right = ptmi->rc.left;
    ptmi->rc.bottom = ptmi->rc.top + LFRDR_MENUTOPMARGIN;
    
    /* fix bug #4491: use == instead of & for mnutype. */
    if (pmi->mnutype == MFT_OWNERDRAW) {
    }
    else if (pmi->mnutype == MFT_SEPARATOR) {
        pmi->h = LFRDR_MENUSEPARATORY + (inter<<1);
        if (minx > w)
            w = minx;
    }
    else if (pmi->mnutype == MFT_BITMAP) {
        BITMAP* bmp = (BITMAP*)pmi->typedata;
        pmi->h = bmp->bmHeight;
        if (bmp->bmWidth > w)
            w = bmp->bmWidth;
    }
    else {
        SelectFont (HDC_SCREEN_SYS, (PLOGFONT)GetWindowElementAttr 
                (ptmi->hwnd, WE_FONT_MENU));
        GetTabbedTextExtent (HDC_SCREEN_SYS, (char*)pmi->typedata, -1, &size);

        if (pmi->mnutype == MFT_BMPSTRING) {
            pmi->h = MAX (pmi->uncheckedbmp->bmHeight, menu_font_size) + (inter<<1);
            size.cx += pmi->uncheckedbmp->bmWidth;
        }
        else {
            pmi->h = menu_size;
        }

        if (size.cx > w) w = size.cx;

        if (pmi->type == TYPE_PPPMENU)
            pmi->h += inter;

        has_txt = TRUE;
    }
    ptmi->rc.bottom += pmi->h;
    
    if (pmi->type == TYPE_PPPMENU) {
        psubmi = pmi->submenu;
    }
    else
        psubmi = pmi->next;

    if (pmi->submenu)
        has_sub = TRUE;

    while (psubmi) {

        /* fix bug #4491: use == instead of & for mnutype. */
        if (psubmi->mnutype == MFT_OWNERDRAW) {
        }
        else if (psubmi->mnutype == MFT_SEPARATOR) {
            psubmi->h = LFRDR_MENUSEPARATORY + (inter<<1);
            if (minx > w)
                w = minx;
        }
        else if (psubmi->mnutype == MFT_BITMAP) {
            BITMAP* bmp = (BITMAP*)psubmi->typedata;
            psubmi->h = bmp->bmHeight;
            if (bmp->bmWidth > w)
                w = bmp->bmWidth;
        }
        else {
            SelectFont (HDC_SCREEN_SYS, (PLOGFONT)GetWindowElementAttr 
                    (ptmi->hwnd, WE_FONT_MENU));
            GetTabbedTextExtent (HDC_SCREEN_SYS, 
                            (char*)psubmi->typedata, -1, &size);

            if (psubmi->mnutype == MFT_BMPSTRING) {
                psubmi->h = MAX (psubmi->uncheckedbmp->bmHeight, menu_font_size) 
                        + (inter<<1);

                size.cx += psubmi->uncheckedbmp->bmWidth;
            }
            else {
                psubmi->h = menu_size;
            }

            if (size.cx > w) w = size.cx;
        }

        if (psubmi->submenu)
            has_sub = TRUE;

        ptmi->rc.bottom += psubmi->h;

        psubmi = psubmi->next;
    }
    ptmi->rc.bottom += LFRDR_MENUBOTTOMMARGIN;

    if (has_txt) w += mioffx;
    if (has_sub) w += mioffx;
    w += LFRDR_MENULEFTMARGIN;
    w += LFRDR_MENURIGHTMARGIN;

    ptmi->rc.right += w;

    /* adjust the rect according to the alignment flag */
    if (ptmi->flags & TPM_CENTERALIGN) {
        ptmi->rc.left -= w >> 1;
        ptmi->rc.right -= w >> 1;
    }

    if (ptmi->flags & TPM_RIGHTALIGN) {
        ptmi->rc.left -= w;
        ptmi->rc.right -= w;
    }

    h = ptmi->rc.bottom - ptmi->rc.top + 1;
    if (ptmi->flags & TPM_VCENTERALIGN) {
        ptmi->rc.top -= h >> 1;
        ptmi->rc.bottom -= h >> 1;
    }

    if (ptmi->flags & TPM_BOTTOMALIGN) {
        ptmi->rc.top -= h;
        ptmi->rc.bottom -= h;
    }

    /* adjust the rect according to the screen */
    if (ptmi->rc.right > g_rcScr.right) {

        if ((prevtmi = ptmi->prev))
            ptmi->rc.left = prevtmi->rc.left - w;
        else
            ptmi->rc.left = g_rcScr.right - w;

        ptmi->rc.right = ptmi->rc.left + w;

        if (ptmi->rc.left < 0) {
            ptmi->rc.left = 0;
            ptmi->rc.right = w;
        }
    }

    if (ptmi->rc.bottom > g_rcScr.bottom) {
        ptmi->rc.top = g_rcScr.bottom - h;
        ptmi->rc.bottom = ptmi->rc.top + h;

        if (ptmi->rc.top < 0) {
            ptmi->rc.top = 0;
            ptmi->rc.bottom = h;
        }
    }

}

/** type of menu picture */
#define LFRDR_MENU_PIC_RADIO  0x00000001L
#define LFRDR_MENU_PIC_MARK   0x00000002L
#define LFRDR_MENU_PIC_SUB    0x00000004L
#define LFRDR_MENU_PIC_MASK   0x0000000FL

/*
 * Draw menu radio, menu checkmark, menu sub.
 *
 * param hwnd       : handle of window
 * param hdc        : handle of HDC
 * param rect       : the rectangle in which to draw 
 * param pic_type   : type of menu picture
 * param menu_state : state of menu
 *
 * return 0 on success, -1 on failure
 *
 * Author : zhounuohua<zhounuohua@minigui.com>
 * Date   : 2007-11-27
 */
static int mnuDrawMenuPic (HWND hwnd, HDC hdc, RECT* rect, 
        int pic_type, int menu_state)
{
    DWORD rgb_font;
    PMAINWIN pWin;
    int flag = LFRDR_MARK_HAVESHELL;
    int old_bgc;

    pWin = (PMAINWIN) hwnd;
    if (NULL == pWin) return -1;

    if (!pWin->we_rdr) {
        _MG_PRINTF ("GUI>Menu: LFRDR is NULL for window: %p\n", pWin);
        return -1;
    }

    if (menu_state & MFS_CHECKED)
        flag |= LFRDR_MARK_ALL_SELECTED;

    rgb_font = GetWindowElementAttr (hwnd, WE_FGC_MENU);

    switch (pic_type & LFRDR_MENU_PIC_MASK)
    {
        case LFRDR_MENU_PIC_RADIO:
            pWin->we_rdr->draw_radio (hdc, rect, rgb_font, flag); 
            break;

        case LFRDR_MENU_PIC_MARK:
            pWin->we_rdr->draw_checkmark (hdc, rect, rgb_font, flag); 
            break;

        case LFRDR_MENU_PIC_SUB:
            old_bgc = GetBkColor (hdc);
            if (menu_state & MFS_DISABLED)
            {
                if (menu_state & MFS_HILITE)
                {
                    SetBkColor (hdc, GetWindowElementPixelEx 
                        (hwnd, hdc, WE_BGC_DISABLED_ITEM));
                    pWin->we_rdr->draw_arrow (hwnd, hdc, rect, 
                            GetWindowElementAttr (hwnd, WE_FGC_DISABLED_ITEM), 
                        LFRDR_ARROW_RIGHT); 
                }
                else
                {
                    SetBkColor (hdc, GetWindowElementPixelEx 
                        (hwnd, hdc, WE_BGC_MENU));
                    pWin->we_rdr->draw_arrow (hwnd, hdc, rect, 
                            GetWindowElementAttr (hwnd, WE_FGC_DISABLED_ITEM), 
                        LFRDR_ARROW_RIGHT); 
                }
            }
            else if (menu_state & MFS_HILITE)
            {
                SetBkColor (hdc, GetWindowElementPixelEx 
                        (hwnd, hdc, WE_BGC_HIGHLIGHT_ITEM));
                pWin->we_rdr->draw_arrow (hwnd, hdc, rect, 
                        GetWindowElementAttr (hwnd, WE_FGC_HIGHLIGHT_ITEM), 
                        LFRDR_ARROW_RIGHT); 
            }
            else
            {
                SetBkColor (hdc, GetWindowElementPixelEx 
                        (hwnd, hdc, WE_BGC_MENU));
                pWin->we_rdr->draw_arrow (hwnd, hdc, rect, rgb_font, 
                        LFRDR_ARROW_RIGHT); 
            }
            SetBkColor (hdc, old_bgc);
            break;
        default:
            return -1;
    }

    return 0;
}

/*
 * Draw menu bitmap, menu radio, menu checkmark or menu sub
 *
 * return : 0 on success, -1 on failure
 *
 * Author : zhounuohua<zhounuohua@minigui.com>
 * Date   : 2007-11-27
 */
static int DrawMenuPic (HWND hwnd, HDC hdc, int x, int y, 
        PMENUITEM pmi, BOOL draw_sub, int * bmp_w)
{
    RECT mp_rect;           /** rect of menu pic*/
    int mp_height = 0;      /** height of menu pic */
    int mp_type = 0;        /** type of menu pic*/
    int bmp_offy = 0;
    PBITMAP bmp;

    /* not draw menu sub, check */
    if (!draw_sub) {

        /* fix bug #4491: use == instead of & for mnutype. */
        if (pmi->mnutype == MFT_BMPSTRING) {
            /* MFT_BMPSTRING */
            if (pmi->mnustate & MFS_CHECKED)
                bmp = pmi->checkedbmp;
            else
                bmp = pmi->uncheckedbmp;
            
            if (bmp) {
                bmp_offy = (pmi->h - bmp->bmHeight)>>1;
                FillBoxWithBitmap (hdc, x, y + bmp_offy,
                        bmp->bmWidth, pmi->h - (bmp_offy<<1), bmp);
                if (bmp_w) {
                    *bmp_w = MAX (bmp->bmWidth, LFRDR_MENUITEMOFFX);
                }
                return 0;
            }
            else 
                return -1;
        }
    }

    /** get height of pic */
    if (LFRDR_MENUPIC_MIN > pmi->h) {
        mp_height = LFRDR_MENUPIC_MIN;
    }
    else {
        mp_height = (pmi->h) >> 2;
        mp_height += LFRDR_MENUPIC_MIN;
    }

    if (mp_height > LFRDR_MENUITEMOFFX) {
        mp_height = LFRDR_MENUITEMOFFX;
    }

    mp_rect.left = x;
    mp_rect.top  = y;
    mp_rect.top  += (pmi->h - mp_height) >> 1;
    mp_rect.bottom = mp_rect.top + mp_height;
    mp_rect.right = mp_rect.left + mp_height;

    if (bmp_w) {
        if (LFRDR_MENUITEMOFFX < mp_height)
            *bmp_w = mp_height;
    }

    /* get type of menu pic */
    if (draw_sub) {
        /* draw menu sub */
        mp_type = LFRDR_MENU_PIC_SUB;
    }
    else {
        /* draw menu radio or menu check */
        /* fix bug #4491: use == instead of & for mnutype. */
        if (pmi->mnutype == MFT_RADIOCHECK) 
            mp_type = LFRDR_MENU_PIC_RADIO;  
        else if (pmi->mnutype == MFT_MARKCHECK) 
            mp_type = LFRDR_MENU_PIC_MARK;
    }

    mnuDrawMenuPic (hwnd, hdc, &mp_rect, mp_type, pmi->mnustate);
    return 0;
}

#ifdef _MENU_SAVE_BOX
static void do_save_box (TRACKMENUINFO* ptmi)
{
    memset (&ptmi->savedbox, 0, sizeof (BITMAP));
    if (!GetBitmapFromDC (HDC_SCREEN_SYS, ptmi->rc.left, ptmi->rc.top, 
                    RECTW (ptmi->rc), RECTH (ptmi->rc), &ptmi->savedbox))
        return -1;
}

static void do_restore_box (TRACKMENUINFO* ptmi)
{
    FillBoxWithBitmap (HDC_SCREEN_SYS, ptmi->rc.left, ptmi->rc.top,
                        RECTW (ptmi->rc), RECTH (ptmi->rc), &ptmi->savedbox);
}

static void do_free_box (TRACKMENUINFO* ptmi)
{
    free (ptmi->savedbox.bmBits);
}

#else /* _MENU_SAVE_BOX */

#define do_save_box(ptmi) do {} while (0)
#define do_free_box(ptmi) do {} while (0)

static int mnuShowPopupMenu (PTRACKMENUINFO ptmi);
static void mnuHiliteMenuItem (PTRACKMENUINFO ptmi, PMENUITEM pmi, BOOL bHilite);

static void do_restore_box (TRACKMENUINFO* ptmi_hiding)
{
    TRACKMENUINFO* ptmi;
    if (ptmi_hiding == NULL || ptmi_hiding->prev == NULL)
        return;

    ptmi = ptmi_hiding;
    SelectClipRect (HDC_SCREEN_SYS, &ptmi->rc);

    /* redraw parent popup menus here */
    while (ptmi->prev != NULL) {
        ptmi = ptmi->prev;
    }

    do {
        if (DoesIntersect (&ptmi->rc, &ptmi_hiding->rc)){
            mnuShowPopupMenu (ptmi);
            if (ptmi->philite != NULL)
                mnuHiliteMenuItem (ptmi, ptmi->philite, TRUE);
        }
        ptmi = ptmi->next;

    } while (ptmi != NULL);

    SelectClipRect (HDC_SCREEN_SYS, NULL);
}

#endif /* _MENU_SAVE_BOX */

static void draw_down_arrowhead(PTRACKMENUINFO ptmi, HDC hdc)
{
    int l, t, r, b, cx, cy;

    l = ptmi->bottom_scroll_rc.left;
    t = ptmi->bottom_scroll_rc.top; 
    r = ptmi->bottom_scroll_rc.right;
    b = ptmi->bottom_scroll_rc.bottom; 

    cx = (l + r) / 2;
    cy = (t + b) / 2;

    SetPenColor(hdc, GetWindowElementPixelEx (ptmi->hwnd, hdc, WE_FGC_MENU));
    
    MoveTo(hdc, cx-4, cy-2);
    LineTo(hdc, cx+4, cy-2);

    MoveTo(hdc, cx-3, cy-1);
    LineTo(hdc, cx+3, cy-1);

    MoveTo(hdc, cx-2, cy);
    LineTo(hdc, cx+2, cy);

    MoveTo(hdc, cx-1, cy+1);
    LineTo(hdc, cx+1, cy+1);

    MoveTo(hdc, cx, cy+2);
    LineTo(hdc, cx, cy+2);

}

static void draw_bottom_scroll_button(PTRACKMENUINFO ptmi)
{
    PMAINWIN pWin;
    DWORD    bgc;

    pWin = (PMAINWIN) ptmi->hwnd;
    if (NULL == pWin) return;

    if (!pWin->we_rdr) {
        _MG_PRINTF ("GUI>Menu: LFRDR is NULL for window: %p\n", pWin);
        return;
    }

    bgc = GetWindowElementAttr (ptmi->hwnd, WE_BGC_MENU);

    pWin->we_rdr->draw_3dbox (HDC_SCREEN_SYS, &ptmi->bottom_scroll_rc, bgc, 
            LFRDR_3DBOX_FILLED | LFRDR_3DBOX_THICKFRAME);

    draw_down_arrowhead(ptmi, HDC_SCREEN_SYS);
}

static int mnuShowPopupMenu (PTRACKMENUINFO ptmi)
{
    PMENUITEM pmi = ptmi->pmi;
    PMENUITEM psubmi;
    int x, y, offy;
    
    int inter;
    int mioffx;
    int marginlx, marginrx;
    int marginy;
    int font_offy;
    int old_mode;

    PBITMAP bmp;

    PMAINWIN pWin;
    DWORD    bgc;

    DWORD darker_dword;
    DWORD lighter_dword;
    DWORD bgc_dword;
    gal_pixel darker_pixel;
    gal_pixel lighter_pixel;

    pWin = (PMAINWIN) ptmi->hwnd;
    if (NULL == pWin) return -1;

    if (!pWin->we_rdr) {
        _MG_PRINTF ("GUI>Menu: LFRDR is NULL for window: %p\n", pWin);
        return -1;
    }

    bgc = GetWindowElementAttr (ptmi->hwnd, WE_BGC_MENU);

    do_save_box (ptmi);

    inter = LFRDR_INTERMENUITEMY;
    mioffx = LFRDR_MENUITEMOFFX;
    marginlx = LFRDR_MENULEFTMARGIN;
    marginrx = LFRDR_MENURIGHTMARGIN;
    marginy = LFRDR_MENUTOPMARGIN;

    /*add*/
    ptmi->pstart_show_mi = ptmi->pmi;

    ptmi->show_rc.top = ptmi->rc.top; 
    ptmi->show_rc.left = ptmi->rc.left; 
    ptmi->show_rc.right = ptmi->rc.right; 
    ptmi->show_rc.bottom = ptmi->rc.bottom;

    ptmi->selected_rc.top = ptmi->show_rc.top; 
    ptmi->selected_rc.left = ptmi->show_rc.left; 
    ptmi->selected_rc.right = ptmi->show_rc.right; 
    ptmi->selected_rc.bottom = ptmi->show_rc.top + ptmi->pmi->h; 

    ptmi->top_scroll_rc.top = 0;
    ptmi->top_scroll_rc.left= 0;
    ptmi->top_scroll_rc.right= 0;
    ptmi->top_scroll_rc.bottom= 0;

    ptmi->bottom_scroll_rc.top = 0; 
    ptmi->bottom_scroll_rc.left = 0;
    ptmi->bottom_scroll_rc.right = 0;
    ptmi->bottom_scroll_rc.bottom = 0;

    ptmi->draw_bottom_flag = FALSE;
    ptmi->draw_top_flag = FALSE;
    ptmi->mouse_leave_flag = FALSE;/* mouse is in pcurtmi */

    /** get darker and lighter color according to backgroup color */
    bgc_dword = GetWindowElementAttr (ptmi->hwnd, WE_BGC_MENU);
    darker_dword = pWin->we_rdr->calc_3dbox_color 
        (bgc_dword, LFRDR_3DBOX_COLOR_DARKEST);
    lighter_dword = pWin->we_rdr->calc_3dbox_color 
        (bgc_dword, LFRDR_3DBOX_COLOR_LIGHTEST);

    darker_pixel = DWORD2Pixel (HDC_SCREEN_SYS, darker_dword);
    lighter_pixel = DWORD2Pixel (HDC_SCREEN_SYS, lighter_dword);

    if ((ptmi->rc.bottom > g_rcScr.bottom) 
            && ((ptmi->rc.bottom - ptmi->rc.top) > g_rcScr.bottom)){

        ptmi->show_rc.top = ptmi->rc.top; 
        ptmi->show_rc.left = ptmi->rc.left; 
        ptmi->show_rc.right = ptmi->rc.right; 
        ptmi->show_rc.bottom = g_rcScr.bottom - 
            GetSysFontHeight(SYSLOGFONT_MENU) + SHORTEN; 

        ptmi->bottom_scroll_rc.top = ptmi->show_rc.bottom;
        ptmi->bottom_scroll_rc.left = ptmi->rc.left;
        ptmi->bottom_scroll_rc.right = ptmi->rc.right;
        ptmi->bottom_scroll_rc.bottom = g_rcScr.bottom;
        
        ptmi->draw_bottom_flag = TRUE;
    }
    /*add end*/

    /* draw background. */
    x = ptmi->rc.left;
    y = ptmi->rc.top;

    pWin->we_rdr->draw_3dbox (HDC_SCREEN_SYS, &ptmi->show_rc, bgc, 
            LFRDR_3DBOX_FILLED | LFRDR_3DBOX_THICKFRAME);
    
    x = ptmi->show_rc.left + marginlx;
    y = ptmi->show_rc.top  + marginy;

    /* fix bug #4491: use == instead of & for mnutype. */
    if (pmi->mnutype == MFT_BITMAP) {
    
        if (pmi->mnustate & MFS_CHECKED)
            bmp = pmi->checkedbmp;
        else
            bmp = (BITMAP*)pmi->typedata;
        
        offy = (pmi->h - bmp->bmHeight) >> 1;
            
        FillBoxWithBitmap (HDC_SCREEN_SYS, x, y + offy, 
                        ((BITMAP*)pmi->typedata)->bmWidth, 
                        pmi->h - (offy << 1), bmp);

    }
    else if (pmi->mnutype == MFT_SEPARATOR) {
    
        /** two horizontal separator */
        SetPenColor (HDC_SCREEN_SYS, darker_pixel); 
        MoveTo (HDC_SCREEN_SYS, 
            x, y + (pmi->h>>1) - inter + 1);
        LineTo (HDC_SCREEN_SYS, 
            ptmi->show_rc.right - marginrx, y + (pmi->h>>1) - inter + 1);

        SetPenColor (HDC_SCREEN_SYS, lighter_pixel); 
        MoveTo (HDC_SCREEN_SYS, 
            x, y + (pmi->h>>1) - inter + 2);
        LineTo (HDC_SCREEN_SYS, 
            ptmi->show_rc.right - marginrx, y + (pmi->h>>1) - inter + 2);
    }
    else if (pmi->mnutype == MFT_OWNERDRAW) {
    }
    else {
        font_offy = (pmi->h - ((PLOGFONT)GetWindowElementAttr 
                    (ptmi->hwnd, WE_FONT_MENU))->size) >> 1;

        SelectFont (HDC_SCREEN_SYS, (PLOGFONT)GetWindowElementAttr 
                (ptmi->hwnd, WE_FONT_MENU));
        
        if (pmi->type == TYPE_PPPMENU) {
            old_mode = SetBkMode (HDC_SCREEN_SYS, BM_TRANSPARENT);
            SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                    (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_MENU));
            TextOut (HDC_SCREEN_SYS, x + mioffx, y + font_offy,
                (char*)pmi->typedata);
            SetBkMode (HDC_SCREEN_SYS, old_mode);

            SetPenColor (HDC_SCREEN_SYS, GetWindowElementPixelEx 
                    (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_MENU));

            MoveTo (HDC_SCREEN_SYS, x, y + pmi->h - (inter<<1));
            LineTo (HDC_SCREEN_SYS, ptmi->show_rc.right - marginrx, 
                    y + pmi->h - (inter<<1));
            MoveTo (HDC_SCREEN_SYS, x, y + pmi->h - inter);
            LineTo (HDC_SCREEN_SYS, ptmi->show_rc.right - marginrx, 
                    y + pmi->h - inter);
        }
        else {
            int bmp_w = mioffx;

            /** draw radio, checkmark or bitmap */
            DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, x + 1, y, pmi, FALSE, &bmp_w);

            /** menu text */
            old_mode = SetBkMode (HDC_SCREEN_SYS, BM_TRANSPARENT);
            if (pmi->mnustate & MFS_DISABLED)
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_DISABLED_ITEM));
            else if (pmi->mnustate & MFS_HILITE)
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx 
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_HIGHLIGHT_ITEM));
            else
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_MENU));

            TextOut (HDC_SCREEN_SYS, x + bmp_w, y + font_offy, 
                (char*)pmi->typedata);
            SetBkMode (HDC_SCREEN_SYS, old_mode);

            /** draw menu sub */
            if (pmi->submenu) {
                DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, 
                        ptmi->show_rc.right - mioffx - marginrx, 
                        y, pmi, TRUE, 0);
            }
        }
    }
    y += pmi->h;
    
    if (pmi->type == TYPE_PPPMENU)
        psubmi = pmi->submenu;
    else
        psubmi = pmi->next;

    while (psubmi) {
        /*add*/
        if (y >= ptmi->show_rc.bottom){
            break;
        }
        /*add end*/

        /* fix bug #4491: use == instead of & for mnutype. */
        if (psubmi->mnutype == MFT_BITMAP) {
    
            if (psubmi->mnustate & MFS_CHECKED)
                bmp = psubmi->checkedbmp;
            else
                bmp = (BITMAP*)psubmi->typedata;

            offy = (psubmi->h - bmp->bmHeight) >> 1;
            
            FillBoxWithBitmap (HDC_SCREEN_SYS, x, y + offy,
                        ((BITMAP*)psubmi->typedata)->bmWidth, 
                        psubmi->h - (offy << 1), bmp);

            /** draw menu sub */
            if (psubmi->submenu) {
                DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, 
                        ptmi->show_rc.right - mioffx - marginrx, 
                        y, psubmi, TRUE, 0);
            }

        }
        else if (psubmi->mnutype == MFT_SEPARATOR) {
    
            /** two horizontal separator */
            SetPenColor (HDC_SCREEN_SYS, darker_pixel);
            MoveTo (HDC_SCREEN_SYS, x, y + (psubmi->h>>1) - inter + 1);
            LineTo (HDC_SCREEN_SYS, ptmi->show_rc.right - marginrx, 
                    y + (psubmi->h>>1) - inter + 1);
                
            SetPenColor (HDC_SCREEN_SYS, lighter_pixel);
            MoveTo (HDC_SCREEN_SYS, x, y + (psubmi->h>>1) - inter + 2);
            LineTo (HDC_SCREEN_SYS, ptmi->show_rc.right - marginrx, 
                    y + (psubmi->h>>1) - inter + 2);
        }
        else if (pmi->mnutype == MFT_OWNERDRAW) {
        }
        else {
            int old_mode, bmp_w = mioffx;

            /** draw radio, checkmark or bitmap */
            DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, x + 1, y, psubmi, FALSE, &bmp_w);

            /** menu text */
            SelectFont (HDC_SCREEN_SYS, (PLOGFONT)GetWindowElementAttr 
                    (ptmi->hwnd, WE_FONT_MENU));
            old_mode = SetBkMode (HDC_SCREEN_SYS, BM_TRANSPARENT);
            if (psubmi->mnustate & MFS_DISABLED)
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_DISABLED_ITEM));
            else if (psubmi->mnustate & MFS_HILITE)
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx 
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_HIGHLIGHT_ITEM));
            else
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_MENU));

            offy = (psubmi->h - ((PLOGFONT)GetWindowElementAttr 
                    (ptmi->hwnd, WE_FONT_MENU))->size) >> 1;

            TextOut (HDC_SCREEN_SYS, x + bmp_w, y + offy, 
                (char*)psubmi->typedata);
            SetBkMode (HDC_SCREEN_SYS, old_mode);

            /** draw menu sub */
            if (psubmi->submenu) {
                DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, 
                        ptmi->show_rc.right - mioffx - marginrx, 
                        y, psubmi, TRUE, 0);
            }
        }

        y += psubmi->h;

        psubmi = psubmi->next;

    }
    /*add*/
    if (ptmi->draw_bottom_flag){
        draw_bottom_scroll_button(ptmi);
    }
    /*add end*/

    return 0;
}


static int get_start_to_end_height(PTRACKMENUINFO ptmi)
{
    PMENUITEM pmi = ptmi->pstart_show_mi;        
    int h = 0;

    while (pmi != NULL){
         h = h + pmi->h;   
         pmi = pmi->next;
    }

    return h;
}

static void draw_up_arrowhead(PTRACKMENUINFO ptmi, HDC hdc)
{
    int l, t, r, b, cx, cy;

    l = ptmi->top_scroll_rc.left;
    t = ptmi->top_scroll_rc.top; 
    r = ptmi->top_scroll_rc.right;
    b = ptmi->top_scroll_rc.bottom; 

    cx = (l + r) / 2;
    cy = (t + b) / 2;

    SetPenColor(hdc, GetWindowElementPixelEx (ptmi->hwnd, hdc, WE_FGC_MENU));

    MoveTo(hdc, cx-4, cy+2);
    LineTo(hdc, cx+4, cy+2);

    MoveTo(hdc, cx-3, cy+1);
    LineTo(hdc, cx+3, cy+1);

    MoveTo(hdc, cx-2, cy);
    LineTo(hdc, cx+2, cy);

    MoveTo(hdc, cx-1, cy-1);
    LineTo(hdc, cx+1, cy-1);

    MoveTo(hdc, cx, cy-2);
    LineTo(hdc, cx, cy-2);

}

static void draw_top_scroll_button(PTRACKMENUINFO ptmi)
{
    PMAINWIN pWin;
    DWORD    bgc;

    pWin = (PMAINWIN) ptmi->hwnd;
    if (NULL == pWin) return;

    if (!pWin->we_rdr) {
        _MG_PRINTF ("GUI>Menu: LFRDR is NULL for window: %p\n", pWin);
        return;
    }

    bgc = GetWindowElementAttr (ptmi->hwnd, WE_BGC_MENU);

    pWin->we_rdr->draw_3dbox (HDC_SCREEN_SYS, &ptmi->top_scroll_rc, bgc, 
            LFRDR_3DBOX_FILLED | LFRDR_3DBOX_THICKFRAME);

    draw_up_arrowhead(ptmi, HDC_SCREEN_SYS);
}


static int show_scroll_popup_menu (PTRACKMENUINFO ptmi)
{
    PMENUITEM pmi = ptmi->pstart_show_mi;
    PMENUITEM psubmi;
    int x, y, offy;
    
    int inter;
    int mioffx;
    int marginlx, marginrx;
    int marginy;
    int font_offy;

    int h = 0;
    int old_mode;

    PBITMAP bmp;
    
    PMAINWIN pWin;
    DWORD    bgc;
    
    DWORD darker_dword;
    DWORD lighter_dword;
    DWORD bgc_dword;
    gal_pixel darker_pixel;
    gal_pixel lighter_pixel;

    pWin = (PMAINWIN) ptmi->hwnd;
    if (NULL == pWin) return -1;

    if (!pWin->we_rdr) {
        _MG_PRINTF ("GUI>Menu: LFRDR is NULL for window: %p\n", pWin);
        return -1;
    }

    bgc = GetWindowElementAttr (ptmi->hwnd, WE_BGC_MENU);
    do_save_box (ptmi);

    inter = LFRDR_INTERMENUITEMY;
    mioffx = LFRDR_MENUITEMOFFX;
    marginlx = LFRDR_MENULEFTMARGIN;
    marginrx = LFRDR_MENURIGHTMARGIN;
    marginy = LFRDR_MENUTOPMARGIN;

    /** get darker and lighter color according to backgroup color */
    bgc_dword = GetWindowElementAttr (ptmi->hwnd, WE_BGC_MENU);
    darker_dword = pWin->we_rdr->calc_3dbox_color 
        (bgc_dword, LFRDR_3DBOX_COLOR_DARKEST);
    lighter_dword = pWin->we_rdr->calc_3dbox_color 
        (bgc_dword, LFRDR_3DBOX_COLOR_LIGHTEST);

    darker_pixel = DWORD2Pixel (HDC_SCREEN_SYS, darker_dword);
    lighter_pixel = DWORD2Pixel (HDC_SCREEN_SYS, lighter_dword);

    h = get_start_to_end_height(ptmi);
    if ( (h + ptmi->top_scroll_rc.bottom) <= ptmi->bottom_scroll_rc.bottom){
        ptmi->show_rc.bottom = ptmi->bottom_scroll_rc.bottom;        

        ptmi->bottom_scroll_rc.left = 0;
        ptmi->bottom_scroll_rc.top = 0;
        ptmi->bottom_scroll_rc.right = 0;
        ptmi->bottom_scroll_rc.bottom = 0;

        ptmi->draw_bottom_flag = FALSE;
    }

    if (ptmi->pstart_show_mi == ptmi->pmi ){
        ptmi->show_rc.top = ptmi->rc.top;        

        ptmi->top_scroll_rc.left = 0;
        ptmi->top_scroll_rc.top = 0;
        ptmi->top_scroll_rc.right = 0;
        ptmi->top_scroll_rc.bottom = 0;

        ptmi->draw_top_flag = FALSE;

    }

    // draw background.
    x = ptmi->show_rc.left;
    y = ptmi->show_rc.top;

    pWin->we_rdr->draw_3dbox (HDC_SCREEN_SYS, &ptmi->show_rc, bgc, 
            LFRDR_3DBOX_FILLED | LFRDR_3DBOX_THICKFRAME);

    x = ptmi->show_rc.left + marginlx;
    y = ptmi->show_rc.top  + marginy;
    if (pmi->mnutype == MFT_BITMAP) {
    
        if (pmi->mnustate & MFS_CHECKED)
            bmp = pmi->checkedbmp;
        else
            bmp = (BITMAP*)pmi->typedata;

        offy = (pmi->h - bmp->bmHeight) >> 1;
            
        FillBoxWithBitmap (HDC_SCREEN_SYS, x, y + offy,
               ((BITMAP*)pmi->typedata)->bmWidth, pmi->h - (offy << 1), bmp);
    }
    else if (pmi->mnutype == MFT_SEPARATOR) {
    
        /** two horizontal separator */
        SetPenColor (HDC_SCREEN_SYS, darker_pixel); 
        MoveTo (HDC_SCREEN_SYS, 
            x, y + (pmi->h>>1) - inter + 1);
        LineTo (HDC_SCREEN_SYS, 
            ptmi->show_rc.right - marginrx, y + (pmi->h>>1) - inter + 1);

        SetPenColor (HDC_SCREEN_SYS, lighter_pixel); 
        MoveTo (HDC_SCREEN_SYS, 
            x, y + (pmi->h>>1) - inter + 2);
        LineTo (HDC_SCREEN_SYS, 
            ptmi->show_rc.right - marginrx, y + (pmi->h>>1) - inter + 2);
    }
    else if (pmi->mnutype == MFT_OWNERDRAW) {
    }
    else {
        font_offy = (pmi->h - ((PLOGFONT)GetWindowElementAttr 
                    (ptmi->hwnd, WE_FONT_MENU))->size) >> 1;
        
        SelectFont (HDC_SCREEN_SYS, (PLOGFONT)GetWindowElementAttr 
                (ptmi->hwnd, WE_FONT_MENU));
        
        if (pmi->type == TYPE_PPPMENU) {
            old_mode = SetBkMode (HDC_SCREEN_SYS, BM_TRANSPARENT);
            SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                    (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_MENU));
            TextOut (HDC_SCREEN_SYS, x + mioffx, y + font_offy, 
                (char*)pmi->typedata);
            SetBkMode (HDC_SCREEN_SYS, old_mode);

            SetPenColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                    (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_MENU));

            /** two horizontal separator */
            MoveTo (HDC_SCREEN_SYS, 
                x, 
                y + pmi->h - (inter<<1));
            LineTo (HDC_SCREEN_SYS, 
                ptmi->show_rc.right - marginrx, 
                y + pmi->h - (inter<<1));
            MoveTo (HDC_SCREEN_SYS, 
                x, 
                y + pmi->h - inter);
            LineTo (HDC_SCREEN_SYS,
                ptmi->show_rc.right - marginrx, 
                y + pmi->h - inter);
        }
        else {
            int bmp_w = mioffx;

            /** draw radio, checkmark or bitmap */
            DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, x + 1, y, pmi, FALSE, &bmp_w);

            /** menu text */
            old_mode = SetBkMode (HDC_SCREEN_SYS, BM_TRANSPARENT);
            if (pmi->mnustate & MFS_DISABLED)
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_DISABLED_ITEM));
            else if (pmi->mnustate & MFS_HILITE)
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx 
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_HIGHLIGHT_ITEM));
            else
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_MENU));

            TextOut (HDC_SCREEN_SYS, x + bmp_w, y + font_offy, 
                (char*)pmi->typedata);
            SetBkMode (HDC_SCREEN_SYS, old_mode);

            /** draw menu sub */
            if (pmi->submenu) {
                DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, 
                        ptmi->show_rc.right - mioffx - marginrx, 
                        y, pmi, TRUE, 0);
            }
        }
    }
    y += pmi->h;

    if (pmi->type == TYPE_PPPMENU)
        psubmi = pmi->submenu;
    else
        psubmi = pmi->next;

    while (psubmi) {
        /*add*/
        if (y > ptmi->show_rc.bottom) {
            break;
        }
        /*add end*/

        if (psubmi->mnutype == MFT_BITMAP) {
    
            if (psubmi->mnustate & MFS_CHECKED)
                bmp = psubmi->checkedbmp;
            else
                bmp = (BITMAP*)psubmi->typedata;

            offy = (psubmi->h - bmp->bmHeight) >> 1;
            
            FillBoxWithBitmap (HDC_SCREEN_SYS, x, y + offy, 
                            ((BITMAP*)psubmi->typedata)->bmWidth, 
                            psubmi->h - (offy << 1), bmp);

            /** draw menu sub */
            if (psubmi->submenu) {
                DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, 
                        ptmi->show_rc.right - mioffx - marginrx, 
                        y, psubmi, TRUE, 0);
            }
        }
        else if (psubmi->mnutype == MFT_SEPARATOR) {
    
            /** tow horizontal separator */
            SetPenColor (HDC_SCREEN_SYS, darker_pixel); 
            MoveTo (HDC_SCREEN_SYS, 
                x, y + (psubmi->h>>1) - inter + 1);
            LineTo (HDC_SCREEN_SYS, 
                ptmi->show_rc.right - marginrx, y + (psubmi->h>>1) - inter + 1);
                
            SetPenColor (HDC_SCREEN_SYS, lighter_pixel); 
            MoveTo (HDC_SCREEN_SYS, 
                x, y + (psubmi->h>>1) - inter + 2);
            LineTo (HDC_SCREEN_SYS, 
                ptmi->show_rc.right - marginrx, y + (psubmi->h>>1) - inter + 2);
        }
        else if (pmi->mnutype == MFT_OWNERDRAW) {
        }
        else {
            int old_mode, bmp_w = mioffx;

            /** draw radio, checkmark or bitmap */
            DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, x + 1, y, psubmi, FALSE, &bmp_w);

            /** menu text*/
            SelectFont (HDC_SCREEN_SYS, (PLOGFONT)GetWindowElementAttr 
                    (ptmi->hwnd, WE_FONT_MENU));
            old_mode = SetBkMode (HDC_SCREEN_SYS, BM_TRANSPARENT);
            if (psubmi->mnustate & MFS_DISABLED)
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_DISABLED_ITEM));
            else if (psubmi->mnustate & MFS_HILITE)
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx 
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_HIGHLIGHT_ITEM));
            else
                SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx
                        (ptmi->hwnd, HDC_SCREEN_SYS, WE_FGC_MENU));

            offy = (psubmi->h - ((PLOGFONT)GetWindowElementAttr 
                    (ptmi->hwnd, WE_FONT_MENU))->size) >> 1;

            TextOut (HDC_SCREEN_SYS, x + bmp_w, y + offy, 
                (char*)psubmi->typedata);
            SetBkMode (HDC_SCREEN_SYS, old_mode);

            /** draw menu sub */
            if (psubmi->submenu) {
                DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, 
                        ptmi->show_rc.right - mioffx - marginrx, 
                        y, psubmi, TRUE, 0);
            }
        }

        y += psubmi->h;

        psubmi = psubmi->next;
    }

    /*add*/
    if (ptmi->draw_bottom_flag){
        draw_bottom_scroll_button(ptmi);
    }

    if (ptmi->draw_top_flag){
        draw_top_scroll_button(ptmi);
    }
    /*add end*/

    return 0;
}

static void mnuUnhiliteMenu (PMENUITEM pmi)
{
    if (pmi->type == TYPE_PPPMENU)
        pmi = pmi->submenu;
    else {
        if (pmi->mnustate & MFS_HILITE)
            pmi->mnustate &= ~MFS_HILITE;

        pmi = pmi->next;
    }

    while (pmi) {
        if (pmi->mnustate & MFS_HILITE)
            pmi->mnustate &= ~MFS_HILITE;

        pmi = pmi->next;
    }
}

static void mnuCloseMenu (PTRACKMENUINFO ptmi)
{
    PTRACKMENUINFO phead, plast, ptemp;
    HMENU hmnu;
    UINT flags;

    // menu bar info
    HWND hwnd;
    PMENUBAR pmb;
    int barPos;
    
    flags = ptmi->flags;
    hmnu = (HMENU)ptmi->pmi;

    // get first tracking menu.
    phead = ptmi;
    while (phead->prev) {
        phead = phead->prev;
    }
    pmb = phead->pmb;
    hwnd = phead->hwnd;
    barPos = phead->barPos;
    
    // get last tracking menu.
    plast = ptmi;
    while (plast->next) {
        plast = plast->next;
    }

    while (plast) {
        ptemp = plast->prev;

        mnuUnhiliteMenu (plast->pmi);

        SendMessage (HWND_DESKTOP, 
                        MSG_ENDTRACKMENU, 0, (LPARAM)plast);

        plast = ptemp;
    }

    if (pmb && barPos >= 0)
        HiliteMenuBarItem (hwnd, barPos, LFRDR_MENU_STATE_NORMAL);
    
    if (flags & TPM_DESTROY)
        DestroyMenu (hmnu);
}

static PTRACKMENUINFO mnuHitTestMenu (PTRACKMENUINFO plast, int x, int y)
{
    PTRACKMENUINFO ptemp;
    
    // which popup menu mouse in.
    ptemp = plast;
    while (ptemp) {

        if ( PtInRect(&ptemp->rc, x, y))
            return ptemp;

        ptemp = ptemp->prev;
    }

    return NULL;
}

#if 0
static PMENUITEM mnuHitTestMenuItem (PTRACKMENUINFO ptmi, int x, int y)
{
    PMENUITEM pmi = ptmi->pmi;
    PMENUITEM psubmi;
    RECT rc;
    
    rc.left = ptmi->rc.left;
    rc.right = ptmi->rc.right;
    rc.top = ptmi->rc.top + LFRDR_MENUTOPMARGIN;
    rc.bottom = rc.top + pmi->h;

    if (PtInRect (&rc, x, y))
        return pmi;

    rc.top += pmi->h;
    if (pmi->type == TYPE_PPPMENU)
        psubmi = pmi->submenu;
    else
        psubmi = pmi->next;
   
    while (psubmi) {
        
        rc.bottom = rc.top + psubmi->h;

        if (PtInRect (&rc, x, y))
            return psubmi;

        rc.top = rc.bottom;
        psubmi = psubmi->next;
    }

    return NULL;
}
#endif 

static PMENUITEM get_selected_mi(PTRACKMENUINFO ptmi, int x, int y)
{
    PMENUITEM pmi = ptmi->pstart_show_mi;
    PMENUITEM psubmi;
    RECT rc;
    
    rc.left = ptmi->show_rc.left;
    rc.right = ptmi->show_rc.right;
    rc.top = ptmi->show_rc.top + LFRDR_MENUTOPMARGIN;
    rc.bottom = rc.top + pmi->h;

    if (PtInRect (&rc, x, y))
        return pmi;

    rc.top += pmi->h;
    if (pmi->type == TYPE_PPPMENU)
        psubmi = pmi->submenu;
    else
        psubmi = pmi->next;
   
    while (psubmi) {
        
        rc.bottom = rc.top + psubmi->h;

        if (PtInRect (&rc, x, y)){
           /*add*/
            ptmi->selected_rc.left = rc.left; 
            ptmi->selected_rc.top =  rc.top;
            ptmi->selected_rc.right = rc.right; 
            ptmi->selected_rc.bottom = rc.bottom; 
            /*add end*/
            return psubmi;
            }

        rc.top = rc.bottom;
        psubmi = psubmi->next;
    }

    return NULL;
}

static void mnuHiliteMenuItem (PTRACKMENUINFO ptmi, PMENUITEM pmi, BOOL bHilite)
{
    RECT rc;
    PMENUITEM ptemp = ptmi->pstart_show_mi;
    PMAINWIN pWin;
    HWND hwnd = ptmi->hwnd;
    PBITMAP bmp;

    /** the menu item rect to be drawn*/
    RECT rect;
    // NUV int inter;
    int mioffx;
    int marginlx, marginrx;
    int offy;

    pWin = (PMAINWIN) hwnd;
    if (NULL == pWin) return;

    if (!pWin->we_rdr) {
        _MG_PRINTF ("GUI>Menu: LFRDR is NULL for window: %p\n", pWin);
        return;
    }

    if (pmi->type == TYPE_PPPMENU) return;

    // NUV inter = LFRDR_INTERMENUITEMY;
    mioffx = LFRDR_MENUITEMOFFX;
    marginlx = LFRDR_MENULEFTMARGIN;
    marginrx = LFRDR_MENURIGHTMARGIN;

    rc.left = ptmi->show_rc.left;
    rc.right = ptmi->show_rc.right;
    rc.top = ptmi->show_rc.top + LFRDR_MENUTOPMARGIN;

    if (ptemp == pmi)
        ptemp = pmi;
    else {
        rc.top += ptemp->h;
        if (ptemp->type == TYPE_PPPMENU)
            ptemp = ptemp->submenu;
        else
            ptemp = ptemp->next;

        while (ptemp != pmi) {
            if (ptemp == NULL) return;   
            rc.top += ptemp->h;
            ptemp = ptemp->next;
        }
    }

    rc.bottom = rc.top + ptemp->h;

    if (ptemp->mnutype == MFT_SEPARATOR)
        return;

    if (bHilite)
        ptemp->mnustate |= MFS_HILITE;
    else
        ptemp->mnustate &= ~MFS_HILITE;

    /** rect to be drawn */
    rect.left = rc.left + marginlx;
    rect.top = rc.top;
    rect.right = rect.left + RECTW (rc) - marginlx - marginrx;
    rect.bottom = rect.top + RECTH (rc);

    /** draw the rect, high priority for disabled item, */
    if (ptemp->mnustate & MFS_DISABLED)
    {
        /** render a disabled item */
        SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx 
                (hwnd, HDC_SCREEN_SYS, WE_FGC_DISABLED_ITEM));
        if (ptemp->mnustate & MFS_HILITE)
            pWin->we_rdr->draw_disabled_menu_item (hwnd, HDC_SCREEN_SYS, &rect,
                    GetWindowElementAttr (hwnd, WE_BGC_DISABLED_ITEM));
        else
            pWin->we_rdr->draw_disabled_menu_item (hwnd, HDC_SCREEN_SYS, &rect,
                    GetWindowElementAttr (hwnd, WE_BGC_MENU));
    }
    else if (ptemp->mnustate & MFS_HILITE)
    {
        /** render a hilite item */
        SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx 
                (hwnd, HDC_SCREEN_SYS, WE_FGC_HIGHLIGHT_ITEM));
        pWin->we_rdr->draw_hilite_menu_item (hwnd, HDC_SCREEN_SYS, &rect,
                GetWindowElementAttr (hwnd, WE_BGC_HIGHLIGHT_ITEM));
    }
    else
    {
        /** render a normal item as unhilite */
        SetTextColor (HDC_SCREEN_SYS, GetWindowElementPixelEx 
                (hwnd, HDC_SCREEN_SYS, WE_FGC_MENU));

        pWin->we_rdr->draw_normal_menu_item (hwnd, HDC_SCREEN_SYS, &rect,
                GetWindowElementAttr (hwnd, WE_BGC_MENU));
    }

    if (ptemp->mnutype == MFT_BITMAP) {

        /** draw check or unchecked bitmap */
        if (ptemp->mnustate & MFS_CHECKED)
            bmp = ptemp->checkedbmp;
        else if (bHilite)
            bmp = ptemp->uncheckedbmp;
        else
            bmp = (BITMAP*)ptemp->typedata;

        offy = (ptemp->h - bmp->bmHeight) >> 1;
            
        FillBoxWithBitmap (HDC_SCREEN_SYS, rc.left + marginlx, rc.top + offy,
                        ((BITMAP*)ptemp->typedata)->bmWidth, 
                        ptemp->h - (offy << 1), bmp);

        /** draw menu sub */
        if (ptemp->submenu) {
            DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, 
                    rc.right - mioffx - marginrx,
                    rc.top, ptemp, TRUE, 0);
        }
    }
    else if (ptemp->mnutype == MFT_OWNERDRAW) {
    }
    else {
        int old_mode, bmp_w = mioffx;

        /** draw radio, checkmark or bitmap */
        DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, 
                rc.left + marginlx + 1, rc.top, ptemp, FALSE, &bmp_w);

        /** menu text */
        SelectFont (HDC_SCREEN_SYS, (PLOGFONT)GetWindowElementAttr 
                (ptmi->hwnd, WE_FONT_MENU));
        old_mode = SetBkMode (HDC_SCREEN_SYS, BM_TRANSPARENT);
        offy = (ptemp->h - ((PLOGFONT)GetWindowElementAttr 
                    (ptmi->hwnd, WE_FONT_MENU))->size) >> 1;
        
        TextOut (HDC_SCREEN_SYS, rc.left + bmp_w + marginlx, rc.top + offy,
            (char*)ptemp->typedata);
        SetBkMode (HDC_SCREEN_SYS, old_mode);
        
        /** draw menu sub */
        if (ptemp->submenu) {
            DrawMenuPic (ptmi->hwnd, HDC_SCREEN_SYS, 
                    rc.right - mioffx - marginrx, rc.top, ptemp, TRUE, 0);
        }
    }
}

static int mnuOpenNewSubMenu (PTRACKMENUINFO ptmi, PMENUITEM pmi, 
                int x, int y)
{
    PTRACKMENUINFO pnewtmi;

    pnewtmi = TrackMenuInfoAlloc ();
    
    pnewtmi->rc.left = x;
    pnewtmi->rc.top  = y;
    pnewtmi->pmi = pmi;
    pnewtmi->pmb = ptmi->pmb;
    pnewtmi->philite = NULL;
    pnewtmi->hwnd = ptmi->hwnd;
    pnewtmi->flags = ptmi->flags;
    pnewtmi->next = NULL;
    pnewtmi->prev = NULL;
   
    if (SendMessage (HWND_DESKTOP, MSG_TRACKPOPUPMENU,
                    0, (LPARAM)(pnewtmi)) < 0) {
        FreeTrackMenuInfo (pnewtmi);
        return -1;
    }

    return 0;
}

static PMENUITEM mnuGetNextMenuItem (PTRACKMENUINFO ptmi, PMENUITEM pmi)
{
    PMENUITEM head = ptmi->pmi;
    PMENUITEM next;

    if (head->type == TYPE_PPPMENU)
        head = head->submenu;

    if (pmi == NULL)
        next = head;
    else if (pmi->next)
        next = pmi->next;
    else
        next = head;

    while (next && (next->mnutype == MFT_SEPARATOR))
        next = next->next;

    if (next == NULL)
        next = head;

    while (next && (next->mnutype == MFT_SEPARATOR))
        next = next->next;

    return next;
}

static PMENUITEM mnuGetPrevItem (PMENUITEM head, PMENUITEM pmi)
{
    if (head == pmi)
        return NULL;

    while (head) {
        if (head->next == pmi)
            return head;

        head = head->next;
    }

    return NULL;
}

static PMENUITEM mnuGetPrevMenuItem (PTRACKMENUINFO ptmi, PMENUITEM pmi)
{
    PMENUITEM head = ptmi->pmi, tail;
    PMENUITEM prev;

    if (head->type == TYPE_PPPMENU)
        head = head->submenu;

    prev = pmi;

    do {
        prev = mnuGetPrevItem (head, prev);

        if (prev) {
            if (prev->mnutype == MFT_SEPARATOR)
                continue;
            else
                return prev;
        }
        else
            break;
    }while (TRUE);

    tail = head;
    while (tail && tail->next)
        tail = tail->next;

    prev = tail;
    do {
        if (prev) {
            if (!(prev->mnutype == MFT_SEPARATOR))
                return prev;
        }
        else
            break;
            
        prev = mnuGetPrevItem (head, prev);
    }while (TRUE);

    return NULL;
}

static int mnuGetMenuItemY (PTRACKMENUINFO ptmi, PMENUITEM pmi)
{
    //PMENUITEM head = ptmi->pmi;
    //int y = ptmi->rc.top + LFRDR_MENUTOPMARGIN;

    PMENUITEM head = ptmi->pstart_show_mi;
    int y = ptmi->show_rc.top + LFRDR_MENUTOPMARGIN;

    if (pmi == head) return y;
    
    y += head->h;
    
    if (head->type == TYPE_PPPMENU)
        head = head->submenu;
    else
        head = head->next;

    while (head) {
        if (head == pmi)
            return y;

        y += head->h;

        head = head->next;
    }

    return 0;
}

static int get_menu_item_y_from_pstart (PTRACKMENUINFO ptmi, PMENUITEM pmi)
{
    PMENUITEM head = ptmi->pstart_show_mi;
    int y = head->h;

    if (pmi == head) return y;
    
    y += head->h;
    
    if (head->type == TYPE_PPPMENU)
        head = head->submenu;
    else
        head = head->next;

    while (head) {
        if (head == pmi)
            return y;

        y += head->h;

        head = head->next;
    }

    return 0;
}

static void mnu_scroll_menu (PTRACKMENUINFO ptmi, int x, int y);

static void cursor_block_down(PTRACKMENUINFO pcurtmi, PMENUITEM pcurmi)
{
    PMENUITEM pnewmi;
    int y;
    

    pnewmi = mnuGetNextMenuItem (pcurtmi, pcurmi);

    y = get_menu_item_y_from_pstart (pcurtmi, pnewmi);

    if (pcurtmi->rc.bottom > g_rcScr.bottom){

        if (pcurmi != NULL){ 
            if (pcurmi->next == NULL){
                    mnuShowPopupMenu (pcurtmi); 
                    mnuHiliteMenuItem (pcurtmi, pnewmi, TRUE);
                    pcurtmi->philite = pnewmi;
                    return;

            }
            if (pnewmi == pcurtmi->pmi){
                    mnuShowPopupMenu (pcurtmi); 
                    mnuHiliteMenuItem (pcurtmi, pnewmi, TRUE);
                    pcurtmi->philite = pnewmi;
                    return;
            }
        }

        if ((y + pcurtmi->top_scroll_rc.bottom) >= pcurtmi->show_rc.bottom){
            mnu_scroll_menu(pcurtmi, (pcurtmi->rc.left + pcurtmi->rc.right) /2, 
                    g_rcScr.bottom - 1);

        }
        else{
            if (pnewmi != pcurmi) {

                if (pcurmi){ 
                    mnuHiliteMenuItem (pcurtmi, pcurmi, FALSE);
                }

                if (pnewmi) {
                    mnuHiliteMenuItem (pcurtmi, pnewmi, TRUE);
                    pcurtmi->philite = pnewmi;
                }
            }
        }

    }
    else{
        if (pnewmi != pcurmi) {
            if (pcurmi){ 
                mnuHiliteMenuItem (pcurtmi, pcurmi, FALSE);
            }

            if (pnewmi) {
                mnuHiliteMenuItem (pcurtmi, pnewmi, TRUE);
                pcurtmi->philite = pnewmi;
            }
        }
    }

}

static void cursor_block_up(PTRACKMENUINFO pcurtmi, PMENUITEM pcurmi)
{
    PMENUITEM pnewmi;
    // NUV int y;

    if (pcurmi == NULL) return;

    pnewmi = mnuGetPrevMenuItem (pcurtmi, pcurmi);

    // NUV y = get_menu_item_y_from_pstart(pcurtmi, pcurmi);

    if (pcurtmi->rc.bottom > g_rcScr.bottom){
        if (pnewmi == mnuGetPrevMenuItem(pcurtmi, pcurtmi->pmi)){
            mnuShowPopupMenu (pcurtmi); 
            mnuHiliteMenuItem (pcurtmi, pcurmi, TRUE);
            pcurtmi->philite = pcurmi;
            return;
        }

        if (pnewmi == pcurtmi->pmi){
            mnuShowPopupMenu (pcurtmi); 
            if (pcurmi){ 
                mnuHiliteMenuItem (pcurtmi, pcurmi, FALSE);
            }
            mnuHiliteMenuItem (pcurtmi, pnewmi, TRUE);
            pcurtmi->philite = pnewmi;
            return;
        }

        if (pcurmi == pcurtmi->pstart_show_mi){
            mnu_scroll_menu(pcurtmi, (pcurtmi->rc.left + pcurtmi->rc.right) /2, 
                    pcurtmi->top_scroll_rc.top + 1);
        }
        else {
            if (pnewmi != pcurmi) {

                if (pcurmi){ 
                    mnuHiliteMenuItem (pcurtmi, pcurmi, FALSE);
                    draw_bottom_scroll_button(pcurtmi);
                }

                if (pnewmi) {
                    mnuHiliteMenuItem (pcurtmi, pnewmi, TRUE);
                    pcurtmi->philite = pnewmi;
                }
            }
        }
    }
    else {
        if (pnewmi != pcurmi) {

            if (pcurmi){ 
                mnuHiliteMenuItem (pcurtmi, pcurmi, FALSE);
            }

            if (pnewmi) {
                mnuHiliteMenuItem (pcurtmi, pnewmi, TRUE);
                pcurtmi->philite = pnewmi;
            }
        }
    }

}

static void mnuTrackMenuWithKey (PTRACKMENUINFO ptmi, 
                                int message, int scan, DWORD status)
{
    PTRACKMENUINFO phead, pcurtmi, pprevtmi;
    PMENUITEM pcurmi;
    HWND hwnd;
    HMENU hmnu;
    UINT flags;
    int barItem;
    int id;
    
    flags = ptmi->flags;
    hmnu = (HMENU)ptmi->pmi;
    
    phead = ptmi;
    while (phead->prev) {
        phead = phead->prev;
    }
    
    // get last tracking menu, the last menu is the current menu.
    pcurtmi = ptmi;
    while (pcurtmi->next) {
        pcurtmi = pcurtmi->next;
    }

    pprevtmi = pcurtmi->prev;

    pcurmi = pcurtmi->philite;

    if (message == MSG_KEYDOWN) {
        switch (scan) {
            case SCANCODE_CURSORBLOCKDOWN:
            case SCANCODE_CURSORBLOCKUP:
                if (scan == SCANCODE_CURSORBLOCKDOWN){
                    cursor_block_down(pcurtmi, pcurmi);    
                }
                else{// scan == SCANCODE_CURSORBLOCKUP:
                    cursor_block_up(pcurtmi, pcurmi);    
                }
                
                break;
                
            case SCANCODE_CURSORBLOCKRIGHT:
                if (pcurmi && pcurmi->submenu)
                    mnuOpenNewSubMenu (pcurtmi, 
                                pcurmi->submenu, 
                                pcurtmi->rc.right,
                                mnuGetMenuItemY (pcurtmi, pcurmi));
                else {
                    if (phead->pmb) {
                        barItem  = mnuGetNextMenuBarItem (phead->pmb, 
                                                          phead->barPos);
                        if (barItem != phead->barPos) {
                            hwnd = phead->hwnd;

                            // close current popup menu.
                            SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 0);

                            // open new popup menu.
                            TrackMenuBar (hwnd, barItem);
                        }
                    }
                }
                break;
                
            case SCANCODE_ESCAPE:
                if (pprevtmi) {
                    SendMessage (HWND_DESKTOP, 
                        MSG_ENDTRACKMENU, 0, (LPARAM)pcurtmi);
                }else{
                    SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);
                }
                break;

            case SCANCODE_CURSORBLOCKLEFT:
                if (pprevtmi)
                    SendMessage (HWND_DESKTOP, 
                        MSG_ENDTRACKMENU, 0, (LPARAM)pcurtmi);
                else {
                    if (phead->pmb) {
                        barItem  = mnuGetPrevMenuBarItem (phead->pmb, 
                                                          phead->barPos);
                        if (barItem != phead->barPos) {
                            hwnd = phead->hwnd;

                            // close current popup menu.
                            SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 0);

                            // open new popup menu.
                            TrackMenuBar (hwnd, barItem);
                        }
                    }
                }
                break;
                
            case SCANCODE_ENTER:
                if (pcurmi == NULL) return;
                if (pcurmi->mnutype == MFT_SEPARATOR) return;
                if (pcurmi->mnustate & MFS_DISABLED) return;
                
                if (pcurmi->submenu) {
                    mnuOpenNewSubMenu (pcurtmi, 
                                pcurmi->submenu, 
                                pcurtmi->rc.right,
                                mnuGetMenuItemY (pcurtmi, pcurmi));
                    return;
                }

                if (pcurmi && pcurmi->type != TYPE_PPPMENU &&
                    pcurmi->submenu == NULL) {

                    hwnd = ptmi->hwnd;
                    id = pcurmi->id;
        
                    SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);
       
                    if (flags & TPM_SYSCMD)
                        SendNotifyMessage (hwnd, MSG_SYSCOMMAND, id, 0);
                    else
                        SendNotifyMessage (hwnd, MSG_COMMAND, id, 0);

                    if (flags & TPM_DESTROY)
                        DestroyMenu (hmnu);
                }
                break;
        }
    }
    else if (message == MSG_KEYUP) {
        switch (scan) {
            case SCANCODE_LEFTALT:
            case SCANCODE_RIGHTALT:
                SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);

                if (flags & TPM_DESTROY)
                    DestroyMenu (hmnu);
                break;
        }
    }
}

static void mnu_scroll_menu (PTRACKMENUINFO ptmi, int x, int y)
{
    PTRACKMENUINFO phead, plast;
    PTRACKMENUINFO pcurtmi, pclose, ptemp;
    PMENUITEM pcurmi, pnewmi;
    PMENUITEM philite;
    int barItem;
    HWND hwnd;
    
    // get first tracking menu.
    phead = ptmi;
    while (phead->prev) {
        phead = phead->prev;
    }
        
    // get last tracking menu.
    plast = ptmi;
    while (plast->next) {
        plast = plast->next;
    }

    pcurtmi = mnuHitTestMenu (plast, x, y);

    if (pcurtmi == NULL) {
        if (plast->philite){
            mnuHiliteMenuItem (plast, plast->philite, FALSE);
            plast->mouse_leave_flag = TRUE;
            draw_bottom_scroll_button(plast);
        }

        // check to see if move to the other sub menu of menu bar.
        if (phead->pmb) {
            barItem = MenuBarHitTest (phead->hwnd, x, y);
            if (barItem >= 0 && barItem != phead->barPos) {
                hwnd = phead->hwnd;

                // close current popup menu.
                SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 0);

                // open new popup menu.
                TrackMenuBar (hwnd, barItem);
            }
        }
            
        return;
    }

    pcurmi = get_selected_mi(pcurtmi, x, y);

    if (PtInRect(&pcurtmi->bottom_scroll_rc, x, y)){

        pcurtmi->top_scroll_rc.left = pcurtmi->rc.left;
        pcurtmi->top_scroll_rc.top = 0;
        pcurtmi->top_scroll_rc.right = pcurtmi->rc.right;
        //pcurtmi->top_scroll_rc.bottom = pcurtmi->top_scroll_rc.top + 
        //pcurtmi->pmi->h - SHORTEN;
        pcurtmi->top_scroll_rc.bottom = pcurtmi->top_scroll_rc.top + 
            GetSysFontHeight(SYSLOGFONT_MENU) - SHORTEN; 

        ptmi->show_rc.top = pcurtmi->top_scroll_rc.bottom;
        ptmi->show_rc.left = pcurtmi->rc.left; 
        ptmi->show_rc.right = pcurtmi->rc.right; 
        //ptmi->show_rc.bottom = g_rcScr.bottom - ptmi->pmi->h + SHORTEN;  
        ptmi->show_rc.bottom = g_rcScr.bottom - 
            GetSysFontHeight(SYSLOGFONT_MENU) + SHORTEN;  

        pcurmi = get_selected_mi(pcurtmi, x, y);
        pcurtmi->pstart_show_mi = 
            mnuGetNextMenuItem(pcurtmi, pcurtmi->pstart_show_mi); 

        pcurtmi->draw_top_flag = TRUE;

        show_scroll_popup_menu(pcurtmi);
    }

    if (PtInRect(&pcurtmi->top_scroll_rc, x, y)){

        pnewmi = mnuGetPrevMenuItem(pcurtmi, pcurtmi->pstart_show_mi); 
        
        if (pnewmi == mnuGetPrevMenuItem(pcurtmi, pcurtmi->pmi)){
            mnuShowPopupMenu (pcurtmi); 
            //mnuHiliteMenuItem (pcurtmi, pcurtmi->pmi, TRUE);
            //pcurtmi->philite = pcurmi;
            return;
        }

        if (pnewmi == pcurtmi->pmi){
            mnuShowPopupMenu (pcurtmi); 
            if (pcurtmi->philite) { 
                mnuHiliteMenuItem (pcurtmi, pcurtmi->philite, FALSE);
            }
            mnuHiliteMenuItem (pcurtmi, pnewmi, TRUE);
            pcurtmi->philite = pnewmi;
            return;
        }

        //pcurtmi->pstart_show_mi = mnuGetPrevMenuItem
        //(pcurtmi, pcurtmi->pstart_show_mi); 
        pcurtmi->pstart_show_mi = pnewmi;

        pcurtmi->show_rc.top = pcurtmi->top_scroll_rc.bottom;
        pcurtmi->show_rc.left = pcurtmi->rc.left; 
        pcurtmi->show_rc.right = pcurtmi->rc.right; 
        //pcurtmi->show_rc.bottom = g_rcScr.bottom - pcurtmi->pmi->h + SHORTEN; 
        ptmi->show_rc.bottom = g_rcScr.bottom - 
            GetSysFontHeight(SYSLOGFONT_MENU) + SHORTEN;  

        pcurtmi->bottom_scroll_rc.top = pcurtmi->show_rc.bottom;
        pcurtmi->bottom_scroll_rc.left = pcurtmi->rc.left;
        pcurtmi->bottom_scroll_rc.right = pcurtmi->rc.right;
        pcurtmi->bottom_scroll_rc.bottom = g_rcScr.bottom;

        pcurtmi->draw_bottom_flag = TRUE;
        pcurmi = pcurtmi->pstart_show_mi;

        show_scroll_popup_menu(pcurtmi);
          
    }

    if (pcurtmi != plast) {
        pclose = pcurtmi->next->next;
       
        if (pclose != NULL) {
            while (pclose != plast) {
                ptemp = plast->prev;
                SendMessage (HWND_DESKTOP, MSG_ENDTRACKMENU, 0, (LPARAM)plast);
                plast = ptemp;
            }
            SendMessage (HWND_DESKTOP, MSG_ENDTRACKMENU, 0, (LPARAM)plast);
            plast = pcurtmi->next;
        }
            
        if (plast->philite) {
            mnuHiliteMenuItem (plast, plast->philite, FALSE);
            plast->philite = NULL;
        }
    }

    philite = pcurtmi->philite;
    if ((pcurmi == philite) && !pcurtmi->mouse_leave_flag){
            return;
    }
    pcurtmi->mouse_leave_flag = FALSE;// mouse come back pcurtmi

    // current hilite menu item is a bottom most item.
    if (philite) {
        if (pcurtmi != plast)
            SendMessage (HWND_DESKTOP, MSG_ENDTRACKMENU, 0, (LPARAM)plast);
        mnuHiliteMenuItem (pcurtmi, philite, FALSE);
        draw_bottom_scroll_button(pcurtmi);
    }

    pcurtmi->philite = pcurmi;
    if (pcurmi == NULL) {
         return;
    }

    if (pcurmi->type == TYPE_PPPMENU) return;


    mnuHiliteMenuItem (pcurtmi, pcurmi, TRUE);
    
     if (pcurtmi->draw_bottom_flag){
         draw_bottom_scroll_button(ptmi);
    }

    if (pcurmi->submenu)
        mnuOpenNewSubMenu (pcurtmi, pcurmi->submenu, 
                           pcurtmi->rc.right, mnuGetMenuItemY (pcurtmi, pcurmi));
}

#if 0
static void mnuTrackMenu (PTRACKMENUINFO ptmi, int x, int y)
{
    PTRACKMENUINFO phead, plast;
    PTRACKMENUINFO pcurtmi, pclose, ptemp;
    PMENUITEM pcurmi;
    PMENUITEM philite;
    int barItem;
    HWND hwnd;
    
    // get first tracking menu.
    phead = ptmi;
    while (phead->prev) {
        phead = phead->prev;
    }
        
    // get last tracking menu.
    plast = ptmi;
    while (plast->next) {
        plast = plast->next;
    }

    pcurtmi = mnuHitTestMenu (plast, x, y);


    if (pcurtmi == NULL) {
        if (plast->philite)
            mnuHiliteMenuItem (plast, plast->philite, FALSE);

        // check to see if move to the other sub menu of menu bar.
        if (phead->pmb) {
            barItem = MenuBarHitTest (phead->hwnd, x, y);
            if (barItem >= 0 && barItem != phead->barPos) {
                    
                hwnd = phead->hwnd;

                // close current popup menu.
                SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 0);

                // open new popup menu.
                TrackMenuBar (hwnd, barItem);
            }
        }
            
        return;
    }

    pcurmi = get_selected_mi(pcurtmi, x, y);
    if (PtInRect(&pcurtmi->bottom_scroll_rc, x, y)){
            
        pcurtmi->pstart_show_mi = pcurtmi->pstart_show_mi->next;
        show_scroll_popup_menu(pcurtmi);
          
    }

    if (pcurtmi != plast) {
        pclose = pcurtmi->next->next;
       
        if (pclose != NULL) {
            while (pclose != plast) {
                ptemp = plast->prev;
                SendMessage (HWND_DESKTOP, MSG_ENDTRACKMENU, 0, (LPARAM)plast);
                plast = ptemp;
            }
            SendMessage (HWND_DESKTOP, MSG_ENDTRACKMENU, 0, (LPARAM)plast);
            plast = pcurtmi->next;
        }
            
        if (plast->philite) {
            mnuHiliteMenuItem (plast, plast->philite, FALSE);
            plast->philite = NULL;
        }
    }

    philite = pcurtmi->philite;
    if (pcurmi == philite) return;

    // current hilite menu item is a bottom most item.
    if (philite) {
        if (pcurtmi != plast)
            SendMessage (HWND_DESKTOP, MSG_ENDTRACKMENU, 0, (LPARAM)plast);
        mnuHiliteMenuItem (pcurtmi, philite, FALSE);
    }

    pcurtmi->philite = pcurmi;
    if (pcurmi == NULL) return;
    if (pcurmi->type == TYPE_PPPMENU) return;


    mnuHiliteMenuItem (pcurtmi, pcurmi, TRUE);

    if (pcurmi->submenu)
        mnuOpenNewSubMenu (pcurtmi, pcurmi->submenu, 
            pcurtmi->rc.right, mnuGetMenuItemY (pcurtmi, pcurmi));
}
#endif 

static int mnuTrackMenuOnButtonDown (PTRACKMENUINFO ptmi, 
        int message, int x, int y)
{
    PTRACKMENUINFO plast, phead;
    PTRACKMENUINFO pcurtmi;
    HMENU hmnu;
    UINT flags;
    int barItem;
    
    flags = ptmi->flags;
    hmnu = (HMENU)ptmi->pmi;
    
    // get first tracking menu.
    phead = ptmi;
    while (phead->prev) {
        phead = phead->prev;
    }

    // get last tracking menu.
    plast = ptmi;
    while (plast->next) {
        plast = plast->next;
    }

    pcurtmi = mnuHitTestMenu (plast, x, y);

    if (pcurtmi == NULL) {      // close menu.
        if (phead->pmb) {
            barItem = MenuBarHitTest (phead->hwnd, x, y);
            if (barItem == phead->barPos)
                return 0;
        }

        SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);

        if (flags & TPM_DESTROY)
            DestroyMenu (hmnu);

        return -1;
    }

    return 0;
}

static int mnuTrackMenuOnButtonUp (PTRACKMENUINFO ptmi, 
        int message, int x, int y)
{
    PTRACKMENUINFO plast;
    PTRACKMENUINFO pcurtmi;
    PMENUITEM pcurmi;
    HWND hwnd;
    UINT flags;
    int  id;
    HMENU hmnu;
    
    // get last tracking menu.
    plast = ptmi;
    while (plast->next) {
        plast = plast->next;
    }

    pcurtmi = mnuHitTestMenu (plast, x, y);

    if (pcurtmi == NULL || pcurtmi->philite == NULL) return 0;

    /*add*/ 
    pcurmi = get_selected_mi(pcurtmi, x, y);
    if (pcurmi == NULL) return 0;
    /*add end*/

    if (pcurmi->mnutype == MFT_SEPARATOR) return 0;
    if (pcurmi->mnustate & MFS_DISABLED) return 0;
        
    if (pcurmi->type != TYPE_PPPMENU &&
        pcurmi->submenu == NULL)
    {
        hwnd = ptmi->hwnd;
        flags = ptmi->flags;
        id = pcurmi->id;
        hmnu = (HMENU)ptmi->pmi;
        

        SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);

        if (flags & TPM_SYSCMD)
            SendNotifyMessage (hwnd, MSG_SYSCOMMAND, id, 0);
        else
            SendNotifyMessage (hwnd, MSG_COMMAND, id, 0);

        if (flags & TPM_DESTROY)
            DestroyMenu (hmnu);
            
        return id;
    }

    return 0;
}

int PopupMenuTrackProc (PTRACKMENUINFO ptmi, 
    int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITMENU:
            mnuGetPopupMenuExtent (ptmi);
        break;

        case MSG_SHOWMENU:
            if (GetCurrentCursor() != GetSystemCursor(IDC_ARROW))
                SetCursor(GetSystemCursor(IDC_ARROW));
            return mnuShowPopupMenu (ptmi); 
        break;

        case MSG_HIDEMENU:
            do_restore_box (ptmi);
            mnuUnhiliteMenu (ptmi->pmi);
        break;

        case MSG_ENDTRACKMENU:
            do_free_box (ptmi);
            if (ptmi->hwnd != HWND_DESKTOP)
                SendNotifyMessage (ptmi->hwnd, MSG_ENDTRACKMENU, 
                            (WPARAM)ptmi->pmi, (LPARAM)ptmi->pmb);
            FreeTrackMenuInfo (ptmi);
        break;

        case MSG_CLOSEMENU:
            mnuCloseMenu (ptmi);
        break;

        case MSG_LBUTTONDOWN:
        case MSG_RBUTTONDOWN:
            if (mnuTrackMenuOnButtonDown (ptmi, 
                    message, (int)wParam, (int)lParam) < 0)
                return 1;
        break;

        case MSG_LBUTTONUP:
        case MSG_RBUTTONUP:
            mnuTrackMenuOnButtonUp (ptmi, message, (int)wParam, (int)lParam);
            return 1;

        case MSG_MOUSEMOVE:
            mnu_scroll_menu(ptmi, (int)wParam, (int)lParam);
            break;

        case MSG_KEYDOWN:
        case MSG_KEYUP:
            mnuTrackMenuWithKey (ptmi, message, (int)wParam, (DWORD)lParam);
            break;

        default:
            return 1;
    }

    return 0;
}
    
int GUIAPI TrackPopupMenu (HMENU hmnu, UINT uFlags, int x, int y, HWND hwnd)
{
    PTRACKMENUINFO ptmi;

    // close current menu firstly.
    SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);

    ptmi = TrackMenuInfoAlloc ();
    
    ptmi->rc.left = x;
    ptmi->rc.top  = y;
    ptmi->pmi = (PMENUITEM)hmnu;
    ptmi->pmb = NULL;
    ptmi->barPos = -1;
    ptmi->philite = NULL;
    ptmi->hwnd = hwnd;
    ptmi->flags = uFlags;
    ptmi->next = NULL;
    ptmi->prev = NULL;
   
    if (ptmi->pmi == NULL) return ERR_INVALID_HANDLE;
    if (ptmi->pmi->category != TYPE_HMENU) return ERR_INVALID_HANDLE;
    
    SendMessage (hwnd, MSG_ACTIVEMENU, 0, (LPARAM)hmnu);
    if (SendMessage (HWND_DESKTOP, MSG_TRACKPOPUPMENU,
                    0, (LPARAM)(ptmi)) < 0) {
        FreeTrackMenuInfo (ptmi);
        return -1;
    }

    return 0;
}

int GUIAPI TrackMenuBar (HWND hwnd, int pos)
{
    PMENUITEM pppMenu;
    PMENUBAR  menuBar;
    RECT rcMenuItem;
    PTRACKMENUINFO ptmi;

    menuBar = (PMENUBAR)GetMenu (hwnd);
    if (!menuBar) return ERR_INVALID_HANDLE;
    
    if (!(pppMenu = mnuGetMenuItem ((HMENU)menuBar, pos, MF_BYPOSITION))) {
        return ERR_INVALID_HANDLE;
    }
    if (pppMenu->mnustate & MFS_DISABLED)
        return -1;

    pppMenu = pppMenu->submenu;

    if (!pppMenu) return ERR_INVALID_HANDLE;
    
    // close current menu firstly.
    SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);

    ptmi = TrackMenuInfoAlloc ();
    
    GetMenuBarItemRect (hwnd, pos, &rcMenuItem);
    WindowToScreen (hwnd, &rcMenuItem.left, &rcMenuItem.bottom);
    
    ptmi->rc.left = rcMenuItem.left - (LFRDR_MENUITEMOFFX>>1);
    ptmi->rc.top  = rcMenuItem.bottom;
    ptmi->pmi = pppMenu;
    ptmi->pmb = menuBar;
    ptmi->barPos = pos;
    ptmi->philite = NULL;
    ptmi->hwnd = hwnd;
    ptmi->flags = TPM_DEFAULT;
    ptmi->next = NULL;
    ptmi->prev = NULL;
   
    HiliteMenuBarItem (hwnd, pos, LFRDR_MENU_STATE_NORMAL);
    HiliteMenuBarItem (hwnd, pos, LFRDR_MENU_STATE_HILITE);

    SendMessage (hwnd, MSG_ACTIVEMENU, pos, (LPARAM)pppMenu);

    if (SendMessage (HWND_DESKTOP, MSG_TRACKPOPUPMENU,
                    0, (LPARAM)(ptmi)) < 0) {
        FreeTrackMenuInfo (ptmi);
        return -1;
    }

    return 0;
}
#endif
