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
** lf_classic.c: The classic LF implementation file.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#include "minigui.h"
#include "gdi.h"
#include "gal.h"
#include "window.h"
#include "fixedmath.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "element.h"
#include "dc.h"
#include "icon.h"
#include "ctrl/button.h"
#include "ctrl/trackbar.h"
#include "ctrl/scrollbar.h"

#include "lf_common.h"
#define V_DIFF 0.2

#define CAP_BTN_INTERVAL 2

#define UNDEFINED_HUE   (itofix(-1))

#define FULL_V    (itofix(1))

extern BOOL wndGetVScrollBarRect (const MAINWIN* pWin, RECT* rcVBar);
extern BOOL wndGetHScrollBarRect (const MAINWIN* pWin, RECT* rcHBar);

WINDOW_ELEMENT_RENDERER __mg_wnd_rdr_classic;

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
 * increase_vh:
 *      increase brightness of color in HSV space, i.e. change v, h value
 *      of a color
 * Author: XuguangWang
 * Date: 2007-11-22
 */
static void 
increase_vh(fixed src_v, fixed src_h, fixed* dest_v, fixed* dest_h, 
            BOOL is_half)
{
    int h;
    int delta_h;
    int aim_h;
    if (src_v < FULL_V) {
#ifdef V_LINE_CHANGE
        src_v = fixadd(src_v, ftofix(V_DIFF)<<(!is_half));
#else
        src_v = fixadd(src_v, fixdiv(src_v, itofix(3))>>is_half);
#endif
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


        /*-1>>1 == 1*/
        if (delta_h == -1)
        {
            *dest_h = itofix(aim_h);
            return;
        }

        /*not is_halt, nearer to aim_h*/
        *dest_h = itofix (aim_h - ((delta_h >>1)>>!is_half));
    }
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
    fixed h;
    fixed s;
    fixed v;
    fixed r = itofix(GetRValue(color));
    fixed g = itofix(GetGValue(color));
    fixed b = itofix(GetBValue(color));
    UINT  a = GetAValue(color);
    /*RGB => HSV*/
    RGB2HSV(r, g, b, &h, &s, &v);

    /*chang V and H of HSV*/
    switch (flag) {
        case LFRDR_3DBOX_COLOR_DARKER:
#ifdef V_LINE_CHANGE
            v = fixsub(v, ftofix(V_DIFF));
            if (v < 0)
                v = 0;
#else
            v = fixsub(v, fixdiv(v, itofix(4)));
#endif
            break;
        case LFRDR_3DBOX_COLOR_DARKEST:
#ifdef V_LINE_CHANGE
            v = fixsub(v, ftofix(V_DIFF)<<1);
            if (v < 0)
                v = 0;
#else
            v = fixsub(v, fixdiv(v, itofix(2)));
#endif
            break;
        case LFRDR_3DBOX_COLOR_LIGHTER:
            increase_vh(v, h, &v, &h, TRUE);
            break;
        case LFRDR_3DBOX_COLOR_LIGHTEST:
            increase_vh(v, h, &v, &h, FALSE);
            break;
    }
    /*HSV => RGB*/
    HSV2RGB(h, s, v, &r, &g, &b);

    return MakeRGBA(fixtoi(r), fixtoi(g), fixtoi(b), a);
}

/*
 * draw_one_frame:
 *      draw a frame of 3dbox.
 * Author: XuguangWang
 * Date: 2007-11-22
 */
static void 
draw_one_frame (HDC hdc, const RECT* rc, DWORD lt_color, DWORD rb_color)
{
    SetPenColor (hdc, RGBA2Pixel(hdc, GetRValue(lt_color), 
                GetGValue(lt_color), GetBValue(lt_color), 
                GetAValue(lt_color)));

    MoveTo(hdc, rc->left, rc->bottom-1);
    LineTo(hdc, rc->left, rc->top);
    LineTo(hdc, rc->right-1, rc->top);

    SetPenColor(hdc, RGBA2Pixel(hdc, GetRValue(rb_color), 
                GetGValue(rb_color), GetBValue(rb_color), 
                GetAValue(rb_color)));

    MoveTo (hdc, rc->left+1, rc->bottom-1);
    LineTo (hdc, rc->right-1, rc->bottom-1);
    LineTo (hdc, rc->right-1, rc->top);
}

static int fill_iso_triangle_snd(HDC hdc, DWORD color, 
                                POINT ap, POINT bp1, POINT bp2)
{ 
    int x1, y1;
    int  startx,endx,starty,endy;
    int xinc, yinc;
    gal_pixel old_color;

    if(bp1.y != bp2.y && bp1.x != bp2.x) return -1;

    x1 =ap.x;
    y1 =ap.y;
    SetPixel (hdc,x1,y1, RGBA2Pixel(hdc,GetRValue(color),
                GetGValue(color), GetBValue(color), GetAValue(color)));
    old_color = SetPenColor (hdc, RGBA2Pixel(hdc,GetRValue(color),
                GetGValue(color), GetBValue(color), GetAValue(color)));

    if(bp1.y ==bp2.y){
        startx = (bp1.x >bp2.x)?bp2.x:bp1.x;
        endx =(bp1.x >bp2.x)?bp1.x:bp2.x;
        starty =endy=bp1.y;
        for(xinc =startx;xinc <=endx;xinc++){
            MoveTo(hdc,x1,y1);
            LineTo(hdc,xinc,starty);
        }
    }
    else if(bp1.x ==bp2.x){
        starty = (bp1.y >bp2.y)?bp2.y:bp1.y;
        endy = (bp1.y >bp2.y)?bp1.y:bp2.y;
        startx =endx=bp1.x;
        for(yinc=starty; yinc<=endy; yinc++){
            MoveTo(hdc, x1, y1);
            if(yinc == y1 && x1 < startx)
                LineTo(hdc, startx+1, yinc);
            else
                LineTo(hdc, startx, yinc);

        }
    }
    else{
        printf("is not a isosceles triangle\n");
    }

    SetPenColor(hdc, old_color);

    return 0;
}

/*initialize and terminate interface*/
static int init (PWERENDERER renderer) 
{
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
    
    gui_LoadIconRes(HDC_SCREEN, renderer->name, (char*)SYSICON_TREEFOLD);
    gui_LoadIconRes(HDC_SCREEN, renderer->name, (char*)SYSICON_TREEUNFOLD);

    renderer->private_info = NULL;

    return 0;
}

static int deinit (PWERENDERER renderer)
{
    /* Destroy system icon. */
    TermRendererSystemIcon (renderer->we_icon[0], renderer->we_icon[1]);

    UnregisterSystemBitmap (HDC_SCREEN, renderer->name, SYSBMP_RADIOBUTTON);
    UnregisterSystemBitmap (HDC_SCREEN, renderer->name, SYSBMP_CHECKBUTTON);
    
    gui_UnLoadIconRes(HDC_SCREEN, renderer->name, (char*)SYSICON_TREEFOLD);
    gui_UnLoadIconRes(HDC_SCREEN, renderer->name, (char*)SYSICON_TREEUNFOLD);

    __mg_wnd_rdr_classic.private_info = NULL;

    return 0;
}

/*
 * draw_3dbox:
 *      draw a 3dbox.
 * param color - the main color of 3dbox     
 * param flag - how to draw the 3dbox (thin or thick frame, filled or 
 *      unfilled)   
 * Author: XuguangWang
 * Date: 2007-11-22
 */
static void draw_3dbox (HDC hdc, const RECT* pRect, DWORD color, DWORD flag)
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
                /*FIXME*/
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
    return;
}

/* This function draw a radio.
 * Author  : lfzhang
 * Date    : 2007-11-21
 */
static void draw_radio (HDC hdc, const RECT* pRect, DWORD color, int status)
{
    int radius, center_x, center_y, w, h;
    gal_pixel color_pixel, color_lightest, color_old;
    DWORD new_c;

    //FIXME
    /*if (pRect == NULL || hdc == HDC_INVALID)
        return;*/
    if (pRect == NULL)
        return;

    w = pRect->right - pRect->left;
    h = pRect->bottom - pRect->top;
    /*draw nothing*/
    if (w < 6 || h < 6)
        return;

    color_pixel = RGBA2Pixel (hdc, GetRValue(color), GetGValue(color), 
                        GetBValue(color), GetAValue(color));
    new_c = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_LIGHTEST);
    new_c = calc_3dbox_color (new_c, LFRDR_3DBOX_COLOR_LIGHTEST);
    new_c = calc_3dbox_color (new_c, LFRDR_3DBOX_COLOR_LIGHTEST);
    color_lightest = RGBA2Pixel (hdc, GetRValue(new_c), GetGValue(new_c), 
                        GetBValue(new_c), GetAValue(new_c));

    radius = w>h ? (h>>1)-1 : (w>>1)-1;
    center_x = pRect->left + (w>>1);
    center_y = pRect->top + (h>>1);

    color_old = SetBrushColor (hdc, color_pixel);

    if (status & LFRDR_MARK_HAVESHELL)
    {
        FillCircle (hdc, center_x, center_y, radius);
        SetBrushColor (hdc, color_lightest);
        FillCircle (hdc, center_x, center_y, radius-1);

        SetBrushColor (hdc, GetBkColor(hdc));
        FillCircle (hdc, center_x, center_y, radius-2);

        SetBrushColor (hdc, color_pixel);
    }

    if (status & LFRDR_MARK_ALL_SELECTED)
    {
        FillCircle (hdc, center_x, center_y, radius>>1);
        SetBrushColor (hdc, color_lightest);
        FillCircle (hdc, center_x-1, center_y-1, 1);
    }

    SetBrushColor (hdc, color_old);
    return;
}

/* This function draw a checkbox.
 * Author  : lfzhang
 * Date    : 2007-11-22
 */
static void draw_checkbox (HDC hdc, const RECT* pRect, DWORD color, int status)
{
    int i, w, h, side_len, boundary;
    int box_l, box_t, box_r, box_b;
    int cross_l, cross_t, cross_r, cross_b;
    int border_cut;
    gal_pixel color_pixel, pen_color_old, bru_color_old;

    if (pRect == NULL)
        return;
    
    w = pRect->right - pRect->left;
    h = pRect->bottom - pRect->top;

    /*Draw nothing.*/
    if (w < 6 || h < 6)
        return;

    side_len = w>=h ? h : w;
    boundary = w>=h ? (w-h)>>1: (h-w)>>1;
    border_cut = (side_len+1)>>3;
    
    color_pixel = RGBA2Pixel (hdc, GetRValue(color), GetGValue(color), 
                        GetBValue(color), GetAValue(color));
    pen_color_old = SetPenColor (hdc, color_pixel);
    
    if (w > h)
    {
        box_l = pRect->left + boundary;
        box_t = pRect->top;
        box_r = box_l + side_len-1;
        box_b = pRect->bottom-1;
    }
    else if (w < h)
    {
        box_l = pRect->left;
        box_t = pRect->top + boundary;
        box_r = pRect->right-1;
        box_b = box_t + side_len-1;
    }
    else
    {
        box_l = pRect->left;
        box_t = pRect->top;
        box_r = pRect->right-1;
        box_b = pRect->bottom-1;
    }
    
    cross_l = box_l + ((side_len+1)>>2);
    cross_t = box_t + ((side_len+1)>>2);
    cross_r = box_r - ((side_len+1)>>2);
    cross_b = box_b - ((side_len+1)>>2);
    
    /*Draw border.*/
    if (status & LFRDR_MARK_HAVESHELL)
    {
        for (i=(side_len+1)>>3; i>0; i--)
        {
            MoveTo (hdc, box_l, box_t + i-1);
            LineTo (hdc, box_r, box_t + i-1);

            MoveTo (hdc, box_r, box_b - i+1);
            LineTo (hdc, box_l, box_b - i+1);

            MoveTo (hdc, box_r-i+1, box_t);
            LineTo (hdc, box_r-i+1, box_b);
            
            MoveTo (hdc, box_l+i-1, box_b);
            LineTo (hdc, box_l+i-1, box_t);
          
        }
    }
    
    /*Draw cross*/
    if (status & LFRDR_MARK_ALL_SELECTED)
    {
        for (i=(side_len+1)>>3; i>0; i--)
        {
            MoveTo (hdc, cross_l+i-1, cross_b);
            LineTo (hdc, cross_r-((side_len>>3)-i+1), cross_t);
            
            MoveTo (hdc, cross_l+i-1, cross_t);
            LineTo (hdc, cross_r-((side_len>>3)-i+1), cross_b);
        }
    }
    else if (status & LFRDR_MARK_HALF_SELECTED)
    {
        bru_color_old = SetBrushColor (hdc, COLOR_lightgray);
        FillBox (hdc, box_l + border_cut, box_t + border_cut, 
                        (box_r-box_l) - (border_cut<<1)+1, 
                        (box_b-box_t) - (border_cut<<1)+1) ;

        for (i=(side_len+1)>>3; i>0; i--)
        {
            MoveTo (hdc, cross_l+i-1, cross_b);
            LineTo (hdc, cross_r-((side_len>>3)-i+1), cross_t);
            
            MoveTo (hdc, cross_l+i-1, cross_t);
            LineTo (hdc, cross_r-((side_len>>3)-i+1), cross_b);
        }
        SetBrushColor (hdc, bru_color_old);
    }
    
    SetPenColor (hdc, pen_color_old);
    
    return;
}

/* This function draw a checkmark.
 * Author  : lfzhang
 * Date    : 2007-11-22
 */
static void draw_checkmark (HDC hdc, const RECT* pRect, DWORD color, int status)
{
    int i, w, h, side_len, boundary;
    int box_l, box_t, box_r, box_b;
    int hook_l, hook_t, hook_r, hook_b;
    int border_cut;
    gal_pixel color_pixel, pen_color_old, bru_color_old;
    
    if (pRect == NULL)
        return;
    
    w = pRect->right - pRect->left;
    h = pRect->bottom - pRect->top;
    
    /*Draw nothing.*/
    if (w < 6 || h < 6)
        return;

    side_len = w>=h ? h : w;
    boundary = w>=h ? (w-h)>>1: (h-w)>>1;
    border_cut = (side_len+1)>>3;

    color_pixel = RGBA2Pixel (hdc, GetRValue(color), GetGValue(color), 
                        GetBValue(color), GetAValue(color));
    pen_color_old = SetPenColor (hdc, color_pixel);
    
    if (w > h)
    {
        box_l = pRect->left + boundary;
        box_t = pRect->top;
        box_r = box_l + side_len-1;
        box_b = pRect->bottom-1;
    }
    else if (w < h)
    {
        box_l = pRect->left;
        box_t = pRect->top + boundary;
        box_r = pRect->right-1;
        box_b = box_t + side_len-1;
    }
    else
    {
        box_l = pRect->left;
        box_t = pRect->top;
        box_r = pRect->right-1;
        box_b = pRect->bottom-1;
    }
    
  
    hook_l = box_l + (side_len>>2);
    hook_t = box_t + (side_len>>2);
    hook_r = box_r - (side_len>>2);
    hook_b = box_b - (side_len>>2);

    bru_color_old = SetBrushColor (hdc, GetBkColor (hdc));
    FillBox (hdc, box_l + border_cut, box_t + border_cut, 
                    (box_r-box_l) - (border_cut<<1)+1, 
                    (box_b-box_t) - (border_cut<<1)+1) ;
    SetBrushColor (hdc, bru_color_old);

    /*Draw border.*/
    if (status & LFRDR_MARK_HAVESHELL)
    {
        for (i=(side_len+1)>>3; i>0; i--)
        {
            MoveTo (hdc, box_l, box_t + i-1);
            LineTo (hdc, box_r, box_t + i-1);

            MoveTo (hdc, box_r, box_b - i+1);
            LineTo (hdc, box_l, box_b - i+1);

            MoveTo (hdc, box_r-i+1, box_t);
            LineTo (hdc, box_r-i+1, box_b);
            
            MoveTo (hdc, box_l+i-1, box_b);
            LineTo (hdc, box_l+i-1, box_t);
        }
    }
    
    /*Draw hook*/
    if (status & LFRDR_MARK_ALL_SELECTED)
    {
        for (i=(side_len+1)>>3; i>0; i--)
        {
            MoveTo (hdc, hook_l, hook_t+((side_len+1)>>3)+i-1);
            LineTo (hdc, hook_l+((side_len+1)>>3), hook_b-(((side_len+1)>>3)-i+1));
            LineTo (hdc, hook_r, hook_t+i-1);
        }
    }
    else if (status & LFRDR_MARK_HALF_SELECTED)
    {
        bru_color_old = SetBrushColor (hdc, COLOR_lightgray);
        FillBox (hdc, box_l + border_cut, box_t + border_cut, 
                        (box_r-box_l) - (border_cut<<1)+1, 
                        (box_b-box_t) - (border_cut<<1)+1) ;

        for (i=(side_len+1)>>3; i>0; i--)
        {
            MoveTo (hdc, hook_l, hook_t+((side_len+1)>>3)+i-1);
            LineTo (hdc, hook_l+((side_len+1)>>3), hook_b-(((side_len+1)>>3)-i+1));
            LineTo (hdc, hook_r, hook_t+i-1);
        }
        SetBrushColor (hdc, bru_color_old);
    }
    
    SetPenColor (hdc, pen_color_old);

    return;
}

/*
 * draw_arrow:
 *  This function draw a arrow by the color.
 * 
 * param hdc : the HDC .
 * param pRect : the point to the rectangle area to drawing.
 * param color : the brush color.
 * param status : the direction of the arrow.
 *
 * Author : wangjian<wangjian@minigui.org>
 * Date : 2007-11-23.
 */
static void draw_arrow (HWND hWnd, HDC hdc, const RECT* pRect, DWORD color, int status)
{
    int w, h;
    int     _index, col_index; 
    // p1, the apex point of the isosceles triangle,
    // p2,p3, the 2 base points if the isosceles tiangle.
    POINT p1, p2, p3; 
    gal_pixel old_pen_color;
    DWORD color_3d;
    int vdelta,hdelta;

    w = pRect->right - pRect->left;
    h = pRect->bottom - pRect->top;
    if(w <= 0 || h <= 0) return;

    vdelta =((w>>2)) >(h/3)?(h/3):((w>>2));
    hdelta =((h>>2)) >(w/3)?(w/3):((h>>2));

    if(status & LFRDR_ARROW_HAVESHELL)
    {
        _index = WE_MAINC_THREED_BODY & WE_ATTR_INDEX_MASK;
        col_index = 
            (WE_MAINC_THREED_BODY & WE_ATTR_TYPE_COLOR_MASK) >> 8;
        color_3d = __mg_wnd_rdr_classic.we_colors[_index][col_index];
        
        draw_3dbox(hdc, pRect, color_3d, 
                status | LFRDR_3DBOX_FILLED);
    } 
    switch(status & LFRDR_ARROW_DIRECT_MASK)
    {
        case LFRDR_ARROW_UP:
            p1.x = pRect->left + (w>>1);
            p1.y = pRect->top + (h/3);
            p2.x = p1.x - vdelta;
            p2.y = p1.y + vdelta;
            p3.x = p1.x + vdelta;
            p3.y = p1.y + vdelta;
            break;
        case LFRDR_ARROW_DOWN:
            p1.x = pRect->left  + (w>>1);
            p1.y = pRect->bottom- (h/3) - 1;
            p2.x = p1.x - vdelta;
            p2.y = p1.y - vdelta;
            p3.x = p1.x + vdelta;
            p3.y = p1.y - vdelta;
            break;
        case LFRDR_ARROW_LEFT:
            p1.x = pRect->left  + (w/3);
            p1.y = pRect->top   + (h>>1);
            p2.x = p1.x + hdelta;
            p2.y = p1.y - hdelta;
            p3.x = p1.x + hdelta;
            p3.y = p1.y + hdelta;
            break;
        case LFRDR_ARROW_RIGHT:
            p1.x = pRect->right - (w/3) - 1;
            p1.y = pRect->top   + (h>>1);
            p2.x = p1.x - hdelta;
            p2.y = p1.y - hdelta;
            p3.x = p1.x - hdelta;
            p3.y = p1.y + hdelta;
            break;
        default :
            return;
    }

    if(status & LFRDR_ARROW_NOFILL)
    {
        old_pen_color = SetPenColor(hdc, RGBA2Pixel(hdc, GetRValue(color),
                    GetGValue(color), GetBValue(color), GetAValue(color)));
        MoveTo(hdc, p1.x, p1.y);
        LineTo(hdc, p2.x, p2.y);
        LineTo(hdc, p3.x, p3.y);
        LineTo(hdc, p1.x, p1.y);
        SetPenColor (hdc, old_pen_color);
    }
    else
    {
        fill_iso_triangle_snd(hdc, color, p1, p2, p3);
    }
    return;
}

/*
 * draw_fold:
 *  This function draw a fold by the color.
 * 
 * param hdc : the HDC .
 * param pRect : the point to the rectangle area to drawing.
 * param color : the pen color.
 * param is_opened : the status of the fold.
 *
 * Author : wangjian<wangjian@minigui.org>
 * Date : 2007-11-23.
 */
static void draw_fold (HWND hWnd, HDC hdc, const RECT* pRect, 
        DWORD color, int status, int next)
{
    int i;
    int pen_width;
    int w, h, minSize;
    int centerX, centerY;
    gal_pixel old_pen_color;

    w = RECTWP(pRect);
    h = RECTHP(pRect);

    if(w < 4 || h < 4) return;

    centerX = pRect->left + (w>>1);
    centerY = pRect->top + (h>>1);

    /* houhh 20090410, if the treeview with icon style.*/
    if (status & LFRDR_TREE_WITHICON) {
        char szValue[255];
        HICON hFoldIcon   = (HICON)RetrieveRes (gui_GetIconFile("classic", (char*)SYSICON_TREEFOLD, szValue));
        HICON hUnFoldIcon = (HICON)RetrieveRes (gui_GetIconFile("classic", (char*)SYSICON_TREEUNFOLD, szValue));

        if (status & LFRDR_TREE_CHILD) {
            if (status & LFRDR_TREE_FOLD) {
                if (hFoldIcon)
                    DrawIcon (hdc, centerX, centerY - (h>>1),
                            h, h, hFoldIcon);
            }
            else {
                if (hUnFoldIcon)
                    DrawIcon (hdc, centerX, centerY - (h>>1),
                            h, h, hUnFoldIcon);
            }
        }
    }
    else {
        old_pen_color = SetPenColor(hdc, RGBA2Pixel(hdc,GetRValue(color),
                    GetGValue(color), GetBValue(color), GetAValue(color)));

        minSize = MIN(w, h)>>1;
        pen_width = (minSize>>3) + 1;

        centerX = pRect->left + (w>>1);
        centerY = pRect->top + (h>>1);

        if (status & LFRDR_TREE_CHILD) {
            MoveTo (hdc, centerX, centerY - h + minSize);
            LineTo (hdc, centerX, centerY - minSize);
            MoveTo (hdc, centerX + minSize, centerY);

            if(status & LFRDR_TREE_FOLD)
                LineTo (hdc, centerX + w, centerY);
            else
                //LineTo (hdc, centerX + minSize + w, centerY);
                LineTo (hdc, centerX + w, centerY);

            if (status & LFRDR_TREE_NEXT) {
                MoveTo (hdc, centerX, centerY + minSize);

                if(status & LFRDR_TREE_FOLD)
                    LineTo (hdc, centerX, centerY - minSize + h);
                else
                    LineTo (hdc, centerX, centerY - minSize + h + h*next);
            }
        }
        else {
            MoveTo (hdc, centerX, centerY - h + minSize);
            LineTo (hdc, centerX, centerY);
            LineTo (hdc, centerX + w, centerY);

            if (status & LFRDR_TREE_NEXT) {
                MoveTo (hdc, centerX, centerY);
                LineTo (hdc, centerX, centerY + h - minSize);
            }
        }

        if (!(status & LFRDR_TREE_CHILD)) {
            SetPenColor(hdc, old_pen_color);
            return;
        }

        for(i = 0; i < pen_width; i++)
        {
            Rectangle(hdc, centerX - minSize + i, centerY - minSize + i, 
                    centerX + minSize - 1 - i, centerY + minSize - 1 - i);

            MoveTo(hdc, centerX - minSize + 2 * pen_width, 
                    centerY - (pen_width>>1) + i);
            LineTo(hdc, centerX + minSize -1 - 2 * pen_width, 
                    centerY - (pen_width>>1) + i);

            if(status & LFRDR_TREE_FOLD)
            {
                MoveTo(hdc, centerX - (pen_width>>1) + i, 
                        centerY - minSize + 2 * pen_width);
                LineTo(hdc, centerX - (pen_width>>1) + i,
                        centerY + minSize - 1 - 2 * pen_width);
            }
        }

        SetPenColor(hdc, old_pen_color);
    }
}

/*
 * draw_focus_frame:
 *  This function draw a focus frame by the color.
 *
 * param hdc : the HDC .
 * param pRect : the rectangle for the frame.
 * param color : the pen color.
 *
 * Author : wangjian<wangjian@minigui.org>
 * Date : 2007-11-26.
 */
static void draw_focus_frame (HDC hdc, const RECT *pRect, DWORD color)
{
#if 0
    gal_pixel old_pen_color;
    
    old_pen_color = SetPenColor(hdc, RGBA2Pixel(hdc, GetRValue(~color),
                    GetGValue(~color),GetBValue(~color),GetAValue(~color)));
    FocusRect(hdc, pRect->left, pRect->top, 
            pRect->right, pRect->bottom);
    SetPenColor(hdc, old_pen_color);
    return;
#else    
    int i;
    gal_pixel pixel;

    
    pixel = RGBA2Pixel(hdc, GetRValue(color), GetGValue(color), 
                            GetBValue(color), GetAValue(color));

    for(i = pRect->left; i < pRect->right; i++)
    {
        if(i & 0x01)
        {
            SetPixel(hdc, i, pRect->top ,pixel);
            SetPixel(hdc, i, pRect->bottom ,pixel);
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
    return;
#endif
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
    gal_pixel old_bc;
    old_bc = SetBrushColor (hdc, DWORD2Pixel (hdc, color));
    FillBox (hdc, pRect->left, pRect->top, RECTWP (pRect), RECTHP (pRect));
    SetBrushColor (hdc, old_bc);
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
    gal_pixel old_bc;
    old_bc = SetBrushColor (hdc, DWORD2Pixel (hdc, color));
    FillBox (hdc, pRect->left, pRect->top, RECTWP (pRect), RECTHP (pRect));
    SetBrushColor (hdc, old_bc);
}

static void draw_disabled_menu_item 
(HWND hWnd, HDC hdc, const RECT* pRect, DWORD color)
{
    gal_pixel old_bc;
    old_bc = SetBrushColor (hdc, DWORD2Pixel (hdc, color));
    FillBox (hdc, pRect->left, pRect->top, RECTWP (pRect), RECTHP (pRect));
    SetBrushColor (hdc, old_bc);
}

/*
 * draw_push_button:
 *  This function draw a push button by the color.
 *
 * param hdc : the HDC .
 * param pRect : the rectangle for the frame.
 * param color1 : the pen color.
 * param color2 : No used in this renderer.
 * param status : the button status.
 *
 * Author : wangjian<wangjian@minigui.org>
 * Date : 2007-11-26.
 */

#define STATUS_GET_CHECK(status) ((status) & BST_CHECK_MASK)

static void draw_push_button (HWND hWnd, HDC hdc, const RECT* pRect, 
        DWORD color1, DWORD color2, int status)
{
    DWORD dark_color;
    gal_pixel dark_pixel;
    gal_pixel old_pixel;
    int i;

    /*draw the integer of button*/
    draw_3dbox (hdc, pRect, color1, (status & BST_POSE_MASK)|
                LFRDR_3DBOX_THICKFRAME);//|LFRDR_3DBOX_FILLED

    /*modify if it is a pushlike button*/
    if (STATUS_GET_CHECK (status) != BST_UNCHECKED)
    {
        dark_color = calc_3dbox_color (color1, LFRDR_3DBOX_COLOR_DARKER);
        dark_pixel = DWORD2Pixel(hdc, dark_color);

        if (STATUS_GET_CHECK (status) == BST_CHECKED)
        {
            /*checked looks like a pressed button*/
            draw_3dbox (hdc, pRect, color1, 
                    LFRDR_BTN_STATUS_PRESSED|LFRDR_3DBOX_THICKFRAME);

            /*fill content with a darker color*/
            old_pixel = SetBrushColor(hdc, dark_pixel);
            FillBox(hdc, pRect->left+2, pRect->top+2,
                    RECTWP(pRect)-4, RECTHP(pRect)-4);
            SetBrushColor(hdc, old_pixel);
        }

        else if (STATUS_GET_CHECK (status) == BST_INDETERMINATE)
        {
            draw_3dbox (hdc, pRect, color1, 
                    LFRDR_BTN_STATUS_PRESSED);

            /*checked looks like a half pressed button*/
            old_pixel = SetPenColor(hdc, dark_pixel);
            for (i=1; i<BTN_WIDTH_BORDER; i++)
            Rectangle(hdc, pRect->left+i, pRect->top+i,
                    pRect->right-i-1, pRect->bottom-i-1);
            SetPenColor(hdc, old_pixel);
        }
    }
    return;
}

/* This function draw a radio button. */
#define SIZE_RADIOBTN 12
static void draw_radio_button (HWND hWnd, HDC hdc, const RECT* pRect, int status)
{
#if 0
    int w, h;
    int box_l, box_t;
    const BITMAP* radio_bmp;

    radio_bmp = 
        GetSystemBitmapEx (__mg_wnd_rdr_classic.name, SYSBMP_RADIOBUTTON);

    w = pRect->right - pRect->left;
    h = pRect->bottom - pRect->top;

    if(w >= 12 && h >= 12)//donnot scale
    {
        box_l = pRect->left + (w>>1)-6;
        box_t = pRect->top + (h>>1)-6;

        /*parity check*/
        if (w & 0x1)
            box_l += 1;
    
        if (h & 0x1)
            box_t += 1;
    
        FillBoxWithBitmapPart (hdc, box_l, box_t, 12, 12, 0, 0, radio_bmp, 
                    /*status * 12*/
                    (status<<3)+(status<<2), 0);
    }
    else {
        FillBitmapPartInBox (hdc, pRect->left, pRect->top, w, h,
            radio_bmp, (status<<3)+(status<<2), 0, 12,12);
    }
#else
    int w, h;
    int box_l, box_t, off_h = 0, off_v = 0;
    const BITMAP* radio_bmp;

    radio_bmp = 
        GetSystemBitmapEx (__mg_wnd_rdr_classic.name, SYSBMP_RADIOBUTTON);

    /* 
     * We do not scale the image, but restrict the output in the specified 
     * rectangle, and paint the image in the center of the rectangle.
     */

    w = RECTWP (pRect);
    h = RECTHP (pRect);
    if (w <= 0 || h <= 0)
        return;

    box_l = pRect->left + (w>>1) - (SIZE_RADIOBTN >> 1);
    box_t = pRect->top + (h>>1) - (SIZE_RADIOBTN >> 1);
    if (w & 0x1) box_l += 1;
    if (h & 0x1) box_t += 1;

    if (box_l < pRect->left) {
        off_h = pRect->left - box_l;
        box_l = pRect->left;
    }

    if (box_t < pRect->top) {
        off_v = pRect->top - box_t;
        box_t = pRect->top;
    }

    gui_fill_box_with_bitmap_part_except_incompatible(hdc, box_l, box_t, 
            SIZE_RADIOBTN - (off_h << 1), SIZE_RADIOBTN - (off_v << 1), 0, 0, 
            radio_bmp, status * SIZE_RADIOBTN + off_h, off_v);
#endif
}

/* This function draw a check button. */
#define SIZE_CHECKBOX 13
static void draw_check_button (HWND hWnd, HDC hdc, const RECT* pRect, int status)
{
#if 0
    int w, h;
    int box_l, box_t;
    const BITMAP* check_bmp;

    check_bmp = 
        GetSystemBitmapEx (__mg_wnd_rdr_classic.name, SYSBMP_CHECKBUTTON);

    w = pRect->right - pRect->left;
    h = pRect->bottom - pRect->top;

    if( w >= 13 && h >= 13){
        box_l = pRect->left + (w>>1)-6;
        box_t = pRect->top + (h>>1)-6;

        /*parity check*/
        if (w & 0x1)
            box_l += 1;
    
        if (h & 0x1)
            box_t += 1;

        FillBoxWithBitmapPart (hdc, box_l, box_t, 13, 13, 0, 0, check_bmp, 
                        /*status * 13*/
                        (status<<3)+(status<<2)+status, 0);
    }
    else {
        FillBitmapPartInBox (hdc, pRect->left, pRect->top, w, h,
            check_bmp, (status<<3)+(status<<2), 0, 13,13);
    }
    return;
#else
    int w, h;
    int box_l, box_t, off_h = 0, off_v = 0;
    const BITMAP* check_bmp;

    check_bmp = 
        GetSystemBitmapEx (__mg_wnd_rdr_classic.name, SYSBMP_CHECKBUTTON);

    /* 
     * We do not scale the image, but restrict the output in the specified 
     * rectangle, and paint the image in the center of the rectangle.
     */

    w = RECTWP (pRect);
    h = RECTHP (pRect);
    if (w <= 0 || h <= 0)
        return;

    box_l = pRect->left + (w >> 1) - (SIZE_CHECKBOX >> 1);
    box_t = pRect->top + (h >> 1) - (SIZE_CHECKBOX >> 1);
    if (w & 0x1) box_l += 1;
    if (h & 0x1) box_t += 1;

    if (box_l < pRect->left) {
        off_h = pRect->left - box_l;
        box_l = pRect->left;
    }

    if (box_t < pRect->top) {
        off_v = pRect->top - box_t;
        box_t = pRect->top;
    }

    if (IsCompatibleDC (HDC_SCREEN, hdc)) {
        FillBoxWithBitmapPart (hdc, box_l, box_t, 
                    SIZE_CHECKBOX - (off_h << 1), SIZE_CHECKBOX - (off_v << 1), 0, 0, 
                    check_bmp, status * SIZE_CHECKBOX + off_h, off_v);
    }
    else {
        HDC mem_dc;
        mem_dc = CreateMemDCFromBitmap (HDC_SCREEN, check_bmp);

        if (mem_dc != HDC_INVALID) {
            BitBlt (mem_dc, status * SIZE_CHECKBOX + off_h, off_v, 
                    SIZE_CHECKBOX - (off_h << 1), SIZE_CHECKBOX - (off_v << 1),
                    hdc, box_l, box_t, 0);
            DeleteMemDC (mem_dc);
        }
    }
#endif
}

static int get_window_border (HWND hWnd, int dwStyle, int win_type)
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
            if (_dwStyle & WS_BORDER)
                /*WIN_INNER_FRAME + WIN_3D_OUTER_FRAME = 3*/
                return GetWindowElementAttr (hWnd, WE_METRICS_WND_BORDER) + 3;
            else if (_dwStyle & WS_THINFRAME)
                /*WIN_INNER_FRAME + WIN_THIN_OUTER_FRAME = 2*/
                return GetWindowElementAttr (hWnd, WE_METRICS_WND_BORDER) + 2;
            else if (_dwStyle & WS_THICKFRAME)
                /*outer frame = border, inner frame is 1*/
                return GetWindowElementAttr (hWnd, WE_METRICS_WND_BORDER) * 2 + 1;
            break;
        }
        case LFRDR_WINTYPE_DIALOG:
        {
            if (_dwStyle & WS_BORDER)
                return 3;
            else if (_dwStyle & WS_THINFRAME)
                return 1; 
            else if (_dwStyle & WS_THICKFRAME)
                return 2; 
            break;
        }
        case LFRDR_WINTYPE_CONTROL:
        {
#if 1
            if(_dwStyle& WS_BORDER) {
                if(_dwStyle & WS_THICKFRAME)    
                    return 2;
                else if(_dwStyle & WS_THINFRAME)    
                    return 1;
                else 
                    return 2;
            }
            else if (_dwStyle & WS_THINFRAME)
                return 1; 
            else if (_dwStyle & WS_THICKFRAME)
                return 2; 
#else
            if ((_dwStyle & WS_BORDER) && (_dwStyle & WS_THINFRAME))
                return 1; 
            else if (_dwStyle & WS_BORDER)
                return 2; 
            else if (_dwStyle & WS_THINFRAME)
                return 1; 
            else if (_dwStyle & WS_THICKFRAME)
                return 2; 
#endif
            break;
        }
    }

    return 0;
}

/*
 * calc_capbtn_area:
 *      calc the icon and buttons' area in caption.
 * Author: XuguangWang
 * Date: 2007-11-22
 */
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

/*
 * calc_hscroll_area:
 *      calc the area of parts of hscrollbar.
 * Author: XuguangWang
 * Date: 2007-11-22
 */
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

/*
 * calc_vscroll_area:
 *      calc the area of parts of vscrollbar.
 * Author: XuguangWang
 * Date: 2007-11-22
 */
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
                cap_h = __mg_wnd_rdr_classic.we_metrics[_idx];
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
                return __mg_wnd_rdr_classic.we_metrics[_idx];
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
                return __mg_wnd_rdr_classic.we_metrics[_idx];
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
                return __mg_wnd_rdr_classic.we_metrics[_idx];
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

/*
 * calc_we_area:
 *      calc the area of window element including parts of caption, scrollbars.
 *      the result(we_area) is on the same coordinates as left and top in
 *      windowinfo of hWnd
 * Author: XuguangWang
 * Date: 2007-11-22
 */
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
            cap_h = GetWindowElementAttr(hWnd, WE_METRICS_CAPTION);
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
            menu_h = GetWindowElementAttr (hWnd, WE_METRICS_MENU);

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

/*
 * calc_thumb_area:
 *      cale thumb area in scrollbar of hWnd.
 * Author: XuguangWang
 * Date: 2007-11-22
 */
static void calc_thumb_area (HWND hWnd, BOOL vertical, LFSCROLLBARINFO* sb_info)
{
    RECT rc;
    int move_range;
    div_t divt;
    int size = 0;
    RECT rcBar;
    
    if (vertical) {
        *sb_info = GetWindowInfo(hWnd)->vscroll;
        calc_vscroll_area(hWnd, HT_VSCROLL, &rc);
        sb_info->arrowLen  = RECTW (rc);
        move_range = RECTH (rc) - (sb_info->arrowLen << 1);

        wndGetVScrollBarRect ((PMAINWIN)hWnd, &rcBar);
        rcBar.top += GetWindowElementAttr (hWnd, WE_METRICS_SCROLLBAR);
        rcBar.bottom -= GetWindowElementAttr (hWnd, WE_METRICS_SCROLLBAR);
        size = RECTH(rcBar);
    }
    else {
        *sb_info = GetWindowInfo(hWnd)->hscroll;
        calc_hscroll_area(hWnd, HT_HSCROLL, &rc);
        sb_info->arrowLen  = RECTH (rc);
        move_range = RECTW (rc) - (sb_info->arrowLen << 1);

        wndGetHScrollBarRect ((PMAINWIN)hWnd, &rcBar);
        rcBar.left += GetWindowElementAttr (hWnd, WE_METRICS_SCROLLBAR);
        rcBar.right -= GetWindowElementAttr (hWnd, WE_METRICS_SCROLLBAR);
        size = RECTW(rcBar);
    }

    if (move_range < 0)
        move_range = 0;

    /* only have one page. */
    if ((sb_info->minPos == sb_info->maxPos) 
        || (sb_info->maxPos <= sb_info->pageStep - 1)) {
        sb_info->barStart = 0;
        sb_info->barLen   = move_range;
        return;
    }

    divt = div (move_range, sb_info->maxPos - sb_info->minPos + 1);

    sb_info->barLen = sb_info->pageStep * divt.quot +
        sb_info->pageStep * divt.rem / 
        (sb_info->maxPos - sb_info->minPos + 1);

    /* houhh 20090728, if vthumb height is not zero, then
     * set is to smallest barLen.*/
    if ((sb_info->barLen || sb_info->pageStep != 0) &&
            sb_info->barLen < LFRDR_SB_MINBARLEN) 
    {
        if (size < LFRDR_SB_MINBARLEN)
        {
            sb_info->barLen = size;
        }
        else
        {
            sb_info->barLen = LFRDR_SB_MINBARLEN;
        }
    }

    if (sb_info->curPos == sb_info->minPos) {
        /* first page, first position. */
        sb_info->barStart = 0;
        return;
    }
    else if (sb_info->curPos + sb_info->pageStep - 1 
            >= sb_info->maxPos) { 
        /* last page */
        sb_info->barStart = move_range - sb_info->barLen;
        return;
    }
    else {
        /* middle page */
        sb_info->barStart = 
            (sb_info->curPos - sb_info->minPos) * divt.quot + 
            (sb_info->curPos - sb_info->minPos) * divt.rem /
            (sb_info->maxPos - sb_info->minPos) + 0.5;

        if (sb_info->barStart + sb_info->barLen > move_range)
            sb_info->barStart = move_range - sb_info->barLen;
        if (sb_info->barStart < 0)
            sb_info->barStart = 0;
    }
}

/*
 * find_interval:
 *      find a half closed interval which val in.
 *      example 5 is in [4, 10)
 * Author: XuguangWang
 * Date: 2007-11-22
 */
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

        tmp = GetWindowElementAttr(hWnd, WE_METRICS_CAPTION);
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
 * param hWnd : the handle of the window.
 * param hdc : the DC of the window.
 * param is_active : whether the window is actived.
 *
 * Author: wangjian<wangjian@minigui.org>
 * Date: 2007-11-22
 */
static void draw_border (HWND hWnd, HDC hdc, BOOL is_active)
{
    int i , border;
    const WINDOWINFO *win_info = NULL;
    DWORD main_3d;
    RECT rect;
    gal_pixel border_color, old_pen_color;
        
    main_3d = GetWindowElementAttr(hWnd, WE_MAINC_THREED_BODY);
    border = (int)GetWindowElementAttr(hWnd, WE_METRICS_WND_BORDER);

    if (border < -2)
        return ;
    
    if(is_active)
    border_color = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_ACTIVE_WND_BORDER);
    else
    border_color = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_INACTIVE_WND_BORDER);
    
    win_info = GetWindowInfo(hWnd);
    
    if(calc_we_area(hWnd, HT_BORDER, &rect) == -1)
        return;

    old_pen_color = SetPenColor(hdc, border_color);

    if (IsDialog(hWnd))   // for dialog
    {    
        if(win_info->dwStyle & WS_BORDER)
        {
            if(is_active)
                draw_3dbox(hdc, &rect, main_3d, 
                        LFRDR_3DBOX_THICKFRAME | LFRDR_BTN_STATUS_NORMAL);
            else
                draw_3dbox(hdc, &rect, main_3d, 
                        LFRDR_3DBOX_THICKFRAME | LFRDR_BTN_STATUS_NORMAL);
            SetPenColor(hdc, RGB2Pixel(hdc, GetRValue(main_3d), 
                        GetGValue(main_3d), GetBValue(main_3d)));
            Rectangle(hdc, rect.left+2, rect.top+2, 
                    rect.right-2-1, rect.bottom-2-1);
        }
        else if(win_info->dwStyle & WS_THICKFRAME)    
        {
            for(i = 0; i < 2 && i < border; i++)
            {
                Rectangle(hdc, rect.left+i, rect.top+i,
                        rect.right-i-1, rect.bottom-i-1);
            }
        }
        else
        {
            Rectangle(hdc, rect.left, rect.top,
                    rect.right-1, rect.bottom-1);
        }
    }
    else if (IsMainWindow (hWnd)) // for main window
    {
        if(win_info->dwStyle & WS_BORDER)
        {
            if(is_active)
                draw_3dbox(hdc, &rect, main_3d, 
                        LFRDR_3DBOX_THICKFRAME | LFRDR_BTN_STATUS_NORMAL);
            else
                draw_3dbox(hdc, &rect, main_3d, 
                        LFRDR_3DBOX_THICKFRAME | LFRDR_BTN_STATUS_NORMAL);
            for(i = 2; i < border + 2; i++)
            {
                Rectangle(hdc, rect.left+i, rect.top+i,
                        rect.right-i-1, rect.bottom-i-1);
            }
        }
        else if(win_info->dwStyle & WS_THICKFRAME)    
        {
            for(i = 0; i < (border<<1); i++)
            {
                Rectangle(hdc, rect.left+i, rect.top+i,
                        rect.right-i-1, rect.bottom-i-1);
            }
        }
        else
        {
            SetPenColor(hdc, RGB2Pixel(hdc, GetRValue(main_3d), 
                        GetGValue(main_3d), GetBValue(main_3d)));
            Rectangle(hdc, rect.left, rect.top, 
                    rect.right-1, rect.bottom-1);
            SetPenColor(hdc, border_color);
            for(i = 1; i < border+1; i++)
            {
                Rectangle(hdc, rect.left+i, rect.top+i,
                        rect.right-i-1, rect.bottom-i-1);
            }
        }
        SetPenColor(hdc, RGB2Pixel(hdc, GetRValue(main_3d), 
                    GetGValue(main_3d), GetBValue(main_3d)));
        Rectangle(hdc, rect.left+i, rect.top+i, 
                    rect.right-i-1, rect.bottom-i-1);
    }

    else if(IsControl(hWnd))   // for control
    {   
        if(win_info->dwStyle & WS_BORDER) {
            if(win_info->dwStyle & WS_THICKFRAME) {
                draw_3dbox(hdc, &rect, main_3d, 
                       LFRDR_3DBOX_THICKFRAME | LFRDR_BTN_STATUS_PRESSED);
                for(i = 2; i < border + 2; i++) {
                    Rectangle(hdc, rect.left+i, rect.top+i,
                            rect.right-i-1, rect.bottom-i-1);
                }
            }
            else if(win_info->dwStyle & WS_THINFRAME) {
                draw_3dbox(hdc, &rect, main_3d, 
                        LFRDR_BTN_STATUS_PRESSED);
            }
            else {
                draw_3dbox(hdc, &rect, main_3d, 
                        LFRDR_3DBOX_THICKFRAME | LFRDR_BTN_STATUS_PRESSED);
            }
        }
        else if(win_info->dwStyle & WS_THICKFRAME) {
            for(i = 0; i < 2; i++) {
                Rectangle(hdc, rect.left+i, rect.top+i,
                        rect.right-i-1, rect.bottom-i-1);
            }
        }
        else {
            Rectangle(hdc, rect.left, rect.top,
                    rect.right-1, rect.bottom-1);
        }
    }
    
    SetPenColor(hdc, old_pen_color);
    return;
}

/* draw_caption:
 *   This function draw the caption of a window. 
 *
 * param hWnd : the handle of the window.
 * param hdc : the DC of the window.
 * param is_active : whether the window is actived.
 *
 * Author: wangjian<wangjian@minigui.org>
 * Date: 2007-11-22
 */
#define ICON_ORIGIN 2
static void draw_caption (HWND hWnd, HDC hdc, BOOL is_active)
{
    int font_h;
    gal_pixel active_color, inactive_color,
              text_active_color, text_inact_color,
              old_brush_color, old_text_color;
    PLOGFONT cap_font, old_font;
    const WINDOWINFO *win_info = NULL;
    RECT rect;
    RECT icon_rect = {0,0,0,0};
    int  ncbutton_w = 0;
    RECT rcTmp = {0};

    win_info = GetWindowInfo(hWnd);
    
    if (!(win_info->dwStyle & WS_CAPTION))
        return;

    active_color = GetWindowElementPixelEx(hWnd, hdc, WE_BGCA_ACTIVE_CAPTION);
    inactive_color = GetWindowElementPixelEx(hWnd, hdc, WE_BGCA_INACTIVE_CAPTION);
    text_active_color = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_ACTIVE_CAPTION);
    text_inact_color = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_INACTIVE_CAPTION);
    cap_font = (PLOGFONT)GetWindowElementAttr(hWnd, WE_FONT_CAPTION);

    if (calc_we_area(hWnd, HT_CAPTION, &rect) == -1)
        return;
    
    if (is_active)
    {
        old_brush_color = SetBrushColor (hdc, active_color); 
        old_text_color = SetTextColor (hdc, text_active_color); 
    } else { 
        old_brush_color = SetBrushColor (hdc, inactive_color); 
        old_text_color = SetTextColor (hdc, text_inact_color); 
    }
    
    FillBox (hdc, rect.left, rect.top, RECTW(rect),RECTH(rect));
    
    if (win_info->hIcon)
    {
        if(calc_we_area (hWnd, HT_ICON, &icon_rect) != -1)
            DrawIcon (hdc, icon_rect.left, icon_rect.top,
                    RECTW(icon_rect), RECTH(icon_rect), win_info->hIcon);
    }
    
    if (win_info->spCaption)
    {
        SetBkMode(hdc, BM_TRANSPARENT);
        old_font = SelectFont(hdc, cap_font);
        font_h = GetFontHeight(hdc);

        if ((win_info->dwExStyle & WS_EX_NOCLOSEBOX)) {
            calc_we_area(hWnd, HT_CLOSEBUTTON, &rcTmp);
            ncbutton_w += RECTW(rcTmp);
        }
        if (win_info->dwStyle & WS_MAXIMIZEBOX) {
            calc_we_area(hWnd, HT_MAXBUTTON, &rcTmp);
            ncbutton_w += RECTW(rcTmp);
        }

        if (win_info->dwStyle & WS_MINIMIZEBOX) {
            calc_we_area(hWnd, HT_MINBUTTON, &rcTmp);
            ncbutton_w += RECTW(rcTmp);
        }
#if 0
        TextOutOmitted (hdc, rect.left + RECTW(icon_rect) + 
                (ICON_ORIGIN << 1),
                rect.top + ((RECTH(rect)-font_h)>>1),
                win_info->spCaption,
                strlen (win_info->spCaption), rcTmp.left - 
                (rect.left + RECTW(icon_rect)+ (ICON_ORIGIN << 1)));
#endif

        TextOutOmitted (hdc, rect.left + RECTW(icon_rect) + 
                (ICON_ORIGIN << 1),
                rect.top + ((RECTH(rect)-font_h)>>1),
                win_info->spCaption,
                strlen (win_info->spCaption),  
                (RECTW(rect) - RECTW(icon_rect) - (ICON_ORIGIN << 1) - ncbutton_w));

        SelectFont(hdc, old_font);
    }

    SetBrushColor(hdc, old_brush_color);
    SetTextColor(hdc, old_text_color);
    return;
}

/* draw_caption_button:
 *   This function draw the caption button of a window. 
 *
 * param hWnd : the handle of the window.
 * param hdc : the DC of the window.
 * param ht_code : the number for close, max, or min button, 0 for all.
 * param status : the status of the button drawing.
 *
 * Author: wangjian<wangjian@minigui.org>
 * Date: 2007-11-22
 */
static void draw_caption_button (HWND hWnd, HDC hdc, int ht_code, int status)
{
    int i, w, h;
    RECT rect;
    DWORD color_3d;
    gal_pixel fgc_3d, old_col;
        
    color_3d = GetWindowElementAttr(hWnd, WE_MAINC_THREED_BODY);
    fgc_3d = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_THREED_BODY);

    switch(ht_code)
    {
        case 0:                     // draw all the 3 buttons as normal.
            status = LFRDR_BTN_STATUS_NORMAL;
        case HT_CLOSEBUTTON:
            if(calc_we_area(hWnd, HT_CLOSEBUTTON, &rect) != -1)
            {
                draw_3dbox(hdc, &rect, color_3d, 
                        status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);

                if(status == LFRDR_BTN_STATUS_DISABLED)
                    /*FIXME , wangjian ????? disable fg color ???????*/ 
                    old_col = SetPenColor(hdc, fgc_3d);
                else
                    old_col = SetPenColor(hdc, fgc_3d);
                
                w = RECTW(rect);
                h = RECTH(rect);

                rect.left += (w>>2);
                rect.top += (h>>2);
                rect.right -= (w>>2)+1;
                rect.bottom -= (h>>2)+1;

                //for(i = 0; i < (w>>3)+1; i++) // the pen width : w/4-1
                for(i = 0; i < (w>>4)+1; i++) // the pen width : w/4-1
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
                SetPenColor(hdc, old_col);
            } 
            if(ht_code == HT_CLOSEBUTTON) break;

        case HT_MAXBUTTON:
            if(calc_we_area(hWnd, HT_MAXBUTTON, &rect) != -1)
            {
                draw_3dbox(hdc, &rect, color_3d, 
                        status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);

                if(status == LFRDR_BTN_STATUS_DISABLED)
                    /*FIXME , wangjian ????? disable fg color ???????*/ 
                    old_col = SetPenColor(hdc, fgc_3d);
                else
                    old_col = SetPenColor(hdc, fgc_3d);
                
                w = RECTW(rect);
                h = RECTH(rect);
                
                rect.left += (w>>2);
                rect.top += (h>>2);
                rect.right -= (w>>2)+1;
                rect.bottom -= (h>>2)+1;

                for(i = 0; i < (w>>4)+1; i++) // pen width w/16+1.
                {
                    Rectangle(hdc, rect.left + i, rect.top + i,
                            rect.right - i, rect.bottom - i);
                    MoveTo(hdc, rect.left + i, rect.top + i + (w>>4) + 1);
                    LineTo(hdc, rect.right - i, rect.top + i + (w>>4) + 1);
                }
                SetPenColor(hdc, old_col);
            }
            if(ht_code == HT_MAXBUTTON) break;

        case HT_MINBUTTON:
            if(calc_we_area(hWnd, HT_MINBUTTON, &rect) != -1)
            {
                draw_3dbox(hdc, &rect, color_3d, 
                        status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);

                if(status == LFRDR_BTN_STATUS_DISABLED)
                    /*FIXME , wangjian ????? disable fg color ???????*/ 
                    old_col = SetPenColor(hdc, fgc_3d);
                else
                    old_col = SetPenColor(hdc, fgc_3d);
                
                w = RECTW(rect);
                h = RECTH(rect);

                rect.left += (w>>2);
                rect.top += (h>>2);
                rect.right -= (w>>2)+1;
                rect.bottom -= (h>>2)+1;
                
                for(i = 0; i < 2*((w>>4)+1); i++) // pen width (w/16+1)X2.
                {
                    MoveTo(hdc, rect.left, rect.bottom - i - (w>>4) - 1);
                    LineTo(hdc, rect.right, rect.bottom - i - (w>>4) - 1);
                }
                SetPenColor(hdc, old_col);
            }
            if(ht_code == HT_MINBUTTON) break;
            if(ht_code == 0) break;

        default:
            return;
    }
    return;
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
        info = GetWindowInfo (hWnd);
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
 * param hWnd : the handle of the window.
 * param hdc : the DC of the window.
 * param sb_pos : the pos need to draw.
 *
 * Author: wangjian<wangjian@minigui.org>
 * Date: 2007-11-22
 */
static void draw_scrollbar (HWND hWnd, HDC hdc, int sb_pos)
{
    RECT rect;
    int sb_status = 0;
    int bn_status = 0;
    DWORD color_3d, fgc_3d, fgc_dis;
    gal_pixel old_brush_color;
    BOOL isCtrl = FALSE;    /** if TRUE it is scrollbar control else not */
    const WINDOWINFO  *info = 
        (WINDOWINFO*)GetWindowInfo (hWnd);
        
    color_3d = GetWindowElementAttr(hWnd, WE_MAINC_THREED_BODY);
    fgc_3d = GetWindowElementAttr(hWnd, WE_FGC_THREED_BODY);
    fgc_dis = GetWindowElementAttr(hWnd, WE_FGC_DISABLED_ITEM);
    
    if (0 == strncasecmp(CTRL_SCROLLBAR, GetClassName(hWnd), strlen(CTRL_SCROLLBAR)))
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
                   old_brush_color = SetBrushColor(hdc, 
                           RGBA2Pixel(hdc,GetRValue(color_3d),
                               GetGValue(color_3d), GetBValue(color_3d), 
                               GetAValue(color_3d)));
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
                    old_brush_color = SetBrushColor(hdc, 
                            RGBA2Pixel(hdc,GetRValue(color_3d),
                                GetGValue(color_3d), GetBValue(color_3d), 
                                GetAValue(color_3d)));
                    FillBox(hdc, rect.left, rect.top, RECTW(rect), RECTH(rect));
                    SetBrushColor(hdc, old_brush_color);
                }
                draw_scrollbar(hWnd, hdc, HT_SB_LEFTARROW);
                draw_scrollbar(hWnd, hdc, HT_SB_HTHUMB);
                draw_scrollbar(hWnd, hdc, HT_SB_RIGHTARROW);
                break;
            }

        case HT_VSCROLL:         // paint the vscrollbar
            {
                if (!isCtrl || !(info->dwStyle & SBS_NOSHAFT))
                { 
                    old_brush_color = SetBrushColor(hdc, 
                            RGBA2Pixel(hdc,GetRValue(color_3d),
                                GetGValue(color_3d), GetBValue(color_3d), 
                                GetAValue(color_3d)));
                    FillBox(hdc, rect.left, rect.top, RECTW(rect), RECTH(rect));
                    SetBrushColor(hdc, old_brush_color);
                }
                draw_scrollbar(hWnd, hdc, HT_SB_UPARROW);
                draw_scrollbar(hWnd, hdc, HT_SB_VTHUMB);
                draw_scrollbar(hWnd, hdc, HT_SB_DOWNARROW);
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

                draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);
                
                if(sb_status & SBS_DISABLED_LTUP || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, LFRDR_ARROW_LEFT);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, LFRDR_ARROW_LEFT);
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
                
                draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);

                if(sb_status & SBS_DISABLED_BTDN || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, LFRDR_ARROW_RIGHT);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, LFRDR_ARROW_RIGHT);
                break;
            }

        case HT_SB_HTHUMB:
            { 
                sb_status = get_scroll_status(hWnd, FALSE);
                if(sb_status & SBS_PRESSED_THUMB)
                    bn_status |= LFRDR_BTN_STATUS_PRESSED;
                else if(sb_status & SBS_HILITE_THUMB)
                    bn_status |= LFRDR_BTN_STATUS_HILITE;
                 
                draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);
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

                draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);

                if(sb_status & SBS_DISABLED_LTUP || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, LFRDR_ARROW_UP);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, LFRDR_ARROW_UP);
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
                
                draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);

                if(sb_status & SBS_DISABLED_BTDN || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, LFRDR_ARROW_DOWN);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, LFRDR_ARROW_DOWN);
                break;
            }

        case HT_SB_VTHUMB: 
            {
                sb_status = get_scroll_status(hWnd, TRUE);
                if(sb_status & SBS_PRESSED_THUMB)
                    bn_status |= LFRDR_BTN_STATUS_PRESSED;
                else if(sb_status & SBS_HILITE_THUMB)
                    bn_status |= LFRDR_BTN_STATUS_HILITE;
                
                draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);
                break;
            } 
        default:
            return;
    }
}

/*
 * draw_trackbar_thumb
 *    
 * This function draw thumb of trackbar
 *
 * Author : zhounuohua<zhounuohua@minigi.com>
 * Data   : 2007-12-18
 */
static void draw_trackbar_thumb (HWND hWnd, HDC hdc, 
        const RECT* pRect, DWORD dwStyle)
{
    /** trackbar status , pressed or hilite */
    int tbstatus = 0, sliderx, slidery, sliderw, sliderh;
    DWORD   color_thumb;
    RECT    rc_draw;

    color_thumb = GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY);

    /** leave little margin */
    if (dwStyle & TBS_VERTICAL) {
        rc_draw.left   = pRect->left;
        rc_draw.top    = pRect->top + 2;
        rc_draw.right  = pRect->right - 1;
        rc_draw.bottom = pRect->bottom - 2;
    }
    else{
        rc_draw.left   = pRect->left + 2;
        rc_draw.top    = pRect->top;
        rc_draw.right  = pRect->right - 2;
        rc_draw.bottom = pRect->bottom - 1;
    }

    sliderx = pRect->left;
    slidery = pRect->top;
    sliderw = RECTWP(pRect);
    sliderh = RECTHP(pRect);

    tbstatus = LFRDR_3DBOX_FILLED | LFRDR_BTN_STATUS_NORMAL;

    /** the two states are mutex, high priority for LFRDR_TBS_PRESSED */
    if (dwStyle & LFRDR_TBS_PRESSED)
        tbstatus |= LFRDR_BTN_STATUS_PRESSED;
    else if (dwStyle & LFRDR_TBS_HILITE)
        tbstatus |= LFRDR_BTN_STATUS_HILITE;

    GetWindowInfo(hWnd)->we_rdr->draw_3dbox (hdc, &rc_draw, color_thumb, tbstatus);

    SetPenColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_THREED_BODY));
    if (dwStyle & TBS_VERTICAL) {
        MoveTo (hdc, sliderx + (sliderw >> 1) - 3 - 1, 
                slidery + (sliderh >> 1));
        LineTo (hdc, sliderx + (sliderw >> 1) + 3 - 1, 
                slidery + (sliderh >> 1));
        MoveTo (hdc, sliderx + (sliderw >> 1) - 2 - 1, 
                slidery + (sliderh >> 1) - 2);
        LineTo (hdc, sliderx + (sliderw >> 1) + 2 - 1, 
                slidery + (sliderh >> 1) - 2);
        MoveTo (hdc, sliderx + (sliderw >> 1) - 2 - 1, 
                slidery + (sliderh >> 1) + 2);
        LineTo (hdc, sliderx + (sliderw >> 1) + 2 - 1, 
                slidery + (sliderh >> 1) + 2);
    }
    else {
        MoveTo (hdc, sliderx + (sliderw >> 1), 
                slidery + (sliderh >> 1) - 3 - 1);
        LineTo (hdc, sliderx + (sliderw >> 1), 
                slidery + (sliderh >> 1) + 3 - 1);
        MoveTo (hdc, sliderx + (sliderw >> 1) - 2, 
                slidery + (sliderh >> 1) - 2 - 1);
        LineTo (hdc, sliderx + (sliderw >> 1) - 2, 
                slidery + (sliderh >> 1) + 2 - 1);
        MoveTo (hdc, sliderx + (sliderw >> 1) + 2, 
                slidery + (sliderh >> 1) - 2 - 1);
        LineTo (hdc, sliderx + (sliderw >> 1) + 2, 
                slidery + (sliderh >> 1) + 2 - 1);
    }
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
            slidery = y + (HEIGHT_VERT_SLIDER>>1) + fixtoi(fixmul(itofix(h - HEIGHT_VERT_SLIDER), fixdiv (itofix (max - pos), itofix (max - min)))) - (sliderh>>1);
            //slidery = y + (HEIGHT_VERT_SLIDER>>1)+ (int)((max - pos) * 
            //        (h - HEIGHT_VERT_SLIDER) / (float)(max - min)) - (sliderh>>1);
        }
        else {
            slidery = y + ((h - sliderh) >> 1); 
            sliderx = x + (WIDTH_HORZ_SLIDER >> 1) + fixtoi (fixmul (itofix (w - WIDTH_HORZ_SLIDER), fixdiv (itofix (pos - min), itofix (max - min)))) - (sliderw>>1);
            //sliderx = x + (WIDTH_HORZ_SLIDER >> 1) + (int)((pos - min) * 
            //        (w - WIDTH_HORZ_SLIDER) / (float)(max - min)) - (sliderw>>1);
        }

        SetRect (rcBar, sliderx, slidery, sliderx + sliderw, slidery + sliderh);
    }
}

static void 
draw_trackbar (HWND hWnd, HDC hdc, LFRDR_TRACKBARINFO *info)
{
    RECT    rc_client, rc_border, rc_ruler, rc_bar, rc_draw;
    int     x, y, w, h;
    int     max, min;
    int     TickFreq;
    int     TickStart, TickEnd;
    int     sliderw, sliderh;
    fixed fTickGap;
    fixed fTick;
    //float   TickGap, Tick;
    DWORD   bgc, light_dword, dwStyle;

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

    /* draw the rulder in middle of trackbar with renderer . */
    rc_draw.left   = rc_ruler.left;
    rc_draw.top    = rc_ruler.top;
    rc_draw.right  = rc_ruler.right - 1;
    rc_draw.bottom = rc_ruler.bottom - 1;
    draw_3dbox (hdc, &rc_draw, bgc, LFRDR_BTN_STATUS_PRESSED);

    max = info->nMax;
    min = info->nMin;
    sliderw = RECTW(rc_bar);
    sliderh = RECTH(rc_bar);

    /* draw the tick of trackbar. */
    if (!(dwStyle & TBS_NOTICK)) {
        SetPenColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_THREED_BODY));
        if (dwStyle & TBS_VERTICAL) {
            TickStart = y + (HEIGHT_VERT_SLIDER >> 1);
            fTickGap = itofix (h - HEIGHT_VERT_SLIDER);
            fTickGap = fixmul (fTickGap, fixdiv (itofix (TickFreq), itofix (max - min)));
            //TickGap = (h - HEIGHT_VERT_SLIDER) / (float)(max - min) * TickFreq;
            TickEnd = y + h - (HEIGHT_VERT_SLIDER >> 1);
#if 1        
            for (fTick = itofix (TickStart); (int)(fixtof(fTick)) <= TickEnd; fTick = fixadd (fTick, fTickGap) ) {
                MoveTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER, (int)(fixtof (fTick)));
                LineTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER + LEN_TICK, (int)(fixtof (fTick)));
            }
            if ((int) (fixtof (fixadd (fixsub (fTick, fTickGap), ftofix (0.9)))) < TickEnd) {
                MoveTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER, TickEnd);
                LineTo (hdc, x + (w>>1) + (sliderw>>1) + GAP_TICK_SLIDER + LEN_TICK, TickEnd);
            }
#endif
        } else {

            TickStart = x + (WIDTH_HORZ_SLIDER >> 1); 
            fTickGap = fixmul (itofix (w - WIDTH_HORZ_SLIDER), fixdiv (itofix (TickFreq), itofix (max - min)));

            TickEnd = x + w - (WIDTH_HORZ_SLIDER >> 1);
            for (fTick = itofix (TickStart); (int)(fixtof(fTick)) <= TickEnd; fTick = fixadd (fTick, fTickGap) ) {
                MoveTo (hdc, (int)(fixtof (fTick)), y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER);
                LineTo (hdc, (int)(fixtof (fTick)), y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER + LEN_TICK);
            }

            if ((int) (fixtof (fixadd (fixsub (fTick, fTickGap), ftofix (0.9)))) < TickEnd)
            {
                MoveTo (hdc, TickEnd, y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER);
                LineTo (hdc, TickEnd, y + (h>>1) + (sliderh>>1) + GAP_TICK_SLIDER + LEN_TICK);
            }
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

/* 
 * disabled_text_out:
 *
 * This function outputs disabled text.
 *
 * Author: XiaoweiYan
 * Date: 2007-12-04
 */

static void 
disabled_text_out (HWND hWnd, HDC hdc, const char* spText, PRECT rc, DWORD dt_fmt)
{
    DWORD mainc, color;
    
    SetBkMode (hdc, BM_TRANSPARENT);
    SetBkColor (hdc, GetWindowBkColor (hWnd));

    rc->left+=1;
    rc->top+=1;
    rc->right+=1;
    rc->bottom+=1;

    mainc = GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY);
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
 *
 * This function draw a propsheet tab. 
 *
 * Author: Peng LIU
 * Date: 2007-12-07
 */
static void 
draw_tab (HWND hWnd, HDC hdc, RECT *rect, char *title, DWORD color, int flag, HICON icon)
{
    int x, ty, by, text_extent;
    SIZE size;
    int eff_chars, eff_len;
    DWORD light_c, darker_c, darkest_c;
   
    light_c = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_LIGHTEST);
    darker_c = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_DARKER);
    darkest_c = calc_3dbox_color (color, LFRDR_3DBOX_COLOR_DARKEST);

    x = rect->left + 2;
    ty = rect->top;

    if (!(flag & LFRDR_TAB_ACTIVE)) {
        if (flag & LFRDR_TAB_BOTTOM) {
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
        SetPenColor(hdc, DWORD2Pixel (hdc, light_c)); 
        MoveTo (hdc, rect->left, ty);
        LineTo (hdc, rect->left, by - 3 );
        SetPenColor(hdc, DWORD2Pixel (hdc, darker_c)); 
        MoveTo (hdc, rect->left + 2, by - 1);
        LineTo (hdc, rect->right - 4, by - 1);

        MoveTo (hdc, rect->left + 1, by - 2);
        LineTo (hdc, rect->left + 1, by - 2);
 
    } else {
        SetPenColor(hdc, DWORD2Pixel (hdc, light_c)); 
        MoveTo (hdc, rect->left, by - 1);
        LineTo (hdc, rect->left, ty + 2);
        MoveTo (hdc, rect->left + 2, ty);
        LineTo (hdc, rect->right - 4, ty);

        MoveTo (hdc, rect->left + 1, ty + 1);
        LineTo (hdc, rect->left + 1, ty + 1);
    }

    if (flag & LFRDR_TAB_BOTTOM) {
        SetPenColor(hdc, DWORD2Pixel (hdc, darkest_c)); 
        MoveTo (hdc, rect->right - 2, by - 3);
        LineTo (hdc, rect->right - 2, ty);

        MoveTo (hdc, rect->right - 3, by - 2 );
        LineTo (hdc, rect->right - 3, by - 2);

        SetPenColor(hdc, DWORD2Pixel (hdc, darker_c)); 
        MoveTo (hdc, rect->right - 3, by -3);
        LineTo (hdc, rect->right - 3, ty);
 
    }else {
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

    return;
}

static void 
draw_progress (HWND hWnd, HDC hdc, 
        int nMax, int nMin, int nPos, BOOL fVertical)
{
    RECT    rcClient;
    int     x, y, w, h;
    ldiv_t   ndiv_progress;
    unsigned int     nAllPart;
    unsigned int     nNowPart;
    int     whOne, nRem;
    int     ix, iy;
    int     i;
    int     step;
    int pbar_border = 2;
    gal_pixel old_color;

    if (nMax == nMin)
        return;
    
    if ((nMax - nMin) > 5)
        step = 5;
    else
        step = 1;

    GetClientRect (hWnd, &rcClient);
    draw_3dbox (hdc, &rcClient, 
        GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY),
                    LFRDR_BTN_STATUS_PRESSED);

    x = rcClient.left + pbar_border;
    y = rcClient.top + pbar_border;
    w = RECTW (rcClient) - (pbar_border << 1);
    h = RECTH (rcClient) - (pbar_border << 1);

    if (hWnd != HWND_NULL)
        old_color = SetBrushColor (hdc, GetWindowBkColor (hWnd));
    else
        old_color = SetBrushColor (hdc, 
                GetWindowElementPixel (HWND_DESKTOP, WE_BGC_DESKTOP));

    //draw the erase background
    FillBox (hdc, rcClient.left + 1, rcClient.top + 1, RECTW (rcClient) - 2, RECTH (rcClient) - 2);
    SetPenColor(hdc, old_color);

    ndiv_progress = ldiv (nMax - nMin, step);
    nAllPart = ndiv_progress.quot;
    
    ndiv_progress = ldiv (nPos - nMin, step);
    nNowPart = ndiv_progress.quot;
    if (fVertical)
        ndiv_progress = ldiv (h, nAllPart);
    else
        ndiv_progress = ldiv (w, nAllPart);
        
    whOne = ndiv_progress.quot;
    nRem = ndiv_progress.rem;

    SetBrushColor (hdc, 
            GetWindowElementPixel (hWnd, WE_BGC_HIGHLIGHT_ITEM));
 
    if (whOne >= 4) {
        if (fVertical) {
            for (i = 0, iy = y + h - 1; i < nNowPart; ++i) {
                if ((iy - whOne) < y) 
                    whOne = iy - y;

                FillBox (hdc, x + 1, iy - whOne, w - 2, whOne - 1);
                iy -= whOne;
                if(nRem > 0) {
                    iy --;
                    nRem --;
                }
            }
        }
        else {
            for (i = 0, ix = x + 1; i < nNowPart; ++i) {
                if ((ix + whOne) > (x + w)) 
                    whOne = x + w - ix;

                FillBox (hdc, ix, y + 1, whOne - 1, h - 2);
                ix += whOne;
                if(nRem > 0) {
                    ix ++;
                    nRem --;
                }
            }
        }
    }
    else {
        // no vertical support

        if (fVertical) {
            int prog = h * nNowPart/nAllPart;

            FillBox (hdc, x + 1, rcClient.bottom - pbar_border - prog, 
                    w - 2, prog);
        }
        else {
            char szText[8];
            SIZE text_ext;
            RECT rc_clip = rcClient;
            int prog = w * nNowPart/nAllPart;

            FillBox (hdc, x, y + 1, prog, h - 2);

            SetBkMode (hdc, BM_TRANSPARENT);
            sprintf (szText, "%d%%", (nNowPart*100/nAllPart));
            GetTextExtent (hdc, szText, -1, &text_ext);
            x += ((w - text_ext.cx) >> 1);
            y += ((h - text_ext.cy) >> 1);

            rc_clip.right = prog;
            SelectClipRect (hdc, &rc_clip);
            SetTextColor (hdc, 
                    GetWindowElementPixel (hWnd, WE_FGC_HIGHLIGHT_ITEM));
            SetBkColor (hdc, 
                    GetWindowElementPixel (hWnd, WE_BGC_HIGHLIGHT_ITEM));
            TextOut (hdc, x, y, szText);

            rc_clip.right = rcClient.right;
            rc_clip.left = prog;
            SelectClipRect (hdc, &rc_clip);
            SetTextColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_WINDOW));
            SetBkColor (hdc, GetWindowBkColor (hWnd));
            TextOut (hdc, x, y, szText);
        }
    }
}

static void draw_header (HWND hWnd, HDC hdc, const RECT *pRect, DWORD color)
{
    DWORD c;
    if(color ==0 )
        c = GetWindowElementAttr(HWND_NULL, WE_MAINC_THREED_BODY);
    else
        c = color;
    draw_3dbox(hdc, pRect, c, LFRDR_3DBOX_FILLED);
}

WINDOW_ELEMENT_RENDERER __mg_wnd_rdr_classic = {
    "classic",
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

