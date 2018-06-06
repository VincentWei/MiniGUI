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

#define FS_WEIGHT_BOOK_LIGHT    (FS_WEIGHT_BOOK | FS_WEIGHT_LIGHT)

#define DRAW_SCANLINE 

#define EDGE_ALPHA      64
#define ALL_WGHT        32
#define ROUND_WGHT      16

#define NEIGHBOR_WGHT    3
#define NEAR_WGHT        1

#ifdef _MGFONT_FT2
int ft2GetLcdFilter (DEVFONT* devfont);
int ft2IsFreeTypeDevfont (DEVFONT* devfont);
#endif
#ifdef _MGFONT_TTF
int ftIsFreeTypeDevfont (DEVFONT* devfont);
#endif

Glyph32 GUIAPI GetGlyphValue (LOGFONT* logfont, const char* mchar, 
        int mchar_len, const char* pre_mchar, int pre_len)
{
    int len_cur_char;

    Glyph32  glyph_value = INV_GLYPH_VALUE;
    DEVFONT* sbc_devfont  = logfont->sbc_devfont;
    DEVFONT* mbc_devfont = logfont->mbc_devfont;

    if (mbc_devfont) {
        len_cur_char = mbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)mchar, mchar_len);

        if (len_cur_char > 0) {
            glyph_value = mbc_devfont->charset_ops->char_glyph_value
                ((Uint8*)pre_mchar, pre_len, (Uint8*)mchar, mchar_len);
            return SET_MBC_GLYPH(glyph_value);
        }
    }

    len_cur_char = sbc_devfont->charset_ops->len_first_char
        ((const unsigned char*)mchar, mchar_len);

    if (len_cur_char > 0) {
        glyph_value = sbc_devfont->charset_ops->char_glyph_value
            (NULL, 0, (Uint8*)mchar, mchar_len);
    }

    return glyph_value;
}

Glyph32 GUIAPI GetGlyphShape (LOGFONT* logfont, const char* mchar, 
        int mchar_len, SHAPETYPE shape_type)
{
    int len_cur_char;

    Glyph32  glyph_value = INV_GLYPH_VALUE;
    DEVFONT* sbc_devfont  = logfont->sbc_devfont;
    DEVFONT* mbc_devfont = logfont->mbc_devfont;

    if (mbc_devfont) {
        len_cur_char = mbc_devfont->charset_ops->len_first_char (
                (const unsigned char*)mchar, mchar_len);

        if (len_cur_char > 0) {
            if (mbc_devfont->charset_ops->glyph_shape)
                glyph_value = mbc_devfont->charset_ops->glyph_shape(
                        (const unsigned char*)mchar, mchar_len, shape_type);
            else
                glyph_value = mbc_devfont->charset_ops->char_glyph_value
                (NULL, 0, (Uint8*)mchar, len_cur_char);

            return SET_MBC_GLYPH(glyph_value);
        }
    }

    len_cur_char = sbc_devfont->charset_ops->len_first_char
        ((const unsigned char*)mchar, mchar_len);

    if (len_cur_char > 0) {
        if (sbc_devfont->charset_ops->glyph_shape)
            glyph_value = sbc_devfont->charset_ops->glyph_shape(
                    (const unsigned char*)mchar, mchar_len, shape_type);
        else
            glyph_value = sbc_devfont->charset_ops->char_glyph_value
                (NULL, 0, (Uint8*)mchar, len_cur_char);

    }

    return glyph_value;
}

int GUIAPI DrawGlyph (HDC hdc, int x, int y, Glyph32 glyph_value, 
        int* adv_x, int* adv_y)
{
    int advance;
    int glyph_type;

    PDC pdc = dc_HDC2PDC(hdc);
    DEVFONT* devfont = SELECT_DEVFONT(pdc->pLogFont, glyph_value);

    /* Transfer logical to device to screen here. */
    coor_LP2SP (pdc, &x, &y);
    pdc->rc_output = pdc->DevRC;

    glyph_type = devfont->charset_ops->glyph_type (glyph_value);

    if (glyph_type == MCHAR_TYPE_ZEROWIDTH) {
        if (adv_x) *adv_x = 0;
        if (adv_y) *adv_y = 0;
        advance = 0;
    }
    else {
		int my_adv_x, my_adv_y;
        /* convert to the start point on baseline. */
        _gdi_get_baseline_point (pdc, &x, &y);

        advance = _gdi_draw_one_glyph (pdc, glyph_value, 
                (pdc->ta_flags & TA_X_MASK) != TA_RIGHT, 
                x, y, &my_adv_x, &my_adv_y);

        if (adv_x) *adv_x = my_adv_x;
        if (adv_y) *adv_y = my_adv_y;
    }

    return advance;
}

#if 0
static BOOL is_font_support_bmptype (LOGFONT* logfont, DEVFONT* devfont,
        int bmptype)
{
    DWORD glyph_type = devfont->font_ops->get_glyph_type (logfont, devfont);
    glyph_type &= DEVFONTGLYPHTYPE_MASK_BMPTYPE;

    if (glyph_type == bmptype)
        return TRUE;

    return FALSE;
}
#endif

int GUIAPI GetGlyphInfo (LOGFONT* logfont, Glyph32 glyph_value, 
        GLYPHINFO* glyph_info)
{
    int advance = 0;
    int pitch = 0;

    /* get the relative device font*/
    DEVFONT* devfont = SELECT_DEVFONT(logfont, glyph_value);
    glyph_value = REAL_GLYPH (glyph_value);

    /*get glyph type*/
    if (glyph_info->mask & GLYPH_INFO_TYPE)
        glyph_info->glyph_type = devfont->charset_ops->glyph_type (glyph_value);

    glyph_info->advance_x = 0;
    glyph_info->advance_y = 0;
    glyph_info->bbox_x = 0;
    glyph_info->bbox_y = 0;

    devfont->font_ops->get_glyph_bbox (logfont, devfont, glyph_value,
            &glyph_info->bbox_x, &glyph_info->bbox_y,
            &glyph_info->bbox_w, &glyph_info->bbox_h);

    /* get the glyph advance info.*/
    advance = devfont->font_ops->get_glyph_advance(logfont, devfont,
            glyph_value, &glyph_info->advance_x, &glyph_info->advance_y);

    if (logfont->style & FS_WEIGHT_BOLD) {
        advance++;
        glyph_info->advance_x++;
    }

    /*get height and descent of devfont*/
    if (glyph_info->mask & GLYPH_INFO_METRICS)
    {
        glyph_info->height = devfont->font_ops->get_font_height
            (logfont, devfont);
        glyph_info->descent = devfont->font_ops->get_font_descent
            (logfont, devfont);
    }

    if (glyph_info->mask & GLYPH_INFO_BMP) {
        DWORD glyph_type = devfont->font_ops->get_glyph_type (logfont, devfont);
        glyph_info->bmp_type = glyph_type & DEVFONTGLYPHTYPE_MASK_BMPTYPE;

        switch (glyph_info->bmp_type) {
            case GLYPHBMP_TYPE_MONO:
                    glyph_info->bits = devfont->font_ops->get_glyph_monobitmap (
                            logfont, devfont, glyph_value, &pitch, NULL);
                    glyph_info->bmp_pitch = pitch;
                    glyph_info->bmp_size = pitch * glyph_info->bbox_h;
                break;

            case GLYPHBMP_TYPE_GREY:
                    glyph_info->bits = devfont->font_ops->get_glyph_greybitmap (
                            logfont, devfont, glyph_value, 
                            &glyph_info->bmp_pitch, NULL);
                    glyph_info->bmp_size = 
                        glyph_info->bmp_pitch * glyph_info->bbox_h;
                break;

            case GLYPHBMP_TYPE_SUBPIXEL:
                    glyph_info->bits = devfont->font_ops->get_glyph_greybitmap (
                            logfont, devfont, glyph_value, 
                            &glyph_info->bmp_pitch, NULL);
                    glyph_info->bmp_size = 
                        glyph_info->bmp_pitch * glyph_info->bbox_h;
                break;

            case GLYPHBMP_TYPE_PRERENDER:
                    devfont->font_ops->get_glyph_prbitmap (logfont,
                            devfont, glyph_value, &glyph_info->prbitmap);
                break;
        }
    }

    return advance;
}

void GUIAPI inline GetGlyphBitmap (LOGFONT* logfont, const char* mchar, 
        int mchar_len, GLYPHBITMAP* glyph_bitmap)
{
    GLYPHINFO glyph_info;
    Glyph32 glyph_value = INV_GLYPH_VALUE;

    memset(&glyph_info, 0, sizeof(GLYPHINFO));
    glyph_value = GetGlyphValue(logfont, (const char*)mchar, 
            mchar_len, NULL, 0);

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

    glyph_bitmap->bmp_size = glyph_info.bmp_size;
    glyph_bitmap->bmp_pitch = glyph_info.bmp_pitch;
    glyph_bitmap->bits = glyph_info.bits;
}

static int  mult (fixed op1, fixed op2) 
{ 
    long s = op2;
    if(s < 0)
        op2 = -op2;
    {
        long op1_hi = (op1 >> 16)& 0xffff; 
        long op1_lo = op1 & 0xffff; 
        long op2_hi = (op2 >> 16)& 0xffff; 
        long op2_lo = op2 & 0xffff; 
        long cross_prod = op1_lo * op2_hi + op1_hi * op2_lo;
        long prod_hi = op1_hi * op2_hi + ((cross_prod >> 16) & 0xffff); 
        long prod_lo = op1_lo * op2_lo + ((cross_prod << 16) & 0xffff0000); 

        prod_hi +=((prod_lo& 0x80000000)>>31);
        if(s <0)
            prod_hi =-prod_hi;

        return (int)prod_hi; 
    }
}

#define CAL_VEC_X2(advance, tenthradian) \
    mult(itofix(advance), fixcos(ftofix(tenthradian)))

#define CAL_VEC_Y2(advance, tenthradian) \
    mult(itofix(advance), fixsin(ftofix(tenthradian)))

#define CAL_VEC_X(advance, tenthradian)       \
    fixtoi (fixmul (itofix(advance), fixcos (ftofix(tenthradian))))

#define CAL_VEC_Y(advance, tenthradian)       \
    fixtoi (fixmul (itofix(advance), fixsin (ftofix(tenthradian))))

#ifndef DRAW_SCANLINE

static BITMAP char_bmp;
static void* char_bits;
static size_t char_bits_size;

BOOL InitTextBitmapBuffer (void)
{
    char_bits = NULL;
    char_bmp.bmBits = NULL;
    return TRUE;
}

void TermTextBitmapBuffer (void)
{
    free (char_bits);
    char_bits = NULL;
    char_bmp.bmBits = NULL;

    char_bits_size = 0;
}

static void prepare_bitmap (PDC pdc, int w, int h)
{
    Uint32 size;
    size = GAL_GetBoxSize (pdc->surface, w + 2, h + 3, &char_bmp.bmPitch);
    char_bmp.bmType = 0;
    char_bmp.bmBitsPerPixel = pdc->surface->format->BitsPerPixel;
    char_bmp.bmBytesPerPixel = pdc->surface->format->BytesPerPixel;
    char_bmp.bmWidth = w;
    char_bmp.bmHeight = h;
    char_bmp.bmAlphaPixelFormat = NULL;

    /*char_bmp.bmBits may be setted to filtered_bits
     * recover it*/
    char_bmp.bmBits = char_bits;

    if (size <= char_bits_size) {
        goto done;
    }
    char_bits_size = ((size + 31) >> 5) << 5;

    char_bits = realloc(char_bmp.bmBits, char_bits_size);    
    char_bmp.bmBits = char_bits;

done:
    memset (char_bmp.bmBits, 0, char_bits_size);
}

static Uint32 allocated_size = 0;
static BYTE* filtered_bits;

static BYTE* do_low_pass_filtering (PDC pdc)
{
    int x, y;
    int bpp = GAL_BytesPerPixel (pdc->surface);
    BYTE *curr_sline, *prev_sline, *next_sline;
    BYTE *dst_line, *dst_pixel;

    if (allocated_size < char_bits_size) {

        filtered_bits = realloc(filtered_bits, char_bits_size);
        allocated_size = char_bits_size;
    }

    if (filtered_bits == NULL)
    {
        return NULL;
    }

    curr_sline = char_bmp.bmBits; 
    next_sline = curr_sline + char_bmp.bmPitch;
    dst_line = filtered_bits;
    dst_pixel = dst_line;;

    /* For the first line, set the background and foreground color directly. */
    for (x = 0; x < char_bmp.bmWidth; x++) {
        int weight = 0;

        if (curr_sline [x]) {
            weight = 16;
        }
        else {
            if (x > 0) {
                if (curr_sline [x - 1]) weight += 3;
                if (next_sline [x - 1]) weight += 1;
            }
            if (x < char_bmp.bmWidth - 1) {
                if (curr_sline [x + 1]) weight += 3;
                if (next_sline [x + 1]) weight += 1;
            }
            if (next_sline [x]) weight += 3;
        }

        dst_pixel = _mem_set_pixel (dst_pixel, 
                        bpp, pdc->filter_pixels [weight]);
    }

    prev_sline = curr_sline;
    curr_sline = curr_sline + char_bmp.bmPitch;
    next_sline = curr_sline + char_bmp.bmPitch;
    dst_line += char_bmp.bmPitch;

    if (char_bmp.bmHeight > 2)
    for (y = 1; y < char_bmp.bmHeight - 1; y++) {
        int weight;

        dst_pixel = dst_line;

        for (x = 0; x < char_bmp.bmWidth; x++) {
            weight = 0;

            /*
             * For the first pixel per line, 
             * set the background and foreground color directly.
             */ 
            if (curr_sline [x]) {
                weight = 16;
            }
            else {
                if (x == 0) {
                    if (curr_sline [x + 1]) weight += 3;

                    if (prev_sline [x]) weight += 3;
                    if (prev_sline [x + 1]) weight += 1;

                    if (next_sline [x]) weight += 3;
                    if (next_sline [x + 1]) weight += 1;
                }
                else if (x == (char_bmp.bmWidth - 1)) {
                    if (curr_sline [x - 1]) weight += 3;

                    if (prev_sline [x - 1]) weight += 1;
                    if (prev_sline [x]) weight += 3;

                    if (next_sline [x - 1]) weight += 1;
                    if (next_sline [x]) weight += 3;
                }
                else {
                    if (curr_sline [x - 1]) weight += 3;
                    if (curr_sline [x + 1]) weight += 3;

                    if (prev_sline [x - 1]) weight += 1;
                    if (prev_sline [x]) weight += 3;
                    if (prev_sline [x + 1]) weight += 1;

                    if (next_sline [x - 1]) weight += 1;
                    if (next_sline [x]) weight += 3;
                    if (next_sline [x + 1]) weight += 1;
                }
            }

            /* set destination pixel according to the weight */
            dst_pixel = _mem_set_pixel (dst_pixel, 
                            bpp, pdc->filter_pixels [weight]);
        }

        prev_sline += char_bmp.bmPitch;
        curr_sline += char_bmp.bmPitch;
        next_sline += char_bmp.bmPitch;
        dst_line += char_bmp.bmPitch;
    }

    dst_pixel = dst_line;
    /* For the last line, set the background and foreground color directly. */
    for (x = 0; x < char_bmp.bmWidth; x++) {
        int weight = 0;

        if (curr_sline [x]) {
            weight = 16;
        }
        else {
            if (x > 0) {
                if (prev_sline [x - 1]) weight += 1;
                if (curr_sline [x - 1]) weight += 3;
            }
            if (x < char_bmp.bmWidth - 1) {
                if (prev_sline [x + 1]) weight += 1;
                if (curr_sline [x + 1]) weight += 3;
            }
            if (prev_sline [x]) weight += 3;
        }

        dst_pixel = _mem_set_pixel (dst_pixel, 
                        bpp, pdc->filter_pixels [weight]);
    }

    return filtered_bits;
}

static BYTE* do_inflate_filtering (PDC pdc)
{
    int x, y;
    int bpp = GAL_BytesPerPixel (pdc->surface);
    BYTE *curr_sline, *prev_sline, *next_sline;
    BYTE *dst_line, *dst_pixel;
    gal_pixel pixel;

    if (allocated_size < char_bits_size) {
        filtered_bits = realloc (filtered_bits, char_bits_size);
        allocated_size = char_bits_size;
    }

    if (filtered_bits == NULL)
    {
        return NULL;
    }

    curr_sline = char_bmp.bmBits;
    next_sline = curr_sline + char_bmp.bmPitch;
    dst_line = filtered_bits;
    dst_pixel = dst_line;;

    for (x = 0; x < char_bmp.bmWidth; x++) {
        if (curr_sline [x]) {
            pixel = pdc->filter_pixels [2];
        }
        else if ((x > 0 && curr_sline [x - 1]) || 
                        (x < char_bmp.bmWidth - 1 && curr_sline [x + 1]) || 
                        next_sline [x])
            pixel = pdc->filter_pixels [1];
        else
            pixel = pdc->filter_pixels [0];

        dst_pixel = _mem_set_pixel (dst_pixel, bpp, pixel);
    }

    prev_sline = curr_sline;
    curr_sline = curr_sline + char_bmp.bmPitch;
    next_sline = curr_sline + char_bmp.bmPitch;
    dst_line += char_bmp.bmPitch;

    for (y = 1; y < char_bmp.bmHeight - 1; y++) {

        dst_pixel = dst_line;
        for (x = 0; x < char_bmp.bmWidth; x++) {
            if (curr_sline [x]) {
                pixel = pdc->filter_pixels [2];
            }
            else if ((x > 0 && curr_sline [x - 1]) || 
                            (x < char_bmp.bmWidth - 1 && curr_sline [x + 1]) ||
                            prev_sline [x] || next_sline [x]) {
                pixel = pdc->filter_pixels [1];
            }
            else
                pixel = pdc->filter_pixels [0];

            dst_pixel = _mem_set_pixel (dst_pixel, bpp, pixel);
        }

        prev_sline += char_bmp.bmPitch;
        curr_sline += char_bmp.bmPitch;
        next_sline += char_bmp.bmPitch;
        dst_line += char_bmp.bmPitch;
    }

    dst_pixel = dst_line;
    for (x = 0; x < char_bmp.bmWidth; x++) {
        if (curr_sline [x]) {
            pixel = pdc->filter_pixels [2];
        }
        else if ((x > 0 && curr_sline [x - 1]) || 
                        (x < char_bmp.bmWidth - 1 && curr_sline [x + 1]) || 
                        prev_sline [x])
            pixel = pdc->filter_pixels [1];
        else
            pixel = pdc->filter_pixels [0];

        dst_pixel = _mem_set_pixel (dst_pixel, bpp, pixel);
    }

    return filtered_bits;
}

static void expand_char_pixmap (PDC pdc, int w, int h, const BYTE* bits, 
            BYTE* expanded, int bold, int italic, int cols, 
            unsigned short scale)
{
    int x, y, w_loop;
    int b = 0;
    BYTE* line;
    unsigned short s_loop;

    w_loop = w / scale;
    line = expanded;
    switch (GAL_BytesPerPixel (pdc->surface)) {
    case 1:
        for (y = 0; y < h; y += scale) {
            Uint8* dst = line;
            memset (dst , pdc->gray_pixels [0], w + bold + italic);

            if (italic)
                dst += (h - y) >> 1;

            for (x = 0; x < w_loop; x++) {

                b = *(bits+x);
                if (b == 255) b = 16;
                else b >>= 4;

                memset (dst, pdc->gray_pixels [b], scale);
                dst += scale;

                if (bold)
                    *dst = pdc->gray_pixels [b];
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;

            bits += cols;
        }
    break;

    case 2:
        for (y = 0; y < h; y += scale) {
            Uint16* dst = (Uint16*) line;
            for (x = 0; x < (w + bold + italic); x ++) {
                dst [x] = pdc->gray_pixels [0];
            }
            
            if (italic)
                dst += ((h - y) >> 1);

            for (x = 0; x < w_loop; x++) {


                b = *(bits+x);
                
                if (b == 255) b = 16;
                else b >>= 4;

                for (s_loop = 0; s_loop < scale; s_loop ++) {
                    *dst = pdc->gray_pixels [b];
                    dst ++;
                }

                if (bold)
                    *dst = pdc->gray_pixels [b];
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;

            bits += cols;
        }
    break;

    case 3:
        for (y = 0; y < h; y += scale) {
            Uint8* expanded = line;
            for (x = 0; x < (w + bold + italic) * 3; x += 3) {
#if 0
                *(Uint16 *) (expanded + x) = pdc->gray_pixels [0];
                *(expanded + x + 2) = pdc->gray_pixels [0] >> 16;
#else
                _mem_set_pixel (expanded + x, 3, pdc->gray_pixels [0]);
#endif
            }

            if (italic)
                expanded += ((h - y) >> 1) * 3;

            for (x = 0; x < w_loop; x++) {
                b = *(bits+x);
                if (b == 255) b = 16;
                else b >>= 4;

                for (s_loop = 0; s_loop < scale; s_loop ++) {
#if 0
                    *(Uint16 *) expanded = pdc->gray_pixels[b];
                    *(expanded + 2) = pdc->gray_pixels[b] >> 16;
                    expanded += 3;
#else
                    expanded = _mem_set_pixel (expanded, 3, pdc->gray_pixels [b]);
#endif
                }

                if (bold) {
#if 0
                    *(Uint16 *)expanded = pdc->gray_pixels[b];
                    *(expanded + 2) = pdc->gray_pixels[b] >> 16;
#else
                    _mem_set_pixel (expanded, 3, pdc->gray_pixels [b]);
#endif
                }
                
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;

            bits += cols;
        }
    break;

    case 4:
        for (y = 0; y < h; y += scale) {
            Uint32* dst = (Uint32*)line;

            for (x = 0; x < (w + bold + italic); x ++) {
                dst [x] = pdc->gray_pixels [0];
            }

            if (italic)
                dst += ((h - y) >> 1);

            for (x = 0; x < w_loop; x++) {

                b = *(bits+x);
                if (b == 255) b = 16;
                else b >>= 4;

                for (s_loop = 0; s_loop < scale; s_loop ++) {
                    *dst = pdc->gray_pixels [b];
                    dst ++;
                }
                if (bold)
                    *dst = pdc->gray_pixels[b];
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;

            bits += cols;
        }
    }
}

static void expand_char_bitmap (int w, int h, 
            const BYTE* bits, int bits_pitch, int bpp, BYTE* expanded, 
            int bg, int fg, int bold, int italic)
{
    int x, y;
    int b = 0;
    BYTE* line;
    const BYTE* line_bits = bits;

    line = expanded;
    switch (bpp) {
    case 1:
        for (y = 0; y < h; y++) {
            expanded = line;
            bits = line_bits;
            for (x = 0; x < (w + bold + italic); x++) {
                *(expanded + x) = bg;
            }

            if (italic)
                expanded += (h - y) >> 1;
            for (x = 0; x < w; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
                    *expanded = fg;
                    if (bold)
                        *(expanded + 1) = fg;
                }

                expanded++;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 2:
        for (y = 0; y < h; y++) {
            expanded = line;
            bits = line_bits;
            for (x = 0; x < (w + bold + italic) << 1; x += 2) {
                *(Uint16 *) (expanded + x) = bg;
            }
            
            if (italic)
                expanded += ((h - y) >> 1) << 1;
            for (x = 0; x < w; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
                    *(Uint16 *) expanded = fg;
                    if (bold)
                        *(Uint16 *)(expanded + 2) = fg;
                }

                expanded += 2;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 3:
        for (y = 0; y < h; y++) {
            expanded = line;
            bits = line_bits;
            for (x = 0; x < (w + bold + italic) * 3; x += 3) {
#if 0
                *(Uint16 *) (expanded + x) = bg;
                *(expanded + x + 2) = bg >> 16;
#else
                _mem_set_pixel (expanded + x, 3, bg);
#endif
            }

            if (italic)
                expanded += 3 * ((h - y) >> 1);

            for (x = 0; x < w; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
#if 0
                    *(Uint16 *) expanded = fg;
                    *(expanded + 2) = fg >> 16;
#else
                    _mem_set_pixel (expanded, 3, fg);
#endif
                    if (bold) {
#if 0
                        *(Uint16 *)(expanded + 3) = fg;
                        *(expanded + 5) = fg >> 16;
#else
                        _mem_set_pixel (expanded + 3, 3, fg);
#endif
                    }
                }
                
                expanded += 3;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 4:
        for (y = 0; y < h; y++) {
            expanded = line;
            bits = line_bits;
            for (x = 0; x < (w + bold + italic) << 2; x += 4) {
                *(Uint32 *) (expanded + x)= bg;
            }

            if (italic)
                expanded += ((h - y) >> 1) << 2;

            for (x = 0; x < w; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
                    *(Uint32 *) expanded = fg;
                    if (bold)
                        *(Uint32 *) (expanded + 4) = fg;
                }

                expanded += 4;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    }
}

static void expand_char_bitmap_scale (int w, int h, 
            const BYTE* bits, int bits_pitch, int bpp, BYTE* expanded, 
            int bg, int fg, int bold, int italic, unsigned short scale)
{
    int x, y, w_loop, b = 0;
    unsigned short s_loop;
    BYTE* line;
    const BYTE* line_bits = bits;

    w_loop = w / scale;
    line = expanded;
    switch (bpp) {
    case 1:
        for (y = 0; y < h; y+=scale) {
            expanded = line;
            bits = line_bits;

            memset (expanded, bg, w + bold + italic);

            if (italic)
                expanded += (h - y) >> 1;

            for (x = 0; x < w_loop; x++) {
                if (x % 8 == 0)
                    b = *bits++;

                if ((b & (128 >> (x % 8)))) {
                    memset (expanded, fg, scale);
                    expanded += scale;

                    if (bold)
                        *expanded = fg;
                }
                else
                    expanded += scale;
            }
            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 2:
        for (y = 0; y < h; y+=scale) {
            expanded = line;
            bits = line_bits;

            for (x = 0; x < (w + bold + italic) << 1; x += 2) {
                *(Uint16 *) (expanded + x) = bg;
            }

            if (italic)
                expanded += ((h - y) >> 1) << 1;

            for (x = 0; x < w_loop; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
                    for (s_loop = 0; s_loop < scale; s_loop++) {
                        *(Uint16 *) expanded = fg;
                        expanded += 2;
                    }
                    if (bold)
                        *(Uint16 *)expanded = fg;
                }
                else
                    expanded += 2 * scale;
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 3:
        for (y = 0; y < h; y+=scale) {
            expanded = line;
            bits = line_bits;

            for (x = 0; x < (w + bold + italic) * 3; x += 3) {
#if 0
                *(Uint16 *) (expanded + x) = bg;
                *(expanded + x + 2) = bg >> 16;
#else
                _mem_set_pixel (expanded + x, 3, bg);
#endif
            }

            if (italic)
                expanded += 3 * ((h - y) >> 1);

            for (x = 0; x < w_loop; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
                    for (s_loop = 0; s_loop < scale; s_loop++) {
#if 0
                        *(Uint16 *) expanded = fg;
                        *(expanded + 2) = fg >> 16;
                        expanded += 3;
#else
                        expanded = _mem_set_pixel (expanded, 3, fg);
#endif
                    }
                    if (bold) {
#if 0
                        *(Uint16 *)expanded = fg;
                        *(expanded + 2) = fg >> 16;
#else
                        _mem_set_pixel (expanded, 3, fg);
#endif
                    }
                }
                else
                    expanded += 3 * scale;
            }
            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 4:
        for (y = 0; y < h; y+=scale) {
            expanded = line;
            bits = line_bits;

            for (x = 0; x < (w + bold + italic) << 2; x += 4) {
                *(Uint32 *) (expanded + x)= bg;
            }

            if (italic)
                expanded += ((h - y) >> 1) << 2;

            for (x = 0; x < w_loop; x++) {

                if (x % 8 == 0)
                    b = *bits++;

                if ((b & (128 >> (x % 8)))) {
                    for (s_loop = 0; s_loop < scale; s_loop++) {
                        *(Uint32 *) expanded = fg;
                        expanded += 4;
                    }

                    if (bold)
                        *(Uint32 *) expanded = fg;
                }
                else {
                    expanded += 4 * scale;
                }
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    }
}


static void expand_char_bitmap_to_rgba_scale (int w, int h, 
            const BYTE* bits, int bits_pitch, RGB* expanded, 
            RGB fg, int bold, int italic, unsigned short scale)
{
    int x; 
    int y; 
    int font_w;
    int font_h;
    int font_x;
    int font_y;
    int italic_blank = 0;

    int b;

    RGB* line_start_rgba;
    BYTE* line_head_bits;

    font_w = (w-italic-bold) / scale;
    font_h = h / scale;

    /*expand font_h line*/
    for (font_y = 0; font_y < font_h; font_y++) {
        
        y = font_y * scale;
        line_head_bits = (BYTE*)bits + bits_pitch * font_y;
        
 
        /*expand a font line*/
        for ( ; y < (font_y+1)* scale; y++) {
            if (italic)
            {
                italic_blank = (h - y) >> 1;
            }

            line_start_rgba = expanded + y * w + italic_blank;

            /*expand a font point*/
            for (font_x = 0; font_x < font_w; font_x++) {

                b = line_head_bits[font_x/8];

                if ((b & (128 >> (font_x % 8)))) {
                    /*a font point => scale bmp point*/
                    for (x = font_x*scale; x < (font_x+1)*scale; x++) {
                        line_start_rgba[x] = fg;
                    }

                    if (bold)
                        line_start_rgba[x] = fg;
                }
                else
                {
                }
            }
        }
    }
}

#define ADJUST_SUBPEXIL_WIEGHT
#ifdef ADJUST_SUBPEXIL_WIEGHT
static void do_subpixel_line(RGB* dist_buf, RGB* pre_line, 
        RGB* cur_line, RGB* next_line, int w, RGB rgba_fg)
{
    int x;
    int weight;
    Uint32 sub_val;

    if (cur_line[0].a == 0)
    {
        weight = 0;
        if (cur_line[1].a != 0)
            weight += NEIGHBOR_WGHT;
        if (pre_line[0].a != 0)
            weight += NEIGHBOR_WGHT;
        if (next_line[0].a != 0)
            weight += NEIGHBOR_WGHT;

        if (pre_line[1].a != 0)
            weight += NEAR_WGHT;
        if (next_line[1].a != 0)
            weight += NEAR_WGHT;

            sub_val = (cur_line[0].r * (ALL_WGHT-weight) + rgba_fg.r * weight)/ALL_WGHT;
            dist_buf[0].r = sub_val;

            sub_val = (cur_line[0].g * (ALL_WGHT-weight) + rgba_fg.g * weight)/ALL_WGHT;
            dist_buf[0].g = sub_val;

            sub_val = (cur_line[0].b * (ALL_WGHT-weight) + rgba_fg.b * weight)/ALL_WGHT;
            dist_buf[0].b = sub_val;

            dist_buf[0].a = 255;

    }
    else
    {
        sub_val = cur_line[0].r * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[1].r * NEIGHBOR_WGHT;

        sub_val += pre_line[0].r * NEIGHBOR_WGHT;
        sub_val += next_line[0].r * NEIGHBOR_WGHT;

        sub_val += pre_line[1].r * NEAR_WGHT;
        sub_val += next_line[1].r * NEAR_WGHT;

        dist_buf[0].r = sub_val/ALL_WGHT;


        sub_val = cur_line[0].g * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[1].g * NEIGHBOR_WGHT;
        sub_val += pre_line[0].g * NEIGHBOR_WGHT;
        sub_val += next_line[0].g * NEIGHBOR_WGHT;

        sub_val += pre_line[1].g * NEAR_WGHT;
        sub_val += next_line[1].g * NEAR_WGHT;

        dist_buf[0].g = sub_val/ALL_WGHT;

        sub_val = cur_line[0].b * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[1].b * NEIGHBOR_WGHT;
        sub_val += pre_line[0].b * NEIGHBOR_WGHT;
        sub_val += next_line[0].b * NEIGHBOR_WGHT;

        sub_val += pre_line[1].b * NEAR_WGHT;
        sub_val += next_line[1].b * NEAR_WGHT;

        dist_buf[0].b = sub_val/ALL_WGHT;

        dist_buf[0].a = 255;
    }

    for (x=1; x<w-1; x++)
    {
        weight = 0;
        if (cur_line[x].a == 0) 
        {
            if (cur_line[x-1].a != 0)
                weight += NEIGHBOR_WGHT;
            if (cur_line[x+1].a != 0)
                weight += NEIGHBOR_WGHT;
            if (pre_line[ x ].a != 0)
                weight += NEIGHBOR_WGHT;
            if (next_line[ x ].a != 0)
                weight += NEIGHBOR_WGHT;

            if (pre_line[x-1].a != 0)
                weight += NEAR_WGHT;
            if (pre_line[x+1].a != 0)
                weight += NEAR_WGHT;
            if (next_line[x-1].a != 0)
                weight += NEAR_WGHT;
            if (next_line[x+1].a != 0)
                weight += NEAR_WGHT;

                sub_val = (cur_line[x].r * (ALL_WGHT-weight) + rgba_fg.r * weight)/ALL_WGHT;
                dist_buf[x].r = sub_val;

                sub_val = (cur_line[x].g * (ALL_WGHT-weight) + rgba_fg.g * weight)/ALL_WGHT;
                dist_buf[x].g = sub_val;

                sub_val = (cur_line[x].b * (ALL_WGHT-weight) + rgba_fg.b * weight)/ALL_WGHT;
                dist_buf[x].b = sub_val;

                dist_buf[x].a = 255;
        }
        else
        {
            sub_val = cur_line[ x ].r * (ALL_WGHT-ROUND_WGHT);
            
            sub_val += cur_line[x-1].r * NEIGHBOR_WGHT;
            sub_val += cur_line[x+1].r * NEIGHBOR_WGHT;
            sub_val += pre_line[ x ].r * NEIGHBOR_WGHT;
            sub_val += next_line[ x ].r * NEIGHBOR_WGHT;


            sub_val += pre_line[x-1].r * NEAR_WGHT;
            sub_val += next_line[x-1].r * NEAR_WGHT;
            sub_val += pre_line[x+1].r * NEAR_WGHT;
            sub_val += next_line[x+1].r * NEAR_WGHT;

            dist_buf[x].r = sub_val/ALL_WGHT;


            sub_val = cur_line[ x ].g * (ALL_WGHT-ROUND_WGHT);
            
            sub_val += cur_line[x-1].g * NEIGHBOR_WGHT;
            sub_val += cur_line[x+1].g * NEIGHBOR_WGHT;
            sub_val += pre_line[ x ].g * NEIGHBOR_WGHT;
            sub_val += next_line[ x ].g * NEIGHBOR_WGHT;


            sub_val += pre_line[x-1].g * NEAR_WGHT;
            sub_val += next_line[x-1].g * NEAR_WGHT;
            sub_val += pre_line[x+1].g * NEAR_WGHT;
            sub_val += next_line[x+1].g * NEAR_WGHT;

            dist_buf[x].g = sub_val/ALL_WGHT;


            sub_val = cur_line[ x ].b * (ALL_WGHT-ROUND_WGHT);
            
            sub_val += cur_line[x-1].b * NEIGHBOR_WGHT;
            sub_val += cur_line[x+1].b * NEIGHBOR_WGHT;
            sub_val += pre_line[ x ].b * NEIGHBOR_WGHT;
            sub_val += next_line[ x ].b * NEIGHBOR_WGHT;


            sub_val += pre_line[x-1].b * NEAR_WGHT;
            sub_val += next_line[x-1].b * NEAR_WGHT;
            sub_val += pre_line[x+1].b * NEAR_WGHT;
            sub_val += next_line[x+1].b * NEAR_WGHT;

            dist_buf[x].b = sub_val/ALL_WGHT;

            dist_buf[x].a = 255;
        }
    }

    weight = 0;
    if (cur_line[x].a == 0) 
    {
        if (cur_line[x-1].a != 0)
            weight += NEIGHBOR_WGHT;
        if (pre_line[ x ].a != 0)
            weight += NEIGHBOR_WGHT;
        if (next_line[ x ].a != 0)
            weight += NEIGHBOR_WGHT;

        if (pre_line[x-1].a != 0)
            weight += NEAR_WGHT;
        if (next_line[x-1].a != 0)
            weight += NEAR_WGHT;

            sub_val = (cur_line[x].r * (ALL_WGHT-weight) + rgba_fg.r * weight)/ALL_WGHT;
            dist_buf[x].r = sub_val;

            sub_val = (cur_line[x].g * (ALL_WGHT-weight) + rgba_fg.g * weight)/ALL_WGHT;
            dist_buf[x].g = sub_val;

            sub_val = (cur_line[x].b * (ALL_WGHT-weight) + rgba_fg.b * weight)/ALL_WGHT;
            dist_buf[x].b = sub_val;

            dist_buf[x].a = 255;

    }
    else
    {
        sub_val = cur_line[x].r * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[x-1].r * NEIGHBOR_WGHT;
        sub_val += pre_line[x].r * NEIGHBOR_WGHT;
        sub_val += next_line[x].r * NEIGHBOR_WGHT;

        sub_val += pre_line[x-1].r * NEAR_WGHT;
        sub_val += next_line[x-1].r * NEAR_WGHT;

        dist_buf[x].r = sub_val/ALL_WGHT;
        
        sub_val = cur_line[x].g * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[x-1].g * NEIGHBOR_WGHT;
        sub_val += pre_line[x].g * NEIGHBOR_WGHT;
        sub_val += next_line[x].g * NEIGHBOR_WGHT;

        sub_val += pre_line[x-1].g * NEAR_WGHT;
        sub_val += next_line[x-1].g * NEAR_WGHT;

        dist_buf[x].g = sub_val/ALL_WGHT;
        
        sub_val = cur_line[x].b * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[x-1].b * NEIGHBOR_WGHT;
        sub_val += pre_line[x].b * NEIGHBOR_WGHT;
        sub_val += next_line[x].b * NEIGHBOR_WGHT;

        sub_val += pre_line[x-1].b * NEAR_WGHT;
        sub_val += next_line[x-1].b * NEAR_WGHT;

        dist_buf[x].b = sub_val/ALL_WGHT;

        dist_buf[x].a = 255;
    }
}

#else /* ADJUST_SUBPEXIL_WIEGHT */

static void do_subpixel_line(RGB* dist_buf, RGB* pre_line, 
        RGB* cur_line, RGB* next_line, int w, RGB rgba_fg)
{
    int x;
    int weight;
    Uint32 sub_val;

    if (cur_line[0].a == 0)
    {
        weight = 0;
        if (cur_line[1].a != 0)
            weight += 3;
        if (pre_line[0].a != 0)
            weight += 3;
        if (next_line[0].a != 0)
            weight += 3;

        if (pre_line[1].a != 0)
            weight++;
        if (next_line[1].a != 0)
            weight++;

            sub_val = (cur_line[0].r * (32-weight) + rgba_fg.r * weight) >> 5;
            dist_buf[0].r = sub_val;

            sub_val = (cur_line[0].g * (32-weight) + rgba_fg.g * weight) >> 5;
            dist_buf[0].g = sub_val;

            sub_val = (cur_line[0].b * (32-weight) + rgba_fg.b * weight) >> 5;
            dist_buf[0].b = sub_val;

            dist_buf[0].a = 255;

    }
    else
    {
        sub_val = cur_line[0].r * 21; /* (32-16+ 3 + 2*1); */
        sub_val += cur_line[1].r * 3;

        sub_val += pre_line[0].r * 3;
        sub_val += next_line[0].r * 3;

        sub_val += pre_line[1].r;
        sub_val += next_line[1].r;

        dist_buf[0].r = sub_val >> 5;


        sub_val = cur_line[0].g * 21;/* (32-16+ 3 + 2*1); */
        sub_val += cur_line[1].g * 3;
        sub_val += pre_line[0].g * 3;
        sub_val += next_line[0].g * 3;

        sub_val += pre_line[1].g;
        sub_val += next_line[1].g;

        dist_buf[0].g = sub_val >> 5;

        sub_val = cur_line[0].b * 21; /* (32-16+ 3 + 2*1); */
        sub_val += cur_line[1].b * 3;
        sub_val += pre_line[0].b * 3;
        sub_val += next_line[0].b * 3;

        sub_val += pre_line[1].b;
        sub_val += next_line[1].b;

        dist_buf[0].b = sub_val >> 5;

        dist_buf[0].a = 255;
    }

    for (x=1; x<w-1; x++)
    {
        weight = 0;
        if (cur_line[x].a == 0) 
        {
            if (cur_line[x-1].a != 0)
                weight += 3;
            if (cur_line[x+1].a != 0)
                weight += 3;
            if (pre_line[ x ].a != 0)
                weight += 3;
            if (next_line[ x ].a != 0)
                weight += 3;

            if (pre_line[x-1].a != 0)
                weight ++;
            if (pre_line[x+1].a != 0)
                weight ++;
            if (next_line[x-1].a != 0)
                weight ++;
            if (next_line[x+1].a != 0)
                weight ++;

                sub_val = (cur_line[x].r * (32-weight) + rgba_fg.r * weight) >> 5;
                dist_buf[x].r = sub_val;

                sub_val = (cur_line[x].g * (32-weight) + rgba_fg.g * weight) >> 5;
                dist_buf[x].g = sub_val;

                sub_val = (cur_line[x].b * (32-weight) + rgba_fg.b * weight) >> 5;
                dist_buf[x].b = sub_val;

                dist_buf[x].a = 255;

        }
        else
        {
            sub_val = cur_line[ x ].r << 4;
            
            sub_val += cur_line[x-1].r * 3;
            sub_val += cur_line[x+1].r * 3;
            sub_val += pre_line[ x ].r * 3;
            sub_val += next_line[ x ].r * 3;


            sub_val += pre_line[x-1].r;
            sub_val += next_line[x-1].r;
            sub_val += pre_line[x+1].r;
            sub_val += next_line[x+1].r;

            dist_buf[x].r = sub_val >> 5;


            sub_val = cur_line[ x ].g << 4 ;
            
            sub_val += cur_line[x-1].g * 3;
            sub_val += cur_line[x+1].g * 3;
            sub_val += pre_line[ x ].g * 3;
            sub_val += next_line[ x ].g * 3;


            sub_val += pre_line[x-1].g;
            sub_val += next_line[x-1].g;
            sub_val += pre_line[x+1].g;
            sub_val += next_line[x+1].g;

            dist_buf[x].g = sub_val >> 5;


            sub_val = cur_line[ x ].b << 4;
            
            sub_val += cur_line[x-1].b * 3;
            sub_val += cur_line[x+1].b * 3;
            sub_val += pre_line[ x ].b * 3;
            sub_val += next_line[ x ].b * 3;


            sub_val += pre_line[x-1].b;
            sub_val += next_line[x-1].b;
            sub_val += pre_line[x+1].b;
            sub_val += next_line[x+1].b;

            dist_buf[x].b = sub_val >> 5;



            dist_buf[x].a = 255;
        }
    }


    weight = 0;
    if (cur_line[x].a == 0) 
    {
        if (cur_line[x-1].a != 0)
            weight += 3;
        if (pre_line[ x ].a != 0)
            weight += 3;
        if (next_line[ x ].a != 0)
            weight += 3;

        if (pre_line[x-1].a != 0)
            weight ++;
        if (next_line[x-1].a != 0)
            weight ++;

            sub_val = (cur_line[x].r * (32-weight) + rgba_fg.r * weight) >> 5;
            dist_buf[x].r = sub_val;

            sub_val = (cur_line[x].g * (32-weight) + rgba_fg.g * weight) >> 5;
            dist_buf[x].g = sub_val;

            sub_val = (cur_line[x].b * (32-weight) + rgba_fg.b * weight) >> 5;
            dist_buf[x].b = sub_val;

            dist_buf[x].a = 255;

    }
    else
    {
        sub_val = cur_line[x].r * 21; /* (32-16+ 3 + 2*1); */
        sub_val += cur_line[x-1].r * 3;
        sub_val += pre_line[x].r * 3;
        sub_val += next_line[x].r * 3;

        sub_val += pre_line[x-1].r;
        sub_val += next_line[x-1].r;

        dist_buf[x].r = sub_val >> 5;
        
        sub_val = cur_line[x].g * 21;
        sub_val += cur_line[x-1].g * 3;
        sub_val += pre_line[x].g * 3;
        sub_val += next_line[x].g * 3;

        sub_val += pre_line[x-1].g;
        sub_val += next_line[x-1].g;

        dist_buf[x].g = sub_val >> 5;
        
        sub_val = cur_line[x].b * 21;
        sub_val += cur_line[x-1].b * 3;
        sub_val += pre_line[x].b * 3;
        sub_val += next_line[x].b * 3;

        sub_val += pre_line[x-1].b;
        sub_val += next_line[x-1].b;

        dist_buf[x].b = sub_val >> 5;

        dist_buf[x].a = 255;
    }

}
#endif /* !ADJUST_SUBPEXIL_WIEGHT */

static void do_subpixel_filter (RGB* rgba_buf, int w, int h, RGB rgba_fg)
{
    RGB* cur_dist_buf = rgba_buf+ w * h;
    RGB* pre_dist_buf = cur_dist_buf + w;
    
    RGB* pre_src_line;
    RGB* cur_src_line;
    RGB* next_src_line;
    RGB* tmp;
    
    int y;

    /* the first line */
    pre_src_line = rgba_buf;
    cur_src_line = rgba_buf;
    next_src_line = rgba_buf + w;

    do_subpixel_line(pre_dist_buf, pre_src_line, cur_src_line, 
        next_src_line, w, rgba_fg);

    /* inner point */
    for (y=1; y<h-1; y++)
    {
        pre_src_line = cur_src_line;
        cur_src_line = next_src_line;
        next_src_line += w;
        
        do_subpixel_line(cur_dist_buf, pre_src_line, cur_src_line, 
            next_src_line, w, rgba_fg);
        
        /*save result of pre_line*/
        memcpy(pre_src_line, pre_dist_buf, w*4);

        /*move roll queue*/
        tmp = cur_dist_buf;
        cur_dist_buf = pre_dist_buf;
        pre_dist_buf = tmp;
    }

    /*cur_line:h-2=>h-1, next_line:h-1=>h-1*/
    pre_src_line = cur_src_line;
    cur_src_line = next_src_line;
    /*next_src_line is itself*/
    do_subpixel_line (cur_dist_buf, pre_src_line, cur_src_line, 
            next_src_line, w, rgba_fg);
    
    memcpy(pre_src_line, pre_dist_buf, w*4);
    memcpy(cur_src_line, cur_dist_buf, w*4);
}

#ifdef _MGFONT_FT2
static void expand_subpixel_freetype (int w, int h, 
                const BYTE* ft_a_buf, int ft_a_pitch, 
                RGB* rgba_buf, RGB rgba_fg, int bold, int italic) 
{
    int x;
    int y;
    int font_w = w - italic;
    const BYTE* sub_a_cur;
    RGB*  rgba_cur;
    Uint32 sub_val;
    int fg_alpha;

    for (y=0; y<h; y++)
    {
        sub_a_cur = ft_a_buf + y * ft_a_pitch;
        
        rgba_cur = rgba_buf + y * w;
        if (italic)
        {
            rgba_cur += (h - y) / 2;
        }

        for (x=0; x<font_w; x++)
        {
#if 0
            sub_val = rgba_fg.r * *sub_a_cur + rgba_cur->r * (255-*sub_a_cur);
            rgba_cur->r = sub_val /255;
            sub_a_cur++;

            sub_val = rgba_fg.g * *sub_a_cur + rgba_cur->g * (255-*sub_a_cur);
            rgba_cur->g = sub_val /255;
            sub_a_cur++;

            sub_val = rgba_fg.b * *sub_a_cur + rgba_cur->b * (255-*sub_a_cur);
            rgba_cur->b = sub_val /255;
            sub_a_cur++;
#else
            if (*sub_a_cur++)
            {
                fg_alpha = *(sub_a_cur-1) + 1;
                sub_val = rgba_fg.r * fg_alpha + rgba_cur->r * (256-fg_alpha);
                rgba_cur->r = sub_val >> 8;
            }

            if (*sub_a_cur++)
            {
                fg_alpha = *(sub_a_cur-1) + 1;
                sub_val = rgba_fg.g * fg_alpha + rgba_cur->g * (256-fg_alpha);
                rgba_cur->g = sub_val >> 8;
            }

            if (*sub_a_cur++)
            {
                fg_alpha = *(sub_a_cur-1) + 1;
                sub_val = rgba_fg.b * fg_alpha + rgba_cur->b * (256-fg_alpha);
                rgba_cur->b = sub_val >> 8;
            }
#endif

            rgba_cur->a = 255;

            if (bold) {
                /* FIXME FIXME */
            }

            rgba_cur++;
        }
    }
}

#endif /* _MGFONT_FT2 */

static void* expand_bkbmp_to_rgba (PDC pdc, DEVFONT* devfont,  
            BITMAP* bk_bmp, const BYTE* bits, 
            int pitch, int bold, int italic, unsigned short scale)
{
    static RGB* rgba_buf = NULL;
    static int    rgba_buf_size = 0;

    int w = bk_bmp->bmWidth;
    int h = bk_bmp->bmHeight;
    int x;
    int y;
    int bpp = bk_bmp->bmBytesPerPixel;
    int needed_rgba_buf_size = w * h * 4 + w*4*2;

    BYTE* src_pixel;
    RGB* dist_line_rgba;

    RGB* src_line_rgba;
    BYTE* dist_pixel;

    gal_pixel pixel;
    RGB rgba_fg;


    /*for char_bmp, and two line used by filter*/
    if (rgba_buf_size < needed_rgba_buf_size)
    {
        rgba_buf_size = needed_rgba_buf_size;
        rgba_buf = realloc(rgba_buf, rgba_buf_size);
    }

    if (rgba_buf == NULL)
        return NULL;

    for (y=0; y<h; y++)
    {
        src_pixel = bk_bmp->bmBits + bk_bmp->bmPitch * y;
        dist_line_rgba = rgba_buf + w * y;

        for (x=0; x<w; x++)
        {
            pixel = _mem_get_pixel(src_pixel, bpp);

            GAL_GetRGBA (pixel, pdc->surface->format, &(dist_line_rgba[x].r),
                    &(dist_line_rgba[x].g), &(dist_line_rgba[x].b),
                    &(dist_line_rgba[x].a));

            dist_line_rgba[x].a = 0;

            src_pixel += bpp;
        }
    }

    /* expand char to rgba_buf; */
    GAL_GetRGBA (pdc->textcolor, pdc->surface->format, &(rgba_fg.r), 
            &(rgba_fg.g), &(rgba_fg.b), &(rgba_fg.a));
    rgba_fg.a = 255;

#ifdef _MGFONT_FT2
    if (ft2IsFreeTypeDevfont (devfont) &&
            ft2GetLcdFilter (devfont) != 0) {
        expand_subpixel_freetype (w, h, bits, pitch, rgba_buf, rgba_fg, 
                bold, italic);
    }
    else 
#endif
    {
        expand_char_bitmap_to_rgba_scale (w, h, bits, pitch, rgba_buf, rgba_fg, 
                bold, italic, scale);
        /*draw rgba_fg to old rgba*/
        do_subpixel_filter (rgba_buf, w, h, rgba_fg);
    }
    
    /*change old_rgba_buf to dc format*/
    for (y=0; y<h; y++)
    {
        src_line_rgba = rgba_buf + w * y;
        dist_pixel = bk_bmp->bmBits + bk_bmp->bmPitch * y;

        for (x=0; x<w; x++)
        {
            pixel = GAL_MapRGB (pdc->surface->format, src_line_rgba[x].r,
                    src_line_rgba[x].g, src_line_rgba[x].b);
            dist_pixel = _mem_set_pixel(dist_pixel, bpp, pixel);
        }
    }

    return bk_bmp->bmBits;
}

//*******************************************************{
static BOOL get_subpixel_bmp (PDC pdc, Glyph32 glyph_value, SIZE* bbx_size, 
        GAL_Rect* fg_gal_rc, int bold, int italic)
{
    LOGFONT* logfont;
    DEVFONT* devfont;
    unsigned short scale;
    int data_pitch = 0;
    const BYTE* data = NULL;
    int font_height;
    bold = 0;

    logfont = pdc->pLogFont;
    devfont = SELECT_DEVFONT (logfont, glyph_value);
    font_height = (devfont->font_ops->get_font_height) (logfont, devfont);

#ifdef _MGFONT_FT2
    if (ft2IsFreeTypeDevfont (devfont) &&
            ft2GetLcdFilter (devfont) &&
            *devfont->font_ops->get_glyph_greybitmap) {
        /* the returned bits will be the subpixled pixmap */
        data = (*devfont->font_ops->get_glyph_greybitmap) (logfont, devfont, 
                REAL_GLYPH(glyph_value), &data_pitch, &scale);
    }
#endif

    if (data == NULL)
    {
        data = (*devfont->font_ops->get_glyph_monobitmap) (logfont, devfont, 
                REAL_GLYPH(glyph_value), &data_pitch, &scale);
    }

    if (data == NULL)
        return 0;

    GAL_GetBox (pdc->surface, fg_gal_rc, &char_bmp);

    if (logfont->style & FS_FLIP_HORZ) {
        HFlipBitmap (&char_bmp,char_bmp.bmBits + 
                char_bmp.bmPitch * char_bmp.bmHeight);
    }
    if (logfont->style & FS_FLIP_VERT) {
        VFlipBitmap (&char_bmp, char_bmp.bmBits + 
                char_bmp.bmPitch * char_bmp.bmHeight);
    }

    expand_bkbmp_to_rgba (pdc, devfont, &char_bmp, (const BYTE*)data , 
            data_pitch, bold, italic, scale);
    return TRUE;
}

static BOOL get_book_bmp (PDC pdc, Glyph32 glyph_value, SIZE* bbx_size, 
        int bold, int italic)
{
    LOGFONT* logfont;
    DEVFONT* devfont;
    unsigned short scale;
    int data_pitch;

    int bpp;
    gal_pixel bgcolor;
    gal_pixel fgcolor;
    

    const BYTE* data;

    logfont = pdc->pLogFont;
    devfont = SELECT_DEVFONT (logfont, glyph_value);
    bgcolor = pdc->bkcolor;

    data = NULL;

    /*get preybitmap and expand*/
    if (devfont->font_ops->get_glyph_greybitmap) {
        data = (*devfont->font_ops->get_glyph_greybitmap) (logfont, devfont, 
                REAL_GLYPH(glyph_value), &data_pitch, &scale);
    }

    if (data)
    {
#if 1 
        if (pdc->bkmode == BM_TRANSPARENT) {
#endif
            if (pdc->alpha_pixel_format && pdc->rop == ROP_SET) {
                /*pdc->gray_pixels -- alpha(0-max)*/
                char_bmp.bmType = BMP_TYPE_PRIV_PIXEL | BMP_TYPE_ALPHA;
                char_bmp.bmAlphaPixelFormat = pdc->alpha_pixel_format;
            }
            else {
                /*pdc->gray_pixels -- from bkcolor to textcolor*/
                char_bmp.bmType = BMP_TYPE_COLORKEY;
                char_bmp.bmColorKey = pdc->gray_pixels [0];
                bgcolor = pdc->gray_pixels [0];
            }
#if 1 
        }
        else  
        {
            /*FIXME can be delete ? */
            char_bmp.bmType = BMP_TYPE_COLORKEY;
            char_bmp.bmColorKey = bgcolor;
        }
#endif 

        /* draw data to char_bmp.bmBits by pdc->gray_pixels*/
        expand_char_pixmap (pdc, bbx_size->cx, bbx_size->cy, data, char_bmp.bmBits, 
                bold, italic, data_pitch, scale);

        return TRUE;
    }

    data = (*devfont->font_ops->get_glyph_monobitmap) (logfont, devfont, 
            REAL_GLYPH(glyph_value), &data_pitch, &scale);

    if (data == NULL)
        return FALSE;

    if (pdc->alpha_pixel_format)
    {
        char_bmp.bmType = BMP_TYPE_PRIV_PIXEL | BMP_TYPE_ALPHA;
        char_bmp.bmAlphaPixelFormat = pdc->alpha_pixel_format;
        bpp = 1;
        bgcolor = 0;
        fgcolor = 1;
    }
    else
    {
        fgcolor = pdc->textcolor;
        bgcolor = pdc->bkcolor;
        bpp = GAL_BytesPerPixel (pdc->surface);
        char_bmp.bmType = BMP_TYPE_COLORKEY;

        if (bgcolor == fgcolor) 
            bgcolor ^= 1;

        char_bmp.bmColorKey = bgcolor;
    }

    if (scale < 2) {
        expand_char_bitmap (bbx_size->cx, bbx_size->cy, data, data_pitch, bpp,
                char_bmp.bmBits, bgcolor, fgcolor, bold, italic);
    }
    else {
        expand_char_bitmap_scale (bbx_size->cx, bbx_size->cy, data, data_pitch, bpp,
                char_bmp.bmBits, bgcolor, fgcolor, 
                bold, italic, scale);
    }

    if (bpp == 1)
        char_bmp.bmBits = do_low_pass_filtering (pdc);
    return TRUE;
}


static BOOL get_light_bmp (PDC pdc, Glyph32 glyph_value, SIZE* bbx_size, 
        GAL_Rect* fg_rect, int bold, int italic)
{
    LOGFONT* logfont;
    DEVFONT* devfont;
    unsigned short scale;
    int pitch;

    gal_pixel bgcolor;
    gal_pixel fgcolor;
    int bpp;
    
    const BYTE* data;

    logfont = pdc->pLogFont;
    devfont = SELECT_DEVFONT (logfont, glyph_value);
    bgcolor = pdc->bkcolor;

    data = NULL;

    data = (*devfont->font_ops->get_glyph_monobitmap) (logfont, devfont, 
            REAL_GLYPH(glyph_value), &pitch, &scale);

    if (data == NULL)
        return FALSE;

    if (pdc->textcolor != pdc->bkcolor)
    {
        char_bmp.bmType = BMP_TYPE_COLORKEY;
        char_bmp.bmColorKey = pdc->filter_pixels [0];

        /* special handle for light style */
        memset (char_bmp.bmBits, 0, char_bits_size);

        if (scale < 2)
        {
            expand_char_bitmap (bbx_size->cx, bbx_size->cy, data, pitch, 1,
                    char_bmp.bmBits + char_bmp.bmPitch + 1, 
                    0, 1, bold, italic);
        }
        else
        {
            expand_char_bitmap_scale (bbx_size->cx, bbx_size->cy, data, pitch, 1,
                    char_bmp.bmBits + char_bmp.bmPitch + 1, 
                    0, 1, bold, italic, scale);
        }

        char_bmp.bmWidth += 2;
        char_bmp.bmHeight += 2;
        char_bmp.bmBits = do_inflate_filtering (pdc);

        /* special handle for light style */

        /*FIXME not need to set pdc->rc_output*/
        //InflateRect (&pdc->rc_output, 1, 1);
        
        fg_rect->x --; fg_rect->y --;
        fg_rect->w += 2; fg_rect->h += 2;
        return TRUE;
    }
    else
    {

        fgcolor = pdc->textcolor;
        bgcolor = pdc->bkcolor;
        char_bmp.bmType = BMP_TYPE_COLORKEY;

        if (bgcolor == fgcolor) 
            bgcolor ^= 1;

        char_bmp.bmColorKey = bgcolor;
        bpp = GAL_BytesPerPixel (pdc->surface);

        if (scale < 2) {
            expand_char_bitmap (bbx_size->cx, bbx_size->cy, data, pitch, bpp,
                    char_bmp.bmBits, bgcolor, fgcolor, bold, italic);
        }
        else {
            expand_char_bitmap_scale (bbx_size->cx, bbx_size->cy, data, pitch, bpp,
                    char_bmp.bmBits, bgcolor, fgcolor, 
                    bold, italic, scale);
        }

        return TRUE;
    }

}

static BOOL get_regular_bmp (PDC pdc, Glyph32 glyph_value, SIZE* bbx_size, 
        int bold, int italic)
{
    LOGFONT* logfont;
    DEVFONT* devfont;
    unsigned short scale;
    int pitch;

    gal_pixel bgcolor = pdc->bkcolor;
    gal_pixel fgcolor = pdc->textcolor;
    int bpp = GAL_BytesPerPixel (pdc->surface);
    
    const BYTE* data;

    logfont = pdc->pLogFont;
    devfont = SELECT_DEVFONT (logfont, glyph_value);
    bgcolor = pdc->bkcolor;

    data = NULL;

    data = (*devfont->font_ops->get_glyph_monobitmap) (logfont, devfont, 
            REAL_GLYPH(glyph_value), &pitch, &scale);

    if (!data)
        return FALSE;

    char_bmp.bmType = BMP_TYPE_COLORKEY;

    if (bgcolor == fgcolor) 
    {
        bgcolor ^= 1;
    }
    char_bmp.bmColorKey = bgcolor;

    if (scale < 2) {
        expand_char_bitmap (bbx_size->cx, bbx_size->cy, data, pitch, bpp,
                char_bmp.bmBits, bgcolor, fgcolor, bold, italic);
    }
    else {
        expand_char_bitmap_scale (bbx_size->cx, bbx_size->cy, data, pitch, bpp,
                char_bmp.bmBits, bgcolor, fgcolor, 
                bold, italic, scale);
    }
    return TRUE;
}

#else /* !DRAW_SCANLINE */

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

BOOL InitTextBitmapBuffer (void)
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

    GAL_GetRGB (pdc->textcolor, pdc->surface->format, &rgba_fg.r,
            &rgba_fg.g, &rgba_fg.b);

    for (x = 0; x < ctxt->bmp_w; x++) {
        if (!PtInRect (&pdc->rc_output, x+xpos, ypos)) {
            bits += 3;
            continue;
        }
        _glyph_move_to_pixel (pdc, x+xpos, ypos);
        pixel = _mem_get_pixel(pdc->cur_dst, GAL_BytesPerPixel (pdc->surface));

            GAL_GetRGB (pixel, pdc->surface->format, &rgba_cur.r,
                    &rgba_cur.g, &rgba_cur.b);

#define C_ALPHA(p, Cs, Cd)          \
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
    DEVFONT* devfont = SELECT_DEVFONT(pdc->pLogFont, glyph_value);

    DWORD glyph_type = devfont->font_ops->get_glyph_type (logfont, devfont);
    switch (glyph_type & DEVFONTGLYPHTYPE_MASK_BMPTYPE) {
        case GLYPHBMP_TYPE_MONO:
            if (logfont->style & FS_WEIGHT_BOOK) {
                if (pdc->alpha_pixel_format)
                    ctxt->cb = _dc_book_scan_line;
            }
            else if (logfont->style & FS_WEIGHT_LIGHT) {
                if (pdc->textcolor != pdc->bkcolor)
                    ctxt->cb = _dc_light_scan_line;
            }
            else if (logfont->style & FS_WEIGHT_SUBPIXEL) {
                ctxt->cb = _dc_subpixel_scan_line;
            }

            if (!ctxt->cb) {
                ctxt->cb = _dc_regular_scan_line;
            }

            data = (BYTE*)(*devfont->font_ops->get_glyph_monobitmap) (logfont, devfont, 
                    REAL_GLYPH(glyph_value), &pitch, &scale);

            if (data == NULL)
                break;

            if (scale > 1 && ctxt->cb != _dc_regular_scan_line) {
                data = _gdi_expand_scale_bits (data, bbox, pitch, bold, scale);
                /* note: pitch is modify. */
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
                data = (BYTE*)(*devfont->font_ops->get_glyph_greybitmap) (logfont, devfont, 
                        REAL_GLYPH (glyph_value), &pitch, &scale);
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
                data = (BYTE*)(*devfont->font_ops->get_glyph_greybitmap) (logfont, devfont, 
                        REAL_GLYPH(glyph_value), &pitch, &scale);
                ctxt->cb = _dc_ft2subpixel_scan_line;
            }
#endif
            break;

        case GLYPHBMP_TYPE_PRERENDER:
            if (devfont->font_ops->get_glyph_prbitmap != NULL) {
                PBITMAP glyph_bmp = &_pre_rdr_bmp;
                if (!(devfont->font_ops->get_glyph_prbitmap) (logfont, devfont,
                            REAL_GLYPH(glyph_value), glyph_bmp)){
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

#endif /* DRAW_SCANLINE */

/*
 * the line direction is from (x1, y1) to (x2, y2),
 * plus advance is the distance from (*parallel_x1,*parallel_y1) to (x1,y1),advance is along direction
 * rotation 90 degree from line direction in counter-clockwise
 * (*parallel_x1,*parallel_y1) is the point at the parallel line reponse to (x1,y1),the line from 
 * (*parallel_x1,*parallel_y1) to (x1,y1) is vertical relative to line from (x1,y1) to (x2,y2)
 * (*parallel_x2,*parallel_y2) is the point at the parallel line reponse to (x2,y2)
 */
void _gdi_get_point_at_parallel(int x1, int y1, int x2, int y2, int advance, int* parallel_x1, 
        int* parallel_y1, int* parallel_x2, int* parallel_y2, PDC pdc)
{
    int adv_x, adv_y;

    adv_x =CAL_VEC_X2(advance,((pdc->pLogFont->rotation +900)%3600+3600)%3600 
            *256/3600);
    adv_y =CAL_VEC_Y2(advance,((pdc->pLogFont->rotation +900)%3600+3600)%3600 
            *256 /3600);

    *parallel_x1 =x1 +adv_x;
    *parallel_y1 =y1 -adv_y;

    *parallel_x2 =x2 +adv_x;
    *parallel_y2 =y2 -adv_y;
}


void _gdi_get_glyph_box_vertices (int x1, int y1, int x2, int y2, POINT* pts, PDC pdc)
{
    int  adv_x, adv_y;
            
    adv_x =CAL_VEC_X2(pdc->pLogFont->ascent,((pdc->pLogFont->rotation +900)%3600 
                +3600)%3600*256/3600);
    adv_y =CAL_VEC_Y2(pdc->pLogFont->ascent,((pdc->pLogFont->rotation +900)%3600 
                +3600)%3600 *256 /3600);
                            
    pts[0].x  = x1+adv_x;
    pts[0].y  = y1-adv_y;
                                                
    pts[1].x =x2 +adv_x;
    pts[1].y =y2 -adv_y;
                                            
                                            
    adv_x =CAL_VEC_X2(pdc->pLogFont->descent,((pdc->pLogFont->rotation +2700)%3600 
                +3600)%3600 * 256 / 3600);
    adv_y =CAL_VEC_Y2(pdc->pLogFont->descent,((pdc->pLogFont->rotation +2700)%3600 
                +3600)%3600 * 256 / 3600);
                                                            
    pts[3].x  = x1+adv_x;
    pts[3].y  = y1-adv_y;
                                                                                
    pts[2].x =x2 +adv_x;
    pts[2].y =y2 -adv_y;
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
        /* TODO: calculate a point on the baseline */

        int adv_x, adv_y;
                        
        if ((pdc->ta_flags & TA_Y_MASK) == TA_TOP) {
            adv_x =CAL_VEC_X2(pdc->pLogFont->ascent,((pdc->pLogFont->rotation 
                            +2700)%3600+3600)%3600 * 256 / 3600);
            adv_y =CAL_VEC_Y2(pdc->pLogFont->ascent,((pdc->pLogFont->rotation 
                            +2700)%3600+3600)%3600 * 256 / 3600);
            *x +=adv_x;
            *y -=adv_y;
        }
        else if ((pdc->ta_flags & TA_Y_MASK) == TA_BOTTOM) {
            adv_x =CAL_VEC_X2(pdc->pLogFont->descent,((pdc->pLogFont->rotation 
                            +900)%3600+3600)%3600 * 256 / 3600);
            adv_y =CAL_VEC_Y2(pdc->pLogFont->descent,((pdc->pLogFont->rotation 
                            +900)%3600+3600)%3600 * 256 / 3600);
            *x +=adv_x;
            *y -=adv_y;
        }
        /*
        int x1 = *x;
        int y1 = *y; 
        int x2 = /x1 + 1000 * fixtoi();
        int y2;

        if ((pdc->ta_flags & TA_Y_MASK) == TA_TOP) {
            _gdi_get_point_at_parallel (x1, y1, x2, y2, 
                    -pdc->pLogFont->ascent, x1, x, y);
        }
        else if ((pdc->ta_flags & TA_Y_MASK) == TA_BOTTOM) {
            _gdi_get_point_at_parallel (x1, y1, x2, y2, 
                    pdc->pLogFont->descent, x1, x, y);
        }
        */
    }
}

#ifdef _MGRM_THREADS
static inline BOOL _gdi_glyph_if_need_lock(DEVFONT* devfont)
{
    BOOL lock = FALSE;
#ifdef _MGFONT_FT2
    lock |= ft2IsFreeTypeDevfont(devfont);
#endif
#ifdef _MGFONT_TTF
    lock |= ftIsFreeTypeDevfont(devfont);
#endif

    return lock;
}
#endif

int _gdi_get_glyph_advance (PDC pdc, Glyph32 glyph_value, 
            BOOL direction, int x, int y, int* adv_x, int* adv_y, BBOX* bbox)
{
    int bold = 0;
    int tmp_x = x;
    int tmp_y = y;
    int adv_len;
    int bbox_x = 0, bbox_y = 0;
    int bbox_w = 0, bbox_h = 0;
    LOGFONT* logfont = pdc->pLogFont;
    DEVFONT* devfont = SELECT_DEVFONT(logfont, glyph_value);

    int advance = 0;
    int glyph_bmptype;

#ifdef _MGRM_THREADS
    BOOL lock = _gdi_glyph_if_need_lock(devfont);
#endif

    bbox_x = x;
    bbox_y = y;

    /* houhh 20091225, noneed to lock for MiniGUI-Process, All App MainWindow
     * is run only in one thread and no desktop thread).*/
#ifdef _MGRM_THREADS
    if (lock) LOCK (&__mg_gdilock);
#endif

    /*in freetype get_glyph_bbox loads glyph, so we must get_glyph_bbox
     * before get_glyph_advance, get_glyph_monobitmap....*/
    devfont->font_ops->get_glyph_bbox (logfont, devfont,
            REAL_GLYPH(glyph_value), &bbox_x, &bbox_y, &bbox_w, &bbox_h);

    if (logfont->style & FS_WEIGHT_BOLD 
            && !(devfont->style & FS_WEIGHT_BOLD)
            && ((devfont->font_ops->get_glyph_type (logfont, devfont) 
                    & DEVFONTGLYPHTYPE_MASK_BMPTYPE)
                == DEVFONTGLYPHTYPE_MONOBMP)) {
        bold = GET_DEVFONT_SCALE (logfont, devfont);
        bbox_w += bold;
    }

    if (bbox) {
        bbox->x = bbox_x;
        bbox->y = bbox_y;
        bbox->w = bbox_w;
        bbox->h = bbox_h;
    }

    adv_len = devfont->font_ops->get_glyph_advance (logfont, devfont, 
            REAL_GLYPH(glyph_value), &tmp_x, &tmp_y);

#ifdef _MGRM_THREADS
    if (lock) UNLOCK (&__mg_gdilock);
#endif

    if (!direction){
        if (bbox) bbox->x -= (bold + pdc->cExtra + adv_len);
    }

    if (direction) {
        tmp_x += bold + pdc->cExtra;
        if (adv_x) *adv_x = tmp_x - x;
        if (adv_y) *adv_y = tmp_y - y;
    }
    else {
        tmp_x -= bold + pdc->cExtra;
        if (adv_x) *adv_x = x - tmp_x;
        if (adv_y) *adv_y = y - tmp_y;
    }

    glyph_bmptype = devfont->font_ops->get_glyph_type (logfont, devfont) 
            & DEVFONTGLYPHTYPE_MASK_BMPTYPE;


    if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP) {
        if (logfont->style & FS_WEIGHT_BOOK_LIGHT) {
            if (adv_x) *adv_x  += 1;
            advance += 1;
        }
    }

    //return adv_len + bold + pdc->cExtra;
    return adv_len + bold + pdc->cExtra + advance;
}


void  _gdi_get_next_point_online (int pre_x, int pre_y, int advance, BOOL direction, PDC pdc, int *next_x, int *next_y)
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
                                                    
    delta_x =CAL_VEC_X2(advance,rotation * 256 / 3600);
    delta_y =CAL_VEC_Y2(advance,rotation * 256 / 3600);

    if(direction){
        *next_x +=delta_x;
        *next_y -=delta_y;
    }
    else{
        *next_x -=delta_x;
        *next_y +=delta_y;
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
        POINT* area, GAL_Rect* gal_rc, int* flag)
{
    int h = pdc->pLogFont->size;
    int ascent = pdc->pLogFont->ascent;

    if(pdc->pLogFont->rotation)
        *flag = ROTATE_RECT;
    else if (pdc->pLogFont->style & FS_SLANT_ITALIC &&
        !(pdc->pLogFont->sbc_devfont->style & FS_SLANT_ITALIC))
        *flag = ITALIC_RECT;
    else
        *flag = ROMAN_RECT;

    switch (*flag)
    {
        case ROMAN_RECT:
        case ITALIC_RECT:
            gal_rc->x = x0;
            gal_rc->y = y0 - ascent;
            gal_rc->w = x1 - x0;
            gal_rc->h = h;
            break;

        case ROTATE_RECT:
            _gdi_get_glyph_box_vertices (x0, y0, x1, y1, 
                     area, pdc);
            break;
    }
}

static void inline draw_back_area (PDC pdc, POINT* area, GAL_Rect* gal_rc, int flag)
{
    int x = gal_rc->x;
    int y = gal_rc->y;
    int w = gal_rc->w;
    int h = gal_rc->h;
    int i;
    int font_style = pdc->pLogFont->style;
    int italic_x;

    switch (flag)
    {
        case ROMAN_RECT:
            _dc_fillbox_clip (pdc, gal_rc);
            break;

        case ITALIC_RECT:
            {
                switch (font_style & FS_FLIP_MASK)
                {
                    case FS_FLIP_HORZ:
                    case FS_FLIP_VERT:
                        for (i=0; i<h; i++)
                        {
                            italic_x = x+(i>>1);
                            _dc_draw_hline_clip (pdc, italic_x, 
                                    italic_x+w-1, y+i);
                        }
                        break;
                    default:
                        for (i=0; i<h; i++)
                        {
                            italic_x = x+((h-i)>>1);
                            _dc_draw_hline_clip (pdc, italic_x, 
                                    italic_x+w-1, y+i);
                        }
                }
            }
            break;

        case ROTATE_RECT:
            MonotoneVerticalPolygonGenerator (pdc, area, 
                    4, _dc_draw_hline_clip);
            break;

        default:
            _MG_PRINTF ("NEWGDI>Glyph: NO WAY\n");
    }

}

static void make_back_rect(RECT* rc_back, POINT* area, GAL_Rect* gal_rc, int flag)
{
    int i;
    switch (flag)
    {
        case ROMAN_RECT:
            rc_back->left = gal_rc->x;
            rc_back->right = gal_rc->x + gal_rc->w;
            rc_back->top = gal_rc->y;
            rc_back->bottom = gal_rc->y + gal_rc->h;
            break;

        case ITALIC_RECT:
            rc_back->left = gal_rc->x;
            rc_back->right = gal_rc->x + gal_rc->w + (gal_rc->h >>1);
            rc_back->top = gal_rc->y;
            rc_back->bottom = gal_rc->y + gal_rc->h;
            break;

        case ROTATE_RECT:
            rc_back->left = area[0].x;
            rc_back->right = area[0].x;
            rc_back->top = area[0].y;
            rc_back->bottom = area[0].y;
            for (i=0; i<4; i++)
            {
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

    make_back_area(pdc, x0, y0, x1, y1, area, &gal_rc, &flag);
    make_back_rect(&rc, area, &gal_rc, flag);

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
                area, &gal_rc, &flag);
    else
        make_back_area(pdc, x+*adv_x, y+*adv_y, x, y, 
                area, &gal_rc, &flag);

    make_back_rect(&rc_back, area, &gal_rc, flag);

    if (!(pdc = __mg_check_ecrgn ((HDC)pdc))) {
        return advance;
    }

#ifndef _MGRM_THREADS
    if (CHECK_DRAWING (pdc)) goto end;
#endif

    pdc->cur_pixel = pdc->bkcolor;
    pdc->cur_ban = NULL;
    pdc->step = 1;

    if(!IntersectRect(&pdc->rc_output, &rc_back, &pdc->rc_output))
        goto end;

    ENTER_DRAWING(pdc);

    draw_back_area (pdc, area, &gal_rc, flag);

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
    if (logfont->style & FS_UNDERLINE_LINE) {
        if (logfont->style & FS_FLIP_VERT) {
            _gdi_get_point_at_parallel(x1, y1, x2, y2, h-(descent<<1), \
                     &draw_x1, &draw_y1, &draw_x2, &draw_y2, pdc);
        }
        else
        {
            draw_x1 = x1;
            draw_y1 = y1;

            draw_x2 = x2;
            draw_y2 = y2;
        }

        if (logfont->rotation == 0 && (logfont->style & FS_SLANT_ITALIC))
        {
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
                _MG_PRINTF ("NEWGDI>Glyph: no under line, rc_output: %d, %d, %d, %d\n",
                    pdc->rc_output.left, pdc->rc_output.top,
                    pdc->rc_output.right, pdc->rc_output.bottom);
            cliprect = cliprect->next;
        }
    } 

    if (logfont->style & FS_STRUCKOUT_LINE) {
        if (logfont->style & FS_FLIP_VERT) {
            _gdi_get_point_at_parallel(x1, y1, x2, y2, (h >>1)-descent +1, \
                     &draw_x1, &draw_y1, &draw_x2, &draw_y2, pdc);
        }
        else {
            _gdi_get_point_at_parallel(x1, y1, x2, y2, (h >>1) - descent, \
                     &draw_x1, &draw_y1, &draw_x2, &draw_y2, pdc);
        }

        if (logfont->rotation == 0 && (logfont->style & FS_SLANT_ITALIC))
        {
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
#ifndef DRAW_SCANLINE 
    BOOL is_draw_glyph = TRUE;
#endif

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
    devfont = SELECT_DEVFONT (logfont, glyph_value);

    glyph_bmptype = devfont->font_ops->get_glyph_type (logfont, devfont) 
            & DEVFONTGLYPHTYPE_MASK_BMPTYPE;

    if ((logfont->style & FS_WEIGHT_BOLD) 
            && !(devfont->style & FS_WEIGHT_BOLD)
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

    if ( pdc->bkmode != BM_TRANSPARENT 
         || logfont->style & FS_UNDERLINE_LINE 
         || logfont->style & FS_STRUCKOUT_LINE ) 
        need_rc_back = TRUE;

    if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP) {
        if (logfont->style & FS_WEIGHT_BOOK_LIGHT) {
            fg_gal_rc.x--; fg_gal_rc.y--;
            fg_gal_rc.w += 2; fg_gal_rc.h += 2;
//            *adv_x  += 1;
//            advance += 1;
        }
    }

    if (need_rc_back) {
        if (direction)
            make_back_area(pdc, x, y, x+*adv_x, y+*adv_y, 
                    area, &bg_gal_rc, &flag);
        else
            make_back_area(pdc, x+*adv_x, y+*adv_y, x, y, 
                    area, &bg_gal_rc, &flag);

        if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP) {
            if (logfont->style & FS_WEIGHT_BOOK_LIGHT) {
                bg_gal_rc.x--; bg_gal_rc.y--;
                bg_gal_rc.w += 2; bg_gal_rc.h += 2;
            }
        }

        make_back_rect(&rc_back, area, &bg_gal_rc, flag);
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


#ifndef _MGRM_THREADS
    if (CHECK_DRAWING (pdc)) goto end;
#endif

    ENTER_DRAWING (pdc);

    pdc->step = 1;
    pdc->cur_ban = NULL;

    /*draw back ground */
    if (pdc->bkmode != BM_TRANSPARENT) {
        pdc->cur_pixel = pdc->bkcolor;
        draw_back_area (pdc, area, &bg_gal_rc, flag);
    }

#ifdef DRAW_SCANLINE 

    /* bbox is the real glyph pixels on one scan-line. */
    bbx_size.cx = bbox.w;
    bbx_size.cy = bbox.h;

    {
        int glyph_ascent = y - bbox.y;
        //int glyph_descent = bbox.y + bbox.h - y;

        _gdi_direct_fillglyph (pdc, glyph_value, &fg_gal_rc, &bbx_size,
            glyph_ascent, advance, italic, bold);
    }

    draw_glyph_lines (pdc, x, y, x + *adv_x, y + *adv_y);

#else /* DRAW_SCANLINE */

    if (devfont->font_ops->get_glyph_prbitmap != NULL) 
    {
        if(!(devfont->font_ops->get_glyph_prbitmap) (logfont, devfont,
                    REAL_GLYPH(glyph_value), &char_bmp)){
            is_draw_glyph = FALSE;
        }
    }
    else {
        prepare_bitmap (pdc, fg_gal_rc.w, fg_gal_rc.h);

        bbx_size.cx = bbox.w;
        bbx_size.cy = bbox.h;

        if (logfont->style & FS_WEIGHT_BOOK) {
            if(! get_book_bmp(pdc, glyph_value, &bbx_size, bold, italic))
                is_draw_glyph = FALSE;
        }
        else if (logfont->style & FS_WEIGHT_LIGHT) {
            if(!get_light_bmp (pdc, glyph_value, &bbx_size,  &fg_gal_rc, bold, italic))
                is_draw_glyph = FALSE;
        }
        else if ((logfont->style & FS_WEIGHT_MASK) == FS_WEIGHT_SUBPIXEL) {
            if (! get_subpixel_bmp (pdc, glyph_value, &bbx_size, 
                        &fg_gal_rc, bold, italic))
                is_draw_glyph = FALSE;
        }
        else {
            if(! get_regular_bmp (pdc, glyph_value, &bbx_size, bold, italic))
                is_draw_glyph = FALSE;
        }

    }
    if (is_draw_glyph) {
        /*draw glyph bitmap*/
        pdc->cur_pixel = pdc->textcolor;
        pdc->skip_pixel = pdc->bkcolor;

        if (char_bmp.bmBits) {
            /* 
             * To optimize:
             * the flip should do when expand bitmap/pixmap.
             */
            if (logfont->style & FS_FLIP_HORZ) {
                HFlipBitmap (&char_bmp, char_bmp.bmBits + 
                        char_bmp.bmPitch * char_bmp.bmHeight);
            }
            if (logfont->style & FS_FLIP_VERT) {
                VFlipBitmap (&char_bmp, char_bmp.bmBits + 
                        char_bmp.bmPitch * char_bmp.bmHeight);
            }

            _dc_fillbox_bmp_clip (pdc, &fg_gal_rc, &char_bmp);
        }
    }
    draw_glyph_lines (pdc, x, y, x + *adv_x, y + *adv_y);
#endif /* !DRAW_SCANLINE */

    LEAVE_DRAWING (pdc);
end:
    pdc->rc_output = rc_tmp;
    UNLOCK_GCRINFO (pdc);
    return advance;
}

int _gdi_get_italic_added_width (LOGFONT* logfont)
{
    if (logfont->style & FS_SLANT_ITALIC
        && !(logfont->sbc_devfont->style & FS_SLANT_ITALIC)) {
        return (logfont->size + 1) >> 1;
    }
    else {
        return 0;
    }

}
void _gdi_start_new_line (PDC pdc)
{
    DEVFONT* sbc_devfont = pdc->pLogFont->sbc_devfont;
    DEVFONT* mbc_devfont = pdc->pLogFont->mbc_devfont;

    if (mbc_devfont) {
        if (mbc_devfont->font_ops->start_str_output) {
            mbc_devfont->font_ops->start_str_output(pdc->pLogFont, mbc_devfont);
        }
    }

    if (sbc_devfont->font_ops->start_str_output) {
        sbc_devfont->font_ops->start_str_output(pdc->pLogFont, sbc_devfont);
    }
}

