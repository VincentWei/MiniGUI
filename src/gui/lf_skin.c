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

#ifdef _MGLF_RDR_SKIN
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
#include "cursor.h"
#include "icon.h"
#include "sysres.h"
#include "fixedmath.h"

#include "lf_common.h"

#define BTN_STATUS_NUM      8
#define CAP_BTN_INTERVAL    5


#define LFSKIN_FILL_TILE    0x01
#define LFSKIN_FILL_STRETCH 0x02
extern BOOL wndGetVScrollBarRect (const MAINWIN* pWin, RECT* rcVBar);
extern BOOL wndGetHScrollBarRect (const MAINWIN* pWin, RECT* rcHBar);

/* Bitmap info */
typedef struct _LFSKIN_BMPINFO
{
    /* The pointer to the whole bitmap. */
    const BITMAP* bmp;

    /* The number of line in this bitmap. */
    unsigned int nr_line;

    /* The number of colume in this bitmap. */
    unsigned int nr_col;

    /* The index of line to current sub bitmap. */
    unsigned int idx_line;

    /* The index of colume to current sub bitmap. */
    unsigned int idx_col;

    /* The left/top margin value of current sub bitmap. */
    unsigned int margin1;

    /* The right/bottom margin value of current sub bitmap. */
    unsigned int margin2;

    /* The margin direct. FALSE for left-to-right (default),
     * TRUE for top-to-bottom. */
    BOOL direct;

    /* The flip flag. FALSE for not flip sub bitmap (default),
     * TRUE for flip. It can be used by draw_tab function.*/
    BOOL flip;

    /* The style of drawing bitmap. It can be one of the
     * following value:
     *  - LFSKIN_FILL_TILE
     *  - LFSKIN_FILL_STRETCH
     */
    unsigned int style;

} LFSKIN_BMPINFO;

WINDOW_ELEMENT_RENDERER __mg_wnd_rdr_skin;

static const char* szSkinKeyNames[WE_LFSKIN_NUMBER] = {
    "skin_bkgnd",
    "skin_caption",
    "skin_caption_btn",

    "skin_scrollbar_vshaft",
    "skin_scrollbar_vthumb",
    "skin_scrollbar_hshaft",
    "skin_scrollbar_hthumb",
    "skin_scrollbar_arrows",

    "skin_tborder",
    "skin_bborder",
    "skin_lborder",
    "skin_rborder",

    "skin_arrows",
    "skin_arrows_shell",

    "skin_radiobtn",
    "skin_checkbtn",
    "skin_pushbtn",

    "skin_tree",

    "skin_header",
    "skin_tab",

    "skin_progressbar_hchunk",
    "skin_progressbar_vchunk",
    "skin_progressbar_htrack",
    "skin_progressbar_vtrack",

    "skin_tbslider_h",
    "skin_trackbar_vert",
    "skin_trackbar_horz",
    "skin_tbslider_v",
};

typedef struct _SKINRESINFO
{
    RES_KEY skin_data [WE_LFSKIN_NUMBER];
} SKINRESINFO;

#define IS_VALID(value) (strncmp (value, "none", 5) != 0)

static BOOL init_skin_def_data (WINDOW_ELEMENT_RENDERER* rdr,SKINRESINFO *info)
{
    int i;
    char bmpfile[MAX_NAME+1] = "bmp/";
    char* buff;
    buff = bmpfile + strlen(bmpfile);
    /*get picture filenames or keys*/
    for (i = 0; i < WE_LFSKIN_NUMBER; i++) {
        /*adjust picture from Etc*/
        if (szSkinKeyNames[i]) {
            if (GetMgEtcValue (rdr->name, szSkinKeyNames[i],
                        buff, sizeof(bmpfile)-(buff-bmpfile)) != ETC_OK) {
                fprintf (stderr,
                        "GUI>Skin LFRDR: Cann't get info of key %s from etc.\n",
                        szSkinKeyNames[i]);
            }
        }

        /* Add when refactoring resource manage */
        if (IS_VALID(bmpfile) &&
                (LoadResource (bmpfile, RES_TYPE_IMAGE, (DWORD)HDC_SCREEN) == NULL) ){
            fprintf(stderr, "GUI>Skin LFRDR: Register skin data error: %s.\n",
                    buff);
            return FALSE;
        }
        info->skin_data [i] = Str2Key(bmpfile);
    }

    return TRUE;
}

static void deinit_skin_def_data (SKINRESINFO* info)
{
    int i;
    /*unregister picture*/
    for (i = 0; i < WE_LFSKIN_NUMBER; i++) {
        ReleaseRes(info->skin_data[i]);
    }
}

/*initialize and terminate interface*/
static int init (PWERENDERER renderer)
{
    SKINRESINFO *info;

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

    if (!(info = calloc (1, sizeof(SKINRESINFO))))
        return -1;

    init_skin_def_data (renderer,info);

    gui_LoadIconRes(HDC_SCREEN, renderer->name, (char*)SYSICON_TREEFOLD);
    gui_LoadIconRes(HDC_SCREEN, renderer->name, (char*)SYSICON_TREEUNFOLD);

    renderer->private_info = (void*)info;
    return 0;
}

static int deinit (PWERENDERER renderer)
{
    SKINRESINFO* info =
        (SKINRESINFO*) renderer->private_info;

    /* Destroy system icon. */
    TermRendererSystemIcon (renderer->we_icon[0], renderer->we_icon[1]);
    deinit_skin_def_data (info);

    gui_UnLoadIconRes(HDC_SCREEN, renderer->name, (char*)SYSICON_TREEFOLD);
    gui_UnLoadIconRes(HDC_SCREEN, renderer->name, (char*)SYSICON_TREEUNFOLD);

    free (info);
    renderer->private_info = NULL;
    return 0;
}

static DWORD on_set_rdr_attr (WINDOW_ELEMENT_RENDERER* rdr,int we_attr_id, DWORD we_attr, BOOL trans)
{
    int index;
    RES_KEY key;
    DWORD old_attr;
    SKINRESINFO *info = (SKINRESINFO *)rdr->private_info;


    if ((we_attr_id & WE_ATTR_TYPE_MASK) == WE_ATTR_TYPE_RDR) {
        index = we_attr_id & WE_ATTR_INDEX_MASK;
        if (index < WE_LFSKIN_NUMBER) {
            if ( !trans) {
                key = Str2Key((const char*)we_attr);
                return (DWORD)key;;
            } else {
                old_attr = (DWORD) info->skin_data [index];
                info->skin_data [index] = Str2Key((const char*)we_attr);
                return old_attr;
            }
        }
    }

    return 0;
}

static DWORD on_get_rdr_attr (WINDOW_ELEMENT_RENDERER* rdr, int we_attr_id)
{
    int index;
    SKINRESINFO *info = (SKINRESINFO *)rdr->private_info;

    if ((we_attr_id & WE_ATTR_TYPE_MASK) == WE_ATTR_TYPE_RDR) {
        index = we_attr_id & WE_ATTR_INDEX_MASK;
        if (index < WE_LFSKIN_NUMBER) {
            return (DWORD)info->skin_data [index];
        }
    }

    return 0;
}

static void unload_sub_bitmap (PBITMAP sub_bmp)
{
    if (NULL != sub_bmp)
    {
        UnloadBitmap (sub_bmp);
        free (sub_bmp);
        sub_bmp = NULL;
    }
}

static PBITMAP get_sub_bitmap (HDC hdc, const LFSKIN_BMPINFO *bmp_info)
{
    Uint8   *start_bits;
    int     bpp, pitch, i, w, h;
    PBITMAP sub_bmp = NULL;
    const BITMAP *full_bmp = NULL;

    if (NULL == (sub_bmp = (PBITMAP)calloc (1, sizeof(BITMAP)))){
        fprintf (stderr, "no memory for sub bmp in skin LF.");
        return NULL;
    }

    full_bmp = bmp_info->bmp;
    memcpy (sub_bmp, full_bmp, sizeof(BITMAP));

    w     = full_bmp->bmWidth / bmp_info->nr_col;
    h     = full_bmp->bmHeight / bmp_info->nr_line;
    bpp   = full_bmp->bmBytesPerPixel;
    pitch = full_bmp->bmPitch / bmp_info->nr_col;

    /*initialize private info*/
    sub_bmp->bmWidth = w;
    sub_bmp->bmHeight = h;
    sub_bmp->bmPitch = pitch;

    if (NULL == (sub_bmp->bmBits = calloc (1, pitch*h))){
        fprintf (stderr, "no memory for sub bmp in skin LF.");
        return NULL;
    }

    start_bits = full_bmp->bmBits
        + h * bmp_info->idx_line * full_bmp->bmPitch
        + w * bmp_info->idx_col * bpp;

    for (i = 0; i < h; i++) {
        memcpy (sub_bmp->bmBits + pitch*i, start_bits, w*bpp);
        start_bits += full_bmp->bmPitch;
    }
    if (sub_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
        pitch = ((w+3) & ~3);
        sub_bmp->bmAlphaMask = calloc(1, h * pitch);
        sub_bmp->bmAlphaPitch = pitch;
        start_bits = full_bmp->bmAlphaMask
            + h * bmp_info->idx_line * full_bmp->bmAlphaPitch
            + w * bmp_info->idx_col;

        for (i = 0; i < h; i++) {
            memcpy (sub_bmp->bmAlphaMask + pitch*i, start_bits, w);
            start_bits += full_bmp->bmAlphaPitch;
        }
    }

    if (bmp_info->flip) {
        VFlipBitmap (sub_bmp, sub_bmp->bmBits);
    }

    return sub_bmp;
}

    static void
draw_area_from_bitmap (HDC hdc, const RECT* rc, const LFSKIN_BMPINFO *bmp_info,
        BOOL do_clip)
{
    PBITMAP  part_bmp;
    int     margin1 = bmp_info->margin1;
    int     margin2 = bmp_info->margin2;
    BOOL    sub_bmp = FALSE;
    const BITMAP* bmp = bmp_info->bmp;

    int subw = 0, subh = 0;
    int new_left = 0, new_top=  0;

    if (0 != bmp_info->nr_col)
        subw = bmp->bmWidth / bmp_info->nr_col;
    if (0 != bmp_info->nr_line)
        subh = bmp->bmHeight / bmp_info->nr_line;

    if (bmp_info->nr_col > 1 || bmp_info->nr_line > 1)
        sub_bmp = TRUE;

    if (do_clip)
        SelectClipRect (hdc, rc);

    if (sub_bmp) {
        part_bmp = get_sub_bitmap (hdc, bmp_info);
    } else {
        part_bmp = (PBITMAP)bmp;
    }

    if (!(bmp_info->direct)) {

        if (bmp_info->style == LFSKIN_FILL_STRETCH) {
            gui_fill_box_with_bitmap_except_incompatible (hdc, rc->left, rc->top,
                    RECTWP(rc), RECTHP(rc), part_bmp);
        }
        else {/*tile*/
            /*draw left*/
            if (margin1)
                gui_fill_box_with_bitmap_part_except_incompatible(hdc, rc->left, rc->top,
                        bmp_info->margin1, RECTHP(rc),
                        0, RECTHP(rc), part_bmp, 0, 0);

            /*draw center*/
            new_left = rc->left+margin1;

            do {
                if ((new_left + subw - margin1 - margin2) <= rc->right - margin2)
                    gui_fill_box_with_bitmap_part_except_incompatible(hdc, new_left, rc->top,
                            subw-margin1-margin2, RECTHP(rc),
                            0, RECTHP(rc), part_bmp, margin1, 0);
                else
                    gui_fill_box_with_bitmap_part_except_incompatible(hdc, new_left, rc->top,
                            rc->right - margin2 - new_left, RECTHP(rc),
                            0, RECTHP(rc), part_bmp, margin1, 0);

                new_left += subw - margin1 - margin2;
            } while (new_left < rc->right-margin2);

            /*draw right*/
            if (margin2)
                gui_fill_box_with_bitmap_part_except_incompatible(hdc, rc->right-bmp_info->margin2, rc->top,
                        bmp_info->margin2, RECTHP(rc),
                        0, RECTHP(rc), part_bmp, (subw -margin2), 0);
        }
    }
    else {
        if (bmp_info->style == LFSKIN_FILL_STRETCH) {
            gui_fill_box_with_bitmap_except_incompatible (hdc, rc->left, rc->top,
                    RECTWP(rc), RECTHP(rc), part_bmp);
        }
        else {/*tile*/
            /*draw up*/
            if (margin1)
                gui_fill_box_with_bitmap_part_except_incompatible(hdc, rc->left, rc->top,
                        RECTWP(rc), bmp_info->margin1,
                        RECTWP(rc), 0, part_bmp, 0, 0);

            /*draw center*/
            new_top = rc->top+margin1;

            do {
                if (new_top + subh - margin1 - margin2 <= rc->bottom - margin2)
                    gui_fill_box_with_bitmap_part_except_incompatible(hdc, rc->left, new_top,
                            RECTWP(rc), subh - margin1 - margin2,
                            RECTWP(rc), 0, part_bmp, 0, margin1);
                else
                    gui_fill_box_with_bitmap_part_except_incompatible(hdc, rc->left, new_top,
                            RECTWP(rc), rc->bottom - margin2 - new_top,
                            RECTWP(rc), 0, part_bmp, 0, margin1);

                new_top += subh-margin1-margin2;
            } while (new_top < rc->bottom-margin2);

            /*draw down*/
            if (margin2)
                gui_fill_box_with_bitmap_part_except_incompatible(hdc, rc->left, rc->bottom-bmp_info->margin2,
                        RECTWP(rc), margin2, RECTWP(rc), 0,
                        part_bmp, 0, subh -margin2);
        }
    }

    if (sub_bmp)
        unload_sub_bitmap (part_bmp);

}

static float minrgb (float r, float g, float b)
{
    float min;

    if (r < g)
        min = r;
    else
        min = g;
    if (b < min)
        min = b;

    return min;
}
static float maxrgb (float r, float g, float b)
{
    float max;

    if (r > g)
        max = r;
    else
        max = g;
    if (b > max)
        max = b;

    return max;
}

    static void
rgb2hsva (float r, float g, float b,
        float *hout, float *sout, float *vout)
{
    float h=0, s=1.0, v=1.0;
    float max_v, min_v, diff, r_dist, g_dist, b_dist;
    float undefined = 0.0;

    max_v = maxrgb (r,g,b);
    min_v = minrgb (r,g,b);
    diff = max_v - min_v;
    v = max_v;

    if (max_v != 0)
        s = diff/max_v;
    else
        s = 0.0;
    if (s == 0)
        h = undefined;
    else {
        r_dist = (max_v - r)/diff;
        g_dist = (max_v - g)/diff;
        b_dist = (max_v - b)/diff;
        if (r == max_v)
            h = b_dist - g_dist;
        else
            if (g == max_v)
                h = 2 + r_dist - b_dist;
            else {
                if (b == max_v)
                    h = 4 + g_dist - r_dist;
                else
                    printf ("rgb2hsv: How did I get here?\n");
            }

        h *= 60;
        if (h < 0)
            h += 360.0;
    }

    *hout = h;
    *sout = s;
    *vout = v;
}

/*
   Assumes (r,g,b) range from 0 to 255
   Sets h in degrees: 0 to 360. s,v in [0,255]
   */
    static void
RGB2HSV (Uint8 r, Uint8 g, Uint8 b,
        Uint16 *hout, Uint8 *sout, Uint8 *vout)
{
    float h, s, v;

    rgb2hsva (r/255., g/255., b/255., &h, &s, &v);

    *hout = h;
    *sout = 255 * s;
    *vout = 255 * v;
}

    static void
HSV2RGB (Uint16 hin, Uint8 sin, Uint8 vin,
        Uint8 *rout, Uint8 *gout, Uint8 *bout)
{
    float h, s, v;
    float r = 0, g = 0, b = 0;
    float f, p, q, t;
    int i;

    h = hin;
    s = sin/255.;
    v = vin/255.;

    if (sin == 0 ) {
        r = v;
        g = v;
        b = v;
    }
    else {
        if (hin == 360)
            h = 0.0;
        h /= 60.;
        i = (int) h;
        f = h - i;
        p = v*(1-s);
        q = v*(1-(s*f));
        t = v*(1-s*(1-f));

        switch (i) {
            case 0:
                r = v;
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = v;
                b = p;
                break;
            case 2:
                r = p;
                g = v;
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = v;
                break;
            case 4:
                r = t;
                g = p;
                b = v;
                break;
            case 5:
                r = v;
                g = p;
                b = q;
                break;
            default:
                r = 1.0;
                b = 1.0;
                b = 1.0;
                break;
        }
    }

    *rout = 255*r;
    *gout = 255*g;
    *bout = 255*b;
}

static DWORD _calc_color (DWORD color, int flag)
{
    Uint16 _h;
    Uint8 _s, _v, _r, _g, _b, _a, tmp;

    _r = GetRValue (color);
    _g = GetGValue (color);
    _b = GetBValue (color);
    _a = GetAValue (color);

    RGB2HSV (_r, _g, _b, &_h, &_s, &_v);

    switch (flag) {
        case LFRDR_3DBOX_COLOR_DARKER:
            tmp = ABS(_v - 113)/2;

            if (_v > tmp)
                _v -= tmp;
            else
                _v = 0;
            break;

        case LFRDR_3DBOX_COLOR_LIGHTER:
            tmp = (255 - _v)/2;

            if (255 - tmp > _v)
                _v += tmp;
            else
                _v = 255;
            break;
    }

    HSV2RGB (_h, _s, _v, &_r, &_g, &_b);

    return MakeRGBA (_r, _g, _b, _a);
}

static DWORD calc_3dbox_color (DWORD color, int flag)
{
    switch (flag) {
        case LFRDR_3DBOX_COLOR_DARKEST:
            return 0xFF646F71;

        case LFRDR_3DBOX_COLOR_LIGHTEST:
            return 0xFFFFFFFF;
        case LFRDR_3DBOX_COLOR_DARKER:
        case LFRDR_3DBOX_COLOR_LIGHTER:
            return _calc_color (color, flag);
    }
    return 0;
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

    static void
draw_checkmark (HDC hdc, const RECT* pRect, DWORD color, int status)
{
    int i, w, h, side_len, boundary;
    int box_l, box_t, box_r, box_b;
    int hook_l, hook_t, hook_r, hook_b;
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
    int offset_d, offset_s = 0;
    const BITMAP *bmp_arrow;
    const BITMAP *bmp_arrow_s;
    DWORD file_arrow, file_arrow_s;
    LFSKIN_BMPINFO bmp_info;
    RECT rc_arrow;

    file_arrow = GetWindowElementAttr (hWnd, WE_LFSKIN_ARROWS);
    file_arrow_s = GetWindowElementAttr (hWnd, WE_LFSKIN_ARROWS_SHELL);

    bmp_arrow_s = GetBitmapFromRes (file_arrow_s);

    if (bmp_arrow_s && (status & LFRDR_ARROW_HAVESHELL))
    {
        bmp_info.bmp        = bmp_arrow_s;
        bmp_info.nr_line    = 4;
        bmp_info.nr_col     = 1;
        bmp_info.idx_col    = 0;
        bmp_info.idx_line   = 0;
        bmp_info.margin1    = 0;
        bmp_info.margin2    = 0;
        bmp_info.direct     = FALSE;
        bmp_info.flip       = FALSE;
        bmp_info.style      = LFSKIN_FILL_STRETCH;

        switch (status & LFRDR_BTN_STATUS_MASK) {
            case LFRDR_BTN_STATUS_HILITE:
                bmp_info.idx_line = 1;
                break;
            case LFRDR_BTN_STATUS_PRESSED:
                bmp_info.idx_line = 2;
                break;
            case LFRDR_BTN_STATUS_DISABLED:
                bmp_info.idx_line = 3;
                break;
            default:
                break;
        }

        draw_area_from_bitmap (hdc, pRect, &bmp_info, FALSE);
    }

    /*draw arrow*/
    bmp_arrow = GetBitmapFromRes (file_arrow);

    if (bmp_arrow){

        if (RECTWP(pRect) <= bmp_arrow->bmWidth)
            rc_arrow.left = pRect->left;
        else
            rc_arrow.left = pRect->left+((RECTWP(pRect) - bmp_arrow->bmWidth)>>1);

        if (RECTHP(pRect) <= (bmp_arrow->bmHeight>>4))
            rc_arrow.top = pRect->top;
        else
            rc_arrow.top = pRect->top+((RECTHP(pRect) - (bmp_arrow->bmHeight>>4))>>1);

        rc_arrow.right = rc_arrow.left + bmp_arrow->bmWidth;
        rc_arrow.bottom = rc_arrow.top + (bmp_arrow->bmHeight>>4);

        switch(status & LFRDR_ARROW_DIRECT_MASK)
        {
            case LFRDR_ARROW_UP:
                offset_d = 0;
                break;
            case LFRDR_ARROW_DOWN:
                offset_d = 1;
                break;
            case LFRDR_ARROW_LEFT:
                offset_d = 2;
                break;
            case LFRDR_ARROW_RIGHT:
                offset_d = 3;
                break;
            default :
                offset_d = 0;
                break;
        }

        switch (status & LFRDR_BTN_STATUS_MASK) {
            case LFRDR_BTN_STATUS_NORMAL:
                offset_s = 0;
                break;
            case LFRDR_BTN_STATUS_HILITE:
                offset_s = 1;
                break;
            case LFRDR_BTN_STATUS_PRESSED:
                offset_s = 2;
                break;
            case LFRDR_BTN_STATUS_DISABLED:
                offset_s = 3;
                break;
            default:
                offset_s = 0;
                break;
        }

        bmp_info.bmp        = bmp_arrow;
        bmp_info.nr_line    = 16;
        bmp_info.nr_col     = 1;
        bmp_info.idx_col    = 0;
        bmp_info.idx_line   = offset_d*4 + offset_s;
        bmp_info.margin1    = 0;
        bmp_info.margin2    = 0;
        bmp_info.direct     = FALSE;
        bmp_info.flip       = FALSE;
        bmp_info.style      = LFSKIN_FILL_STRETCH;

        draw_area_from_bitmap (hdc, &rc_arrow, &bmp_info, FALSE);
    }
}

static void
draw_fold (HWND hWnd, HDC hdc, const RECT* pRect,
        DWORD color, int status, int next)
{
    /* houhh 20090410, if the treeview with icon style.*/
    if (status & LFRDR_TREE_WITHICON) {
        int w, h;
        int centerX, centerY;
        char szValue[255];
        HICON hFoldIcon   = (HICON)RetrieveRes (gui_GetIconFile("skin", (char*)SYSICON_TREEFOLD, szValue));
        HICON hUnFoldIcon = (HICON)RetrieveRes (gui_GetIconFile("skin", (char*)SYSICON_TREEUNFOLD, szValue));

        w = RECTWP(pRect);
        h = RECTHP(pRect);

        if(w < 4 || h < 4) return;

        centerX = pRect->left + (w>>1);
        centerY = pRect->top + (h>>1);

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
        const BITMAP* bitmap;
        DWORD file;
        int flag = 0;

        file = GetWindowElementAttr (hWnd, WE_LFSKIN_TREE);
        if (!(bitmap = GetBitmapFromRes (file)))
            return;

        if (status & LFRDR_TREE_CHILD) {
            if (status & LFRDR_TREE_FOLD)
                flag = 0;
            else
                flag = 1;

            if (bitmap) {
                gui_fill_box_with_bitmap_part_except_incompatible(hdc, pRect->left, pRect->top,
                        bitmap->bmWidth, (bitmap->bmHeight)>>1,
                        0, 0, bitmap, 0, flag * ((bitmap->bmHeight)>>1));
            }
        }
    }
}

    static void
draw_focus_frame (HDC hdc, const RECT *pRect, DWORD color)
{
    int i;
    gal_pixel pixel;


    pixel = RGBA2Pixel(hdc, GetRValue(color), GetGValue(color),
            GetBValue(color), GetAValue(color));

    for(i = pRect->left; i < pRect->right-1; i++)
    {
        if(i & 0x01)
        {
            SetPixel(hdc, i, pRect->top, pixel);
            SetPixel(hdc, i, pRect->bottom - 1, pixel);
        }
    }
    for(i = pRect->top; i < pRect->bottom-1; i++)
    {
        if(i & 0x01)
        {
            SetPixel(hdc, pRect->left, i ,pixel);
            SetPixel(hdc, pRect->right -1, i ,pixel);
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

#define STATUS_GET_CHECK(status) ((status) & BST_CHECK_MASK)
#define GET_BTN_POSE_STATUS(status) ((status) & BST_POSE_MASK)

    static void
draw_push_button (HWND hWnd, HDC hdc, const RECT* pRect,
        DWORD color1, DWORD color2, int status)
{
    DWORD file, dwStyle;
    const BITMAP* bmp;
    LFSKIN_BMPINFO bmp_info;

    file = GetWindowElementAttr (hWnd, WE_LFSKIN_PUSHBUTTON);

    if (!(bmp = GetBitmapFromRes (file)))
        return;

    dwStyle = GetWindowStyle (hWnd);

    bmp_info.bmp = bmp;
    bmp_info.nr_line = 5;
    bmp_info.nr_col = 1;
    bmp_info.idx_col = 0;
    bmp_info.idx_line = 0;
    bmp_info.margin1 = 6;
    bmp_info.margin2 = 6;
    bmp_info.direct = FALSE;
    bmp_info.flip = FALSE;
    bmp_info.style = LFSKIN_FILL_TILE;

    if ((status & BST_POSE_MASK) == BST_PUSHED)
        bmp_info.idx_line = 2;

    if (status & BST_CHECK_MASK)
    {
        if (STATUS_GET_CHECK (status) == BST_CHECKED)
            bmp_info.idx_line = 2;
        else if (STATUS_GET_CHECK (status) == BST_INDETERMINATE)
            bmp_info.idx_line = 4;
    }
    else if ((status & BST_POSE_MASK) == BST_NORMAL)
        bmp_info.idx_line = 0;
    else if ((status & BST_POSE_MASK) == BST_HILITE)
        bmp_info.idx_line = 1;
    else if ((status & BST_POSE_MASK) == BST_DISABLE)
        bmp_info.idx_line = 3;

    if (dwStyle & WS_DISABLED)
        bmp_info.idx_line = 3;

    draw_area_from_bitmap (hdc, pRect, &bmp_info, FALSE);
}

#define SIZE_BUTTON 13
    static void
_draw_radio_check_button (HDC hdc,
        const RECT* pRect, int status, const BITMAP* pBmp)
{
#if 0
    int w, h;
    int box_l = 0, box_t = 0;
    int bmp_w, bmp_h;

    w = RECTWP(pRect);
    h = RECTHP(pRect);

    bmp_w = pBmp->bmWidth;
    bmp_h = pBmp->bmHeight/BTN_STATUS_NUM;

    if(w >= bmp_w && h >= bmp_h){
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
        FillBitmapPartInBox(hdc, pRect->left, pRect->top,w, h,
                (PBITMAP)pBmp, 0, status * bmp_h, bmp_w, bmp_h);
    }
#else
    int  w = RECTWP(pRect);
    int  h = RECTHP(pRect);
    int box_l = 0, box_t = 0;
    int off_h = 0, off_v = 0;

    if (w <= 0 || h <=0) return;

    box_l = pRect->left + (w >> 1) - (SIZE_BUTTON >> 1);
    box_t = pRect->top + (h >> 1) - (SIZE_BUTTON >> 1);
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
                SIZE_BUTTON - (off_h << 1), SIZE_BUTTON - (off_v << 1), 0, 0, 
                pBmp, off_h, status * SIZE_BUTTON + off_v);
#endif
}

    static void
draw_radio_button (HWND hWnd, HDC hdc, const RECT* pRect, int status)
{
    DWORD file;
    const BITMAP* bmp;

    file = GetWindowElementAttr (hWnd, WE_LFSKIN_RADIOBUTTON);
    bmp = GetBitmapFromRes (file);

    if (bmp)
        _draw_radio_check_button (hdc, pRect, status, bmp);
}

    static void
draw_check_button (HWND hWnd, HDC hdc, const RECT* pRect, int status)
{
    DWORD file;
    const BITMAP* bmp;

    file = GetWindowElementAttr (hWnd, WE_LFSKIN_CHECKBUTTON);
    bmp = GetBitmapFromRes (file);

    if (bmp)
        _draw_radio_check_button (hdc, pRect, status, bmp);
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

    border = get_window_border (hWnd, 0, 0);
    cap_h = GetWindowElementAttr (hWnd, WE_METRICS_CAPTION);
    capbtn_h = cap_h - (CAP_BTN_INTERVAL<<1);
    we_area->top = ((cap_h - capbtn_h)>>1);     // Remove -1 for fix bug 4056.
    we_area->bottom = we_area->top + capbtn_h;  // Remove +1 for fix bug 4056.

    switch (which)
    {
        case HT_CLOSEBUTTON:
            CHECKNOT_RET_ERR (IS_CLOSEBTN_VISIBLE (win_info));
            we_area->right = win_w - border - CAP_BTN_INTERVAL;
            we_area->left = we_area->right - capbtn_h;
            return 0;

        case HT_MAXBUTTON:
            CHECKNOT_RET_ERR (IS_MAXIMIZEBTN_VISIBLE (win_info));
            we_area->right = win_w - border - capbtn_h
                        - (CAP_BTN_INTERVAL << 1);
            we_area->left = we_area->right - capbtn_h;
            return 0;

        case HT_MINBUTTON:
            CHECKNOT_RET_ERR (IS_MINIMIZEBTN_VISIBLE (win_info));
            we_area->right = win_w - border - (capbtn_h << 1)
                            - CAP_BTN_INTERVAL * 3;
            we_area->left = we_area->right - capbtn_h;
            return 0;

        case HT_ICON:
            CHECKNOT_RET_ERR (IS_CAPICON_VISIBLE (win_info));

            icon_w = icon->width;
            icon_h = icon->height;

            /*icon is larger, zoomin*/
            if (icon_h > cap_h - (CAP_BTN_INTERVAL<<1))
            {
                icon_h = cap_h - (CAP_BTN_INTERVAL<<1);
                icon_w =  icon_w * icon_h / icon->height;
            }

            we_area->left =  CAP_BTN_INTERVAL;
            we_area->right = we_area->left + icon_w;
            we_area->top =  ((cap_h - icon_h) >> 1);
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
            //we_area->top ++;
            //we_area->bottom --;
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
	//const WINDOW_ELEMENT_RENDERER * rdr = win_info->we_rdr;
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
            we_area->top = cap_h + menu_h;
            we_area->bottom = win_h - border - bottom_inner;
            return 0;

        case HT_SB_VTHUMB:
            we_area->top = cap_h + menu_h + scrollbar
                         + sbar_info->barStart;
            we_area->bottom = we_area->top + sbar_info->barLen;
            we_area->left ++;
            we_area->right --;
            return 0;

        case HT_SB_UPARROW:
            we_area->top = cap_h + menu_h;
            we_area->bottom = we_area->top + scrollbar;
            return 0;

        case HT_SB_DOWNARROW:
            we_area->bottom = win_h - border - bottom_inner;
            we_area->top = we_area->bottom - scrollbar;
            return 0;

        case HT_SB_UPSPACE:
            we_area->top = cap_h + menu_h + scrollbar;
            we_area->bottom = we_area->top + sbar_info->barStart;
            return 0;

        case HT_SB_DOWNSPACE:
            we_area->top = cap_h + menu_h + scrollbar
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
    const WINDOWINFO* win_info = NULL;
    int         cap_h = 0;
    int         icon_w = 0;
    int         icon_h = 0;
    PLOGFONT    cap_font;
    int         win_width, win_height;
    int         btn_w = 0;

    /* DK[24/21/09]: Bug4319
     * Fix a bug that want dereference a NULL Pointer */
	if(HWND_NULL != hWnd) win_info = GetWindowInfo(hWnd);

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
                cap_h = __mg_wnd_rdr_skin.we_metrics[_idx];
                return cap_h;
            }
            else if (hWnd != HWND_NULL) { 
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
                return __mg_wnd_rdr_skin.we_metrics[_idx];
            }
            else if (hWnd != HWND_NULL) {
                if (!IS_MENUBAR_VISIBLE (win_info))
                    return 0;
            }

            return GetWindowElementAttr (hWnd, WE_METRICS_MENU);
        }

        case LFRDR_METRICS_ICON_H:
        {
            PICON       icon;

            if (hWnd != HWND_NULL) {
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
                return __mg_wnd_rdr_skin.we_metrics[_idx];
            }
            else if (hWnd != HWND_NULL) {
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
                return __mg_wnd_rdr_skin.we_metrics[_idx];
            }
            else if (hWnd != HWND_NULL) {
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
            we_area->left = 0;
            we_area->right = win_w;
            if (win_info->dwStyle & WS_CAPTION)
                we_area->top = 0;
            else
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
            if (win_info->dwStyle & WS_CAPTION)
                we_area->top = cap_h;
            else
                we_area->top = cap_h + border;
            we_area->right = win_w - border;
            we_area->bottom = we_area->top + menu_h;
            return 0;

        case HT_CLIENT:
            border = get_window_border (hWnd, 0, 0);
            cap_h = get_window_caption (hWnd);
            menu_h = get_window_menubar (hWnd);

            if (win_info->dwStyle & WS_CAPTION)
                we_area->top = cap_h + menu_h;
            else
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
	//const WINDOW_ELEMENT_RENDERER * rdr = win_info->we_rdr;
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
    /*fix bug 3294: the caption button hit region is no change
     *with border change*/
    SetRect(&rc, border, 2, win_w-border, win_h-border);
    //SetRect(&rc, border, border, win_w-border, win_h-border);

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
    DWORD file;
    const BITMAP *bitmaptop, *bitmapbottom, *bitmapleft, *bitmapright;
    int i , border, cap_h = 0;
    const WINDOWINFO *win_info = NULL;
    RECT rect, area;
    DWORD color;
    BYTE r, g, b;
    gal_pixel old_pen_color;
    LFSKIN_BMPINFO bmp_info;

    if(calc_we_area(hWnd, HT_BORDER, &rect) == -1)
        return;

    border = get_window_border (hWnd, 0, 0);
    /*why draw border, when border is not more than 0*/
    if (border <= 0)
        return;
    file = GetWindowElementAttr (hWnd, WE_LFSKIN_BORDER_TOP);
    bitmaptop = GetBitmapFromRes(file);
    file = GetWindowElementAttr (hWnd, WE_LFSKIN_BORDER_BOTTOM);
    bitmapbottom = GetBitmapFromRes (file);
    file = GetWindowElementAttr (hWnd, WE_LFSKIN_BORDER_LEFT);
    bitmapleft = GetBitmapFromRes (file);
    file = GetWindowElementAttr (hWnd, WE_LFSKIN_BORDER_RIGHT);
    bitmapright = GetBitmapFromRes (file);

    win_info = GetWindowInfo(hWnd);
    old_pen_color = GetPenColor(hdc);

    if (IsMainWindow (hWnd)) {
        if (IsDialog(hWnd))   // for dialog
            border = 0;
        if(win_info->dwStyle & WS_BORDER
                || win_info->dwStyle & WS_THICKFRAME) {
            bmp_info.nr_line = 2;
            bmp_info.nr_col  = 1;
            bmp_info.idx_col = 0;
            bmp_info.margin1 = 0;
            bmp_info.margin2 = 0;
            bmp_info.direct  = FALSE;
            bmp_info.flip    = FALSE;
            bmp_info.style   = LFSKIN_FILL_STRETCH;

            if (border == 0) border += 3;

            if (is_active)
                bmp_info.idx_line = 0;
            else
                bmp_info.idx_line = 1;

            if (win_info->dwStyle & WS_CAPTION)
                cap_h = GetWindowElementAttr (hWnd, WE_METRICS_CAPTION);

            /*************left********************/
            if (bitmapleft) {
                bmp_info.bmp = bitmapleft;
                area.left = rect.left;
                if (win_info->dwStyle & WS_CAPTION)
                    area.top = rect.top + cap_h;
                else
                    area.top = rect.top + border + cap_h;
                area.right = area.left + border;
                area.bottom = rect.bottom - border;
                draw_area_from_bitmap (hdc, &area, &bmp_info, FALSE);
            }
            /*************right*******************/
            if (bitmapright) {
                bmp_info.bmp = bitmapright;
                area.right = rect.right;
                area.left = area.right - border;
                if (win_info->dwStyle & WS_CAPTION)
                    area.top = rect.top + cap_h;
                else
                    area.top = rect.top + border + cap_h;
                area.bottom = rect.bottom - border;
                draw_area_from_bitmap (hdc, &area, &bmp_info, FALSE);
            }

            /************top*********************/
            if (!(win_info->dwStyle & WS_CAPTION) && bitmaptop) {
                    bmp_info.bmp = bitmaptop;

                    bmp_info.nr_line = 2;
                    bmp_info.nr_col  = 1;
                    bmp_info.idx_col = 0;
                    bmp_info.margin1 = 5;
                    bmp_info.margin2 = 5;
                    bmp_info.direct  = FALSE;
                    bmp_info.flip    = FALSE;
                    bmp_info.style   = LFSKIN_FILL_TILE;

                    area.right = rect.right;
                    area.left = rect.left;
                    area.top = rect.top;
                    area.bottom = area.top + border;
                    draw_area_from_bitmap (hdc, &area, &bmp_info, FALSE);
            }

           /************bottom*******************/
            if (bitmapbottom) {
                bmp_info.bmp = bitmapbottom;

                bmp_info.nr_line = 2;
                bmp_info.nr_col  = 1;
                bmp_info.idx_col = 0;
                bmp_info.margin1 = 5;
                bmp_info.margin2 = 5;
                bmp_info.direct  = FALSE;
                bmp_info.flip    = FALSE;
                bmp_info.style   = LFSKIN_FILL_TILE;

                area.right  = rect.right;
                area.left   = rect.left;
                area.bottom = rect.bottom;
                area.top    = area.bottom - border;

                draw_area_from_bitmap (hdc, &area, &bmp_info, FALSE);
            }
        }
        else if (win_info->dwStyle & WS_THINFRAME) {
            if (is_active)
                color = GetWindowElementAttr(hWnd, WE_BGCA_ACTIVE_CAPTION);
            else
                color = GetWindowElementAttr(hWnd, WE_BGCA_INACTIVE_CAPTION);

            r = GetRValue(color);
            g = GetGValue(color);
            b = GetBValue(color);
            SetPenColor (hdc, RGB2Pixel(hdc, r, g, b));

            for(i = 0; i < border; i++) {
                Rectangle(hdc, rect.left+i, rect.top+i,
                        rect.right-i-1, rect.bottom-i-1);
            }
        }
    }
    else if(IsControl(hWnd)) {
        if(is_active)
            color = GetWindowElementAttr(hWnd, WE_BGCB_ACTIVE_CAPTION);
        else
            color = GetWindowElementAttr(hWnd, WE_BGCB_INACTIVE_CAPTION);

        r = GetRValue(color);
        g = GetGValue(color);
        b = GetBValue(color);

        if(win_info->dwStyle & WS_BORDER
                || win_info->dwStyle & WS_THICKFRAME
                || win_info->dwStyle & WS_THINFRAME) {
            SetPenColor(hdc ,RGB2Pixel(hdc, r, g, b));
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
    int font_h;
    gal_pixel text_color, old_text_color;
    PLOGFONT cap_font, old_font;
    const WINDOWINFO *win_info = NULL;
    RECT rect, icon_rect, rcTmp = {0};
    DWORD file;
    const BITMAP* bitmapcaption;
    LFSKIN_BMPINFO lfcaption;
    int ncbutton_w = 0;

    win_info = GetWindowInfo(hWnd);

    if (!(win_info->dwStyle & WS_CAPTION))
        return;

    if (calc_we_area(hWnd, HT_CAPTION, &rect) == -1)
        return;

    file = GetWindowElementAttr (hWnd, WE_LFSKIN_CAPTION);
    if (!(bitmapcaption = GetBitmapFromRes (file)))
        return;

    cap_font = (PLOGFONT)GetWindowElementAttr(hWnd, WE_FONT_CAPTION);

    lfcaption.bmp       = bitmapcaption;
    lfcaption.nr_line   = 2;
    lfcaption.nr_col    = 1;
    lfcaption.idx_col   = 0;
    lfcaption.margin1   = 5;
    lfcaption.margin2   = 5;
    lfcaption.direct    = FALSE;
    lfcaption.flip      = FALSE;
    lfcaption.style     = LFSKIN_FILL_TILE;

    if (is_active) {
        text_color = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_ACTIVE_CAPTION);
        lfcaption.idx_line = 0;
    } else {
        text_color = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_INACTIVE_CAPTION);
        lfcaption.idx_line = 1;
    }

    draw_area_from_bitmap (hdc, &rect, &lfcaption, FALSE);

    SetRectEmpty (&icon_rect);
    if (win_info->hIcon)
    {
        if (calc_we_area (hWnd, HT_ICON, &icon_rect) != -1)
        DrawIcon(hdc, icon_rect.left, icon_rect.top,
                    RECTW(icon_rect), RECTH(icon_rect), win_info->hIcon);
    }

    if (win_info->spCaption)
    {
        SetBkMode(hdc, BM_TRANSPARENT);
        old_font = SelectFont (hdc, cap_font);
        font_h = GetFontHeight (hdc);
        old_text_color = SetTextColor (hdc,text_color);

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

        TextOutOmitted (hdc, rect.left + RECTW(icon_rect) + 
                (ICON_ORIGIN << 1),
                rect.top + ((RECTH(rect)-font_h)>>1),
                win_info->spCaption,
                strlen (win_info->spCaption),  
                (RECTW(rect) - RECTW(icon_rect) - (ICON_ORIGIN << 1) - ncbutton_w));

        SetTextColor (hdc, old_text_color);
        SelectFont (hdc, old_font);
    }
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
    RECT rect;
    DWORD file;
    const BITMAP* bitmapcaptionbtn;
    LFSKIN_BMPINFO bmp_info;

    file = GetWindowElementAttr (hWnd, WE_LFSKIN_CAPTION_BTN);
    if (!(bitmapcaptionbtn = GetBitmapFromRes(file)))
        return;

    if (!(GetWindowStyle(hWnd) & WS_DISABLED)
                && (GetActiveWindow() == hWnd))
        bmp_info.idx_line = 0;
    else
        bmp_info.idx_line = 4;

    bmp_info.bmp = bitmapcaptionbtn;
    bmp_info.nr_line = 8;
    bmp_info.nr_col = 4;
    bmp_info.idx_col = 0;
    bmp_info.margin1 = 0;
    bmp_info.margin2 = 0;
    bmp_info.direct = FALSE;
    bmp_info.flip = FALSE;
    bmp_info.style = LFSKIN_FILL_STRETCH;

    switch(ht_code)
    {
        case 0:                     // draw all the 3 buttons as normal.
            status = LFRDR_BTN_STATUS_NORMAL;
        case HT_CLOSEBUTTON:
            if(calc_we_area(hWnd, HT_CLOSEBUTTON, &rect) != -1)
            {
                if(RECTH(rect) <= 0 || RECTW(rect) <= 0)
                    return;
                if (status & LFRDR_BTN_STATUS_PRESSED) {
                    bmp_info.idx_line += 2;
                }
                else if (status & LFRDR_BTN_STATUS_HILITE) {
                    bmp_info.idx_line += 1;
                }
                else if (status & LFRDR_BTN_STATUS_DISABLED) {
                    bmp_info.idx_line += 3;
                }

                bmp_info.idx_col = 3;
                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);
            }
            if(ht_code == HT_CLOSEBUTTON) break;

        case HT_MAXBUTTON:
            if(calc_we_area(hWnd, HT_MAXBUTTON, &rect) != -1)
            {
                if(RECTH(rect) <= 0 || RECTW(rect) <= 0)
                    return;
                if (status & LFRDR_BTN_STATUS_PRESSED) {
                    bmp_info.idx_line += 2;
                }
                else if (status & LFRDR_BTN_STATUS_HILITE) {
                    bmp_info.idx_line += 1;
                }
                else if (status & LFRDR_BTN_STATUS_DISABLED) {
                    bmp_info.idx_line += 3;
                }

                bmp_info.idx_col = 2;
                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);
           }
            if(ht_code == HT_MAXBUTTON) break;

        case HT_MINBUTTON:
            if(calc_we_area(hWnd, HT_MINBUTTON, &rect) != -1)
            {
                if(RECTH(rect) <= 0 || RECTW(rect) <= 0)
                    return;
                if (status & LFRDR_BTN_STATUS_PRESSED) {
                    bmp_info.idx_line += 2;
                }
                else if (status & LFRDR_BTN_STATUS_HILITE) {
                    bmp_info.idx_line += 1;
                }
                else if (status & LFRDR_BTN_STATUS_DISABLED) {
                    bmp_info.idx_line += 3;
                }

                bmp_info.idx_col = 1;
                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);
            }

            if(ht_code == HT_MINBUTTON) break;
            if(ht_code == 0) break;

        default:
            break;
    }
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
                prc->top++;
                prc->bottom--;
                break;
            }
        case HT_SB_VTHUMB:
            {
                prc->left++;
                prc->right--;
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
    RECT        rect;
    DWORD       file;
    BOOL        isCtrl = FALSE;    /** if TRUE it is scrollbar control else not */
    int         offset_arrow, sb_status;
    const BITMAP        *bmp;
    gal_pixel           old_brush_color;
    LFSKIN_BMPINFO      bmp_info;
    const WINDOWINFO    *info = (WINDOWINFO*)GetWindowInfo (hWnd);

    if (0 == strncasecmp (CTRL_SCROLLBAR, GetClassName(hWnd), strlen(CTRL_SCROLLBAR))) {
        isCtrl = TRUE;
    }

    if (isCtrl) {
        if (0 != calc_scrollbarctrl_area(hWnd, sb_pos, &rect))
            return;
    } else {
           /** draw the rect between H and V */
           if(calc_we_area(hWnd, HT_HSCROLL, &rect) != -1) {
               if(info->dwStyle & WS_VSCROLL) {
                   if (info->dwExStyle & WS_EX_LEFTSCROLLBAR) {
                       rect.right = rect.left;
                       rect.left = rect.right - RECTH(rect);
                   } else {
                       rect.left = rect.right;
                       rect.right = rect.left + RECTH(rect);
                   }
                   old_brush_color =
                       SetBrushColor(hdc, RGB2Pixel(hdc, 0xEE,0xED,0xE5));

                   FillBox(hdc, rect.left, rect.top, RECTW(rect), RECTH(rect));
                   SetBrushColor(hdc, old_brush_color);
               }
           }

           if (sb_pos != 0) {
               if (0 != calc_we_area (hWnd, sb_pos, &rect))
                   return;
               if (sb_pos & HT_SB_MASK) {
                   if (sb_pos == HT_SB_HTHUMB) {
                       rect.top++;
                       rect.bottom--;
                   }
               }
           } else {
               draw_scrollbar (hWnd, hdc, HT_HSCROLL);
               draw_scrollbar (hWnd, hdc, HT_VSCROLL);
               return;
           }
    }

    switch(sb_pos)
    {
        case HT_HSCROLL:       // paint the hscrollbar
            {
                file = GetWindowElementAttr (hWnd, WE_LFSKIN_SCROLLBAR_HSHAFT);
                if (!(bmp = GetBitmapFromRes (file)))
                    return;

                bmp_info.bmp        = bmp;
                bmp_info.nr_line    = 4;
                bmp_info.nr_col     = 1;
                bmp_info.idx_col    = 0;
                bmp_info.margin1    = 0;
                bmp_info.margin2    = 0;
                bmp_info.direct     = FALSE;
                bmp_info.flip       = FALSE;
                bmp_info.idx_line   = 0;
                bmp_info.style      = LFSKIN_FILL_STRETCH;

                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);

                draw_scrollbar (hWnd, hdc, HT_SB_LEFTARROW);
                draw_scrollbar (hWnd, hdc, HT_SB_RIGHTARROW);
                draw_scrollbar (hWnd, hdc, HT_SB_HTHUMB);
                break;
            }
        case HT_VSCROLL:         // paint the vscrollbar
            {
                file = GetWindowElementAttr (hWnd, WE_LFSKIN_SCROLLBAR_VSHAFT);
                if (!(bmp = GetBitmapFromRes (file)))
                    return;

                bmp_info.bmp        = bmp;
                bmp_info.nr_line    = 4;
                bmp_info.nr_col     = 1;
                bmp_info.idx_col    = 0;
                bmp_info.margin1    = 0;
                bmp_info.margin2    = 0;
                bmp_info.direct     = FALSE;
                bmp_info.flip       = FALSE;
                bmp_info.idx_line   = 0;
                bmp_info.style      = LFSKIN_FILL_STRETCH;

                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);

                draw_scrollbar (hWnd, hdc, HT_SB_UPARROW);
                draw_scrollbar (hWnd, hdc, HT_SB_DOWNARROW);
                draw_scrollbar (hWnd, hdc, HT_SB_VTHUMB);
                break;
            }
        case HT_SB_LEFTARROW:
            {
                file = GetWindowElementAttr (hWnd, WE_LFSKIN_SCROLLBAR_ARROWS);
                if (!(bmp = GetBitmapFromRes (file)))
                    return;

                bmp_info.bmp        = bmp;
                bmp_info.nr_line    = 16;
                bmp_info.nr_col     = 1;
                bmp_info.idx_col    = 0;
                bmp_info.margin1    = 0;
                bmp_info.margin2    = 0;
                bmp_info.direct     = FALSE;
                bmp_info.flip       = FALSE;
                bmp_info.idx_line   = 0;
                bmp_info.style      = LFSKIN_FILL_STRETCH;

                sb_status = get_scroll_status(hWnd, FALSE);

                offset_arrow = 0;
                if(sb_status & SBS_HILITE_LTUP){
                    offset_arrow = 1;
                }
                if(sb_status & SBS_PRESSED_LTUP){
                    offset_arrow = 2;
                }

                bmp_info.idx_line = 8 + offset_arrow;
                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);

                break;
            }
        case HT_SB_RIGHTARROW:
            {
                file = GetWindowElementAttr (hWnd, WE_LFSKIN_SCROLLBAR_ARROWS);
                if (!(bmp = GetBitmapFromRes (file)))
                    return;

                bmp_info.bmp        = bmp;
                bmp_info.nr_line    = 16;
                bmp_info.nr_col     = 1;
                bmp_info.idx_col    = 0;
                bmp_info.margin1    = 0;
                bmp_info.margin2    = 0;
                bmp_info.direct     = FALSE;
                bmp_info.flip       = FALSE;
                bmp_info.idx_line   = 0;
                bmp_info.style      = LFSKIN_FILL_STRETCH;

                sb_status = get_scroll_status(hWnd, FALSE);

                offset_arrow = 0;
                if(sb_status & SBS_HILITE_BTDN){
                    offset_arrow = 1;
                }
                if(sb_status & SBS_PRESSED_BTDN){
                    offset_arrow = 2;
                }

                bmp_info.idx_line = 12 + offset_arrow;
                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);

                break;
            }
        case HT_SB_HTHUMB:
            {
                file = GetWindowElementAttr (hWnd, WE_LFSKIN_SCROLLBAR_HTHUMB);
                if (!(bmp = GetBitmapFromRes (file)))
                    return;

                bmp_info.bmp        = bmp;
                bmp_info.nr_line    = 4;
                bmp_info.nr_col     = 1;
                bmp_info.idx_line   = 0;
                bmp_info.idx_col    = 0;
                bmp_info.margin1    = 3;
                bmp_info.margin2    = 3;
                bmp_info.direct     = FALSE;
                bmp_info.flip       = FALSE;
                bmp_info.style      = LFSKIN_FILL_TILE;

                sb_status = get_scroll_status(hWnd, FALSE);
                if(sb_status & SBS_HILITE_THUMB){
                    bmp_info.idx_line = 1;
                }
                if(sb_status & SBS_PRESSED_THUMB){
                    bmp_info.idx_line = 2;
                }

                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);

                break;
            }
        case HT_SB_UPARROW:
            {
                file = GetWindowElementAttr (hWnd, WE_LFSKIN_SCROLLBAR_ARROWS);
                if (!(bmp = GetBitmapFromRes (file)))
                    return;

                bmp_info.bmp        = bmp;
                bmp_info.nr_line    = 16;
                bmp_info.nr_col     = 1;
                bmp_info.idx_col    = 0;
                bmp_info.margin1    = 0;
                bmp_info.margin2    = 0;
                bmp_info.direct     = FALSE;
                bmp_info.flip       = FALSE;
                bmp_info.idx_line   = 0;
                bmp_info.style      = LFSKIN_FILL_STRETCH;

                sb_status = get_scroll_status(hWnd, TRUE);

                offset_arrow = 0;
                if(sb_status & SBS_HILITE_LTUP){
                    offset_arrow = 1;
                }
                if(sb_status & SBS_PRESSED_LTUP){
                    offset_arrow = 2;
                }

                bmp_info.idx_line = offset_arrow;
                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);

                break;
            }
        case HT_SB_DOWNARROW:
            {
                file = GetWindowElementAttr (hWnd, WE_LFSKIN_SCROLLBAR_ARROWS);
                if (!(bmp = GetBitmapFromRes (file)))
                    return;

                bmp_info.bmp        = bmp;
                bmp_info.nr_line    = 16;
                bmp_info.nr_col     = 1;
                bmp_info.idx_col    = 0;
                bmp_info.margin1    = 0;
                bmp_info.margin2    = 0;
                bmp_info.direct     = FALSE;
                bmp_info.flip       = FALSE;
                bmp_info.idx_line   = 0;
                bmp_info.style      = LFSKIN_FILL_STRETCH;

                sb_status = get_scroll_status(hWnd, TRUE);

                offset_arrow = 0;
                if(sb_status & SBS_HILITE_BTDN){
                    offset_arrow =1;
                }
                if(sb_status & SBS_PRESSED_BTDN){
                    offset_arrow = 2;
                }

                bmp_info.idx_line = 4 + offset_arrow;
                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);

                break;
            }
        case HT_SB_VTHUMB:
            {
                file = GetWindowElementAttr (hWnd, WE_LFSKIN_SCROLLBAR_VTHUMB);
                if (!(bmp = GetBitmapFromRes (file)))
                    return;
                /* houhh 20090728, if vthumb height is zero.*/
                if (!RECTH(rect))
                    return;

                bmp_info.bmp        = bmp;
                bmp_info.nr_line    = 4;
                bmp_info.nr_col     = 1;
                bmp_info.idx_line   = 0;
                bmp_info.idx_col    = 0;
                bmp_info.margin1    = 3;
                bmp_info.margin2    = 3;
                bmp_info.direct     = TRUE;
                bmp_info.flip       = FALSE;
                bmp_info.style      = LFSKIN_FILL_TILE;

                sb_status = get_scroll_status(hWnd, TRUE);
                if(sb_status & SBS_HILITE_THUMB){
                    bmp_info.idx_line = 1;
                }
                if(sb_status & SBS_PRESSED_THUMB){
                    bmp_info.idx_line = 2;
                }

                draw_area_from_bitmap (hdc, &rect, &bmp_info, FALSE);

                break;
            }
        default:
            break;
    }
}

static void draw_trackbar_thumb (HWND hWnd, HDC hdc,
        const RECT* pRect, DWORD dwStyle)
{
    /** trackbar status, pressed or hilite */
    RECT    rc_draw;
    BOOL    vertical;
    DWORD   file;
    const BITMAP* bmp;
    LFSKIN_BMPINFO bmp_info;

    /** leave little margin */
    if (dwStyle & TBS_VERTICAL) {
        rc_draw.left   = pRect->left;
        rc_draw.top    = pRect->top ;
        rc_draw.right  = pRect->right;
        rc_draw.bottom = pRect->bottom;
        vertical = TRUE;
    }
    else{
        rc_draw.left   = pRect->left;
        rc_draw.top    = pRect->top;
        rc_draw.right  = pRect->right;
        rc_draw.bottom = pRect->bottom;
        vertical = FALSE;
    }

    if (vertical) {
        file = GetWindowElementAttr (hWnd, WE_LFSKIN_TRACKBAR_VERT);
        if (!(bmp = GetBitmapFromRes (file)))
            return;
    }
    else {
        file = GetWindowElementAttr (hWnd, WE_LFSKIN_TRACKBAR_HORZ);
        if (!(bmp = GetBitmapFromRes (file)))
            return;
    }

    bmp_info.bmp      = bmp;
    bmp_info.nr_line  = 4;
    bmp_info.nr_col   = 1;
    bmp_info.idx_col  = 0;
    bmp_info.idx_line = 0;
    bmp_info.margin1  = 0;
    bmp_info.margin2  = 0;
    bmp_info.direct   = FALSE;
    bmp_info.flip     = FALSE;
    bmp_info.style    = LFSKIN_FILL_STRETCH;

    if (dwStyle & LFRDR_TBS_PRESSED)
        bmp_info.idx_line = 2;
    else if (dwStyle & LFRDR_TBS_HILITE)
        bmp_info.idx_line = 1;
    else if (dwStyle & WS_DISABLED)
        bmp_info.idx_line = 3;

   draw_area_from_bitmap (hdc, &rc_draw, &bmp_info, FALSE);
}

static void
calc_trackbar_rect (HWND hWnd, LFRDR_TRACKBARINFO *info, DWORD dwStyle,
        const RECT* rcClient, RECT* rcRuler, RECT* rcBar, RECT* rcBorder)
{
    int     x, y, w, h;
    int     pos, min, max;
    int     sliderx, slidery, sliderw, sliderh;
    const BITMAP *ruler_bmp = NULL, *hthumb_bmp = NULL, *vthumb_bmp = NULL;
    DWORD ruler, hthumb, vthumb;

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

    if (!rcRuler && !rcBar)
        return;

    if (dwStyle & TBS_VERTICAL) {
        vthumb = GetWindowElementAttr (hWnd, WE_LFSKIN_TRACKBAR_VERT);
        if (!(vthumb_bmp = GetBitmapFromRes (vthumb)))
            return;

        ruler = GetWindowElementAttr (hWnd, WE_LFSKIN_TBSLIDER_V);
        if (!(ruler_bmp = GetBitmapFromRes (ruler)))
            return;
    }
    else {
        hthumb = GetWindowElementAttr (hWnd, WE_LFSKIN_TRACKBAR_HORZ);
        if (!(hthumb_bmp = GetBitmapFromRes (hthumb)))
            return;

        ruler = GetWindowElementAttr (hWnd, WE_LFSKIN_TBSLIDER_H);
        if (!(ruler_bmp = GetBitmapFromRes (ruler)))
            return;
    }
    /* Calculate ruler rect. */
    if (rcRuler) {
        if (dwStyle & TBS_VERTICAL) {
            rcRuler->left   = x + ((w - ruler_bmp->bmWidth)>>1);
            rcRuler->top    = y;
            rcRuler->right  = x + ((w + ruler_bmp->bmWidth)>>1);
            rcRuler->bottom = y + h;
        } else {
            rcRuler->left   = x;
            rcRuler->top    = y + ((h - ruler_bmp->bmHeight)>>1);
            rcRuler->right  = x + w;
            rcRuler->bottom = y + ((h + ruler_bmp->bmHeight)>>1);
        }
    }

    if (rcBar) {
        /* Calculate slider rect. */
        if (dwStyle & TBS_VERTICAL) {
            sliderw = vthumb_bmp->bmWidth;
            sliderh = vthumb_bmp->bmHeight>>2;
            sliderx = x + ((w - sliderw) >> 1);
            slidery = y + (int)(max - pos) * (h - sliderh) / (max - min);
        } else {
            sliderw = hthumb_bmp->bmWidth;
            sliderh = hthumb_bmp->bmHeight>>2;
            slidery = y + ((h - sliderh) >> 1);
            sliderx = x + (int)(pos - min) * (w - sliderw) / (max - min);
        }
        SetRect (rcBar, sliderx, slidery, sliderx + sliderw, slidery + sliderh);
    }
}

static void
draw_trackbar (HWND hWnd, HDC hdc, LFRDR_TRACKBARINFO *info)
{
    DWORD file;
    LFSKIN_BMPINFO bmp_info;
    const BITMAP *bmp;
    RECT    rc_client, rc_border, rc_ruler, rc_bar;
    DWORD   dwStyle;

    dwStyle = GetWindowStyle (hWnd);
    GetClientRect (hWnd, &rc_client);
    calc_trackbar_rect (hWnd, info, dwStyle, &rc_client,
            &rc_ruler, &rc_bar, &rc_border);

    if (dwStyle & TBS_VERTICAL){
        file = GetWindowElementAttr (hWnd, WE_LFSKIN_TBSLIDER_V);
        bmp_info.direct = TRUE;
    }
    else{
        file = GetWindowElementAttr (hWnd, WE_LFSKIN_TBSLIDER_H);
        bmp_info.direct = FALSE;
    }

    if (!(bmp = GetBitmapFromRes (file)))
        return;
    bmp_info.bmp = bmp;
    bmp_info.nr_line = 1;
    bmp_info.nr_col = 1;
    bmp_info.idx_line = 0;
    bmp_info.idx_col = 0;
    bmp_info.margin1 = 2;
    bmp_info.margin2 = 2;
    bmp_info.flip = FALSE;
    bmp_info.style = LFSKIN_FILL_TILE;

    draw_area_from_bitmap (hdc, &rc_ruler, &bmp_info, FALSE);
    draw_trackbar_thumb (hWnd, hdc, &rc_bar, dwStyle);
}

static void
disabled_text_out (HWND hWnd, HDC hdc,
        const char* spText, PRECT rc, DWORD dt_fmt)
{
    DWORD mainc, color;

    SetBkMode (hdc, BM_TRANSPARENT);
    SetBkColor (hdc, GetWindowBkColor (hWnd));

    mainc = GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY);

    rc->left  += 1;
    rc->top   += 1;
    rc->right += 1;
    rc->bottom+= 1;

    color = calc_3dbox_color (mainc, LFRDR_3DBOX_COLOR_LIGHTER);
    SetTextColor (hdc, RGBA2Pixel (hdc, GetRValue (color),
                GetGValue (color), GetBValue (color), GetAValue (color)));
    DrawText (hdc, spText, -1, rc, dt_fmt);

    rc->left  -= 1;
    rc->top   -= 1;
    rc->right -= 1;
    rc->bottom-= 1;

    color = calc_3dbox_color (mainc, LFRDR_3DBOX_COLOR_DARKER);
    SetTextColor (hdc, RGBA2Pixel (hdc, GetRValue (color),
                GetGValue (color), GetBValue (color), GetAValue (color)));
    DrawText (hdc, spText, -1, rc, dt_fmt);
}

/*
 * draw_tab:
 *  This function draw a tab for the propsheet.
 * Author : wangjian<wangjian@minigui.org>
 * Date : 2007-12-18.
 */
static void
draw_tab (HWND hWnd, HDC hdc, RECT *pRect, char *title, DWORD color, int flag, HICON icon)
{
    SIZE size;
    DWORD file;
    int eff_chars, eff_len;
    int x, ty, text_extent;
    const BITMAP* bitmap;
    LFSKIN_BMPINFO bmp_info;

    file = GetWindowElementAttr (hWnd, WE_LFSKIN_TAB);
    if (!(bitmap = GetBitmapFromRes (file)))
        return;

    bmp_info.bmp      = bitmap;
    bmp_info.nr_line  = 4;
    bmp_info.nr_col   = 1;
    bmp_info.idx_col  = 0;
    bmp_info.margin1  = 3;
    bmp_info.margin2  = 4;
    bmp_info.direct   = FALSE;
    bmp_info.flip     = FALSE;
    bmp_info.idx_line = 0;
    bmp_info.style    = LFSKIN_FILL_TILE;

    x = pRect->left + 2;
    ty = pRect->top;

    if (!(flag & LFRDR_TAB_ACTIVE)) {
        if (flag & LFRDR_TAB_BOTTOM) {
            bmp_info.flip = TRUE;
            ty -= 2;
        } else {
            ty += 2;
        }

     } else {
        bmp_info.idx_line = 2;

        if (flag & LFRDR_TAB_BOTTOM) {
            bmp_info.flip = TRUE;
        }
     }

    draw_area_from_bitmap (hdc, pRect, &bmp_info, FALSE);

    /* draw the ICON */
    ty += 2 + 2;
    text_extent = RECTWP (pRect) - 2 * 2;
    if (icon) {
        int icon_x, icon_y;
        icon_x = RECTHP(pRect) - 8;
        icon_y = icon_x;

        DrawIcon (hdc, x, ty, icon_x, icon_y, icon);
        x += icon_x;
        x += 2;
        text_extent -= icon_x + 2;
    }

    /* draw the TEXT */
    //SetBkColor (hdc, DWORD2Pixel (hdc, color));
    SetBkMode (hdc, BM_TRANSPARENT);
    text_extent -= 4;
    eff_len = GetTextExtentPoint (hdc, title, strlen(title),
                  text_extent, &eff_chars, NULL, NULL, &size);

    TextOutLen (hdc, x + 2, ty, title, eff_len);
}

static void
draw_progress (HWND hWnd, HDC hdc,
        int nMax, int nMin, int nPos, BOOL fVertical)
{
    const BITMAP*   bmp;
    DWORD           file;
    RECT            rcClient;
    int             pbar_border = 2;
    int             x, y, w, h;
    int             j, prog_step, prog_len, step;
    ldiv_t          ndiv_progress;
    unsigned int    nAllPart, nNowPart;
    LFSKIN_BMPINFO  bmp_info;

    if (nMax == nMin)
        return;

    GetClientRect (hWnd, &rcClient);

    if (fVertical) {
        file = GetWindowElementAttr (hWnd, WE_LFSKIN_PROGRESS_VTRACKBAR);
        bmp_info.direct = TRUE;
    } else {
        file = GetWindowElementAttr (hWnd, WE_LFSKIN_PROGRESS_HTRACKBAR);
        bmp_info.direct = FALSE;
    }

    if (!(bmp = GetBitmapFromRes (file)))
        return;

    bmp_info.bmp        = bmp;
    bmp_info.nr_line    = 1;
    bmp_info.nr_col     = 1;
    bmp_info.idx_line   = 0;
    bmp_info.idx_col    = 0;
    bmp_info.margin1    = 3;
    bmp_info.margin2    = 3;
    bmp_info.flip       = FALSE;
    bmp_info.style      = LFSKIN_FILL_TILE;

    /*Draw progress trackbar. */
    draw_area_from_bitmap (hdc, &rcClient, &bmp_info, FALSE);

    /* draw chunk */
    x = rcClient.left + pbar_border;
    y = rcClient.top + pbar_border;
    w = RECTW (rcClient) - (pbar_border << 1);
    h = RECTH (rcClient) - (pbar_border << 1);

    if (fVertical) {
        file = GetWindowElementAttr (hWnd, WE_LFSKIN_PROGRESS_VCHUNK);
        if (!(bmp = GetBitmapFromRes (file)))
            return;
        step = bmp->bmHeight;
    }
    else {
        file = GetWindowElementAttr (hWnd, WE_LFSKIN_PROGRESS_HCHUNK);
        if (!(bmp = GetBitmapFromRes (file)))
            return;
        step = bmp->bmWidth;
    }

    ndiv_progress = ldiv (nMax - nMin, step);
    nAllPart = ndiv_progress.quot;

    ndiv_progress = ldiv (nPos - nMin, step);
    nNowPart = ndiv_progress.quot;

    if (fVertical) {
        prog_len = h * nNowPart/nAllPart;
        for (j = 0; j <= prog_len / step; j++) {
            prog_step = (j+1)*step > h ? h : (j+1)*step;
            gui_fill_box_with_bitmap_except_incompatible (hdc, x + 1,
                rcClient.bottom - pbar_border - prog_step, w - 2, 0, bmp);
        }
    }
    else {
        prog_len = w * nNowPart/nAllPart;
        for (j = 0; j <= prog_len/step; j++) {
            prog_step = (j+1)*step > w ? (w-step) : (j*step);
            gui_fill_box_with_bitmap_except_incompatible (hdc, x + prog_step, y + 1, 0, h - 2, bmp);
        }
    }
}

static void draw_header (HWND hWnd, HDC hdc, const RECT *pRect, DWORD color)
{
    const BITMAP* bitmap;
    DWORD file;
    LFSKIN_BMPINFO bmp_info;

    file = GetWindowElementAttr (hWnd, WE_LFSKIN_HEADER);
    if (!(bitmap = GetBitmapFromRes (file)))
        return;

    bmp_info.bmp      = bitmap;
    bmp_info.nr_line  = 4;
    bmp_info.nr_col   = 1;
    bmp_info.idx_line = 0;
    bmp_info.idx_col  = 0;
    bmp_info.margin1  = 2;
    bmp_info.margin2  = 2;
    bmp_info.direct   = FALSE;
    bmp_info.flip     = FALSE;
    bmp_info.style    = LFSKIN_FILL_TILE;

    draw_area_from_bitmap (hdc, pRect, &bmp_info, FALSE);
}

static void erase_background (HWND hWnd, HDC hdc, const RECT *pRect)
{
    RECT  rcClient;
    int   new_left, new_top;
    DWORD file_win, ex_style;
    const BITMAP* bg_bmp = NULL;

    file_win = GetWindowElementAttr (hWnd, WE_LFSKIN_WND_BKGND);
    ex_style = GetWindowExStyle (hWnd);

    ClipRectIntersect (hdc, pRect);

    bg_bmp = GetBitmapFromRes (file_win);
    if (bg_bmp) {
        GetClientRect (hWnd, &rcClient);

        if (ex_style & WS_EX_LFRDR_CUSTOM1) {
            gui_fill_box_with_bitmap_except_incompatible (hdc, 0, 0, RECTW (rcClient),
                    RECTH (rcClient), bg_bmp);
        }
        else {
            new_left = rcClient.left;
            new_top = rcClient.top;

            do {
                do {
                    gui_fill_box_with_bitmap_except_incompatible (hdc, new_left, new_top,
                            bg_bmp->bmWidth, bg_bmp->bmHeight, bg_bmp);
                    new_top += bg_bmp->bmHeight;
                } while (new_top < rcClient.bottom);

                new_top = rcClient.top;
                new_left += bg_bmp->bmWidth;
            } while (new_left < rcClient.right);
        }
    }
    else
        erase_bkgnd (hWnd, hdc, pRect);
}

WINDOW_ELEMENT_RENDERER __mg_wnd_rdr_skin = {
    "skin",
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

    on_get_rdr_attr,
    on_set_rdr_attr,
    erase_background,

    draw_normal_menu_item,
    draw_hilite_menu_item,
    draw_disabled_menu_item,
};
#endif /* _MGLF_RDR_SKIN */
