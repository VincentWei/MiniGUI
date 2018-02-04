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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/ctrl/button.h>

#define INIT_W  30
#define INIT_H  30

#define INTERVAL    10
#define ITEM_NUM_LINE 20

#define VAL_DEBUG
#define INFO_DEBUG
#define FUN_DEBUG
#include "my_debug.h"

enum {
    MPOS_3DBOX,
    MPOS_RADIO,
    MPOS_CHECKBOX,
    MPOS_CHECKMARK,
    MPOS_ARROW,
    MPOS_PUSHBUTTON,
    MPOS_CHECKBUTTON,
    MPOS_RADIOBUTTON,
    MPOS_FOLD,
    MPOS_ITEM,         
};

#define INSERT_MENU_ITEM(hmenu, mii, string, pos) \
    do { \
        HMENU hsubm; \
        (mii).id = pos; \
        (mii).type = MFT_STRING; \
        (mii).typedata = (DWORD)(string); \
        (mii).hsubmenu = 0; \
        hsubm = CreatePopupMenu (&mii); \
        InsertMenuItem(hsubm, pos, MF_BYCOMMAND, &(mii)); \
        (mii).hsubmenu = hsubm; \
        InsertMenuItem(hmenu, pos, MF_BYCOMMAND, &(mii)); \
    }while (0)

static HMENU create_menu (void)
{
    HMENU hmenu;
    MENUITEMINFO mii;
    hmenu = CreateMenu ();
    memset (&mii, 0, sizeof(MENUITEMINFO));

    /*insert to position every time, insert from last to first */
    INSERT_MENU_ITEM(hmenu, mii, "3dbox", MPOS_3DBOX);
    INSERT_MENU_ITEM(hmenu, mii, "radio", MPOS_RADIO);
    INSERT_MENU_ITEM(hmenu, mii, "checkbox", MPOS_CHECKBOX);
    INSERT_MENU_ITEM(hmenu, mii, "checkmark", MPOS_CHECKMARK);
    INSERT_MENU_ITEM(hmenu, mii, "arrow", MPOS_ARROW);
    INSERT_MENU_ITEM(hmenu, mii, "pushbutton", MPOS_PUSHBUTTON);
    INSERT_MENU_ITEM(hmenu, mii, "checkbutton", MPOS_CHECKBUTTON);
    INSERT_MENU_ITEM(hmenu, mii, "radiobutton", MPOS_RADIOBUTTON);
    INSERT_MENU_ITEM(hmenu, mii, "fold", MPOS_FOLD);
    INSERT_MENU_ITEM(hmenu, mii, "item", MPOS_ITEM);
    return hmenu;
}

typedef struct _DRAW_ARGS
{
    int which;
    int x;
    int y;
    int w;
    int h;
    DWORD status;
    int next;
}DRAW_ARGS;

#define EVERY_COLOR(r, g, b) \
    for (r=0; r<=256; r+=51) \
    for (g=0; g<=256; g+=51) \
    for (b=0; b<=256; b+=51) 

static int  change_rect (RECT* rc, int offx, int offy, int i, int win_h)
{
    int w;
    rc->left += offx;
    rc->right += offx;
    if (i >= ITEM_NUM_LINE)
    {
        rc->top += offy;
        rc->bottom += offy;

        if (rc->top > win_h)
            return -1;

        w = rc->right - rc->left;
        rc->left = INTERVAL;
        rc->right = INTERVAL + w;
        return 1;
    }

    return 0;
}

static int draw_items (HWND hWnd)
{
    const WINDOWINFO* win_info;
    const WINDOW_ELEMENT_RENDERER* rdr;

    HDC hdc;
    int r,g,b;
    int win_w; 
    int win_h;
    int offx;
    int offy;
    int status;
    int i = 0;

    RECT rc;

    DRAW_ARGS* draw_args;

    win_info = GetWindowInfo (hWnd);
    rdr = win_info->we_rdr;
    win_w = win_info->cr - win_info->cl;
    win_h = win_info->cb - win_info->ct;
    draw_args = (DRAW_ARGS*)GetWindowAdditionalData (hWnd);

    status = draw_args->status;

    offx = draw_args->w + INTERVAL;
    offy = draw_args->h + INTERVAL;

    rc.left = INTERVAL;
    rc.top = INTERVAL;
    rc.right = INTERVAL + draw_args->w;
    rc.bottom = INTERVAL + draw_args->h;

    hdc = BeginPaint(hWnd);

    switch (draw_args->which)
    {
        case MPOS_3DBOX:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_3dbox (hdc, &rc, MakeRGBA(r, g, b, 0xFF), status);
                switch (change_rect (&rc, offx, offy, i, win_h))
                {
                    case 0: 
                        i++;
                        continue;
                    case 1:
                        i = 0;
                        continue;
                    case -1:
                        goto end;
                }
            }
            break;

        case MPOS_RADIO:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_radio (hdc, &rc, MakeRGBA(r, g, b, 0xFF), status);
                switch (change_rect (&rc, offx, offy, i, win_h))
                {
                    case 0: 
                        i++;
                        continue;
                    case 1:
                        i = 0;
                        continue;
                    case -1:
                        goto end;
                }
            }
            break;
        case MPOS_CHECKBOX:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_checkbox (hdc, &rc, MakeRGBA(r, g, b, 0xFF), status);
                switch (change_rect (&rc, offx, offy, i, win_h))
                {
                    case 0: 
                        i++;
                        continue;
                    case 1:
                        i = 0;
                        continue;
                    case -1:
                        goto end;
                }
            }
            break;
        case MPOS_CHECKMARK:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_checkmark (hdc, &rc, MakeRGBA(r, g, b, 0xFF), status);
                switch (change_rect (&rc, offx, offy, i, win_h))
                {
                    case 0: 
                        i++;
                        continue;
                    case 1:
                        i = 0;
                        continue;
                    case -1:
                        goto end;
                }
            }
            break;
        case MPOS_ARROW:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_arrow (hWnd, hdc, &rc, MakeRGBA(r, g, b, 0xFF), status);
                switch (change_rect (&rc, offx, offy, i, win_h))
                {
                    case 0: 
                        i++;
                        continue;
                    case 1:
                        i = 0;
                        continue;
                    case -1:
                        goto end;
                }
            }
            break;
        case MPOS_PUSHBUTTON:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_push_button (hWnd, hdc, &rc, MakeRGBA(r, g, b, 0xFF), 0, status);
                switch (change_rect (&rc, offx, offy, i, win_h))
                {
                    case 0: 
                        i++;
                        continue;
                    case 1:
                        i = 0;
                        continue;
                    case -1:
                        goto end;
                }
            }
            break;

        case MPOS_CHECKBUTTON:
            rdr->draw_check_button (hWnd, hdc, &rc, status);
            break;
        case MPOS_RADIOBUTTON:
            rdr->draw_radio_button (hWnd, hdc, &rc, status);
            break;
        case MPOS_FOLD:
            TEST_VAL (draw_args->next, %d);
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_fold (hWnd, hdc, &rc, MakeRGBA(r, g, b, 0xFF), 
                        status, draw_args->next);
                switch (change_rect (&rc, offx, offy, i, win_h))
                {
                    case 0: 
                        i++;
                        continue;
                    case 1:
                        i = 0;
                        continue;
                    case -1:
                        goto end;
                }
            }
            break;

        case MPOS_ITEM:         
            rdr->draw_normal_item (hWnd, hdc, &rc,
                    GetWindowElementAttr(hWnd, WE_BGC_WINDOW));
            change_rect (&rc, offx, offy, win_w, win_h);

            rdr->draw_significant_item (hWnd, hdc, &rc,
                    GetWindowElementAttr(hWnd, WE_BGC_SIGNIFICANT_ITEM));
            change_rect (&rc, offx, offy, win_w, win_h);

            rdr->draw_hilite_item (hWnd, hdc, &rc,
                    GetWindowElementAttr(hWnd, WE_BGC_HIGHLIGHT_ITEM));
            change_rect (&rc, offx, offy, win_w, win_h);

            rdr->draw_normal_item (hWnd, hdc, &rc,
                    GetWindowElementAttr(hWnd, WE_BGC_SELECTED_ITEM));
            change_rect (&rc, offx, offy, win_w, win_h);

            rdr->draw_normal_item (hWnd, hdc, &rc,
                    GetWindowElementAttr(hWnd, WE_BGC_SELECTED_LOSTFOCUS));
            change_rect (&rc, offx, offy, win_w, win_h);

            rdr->draw_disabled_item (hWnd, hdc, &rc,
                    GetWindowElementAttr(hWnd, WE_BGC_DISABLED_ITEM));

            break;
    }
end:
    SetMapMode (hdc, MM_TEXT);
    EndPaint (hWnd, hdc);
    return 0;
    
}

#define SWITCH_POSE(args) \
    do { \
        switch ((args)->status & LFRDR_BTN_STATUS_MASK) \
        { \
            case LFRDR_BTN_STATUS_NORMAL: \
                (args)->status &= ~LFRDR_BTN_STATUS_MASK; \
                (args)->status |= LFRDR_BTN_STATUS_HILITE; \
                break; \
            case LFRDR_BTN_STATUS_HILITE: \
                (args)->status &= ~LFRDR_BTN_STATUS_MASK; \
                (args)->status |= LFRDR_BTN_STATUS_PRESSED; \
                break; \
            case LFRDR_BTN_STATUS_PRESSED: \
                (args)->status &= ~LFRDR_BTN_STATUS_MASK; \
                (args)->status |= LFRDR_BTN_STATUS_DISABLED; \
                break; \
            case LFRDR_BTN_STATUS_DISABLED: \
                (args)->status &= ~LFRDR_BTN_STATUS_MASK; \
                (args)->status |= LFRDR_BTN_STATUS_NORMAL; \
        } \
    } while (0)

static void change_status (DRAW_ARGS* args, int key)
{
    int old_check;
    switch (key)
    {
        case SCANCODE_CURSORBLOCKUP:
            args->h --;
            return;
        case SCANCODE_CURSORBLOCKDOWN:
            args->h ++;
            return;
        case SCANCODE_CURSORBLOCKLEFT:
            args->w --;
            return;
        case SCANCODE_CURSORBLOCKRIGHT:
            args->w ++;
            return;
        case SCANCODE_EQUAL:
            TEST_INFO ("SCANCODE_KEYPADPLUS");
            if (args->which == MPOS_FOLD)
            {
                TEST_INFO ("next ++");
                args->next++;
            }
            return;
        case SCANCODE_MINUS:
            TEST_INFO ("SCANCODE_KEYPADMINUS");
            if (args->which == MPOS_FOLD)
            {
                TEST_INFO ("next --");
                args->next--;
            }
            return;
    }

    switch (args->which)
    {
        case MPOS_3DBOX:
            switch (key)
            {
                case SCANCODE_ENTER:
                    SWITCH_POSE(args);
                    return;

                case SCANCODE_F:
                    args->status ^= LFRDR_3DBOX_FILLED;
                    return;

                case SCANCODE_T:
                    args->status ^= LFRDR_3DBOX_THICKFRAME;
                    return;
                    
            }
            return;

        case MPOS_RADIO:
        case MPOS_CHECKBOX:
        case MPOS_CHECKMARK:
            if (key == SCANCODE_S)
                args->status ^= LFRDR_MARK_HAVESHELL;
            else if (key == SCANCODE_C)
                args->status ^= LFRDR_MARK_ALL_SELECTED;
            return;
            
        case MPOS_ARROW:
            if (key == SCANCODE_ENTER)
                args->status = (args->status + 1 + 4) & 3;
            return;
            
        case MPOS_PUSHBUTTON:
            if(key == SCANCODE_ENTER)
                SWITCH_POSE(args);
            else if (key == SCANCODE_C)
            {
                old_check = args->status & BST_CHECK_MASK;
                old_check = old_check==BST_UNCHECKED ? BST_INDETERMINATE : (
                        old_check == BST_INDETERMINATE ?      
                        BST_CHECKED : BST_UNCHECKED);
                args->status &= ~BST_CHECK_MASK;
                args->status |= old_check;
            }
            return;

        case MPOS_CHECKBUTTON:
        case MPOS_RADIOBUTTON:
            if(key == SCANCODE_ENTER)
                SWITCH_POSE(args);
            else if (key == SCANCODE_C)
                args->status ^= LFRDR_BTN_STATUS_SELECTED;
            return;

        case MPOS_FOLD:
             switch (key)
             {
                 case SCANCODE_C:
                     args->status ^= LFRDR_TREE_CHILD;
                     break;
                 case SCANCODE_O:
                     args->status ^= LFRDR_TREE_FOLD;
                     break;
                 case SCANCODE_N:
                     args->status ^= LFRDR_TREE_NEXT;
                     break;
             }
            return;

        case MPOS_ITEM:  
            if (key == SCANCODE_ENTER)
                args->status = (args->status + 1 + 4) & 3;
            return;
    }
}


int test_draw_proc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    DRAW_ARGS* draw_args = (DRAW_ARGS*)GetWindowAdditionalData (hWnd);
    switch (message)
    {
        case MSG_PAINT:
            draw_items (hWnd);
            /*draw diff colors of items*/
            return 0;
        case MSG_KEYDOWN:
            change_status (draw_args, wParam);
            InvalidateRect (hWnd, NULL, TRUE);
            break;

        case MSG_COMMAND:
            TEST_INFO ("MSG_COMMAND");
            draw_args->which = (wParam);
            TEST_VAL(draw_args->which, %d);
            draw_args->x = 0;
            draw_args->y = 0;
            draw_args->w = INIT_W;
            draw_args->h = INIT_H;
            draw_args->status = 0;
            draw_args->next = 0;
            InvalidateRect (hWnd, NULL, TRUE);
            break;
        case MSG_ACTIVEMENU:
            TEST_INFO ("MSG_ACTIVEMENU");
            break;
    }
    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}




int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
    DRAW_ARGS draw_args;

    draw_args.which = 0;
    draw_args.x = 0;
    draw_args.y = 0;
    draw_args.w = INIT_W;
    draw_args.h = INIT_H;
    draw_args.status = 0;
    draw_args.next = 0;
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION |
                         WS_HSCROLL |WS_VSCROLL | WS_MAXIMIZE |
                         WS_MINIMIZE;

    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "test draw";
    CreateInfo.hMenu = create_menu();
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = test_draw_proc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 400;
    CreateInfo.by = 300;
    CreateInfo.iBkColor = COLOR_black;
    CreateInfo.dwAddData = (DWORD)&draw_args;
    CreateInfo.hHosting = (DWORD)HWND_DESKTOP;
    
    hMainWnd = CreateMainWindowEx (&CreateInfo, "classic", NULL, NULL, NULL);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return 0;
}
