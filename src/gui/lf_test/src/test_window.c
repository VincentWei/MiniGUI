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
#include <minigui/mgutils.h>


//#define VAL_DEBUG
#define INFO_DEBUG
//#define FUN_DEBUG
#include "my_debug.h"

#define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))

/*menu data*/

/*if metrix set it to adddate
 * if - + sub and add it*/

LOGFONT* font_sel_dialog (HWND hWnd);

static int bms_mids [] =
{
    WE_METRICS_WND_BORDER,
    WE_FGC_ACTIVE_WND_BORDER,
    WE_FGC_INACTIVE_WND_BORDER,

    WE_METRICS_MENU,
    WE_FGC_MENU,
    WE_BGC_MENU,
    WE_FONT_MENU,

    WE_METRICS_SCROLLBAR,
};

static char* bms_strs [] = 
{
    "metris of border",
    "fgc of active window border",
    "fgc of inactive window border",

    "metrics of menu",
    "fgc of menu",
    "bgc of menu",
    "font of menu",

    "metrics of scrollbar",
};

static int cap_mids [] =
{
    WE_METRICS_CAPTION,
    WE_FONT_CAPTION,

    WE_FGC_ACTIVE_CAPTION,
    WE_BGCA_ACTIVE_CAPTION,
    WE_BGCB_ACTIVE_CAPTION,

    WE_FGC_INACTIVE_CAPTION,
    WE_BGCA_INACTIVE_CAPTION,
    WE_BGCB_INACTIVE_CAPTION,
};

static char* cap_strs [] =
{
    "caption metrics",
    "caption font",

    "fgc of active caption",
    "bgca of active caption",
    "bgcb of active caption",

    "fgc of inactive caption",
    "bgca of inactive caption",
    "bgcb of inactive caption",
};

static int threed_item_mids [] = 
{
    WE_FGC_THREED_BODY,
    WE_MAINC_THREED_BODY,

    WE_FGC_SELECTED_ITEM,
    WE_BGC_SELECTED_ITEM,
    WE_BGC_SELECTED_LOSTFOCUS,

    WE_FGC_DISABLED_ITEM,
    WE_BGC_DISABLED_ITEM,

    WE_FGC_HIGHLIGHT_ITEM,
    WE_BGC_HIGHLIGHT_ITEM,

    WE_FGC_SIGNIFICANT_ITEM,
    WE_BGC_SIGNIFICANT_ITEM,
};

static char* threed_item_strs [] = 
{
    "fgc of 3dbox",
    "main color of 3dbox main",

    "fgc of selected item",
    "bgc of selected item",
    "bgc of selected item losting focus",

    "fgc of disabled item",
    "bgc of disabled item",

    "fgc of hilite item",
    "bgc of hilite item",

    "fgc of significant item",
    "bgc of significant item",
};

static int other_mids [] = 
{
    WE_FGC_WINDOW,
    WE_BGC_WINDOW,

    WE_BGC_DESKTOP,

    WE_FONT_MESSAGEBOX,
    WE_FGC_MESSAGEBOX,

    WE_FONT_TOOLTIP,
    WE_FGC_TOOLTIP,
    WE_BGC_TOOLTIP
};

static char* other_strs [] = 
{
    "fgc of window",
    "bgc of window",

    "bgc of desktop",

    "font of messagebox",
    "fgc of messagebox",

    "font of tooltip",
    "fgc of tooltip",
    "bgc of tooltip",
};

static HMENU create_sub_menu (char* header, int* ids, char** strs, int mi_num)
{
    HMENU hmenu;
    MENUITEMINFO mii;
    int mi_pos = 0;

    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;
    mii.typedata = (DWORD)header;
    hmenu = CreatePopupMenu(&mii);

    while (mi_num>0)
    {
        mii.id = *ids++;
        mii.typedata = (DWORD)*strs++;
        InsertMenuItem(hmenu, mi_pos, MF_BYPOSITION, &mii);
        mi_pos++;
        mi_num--;
    }
    return hmenu;
}

#if 1
#define INSERT_SEPARATOR(hmenu, pos, mii) \
    do { \
        mii.type = MFT_SEPARATOR;\
        mii.typedata = 0; \
        InsertMenuItem(hmenu, pos, MF_BYPOSITION, &mii);\
    }while(0)

#else
#define INSERT_SEPARATOR(hmenu, pos, mii)  (void)0
#endif
static HMENU create_menu (void)
{
    HMENU hmenu;
    MENUITEMINFO mii;
    hmenu = CreateMenu ();
    memset (&mii, 0, sizeof(MENUITEMINFO));
    
    /*insert to position every time, insert from last to first */
    mii.type = MFT_STRING;
    mii.typedata = (DWORD)"others";
    mii.hsubmenu = create_sub_menu("others",
            other_mids, other_strs, ARRAY_LEN(other_mids));
    InsertMenuItem(hmenu, 0, MF_BYPOSITION, &mii);

    INSERT_SEPARATOR (hmenu, 0, mii);
   
    mii.type = MFT_STRING;
    mii.typedata = (DWORD)"3dbox and items";
    mii.hsubmenu = create_sub_menu("3dbox and items",
            threed_item_mids, threed_item_strs, ARRAY_LEN(threed_item_mids));
    InsertMenuItem(hmenu, 0, MF_BYPOSITION, &mii);

    INSERT_SEPARATOR (hmenu, 0, mii);

    mii.type = MFT_STRING;
    mii.typedata = (DWORD)"caption";
    mii.hsubmenu = create_sub_menu("caption",
            cap_mids, cap_strs, ARRAY_LEN(cap_mids));
    InsertMenuItem(hmenu, 0, MF_BYPOSITION, &mii);

    INSERT_SEPARATOR (hmenu, 0, mii);

    mii.type = MFT_STRING;
    mii.typedata = (DWORD)"border menu and scrollbar";
    mii.hsubmenu = create_sub_menu("border menu and scrollbar",
            bms_mids, bms_strs, ARRAY_LEN(bms_mids));
    InsertMenuItem(hmenu, 0, MF_BYPOSITION, &mii);

    return hmenu;
}

typedef struct _WND_DATA
{
    int old_ht_code;
    int attr_id;
    int (*calc_we_area) (HWND hWnd, int which, RECT* we_area);
    int (*hit_test) (HWND hWnd, int x, int y);

    LOGFONT* old_font_tt;
    LOGFONT* old_font_menu;
    LOGFONT* old_font_cap;
    LOGFONT* old_font_msgbx;

}WND_DATA;

static int init_window(HWND hWnd)
{
    SCROLLINFO si;
    WND_DATA* data;

    si.fMask = SIF_ALL;
    si.nMax = 100;
    si.nMin = 0;
    si.nPage = 10;
    si.nPos = 0;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
    EnableScrollBar(hWnd, SB_HORZ, TRUE);
    EnableScrollBar(hWnd, SB_VERT, TRUE);

    data = malloc(sizeof(WND_DATA));
    if (!data)
        return -1;
    data->old_ht_code = HT_BORDER;
    data->attr_id = WE_METRICS_WND_BORDER;
    data->calc_we_area = GetWindowInfo(hWnd)->we_rdr->calc_we_area;
    data->hit_test = GetWindowInfo(hWnd)->we_rdr->hit_test;

    data->old_font_cap = (LOGFONT*)GetWindowElementAttr (hWnd, WE_FONT_CAPTION);
    data->old_font_tt = (LOGFONT*)GetWindowElementAttr (hWnd, WE_FONT_TOOLTIP);
    data->old_font_msgbx = (LOGFONT*)GetWindowElementAttr (hWnd, WE_FONT_MESSAGEBOX);
    data->old_font_menu = (LOGFONT*)GetWindowElementAttr (hWnd, WE_FONT_MENU);

    SetWindowAdditionalData(hWnd, (DWORD)data);
    return 0;
}

static void free_wnd_data(HWND hWnd)
{
    free ((void*)GetWindowAdditionalData(hWnd));
    SetWindowAdditionalData(hWnd, 0);
}

static void change_wnd_attr (HWND hWnd, int attr_id)
{
    COLORDATA color_data;
    WND_DATA* wnd_data;
    LOGFONT* font;
    LOGFONT* old_font;

    switch (attr_id & WE_ATTR_TYPE_MASK)
    {
        case WE_ATTR_TYPE_COLOR:
            TEST_INFO("get color");
            if (IDOK ==
                    ColorSelDialog(hWnd, 100, 100, 300, 300, &color_data))
            {
                if ((attr_id & WE_ATTR_INDEX_MASK) == WE_MESSAGEBOX ||
                    (attr_id & WE_ATTR_INDEX_MASK) == WE_TOOLTIP||
                    (attr_id & WE_ATTR_INDEX_MASK) == WE_DESKTOP)

                {
                    WINDOW_ELEMENT_RENDERER* def_rdr;
                    def_rdr = (WINDOW_ELEMENT_RENDERER*)GetDefaultWindowElementRenderer();
                    def_rdr->we_colors [attr_id & WE_ATTR_INDEX_MASK] 
                            [(attr_id & WE_ATTR_TYPE_COLOR_MASK) >> 8] =
                            (DWORD)MakeRGBA(color_data.r, color_data.g, 
                                            color_data.b, 0xff);
                    return;
                }
                else
                {
                    SetWindowElementAttr(hWnd, attr_id, 
                            (DWORD)MakeRGBA(color_data.r, color_data.g, 
                                color_data.b, 0xff));

                    printf("set bgc of desktop\n");
                    UpdateWindow (HWND_DESKTOP, TRUE);
                }
            }
            break;

        case WE_ATTR_TYPE_FONT:
            font = font_sel_dialog(hWnd);
            if (font)
            {
                wnd_data = (WND_DATA*)GetWindowAdditionalData(hWnd);

                if ((attr_id & WE_ATTR_INDEX_MASK) == WE_MESSAGEBOX ||
                    (attr_id & WE_ATTR_INDEX_MASK) == WE_TOOLTIP)
                {
                    WINDOW_ELEMENT_RENDERER* def_rdr;
                    def_rdr = (WINDOW_ELEMENT_RENDERER*)GetDefaultWindowElementRenderer();
                    def_rdr->we_fonts[attr_id & WE_ATTR_INDEX_MASK] = font;
                    return;
                }
                else
                    old_font = (LOGFONT*)SetWindowElementAttr (hWnd, attr_id, 
                            (DWORD)font);

                if (attr_id == WE_FONT_MESSAGEBOX)
                    TEST_INFO ("set messagebox font");

                if (font == old_font)
                    return;

                if (old_font != wnd_data->old_font_cap &&
                    old_font != wnd_data->old_font_tt &&
                    old_font != wnd_data->old_font_menu &&
                    old_font != wnd_data->old_font_msgbx)
                {
                    DestroyLogFont (old_font);
                }
            }
            break;

        case WE_ATTR_TYPE_METRICS:
            TEST_INFO("set metrics which to be changed");
            ((WND_DATA*)GetWindowAdditionalData(hWnd))->attr_id = attr_id;
            break;
    }

}

static void change_metrics(HWND hWnd, int key_code)
{
    int which;
    int metrics;
    DWORD style;
    which = ((WND_DATA*)GetWindowAdditionalData(hWnd))->attr_id;
    switch (key_code)
    {
        case SCANCODE_EQUAL:
        case SCANCODE_KEYPADPLUS:
            metrics = GetWindowElementAttr(hWnd, which);
            metrics++;
            SetWindowElementAttr(hWnd, which, metrics);
            UpdateWindow (hWnd, TRUE);
            break;

        case SCANCODE_MINUS:
        case SCANCODE_KEYPADMINUS:
            metrics = GetWindowElementAttr(hWnd, which);
            metrics--;
            SetWindowElementAttr(hWnd, which, metrics);
            UpdateWindow (hWnd, TRUE);
            break;
        case SCANCODE_B:
            style = GetWindowStyle (hWnd);
            if (style & WS_THINFRAME)
            {

                ExcludeWindowStyle (hWnd, WS_THINFRAME);
                IncludeWindowStyle (hWnd, WS_BORDER);
                printf ("border window\n");
            }
            else if (style & WS_BORDER)
            {
                ExcludeWindowStyle (hWnd, WS_BORDER);
                IncludeWindowStyle (hWnd, WS_THICKFRAME);
                printf("thick frame window\n");
            }
            else if (style & WS_THICKFRAME)
            {
                ExcludeWindowStyle (hWnd, WS_THICKFRAME);
                style &= ~WS_THICKFRAME;
                printf("no border window\n");
            }
            else 
            {
                IncludeWindowStyle (hWnd, WS_THINFRAME);
                style |= WS_THINFRAME;
                printf("thin frame window\n");
            }
            UpdateWindow (hWnd, TRUE);
    }
}

static void hittest_calc (HWND hWnd, int x, int y)
{
    int ht_code;
    WND_DATA* wnd_data;
    gal_pixel old_pixel;
    RECT rc;
    HDC hdc;

    wnd_data = (WND_DATA*)GetWindowAdditionalData(hWnd);
    ht_code = wnd_data->hit_test(hWnd, x, y);

    if ((ht_code & HT_BORDER_MASK) == HT_BORDER_MASK)
        ht_code = HT_BORDER;

    if (ht_code == wnd_data->old_ht_code)
    {
        /*move in old area*/
        if (wnd_data->calc_we_area(hWnd, ht_code, &rc) == 0)
        {
            hdc = GetDC(hWnd);
            old_pixel = SetPenColor(hdc, COLOR_green);
            Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
            old_pixel = SetPenColor(hdc, old_pixel);
            ReleaseDC(hdc);
        }
    }
    else
    {
        /*move into new area, erase old rect*/
        UpdateWindow(hWnd, TRUE);
    }
    wnd_data->old_ht_code = ht_code;
}

static int test_window_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static char* rdr_names [] = {"classic", "flat", "fashion", "skin"};
    static int rdr_index;

    switch (message)
    {
        case MSG_NCMOUSEMOVE:
            hittest_calc (hWnd, LOSWORD(lParam), HISWORD(lParam));
            break;

        case MSG_PAINT:
            hdc = BeginPaint(hWnd);

            TabbedTextOut(hdc, 20, 20,
                    "please select menu.\n" 
                    "If you select a metrics,\n"
                    "press \'+ -\' to argument or minish it.\n\n"
                    "Press \'ENTER\' to change the window renderer.\n"
                    "Press \'B\' to change the window border style.\n");
            EndPaint(hWnd, hdc);
            return 0;

        case MSG_KEYDOWN:
            if (wParam == SCANCODE_ENTER)
            {
                WND_DATA* data = (WND_DATA*)GetWindowAdditionalData (hWnd);
                rdr_index = (rdr_index + ARRAY_LEN(rdr_names) + 1) % 
                        ARRAY_LEN(rdr_names);
                SetWindowElementRenderer (hWnd, rdr_names[rdr_index], NULL);
                data->calc_we_area = GetWindowInfo(hWnd)->we_rdr->calc_we_area;
                data->hit_test = GetWindowInfo(hWnd)->we_rdr->hit_test;
            }
            /*add data saves which metrics to be add or sub*/
            else
                change_metrics (hWnd, wParam);
            return 0;

        case MSG_COMMAND:
            printf("get MSG_COMMAND\n");
            change_wnd_attr (hWnd, wParam);
            if ((wParam & WE_ATTR_INDEX_MASK) == WE_MESSAGEBOX)
                MessageBox (hWnd, "messagebox", "messagebox",
                        MB_OK|MB_ICONINFORMATION|MB_ALIGNCENTER);
            else if ((wParam & WE_ATTR_INDEX_MASK) == WE_TOOLTIP)
            {
                HWND htip;
                TEST_INFO ("set tooltop attr");
                htip = createToolTipWin (hWnd, 50, 50, 5000, "Hi!");
            }
            else
                UpdateWindow (hWnd, TRUE);
            return 0;

        case MSG_CREATE:
            init_window (hWnd);
            break;

        case MSG_CLOSE:
            free_wnd_data (hWnd);
            DestroyMainWindow (hWnd);
            break;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;

    HWND hwnd_clas;
    HWND hwnd_flat;
    HWND hwnd_fshn;

    MAINWINCREATE CreateInfo;


#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION |
                         WS_HSCROLL |WS_VSCROLL | WS_MAXIMIZEBOX |
                         WS_MINIMIZEBOX;

    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "classic window";
    CreateInfo.hMenu = create_menu();
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = test_window_proc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 600;
    CreateInfo.by = 300;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hwnd_clas = CreateMainWindowEx (&CreateInfo, "classic", NULL, NULL, NULL);
    CreateInfo.spCaption ="Fashion window";
    hwnd_fshn = CreateMainWindowEx (&CreateInfo, "fashion", NULL, NULL, NULL);
    ShowWindow(hwnd_clas, SW_SHOWNORMAL);
    ShowWindow(hwnd_fshn, SW_SHOWNORMAL);
    
    if (hwnd_clas == HWND_INVALID)
        return -1;

    while (GetMessage(&Msg, hwnd_clas)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return 0;
}
