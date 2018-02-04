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
** propsheet.c: the Property Sheet (Tab) control.
**
** Create date: 2001/11/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"

#ifdef _MGCTRL_PROPSHEET
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/static.h"
#include "ctrl/button.h"
#include "ctrl/propsheet.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "propsheet_impl.h"
#include "ctrlmisc.h"
    
/*for tmp usage*/
#define MWM_ICONX               6
#define MWM_ICONY               7
#define MWM_SB_WIDTH            23

#define WEC_TAB_DARKER          30
#define WEC_TAB_DARKEST            31  
#define WEC_TAB_LIGHT           32
#define TAB_MARGIN 8

static LRESULT
PropSheetCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL RegisterPropSheetControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_PROPSHEET;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = 
    GetWindowElementPixel (HWND_NULL, WE_MAINC_THREED_BODY);
    WndClass.WinProc     = PropSheetCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

/**************************internal functions******************************/

#define _ICON_OFFSET 2
#define _GAP_ICON_TEXT 2
#define _MIN_TAB_WIDTH  10

#define LEFT_SCROLL_BTN_CLICKED  1
#define RIGHT_SCROLL_BTN_CLICKED 2
#define NO_SCROLL_BTN_CLICKED 3

/* 
 * get_metrics:
 *
 * This function get metrics of window elements which is identified by id. 
 *
 * Author: Peng LIU
 * Date: 2007-12-05
 */
static int get_metrics (int id)
{
    return GetSysCharHeight();
}

static int set_page_title (PROPPAGE * page, char *ptr)
{
    int len = 0;
    
    if (page->title) {
        FreeFixStr (page->title);
    }
    if (ptr) {
        len = strlen (ptr);
        if (!(page->title = FixStrAlloc (len))) {
            return PS_ERR;
        }
        strcpy (page->title, ptr);
    } else {
        page->title = NULL;
    }
    return PS_OKAY;
}


static void show_hide_page (PPROPPAGE page, int show_cmd)
{
    HWND focus;
    
    ShowWindow (page->hwnd, show_cmd);

    focus = GetNextDlgTabItem (page->hwnd, (HWND)0, 0);
    if (SendMessage (page->hwnd, MSG_SHOWPAGE, (WPARAM)focus, show_cmd) && 
        show_cmd == SW_SHOW) {
        if (focus) {
            SetFocus (focus);
        }
    }
}


static PPROPPAGE get_left_page (PPROPSHEETDATA propsheet, PPROPPAGE page)
{
    PPROPPAGE left;
    left = propsheet->head;
    
    if (page == left) {
        return NULL;
    }
    while (left->next) {
        if (left->next == page) {
            return left;
        }
        left = left->next;
    }
    return NULL;
}

/* recalculate widths of the tabs after a page added or removed. */
static void 
recalc_tab_widths (HWND hwnd, PPROPSHEETDATA propsheet, DWORD dwStyle)
{
    PPROPPAGE page;

    if (propsheet->page_count == 0)
        return;

    if (dwStyle & PSS_COMPACTTAB) {
        
        HDC hdc;
        SIZE ext;
        int total_width = 0;

        hdc = GetClientDC (hwnd);
        page = propsheet->head;
        while (page) {
            page->width = 0;
            if (page->title) {
                GetTextExtent (hdc, page->title, -1, &ext);
                page->width = ext.cx + TAB_MARGIN;
            }
            if (page->icon) {
                page->width += get_metrics (MWM_ICONX);
            }
            if (page->width < _MIN_TAB_WIDTH) {
                page->width = _MIN_TAB_WIDTH;
            }
            total_width += page->width;
            /*added by lfzhang*/
            if (total_width > propsheet->head_rc.right){
                total_width -= page->width;
                page->width = propsheet->head_rc.right - total_width;
                total_width += page->width;
                break;
            }
            page = page->next;
        }
        ReleaseDC (hdc);
        /* old_code
        if (total_width > propsheet->head_rc.right) {
            int new_width = 0, step = 0;
            page = propsheet->head;

            step = (total_width - propsheet->head_rc.right)/propsheet->page_count;
            if (step == 0)
                step = 1;

            while (page) {
                page->width -= step;
                if (page->width < _MIN_TAB_WIDTH) {
                    page->width = _MIN_TAB_WIDTH;
                }

                if (!(page->next)
                    && (new_width + page->width > propsheet->head_rc.right)) {
                    page->width = propsheet->head_rc.right - new_width;
                }
                else
                    new_width += page->width;

                page = page->next;
            }
            total_width = new_width;
        }*/
        propsheet->head_width = total_width;
    
    } else {
    
        int width;

        //width = propsheet->head_rc.right * 8 / (propsheet->page_count * 10);
        width = propsheet->head_rc.right / propsheet->page_count;
        //if (width < _MIN_TAB_WIDTH)
            //width = propsheet->head_rc.right / propsheet->page_count;

        page = propsheet->head;
        while (page) {
            page->width = width;
            page = page->next;
        }
        //propsheet->head_width = width * propsheet->page_count;
        propsheet->head_width = propsheet->head_rc.right;
    }
}



/* notice!!!
   This is the core function of propsheet normal style !!
   and it is run very slow, because propsheet 
   is designed by single-link-list */
static void update_propsheet (PPROPSHEETDATA propsheet)
{
    PPROPPAGE temp, page;
    int tab_width = 0;
    
    if (propsheet->first_display_page == NULL) {
        propsheet->first_display_page = propsheet->head;
    }
    page = propsheet->head;
    /*test the tabs are overload ?*/
    propsheet->overload = FALSE;
    while (page) {
        tab_width += page->width;

        if (tab_width > 
            (propsheet->head_rc.right - get_metrics (MWM_SB_WIDTH) * 2)) {
            propsheet->overload = TRUE;
            propsheet->head_width = propsheet->head_rc.right - 
              get_metrics (MWM_SB_WIDTH) * 2;
            break;
        }

        page = page->next;
    }
 
    if (propsheet->overload == FALSE) {
        propsheet->first_display_page = propsheet->head;
    }
   
    /* test if active is right of first_display_page */

    if (! propsheet->active)
        return;
    page = propsheet->active->next;

    while (page) {
        if (page == propsheet->first_display_page) {
            propsheet->first_display_page = propsheet->active;
            break;
        }
        page = page->next;
    }

    /* calc the first-display-page */
    page = propsheet->first_display_page;
    while (page) {
        temp = page;
        tab_width = 0;
        while (temp) {
            tab_width += temp->width;
            if (temp == propsheet->active) {
                if (tab_width <= propsheet->head_width) {
                    return;
                }
            }
            temp = temp->next;
        }
        propsheet->first_display_page = propsheet->first_display_page->next;
        page = page->next;
    }
}



static void 
scroll_tab_right (HWND hwnd, PCONTROL ctrl, PPROPSHEETDATA propsheet)
{
    if (propsheet->active->next == NULL) {
        return;
    }
    show_hide_page (propsheet->active, SW_HIDE);
    propsheet->active = propsheet->active->next;
    NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED); 
    update_propsheet (propsheet);
    show_hide_page (propsheet->active, SW_SHOW);
}

static void scroll_tab_left (HWND hwnd, PCONTROL ctrl, PPROPSHEETDATA propsheet)
{
    /* not used vars
    PPROPPAGE page;
    page = propsheet->first_display_page;
    */

    if (propsheet->active == propsheet->head) {
        return;
    }
    show_hide_page (propsheet->active, SW_HIDE);
    propsheet->active = get_left_page (propsheet, propsheet->active); 
    if (propsheet->active == NULL) {
        propsheet->active = propsheet->head;
    }
    NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED); 
    update_propsheet (propsheet);
    show_hide_page (propsheet->active, SW_SHOW); 
}



/* destroy a page */
static void destroy_page (PROPPAGE * page)
{
    FreeFixStr (page->title);
    DestroyWindow (page->hwnd);
}

#define  OFFSET_VALUE 1
/* create a new page */
static BOOL 
create_page (HWND hwnd, DWORD style, PPROPSHEETDATA propsheet, PROPPAGE *page, 
        PDLGTEMPLATE dlgtpl, WNDPROC proc)
{
    int i, len, w, h;
    PCTRLDATA ctrl;
    HWND hCtrl;
    RECT rcPage;
    DWORD ex_style;

    if ((dlgtpl->controlnr > 0) && !dlgtpl->controls)
        return FALSE;
 
    if (GetWindowExStyle (hwnd) & WS_EX_TRANSPARENT)
        ex_style = WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR;
    else
        ex_style =  WS_EX_USEPARENTRDR;
    
    GetClientRect (hwnd, &rcPage);
    
    w = RECTW(rcPage) - OFFSET_VALUE*2; 
    h = RECTH(rcPage) -RECTH(propsheet->head_rc) - OFFSET_VALUE*2;
    /* create content page */ 
    if ((style & 0xf0L) == PSS_BOTTOM) {
        page->hwnd = CreateWindowEx (CTRL_STATIC, "", SS_LEFT, ex_style, 
                        IDC_STATIC, 
                        OFFSET_VALUE, OFFSET_VALUE, 
                        w, h,
                        hwnd, dlgtpl->dwAddData);
    } else {
        page->hwnd = CreateWindowEx (CTRL_STATIC, "", SS_LEFT, ex_style, 
                        IDC_STATIC, 
                        OFFSET_VALUE, OFFSET_VALUE + propsheet->head_rc.bottom, 
                        w, h,
                        hwnd, dlgtpl->dwAddData);
    }
    if (page->hwnd == HWND_INVALID)
        return FALSE;

    len = strlen (dlgtpl->caption);
    page->title = FixStrAlloc (len);
    if (page->title)
        strcpy (page->title, dlgtpl->caption);

    page->proc = proc;
    page->icon = dlgtpl->hIcon;

    for (i = 0; i < dlgtpl->controlnr; i++) {
        ctrl = dlgtpl->controls + i;
        hCtrl = CreateWindowEx (ctrl->class_name, ctrl->caption,
                    ctrl->dwStyle | WS_CHILD, ctrl->dwExStyle,
                    ctrl->id, ctrl->x, ctrl->y, ctrl->w, ctrl->h,
                    page->hwnd, ctrl->dwAddData);
        
        if (hCtrl == HWND_INVALID) {
            DestroyWindow (page->hwnd);
            if (page->title) {
                FreeFixStr (page->title);
            }
            return FALSE;
        }
    }

    SetWindowCallbackProc (page->hwnd, page->proc);
    SendMessage (page->hwnd, MSG_INITPAGE, 0, dlgtpl->dwAddData);
    return TRUE;
}



/* Given a page, calc it's tab 
   width follow current DC */
static int 
tab_required_width (HDC hdc, PPROPSHEETDATA propsheet, PPROPPAGE page)
{
    int width = 0;
    int iconW, iconH;
    SIZE ext;
    
    if (page->title) {
        GetTextExtent (hdc, page->title, -1, &ext);
        width = ext.cx + 18;
    }
    if (page->icon) {
        GetIconSize(page->icon, &iconW, &iconH);
        width += (iconW + 2*_ICON_OFFSET);
    }
    if (width < _MIN_TAB_WIDTH) {
        width = _MIN_TAB_WIDTH;
    }

    if (width >= propsheet->head_rc.right - get_metrics (MWM_SB_WIDTH) *2) {
        width = propsheet->head_rc.right - get_metrics (MWM_SB_WIDTH) *2;
    }

    return width;
}


/* resize children after the sheet resized. */
static void 
resize_children (PPROPSHEETDATA propsheet, const RECT* rcPage, DWORD style)
{
    PPROPPAGE page;
    int w, h;

#ifdef __TARGET_MSTUDIO__
    w = RECTWP(rcPage) - OFFSET_VALUE; 
    h = RECTHP(rcPage) -RECTH(propsheet->head_rc) - OFFSET_VALUE;
#else
    w = RECTWP(rcPage) - 2*OFFSET_VALUE; 
    h = RECTHP(rcPage) -RECTH(propsheet->head_rc) - 2*OFFSET_VALUE;
#endif

    page = propsheet->head;
    while (page) {
        if ((style & 0xf0L) == PSS_BOTTOM) {
            MoveWindow (page->hwnd, OFFSET_VALUE, OFFSET_VALUE, 
                    w, h,
                    page == propsheet->active);
        } else {
            MoveWindow (page->hwnd, 
                    OFFSET_VALUE, OFFSET_VALUE + propsheet->head_rc.bottom,
                    w, h,
                    page == propsheet->active);
        }
        page = page->next;
    }
}

/* return the page from index. */
static PPROPPAGE get_page (PPROPSHEETDATA propsheet, int index)
{
    int i = 0;
    PPROPPAGE page;

    page = propsheet->head;
    while (page) {
        if (i == index) {
            return page;
        }
        i++;
        page = page->next;
    }

    return NULL;
}

/* append a new page */
static int append_page (PPROPSHEETDATA propsheet, PPROPPAGE new_page)
{
    int i = 0;
    PPROPPAGE page;

    page = propsheet->head;
    while (page && page->next) {
        i++;
        page = page->next;
    }

    new_page->next = NULL;
    if (page) {
        i++;
        page->next = new_page;
    } else {
        propsheet->head = new_page;
    }

    propsheet->page_count++;
    return i;
}

/* remove a page */
static void remove_page (PPROPSHEETDATA propsheet, PPROPPAGE rm_page)
{
    PPROPPAGE page = propsheet->head, prev = NULL;

    while (page) {
        if (page == rm_page) {
            if (prev) {
                prev->next = rm_page->next;
            } else {
                propsheet->head = rm_page->next;
            }
            propsheet->page_count--; 
            break;
        }
        prev = page;
        page = page->next;
    }
}

/* 
 * draw_scroll_button:
 *
 * This function draw scroll button of propsheet control . 
 *
 * Author: Peng LIU
 * Date: 2007-12-05
 */
static void 
draw_scroll_button (HWND hWnd, HDC hdc, RECT *title_rc, 
    PPROPSHEETDATA propsheet, DWORD style)
{
    RECT btn_rect = {0, 0, 0, 0};
    WINDOWINFO  *info;
    DWORD color; 

    info = (WINDOWINFO*)GetWindowInfo (hWnd);
    color = GetWindowElementAttr (hWnd, WE_FGC_WINDOW);
    
    /* draw left arrow */ 
    btn_rect.top = ((style & 0xf0L) == PSS_BOTTOM) ? 
            title_rc->top + 1 + 2 : title_rc->top + 1;
    btn_rect.right = get_metrics (MWM_SB_WIDTH);
    btn_rect.bottom = ((style & 0xf0L) == PSS_BOTTOM) ? 
            title_rc->bottom + 2 - 2 : title_rc->bottom - 2 - 1;
    //SelectClipRect (hdc, &btn_rect);
    info->we_rdr->draw_arrow (hWnd, hdc, &btn_rect, color , LFRDR_ARROW_LEFT); 
 
    /* draw right arrow */
    btn_rect.left = propsheet->head_rc.right - get_metrics(MWM_SB_WIDTH);
    btn_rect.right = propsheet->head_rc.right;
    //SelectClipRect (hdc, &btn_rect);
    info->we_rdr->draw_arrow (hWnd, hdc, &btn_rect, color , LFRDR_ARROW_RIGHT);  
}

/* 
 * draw_propsheet:
 *
 * This function draw nonclilent area of propsheet control . 
 *
 * Author: Peng LIU
 * Date: 2007-12-03
 */
static void draw_propsheet (HWND hwnd, HDC hdc, PCONTROL ctrl, 
   PPROPSHEETDATA propsheet, PPROPPAGE page)
{
    RECT title_rc = {0, 0, 1, 0}, rc;
    int flag = 0;
    DWORD main_c, old_brush_c;
    int scrollbtn_left = 0;

    title_rc.bottom = propsheet->head_rc.bottom;
    if ((ctrl->dwStyle & 0xf0L) == PSS_BOTTOM) {
        title_rc.top = propsheet->head_rc.top;
    } 
    
    main_c = GetWindowElementAttr (hwnd, WE_MAINC_THREED_BODY);

    /* Draw the content window border*/
    if ((ctrl->dwStyle & 0xf0L) == PSS_BOTTOM)  {
        SetRect(&rc, 0, 0, 
                propsheet->head_rc.right, 
                ctrl->bottom - ctrl->top - RECTH(propsheet->head_rc) + 1);
        /*
        rc.left = 0;
        rc.right = propsheet->head_rc.right;
        rc.top = 0;
        rc.bottom = rc.top + ctrl->bottom - ctrl->top - RECTH(propsheet->head_rc) + 1;
        */
    }
    else {
        SetRect(&rc, 0, propsheet->head_rc.bottom - 1, 
                propsheet->head_rc.right,
                propsheet->head_rc.bottom 
                + ctrl->bottom - ctrl->top - RECTH(propsheet->head_rc));
        /*
        rc.left = 0;
        rc.top = propsheet->head_rc.bottom - 1;
        rc.right = propsheet->head_rc.right;
        rc.bottom = rc.top + ctrl->bottom - ctrl->top - RECTH(propsheet->head_rc) + 1;
        */
    }
    ((WINDOWINFO*)GetWindowInfo (hwnd))->we_rdr->draw_3dbox(hdc, 
            &rc, main_c, LFRDR_BTN_STATUS_NORMAL);


    /* Just for PSS_SCROLLABLE style 
       if title is overload (too many titles for the propsheet) 
       we should move-right the leftmot title some pixel 
       and we should draw a small icon (left arrow) for the sheet */
    if ((ctrl->dwStyle & 0x0fL) == PSS_SCROLLABLE) {
        title_rc.right = (propsheet->overload == TRUE) ? 
                        title_rc.right + get_metrics (MWM_SB_WIDTH) : title_rc.right;
        if (propsheet->overload == TRUE)
            scrollbtn_left = propsheet->head_rc.right - get_metrics (MWM_SB_WIDTH);
    }

    old_brush_c = SetBrushColor (hdc, DWORD2Pixel (hdc, main_c));
    while (page) {
        if (((ctrl->dwStyle & 0x0fL) == PSS_SCROLLABLE) && propsheet->overload == TRUE)
            if (((title_rc.right + page->width > scrollbtn_left) || (page->width == 0))
                            && !(propsheet->first_display_page == page))
                break;
        
        if ((ctrl->dwStyle & 0x0fL) == PSS_COMPACTTAB) {
            if (((title_rc.right + page->width > propsheet->head_rc.right) || (page->width == 0))
                            && !(propsheet->first_display_page == page))
                break;
        }

        /* draw some box for title */
        if (title_rc.right == 1)
            title_rc.left = title_rc.right - 1;
        else
            title_rc.left = title_rc.right;

        title_rc.right = title_rc.left + page->width;
        
        flag = 0;
        if (page == propsheet->active)
            flag = flag | LFRDR_TAB_ACTIVE;

        if ((ctrl->dwStyle & 0xf0L) == PSS_BOTTOM) 
            flag = flag | LFRDR_TAB_BOTTOM;

        if (page->icon) {
            flag = flag | LFRDR_TAB_ICON;
             
            ((WINDOWINFO*)GetWindowInfo (hwnd))->we_rdr->draw_tab (hwnd, hdc, 
                    &title_rc, page->title,main_c, flag, page->icon);
        }
        else 
            ((WINDOWINFO*)GetWindowInfo (hwnd))->we_rdr->draw_tab (hwnd, hdc, 
                    &title_rc, page->title, main_c, flag, 0);

        page = page->next;
    }

    /* draw scroll button , just for PSS_SCROLLABLE style */
    if ((ctrl->dwStyle & 0x0fL) == PSS_SCROLLABLE && 
         propsheet->overload == TRUE)  
        draw_scroll_button (hwnd, hdc, &title_rc, propsheet, ctrl->dwStyle);

    SetBrushColor (hdc, DWORD2Pixel(hdc, old_brush_c));
}

static int insert_new_page_normal_style (HWND hwnd, PPROPSHEETDATA propsheet, 
    PPROPPAGE page, DWORD style)
{
    int require_width, index;
    HDC hdc = GetClientDC (hwnd);
    
    require_width = tab_required_width (hdc, propsheet, page);
    ReleaseDC (hdc);
    
    index = 0;
    
    if (propsheet->active) {
        show_hide_page (propsheet->active, SW_HIDE);
    }
    propsheet->active = page;
    
    page->width = require_width;
    index = append_page (propsheet, page);
    update_propsheet (propsheet);

    return index;
}


static int add_new_page_normal_style (HWND hwnd, PCONTROL ctrl, PPROPSHEETDATA propsheet, DLGTEMPLATE *dlg, WNDPROC proc)
{
    PPROPPAGE page;
    int index;
    if (!(page = calloc (1, sizeof (PROPPAGE)))) {
        return PS_ERR;
    }
        
    if (!create_page (hwnd, ctrl->dwStyle, propsheet, page, dlg, proc)) {
        free (page);
        return PS_ERR;
    }
    
    index = insert_new_page_normal_style (hwnd, propsheet, page, ctrl->dwStyle);
    
    NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
    show_hide_page (page, SW_SHOW);
    
    InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
    
    /* dump_propsheetdata (propsheet); */
    return index;
}


static int test_clicked_scroll_button (int x, int y, int btn_top, 
               int btn_bottom, PPROPSHEETDATA propsheet)
{
    RECT btn_rect = {0, 0, 0, 0};
    btn_rect.right = get_metrics(MWM_SB_WIDTH);
    btn_rect.top = btn_top;
    btn_rect.bottom = btn_bottom;
    
    if (PtInRect (&btn_rect, x, y)) {
        return LEFT_SCROLL_BTN_CLICKED;
    } else {
        btn_rect.left = propsheet->head_rc.right - get_metrics(MWM_SB_WIDTH);
        btn_rect.right = propsheet->head_rc.right;
        if (PtInRect (&btn_rect, x, y)) {
            return RIGHT_SCROLL_BTN_CLICKED;
        } else {
            return NO_SCROLL_BTN_CLICKED;
        }
    }

}


static void click_tab_bar (HWND hwnd, PCONTROL ctrl, PPROPSHEETDATA propsheet,
     LPARAM lParam)
{
    int x = LOSWORD(lParam), y = HISWORD(lParam);
    PPROPPAGE page;
    RECT title_rc = {0, 0, 0, 0};
    int scrollbtn_left = 0;
    int scroll_clicked;

    title_rc.bottom = propsheet->head_rc.bottom;
    title_rc.top = propsheet->head_rc.top;
    
    if ((ctrl->dwStyle & 0x0fL)!= PSS_SCROLLABLE) {
        page = propsheet->head;
        while (page) {
            title_rc.left = title_rc.right;
            title_rc.right = title_rc.left + page->width;
            if (PtInRect (&title_rc, x, y) && page != propsheet->active) {
                show_hide_page (propsheet->active, SW_HIDE);
                propsheet->active = page;
                NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
                show_hide_page (page, SW_SHOW);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return;
            }
            page = page->next;
        }

    } else {
        /* Normal style !!! */
        page = propsheet->first_display_page;
        title_rc.right = (propsheet->overload == TRUE) ? 
        title_rc.right + get_metrics(MWM_SB_WIDTH) : title_rc.right;
 
        /* check if left-right-scroll-bar clicked 
           if they are clicked, just change the active title */
        if (propsheet->overload == TRUE) {
            scrollbtn_left = propsheet->head_rc.right - get_metrics (MWM_SB_WIDTH);
            scroll_clicked = test_clicked_scroll_button (x, y, 
                                 title_rc.top, title_rc.bottom, propsheet);
            if (scroll_clicked == LEFT_SCROLL_BTN_CLICKED) {
                scroll_tab_left (hwnd, ctrl, propsheet);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return;
            } 
            if (scroll_clicked == RIGHT_SCROLL_BTN_CLICKED) {
                scroll_tab_right (hwnd, ctrl, propsheet);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return;
            }
        }
        while (page) {
            title_rc.left = title_rc.right;
            title_rc.right = title_rc.left + page->width;
            
            if (propsheet->overload == TRUE) 
                if (title_rc.right > scrollbtn_left)
                    return;
            
            if (PtInRect (&title_rc, x, y) && page != propsheet->active) {
                show_hide_page (propsheet->active, SW_HIDE);
                propsheet->active = page;
                NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
                show_hide_page (page, SW_SHOW);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return;
            }
            page = page->next;
        }
    }
}


/* change page title */
static int set_page_title_normal_style (HDC hdc, PPROPSHEETDATA propsheet, 
    PROPPAGE *page, char *title)
{
    int len = 0;
    
    if (page->title) {
        FreeFixStr (page->title);
    }
    if (title) {
        len = strlen (title);
        if (!(page->title = FixStrAlloc (len))) {
            return PS_ERR;
        }
        strcpy (page->title, title);
        page->width = tab_required_width (hdc, propsheet, page);
        update_propsheet (propsheet);
    } else {
        page->title = NULL;
    }
    return PS_OKAY;
}


static BOOL delete_page (HWND hwnd, PCONTROL ctrl, PPROPSHEETDATA propsheet, 
    int index)
{
    PPROPPAGE page;
    BOOL notify = FALSE;
        
    if ((page = get_page (propsheet, index)) == NULL) {
        return PS_ERR;
    }
    /* first_display_page == active */
    if (propsheet->active == page && propsheet->first_display_page == page) {
        if (page->next) {
            propsheet->active = page->next;
            propsheet->first_display_page = page->next;
        } else {
            PPROPPAGE left = get_left_page (propsheet, propsheet->active);
            if (left != NULL) {
                propsheet->active = left;
                propsheet->first_display_page = left;
            } else {
                propsheet->active = propsheet->first_display_page = NULL;
            } 
        }
        notify = TRUE;
        /* propsheet->active = page->next; */
        /* propsheet->first_display_page = page->next; */
    } else {
        if ((propsheet->active == page) && 
            (propsheet->first_display_page != page)) {
            if (propsheet->active->next == NULL) 
                propsheet->active = get_left_page (propsheet, page);
            else 
                propsheet->active = propsheet->active->next;

            notify = TRUE;
            goto del;
        }
        if ((propsheet->active != page) && 
            (propsheet->first_display_page == page)) {
            if (propsheet->first_display_page->next == NULL) {
                propsheet->first_display_page = get_left_page (propsheet, page);
            } else {
                propsheet->first_display_page = propsheet->first_display_page->next;
            }
            goto del;
        }
    }
 del:
    remove_page (propsheet, page);
    destroy_page (page);
    free (page);

    if (propsheet->head) {
        update_propsheet (propsheet);
    }

    if (propsheet->active) {
        show_hide_page (propsheet->active, SW_SHOW);
    }

    if (notify)
        NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
    
    InvalidateRect (hwnd, &propsheet->head_rc, TRUE);

    /* dump_propsheetdata (propsheet); */
    return PS_OKAY;
}




/* window procedure of the property sheet. */
static LRESULT
PropSheetCtrlProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PCONTROL      ctrl;
    PPROPSHEETDATA propsheet;

    ctrl = gui_Control (hwnd); 
    propsheet = (PROPSHEETDATA *) ctrl->dwAddData2;
    
    switch (message) {
        
    case MSG_CREATE: {
#ifdef __TARGET_MSTUDIO__
        SetWindowBkColor(hwnd, 
                GetWindowElementPixel (hwnd, WE_MAINC_THREED_BODY));
#endif

        if (!(propsheet = calloc (1, sizeof (PROPSHEETDATA)))) {
            return -1;
        }
        ctrl->dwAddData2 = (DWORD)propsheet;
        break;
    }
        
    /* make the client size same as window size */
    case MSG_SIZECHANGED: {
        const RECT* rcWin = (RECT *)wParam;
        RECT* rcClient = (RECT *)lParam;
        
        /* cale the width of content page */
        *rcClient = *rcWin;
        propsheet->head_rc.right = RECTWP (rcClient);

        if ((ctrl->dwStyle & 0xf0L) == PSS_BOTTOM) {
            propsheet->head_rc.top = RECTHP (rcClient) - get_metrics (MWM_ICONY)
                - 2 - _ICON_OFFSET * 2;
            propsheet->head_rc.bottom = RECTHP (rcClient);
        } else {
            propsheet->head_rc.bottom = get_metrics (MWM_ICONY) + 2 
                + _ICON_OFFSET * 2;
        }

        if ((ctrl->dwStyle & 0x0fL)!= PSS_SCROLLABLE) {
            recalc_tab_widths (hwnd, propsheet, ctrl->dwStyle);
        } else {
            HDC hdc; 
            propsheet->head_width = propsheet->head_rc.right;

            if (propsheet->head) {
                PPROPPAGE page;
                hdc = GetClientDC(hwnd);
                page = propsheet->head;
                while(page) {
                    page->width = tab_required_width (hdc, propsheet, page);
                    page = page->next;
                }

                ReleaseDC (hdc);

                update_propsheet (propsheet);
            }
        }
        resize_children (propsheet, rcClient, ctrl->dwStyle);
        InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
        return 1;
    }

    case MSG_DESTROY: {
        PPROPPAGE page, temp;
        page = propsheet->head;
        while (page) {
            temp = page->next;
            destroy_page (page);
            free (page);
            page = temp;
        }
        free (propsheet);
        break;
    }

    case MSG_GETDLGCODE: {
        return DLGC_WANTTAB | DLGC_WANTARROWS;
    }
     
    case PSM_SHEETCMD: {
        int index = 0;
        PPROPPAGE page = propsheet->head;
        while (page) {
            if (SendMessage (page->hwnd, MSG_SHEETCMD, wParam, lParam))
                /* when encounter an error, return page index plus 1. */
                return index + 1;
            index++;
            page = page->next;
        }
        return 0; /* success */
    }

    case PSM_SETACTIVEINDEX: {
        PPROPPAGE page;

        if ((page = get_page (propsheet, wParam)) && page != propsheet->active)         {
            show_hide_page (propsheet->active, SW_HIDE);
            propsheet->active = page;
            update_propsheet (propsheet);
            NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
            show_hide_page (page, SW_SHOW);
            
            InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
            return PS_OKAY;
        }
        return PS_ERR;
    }
        
    case PSM_GETACTIVEINDEX: {
        int index = 0;
        PPROPPAGE page = propsheet->head;
        while (page) {
            if (page == propsheet->active) {
                return index;
            }
            index ++;
            page = page->next;
        }
        return PS_ERR;
    }

    case PSM_GETACTIVEPAGE: {
        return (LRESULT)((propsheet->active) ?  propsheet->active->hwnd : HWND_INVALID);
    }

    case PSM_GETPAGE: {
        int index = 0;
        PPROPPAGE page = propsheet->head;
        while (page) {
            if (index == wParam) {
                return (LRESULT)page->hwnd;
            }
            index ++;
            page = page->next;
        }
        return (LRESULT)HWND_INVALID;
    }
        
    case PSM_GETPAGEINDEX: {
        int index = 0;
        PPROPPAGE page = propsheet->head;
        while (page) {
            if (page->hwnd == (HWND)wParam) {
                return index;
            }
            index ++;
            page = page->next;
        }
        return PS_ERR;
    }
        
    case PSM_GETPAGECOUNT: {
        return propsheet->page_count;
    }

    case PSM_GETTITLELENGTH: {
        int len = PS_ERR;
        PPROPPAGE page;
        
        if ((page = get_page (propsheet, wParam))) {
            len = strlen (page->title);
        }
        return len;
    }

    case PSM_GETTITLE: {
        char* buffer = (char*)lParam;
        PPROPPAGE page;
        
        if ((page = get_page (propsheet, wParam))) {
            strcpy (buffer, page->title);
            return PS_OKAY;
        }
        
        return PS_ERR;
    }

    case PSM_SETTITLE: {
        BOOL rc = PS_ERR;
        char* buffer = (char*)lParam;
        PPROPPAGE page;
        HDC hdc;
        if ((ctrl->dwStyle & 0x0fL) != PSS_SCROLLABLE) {
            if ((page = get_page (propsheet, wParam))) {
                rc = set_page_title (page, buffer);
                recalc_tab_widths (hwnd, propsheet, ctrl->dwStyle);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
            }
        } else {
            if ((page = get_page (propsheet, wParam))) {
                hdc = GetClientDC (hwnd);
                rc = set_page_title_normal_style (hdc, propsheet, page, buffer);
                ReleaseDC (hdc);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
            }
        }
        return rc;
    }

    case PSM_ADDPAGE: {
        if ((ctrl->dwStyle & 0x0fL)  != PSS_SCROLLABLE) {
            int index;
            PPROPPAGE page;
            if ((propsheet->head_rc.right / (propsheet->page_count + 1)) < 
                    _MIN_TAB_WIDTH) {
                return PS_ERR;
            }
            if (!(page = calloc (1, sizeof (PROPPAGE)))) {
                return PS_ERR;
            }
            if (!create_page (hwnd, ctrl->dwStyle, propsheet, page, 
                        (DLGTEMPLATE *)wParam, (WNDPROC)lParam)) {
                free (page);
                return PS_ERR;
            }
            
            index = append_page (propsheet, page);
            if (propsheet->active) {
                show_hide_page (propsheet->active, SW_HIDE);
            }
            propsheet->active = page;
            NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
            show_hide_page (page, SW_SHOW);
            recalc_tab_widths (hwnd, propsheet, ctrl->dwStyle);
            InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
            return index;
        } else {
            return add_new_page_normal_style (hwnd, ctrl, 
                       propsheet, (DLGTEMPLATE *)wParam, 
                      (WNDPROC)lParam);
        }
    }
        
    case PSM_REMOVEPAGE: {
        if ((ctrl->dwStyle & 0x0fL) != PSS_SCROLLABLE) {
            PPROPPAGE page;
            if ((page = get_page (propsheet, wParam))) {
                remove_page (propsheet, page);
                destroy_page (page);
                free (page);
                recalc_tab_widths (hwnd, propsheet, ctrl->dwStyle);
            } else {
                return PS_ERR;
            }
            if (propsheet->active == page) {
                propsheet->active = propsheet->head;
                NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
                if (propsheet->active) {
                    show_hide_page (propsheet->active, SW_SHOW);
                }
            }
            InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
            return PS_OKAY;
        } else {
            return delete_page (hwnd, ctrl, propsheet, wParam);
        }
    }
        
    case MSG_LBUTTONDOWN: {
        click_tab_bar (hwnd, ctrl, propsheet, lParam);
        /* dump_propsheetdata (propsheet); */
        break;
    }

    case MSG_KEYDOWN: {
        PPROPPAGE page, new_active = NULL;
        if (!(lParam & KS_CTRL) || (propsheet->head == NULL)) {
            break;
        }
        /* Key borad message for PSS_COMPACTTAB and PSS_SIMPLE */
        switch (LOWORD (wParam)) {
        case SCANCODE_CURSORBLOCKDOWN:
        case SCANCODE_CURSORBLOCKRIGHT:
            if ((ctrl->dwStyle & 0x0fL)!= PSS_SCROLLABLE) {
                new_active = propsheet->active->next;
                if (new_active == NULL)
                    new_active = propsheet->head;
                break;
            } else {
                scroll_tab_right (hwnd, ctrl, propsheet);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return 0;
            }
        case SCANCODE_CURSORBLOCKUP:
        case SCANCODE_CURSORBLOCKLEFT:
            if ((ctrl->dwStyle & 0x0fL)!= PSS_SCROLLABLE) {
                page = propsheet->head;
                if (propsheet->head == propsheet->active) {
                    while (page && page->next) {
                        page = page->next;
                    }
                } else {
                    while (page) {
                        if (page->next == propsheet->active)
                            break;
                        page = page->next;
                    }
                }
                new_active = page;
                break;
            } else {
                scroll_tab_left (hwnd, ctrl, propsheet);
                InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
                return 0;
            }
        }  /* switch */
        if (new_active == NULL) {
            break;
        }
        show_hide_page (propsheet->active, SW_HIDE);
        propsheet->active = new_active;
        NotifyParent (hwnd, ctrl->id, PSN_ACTIVE_CHANGED);
        show_hide_page (new_active, SW_SHOW);
        InvalidateRect (hwnd, &propsheet->head_rc, TRUE);
        return 0;
    }

    case MSG_NCPAINT:
        
    case MSG_PAINT: {
        HDC  hdc = BeginPaint (hwnd);
        PPROPPAGE page = ((ctrl->dwStyle & 0x0fL) == PSS_SCROLLABLE) ? 
          propsheet->first_display_page : propsheet->head;
        draw_propsheet (hwnd, hdc, ctrl, propsheet, page);
        EndPaint (hwnd, hdc);
        return 0;
    }
        
    default:
        break;
    }
    
    return DefaultControlProc (hwnd, message, wParam, lParam);
}


/* static void dump_propsheetdata (PROPSHEETDATA *p) */
/* { */
/*     fprintf (stdout, "----- P R O P S H E E T -------\n"); */
/*     fprintf (stdout, "head_rc            : left = %d, right = %d, top = %d bottom = %d\n", */
/*              p->head_rc.left, p->head_rc.right, p->head_rc.top, p->head_rc.bottom); */
/*     fprintf (stdout, "head_width         : %d\n", p->head_width); */
/*     fprintf (stdout, "page_count         : %d\n", p->page_count); */
       
/*     fprintf (stdout, "active             : %p\n", p->active); */
/*     fprintf (stdout, "first_display_page : %p\n", p->first_display_page); */
/*     fprintf (stdout, "head               : %p\n", p->head); */
/*     fprintf (stdout, "overload           : %s\n", */
/*              p->overload ? "TRUE" : "FALSE"); */
/* } */

/* static void dump_proppage (PROPPAGE *p) */
/* { */
/*     fprintf (stdout, "----- P R O P P A G E -------\n"); */
/*     fprintf (stdout, "char *title : %s\n", p->title); */
/*     fprintf (stdout, "HICON icon  : %p\n", p->icon); */
/*     fprintf (stdout, "int width   : %d\n", p->width); */
/*     fprintf (stdout, "HWND hwnd   : %p\n", p->hwnd); */
/*     fprintf (stdout, "WNDPROC proc: %p\n", p->proc); */
/*     fprintf (stdout, "next        : %p\n", p->next); */
/* } */



#endif /* _CTRL_PROPSHEET*/

