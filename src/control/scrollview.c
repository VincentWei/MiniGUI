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
** scrollview.c: a scrollview control
**
** Create date: 2004/03/01
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
#include "ctrl/scrollview.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "ctrlmisc.h"
#include "scrolled.h"
#include "scrollview_impl.h"

/* ------------------------------------ svlist --------------------------- */

static PSVITEMDATA svlistItemAdd (PSVLIST psvlist, PSVITEMDATA preitem, 
                PSVITEMDATA nextitem, PSVITEMINFO pii, int *idx)
{
    PSVITEMDATA ci;
    int ret;

    ci = (PSVITEMDATA) malloc (sizeof(SVITEMDATA));
    if (!ci)
        return NULL;

    if (pii) {
        if (pii->nItemHeight <= 0) 
            ci->nItemHeight = psvlist->nDefItemHeight;
        else
            ci->nItemHeight = pii->nItemHeight;

        ((MgItem *)ci)->addData = pii->addData;
    }
    else {
        ci->nItemHeight = psvlist->nDefItemHeight;
        ((MgItem *)ci)->addData = 0;
    }

    ret = mglist_add_item ( (MgList *)psvlist, (MgItem *)ci, (MgItem *)preitem, 
                      (MgItem *)nextitem, pii->nItem, idx );
    if (ret < 0) {
        free (ci);
        return NULL;
    }

    return ci;
}

static int svlistItemDel (PSVLIST psvlist, PSVITEMDATA pci)
{
    mglist_del_item ((MgList *)psvlist, (MgItem *)pci);
    free (pci);
    return 0;
}

static void svlist_reset_content (HWND hWnd, PSVLIST psvlist)
{
    PSVITEMDATA pci;

    while (!mglist_empty((MgList *)psvlist)) {
        pci = (PSVITEMDATA)mglist_first_item ((MgList *)psvlist);
        svlistItemDel (psvlist, pci);
    }
    ((MgList *)psvlist)->nTotalItemH = 0;
}

//FIXME, to be moved to listmodel.c
static int isInItem (MgList *mglst, int mouseX, int mouseY, 
                     MgItem **pRet, int *item_y)
{
    //PSVLIST psvlist = (PSVLIST)mglst;
    int ret = 0;
    PSVITEMDATA pci = NULL;
    int h = 0;
    list_t *me;

    if (mouseY < 0 || mouseY >= mglst->nTotalItemH)
        return -1;

    mglist_for_each (me, mglst) {
        pci = (PSVITEMDATA)mglist_entry (me);
        h += pci->nItemHeight;
        if (h > mouseY)
            break;
        ret++;
    }

    if (pRet)
        *pRet = (MgItem *)pci;
    if (item_y)
        *item_y = h - pci->nItemHeight;

    return ret;
}

int scrollview_is_in_item (PSVDATA psvdata, int mousey, HSVITEM *phsvi, int *itemy)
{
    return isInItem ((MgList *)&psvdata->svlist, 0, mousey, (MgItem **)phsvi, itemy);
}

/* ---------------------------------------------------------------------------- */

HSVITEM scrollview_add_item (HWND hWnd, PSVDATA psvdata, HSVITEM prehsvi, PSVITEMINFO pii, int *idx)
{
    PSVLIST psvlist = &psvdata->svlist;
    PSVITEMDATA pci;
    int index;

    if ( (pci =svlistItemAdd (psvlist, (PSVITEMDATA)prehsvi, NULL, pii, &index)) ) {
        mglist_adjust_items_height (hWnd, (MgList *)psvlist, psvscr, pci->nItemHeight);
    }

    if (idx)
        *idx = index;

    return (HSVITEM)pci;
}

//FIXME
HSVITEM scrollview_add_item_ex (HWND hWnd, PSVDATA psvdata, HSVITEM prehsvi, 
                HSVITEM nexthsvi, PSVITEMINFO pii, int *idx)
{
    PSVLIST psvlist = &psvdata->svlist;
    PSVITEMDATA pci;
    int index;

    if ( (pci =svlistItemAdd (psvlist, (PSVITEMDATA)prehsvi, 
                      (PSVITEMDATA)nexthsvi, pii, &index)) ) {
        mglist_adjust_items_height (hWnd, (MgList *)psvlist, psvscr, pci->nItemHeight);
    }

    if (idx)
        *idx = index;

    return (HSVITEM)pci;
}

int scrollview_move_item (PSVDATA psvdata, HSVITEM hsvi, HSVITEM prehsvi)
{
    PSVITEMDATA pci, preitem;

    if (!hsvi)
        return -1;
    pci = (PSVITEMDATA)hsvi;
    preitem = (PSVITEMDATA)prehsvi;

    mglist_move_item ((MgList *)&psvdata->svlist, (MgItem *)pci, (MgItem *)preitem);
    scrollview_refresh_content (psvdata);
    return 0;
}

int scrollview_del_item (HWND hWnd, PSVDATA psvdata, int nItem, HSVITEM hsvi)
{
    PSVLIST psvlist = &psvdata->svlist;
    PSVITEMDATA pci;
    int del_h;

    if (hsvi)
        pci = (PSVITEMDATA)hsvi;
    else {
        if ( !(pci=(PSVITEMDATA)mglist_getitem_byindex((MgList *)psvlist, nItem)) )
            return -1;
    }
    del_h = pci->nItemHeight;

    if (svlistItemDel (psvlist, pci) >= 0) {
        mglist_adjust_items_height (hWnd, (MgList *)psvlist, psvscr, -del_h);
    }
    return 0;
}

DWORD scrollview_get_item_adddata (HSVITEM hsvi)
{
    return mglist_get_item_adddata (hsvi);
}   

int scrollview_get_item_index (HWND hWnd, HSVITEM hsvi)
{
    PSVDATA psvdata = (PSVDATA)GetWindowAdditionalData2(hWnd);
    return mglist_get_item_index ((MgList *)&psvdata->svlist, (MgItem *)hsvi);
}

int scrollview_is_item_hilight (HWND hWnd, HSVITEM hsvi)
{
    PSVDATA psvdata = (PSVDATA)GetWindowAdditionalData2(hWnd);
    return (int)mglist_is_item_hilight ((MgList *)&psvdata->svlist, (MgItem *)hsvi);
}

int scrollview_is_item_selected (HSVITEM hsvi)
{
    return mglist_is_item_selected ((MgItem *)hsvi);
}

static int svGetItemRect (PSVDATA psvdata, HSVITEM hsvi, RECT *rcItem, BOOL bConv)
{
    PSVITEMDATA pci = (PSVITEMDATA)hsvi;

    if (!rcItem || !hsvi)
        return -1;

    rcItem->top = scrollview_get_item_ypos (psvdata, hsvi);
    if (bConv)
        scrolled_content_to_window ((PSCRDATA)psvdata, NULL, &rcItem->top);
    rcItem->bottom = rcItem->top + pci->nItemHeight;

    rcItem->left = 0;
    if (bConv)
        scrolled_content_to_window ((PSCRDATA)psvdata, &rcItem->left, NULL);
//FIXME
    rcItem->right = rcItem->left + MAX(psvscr->nContWidth, psvscr->visibleWidth);

    /* only y position is important */
    return 1;
}

int scrollview_get_item_rect (HWND hWnd, HITEM hsvi, RECT *rcItem, BOOL bConv)
{
    PSVDATA psvdata = (PSVDATA)GetWindowAdditionalData2(hWnd);
    return svGetItemRect (psvdata, hsvi, rcItem, bConv);
}

int scrollview_get_item_ypos (PSVDATA psvdata, HSVITEM hsvi)
{
    PSVLIST psvlist = &psvdata->svlist;
    PSVITEMDATA ci = (PSVITEMDATA)hsvi, pci;
    list_t *me;
    int h = 0;

    mglist_for_each (me, psvlist) {
        pci = (PSVITEMDATA)mglist_entry (me);
        if (pci == ci)
            return h;
        h += pci->nItemHeight;
    }
    return -1;
}

int scrollview_set_item_height (HWND hWnd, HSVITEM hsvi, int height)
{
    PSVDATA psvdata = (PSVDATA)GetWindowAdditionalData2(hWnd);
    PSVITEMDATA pci = (PSVITEMDATA)hsvi;
    int diff;

    if (!psvdata || !pci || height < 0 || height == pci->nItemHeight) return -1;

    diff = height - pci->nItemHeight;
    pci->nItemHeight = height;

    mglist_adjust_items_height (hWnd, (MgList *)&psvdata->svlist, psvscr, diff);

    return height - diff;
}

SVITEM_DRAWFUNC
scrollview_set_itemdraw (PSVDATA psvdata, SVITEM_DRAWFUNC draw_func)
{
    SVITEM_DRAWFUNC oldfn;

    oldfn = mglist_set_itemdraw((MgList *)&psvdata->svlist, draw_func);
    scrollview_refresh_content (psvdata);

    return oldfn;
}

void scrollview_reset_content (HWND hWnd, PSVDATA psvdata)
{
    /* delete all svlist content */
    //svlist_reset_content (hsvwnd, &psvdata->svlist);

    if (psvscr->sbPolicy != SB_POLICY_ALWAYS) {
        ShowScrollBar (hWnd, SB_HORZ, FALSE);
        ShowScrollBar (hWnd, SB_VERT, FALSE);
    }

    /* reset content and viewport size */
    scrolled_init_contsize (hWnd, psvscr);
    /* reset svlist window */
    //scrollview_set_svlist (hWnd, psvscr);
    /* reset viewport window */
    scrolled_set_visible (hWnd, psvscr);

    scrolled_set_hscrollinfo (hWnd, psvscr);
    scrolled_set_vscrollinfo (hWnd, psvscr);

    /* delete all svlist content */
    svlist_reset_content (hsvwnd, &psvdata->svlist);
    //FIXME
    //scrollview_refresh_content (psvdata);
    InvalidateRect (hWnd, NULL, TRUE);
}

/* -------------------------------------------------------------------------- */

void scrollview_draw (HWND hWnd, HDC hdc, PSVDATA psvdata)
{
    list_t *me;
    PSVITEMDATA pci;
    RECT rcDraw;
    int h = 0;
    RECT rcVis;
    PSVLIST psvlist = &psvdata->svlist;

    rcDraw.left = 0;
    rcDraw.top = 0;
    rcDraw.right = MAX(psvscr->nContWidth, psvscr->visibleWidth);
    rcDraw.bottom = MAX(psvscr->nContHeight, psvscr->visibleHeight);

    scrolled_content_to_window (psvscr, &rcDraw.left, &rcDraw.top);
    scrolled_content_to_window (psvscr, &rcDraw.right, &rcDraw.bottom);

    scrolled_get_visible_rect (psvscr, &rcVis);
    ClipRectIntersect (hdc, &rcVis);

    mglist_for_each (me, psvlist) {
        pci = (PSVITEMDATA)mglist_entry (me);
        rcDraw.top += h;
        rcDraw.bottom = rcDraw.top + pci->nItemHeight;
        if (rcDraw.bottom < rcVis.top) {
            h = pci->nItemHeight;
            continue;
        }
        if (rcDraw.top > rcVis.bottom)
            break;
        if (((MgList *)psvlist)->iop.drawItem && pci->nItemHeight > 0) {
            ((MgList *)psvlist)->iop.drawItem (hWnd, (HSVITEM)pci, hdc, &rcDraw);
        }
        h = pci->nItemHeight;
    }
}

/* adjust the position and size of the svlist window */
void scrollview_set_svlist (HWND hWnd, PSCRDATA pscrdata, BOOL visChanged)
{
    PSVDATA psvdata = (PSVDATA) GetWindowAdditionalData2 (hWnd);

    if (visChanged)
        InvalidateRect (hWnd, NULL, TRUE);
    else {
        //should be ScrollWindow ?
        //scrollview_refresh_content (psvdata);
        scrolled_refresh_view (&psvdata->scrdata);
    }
}

static int svlist_init (HWND hWnd, PSVLIST psvlist)
{
    mglist_init((MgList *)psvlist, hWnd);

    psvlist->nDefItemHeight = SV_DEF_ITEMHEIGHT;

    ((MgList *)psvlist)->iop.getRect = scrollview_get_item_rect;
    ((MgList *)psvlist)->iop.isInItem = isInItem;

    return 0;
}

/*
 * initialize scrollview internal structure
 */
static int svInitData (HWND hWnd, PSVDATA psvdata)
{
    RECT rcWnd;

    GetClientRect (hWnd, &rcWnd);
    scrolled_init (hWnd, psvscr, 
                    RECTW(rcWnd) - SV_LEFTMARGIN - SV_RIGHTMARGIN,
                    RECTH(rcWnd) - SV_TOPMARGIN - SV_BOTTOMMARGIN);
    scrolled_init_margins (psvscr, SV_LEFTMARGIN, SV_TOPMARGIN, 
                           SV_RIGHTMARGIN, SV_BOTTOMMARGIN);

    svlist_init (hWnd, &psvdata->svlist);

    psvscr->move_content = scrollview_set_svlist; 
    //psvdata->flags = 0;

    return 0;
}

/* 
 * shoulded be called before scrollview is used
 * hWnd: the scrolled window
 */
int scrollview_init (HWND hWnd, PSVDATA psvdata)
{
    if (!psvdata)
        return -1;

    SetWindowAdditionalData2 (hWnd, 0);
    //ShowScrollBar (hWnd, SB_HORZ, FALSE);
    //ShowScrollBar (hWnd, SB_VERT, FALSE);

    svInitData (hWnd, psvdata);
    SetWindowAdditionalData2 (hWnd, (DWORD) psvdata);

    /* set scrollbar status */
    scrolled_set_hscrollinfo (hWnd, psvscr);
    scrolled_set_vscrollinfo (hWnd, psvscr);

    return 0;
}

/*
 * destroy a scrollview
 */
void scrollview_destroy (PSVDATA psvdata)
{
    svlist_reset_content (hsvwnd, &psvdata->svlist);
}

/* --------------------------------------------------------------------------------- */
LRESULT ScrollViewCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PSVDATA psvdata = NULL;

    if (message != MSG_CREATE) {
        psvdata = (PSVDATA) GetWindowAdditionalData2 (hWnd);
        /* not used vars
        PSVLIST psvlist = NULL;
        psvlist = &psvdata->svlist;
        */
    }

    switch (message) {

    case MSG_CREATE:
    {
        psvdata = (PSVDATA) malloc(sizeof (SVDATA));
        if (!psvdata)
            return -1;
        scrollview_init (hWnd, psvdata);
        if (GetWindowStyle(hWnd) & SVS_AUTOSORT) {
            scrollview_set_autosort (psvdata);
        }
        break;
    }

    case MSG_DESTROY:
        scrollview_destroy (psvdata);
        free (psvdata);
        break;
    
    case MSG_GETDLGCODE:
        return DLGC_WANTARROWS;

    case MSG_KEYDOWN:
    {
        HSVITEM hsvi = 0, curHilighted;

        curHilighted = (HSVITEM) mglist_get_hilighted_item((MgList *)&psvdata->svlist);

        if (wParam == SCANCODE_CURSORBLOCKDOWN) {
            hsvi = scrollview_get_next_item(psvdata, curHilighted);
            if (GetWindowStyle(hWnd) & SVS_LOOP && !hsvi) {
                hsvi = scrollview_get_next_item(psvdata, 0);
            }
        }
        else if (wParam == SCANCODE_CURSORBLOCKUP) {
            hsvi = scrollview_get_prev_item(psvdata, curHilighted);
            if (GetWindowStyle(hWnd) & SVS_LOOP && !hsvi) {
                hsvi = scrollview_get_prev_item(psvdata, 0);
            }
        }
        else if (wParam == SCANCODE_HOME) {
            hsvi = scrollview_get_next_item(psvdata, 0);
        }
        else if (wParam == SCANCODE_END) {
            hsvi = scrollview_get_prev_item(psvdata, 0);
        }

        /* skip the invisible items */
        while ( hsvi && ((PSVITEMDATA)hsvi)->nItemHeight <= 0 ) {
            hsvi = (wParam == SCANCODE_CURSORBLOCKDOWN || wParam == SCANCODE_HOME) ? 
                   scrollview_get_next_item(psvdata, hsvi) : scrollview_get_prev_item(psvdata, hsvi);
        }

        if (hsvi) {
            if (hsvi != curHilighted) {
                NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), SVN_SELCHANGING, (DWORD)curHilighted);
                scrollview_hilight_item (psvdata, hsvi);
                NotifyParentEx (hWnd, GetDlgCtrlID(hWnd), SVN_SELCHANGED, (DWORD)hsvi);
            }
            scrollview_make_item_visible (psvdata, hsvi);
        }
        break;
    }
//FIXME, moved to listmode.c
    case MSG_PAINT:
    {
        HDC hdc = BeginPaint (hWnd);
        scrollview_draw (hWnd, hdc, psvdata);
        EndPaint (hWnd, hdc);
        return 0;
    }

    case SVM_ADDITEM:
    {
        int idx;
        HSVITEM hsvi;

        hsvi = scrollview_add_item (hWnd, psvdata, 0, (PSVITEMINFO)lParam, &idx);
        if (wParam)
            *(HSVITEM *)wParam = hsvi;
        return idx;
    }

    case SVM_DELITEM:
        scrollview_del_item (hWnd, psvdata, wParam, (HSVITEM)lParam);
        return 0;

    case SVM_RESETCONTENT:
        scrollview_reset_content (hWnd, psvdata);
        return 0;

    case SVM_SETITEMHEIGHT:
    {
        int nItem = (int)wParam;
        int new_h = (int)lParam;
        HSVITEM hsvi;

        hsvi = scrollview_get_item_by_index (psvdata, nItem);
        scrollview_set_item_height (hWnd, hsvi, new_h);
        return 0;
    }


    }/* end switch */

    //return DefaultScrolledProc (hWnd, message, wParam, lParam);
    return DefaultItemViewProc (hWnd, message, wParam, lParam, 
                                psvscr, (MgList *)&psvdata->svlist);
}

BOOL RegisterScrollViewControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_SCROLLVIEW;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = ScrollViewCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#endif /* _MGCTRL_SCROLLVIEW */

