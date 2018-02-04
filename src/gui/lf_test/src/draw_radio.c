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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "../../../include/fixedmath.h"
/*
#include "../../../include/cliprect.h"
#include "../../../include/gal.h"
#include "../../../include/internals.h"
#include "../../../include/menu.h"
#include "../../../include/ctrlclass.h"
#include "../../../include/element.h"
#include "../../../include/dc.h"
*/
//#define LFRDR_MARK_SELECTED    0x0001
//#define LFRDR_MARK_HAVESHELL   0x0010
#define WIDTH_MINIMUM    6
#if 1
#define UNDEFINED_HUE   (itofix(-1))
#define TO_LIGHTEST_COLOR   2
#define TO_LIGHTER_COLOR    1
#define TO_DARKER_COLOR     (-1)
#define TO_DARKEST_COLOR    (-2)
#define FULL_V    (itofix(1))

static void 
increase_vh(fixed src_v, fixed src_h, fixed* dest_v, fixed* dest_h, 
            BOOL is_half)
{
    int h;
    int delta_h;
    int aim_h;
    if (src_v < FULL_V) {
        src_v = fixadd(src_v, fixdiv(src_v, itofix(3))>>is_half);
        if (src_v >= FULL_V)
            *dest_v = FULL_V;
        else
            *dest_v = src_v;

        *dest_h = src_h;
    }
    else {
        *dest_v = FULL_V;

        h = fixtoi(src_h);
        if (h % 60 == 0)
        {
            *dest_h = src_h;
            return;
        }

        /* change h to nearest (60, 180, 300)*/
        aim_h = h / 120 * 120 +60;
        delta_h = aim_h - h;

        /*not is_halt, nearer to aim_h*/
        *dest_h = itofix (aim_h - ((delta_h >>1)>>!is_half));
    }
}

static void RGB2HSV(fixed r, fixed g, fixed b, fixed* h, fixed* s, fixed* v)
{
    fixed min; 
    fixed max; 
    fixed delta;
    fixed tmp;

    /*change r g b to [0, 1]*/
    r = fixdiv (r, itofix(255));
    g = fixdiv (g, itofix(255));
    b = fixdiv (b, itofix(255));

    tmp = MIN(r, g);
    min = MIN(tmp, b);
    tmp = MAX(r, g);
    max = MAX(tmp, b);

    *v = max; // v
    delta = max - min;

    if (max != 0)
        *s = fixdiv(delta, max);
    else {
        *s = 0;
        *h = UNDEFINED_HUE;
        return;
    }

    if (fixtof(delta) == 0)
    {
        *h = 0;
        *s = 0;
        return;
    }


    if (r == max)
        /*between yellow & magenta*/
        *h = fixdiv(fixsub(g, b), delta);
    else if( g == max )
        /*between cyan & yellow*/
        *h = fixadd(itofix(2), fixdiv(fixsub(b, r), delta));
    else 
        /*magenta & cyan*/
        *h = fixadd(itofix(4), fixdiv(fixsub(r, g), delta));

    /*degrees*/
    *h = fixmul(*h, itofix(60)); 
    if (*h < itofix(0))
        *h = fixadd(*h, itofix(360));

}

static void HSV2RGB(fixed h, fixed s, fixed v, fixed* r, fixed* g, fixed* b)
{
    int i;
    fixed f, p, q, t;

    if (s == 0) {
        *r = fixmul(v, itofix(255));
        *g = fixmul(v, itofix(255));
        *b = fixmul(v, itofix(255));
        return;
    }

    /*sector 0 to 5*/
    h = fixdiv(h, itofix(60));
    i = (h >> 16) % 6;
    /*factorial part of h*/
    f = fixsub(h, itofix(i));
    p = fixmul(v, fixsub(itofix(1), s));
    /*q = v * (1 - s*f)*/
    q = fixmul (v, fixsub (itofix(1), fixmul(s, f)));
   
    /*t = v * (1 - s*(1-f))*/
    t = fixmul (v, fixsub (itofix(1), fixmul(s, fixsub( itofix(1), f))));
    
    switch (i) {
        case 0: 
            *r = fixmul (v, itofix(255)); 
            *g = fixmul (t, itofix(255)); 
            *b = fixmul (p, itofix(255));
            break;
        case 1:
            *r = fixmul (q, itofix(255));
            *g = fixmul (v, itofix(255));
            *b = fixmul (p, itofix(255));
            break;
        case 2:
            *r = fixmul (p, itofix(255));
            *g = fixmul (v, itofix(255));
            *b = fixmul (t, itofix(255));
            break;
        case 3:
            *r = fixmul (p, itofix(255));
            *g = fixmul (q, itofix(255));
            *b = fixmul (v, itofix(255));
            break;
        case 4:
            *r = fixmul (t, itofix(255));
            *g = fixmul (p, itofix(255));
            *b = fixmul (v, itofix(255));
            break;
        case 5:
            *r = fixmul (v, itofix(255));
            *g = fixmul (p, itofix(255));
            *b = fixmul (q, itofix(255));
            break;
    }

}

static void calc_color(DWORD src_color, int flag, DWORD* dest_color)
{
    
    fixed h;
    fixed s;
    fixed v;
    fixed r = itofix(GetRValue(src_color));
    fixed g = itofix(GetGValue(src_color));
    fixed b = itofix(GetBValue(src_color));
    /*RGB => HSV*/
    RGB2HSV(r, g, b, &h, &s, &v);

    /*chang V and H of HSV*/
    switch (flag) {
        case TO_DARKER_COLOR:
            v = fixsub(v, fixdiv(v, itofix(6)));
            break;
        case TO_DARKEST_COLOR:
            v = fixsub(v, fixdiv(v, itofix(3)));
            break;
        case TO_LIGHTER_COLOR:
            increase_vh(v, h, &v, &h, TRUE);
            break;
        case TO_LIGHTEST_COLOR:
            increase_vh(v, h, &v, &h, FALSE);
            break;
    }
    /*HSV => RGB*/
    HSV2RGB(h, s, v, &r, &g, &b);

    *dest_color = MakeRGB(fixtoi(r), fixtoi(g), fixtoi(b));
}
#endif
static DWORD convert_color(int flags, DWORD c)
{
    DWORD new_c;
    new_c = 0;
    printf("c:%x.r:%d, g:%d, b:%d.\n", c, GetRValue(c), GetGValue(c), GetBValue(c));
    if (flags ==1 )
    {
        if (GetRValue(c) <= 128)
            new_c |= (GetRValue(c) + 10);
        if (GetGValue(c) <= 128)
            new_c |= ((GetGValue(c) + 10) << 8);
        if (GetBValue(c) <= 128)
            new_c |= ((GetBValue(c) + 10) << 16);
        return new_c;
    }
    if (flags == 0)
    {
        if (GetRValue(c) >= 30)
            new_c |= GetRValue(c) - 10;
        if (GetGValue(c) >= 30)
            new_c |= ((GetGValue(c) - 10) << 8);
        if (GetBValue(c) >= 30)
            new_c |= ((GetBValue(c) - 10) << 16);
        return new_c;
    }
    return c;
}

static int draw_radio (HDC hdc, const RECT* pRect, DWORD color, int status)
{
    int radius, center_x, center_y, w, h;
    gal_pixel color_pixel, color_lighter;
    DWORD new_c;

    if (pRect == NULL || hdc == HDC_INVALID)
        return -1;

    color_pixel = RGBA2Pixel (hdc, GetRValue(color), GetGValue(color), 
                        GetBValue(color), GetAValue(color));
    calc_color (color, TO_LIGHTEST_COLOR, &new_c);
    calc_color (new_c, TO_LIGHTEST_COLOR, &new_c);
    //calc_color (new_c, TO_LIGHTEST_COLOR, &new_c);
    color_lighter = RGBA2Pixel (hdc, GetRValue(new_c), GetGValue(new_c), 
            GetBValue(new_c), GetAValue(new_c));
    printf("src_c:%x, dst_c:%x.\n", color, new_c);

    w = pRect->right - pRect->left;
    h = pRect->bottom - pRect->top;
    radius = w>h ? (h>>1)-1 : (w>>1)-1;
    center_x = pRect->left + (w>>1);
    center_y = pRect->top + (h>>1);

    /*draw nothing*/
    if (w < 6 || h < 6)
        return 1;

    SetBrushColor (hdc, color_pixel);

    if (status & LFRDR_MARK_HAVESHELL)
    {
        printf("have shell.\n");
        FillCircle (hdc, center_x, center_y, radius);
         /* TODO:Instead of PIXEL_lightgray by new color through a call color 
         * convert function .
         * */
        SetBrushColor (hdc, color_lighter);
        FillCircle (hdc, center_x, center_y, radius-1);

        SetBrushColor (hdc, GetBkColor(hdc));
        FillCircle (hdc, center_x, center_y, radius-2);

        SetBrushColor (hdc, color_pixel);
    }

    if (status & LFRDR_MARK_SELECTED)
    {
        FillCircle (hdc, center_x, center_y, radius>>1);
        /* TODO:Instead of PIXEL_lightgray by new color through a call color 
         * convert function .
         * */

        SetBrushColor (hdc, color_lighter);
        FillCircle (hdc, center_x-1, center_y-1, 1);
    }

    return 0;
}

RECT rc;
DWORD color;
int status;
static int PainterWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    HWND hwnd_button_zoomout, hwnd_button_zoomin;
    
    switch (message) {
        case MSG_CREATE:
            rc.left = 100; rc.top =100; rc.right = 108; rc.bottom = 108;
            color = 0x000F0F0F;
            status = LFRDR_MARK_HAVESHELL | LFRDR_MARK_SELECTED;
            hwnd_button_zoomout = CreateWindowEx(CTRL_BUTTON, "zoom out", 
	                		WS_CHILD | WS_VISIBLE | BS_NOBORDER ,
			                0, 1000, 0, 0, 100, 20, hWnd, 0);
            
            hwnd_button_zoomin = CreateWindowEx(CTRL_BUTTON, "zoom in", 
	                		WS_CHILD | WS_VISIBLE | BS_NOBORDER ,
			                0, 1001, 110, 0, 100, 20, hWnd, 0);
            return 0;
        
        case MSG_COMMAND:
            switch (wParam){
                case 1000:
                    rc.left = 100; rc.top =100; rc.right += 1; rc.bottom += 1;
                    srand(time(NULL));
                    color = rand();
                    //color = convert_color (1, color);
                    break;

                case 1001:
                    rc.left = 100; rc.top =100; rc.right -= 1; rc.bottom -= 1;
                    srand(time(NULL));
                    color = rand();
                    //color = convert_color (0, color);
                    break;
            }
            if (status & LFRDR_MARK_SELECTED)
                status = LFRDR_MARK_HAVESHELL;
            else
                status = LFRDR_MARK_HAVESHELL | LFRDR_MARK_SELECTED;

            UpdateWindow (hWnd, TRUE);
            return 0;
        
        case MSG_PAINT:

            hdc = BeginPaint(hWnd);
            draw_radio (hdc, &rc, color, status);
            EndPaint(hWnd, hdc);
            break;
        
        case MSG_CLOSE:
            DestroyAllControls (hWnd);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

int MiniGUIMain (int args, const char* arg[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "draw_radio" , 0 , 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Draw_Radio";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = PainterWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 240;
    CreateInfo.by = 320;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

