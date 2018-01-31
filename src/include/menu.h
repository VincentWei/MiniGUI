/*
** $Id: menu.h 9892 2008-03-20 02:50:02Z xwyan $
**
** menu.h: the head file of menu module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
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

