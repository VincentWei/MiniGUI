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
** button.c: the Menu Button Control module.
**
** Create date: 2000/11/16
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGCTRL_MENUBUTTON
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/menubutton.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "ctrlmisc.h"

#define _USE_FIXSTR  1

#include "menubutton_impl.h"

#define _WIDTH_BORDER       4
#define _WIDTH_MENUBAR   12
#define _HEIGHT_MENUBAR  8

#define _INTER_BARTEXT   4

static LRESULT MenuButtonCtrlProc (HWND hWnd, UINT message, WPARAM wParam, 
                LPARAM lParam);

BOOL RegisterMenuButtonControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_MENUBUTTON;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = 
        GetWindowElementPixel (HWND_NULL, WE_MAINC_THREED_BODY);
    WndClass.WinProc     = MenuButtonCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

static BOOL mbInitMenuButtonData (MENUBTNDATA* mb_data, int len)
{
    int i;
    PMBITEM pmbi;
    
    mb_data->str_cmp = strncmp;

    mb_data->cur_sel = MB_INV_ITEM;

    // init item buffer.
    if (!(mb_data->buff_head = malloc (len * sizeof (MBITEM))))
        return FALSE;

    mb_data->buff_len = len;
    mb_data->buff_tail = mb_data->buff_head + len;
    mb_data->free_list = mb_data->buff_head;

    pmbi = mb_data->free_list;
    for (i = 0; i < len - 1; i++) {
        pmbi->next = pmbi + 1;
        pmbi ++;
    }
    pmbi->next = NULL;

    return TRUE;
}

static void mbMenuButtonCleanUp (MENUBTNDATA* mb_data)
{
    PMBITEM pmbi;
    PMBITEM next;

    pmbi = mb_data->first_item;
    while (pmbi) {
#if _USE_FIXSTR
        FreeFixStr (pmbi->text);
#else
        free (pmbi->text);
#endif
        next = pmbi->next;
        if (pmbi < mb_data->buff_head || pmbi > mb_data->buff_tail)
            free (pmbi);

        pmbi = next;
    }
    
    free (mb_data->buff_head);
}

static void mbResetMenuButtonContent (PMENUBTNDATA mb_data)
{
    int i;
    PMBITEM pmbi, next;

    mb_data->item_count = 0;
    mb_data->cur_sel    = MB_INV_ITEM;

    pmbi = mb_data->first_item;
    while (pmbi) {
#if _USE_FIXSTR
        FreeFixStr (pmbi->text);
#else
        free (pmbi->text);
#endif
        next = pmbi->next;
        if (pmbi < mb_data->buff_head || pmbi > mb_data->buff_tail)
            free (pmbi);

        pmbi = next;
    }

    mb_data->first_item = NULL;
    mb_data->free_list = mb_data->buff_head;

    pmbi = mb_data->free_list;
    for (i = 0; i < mb_data->buff_len - 1; i++) {
        pmbi->next = pmbi + 1;
        pmbi ++;
    }
    pmbi->next = NULL;
}

static PMBITEM mbAllocItem (PMENUBTNDATA mb_data)
{
    PMBITEM pmbi;

    if (mb_data->free_list) {
        pmbi = mb_data->free_list;
        mb_data->free_list = pmbi->next;
    }
    else
        pmbi = (PMBITEM) malloc (sizeof (MBITEM));
    
    return pmbi;
}

static void mbFreeItem (PMENUBTNDATA mb_data, PMBITEM pmbi)
{
    if (pmbi < mb_data->buff_head || pmbi > mb_data->buff_tail)
        free (pmbi);
    else {
        pmbi->next = mb_data->free_list;
        mb_data->free_list = pmbi;
    }
}

static int mbAddNewItem (DWORD dwStyle, 
        PMENUBTNDATA mb_data, PMBITEM newItem, int pos)
{
    PMBITEM pmbi;
    PMBITEM insPosItem = NULL;
    int insPos = 0;

    newItem->next = NULL;
    if (!mb_data->first_item)
        insPosItem = NULL;
    else if (dwStyle & MBS_SORT) {
        pmbi = mb_data->first_item;

        if (mb_data->str_cmp (newItem->text, pmbi->text, (size_t)-1) < 0) {
            insPosItem = NULL;
            insPos = 0;
        }
        else {
            while (pmbi->next) {
                if (mb_data->str_cmp (newItem->text, pmbi->next->text, 
                                    (size_t)-1) <= 0)
                    break;
            
                pmbi = pmbi->next;
                insPos ++;
            }
            insPosItem = pmbi;
        }
    }
    else {
        pmbi = mb_data->first_item;

        if (pos < 0) {
            while (pmbi->next) {
                pmbi = pmbi->next;
                insPos ++;
            }
            insPosItem = pmbi;
        }
        else if (pos > 0) {
            int index = 0;

            while (pmbi->next) {
                if (pos == index)
                    break;
                pmbi = pmbi->next;
                index ++;
                insPos ++;
            }
            insPosItem = pmbi;
        }
    }

    if (insPosItem) {
        pmbi = insPosItem->next;
        insPosItem->next = newItem;
        newItem->next = pmbi;

        insPos ++;
    }
    else {
        pmbi = mb_data->first_item;
        mb_data->first_item = newItem;
        newItem->next = pmbi;
    }

    mb_data->item_count ++;

    return insPos;
}

#if 0
static PMBITEM mbRemoveItem (PMENUBTNDATA mb_data, int* pos)
{
    int index = 0;
    PMBITEM pmbi, prev;

    if (!mb_data->first_item)
        return NULL;

    if (*pos < 0) {
        prev = mb_data->first_item;
        pmbi = mb_data->first_item;
        while (pmbi->next) {
            prev = pmbi;
            pmbi = pmbi->next;
            index ++;
        }

        if (pmbi == mb_data->first_item) {
            mb_data->first_item = mb_data->first_item->next;
            *pos = 0;
            return pmbi;
        }
        else {
            prev->next = pmbi->next;
            *pos = index;
            return pmbi;
        }
    }
    else if (*pos == 0) {
        pmbi = mb_data->first_item;
        mb_data->first_item = pmbi->next;
        return pmbi;
    }
    else {
        index = 0;
        prev = mb_data->first_item;
        pmbi = mb_data->first_item;
        while (pmbi->next) {
            if (*pos == index)
                break;

            prev = pmbi;
            pmbi = pmbi->next;
            index ++;
        }

        if (pmbi == mb_data->first_item) {
            mb_data->first_item = mb_data->first_item->next;
            *pos = 0;
            return pmbi;
        }
        else {
            prev->next = pmbi->next;
            *pos = index;
            return pmbi;
        }
    }

    return NULL;
}
#else
static PMBITEM mbRemoveItem (PMENUBTNDATA mb_data, int* pos)
{
    int index = 0;
    PMBITEM pmbi, prev;

    if (!mb_data->first_item)
        return NULL;

    if (*pos < 0) {
        return NULL;
    }
    else if (*pos == 0) {
        pmbi = mb_data->first_item;
        mb_data->first_item = pmbi->next;
        return pmbi;
    }
    else { /* index > 0 */
        index = 0;
        prev = mb_data->first_item;
        pmbi = mb_data->first_item;
        while (pmbi->next) {
            if (*pos == index) {
                break;
            }

            prev = pmbi;
            pmbi = pmbi->next;
            index ++;
        }

        if (*pos <= index) {
            prev->next = pmbi->next;
            return pmbi;
        }
    }

    return NULL;
}
#endif

static PMBITEM mbGetItem (PMENUBTNDATA mb_data, int pos)
{
    int index = 0;
    PMBITEM pmbi;

    pmbi = mb_data->first_item;

    while (pmbi) {
        if (index == pos)
            break;

        index ++;
        pmbi = pmbi->next;
    }

    return pmbi;
}

static void mbGetRects (HWND hWnd, DWORD dwStyle,
                                    RECT* prcClient,
                                    RECT* prcText,
                                    RECT* prcMenuBar)
{
    GetClientRect (hWnd, prcClient);
    prcClient->right --;
    prcClient->bottom --;

    if (dwStyle & MBS_LEFTARROW) {
        SetRect (prcMenuBar, prcClient->left + _WIDTH_BORDER,
                      (prcClient->top + prcClient->bottom - _HEIGHT_MENUBAR) >> 1,
                      prcClient->left + _WIDTH_BORDER + _WIDTH_MENUBAR,
                      (prcClient->top + prcClient->bottom + _HEIGHT_MENUBAR) >> 1);

        SetRect (prcText, prcMenuBar->right + _INTER_BARTEXT,
                      prcClient->top + _WIDTH_BORDER,
                      prcClient->right - _INTER_BARTEXT,
                      prcClient->bottom - _WIDTH_BORDER);

    }
    else {
        SetRect (prcText, prcClient->left + _WIDTH_BORDER,
                      prcClient->top + _WIDTH_BORDER,
                      prcClient->right - _WIDTH_MENUBAR - (_INTER_BARTEXT << 1),
                      prcClient->bottom - _WIDTH_BORDER);

        SetRect (prcMenuBar, prcText->right + _INTER_BARTEXT,
                      (prcClient->top + prcClient->bottom - _HEIGHT_MENUBAR) >> 1,
                      prcClient->right - _INTER_BARTEXT,
                      (prcClient->top + prcClient->bottom + _HEIGHT_MENUBAR) >> 1);
    }
}

#define _OFF_CMDID      100

static HMENU mbPopupMenu (HWND hWnd)
{
    HMENU hmnu;
    MENUITEMINFO mii;
    PMENUBTNDATA mb_data;
    int index = 0;
    PMBITEM pmbi;
    RECT rc;

    mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetWindowCaption (hWnd);
    if (!(hmnu = CreatePopupMenu (&mii)))
        return 0;

    pmbi = mb_data->first_item;
    while (pmbi) {

        memset (&mii, 0, sizeof(MENUITEMINFO));
        mii.state       = 0;
        mii.id          = index + _OFF_CMDID;
        if (pmbi->text && pmbi->bmp) {
            mii.type        = MFT_BMPSTRING;
            mii.uncheckedbmp = pmbi->bmp;
            mii.checkedbmp= pmbi->bmp;
            mii.typedata    = (DWORD)pmbi->text;
        }
        else if (pmbi->text) {
            mii.type        = MFT_STRING;
            mii.typedata    = (DWORD)pmbi->text;
        }
        else if (pmbi->bmp) {
            mii.type        = MFT_BITMAP;
            mii.typedata    = (DWORD)pmbi->bmp;
        }
        else
            goto error;

        if (InsertMenuItem (hmnu, index, TRUE, &mii))
            goto error;

        index ++;
        pmbi = pmbi->next;
    }

    if (index == 0) goto error;

    GetClientRect (hWnd, &rc);
    ClientToScreen (hWnd, &rc.left, &rc.top);
    TrackPopupMenu (GetPopupSubMenu (hmnu), TPM_LEFTALIGN,
                                    rc.left, rc.top, hWnd);

    return hmnu;

error:
    DestroyMenu (hmnu);
    return 0;
}

static LRESULT MenuButtonCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PMENUBTNDATA mb_data;
    WINDOWINFO  *wnd_info;
    
    wnd_info = (WINDOWINFO*)GetWindowInfo (hWnd);
    if (wnd_info == NULL)
    return -1;

    switch (message) {
    case MSG_CREATE:
        if ((mb_data = (MENUBTNDATA *) calloc (1, sizeof(MENUBTNDATA))) == NULL)
            return -1;

        SetWindowAdditionalData2 (hWnd, (DWORD) mb_data);
        if (!mbInitMenuButtonData (mb_data, DEF_MB_BUFFER_LEN)) {
            free (mb_data);
            return -1;
        }
        break;
       
    case MSG_DESTROY:
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        mbMenuButtonCleanUp (mb_data);
        free (mb_data);
        break;
        
    case MBM_SETSTRCMPFUNC:
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        if (mb_data->item_count == 0 && lParam) {
            mb_data->str_cmp = (STRCMP)lParam;
            return MB_OKAY;
        }
        return MB_ERR;

    case MBM_ADDITEM:
    {
        PMBITEM newItem;
        int pos;
        PMENUBUTTONITEM want = (PMENUBUTTONITEM)lParam;
        DWORD style = GetWindowStyle (hWnd);
#if _USE_FIXSTR
        int len;
#endif
        
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        newItem = mbAllocItem (mb_data);
        if (!newItem) {
            NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_ERRSPACE);
            return MB_ERR_SPACE;
        }

#if _USE_FIXSTR
        if (want->text) {
            len = strlen (want->text);
            newItem->text = FixStrAlloc (len);
            if (!newItem->text) {
                mbFreeItem (mb_data, newItem);
                return MB_ERR_SPACE;
            }
            if (len > 0)
                strcpy (newItem->text, want->text);
        }
        else {
            newItem->text = "";
        }
#else
        if (want->text) {
            newItem->text = strdup (want->text);
            if (!newItem->text) {
                mbFreeItem (mb_data, newItem);
                return MB_ERR_SPACE;
            }
        }
        else {
            newItem->text = "";
        }
#endif
        
        newItem->bmp  = want->bmp;
        newItem->data = want->data;
        pos = mbAddNewItem (style, mb_data, newItem, (int)wParam);
        return pos;
    }
        
    case MBM_DELITEM:
    {
        PMBITEM removed;
        int delete = (int)wParam;

        mb_data = (PMENUBTNDATA) GetWindowAdditionalData2 (hWnd);
        removed = mbRemoveItem (mb_data, &delete);
        if (removed) {
#if _USE_FIXSTR
            FreeFixStr (removed->text);
#else
            free (removed->text);
#endif
            mbFreeItem (mb_data, removed);

            mb_data->item_count --;

            if (mb_data->cur_sel == delete) {
                mb_data->cur_sel = MB_INV_ITEM;
                UpdateWindow(hWnd, TRUE);
            }

            if (mb_data->cur_sel > 0 && mb_data->cur_sel > delete)
                mb_data->cur_sel -= 1;

            return MB_OKAY;
        }
        return MB_INV_ITEM;
    }

    case MBM_RESETCTRL:
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        mbResetMenuButtonContent (mb_data);
        InvalidateRect (hWnd, NULL, TRUE);
        return MB_OKAY;
        
    case MBM_GETCURITEM:
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        return mb_data->cur_sel;
        
    case MBM_SETCURITEM:
    {
        int old, new;

        new = (int)wParam;
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        old = mb_data->cur_sel;
        if (new >= 0 && new < mb_data->item_count) {
            mb_data->cur_sel = new;
            InvalidateRect (hWnd, NULL, TRUE);
        }

        return old;
    } 

    case MBM_SETITEMDATA:
    {
        PMBITEM pmbi;
        PMENUBUTTONITEM want = (PMENUBUTTONITEM)lParam;

        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        pmbi = mbGetItem (mb_data, (int)wParam);
        if (pmbi == NULL)
            return MB_INV_ITEM;

        if (want->which & MB_WHICH_TEXT) {
#if _USE_FIXSTR
            int len;

            FreeFixStr (pmbi->text);
            len = strlen (want->text);
            pmbi->text = FixStrAlloc (len);
            if (!pmbi->text) {
                pmbi->text = "";
                return MB_ERR_SPACE;
            }
            if (len > 0)
                strcpy (pmbi->text, want->text);
#else
            free (pmbi->text);
            pmbi->text = strdup (want->text);
            if (!pmbi->text) {
                pmbi->text = "";
                return MB_ERR_SPACE;
            }
#endif
        }
        if (want->which & MB_WHICH_BMP) {
            pmbi->bmp = want->bmp;
        }
        if (want->which & MB_WHICH_ATTDATA) {
            pmbi->data = want->data;
        }

        if ((int)wParam == mb_data->cur_sel)
            InvalidateRect (hWnd, NULL, TRUE);
        return MB_OKAY;
    }

    case MBM_GETITEMDATA:
    {
        PMBITEM pmbi;
        PMENUBUTTONITEM want = (PMENUBUTTONITEM)lParam;

        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        pmbi = mbGetItem (mb_data, (int)wParam);
        if (pmbi == NULL)
            return MB_INV_ITEM;

        if (want->which & MB_WHICH_TEXT) {
            want->text = pmbi->text;
        }
        if (want->which & MB_WHICH_BMP) {
            want->bmp = pmbi->bmp;
        }
        if (want->which & MB_WHICH_ATTDATA) {
            want->data = pmbi->data;
        }
        return MB_OKAY;
    }

    case MSG_CHAR:
        if (wParam == ' ') {
            mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
            mb_data->hmnu = mbPopupMenu (hWnd);
            if (mb_data->hmnu)
                NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_STARTMENU);
        }
        break;
        
    case MSG_LBUTTONDOWN:
        SetCapture (hWnd);
        break;
            
    case MSG_LBUTTONUP:
    {
        int x, y;
        RECT rcClient, rcText, rcMenuBar;

        if (GetCapture() != hWnd)
            break;
        ReleaseCapture ();

        x = LOSWORD(lParam);
        y = HISWORD(lParam);
        ScreenToClient (hWnd, &x, &y);
        mbGetRects (hWnd, GetWindowStyle (hWnd),
                        &rcClient, &rcText, &rcMenuBar);
        if (PtInRect (&rcMenuBar, x, y)) {
            mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
            mb_data->hmnu = mbPopupMenu (hWnd);
            if (mb_data->hmnu)
                NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_STARTMENU);
        }
        else if (PtInRect (&rcClient, x, y)) {
            NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_CLICKED);
        }
        break;
    }

    case MSG_ENDTRACKMENU:
    {
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        if (mb_data->hmnu) {
            DestroyMenu (mb_data->hmnu);
            NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_ENDMENU);
            mb_data->hmnu = 0;
        }
        break;
    }

    case MSG_COMMAND:
    {
        int index = wParam - _OFF_CMDID;

        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        if (index < mb_data->item_count && index >= 0) {
            if (index != mb_data->cur_sel) {
                mb_data->cur_sel = index;
                InvalidateRect (hWnd, NULL, TRUE);
                NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_CHANGED);
            }
            NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_SELECTED);
        }

        break;
    }

    case MSG_PAINT:
    {
        HDC hdc;
        RECT rcClient, rcText, rcMenuBar;
        UINT uFormat;
        DWORD dwStyle = GetWindowStyle (hWnd), dwExStyle = GetWindowExStyle(hWnd);
        const char* text;
        BITMAP* bmp;
        DWORD fg_color;
        gal_pixel fc;
        
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        mbGetRects (hWnd, dwStyle,
                        &rcClient, &rcText, &rcMenuBar);

        if (mb_data->cur_sel < 0) {
            text = GetWindowCaption (hWnd);
            bmp = NULL;
        }
        else {
            PMBITEM pmbi = mbGetItem (mb_data, mb_data->cur_sel);
            text = pmbi->text;
            bmp = pmbi->bmp;
        }

        hdc = BeginPaint (hWnd);
        if (!(dwStyle & MBS_NOBUTTON)){
            fg_color = GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY);
            
            if (dwExStyle & WS_EX_TRANSPARENT)
                wnd_info->we_rdr->draw_3dbox (hdc, &rcClient, fg_color,0);
            else 
                wnd_info->we_rdr->draw_3dbox (hdc, &rcClient, fg_color,
                        LFRDR_3DBOX_FILLED);
        }

        if (bmp) {
            static int top = 0, height = 0;
            if (rcText.bottom - rcText.top < bmp->bmHeight) {
                top = rcText.top;
                height = rcText.bottom - rcText.top;
            }
            else {
                top = (rcText.top + rcText.bottom - bmp->bmHeight) >>1;
                height = bmp->bmHeight;
            }

            if (dwStyle & MBS_LEFTARROW) {
                FillBoxWithBitmapPart (hdc,  
                            rcMenuBar.right + _INTER_BARTEXT, 
                            top,
                            bmp->bmWidth,
                            height,
                            0, 0,
                            bmp,
                            0,
                            0);
                /*
                FillBoxWithBitmap (hdc, 
                            rcMenuBar.right + _INTER_BARTEXT, 
                            //(rcText.top + rcText.bottom - bmp->bmHeight) >> 1, 
                            top,
                            0, 0, bmp);
                            */
                rcText.left += bmp->bmWidth + _INTER_BARTEXT;
            }
            else {
                FillBoxWithBitmapPart (hdc,  
                            rcMenuBar.left - _INTER_BARTEXT -  bmp->bmWidth, 
                            top,
                            bmp->bmWidth,
                            height,
                            0, 0,
                            bmp,
                            0,
                            0);
                /*
                FillBoxWithBitmap (hdc, 
                            rcMenuBar.left - _INTER_BARTEXT -  bmp->bmWidth, 
                            (rcText.top + rcText.bottom - bmp->bmHeight) >> 1, 
                            0, 0, bmp);
                            */
                rcText.right -= bmp->bmWidth + _INTER_BARTEXT;
            }
        }

        rcText.left +=  (_INTER_BARTEXT >> 1);

        if (text) {
            uFormat = DT_SINGLELINE | DT_VCENTER;
            switch (dwStyle & MBS_ALIGNMASK) {
                case MBS_ALIGNRIGHT:
                    uFormat |= DT_RIGHT;
                    break;
                case MBS_ALIGNCENTER:
                    uFormat |= DT_CENTER;
                    break;
                default:
                    uFormat |= DT_LEFT;
                    break;
            }
            SetBkMode (hdc, BM_TRANSPARENT);

            if (dwStyle & WS_DISABLED) {
                wnd_info->we_rdr->disabled_text_out (hWnd, hdc, text, &rcText, 
                                uFormat);
            }
            else {
                fg_color = GetWindowElementAttr (hWnd, WE_FGC_WINDOW);
                fc = RGBA2Pixel (hdc, GetRValue(fg_color), GetGValue(fg_color), 
                               GetBValue(fg_color), GetAValue(fg_color));
                SetTextColor (hdc, fc);
                DrawText (hdc, text, -1, &rcText, uFormat);
            }
        }

        fg_color = GetWindowElementAttr (hWnd, WE_FGC_THREED_BODY);
        wnd_info->we_rdr->draw_arrow (hWnd, hdc, &rcMenuBar, 
                fg_color, LFRDR_ARROW_DOWN);

        EndPaint (hWnd, hdc);
          
        return 0;
    }

    default:
        break;
    }

    return DefaultControlProc (hWnd, message, wParam, lParam);
}

#endif /* _MGCTRL_MENUBUTTON */
