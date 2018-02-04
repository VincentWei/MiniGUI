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
** lf_fashion.c: The fashion LF implementation file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGLF_RDR_FASHION
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "menu.h"
#include "ctrlclass.h"
#include "element.h"
#include "dc.h"
#include "icon.h"
#include "fixedmath.h"
#include <minigui/mgplus.h>
#include "lf_common.h"

#define BTN_STATUS_NUM      8
#define CAP_BTN_INTERVAL    2

#define V_DIFF 0.2
#define FULL_V    (itofix(1))
#define UNDEFINED_HUE   (itofix(-1))

/*scrollbar color*/
#define SCROLL_BK_COLOR_A 0xFFECF1F3
#define SCROLL_BK_COLOR_B 0xFFFBFEFE
#define SCROLL_BTN_COLOR_A 0xFFFCD8C9
#define SCROLL_BTN_COLOR_B 0xFFFBCDB6 
/*button highlight color*/
#define BTN_HILITE_COLOR 0xFF3CC7FF

#define DEF_CHUNK_COLOR   0xFF31D32E
#define DEF_SPACE_SIZE      2
#define DEF_CHUNK_SIZE      6

WINDOW_ELEMENT_RENDERER wnd_rdr_fashion;

/*initialize and terminate interface*/
static int init (PWERENDERER renderer) 
{
   /*get information from MiniGUI.cfg file*/
    InitWindowElementAttrs (renderer);

    renderer->refcount = 0;

    /*we_fonts*/
    renderer->we_fonts[0] = GetSystemFont (SYSLOGFONT_CAPTION);
    renderer->we_fonts[1] = GetSystemFont (SYSLOGFONT_MENU);
    renderer->we_fonts[2] = GetSystemFont (SYSLOGFONT_WCHAR_DEF);
    renderer->we_fonts[3] = GetSystemFont (SYSLOGFONT_WCHAR_DEF);

    /*icon*/
    if (!InitRendererSystemIcon (renderer->name, 
                renderer->we_icon[0], renderer->we_icon[1]))
        return -1;
    
    if (!RegisterSystemBitmap (HDC_SCREEN, renderer->name, SYSBMP_RADIOBUTTON))
        return -1;

    if (!RegisterSystemBitmap (HDC_SCREEN, renderer->name, SYSBMP_CHECKBUTTON))
        return -1;

    renderer->private_info = NULL;
    return 0;
}

static int deinit (PWERENDERER renderer)
{
    /* Destroy system icon. */
    TermRendererSystemIcon (renderer->we_icon[0], renderer->we_icon[1]);

    UnregisterSystemBitmap (HDC_SCREEN, renderer->name, SYSBMP_RADIOBUTTON);
    UnregisterSystemBitmap (HDC_SCREEN, renderer->name, SYSBMP_CHECKBUTTON);

    wnd_rdr_fashion.private_info = NULL;
    return 0;
}

/*
 * RGB2HSV:
 *      This function translate color in RGB space to in HSV space.
 * Author: XuguangWang
 * Date: 2007-11-22
 */
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

/*
 * HSV2RGB:
 *      This function translate color in HSV space to in RGB space.
 * Author: XuguangWang
 * Date: 2007-11-22
 */
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

/*
 * gradient_color:
 *      calc a brighter or darker color           
 * color  : RGB color
 * flag   : LFRDR_3DBOX_COLOR_DARKER or LFRDR_3DBOX_COLOR_LIGHTER
 * degree : in [0, 255]
 * Author: zhounuohua
 * Date: 2008-06-04
 */
static DWORD gradient_color (DWORD color, int flag, int degree)
{
    fixed h;
    fixed s;
    fixed v;
    fixed r = itofix(GetRValue(color));
    fixed g = itofix(GetGValue(color));
    fixed b = itofix(GetBValue(color));
    UINT  a = GetAValue(color);
    /*RGB => HSV*/
    RGB2HSV(r, g, b, &h, &s, &v);

    /*chang V and S of HSV*/
    switch (flag) {
        case LFRDR_3DBOX_COLOR_DARKER:
            {
                v = fixsub(v, degree * v/255);
                if (v < 0)
                    v = 0;
            }
            break;

        case LFRDR_3DBOX_COLOR_LIGHTER:
            {
                if (v > FULL_V)
                    v = FULL_V;
                else
                    v = fixadd (v, degree * v/255);

                if (v > FULL_V) 
                    v = FULL_V;

                if (v == FULL_V) {
                    s = fixsub (s, degree * s/255);
                    if (s < 0)
                        s = 0;
                }
            }
            break;
        default:
            return color;
    }
    /*HSV => RGB*/
    HSV2RGB(h, s, v, &r, &g, &b);

    return MakeRGBA(fixtoi(r), fixtoi(g), fixtoi(b), a);
}

/*
 * calc_3dbox_color:
 *      calc a less brighter, much bright, less darker, or much darker           
 *      color of color which used in 3dbox.     
 * Author: XuguangWang
 * Date: 2007-11-22
 */
static DWORD calc_3dbox_color (DWORD color, int flag)
{
    int degree;
    switch (flag) {
        case LFRDR_3DBOX_COLOR_DARKER:
            degree = 10;
            break;
        case LFRDR_3DBOX_COLOR_DARKEST:
            flag = LFRDR_3DBOX_COLOR_DARKER;
            degree = 255;
            break;
        case LFRDR_3DBOX_COLOR_LIGHTER:
            degree = 10;
            break;
        case LFRDR_3DBOX_COLOR_LIGHTEST:
            flag = LFRDR_3DBOX_COLOR_LIGHTER;
            degree = 255;
            break;
        default:
            return 0;
    }
    return gradient_color (color, flag, degree);
}

static void 
draw_one_frame (HDC hdc, const RECT* rc, 
        DWORD lt_color, DWORD rb_color)
{
    SetPenColor (hdc, RGBA2Pixel(hdc, GetRValue(lt_color), 
                GetGValue(lt_color), GetBValue(lt_color), 
                GetAValue(lt_color)));

    MoveTo(hdc, rc->left, rc->bottom-1);
    LineTo(hdc, rc->left, rc->top);
    LineTo(hdc, rc->right-2, rc->top);

    SetPenColor(hdc, RGBA2Pixel(hdc, GetRValue(rb_color), 
                GetGValue(rb_color), GetBValue(rb_color), 
                GetAValue(rb_color)));

    MoveTo (hdc, rc->left+1, rc->bottom-1);
    LineTo (hdc, rc->right-1, rc->bottom-1);
    LineTo (hdc, rc->right-1, rc->top);
}

static int fill_iso_triangle (HDC hdc, DWORD color, 
                                POINT ap, POINT bp1, POINT bp2)
{ 
    int x1, x2, y1, y2;
    int xdelta, ydelta;
    int xinc, yinc;
    int rem;
    gal_pixel old_color;

    if(bp1.y != bp2.y && bp1.x != bp2.x) return -1;        

    x1 = ap.x; 
    y1 = ap.y; 
    x2 = bp1.x;
    y2 = bp1.y; 

    xdelta = x2 - x1;
    ydelta = y2 - y1;
    if (xdelta < 0) xdelta = -xdelta;
    if (ydelta < 0) ydelta = -ydelta;

    xinc = (x2 > x1) ? 1 : -1;
    yinc = (y2 > y1) ? 1 : -1;

    SetPixel(hdc,x1,y1, RGBA2Pixel(hdc,GetRValue(color),
                GetGValue(color), GetBValue(color), GetAValue(color)));
    old_color = SetPenColor(hdc, RGBA2Pixel(hdc,GetRValue(color),
                GetGValue(color), GetBValue(color), GetAValue(color)));

    if (xdelta >= ydelta) 
    {
        rem = xdelta >> 1;
        while (x1 != x2) 
        {
            x1 += xinc;
            rem += ydelta;
            if (rem >= xdelta) 
            {
                rem -= xdelta;
                y1 += yinc;
            }
            MoveTo(hdc, x1, y1);
            if(bp1.y == bp2.y)
                LineTo(hdc, x1 + (ap.x - x1)*2, y1);
            else
                LineTo(hdc, x1, y1 + (ap.y -y1)*2);
        }
    } 
    else 
    {
        rem = ydelta >> 1;
        while (y1 != y2) 
        {
            y1 += yinc;
            rem += xdelta;
            if (rem >= ydelta) 
            {
                rem -= ydelta;
                x1 += xinc;
            }
            MoveTo(hdc, x1, y1);
            if(bp1.y == bp2.y)
                LineTo(hdc, x1 + (ap.x - x1)*2, y1);
            else
                LineTo(hdc, x1, y1 + (ap.y -y1)*2);
        }
    }
    
    SetPenColor(hdc, old_color);
    
    return 0;
}

/*
 * draw_a frame for fashion button.
 */
static void draw_fashion_frame(HDC hdc, RECT rect, DWORD color)
{
    gal_pixel old_pen_color;
    
    rect.right--;
    rect.bottom--;
    
    old_pen_color = SetPenColor(hdc, 
            RGBA2Pixel(hdc, GetRValue(color), GetGValue(color), 
                GetBValue(color), GetAValue(color)));
    
    MoveTo(hdc, rect.left+1, rect.top);
    LineTo(hdc, rect.right-1, rect.top);
    MoveTo(hdc, rect.left, rect.top+1);
    LineTo(hdc, rect.left, rect.bottom-1);
    
    MoveTo(hdc, rect.left+1, rect.bottom);
    LineTo(hdc, rect.right-1, rect.bottom);
    MoveTo(hdc, rect.right, rect.bottom-1);
    LineTo(hdc, rect.right, rect.top+1);

    SetPenColor(hdc, old_pen_color);
}

static void 
draw_3dbox (HDC hdc, const RECT* pRect, DWORD color, DWORD flag)
{
    DWORD light_color;
    DWORD dark_color;
    RECT rc_tmp = *pRect;
    BOOL is_have_lighter_color = FALSE;
    gal_pixel old_brush_color;
    gal_pixel old_pen_color;

    /*much small rect*/
    if (RECTW(rc_tmp)<2 || RECTH(rc_tmp)<2)
        return;

    old_brush_color = GetBrushColor(hdc);
    old_pen_color = GetPenColor(hdc);
    
    /*draw outer frame*/
    light_color = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_LIGHTEST);
    dark_color = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_DARKEST);
    if ((flag & LFRDR_BTN_STATUS_MASK) == LFRDR_BTN_STATUS_PRESSED)
    {
        draw_one_frame(hdc, &rc_tmp, dark_color, light_color);
    }
    else
    {
        /*thick frame left-top is main color, to draw a outline*/
        if (flag & LFRDR_3DBOX_THICKFRAME)
            draw_one_frame(hdc, &rc_tmp, color, dark_color);
        else
            draw_one_frame(hdc, &rc_tmp, light_color, dark_color);
    }

    /*draw inner frame*/
    if ((flag & LFRDR_3DBOX_THICKFRAME) 
            && (RECTW(rc_tmp)>6 && RECTH(rc_tmp)>6)) {
        light_color = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_LIGHTER);
        dark_color = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_DARKER);
        is_have_lighter_color = TRUE;

        DWINDLE_RECT(rc_tmp);
        if ((flag & LFRDR_BTN_STATUS_MASK) == LFRDR_BTN_STATUS_PRESSED)
        {
            draw_one_frame(hdc, &rc_tmp, dark_color, light_color);
        }
        else
        {
            draw_one_frame(hdc, &rc_tmp, light_color, dark_color);
        }
    }

    if (flag & LFRDR_3DBOX_FILLED) {
        DWINDLE_RECT(rc_tmp);
        switch (flag & LFRDR_BTN_STATUS_MASK) {
            case LFRDR_BTN_STATUS_HILITE:
                if (!is_have_lighter_color)
                    light_color = 
                        calc_3dbox_color (color, LFRDR_3DBOX_COLOR_LIGHTER);

                SetBrushColor(hdc, RGBA2Pixel(hdc, GetRValue(light_color), 
                            GetGValue(light_color), GetBValue(light_color), 
                            GetAValue(light_color)));
                break;

            case LFRDR_BTN_STATUS_DISABLED:
            default:

                SetBrushColor(hdc, RGBA2Pixel(hdc, GetRValue(color), 
                            GetGValue(color), GetBValue(color), 
                            GetAValue(color)));
        }
        FillBox(hdc,  rc_tmp.left, rc_tmp.top,
                RECTW(rc_tmp), RECTH(rc_tmp));
    }

    SetPenColor(hdc, old_pen_color);
    SetBrushColor(hdc, old_brush_color);
}

static void 
draw_radio (HDC hdc, const RECT* pRect, DWORD color, int status)
{
    int radius, center_x, center_y, w, h;
    gal_pixel _pixel, old_pixel;

    if (pRect == NULL)
        return;

    /*no shell style*/
    if (status & LFRDR_MARK_ALL_SELECTED) {
        w = pRect->right - pRect->left;
        h = pRect->bottom - pRect->top;

        /*draw nothing*/
        if (w < 6 || h < 6)
            return;

        _pixel = RGBA2Pixel (hdc, GetRValue(color), 
                        GetGValue(color), GetBValue(color), 
                        GetAValue(color));

        radius = w>h ? (h>>1)-1 : (w>>1)-1;
        center_x = pRect->left + (w>>1);
        center_y = pRect->top + (h>>1);

        old_pixel = SetBrushColor (hdc, _pixel);

        SetBrushColor (hdc, _pixel);
        FillCircle (hdc, center_x, center_y, radius>>1);
        SetBrushColor (hdc, old_pixel);
    }
}

static void 
draw_checkbox (HDC hdc, const RECT* pRect, DWORD color, int status)
{
}

static void 
draw_checkmark (HDC hdc, const RECT* pRect, DWORD color, int status)
{
    int i, w, h, side_len, boundary;
    int box_l, box_t, box_r, box_b;
    int hook_l, hook_t, hook_r, hook_b;
    int border_cut;
    gal_pixel _pixel, old_pixel;
    
    if (!pRect)
        return;
    
    if ((status & LFRDR_MARK_HALF_SELECTED) 
            || (status & LFRDR_MARK_ALL_SELECTED))  
    {
        w = RECTWP (pRect);
        h = RECTHP (pRect);
        
        if (w < 6 || h < 6)
            return;

        side_len = w>h ? h : w;
        boundary = ABS(w-h)>>1;
        border_cut = (side_len+1)>>3;

        _pixel = RGBA2Pixel (hdc, GetRValue(color), 
                        GetGValue(color), 
                        GetBValue(color), 
                        GetAValue(color));

        old_pixel = SetPenColor (hdc, _pixel);
    
        if (w > h) {
            box_l = pRect->left + boundary;
            box_t = pRect->top;
            box_r = box_l + side_len-1;
            box_b = pRect->bottom-1;
        }
        else if (w < h) {
            box_l = pRect->left;
            box_t = pRect->top + boundary;
            box_r = pRect->right-1;
            box_b = box_t + side_len-1;
        }
        else {
            box_l = pRect->left;
            box_t = pRect->top;
            box_r = pRect->right-1;
            box_b = pRect->bottom-1;
        }
    
        hook_l = box_l + (side_len>>2);
        hook_t = box_t + (side_len>>2);
        hook_r = box_r - (side_len>>2);
        hook_b = box_b - (side_len>>2);

        for (i=(side_len+1)>>3; i>0; i--)
        {
            MoveTo (hdc, hook_l, hook_t+((side_len+1)>>3)+i-1);
            LineTo (hdc, hook_l+((side_len+1)>>3), 
                    hook_b-(((side_len+1)>>3)-i+1));
            LineTo (hdc, hook_r, hook_t+i-1);
        }

        SetPenColor (hdc, old_pixel);
    }
}

static void 
draw_arrow (HWND hWnd, HDC hdc, const RECT* pRect, DWORD color, int status)
{
    DWORD c1, c2;
    int w, h, i;
    RECT rc_tmp = *pRect, rc_tmp2;
    // p1, the apex point of the isosceles triangle,
    // p2,p3, the 2 base points if the isosceles tiangle.
    POINT p1, p2, p3, point;
    gal_pixel pixel;
    gal_pixel gradcolor; 
    gal_pixel old_pen_color, pixels[2], pixel2[2];
    mgplus_pattern_t ptn;

    w = RECTW (*pRect);
    h = RECTH (*pRect);
    if(w <= 0 || h <= 0) return;

    if(status & LFRDR_ARROW_HAVESHELL)
    {
        c1 = GetWindowElementAttr (hWnd, WE_BGCB_ACTIVE_CAPTION);
        c2 = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 80);
       switch (status & LFRDR_BTN_STATUS_MASK) 
        {
            case LFRDR_BTN_STATUS_HILITE:
                pixel = gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 220);
                pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                            (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 100);
                pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                            (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                
                pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 100);
                pixel2[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                            (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 200);
                pixel2[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                            (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#if 0
                pixel[0] = DWORD2Pixel (hdc, gradient_color 
                        (c2, LFRDR_3DBOX_COLOR_LIGHTER, 220)); 
                pixel[1] = DWORD2Pixel (hdc, gradient_color 
                        (c1, LFRDR_3DBOX_COLOR_LIGHTER, 100)); 

                pixel2[0] = DWORD2Pixel (hdc, gradient_color 
                        (c1, LFRDR_3DBOX_COLOR_LIGHTER, 100));
                pixel2[1] = DWORD2Pixel (hdc, gradient_color 
                        (c1, LFRDR_3DBOX_COLOR_LIGHTER, 200));
#endif
                break; 
            case LFRDR_BTN_STATUS_PRESSED:
                pixel = gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 200);
                pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                            (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_DARKER, 100);
                pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                            (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                
                pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_DARKER, 100);
                pixel2[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                           (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 100);
                pixel2[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                           (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#if 0
                pixel[0] = DWORD2Pixel (hdc, gradient_color 
                        (c2, LFRDR_3DBOX_COLOR_LIGHTER, 200)); 
                pixel[1] = DWORD2Pixel (hdc, gradient_color 
                        (c1, LFRDR_3DBOX_COLOR_DARKER, 100)); 

                pixel2[0] = DWORD2Pixel (hdc, gradient_color 
                        (c1, LFRDR_3DBOX_COLOR_DARKER, 100)); 
                pixel2[1] = DWORD2Pixel (hdc, gradient_color 
                        (c1, LFRDR_3DBOX_COLOR_LIGHTER, 100));
#endif
                break;
            default:
                pixel = gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 250);
                pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                            (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                pixel = gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 10);
                pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                            (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                
                pixel = gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 10);
                pixel2[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                            (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 150);
                pixel2[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                            (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#if 0
                pixel[0] = DWORD2Pixel (hdc, gradient_color 
                        (c2, LFRDR_3DBOX_COLOR_LIGHTER, 250)); 
                pixel[1] = DWORD2Pixel (hdc, gradient_color 
                        (c2, LFRDR_3DBOX_COLOR_LIGHTER, 10)); 

                pixel2[0] = DWORD2Pixel (hdc, gradient_color 
                        (c2, LFRDR_3DBOX_COLOR_LIGHTER, 10)); 
                pixel2[1] = DWORD2Pixel (hdc, gradient_color 
                        (c1, LFRDR_3DBOX_COLOR_LIGHTER, 150));
#endif
                break;
        }
        draw_fashion_frame(hdc, rc_tmp, 
                gradient_color (c1, LFRDR_3DBOX_COLOR_DARKEST, 110)); 
        switch (status & LFRDR_ARROW_DIRECT_MASK)
        {
            case LFRDR_ARROW_UP:
            case LFRDR_ARROW_DOWN:
                {
                    rc_tmp.top +=1;
                    rc_tmp.left += 1;
                    rc_tmp.right = rc_tmp.left + RECTW (*pRect)/3;
                    rc_tmp.bottom -= 2;

                    rc_tmp2.top = rc_tmp.top;
                    rc_tmp2.left = rc_tmp.right + 1;
                    rc_tmp2.right = pRect->right -2;
                    rc_tmp2.bottom = rc_tmp.bottom;
                    ptn = mGPlusCreateLinearPattern (rc_tmp, 
                            MGPLUS_GRADIENT_LINEAR_HORIZONTAL, pixels, 2, MGPLUS_GRADIENT_BIT_16);

                    for (i = rc_tmp.left; i <= rc_tmp.right; i ++)
                    {
                        point.y = rc_tmp.top;
                        point.x = i;
                        mGPlusGetGradientColorValue (ptn, &point, &gradcolor);
                        SetPenColor (hdc, gradcolor);
                        MoveTo (hdc, i, rc_tmp.top);
                        LineTo (hdc, i, rc_tmp.bottom);
                    }
                    mGPlusFreeColorPattern (ptn);
                    ptn = mGPlusCreateLinearPattern (rc_tmp2, 
                            MGPLUS_GRADIENT_LINEAR_HORIZONTAL, pixel2, 2, MGPLUS_GRADIENT_BIT_16);

                    for (i = rc_tmp2.left; i <= rc_tmp2.right; i ++)
                    {
                        point.y = rc_tmp2.top;
                        point.x = i;
                        mGPlusGetGradientColorValue (ptn, &point, &gradcolor);
                        SetPenColor (hdc, gradcolor);
                        MoveTo (hdc, i, rc_tmp2.top);
                        LineTo (hdc, i, rc_tmp2.bottom);
                    }
                    mGPlusFreeColorPattern (ptn);
                }
                break;
            case LFRDR_ARROW_LEFT:
            case LFRDR_ARROW_RIGHT:
                {
                    rc_tmp.top = rc_tmp.top + 1;
                    rc_tmp.left = rc_tmp.left + 1;
                    rc_tmp.right = rc_tmp.right - 2;
                    rc_tmp.bottom = rc_tmp.top + RECTH (*pRect)/3;

                    rc_tmp2.top = rc_tmp.bottom;
                    rc_tmp2.left = rc_tmp.left;
                    rc_tmp2.right = rc_tmp.right;
                    rc_tmp2.bottom = pRect->bottom - 2;
                    ptn = mGPlusCreateLinearPattern (rc_tmp, 
                            MGPLUS_GRADIENT_LINEAR_VERTICAL, pixels, 2, MGPLUS_GRADIENT_BIT_16);

                    for (i = rc_tmp.top; i <= rc_tmp.bottom; i ++)
                    {
                        point.x = rc_tmp.left;
                        point.y = i;
                        mGPlusGetGradientColorValue (ptn, &point, &gradcolor);
                        SetPenColor (hdc, gradcolor);
                        MoveTo (hdc, rc_tmp.left, i);
                        LineTo (hdc, rc_tmp.right, i);
                    }
                    mGPlusFreeColorPattern (ptn);
                    ptn = mGPlusCreateLinearPattern (rc_tmp2, 
                            MGPLUS_GRADIENT_LINEAR_VERTICAL, pixel2, 2, MGPLUS_GRADIENT_BIT_16);

                    for (i = rc_tmp2.top; i <= rc_tmp2.bottom; i ++)
                    {
                        point.x = rc_tmp2.left;
                        point.y = i;
                        mGPlusGetGradientColorValue (ptn, &point, &gradcolor);
                        SetPenColor (hdc, gradcolor);
                        MoveTo (hdc, rc_tmp2.left, i);
                        LineTo (hdc, rc_tmp2.right, i);
                    }
                    mGPlusFreeColorPattern (ptn);
                }
                break;
        }
   }
     
    switch(status & LFRDR_ARROW_DIRECT_MASK)
    {
        case LFRDR_ARROW_UP:
            p1.x = pRect->left  + (w>>1);
            p1.y = pRect->top   + (h/3);
            p2.x = pRect->left  + (w>>2);
            p2.y = pRect->bottom- (h/3) - 1;
            //p2.y = p1.y + (w>>2);
            p3.x = pRect->right - (w>>2) - 1;
            p3.y = pRect->bottom- (h/3) -1;
            //p3.y = p1.y + (w>>2);
            break;
        case LFRDR_ARROW_DOWN:
            p1.x = pRect->left  + (w>>1);
            p1.y = pRect->bottom- (h/3) - 1;
            p2.x = pRect->left  + (w>>2);
            p2.y = pRect->top   + (h/3);
            p3.x = pRect->right - (w>>2) - 1;
            p3.y = pRect->top   + (h/3);
            break;
        case LFRDR_ARROW_LEFT:
            p1.x = pRect->left  + (w/3);
            p1.y = pRect->top   + (h>>1);
            p2.x = pRect->right - (w/3) - 1;
            p2.y = pRect->top   + (h>>2);
            p3.x = pRect->right - (w/3) - 1;
            p3.y = pRect->bottom- (h>>2) - 1;
            break;
        case LFRDR_ARROW_RIGHT:
            p1.x = pRect->right - (w/3) - 1;
            p1.y = pRect->top   + (h>>1);
            p2.x = pRect->left  + (w/3);
            p2.y = pRect->top   + (h>>2);
            p3.x = pRect->left  + (w/3);
            p3.y = pRect->bottom- (h>>2) - 1;
            break;
        default :
            return ;
    }

    color = gradient_color (color, LFRDR_3DBOX_COLOR_LIGHTER, 10);
    if(status & LFRDR_ARROW_NOFILL) {
        old_pen_color = SetPenColor(hdc, RGBA2Pixel(hdc,GetRValue(color),
                    GetGValue(color), GetBValue(color), GetAValue(color)));
        MoveTo(hdc, p1.x, p1.y);
        LineTo(hdc, p2.x, p2.y);
        LineTo(hdc, p3.x, p3.y);
        LineTo(hdc, p1.x, p1.y);
        SetPenColor (hdc, old_pen_color);
    }
    else {
        fill_iso_triangle(hdc, color, p1, p2, p3);
    }
    return ;
}

static void 
draw_fold (HWND hWnd, HDC hdc, const RECT* pRect, 
        DWORD color, int status, int next)
{
    int flag;

    if (status & LFRDR_TREE_CHILD) {
        if (status & LFRDR_TREE_FOLD)
            flag = LFRDR_ARROW_RIGHT;
        else
            flag = LFRDR_ARROW_DOWN;
    }
    else 
        return;

    return draw_arrow (hWnd, hdc, pRect, color, flag);
}

static void 
draw_focus_frame (HDC hdc, const RECT *pRect, DWORD color)
{
    int i;
    gal_pixel pixel;

    
    pixel = RGBA2Pixel(hdc, GetRValue(color), GetGValue(color), 
                            GetBValue(color), GetAValue(color));

    for(i = pRect->left; i < pRect->right; i++)
    {
        if(i & 0x01)
        {
            SetPixel(hdc, i, pRect->top, pixel);
            SetPixel(hdc, i, pRect->bottom, pixel);
        }
    }
    for(i = pRect->top; i < pRect->bottom; i++)
    {
        if(i & 0x01)
        {
            SetPixel(hdc, pRect->left, i ,pixel);
            SetPixel(hdc, pRect->right, i ,pixel);
        }
    }
}

static void draw_normal_item 
(HWND hWnd, HDC hdc, const RECT* pRect, DWORD color)
{
    gal_pixel old_bc;
    old_bc = SetBrushColor (hdc, DWORD2Pixel (hdc, color));
    FillBox (hdc, pRect->left, pRect->top, RECTWP (pRect), RECTHP (pRect));
    SetBrushColor (hdc, old_bc);
}

static void draw_hilite_item 
(HWND hWnd, HDC hdc, const RECT* pRect, DWORD color)
{
    gal_pixel pixels[3];
    mgplus_pattern_t ptn;
    int i;
    POINT point;
    gal_pixel pixel_dst;
    gal_pixel pixel_org;
    gal_pixel conver_pixel;
    int corner, l, t, r, b;
   
    conver_pixel = color;
    pixels[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    conver_pixel = gradient_color (color, LFRDR_3DBOX_COLOR_LIGHTER, 120);
    pixels[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    pixels[2] = pixels[0];
#if 0
    pixels[0] = DWORD2Pixel (hdc, color);
    pixels[1] = DWORD2Pixel (hdc, gradient_color 
            (color, LFRDR_3DBOX_COLOR_LIGHTER, 120));
    pixels[2] = pixels[0];
#endif
    ptn = mGPlusCreateLinearPattern (*pRect, 
            MGPLUS_GRADIENT_LINEAR_VERTICAL,
            pixels, 3, MGPLUS_GRADIENT_BIT_16);

    pixel_org = GetPenColor (hdc);
    point.x = pRect->left;
    for (i = pRect->top; i < pRect->bottom; ++i) {
        point.y = i;
        if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
            SetPenColor (hdc, pixel_dst);
            MoveTo (hdc, point.x, point.y);
            LineTo (hdc, pRect->right - 1, point.y);
        }
        else {
            printf ("mGPlusGetGradientColorValue return false.\n");
        }
    }

    SetPenColor (hdc, pixel_org);
    mGPlusFreeColorPattern (ptn);

     /*draw border*/
    corner = 1;
    l = pRect->left;
    t = pRect->top;
    r = pRect->right;
    b = pRect->bottom;

    SetPenColor(hdc, pixels[0]); 
    MoveTo(hdc, l + corner, t);
    LineTo(hdc, r - 1 - corner, t);
    LineTo(hdc, r - 1, t+corner);
    LineTo(hdc, r - 1, b - 1-corner);
    LineTo(hdc, r - 1-corner, b - 1);
    LineTo(hdc, l+corner, b - 1);
    LineTo(hdc, l, b - 1-corner);
    LineTo(hdc, l, t+corner);
    LineTo(hdc, l+corner, t);
}

static void draw_disabled_item 
(HWND hWnd, HDC hdc, const RECT* pRect, DWORD color)
{
    gal_pixel old_bc;
    old_bc = SetBrushColor (hdc, DWORD2Pixel (hdc, color));
    FillBox (hdc, pRect->left, pRect->top, RECTWP (pRect), RECTHP (pRect));
    SetBrushColor (hdc, old_bc);
}

static void draw_significant_item 
(HWND hWnd, HDC hdc, const RECT* pRect, DWORD color)
{
    gal_pixel old_bc;
    old_bc = SetBrushColor (hdc, DWORD2Pixel (hdc, color));
    FillBox (hdc, pRect->left, pRect->top, RECTWP (pRect), RECTHP (pRect));
    SetBrushColor (hdc, old_bc);
}

static void draw_normal_menu_item 
(HWND hWnd, HDC hdc, const RECT* pRect, DWORD color)
{
    gal_pixel old_bc;
    old_bc = SetBrushColor (hdc, DWORD2Pixel (hdc, color));
    FillBox (hdc, pRect->left, pRect->top, RECTWP (pRect), RECTHP (pRect));
    SetBrushColor (hdc, old_bc);
}

static void draw_hilite_menu_item 
(HWND hWnd, HDC hdc, const RECT* pRect, DWORD color)
{
    gal_pixel pixels[3];
    mgplus_pattern_t ptn;
    int i;
    POINT point;
    gal_pixel pixel_dst;
    gal_pixel pixel_org;
    gal_pixel conver_pixel;
    int corner, l, t, r, b;

    conver_pixel = color;
    pixels[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    
    conver_pixel = gradient_color (color, LFRDR_3DBOX_COLOR_LIGHTER, 240);
    pixels[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);

    conver_pixel = gradient_color (color, LFRDR_3DBOX_COLOR_LIGHTER, 100);
    pixels[2] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
#if 0 
    pixels[1] = DWORD2Pixel (hdc, color);
    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (color, LFRDR_3DBOX_COLOR_LIGHTER, 240));
    pixels[2] = DWORD2Pixel (hdc, gradient_color 
            (color, LFRDR_3DBOX_COLOR_LIGHTER, 100));
#endif
    ptn = mGPlusCreateLinearPattern (*pRect, 
            MGPLUS_GRADIENT_LINEAR_VERTICAL,
            pixels, 3, MGPLUS_GRADIENT_BIT_16);

    pixel_org = GetPenColor (hdc);
    for (i = pRect->top; i < pRect->bottom; ++i) {
        point.x = pRect->left;
        point.y = i;
        if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
            SetPenColor (hdc, pixel_dst);
            MoveTo (hdc, point.x, point.y);
            LineTo (hdc, pRect->right - 1, point.y);
        }
        else {
            printf ("mGPlusGetGradientColorValue return false.\n");
        }
    }

    SetPenColor (hdc, pixel_org);
    mGPlusFreeColorPattern (ptn);

     /*draw border*/
    corner = 1;
    l = pRect->left;
    t = pRect->top;
    r = pRect->right;
    b = pRect->bottom;

    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (color, LFRDR_3DBOX_COLOR_DARKER, 140));
    SetPenColor(hdc, pixels[0]); 
    --r;
    --b;
    Rectangle (hdc, l, t, r, b);
    ++l;
    ++t;
    --r;
    --b;
    Rectangle (hdc, l, t, r, b);
}

static void draw_disabled_menu_item 
(HWND hWnd, HDC hdc, const RECT* pRect, DWORD color)
{
    gal_pixel old_bc;
    old_bc = SetBrushColor (hdc, DWORD2Pixel (hdc, color));
    FillBox (hdc, pRect->left, pRect->top, RECTWP (pRect), RECTHP (pRect));
    SetBrushColor (hdc, old_bc);
}

#define STATUS_GET_CHECK(status) ((status) & BST_CHECK_MASK)

#define GET_BTN_POSE_STATUS(status) ((status) & BST_POSE_MASK)

static void 
draw_push_button (HWND hWnd, HDC hdc, const RECT* pRect, 
        DWORD color1, DWORD color2, int status)
{
    POINT   point;
    DWORD   c1, tmpcolor;
    RECT    frRect, upRect, downRect;
    int     i, corner = 2;
    BOOL    flag = TRUE;
    gal_pixel pixel;
    gal_pixel uppixel[2], downpixel[2], old_pen_color, color;
    mgplus_pattern_t ptn;

    frRect.left = pRect->left;
    frRect.right = pRect->right-1;
    frRect.top = pRect->top;
    frRect.bottom = pRect->bottom-1;

    upRect.left = pRect->left +1;
    upRect.right = pRect->right -2;
    upRect.top = pRect->top +1;
    upRect.bottom = pRect->top + RECTH(*pRect)/2-3;

    downRect.left = pRect->left+1;
    downRect.right = pRect->right-2;
    downRect.top = upRect.bottom+1;
    downRect.bottom = pRect->bottom-2;

    old_pen_color = GetPenColor (hdc);
    c1 = GetWindowElementAttr (hWnd, WE_BGCB_ACTIVE_CAPTION);
    tmpcolor = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 80);

    if (status == BST_FOCUS) {
        flag = FALSE;
    }
    else if (STATUS_GET_CHECK (status) != BST_UNCHECKED)
    {
        if (STATUS_GET_CHECK (status) == BST_CHECKED) {
            /*Draw checked status: Use caption color*/
            pixel = gradient_color (tmpcolor, LFRDR_3DBOX_COLOR_LIGHTER, 250);
            uppixel[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                         (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            pixel = gradient_color (tmpcolor, LFRDR_3DBOX_COLOR_LIGHTER, 70);
            uppixel[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                         (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 50);
            downpixel[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                           (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 150);
            downpixel[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                           (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#if 0
            uppixel[0] = DWORD2Pixel (hdc, gradient_color (tmpcolor,
                        LFRDR_3DBOX_COLOR_LIGHTER, 250));
            uppixel[1] = DWORD2Pixel (hdc, gradient_color (tmpcolor, 
                        LFRDR_3DBOX_COLOR_LIGHTER, 70));
            downpixel[0] = DWORD2Pixel (hdc, gradient_color (c1,
                        LFRDR_3DBOX_COLOR_LIGHTER, 50));
            downpixel[1] = DWORD2Pixel (hdc, gradient_color (c1,
                        LFRDR_3DBOX_COLOR_LIGHTER, 150));
#endif
        }
        else if (STATUS_GET_CHECK (status) == BST_INDETERMINATE) {
            /*Use color1*/
            //tmpcolor = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 10);
            pixel = tmpcolor;
            uppixel[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                         (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            uppixel[0] = uppixel[1];
            downpixel[0] = downpixel[1] = uppixel[0];
#if 0
            uppixel[0] = uppixel[1] = DWORD2Pixel (hdc, tmpcolor);
            downpixel[0] = downpixel[1] = DWORD2Pixel (hdc, tmpcolor);
#endif
        }
        else {
            flag = FALSE;
        }
    }
    else {
            /*Draw normal status*/
        if (GET_BTN_POSE_STATUS(status) == BST_PUSHED) {
            pixel = gradient_color (tmpcolor, LFRDR_3DBOX_COLOR_LIGHTER, 200);
            uppixel[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                         (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            pixel = gradient_color (tmpcolor, LFRDR_3DBOX_COLOR_LIGHTER, 70);
            uppixel[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                         (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 50);
            downpixel[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                           (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 150);
            downpixel[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                           (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#if 0 
            uppixel[0] = DWORD2Pixel (hdc, gradient_color (tmpcolor,
                        LFRDR_3DBOX_COLOR_LIGHTER, 200));
            uppixel[1] = DWORD2Pixel (hdc, gradient_color (tmpcolor, 
                        LFRDR_3DBOX_COLOR_LIGHTER, 70));
            downpixel[0] = DWORD2Pixel (hdc, gradient_color (c1,
                        LFRDR_3DBOX_COLOR_LIGHTER, 50));
            downpixel[1] = DWORD2Pixel (hdc, gradient_color (c1,
                        LFRDR_3DBOX_COLOR_LIGHTER, 150));
#endif
        }
        else if (GET_BTN_POSE_STATUS(status) == BST_HILITE) {
            /*Draw hilight status*/
            pixel = gradient_color (tmpcolor, LFRDR_3DBOX_COLOR_LIGHTER, 250);
            uppixel[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                         (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            pixel = gradient_color (tmpcolor, LFRDR_3DBOX_COLOR_LIGHTER, 70);
            uppixel[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                         (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 90);
            downpixel[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                           (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 180);
            downpixel[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                           (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#if 0
            uppixel[0] = DWORD2Pixel (hdc, gradient_color (tmpcolor,
                        LFRDR_3DBOX_COLOR_LIGHTER, 250));
            uppixel[1] = DWORD2Pixel (hdc, gradient_color (tmpcolor, 
                        LFRDR_3DBOX_COLOR_LIGHTER, 70));
            downpixel[0] = DWORD2Pixel (hdc, gradient_color (c1,
                        LFRDR_3DBOX_COLOR_LIGHTER, 90));
            downpixel[1] = DWORD2Pixel (hdc, gradient_color (c1,
                        LFRDR_3DBOX_COLOR_LIGHTER, 180));
#endif
        }
        else if (GET_BTN_POSE_STATUS(status) == BST_DISABLE) {
            pixel = tmpcolor;
            uppixel[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                         (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
            uppixel[0] = uppixel[1];
            downpixel[0] = downpixel[1] = uppixel[0];
#if 0
            uppixel[0] = uppixel[1] = DWORD2Pixel (hdc, tmpcolor);
            downpixel[0] = downpixel[1] = DWORD2Pixel (hdc, tmpcolor);
#endif
        }
        else
            flag = FALSE;
    }

    if (!flag)
    {
        pixel = gradient_color (tmpcolor, LFRDR_3DBOX_COLOR_LIGHTER, 250);
        uppixel[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                     (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
        pixel = gradient_color (tmpcolor, LFRDR_3DBOX_COLOR_LIGHTER, 70);
        uppixel[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                     (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
        pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 50);
        downpixel[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                       (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
        pixel = gradient_color (c1, LFRDR_3DBOX_COLOR_LIGHTER, 150);
        downpixel[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                       (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#if 0
        uppixel[0] = DWORD2Pixel (hdc, gradient_color (tmpcolor,
                    LFRDR_3DBOX_COLOR_LIGHTER, 250));
        uppixel[1] = DWORD2Pixel (hdc, gradient_color (tmpcolor, 
                    LFRDR_3DBOX_COLOR_LIGHTER, 70));
        downpixel[0] = DWORD2Pixel (hdc, gradient_color (c1,
                    LFRDR_3DBOX_COLOR_LIGHTER, 50));
        downpixel[1] = DWORD2Pixel (hdc, gradient_color (c1,
                    LFRDR_3DBOX_COLOR_LIGHTER, 150));
#endif
    }
    ptn = mGPlusCreateLinearPattern (upRect, MGPLUS_GRADIENT_LINEAR_VERTICAL,
            uppixel, 2, MGPLUS_GRADIENT_BIT_16);

    point.x = upRect.left;
    point.y = upRect.top;
    mGPlusGetGradientColorValue (ptn, &point, &color);
    SetPenColor (hdc, color);
    MoveTo (hdc, upRect.left + 2, upRect.top);
    LineTo (hdc, upRect.right - 2, upRect.top);

    point.x = upRect.left;
    point.y = upRect.top + 1;
    mGPlusGetGradientColorValue (ptn, &point, &color);
    SetPenColor (hdc, color);
    MoveTo (hdc, upRect.left + 1, upRect.top + 1);
    LineTo (hdc, upRect.right - 1, upRect.top +1);

    for (i = upRect.top + 2; i <= upRect.bottom; i ++)
    {
        point.x = upRect.left;
        point.y = i;
        mGPlusGetGradientColorValue (ptn, &point, &color);
        SetPenColor (hdc, color);
        MoveTo (hdc, upRect.left, i);
        LineTo (hdc, upRect.right, i);
    }
    mGPlusFreeColorPattern (ptn);

    ptn = mGPlusCreateLinearPattern (downRect, MGPLUS_GRADIENT_LINEAR_VERTICAL,
            downpixel, 2, MGPLUS_GRADIENT_BIT_16);
    for (i = downRect.top; i <= downRect.bottom - 2; i ++)
    {
        point.x = downRect.left;
        point.y = i;
        mGPlusGetGradientColorValue (ptn, &point, &color);
        SetPenColor (hdc, color);
        MoveTo (hdc, downRect.left, i);
        LineTo (hdc, downRect.right, i);
    }

    point.x = downRect.left;
    point.y = downRect.bottom;
    mGPlusGetGradientColorValue (ptn, &point, &color);
    SetPenColor (hdc, color);
    MoveTo (hdc, downRect.left + 2, downRect.bottom);
    LineTo (hdc, downRect.right - 2, downRect.bottom);

    point.x = downRect.left;
    point.y = downRect.bottom - 1;
    mGPlusGetGradientColorValue (ptn, &point, &color);
    SetPenColor (hdc, color);
    MoveTo (hdc, downRect.left + 1, downRect.bottom - 1);
    LineTo (hdc, downRect.right - 1, downRect.bottom - 1);

    mGPlusFreeColorPattern (ptn);

    //draw border top
    SetPenColor(hdc, DWORD2Pixel(hdc, c1));
    MoveTo(hdc, frRect.left + corner, frRect.top);
    LineTo(hdc, frRect.right - corner, frRect.top);
    LineTo(hdc, frRect.right, frRect.top+corner);

    //right
    LineTo(hdc, frRect.right, frRect.bottom-corner);
    LineTo(hdc, frRect.right-corner, frRect.bottom);

    //bottom
    LineTo(hdc, frRect.left+corner, frRect.bottom);
    LineTo(hdc, frRect.left, frRect.bottom-corner);

    //left
    LineTo(hdc, frRect.left, frRect.top+corner);
    LineTo(hdc, frRect.left+corner, frRect.top);

    SetPenColor(hdc, old_pen_color);
}

static void 
_draw_check_button (HDC hdc, 
        const RECT* pRect, int status, const BITMAP* pBmp)
{
    int w, h;
    int box_l = 0, box_t = 0;
    int bmp_w, bmp_h;

    w = RECTWP(pRect); 
    h = RECTHP(pRect);

    bmp_w = pBmp->bmWidth; 
    bmp_h = pBmp->bmHeight/BTN_STATUS_NUM;

    if (w >= bmp_w && h >= bmp_h){
        box_l = pRect->left + ((w - bmp_w)>>1);

        box_t = pRect->top + ((h - bmp_h)>>1);

	    /*parity check*/
    	if (w & 0x1)
        	box_l += 1;
    
	    if (h & 0x1)
    	    box_t += 1;

	    FillBoxWithBitmapPart (hdc, box_l, box_t, bmp_w, bmp_h, 
            0, 0, pBmp, 0, status * bmp_h);
	}
	else {
		FillBitmapPartInBox(hdc, pRect->left, pRect->top, w, h,
			(PBITMAP)pBmp, 0, status*bmp_h, bmp_w, bmp_h);
	}
}

static void 
_draw_radio_button (HWND hWnd, HDC hdc,
        const RECT* pRect, int status, const BITMAP* pBmp)
{
    int w, h;
    int box_l = 0, box_t = 0, box_r, box_b;
    int bmp_w, bmp_h;
    gal_pixel bgc;

    w = RECTWP(pRect); 
    h = RECTHP(pRect);

    bmp_w = pBmp->bmWidth; 
    bmp_h = pBmp->bmHeight/BTN_STATUS_NUM;

	if( w >= bmp_w && h >= bmp_w){
        box_l = pRect->left + ((w - bmp_w)>>1);

        box_t = pRect->top + ((h - bmp_h)>>1);

	    /*parity check*/
    	if (w & 0x1)
        	box_l += 1;
    
	    if (h & 0x1)
    	    box_t += 1;

   		FillBoxWithBitmapPart (hdc, box_l, box_t, bmp_w, bmp_h, 
            	0, 0, pBmp, 0, status * bmp_h);
	}
	else{
		FillBitmapPartInBox(hdc, pRect->left, pRect->top, w, h,
			(PBITMAP)pBmp, 0, status*bmp_h, bmp_w, bmp_h);
	}

    box_r = box_l + bmp_w;
    box_b = box_t + bmp_h;
    bgc = DWORD2Pixel (hdc, GetWindowElementAttr (hWnd, WE_BGC_WINDOW));
    //bgc = PIXEL_red;

    /** left-top  corner */
    SetPixel (hdc, box_l, box_t, bgc);
    SetPixel (hdc, box_l + 1, box_t, bgc);
    SetPixel (hdc, box_l + 2, box_t, bgc);
    SetPixel (hdc, box_l + 3, box_t, bgc);
    SetPixel (hdc, box_l + 4, box_t, bgc);
    
    SetPixel (hdc, box_l, box_t + 1, bgc);
    SetPixel (hdc, box_l + 1, box_t + 1, bgc);
    SetPixel (hdc, box_l + 2, box_t + 1, bgc);
    //SetPixel (hdc, box_l + 3, box_t + 1, bgc);

    SetPixel (hdc, box_l, box_t + 2, bgc);
    SetPixel (hdc, box_l + 1, box_t + 2, bgc);
    //SetPixel (hdc, box_l + 2, box_t + 2, bgc);

    SetPixel (hdc, box_l, box_t + 3, bgc);
    //SetPixel (hdc, box_l + 1, box_t + 3, bgc);

    //SetPixel (hdc, box_l, box_t + 4, bgc);


    /** right-top  corner */
    SetPixel (hdc, box_r - 4, box_t, bgc);
    SetPixel (hdc, box_r - 3, box_t, bgc);
    SetPixel (hdc, box_r - 2, box_t, bgc);
    SetPixel (hdc, box_r - 1, box_t, bgc);
    SetPixel (hdc, box_r, box_t, bgc);

    SetPixel (hdc, box_r - 3, box_t + 1, bgc);
    SetPixel (hdc, box_r - 2, box_t + 1, bgc);
    SetPixel (hdc, box_r - 1, box_t + 1, bgc);
    SetPixel (hdc, box_r, box_t + 1, bgc);

    SetPixel (hdc, box_r - 2, box_t + 2, bgc);
    SetPixel (hdc, box_r - 1, box_t + 2, bgc);
    SetPixel (hdc, box_r, box_t + 2, bgc);

    SetPixel (hdc, box_r - 1, box_t + 3, bgc);
    SetPixel (hdc, box_r, box_t + 3, bgc);

    SetPixel (hdc, box_r, box_t + 4, bgc);

    /** left-bottom  corner */
    SetPixel (hdc, box_l, box_b, bgc);
    SetPixel (hdc, box_l + 1, box_b, bgc);
    SetPixel (hdc, box_l + 2, box_b, bgc);
    SetPixel (hdc, box_l + 3, box_b, bgc);
    SetPixel (hdc, box_l + 4, box_b, bgc);
    
    SetPixel (hdc, box_l, box_b - 1, bgc);
    SetPixel (hdc, box_l + 1, box_b - 1, bgc);
    SetPixel (hdc, box_l + 2, box_b - 1, bgc);
    SetPixel (hdc, box_l + 3, box_b - 1, bgc);

    SetPixel (hdc, box_l, box_b - 2, bgc);
    SetPixel (hdc, box_l + 1, box_b - 2, bgc);
    SetPixel (hdc, box_l + 2, box_b - 2, bgc);

    SetPixel (hdc, box_l, box_b - 3, bgc);
    SetPixel (hdc, box_l + 1, box_b - 3, bgc);

    SetPixel (hdc, box_l, box_b - 4, bgc);

    /** right-top  corner */
    SetPixel (hdc, box_r - 4, box_b, bgc);
    SetPixel (hdc, box_r - 3, box_b, bgc);
    SetPixel (hdc, box_r - 2, box_b, bgc);
    SetPixel (hdc, box_r - 1, box_b, bgc);
    SetPixel (hdc, box_r, box_b, bgc);

    SetPixel (hdc, box_r - 4, box_b - 1, bgc);
    SetPixel (hdc, box_r - 3, box_b - 1, bgc);
    SetPixel (hdc, box_r - 2, box_b - 1, bgc);
    SetPixel (hdc, box_r - 1, box_b - 1, bgc);
    SetPixel (hdc, box_r, box_b - 1, bgc);

    SetPixel (hdc, box_r - 2, box_b - 2, bgc);
    SetPixel (hdc, box_r - 1, box_b - 2, bgc);
    SetPixel (hdc, box_r, box_b - 2, bgc);

    SetPixel (hdc, box_r - 1, box_b - 3, bgc);
    SetPixel (hdc, box_r, box_b - 3, bgc);

    SetPixel (hdc, box_r - 1, box_b - 4, bgc);
    SetPixel (hdc, box_r, box_b - 4, bgc);
}

static void 
draw_radio_button (HWND hWnd, HDC hdc, const RECT* pRect, int status)
{
    const BITMAP* bmp = 
        GetSystemBitmapEx (wnd_rdr_fashion.name, SYSBMP_RADIOBUTTON);

    _draw_radio_button (hWnd, hdc, pRect, status, bmp);
}

static void 
draw_check_button (HWND hWnd, HDC hdc, const RECT* pRect, int status)
{
    const BITMAP* bmp = 
        GetSystemBitmapEx (wnd_rdr_fashion.name, SYSBMP_CHECKBUTTON);

    _draw_check_button (hdc, pRect, status, bmp);
}

static int calc_capbtn_area (HWND hWnd, int which, RECT* we_area)
{
    int border;
    int cap_h;
    int capbtn_h;
    const WINDOWINFO* win_info = GetWindowInfo (hWnd);
    PICON icon = (PICON)(win_info->hIcon);
    int icon_h;
    int icon_w;
    int win_w = win_info->right - win_info->left;

    cap_h = GetWindowElementAttr (hWnd, WE_METRICS_CAPTION);

    switch (which)
    {
        case HT_CLOSEBUTTON:            
            CHECKNOT_RET_ERR (IS_CLOSEBTN_VISIBLE (win_info));
            border = get_window_border (hWnd, 0, 0);
            capbtn_h = cap_h - (CAP_BTN_INTERVAL<<1);
            we_area->right = win_w - border - CAP_BTN_INTERVAL;
            we_area->left = we_area->right - capbtn_h;
            we_area->top = border + ((cap_h-capbtn_h)>>1); 
            we_area->bottom = we_area->top + capbtn_h;
            return 0;

        case HT_MAXBUTTON:
            CHECKNOT_RET_ERR (IS_MAXIMIZEBTN_VISIBLE (win_info));
            border = get_window_border (hWnd, 0, 0);
            capbtn_h = cap_h - (CAP_BTN_INTERVAL<<1);

            we_area->right = win_w - border - capbtn_h
                        - (CAP_BTN_INTERVAL<<1);
            we_area->left = we_area->right - capbtn_h;
            we_area->top = border + ((cap_h-capbtn_h)>>1); 
            we_area->bottom = we_area->top + capbtn_h;
            return 0;

        case HT_MINBUTTON:
            CHECKNOT_RET_ERR (IS_MINIMIZEBTN_VISIBLE (win_info));

            border = get_window_border (hWnd, 0, 0);
            capbtn_h = cap_h - (CAP_BTN_INTERVAL<<1);

            we_area->right = win_w - border - (capbtn_h<<1)
                            - CAP_BTN_INTERVAL*3;
            we_area->left = we_area->right - capbtn_h;
            we_area->top = border + ((cap_h-capbtn_h)>>1); 
            we_area->bottom = we_area->top + capbtn_h;
            return 0;

        case HT_ICON:
            CHECKNOT_RET_ERR (IS_CAPICON_VISIBLE (win_info));

            border = get_window_border (hWnd, 0, 0);

            icon_w = icon->width;
            icon_h = icon->height;

            /*icon is larger, zoomin*/
            if (icon_h > cap_h - (CAP_BTN_INTERVAL<<1))
            {
                icon_h = cap_h - (CAP_BTN_INTERVAL<<1);
                icon_w =  icon_w * icon_h / icon->height;
            }

            we_area->left = border + CAP_BTN_INTERVAL;
            we_area->right = we_area->left + icon_w;
            we_area->top = border + ((cap_h - icon_h) >> 1);
            we_area->bottom = we_area->top + icon_h;
            return 0;

        default:
            return -1;
    }
}

static int calc_hscroll_area(HWND hWnd, int which, RECT* we_area)
{
    int border;
    int scrollbar;
    int left_inner = 0;
    int right_inner = 0;
    const WINDOWINFO* win_info = GetWindowInfo (hWnd);
    const LFSCROLLBARINFO* sbar_info = &(win_info->hscroll);

    int win_w = win_info->right - win_info->left;
    int win_h = win_info->bottom - win_info->top;

    border = get_window_border (hWnd, 0, 0);
    scrollbar = GetWindowElementAttr(hWnd, WE_METRICS_SCROLLBAR);

    if (IS_VSCROLL_VISIBLE (win_info)) {
        if (IS_LEFT_VSCOLLBAR (win_info)) 
            left_inner = scrollbar;
        else
            right_inner = scrollbar;
    }

    we_area->bottom = win_h - border;
    we_area->top = we_area->bottom - scrollbar;

    switch (which) {
        case HT_HSCROLL:
            we_area->left = border + left_inner;
            we_area->right = win_w - border - right_inner;
            return 0;

        case HT_SB_HTHUMB:
            we_area->left = border + left_inner + scrollbar
                            + sbar_info->barStart;
            we_area->right = we_area->left + sbar_info->barLen;
            return 0;

        case HT_SB_LEFTARROW:
            we_area->left = border + left_inner;
            we_area->right = we_area->left + scrollbar;
            return 0;

        case HT_SB_RIGHTARROW:
            we_area->right = win_w - border - right_inner;
            we_area->left = we_area->right - scrollbar;
            return 0;

        case HT_SB_LEFTSPACE:
            we_area->left = border + left_inner + scrollbar;
            we_area->right = we_area->left + sbar_info->barStart;
            return 0;

        case HT_SB_RIGHTSPACE:
            we_area->left = border + left_inner + scrollbar
                            + sbar_info->barStart + sbar_info->barLen;
            we_area->right = win_w - border - right_inner - scrollbar;
            return 0;

        default:
            return -1;

    }

}

static int calc_vscroll_area(HWND hWnd, int which, RECT* we_area)
{
    int border;
    int cap_h;
    int menu_h;
    int scrollbar;
    int bottom_inner = 0;

    const WINDOWINFO* win_info = GetWindowInfo (hWnd);
    const LFSCROLLBARINFO* sbar_info = &(win_info->vscroll);

    int win_w = win_info->right - win_info->left;
    int win_h = win_info->bottom - win_info->top;

    border = get_window_border (hWnd, 0, 0);
    cap_h = get_window_caption (hWnd);
    menu_h = get_window_menubar (hWnd);
    scrollbar = GetWindowElementAttr(hWnd, WE_METRICS_SCROLLBAR);

    if (IS_HSCROLL_VISIBLE (win_info))
        bottom_inner = scrollbar;

    if (IS_LEFT_VSCOLLBAR (win_info)) {
        we_area->left = border;
        we_area->right = we_area->left + scrollbar;
    }
    else {
        we_area->right = win_w - border;
        we_area->left = we_area->right - scrollbar;
    }

    switch (which) {
        case HT_VSCROLL:
            we_area->top = border + cap_h + menu_h;
            we_area->bottom = win_h - border - bottom_inner ;
            return 0;

        case HT_SB_VTHUMB:
            we_area->top = border + cap_h + menu_h + scrollbar
                         + sbar_info->barStart;
            we_area->bottom = we_area->top + sbar_info->barLen;
            return 0;

        case HT_SB_UPARROW:
            we_area->top = border + cap_h + menu_h;
            we_area->bottom = we_area->top + scrollbar;
            return 0;

        case HT_SB_DOWNARROW:
            we_area->bottom = win_h - border - bottom_inner;
            we_area->top = we_area->bottom - scrollbar;
            return 0;

        case HT_SB_UPSPACE:
            we_area->top = border + cap_h + menu_h + scrollbar;
            we_area->bottom = we_area->top + sbar_info->barStart;
            return 0;

        case HT_SB_DOWNSPACE:
            we_area->top = border + cap_h + menu_h + scrollbar
                         + sbar_info->barStart + sbar_info->barLen;
            we_area->bottom = win_h - border - scrollbar - bottom_inner;
            return 0;

        default:
            return -1;

    }

}

static int 
get_window_border (HWND hWnd, int dwStyle, int win_type)
{
    int _dwStyle = dwStyle;
    int _type = win_type;

    if (hWnd != HWND_NULL) {
        _type = lf_get_win_type (hWnd);
        _dwStyle = GetWindowStyle (hWnd);
    }

    switch (_type)
    {
        case LFRDR_WINTYPE_MAINWIN:
        {
            if (_dwStyle & WS_BORDER
                ||_dwStyle & WS_THICKFRAME)
                return GetWindowElementAttr (hWnd, WE_METRICS_WND_BORDER) + 3;
            else if(_dwStyle & WS_THINFRAME)
                return GetWindowElementAttr (hWnd, WE_METRICS_WND_BORDER);
        }
        case LFRDR_WINTYPE_DIALOG:
        {
            if (_dwStyle & WS_BORDER
                ||_dwStyle & WS_THICKFRAME)
                return 3;
            else if(_dwStyle & WS_THINFRAME)
                return 0; 
        }
        case LFRDR_WINTYPE_CONTROL:
        {
            if (_dwStyle & WS_BORDER
                    || _dwStyle & WS_THINFRAME
                    || _dwStyle & WS_THICKFRAME)
                return 1; 
        }
    }

    return 0;
}

static int calc_we_metrics (HWND hWnd, 
            LFRDR_WINSTYLEINFO* style_info, int which)
{
    const WINDOWINFO* win_info;
    int         cap_h = 0;
    int         icon_w = 0;
    int         icon_h = 0;
    PLOGFONT    cap_font;
    int         win_width, win_height;
    int         btn_w = 0;

    switch (which & LFRDR_METRICS_MASK)
    {
        case LFRDR_METRICS_BORDER:
            if (style_info)
                return get_window_border (HWND_NULL, 
                        style_info->dwStyle, style_info->winType);
            else if (hWnd != HWND_NULL) 
                return get_window_border (hWnd, 0, 0);
            else
                return GetWindowElementAttr(hWnd, WE_METRICS_WND_BORDER);

        case LFRDR_METRICS_CAPTION_H:
        {
            int cap_h, _idx;

            if (style_info) {
                if (!(style_info->dwStyle & WS_CAPTION)) {
                    return 0;
                }

                _idx = WE_METRICS_CAPTION & WE_ATTR_INDEX_MASK;
                cap_h = wnd_rdr_fashion.we_metrics[_idx];
                return cap_h;
            }
            else if (hWnd != HWND_NULL) {
                win_info = GetWindowInfo(hWnd);
                if (!IS_CAPTION_VISIBLE (win_info))
                    return 0;
            }

            cap_h = GetWindowElementAttr (hWnd, WE_METRICS_CAPTION);
            return cap_h;
        }

        case LFRDR_METRICS_MENU_H:
        {
            int _idx;

            if (style_info) {
                _idx = WE_METRICS_MENU & WE_ATTR_INDEX_MASK;
                return wnd_rdr_fashion.we_metrics[_idx];
            }
            else if (hWnd != HWND_NULL) {
                win_info = GetWindowInfo(hWnd);
                if (!IS_MENUBAR_VISIBLE (win_info))
                    return 0;
            }

            return GetWindowElementAttr (hWnd, WE_METRICS_MENU);
        }

        case LFRDR_METRICS_ICON_H:
        {
            PICON       icon;

            if (hWnd != HWND_NULL) {
                win_info = GetWindowInfo(hWnd);

                if (!IS_CAPTION_VISIBLE (win_info))
                    return 0;

                icon = (PICON)(win_info->hIcon);
                cap_h = 
                    calc_we_metrics (hWnd, style_info, LFRDR_METRICS_CAPTION_H);
                icon_h = icon ? icon->height : 0;

                return (icon_h<cap_h) ? icon_h : cap_h;
            }
            else {
                return 16;
            }
        }
            
        case LFRDR_METRICS_ICON_W:
        {
            PICON       icon;

            if (hWnd != HWND_NULL) {
                win_info = GetWindowInfo(hWnd);

                if (!IS_CAPTION_VISIBLE (win_info))
                    return 0;

                icon = (PICON)(win_info->hIcon);
                cap_h = 
                    calc_we_metrics (hWnd, style_info, LFRDR_METRICS_CAPTION_H);
                icon_h = icon ? icon->height : 0;
                icon_w = icon ? icon->width : 0;

                return  (icon_h<cap_h)? icon_w : (icon_w*cap_h/icon_h);
            }
            else {
                return 16;
            }
        }
            
        case LFRDR_METRICS_VSCROLL_W:
        {
            int _idx;
            if (style_info) {
                if (!(style_info->dwStyle & WS_VSCROLL)) {
                    return 0;
                }
                _idx = WE_METRICS_SCROLLBAR & WE_ATTR_INDEX_MASK;
                return wnd_rdr_fashion.we_metrics[_idx];
            }
            else if (hWnd != HWND_NULL) {
                win_info = GetWindowInfo(hWnd);
                if (!IS_VSCROLL_VISIBLE (win_info))
                    return 0;
            }
            return GetWindowElementAttr (hWnd, WE_METRICS_SCROLLBAR);
        }

        case LFRDR_METRICS_HSCROLL_H:
        {
            int _idx;
            if (style_info) {
                if (!(style_info->dwStyle & WS_HSCROLL)) {
                    return 0;
                }
                _idx = WE_METRICS_SCROLLBAR & WE_ATTR_INDEX_MASK;
                return wnd_rdr_fashion.we_metrics[_idx];
            }
            else if (hWnd != HWND_NULL) {
                win_info = GetWindowInfo(hWnd);
                if (!IS_HSCROLL_VISIBLE (win_info))
                    return 0;
            }
            return GetWindowElementAttr (hWnd, WE_METRICS_SCROLLBAR);
        }

        case LFRDR_METRICS_MINWIN_WIDTH:
        {
            int _style, _win_type;

            win_width = 
                (calc_we_metrics (hWnd, style_info, LFRDR_METRICS_BORDER)<< 1);

            if (style_info) {
                _style = style_info->dwStyle;
                _win_type = style_info->winType;
            }
            else if (hWnd == HWND_NULL) {
                    return win_width;
            }
            else {
                win_info = GetWindowInfo (hWnd);

                _style = win_info->dwStyle;
                _win_type = lf_get_win_type(hWnd);
            }

            if (!(_style & WS_CAPTION))
                return win_width;

            cap_h = 
                calc_we_metrics (hWnd, style_info, LFRDR_METRICS_CAPTION_H);

            if (_win_type==LFRDR_WINTYPE_MAINWIN) {
                icon_w = 
                    calc_we_metrics (hWnd, style_info, LFRDR_METRICS_ICON_W);
                icon_h = 
                    calc_we_metrics (hWnd, style_info, LFRDR_METRICS_ICON_H);
            }

            win_width += CAP_BTN_INTERVAL;

            win_width += icon_w ? (icon_w + CAP_BTN_INTERVAL) : 0;

            cap_font = (PLOGFONT)GetWindowElementAttr (hWnd, WE_FONT_CAPTION);

            win_width += (cap_font) ? 
                         ((cap_font->size << 1) + CAP_BTN_INTERVAL) : 0;

            /*two char and internal*/
            btn_w = GetWindowElementAttr(hWnd, WE_METRICS_CAPTION)
                    - CAP_BTN_INTERVAL;
            
            /*buttons and internvals*/
            win_width += btn_w; 

            if (_style & WS_MINIMIZEBOX)
                win_width += btn_w; 
            if (_style & WS_MAXIMIZEBOX)
                win_width += btn_w; 

            return win_width;
        }

        case LFRDR_METRICS_MINWIN_HEIGHT:
        {
            win_height =
                (calc_we_metrics (hWnd, style_info, LFRDR_METRICS_BORDER)<< 1);
            win_height += 
                calc_we_metrics (hWnd, style_info, LFRDR_METRICS_CAPTION_H);

            return win_height;
        }
    }
    return 0;
}

static int calc_we_area (HWND hWnd, int which, RECT* we_area)
{
    int border;
    int cap_h;
    int menu_h;
    const WINDOWINFO* win_info = NULL;

    int win_w = 0;
    int win_h = 0;

    win_info = GetWindowInfo (hWnd);
    win_w = win_info->right - win_info->left;
    win_h = win_info->bottom - win_info->top;

    switch (which) {
        case HT_BORDER:
            CHECKNOT_RET_ERR(IS_BORDER_VISIBLE (win_info));
            we_area->left = 0;
            we_area->right = win_w;
            we_area->top = 0;
            we_area->bottom = win_h;
            return 0;

        case HT_CAPTION:
            CHECKNOT_RET_ERR (IS_CAPTION_VISIBLE (win_info));
            border = get_window_border (hWnd, 0, 0);
            cap_h = get_window_caption(hWnd);
            we_area->left = border;
            we_area->right = win_w - border;
            we_area->top = border;
            we_area->bottom = we_area->top + cap_h; 
            return 0;

        case HT_CLOSEBUTTON:            
        case HT_MAXBUTTON:
        case HT_MINBUTTON:
        case HT_ICON:
            CHECKNOT_RET_ERR (IS_CAPTION_VISIBLE (win_info));
            return calc_capbtn_area(hWnd, which, we_area);

        case HT_MENUBAR:
            CHECKNOT_RET_ERR (IS_MENUBAR_VISIBLE (win_info));
            border = get_window_border (hWnd, 0, 0);
            cap_h = get_window_caption (hWnd);
            menu_h = get_window_menubar(hWnd);

            we_area->left = border;
            we_area->top = border + cap_h;
            we_area->right = win_w - border; 
            we_area->bottom = we_area->top + menu_h;
            return 0;

        case HT_CLIENT:
            border = get_window_border (hWnd, 0, 0);
            cap_h = get_window_caption (hWnd);
            menu_h = get_window_menubar (hWnd);

            we_area->top = border + cap_h + menu_h;
            we_area->bottom = win_h - border - 
                get_window_scrollbar(hWnd, FALSE);

            if (IS_LEFT_VSCOLLBAR (win_info)) {
                we_area->left = border +
                    get_window_scrollbar(hWnd, TRUE);
                we_area->right = win_w - border;
            }
            else
            {
                we_area->left = border;
                we_area->right = win_w - border -
                    get_window_scrollbar(hWnd, TRUE);
            }
            return 0;

        case HT_HSCROLL:
            CHECKNOT_RET_ERR (IS_HSCROLL_VISIBLE (win_info));
            return calc_hscroll_area(hWnd, which, we_area);

        case HT_VSCROLL:
            CHECKNOT_RET_ERR (IS_VSCROLL_VISIBLE (win_info));
            return calc_vscroll_area(hWnd, which, we_area);

        default:
            if (which & HT_SB_MASK) {
                if (which < HT_SB_UPARROW) {
                    CHECKNOT_RET_ERR (IS_HSCROLL_VISIBLE (win_info));
                    return calc_hscroll_area(hWnd, which, we_area);
                }
                else {
                    CHECKNOT_RET_ERR (IS_VSCROLL_VISIBLE (win_info));
                    return calc_vscroll_area(hWnd, which, we_area);
                }
            }
            else
                return -1;
    }
}


static void 
calc_thumb_area (HWND hWnd, BOOL vertical, LFSCROLLBARINFO* sb_info)
{
    RECT rc;
    int move_range;
    div_t divt;
    
    if (vertical) {
        *sb_info = GetWindowInfo(hWnd)->vscroll;
        calc_vscroll_area(hWnd, HT_VSCROLL, &rc);
        sb_info->arrowLen  = RECTW (rc);
        move_range = RECTH (rc) - (sb_info->arrowLen << 1);
    }
    else {
        *sb_info = GetWindowInfo(hWnd)->hscroll;
        calc_hscroll_area(hWnd, HT_HSCROLL, &rc);
        sb_info->arrowLen  = RECTH (rc);
        move_range = RECTW (rc) - (sb_info->arrowLen << 1);
    }

    if (move_range < 0)
        move_range = 0;

    divt = div (move_range, sb_info->maxPos - sb_info->minPos + 1);
    sb_info->barLen = sb_info->pageStep * divt.quot +
        sb_info->pageStep * divt.rem / 
        (sb_info->maxPos - sb_info->minPos + 1);

    if (sb_info->barLen < LFRDR_SB_MINBARLEN)
        sb_info->barLen = LFRDR_SB_MINBARLEN;

    if (sb_info->minPos == sb_info->maxPos)
    {
        sb_info->barStart = 0;
        sb_info->barLen   = move_range;
    }
    else
    {
        divt = div (move_range, sb_info->maxPos - sb_info->minPos + 1);
        sb_info->barStart = 
            (sb_info->curPos - sb_info->minPos) * divt.quot + 
            (sb_info->curPos - sb_info->minPos) * divt.rem /
            (sb_info->maxPos - sb_info->minPos + 1) + 0.5;

        if (sb_info->barStart + sb_info->barLen > move_range)
            sb_info->barStart = move_range - sb_info->barLen;
        if (sb_info->barStart < 0)
            sb_info->barStart = 0;
    }
}

static int find_interval(int* array, int len, int val)
{
    int i;
    for (i=0; i<len-1; i++)
    {
        if (array[i]<=val && val<array[i+1])
            break;
    }

    if (i == len-1)
        return -1;
    else
        return i;
}

/*
 * test_caption:
 *     test mouse in which part of caption 
 * Author: XuguangWang
 * Date: 2007-11-22
 */
static int test_caption(HWND hWnd, int x, int y)
{
    RECT rc;
    if (calc_we_area(hWnd, HT_ICON, &rc) == 0)
        CHECK_RET_VAL(PtInRect(&rc, x, y), HT_ICON);

    if (calc_we_area(hWnd, HT_MINBUTTON, &rc) == 0)
        CHECK_RET_VAL(PtInRect(&rc, x, y), HT_MINBUTTON);

    if (calc_we_area(hWnd, HT_MAXBUTTON, &rc) == 0)
        CHECK_RET_VAL(PtInRect(&rc, x, y), HT_MAXBUTTON);

    if (calc_we_area(hWnd, HT_CLOSEBUTTON, &rc) == 0)
        CHECK_RET_VAL(PtInRect(&rc, x, y), HT_CLOSEBUTTON);

    return HT_CAPTION;
}

/*
 * test_scroll:
 *     test mouse in which part of the scrollbar indicated by is_vertival. 
 * Author: XuguangWang
 * Date: 2007-11-22
 */
static int test_scroll(const LFSCROLLBARINFO* sb_info, 
        int left, int right, int x, BOOL is_vertival)
{
    static int x_poses[5] = {
        HT_SB_LEFTARROW, HT_SB_LEFTSPACE, HT_SB_HTHUMB,
        HT_SB_RIGHTSPACE, HT_SB_RIGHTARROW
    };

    int array[6];
    int x_pos;
    array[0] = left;
    array[1] = left+sb_info->arrowLen;
    array[2] = array[1] + sb_info->barStart;
    array[3] = array[2] + sb_info->barLen;
    array[5] = right;
    array[4] = array[5] - sb_info->arrowLen;

    x_pos = find_interval(array, 6, x);

    if (x_pos == -1)
        return HT_SB_UNKNOWN;
    else
        return x_poses[x_pos]|((is_vertival) ? HT_SB_VMASK : 0);
}

/*
 * hit_test:
 *      get which window element including parts of caption, scrollbars
 *      mouse in.
 * Author: XuguangWang
 * Date: 2007-11-22
 */
static int hit_test (HWND hWnd, int x, int y)
{
    static const int ht_inner_border [4] = {
        HT_CAPTION, HT_MENUBAR, HT_VSCROLL, HT_HSCROLL
    };

    static const int ht_on_border [3][3] = {
        {HT_CORNER_TL,   HT_BORDER_TOP,    HT_CORNER_TR},
        {HT_BORDER_LEFT, HT_UNKNOWN,       HT_BORDER_RIGHT},
        {HT_CORNER_BL,   HT_BORDER_BOTTOM, HT_CORNER_BR},
    };

    RECT rc;
    int border;
    int x_pos;
    int y_pos;
    int array[5];
    int tmp;
    int win_w;
    int win_h;

    const WINDOWINFO* win_info = GetWindowInfo(hWnd);
    win_w = win_info->right - win_info->left;
    win_h = win_info->bottom - win_info->top;

    /*mouse not in this window*/
    CHECK_RET_VAL(!PtInRect((RECT*)win_info, x, y), HT_OUT);

    /*mouse in client area*/
    CHECK_RET_VAL(PtInRect((RECT*)&(win_info->cl), x, y), HT_CLIENT);

    /*change x y to hwnd coordiante*/
    x -= win_info->left;
    y -= win_info->top;

    border = get_window_border (hWnd, 0, 0);
    SetRect(&rc, border, border, win_w-border, win_h-border);

    /*mouse on border*/
    if (!PtInRect(&rc, x, y)) {
        if (!(win_info->dwStyle & WS_CAPTION) || (win_info->dwStyle & WS_DLGFRAME))
            return HT_UNKNOWN;

        //tmp = GetWindowElementAttr(hWnd, WE_METRICS_CAPTION);
        tmp = get_window_caption(hWnd);
        array[0] = 0;
        array[1] = array[0] + tmp;
        array[3] = win_w;
        array[2] = array[3] - tmp;
        x_pos = find_interval(array, 4, x);

        array[0] = 0;
        array[1] = array[0] + tmp;
        array[3] = win_h;
        array[2] = array[3] - tmp;
        y_pos = find_interval(array, 4, y);    

        if (x_pos!=-1 && y_pos!=-1)
            return ht_on_border[y_pos][x_pos];
        else
            return HT_UNKNOWN;
    }
    /*mouse inner border*/
    else {
        array[2] = win_info->ct - win_info->top;
        array[1] = array[2] - get_window_menubar(hWnd);
        array[0] = array[1] - get_window_caption(hWnd);
        array[3] = win_info->cb - win_info->top;
        array[4] = array[3] + get_window_scrollbar(hWnd, FALSE);
        y_pos = find_interval(array, 5, y);    
        if (y_pos != -1)
            switch (ht_inner_border[y_pos]) {
                case HT_CAPTION:
                    return test_caption(hWnd, x, y);

                case HT_MENUBAR:
                    return HT_MENUBAR;

                case HT_HSCROLL:
                    return test_scroll(&(win_info->hscroll),
                           win_info->cl - win_info->left, 
                           win_info->cr - win_info->left, 
                           x, FALSE);

                case HT_VSCROLL:
                    return test_scroll(&(win_info->vscroll), 
                            win_info->ct - win_info->top, 
                            win_info->cb - win_info->top, 
                            y, TRUE);
                default:
                    return HT_UNKNOWN;
            }
        else
            return HT_UNKNOWN;
    }

}

/* draw_border:
 *   This function draw the border of a window. 
 *
 * \param hWnd : the handle of the window.
 * \param hdc : the DC of the window.
 * \param is_active : whether the window is actived.
 * \return : 0 for succeed, other for failure. 
 *
 * Author: wangjian<wangjian@minigui.org>
 * Date: 2007-12-13
 */
static void draw_border (HWND hWnd, HDC hdc, BOOL is_active)
{
    int i , border;
    const WINDOWINFO *win_info = NULL;
    RECT rect;
    DWORD c1, c2;
    int l, t, r, b;
    gal_pixel old_pen_color;
    
    if(calc_we_area(hWnd, HT_BORDER, &rect) == -1)
        return;
    
    l = rect.left;
    t = rect.top;
    r = rect.right-1;
    b = rect.bottom-1;
    
    if(is_active) {
        c1 = GetWindowElementAttr(hWnd, WE_BGCB_ACTIVE_CAPTION);
    } else {
        c1 = GetWindowElementAttr(hWnd, WE_BGCB_INACTIVE_CAPTION);
    }
    c2 = c1;
    
    border = (int)GetWindowElementAttr(hWnd, WE_METRICS_WND_BORDER);
    win_info = GetWindowInfo(hWnd);
    old_pen_color = GetPenColor(hdc);

    if (IsMainWindow (hWnd)) // for main window
    {
        if (IsDialog(hWnd))   // for dialog
            border = 0;    
        if(win_info->dwStyle & WS_BORDER
                || win_info->dwStyle & WS_THICKFRAME)
        {
            /** left and top border */
            SetPenColor(hdc, DWORD2Pixel (hdc, c1));
            MoveTo(hdc, l, b);
            LineTo(hdc, l, t);
            LineTo(hdc, r - 1, t);
            for(i = 1; i < 4 && i < border + 3; i++)
            {
                c2 = gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 30);
                SetPenColor(hdc, DWORD2Pixel (hdc, c2));
                MoveTo(hdc, l+i, b-i);
                LineTo(hdc, l+i, t+i);
                LineTo(hdc, r+i - 1, t+i);
            }

            c2 = gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 30);
            SetPenColor(hdc, DWORD2Pixel (hdc, c2));
            for(;i < border+3; i++)
            {
                MoveTo(hdc, l+i, b-i);
                LineTo(hdc, l+i, t+i);
                LineTo(hdc, r+i - 1, t+i);
            }

            /** right and bottom border */
            c2 = gradient_color (c1, LFRDR_3DBOX_COLOR_DARKER, 80);
            for(i = 0; i < 3 && i < border + 3; i++)
            {
                c2 = gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 80);
                SetPenColor(hdc, DWORD2Pixel (hdc, c2));
                MoveTo(hdc, l+i + 1, b-i);
                LineTo(hdc, r-i, b-i);
                LineTo(hdc, r-i, t+i);
            }

            c2 = gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 80);
            SetPenColor(hdc, DWORD2Pixel (hdc, c2));
            for(;i < border+3; i++)
            {
                MoveTo(hdc, l+i + 1, b-i);
                LineTo(hdc, r-i, b-i);
                LineTo(hdc, r-i, t+i);
            }
        }
        else if(win_info->dwStyle & WS_THINFRAME)
        {
            SetPenColor(hdc, DWORD2Pixel (hdc, c1));
            for(i = 0; i < border; i++)
            {
                Rectangle(hdc, rect.left+i, rect.top+i,
                        rect.right-i-1, rect.bottom-i-1);
            }
        }
    }
    else if(IsControl(hWnd))   // for control
    {
        if(win_info->dwStyle & WS_BORDER
                || win_info->dwStyle & WS_THICKFRAME
                || win_info->dwStyle & WS_THINFRAME)    
        {
            SetPenColor(hdc, DWORD2Pixel (hdc, c2));
            Rectangle(hdc, rect.left, rect.top,
                    rect.right-1, rect.bottom-1);
        }
        
    }

    SetPenColor(hdc, old_pen_color);
}

/* draw_caption:
 *   This function draw the caption of a window. 
 *
 * \param hWnd : the handle of the window.
 * \param hdc : the DC of the window.
 * \param is_active : whether the window is actived.
 *
 * Author: wangjian<wangjian@minigui.org>
 * Date: 2007-12-13
 */
#define ICON_ORIGIN 2
static void draw_caption (HWND hWnd, HDC hdc, BOOL is_active)
{
    int font_h, i;
    int j = 0;
    gal_pixel pixel;
    gal_pixel text_color, old_text_color, old_pen_color;
    PLOGFONT cap_font, old_font;
    const WINDOWINFO *win_info;
    RECT rect, del_rect, icon_rect, rcTmp;
    int win_w;
    int border;

    DWORD ca, cb;
    mgplus_pattern_t ptn;
    POINT point;
    RECT rect_line;
    gal_pixel pixel_dst;
    gal_pixel pixel_org;
    gal_pixel pixels[2];

    SetRectEmpty (&icon_rect);
    win_info = GetWindowInfo(hWnd);
    
    if (!(win_info->dwStyle & WS_CAPTION))
        return;
    if(calc_we_area(hWnd, HT_CAPTION, &rect) == -1)
        return;
    if(RECTH(rect) <= 1 || RECTW(rect) <= 0)
        return;
    
    cap_font = (PLOGFONT)GetWindowElementAttr(hWnd, WE_FONT_CAPTION);

    if(is_active) {
        text_color = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_ACTIVE_CAPTION);
        ca = GetWindowElementAttr(hWnd, WE_BGCA_ACTIVE_CAPTION);
        cb = GetWindowElementAttr(hWnd, WE_BGCB_ACTIVE_CAPTION);
    } else {
        text_color = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_INACTIVE_CAPTION);
        ca = GetWindowElementAttr(hWnd, WE_BGCA_INACTIVE_CAPTION);
        cb = GetWindowElementAttr(hWnd, WE_BGCB_INACTIVE_CAPTION);
    }

    /** draw backgroup right */

    pixel = ca;
    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    pixel = gradient_color (ca, LFRDR_3DBOX_COLOR_LIGHTER, 180);
    pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#if 0
    pixels[1] = DWORD2Pixel (hdc, ca);
    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (ca, LFRDR_3DBOX_COLOR_LIGHTER, 180));
#endif
    ptn = mGPlusCreateLinearPattern (rect, 
            MGPLUS_GRADIENT_LINEAR_VERTICAL, pixels, 2, MGPLUS_GRADIENT_BIT_16);
    old_pen_color = GetPenColor (hdc);
    pixel_org = GetPenColor (hdc);
    point.x = rect.left;
    for (i = rect.top; i <= rect.bottom - 1; ++i) {
        point.y = i;
        if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
            SetPenColor (hdc, pixel_dst);
            MoveTo (hdc, point.x, point.y);
            LineTo (hdc, rect.right - 1, point.y);
        }
        else {
            printf ("mGPlusGetGradientColorValue return false.\n");
        }
    }

    SetPenColor (hdc, pixel_org);
    mGPlusFreeColorPattern (ptn);
    
    /** draw backgroup left */
    memset (&rcTmp, 0, sizeof (RECT));
    if (win_info->dwStyle & WS_MINIMIZEBOX) {
        calc_we_area(hWnd, HT_MINBUTTON, &rcTmp);
    }
    else if (win_info->dwStyle & WS_MAXIMIZEBOX) {
        calc_we_area(hWnd, HT_MAXBUTTON, &rcTmp);
    }
    else {
        calc_we_area(hWnd, HT_CLOSEBUTTON, &rcTmp);
    }

#if 0
    pixels[1] = DWORD2Pixel (hdc, 
        gradient_color (cb, LFRDR_3DBOX_COLOR_DARKER, 60));
    pixels[0] = DWORD2Pixel (hdc, 
        gradient_color (cb, LFRDR_3DBOX_COLOR_LIGHTER, 80));
#endif
    pixel = gradient_color (cb, LFRDR_3DBOX_COLOR_LIGHTER, 80);
    pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    
    pixel = gradient_color (cb, LFRDR_3DBOX_COLOR_DARKER, 60);
    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    
    ptn = mGPlusCreateLinearPattern (rect, 
            MGPLUS_GRADIENT_LINEAR_VERTICAL, pixels, 2, MGPLUS_GRADIENT_BIT_16);
    pixel_org = GetPenColor (hdc);
    point.x = rect.left;
    for (i = rect.bottom - 1; i>= rect.top; --i, ++j) {
        point.y = i;
        if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
            SetPenColor (hdc, pixel_dst);
            MoveTo (hdc, point.x, point.y);
            LineTo (hdc, rcTmp.left - 10 - j, point.y);
        }
        else {
            printf ("mGPlusGetGradientColorValue return false.\n");
        }
    }

    SetPenColor (hdc, pixel_org);
    mGPlusFreeColorPattern (ptn);

    /** draw backgroup 3 */

    /** draw the first dark line */
#if 0
    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (cb, LFRDR_3DBOX_COLOR_LIGHTER, 40));
    pixels[1] = DWORD2Pixel (hdc, gradient_color 
            (cb, LFRDR_3DBOX_COLOR_LIGHTER, 65));
#endif
    
    pixel = gradient_color (cb, LFRDR_3DBOX_COLOR_LIGHTER, 40);
    pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    pixel = gradient_color (cb, LFRDR_3DBOX_COLOR_LIGHTER, 65);
    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    
    rect_line.left = rect.left;
    rect_line.top = rect.top + 2;
    rect_line.right = rect_line.left + rcTmp.left - 10 - (rect.bottom - rect.top) - 10;
    rect_line.bottom = rect_line.top + 1;
   
    SetPenColor (hdc, DWORD2Pixel (hdc, gradient_color (cb, LFRDR_3DBOX_COLOR_LIGHTER, 40)));
    MoveTo (hdc, rect_line.left, rect_line.top);
    LineTo (hdc, rect_line.right - 10, rect_line.top);
    
    rect_line.left = rect_line.right - 10;
    ptn = mGPlusCreateLinearPattern (rect_line, 
            MGPLUS_GRADIENT_LINEAR_HORIZONTAL, pixels, 2, MGPLUS_GRADIENT_BIT_16);

    /* Draw right little */
    for (i = rect_line.left; i <= rect_line.right; ++i) {
        point.x = i;
        point.y = rect_line.top;

        if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
            SetPixel (hdc, point.x, point.y, pixel_dst);
        }
        else {
            printf ("mGPlusGetGradientColorValue return false.\n");
        }
    }

    mGPlusFreeColorPattern (ptn);

    /** draw the second dark line */

    rect_line.left = rect.left;
    rect_line.top = rect.bottom - 4;
    rect_line.bottom = rect_line.top + 1;
    rect_line.right = rect_line.left + rcTmp.left - 10 - 20;
#if 0
    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (cb, LFRDR_3DBOX_COLOR_DARKER, 100));
    pixels[1] = DWORD2Pixel (hdc, gradient_color 
            (cb, LFRDR_3DBOX_COLOR_DARKER, 50));
#endif

    pixel = gradient_color (cb, LFRDR_3DBOX_COLOR_DARKER, 100);
    pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    pixel = gradient_color (cb, LFRDR_3DBOX_COLOR_DARKER, 50);
    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    
    SetPenColor (hdc, DWORD2Pixel (hdc, gradient_color (cb, LFRDR_3DBOX_COLOR_DARKER, 100)));
    MoveTo (hdc, rect_line.left, rect_line.top);
    LineTo (hdc, rect_line.right - 10, rect_line.top);

    rect_line.left = rect_line.right - 10;
    ptn = mGPlusCreateLinearPattern (rect_line, 
            MGPLUS_GRADIENT_LINEAR_HORIZONTAL, pixels, 2, MGPLUS_GRADIENT_BIT_16);

    for (i = rect_line.left; i <= rect_line.right; ++i) {
        point.x = i;
        point.y = rect_line.top;

        if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
            SetPixel (hdc, point.x, point.y, pixel_dst);
        }
        else {
            printf ("mGPlusGetGradientColorValue return false.\n");
        }
    }

    mGPlusFreeColorPattern (ptn);

    /** draw the third light line */
#if 0 
    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (cb, LFRDR_3DBOX_COLOR_LIGHTER, 100));
    pixels[1] = DWORD2Pixel (hdc, gradient_color 
            (cb, LFRDR_3DBOX_COLOR_LIGHTER, 70));
#endif

    pixel = gradient_color (cb, LFRDR_3DBOX_COLOR_LIGHTER, 100);
    pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    pixel = gradient_color (cb, LFRDR_3DBOX_COLOR_LIGHTER, 70);
    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    
    
    rect_line.left = rect.left;
    rect_line.top = rect.top + 3;
    rect_line.right = rect_line.left + rcTmp.left - 10 - (rect.bottom - rect.top) - 10;
    rect_line.bottom = rect_line.top + 1;

    SetPenColor (hdc, DWORD2Pixel (hdc, gradient_color (cb, LFRDR_3DBOX_COLOR_LIGHTER, 100)));
    MoveTo (hdc, rect_line.left, rect_line.top);
    LineTo (hdc, rect_line.right - 10, rect_line.top);

    rect_line.left = rect_line.right - 10;

    ptn = mGPlusCreateLinearPattern (rect_line, 
            MGPLUS_GRADIENT_LINEAR_HORIZONTAL, pixels, 2, MGPLUS_GRADIENT_BIT_16);

    for (i = rect_line.left; i <= rect_line.right; ++i) {
        point.x = i;
        point.y = rect_line.top;

        if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
            SetPixel (hdc, point.x, point.y, pixel_dst);
        }
        else {
            printf ("mGPlusGetGradientColorValue return false.\n");
        }
    }

    mGPlusFreeColorPattern (ptn);

    /** draw the fourth light line */

    rect_line.left = rect.left;
    rect_line.top = rect.bottom - 3;
    rect_line.bottom = rect_line.top + 1;
    rect_line.right = rect_line.left + rcTmp.left - 10 - 20;
    /** FIXE ME */
#if 0 
    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (cb, LFRDR_3DBOX_COLOR_LIGHTER, 40));
    pixels[1] = DWORD2Pixel (hdc, gradient_color 
            (cb, LFRDR_3DBOX_COLOR_DARKER, 55));
#else 
    pixel = gradient_color (cb, LFRDR_3DBOX_COLOR_LIGHTER, 40);
    pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    pixel = gradient_color (cb, LFRDR_3DBOX_COLOR_DARKER, 55);
    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
//    pixel = DWORD2Pixel (hdc, gradient_color 
//            (cb, LFRDR_3DBOX_COLOR_LIGHTER, 40));
#endif
    SetPenColor (hdc, DWORD2Pixel (hdc, gradient_color (cb, LFRDR_3DBOX_COLOR_LIGHTER, 40)));
    MoveTo (hdc, rect_line.left, rect_line.top);
    LineTo (hdc, rect_line.right - 10, rect_line.top);

    rect_line.left = rect_line.right - 10;

    //pixels[1] = DWORD2Pixel (hdc, cb);

    ptn = mGPlusCreateLinearPattern (rect_line, 
            MGPLUS_GRADIENT_LINEAR_HORIZONTAL, pixels, 2, MGPLUS_GRADIENT_BIT_16);

    for (i = rect_line.left; i <= rect_line.right; ++i) {
        point.x = i;
        point.y = rect_line.top;

        if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
            SetPixel (hdc, point.x, point.y, pixel_dst);
        }
        else {
            printf ("mGPlusGetGradientColorValue return false.\n");
        }
    }

    mGPlusFreeColorPattern (ptn);
    /** draw icon */
    if(win_info->hIcon)
    {
        if(calc_we_area(hWnd, HT_ICON, &icon_rect) != -1)
        DrawIcon(hdc, icon_rect.left, icon_rect.top,
                    RECTW(icon_rect), RECTH(icon_rect), win_info->hIcon);
    }

    /** draw caption title */
    
    if(win_info->spCaption)
    {
        border = get_window_border (hWnd, win_info->dwStyle, 
                lf_get_win_type(hWnd));
        win_w = win_info->right - win_info->left;

        if (calc_we_area(hWnd, HT_MINBUTTON, &del_rect) != -1){

        }
        else if(calc_we_area(hWnd, HT_CLOSEBUTTON, &del_rect) != -1){

        }
        else if(calc_we_area(hWnd, HT_MAXBUTTON, &del_rect) != -1){

        }
        else 
        {
            del_rect.left = win_w - border;
        }
        del_rect.right = win_w;
        del_rect.left -= CAP_BTN_INTERVAL;

        del_rect.top = border;
        del_rect.bottom = border + 
                    GetWindowElementAttr (hWnd, WE_METRICS_CAPTION);

        ExcludeClipRect (hdc, &del_rect);
        SetBkMode(hdc, BM_TRANSPARENT);
        old_font = SelectFont(hdc, cap_font);
        font_h = GetFontHeight(hdc);
        old_text_color = SetTextColor(hdc,text_color);

        calc_we_area(hWnd, HT_CLOSEBUTTON, &rcTmp);
        if (win_info->dwStyle & WS_MAXIMIZEBOX)
        {
            calc_we_area(hWnd, HT_MAXBUTTON, &rcTmp);
        }

        if (win_info->dwStyle & WS_MINIMIZEBOX)
        {
            calc_we_area(hWnd, HT_MINBUTTON, &rcTmp);
        }
        
        TextOutOmitted (hdc, rect.left + RECTW(icon_rect)+ 
                (ICON_ORIGIN << 1),
                rect.top + ((RECTH(rect)-font_h)>>1), 
                win_info->spCaption,
                strlen (win_info->spCaption), rcTmp.left - 
                (rect.left + RECTW(icon_rect)+ (ICON_ORIGIN << 1)));

        SetTextColor(hdc, old_text_color);
        SelectFont(hdc, old_font);
        IncludeClipRect (hdc, &del_rect);
    }
    
    SetPenColor(hdc, old_pen_color);
}

/*
 * This function make a alpha color, alpha = 0.5
 * used by the funtion draw_caption_button.
 * Author: wangjian<wangjian@minigui.org>
 * Date: 2007-12-13.
 */
static DWORD calc_alpha(DWORD c1, DWORD c2)
{
    BYTE r,g,b;
    BYTE r1,g1,b1;
    BYTE r2,g2,b2;

    r1 = GetRValue(c1);
    r2 = GetRValue(c2);
    g1 = GetGValue(c1);
    g2 = GetGValue(c2);
    b1 = GetBValue(c1);
    b2 = GetBValue(c2);

    r = (r1 + r2) >> 1;
    g = (g1 + g2) >> 1;
    b = (b1 + b2) >> 1;
    
    return MakeRGB(r,g,b);

}

typedef struct PathCxt {
    HDC hdc;
    mgplus_pattern_t ptn;
}PathCxt;

void fill_path_cb (void *context, int x1, int x2, int y)
{
    HDC hdc = ((PathCxt *) context)->hdc;
    mgplus_pattern_t ptn = ((PathCxt *)context)->ptn;
    POINT point;
    gal_pixel pixel;

    if (!ptn)
        return;

#if 0
    point.x = x1;
    point.y = y;
    if (!mGPlusGetGradientColorValue (ptn, &point, &pixel))
    {
        printf ("Error: get gradient color fail.\n");
        return;
    }

    gal_pixel pen_org;
    pen_org = SetPenColor (hdc, pixel);
    MoveTo (hdc, x1, y);
    LineTo (hdc, x2, y);
    SetPenColor (hdc, pen_org);
#else
    int i;
    point.y = y;
    for (i = x1; i <= x2; ++i) {
        point.x = i;
        if (!mGPlusGetGradientColorValue (ptn, &point, &pixel))
        {
            printf ("Error: get gradient color fail---.\n");
            return;
        }

        SetPixel (hdc, point.x, point.y, pixel);
    }
#endif
}

/* draw_caption_button:
 *   This function draw the caption button of a window. 
 *
 * \param hWnd : the handle of the window.
 * \param hdc : the DC of the window.
 * \param ht_code : the number for close, max, or min button, 0 for all.
 * \param status : the status of the button drawing.
 *
 * Author: wangjian<wangjian@minigui.org>
 * Date: 2007-12-15
 */
static void 
draw_caption_button (HWND hWnd, HDC hdc, int ht_code, int status)
{
    int i, w, h;
    RECT rect;
    gal_pixel old_col;
    gal_pixel pixel;
    DWORD cap_c, alp_c;
    MGPLUS_PATH   *path;
    POINT inPoint, outPoint;
        
    old_col = GetPenColor(hdc);
    cap_c = GetWindowElementAttr(hWnd, WE_BGCB_ACTIVE_CAPTION);
    gal_pixel pixels[2];
#if 0
    pixel = ca;
    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
    pixel = gradient_color (ca, LFRDR_3DBOX_COLOR_LIGHTER, 180);
    pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#endif
    switch(ht_code)
    {
        case 0:                     // draw all the 3 buttons as normal.
            status = LFRDR_BTN_STATUS_NORMAL;
        case HT_CLOSEBUTTON:
            if(calc_we_area(hWnd, HT_CLOSEBUTTON, &rect) != -1)
            {
                if(RECTH(rect) <= 0 || RECTW(rect) <= 0)
                    return;
                if(status & LFRDR_BTN_STATUS_PRESSED){
#if 0
                    pixels[1] = DWORD2Pixel (hdc, gradient_color 
                            (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 50));
#endif
                    pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 50);
                    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                }
                else if(status & LFRDR_BTN_STATUS_HILITE){
#if 0
                    pixels[1] = DWORD2Pixel (hdc, gradient_color 
                            (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 80));
#endif
                    pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 80);
                    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                }
                else {
#if 0
                    pixels[1] = DWORD2Pixel (hdc, gradient_color 
                            (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 10));
#endif
                    pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 10);
                    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                }
#if 0
                pixels[0] = DWORD2Pixel (hdc, gradient_color 
                        (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 250));
#endif

                pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 250);
                pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
                path = mGPlusCreatePath(MGPLUS_PATH_WIND_EVEN_ODD);
                if (!path)
                    return;

                mGPlusNewSubPath (path);
                mGPlusPathAddCircle (path, (rect.left + rect.right) >> 1, 
                        (rect.top + rect.bottom) >> 1, RECTH (rect) >> 1);

                PathCxt context;
                context.hdc = hdc;
#if 0
                context.ptn = mGPlusCreateLinearPattern (rect, 
                        MGPLUS_GRADIENT_LINEAR_VERTICAL,
                        pixels, 2);
#else
                inPoint.x = ((rect.left + rect.right) >> 1) - 3;
                inPoint.y = rect.top + 3;

                outPoint.x = ((rect.left + rect.right) >> 1);// + 2;
                //outPoint.y = rect.bottom - 3;
                outPoint.y = ((rect.top + rect.bottom) >> 1); //+ 2;

                context.ptn = mGPlusCreateRadialPattern (&inPoint, 
                        //&outPoint, 0, ((RECTW (rect) * 3) >> 2) + 1,
                        &outPoint, 0, RECTW(rect)/2,
                        pixels, 2, MGPLUS_GRADIENT_BIT_16);
#endif
                if (!context.ptn)
                    return;

                mGPlusPathGenerator(path, fill_path_cb, (void*)&context);
                mGPlusFreePath (path);

                alp_c = calc_alpha(cap_c, pixels[1]);
                SetPenColor(hdc, RGB2Pixel(hdc, GetRValue(alp_c), 
                                        GetGValue(alp_c), GetBValue(alp_c)));
                Circle(hdc, 
                    rect.left + (RECTW(rect)>>1), rect.top + (RECTW(rect)>>1),
                    (RECTW(rect)>>1));
             
                SetPenColor(hdc, PIXEL_black);
                
                rect.right --; rect.bottom --;
                
                w = RECTW(rect);
                h = RECTH(rect);

                rect.left   += (w>>2)+1;
                rect.top    += (h>>2)+1;
                rect.right  -= (w>>2)+1;
                rect.bottom -= (h>>2)+1;

                for(i = 0; i < (w>>4)+2; i++) // the pen width : w/4-1
                {
                    MoveTo(hdc, rect.left, rect.top + i);
                    LineTo(hdc, rect.right - i, rect.bottom);
                    MoveTo(hdc, rect.left + i, rect.bottom);
                    LineTo(hdc, rect.right, rect.top + i);
                    
                    MoveTo(hdc, rect.left + i, rect.top);
                    LineTo(hdc, rect.right, rect.bottom - i);
                    MoveTo(hdc, rect.left, rect.bottom - i);
                    LineTo(hdc, rect.right - i, rect.top);
                }
            } 
            if(ht_code == HT_CLOSEBUTTON) break;

        case HT_MAXBUTTON:
            if(calc_we_area(hWnd, HT_MAXBUTTON, &rect) != -1)
            {
                if(RECTH(rect) <= 0 || RECTW(rect) <= 0)
                    return;
                if(status & LFRDR_BTN_STATUS_PRESSED){
#if 0 
                    pixels[1] = DWORD2Pixel (hdc, gradient_color 
                            //(cap_c, LFRDR_3DBOX_COLOR_DARKER, 90));
                            (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 50));
#else
                    pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 50);
                    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#endif
                }
                else if(status & LFRDR_BTN_STATUS_HILITE){
#if 0 
                    pixels[1] = DWORD2Pixel (hdc, gradient_color 
                            //(cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 30));
                            (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 80));
#else
                    pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 80);
                    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#endif
                }
                else {
#if 0 
                    pixels[1] = DWORD2Pixel (hdc, gradient_color 
                            //(cap_c, LFRDR_3DBOX_COLOR_DARKER, 30));
                            (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 10));
#else
                    pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 10);
                    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#endif
                }

#if 0 
                pixels[0] = DWORD2Pixel (hdc, gradient_color 
                        (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 250));
#else
                
                pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 250);
                pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#endif           
                path = mGPlusCreatePath(MGPLUS_PATH_WIND_EVEN_ODD);
                if (!path)
                    return;

                mGPlusNewSubPath (path);
                mGPlusPathAddCircle (path, (rect.left + rect.right) >> 1, 
                        (rect.top + rect.bottom) >> 1, RECTH (rect) >> 1);

                PathCxt context;
                context.hdc = hdc;
#if 0
                context.ptn = mGPlusCreateLinearPattern (rect, 
                        MGPLUS_GRADIENT_LINEAR_VERTICAL,
                        pixels, 2);
#else
                inPoint.x = ((rect.left + rect.right) >> 1) - 3;
                inPoint.y = rect.top + 3;


                outPoint.x = ((rect.left + rect.right) >> 1);// + 2;
                //outPoint.y = rect.bottom - 3;
                outPoint.y = ((rect.top + rect.bottom) >> 1);// + 2;

                context.ptn = mGPlusCreateRadialPattern (&inPoint, 
                        &outPoint, 0, RECTW (rect)/2,
                        pixels, 2, MGPLUS_GRADIENT_BIT_16);
#endif

                mGPlusPathGenerator(path, fill_path_cb, (void*)&context);
                mGPlusFreePath (path);
                alp_c = calc_alpha(cap_c, pixels[1]);

                SetPenColor(hdc, RGB2Pixel(hdc, GetRValue(alp_c), 
                                        GetGValue(alp_c), GetBValue(alp_c)));
                Circle(hdc, 
                    rect.left + (RECTW(rect)>>1), rect.top + (RECTW(rect)>>1),
                    (RECTW(rect)>>1));
             
                SetPenColor(hdc, PIXEL_black);
                
                rect.right --; rect.bottom --;
                
                w = RECTW(rect);
                h = RECTH(rect);

                rect.left   += (w>>2)+1;
                rect.top    += (h>>2)+1;
                rect.right  -= (w>>2)+1;
                rect.bottom -= (h>>2)+1;
                
                for(i = 0; i < (w>>4)+1; i++) // pen width w/16+1.
                {
                    Rectangle(hdc, rect.left + i, rect.top + i,
                            rect.right - i, rect.bottom - i);
                    MoveTo(hdc, rect.left + i, rect.top + i + (w>>4) + 1);
                    LineTo(hdc, rect.right - i, rect.top + i + (w>>4) + 1);
                }
            }
            if(ht_code == HT_MAXBUTTON) break;

        case HT_MINBUTTON:
            if(calc_we_area(hWnd, HT_MINBUTTON, &rect) != -1)
            {
                if(RECTH(rect) <= 0 || RECTW(rect) <= 0)
                    return;

                if(status & LFRDR_BTN_STATUS_PRESSED){
#if 0
                    pixels[1] = DWORD2Pixel (hdc, gradient_color 
                            (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 50));
#else
                    pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 50);
                    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#endif
                }
                else if(status & LFRDR_BTN_STATUS_HILITE){
#if 0
                    pixels[1] = DWORD2Pixel (hdc, gradient_color 
                            (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 80));
#else
                    pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 80);
                    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#endif
                }
                else {
#if 0
                    pixels[1] = DWORD2Pixel (hdc, gradient_color 
                            (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 10));
#else
                    pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 10);
                    pixels[1] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#endif
                }
#if 0
                pixels[0] = DWORD2Pixel (hdc, gradient_color 
                        (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 250));
#else
                pixel = gradient_color (cap_c, LFRDR_3DBOX_COLOR_LIGHTER, 250);
                pixels[0] = (pixel & 0xFF000000) | ((pixel & 0x000000FF) << 16) |
                                (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
#endif

                path = mGPlusCreatePath(MGPLUS_PATH_WIND_EVEN_ODD);
                if (!path)
                    return;

                mGPlusNewSubPath (path);
                mGPlusPathAddCircle (path, (rect.left + rect.right) >> 1, 
                        (rect.top + rect.bottom) >> 1, RECTH (rect) >> 1);

                PathCxt context;
                context.hdc = hdc;
#if 0
                context.ptn = mGPlusCreateLinearPattern (rect, 
                        MGPLUS_GRADIENT_LINEAR_VERTICAL,
                        pixels, 2);
#else
                inPoint.x = ((rect.left + rect.right) >> 1) - 3;
                inPoint.y = rect.top + 3;


                outPoint.x = ((rect.left + rect.right) >> 1);// + 2;
                //outPoint.y = rect.bottom - 3;
                outPoint.y = ((rect.top + rect.bottom) >> 1);// + 2;

                context.ptn = mGPlusCreateRadialPattern (&inPoint, 
                        &outPoint, 0, RECTW (rect)/2,
                        pixels, 2, MGPLUS_GRADIENT_BIT_16);
#endif
                mGPlusPathGenerator(path, fill_path_cb, (void*)&context);
                mGPlusFreePath (path);

                alp_c = calc_alpha(cap_c, pixels[1]);
                SetPenColor(hdc, RGB2Pixel(hdc, GetRValue(alp_c), 
                                        GetGValue(alp_c), GetBValue(alp_c)));
                Circle(hdc, 
                    rect.left + (RECTW(rect)>>1), rect.top + (RECTW(rect)>>1),
                    (RECTW(rect)>>1));
             
                SetPenColor(hdc, PIXEL_black);
                
                rect.right --; rect.bottom --;
                
                w = RECTW(rect);
                h = RECTH(rect);

                rect.left   += (w>>2)+1;
                rect.top    += (h>>2)+1;
                rect.right  -= (w>>2)+1;
                rect.bottom -= (h>>2)+1;
                
                for(i = 0; i < ((w>>4)+1); i++) // pen width (w/16+1)X2.
                {
                    MoveTo(hdc, rect.left + i, rect.bottom - i - (w>>3));
                    LineTo(hdc, rect.right - i, rect.bottom - i - (w>>3));
                    MoveTo(hdc, rect.left + i, rect.bottom + i - (w>>3));
                    LineTo(hdc, rect.right - i, rect.bottom + i - (w>>3));
                }

            }
            if(ht_code == HT_MINBUTTON) break;
            if(ht_code == 0) break;

        default:
            break;
    }
    SetPenColor(hdc, old_col);
}

static int calc_scrollbarctrl_area(HWND hWnd, int sb_pos, PRECT prc)
{
    PSCROLLBARDATA pScData = (PSCROLLBARDATA) GetWindowAdditionalData2 (hWnd);
    GetClientRect (hWnd, prc);
    
    switch (sb_pos)
    {
        case HT_HSCROLL:
        case HT_VSCROLL:
                break;
        case HT_SB_LEFTARROW:
            {
                prc->right = prc->left + pScData->arrowLen;
                break;
            }
        case HT_SB_UPARROW:
            {
                prc->bottom = prc->top + pScData->arrowLen;
                break;
            }
        case HT_SB_RIGHTARROW:
            {
                prc->left = prc->right - pScData->arrowLen;
                break;
            }
        case HT_SB_DOWNARROW:
            {
                prc->top = prc->bottom - pScData->arrowLen;
                break;
            }
        case HT_SB_HTHUMB:
            {
                prc->left = pScData->barStart + pScData->arrowLen;
                prc->right = prc->left + pScData->barLen;
                break;
            }
        case HT_SB_VTHUMB:
            {
                prc->top = pScData->barStart + pScData->arrowLen;
                prc->bottom = prc->top + pScData->barLen;
                break;
            }
        default:
            return -1;
    }
    if(0 >= prc->right - prc->left || 0 >= prc->bottom - prc->top)
        return -1;
  
    return 0;
}

static int get_scroll_status (HWND hWnd, BOOL isVert)
{
    int sb_status; 
    const WINDOWINFO  *info;
    
    if (0 == strncasecmp(CTRL_SCROLLBAR, GetClassName(hWnd), strlen(CTRL_SCROLLBAR)))
    {
        sb_status = ((PSCROLLBARDATA)GetWindowAdditionalData2(hWnd))->status;
    }
    else
    {
        info = (WINDOWINFO*)GetWindowInfo (hWnd);
        if(isVert)
            sb_status = info->vscroll.status; 
        else
            sb_status = info->hscroll.status; 
    }
   return sb_status; 
}

/* draw_scrollbar:
 *   This function draw the scrollbar of a window. 
 *
 * \param hWnd : the handle of the window.
 * \param hdc : the DC of the window.
 * \param sb_pos : the pos need to draw.
 * \param status : the status of the drawing part. 
 *
 * Author: wangjian<wangjian@minigui.org>
 * Date: 2007-11-22
 */
static void draw_scrollbar (HWND hWnd, HDC hdc, int sb_pos)
{
    int i;
    POINT point;
    BOOL isCtrl = FALSE;    /** if TRUE it is scrollbar control else not */
    int sb_status = 0;
    int bn_status = 0;
    mgplus_pattern_t ptn;
    RECT rect, rect1, rect2;
    DWORD fgc_3d, fgc_dis, bgc;
    gal_pixel pixel[2], pixel2[2], color, conver_pixel;
    gal_pixel old_brush_color, old_pen_color;
    const WINDOWINFO  *info = 
        (WINDOWINFO*)GetWindowInfo (hWnd);
        
    old_pen_color = GetPenColor(hdc);
    fgc_dis = GetWindowElementAttr(hWnd, WE_BGCB_ACTIVE_CAPTION);
    fgc_3d = gradient_color (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 80);
    bgc = GetWindowElementAttr(hWnd, WE_BGC_WINDOW);
    
    if (0 == strncasecmp(CTRL_SCROLLBAR, GetClassName(hWnd), 
                strlen(CTRL_SCROLLBAR)))
    {
        isCtrl = TRUE;
    }
    
    if (isCtrl)
    {
        if (0 != calc_scrollbarctrl_area(hWnd, sb_pos, &rect))
            return;
    }
    else
    { 
           /** draw the rect between H and V */
           if(calc_we_area(hWnd, HT_HSCROLL, &rect) != -1)
           {
               if(info->dwStyle & WS_VSCROLL)
               {
                   if (info->dwExStyle & WS_EX_LEFTSCROLLBAR) 
                   {
                       rect.right = rect.left;
                       rect.left = rect.right - RECTH(rect);
                   }
                   else 
                   {
                       rect.left = rect.right;
                       rect.right = rect.left + RECTH(rect);
                   }
                   old_brush_color = 
                       SetBrushColor(hdc, RGB2Pixel(hdc, 0xEE,0xED,0xE5));
                   FillBox(hdc, rect.left, rect.top, RECTW(rect), RECTH(rect));
                   SetBrushColor(hdc, old_brush_color);
               }
           }

           if (sb_pos != 0)
           {
               if (0 != calc_we_area(hWnd, sb_pos, &rect)) 
                   return;
           }
           else
           {
               draw_scrollbar (hWnd, hdc, HT_HSCROLL);
               draw_scrollbar (hWnd, hdc, HT_VSCROLL);
               return;
           }
    }

    switch(sb_pos)
    {
        case HT_HSCROLL:       // paint the hscrollbar
            { 
                if (!isCtrl || !(info->dwStyle & SBS_NOSHAFT))
                { 
                    SetPenColor(hdc, DWORD2Pixel (hdc, gradient_color (fgc_dis, 
                                    LFRDR_3DBOX_COLOR_DARKER, 10)));
                    Rectangle(hdc, rect.left, rect.top, rect.right-1, 
                            rect.bottom-1);
#if 0
                    pixel[0] = DWORD2Pixel (hdc, gradient_color (bgc, 
                                LFRDR_3DBOX_COLOR_LIGHTER, 20));
                    pixel[1] = DWORD2Pixel (hdc, gradient_color (bgc,
                                LFRDR_3DBOX_COLOR_LIGHTER, 220));
#endif
                    conver_pixel = gradient_color (bgc, LFRDR_3DBOX_COLOR_LIGHTER, 20);
                    pixel[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                               (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
                    conver_pixel = gradient_color (bgc, LFRDR_3DBOX_COLOR_LIGHTER, 220);
                    pixel[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                               (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
                    
                    rect.top++; rect.left++;
                    rect.right-=2; rect.bottom-=2;
                    
                    ptn = mGPlusCreateLinearPattern (rect,
                            MGPLUS_GRADIENT_LINEAR_VERTICAL, pixel, 2, MGPLUS_GRADIENT_BIT_16);

                    for (i = rect.top; i<= rect.bottom; i ++)
                    {
                        point.x = rect.left;
                        point.y = i;
                        mGPlusGetGradientColorValue (ptn, &point, &color);
                        SetPenColor (hdc, color);
                        MoveTo (hdc, rect.left, i);
                        LineTo (hdc, rect.right, i);
                    }
                    mGPlusFreeColorPattern (ptn);
                } 
                draw_scrollbar(hWnd, hdc, HT_SB_LEFTARROW);
                draw_scrollbar(hWnd, hdc, HT_SB_RIGHTARROW);
                draw_scrollbar(hWnd, hdc, HT_SB_HTHUMB);
                break;
            }
        case HT_VSCROLL:         // paint the vscrollbar
            {
                if (!isCtrl || !(info->dwStyle & SBS_NOSHAFT))
                { 
                    SetPenColor(hdc, DWORD2Pixel (hdc, gradient_color (fgc_dis, 
                                    LFRDR_3DBOX_COLOR_DARKER, 10)));
                    Rectangle(hdc, rect.left, rect.top, rect.right-1, rect.bottom-1);
                    conver_pixel = gradient_color (bgc, LFRDR_3DBOX_COLOR_LIGHTER, 20);
                    pixel[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                               (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
                    conver_pixel = gradient_color (bgc, LFRDR_3DBOX_COLOR_LIGHTER, 200);
                    pixel[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                               (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
#if 0
                    pixel[1] = DWORD2Pixel (hdc, gradient_color (bgc, 
                                LFRDR_3DBOX_COLOR_LIGHTER, 20));
                    pixel[0] = DWORD2Pixel (hdc, gradient_color (bgc,
                                LFRDR_3DBOX_COLOR_LIGHTER, 200));
#endif
                    rect.top++; rect.left++;
                    rect.right-=2; rect.bottom-=2;
                    
                    ptn = mGPlusCreateLinearPattern (rect, 
                            MGPLUS_GRADIENT_LINEAR_HORIZONTAL, pixel, 2, MGPLUS_GRADIENT_BIT_16);

                    for (i = rect.left; i<= rect.right; i ++)
                    {
                        point.y = rect.top;
                        point.x = i;
                        mGPlusGetGradientColorValue (ptn, &point, &color);
                        SetPenColor (hdc, color);
                        MoveTo (hdc, i, rect.top);
                        LineTo (hdc, i, rect.bottom);
                    }
                    mGPlusFreeColorPattern (ptn);
                } 
                draw_scrollbar(hWnd, hdc, HT_SB_UPARROW);
                draw_scrollbar(hWnd, hdc, HT_SB_DOWNARROW);
                draw_scrollbar(hWnd, hdc, HT_SB_VTHUMB);
                break;
            }
        case HT_SB_LEFTARROW:
            {
                sb_status = get_scroll_status(hWnd, FALSE);
                if (sb_status & SBS_DISABLED_LTUP)
                    bn_status |= LFRDR_BTN_STATUS_DISABLED;
                else
                {
                    if(sb_status & SBS_PRESSED_LTUP)
                        bn_status |= LFRDR_BTN_STATUS_PRESSED;
                    else if(sb_status & SBS_HILITE_LTUP)
                        bn_status |= LFRDR_BTN_STATUS_HILITE;
                }
                
                if(sb_status & SBS_DISABLED_LTUP || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, 
                            bn_status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_LEFT);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, 
                            bn_status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_LEFT);
                break;
            }
        case HT_SB_RIGHTARROW:
            {
                sb_status = get_scroll_status(hWnd, FALSE);
                if (sb_status & SBS_DISABLED_BTDN) 
                    bn_status |= LFRDR_BTN_STATUS_DISABLED;
                else
                {
                    if(sb_status & SBS_PRESSED_BTDN)
                        bn_status |= LFRDR_BTN_STATUS_PRESSED;
                    else if(sb_status & SBS_HILITE_BTDN)
                        bn_status |= LFRDR_BTN_STATUS_HILITE;
                }
                
                if(sb_status & SBS_DISABLED_BTDN || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, 
                            bn_status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_RIGHT);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, 
                            bn_status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_RIGHT);
                break;
            }
        case HT_SB_HTHUMB:
            { 
                //draw left and right sides
                SetPenColor (hdc, DWORD2Pixel (hdc, gradient_color (fgc_dis,
                                LFRDR_3DBOX_COLOR_DARKER, 10)));
                MoveTo (hdc, rect.left, rect.top+1);
                LineTo (hdc, rect.left, rect.bottom-2);
                MoveTo (hdc, rect.right, rect.top+1);
                LineTo (hdc, rect.right, rect.bottom-2);
                    
                conver_pixel = gradient_color (fgc_3d, LFRDR_3DBOX_COLOR_LIGHTER, 250);
                pixel[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                           (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
                conver_pixel = gradient_color (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 70);
                pixel[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                           (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
                
                conver_pixel = gradient_color (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 70);
                pixel2[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                            (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
                conver_pixel = gradient_color (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 100);
                pixel2[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                            (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
#if 0 
                pixel[0] = DWORD2Pixel (hdc, gradient_color 
                        (fgc_3d, LFRDR_3DBOX_COLOR_LIGHTER, 250)); 
                pixel[1] = DWORD2Pixel (hdc, gradient_color 
                        (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 70)); 

                pixel2[0] = DWORD2Pixel (hdc, gradient_color 
                        (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 70)); 
                pixel2[1] = DWORD2Pixel (hdc, gradient_color 
                        (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 100));
#endif
                rect1.top = rect.top + 1;
                rect1.left = rect.left + 1;
                rect1.right = rect.right - 1;
                rect1.bottom = rect.top + RECTH(rect)/3;

                rect2.top = rect1.bottom+1;
                rect2.left = rect1.left;
                rect2.right = rect1.right;
                rect2.bottom = rect.bottom-2;

                ptn = mGPlusCreateLinearPattern (rect1, 
                        MGPLUS_GRADIENT_LINEAR_VERTICAL, pixel, 2, MGPLUS_GRADIENT_BIT_16);

                for (i = rect1.top; i <= rect1.bottom; i ++)
                {
                    point.x = rect1.left;
                    point.y = i;
                    mGPlusGetGradientColorValue (ptn, &point, &color);
                    SetPenColor (hdc, color);
                    MoveTo (hdc, rect1.left, i);
                    LineTo (hdc, rect1.right, i);
                }
                mGPlusFreeColorPattern (ptn);
                ptn = mGPlusCreateLinearPattern (rect2, 
                        MGPLUS_GRADIENT_LINEAR_VERTICAL, pixel2, 2, MGPLUS_GRADIENT_BIT_16);

                for (i = rect2.top; i <= rect2.bottom; i ++)
                {
                    point.x = rect2.left;
                    point.y = i;
                    mGPlusGetGradientColorValue (ptn, &point, &color);
                    SetPenColor (hdc, color);
                    MoveTo (hdc, rect2.left, i);
                    LineTo (hdc, rect2.right, i);
                }
                mGPlusFreeColorPattern (ptn);
                break;
            }
        case HT_SB_UPARROW:
            {
                sb_status = get_scroll_status(hWnd, TRUE);
                if (sb_status & SBS_DISABLED_LTUP)
                    bn_status |= LFRDR_BTN_STATUS_DISABLED;
                else
                {
                    if(sb_status & SBS_PRESSED_LTUP)
                        bn_status |= LFRDR_BTN_STATUS_PRESSED;
                    else if(sb_status & SBS_HILITE_LTUP)
                        bn_status |= LFRDR_BTN_STATUS_HILITE;
                }

                if(sb_status & SBS_DISABLED_LTUP || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, 
                            bn_status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_UP);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, 
                            bn_status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_UP);
                break;
            }
        case HT_SB_DOWNARROW:
            {
                sb_status = get_scroll_status(hWnd, TRUE);
                if (sb_status & SBS_DISABLED_BTDN)
                    bn_status |= LFRDR_BTN_STATUS_DISABLED;
                else
                {
                    if(sb_status & SBS_PRESSED_BTDN)
                        bn_status |= LFRDR_BTN_STATUS_PRESSED;
                    else if(sb_status & SBS_HILITE_BTDN)
                        bn_status |= LFRDR_BTN_STATUS_HILITE;
                }
                
                if(sb_status & SBS_DISABLED_BTDN || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, 
                            bn_status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_DOWN);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, 
                            bn_status | LFRDR_ARROW_HAVESHELL | LFRDR_ARROW_DOWN);
                break;
            }
        case HT_SB_VTHUMB: 
            {
                //draw top and bottom sides
                SetPenColor(hdc, DWORD2Pixel (hdc, gradient_color 
                            (fgc_3d, LFRDR_3DBOX_COLOR_DARKER, 10)));
                MoveTo (hdc, rect.left+1, rect.top);
                LineTo (hdc, rect.right-2, rect.top);
                MoveTo (hdc, rect.left+1, rect.bottom);
                LineTo (hdc, rect.right-2, rect.bottom);

                conver_pixel = gradient_color (fgc_3d, LFRDR_3DBOX_COLOR_LIGHTER, 250);
                pixel[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                           (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
                conver_pixel = gradient_color (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 70);
                pixel[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                           (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
                
                conver_pixel = gradient_color (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 70);
                pixel2[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                            (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
                conver_pixel = gradient_color (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 100);
                pixel2[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                            (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
#if 0
                pixel[0] = DWORD2Pixel (hdc, gradient_color 
                        (fgc_3d, LFRDR_3DBOX_COLOR_LIGHTER, 250)); 
                pixel[1] = DWORD2Pixel (hdc, gradient_color 
                        (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 70)); 
                        //(fgc_dis, LFRDR_3DBOX_COLOR_DARKER, 20)); 

                pixel2[0] = DWORD2Pixel (hdc, gradient_color 
                        (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 70)); 
                        //(fgc_dis, LFRDR_3DBOX_COLOR_DARKER, 20));
                pixel2[1] = DWORD2Pixel (hdc, gradient_color 
                        (fgc_dis, LFRDR_3DBOX_COLOR_LIGHTER, 100));
#endif
                rect1.top = rect.top + 1;
                rect1.left = rect.left + 1;
                rect1.right = rect1.left + RECTW (rect)/3;
                rect1.bottom = rect.bottom - 1;

                rect2.top = rect1.top;
                rect2.left = rect1.right;
                rect2.right = rect.right - 2;
                rect2.bottom = rect1.bottom;

                ptn = mGPlusCreateLinearPattern (rect1, 
                        MGPLUS_GRADIENT_LINEAR_HORIZONTAL, pixel, 2, MGPLUS_GRADIENT_BIT_16);

                for (i = rect1.left; i <= rect1.right; i ++)
                {
                    point.y = rect1.top;
                    point.x = i;
                    mGPlusGetGradientColorValue (ptn, &point, &color);
                    SetPenColor (hdc, color);
                    MoveTo (hdc, i, rect1.top);
                    LineTo (hdc, i, rect1.bottom);
                }
                mGPlusFreeColorPattern (ptn);
                ptn = mGPlusCreateLinearPattern (rect2, 
                        MGPLUS_GRADIENT_LINEAR_HORIZONTAL, pixel2, 2, MGPLUS_GRADIENT_BIT_16);

                for (i = rect2.left; i <= rect2.right; i ++)
                {
                    point.y = rect2.top;
                    point.x = i;
                    mGPlusGetGradientColorValue (ptn, &point, &color);
                    SetPenColor (hdc, color);
                    MoveTo (hdc, i, rect2.top);
                    LineTo (hdc, i, rect2.bottom);
                }
                mGPlusFreeColorPattern (ptn);
                break;
            } 
        default:
            break;
    }
    SetPenColor(hdc, old_pen_color);
}

static void draw_trackbar_thumb (HWND hWnd, HDC hdc, 
        const RECT* pRect, DWORD dwStyle)
{
    RECT    rc_draw;
    BOOL    vertical;

    gal_pixel pixels[3], conver_pixel;
    mgplus_pattern_t ptn;
    int i;
    POINT point;
    gal_pixel pixel_dst;
    gal_pixel pixel_org;
    DWORD bgc;

    int corner, l, t, r, b;

    /** leave little margin */
    if (dwStyle & TBS_VERTICAL) {
        rc_draw.left   = pRect->left;
        rc_draw.top    = pRect->top + 2;
        rc_draw.right  = pRect->right - 1;
        rc_draw.bottom = pRect->bottom - 2;
        vertical = TRUE;
    }
    else{
        rc_draw.left   = pRect->left + 2;
        rc_draw.top    = pRect->top;
        rc_draw.right  = pRect->right - 2;
        rc_draw.bottom = pRect->bottom - 1;
        vertical = FALSE;
    }

    bgc = GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY);
    //pixels[0] = DWORD2Pixel (hdc, bgc);
    conver_pixel = bgc;
    pixels[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    
    bgc = GetWindowElementAttr(hWnd, WE_BGCB_ACTIVE_CAPTION);
    conver_pixel = gradient_color(bgc, LFRDR_3DBOX_COLOR_LIGHTER, 20);
    pixels[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    pixels[2] = pixels[0];
#if 0 
    pixels[1] = DWORD2Pixel (hdc, gradient_color 
            (bgc, LFRDR_3DBOX_COLOR_LIGHTER, 20));
    pixels[2] = pixels[0];
#endif

    if (!vertical) {
        ptn = mGPlusCreateLinearPattern (rc_draw, 
                MGPLUS_GRADIENT_LINEAR_VERTICAL,
                pixels, 3, MGPLUS_GRADIENT_BIT_16);

        pixel_org = GetPenColor (hdc);
        point.x = rc_draw.left;
        for (i = rc_draw.top; i < rc_draw.bottom; ++i) {
            point.y = i;
            if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
                SetPenColor (hdc, pixel_dst);
                MoveTo (hdc, point.x, point.y);
                LineTo (hdc, rc_draw.right - 1, point.y);
            }
            else {
                printf ("mGPlusGetGradientColorValue return false.\n");
            }
        }
    }else {
        ptn = mGPlusCreateLinearPattern (rc_draw, 
                MGPLUS_GRADIENT_LINEAR_HORIZONTAL,
                pixels, 3, MGPLUS_GRADIENT_BIT_16);

        pixel_org = GetPenColor (hdc);
        point.y = rc_draw.top;
        for (i = rc_draw.left; i < rc_draw.right; ++i) {
            point.x = i;
            if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
                SetPenColor (hdc, pixel_dst);
                MoveTo (hdc, point.x, point.y);
                LineTo (hdc, point.x, rc_draw.bottom);
            }
            else {
                printf ("mGPlusGetGradientColorValue return false.\n");
            }
        }
    }

    SetPenColor (hdc, pixel_org);
    mGPlusFreeColorPattern (ptn);

     /*draw border*/
    corner = 1;
    l = rc_draw.left;
    t = rc_draw.top;
    r = rc_draw.right;
    b = rc_draw.bottom;

    SetPenColor(hdc, pixels[1]); 
    MoveTo(hdc, l + corner, t);
    LineTo(hdc, r - 1 - corner, t);
    LineTo(hdc, r - 1, t+corner);
    LineTo(hdc, r - 1, b - 1-corner);
    LineTo(hdc, r - 1-corner, b - 1);
    LineTo(hdc, l+corner, b - 1);
    LineTo(hdc, l, b - 1-corner);
    LineTo(hdc, l, t+corner);
    LineTo(hdc, l+corner, t);
}

static void 
calc_trackbar_rect (HWND hWnd, LFRDR_TRACKBARINFO *info, DWORD dwStyle,
        const RECT* rcClient, RECT* rcRuler, RECT* rcBar, RECT* rcBorder)
{
    int x, y, w, h;
    int pos, min, max;
    int sliderx, slidery, sliderw, sliderh;

    x = rcClient->left;
    y = rcClient->top;
    w = RECTWP (rcClient);
    h = RECTHP (rcClient);

    pos = info->nPos;
    max = info->nMax;
    min = info->nMin;

    /* Calculate border rect. */
    if (dwStyle & TBS_BORDER) {
        x += TB_BORDER;
        y += TB_BORDER;
        w -= TB_BORDER << 1;
        h -= TB_BORDER << 1;
    }

    if (rcBorder) {
        SetRect (rcBorder, x, y, x+w, y+h);
    }

    /* Calculate ruler rect. */
    if (rcRuler) {
        if (dwStyle & TBS_VERTICAL) {
            rcRuler->left = x + ((w - WIDTH_VERT_RULER)>>1);
            rcRuler->top = y + (HEIGHT_VERT_SLIDER >> 1);
            rcRuler->right = x + ((w + WIDTH_VERT_RULER)>>1);
            rcRuler->bottom = y + h - (HEIGHT_VERT_SLIDER >> 1);
        }
        else {
            rcRuler->left = x + (WIDTH_HORZ_SLIDER >> 1);
            rcRuler->top = y + ((h - HEIGHT_HORZ_RULER)>>1);
            rcRuler->right = x + w - (WIDTH_HORZ_SLIDER >> 1);
            rcRuler->bottom = y + ((h + HEIGHT_HORZ_RULER)>>1);
        }
    }

    if (rcBar) {
        /* Calculate slider rect. */
        if (dwStyle & TBS_VERTICAL) {
            sliderw = WIDTH_VERT_SLIDER;
            sliderh = HEIGHT_VERT_SLIDER;
        }
        else {
            sliderw = WIDTH_HORZ_SLIDER;
            sliderh = HEIGHT_HORZ_SLIDER;
        }

        if (dwStyle & TBS_VERTICAL) {
            sliderx = x + ((w - sliderw) >> 1); 
            slidery = y + (HEIGHT_VERT_SLIDER>>1)+ (int)((max - pos) * 
                    (h - HEIGHT_VERT_SLIDER) / (float)(max - min)) - (sliderh>>1);
        }
        else {
            slidery = y + ((h - sliderh) >> 1); 
            sliderx = x + (WIDTH_HORZ_SLIDER >> 1) + (int)((pos - min) * 
                    (w - WIDTH_HORZ_SLIDER) / (float)(max - min)) - (sliderw>>1);
        }

        SetRect (rcBar, sliderx, slidery, sliderx + sliderw, slidery + sliderh);
    }
}


static void 
draw_trackbar (HWND hWnd, HDC hdc, LFRDR_TRACKBARINFO *info)
{
    RECT    rc_client, rc_border, rc_ruler, rc_bar, rc_draw, rect;
    int     x, y, w, h;
    int     max, min;
    int     TickFreq;
    int     TickStart, TickEnd;
    int     sliderw, sliderh;
    float   TickGap, Tick;
    DWORD   bgc, light_dword, dwStyle;

    gal_pixel pixels[2];
    gal_pixel conver_pixel;
    mgplus_pattern_t ptn;
    int i;
    POINT point;

    /** same as backgroud color */
    bgc = GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY);

    GetClientRect (hWnd, &rc_client);
    dwStyle = GetWindowStyle (hWnd);

    calc_trackbar_rect (hWnd, info, dwStyle, &rc_client, 
            &rc_ruler, &rc_bar, &rc_border);

    x = rc_border.left;
    y = rc_border.top;
    w = RECTW (rc_border);
    h = RECTH (rc_border);

    /* get data of trackbar. */
    TickFreq = info->nTickFreq;

    /* draw the border according to trackbar style with renderer. */
    rc_draw.left   = rc_client.left;
    rc_draw.top    = rc_client.top;
    rc_draw.right  = rc_client.right - 1;
    rc_draw.bottom = rc_client.bottom  - 1;
    draw_3dbox (hdc, &rc_draw, bgc, LFRDR_BTN_STATUS_PRESSED);

    rect = rc_draw;
    ++rect.left;
    ++rect.top;
    --rect.right;
    --rect.bottom;
    gal_pixel pixel_dst;
    gal_pixel pixel_org;
#if 0
    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (bgc, LFRDR_3DBOX_COLOR_DARKER, 60));
    pixels[1] = DWORD2Pixel (hdc, gradient_color 
            (bgc, LFRDR_3DBOX_COLOR_LIGHTER, 90));
#endif

    conver_pixel = gradient_color (bgc, LFRDR_3DBOX_COLOR_DARKER, 60);
    pixels[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    conver_pixel = gradient_color (bgc, LFRDR_3DBOX_COLOR_LIGHTER, 90);
    pixels[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    
    if (!(dwStyle & TBS_VERTICAL)) {
        ptn = mGPlusCreateLinearPattern (rect, 
                MGPLUS_GRADIENT_LINEAR_VERTICAL,
                pixels, 2, MGPLUS_GRADIENT_BIT_16);

        pixel_org = GetPenColor (hdc);
        point.x = rect.left;
        for (i = rect.top; i < rect.bottom; ++i) {
            point.y = i;
            if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
                SetPenColor (hdc, pixel_dst);
                MoveTo (hdc, point.x, point.y);
                LineTo (hdc, rect.right - 1, point.y);
            }
            else {
                printf ("mGPlusGetGradientColorValue return false.\n");
            }
        }
    }else {
        ptn = mGPlusCreateLinearPattern (rect, 
                MGPLUS_GRADIENT_LINEAR_HORIZONTAL,
                pixels, 2, MGPLUS_GRADIENT_BIT_16);

        pixel_org = GetPenColor (hdc);
        point.y = rect.top;
        for (i = rect.left; i < rect.right; ++i) {
            point.x = i;
            if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
                SetPenColor (hdc, pixel_dst);
                MoveTo (hdc, point.x, point.y);
                LineTo (hdc, point.x, rect.bottom);
            }
            else {
                printf ("mGPlusGetGradientColorValue return false.\n");
            }
        }
    }

    SetPenColor (hdc, pixel_org);
    mGPlusFreeColorPattern (ptn);

    /* draw the rulder in middle of trackbar with renderer . */
    rc_draw.left   = rc_ruler.left;
    rc_draw.top    = rc_ruler.top;
    rc_draw.right  = rc_ruler.right - 1;
    rc_draw.bottom = rc_ruler.bottom - 1;
    draw_3dbox (hdc, &rc_draw, bgc, LFRDR_BTN_STATUS_PRESSED);

    rect = rc_draw;
    ++rect.left;
    ++rect.top;
    --rect.right;
    --rect.bottom;
#if 0
    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (bgc, LFRDR_3DBOX_COLOR_DARKER, 40));
    pixels[1] = DWORD2Pixel (hdc, gradient_color 
            (bgc, LFRDR_3DBOX_COLOR_LIGHTER, 100));
#endif

    conver_pixel = gradient_color (bgc, LFRDR_3DBOX_COLOR_DARKER, 40);
    pixels[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    conver_pixel = gradient_color (bgc, LFRDR_3DBOX_COLOR_LIGHTER, 100);
    pixels[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    
    if (!(dwStyle & TBS_VERTICAL)) {
        ptn = mGPlusCreateLinearPattern (rect, 
                MGPLUS_GRADIENT_LINEAR_VERTICAL,
                pixels, 2, MGPLUS_GRADIENT_BIT_16);

        pixel_org = GetPenColor (hdc);
        point.x = rect.left;
        for (i = rect.top; i < rect.bottom; ++i) {
            point.y = i;
            if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
                SetPenColor (hdc, pixel_dst);
                MoveTo (hdc, point.x, point.y);
                LineTo (hdc, rect.right - 1, point.y);
            }
            else {
                printf ("mGPlusGetGradientColorValue return false.\n");
            }
        }
    }else {
        ptn = mGPlusCreateLinearPattern (rect, 
                MGPLUS_GRADIENT_LINEAR_HORIZONTAL,
                pixels, 2, MGPLUS_GRADIENT_BIT_16);

        pixel_org = GetPenColor (hdc);
        point.y = rect.top;
        for (i = rect.left; i < rect.right; ++i) {
            point.x = i;
            if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
                SetPenColor (hdc, pixel_dst);
                MoveTo (hdc, point.x, point.y);
                LineTo (hdc, point.x, rect.bottom);
            }
            else {
                printf ("mGPlusGetGradientColorValue return false.\n");
            }
        }
    }

    SetPenColor (hdc, pixel_org);
    mGPlusFreeColorPattern (ptn);

    max = info->nMax;
    min = info->nMin;
    sliderw = RECTW(rc_bar);
    sliderh = RECTH(rc_bar);

    /* draw the tick of trackbar. */
    if (!(dwStyle & TBS_NOTICK)) {
        SetPenColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_THREED_BODY));
        if (dwStyle & TBS_VERTICAL) {
            TickStart = y + (HEIGHT_VERT_SLIDER >> 1); 
            //TickGap = (h - HEIGHT_VERT_SLIDER) / (float)(max - min) * TickFreq;
            TickGap = itofix (h - HEIGHT_VERT_SLIDER);
            TickGap = fixmul (TickGap, fixdiv (itofix (TickFreq), itofix (max - min)));
            TickEnd = y + h - (HEIGHT_VERT_SLIDER >> 1);
#if 0
            for (Tick = TickStart; (int)Tick <= TickEnd; Tick += TickGap) { 
                MoveTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER, (int) Tick);
                LineTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER + LEN_TICK, (int) Tick);
            }
            if ((int)(Tick - TickGap + 0.9) < TickEnd) {
                MoveTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER, TickEnd);
                LineTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER + LEN_TICK, TickEnd);
            }
#else
            for (Tick = itofix (TickStart); (int)(fixtof(Tick)) <= TickEnd; Tick = fixadd (Tick, TickGap) ) {
                MoveTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER, (int)(fixtof (Tick)));
                LineTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER + LEN_TICK, (int)(fixtof (Tick)));
            }
            if ((int) (fixtof (fixadd (fixsub (Tick, TickGap), ftofix (0.9)))) < TickEnd) {
                MoveTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER, TickEnd);
                LineTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER + LEN_TICK, TickEnd);
            }
#endif
        } else {
            TickStart = x + (WIDTH_HORZ_SLIDER >> 1); 
            //TickGap = (w - WIDTH_HORZ_SLIDER) / (float)(max - min) * TickFreq;
            TickGap = fixmul (itofix (w - WIDTH_HORZ_SLIDER), fixdiv (itofix (TickFreq), itofix (max - min)));
            TickEnd = x + w - (WIDTH_HORZ_SLIDER >> 1);
#if 0
            for (Tick = TickStart; (int)Tick <= TickEnd; Tick += TickGap) { 
                MoveTo (hdc, (int)Tick, y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER);
                LineTo (hdc, (int)Tick, y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER + LEN_TICK);
            }
            if ((int)(Tick - TickGap + 0.9) < TickEnd) {
                MoveTo (hdc, TickEnd, y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER);
                LineTo (hdc, TickEnd, y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER + LEN_TICK);
            }
#else
            for (Tick = itofix (TickStart); (int)(fixtof(Tick)) <= TickEnd; Tick = fixadd (Tick, TickGap) ) {
                MoveTo (hdc, (int)(fixtof (Tick)), y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER);
                LineTo (hdc, (int)(fixtof (Tick)), y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER + LEN_TICK);
            }

            if ((int) (fixtof (fixadd (fixsub (Tick, TickGap), ftofix (0.9)))) < TickEnd)
            {
                MoveTo (hdc, TickEnd, y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER);
                LineTo (hdc, TickEnd, y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER + LEN_TICK);
            }
#endif
        }
    }
    draw_trackbar_thumb (hWnd, hdc, &rc_bar, dwStyle);

    /* draw the focus frame with renderer. */
    if (dwStyle & TBS_FOCUS) {
        light_dword = calc_3dbox_color (bgc, LFRDR_3DBOX_COLOR_LIGHTEST);
        rc_draw.left   = x + 1;
        rc_draw.top    = y + 1;
        rc_draw.right  = x + w - 3; 
        rc_draw.bottom = y + h - 3;
        draw_focus_frame (hdc, &rc_draw, light_dword);
    }
}

static void 
disabled_text_out (HWND hWnd, HDC hdc, 
        const char* spText, PRECT rc, DWORD dt_fmt)
{
    DWORD mainc, color;
    
    SetBkMode (hdc, BM_TRANSPARENT);
    SetBkColor (hdc, GetWindowBkColor (hWnd));

    mainc = GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY);
    rc->left+=1;
    rc->top+=1;
    rc->right+=1;
    rc->bottom+=1;
    color = calc_3dbox_color (mainc, LFRDR_3DBOX_COLOR_LIGHTER);
    SetTextColor (hdc,
            RGBA2Pixel (hdc, GetRValue (color), GetGValue (color), 
                GetBValue (color), GetAValue (color)));
    DrawText (hdc, spText, -1, rc, dt_fmt);

    rc->left-=1;
    rc->top-=1;
    rc->right-=1;
    rc->bottom-=1;
    color = calc_3dbox_color (mainc, LFRDR_3DBOX_COLOR_DARKER);
    SetTextColor (hdc,
            RGBA2Pixel (hdc, GetRValue (color), GetGValue (color), 
                GetBValue (color), GetAValue (color)));
    DrawText (hdc, spText, -1, rc, dt_fmt);
}

/*
 * draw_tab:
 *  This function draw a tab for the propsheet.
 * Author : wangjian<wangjian@minigui.org>
 * Date : 2007-12-18.
 */
static void 
draw_tab (HWND hWnd, HDC hdc, RECT *rect, char *title, DWORD color, int flag, HICON icon)
{
    int x, ty, by, text_extent;
    SIZE size;
    int eff_chars, eff_len;
    DWORD light_c, darker_c, darkest_c;
    DWORD c1;
#if 0   
    gal_pixel pixels[2];
    mgplus_pattern_t ptn;
    int i;
    POINT point;
    gal_pixel pixel_dst;
    gal_pixel pixel_org;
    pixels[1] = DWORD2Pixel (hdc, color);
    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (color, LFRDR_3DBOX_COLOR_LIGHTER, 200));
    ptn = mGPlusCreateLinearPattern (*rect, 
            MGPLUS_GRADIENT_LINEAR_VERTICAL,
            pixels, 2);

    pixel_org = GetPenColor (hdc);
    point.x = rect->left + 1;
    for (i = rect->top + 3; i < rect->bottom; ++i) {
        point.y = i;
        if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
            SetPenColor (hdc, pixel_dst);
            MoveTo (hdc, point.x, point.y);
            LineTo (hdc, rect->right - 1, point.y);
        }
        else {
            printf ("mGPlusGetGradientColorValue return false.\n");
        }
    }

    SetPenColor (hdc, pixel_org);
    mGPlusFreeColorPattern (ptn);
#endif
    light_c = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_LIGHTEST);
    darker_c = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_DARKER);
    darkest_c = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_DARKEST);

    c1 = GetWindowElementAttr(hWnd, WE_BGCB_ACTIVE_CAPTION);

    x = rect->left + 2;
    ty = rect->top;

    if (!(flag & LFRDR_TAB_ACTIVE)) {
        if (flag & LFRDR_TAB_BOTTOM) {
            ty -= 2;
            by = rect->bottom - 2;
        } else {
            ty += 2;
            by = rect->bottom;
        }

     } else {
        by = rect->bottom ;
        FillBox (hdc, rect->left+1, ty, rect->right - rect->left, by);
     }

    /* draw the title's edge */
    /* pc3d style & flat & grap */
    if (flag & LFRDR_TAB_BOTTOM) {
        /* left and bottom */
        SetPenColor(hdc, DWORD2Pixel (hdc, light_c)); 
        MoveTo (hdc, rect->left, ty);
        LineTo (hdc, rect->left, by - 3 );
        if (flag & LFRDR_TAB_ACTIVE) {
            SetPenColor(hdc, DWORD2Pixel (hdc, c1)); 
            MoveTo (hdc, rect->left + 2, by - 1);
            LineTo (hdc, rect->right - 4, by - 1);
            MoveTo (hdc, rect->left + 1, by - 2);
            LineTo (hdc, rect->right - 3, by - 2);
            MoveTo (hdc, rect->left , by - 3);
            LineTo (hdc, rect->right - 2, by - 3);
        } else {
            SetPenColor(hdc, DWORD2Pixel (hdc, darker_c)); 
            MoveTo (hdc, rect->left + 2, by - 1);
            LineTo (hdc, rect->right - 4, by - 1);
            MoveTo (hdc, rect->left + 1, by - 2);
            LineTo (hdc, rect->left + 1, by - 2);
        }
        /*right*/
        SetPenColor(hdc, DWORD2Pixel (hdc, darkest_c)); 
        MoveTo (hdc, rect->right - 2, by - 3);
        LineTo (hdc, rect->right - 2, ty);
        MoveTo (hdc, rect->right - 3, by - 2 );
        LineTo (hdc, rect->right - 3, by - 2);
        SetPenColor(hdc, DWORD2Pixel (hdc, darker_c)); 
        MoveTo (hdc, rect->right - 3, by -3);
        LineTo (hdc, rect->right - 3, ty);
    } else {
        /*left and top */
        SetPenColor(hdc, DWORD2Pixel (hdc, light_c)); 
        MoveTo (hdc, rect->left, by - 1);
        LineTo (hdc, rect->left, ty + 2);
        if (flag & LFRDR_TAB_ACTIVE) {
            SetPenColor(hdc, DWORD2Pixel (hdc, c1)); 
            MoveTo (hdc, rect->left , ty+2);
            LineTo (hdc, rect->right - 2, ty+2);
            MoveTo (hdc, rect->left + 1, ty+1);
            LineTo (hdc, rect->right - 3, ty+1);
            MoveTo (hdc, rect->left + 2, ty);
            LineTo (hdc, rect->right - 4, ty);
        } else {
            MoveTo (hdc, rect->left + 2, ty);
            LineTo (hdc, rect->right - 4, ty);
            MoveTo (hdc, rect->left + 1, ty + 1);
            LineTo (hdc, rect->left + 1, ty + 1);
        }
        /*right*/
        SetPenColor(hdc, DWORD2Pixel (hdc, darkest_c)); 
        MoveTo (hdc, rect->right - 2, ty + 2 );
        LineTo (hdc, rect->right - 2, by - 2);
        MoveTo (hdc, rect->right - 3, ty + 1 );
        LineTo (hdc, rect->right - 3, ty + 1);
        SetPenColor(hdc, DWORD2Pixel (hdc, darker_c)); 
        MoveTo (hdc, rect->right - 3, ty + 2 );
        LineTo (hdc, rect->right - 3, by - 2);
    }

    /* draw the ICON */
    ty += 2 + 2;
    text_extent = RECTWP (rect) - 2 * 2;
    if (icon) {
        int icon_x, icon_y;
        icon_x = RECTHP(rect) - 8;
        icon_y = icon_x;
        
        DrawIcon (hdc, x, ty, icon_x, icon_y, icon);
        x += icon_x;
        x += 2;
        text_extent -= icon_x + 2;
    }
        
    /* draw the TEXT */
    SetBkColor (hdc, DWORD2Pixel (hdc, color)); 
    text_extent -= 4;
    eff_len = GetTextExtentPoint (hdc, title, strlen(title), 
                  text_extent, &eff_chars, NULL, NULL, &size);

    TextOutLen (hdc, x + 2, ty, title, eff_len);
}

static void 
draw_progress (HWND hWnd, HDC hdc, 
        int nMax, int nMin, int nPos, BOOL fVertical)
{
    POINT   point;
    RECT    rcClient;
    int     x, y, w, h;
    ldiv_t   ndiv_progress;
    unsigned int     nAllPart;
    unsigned int     nNowPart;
    int     i, step, pbar_border = 2;
    gal_pixel color, pixel[2], bkpixel[2], conver_pixel;
    DWORD c1, c2;
    mgplus_pattern_t ptn;
    int prog;
    char szText[8];
    SIZE text_ext;

    if (nMax == nMin)
        return;
    
    if ((nMax - nMin) > 5)
        step = 5;
    else
        step = 1;

    GetClientRect (hWnd, &rcClient);
    //draw progressbar bkcolor
    c1 = GetWindowElementAttr (hWnd, WE_BGC_WINDOW);
    c2 = GetWindowElementAttr (hWnd, WE_BGCB_ACTIVE_CAPTION);

#if 0
    bkpixel[0] = DWORD2Pixel (hdc, c1);
    bkpixel[1] = DWORD2Pixel (hdc, gradient_color (bkpixel[0], 
                LFRDR_3DBOX_COLOR_LIGHTER, 120));
#endif

    conver_pixel = c1;
    bkpixel[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                 (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    
    conver_pixel = gradient_color (bkpixel[0], LFRDR_3DBOX_COLOR_LIGHTER, 120);
    bkpixel[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                 (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    
    ptn = mGPlusCreateLinearPattern (rcClient, MGPLUS_GRADIENT_LINEAR_VERTICAL, 
            bkpixel, 2, MGPLUS_GRADIENT_BIT_16);

    point.x = rcClient.left;
    for (i = rcClient.top; i<= rcClient.bottom - 2; ++i)
    {
        point.y = i;
        mGPlusGetGradientColorValue (ptn, &point, &color);
        SetPenColor (hdc, color);
        MoveTo (hdc, rcClient.left, i);
        LineTo (hdc, rcClient.right, i);
    }
    mGPlusFreeColorPattern (ptn);

    //draw progressbar frame
    draw_fashion_frame (hdc, rcClient, gradient_color
            (c1, LFRDR_3DBOX_COLOR_DARKER, 120));

    x = rcClient.left + pbar_border;
    y = rcClient.top + pbar_border;
    w = RECTW (rcClient) - (pbar_border << 1);
    h = RECTH (rcClient) - (pbar_border << 1);

    ndiv_progress = ldiv (nMax - nMin, step);
    nAllPart = ndiv_progress.quot;
    
    ndiv_progress = ldiv (nPos - nMin, step);
    nNowPart = ndiv_progress.quot;

#if 0
    pixel[1] = DWORD2Pixel (hdc, gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER,
                50));
    pixel[0] = DWORD2Pixel (hdc, gradient_color (gradient_color 
                (c2, LFRDR_3DBOX_COLOR_LIGHTER, 80), LFRDR_3DBOX_COLOR_LIGHTER,
                250));
#endif

    conver_pixel = gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 50);
    pixel[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
               (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    
    conver_pixel = gradient_color (gradient_color (c2, LFRDR_3DBOX_COLOR_LIGHTER, 80),
                                   LFRDR_3DBOX_COLOR_LIGHTER,
                                   250);
    pixel[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
               (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    
    if (fVertical) {
        prog = h * nNowPart/nAllPart;
        if (nPos == nMax)
            prog = h;

        ptn = mGPlusCreateLinearPattern (rcClient,
                MGPLUS_GRADIENT_LINEAR_HORIZONTAL, pixel, 2, MGPLUS_GRADIENT_BIT_16);
        point.y = rcClient.top + 1;
        for (i = rcClient.left + 1; i <= rcClient.right - 2; ++i) {
            point.x = i;
            mGPlusGetGradientColorValue (ptn, &point, &color);
            SetPenColor (hdc, color);
            MoveTo (hdc, i, rcClient.bottom - prog - 3);
            LineTo (hdc, i, rcClient.bottom - 2);
        }

        mGPlusFreeColorPattern (ptn);
    }
    else {
        prog = w * nNowPart/nAllPart;
        if (nPos == nMax)
            prog = w;

        ptn = mGPlusCreateLinearPattern (rcClient,
                MGPLUS_GRADIENT_LINEAR_VERTICAL, pixel, 2, MGPLUS_GRADIENT_BIT_16);
        point.x = rcClient.left + 1;
        for (i = rcClient.top + 1; i <= rcClient.bottom - 2; ++i) {
            point.y = i;
            mGPlusGetGradientColorValue (ptn, &point, &color);
            SetPenColor (hdc, color);
            MoveTo (hdc, rcClient.left + 1, i);
            LineTo (hdc, rcClient.left + prog + 2, i);
        }

        mGPlusFreeColorPattern (ptn);
    }

    SetBkMode (hdc, BM_TRANSPARENT);
    sprintf (szText, "%d%%", (nNowPart*100/nAllPart));
    GetTextExtent (hdc, szText, -1, &text_ext);
    x += ((w - text_ext.cx) >> 1) + 1;
    y += ((h - text_ext.cy) >> 1);

    SetTextColor (hdc, GetWindowElementPixel (hWnd, WE_BGCB_ACTIVE_CAPTION));
    TextOut (hdc, x, y, szText);
}

static void draw_header (HWND hWnd, HDC hdc, const RECT *pRect, DWORD color)
{
    gal_pixel pixels[2];
    mgplus_pattern_t ptn;
    int i;
    POINT point;
    gal_pixel pixel_dst;
    gal_pixel pixel_org, conver_pixel;

    if (pRect->right < pRect->left || pRect->bottom < pRect->top) {
        return;
    }

    conver_pixel = gradient_color (color, LFRDR_3DBOX_COLOR_DARKER, 60);
    pixels[1] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
    conver_pixel = gradient_color (color, LFRDR_3DBOX_COLOR_LIGHTER, 120);
    pixels[0] = (conver_pixel & 0xFF000000) | ((conver_pixel & 0x000000FF) << 16) |
                (conver_pixel & 0x0000FF00) | ((conver_pixel & 0x00FF0000) >> 16);
#if 0 
    pixels[1] = DWORD2Pixel (hdc, gradient_color 
            (color, LFRDR_3DBOX_COLOR_DARKER, 60));
    pixels[0] = DWORD2Pixel (hdc, gradient_color 
            (color, LFRDR_3DBOX_COLOR_LIGHTER, 120));
#endif
    ptn = mGPlusCreateLinearPattern (*pRect, 
            MGPLUS_GRADIENT_LINEAR_VERTICAL,
            pixels, 2, MGPLUS_GRADIENT_BIT_16);

    pixel_org = GetPenColor (hdc);
    point.x = pRect->left;
    for (i = pRect->top; i < pRect->bottom; ++i) {
        point.y = i;
        if (mGPlusGetGradientColorValue (ptn, &point, &pixel_dst)) {
            SetPenColor (hdc, pixel_dst);
            MoveTo (hdc, point.x, point.y);
            LineTo (hdc, pRect->right - 1, point.y);
        }
    }

    mGPlusFreeColorPattern (ptn);

    /*border*/
    SetPenColor(hdc, PIXEL_darkgray);
    MoveTo(hdc, pRect->right-1,pRect->top);
    LineTo(hdc, pRect->right-1,pRect->bottom-1);
    LineTo(hdc, pRect->left,pRect->bottom-1);

    SetPenColor (hdc, pixel_org);
}

WINDOW_ELEMENT_RENDERER wnd_rdr_fashion = {
    "fashion",
    init,
    deinit,

    calc_3dbox_color,
    draw_3dbox,
    draw_radio,
    draw_checkbox,
    draw_checkmark,
    draw_arrow,
    draw_fold,
    draw_focus_frame,

    draw_normal_item,
    draw_hilite_item,
    draw_disabled_item,
    draw_significant_item,

    draw_push_button,
    draw_radio_button,
    draw_check_button,

    draw_border,
    draw_caption,
    draw_caption_button,
    draw_scrollbar,

    calc_trackbar_rect,
    draw_trackbar,

    calc_we_area,
    calc_we_metrics,
    hit_test,
    NULL,
    NULL,

    calc_thumb_area,
    disabled_text_out,
 
    draw_tab,
    draw_progress,
    draw_header,

    NULL,
    NULL,
    erase_bkgnd,

    draw_normal_menu_item,
    draw_hilite_menu_item,
    draw_disabled_menu_item,
};
#endif /* _MGLF_RDR_FASHION */
