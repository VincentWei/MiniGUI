///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** glyph.c: Low level glyph routines.
**
** Create date: 2008/02/02
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "pixel_ops.h"
#include "cursor.h"
#include "drawtext.h"
#include "fixedmath.h"
#include "glyph.h"

#define EDGE_ALPHA      64
#define ALL_WGHT        32
#define ROUND_WGHT      16

#define NEIGHBOR_WGHT    3
#define NEAR_WGHT        1

#ifdef _MGFONT_FT2
int ft2GetLcdFilter (DEVFONT* devfont);
int ft2IsFreeTypeDevfont (DEVFONT* devfont);
#endif

#define FS_WEIGHT_AUTOBOLD  29

Glyph32 GetGlyphValueAlt(LOGFONT* lf, Achar32 chv)
{
    Glyph32 gv = INV_GLYPH_VALUE;
    int i, dfi = 0;
    DEVFONT* df;

    if (IS_MBCHV(chv)) {
        chv = REAL_ACHAR(chv);
        for (i = 1; i < MAXNR_DEVFONTS; i++) {
            if ((df = lf->devfonts[i]) != NULL) {
                if (df->font_ops->get_glyph_value)
                    gv = df->font_ops->get_glyph_value(lf, df, chv);
                else
                    gv = chv;

                if (df->font_ops->is_glyph_existed(lf, df, gv)) {
                    dfi = i;
                    break;
                }
            }
        }

        if (dfi == 0) {
            if ((df = lf->devfonts[1]) != NULL) {
                dfi = 1;
                if (df->font_ops->get_glyph_value)
                    gv = df->font_ops->get_glyph_value(lf, df, chv);
                else
                    gv = chv;
            }
            else
                goto error;
        }
    }
    else {
        dfi = 0;
        if ((df = lf->devfonts[0])) {
            if (df->font_ops->get_glyph_value)
                gv = df->font_ops->get_glyph_value(lf, df, chv);
            else
                gv = chv;
        }
        else
            goto error;
    }

    return SET_GLYPH_DFI(gv, dfi);

error:
    return INV_GLYPH_VALUE;
}

Glyph32 GUIAPI GetGlyphValue (LOGFONT* logfont, const char* mchar,
        int mchar_len, const char* pre_mchar, int pre_len)
{
    Achar32 achar = GetACharValue(logfont, mchar,
        mchar_len, pre_mchar, pre_len);

    return GetGlyphValueAlt(logfont, achar);
}

int GUIAPI GetGlyphInfo (LOGFONT* logfont, Glyph32 glyph_value,
        GLYPHINFO* glyph_info)
{
    /* get the relative device font */
    DEVFONT* devfont = SELECT_DEVFONT_BY_GLYPH(logfont, glyph_value);
    glyph_value = REAL_GLYPH (glyph_value);
    SIZE sz;

    /* get metrics of the glyph */
    if ((glyph_info->mask & GLYPH_INFO_METRICS) ||
            (glyph_info->mask & GLYPH_INFO_BMP)) {
        glyph_info->advance_x = 0;
        glyph_info->advance_y = 0;
        glyph_info->bbox_x = 0;
        glyph_info->bbox_y = 0;

        _font_get_glyph_advance (logfont, devfont, glyph_value, TRUE,
            0, 0, 0, &glyph_info->advance_x, &glyph_info->advance_x,
            (BBOX*)(&glyph_info->bbox_x));

        sz.cx = glyph_info->bbox_w;
        sz.cy = glyph_info->bbox_h;
    }

    /* get glyph bitmap info */
    if (glyph_info->mask & GLYPH_INFO_BMP) {
        DWORD bmptype = devfont->font_ops->get_glyph_bmptype (logfont, devfont);
        glyph_info->bmp_type = bmptype & DEVFONTGLYPHTYPE_MASK_BMPTYPE;

        switch (glyph_info->bmp_type) {
        case GLYPHBMP_TYPE_MONO:
            glyph_info->bits = devfont->font_ops->get_glyph_monobitmap (
                logfont, devfont, glyph_value, &sz, &glyph_info->bmp_pitch, NULL);
            break;

        case GLYPHBMP_TYPE_GREY:
            glyph_info->bits = devfont->font_ops->get_glyph_greybitmap (
                    logfont, devfont, glyph_value,
                    &sz, &glyph_info->bmp_pitch, NULL);
            break;

        case GLYPHBMP_TYPE_SUBPIXEL:
            glyph_info->bits = devfont->font_ops->get_glyph_greybitmap (
                    logfont, devfont, glyph_value,
                    &sz, &glyph_info->bmp_pitch, NULL);
            break;

        case GLYPHBMP_TYPE_PRERENDER:
            devfont->font_ops->get_glyph_prbitmap (logfont,
                    devfont, glyph_value, &glyph_info->prbitmap);
            break;
        }
    }

    glyph_info->bmp_width = sz.cx;
    glyph_info->bmp_height = sz.cy;
    return 0;
}

void GUIAPI GetGlyphBitmap (LOGFONT* logfont, const char* mchar,
        int mchar_len, GLYPHBITMAP* glyph_bitmap)
{
    Achar32 achar;
    GLYPHINFO glyph_info;
    Glyph32 glyph_value = INV_GLYPH_VALUE;

    memset(&glyph_info, 0, sizeof(GLYPHINFO));
    achar = GetACharValue(logfont,(const char*)mchar,
            mchar_len, NULL, 0);
    glyph_value = GetGlyphValueAlt(logfont, achar);

    glyph_info.mask = GLYPH_INFO_METRICS | GLYPH_INFO_BMP;
    glyph_info.bmp_type = GLYPHBMP_TYPE_MONO;

    if(glyph_value != INV_GLYPH_VALUE)
        GetGlyphInfo(logfont, glyph_value, &glyph_info);

    glyph_bitmap->bbox_x = glyph_info.bbox_x;
    glyph_bitmap->bbox_y = glyph_info.bbox_y;
    glyph_bitmap->bbox_w = glyph_info.bbox_w;
    glyph_bitmap->bbox_h = glyph_info.bbox_h;

    glyph_bitmap->advance_x = glyph_info.advance_x;
    glyph_bitmap->advance_y = glyph_info.advance_y;

    glyph_bitmap->bmp_size = glyph_info.bmp_pitch * glyph_info.bmp_height;
    glyph_bitmap->bmp_pitch = glyph_info.bmp_pitch;
    glyph_bitmap->bits = glyph_info.bits;
}

static int mult (fixed op1, fixed op2)
{
    long s = op2;
    if (s < 0)
        op2 = -op2;
    {
        long op1_hi = (op1 >> 16)& 0xffff;
        long op1_lo = op1 & 0xffff;
        long op2_hi = (op2 >> 16)& 0xffff;
        long op2_lo = op2 & 0xffff;
        long cross_prod = op1_lo * op2_hi + op1_hi * op2_lo;
        long prod_hi = op1_hi * op2_hi + ((cross_prod >> 16) & 0xffff);
        long prod_lo = op1_lo * op2_lo + ((cross_prod << 16) & 0xffff0000);

        prod_hi += ((prod_lo& 0x80000000)>>31);
        if (s < 0)
            prod_hi = -prod_hi;

        return (int)prod_hi;
    }
}

#define CAL_VEC_X2(advance, tenthradian) \
    mult(itofix(advance), fixcos(ftofix(tenthradian)))

#define CAL_VEC_Y2(advance, tenthradian) \
    mult(itofix(advance), fixsin(ftofix(tenthradian)))

#define CAL_VEC_X(advance, tenthradian)       \
    fixtoi(fixmul(itofix(advance), fixcos(ftofix(tenthradian))))

#define CAL_VEC_Y(advance, tenthradian)       \
    fixtoi(fixmul(itofix(advance), fixsin(ftofix(tenthradian))))


/* global variables for pre-rendered bitmap glyph */
static BITMAP _pre_rdr_bmp;
static void* _pre_rdr_bits = NULL;
static size_t _pre_rdr_bits_size = 0;

/* global variables for subpixel filter */
static RGB* _prev_bk_org = NULL, *_prev_bk;
static RGB* _next_bk_org = NULL, *_next_bk;
static RGB* _curr_bk_org = NULL, *_curr_bk;
static int  _line_bk_size = 0;

/* global variables for subpixel filter */
static BYTE* _scaled_bits = NULL;
static size_t _scaled_bits_size = 0;

BOOL gdi_InitTextBitmapBuffer (void)
{
    return TRUE;
}

void TermTextBitmapBuffer (void)
{
    free (_pre_rdr_bits);
    _pre_rdr_bits = NULL;
    _pre_rdr_bits_size = 0;

    free (_prev_bk);
    free (_next_bk);
    free (_curr_bk);
    _line_bk_size = 0;

    free (_scaled_bits);
    _scaled_bits_size = 0;
}

static BOOL _realloc_bk_buffers (size_t size)
{
    if (size > _line_bk_size) {
        size = ((size + 7) >> 3) << 3;
        _prev_bk_org = (RGB*)realloc (_prev_bk_org, sizeof(RGB)*size);
        _curr_bk_org = (RGB*)realloc (_curr_bk_org, sizeof(RGB)*size);
        _next_bk_org = (RGB*)realloc (_next_bk_org, sizeof(RGB)*size);

        if (!(_prev_bk_org && _next_bk_org && _curr_bk_org)) {
            return FALSE;
        }

        _prev_bk = _prev_bk_org;
        _curr_bk = _curr_bk_org;
        _next_bk = _next_bk_org;
        _line_bk_size = size;
    }

    return TRUE;
}

static BOOL _prepare_pre_rdr_bmp (PDC pdc, int w, int h)
{
    int i;
    BYTE *p;
    Uint32 size, pitch;

    size = GAL_GetBoxSize (pdc->surface, w, h, &pitch);
    if (size > _pre_rdr_bits_size) {
        size = ((size + 31) >> 5) << 5;
        _pre_rdr_bits = realloc (_pre_rdr_bits, size);
        if (_pre_rdr_bits == NULL)
            return FALSE;
        _pre_rdr_bits_size = size;
    }

    /* pls note that bmpfont is one big bitmap, have some glyphs. */
    p = _pre_rdr_bits;
    for (i = 0; i < _pre_rdr_bmp.bmHeight; i++) {
        memcpy (p, _pre_rdr_bmp.bmBits, pitch);
        _pre_rdr_bmp.bmBits += _pre_rdr_bmp.bmPitch;
        p += pitch;
    }
    _pre_rdr_bmp.bmBits  = _pre_rdr_bits;
    _pre_rdr_bmp.bmPitch = pitch;

    return TRUE;
}

static BOOL _prepare_scaled_bits (size_t expected)
{
    if (expected > _scaled_bits_size) {
        expected = ((expected + 31) >> 5) << 5;
        _scaled_bits = (BYTE*)realloc (_scaled_bits, expected);
        if (_scaled_bits == NULL)
            return FALSE;

        _scaled_bits_size = expected;
    }

    memset (_scaled_bits, 0, _scaled_bits_size);
    return TRUE;
}

typedef struct _SCANLINE_CTXT
{
    int   glyph_ascent;
    int   glyph_advance;
    int   glyph_italic;
    int   glyph_line;

    int   bmp_w;
    int   bmp_h;
    int   bmp_pitch;
    int   bmp_scale;

    BYTE* line_bits;
    BYTE* prev_bits;
    GAL_Rect* fg_rc;
} SCANLINE_CTXT;

typedef void (* CB_DRAW_SCANLINE) (PDC pdc, int x, int y, SCANLINE_CTXT* ctxt);

typedef struct _GLYPH_CTXT
{
    BYTE* data;
    int   pitch;
    int   scale;
    int   style;
    CB_DRAW_SCANLINE cb;

} GLYPH_CTXT;

/* global variable for fast check of clipping */
static BOOL _noneed_clip;

#define BITS(b)       (b & (128 >> (x % 8)))
#define PREVBITS(b)   (b & (128 >> ((x-1) % 8)))
#define NEXTBITS(b)   (b & (128 >> ((x+1) % 8)))

/* extented bits get.*/
#define EXPREVBITS(b, x) (b & (128 >> ((x) % 8)))
#define EXNEXTBITS(b, x) (b & (128 >> ((x) % 8)))

#define EX2PREVBITS(b,b2,x)                     \
            if((x % 8) == 0)                    \
                if (EXPREVBITS(b2, x-1))        \
                    weight += NEIGHBOR_WGHT;    \
            else                                \
                if (PREVBITS(b))                \
                    weight += NEIGHBOR_WGHT;

#define EX2NEXTBITS(b,b2,x)                     \
            if((x % 8) == 7)                    \
                if (EXNEXTBITS(b2, x+1))        \
                    weight += NEIGHBOR_WGHT;    \
            else                                \
                if (NEXTBITS(b))                \
                    weight += NEIGHBOR_WGHT;

#define MOVETOPIXEL(x, y) pdc->move_to (pdc, x, y)

static int old_pixel_x = -1;
static int old_pixel_y = -1;
static inline void _glyph_move_to_pixel (PDC pdc, int x, int y)
{
    if (y == old_pixel_y) {
        pdc->step_x (pdc, x - old_pixel_x);
        old_pixel_x = x;
    }
    else {
        pdc->move_to (pdc, x, y);
        old_pixel_x = x;
        old_pixel_y = y;
    }
}

static void _glyph_draw_pixel (PDC pdc, int x, int y, gal_pixel pixel)
{
    gal_pixel old_color;

    if (_noneed_clip) {
        _glyph_move_to_pixel (pdc, x, y);

        old_color = pdc->cur_pixel;
        pdc->cur_pixel = pixel;
        pdc->draw_pixel(PDC_TO_COMP_CTXT(pdc));
        pdc->cur_pixel = old_color;
    }
    else {
        PCLIPRECT cliprect;
        int top;

        if ((cliprect = pdc->cur_ban) == NULL)
            return;

        top = cliprect->rc.top;
        while (cliprect && cliprect->rc.top == top) {
            RECT eff_rc;

            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)) {
                if (PtInRect (&eff_rc, x, y)) {
                    _glyph_move_to_pixel (pdc, x, y);

                    old_color = pdc->cur_pixel;
                    pdc->cur_pixel = pixel;
                    pdc->draw_pixel(PDC_TO_COMP_CTXT(pdc));
                    pdc->cur_pixel = old_color;
                    return;
                }
            }

            cliprect = cliprect->next;
        }
    }
}

static void _glyph_draw_pixels (PDC pdc, int x, int y, gal_pixel pixel, int w)
{
    /* Save pdc cur_pixel color */
    gal_pixel old_color = 0;
    if (_noneed_clip) {
        _glyph_move_to_pixel (pdc, x, y);
        old_color = pdc->cur_pixel;
        pdc->cur_pixel = pixel;
        pdc->draw_pixel_span (PDC_TO_COMP_CTXT(pdc), w);
        pdc->cur_pixel = old_color;
    }
    else {
        PCLIPRECT cliprect;
        int top;

        if ((cliprect = pdc->cur_ban) == NULL)
            return;

        old_color = pdc->cur_pixel;
        pdc->cur_pixel = pixel;

        top = cliprect->rc.top;
        while (cliprect && cliprect->rc.top == top) {
            RECT eff_rc;
            int _x = x, _y = y, _w = w;

            /* no need to check the current cliprect */
#if 0
            if (_x >= cliprect->rc.right) {
                pdc->cur_pixel = old_color;
                return;
            }
#else
            if (_x >= cliprect->rc.right) {
                cliprect = cliprect->next;
                continue;
            }
#endif

            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)
                    && _dc_cliphline (&eff_rc, &_x, &_y, &_w) && _w > 0) {

                _glyph_move_to_pixel (pdc, _x, _y);
                pdc->draw_pixel_span (PDC_TO_COMP_CTXT(pdc), _w);
            }

            cliprect = cliprect->next;
        }

        pdc->cur_pixel = old_color;
    }
    pdc->cur_pixel = old_color;
}

#include "../newgal/blit.h"

static void _glyph_blend_pixel (PDC pdc, int x, int y, gal_pixel pixel)
{
    Uint8* p = (Uint8*)&pixel;
    unsigned sR,sG,sB,sA;
    unsigned dR,dG,dB;
    gal_pixel tmppixel;
    int bpp = pdc->surface->format->BytesPerPixel;
    GAL_PixelFormat *fmt = pdc->surface->format;
    GAL_PixelFormat *srcfmt = pdc->alpha_pixel_format;

    if (PtInRect (&pdc->rc_output, x, y)) {
        _glyph_move_to_pixel (pdc, x, y);
        DISEMBLE_RGBA (p, bpp, srcfmt, tmppixel, sR, sG, sB, sA);
        DISEMBLE_RGB (pdc->cur_dst, bpp, fmt, tmppixel, dR, dG, dB);
        ALPHA_BLEND  (sR, sG, sB, sA, dR, dG, dB);
        tmppixel = GAL_MapRGB (fmt, dR, dG, dB);
        _glyph_draw_pixel (pdc, x, y, tmppixel);
    }
}

static void _glyph_blend_pixels (PDC pdc, int x, int y, gal_pixel pixel, int w)
{
    Uint8* p = (Uint8*)&pixel;
    unsigned sR,sG,sB,sA;
    unsigned dR,dG,dB;
    gal_pixel tmppixel;
    int bpp = pdc->surface->format->BytesPerPixel;
    GAL_PixelFormat *fmt = pdc->surface->format;
    GAL_PixelFormat *srcfmt = pdc->alpha_pixel_format;

    DISEMBLE_RGBA (p, bpp, srcfmt, tmppixel, sR, sG, sB, sA);
    while (w-- > 0) {
        if (PtInRect (&pdc->rc_output, x, y)) {
            _glyph_move_to_pixel (pdc, x, y);
            DISEMBLE_RGB (pdc->cur_dst, bpp, fmt, tmppixel, dR, dG, dB);
            ALPHA_BLEND  (sR, sG, sB, sA, dR, dG, dB);
            tmppixel = GAL_MapRGB (fmt, dR, dG, dB);

            _glyph_draw_pixel (pdc, x, y, tmppixel);
        }
        ++x;
    }
}

static void _dc_bmpfont_scan_line (PDC pdc, int xpos, int ypos,
        SCANLINE_CTXT* ctxt)
{
#if 1
    BITMAP* tmp_bmp  = (BITMAP*)ctxt->line_bits;
    _dc_fillbox_bmp_clip (pdc, ctxt->fg_rc, tmp_bmp);
#else
    GAL_Rect dst_rect;
    BITMAP* tmp_bmp  = (BITMAP*)ctxt->param;
    BITMAP* tmp_bmp  = (BITMAP*)ctxt->line_bits;
    BYTE* bits = tmp_bmp->bmBits;
    tmp_bmp->bmBits  =  ctxt->line_bits;

    dst_rect.x = xpos;
    dst_rect.y = ypos;
    dst_rect.w = xpos + ctxt->w;
    dst_rect.h = 1;

    _dc_fillbox_bmp_clip (pdc, &dst_rect, tmp_bmp);

    tmp_bmp->bmBits = bits;
#endif
}

static inline int get_italic_offx (PDC pdc, int glyph_italic,
            int glyph_ascent, int glyph_line, int glyph_adavnce)
{
    int offx;
    int s1 = 1, s2 = 1;

    if (glyph_italic) {
        offx = (glyph_ascent - glyph_line) >> 1;

        if (pdc->pLogFont->style & FS_FLIP_HORZ)
            s1 = -1;
        if (pdc->pLogFont->style & FS_FLIP_VERT)
            s2 = -1;
        if (s1 * s2 == -1)
            offx = -offx + glyph_adavnce;
    }
    else
        offx = 0;

    return offx;
}

static void _dc_bookgrey_scan_line (PDC pdc, int xpos, int ypos,
        SCANLINE_CTXT* ctxt)
{
    int x = 0;
    int b = 0;
    int off_x;

    if (ctxt->bmp_scale == 1) {

        if (!(_noneed_clip || _dc_which_region_ban (pdc, ypos))) {
            goto nodraw_ret;
        }

        off_x = get_italic_offx (pdc, ctxt->glyph_italic,
                        ctxt->glyph_ascent, ctxt->glyph_line,
                        ctxt->glyph_advance);

        for (x = 0; x < ctxt->bmp_w; x++) {
            b = *(ctxt->line_bits + x);
#ifdef _MGFONT_TTF_GRAYADJUST
            if (b != 0) b += _MGFONT_TTF_GRAYADJUST_VALUE;
            if (b >= 255) b = 16;
            else b >>= 4;
#else
            if (b == 255) b = 16;
            else b >>= 4;
#endif

            if (b == 0) continue;

            if (pdc->alpha_pixel_format && pdc->bkmode == BM_TRANSPARENT) {
                /* pdc->gray_pixels -- alpha(0-max) */
                _glyph_blend_pixel (pdc,
                        x + off_x + xpos, ypos, pdc->gray_pixels [b]);
            }
            else {
                /* pdc->gray_pixels -- from bkcolor to textcolor */
                _glyph_draw_pixel (pdc,
                        x + off_x + xpos, ypos, pdc->gray_pixels [b]);
            }
        }
    }
    else {
        int i = 0;
        int scale = ctxt->bmp_scale;

        for (x = 0; x < ctxt->bmp_w; x++) {
            b = *(ctxt->line_bits+x);
            if (b == 255) b = 16;
            else b >>= 4;

            if (b == 0) continue;

            if (pdc->alpha_pixel_format && pdc->bkmode == BM_TRANSPARENT) {
                for (i = 0; i < scale; i++) {
                    if (!(_noneed_clip || _dc_which_region_ban (pdc, ypos + i)))
                        continue;

                    off_x = get_italic_offx (pdc, ctxt->glyph_italic,
                                ctxt->glyph_ascent, ctxt->glyph_line + i,
                                ctxt->glyph_advance);
                    _glyph_blend_pixels (pdc,
                            x * scale + off_x + xpos, ypos + i, pdc->gray_pixels [b], scale);
                }
            }
            else {
                for (i = 0; i < scale; i++) {
                    if (!(_noneed_clip || _dc_which_region_ban (pdc, ypos + i)))
                        continue;

                    off_x = get_italic_offx (pdc, ctxt->glyph_italic,
                                ctxt->glyph_ascent, ctxt->glyph_line + i,
                                ctxt->glyph_advance);
                    _glyph_draw_pixels (pdc,
                            x * scale + off_x + xpos, ypos + i, pdc->gray_pixels [b], scale);
                }
            }
        }

        ctxt->fg_rc->y += scale - 1;
    }

nodraw_ret:
    ctxt->glyph_line += ctxt->bmp_scale;
}

static void _dc_book_scan_line (PDC pdc, int xpos, int ypos,
        SCANLINE_CTXT* ctxt)
{
    int x = 0;
    int b = 0, n_b = 0, p_b = 0;
    BYTE *curr_sline, *prev_sline, *next_sline;
    int b_up = 0, b_down = 0, n_b_up = 0,
        n_b_down = 0, p_b_up = 0, p_b_down = 0;

    if (!(_noneed_clip || _dc_which_region_ban (pdc, ypos))) {
        if (0 != ctxt->glyph_line)
            goto nodraw_ret;
    }

    xpos += get_italic_offx (pdc, ctxt->glyph_italic,
        ctxt->glyph_ascent, ctxt->glyph_line, ctxt->glyph_advance);

    curr_sline = ctxt->line_bits;
    next_sline = curr_sline + ctxt->bmp_pitch;
    prev_sline = ctxt->prev_bits;

    if(ctxt->glyph_line == 0) {
        int weight;
        _dc_which_region_ban (pdc, ypos);
        for (x = 0; x < ctxt->bmp_w; x++) {
            if (x % 8 == 0){
                if (x > 0) {
                    b_up = *(curr_sline-1);
                    n_b_up = *(next_sline-1);
                }

                b   = *curr_sline++;
                n_b = *next_sline++;

                b_down = b;
                n_b_down = b;
            }
            else if (x % 8 == 7) {
                b_up = b; n_b_up = n_b;
                b_down = *curr_sline; n_b_down = *curr_sline;
            }
            else {
                b_down = b_up = b;
                n_b_down = n_b_up = n_b;
            }

            /* filter the first line */
            weight = 0;
            if (BITS(b))
                weight += 3;
            if (x > 0) {
                if (EXPREVBITS(b_up, x-1)) weight += 1;
            }
            if (x < ctxt->bmp_w - 1) {
                if (EXNEXTBITS(b_down, x+1)) weight += 1;
            }

            if (weight)
                _glyph_blend_pixel (pdc,
                    x + xpos, ypos, pdc->filter_pixels [weight]);
        }

        _dc_which_region_ban (pdc, ypos+1);
        curr_sline = ctxt->line_bits;
        next_sline = curr_sline + ctxt->bmp_pitch;
        for (x = 0; x < ctxt->bmp_w; x++) {
            if (x % 8 == 0){
                if (x > 0) {
                    b_up = *(curr_sline-1);
                    n_b_up = *(next_sline-1);
                }

                b   = *curr_sline++;
                n_b = *next_sline++;

                b_down = b;
                n_b_down = b;
            }
            else if (x % 8 == 7) {
                b_up = b; n_b_up = n_b;
                b_down = *curr_sline; n_b_down = *curr_sline;
            }
            else {
                b_down = b_up = b;
                n_b_down = n_b_up = n_b;
            }

            weight = 0;
            if (BITS(b)) {
                weight = 16;
            }
            else {
                if (x > 0) {
                    if (EXPREVBITS(b_up, x-1)) weight += 3;
                    if (EXPREVBITS(n_b_up, x-1)) weight += 1;
                }
                if (x < ctxt->bmp_w - 1) {
                    if (EXNEXTBITS(b_down, x+1)) weight += 3;
                    if (EXNEXTBITS(n_b_down, x+1)) weight += 1;
                }
                if (BITS(n_b)) weight += 3;
            }

            if (weight)
                _glyph_blend_pixel (pdc,
                    x + xpos, ypos + 1, pdc->filter_pixels [weight]);
        }
        /* set the next scanline position.*/
        ctxt->fg_rc->y++;
    }
    else if (ctxt->glyph_line == ctxt->bmp_h - 1) {
        int weight;
        for (x = 0; x < ctxt->bmp_w; x++) {
            weight = 0;
            if (x % 8 == 0){
                if (x > 0) {
                    b_up = *(curr_sline-1);
                    p_b_up = *(prev_sline-1);
                }
                b   = *curr_sline++;
                p_b = *prev_sline++;
                b_down = b;
                p_b_down = b;
            }
            else if (x % 8 == 7) {
                b_up = b; p_b_up = p_b;
                b_down = *curr_sline; p_b_down = *curr_sline;
            }
            else {
                b_down = b_up = b;
                p_b_down = p_b_up = p_b;
            }

            if (BITS(b)) {
                weight = 16;
            }
            else {
                if (x > 0) {
                    if (EXPREVBITS(b_up, x-1))   weight += 3;
                    if (EXPREVBITS(p_b_up, x-1)) weight += 1;
                }
                if (x < ctxt->bmp_w- 1) {
                    if (EXNEXTBITS(b_down, x+1))   weight += 3;
                    if (EXNEXTBITS(p_b_down, x+1)) weight += 1;
                }
                if (BITS(p_b)) weight += 3;
            }
            if (weight)
                _glyph_blend_pixel (pdc,
                    x + xpos, ypos, pdc->filter_pixels [weight]);

            weight = 0;
            if (BITS(b)) {
                weight += 3;
            }

            if (x > 0) {
                if (EXPREVBITS(b_up, x-1))   weight += 1;
            }
            if (x < ctxt->bmp_w- 1) {
                if (EXNEXTBITS(b_down, x+1))   weight += 1;
            }
            if (weight)
                _glyph_blend_pixel (pdc,
                    x + xpos, ypos + 1, pdc->filter_pixels [weight]);
        }
    }
    else {
        int weight;
        for (x = 0; x < ctxt->bmp_w; x++) {
            if (x % 8 == 0){
                if (x > 0) {
                    b_up   = *(curr_sline-1);
                    n_b_up = *(next_sline-1);
                    p_b_up = *(prev_sline-1);
                }

                b   = *curr_sline++;
                n_b = *next_sline++;
                p_b = *prev_sline++;

                b_down   = b;
                n_b_down = n_b;
                p_b_down = p_b;
            }
            else if (x % 8 == 7) {
                b_up   = b;   b_down   = *curr_sline;
                p_b_up = p_b; p_b_down = *prev_sline;
                n_b_up = n_b; n_b_down = *next_sline;
            }
            else {
                b_down   = b_up   = b;
                n_b_down = n_b_up = n_b;
                p_b_down = p_b_up = p_b;
            }

            weight = 0;
            if (BITS(b)) {
                weight = 16;
            }
            else {
                if (x == 0) {
                    if (BITS(b))       weight += 3;

                    if (BITS(p_b))     weight += 3;
                    if (EXNEXTBITS(p_b_down, x+1)) weight += 1;

                    if (BITS(n_b))     weight += 3;
                    if (EXNEXTBITS(n_b_down, x+1)) weight += 1;
                }
                else if (x == (ctxt->bmp_w- 1)) {
                    if (EXPREVBITS(b_up, x-1))   weight += 3;

                    if (BITS(p_b))     weight += 3;
                    if (EXPREVBITS(p_b_up, x-1)) weight += 1;

                    if (BITS(n_b))     weight += 3;
                    if (EXPREVBITS(n_b_up, x-1)) weight += 1;
                }
                else {
                    if (EXPREVBITS(b_up, x-1))   weight += 3;

                    if (EXNEXTBITS(b_down, x+1))   weight += 3;
                    if (EXPREVBITS(p_b_up, x-1))   weight += 1;
                    if (BITS(p_b))                 weight += 3;
                    if (EXNEXTBITS(p_b_down, x+1)) weight += 1;

                    if (EXPREVBITS(n_b_up, x-1))  weight += 1;
                    if (BITS(n_b))             weight += 3;
                    if (EXNEXTBITS(n_b_down, x+1))  weight += 1;
                }
            }
            if (weight)
                _glyph_blend_pixel (pdc,
                    x + xpos, ypos, pdc->filter_pixels [weight]);
        }
    }

nodraw_ret:
    ctxt->glyph_line++;
}

static void do_light_line(PDC pdc, int xpos, int ypos, SCANLINE_CTXT* ctxt,
        BYTE* c_line, BYTE* p_line, BYTE* n_line)
{
    int x = 0;
    int b = 0, n_b = 0, p_b = 0;
    int b_up = 0, b_down = 0;

    if (c_line) {
        b = *c_line;
    }

    /* note: start draw pixel position is x+1, first and end pixel is
     * filter_pixels[1].*/
    for (x = 0; x < ctxt->bmp_w; x++) {
        if (x % 8 == 0){
            if (x > 0 && c_line) b_up = *(c_line-1);
            else b_up = 0;

            if (c_line) {
                b = *c_line++;
                b_down = b;
            }
            if (n_line) n_b = *n_line++;
            if (p_line) p_b = *p_line++;
        }
        else if (x % 8 == 7) {
            b_up = b;
            if (c_line) b_down = *c_line;
        }
        else {
           b_down = b_up = b;
        }

        if (BITS(b)) {
            /* check first pixel.*/
            if (x == 0)
                _glyph_draw_pixel (pdc,
                        (x) + xpos, ypos, pdc->filter_pixels [1]);
            _glyph_draw_pixel (pdc,
                (x+1) + xpos, ypos, pdc->filter_pixels [2]);
        }
        else if((x > 0 && EXPREVBITS(b_up, x-1)) ||
                (x < ctxt->bmp_w - 1 && EXNEXTBITS(b_down, x+1)) ||
                BITS(p_b) || BITS(n_b)) {
            _glyph_draw_pixel (pdc,
                (x+1) + xpos, ypos, pdc->filter_pixels [1]);
        }
    }

    /* check end pixel.*/
    if (c_line) {
        if (PREVBITS(b))
            _glyph_draw_pixel (pdc,
                (x+1) + xpos, ypos, pdc->filter_pixels [1]);
    }
}

static void _dc_light_scan_line(PDC pdc, int xpos, int ypos,
        SCANLINE_CTXT* ctxt)
{
    BYTE *curr_sline, *prev_sline, *next_sline;

    if (!(_noneed_clip || _dc_which_region_ban (pdc, ypos))) {
        if (ctxt->glyph_line != 0)
            goto nodraw_ret;
    }

    xpos += get_italic_offx (pdc, ctxt->glyph_italic,
        ctxt->glyph_ascent, ctxt->glyph_line, ctxt->glyph_advance);

    curr_sline = ctxt->line_bits;
    next_sline = curr_sline + ctxt->bmp_pitch;
    prev_sline = ctxt->prev_bits;

    if (ctxt->glyph_line == 0) {
        next_sline = curr_sline;

        do_light_line (pdc, xpos, ypos, ctxt,
                NULL, NULL, next_sline);

        /* move ypos to next_line.*/
        ctxt->fg_rc->y++;
        next_sline = curr_sline + ctxt->bmp_pitch;

        _dc_which_region_ban(pdc, ypos+1);
        do_light_line(pdc, xpos, ypos+1, ctxt,
                curr_sline, NULL, next_sline);
    }
    else if (ctxt->glyph_line == ctxt->bmp_h - 1) {

        do_light_line(pdc, xpos, ypos, ctxt,
                curr_sline, prev_sline, NULL);

        /* move ypos to next_line.*/
        ctxt->fg_rc->y++;
        ypos++;
        prev_sline = curr_sline;

        do_light_line(pdc, xpos, ypos, ctxt,
                NULL, prev_sline, NULL);
    }
    else {
        do_light_line(pdc, xpos, ypos, ctxt,
                curr_sline, prev_sline, next_sline);
    }

nodraw_ret:
    ctxt->glyph_line++;
}

#ifdef _MGFONT_FT2
static void _dc_ft2subpixel_scan_line(PDC pdc, int xpos, int ypos,
        SCANLINE_CTXT* ctxt)
{
    Uint32 sub_val;
    int fg_alpha;
    RGB   rgba_cur;
    RGB   rgba_fg;
    gal_pixel pixel = 0;

    int x = 0;
    const BYTE* bits = ctxt->line_bits;

    if (!(_noneed_clip || _dc_which_region_ban (pdc, ypos))) {
        goto nodraw_ret;
    }

    xpos += get_italic_offx (pdc, ctxt->glyph_italic,
                    ctxt->glyph_ascent, ctxt->glyph_line,
                    ctxt->glyph_advance);

    GAL_GetRGB (pdc->textcolor, pdc->surface->format, &rgba_fg.r,
            &rgba_fg.g, &rgba_fg.b);

    for (x = 0; x < ctxt->bmp_w; x++) {
        if (!PtInRect (&pdc->rc_output, x+xpos, ypos)) {
            bits += 3;
            continue;
        }

        // VincentWei: skip null pixel
        if (bits[0] == 0 && bits[1] == 0 && bits[2] == 0) {
            bits += 3;
            continue;
        }

        _glyph_move_to_pixel (pdc, x+xpos, ypos);
        pixel = _mem_get_pixel(pdc->cur_dst, GAL_BytesPerPixel (pdc->surface));

        GAL_GetRGB (pixel, pdc->surface->format, &rgba_cur.r,
                    &rgba_cur.g, &rgba_cur.b);

#define C_ALPHA(p, Cs, Cd)              \
            if (*p++) {                 \
                fg_alpha = *(p-1) + 1;  \
                sub_val  = Cs * fg_alpha + Cd * (256-fg_alpha); \
                Cd = sub_val >> 8;      \
            }

        C_ALPHA(bits, rgba_fg.r, rgba_cur.r);
        C_ALPHA(bits, rgba_fg.g, rgba_cur.g);
        C_ALPHA(bits, rgba_fg.b, rgba_cur.b);

#undef C_ALPHA

        pdc->cur_pixel = GAL_MapRGB (pdc->surface->format, rgba_cur.r,
                    rgba_cur.g, rgba_cur.b);

        if (pdc->cur_pixel != pixel) {
            _glyph_draw_pixel (pdc,
                    x+xpos, ypos, pdc->cur_pixel);
        }
    }

nodraw_ret:
    ctxt->glyph_line += ctxt->bmp_scale;
}

#endif /* _MGFONT_FT2 */

#define C_WEIGHT(c1, c2)                                        \
    sub_val = (c1 * (ALL_WGHT-weight) + c2 * weight)/ALL_WGHT;  \
    c1 = sub_val;

#define CALCPIXEL(Cs, Cd)       \
    C_WEIGHT(Cs.r, Cd.r);       \
    C_WEIGHT(Cs.g, Cd.g);       \
    C_WEIGHT(Cs.b, Cd.b);       \
    Cs.a = 255;

#define GET_RGBA(Cs, x, y)                  \
{                                           \
    unsigned char dR,dG,dB,dA;              \
    gal_pixel tmppixel;                     \
    MOVETOPIXEL(x, y);                      \
    DISEMBLE_RGBA (pdc->cur_dst,            \
        pdc->surface->format->BytesPerPixel,\
        pdc->surface->format,               \
        tmppixel, dR, dG, dB, dA);          \
    Cs.r=dR; Cs.g=dG; Cs.b=dB; Cs.a=0;      \
}

static void do_subpixel_line(PDC pdc, int xpos, int ypos, SCANLINE_CTXT* ctxt,
        BYTE* curr_line, BYTE* prev_line, BYTE* next_line, RGB rgba_fg)
{
    int x = 0;
    int weight;
    Uint32 sub_val;
    RGB rgba_cur,  rgba_cur_p,  rgba_cur_n;
    RGB rgba_prev, rgba_prev_p, rgba_prev_n;
    RGB rgba_next, rgba_next_p, rgba_next_n;

    int b, n_b, p_b;
    int b_up = 0, b_down = 0, n_b_up = 0,
        n_b_down = 0, p_b_up = 0, p_b_down = 0;

    b   = *curr_line++;
    n_b = *next_line++;
    p_b = *prev_line++;
    b_up   = b;   b_down   = b;
    p_b_up = p_b; p_b_down = p_b;
    n_b_up = n_b; n_b_down = n_b;

    rgba_cur  = rgba_cur_p  = rgba_cur_n  = rgba_fg;
    rgba_prev = rgba_prev_p = rgba_prev_n = rgba_fg;
    rgba_next = rgba_next_p = rgba_next_n = rgba_fg;

    if (!BITS(b)) {
        weight = 0;

        if (NEXTBITS(b))
            weight += NEIGHBOR_WGHT;
        if (BITS(p_b))
            weight += NEIGHBOR_WGHT;
        if (BITS(n_b))
            weight += NEIGHBOR_WGHT;

        if (NEXTBITS(p_b))
            weight += NEAR_WGHT;
        if (NEXTBITS(n_b))
            weight += NEAR_WGHT;

        rgba_cur = _curr_bk[x];
        CALCPIXEL(rgba_cur, rgba_fg);
        pdc->cur_pixel = GAL_MapRGB(pdc->surface->format, rgba_cur.r,
                rgba_cur.g, rgba_cur.b);
        _glyph_draw_pixel (pdc,
            x + xpos, ypos, pdc->cur_pixel);
    }
    else {
        if (!BITS(b))
            rgba_cur = _curr_bk[x];
        if (!NEXTBITS(b))
            rgba_cur_n = _curr_bk[x+1];

        if (!BITS(p_b))
            rgba_prev = _prev_bk[x];
        if (!NEXTBITS(p_b))
            rgba_prev_n = _prev_bk[x+1];

        if (!BITS(n_b))
            rgba_next = _next_bk[x];
        if (!NEXTBITS(n_b))
            rgba_next_n = _next_bk[x+1];

#define C_WEIGHT2(c) sub_val = \
        rgba_cur.c * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT); \
        sub_val += rgba_cur_n.c * NEIGHBOR_WGHT; \
        sub_val += rgba_prev.c * NEIGHBOR_WGHT; \
        sub_val += rgba_next.c * NEIGHBOR_WGHT;\
        sub_val += rgba_prev_n.c * NEAR_WGHT;     \
        sub_val += rgba_next_n.c * NEAR_WGHT;    \
        rgba_cur.c = sub_val/ALL_WGHT;

        C_WEIGHT2(r);
        C_WEIGHT2(g);
        C_WEIGHT2(b);
        pdc->cur_pixel = GAL_MapRGB(pdc->surface->format, rgba_cur.r,
                rgba_cur.g, rgba_cur.b);
        _glyph_draw_pixel (pdc,
            x + xpos, ypos, pdc->cur_pixel);
    }

    for (x = 1; x < (ctxt->bmp_w-1); x++) {
        if (x % 8 == 0){
            b_up   = *(curr_line-1);
            p_b_up = *(prev_line-1);
            n_b_up = *(next_line-1);

            b   = *curr_line++;
            n_b = *next_line++;
            p_b = *prev_line++;

            b_down   = b;
            p_b_down = p_b;
            n_b_down = n_b;
        }
        else if (x % 8 == 7) {
            b_up   = b;   b_down   = *curr_line;
            p_b_up = p_b; p_b_down = *prev_line;
            n_b_up = n_b; n_b_down = *next_line;
        }
        else {
           b_down   = b_up   = b;
           p_b_down = p_b_up = p_b;
           n_b_down = n_b_up = n_b;
        }

        if (!BITS(b)) {
            weight = 0;
            if (BITS(b))
                weight += NEIGHBOR_WGHT;
            if (EXNEXTBITS(b_down, x+1))
                weight += NEIGHBOR_WGHT;

            if (BITS(p_b))
                weight += NEIGHBOR_WGHT;
            if (BITS(n_b))
                weight += NEIGHBOR_WGHT;

            if (EXPREVBITS(p_b_up, x-1))
                weight += NEAR_WGHT;
            if (EXNEXTBITS(p_b_down, x+1))
                weight += NEAR_WGHT;

            if (EXPREVBITS(n_b_up, x-1))
                weight += NEAR_WGHT;
            if (EXNEXTBITS(n_b_down, x+1))
                weight += NEAR_WGHT;

            rgba_cur = _curr_bk[x];
            CALCPIXEL(rgba_cur, rgba_fg);
            pdc->cur_pixel = GAL_MapRGB(pdc->surface->format, rgba_cur.r,
                    rgba_cur.g, rgba_cur.b);
            _glyph_draw_pixel (pdc,
                x + xpos, ypos, pdc->cur_pixel);
        }
        else {
            if (!BITS(b))
                rgba_cur = _curr_bk[x];
            else
                rgba_cur = rgba_fg;

            if (!EXPREVBITS(b_up, x-1))
                rgba_cur_p = _curr_bk[x-1];
            else
                rgba_cur_p = rgba_fg;
            if (!EXNEXTBITS(b_down, x+1))
                rgba_cur_n = _curr_bk[x+1];
            else
                rgba_cur_n = rgba_fg;

            if (!BITS(p_b))
                rgba_prev = _prev_bk[x];
            else
                rgba_prev = rgba_fg;
            if (!EXPREVBITS(p_b_up, x-1))
                rgba_prev_p = _prev_bk[x-1];
            else
                rgba_prev_p = rgba_fg;
            if (!EXNEXTBITS(p_b_down, x+1))
                rgba_prev_n = _prev_bk[x+1];
            else
                rgba_prev_n = rgba_fg;

            if (!BITS(n_b))
                rgba_next = _next_bk[x];
            else
                rgba_next = rgba_fg;
            if (!EXPREVBITS(n_b_up, x-1))
                rgba_next_p = _next_bk[x-1];
            else
                rgba_next_p = rgba_fg;
            if (!EXNEXTBITS(n_b_down, x+1))
                rgba_next_n = _next_bk[x+1];
            else
                rgba_next_n = rgba_fg;

#define C_WEIGHT3(c) sub_val = rgba_cur.c * (ALL_WGHT-ROUND_WGHT); \
            sub_val += rgba_cur_p.c * NEIGHBOR_WGHT; \
            sub_val += rgba_cur_n.c * NEIGHBOR_WGHT; \
            sub_val += rgba_prev.c * NEIGHBOR_WGHT;  \
            sub_val += rgba_next.c * NEIGHBOR_WGHT;  \
            sub_val += rgba_prev_p.c * NEAR_WGHT;    \
            sub_val += rgba_next_p.c * NEAR_WGHT;    \
            sub_val += rgba_prev_n.c * NEAR_WGHT;    \
            sub_val += rgba_next_n.c * NEAR_WGHT;    \
            rgba_cur.c = sub_val/ALL_WGHT;

            rgba_cur = rgba_fg;
            C_WEIGHT3(r);
            C_WEIGHT3(g);
            C_WEIGHT3(b);

            rgba_cur.a = 255;
            pdc->cur_pixel = GAL_MapRGB(pdc->surface->format, rgba_cur.r,
                    rgba_cur.g, rgba_cur.b);
            _glyph_draw_pixel (pdc,
                x + xpos, ypos, pdc->cur_pixel);
        }
    }

    if (x % 8 == 0){
        b   = *curr_line++;
        n_b = *next_line++;
        p_b = *prev_line++;
    }
    if (!BITS(b)) {
        weight = 0;
        if (PREVBITS(b))
            weight += NEIGHBOR_WGHT;
        if (BITS(p_b))
            weight += NEIGHBOR_WGHT;
        if (BITS(n_b))
            weight += NEIGHBOR_WGHT;

        if (PREVBITS(p_b))
            weight += NEAR_WGHT;
        if (PREVBITS(n_b))
            weight += NEAR_WGHT;

        rgba_cur = _curr_bk[0];
        CALCPIXEL(rgba_cur, rgba_fg);
        pdc->cur_pixel = GAL_MapRGB(pdc->surface->format, rgba_cur.r,
                rgba_cur.g, rgba_cur.b);
        _glyph_draw_pixel (pdc,
            x + xpos, ypos, pdc->cur_pixel);
    }
    else{
        if (!BITS(b))
            rgba_cur = _curr_bk[x];
        else
            rgba_cur = rgba_fg;

        if (!PREVBITS(b))
            rgba_cur_p = _curr_bk[x-1];
        else
            rgba_cur_p = rgba_fg;

        if (!BITS(p_b))
            rgba_prev = _prev_bk[x];
        else
            rgba_prev = rgba_fg;
        if (!PREVBITS(p_b))
            rgba_prev_p = _prev_bk[x-1];
        else
            rgba_prev_p = rgba_fg;

        if (!BITS(n_b))
            rgba_next = _next_bk[x];
        else
            rgba_next = rgba_fg;

        if (!PREVBITS(n_b))
            rgba_next_p = _next_bk[x-1];
        else
            rgba_next_p = rgba_fg;

#define C_WEIGHT4(c) sub_val = rgba_cur.c * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);\
        sub_val += rgba_cur_p.c * NEIGHBOR_WGHT; \
        sub_val += rgba_prev.c * NEIGHBOR_WGHT;  \
        sub_val += rgba_next.c * NEIGHBOR_WGHT;  \
        sub_val += rgba_prev_p.c * NEAR_WGHT;    \
        sub_val += rgba_next_p.c * NEAR_WGHT;    \
        rgba_cur.c = sub_val/ALL_WGHT;

        C_WEIGHT4(r);
        C_WEIGHT4(g);
        C_WEIGHT4(b);
        rgba_cur.a = 255;
        pdc->cur_pixel = GAL_MapRGB(pdc->surface->format, rgba_cur.r,
                rgba_cur.g, rgba_cur.b);
        _glyph_draw_pixel (pdc,
            x + xpos, ypos, pdc->cur_pixel);
    }
}

static void _dc_subpixel_scan_line(PDC pdc, int xpos, int ypos,
        SCANLINE_CTXT* ctxt)
{
    BYTE *curr_line, *prev_line, *next_line;
    RGB rgba_fg;
    int i = 0;
    RGB* p  = NULL;

    if (!_realloc_bk_buffers (ctxt->bmp_w))
        goto nodraw_ret;

    if(ctxt->glyph_line == 0) {
        p  = _prev_bk;
        for (i = 0; i < ctxt->bmp_w; i++){
            GET_RGBA(_curr_bk[i], xpos+i, ypos);
            GET_RGBA(_next_bk[i], xpos+i, ypos+1);
        }
        _prev_bk = _curr_bk;
    }
    else if(ctxt->glyph_line == ctxt->bmp_h-1) {
        p  = _prev_bk;
        _prev_bk = _curr_bk;
        _curr_bk = _next_bk;
    }
    else {
        p  = _prev_bk;
        _prev_bk = _curr_bk;
        _curr_bk = _next_bk;
        for (i = 0; i < ctxt->bmp_w; i++){
            GET_RGBA(p[i], xpos+i, ypos+1);
        }
        _next_bk = p;
    }

    if (!(_noneed_clip || _dc_which_region_ban (pdc, ypos))) {
        goto nodraw_ret;
    }

    xpos += get_italic_offx (pdc, ctxt->glyph_italic,
        ctxt->glyph_ascent, ctxt->glyph_line, ctxt->glyph_advance);

    GAL_GetRGBA (pdc->textcolor, pdc->surface->format, &rgba_fg.r,
            &rgba_fg.g, &rgba_fg.b, &rgba_fg.a);

    curr_line = ctxt->line_bits;
    next_line = curr_line + ctxt->bmp_pitch;
    prev_line = ctxt->prev_bits;

    /* note: restore pointer for realloc (_prev_bk...).*/
    if(ctxt->glyph_line == 0) {
        do_subpixel_line(pdc, xpos, ypos, ctxt,
                curr_line, curr_line, next_line, rgba_fg);
        _prev_bk = p;
    }
    else if(ctxt->glyph_line == ctxt->bmp_h-1) {
        do_subpixel_line(pdc, xpos, ypos, ctxt,
                curr_line, prev_line, curr_line, rgba_fg);
        _next_bk = p;
    }
    else {
        do_subpixel_line(pdc, xpos, ypos, ctxt,
                curr_line, prev_line, next_line, rgba_fg);
    }

nodraw_ret:
    if(ctxt->glyph_line == 0) {
        _prev_bk = p;
    }
    else if(ctxt->glyph_line == ctxt->bmp_h-1) {
        _next_bk = p;
    }
    ctxt->glyph_line++;
}

static inline BOOL _get_bit_runlength (const BYTE* line, int offset, int *runlength, int nr_left_bits)
{
    BOOL bit;
    int i;

    line += offset >> 3;
    if (*line & (0x80 >> (offset % 8)))
        bit = TRUE;
    else
        bit = FALSE;

    *runlength = 1;
    for (i = 1; i < nr_left_bits; i++) {
        (offset)++;
        if ((offset % 8) == 0)
            line ++;

        if ((*line & (0x80 >> (offset % 8))) && bit)
            (*runlength) ++;
        else if (!(*line & (0x80 >> (offset % 8))) && !bit)
            (*runlength) ++;
        else
            break;
    }

    return bit;
}

#if 0
/* used for horizontal flip */
static void _dc_regular_scan_line_slow (PDC pdc, int xpos, int ypos,
        SCANLINE_CTXT* ctxt)
{
    int x = 0;
    const BYTE* bits = ctxt->line_bits;
    BYTE b;

    if (ctxt->bmp_scale == 1) {
        b = *bits;
        for (x = 0; x < ctxt->bmp_w; x++) {
            if (x % 8 == 0)
                b = *bits++;
            if ((b & (128 >> (x % 8)))) {
                _glyph_draw_pixel (pdc,
                    x + xpos, ypos, pdc->textcolor);
            }
            else{
                if (pdc->bkmode == BM_TRANSPARENT)
                    continue;

                _glyph_draw_pixel (pdc,
                    x + xpos, ypos, pdc->bkcolor);
            }
        }
    }
    else {
        int i = 0;
        int scale = ctxt->bmp_scale;

        b = *bits;
        for (x = 0; x < ctxt->bmp_w; x++) {
            if (x % 8 == 0)
                b = *bits++;
            if ((b & (128 >> (x % 8)))) {
                for (i = 0; i < scale; i++) {
                    _glyph_draw_pixels (pdc,
                        x*scale + xpos, ypos+i, pdc->textcolor, scale);
                }
            }
            else {
                if (pdc->bkmode == BM_TRANSPARENT)
                    continue;

                for (i = 0; i < scale; i++) {
                    _glyph_draw_pixels (pdc,
                        x*scale + xpos, ypos+i, pdc->bkcolor, scale);
                }
            }
        }
        ctxt->fg_rc->y += scale-1;
    }
}
#endif

static void _dc_regular_scan_line (PDC pdc, int xpos, int ypos,
        SCANLINE_CTXT* ctxt)
{
    int x = 0, runlength, w = ctxt->bmp_w;
    const BYTE* bits = ctxt->line_bits;

    if (ctxt->bmp_scale == 1) {

        if (!(_noneed_clip || _dc_which_region_ban (pdc, ypos))) {
            goto nodraw_ret;
        }

        xpos += get_italic_offx (pdc, ctxt->glyph_italic,
                ctxt->glyph_ascent, ctxt->glyph_line, ctxt->glyph_advance);

        do {
            if (_get_bit_runlength (bits, x, &runlength, w)) {
                _glyph_draw_pixels (pdc,
                    x + xpos, ypos, pdc->textcolor, runlength);
            }

            w -= runlength;
            x += runlength;
        } while (w > 0);
    }
    else {
        int i = 0, off_x;
        int scale = ctxt->bmp_scale;

        do {
            if (_get_bit_runlength (bits, x, &runlength, w)) {
                for (i = 0; i < scale; i++) {
                    if (!(_noneed_clip || _dc_which_region_ban (pdc, ypos + i))) {
                        continue;
                    }

                    off_x = get_italic_offx (pdc, ctxt->glyph_italic,
                            ctxt->glyph_ascent, ctxt->glyph_line + i, ctxt->glyph_advance);
                    _glyph_draw_pixels (pdc,
                        x*scale + off_x + xpos, ypos + i,
                        pdc->textcolor, scale * runlength);
                }
            }

            w -= runlength;
            x += runlength;
        } while (w > 0);

        ctxt->fg_rc->y += scale-1;
    }

nodraw_ret:
    ctxt->glyph_line += ctxt->bmp_scale;
}

static inline BOOL _check_bit_in_line (const BYTE* line, int offset)
{
    line += offset >> 3;
    return *line & (0x80 >> (offset % 8));
}

static inline void _set_bit_in_line (BYTE* line, int offset)
{
    line += offset >> 3;
    *line |= (0x80 >> (offset % 8));
}

static BYTE* _gdi_expand_bold_bits (const BYTE* src_bits, SIZE* size, int *pitch,
        int scale)
{
    BYTE* dst_line;
    const BYTE* src_line;
    int scaled_pitch;
    int i, j, w = size->cx / scale, h = size->cy / scale;

    scaled_pitch = (w + 7) / 8;

    _prepare_scaled_bits (scaled_pitch * (h + 1));
    if (_scaled_bits == NULL)
        return NULL;

    src_line = src_bits;
    dst_line = _scaled_bits;

    for (i = 0; i < h; i ++) {
        for (j = 0; j < w - 1; j++) {

            if (_check_bit_in_line (src_line, j)) {
                _set_bit_in_line (dst_line, j);

                /* bold */
                if (j == (w - 1) || !_check_bit_in_line (src_line, j+1))
                    _set_bit_in_line (dst_line, j + 1);
            }
        }

        dst_line += scaled_pitch;
        src_line += *pitch;
    }

    *pitch = scaled_pitch;
    return _scaled_bits;
}

static BYTE* _gdi_expand_scale_bits (const BYTE* src_bits, SIZE* size, int pitch,
        int bold, int scale)
{
    BYTE* dst_line;
    const BYTE* src_line;
    int scaled_pitch = (size->cx + 7) / 8;
    int i, j, w = size->cx / scale, h = size->cy / scale;

    _prepare_scaled_bits (scaled_pitch * (size->cy + 1));
    if (_scaled_bits == NULL)
        return NULL;

    src_line = src_bits;
    dst_line = _scaled_bits;

    if (bold) {
        w--;
    }

    for (i  = 0; i < h; i ++) {
        for (j = 0; j < w; j++) {
            int k;
            if (_check_bit_in_line (src_line, j)) {
                for (k = 0; k < scale; k++)
                    _set_bit_in_line (dst_line, j * scale + k);

                /* bold */
                if (bold && (j == (w - 1) || !_check_bit_in_line (src_line, j+1))) {
                    for (k = 0; k < scale; k++)
                        _set_bit_in_line (dst_line, (j + 1) * scale + k);
                }
            }

            for (k = 1; k < scale; k++) {
                memcpy (dst_line + scaled_pitch * k, dst_line, scaled_pitch);
            }
        }

        dst_line += scaled_pitch * scale;
        src_line += pitch;
    }

    return _scaled_bits;
}

static BYTE* _gdi_flip_monobitmap_horz (BYTE* src_bits, const SIZE* size, int pitch)
{
    BYTE* dst_line;
    const BYTE* src_line;
    int i, j;
    BYTE* inter_line;

    src_line = src_bits;
    if (src_bits != _scaled_bits) {
        _prepare_scaled_bits (pitch * (size->cy + 1));
        if (_scaled_bits == NULL)
            return NULL;

        inter_line = NULL;
        dst_line = _scaled_bits;
    }
    else {
        inter_line = src_bits + (pitch * size->cy);
        dst_line = src_bits;
    }

    for (i = 0; i < size->cy; i++) {
        const BYTE* test_line;
        if (inter_line) {
            memcpy (inter_line, src_line, pitch);
            memset (dst_line, 0, pitch);
            test_line = inter_line;
        }
        else {
            test_line = src_line;
        }

        for (j = 0; j < size->cx; j++) {
            if (_check_bit_in_line (test_line, j)) {
                _set_bit_in_line (dst_line, size->cx - j - 1);
            }
        }

        dst_line += pitch;
        src_line += pitch;
    }

    return _scaled_bits;
}

static BYTE* _gdi_flip_greybitmap_horz (BYTE* src_bits, const SIZE* size, int pitch)
{
    BYTE* dst_line;
    const BYTE* src_line;
    int i, j;
    BYTE* inter_line;

    src_line = src_bits;
    if (src_bits != _scaled_bits) {
        _prepare_scaled_bits (pitch * (size->cy + 1));
        if (_scaled_bits == NULL)
            return NULL;

        inter_line = NULL;
        dst_line = _scaled_bits;
    }
    else {
        inter_line = src_bits + (pitch * size->cy);
        dst_line = src_bits;
    }

    for (i = 0; i < size->cy; i ++) {
        const BYTE* test_line;
        if (inter_line) {
            memcpy (inter_line, src_line, pitch);
            test_line = inter_line;
        }
        else {
            test_line = src_line;
        }

        for (j = 0; j < size->cx; j++) {
            dst_line [j] = test_line [size->cx - j - 1];
        }

        dst_line += pitch;
        src_line += pitch;
    }

    return _scaled_bits;
}

static BYTE* _gdi_flip_subpixels_horz (BYTE* src_bits, const SIZE* size, int pitch)
{
    BYTE* dst_line;
    const BYTE* src_line;
    int i, j;
    BYTE* inter_line;

    src_line = src_bits;
    if (src_bits != _scaled_bits) {
        _prepare_scaled_bits (pitch * (size->cy + 1));
        if (_scaled_bits == NULL)
            return NULL;

        inter_line = NULL;
        dst_line = _scaled_bits;
    }
    else {
        inter_line = src_bits + (pitch * size->cy);
        dst_line = src_bits;
    }

    src_line = src_bits;
    dst_line = _scaled_bits;
    for (i = 0; i < size->cy; i ++) {
        const BYTE* test_line;
        if (inter_line) {
            memcpy (inter_line, src_line, pitch);
            test_line = inter_line;
        }
        else {
            test_line = src_line;
        }

        for (j = 0; j < size->cx; j++) {
            dst_line [j*3+0] = test_line [(size->cx - j - 1)*3+0];
            dst_line [j*3+1] = test_line [(size->cx - j - 1)*3+1];
            dst_line [j*3+2] = test_line [(size->cx - j - 1)*3+2];
        }

        dst_line += pitch;
        src_line += pitch;
    }

    return _scaled_bits;
}

static BYTE* _gdi_flip_bitmap_vert (BYTE* src_bits, const SIZE* size, int pitch)
{
    BYTE* dst_line;
    BYTE* src_line;
    int i;
    BYTE* inter_line;

    src_line = src_bits;
    if (src_bits != _scaled_bits) {
        _prepare_scaled_bits (pitch * (size->cy + 1));
        if (_scaled_bits == NULL)
            return NULL;

        dst_line = _scaled_bits + pitch * size->cy;
        inter_line = NULL;

        for (i = 0; i < size->cy; i++) {
            dst_line -= pitch;
            memcpy (dst_line, src_line, pitch);
            src_line += pitch;
        }
    }
    else {
        dst_line = src_bits + pitch * size->cy;
        inter_line = src_bits + pitch * size->cy;

        for (i = 0; i < (size->cy >> 1); i++) {
            dst_line -= pitch;
            memcpy (inter_line, src_line, pitch);
            memcpy (src_line, dst_line, pitch);
            memcpy (dst_line, inter_line, pitch);
            src_line += pitch;
        }
    }

    return _scaled_bits;
}

static BOOL _gdi_get_glyph_data (PDC pdc, Glyph32 glyph_value,
        SIZE* bbox, int bold, GLYPH_CTXT* ctxt)
{
    int pitch = 0;
    unsigned short scale = 1;
    BYTE* data = NULL;
    PLOGFONT logfont = pdc->pLogFont;
    DEVFONT* devfont = SELECT_DEVFONT_BY_GLYPH(pdc->pLogFont, glyph_value);
    glyph_value = REAL_GLYPH(glyph_value);

    DWORD bmptype = devfont->font_ops->get_glyph_bmptype (logfont, devfont);
    switch (bmptype & DEVFONTGLYPHTYPE_MASK_BMPTYPE) {
    case GLYPHBMP_TYPE_MONO:
        if ((logfont->style & FS_RENDER_MASK) == FS_RENDER_GREY) {
            if (pdc->alpha_pixel_format)
                ctxt->cb = _dc_book_scan_line;
        }
        else if ((logfont->style & FS_RENDER_MASK) == FS_RENDER_SUBPIXEL) {
            ctxt->cb = _dc_subpixel_scan_line;
        }
        else if (logfont->style & FS_DECORATE_OUTLINE) {
            if (pdc->textcolor != pdc->bkcolor)
                ctxt->cb = _dc_light_scan_line;
        }

        if (!ctxt->cb) {
            ctxt->cb = _dc_regular_scan_line;
        }

        data = (BYTE*)(*devfont->font_ops->get_glyph_monobitmap) (logfont,
                devfont, glyph_value, bbox, &pitch, &scale);
        bbox->cx += bold;

        if (data == NULL)
            break;

        if (scale > 1 && ctxt->cb != _dc_regular_scan_line) {
            data = _gdi_expand_scale_bits (data, bbox, pitch, bold, scale);
            /* note: pitch is modified. */
            pitch = (bbox->cx + 7) / 8;
        }
        else if (bold) {
            data = _gdi_expand_bold_bits (data, bbox, &pitch, scale);
            bbox->cx = bbox->cx / scale;
            bbox->cy = bbox->cy / scale;
        }
        else if (scale > 1 && (ctxt->cb == _dc_regular_scan_line)) {
            bbox->cx = bbox->cx / scale;
            bbox->cy = bbox->cy / scale;
        }

        if (data == NULL)
            break;

        /* flip the monobitmap */
        if (logfont->style & FS_FLIP_HORZ)
            data = _gdi_flip_monobitmap_horz (data, bbox, pitch);
        if (logfont->style & FS_FLIP_VERT)
            data = _gdi_flip_bitmap_vert (data, bbox, pitch);
        break;

    case GLYPHBMP_TYPE_GREY:
        /* get preybitmap */
        if (devfont->font_ops->get_glyph_greybitmap) {
            data = (BYTE*)(*devfont->font_ops->get_glyph_greybitmap) (logfont,
                    devfont, glyph_value, bbox, &pitch, &scale);
            ctxt->cb = _dc_bookgrey_scan_line;
            if (data && scale > 1) {
                bbox->cx = bbox->cx / scale;
                bbox->cy = bbox->cy / scale;
            }
        }

        /* flip the greybitmap */
        if (logfont->style & FS_FLIP_HORZ)
            data = _gdi_flip_greybitmap_horz (data, bbox, pitch);
        if (logfont->style & FS_FLIP_VERT)
            data = _gdi_flip_bitmap_vert (data, bbox, pitch);
        break;

    case GLYPHBMP_TYPE_SUBPIXEL:
#ifdef _MGFONT_FT2
        if (ft2IsFreeTypeDevfont (devfont) &&
                ft2GetLcdFilter (devfont) &&
                *devfont->font_ops->get_glyph_greybitmap) {
            /* the returned bits will be the subpixled pixmap */
            data = (BYTE*)(*devfont->font_ops->get_glyph_greybitmap) (logfont,
                    devfont, glyph_value, bbox, &pitch, &scale);
            // the width of SUBPIXEL bitmap is 3x real width of bbox
            bbox->cx /= 3;
            ctxt->cb = _dc_ft2subpixel_scan_line;

            /* flip the subpixeled pixmap */
            if (logfont->style & FS_FLIP_HORZ)
                data = _gdi_flip_subpixels_horz (data, bbox, pitch);
            if (logfont->style & FS_FLIP_VERT)
                data = _gdi_flip_bitmap_vert (data, bbox, pitch);
        }
#endif
        break;

    case GLYPHBMP_TYPE_PRERENDER:
        if (devfont->font_ops->get_glyph_prbitmap != NULL) {
            PBITMAP glyph_bmp = &_pre_rdr_bmp;
            if (!(devfont->font_ops->get_glyph_prbitmap) (logfont, devfont,
                        glyph_value, glyph_bmp)) {
                return FALSE;
            }

            /* FLIP for pre-rendered bitmap glyph. */
            if ((logfont->style & FS_FLIP_HORZ) ||
                    (logfont->style & FS_FLIP_VERT)) {

                _prepare_pre_rdr_bmp (pdc,
                    glyph_bmp->bmWidth, glyph_bmp->bmHeight + 1);

                if (logfont->style & FS_FLIP_HORZ) {
                    HFlipBitmap (glyph_bmp, glyph_bmp->bmBits
                            + glyph_bmp->bmPitch * glyph_bmp->bmHeight);
                }
                if (logfont->style & FS_FLIP_VERT) {
                    VFlipBitmap (glyph_bmp, glyph_bmp->bmBits
                            + glyph_bmp->bmPitch * glyph_bmp->bmHeight);
                }
            }

            data = (BYTE*)glyph_bmp;
            ctxt->cb = _dc_bmpfont_scan_line;
        }
        break;
    }

    if (!data || !ctxt->cb)
        return FALSE;

#if 0
    if (ctxt->cb != _dc_bmpfont_scan_line && logfont->style & FS_FLIP_VERT) {
        data  = data + (bbox->cy-1) * pitch;
        pitch = -pitch;
    }
#endif

    ctxt->data  = data;
    ctxt->pitch = pitch;
    ctxt->scale = scale;
    ctxt->style = logfont->style;

    return TRUE;
}

static BOOL _gdi_ifneed_draw_glyph (PDC pdc, GLYPH_CTXT* ctxt, const GAL_Rect* fg_gal_rc)
{
    BOOL has_inter = FALSE;
    PCLIPRECT cliprect;
    RECT eff_rc;
    RECT glyph_rc;

    /* pdc->rc_output is big than fg_gal_rc. */
    glyph_rc.left   = fg_gal_rc->x;
    glyph_rc.top    = fg_gal_rc->y;
    glyph_rc.right  = fg_gal_rc->x + fg_gal_rc->w;
    glyph_rc.bottom = fg_gal_rc->y + fg_gal_rc->h;

    _noneed_clip = FALSE;
    has_inter   = FALSE;
    cliprect    = pdc->ecrgn.head;
    while (cliprect) {
        if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)) {
            if (EqualRect(&glyph_rc, &eff_rc) ||
                    IsCovered(&glyph_rc, &eff_rc)){
                _noneed_clip = TRUE;
                has_inter   = TRUE;
                break;
            }
            else if (IntersectRect (&eff_rc, &eff_rc, &glyph_rc)) {
                has_inter = TRUE;
                break;
            }
        }
        cliprect = cliprect->next;
    }

    if (!has_inter)
        return FALSE;

    return TRUE;
}

static inline void _gdi_direct_fillglyph (PDC pdc, Glyph32 glyph_value,
        GAL_Rect* fg_gal_rc, SIZE* bbox,
        int glyph_ascent, int glyph_advance, int italic, int bold)
{
    BOOL exist = FALSE;
    GLYPH_CTXT  g_ctxt = {0};
    GLYPH_CTXT* ctxt = &g_ctxt;
    BOOL if_draw_glyph = _gdi_ifneed_draw_glyph (pdc, ctxt, fg_gal_rc);

    if (!if_draw_glyph) return;

    exist = _gdi_get_glyph_data (pdc, glyph_value, bbox, bold, &g_ctxt);

    if (exist) {
        SCANLINE_CTXT sl_ctxt;

        sl_ctxt.glyph_ascent  = glyph_ascent;
        sl_ctxt.glyph_advance = glyph_advance;
        sl_ctxt.glyph_italic  = italic;
        sl_ctxt.glyph_line = 0;
        sl_ctxt.bmp_w      = bbox->cx;
        sl_ctxt.bmp_h      = bbox->cy;
        sl_ctxt.bmp_pitch  = ctxt->pitch;
        sl_ctxt.bmp_scale  = ctxt->scale;
        sl_ctxt.fg_rc      = fg_gal_rc;
        sl_ctxt.line_bits  = ctxt->data;

        if (ctxt->cb == _dc_bmpfont_scan_line) {
            ctxt->cb (pdc, fg_gal_rc->x, fg_gal_rc->y, &sl_ctxt);
        }
        else {
            int y, real_y;
            for (y = 0; y < sl_ctxt.bmp_h; y++) {
                real_y = fg_gal_rc->y + y;
                ctxt->cb (pdc, fg_gal_rc->x, real_y, &sl_ctxt);
                sl_ctxt.prev_bits = sl_ctxt.line_bits;
                sl_ctxt.line_bits += ctxt->pitch;
            }
        }

#if 0
        /* optimized by ymwei */
        _gdi_free_glyph_data (pdc, glyph_value, ctxt);
#else
        /* reset the old_pixel info */
        old_pixel_x = -1;
        old_pixel_y = -1;
#endif
    }
}

void _gdi_get_rotated_point(int *x, int *y, int rotation)
{
    int advance = fixtoi(fixsqrt(fixadd(itofix(*x * *x), itofix(*y * *y))));

    *x += CAL_VEC_X2(advance, ((rotation+900)%3600+3600)%3600
            *256/3600);
    *y -= CAL_VEC_Y2(advance, ((rotation+900)%3600+3600)%3600
            *256/3600);
}

/*
 * the line direction is from (x1, y1) to (x2, y2),
 * the advance is the distance from (*parallel_x1,*parallel_y1) to
 * (x1,y1), advance is along direction
 * rotation 90 degree from line direction in counter-clockwise
 * (*parallel_x1,*parallel_y1) is the point at the parallel line
 * corresponding to (x1,y1).
 * the line from (*parallel_x1,*parallel_y1) to (x1,y1) is vertical to
 * to line from (x1,y1) to (x2,y2)
 * (*parallel_x2,*parallel_y2) is the point at the parallel line
 * corresponding to (x2,y2)
 */
void _gdi_get_point_at_parallel(int x1, int y1, int x2, int y2, int advance,
        int* parallel_x1, int* parallel_y1,
        int* parallel_x2, int* parallel_y2, int rotation)
{
    int adv_x, adv_y;

    adv_x = CAL_VEC_X2(advance,((rotation+900)%3600+3600)%3600
            *256/3600);
    adv_y = CAL_VEC_Y2(advance,((rotation+900)%3600+3600)%3600
            *256/3600);

    *parallel_x1 = x1 + adv_x;
    *parallel_y1 = y1 - adv_y;

    *parallel_x2 = x2 + adv_x;
    *parallel_y2 = y2 - adv_y;
}

void _gdi_get_glyph_box_vertices(int x1, int y1, int x2, int y2,
        POINT* pts, PLOGFONT logfont)
{
    int  adv_x, adv_y;

    adv_x = CAL_VEC_X2(logfont->ascent, ((logfont->rotation+900)%3600
                +3600)%3600*256/3600);
    adv_y = CAL_VEC_Y2(logfont->ascent, ((logfont->rotation+900)%3600
                +3600)%3600*256/3600);

    pts[0].x = x1 + adv_x;
    pts[0].y = y1 - adv_y;

    pts[1].x = x2 + adv_x;
    pts[1].y = y2 - adv_y;


    adv_x = CAL_VEC_X2(logfont->descent, ((logfont->rotation+2700)%3600
                +3600)%3600*256/3600);
    adv_y = CAL_VEC_Y2(logfont->descent, ((logfont->rotation+2700)%3600
                +3600)%3600*256/3600);

    pts[3].x = x1 + adv_x;
    pts[3].y = y1 - adv_y;

    pts[2].x = x2 + adv_x;
    pts[2].y = y2 - adv_y;
}

void _gdi_get_baseline_point (PDC pdc, int* x, int* y)
{
    if ((pdc->ta_flags & TA_Y_MASK) == TA_BASELINE)
        return;

    if (pdc->pLogFont->rotation == 0) {
        if ((pdc->ta_flags & TA_Y_MASK) == TA_TOP) {
            *y += pdc->pLogFont->ascent;
        }
        else if ((pdc->ta_flags & TA_Y_MASK) == TA_BOTTOM) {
            *y -= pdc->pLogFont->descent;
        }
    }
    else {
        /* calculate a point on the baseline */
        int adv_x, adv_y;

        if ((pdc->ta_flags & TA_Y_MASK) == TA_TOP) {
            adv_x = CAL_VEC_X2(pdc->pLogFont->ascent, ((pdc->pLogFont->rotation
                            +2700)%3600+3600)%3600 * 256 / 3600);
            adv_y = CAL_VEC_Y2(pdc->pLogFont->ascent, ((pdc->pLogFont->rotation
                            +2700)%3600+3600)%3600 * 256 / 3600);

            *x += adv_x;
            *y -= adv_y;
        }
        else if ((pdc->ta_flags & TA_Y_MASK) == TA_BOTTOM) {
            adv_x = CAL_VEC_X2(pdc->pLogFont->descent, ((pdc->pLogFont->rotation
                            +900)%3600+3600)%3600 * 256 / 3600);
            adv_y = CAL_VEC_Y2(pdc->pLogFont->descent, ((pdc->pLogFont->rotation
                            +900)%3600+3600)%3600 * 256 / 3600);
            *x += adv_x;
            *y -= adv_y;
        }
    }
}

int _font_get_glyph_log_width(LOGFONT* logfont, Glyph32 gv)
{
    int width, bold = 0, tmp_x = 0, tmp_y = 0;
    int glyph_bmptype;
    DEVFONT* devfont = SELECT_DEVFONT_BY_GLYPH(logfont, gv);

    if (gv == INV_GLYPH_VALUE)
        return 0;

    gv = REAL_GLYPH(gv);
    width = devfont->font_ops->get_glyph_advance(logfont, devfont, gv,
            &tmp_x, &tmp_y);

    glyph_bmptype = devfont->font_ops->get_glyph_bmptype (logfont, devfont)
            & DEVFONTGLYPHTYPE_MASK_BMPTYPE;

    // VincentWei: only use auto bold when the weight of devfont does not
    // match the weight of logfont.
    if (((int)(logfont->style & FS_WEIGHT_MASK) -
            (int)(devfont->style & FS_WEIGHT_MASK)) > FS_WEIGHT_AUTOBOLD
            && (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP)) {
        bold = GET_DEVFONT_SCALE (logfont, devfont);
    }

    width += bold;

    if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP) {
        if ((logfont->style & FS_RENDER_MASK) == FS_RENDER_GREY ||
                (logfont->style & FS_DECORATE_OUTLINE)) {
            width++;
        }
    }

    return width;
}

int _font_get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont,
        Glyph32 glyph_value, BOOL direction, int ch_extra,
        int x, int y, int* adv_x, int* adv_y, BBOX* bbox)
{
    int bold = 0;
    int tmp_x = x;
    int tmp_y = y;
    int adv_len = 0;
    int bbox_x = x, bbox_y = y;
    int bbox_w = 0, bbox_h = 0;

    int advance = 0;
    int glyph_bmptype;

    // skip empty glyph
    if (glyph_value == INV_GLYPH_VALUE) {
        goto done;
    }

    glyph_bmptype = devfont->font_ops->get_glyph_bmptype (logfont, devfont)
            & DEVFONTGLYPHTYPE_MASK_BMPTYPE;

    devfont->font_ops->get_glyph_bbox (logfont, devfont,
            REAL_GLYPH(glyph_value), &bbox_x, &bbox_y, &bbox_w, &bbox_h);

    // VincentWei: only use auto bold when the weight of devfont does not
    // match the weight of logfont.
    if (((int)(logfont->style & FS_WEIGHT_MASK) -
            (int)(devfont->style & FS_WEIGHT_MASK)) > FS_WEIGHT_AUTOBOLD
            && (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP)) {
        bold = GET_DEVFONT_SCALE (logfont, devfont);
        bbox_w += bold;
    }

    adv_len = devfont->font_ops->get_glyph_advance (logfont, devfont,
            REAL_GLYPH(glyph_value), &tmp_x, &tmp_y);

    // VincentWei: adjust the bbox_x for not rotated glyph
    if (!direction && logfont->rotation == 0) {
        bbox_x -= (bold + ch_extra + adv_len);
    }
    else if (logfont->rotation) {
        // adjust acorrding to the alignment
    }

    if (direction) {
        tmp_x += bold + ch_extra;
        if (adv_x) *adv_x = tmp_x - x;
        if (adv_y) *adv_y = tmp_y - y;
    }
    else {
        tmp_x -= bold + ch_extra;
        if (adv_x) *adv_x = x - tmp_x;
        if (adv_y) *adv_y = y - tmp_y;
    }

    if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP) {
        if ((logfont->style & FS_RENDER_MASK) == FS_RENDER_GREY ||
            (logfont->style & FS_DECORATE_OUTLINE)) {
            if (adv_x) *adv_x  += 1;
            advance += 1;
        }
    }

done:
    if (bbox) {
        bbox->x = bbox_x;
        bbox->y = bbox_y;
        bbox->w = bbox_w;
        bbox->h = bbox_h;
    }

    return adv_len + bold + ch_extra + advance;
}

int _font_get_glyph_metrics(LOGFONT* logfont,
        Glyph32 gv, int* adv_x, int* adv_y, BBOX* bbox)
{
    int bold = 0;
    int tmp_x = 0;
    int tmp_y = 0;
    int bbox_x = 0, bbox_y = 0;
    int bbox_w = 0, bbox_h = 0;
    int gbt, width;;

    DEVFONT* devfont = SELECT_DEVFONT_BY_GLYPH(logfont, gv);
    gbt = devfont->font_ops->get_glyph_bmptype (logfont, devfont)
            & DEVFONTGLYPHTYPE_MASK_BMPTYPE;

    devfont->font_ops->get_glyph_bbox (logfont, devfont,
            REAL_GLYPH(gv), &bbox_x, &bbox_y, &bbox_w, &bbox_h);

    if ((logfont->style & FS_WEIGHT_MASK) > FS_WEIGHT_MEDIUM
            && (devfont->style & FS_WEIGHT_MASK) < FS_WEIGHT_DEMIBOLD
            && (gbt == DEVFONTGLYPHTYPE_MONOBMP)) {
        bold = GET_DEVFONT_SCALE (logfont, devfont);
        bbox_w += bold;
    }

    if (bbox) {
        bbox->x = bbox_x;
        bbox->y = bbox_y;
        bbox->w = bbox_w;
        bbox->h = bbox_h;
    }

    width = devfont->font_ops->get_glyph_advance (logfont, devfont,
        REAL_GLYPH(gv), &tmp_x, &tmp_y);

    tmp_x += bold;
    if (gbt == DEVFONTGLYPHTYPE_MONOBMP) {
        if ((logfont->style & FS_RENDER_MASK) == FS_RENDER_GREY ||
                (logfont->style & FS_DECORATE_OUTLINE)) {
            tmp_x++;
            width++;
        }
    }

    if (adv_x) *adv_x = tmp_x;
    if (adv_y) *adv_y = tmp_y;

    return width;
}

void _gdi_get_next_point_online (int pre_x, int pre_y, int advance,
        BOOL direction, PDC pdc, int *next_x, int *next_y)
{
    int  delta_x, delta_y;
    int  rotation;

    rotation =pdc->pLogFont->rotation;
    rotation =(rotation%3600 +3600)%3600;

    if(advance <0) advance =-advance;
    *next_x =pre_x;
    *next_y =pre_y;
    switch(rotation){
        case 0:
        {
            if(direction){
                *next_x += advance;
            }
            else{
                *next_x -= advance;
            }
        }
        case 900:
        {
            if(direction){
                *next_y -= advance;
            }
            else{
                *next_y += advance;
            }
        }
        case 1800:
        {
            if(direction){
                *next_x -= advance;
            }
            else{
                *next_x += advance;
            }
        }
        case 2700:
        {
            if(direction){
                *next_y += advance;
            }
            else{
                *next_y -= advance;
            }
        }
    }

    delta_x = CAL_VEC_X2(advance,rotation * 256 / 3600);
    delta_y = CAL_VEC_Y2(advance,rotation * 256 / 3600);

    if(direction){
        *next_x += delta_x;
        *next_y -= delta_y;
    }
    else{
        *next_x -= delta_x;
        *next_y += delta_y;
    }
}

#define SIN_DEGREE(degree) \
    fixsin (fixdiv (itofix(degree), ftofix(62.831852)))

#define COS_DEGREE(degree) \
    fixcos (fixdiv (itofix(degree), ftofix(62.831852)))


int _gdi_get_null_glyph_advance (PDC pdc, int advance, BOOL direction,
        int x, int y, int* adv_x, int* adv_y)
{
    _gdi_get_next_point_online (x,y,advance,direction,pdc,adv_x,adv_y);

    *adv_x =*adv_x -x;
    *adv_y =*adv_y -y;

    return advance;
}


#define ROMAN_RECT  0
#define ITALIC_RECT 1
#define ROTATE_RECT 2

static void make_back_area(PDC pdc, int x0, int y0, int x1, int y1,
        POINT* area, GAL_Rect* gal_rc, int italic, int bold, int* flag)
{
    int h = pdc->pLogFont->size;
    int ascent = pdc->pLogFont->ascent;

    if (pdc->pLogFont->rotation)
        *flag = ROTATE_RECT;
    else if (italic > 0)
        *flag = ITALIC_RECT;
    else
        *flag = ROMAN_RECT;

    switch (*flag) {
    case ROMAN_RECT:
    case ITALIC_RECT:
        gal_rc->x = x0;
        gal_rc->y = y0 - ascent;
        gal_rc->w = x1 - x0;
        gal_rc->h = h;
        break;

    case ROTATE_RECT:
        _gdi_get_glyph_box_vertices (x0, y0, x1, y1,
                 area, pdc->pLogFont);
        break;
    }
}

static void draw_back_area (PDC pdc, POINT* area, GAL_Rect* gal_rc,
        int italic, int bold, int flag)
{
    int x = gal_rc->x;
    int y = gal_rc->y;
    int w = gal_rc->w;
    int h = gal_rc->h;
    int i;
    int font_style = pdc->pLogFont->style;
    int italic_x;

    switch (flag) {
    case ROMAN_RECT:
        _dc_fillbox_clip (pdc, gal_rc);
        break;

    case ITALIC_RECT: {
        switch (font_style & FS_FLIP_MASK) {
        case FS_FLIP_HORZ:
        case FS_FLIP_VERT:
            for (i = 0; i < h; i++) {
                italic_x = x + (i >> 1);
                _dc_draw_hline_clip (pdc, italic_x,
                        italic_x + w - 1, y + i);
            }
            break;

        default:
            for (i = 0; i < h; i++) {
                italic_x = x + ((h - i) >> 1);
                _dc_draw_hline_clip (pdc, italic_x,
                        italic_x + w - 1, y + i);
            }
        }
        break;
    }

    case ROTATE_RECT:
        MonotoneVerticalPolygonGenerator (pdc, area,
                4, _dc_draw_hline_clip);
        break;

    default:
        _WRN_PRINTF ("No way to draw background\n");
        break;
    }
}

static void make_back_rect(RECT* rc_back, POINT* area, GAL_Rect* gal_rc,
        int italic, int bold, int flag)
{
    int i;
    switch (flag) {
    case ROMAN_RECT:
        rc_back->left = gal_rc->x;
        rc_back->right = gal_rc->x + gal_rc->w;
        rc_back->top = gal_rc->y;
        rc_back->bottom = gal_rc->y + gal_rc->h;
        break;

    case ITALIC_RECT:
        rc_back->left = gal_rc->x;
        rc_back->right = gal_rc->x + gal_rc->w;
        rc_back->top = gal_rc->y;
        rc_back->bottom = gal_rc->y + gal_rc->h;
        break;

    case ROTATE_RECT:
        rc_back->left = area[0].x;
        rc_back->right = area[0].x;
        rc_back->top = area[0].y;
        rc_back->bottom = area[0].y;

        for (i=0; i<4; i++) {
            if (rc_back->left > area[i].x)
                rc_back->left = area[i].x;

            if (rc_back->right < area[i].x)
                rc_back->right = area[i].x;

            if (rc_back->top > area[i].y)
                rc_back->top = area[i].y;

            if (rc_back->bottom < area[i].y)
                rc_back->bottom = area[i].y;
        }
    }
}

void _gdi_calc_glyphs_size_from_two_points (PDC pdc, int x0, int y0,
        int x1, int y1, SIZE* size)
{
    POINT area[4];
    GAL_Rect gal_rc;
    int flag;
    RECT rc;

    make_back_area(pdc, x0, y0, x1, y1, area, &gal_rc, 0, 0, &flag);
    make_back_rect(&rc, area, &gal_rc, 0, 0, flag);

    size->cx = RECTW (rc);
    size->cy = RECTH (rc);
}

int _gdi_draw_null_glyph (PDC pdc, int advance, BOOL direction,
        int x, int y, int* adv_x, int* adv_y)
{
    POINT area[4];
    GAL_Rect gal_rc;
    RECT rc_back;
    RECT rc_tmp = pdc->rc_output;
    int flag;

    _gdi_get_null_glyph_advance (pdc, advance, direction, x, y, adv_x, adv_y);

    if (pdc->bkmode == BM_TRANSPARENT)
        return advance;

    if (direction)
        make_back_area(pdc, x, y, x+*adv_x, y+*adv_y,
                area, &gal_rc, 0, 0, &flag);
    else
        make_back_area(pdc, x+*adv_x, y+*adv_y, x, y,
                area, &gal_rc, 0, 0, &flag);

    make_back_rect(&rc_back, area, &gal_rc, 0, 0, flag);

    if (!(pdc = __mg_check_ecrgn ((HDC)pdc))) {
        return advance;
    }

    if (WITHOUT_DRAWING (pdc)) goto end;

    pdc->cur_pixel = pdc->bkcolor;
    pdc->cur_ban = NULL;
    pdc->step = 1;

    if(!IntersectRect(&pdc->rc_output, &rc_back, &pdc->rc_output))
        goto end;

    ENTER_DRAWING(pdc);

    draw_back_area (pdc, area, &gal_rc, 0, 0, flag);

    LEAVE_DRAWING (pdc);
end:
    pdc->rc_output = rc_tmp;
    UNLOCK_GCRINFO (pdc);
    return advance;
}

static void draw_glyph_lines (PDC pdc, int x1, int y1, int x2, int y2)
{
    LOGFONT* logfont;
    int h;
    int descent;

    int draw_x1;
    int draw_y1;
    int draw_x2;
    int draw_y2;

    RECT eff_rc;
    CLIPRECT* cliprect;

    logfont = pdc->pLogFont;
    h = logfont->size;
    descent = logfont->descent;

    if (x1 == x2 && y1 == y2) {
        return;
    }

    pdc->cur_pixel = pdc->textcolor;
    if (logfont->style & FS_DECORATE_UNDERLINE) {
        if (logfont->style & FS_FLIP_VERT) {
            _gdi_get_point_at_parallel(x1, y1, x2, y2, h-(descent<<1), \
                     &draw_x1, &draw_y1, &draw_x2, &draw_y2,
                     pdc->pLogFont->rotation);
        }
        else
        {
            draw_x1 = x1;
            draw_y1 = y1;

            draw_x2 = x2;
            draw_y2 = y2;
        }

        if (logfont->rotation == 0
                && (logfont->style & FS_SLANT_ITALIC)) {
            if (logfont->style & FS_FLIP_HORZ) {
                /*try and try, -1 is right*/
                draw_x1 += ((h - descent) >>1) - 1;
                draw_x2 += ((h - descent) >>1) - 1;
            }
            else {
                draw_x1 += descent >>1;
                draw_x2 += descent >>1;
            }
        }

        cliprect = pdc->ecrgn.head;
        while (cliprect) {
            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)
                    && LineClipper (&eff_rc, &draw_x1, &draw_y1,
                        &draw_x2, &draw_y2)) {
                pdc->move_to (pdc, draw_x1, draw_y1);
                LineGenerator (pdc, draw_x1, draw_y1, draw_x2, draw_y2,
                        _dc_set_pixel_noclip);
            }
            else
                _DBG_PRINTF ("%s: no under line, rc_output: %d, %d, %d, %d\n",
                    __FUNCTION__,
                    pdc->rc_output.left, pdc->rc_output.top,
                    pdc->rc_output.right, pdc->rc_output.bottom);
            cliprect = cliprect->next;
        }
    }

    if (logfont->style & FS_DECORATE_STRUCKOUT) {
        if (logfont->style & FS_FLIP_VERT) {
            _gdi_get_point_at_parallel(x1, y1, x2, y2, (h >>1)-descent +1, \
                    &draw_x1, &draw_y1, &draw_x2, &draw_y2,
                    pdc->pLogFont->rotation);
        }
        else {
            _gdi_get_point_at_parallel(x1, y1, x2, y2, (h >>1) - descent, \
                    &draw_x1, &draw_y1, &draw_x2, &draw_y2,
                    pdc->pLogFont->rotation);
        }

        if (logfont->rotation == 0 && (logfont->style & FS_SLANT_ITALIC)) {
            draw_x1 += h>>2;
            draw_x2 += h>>2;
        }

        cliprect = pdc->ecrgn.head;
        while (cliprect) {
            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)
                    && LineClipper (&eff_rc, &draw_x1, &draw_y1,
                        &draw_x2, &draw_y2)) {
                pdc->move_to (pdc, draw_x1, draw_y1);
                LineGenerator (pdc, draw_x1, draw_y1, draw_x2, draw_y2,
                        _dc_set_pixel_noclip);
            }
            cliprect = cliprect->next;
        }
    }
}

int _gdi_draw_one_glyph (PDC pdc, Glyph32 glyph_value, BOOL direction,
            int x, int y, int* adv_x, int* adv_y)
{
    LOGFONT* logfont;
    DEVFONT* devfont;

    BBOX bbox;
    int advance;
    GAL_Rect fg_gal_rc;
    GAL_Rect bg_gal_rc;

    RECT rc_output;
    RECT rc_front;
    RECT rc_tmp;
    RECT rc_back;
    BOOL need_rc_back = FALSE;

    int italic = 0;
    int bold = 0;
    SIZE bbx_size;

    POINT area[4];
    int flag = 0;
    int glyph_bmptype;

    y += pdc->alExtra;

    rc_tmp = pdc->rc_output;
    advance = _gdi_get_glyph_advance (pdc, glyph_value, direction,
            x, y, adv_x, adv_y, &bbox);

    logfont = pdc->pLogFont;
    devfont = SELECT_DEVFONT_BY_GLYPH (logfont, glyph_value);

    glyph_bmptype = devfont->font_ops->get_glyph_bmptype (logfont, devfont)
            & DEVFONTGLYPHTYPE_MASK_BMPTYPE;

    // VincentWei: only use auto bold when the weight of devfont does not
    // match the weight of logfont.
    if (((int)(logfont->style & FS_WEIGHT_MASK) -
            (int)(devfont->style & FS_WEIGHT_MASK)) > FS_WEIGHT_AUTOBOLD
            && (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP)) {
        bold = GET_DEVFONT_SCALE (logfont, devfont);
    }

    if (logfont->style & FS_SLANT_ITALIC
            && !(devfont->style & FS_SLANT_ITALIC)) {
        italic = devfont->font_ops->get_font_height (logfont, devfont) >> 1;
    }

    fg_gal_rc.x = bbox.x;
    fg_gal_rc.y = bbox.y;
    fg_gal_rc.w = bbox.w + italic;
    fg_gal_rc.h = bbox.h;

    if (pdc->bkmode != BM_TRANSPARENT
         || logfont->style & FS_DECORATE_UNDERLINE
         || logfont->style & FS_DECORATE_STRUCKOUT)
        need_rc_back = TRUE;

    if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP
            && (logfont->style & FS_DECORATE_OUTLINE)) {
        fg_gal_rc.x--; fg_gal_rc.y--;
        fg_gal_rc.w += 2; fg_gal_rc.h += 2;
    }

    if (need_rc_back) {
        if (direction)
            make_back_area(pdc, x, y, x+*adv_x, y+*adv_y,
                    area, &bg_gal_rc, italic, bold, &flag);
        else
            make_back_area(pdc, x+*adv_x, y+*adv_y, x, y,
                    area, &bg_gal_rc, italic, bold, &flag);

        if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP
                && (logfont->style & FS_DECORATE_OUTLINE)) {
            bg_gal_rc.x--; bg_gal_rc.y--;
            bg_gal_rc.w += 2; bg_gal_rc.h += 2;
        }
        make_back_rect(&rc_back, area, &bg_gal_rc, italic, bold, flag);
    }

    rc_front.left = fg_gal_rc.x;
    rc_front.top = fg_gal_rc.y;
    rc_front.right = fg_gal_rc.x + fg_gal_rc.w;
    rc_front.bottom = fg_gal_rc.y + fg_gal_rc.h + 1; /* for under line */

    if (need_rc_back)
        GetBoundRect (&rc_output, &rc_back, &rc_front);
    else
        rc_output = rc_front;

    if (!(pdc = __mg_check_ecrgn ((HDC)pdc))) {
        return advance;
    }

    if (!IntersectRect(&pdc->rc_output, &rc_output, &pdc->rc_output)) {
        goto end;
    }

    if (WITHOUT_DRAWING (pdc)) goto end;

    ENTER_DRAWING (pdc);

    pdc->step = 1;
    pdc->cur_ban = NULL;

    /* draw back ground */
    if (pdc->bkmode != BM_TRANSPARENT) {
        pdc->cur_pixel = pdc->bkcolor;
        draw_back_area (pdc, area, &bg_gal_rc, italic, bold, flag);
    }

    /* bbox is the real glyph pixels on one scan-line. */
    bbx_size.cx = bbox.w;
    bbx_size.cy = bbox.h;

    _gdi_direct_fillglyph (pdc, glyph_value, &fg_gal_rc, &bbx_size,
            y - bbox.y, advance, italic, bold);

    draw_glyph_lines (pdc, x, y, x + *adv_x, y + *adv_y);

    LEAVE_DRAWING (pdc);
end:
    pdc->rc_output = rc_tmp;
    UNLOCK_GCRINFO (pdc);
    return advance;
}

int _gdi_get_italic_added_width (LOGFONT* logfont)
{
    /* FIXME: use the correct devfont for auto-italic */
    if (logfont->style & FS_SLANT_ITALIC
            && !(logfont->devfonts[0]->style & FS_SLANT_ITALIC)) {
        return (logfont->size + 1) >> 1;
    }
    else {
        return 0;
    }
}

void _gdi_start_new_line (PDC pdc)
{
    int i;
    DEVFONT* df;

    for (i = 0; i < MAXNR_DEVFONTS; i++) {
        if ((df = pdc->pLogFont->devfonts[i])) {
            if (df->font_ops->start_str_output) {
                df->font_ops->start_str_output(pdc->pLogFont, df);
            }
        }
    }
}

int GUIAPI DrawGlyph (HDC hdc, int x, int y, Glyph32 glyph_value,
        int* adv_x, int* adv_y)
{
    int my_adv_x, my_adv_y;
    int advance;
    PDC pdc;

    if (glyph_value == INV_GLYPH_VALUE)
        return 0;

    pdc = dc_HDC2PDC(hdc);
    /* Transfer logical to device to screen here. */
    coor_LP2SP (pdc, &x, &y);
    pdc->rc_output = pdc->DevRC;

    /* convert to the start point on baseline. */
    _gdi_get_baseline_point (pdc, &x, &y);

    advance = _gdi_draw_one_glyph (pdc, glyph_value,
            (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
            x, y, &my_adv_x, &my_adv_y);

    if (adv_x) *adv_x = my_adv_x;
    if (adv_y) *adv_y = my_adv_y;

    return advance;
}

int GUIAPI DrawGlyphStrings(HDC hdc, Glyph32* glyphs, int nr_glyphs,
        const POINT* pts)
{
    int count = 0;
    int i;
    PDC pdc = dc_HDC2PDC(hdc);

    for (i = 0; i < nr_glyphs; i++) {
        int x, y;
        int my_adv_x, my_adv_y;
        Glyph32 gv = glyphs[i];

        if (gv == INV_GLYPH_VALUE)
            break;

        count++;

        /* Transfer logical to device to screen here. */
        x = pts[i].x;
        y = pts[i].y;

        coor_LP2SP (pdc, &x, &y);
        pdc->rc_output = pdc->DevRC;

        /* convert to the start point on baseline. */
        _gdi_get_baseline_point (pdc, &x, &y);

        _gdi_draw_one_glyph (pdc, gv,
                (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                x, y, &my_adv_x, &my_adv_y);
    }

    return count;
}

int GUIAPI GetGlyphsExtentPoint(HDC hdc, Glyph32* glyphs, int nr_glyphs,
        int max_extent, SIZE* size)
{
    int i = 0;
    int advance = 0;
    int adv_x, adv_y;
    PDC pdc = dc_HDC2PDC(hdc);

    size->cx = 0;
    size->cy = 0;

    while (i < nr_glyphs) {
        Glyph32 gv = glyphs[i];

        if (gv == INV_GLYPH_VALUE) {
            adv_x = adv_y = 0;
        }
        else {
            advance += _gdi_get_glyph_advance (pdc, gv,
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    0, 0, &adv_x, &adv_y, NULL);
        }

        if (max_extent > 0 && advance > max_extent)
            break;

        size->cx += adv_x;
        size->cy += adv_y;
        i++;
    }

    _gdi_calc_glyphs_size_from_two_points (pdc, 0, 0,
            size->cx, size->cy, size);

    return i;
}

int GUIAPI GetGlyphsExtent(HDC hdc, Glyph32* glyphs, int nr_glyphs, SIZE* size)
{
    int i = 0;
    int advance = 0;
    int adv_x, adv_y;
    PDC pdc = dc_HDC2PDC(hdc);

    size->cx = 0;
    size->cy = 0;
    while (i < nr_glyphs) {
        Glyph32 gv = glyphs[i];

        if (gv == INV_GLYPH_VALUE) {
            adv_x = adv_y = 0;
        }
        else {
            advance += _gdi_get_glyph_advance (pdc, gv,
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    0, 0, &adv_x, &adv_y, NULL);
        }
        size->cx += adv_x;
        size->cy += adv_y;
        i ++;
    }

    _gdi_calc_glyphs_size_from_two_points (pdc, 0, 0,
            size->cx, size->cy, size);

    return advance;
}

