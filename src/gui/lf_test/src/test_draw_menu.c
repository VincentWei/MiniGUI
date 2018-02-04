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

#define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))

enum {
    MPOS_3DBOX,
    MPOS_RADIO,
    MPOS_CHECKBOX,
    MPOS_CHECKMARK,

    MPOS_ARROW,
    MPOS_PUSHBUTTON,
    MPOS_FOLD,
    MPOS_ITEM,
    MPOS_OTHER,

    MIID_RADIOBUTTON = 100,
    MIID_CHECKBUTTON,
};

typedef struct _MII_CREATE
{
    DWORD type;
    char* str;
    DWORD state;
    DWORD draw_status;
    DWORD draw_status_mask;
    int start;
    int end;
}MII_CREATE;

static MII_CREATE tridbox_miic [] = 
{
    {MFT_STRING, "3dbox", 0},
    {MFT_MARKCHECK, "thick frame", 0, LFRDR_3DBOX_THICKFRAME},
    {MFT_MARKCHECK, "fill", 0, LFRDR_3DBOX_FILLED},

    {MFT_RADIOCHECK, "normal", MFS_CHECKED, LFRDR_BTN_STATUS_NORMAL, 
        LFRDR_BTN_STATUS_MASK, 2, 5},
    {MFT_RADIOCHECK, "hilite", 0, LFRDR_BTN_STATUS_HILITE, 
        LFRDR_BTN_STATUS_MASK,2, 5},
    {MFT_RADIOCHECK, "pressed", 0, LFRDR_BTN_STATUS_PRESSED, 
        LFRDR_BTN_STATUS_MASK,2, 5},
    {MFT_RADIOCHECK, "disable", 0, LFRDR_BTN_STATUS_DISABLED, 
        LFRDR_BTN_STATUS_MASK,2, 5},
};

static MII_CREATE radio_miic [] = 
{
    {MFT_STRING, "radio", 0},
    {MFT_MARKCHECK, "shell", MFS_CHECKED, LFRDR_MARK_HAVESHELL},
    {MFT_RADIOCHECK, "no selected", MFS_CHECKED, 0,
            LFRDR_MARK_SELECTED_MASK, 1, 3},
    {MFT_RADIOCHECK, "all selected", 0, LFRDR_MARK_ALL_SELECTED,
            LFRDR_MARK_SELECTED_MASK, 1, 3},
    {MFT_RADIOCHECK, "half selected", 0, LFRDR_MARK_HALF_SELECTED,
            LFRDR_MARK_SELECTED_MASK, 1, 3},
};


static MII_CREATE checkbox_miic [] = 
{
    {MFT_STRING, "checkbox", 0},

    {MFT_MARKCHECK, "shell", MFS_CHECKED, LFRDR_MARK_HAVESHELL},

    {MFT_RADIOCHECK, "no selected", MFS_CHECKED, 0,
            LFRDR_MARK_SELECTED_MASK, 1, 3},
    {MFT_RADIOCHECK, "all selected", 0, LFRDR_MARK_ALL_SELECTED,
            LFRDR_MARK_SELECTED_MASK, 1, 3},
    {MFT_RADIOCHECK, "half selected", 0, LFRDR_MARK_HALF_SELECTED,
            LFRDR_MARK_SELECTED_MASK, 1, 3},
};

static MII_CREATE checkmark_miic [] = 
{
    {MFT_STRING, "checkmark", 0},

    {MFT_MARKCHECK, "shell", MFS_CHECKED, LFRDR_MARK_HAVESHELL},

    {MFT_RADIOCHECK, "no selected", MFS_CHECKED, 0,
            LFRDR_MARK_SELECTED_MASK, 1, 3},
    {MFT_RADIOCHECK, "all selected", 0, LFRDR_MARK_ALL_SELECTED,
            LFRDR_MARK_SELECTED_MASK, 1, 3},
    {MFT_RADIOCHECK, "half selected", 0, LFRDR_MARK_HALF_SELECTED,
            LFRDR_MARK_SELECTED_MASK, 1, 3},
};

static MII_CREATE arrow_miic [] = 
{
    {MFT_STRING, "arrow", 0},
    {MFT_MARKCHECK, "shell", 0, LFRDR_ARROW_HAVESHELL},
    {MFT_MARKCHECK, "no fill", 0, LFRDR_ARROW_NOFILL},
    {MFT_RADIOCHECK, "left", MFS_CHECKED, LFRDR_ARROW_LEFT,
            LFRDR_ARROW_DIRECT_MASK, 2, 5},
    {MFT_RADIOCHECK, "right", 0, LFRDR_ARROW_RIGHT,
            LFRDR_ARROW_DIRECT_MASK, 2, 5},
    {MFT_RADIOCHECK, "up", 0, LFRDR_ARROW_UP,
            LFRDR_ARROW_DIRECT_MASK, 2, 5},
    {MFT_RADIOCHECK, "down", 0, LFRDR_ARROW_DOWN,
            LFRDR_ARROW_DIRECT_MASK, 2, 5},
};

static MII_CREATE pushbtn_miic [] = 
{
    {MFT_STRING, "pushbtn", 0},

    {MFT_RADIOCHECK, "unchecked", MFS_CHECKED, BST_UNCHECKED, 
            BST_CHECK_MASK, 0, 2},
    {MFT_RADIOCHECK, "checked", 0, BST_CHECKED,
            BST_CHECK_MASK, 0, 2},
    {MFT_RADIOCHECK, "indeterminate", 0, BST_INDETERMINATE,
            BST_CHECK_MASK, 0, 2},


    {MFT_RADIOCHECK, "normal", MFS_CHECKED, BST_NORMAL,
            BST_POSE_MASK, 3, 6},
    {MFT_RADIOCHECK, "hilite", 0, BST_HILITE,
            BST_POSE_MASK, 3, 6},
    {MFT_RADIOCHECK, "pushed", 0, BST_PUSHED,
            BST_POSE_MASK, 3, 6},
    {MFT_RADIOCHECK, "disable", 0, BST_DISABLE,
            BST_POSE_MASK, 3, 6},
};

static MII_CREATE fold_miic [] = 
{
    {MFT_STRING, "fold", 0},

    {MFT_MARKCHECK, "have child", 0, LFRDR_TREE_CHILD},
    {MFT_MARKCHECK, "have next", 0, LFRDR_TREE_NEXT},
    {MFT_MARKCHECK, "fold", 0, LFRDR_TREE_FOLD},
};

static MII_CREATE item_miic [] = 
{
    {MFT_STRING, "item", 0},

    {MFT_RADIOCHECK, "normal", MFS_CHECKED, 0,
            BST_POSE_MASK, 0, 3},
    {MFT_RADIOCHECK, "hilite", 0, 1,
            BST_POSE_MASK, 0, 3},
    {MFT_RADIOCHECK, "disable", 0, 2,
            BST_POSE_MASK, 0, 3},
    {MFT_RADIOCHECK, "significant", 0, 3,
            BST_POSE_MASK, 0, 3},
};

static MII_CREATE  other_miic [] = 
{
    {MFT_STRING, "other", 0},

    {MFT_STRING, "radio button", 0},
    {MFT_STRING, "check button", 0},
};

static MII_CREATE* all_miic [] =
{
    tridbox_miic,
    radio_miic,
    checkbox_miic,
    checkmark_miic,

    arrow_miic,
    pushbtn_miic,
    fold_miic,
    item_miic,
    other_miic,
};

static void insert_sub_menu (HMENU hmenu, int pos, MII_CREATE* miic, int len)
{
    MENUITEMINFO mii;
    HMENU hsubmenu;

    /*create submenu and insert to hmenu*/
    memset (&mii, 0, sizeof(mii));
    mii.type = miic->type;
    mii.id = pos;
    mii.typedata = (DWORD)miic->str;
    mii.state = miic->state;
    hsubmenu = CreatePopupMenu (&mii);
    mii.hsubmenu = hsubmenu;
    InsertMenuItem (hmenu, pos, MF_BYPOSITION, &mii);

    /*from last to first insert menu item*/
    mii.hsubmenu = 0;
    miic += (len - 1);
    len -= 2;

    while (len >= 0)
    {
        mii.type = miic->type;
        mii.id = len;
        mii.typedata = (DWORD)miic->str;
        mii.state = miic->state;
        InsertMenuItem (hsubmenu, 0, MF_BYPOSITION, &mii);
        miic--;
        len--;
    }
}

#define INSERT_SUB_MENU(hmenu, pos, miic_array) \
        insert_sub_menu (hmenu, pos, miic_array, ARRAY_LEN(miic_array))

static HMENU create_menu (void)
{
    HMENU hmenu;
    HMENU hsubmenu;
    hmenu = CreateMenu ();
    MENUITEMINFO mii;
    memset (&mii, 0, sizeof(mii));

    /*insert to position every time, insert from last to first */

    INSERT_SUB_MENU(hmenu, MPOS_3DBOX, tridbox_miic);

    INSERT_SUB_MENU(hmenu, MPOS_RADIO, radio_miic);
    INSERT_SUB_MENU(hmenu, MPOS_CHECKBOX, checkbox_miic);
    INSERT_SUB_MENU(hmenu, MPOS_CHECKMARK, checkmark_miic);

    INSERT_SUB_MENU(hmenu, MPOS_ARROW, arrow_miic);
    INSERT_SUB_MENU(hmenu, MPOS_PUSHBUTTON, pushbtn_miic);
    INSERT_SUB_MENU(hmenu, MPOS_FOLD, fold_miic);

    INSERT_SUB_MENU(hmenu, MPOS_ITEM, item_miic);

    mii.type = MFT_STRING;
    mii.id = MPOS_OTHER;
    mii.typedata = (DWORD)"other";
    hsubmenu = CreatePopupMenu (&mii);
    mii.hsubmenu = hsubmenu;
    InsertMenuItem (hmenu, MPOS_OTHER, MF_BYPOSITION, &mii);

    mii.hsubmenu = 0;

    mii.type = MFT_STRING;
    mii.id = MIID_CHECKBUTTON;
    mii.typedata = (DWORD)"checkbutton";
    InsertMenuItem (hsubmenu, 0, MF_BYPOSITION, &mii);

    mii.type = MFT_STRING;
    mii.id = MIID_RADIOBUTTON;
    mii.typedata = (DWORD)"radio button";
    InsertMenuItem (hsubmenu, 1, MF_BYPOSITION, &mii);

    return hmenu;
}

typedef struct _MENU_CHECK {
    BOOL is_radio;
    int start;
    int end;
} MENU_CHECK;

static void change_mi_state (HMENU hmenu, int item_pos, int which)
{
    MENUITEMINFO mii;
    mii.mask = MIIM_STATE|MIIM_TYPE;
    MII_CREATE* miic = all_miic[which] + item_pos + 1;
    
    TEST_VAL (which, %d);
    TEST_VAL (item_pos, %d);
    if (which == MPOS_OTHER)
        return;

    GetMenuItemInfo (hmenu, item_pos, MF_BYPOSITION, &mii);

    if (mii.type == MFT_RADIOCHECK)
    {
        TEST_VAL (miic->start, %d);
        TEST_VAL (miic->end, %d);
        CheckMenuRadioItem (hmenu, miic->start, miic->end,
                item_pos, MF_BYPOSITION);
    }
    else if (mii.type == MFT_MARKCHECK)
    {
        mii.state ^= MFS_CHECKED;
        SetMenuItemInfo (hmenu, item_pos, MF_BYPOSITION, &mii);
    }
}

static DWORD collect_mi_state (HMENU hmenu, int which)
{
    int item_num;
    MENUITEMINFO mii;
    MII_CREATE* miic;
    int i;
    DWORD status = 0;

    miic = all_miic[which];
    mii.mask = MIIM_STATE|MIIM_TYPE;
    item_num = GetMenuItemCount (hmenu);

    if (which == MPOS_OTHER)
        return 0;

    for (i=0; i<item_num; i++)
    {
        GetMenuItemInfo (hmenu, i, MF_BYPOSITION, &mii);
        if (mii.state == MFS_CHECKED)
            status |= miic[i+1].draw_status;
    }
    return status;
}

typedef struct _DRAW_ARGS
{
    int which;
    int w;
    int h;
    DWORD status;
    int fold_next;

    int next_which;
    int next_hmenu;
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

#define CHANGE_RECT(rc, offx, offy, i, win_h, end) \
    do {\
        switch (change_rect (&rc, offx, offy, i, win_h))\
        {\
            case 0: \
                i++;\
                continue;\
            case 1:\
                i = 0;\
                continue;\
            case -1:\
                goto end;\
        }\
    }while (0)

#define DRAW_AREA(hwnd, hdc, fun, status, rc, offx, offy)\
    do {\
        Rectangle (hdc, (rc).left, (rc).top, (rc).right, (rc).bottom);\
        fun (hwnd, hdc, &rc, status);\
        (rc).left += offx;\
        (rc).right += offx;\
        (rc).top += offy;\
        (rc).bottom += offy;\
    }while (0)

#define DRAW_INFO(hdc, str, rc, offx, offy)\
    do {\
        Rectangle (hdc, (rc).left, (rc).top, (rc).right, (rc).bottom);\
        DrawText (hdc, str, -1, &rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);\
        (rc).left += offx;\
        (rc).right += offx;\
        (rc).top += offy;\
        (rc).bottom += offy;\
    } while (0)

typedef void (*DRAW_FUN) (HWND, HDC, const RECT*, int);

static void draw_cr_button (HWND hWnd, HDC hdc, DRAW_FUN fun, int x, int y,
        int w, int h)
{
    RECT rc;

    rc.left = x;
    rc.right = x + w;
    rc.top = y;
    rc.bottom = y + h;

    SetTextColor (hdc, COLOR_green);
    SetBkMode (hdc, BM_TRANSPARENT);
    SetPenColor (hdc, COLOR_green);

    DRAW_INFO(hdc, "", rc, w, 0);

    DRAW_INFO(hdc, "normal", rc, w, 0);
    DRAW_INFO(hdc, "hilte", rc, w, 0);
    DRAW_INFO(hdc, "pressed", rc, w, 0);
    DRAW_INFO(hdc, "disable", rc, -w*4, h);

    DRAW_INFO(hdc, "unchecked", rc, w, 0);
    DRAW_AREA (hWnd, hdc, fun, LFRDR_BTN_STATUS_NORMAL, rc, w, 0);
    DRAW_AREA (hWnd, hdc, fun, LFRDR_BTN_STATUS_HILITE, rc, w, 0);
    DRAW_AREA (hWnd, hdc, fun, LFRDR_BTN_STATUS_PRESSED, rc, w, 0);
    DRAW_AREA (hWnd, hdc, fun, LFRDR_BTN_STATUS_DISABLED, rc, -w*4, h);

    DRAW_INFO(hdc, "checked", rc, w, 0);
    DRAW_AREA (hWnd, hdc, fun, LFRDR_BTN_STATUS_SELECTED|LFRDR_BTN_STATUS_NORMAL, 
            rc, w, 0);
    DRAW_AREA (hWnd, hdc, fun, LFRDR_BTN_STATUS_SELECTED|LFRDR_BTN_STATUS_HILITE, 
            rc, w, 0);
    DRAW_AREA (hWnd, hdc, fun, LFRDR_BTN_STATUS_SELECTED|LFRDR_BTN_STATUS_PRESSED, 
            rc, w, 0);
    DRAW_AREA (hWnd, hdc, fun, LFRDR_BTN_STATUS_SELECTED|LFRDR_BTN_STATUS_DISABLED, 
            rc, -w*5, h);
}

static int draw_prompting (HDC hdc)
{
    
    RECT rc;
    int h;
    rc.left = INTERVAL;
    rc.top = INTERVAL;
    rc.right = 500;
    rc.bottom = 500;
    SetTextColor (hdc, COLOR_green);
    SetBkMode (hdc, BM_TRANSPARENT);
    h = DrawText(hdc, "please press keyboard:\n", -1, &rc, DT_LEFT);

    rc.top += h;
    DrawText(hdc, 
            "LEFT - minish the width\n"
            "UP - minish the height\n"
            "DOWN - argument the height\n"
            "RIGHT - argument the width",
            -1, &rc, DT_LEFT|DT_CALCRECT);

    h = DrawText(hdc, 
            "LEFT - minish the width\n"
            "UP - minish the height\n"
            "DOWN - argument the height\n"
            "RIGHT - argument the width",
            -1, &rc, DT_LEFT);

    rc.left = rc.right + 3*INTERVAL;
    rc.right = 500;
    DrawText(hdc, 
            "\'+\' - argument the next of draw_fold\n"
            "\'-\' - minish the next of draw_fold\n"
            "ENTER - change the renderer",
            -1, &rc, DT_LEFT);
    return rc.top + h;
}

static int draw (HWND hWnd)
{
    const WINDOWINFO* win_info;
    const WINDOW_ELEMENT_RENDERER* rdr;

    HDC hdc;
    HDC memdc;
    int r,g,b;
    int win_w; 
    int win_h;
    int offx;
    int offy;
    int status;
    int i = 0;
    int promp_h;

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
    memdc = CreateCompatibleDC (hdc);

    SetBrushColor (memdc, COLOR_black);
    FillBox (memdc, 0, 0, win_w, win_h);

    promp_h = draw_prompting (memdc);
    rc.top += promp_h;
    rc.bottom += promp_h;

    switch (draw_args->which)
    {
        case MPOS_3DBOX:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_3dbox (memdc, &rc, MakeRGBA(r, g, b, 0xFF), status);
                CHANGE_RECT(rc, offx, offy, i, win_h, end);
            }
            break;

        case MPOS_RADIO:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_radio (memdc, &rc, MakeRGBA(r, g, b, 0xFF), status);
                CHANGE_RECT(rc, offx, offy, i, win_h, end);
            }
            break;
        case MPOS_CHECKBOX:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_checkbox (memdc, &rc, MakeRGBA(r, g, b, 0xFF), status);
                CHANGE_RECT(rc, offx, offy, i, win_h, end);
            }
            break;
        case MPOS_CHECKMARK:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_checkmark (memdc, &rc, MakeRGBA(r, g, b, 0xFF), status);
                CHANGE_RECT(rc, offx, offy, i, win_h, end);
            }
            break;
        case MPOS_ARROW:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_arrow (hWnd, memdc, &rc, MakeRGBA(r, g, b, 0xFF), status);
                CHANGE_RECT(rc, offx, offy, i, win_h, end);
            }
            break;
        case MPOS_PUSHBUTTON:
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_push_button (hWnd, memdc, &rc, MakeRGBA(r, g, b, 0xFF), 0, status);
                CHANGE_RECT(rc, offx, offy, i, win_h, end);
            }
            break;

        case MPOS_FOLD:
            TEST_VAL (draw_args->fold_next, %d);
            EVERY_COLOR(r, g, b)
            {
                rdr->draw_fold (hWnd, memdc, &rc, MakeRGBA(r, g, b, 0xFF), 
                        status, draw_args->fold_next);
                CHANGE_RECT(rc, 
                        offx + (draw_args-> w), 
                        offy + (draw_args->h * draw_args->fold_next + 
                                (draw_args-> h >> 1)), 
                        i, win_h, end);
            }
            break;

        case MPOS_ITEM:
            EVERY_COLOR(r, g, b)
            {
                switch (status)
                {
                    case 0:
                        rdr->draw_normal_item (hWnd, memdc, &rc, MakeRGBA(r, g, b, 0xFF));
                        break;
                    case 1:
                        rdr->draw_hilite_item (hWnd, memdc, &rc, MakeRGBA(r, g, b, 0xFF));
                        break;
                    case 2:
                        rdr->draw_disabled_item (hWnd, memdc, &rc, MakeRGBA(r, g, b, 0xFF));
                        break;
                    case 3:
                        rdr->draw_significant_item (hWnd, memdc, &rc, MakeRGBA(r, g, b, 0xFF));
                        break;
                    default:
                        assert(0);
                }
                CHANGE_RECT(rc, offx, offy, i, win_h, end);
            }
            break;

        case MIID_CHECKBUTTON:
            TEST_INFO("draw checkbutton");
            draw_cr_button (hWnd, memdc, rdr->draw_check_button, rc.left, rc.top,
                    draw_args->w * 3, draw_args->h);
            break;
        case MIID_RADIOBUTTON:
            draw_cr_button (hWnd, memdc, rdr->draw_radio_button, rc.left, rc.top,
                    draw_args->w * 3, draw_args->h);
            break;
    }
end:

    BitBlt (memdc, 0, 0, 0, 0, hdc, 0, 0, 0);
    DeleteMemDC (memdc);
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

static BOOL change_metrics (DRAW_ARGS* args, int key)
{
    switch (key)
    {
        case SCANCODE_CURSORBLOCKUP:
            args->h --;
            return TRUE;
        case SCANCODE_CURSORBLOCKDOWN:
            args->h ++;
            return TRUE;
        case SCANCODE_CURSORBLOCKLEFT:
            args->w --;
            return TRUE;
        case SCANCODE_CURSORBLOCKRIGHT:
            args->w ++;
            return TRUE;
        case SCANCODE_EQUAL:
            TEST_INFO ("SCANCODE_KEYPADPLUS");
            if (args->which == MPOS_FOLD)
            {
                TEST_INFO ("next ++");
                args->fold_next++;
                return TRUE;
            }
            else
                return FALSE;
        case SCANCODE_MINUS:
            TEST_INFO ("SCANCODE_KEYPADMINUS");
            if (args->which == MPOS_FOLD)
            {
                TEST_INFO ("next --");
                args->fold_next--;
                return TRUE;
            }
            else
                return FALSE;
    }

    return FALSE;

}


int test_draw_proc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    DRAW_ARGS* draw_args = (DRAW_ARGS*)GetWindowAdditionalData (hWnd);
    static char* rdr_names [] = {"classic", "flat", "fashion"};
    static int rdr_index;
    switch (message)
    {
        case MSG_PAINT:
            draw (hWnd);
            /*draw diff colors of items*/
            return 0;
        case MSG_KEYDOWN:
            if (wParam == SCANCODE_ENTER)
            {
                rdr_index = (rdr_index + ARRAY_LEN(rdr_names) + 1) % 
                    ARRAY_LEN(rdr_names);
                SetWindowElementRenderer (hWnd, rdr_names[rdr_index], NULL);
            }
            else
            {
                if (change_metrics (draw_args, wParam))
                    InvalidateRect (hWnd, NULL, FALSE);
            }
            break;

        case MSG_ACTIVEMENU:
            TEST_INFO ("MSG_ACTIVEMENU");
            draw_args->next_which = wParam;
            draw_args->next_hmenu = (HMENU)lParam;
            break;

        case MSG_COMMAND:
            TEST_INFO ("MSG_COMMAND");
            switch (wParam)
            {
                case MIID_RADIOBUTTON:
                case MIID_CHECKBUTTON:
                    draw_args->which = wParam;
                    draw_args->status = 0;
                    break;
                default:
                    draw_args->which = draw_args->next_which;
                    change_mi_state (draw_args->next_hmenu, wParam,
                            draw_args->which);
                    draw_args->status = collect_mi_state (draw_args->next_hmenu,
                            draw_args->which);
                    break;
            }
            //draw_args->w = INIT_W;
            //draw_args->h = INIT_H;
            InvalidateRect (hWnd, NULL, FALSE);
            break;
        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
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
    draw_args.w = INIT_W;
    draw_args.h = INIT_H;
    draw_args.status = 0;
    draw_args.fold_next = 0;
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
    CreateInfo.rx = 800;
    CreateInfo.by = 600;
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
