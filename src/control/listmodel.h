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
** listmodel.h: header file of item data model.
*/

#ifndef _LISTMODEL_H
#define _LISTMODEL_H


#include "ctrl/scrollview.h"

#include "list.h"
#include "scrolled.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ---------------------------------------------------------------- */

typedef GHANDLE HITEM;
typedef SVITEM_CMP MGITEM_CMP;

typedef struct _MgList MgList;
typedef struct _MgItem MgItem;

typedef int (*MGITEM_CMP_EX) (MgList *mglst, HITEM hsvi1, HITEM hsvi2);
typedef int (*MGITEM_GETRECT) (HWND hWnd, HITEM hitem, RECT *rcItem, BOOL bConv);
typedef int (*MGITEM_ISIN)    (MgList *mglst, int mouseX, int mouseY, MgItem**pRet, int *item_y);

typedef struct _mgitem_operations
{
    SVITEM_INITFUNC     initItem;     /** called when an scrollview item is created */
    SVITEM_DESTROYFUNC  destroyItem;  /** called when an item is destroied */
    SVITEM_DRAWFUNC     drawItem;     /** call this to draw an item */

    MGITEM_GETRECT      getRect;      /** call this to get item rect */
    MGITEM_ISIN         isInItem;     /** call this to decides whether a position is in an item rect */
} MGITEMOPS;
typedef MGITEMOPS* PMGITEMOPS;


/* ---------------------------------------------------------------- */

#define CF_AUTOSORT      0x00000001    /* auto sort style */

/* item status flags */
#define CTST_NORMAL     0x0000    //normal status
#define CTST_SELECTED   0x0001L   //selected status

//#define MGLIST_ST_NORMAL     0x00000000L
#define MGLIST_ST_FROZEN     0x00000010L

#define MG_CMP_TYPE_NORMAL      0
#define MG_CMP_TYPE_EX          1


/* item struct */
struct _MgItem
{
    list_t    list;          // list pointer
    list_t    sel_list;      // selected list pointer
    DWORD     flags;         // item flags

    DWORD     addData;       // item additional data
};

struct _MgList
{
    /* current list item number */
    int nItemNr;

    /* total item height */
    int nTotalItemH;

    /* ptr of the hilighted item */
    MgItem* pItemHilighted;

    /* item list queue */
    list_t queue;
    /* selected item list queue */
    list_t sel_queue;

//FIXME, should has only one copy for a class
    /* item compare function */
    MGITEM_CMP itemCmp;
    MGITEMOPS iop;

    /* mglist window */
    HWND hLst;

    DWORD flags;

};

/* ------------------------- external api -------------------------- */

MG_EXPORT int DefaultItemViewProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam,
                         PSCRDATA pscrdata, MgList *mglst);

MG_EXPORT SVITEM_DRAWFUNC mglist_set_itemdraw (MgList *mglst, SVITEM_DRAWFUNC draw_func);

MG_EXPORT int     mglist_init               (MgList *mglst, HWND hWnd);
MG_EXPORT void    mglist_destroy            (MgList *mglst);
MG_EXPORT int     mglist_add_item           (MgList *mglst, MgItem* hitem, MgItem* hprev, MgItem *hnext, 
                                             int nItem, int *idx);
MG_EXPORT int     mglist_del_item           (MgList *mglst, MgItem* hitem);
MG_EXPORT DWORD   mglist_get_item_adddata   (HITEM hitem);
MG_EXPORT DWORD   mglist_set_item_adddata   (HITEM hitem, DWORD adddata);
MG_EXPORT void    mglist_hilight_item       (MgList *mglst, MgItem* hitem);
MG_EXPORT void    mglist_refresh_item_ex    (MgList *mglst, HITEM hivi, const RECT *rcInv);
MG_EXPORT void    mglist_refresh_item       (MgList *mglst, HITEM hivi);
MG_EXPORT void    mglist_set_itemops        (MgList *mglst, PMGITEMOPS iop);
MG_EXPORT MgItem* mglist_get_item_by_index  (MgList *mglst, int nItem);
MG_EXPORT MgItem* mglist_get_next_item      (MgList *mglst, MgItem* hitem);
MG_EXPORT MgItem* mglist_get_prev_item      (MgList *mglst, MgItem* hitem);
MG_EXPORT int     mglist_move_item          (MgList *mglst, MgItem* pci, MgItem* preitem);
MG_EXPORT MgItem* mglist_getitem_byindex    (MgList *mglst, int index);
MG_EXPORT int     mglist_get_item_index     (MgList *mglst, MgItem *item);
MG_EXPORT int     mglist_sort_items         (MgList *mglst, void* pfn, int fn_type);
MG_EXPORT BOOL    mglist_make_item_visible  (MgList *mglst, PSCRDATA pscrdata, HITEM hitem);
MG_EXPORT void    mglist_freeze             (HWND hWnd, MgList *mglst, BOOL lock);
MG_EXPORT int     mglist_adjust_items_height(HWND hWnd, MgList *mglst, PSCRDATA pscrdata, int diff);
MG_EXPORT void    mglist_redraw_item        (MgList *mglst, HITEM hivi);


#define mglist_for_each(pos, list) \
    list_for_each(pos, &((MgList *)list)->queue)

#define mglist_entry(ptr) \
    list_entry(ptr, MgItem, list)

#define mglist_empty(mglst) \
    list_empty(&(mglst)->queue)

#define mglist_first_item(mglst) \
    list_entry((mglst)->queue.next, MgItem, list)


static inline int mglist_is_item_hilight (MgList *mglst, MgItem* hitem)
{
    return mglst->pItemHilighted == hitem;
}   

static inline int mglist_is_item_selected (MgItem* pci)
{   
    return pci->flags & CTST_SELECTED;
}   
    
static inline MgItem* mglist_get_hilighted_item (MgList *mglst)
{
    return mglst->pItemHilighted;
}

static inline MgItem* mglist_set_hilighted_item (MgList *mglst, MgItem *item)
{   
    MgItem *old;

    old = mglst->pItemHilighted;
    mglst->pItemHilighted = item;
    return old;
}   

static inline void mglist_select_item (MgList *mglst, MgItem* pci, BOOL bSel)
{
    if (bSel) {
        if (!(pci->flags & CTST_SELECTED)) {
            pci->flags |= CTST_SELECTED;
            list_add_tail (&pci->sel_list, &mglst->sel_queue);
        }
    }
    else {
        if (pci->flags & CTST_SELECTED) {
            pci->flags &= ~CTST_SELECTED;
            list_del (&pci->sel_list);
        }
    }
}

static inline void mglist_unselect_all (MgList *mglst)
{
    MgItem* pci;

    while (!list_empty (&mglst->sel_queue)) {
        pci = list_entry (mglst->sel_queue.next, MgItem, sel_list);
        if (pci->flags & CTST_SELECTED) {
            pci->flags &= ~CTST_SELECTED;
            list_del (mglst->sel_queue.next);
        }
    }
}

/* unselect all and select this one */
static inline void mglist_select_one_item (MgList *mglst, MgItem* hitem)
{
    MgItem* pci = (MgItem*)hitem;

    if (!pci) return;

    mglist_unselect_all (mglst);
    mglist_select_item (mglst, pci, TRUE);
}

static inline void mglist_select_all (MgList *mglst)
{
    MgItem* pci;
    list_t *me;

    list_for_each (me, &mglst->queue) {
        pci = list_entry (me, MgItem, list);
        mglist_select_item (mglst, pci, TRUE);
    }
}

static inline int mglist_get_item_count (MgList *mglst)
{
    return mglst->nItemNr;
}

static inline void mglist_set_autosort (MgList *mglst)
{
    mglst->flags |= CF_AUTOSORT;
}

static inline BOOL mglist_is_frozen (MgList *mglst)
{
    return mglst->flags & MGLIST_ST_FROZEN;
}

static inline int mglist_get_total_item_height (MgList *mglst)
{
    return mglst->nTotalItemH;
}

/* -------------------------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _LISTMODEL_H */

