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
** button.c: the Button Control module.
**
** Create date: 1999/8/23
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"

#ifdef _MGCTRL_BUTTON
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/button.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "fixedmath.h"

#include "button_impl.h"
#include "ctrlmisc.h"

#   define BTN_WIDTH_BMP       20 
#   define BTN_HEIGHT_BMP      20 
#   define BTN_INTER_BMPTEXT   2

#define BUTTON_STATUS(pctrl)   (((PBUTTONDATA)((pctrl)->dwAddData2))->status)
#define BUTTON_DATA(pctrl)     (((PBUTTONDATA)((pctrl)->dwAddData2))->data)
#define BUTTON_TYPE(pctrl)     ((pctrl)->dwStyle & BS_TYPEMASK)

#define BUTTON_IS_AUTO(pctrl) \
        (BUTTON_TYPE(pctrl) == BS_AUTO3STATE || \
         BUTTON_TYPE(pctrl) == BS_AUTOCHECKBOX || \
         BUTTON_TYPE(pctrl) == BS_AUTORADIOBUTTON)

#define BUTTON_IS_PUSHBTN(pctrl) \
        (BUTTON_TYPE(pctrl) == BS_PUSHBUTTON || \
         BUTTON_TYPE(pctrl) == BS_DEFPUSHBUTTON)

#define BUTTON_IS_RADIOBTN(pctrl) \
        (BUTTON_TYPE(pctrl) == BS_AUTORADIOBUTTON || \
         BUTTON_TYPE(pctrl) == BS_RADIOBUTTON)

#define BUTTON_IS_CHECKBTN(pctrl) \
        (BUTTON_TYPE(pctrl) == BS_AUTOCHECKBOX || \
         BUTTON_TYPE(pctrl) == BS_CHECKBOX)

#define BUTTON_IS_3STATEBTN(pctrl) \
        (BUTTON_TYPE(pctrl) == BS_AUTOCHECKBOX || \
         BUTTON_TYPE(pctrl) == BS_CHECKBOX)

#define BUTTON_SET_POSE(pctrl, status) \
        do { \
            BUTTON_STATUS(pctrl) &= ~BST_POSE_MASK; \
            BUTTON_STATUS(pctrl) |= status; \
        }while(0)

#define BUTTON_SET_CHECK(pctrl, check) \
        do { \
            BUTTON_STATUS(pctrl) &= ~BST_CHECK_MASK;\
            BUTTON_STATUS(pctrl) |= check;\
        }while(0)

#define BUTTON_GET_POSE(pctrl)  \
        (BUTTON_STATUS(pctrl) & BST_POSE_MASK)

#define BUTTON_GET_CHECK(pctrl) \
        (BUTTON_STATUS(pctrl) & BST_CHECK_MASK)

#define BUTTON_OFFSET  1

static LRESULT ButtonCtrlProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void btnGetRects (PCONTROL pctrl, RECT* prcClient, 
                                    RECT* prcContent, 
                                    RECT* prcBitmap);

/*when check a radio button, uncheck others in the same group*/
static void btnUncheckOthers (PCONTROL pctrl)
{
    PCONTROL pGroupLeader = pctrl;
    PCONTROL pOthers;
    DWORD    type = BUTTON_TYPE(pctrl);

    while (pGroupLeader) {
        if (pGroupLeader->dwStyle & WS_GROUP)
            break;

        pGroupLeader = pGroupLeader->prev;
    }

	if(!pGroupLeader)
		return ;

    pOthers = pGroupLeader;
        
    do{
        if (BUTTON_TYPE(pOthers)!= type)
            break;
        
        if ((pOthers != pctrl) && 
                (BUTTON_GET_CHECK(pOthers)!= BST_UNCHECKED)) {
            BUTTON_SET_CHECK(pOthers, BST_UNCHECKED);
            InvalidateRect ((HWND)pOthers, NULL, TRUE);
        }

        pOthers = pOthers->next;
    } while (pOthers && !(pOthers->dwStyle & WS_GROUP));

}

/*
 * change_checked:
 *      This function change check status.
 * return -1: can't change(the button in disable status)
 *        0 : new check is old check, didn't change
 *        1 : changed
 * Author: XuguangWang
 * Date: 2007-12-7
 */
static int change_checked (PCONTROL pctrl, int check_status)
{
    int old_status = BUTTON_STATUS(pctrl) & BST_CHECK_MASK;
    /*if push button return*/
    MG_CHECK_RET (BUTTON_TYPE(pctrl) != BS_PUSHBUTTON, -1);

    MG_CHECK_RET (old_status != check_status, 0);

    switch (pctrl->dwStyle & BS_TYPEMASK)
    {
        case BS_RADIOBUTTON:
        case BS_AUTORADIOBUTTON:
            MG_CHECK_RET (check_status != BST_INDETERMINATE, -1);
            BUTTON_SET_CHECK (pctrl, check_status);
            btnUncheckOthers (pctrl);
            break;

        case BS_CHECKBOX:
        case BS_AUTOCHECKBOX:
            MG_CHECK_RET (check_status != BST_INDETERMINATE, -1);
            BUTTON_SET_CHECK (pctrl, check_status);
            break;

        case BS_3STATE:
        case BS_AUTO3STATE:
            BUTTON_SET_CHECK (pctrl, check_status);
            break;
    }

    return 1;
}

/**
 * switch_check:
 *      switch check status when the button is clicked
 * return: new check status
 * Author: XuguangWang
 * Date: 2007-12-7
 */
static int switch_check (PCONTROL pctrl)
{ 
    int old_check = BUTTON_GET_CHECK(pctrl);
    /*calc new ckeck status*/
    switch (BUTTON_TYPE(pctrl))
    {
        case BS_RADIOBUTTON:
        case BS_AUTORADIOBUTTON:
            return BST_CHECKED;

        case BS_CHECKBOX:
        case BS_AUTOCHECKBOX:
            return old_check==BST_CHECKED ? BST_UNCHECKED : BST_CHECKED;

        case BS_3STATE:
        case BS_AUTO3STATE:
            return old_check==BST_UNCHECKED ? BST_CHECKED : (
                    BUTTON_GET_CHECK(pctrl) == BST_CHECKED ?      
                    BST_INDETERMINATE : BST_UNCHECKED
                    );
        default:
            return -1;
    }
}

/*change_status_from_pose
 *      change button status (pose and check status), and invalidate window.
 *      pose status -- normal, hilite, pressed(pushed), disable.
 *      check status -- unchecked, checked, indeterminate.
 *
 * param noti_pose_changing: indicating if the status changing is from key or 
 *           mouse lbutton up and down.
 *           TRUE -- the button(if it is auto) should notify parent
 *           FALSE -- the pose changing is from mouse moving, so button need
 *                  not notify parent
 *
 * param change_valid: if to normal(new_pose) is valid(it can change check status)
 *           TRUE -- key up or mouse lbutton up on the button
 *           FALSE -- key lbutton up is not on the button, so it's not a full
 *                    click
 * */

static void change_status_from_pose (PCONTROL pctrl, int new_pose, 
             BOOL noti_pose_changing, BOOL change_valid)
{
    static int nt_code[3][3] = {
        /*normal->nochange, hilite, pressed*/
        {          0,  BN_HILITE, BN_PUSHED},
        /*hilite->normal, nochange, pressed*/
        {BN_UNHILITE,          0, BN_PUSHED},
        /*pressed->normal, hilite, nochange*/
        {BN_UNPUSHED, BN_UNPUSHED,        0},
    };

    DWORD dwStyle = pctrl->dwStyle;
    int old_pose = BUTTON_GET_POSE(pctrl);
    int new_check;

    RECT rcClient;
    RECT rcContent;
    RECT rcBitmap;

    /*make sure it can be changed, and avoid changing more than once*/
    MG_CHECK (old_pose!= BST_DISABLE && old_pose != new_pose);

    /*set new pose*/
    BUTTON_SET_POSE(pctrl, new_pose);

    if ((dwStyle & BS_NOTIFY) && noti_pose_changing)
    {
        if (new_pose != BST_DISABLE)
            NotifyParent ((HWND)pctrl, pctrl->id, 
                    nt_code[old_pose][new_pose]);
        /*disable the button*/
        else
        {
            /*before to disable, firstly to normal*/
            switch (old_pose)
            {
                case BST_PUSHED:
                    NotifyParent ((HWND)pctrl, pctrl->id, BN_UNPUSHED);
                case BST_HILITE:
                    NotifyParent ((HWND)pctrl, pctrl->id, BN_UNHILITE);
                case BST_NORMAL:
                    NotifyParent ((HWND)pctrl, pctrl->id, BN_DISABLE);
                    break;
                default:
                    assert (0);
                    break;
            }
        }
    }

    /*complete a click if the click if valid
     * (mouse lbutton up in button )*/
    if (old_pose == BST_PUSHED && new_pose != BST_DISABLE
            && change_valid)
    {
        if (!BUTTON_IS_PUSHBTN(pctrl) && BUTTON_IS_AUTO(pctrl))
        {
            new_check = switch_check(pctrl);
            change_checked(pctrl, new_check);
        }

        NotifyParent ((HWND)pctrl, pctrl->id, BN_CLICKED);
    }

    /* DK: When the radio or check button state changes do not erase the background, 
     * and the prospects for direct rendering */
    btnGetRects (pctrl, &rcClient, &rcContent, &rcBitmap);
    if (BUTTON_IS_PUSHBTN(pctrl) || (pctrl->dwStyle & BS_PUSHLIKE)) 
        InvalidateRect((HWND)pctrl, NULL, TRUE);
    else 
        InvalidateRect((HWND)pctrl, &rcBitmap, FALSE);
}

/*btnGetRects:
 *      get the client rect(draw body of button), the content rect
 *      (draw contern), and bitmap rect(draw a little icon)
 */
static void btnGetRects (PCONTROL pctrl, RECT* prcClient, 
                                    RECT* prcContent, 
                                    RECT* prcBitmap)
{
    GetClientRect ((HWND)pctrl, prcClient);
    
    if (BUTTON_IS_PUSHBTN(pctrl) || (pctrl->dwStyle & BS_PUSHLIKE))
    {
        SetRect (prcContent, (prcClient->left   + BTN_WIDTH_BORDER),
                (prcClient->top    + BTN_WIDTH_BORDER),
                (prcClient->right  - BTN_WIDTH_BORDER),
                (prcClient->bottom - BTN_WIDTH_BORDER));

        if (BUTTON_GET_POSE(pctrl) == BST_PUSHED)
        {
            prcContent->left ++;
            prcContent->top ++;
            prcContent->right ++;
            prcContent->bottom++;
        }

        SetRectEmpty (prcBitmap);
        return;
    }

    if (pctrl->dwStyle & BS_LEFTTEXT) {
        SetRect (prcContent, prcClient->left + 1,
                          prcClient->top + 1,
                          prcClient->right - BTN_WIDTH_BMP - BTN_INTER_BMPTEXT,
                          prcClient->bottom - 1);
        SetRect (prcBitmap, prcClient->right - BTN_WIDTH_BMP,
                            prcClient->top + 1,
                            prcClient->right - 1,
                            prcClient->bottom - 1);
    }
    else {
        SetRect (prcContent, prcClient->left + BTN_WIDTH_BMP + BTN_INTER_BMPTEXT,
                          prcClient->top + 1,
                          prcClient->right - 1,
                          prcClient->bottom - 1);
        SetRect (prcBitmap, prcClient->left + 1,
                            prcClient->top + 1,
                            prcClient->left + BTN_WIDTH_BMP,
                            prcClient->bottom - 1);
    }

}

/**
 * calc DrawText format from button style
 */

static int btnGetTextFmt (int dwStyle)
{
    UINT dt_fmt;
    if (dwStyle & BS_MULTLINE)
        dt_fmt = DT_WORDBREAK;
    else
        dt_fmt = DT_SINGLELINE;

    if ((dwStyle & BS_TYPEMASK) == BS_PUSHBUTTON
            || (dwStyle & BS_TYPEMASK) == BS_DEFPUSHBUTTON
            || (dwStyle & BS_PUSHLIKE))
        dt_fmt |= DT_CENTER | DT_VCENTER;
    else {
        if ((dwStyle & BS_ALIGNMASK) == BS_RIGHT)
            dt_fmt = DT_WORDBREAK | DT_RIGHT;
        else if ((dwStyle & BS_ALIGNMASK) == BS_CENTER)
            dt_fmt = DT_WORDBREAK | DT_CENTER;
        else dt_fmt = DT_WORDBREAK | DT_LEFT;
            
        if ((dwStyle & BS_ALIGNMASK) == BS_TOP)
            dt_fmt |= DT_SINGLELINE | DT_TOP;
        else if ((dwStyle & BS_ALIGNMASK) == BS_BOTTOM)
            dt_fmt |= DT_SINGLELINE | DT_BOTTOM;
        else dt_fmt |= DT_SINGLELINE | DT_VCENTER;
    }

    return dt_fmt;
}

/**
 * draw bitmap content in button with BS_BITMAP style
 */
static void draw_bitmap_button (PCONTROL pctrl, HDC hdc, DWORD dwStyle, 
                          RECT *prcText)
{
    if (BUTTON_DATA(pctrl)) {
        int x = prcText->left;
        int y = prcText->top;
        int w = RECTWP (prcText);
        int h = RECTHP (prcText);
        PBITMAP bmp = (PBITMAP)(BUTTON_DATA(pctrl));
        RECT prcClient;

        GetClientRect ((HWND)pctrl, &prcClient);
        if (dwStyle & BS_REALSIZEIMAGE) {
            x += (w - bmp->bmWidth) >> 1;
            y += (h - bmp->bmHeight) >> 1;
            w = h = 0;

            if (bmp->bmWidth > RECTW(prcClient)){
                x = prcClient.left + BTN_WIDTH_BORDER;
                w = RECTW(prcClient) - 2*BTN_WIDTH_BORDER-1;
            }

            if (bmp->bmHeight > RECTH(prcClient)){
                y = prcClient.top + BTN_WIDTH_BORDER;
                h = RECTH(prcClient) - 2*BTN_WIDTH_BORDER-1;
            }

        }
        else {
            x = prcClient.top + BTN_WIDTH_BORDER;
            y = prcClient.left + BTN_WIDTH_BORDER;
            w = RECTW (prcClient) - 2*BTN_WIDTH_BORDER-1;
            h = RECTH (prcClient) - 2*BTN_WIDTH_BORDER-1;
        }

        FillBoxWithBitmap (hdc, x, y, w, h, bmp);
    }
}

/**
 * draw bitmap content in button with BS_ICON style
 */
static void draw_icon_button(PCONTROL pctrl, HDC hdc, DWORD dwStyle, RECT *prcText)
{
    if (BUTTON_DATA(pctrl)) {
        int x = prcText->left;
        int y = prcText->top;
        int w = RECTWP (prcText);
        int h = RECTHP (prcText);
        HICON icon = (HICON)(BUTTON_DATA(pctrl));
        RECT prcClient;

        if (dwStyle & BS_REALSIZEIMAGE) {
            int iconw, iconh;

            GetIconSize (icon, &iconw, &iconh);
            x += (w - iconw) >> 1;
            y += (h - iconh) >> 1;
            w = h = 0;
        }
        else {
            GetClientRect ((HWND)pctrl, &prcClient);
            x = prcClient.top;
            y = prcClient.left;
            w = RECTW (prcClient);
            h = RECTH (prcClient);
        }

        DrawIcon (hdc, x, y, w, h, icon);

    }
}

/**
 * paint_content_focus:
 *      draw content in button and focusRect (if it have focus) 
 * Author: XuguangWang     
 * Date: 2007-12-7
 */
static void paint_content_focus(HDC hdc, PCONTROL pctrl, RECT* prc_cont)
{
    DWORD dt_fmt = 0;
    DWORD fg_color = 0xFFFF;
    DWORD text_pixel;
    BOOL is_get_fg = FALSE;
    BOOL is_get_fmt = FALSE;
    gal_pixel old_pixel;
    RECT focus_rc;
    const WINDOW_ELEMENT_RENDERER* win_rdr;

    /* Not used vars.
    int status, type;
    status = BUTTON_STATUS (pctrl);
    type = BUTTON_TYPE (pctrl);
    */
    win_rdr = GetWindowInfo((HWND)pctrl)->we_rdr;
    
    /*draw button content*/
    switch (pctrl->dwStyle & BS_CONTENTMASK)
    {
        case BS_BITMAP:
            draw_bitmap_button(pctrl, hdc, pctrl->dwStyle, prc_cont);
            break;
        case BS_ICON:
            draw_icon_button(pctrl, hdc, pctrl->dwStyle, prc_cont);
            break;
        default:
            fg_color = GetWindowElementAttr((HWND)pctrl, WE_FGC_THREED_BODY);

            is_get_fg = TRUE;
            text_pixel = RGBA2Pixel(hdc, GetRValue(fg_color),
                    GetGValue(fg_color), GetBValue(fg_color), 
                    GetAValue(fg_color));

            old_pixel = SetTextColor(hdc, text_pixel);
            dt_fmt = btnGetTextFmt(pctrl->dwStyle);
            is_get_fmt = TRUE;
            SetBkMode (hdc, BM_TRANSPARENT);
            DrawText (hdc, pctrl->spCaption, -1, prc_cont, dt_fmt);
            SetTextColor(hdc, old_pixel);

            /*disable draw text*/
            if ((BUTTON_GET_POSE(pctrl) == BST_DISABLE) 
                    | (pctrl->dwStyle & WS_DISABLED))
                win_rdr->disabled_text_out ((HWND)pctrl, hdc, pctrl->spCaption, 
                                prc_cont, dt_fmt);
    }

    /*draw focus frame*/
    if (BUTTON_STATUS(pctrl) & BST_FOCUS)
    {
        focus_rc = *prc_cont;

        if (!BUTTON_IS_PUSHBTN(pctrl) && 
            !(pctrl->dwStyle & BS_PUSHLIKE) && 
            is_get_fmt)
        {
            dt_fmt |= DT_CALCRECT;
            DrawText (hdc, pctrl->spCaption, -1, &focus_rc, dt_fmt);
        }

        if (!is_get_fg)
            fg_color = GetWindowElementAttr((HWND)pctrl, WE_FGC_THREED_BODY);
        win_rdr->draw_focus_frame(hdc, &focus_rc, fg_color);
    }
}

/**
 * paint_push_btn:
 *      draw push or pushlike button
 * Author: XuguangWang     
 * Date: 2007-12-7
 */
static void paint_push_btn (HDC hdc, PCONTROL pctrl)
{
    const WINDOW_ELEMENT_RENDERER* win_rdr;
    DWORD main_color;

    RECT rcClient;
    RECT rcContent;
    RECT rcBitmap;
    win_rdr = GetWindowInfo((HWND)pctrl)->we_rdr;
    main_color = GetWindowElementAttr((HWND)pctrl, WE_MAINC_THREED_BODY);

    btnGetRects (pctrl, &rcClient, &rcContent, &rcBitmap);
    win_rdr->draw_push_button((HWND)pctrl, hdc, &rcClient, main_color, 
            0xFFFFFFFF, BUTTON_STATUS(pctrl));

    paint_content_focus(hdc, pctrl, &rcContent);
}


#if 1
/*a matrix from button control status to renderer status*/
static int ctrlst_rdrst[12] = {
    /*unckecked*/
    LFRDR_BTN_STATUS_NORMAL, 
    LFRDR_BTN_STATUS_HILITE, 
    LFRDR_BTN_STATUS_PRESSED, 
    LFRDR_BTN_STATUS_DISABLED,
    /*checked*/
    LFRDR_BTN_STATUS_NORMAL|LFRDR_BTN_STATUS_SELECTED,    
    LFRDR_BTN_STATUS_HILITE|LFRDR_BTN_STATUS_SELECTED,    
    LFRDR_BTN_STATUS_PRESSED|LFRDR_BTN_STATUS_SELECTED,    
    LFRDR_BTN_STATUS_DISABLED|LFRDR_BTN_STATUS_SELECTED,    
    /*indeterminate*/
    LFRDR_BTN_STATUS_DISABLED,    
    LFRDR_BTN_STATUS_DISABLED,    
    LFRDR_BTN_STATUS_DISABLED,    
    LFRDR_BTN_STATUS_DISABLED,    
};

#else
static int ctrlst_rdrst[12] = {
    /*unckecked*/
    BST_NORMAL, 
    BST_HILITE, 
    BST_PUSHED, 
    BST_DISABLE,
    /*checked*/
    BST_NORMAL  | BST_CHECKED,    
    BST_HILITE  | BST_CHECKED,    
    BST_PUSHED  | BST_CHECKED,    
    BST_DISABLE | BST_CHECKED,    
    /*indeterminate*/
    BST_DISABLE,    
    BST_DISABLE,    
    BST_DISABLE,    
    BST_DISABLE,    
};
#endif
/**
 * paint_check_radio_btn:
 *      draw check button, radio button and 3state button
 * Author: XuguangWang     
 * Date: 2007-12-7
 */
static void paint_check_radio_btn (HDC hdc, PCONTROL pctrl)
{
    const WINDOW_ELEMENT_RENDERER* win_rdr;
    int rdr_st;

    RECT rcClient;
    RECT rcContent;
    RECT rcBitmap;

    win_rdr = GetWindowInfo((HWND)pctrl)->we_rdr;

    /* Not used vars
    gal_pixel text_pixel;
    text_pixel = GetWindowElementPixelEx((HWND)pctrl, hdc, WE_FGC_WINDOW);
    */

    rdr_st = ctrlst_rdrst [ BUTTON_STATUS (pctrl) &
                    (BST_POSE_MASK|BST_CHECK_MASK) ];
    btnGetRects (pctrl, &rcClient, &rcContent, &rcBitmap);

    if (BUTTON_IS_RADIOBTN(pctrl))
        win_rdr->draw_radio_button((HWND)pctrl, hdc, &rcBitmap, rdr_st);
    else
        win_rdr->draw_check_button((HWND)pctrl, hdc, &rcBitmap, rdr_st);

    /* DK: just draw text after background has been erased.*/
    paint_content_focus(hdc, pctrl, &rcContent);
}



BOOL RegisterButtonControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_BUTTON;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = 
        GetWindowElementPixel (HWND_NULL, WE_MAINC_THREED_BODY);
    WndClass.WinProc     = ButtonCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

/**
 * mouse_in_client:
 *      calc if mouse is in client
 * Author: XuguangWang     
 * Date: 2007-12-7
 */
static inline BOOL mouse_in_client(LPARAM lParam, PCONTROL pctrl)
{
    int x, y;
    x = LOSWORD(lParam);
    y = HISWORD(lParam);
    ScreenToClient (GetParent ((HWND)pctrl), &x, &y);
    return (PtInRect ((PRECT) &(pctrl->cl), x, y));
}

static LRESULT ButtonCtrlProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PCONTROL    pctrl;
    DWORD       dwStyle;
    PBUTTONDATA pData;

    pctrl   = gui_Control(hWnd);
    dwStyle = pctrl->dwStyle;

    switch(uMsg)
    {
        case MSG_CREATE:
            pData = (BUTTONDATA*) calloc (1, sizeof(BUTTONDATA));
            SetWindowBkColor (hWnd, GetWindowElementPixel (hWnd, 
                        WE_MAINC_THREED_BODY));
            if (pData == NULL) 
                return -1;

            pData->status = 0;
            pData->data = pctrl->dwAddData;
            pctrl->dwAddData2 = (DWORD) pData;

            if (!BUTTON_IS_PUSHBTN(pctrl) && dwStyle & BS_CHECKED)
                BUTTON_SET_CHECK(pctrl, BST_CHECKED);

        break;
       
        case MSG_DESTROY:
            free ((void*) pctrl->dwAddData2);
        break;
        
        case BM_GETCHECK:
            if (!BUTTON_IS_PUSHBTN(pctrl))
                return BUTTON_GET_CHECK(pctrl);
            else
                return -1;

        case BM_SETCHECK:
        {
            int set_ret;
            UINT old_check;
     //       MG_CHECK_RET(BUTTON_IS_PUSHBTN(pctrl), -1);
            old_check = BUTTON_GET_CHECK(pctrl);
            set_ret = change_checked(pctrl,wParam);
            if (set_ret == 1)
                InvalidateRect (hWnd, NULL, TRUE);

            return old_check;
        }
        
        case BM_GETSTATE:
            return (int)(BUTTON_GET_POSE(pctrl));

        case BM_SETSTATE:
        {
            DWORD old_pose = BUTTON_GET_POSE(pctrl);
            
            if (wParam)
                BUTTON_SET_POSE(pctrl, BST_PUSHED);
            else
                BUTTON_SET_POSE(pctrl, BST_NORMAL);
            
            if (old_pose != BUTTON_GET_POSE(pctrl))
                InvalidateRect (hWnd, NULL, TRUE);

            return old_pose;
        }

        case BM_ENABLE:
        {
            if (wParam == FALSE)
            {
                /*disable a button*/
                MG_CHECK_RET (BUTTON_GET_POSE(pctrl) != BST_DISABLE, 0);
                change_status_from_pose (pctrl, BST_DISABLE, TRUE, FALSE);
            }
            else
            {
                /*enable a button*/
                MG_CHECK_RET (BUTTON_GET_POSE(pctrl) == BST_DISABLE, 0);
                BUTTON_SET_POSE (pctrl, BST_NORMAL);
                NotifyParent(hWnd, pctrl->id, BN_ENABLE);
            }
            InvalidateRect (hWnd, NULL, TRUE);
            return 0;
        }

        case BM_GETIMAGE:
        {
            int* image_type = (int*) wParam;

            if (dwStyle & BS_BITMAP) {
                if (image_type) {
                    *image_type = BM_IMAGE_BITMAP;
                }
                return (int)(BUTTON_DATA (pctrl));
            }
            else if (dwStyle & BS_ICON) {
                if (image_type) {
                    *image_type = BM_IMAGE_ICON;
                }
                return (int)(BUTTON_DATA (pctrl));
            }
        }
        return 0;
        
        case BM_SETIMAGE:
        {
            int oldImage = (int)BUTTON_DATA (pctrl);

            if (wParam == BM_IMAGE_BITMAP) {
                pctrl->dwStyle &= ~BS_ICON;
                pctrl->dwStyle |= BS_BITMAP;
            }
            else if (wParam == BM_IMAGE_ICON){
                pctrl->dwStyle &= ~BS_BITMAP;
                pctrl->dwStyle |= BS_ICON;
            }

            if (lParam) {
                 BUTTON_DATA (pctrl) = (DWORD)lParam;
                 InvalidateRect (hWnd, NULL, TRUE);
            }

            return oldImage;
        }
        break;

        case BM_CLICK:
        {
            int new_check;
            if (BUTTON_IS_AUTO(pctrl))
            {
                new_check = switch_check(pctrl);
                change_checked(pctrl, new_check);
                InvalidateRect (hWnd, NULL, TRUE);
            }

            NotifyParent (hWnd, pctrl->id, BN_CLICKED);
        }
        break;
        
        case MSG_SETTEXT:
        {
            int len = strlen((char*)lParam);
            FreeFixStr (pctrl->spCaption);
            pctrl->spCaption = FixStrAlloc (len);
            if (len > 0)
                strcpy (pctrl->spCaption, (char*)lParam);
            InvalidateRect (hWnd, NULL, TRUE);
            break;
        }

        
                
        case BM_SETSTYLE:
            pctrl->dwStyle &= 0xFFFF0000;
            pctrl->dwStyle |= (DWORD)(wParam & 0x0000FFFF);
            if (LOWORD (lParam))
                InvalidateRect (hWnd, NULL, TRUE);
        break;
        
        case MSG_CHAR:
            if (HIBYTE (wParam)==0 && BUTTON_IS_CHECKBTN (pctrl)) {
                int old_check = BUTTON_GET_CHECK(pctrl);
                
                if (LOBYTE(wParam) == '+' || LOBYTE(wParam) == '=')
                    BUTTON_SET_CHECK(pctrl, BST_CHECKED);
                else if (LOBYTE(wParam) == '-')
                    BUTTON_SET_CHECK(pctrl, BST_UNCHECKED);
                    
                if (old_check != BUTTON_GET_CHECK(pctrl))
                    InvalidateRect (hWnd, NULL, TRUE);
            }
        break;
        
        case MSG_ENABLE:
            if (wParam && (dwStyle & WS_DISABLED))
                pctrl->dwStyle &= ~WS_DISABLED;
            else if (!wParam && !(dwStyle & WS_DISABLED))
                pctrl->dwStyle |= WS_DISABLED;
            else
                return 0;
            InvalidateRect (hWnd, NULL, TRUE);
        return 0;

        case MSG_GETDLGCODE:
            switch (dwStyle & BS_TYPEMASK) {
                case BS_CHECKBOX:
                case BS_AUTOCHECKBOX:
                return DLGC_WANTCHARS | DLGC_BUTTON;
                
                case BS_RADIOBUTTON:
                case BS_AUTORADIOBUTTON:
                return DLGC_RADIOBUTTON | DLGC_BUTTON;
                
                case BS_DEFPUSHBUTTON:
                return DLGC_DEFPUSHBUTTON;
                
                case BS_PUSHBUTTON:
                return DLGC_PUSHBUTTON;
                
                case BS_3STATE:
                case BS_AUTO3STATE:
                return DLGC_3STATE;

                default:
                return 0;
            }
        break;

        case MSG_NCHITTEST:
        {
            int x, y;
            
            x = (int)wParam;
            y = (int)lParam;
        
            if (PtInRect ((PRECT) &(pctrl->cl), x, y))
                return HT_CLIENT;
            else  
                return HT_OUT;
        }
        break;
    
        case MSG_KILLFOCUS:
            BUTTON_STATUS(pctrl) &= (~BST_FOCUS);
            if (GetCapture() == hWnd) {
                ReleaseCapture ();

                BUTTON_SET_POSE(pctrl, BST_NORMAL);
            }

            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pctrl->id, BN_KILLFOCUS);

            InvalidateRect (hWnd, NULL, TRUE);
        break;

        case MSG_SETFOCUS:
            BUTTON_STATUS(pctrl) |= BST_FOCUS;

            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pctrl->id, BN_SETFOCUS);

            InvalidateRect (hWnd, NULL, TRUE);
        break;
        
        case MSG_KEYDOWN:
            if (wParam != SCANCODE_SPACE 
                    && wParam != SCANCODE_ENTER
                    && wParam != SCANCODE_KEYPADENTER)
                break;

            if (GetCapture () == hWnd)
                break;
            
            SetCapture (hWnd);
            //FIXME not needed 
            //BUTTON_STATUS(pctrl) |= BST_FOCUS;
            change_status_from_pose(pctrl, BST_PUSHED, TRUE, TRUE);

        break;
        
        case MSG_KEYUP:
        {
            if (wParam != SCANCODE_SPACE 
                    && wParam != SCANCODE_ENTER
                    && wParam != SCANCODE_KEYPADENTER)
                break;
                
            if (GetCapture () == hWnd)
            {
                ReleaseCapture ();
                change_status_from_pose(pctrl, BST_NORMAL, TRUE, TRUE);
            }
        }
        break;
      
        case MSG_LBUTTONDBLCLK:
            if (dwStyle & BS_NOTIFY && 
                BUTTON_GET_POSE(pctrl) != BST_DISABLE)
                NotifyParent (hWnd, pctrl->id, BN_DBLCLK);
        break;
        
        case MSG_LBUTTONDOWN:
            if (GetCapture () == hWnd)
                break;

            SetCapture (hWnd);

            BUTTON_STATUS(pctrl) |= BST_FOCUS;
            change_status_from_pose(pctrl, BST_PUSHED, TRUE, TRUE);
        break;
    
        case MSG_LBUTTONUP:
        {
            if (GetCapture() == hWnd)
            {
                ReleaseCapture ();

                if (mouse_in_client(lParam, pctrl))
                    change_status_from_pose(pctrl, BST_HILITE, TRUE, TRUE);
                else
                    change_status_from_pose(pctrl, BST_NORMAL, TRUE, FALSE);
            }
            return 0;
        }

        case MSG_MOUSEMOVEIN:
        {
            /*leave a button, nomralize it*/
            if (!wParam)
                change_status_from_pose(pctrl, BST_NORMAL, TRUE, FALSE);
            
            /*because if lbutton downed on space out of the button, 
             * and then  move to it, it will receive MSG_MOUSEMOVEIN, 
             * but it should not be hilited by MSG_MOUSEMOVEIN(wParam == TRUE)
             * */
            break;
        }
                
        case MSG_MOUSEMOVE:
        {
            /*left button is not downed*/
            if (!(wParam & KS_LEFTBUTTON))
            {
                /*mouse not captured by keydowning on other button*/
                if (GetCapture()==HWND_NULL)
                    change_status_from_pose(pctrl, BST_HILITE, TRUE, FALSE);

                break;
            }

            /*the button capture mouse by mouse*/
            if (GetCapture() == hWnd)
            {
                /*normal -- pushed, while lbutton is being downed*/
                if (mouse_in_client(lParam, pctrl))
                    change_status_from_pose(pctrl, BST_PUSHED, FALSE, FALSE);
                else
                    change_status_from_pose(pctrl, BST_NORMAL, FALSE, FALSE);
            }
            break;
        }
        case MSG_PAINT:
        {
            hdc = BeginPaint (hWnd);
            SelectFont (hdc, GetWindowFont(hWnd));

            if (BUTTON_IS_PUSHBTN(pctrl) || (pctrl->dwStyle & BS_PUSHLIKE))
                paint_push_btn(hdc, pctrl);
            else
                paint_check_radio_btn(hdc, pctrl);

            EndPaint (hWnd, hdc);
            return 0;
        }

        default:
        break;
    }
    
    return DefaultControlProc (hWnd, uMsg, wParam, lParam);
}

#endif /* _MGCTRL_BUTTON */

