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
** scrolled.c: basic scrolled window support
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

#include "scrolled.h"


#define SV_H_OUTRANGE() \
            (pscrdata->nContWidth > pscrdata->visibleWidth)
                
#define SV_V_OUTRANGE() \
            (pscrdata->nContHeight > pscrdata->visibleHeight)
                
#define svHScroll(scrollval, newpos) \
            svScroll(hWnd, pscrdata, scrollval, newpos, TRUE);

#define svVScroll(scrollval, newpos) \
            svScroll (hWnd, pscrdata, scrollval, newpos, FALSE);

/* ---------------------------------------------------------------------------- */

/* adjust the horz scrollbar according to the content offset value */
int scrolled_set_hscrollinfo (HWND hWnd, PSCRDATA pscrdata)
{
    SCROLLINFO si;

    if (pscrdata->sbPolicy == SB_POLICY_NEVER) {
        ShowScrollBar (hWnd, SB_HORZ, FALSE);
        return 0;
    }
    else if (pscrdata->sbPolicy == SB_POLICY_AUTOMATIC) {
        if (!SV_H_OUTRANGE()) {
            ShowScrollBar (hWnd, SB_HORZ, FALSE);
            return 0;
        }
    }

    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = pscrdata->nContWidth - 1;
    si.nPage = pscrdata->visibleWidth;
    si.nPos = pscrdata->nContX;

    SetScrollInfo (hWnd, SB_HORZ, &si, TRUE);
    ShowScrollBar (hWnd, SB_HORZ, TRUE);

    return 0;
}

/* adjust the vert scrollbar according to the content offset value */
int scrolled_set_vscrollinfo (HWND hWnd, PSCRDATA pscrdata)
{
    SCROLLINFO si;

    if (pscrdata->sbPolicy == SB_POLICY_NEVER) {
        ShowScrollBar (hWnd, SB_VERT, FALSE);
        return 0;
    }
    else if (pscrdata->sbPolicy == SB_POLICY_AUTOMATIC) {
        if (!SV_V_OUTRANGE()) {
            ShowScrollBar (hWnd, SB_VERT, FALSE);
            return 0;
        }
    }

    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = pscrdata->nContHeight - 1;
    si.nPage = pscrdata->visibleHeight;
    si.nPos = pscrdata->nContY;

    SetScrollInfo (hWnd, SB_VERT, &si, TRUE);
    ShowScrollBar (hWnd, SB_VERT, TRUE);

    return 0;
}

/* set content position */
BOOL scrolled_set_cont_pos (HWND hWnd, PSCRDATA pscrdata, int cont_x, int cont_y)
{
    BOOL bChange = FALSE;
    int cont_w, cont_h;

    cont_w = pscrdata->nContWidth - pscrdata->visibleWidth;
    cont_h = pscrdata->nContHeight - pscrdata->visibleHeight;
    
    if (cont_x > cont_w && cont_w > 0)
        cont_x = cont_w;

    if (cont_y > cont_h && cont_h > 0)
        cont_y = cont_h;

    if (cont_x >= 0 && cont_x <= cont_w) {
        //move content
        pscrdata->prevnContX = pscrdata->nContX;
        pscrdata->nContX = cont_x;
        scrolled_set_content (hWnd, pscrdata, FALSE);
        pscrdata->prevnContX = pscrdata->nContX;
        SetScrollPos (hWnd, SB_HORZ, cont_x);
        bChange = TRUE;
    }
    if (cont_y >= 0 && cont_y <= cont_h) {
        //move content
        pscrdata->prevnContY = pscrdata->nContY;
        pscrdata->nContY = cont_y;
        scrolled_set_content (hWnd, pscrdata, FALSE);
        pscrdata->prevnContY = pscrdata->nContY;
        SetScrollPos (hWnd, SB_VERT, cont_y);
        bChange = TRUE;
    }
    return bChange;
}

/* make a point in the content area visible */
BOOL scrolled_make_pos_visible (HWND hWnd, PSCRDATA pscrdata, int pos_x, int pos_y)
{
    int cont_x = -1, cont_y = -1;

    if (pos_x >= 0) {
        if (pos_x >= pscrdata->nContWidth)
            pos_x = pscrdata->nContWidth - 1;
        if (pos_x < pscrdata->nContX)
            cont_x = pos_x;
        else if (pos_x > pscrdata->nContX + pscrdata->visibleWidth - 1)
            cont_x = pos_x - pscrdata->visibleWidth + 1;
    }
    if (pos_y >= 0) {
        if (pos_y >= pscrdata->nContHeight)
            pos_y = pscrdata->nContHeight - 1;
        if (pos_y < pscrdata->nContY)
            cont_y = pos_y;
        else if (pos_y > pscrdata->nContY + pscrdata->visibleHeight - 1)
            cont_y = pos_y - pscrdata->visibleHeight + 1;
    }
    return scrolled_set_cont_pos (hWnd, pscrdata, cont_x, cont_y);
}

/* scrollbar action */
static void svScroll (HWND hWnd, PSCRDATA pscrdata, int scrollval, int newpos, BOOL bHScroll)
{
    int scrollBoundMax;
    int scrollBoundMin;
    int scroll = 0;
    BOOL bScroll = FALSE;
    int nOffset;
    
    if (bHScroll) {
        scrollBoundMax = pscrdata->nContWidth - pscrdata->visibleWidth;
        nOffset = pscrdata->nContX;
    }
    else {
        scrollBoundMax = pscrdata->nContHeight - pscrdata->visibleHeight;
        nOffset = pscrdata->nContY;
    }
    scrollBoundMin = 0;

    if (newpos >= 0) { // SB_THUMBTRACK
        scrollval = newpos - nOffset;
    }

    scroll = (scrollval>0?scrollval:-scrollval);

    if (scrollval > 0 && nOffset < scrollBoundMax) {
        if ((nOffset + scroll) > scrollBoundMax)
            nOffset = scrollBoundMax;
        else
            nOffset += scroll;
        bScroll = TRUE;
    }
    else if ( scrollval < 0 && nOffset > scrollBoundMin) {
        if ((nOffset - scroll) < scrollBoundMin)
            nOffset = scrollBoundMin;
        else
            nOffset -= scroll;
        bScroll = TRUE;
    }
    if (bScroll) {
        scrolled_set_cont_pos (hWnd, pscrdata, (bHScroll?nOffset:-1), (bHScroll?-1:nOffset));
    }
}

/* set the position and size of the viewport window */
void scrolled_set_visible (HWND hWnd, PSCRDATA pscrdata)
{
    /* maybe we need to reset content width and height */
    if (pscrdata->nContWidth < pscrdata->visibleWidth) {
    /* houhh 20101012, do not need to reset nContWidth here, Because the content
     * width should be modify by add content.*/
#if 0
        pscrdata->nContWidth = pscrdata->visibleWidth;
#endif
		pscrdata->nContX = 0;
		pscrdata->prevnContX = 0;
    }
	else {
		if(pscrdata->nContX > pscrdata->nContWidth - pscrdata->visibleWidth
                &&(pscrdata->nContWidth - pscrdata->visibleWidth) > 0 ) {
            pscrdata->prevnContX = pscrdata->nContX;
			pscrdata->nContX = pscrdata->nContWidth - pscrdata->visibleWidth;
        }
	}

    if (pscrdata->nContHeight < pscrdata->visibleHeight) {
#if 0
        pscrdata->nContHeight = pscrdata->visibleHeight;
#endif
		pscrdata->nContY = 0;
		pscrdata->prevnContY = 0;
    }
	else{
		if(pscrdata->nContY > pscrdata->nContHeight - pscrdata->visibleHeight
                && (pscrdata->nContHeight - pscrdata->visibleHeight) > 0) {
			pscrdata->nContY = pscrdata->nContHeight - pscrdata->visibleHeight;
            pscrdata->prevnContY = pscrdata->nContY;
        }
	}
    /* refresh the whole window as long as the visible area is changed */
    scrolled_set_content (hWnd, pscrdata, TRUE);
    pscrdata->prevnContX = pscrdata->nContX;
    pscrdata->prevnContY = pscrdata->nContY;
}

int scrolled_visible_to_content (PSCRDATA pscrdata, int *x, int *y)
{
    if (x)
        *x = *x + pscrdata->nContX;
    if (y)
        *y = *y + pscrdata->nContY;

    return 0;
}

int scrolled_content_to_visible (PSCRDATA pscrdata, int *x, int *y)
{
    if (x)
        *x = *x - pscrdata->nContX;

    if (y)
        *y = *y - pscrdata->nContY;

    if (!x || !y)
        return -1;

    if (*x < 0 || *x > pscrdata->visibleWidth ||
               *y < 0 || *y > pscrdata->visibleHeight)
        return -1;
    return 0;
}

/* recalc visible area and container area, then set scrollbar */
void scrolled_recalc_areas (HWND hWnd, PSCRDATA pscrdata, int new_w, int new_h)
{
    if (!pscrdata || new_w <= 0 || new_h <= 0)
        return;

    /* reset viewport as client - margin */
    pscrdata->visibleWidth = new_w;
    pscrdata->visibleHeight = new_h;
    scrolled_set_visible (hWnd, pscrdata);

/*    
    int old_vw, old_vh;

    old_vw = pscrdata->visibleWidth;
    old_vh = pscrdata->visibleHeight;

    if (pscrdata->nContWidth == old_vw) {
        pscrdata->nContWidth = pscrdata->visibleWidth;
        //should refresh the whole window
        scrolled_set_content (hWnd, pscrdata, TRUE);
    }
    if (pscrdata->nContHeight == old_vh) {
        pscrdata->nContHeight = pscrdata->visibleHeight;
        //should refresh the whole window
        scrolled_set_content (hWnd, pscrdata, TRUE);
    }
 */
    scrolled_set_hscrollinfo (hWnd, pscrdata);
    scrolled_set_vscrollinfo (hWnd, pscrdata);
}

void scrolled_hscroll (HWND hWnd, PSCRDATA pscrdata, WPARAM wParam, LPARAM lParam)
{
    int hscroll = 0, newpos = -1;

    if (wParam == SB_LINERIGHT)
        hscroll = pscrdata->hScrollVal;
    else if (wParam == SB_LINELEFT)
        hscroll = -pscrdata->hScrollVal;
    else if (wParam == SB_PAGERIGHT)
        hscroll = pscrdata->hScrollPageVal;
    else if (wParam == SB_PAGELEFT)
        hscroll = -pscrdata->hScrollPageVal;
    else if (wParam == SB_THUMBTRACK)
        newpos = lParam;
    else if (wParam == SB_THUMBPOSITION) {
        newpos = lParam;
    }

    svHScroll (hscroll, newpos);
}

void scrolled_vscroll (HWND hWnd, PSCRDATA pscrdata, WPARAM wParam, LPARAM lParam)
{
    int vscroll = 0, newpos = -1;

    if (wParam == SB_LINEDOWN)
        vscroll = pscrdata->vScrollVal;
    else if (wParam == SB_LINEUP)
        vscroll = -pscrdata->vScrollVal;
    else if (wParam == SB_PAGEDOWN)
        vscroll = pscrdata->vScrollPageVal;
    else if (wParam == SB_PAGEUP)
        vscroll = -pscrdata->vScrollPageVal;
    else if (wParam == SB_THUMBTRACK) {
        newpos = lParam;
    }
    else if (wParam == SB_THUMBPOSITION) {
        newpos = lParam;
    }

    if (wParam != SB_THUMBPOSITION) 
        svVScroll (vscroll, newpos);
}

int scrolled_set_cont_width (HWND hWnd, PSCRDATA pscrdata, int cont_w)
{
    int oldval = pscrdata->nContWidth;

    if (cont_w < 0)
        return 0;
    //pscrdata->nContWidth = cont_w < pscrdata->visibleWidth ? pscrdata->visibleWidth : cont_w;
    pscrdata->nContWidth = cont_w;

    if (pscrdata->nContWidth == oldval)
        return 0;

    if (pscrdata->nContX > pscrdata->nContWidth - pscrdata->visibleWidth
            && (pscrdata->nContWidth - pscrdata->visibleWidth) > 0) {
        pscrdata->prevnContX = pscrdata->nContX;
        pscrdata->nContX = pscrdata->nContWidth - pscrdata->visibleWidth;
    }
    else if(pscrdata->nContWidth <= pscrdata->visibleWidth) {
        pscrdata->prevnContX = pscrdata->nContX;
        pscrdata->nContX = 0;
    }

    scrolled_set_hscrollinfo (hWnd, pscrdata);
    //move content
    scrolled_set_content (hWnd, pscrdata, FALSE);
    pscrdata->prevnContX = pscrdata->nContX;

    return pscrdata->nContWidth;
}

int scrolled_set_cont_height (HWND hWnd, PSCRDATA pscrdata, int cont_h)
{
    int oldval = pscrdata->nContHeight;

    if (cont_h < 0)
        return 0;

    //pscrdata->nContHeight = cont_h < pscrdata->visibleHeight ? pscrdata->visibleHeight : cont_h;
    pscrdata->nContHeight = cont_h;
    if (pscrdata->nContHeight == oldval)
        return 0;

    if (pscrdata->nContY > pscrdata->nContHeight - pscrdata->visibleHeight
            && (pscrdata->nContHeight - pscrdata->visibleHeight) > 0) {
        pscrdata->prevnContY = pscrdata->nContY;
        pscrdata->nContY = pscrdata->nContHeight - pscrdata->visibleHeight;
    }
    else if (pscrdata->nContHeight <= pscrdata->visibleHeight) {
        pscrdata->prevnContY = pscrdata->nContY;
        pscrdata->nContY = 0;
    }

    scrolled_set_vscrollinfo (hWnd, pscrdata);
    //move content
    scrolled_set_content (hWnd, pscrdata, FALSE);
    pscrdata->prevnContY = pscrdata->nContY;

    return pscrdata->nContHeight;
}

/*
int scrolled_SetVisibleWidth (HWND hWnd, PSCRDATA pscrdata, int vis_w)
{
    int oldval = pscrdata->visibleWidth;

    if (vis_w < 0)
        return 0;

    pscrdata->visibleWidth = vis_w < pscrdata->nContWidth ? vis_w : pscrdata->nContWidth;
    if (pscrdata->visibleWidth = oldval)
        return 0;

    scrolled_set_hscrollinfo (hWnd, pscrdata);

    return pscrdata->visibleWidth;
}
*/

void scrolled_set_scrollval (PSCRDATA pscrdata, int hval, int vval)
{
    if (hval > 0)
        pscrdata->hScrollVal = hval;
    if (vval > 0)
        pscrdata->vScrollVal = vval;
}

void scrolled_set_scrollpageval (PSCRDATA pscrdata, int hval, int vval)
{
    if (hval > 0)
        pscrdata->hScrollPageVal = hval;
    if (vval > 0)
        pscrdata->vScrollPageVal = vval;
}

void scrolled_set_margins (PSCRDATA pscrdata, RECT *rc)
{
    RECT rcWnd;

    if (!rc)
        return;

    if (rc->left >= 0)
        pscrdata->leftMargin = rc->left;
    if (rc->right >= 0)
        pscrdata->rightMargin = rc->right;
    if (rc->top >= 0)
        pscrdata->topMargin = rc->top;
    if (rc->bottom >= 0)
        pscrdata->bottomMargin = rc->bottom;

    GetClientRect (pscrdata->hSV, &rcWnd);
    scrolled_recalc_areas (pscrdata->hSV, pscrdata,
        RECTW(rcWnd) - pscrdata->leftMargin - pscrdata->rightMargin,
        RECTH(rcWnd) - pscrdata->topMargin - pscrdata->bottomMargin);
}

void scrolled_refresh_rect (PSCRDATA pscrdata, RECT *rc)
{
    RECT rcUpdate;

    memcpy (&rcUpdate, rc, sizeof(RECT));

    scrolled_content_to_window (pscrdata, NULL, &rcUpdate.top);
    rcUpdate.bottom = rcUpdate.top + RECTHP(rc);

    if (rcUpdate.right == 0) {
        rcUpdate.left = 0;
        scrolled_content_to_window (pscrdata, &rcUpdate.left, NULL);
        if (rcUpdate.left < 0)
            rcUpdate.right = rcUpdate.left + pscrdata->nContWidth;
        else
            rcUpdate.right = rcUpdate.left + pscrdata->visibleWidth;
    }

    InvalidateRect ( pscrdata->hSV, &rcUpdate, TRUE);
}

void scrolled_init_contsize (HWND hWnd, PSCRDATA pscrdata)
{
    pscrdata->nContX = 0;
    pscrdata->nContY = 0;
    pscrdata->prevnContX = 0;
    pscrdata->prevnContY = 0;

    pscrdata->nContWidth = pscrdata->visibleWidth;
    pscrdata->nContHeight = pscrdata->visibleHeight;
}

void scrolled_init_margins (PSCRDATA pscrdata, int left, int top, 
                            int right, int bottom)
{
    pscrdata->leftMargin = left;
    pscrdata->topMargin = top;
    pscrdata->rightMargin = right;
    pscrdata->bottomMargin = bottom;
}

/* set initial content size and viewport size */
int scrolled_init (HWND hWnd, PSCRDATA pscrdata, int w, int h)
{
    RECT rcWnd;

    /* new added */
    pscrdata->leftMargin    = 0;
    pscrdata->topMargin     = 0;
    pscrdata->rightMargin   = 0;
    pscrdata->bottomMargin  = 0;

    pscrdata->nContX = 0;
    pscrdata->nContY = 0;
    pscrdata->prevnContX = 0;
    pscrdata->prevnContY = 0;

    if (w > 0 && h > 0) {
        pscrdata->visibleWidth = w;
        pscrdata->visibleHeight = h;
    }
    else {
        GetClientRect (hWnd, &rcWnd);
        pscrdata->visibleWidth = RECTW(rcWnd);
        pscrdata->visibleHeight = RECTH(rcWnd);
    }

    pscrdata->nContWidth = pscrdata->visibleWidth; 
    pscrdata->nContHeight = pscrdata->visibleHeight; 

    pscrdata->sbPolicy = SB_POLICY_AUTOMATIC;

    pscrdata->hScrollVal = HSCROLL;
    pscrdata->vScrollVal = VSCROLL;
    pscrdata->hScrollPageVal = pscrdata->visibleWidth;
    pscrdata->vScrollPageVal = pscrdata->visibleHeight;

    pscrdata->hSV = hWnd;

    return 0;
}

int DefaultScrolledProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    PSCRDATA pscrdata = NULL;

    if (message != MSG_CREATE)
        pscrdata = (PSCRDATA) GetWindowAdditionalData2 (hWnd);

    switch (message) {

    case MSG_HSCROLL:
        scrolled_hscroll (hWnd, pscrdata, wParam, lParam);
        break;

    case MSG_VSCROLL:
        scrolled_vscroll (hWnd, pscrdata, wParam, lParam);
        break;

    /* 
     * three cases: 
     * 1. move window; 2. set content range; 3. show scrollbar
     */
    case MSG_SIZECHANGED:
    {
        RECT *rcClient = (RECT*)lParam;

        //default case : the whole client area is the visible area
        //scrollwnd control is this case
        //scrollview control is different

        if (!pscrdata)
            return 0;

        //scrolled_recalc_areas (hWnd, pscrdata, RECTWP(rcClient), RECTHP(rcClient));
        scrolled_recalc_areas (hWnd, pscrdata, 
                        RECTWP(rcClient) - pscrdata->leftMargin - pscrdata->rightMargin,
                        RECTHP(rcClient) - pscrdata->topMargin - pscrdata->bottomMargin);
        return 0;
    }

    case MSG_KEYDOWN:
    {
        if (wParam == SCANCODE_PAGEDOWN) {
            SendMessage (hWnd, MSG_VSCROLL, SB_PAGEDOWN, 0);
        }
        else if (wParam == SCANCODE_PAGEUP)
            SendMessage (hWnd, MSG_VSCROLL, SB_PAGEUP, 0);
        else if (wParam == SCANCODE_CURSORBLOCKLEFT)
            SendMessage (hWnd, MSG_HSCROLL, SB_PAGELEFT, 0);
        else if (wParam == SCANCODE_CURSORBLOCKRIGHT)
            SendMessage (hWnd, MSG_HSCROLL, SB_PAGERIGHT, 0);
        break;
    }

    case SVM_SETCONTRANGE:
        scrolled_set_cont_width (hWnd, pscrdata, wParam);
        scrolled_set_cont_height (hWnd, pscrdata, lParam);
        return 0;

    case SVM_SETCONTWIDTH:
        return scrolled_set_cont_width (hWnd, pscrdata, wParam);

    case SVM_SETCONTHEIGHT:
        return scrolled_set_cont_height (hWnd, pscrdata, wParam);

    case SVM_GETHSCROLLVAL:
        return pscrdata->hScrollVal;
    case SVM_GETVSCROLLVAL:
        return pscrdata->vScrollVal;
    case SVM_GETHSCROLLPAGEVAL:
        return pscrdata->hScrollPageVal;
    case SVM_GETVSCROLLPAGEVAL:
        return pscrdata->vScrollPageVal;

    case SVM_SETSCROLLVAL:
    {
        scrolled_set_scrollval (pscrdata, wParam, lParam);
        return 0;
    }
    case SVM_SETSCROLLPAGEVAL:
    {
        scrolled_set_scrollpageval (pscrdata, wParam, lParam);
        return 0;
    }

    case SVM_SETCONTPOS:
    {
        scrolled_set_cont_pos (hWnd, pscrdata, wParam, lParam);
        return 0;
    }

    /* get scrolled size information */
    case SVM_GETCONTWIDTH:
        return pscrdata->nContWidth;
    case SVM_GETCONTHEIGHT:
        return pscrdata->nContHeight;
    case SVM_GETCONTENTX:
        return pscrdata->nContX;
    case SVM_GETCONTENTY:
        return pscrdata->nContY;
    case SVM_GETVISIBLEWIDTH:
        return pscrdata->visibleWidth;
    case SVM_GETVISIBLEHEIGHT:
        return pscrdata->visibleHeight;

    case SVM_GETMARGINS:
    {
        RECT *rc = (RECT *)lParam;
        if (rc)
            scrolled_get_margins (pscrdata, rc);
        return 0;
    }

    case SVM_GETLEFTMARGIN:
        return pscrdata->leftMargin;
    case SVM_GETRIGHTMARGIN:
        return pscrdata->rightMargin;
    case SVM_GETTOPMARGIN:
        return pscrdata->topMargin;
    case SVM_GETBOTTOMMARGIN:
        return pscrdata->bottomMargin;

    case SVM_SETMARGINS:
    {
        RECT *rc = (RECT *)lParam;
        if (rc)
            scrolled_set_margins (pscrdata, rc);
        return 0;
    }

    case SVM_MAKEPOSVISIBLE:
        scrolled_make_pos_visible (hWnd, pscrdata, wParam, lParam);
        return 0;

    }/* end switch */

    return DefaultControlProc (hWnd, message, wParam, lParam);
}


#endif /* _MGCTRL_SCROLLVIEW */
