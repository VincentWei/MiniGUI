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
** iconview.c: an iconview control
**
** Craeted By Zhong Shuyi at 2004/03/01.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

#ifdef _MGCTRL_ICONVIEW
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"

#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "ctrl/iconview.h"
#include "scrolled.h"
#include "listmodel.h"
#include "iconview_impl.h"


/* ------------------------------------ ivlist --------------------------- */

static void freeIvItem (IconviewItem *pivi)
{
    if (pivi->label)
        free (pivi->label);
    free (pivi);
}

/*
 * ivlistItemAdd : add an new icon item to the icon list
 */
static IconviewItem* 
ivlistItemAdd (IconviewList* pivlist, IconviewItem* preitem, 
            IconviewItem* nextitem, PIVITEMINFO pii, int *idx)
{
    IconviewItem* ci;
    int ret;

    ci = (IconviewItem*) calloc (1, sizeof(IconviewItem));
    if (!ci)
        return NULL;

    if (pii) {
        ((MgItem *)ci)->addData = pii->addData;
        ci->bmp = pii->bmp;
        if (pii->label)
            ci->label = strdup (pii->label);
    }

    ret = mglist_add_item ( (MgList *)pivlist, (MgItem *)ci, 
            (MgItem *)preitem, (MgItem *)nextitem, pii->nItem, idx);

    if (ret < 0) {
        freeIvItem (ci);
        return NULL;
    }

    return ci;
}

/*
 * ivlistItemDel : Deletes an icon item from the icon list
 */
static int ivlistItemDel (IconviewList* pivlist, IconviewItem* pci)
{
    mglist_del_item ((MgList *)pivlist, (MgItem *)pci);
    freeIvItem (pci);
    return 0;
}

/*
 * ivlist_reset_content : Clear icon list
 */
static void ivlist_reset_content (HWND hWnd, IconviewList* pivlist)
{
    IconviewItem* pci;

    while (!mglist_empty((MgList *)pivlist)) {
        pci = (IconviewItem*)mglist_first_item ((MgList *)pivlist);
        ivlistItemDel (pivlist, pci);
    }
    ((MgList *)pivlist)->nTotalItemH = 0;
}

/*
 * isInItem : Decides wheter an item is being clicked
 */
static int isInItem (MgList *mglst, int mouseX, int mouseY, 
                     MgItem** pRet, int *itemy)
{
    IconviewList *pivlist = (IconviewList *)mglst;
    IconviewItem* pci = NULL;
    int row, col, index;

    if (mouseY < 0 || mouseY >= mglst->nTotalItemH)
        return -1;

    if (mouseX < 0 || mouseX >= pivlist->nItemWidth * pivlist->nCol)
        return -1;

    row = mouseY / pivlist->nItemHeight;
    col = mouseX / pivlist->nItemWidth;

    index = row * pivlist->nCol + col;
    pci = (IconviewItem *)mglist_getitem_byindex (mglst, index);

    if (pRet)
        *pRet = (MgItem*)pci;

    return index;
}

int 
iconview_is_in_item (IconviewData* pivdata, 
        int mousex, int mousey, HITEM *phivi)
{
    return isInItem ((MgList *)&pivdata->ivlist, 
            mousex, mousey, (MgItem**)phivi, NULL);
}

/* --------------------------------------------------------------------------------- */

HITEM iconview_add_item (HWND hWnd, IconviewData* pivdata, HITEM prehivi,
                         PIVITEMINFO pii, int *idx)
{
    IconviewList* pivlist = &pivdata->ivlist;
    IconviewItem* pci;
    int index;

    if ((pci = 
        ivlistItemAdd (pivlist, (IconviewItem*)prehivi, NULL, pii, &index))) {
        int newh = 0;
        int count = mglist_get_item_count((MgList *)pivlist) ;

        if (pivlist->nCol == 1 
            || count % pivlist->nCol == 1)
            newh = pivlist->nItemHeight;

        if(count <=  pivlist->nCol) {
            scrolled_set_cont_width (hWnd, pivscr, 
                    count * pivdata->ivlist.nItemWidth);
        }
        mglist_adjust_items_height (hWnd, (MgList *)pivlist, pivscr, newh);
    }

    if (idx)
        *idx = index;

    return (HITEM)pci;
}

//FIXME
HITEM iconview_add_item_ex (HWND hWnd, IconviewData* pivdata, HITEM prehivi, 
                HITEM nexthivi, PIVITEMINFO pii, int *idx)
{
    IconviewList* pivlist = &pivdata->ivlist;
    IconviewItem* pci;
    int index;

    if ( (pci = ivlistItemAdd (pivlist, 
            (IconviewItem*)prehivi, (IconviewItem*)nexthivi, pii, &index))) {
        int count = mglist_get_item_count((MgList *)pivlist) ;

        if (count % pivlist->nCol == 1)
            mglist_adjust_items_height (hWnd, 
                (MgList *)pivlist, pivscr, pivlist->nItemHeight);
        if(count <=  pivlist->nCol) {
            scrolled_set_cont_width (hWnd, pivscr, 
                    count * pivdata->ivlist.nItemWidth);
        }
    }

    if (idx)
        *idx = index;

    return (HITEM)pci;
}

int iconview_move_item (IconviewData* pivdata, HITEM hivi, HITEM prehivi)
{
    if (!hivi)
        return -1;

    mglist_move_item ((MgList *)&pivdata->ivlist, 
            (MgItem *)hivi, (MgItem *)prehivi);
    scrolled_refresh_content (pivscr);
    return 0;
}

int 
iconview_del_item (HWND hWnd, IconviewData* pivdata, int nItem, HITEM hivi)
{
    IconviewList* pivlist = &pivdata->ivlist;
    IconviewItem* pci;

    if (hivi)
        pci = (IconviewItem*)hivi;
    else {
        if (!(pci =
            (IconviewItem*)mglist_getitem_byindex((MgList *)pivlist, nItem)))
            return -1;
    }

    if (ivlistItemDel (pivlist, pci) >= 0) {
        int delh = 0;
        int count = mglist_get_item_count((MgList *)pivlist) ;
        if (count % pivlist->nCol == 0)
            delh = pivlist->nItemHeight;

        if(count <=  pivlist->nCol) {
            scrolled_set_cont_width (hWnd, pivscr, 
                    count * pivdata->ivlist.nItemWidth);
        }
        mglist_adjust_items_height (hWnd, (MgList *)pivlist, pivscr, -delh);
    }
    return 0;
}

PBITMAP iconview_get_item_bitmap (HITEM hitem)
{
    return ((IconviewItem *)hitem)->bmp;
}

const char* iconview_get_item_label (HITEM hitem)
{
    return ((IconviewItem *)hitem)->label;
}

static void recalc_total_h (HWND hWnd, IconviewData *pivdata)
{
    int newh;
    int content;
    IconviewList* pivlist = &pivdata->ivlist;
    int oldh = ((MgList *)pivlist)->nTotalItemH;

    //pivdata->ivlist.nCol = pivscr->nContWidth / pivdata->ivlist.nItemWidth;
    pivdata->ivlist.nCol = MAX(pivscr->nContWidth, pivscr->visibleWidth) / pivdata->ivlist.nItemWidth;
    content = pivdata->ivlist.nCol * pivdata->ivlist.nItemWidth;
    scrolled_set_cont_width (hWnd, pivscr, content);

	//FIXED BY Dongjunjie
	if(pivdata->ivlist.nCol <= 0)
		pivdata->ivlist.nCol = 1;
    newh = (mglist_get_item_count ((MgList *)pivlist) + pivlist->nCol - 1)
           / pivlist->nCol * pivlist->nItemHeight;
    if (newh != oldh) {
        mglist_adjust_items_height (hWnd, 
            (MgList *)pivlist, pivscr, newh - oldh);
    }
}

static int 
iconview_get_item_rect (HWND hWnd, HITEM hivi, RECT *rcItem, BOOL bConv)
{
    IconviewData *pivdata = (IconviewData *)GetWindowAdditionalData2(hWnd);

    iconview_get_item_pos (pivdata, hivi, &rcItem->left, &rcItem->top);
    if (bConv)
        scrolled_content_to_window (pivscr, &rcItem->left, &rcItem->top);
    rcItem->bottom = rcItem->top + pivdata->ivlist.nItemHeight;
    rcItem->right = rcItem->left + pivdata->ivlist.nItemWidth;

    return 0;
}

int iconview_get_item_pos (IconviewData* pivdata, HITEM hivi, int *x, int *y)
{
    int index;
    IconviewList *pivlist = &pivdata->ivlist;

    index = mglist_get_item_index ((MgList *)pivlist, (MgItem *)hivi);
    if (index < 0)
        return -1;

    *y = (index / pivlist->nCol ) * pivlist->nItemHeight;
    *x = (index % pivlist->nCol ) * pivlist->nItemWidth;

    return 0;
}

int iconview_is_item_hilight (HWND hWnd, HITEM hivi)
{
    IconviewData* pivdata = (IconviewData*)GetWindowAdditionalData2(hWnd);
    return (int)mglist_is_item_hilight (
            (MgList *)&pivdata->ivlist, (MgItem *)hivi);
}

DWORD iconview_get_item_adddata (HITEM hivi)
{
    return mglist_get_item_adddata (hivi);
}

void iconview_reset_content (HWND hWnd, IconviewData* pivdata)
{
    /* delete all ivlist content */
    ivlist_reset_content (hivwnd, &pivdata->ivlist);

    if (pivdata->scrdata.sbPolicy != SB_POLICY_ALWAYS) {
        ShowScrollBar (hWnd, SB_HORZ, FALSE);
        ShowScrollBar (hWnd, SB_VERT, FALSE);
    }

    /* reset content and viewport size */
    scrolled_init_contsize (hWnd, &pivdata->scrdata);
    /* reset ivlist window */
    //iconview_set_ivlist (hWnd, &pivdata->scrdata);
    /* reset viewport window */
    scrolled_set_visible (hWnd, &pivdata->scrdata);

    scrolled_set_hscrollinfo (hWnd, &pivdata->scrdata);
    scrolled_set_vscrollinfo (hWnd, &pivdata->scrdata);

    //FIXME
    //scrolled_refresh_content (pivscr);
    InvalidateRect (hWnd, NULL, TRUE);
}

static void ivDrawItem (HWND hWnd, GHANDLE hivi, HDC hdc, RECT *rcDraw)
{
    IconviewItem *ivitem = (IconviewItem *)hivi;
    RECT rcTxt;
    int x, y;
    const WINDOWINFO* win_info = GetWindowInfo (hWnd);

    memset (&rcTxt, 0, sizeof(rcTxt));
    SetBkMode (hdc, BM_TRANSPARENT);

    if (iconview_is_item_hilight(hWnd, hivi)) {
        SetTextColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_HIGHLIGHT_ITEM));
        win_info->we_rdr->draw_hilite_item (hWnd, hdc, rcDraw, 
                GetWindowElementAttr (hWnd, WE_BGC_HIGHLIGHT_ITEM));
    }
    else {
        SetTextColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_WINDOW));
       // win_info->we_rdr->draw_normal_item (hWnd, hdc, rcDraw, 
       //         GetWindowElementAttr (hWnd, WE_BGC_WINDOW));
    }

    if (ivitem->label) {
        rcTxt = *rcDraw;
        rcTxt.top = rcTxt.bottom - GetWindowFont(hWnd)->size - 2;
        DrawText (hdc, ivitem->label, -1, 
                &rcTxt, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }

    if (ivitem->bmp) {
        int bmpw = 0, bmph = 0;

        x = rcDraw->left + (RECTWP(rcDraw) - (int)ivitem->bmp->bmWidth ) / 2; 
        if (x < rcDraw->left) {
            x = rcDraw->left;
            bmpw = RECTWP(rcDraw);
        }
        y = rcDraw->top 
            + ( RECTHP(rcDraw) - RECTH(rcTxt) - (int)ivitem->bmp->bmHeight) / 2;
        if (y < rcDraw->top) {
            y = rcDraw->top;
            bmph = RECTHP(rcDraw) - RECTH(rcTxt);
        }
        FillBoxWithBitmap (hdc, x, y, bmpw, bmph, ivitem->bmp);
    }
}

/* adjust the position and size of the ivlist window */
void iconview_set_ivlist (HWND hWnd, PSCRDATA pscrdata, BOOL visChanged)
{
    IconviewData* pivdata = (IconviewData*) GetWindowAdditionalData2 (hWnd);

    if (visChanged)
        InvalidateRect (hWnd, NULL, TRUE);
    else {
        //scrolled_refresh_content (pivscr);
        scrolled_refresh_view (pivscr);
    }
}

static int ivlist_init (HWND hWnd, IconviewList* pivlist)
{
    mglist_init((MgList *)pivlist, hWnd);

    pivlist->nItemWidth = IV_DEF_ITEMWIDTH;
    pivlist->nItemHeight = IV_DEF_ITEMHEIGHT;
    pivlist->nCol = 0;

    ((MgList *)pivlist)->iop.drawItem = ivDrawItem;
    ((MgList *)pivlist)->iop.getRect = iconview_get_item_rect;
    ((MgList *)pivlist)->iop.isInItem = isInItem;

    return 0;
}

/*
 * initialize iconview internal structure
 */
static int ivInitData (HWND hWnd, IconviewData* pivdata)
{
    RECT rcWnd;

    GetClientRect (hWnd, &rcWnd);
    scrolled_init (hWnd, &pivdata->scrdata, 
                    RECTW(rcWnd) - IV_LEFTMARGIN - IV_RIGHTMARGIN,
                    RECTH(rcWnd) - IV_TOPMARGIN - IV_BOTTOMMARGIN);

#ifdef __TARGET_MSTUDIO__
    pivdata->scrdata.sbPolicy = SB_POLICY_ALWAYS;
#endif

    scrolled_init_margins (pivscr, IV_LEFTMARGIN, IV_TOPMARGIN,
                           IV_RIGHTMARGIN, IV_BOTTOMMARGIN);

    ivlist_init (hWnd, &pivdata->ivlist);
    pivdata->ivlist.nCol = pivscr->nContWidth / pivdata->ivlist.nItemWidth;
	if(pivdata->ivlist.nCol <= 0)
		pivdata->ivlist.nCol = 1;

    pivdata->scrdata.move_content = iconview_set_ivlist; 
    pivdata->flags = 0;

    return 0;
}

/* 
 * shoulded be called before iconview is used
 * hWnd: the scrolled window
 */
//FIXME, to delete
int iconview_init (HWND hWnd, IconviewData* pivdata)
{
    if (!pivdata)
        return -1;

    SetWindowAdditionalData2 (hWnd, 0);
    ShowScrollBar (hWnd, SB_HORZ, FALSE);
    ShowScrollBar (hWnd, SB_VERT, FALSE);

    ivInitData (hWnd, pivdata);
    SetWindowAdditionalData2 (hWnd, (DWORD) pivdata);

    /* set scrollbar status */
    scrolled_set_hscrollinfo (hWnd, pivscr);
    scrolled_set_vscrollinfo (hWnd, pivscr);

    return 0;
}

/*
 * destroy a iconview
 */
void iconview_destroy (IconviewData* pivdata)
{
    ivlist_reset_content (hivwnd, &pivdata->ivlist);
}

static LRESULT IconViewCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    IconviewData* pivdata = NULL;
    IconviewList* pivlist = NULL;

    if (message != MSG_CREATE) {
        pivdata = (IconviewData*) GetWindowAdditionalData2 (hWnd);
        pivlist = &pivdata->ivlist;
    }

    switch (message) {

    case MSG_CREATE:
    {
        pivdata = (IconviewData*) malloc(sizeof (IconviewData));
        if (!pivdata)
            return -1;
        iconview_init (hWnd, pivdata);
        if (GetWindowStyle(hWnd) & IVS_AUTOSORT) {
            iconview_set_autosort (pivdata);
        }
        break;
    }

    case MSG_DESTROY:
        iconview_destroy (pivdata);
        free (pivdata);
        break;
    
    case MSG_GETDLGCODE:
        return DLGC_WANTARROWS;

    case MSG_KEYDOWN:
    {
        HITEM hivi = 0, curHilighted;
        int cursel;
        int count = mglist_get_item_count ((MgList *)&pivdata->ivlist);

        curHilighted = 
            (HITEM) mglist_get_hilighted_item((MgList *)&pivdata->ivlist);
        cursel = mglist_get_item_index (
                (MgList *)&pivdata->ivlist, (MgItem *)curHilighted);

        if (wParam == SCANCODE_CURSORBLOCKDOWN) {
            int next = cursel + pivdata->ivlist.nCol;
            if (GetWindowStyle(hWnd) & IVS_LOOP && next >= count) {
                next -= (count + pivdata->ivlist.nCol 
                        - count % pivdata->ivlist.nCol);
                if (next < 0) next += pivdata->ivlist.nCol;
            }
            hivi = (HITEM)mglist_getitem_byindex (
                    (MgList *)&pivdata->ivlist, next);
        }
        else if (wParam == SCANCODE_CURSORBLOCKUP) {
            int next = cursel - pivdata->ivlist.nCol;
            if (GetWindowStyle(hWnd) & IVS_LOOP && next < 0) {
                next += (count + pivdata->ivlist.nCol 
                        - count % pivdata->ivlist.nCol);
                if (next >= count) next -= pivdata->ivlist.nCol;
            }
            hivi = (HITEM)mglist_getitem_byindex (
                    (MgList *)&pivdata->ivlist, next);
        }
        else if (wParam == SCANCODE_CURSORBLOCKLEFT) {
            hivi = iconview_get_prev_item(pivdata, curHilighted);
            if (GetWindowStyle(hWnd) & IVS_LOOP && !hivi) {
                hivi = iconview_get_prev_item(pivdata, 0);
            }
        }
        else if (wParam == SCANCODE_CURSORBLOCKRIGHT) {
            hivi = iconview_get_next_item(pivdata, curHilighted);
            if (GetWindowStyle(hWnd) & IVS_LOOP && !hivi) {
                hivi = iconview_get_next_item(pivdata, 0);
            }
        }
        else if (wParam == SCANCODE_HOME) {
            hivi = iconview_get_next_item(pivdata, 0);
        }
        else if (wParam == SCANCODE_END) {
            hivi = iconview_get_prev_item(pivdata, 0);
        }

        if (hivi) {
            if (hivi != curHilighted) {
                mglist_hilight_item (
                    (MgList *)&pivdata->ivlist, (MgItem *)hivi);
                NotifyParentEx (hWnd, 
                    GetDlgCtrlID(hWnd), IVN_SELCHANGED, (DWORD)hivi);
            }
            mglist_make_item_visible ((MgList *)&pivdata->ivlist, pivscr, hivi);
        }
        break;
    }

    case MSG_PAINT:
    {
        HDC hdc = BeginPaint (hWnd);
        list_t *me;
        IconviewItem* pci;
        RECT rcAll, rcDraw;
        int i = 0;
        RECT rcVis;

        rcAll.left = 0;
        rcAll.top = 0;
        rcAll.right = pivscr->nContWidth;
        rcAll.bottom = pivscr->nContHeight;

        scrolled_content_to_window (pivscr, &rcAll.left, &rcAll.top);
        scrolled_content_to_window (pivscr, &rcAll.right, &rcAll.bottom);

        scrolled_get_visible_rect (pivscr, &rcVis);
        ClipRectIntersect (hdc, &rcVis);

        mglist_for_each (me, pivlist) {
            pci = (IconviewItem*)mglist_entry (me);

            rcDraw.left = rcAll.left + (i % pivlist->nCol) * pivlist->nItemWidth;
            rcDraw.right = rcDraw.left + pivlist->nItemWidth;
            rcDraw.top = rcAll.top + (i / pivlist->nCol) * pivlist->nItemHeight;
            rcDraw.bottom = rcDraw.top + pivlist->nItemHeight;

            if (rcDraw.bottom < rcVis.top) {
                i++;
                continue;
            }
            if (rcDraw.top > rcVis.bottom)
                break;

            if ( ((MgList *)pivlist)->iop.drawItem ) {
                ((MgList *)pivlist)->iop.drawItem (hWnd, (HITEM)pci, hdc, &rcDraw);
            }
            i++;
        }

        EndPaint (hWnd, hdc);
        return 0;
    }

    case MSG_SIZECHANGED:
        if (pivdata) {
            DefaultItemViewProc (hWnd, message, wParam, lParam, 
                                 pivscr, (MgList *)&pivdata->ivlist);
            recalc_total_h (hWnd, pivdata);
        }
        return 0;

    case IVM_ADDITEM:
    {
        int idx;
        HITEM hivi;

        hivi = iconview_add_item (hWnd, pivdata, 0, (PIVITEMINFO)lParam, &idx);
        if (wParam)
            *(HITEM *)wParam = hivi;
        return hivi?idx:-1;
    }

    case IVM_DELITEM:
        iconview_del_item (hWnd, pivdata, wParam, (HITEM)lParam);
        return 0;

    case IVM_RESETCONTENT:
        iconview_reset_content (hWnd, pivdata);
        return 0;

    case IVM_SETITEMSIZE:
    {
        
        if (wParam == 0) 
            pivdata->ivlist.nItemWidth = IV_DEF_ITEMWIDTH;
        else
            pivdata->ivlist.nItemWidth = wParam;
        
        if (lParam == 0)
            pivdata->ivlist.nItemHeight = IV_DEF_ITEMHEIGHT;
        else
            pivdata->ivlist.nItemHeight = lParam;
        
        pivdata->ivlist.nCol = pivscr->nContWidth / pivdata->ivlist.nItemWidth;
		if(pivdata->ivlist.nCol <= 0)
			pivdata->ivlist.nCol = 1;
        return 0;
    }
    case IVM_GETFIRSTVISIBLEITEM:
    {
        int i = 0;
        int top = 0, bottom = 0; 
        list_t *me;

        mglist_for_each (me, pivlist) {
            top = (i / pivlist->nCol) * pivlist->nItemHeight;
            bottom = top + pivlist->nItemHeight;
            scrolled_content_to_visible (pivscr, NULL, &bottom);

            if (bottom < 0)
                i++;
            else
                break;
        }

        return i;
    }

    }/* end switch */

    return DefaultItemViewProc (hWnd, message, wParam, lParam, 
                                pivscr, (MgList *)&pivdata->ivlist);
}

BOOL RegisterIconViewControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_ICONVIEW;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = IconViewCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

///////////////////////////
// Add by dongjunjie
GHANDLE iconview_get_item(HWND hwnd, int index)
{
	IconviewData* pivdata = (IconviewData*)GetWindowAdditionalData2(hwnd);
	return (GHANDLE)mglist_getitem_byindex((MgList*)&pivdata->ivlist, index);
}

BOOL iconview_set_item_lable(GHANDLE hivi, const char* strLabel)
{
	IconviewItem * iitem;

	if (hivi == 0 || strLabel == NULL) {
		return FALSE;
	}

	iitem = (IconviewItem*)hivi;
	if(iitem->label)
		free(iitem->label);
	iitem->label = strdup(strLabel);
	return TRUE;
}

PBITMAP iconview_set_item_bitmap(GHANDLE hivi, PBITMAP pbmp)
{
	IconviewItem * iitem;
	PBITMAP pold;
	if (hivi == 0)
		return NULL;

	iitem = (IconviewItem*)hivi;
	pold = iitem->bmp;
	iitem->bmp = pbmp;
	return pold;
}


#endif /* _MGCTRL_ICONVIEW */

