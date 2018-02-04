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
** listmodel.c: List data model.
**
** Create date: 2004/10/09
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

#ifdef _MGCTRL_SCROLLVIEW
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "ctrlmisc.h"
#include "listmodel.h"
#include "scrolled.h"

/*
 * mglist_get_sort_index : Get sort index of an item
 */
int mglist_get_sort_index (MgList *mglst, MgItem *newci, BOOL bAdd)
{
    list_t *me;
    MgItem *pci = NULL;
    int i = 0;

    mglist_for_each (me, mglst) {
        pci = mglist_entry (me);
        if (mglst->itemCmp ((HITEM)newci, (HITEM)pci) < 0) {
            if (bAdd)
                list_add_tail (&newci->list, &pci->list);
            return i;
        }
        i++;
    }

    if (bAdd)
        list_add_tail (&newci->list, &mglst->queue);
    return i;
}

/*
 * mgitem_add : add an item to the list
 */
int mglist_add_item (MgList *mglst, MgItem *ci, MgItem* preitem, 
                         MgItem* nextitem, int nItem, int *idx)
{
    int ret;

    ci->flags = CTST_NORMAL;

    if (mglst->iop.initItem) {
        if (mglst->iop.initItem (mglst->hLst, (HITEM)ci) <0) {
            return -1;
        }
    }

    if (mglst->flags & CF_AUTOSORT && mglst->itemCmp) {
        ret = mglist_get_sort_index (mglst, ci, TRUE);
    }
    else {
        if (preitem) {
            list_add (&ci->list, &preitem->list);
            ret = -1;
        }
        else if (nextitem) {
            list_add_tail (&ci->list, &nextitem->list);
            ret = -1;
        }
        else {
            ret = list_add_by_index (&ci->list, &mglst->queue, nItem);
        }
    }

    mglst->nItemNr ++;

    if (idx)
        *idx = ( ret<0 ? mglst->nItemNr-1 : ret);

    return 0;
}

int mglist_del_item (MgList *mglst, MgItem* pci)
{
    if (mglst->iop.destroyItem) {
        mglst->iop.destroyItem (mglst->hLst, (HITEM)pci);
    }

    list_del (&pci->list);
    mglst->nItemNr --;

    if (pci == mglst->pItemHilighted)
        mglst->pItemHilighted = NULL;

    if (pci->flags & CTST_SELECTED)
        list_del (&pci->sel_list);

    return 0;
}

int
mglist_move_item (MgList *mglst, MgItem* pci, MgItem* preitem)
{
    list_del (&pci->list);

    if (preitem)
        list_add_tail(&pci->list, &preitem->list);
    else
        list_add_tail(&pci->list, &mglst->queue);

    return 0;
}

int mglist_adjust_items_height (HWND hWnd, MgList *mglst, PSCRDATA pscrdata, int diff)
{
    mglst->nTotalItemH += diff;

    if (mglist_is_frozen(mglst))
        return 0;

    scrolled_set_cont_height (hWnd, pscrdata, mglst->nTotalItemH + 1);//+1! where? 
    scrolled_refresh_content (pscrdata);
    return 0;
}

/* ---------------------------------------------------------------------------- */

int mglist_init (MgList *mglst, HWND hWnd)
{
    INIT_LIST_HEAD(&mglst->queue);
    INIT_LIST_HEAD(&mglst->sel_queue);

    mglst->nItemNr = 0;
    mglst->pItemHilighted = NULL;
    mglst->nTotalItemH = 0;

    mglst->itemCmp = NULL;
    mglst->hLst = hWnd;

    mglst->flags = 0;

    memset (&mglst->iop, 0, sizeof(mglst->iop));

    return 0;
}

GET_ENTRY_BY_INDEX(mlGetItemByIndex, MgItem, list)
GET_ENTRY_INDEX(mlGetItemIndex, MgItem, list)

MgItem* mglist_getitem_byindex (MgList *mglst, int index)
{
    return mlGetItemByIndex(&mglst->queue, index);
}

int mglist_get_item_index (MgList *mglst, MgItem *item)
{
    return mlGetItemIndex(&mglst->queue, item);
}

MgItem* mglist_get_next_item (MgList *mglst, MgItem* hitem)
{
    list_t *me = hitem ? hitem->list.next : mglst->queue.next;
    return ( (me == &mglst->queue) ? 0 : mglist_entry(me) );
}

MgItem* mglist_get_prev_item (MgList *mglst, MgItem* hitem)
{
    list_t *me = hitem ? hitem->list.prev : mglst->queue.prev;
    return ( (me == &mglst->queue) ? 0 : mglist_entry(me) );
}

void mglist_hilight_item (MgList *mglst, MgItem* hitem)
{
    if (hitem) {
        MgItem* oldhitem;

        oldhitem = mglst->pItemHilighted;
        mglst->pItemHilighted = hitem;
        mglist_select_item (mglst, hitem, TRUE);
        mglist_refresh_item (mglst, (HITEM)oldhitem);
        mglist_refresh_item (mglst, (HITEM)hitem);
    }
}

DWORD mglist_get_item_adddata (HITEM hitem)
{
    return hitem ? ((MgItem *)hitem)->addData : 0;
}

DWORD mglist_set_item_adddata (HITEM hitem, DWORD adddata)
{
    DWORD old;

    if (!hitem)
        return 0;

    old = ((MgItem *)hitem)->addData;
    ((MgItem *)hitem)->addData = adddata;
    return old;
}

void mglist_freeze (HWND hWnd, MgList *mglst, BOOL lock)
{
    if (lock) {
        mglst->flags |= MGLIST_ST_FROZEN;
    }
    else {
        PSCRDATA pscrdata = (PSCRDATA)GetWindowAdditionalData2(hWnd);
        mglst->flags &= ~MGLIST_ST_FROZEN;
        scrolled_set_cont_height (hWnd, pscrdata, mglst->nTotalItemH);
        //FIXME
        scrolled_refresh_content (pscrdata);
    }
}

int mglstSortItems (MgList *mglst, void* pfn, int fn_type)
{
    MgItem *pci, *pci2;
    list_t *me, *me2;
    MGITEM_CMP pcmp = NULL;
    MGITEM_CMP_EX pcmp_ex = NULL;
    int ret;

    if (fn_type == MG_CMP_TYPE_NORMAL) {
        if ( !(pcmp = (MGITEM_CMP)pfn) )
            return -1;
    }
    else if (fn_type == MG_CMP_TYPE_EX) {
        if ( !(pcmp_ex = (MGITEM_CMP_EX)pfn) )
            return -1;
    }
    else
        return -1;

    mglist_for_each (me, mglst) {
        pci = mglist_entry (me);
        pci2 = NULL;
        ret = 0;
        mglist_for_each (me2, mglst) {
            if (me == me2)
                break;
            pci2 = mglist_entry (me2);
            if (pcmp)
                ret = pcmp ((HITEM)pci, (HITEM)pci2);
            else
                ret = pcmp_ex (mglst, (HITEM)pci, (HITEM)pci2);
            if (ret < 0)
                break;
        }
        if (ret < 0) {
            me = me->prev;
            mglist_move_item (mglst, pci, pci2);
        }
    }

    return 0;
}

int mglist_sort_items (MgList *mglst, void* pfn, int fn_type)
{
    int ret;

    mglist_freeze (mglst->hLst, mglst, TRUE);
    ret = mglstSortItems (mglst, pfn, fn_type);
    mglist_freeze (mglst->hLst, mglst, FALSE);

    return ret;
}

void mglist_refresh_item_ex (MgList *mglst, HITEM hitem, const RECT *rcInv)
{
    RECT rcItem, rcTmp;

    if (!hitem || mglst->flags & MGLIST_ST_FROZEN) 
        return;

    mglst->iop.getRect (mglst->hLst, hitem, &rcItem, TRUE);
    rcTmp = *rcInv;
    OffsetRect (&rcTmp, rcItem.left, rcItem.top);

    if (IntersectRect(&rcTmp, &rcItem, &rcTmp))
        InvalidateRect (mglst->hLst, &rcTmp, TRUE);
}

void mglist_refresh_item (MgList *mglst, HITEM hitem)
{
    RECT rcItem;

    if (!hitem || mglst->flags & MGLIST_ST_FROZEN) 
        return;

    mglst->iop.getRect (mglst->hLst, hitem, &rcItem, TRUE);

    InvalidateRect (mglst->hLst, &rcItem, TRUE);
}

void mglist_redraw_item (MgList *mglst, HITEM hivi)
{
    RECT rcItem;
    HDC hdc = GetClientDC (mglst->hLst);

    if (!hivi) 
        return;

    mglst->iop.getRect (mglst->hLst, hivi, &rcItem, TRUE);
    mglst->iop.drawItem (mglst->hLst, hivi, hdc, &rcItem);

    ReleaseDC (hdc);
}

BOOL mglist_make_item_visible (MgList *mglst, PSCRDATA pscrdata, HITEM hitem)
{
    int pos_x, pos_y;
    RECT rcItem;
    int ret;

    if (!hitem) return FALSE;

    ret = mglst->iop.getRect (mglst->hLst, hitem, &rcItem, FALSE);
    if (ret < 0)
        return FALSE;
    else if (ret == 1) { /* make y pos visible */
        pos_x = -1;
    }
    else { /* x/y all visible */
        pos_x = (rcItem.left >= pscrdata->nContX) ? rcItem.right : rcItem.left;
    }

    pos_y = (rcItem.bottom >= (pscrdata->nContY + pscrdata->visibleHeight)) ? rcItem.bottom : rcItem.top;

    return scrolled_make_pos_visible (mglst->hLst, pscrdata, pos_x, pos_y);
}


/* -------------------------------------------------------------------------- */

SVITEM_DRAWFUNC mglist_set_itemdraw (MgList *mglst, SVITEM_DRAWFUNC draw_func)
{
    SVITEM_DRAWFUNC oldfn;

    oldfn = mglst->iop.drawItem;
    mglst->iop.drawItem = draw_func;

//FIXME, repaint ?
    return oldfn;
}

void mglist_set_itemops (MgList *mglst, PMGITEMOPS iop)
{
    mglst->iop = *iop;
}

int DefaultItemViewProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam, 
                         PSCRDATA pscrdata, MgList *mglst)
{
    switch (message) {

    case MSG_RBUTTONDOWN:
    case MSG_LBUTTONDOWN:
    {
        int mouseY = HISWORD (lParam);
        int mouseX = LOSWORD (lParam);
        int nItem;
        RECT rcVis;
        MgItem *hitem;

        if ( !mglst->iop.isInItem )
            break;
        scrolled_get_visible_rect (pscrdata, &rcVis);
        /* not in the visible area */
        if (!PtInRect (&rcVis, mouseX, mouseY))
            break;
        scrolled_window_to_content (pscrdata, &mouseX, &mouseY);
        if ( (nItem = mglst->iop.isInItem(mglst, mouseX, mouseY, &hitem, NULL)) >= 0 ) {
            if ( !(GetWindowStyle(hWnd) & SVS_UPNOTIFY) )
                NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), SVN_CLICKED, (DWORD)hitem);
            if (!mglist_is_item_hilight(mglst, hitem)) {
                mglist_hilight_item (mglst, hitem);
                mglist_make_item_visible (mglst, pscrdata, (HITEM)hitem);
                NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), SVN_SELCHANGED, (DWORD)hitem);
            }

            /* FIXME: should we auto-select the item? */
            //mglist_select_item (mglst, hitem, TRUE);
        }

        break;
    }

    case MSG_FREEZECTRL:
        mglist_freeze (hWnd, mglst, wParam);
        return 0;

    case SVM_SETITEMDRAW:
        return (LRESULT)mglist_set_itemdraw (mglst, (SVITEM_DRAWFUNC)lParam);

    case SVM_SETITEMINIT:
    {
        SVITEM_INITFUNC old_pfn;
        old_pfn = mglst->iop.initItem;
        mglst->iop.initItem = (SVITEM_INITFUNC)lParam;
        return (LRESULT)old_pfn;
    }
    case SVM_SETITEMDESTROY:
    {
        SVITEM_DESTROYFUNC old_pfn;
        old_pfn = mglst->iop.destroyItem;
        mglst->iop.destroyItem = (SVITEM_DESTROYFUNC)lParam;
        return (LRESULT)old_pfn;
    }

    case SVM_GETITEMADDDATA:
    {
        if (lParam)
            return mglist_get_item_adddata((HITEM)lParam);
        else {
            HITEM hitem = (HITEM)mglist_getitem_byindex(mglst, wParam);
            return mglist_get_item_adddata(hitem);
        }
    }
    case SVM_SETITEMADDDATA:
    {
        HITEM hitem = (HITEM)mglist_getitem_byindex(mglst, wParam);
        mglist_set_item_adddata (hitem, lParam);
        return 0;
    }

    case SVM_REFRESHITEM:
    {
        if (lParam)
            mglist_refresh_item (mglst, (HITEM)lParam);
        else {
            HITEM hitem = (HITEM) mglist_getitem_byindex(mglst, wParam);
            mglist_refresh_item (mglst, hitem);
        }
        return 0;
    }

    case SVM_SETITEMOPS:
#if 0
        *((SVITEMOPS*)&mglst->iop) = *(SVITEMOPS*)lParam;
        //mglist_set_itemops ((MgList *)&psvdata->svlist, (SVITEMOPS*)lParam);
#else
        mglst->iop.initItem = ((SVITEMOPS*)lParam)->initItem;
        mglst->iop.destroyItem = ((SVITEMOPS*)lParam)->destroyItem;
        mglst->iop.drawItem = ((SVITEMOPS*)lParam)->drawItem;
#endif
        return 0;

    case SVM_GETCURSEL:
        return mglist_get_item_index(mglst, mglist_get_hilighted_item(mglst));

    case SVM_SETCURSEL:
    {
        HITEM hitem = (HITEM)mglist_getitem_byindex(mglst, wParam);
        if (hitem) {
            //FIXME, should we automatically refresh hilighted item ?
            //mglist_set_hilighted_item (mglst, (MgItem *)hitem);
            mglist_hilight_item (mglst, (MgItem *)hitem);
            // make this item visible
            if (lParam)
                mglist_make_item_visible (mglst, pscrdata, hitem);
        }

        return 0;
    }

    case SVM_SHOWITEM:
    {
        if (lParam)
            mglist_make_item_visible (mglst, pscrdata, (HITEM)lParam);
        else {
            HITEM hitem = (HITEM)mglist_getitem_byindex(mglst, wParam);
            mglist_make_item_visible (mglst, pscrdata, hitem);
        }
        return 0;
    }

    case SVM_SELECTITEM:
    {
        HITEM hitem = (HITEM)mglist_getitem_byindex(mglst, wParam);
        if (hitem) {
            mglist_select_item (mglst, (MgItem *)hitem, lParam);
        }
        return 0;
    }

    case SVM_CHOOSEITEM:
    {
        HITEM hitem;

        if (lParam)
            hitem = (HITEM)lParam;
        else
            hitem = (HITEM)mglist_getitem_byindex(mglst, wParam);

        if (!hitem) 
            return -1;
        
        mglist_select_item (mglst, (MgItem *)hitem, TRUE);
        mglist_make_item_visible (mglst, pscrdata, hitem);
        return 0;
    }

    case SVM_SETITEMCMP:
    {
        SVITEM_CMP oldcmp = mglst->itemCmp;
        mglst->itemCmp = (SVITEM_CMP)lParam;
        return (LRESULT)oldcmp;
    }

    case SVM_SORTITEMS:
    {
        SVITEM_CMP pfn = (SVITEM_CMP)lParam;
 
        if (mglist_sort_items (mglst, (void*)pfn, MG_CMP_TYPE_NORMAL) == 0) {
            //FIXME, need ?
            InvalidateRect (hWnd, NULL, TRUE);
        }
        return 0;
    }

    case SVM_GETITEMCOUNT:
        return mglist_get_item_count(mglst);

    }/* end switch */

    return DefaultScrolledProc (hWnd, message, wParam, lParam);
}

/* -------------------------------------------------------------------------- */


#endif /* _MGCTRL_SCROLLVIEW */

