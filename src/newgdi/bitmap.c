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
** Bitmap operations of GDI.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/06/12, derived from original gdi.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "pixel_ops.h"
#include "cursor.h"
#include "readbmp.h"
#include "bitmap.h"
#include "fixedmath.h"

BOOL BitmapDDAScalerEx (void* context, const BITMAP* src_bmp, 
        int dst_w, int dst_h,
        CB_GET_LINE_BUFF cb_line_buff, CB_LINE_SCALED cb_line_scaled, 
        GAL_PixelFormat *format);

BOOL BitmapDDAScaler2 (void* context, const BITMAP* src_bmp, int dst_w, int dst_h,
            CB_GET_LINE_BUFF cb_line_buff, CB_LINE_SCALED cb_line_scaled);

#define BitmapDDAScaler(context, src_bmp, dst_w, \
                    dst_h, cb_get_line_buff, cb_line_scaled) \
        BitmapDDAScalerEx(context, src_bmp, dst_w,  \
                    dst_h, cb_get_line_buff, cb_line_scaled, NULL) 

BOOL BitmapDDAScaler2 (void* context, const BITMAP* src_bmp, int dst_w, int dst_h,
            CB_GET_LINE_BUFF cb_line_buff, CB_LINE_SCALED cb_line_scaled);

/****************************** Bitmap Support *******************************/
void _dc_fillbox_clip (PDC pdc, const GAL_Rect* rect)
{
    PCLIPRECT cliprect;
    RECT eff_rc;

    cliprect = pdc->ecrgn.head;
    if (pdc->rop == ROP_SET) {
        while (cliprect) {
            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)) {
                SET_GAL_CLIPRECT (pdc, eff_rc);
                GAL_FillRect (pdc->surface, rect, pdc->cur_pixel);
            }
            cliprect = cliprect->next;
        }
    }
    else {
        pdc->step = 1;
        while (cliprect) {
            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)) {
                int _w = RECTW(eff_rc), _h = RECTH(eff_rc);
                pdc->move_to (pdc, eff_rc.left, eff_rc.top);
                while (_h--) {
                    pdc->draw_pixel_span (PDC_TO_COMP_CTXT(pdc), _w);
                    pdc->cur_dst += pdc->surface->pitch;
                }
            }
            cliprect = cliprect->next;
        }
    }

}

#ifdef _MGHAVE_ADV_2DAPI
void _dc_fillbox_clip_nosolid_brush (PDC pdc, const GAL_Rect* rect)
{
    PCLIPRECT cliprect;
    RECT eff_rc;
    
    SetRect(&eff_rc, rect->x, rect->y,
            rect->x + rect->w, rect->y + rect->h);

    cliprect = pdc->ecrgn.head;
    while (cliprect) {
        if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)) {
            int _w = RECTW(eff_rc), _h = RECTH(eff_rc);
            int y = eff_rc.top;
            while (_h--) {
                _dc_fill_span_brush_helper (pdc, eff_rc.left, y++, _w);
            }
        }

        cliprect = cliprect->next;
    }
}
#endif

void _dc_fillbox_bmp_clip (PDC pdc, const GAL_Rect* rect, BITMAP* bmp)
{
    PCLIPRECT cliprect;
    RECT eff_rc;

    cliprect = pdc->ecrgn.head;
    if (pdc->rop == ROP_SET) {
        while (cliprect) {
            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)) {
                SET_GAL_CLIPRECT (pdc, eff_rc);
                GAL_PutBox (pdc->surface, rect, bmp);
            }

            cliprect = cliprect->next;
        }
    }
    else {
        BYTE* row;
        int _w, _h;

        pdc->step = 1;
        while (cliprect) {
            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)) {

                pdc->move_to (pdc, eff_rc.left, eff_rc.top);
                row = bmp->bmBits + bmp->bmPitch * (eff_rc.top - rect->y)
                        + bmp->bmBytesPerPixel * (eff_rc.left - rect->x);

                _h = RECTH(eff_rc); _w = RECTW(eff_rc);
                while (--_h) {
                    pdc->draw_src_span (PDC_TO_COMP_CTXT(pdc),
                        row, pdc->bkmode, _w);

                    row += bmp->bmPitch;
                    _dc_step_y (pdc, 1);
                }
            }

            cliprect = cliprect->next;
        }
    }
}

void GUIAPI FillBox (HDC hdc, int x, int y, int w, int h)
{
    PDC pdc;
    GAL_Rect rect;

    if (w<=0 || h<=0) {
        return;
    }

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    /* Transfer logical to device to screen here. */
    w += x; h += y;
    coor_LP2SP (pdc, &x, &y);
    coor_LP2SP (pdc, &w, &h);
    SetRect (&pdc->rc_output, x, y, w, h);
    NormalizeRect (&pdc->rc_output);
    w = RECTW (pdc->rc_output); h = RECTH (pdc->rc_output);
    rect.x = x; rect.y = y; rect.w = w; rect.h = h;

    pdc->cur_pixel = pdc->brushcolor;
    pdc->cur_ban = NULL;
    pdc->step = 1;

    ENTER_DRAWING (pdc);

#ifdef _MGHAVE_ADV_2DAPI
    if (pdc->brush_type == BT_SOLID)
        _dc_fillbox_clip (pdc, &rect);
    else
        _dc_fillbox_clip_nosolid_brush (pdc, &rect);
#else
    _dc_fillbox_clip (pdc, &rect);
#endif
    
    LEAVE_DRAWING (pdc);
    UNLOCK_GCRINFO (pdc);
}

BOOL GUIAPI GetBitmapFromDC (HDC hdc, int x, int y, int w, int h, BITMAP* bmp)
{
    PDC pdc;
    GAL_Rect rect;
    int ret;

    pdc = dc_HDC2PDC (hdc);
    if (dc_IsGeneralDC (pdc)) {
        LOCK_GCRINFO (pdc);
        dc_GenerateECRgn (pdc, FALSE);
    }

    w += x; h += y;
    coor_LP2SP (pdc, &x, &y);
    coor_LP2SP (pdc, &w, &h);
    SetRect (&pdc->rc_output, x, y, w, h);
    NormalizeRect (&pdc->rc_output);
    w = RECTW (pdc->rc_output); h = RECTH (pdc->rc_output);
    rect.x = x; rect.y = y; rect.w = w; rect.h = h;

    ENTER_DRAWING_NOCHECK (pdc);

    ret = GAL_GetBox (pdc->surface, &rect, bmp);

    LEAVE_DRAWING_NOCHECK (pdc);
    UNLOCK_GCRINFO (pdc);

    if (ret) return FALSE;
    return TRUE;
}

static void* _get_line_buff_fillbox (void* context, int y, void** alpha_line_mask)
{
    struct _SCALER_INFO_FILLBMP* info = (struct _SCALER_INFO_FILLBMP*) context;

    *(BYTE**)alpha_line_mask = info->line_alpha_buff;
    return info->line_buff;
}

static void _line_scaled_fillbox (void* context, const void* line, int y)
{
    int top;
    CLIPRECT* cliprect;
    RECT rc_output, eff_rc;
    struct _SCALER_INFO_FILLBMP* info = (struct _SCALER_INFO_FILLBMP*) context;

    if (!_dc_which_region_ban (info->pdc, info->dst_rc.top + y))
        return;

    //if (info->pdc->rop != ROP_SET) {
    if (info->pdc->rop == ROP_SET) {
        BITMAP tmp_bmp = *info->bmp;
        GAL_Rect rect;
        rect.x = info->dst_rc.left;
        rect.y = info->dst_rc.top + y;
        rect.w = RECTW (info->dst_rc);
        rect.h = 1;

        rc_output.left = info->dst_rc.left;
        rc_output.right = info->dst_rc.right;
        rc_output.top = info->dst_rc.top + y;
        rc_output.bottom = rc_output.top + 1;
        
        tmp_bmp.bmWidth = rect.w;
        tmp_bmp.bmHeight = 1;
        tmp_bmp.bmPitch = GAL_BytesPerPixel (info->pdc->surface) * rect.w;
        tmp_bmp.bmBits = (Uint8*)line;
        if (tmp_bmp.bmType & BMP_TYPE_ALPHA_MASK) {
            tmp_bmp.bmAlphaMask = (Uint8*)info->line_alpha_buff;
            tmp_bmp.bmAlphaPitch = (tmp_bmp.bmWidth + 3) & (~3);
        }
        else {
            tmp_bmp.bmAlphaMask = NULL;
            tmp_bmp.bmAlphaPitch = 0;
        }

        cliprect = info->pdc->cur_ban;
        top = cliprect->rc.top;
        while (cliprect && cliprect->rc.top == top) {
            if (IntersectRect (&eff_rc, &rc_output, &cliprect->rc)) {
                SET_GAL_CLIPRECT (info->pdc, eff_rc);
                GAL_PutBox (info->pdc->surface, &rect, &tmp_bmp);
            }

            cliprect = cliprect->next;
        }
    }
    else {
        rc_output.left = info->dst_rc.left;
        rc_output.right = info->dst_rc.right;
        rc_output.top = info->dst_rc.top + y;
        rc_output.bottom = rc_output.top + 1;

        /* draw in this ban */
        cliprect = info->pdc->cur_ban;
        top = cliprect->rc.top;
        while (cliprect && cliprect->rc.top == top) {
            if (IntersectRect (&eff_rc, &rc_output, &cliprect->rc)) {
                BYTE* row = (BYTE*)line;
                row += GAL_BytesPerPixel (info->pdc->surface) * 
                        (eff_rc.left - rc_output.left);
                info->pdc->move_to (info->pdc, eff_rc.left, eff_rc.top);
                info->pdc->draw_src_span (PDC_TO_COMP_CTXT(info->pdc),
                        row, info->pdc->bkmode, RECTW (eff_rc));
            }
            cliprect = cliprect->next;
        }
    }
}

static int _bmp_decode_rle (const BITMAP* bmp, const BYTE* encoded,
                            BYTE* decoded, int* skip, int* run)
{
    int i, consumed = 0;

    *skip = 0;
    *run = 0;

    if (bmp->bmBytesPerPixel > 3) {
        *skip = MGUI_ReadLE16Mem (&encoded);
        *run = MGUI_ReadLE16Mem (&encoded);
        consumed += 4;
    }
    else {
        *skip = encoded [0];
        *run = encoded [1];
        consumed += 2;
        encoded += 2;
    }

    for (i = 0; i < *run; i++) {
        decoded = _mem_set_pixel (decoded, bmp->bmBytesPerPixel, 
                *((gal_pixel*)encoded));
    }

    if (*run > 0) {
        consumed += bmp->bmBytesPerPixel;
    }
    
    return consumed;
}

static int _bmp_decode_alpha_rle (const BITMAP* bmp, const BYTE* encoded,
                            BYTE* decoded, int* skip, int* run)
{
    int i, consumed = 0;
    *skip = 0;
    *run = 0;

    if (bmp->bmBytesPerPixel > 3) {
        *skip = MGUI_ReadLE16Mem (&encoded);
        *run = MGUI_ReadLE16Mem (&encoded);
        consumed += 4;
    }
    else {
        *skip = encoded [0];
        *run = encoded [1];
        consumed += 2;
        encoded += 2;
    }
    
    for (i = 0; i < *run; i++) {
        decoded[i] = *encoded;
    }

    if (*run > 0) {
        consumed += 1;
    }
    
    return consumed;
}

PDC _begin_fill_bitmap (HDC hdc, int x, int y, int w, int h, 
                             const BITMAP* bmp, FILLINFO* fill_info)
{
    PDC pdc;
    GAL_Rect rect;
    int sw, sh, size;
    struct _SCALER_INFO_FILLBMP *info;

    if (!bmp || bmp->bmWidth == 0 || bmp->bmHeight == 0 || bmp->bmBits == NULL)
        return NULL;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return NULL;

    info = &fill_info->scaler_info;
    info->line_buff = NULL;
    fill_info->decoded_buff = NULL;
    fill_info->decoded_alpha_buff = NULL;

    sw = bmp->bmWidth;
    sh = bmp->bmHeight;

    if (bmp->bmType & BMP_TYPE_RLE) {
        size = (bmp->bmBytesPerPixel * sw +3) & (-3);
        fill_info->decoded_buff = malloc (size);
        if (fill_info->decoded_buff == NULL)
            goto fail;
        size = (sw +3) & (~3);
        fill_info->decoded_alpha_buff = malloc(size);
        if (fill_info->decoded_alpha_buff == NULL)
            goto fail;
        fill_info->encoded_bits = bmp->bmBits;
        fill_info->encoded_alpha_mask = bmp->bmAlphaMask;

        /* disable scale for the RLE bitmap */
        w = sw; h = sh;
    }
    else {
        if (w <= 0) w = sw;
        if (h <= 0) h = sh;
    }

    /* Transfer logical to device to screen here. */
    w += x; h += y;
    coor_LP2SP (pdc, &x, &y);
    coor_LP2SP (pdc, &w, &h);
    SetRect (&pdc->rc_output, x, y, w, h);
    NormalizeRect (&pdc->rc_output);
    w = RECTW (pdc->rc_output); h = RECTH (pdc->rc_output);
    rect.x = x; rect.y = y; rect.w = w; rect.h = h;

    if (w != sw || h != sh) {
        info->pdc = pdc;
        info->bmp = bmp;
        SetRect (&info->dst_rc, rect.x, rect.y, 
                        rect.x + rect.w, rect.y + rect.h);
        info->line_buff = malloc (GAL_BytesPerPixel (pdc->surface) * w);
        if (info->line_buff == NULL) {
            goto fail;
        }

        info->line_alpha_buff = malloc (w);
        if (info->line_alpha_buff == NULL) {
            goto fail;
        }
    }
    else {
        info->line_buff = NULL;
        info->line_alpha_buff = NULL;
    }

    pdc->step = 1;
    pdc->cur_ban = NULL;
    pdc->cur_pixel = pdc->brushcolor;
    pdc->skip_pixel = bmp->bmColorKey;
    fill_info->old_bkmode = pdc->bkmode;
    if (bmp->bmType & BMP_TYPE_COLORKEY) {
        pdc->bkmode = BM_TRANSPARENT;
    }

    if(__mg_enter_drawing (pdc) < 0) {
        goto fail;
    }

    fill_info->dst_rect = rect;

    return pdc;

fail:
    UNLOCK_GCRINFO (pdc);
    if (fill_info->decoded_buff)
        free (fill_info->decoded_buff);
    if (fill_info->decoded_alpha_buff)
        free (fill_info->decoded_alpha_buff);
    if (info->line_buff)
        free (info->line_buff);
    if (info->line_alpha_buff)
        free (info->line_alpha_buff);
    return NULL;
}

void _end_fill_bitmap (PDC pdc, const BITMAP* bmp, FILLINFO *fill_info)
{
    pdc->bkmode = fill_info->old_bkmode;

    __mg_leave_drawing (pdc);

    if (fill_info->decoded_buff)
        free (fill_info->decoded_buff);

    if (fill_info->decoded_alpha_buff)
        free (fill_info->decoded_alpha_buff);

    if (fill_info->scaler_info.line_buff)
        free (fill_info->scaler_info.line_buff);

    if (fill_info->scaler_info.line_alpha_buff)
        free (fill_info->scaler_info.line_alpha_buff);

    UNLOCK_GCRINFO (pdc);
}

static void _fill_bitmap (PDC pdc, const BITMAP *bmp, FILLINFO *fill_info)
{
    int sw = bmp->bmWidth;
    int sh = bmp->bmHeight;
    int w = fill_info->dst_rect.w;
    int h = fill_info->dst_rect.h;

    if (w != sw || h != sh) {
        if (pdc->bitmap_scaler) {
            pdc->bitmap_scaler(&fill_info->scaler_info, bmp, w, h,
                _get_line_buff_fillbox, _line_scaled_fillbox, 
                pdc->surface->format);
        }
        else{
            BitmapDDAScaler (&fill_info->scaler_info, bmp, w, h,
                _get_line_buff_fillbox, _line_scaled_fillbox);
        }
    }
    else {
        _dc_fillbox_bmp_clip (pdc, &fill_info->dst_rect, (BITMAP*)bmp);
    }
}

void _fill_bitmap_scanline (PDC pdc, const BITMAP* bmp, 
                FILLINFO* fill_info, int y)
{
    GAL_Rect dst_rect;
    dst_rect.x = fill_info->dst_rect.x;
    dst_rect.y = fill_info->dst_rect.y + y;
    dst_rect.w = fill_info->dst_rect.w;
    dst_rect.h = 1;

    _dc_fillbox_bmp_clip (pdc, &dst_rect, (BITMAP*)bmp);
}

BOOL GUIAPI FillBoxWithBitmap (HDC hdc, int x, int y, int w, int h, 
                const BITMAP* bmp)
{
    PDC pdc;
    FILLINFO fill_info;

    pdc = _begin_fill_bitmap (hdc, x, y, w, h, bmp, &fill_info);
    if (pdc == NULL)
        return FALSE;

    if (bmp->bmType & BMP_TYPE_RLE) {
        int y;
        GAL_Rect dst_rect;
        BITMAP my_bmp = *bmp;
        int consumed, skip, run;
        int consumed_alpha, skip_alpha, run_alpha;

        my_bmp.bmHeight = 1;
        my_bmp.bmBits = fill_info.decoded_buff;
        my_bmp.bmAlphaMask = fill_info.decoded_alpha_buff;

        for (y = 0; y < fill_info.dst_rect.h; y ++) {
            dst_rect.x = fill_info.dst_rect.x;
            dst_rect.y = fill_info.dst_rect.y + y;
            dst_rect.h = 1;

            do {
                /* Decode pixel data */
                consumed = _bmp_decode_rle (bmp, fill_info.encoded_bits, 
                        fill_info.decoded_buff, &skip, &run);
                fill_info.encoded_bits += consumed;
                
                /* Decode alpha data */
                if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                    consumed_alpha = _bmp_decode_alpha_rle (bmp, fill_info.encoded_alpha_mask,
                            fill_info.decoded_alpha_buff, &skip_alpha, &run_alpha);

                    if ((skip != skip_alpha) || (run != run_alpha)) {
                        fprintf(stderr, "Bitmap data error!\n");
                        return FALSE;
                    }
                    fill_info.encoded_alpha_mask += consumed_alpha;
                }

                /* Draw bmp clip */
                if ((skip + run) > 0) {
                    dst_rect.x += skip;
                    dst_rect.w = run;
                    _dc_fillbox_bmp_clip (pdc, &dst_rect, &my_bmp);

                    dst_rect.x += run;
                }
            } while (skip || run);
        }
        
    }
    else
        _fill_bitmap (pdc, bmp, &fill_info);

    _end_fill_bitmap (pdc, bmp, &fill_info);

    return TRUE;
}

#define ISTRANSL(pixel, fmt) (((pixel & fmt->Amask) >> fmt->Ashift) == 0)
#define ISOPAQUE(pixel1, pixel2) (pixel1 == pixel2)

static int _get_skip_run(HDC hdc, PBITMAP bmp, const BYTE *bits,
        const BYTE *alpha_mask, int *skip, int *run, int len)
{
    PDC pdc;
    GAL_PixelFormat *fmt;
    int i;

    *skip = 0;
    *run = 0;
    pdc = dc_HDC2PDC(hdc);
    fmt = pdc->surface->format;
    if (len >= bmp->bmWidth) {
        return -1;
    }
    
    switch (bmp->bmBytesPerPixel) {
        case 1:
        case 2:
        case 3:
            return -1;
        case 4:
            {
                Uint32 *pixel = (Uint32 *)bits;
                if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                    for (i = len; i < bmp->bmWidth; i++) {
                        if (alpha_mask[i] == 0) {
                            (*skip)++;
                        } else {
                            break;
                        }
                    }
                    if ((len + *skip) == bmp->bmWidth) {
                        break;
                    }
                    for (i = len + *skip; i < bmp->bmWidth -1; i++) {
                        if (ISOPAQUE(pixel[i], pixel[i+1])
                                && alpha_mask[i] == alpha_mask[i+1]) {
                            (*run)++;
                        } else {
                            break;
                        }
                    }
                    (*run)++;
                } else {
                    if (fmt->Amask) {
                        /* Bitmap has its own alpha */
                        for (i = len; i < bmp->bmWidth; i++) {
                            if (ISTRANSL(pixel[i], fmt)) {
                                (*skip)++;
                            } else {
                                break;
                            }
                        }
                        if ((len + *skip)== bmp->bmWidth) {
                            break;
                        }
                    }
                    for (i = len + *skip; i < bmp->bmWidth -1; i++) {
                        if (ISOPAQUE(pixel[i], pixel[i+1])) {
                            (*run)++;
                        } else {
                            break;
                        }
                    }
                    (*run)++;
                }
            }
            break;
    }
    
    return (*skip + *run);
}

#define CPY_WORD(dst, src) memcpy(dst, src, 2), dst += 2
static int _bmp_encode_rle_sl(HDC hdc, PBITMAP bmp,
        const BYTE *src_bits, const BYTE *src_alpha_mask,
        int *size_pixel, int *size_alpha)
{
    BYTE *p_dst_bits, *p_dst_alpha;
    BYTE *p_src_bits = (BYTE *)src_bits;
    BYTE *p_src_alpha = (BYTE *)src_alpha_mask;
    int total = 0, ret;
    int skip, run, i;

    while (total < bmp->bmWidth)
    {

        ret = _get_skip_run(hdc, bmp, src_bits, src_alpha_mask,
                    &skip, &run, total);
        total += ret;

        switch(bmp->bmBytesPerPixel)
        {
            case 1:
            case 2:
            case 3:
                return -1;
            case 4:
                {
                    /* Encode pixel data */
                    p_dst_bits = bmp->bmBits + *size_pixel;
                    /* Save skip and run */
                    CPY_WORD(p_dst_bits, &skip);
                    CPY_WORD(p_dst_bits, &run);
                    (*size_pixel) += 4;

                    p_src_bits += skip * bmp->bmBytesPerPixel;
                    if (run > 0) {
                        memcpy(p_dst_bits, p_src_bits, bmp->bmBytesPerPixel);
                        p_src_bits += run * bmp->bmBytesPerPixel;
                        (*size_pixel) += bmp->bmBytesPerPixel;
                    }
                    
                    /* Encode alpha mask */
                    if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        p_dst_alpha = bmp->bmAlphaMask + *size_alpha;
                        /* Save skip and run in alpha_mask */
                        CPY_WORD(p_dst_alpha, &skip);
                        CPY_WORD(p_dst_alpha, &run);
                        (*size_alpha) += 4;

                        p_src_alpha += skip;
                        if (run > 0) {
                            p_dst_alpha[0] = p_src_alpha[0];
                            p_src_alpha += run;
                            (*size_alpha) += 1;
                        }
                    }
                }
                break;
        }
    }

    /* To mark one row end of 0,0... */
    if (bmp->bmBytesPerPixel == 4) {
        for (i = 0; i < 4; i++) {
            bmp->bmBits[(*size_pixel)++] = 0;
        }
    }
    if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
        for (i = 0; i < 4; i++) {
            bmp->bmAlphaMask[(*size_alpha)++] = 0;
        }
    }
    
    return 0;
}
int GUIAPI EncodeRleBitmap(HDC hdc, PBITMAP bmp)
{
    BYTE* bmBits = NULL;
    BYTE *src_bits = NULL, *src_alpha_mask = NULL;
    int total_pixel = 0, total_alpha = 0;
    int size, alpha_pitch = 0, y;

    if (!bmp || bmp->bmWidth == 0 || bmp->bmHeight == 0
            || bmp->bmBits == NULL) {
        fprintf(stderr, "EncodeRleBitmap: Bitmap data error!\n");
        return -1;
    }
    if (bmp->bmBytesPerPixel < 4) {
        fprintf(stderr, "Not support %d pixel now!\n", bmp->bmBitsPerPixel);
        return -1;
    }

    /* Save pixel data */
    size = bmp->bmPitch * bmp->bmHeight;
    src_bits = calloc(1, size);
    if (src_bits == NULL) {
        fprintf(stderr, "EncodeRleBitmap: malloc error!\n");
        return -1;
    }
    memcpy(src_bits, bmp->bmBits, size);

    /*  Worst case */
    bmBits = realloc(bmp->bmBits, size * 2);
    if (bmBits == NULL) {
        free(src_bits);
        fprintf(stderr, "EncodeRleBitmap: realloc pixel data  error!\n");
        return -1;
    }
    /* If use alpha_mask */
    if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
        if (bmp->bmAlphaMask == NULL) {
            free(src_bits);
            free(bmBits);
            fprintf(stderr, "EncodeRleBitmap: Alpha mask data error!\n");
            return -1;
        }

        /* Save alpha mask data */
        alpha_pitch = bmp->bmAlphaPitch;
        size = alpha_pitch * bmp->bmHeight;
        src_alpha_mask = calloc(1, size);
        if (src_alpha_mask == NULL) {
            free(src_bits);
            free(bmBits);
            fprintf(stderr, "EncodeRleBitmap: malloc error!\n");
            return -1;
        }
        memcpy(src_alpha_mask, bmp->bmAlphaMask, size);

        /*  Worst case */
        bmp->bmAlphaMask = realloc(bmp->bmAlphaMask, size * 5);
        if (bmp->bmAlphaMask == NULL) {
            free(src_bits);
            free(bmBits);
            free(src_alpha_mask);
            fprintf(stderr, "EncodeRleBitmap: realloc alpha mask error!\n");
            return -1;
        }
    }
    bmp->bmBits = bmBits;
    
    /* Encode pixel data */
    for (y = 0; y < bmp->bmHeight; y++) {
        _bmp_encode_rle_sl(hdc, bmp, src_bits, src_alpha_mask,
                &total_pixel, &total_alpha);
        src_bits += bmp->bmPitch;
        if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
            src_alpha_mask += alpha_pitch;
        }
    }
    bmp->bmBits = realloc(bmp->bmBits, total_pixel);
    bmp->bmBitsPerPixel = total_pixel;
    if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
        bmp->bmAlphaMask = realloc(bmp->bmAlphaMask, total_alpha);
    }
    
    bmp->bmType |= BMP_TYPE_RLE;

    /* End: free data */
    src_bits -= bmp->bmPitch * bmp->bmHeight;
    if (src_bits) {
        free(src_bits);
    }
    if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
        src_alpha_mask -= alpha_pitch * bmp->bmHeight;
        if (src_alpha_mask) {
            free(src_alpha_mask);
        }
    }

    return 0;
}

struct _SCALER_INFO_FILLBMPPART
{
    PDC pdc;
    int dst_x, dst_y;
    int dst_w, dst_h;
    int off_x, off_y;
    BYTE* line_buff;
    BYTE* line_alpha_buff;
    const BITMAP* bmp;
};

static void* _get_line_buff_fillboxpart (void* context, int y, void** alpha_line_mask)
{
    struct _SCALER_INFO_FILLBMPPART* info = 
            (struct _SCALER_INFO_FILLBMPPART*) context;

    *(BYTE**)alpha_line_mask = info->line_alpha_buff;
    return info->line_buff;
}

static void _line_scaled_fillboxpart (void* context, const void* line, int y)
{
    int top;
    RECT rc_output, eff_rc;
    CLIPRECT* cliprect; 

    struct _SCALER_INFO_FILLBMPPART* info = 
            (struct _SCALER_INFO_FILLBMPPART*) context;

    if (y < info->off_y || y >= (info->off_y + info->dst_h))
        return;

    if (!_dc_which_region_ban (info->pdc, info->dst_y + y - info->off_y))
        return;

    if (info->pdc->rop == ROP_SET) {
        BITMAP tmp_bmp = *info->bmp;
        GAL_Rect rect;
        rect.x = info->dst_x;
        rect.y = info->dst_y + y - info->off_y;
        rect.w = info->dst_w;
        rect.h = 1;

        rc_output.left = info->dst_x;
        rc_output.right = info->dst_x + info->dst_w;
        rc_output.top = info->dst_y + y - info->off_y;
        rc_output.bottom = rc_output.top + 1;

        tmp_bmp.bmWidth = info->dst_w;
        tmp_bmp.bmHeight = 1;
        tmp_bmp.bmPitch = GAL_BytesPerPixel (info->pdc->surface) * info->dst_w;
        tmp_bmp.bmBits = (Uint8*)line;
        tmp_bmp.bmBits += GAL_BytesPerPixel (info->pdc->surface) * info->off_x;
        if (tmp_bmp.bmType & BMP_TYPE_ALPHA_MASK) {
            tmp_bmp.bmAlphaMask = (Uint8*)info->line_alpha_buff + info->off_x;
            tmp_bmp.bmAlphaPitch = (tmp_bmp.bmWidth + 3) & (~3);
        }
        else {
            tmp_bmp.bmAlphaMask = NULL;
            tmp_bmp.bmAlphaPitch = 0;
        }

        cliprect = info->pdc->cur_ban;
        top = cliprect->rc.top;
        while (cliprect && cliprect->rc.top == top) {
            if (IntersectRect (&eff_rc, &rc_output, &cliprect->rc)) {
                SET_GAL_CLIPRECT (info->pdc, eff_rc);
                GAL_PutBox (info->pdc->surface, &rect, &tmp_bmp);
            }

            cliprect = cliprect->next;
        }
    }
    else {
        rc_output.left = info->dst_x;
        rc_output.right = info->dst_x + info->dst_w;
        rc_output.top = info->dst_y + y - info->off_y;
        rc_output.bottom = rc_output.top + 1;

        /* draw in this ban */
        cliprect = info->pdc->cur_ban;
        top = cliprect->rc.top;
        while (cliprect && cliprect->rc.top == top) {
            if (IntersectRect (&eff_rc, &rc_output, &cliprect->rc)) {
                BYTE* row = (BYTE*)line;
                row += GAL_BytesPerPixel (info->pdc->surface) * 
                        (info->off_x + eff_rc.left - rc_output.left);
                info->pdc->move_to (info->pdc, eff_rc.left, eff_rc.top);
                info->pdc->draw_src_span (PDC_TO_COMP_CTXT(info->pdc),
                        row, info->pdc->bkmode, RECTW (eff_rc));
            }
            cliprect = cliprect->next;
        }
    }
}

BOOL GUIAPI FillBoxWithBitmapPart (HDC hdc, int x, int y, int w, int h,
                int bw, int bh, const BITMAP* bmp, int xo, int yo)
{
    PDC pdc;
    GAL_Rect rect;
    int old_bkmode;
    struct _SCALER_INFO_FILLBMPPART info;
    
    if (bmp->bmWidth == 0 || bmp->bmHeight == 0 || bmp->bmBits == NULL)
        return FALSE;

    if (bmp->bmType & BMP_TYPE_RLE)
        return FALSE;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return TRUE;

    /* Transfer logical to device to screen here. */
    w += x; h += y;
    coor_LP2SP(pdc, &x, &y);
    coor_LP2SP(pdc, &w, &h);
    SetRect (&pdc->rc_output, x, y, w, h);
    NormalizeRect (&pdc->rc_output);
    w = RECTW (pdc->rc_output); h = RECTH (pdc->rc_output);
    rect.x = x; rect.y = y; rect.w = w; rect.h = h;

    if (bw <= 0) bw = bmp->bmWidth;
    if (bh <= 0) bh = bmp->bmHeight;

    if (xo < 0 || yo < 0 || (xo + w) > bw || (yo + h) > bh) {
        UNLOCK_GCRINFO (pdc);
        return FALSE;
    }

    if (bw != bmp->bmWidth || bh != bmp->bmHeight) {
        info.pdc = pdc;
        info.dst_x = x; info.dst_y = y;
        info.dst_w = w; info.dst_h = h;
        info.off_x = xo; info.off_y = yo;
        info.bmp = bmp;
        info.line_buff = malloc (GAL_BytesPerPixel (pdc->surface) * bw);
        if (info.line_buff == NULL) {
            UNLOCK_GCRINFO (pdc);
            return FALSE;
        }

        info.line_alpha_buff = malloc (bw);
        if (info.line_alpha_buff == NULL) {
            free (info.line_buff);
            UNLOCK_GCRINFO (pdc);
            return FALSE;
        }
    }
    else {
        info.line_buff = NULL;
        info.line_alpha_buff = NULL;
    }

    pdc->step = 1;
    pdc->cur_ban = NULL;
    pdc->cur_pixel = pdc->brushcolor;
    pdc->skip_pixel = bmp->bmColorKey;
    old_bkmode = pdc->bkmode;
    if (bmp->bmType & BMP_TYPE_COLORKEY) {
        pdc->bkmode = BM_TRANSPARENT;
    }

    ENTER_DRAWING (pdc);

    if (bw != bmp->bmWidth || bh != bmp->bmHeight) {

        pdc->bitmap_scaler(&info, bmp, bw, bh,
                _get_line_buff_fillboxpart, 
                _line_scaled_fillboxpart,
                pdc->surface->format);
    }
    else {
        BITMAP part = *bmp;
        if (xo != 0 || yo != 0) {
            part.bmBits += part.bmPitch * yo + 
                    xo * GAL_BytesPerPixel (pdc->surface);
            part.bmAlphaMask += yo * part.bmAlphaPitch + xo;
        }
        _dc_fillbox_bmp_clip (pdc, &rect, &part);
    }

    pdc->bkmode = old_bkmode;

    LEAVE_DRAWING (pdc);

    if (info.line_buff)
        free (info.line_buff);

    if (info.line_alpha_buff)
        free (info.line_alpha_buff);

    UNLOCK_GCRINFO (pdc);

    return TRUE;
}

BOOL GUIAPI FillBitmapPartInBox (HDC hdc, int box_x, int box_y, int box_w, int box_h, 
    const BITMAP* pbmp, int bmp_left, int bmp_top, int bmp_width, int bmp_height)
{
    int w_scale, h_scale;
    BOOL bret;
    if(!pbmp) 
        return FALSE;
    
    if(bmp_width<=0) 
        bmp_width = pbmp->bmWidth;
    if(bmp_height<=0)
        bmp_height = pbmp->bmHeight;
    if(box_w<=0)
        box_w = bmp_width;
    if(box_h<=0)
        box_h = bmp_height;
    
    if(bmp_width<=0 || bmp_height<=0)
        return FALSE;
    
    w_scale = ((box_w<<16) + bmp_width - 1)/bmp_width;
    h_scale = ((box_h<<16) + bmp_height - 1)/bmp_height;

    bret = FillBoxWithBitmapPart(hdc,
        box_x, box_y, box_w, box_h,
        (pbmp->bmWidth*w_scale)>>16, (pbmp->bmHeight*h_scale)>>16,
        pbmp,
        (bmp_left*w_scale)>>16, (bmp_top*h_scale)>>16);
    return bret;
}

void GUIAPI BitBlt (HDC hsdc, int sx, int sy, int sw, int sh,
        HDC hddc, int dx, int dy, DWORD dwRop)
{
    PCLIPRECT cliprect;
    PDC psdc, pddc;
    RECT srcOutput, dstOutput;
    GAL_Rect dst, src;
    RECT eff_rc;

    psdc = dc_HDC2PDC (hsdc);
    if (!(pddc = __mg_check_ecrgn (hddc)))
        return;

    /* The coordinates should be in device space. */
#if 0
    sw += sx; sh += sy;
    coor_LP2SP (psdc, &sx, &sy);
    coor_LP2SP (psdc, &sw, &sh);
    SetRect (&srcOutput, sx, sy, sw, sh);
    NormalizeRect (&srcOutput);
    (sw > sx) ? (sw -= sx) : (sw = sx - sw);
    (sh > sy) ? (sh -= sy) : (sh = sy - sh);
    coor_LP2SP (pddc, &dx, &dy);
    SetRect (&dstOutput, dx, dy, dx + sw, dy + sh);
    NormalizeRect (&dstOutput);
#else
    if (sw <= 0) sw = RECTW (psdc->DevRC);
    if (sh <= 0) sh = RECTH (psdc->DevRC);

    coor_DP2SP (psdc, &sx, &sy);
    SetRect (&srcOutput, sx, sy, sx + sw, sy + sh);

    coor_DP2SP (pddc, &dx, &dy);
    SetRect (&dstOutput, dx, dy, dx + sw, dy + sh);
#endif

    if (pddc->surface == psdc->surface) {
        if (sx == dx && sy == dy)
            goto empty_ret;
        GetBoundRect (&pddc->rc_output, &srcOutput, &dstOutput);
    }
    else
        pddc->rc_output = dstOutput;

    if (pddc->surface != psdc->surface && (psdc->surface ==
                __gal_screen))
        psdc->rc_output = srcOutput;

    ENTER_DRAWING (pddc);

    if (pddc->surface !=  psdc->surface && (psdc->surface ==
                __gal_screen))
        kernel_ShowCursorForGDI (FALSE, psdc);

    /*
     * The CLIPRECT's RECT order is: rc1 rc2 rc3 rc4 rc5 rc6
     *
     * In same TOP   rc1 -> rc2 -> rc3..  -->   
     *      ( rc1.left > rc2.left > rc3.left .. rc1.top == rc2.top == rc3.top ...)
     * In same TOP   rc4 -> rc5 -> rc6 .. -->   
     *      ( rc4.left > rc5.left ...  rc4.top==rc5.top ...  rc1.top < rc4.top < rc6.top ... )
     * ....
     *
     * if surface is same, and dy > sy, should 
     *      if dx > sx, should copy every rect like order: rc6 rc5 rc4, rc3 ... rc1
     *      else like: rc4 rc5 rc6, rc1 rc2 rc3 ...
     *
     * if dy <= sy, should
     *      if dx > sx, should rc3 rc2 rc1 ..., rc6 rc5 rc4 ...
     *      else should     rc1 rc2 rc3 ..., rc4 rc5 rc6 ...
     *
     */
    if (pddc->surface == psdc->surface) {
        if (dy > sy) {
            cliprect = pddc->ecrgn.tail;
            if (dx > sx) {
                while (cliprect) {
                    if (IntersectRect (&eff_rc, &pddc->rc_output, &cliprect->rc)) {
                        SET_GAL_CLIPRECT (pddc, eff_rc);

                        src.x = sx; src.y = sy; src.w = sw; src.h = sh;
                        dst.x = dx; dst.y = dy; dst.w = sw; dst.h = sh;
                        GAL_BlitSurface (psdc->surface, &src, pddc->surface, &dst);
                    }
                    cliprect = cliprect->prev;
                }
            }
            else {
                int ban_top;
                PCLIPRECT ban_first = NULL, ban_saved;

                while (cliprect) {
                    /* to find the first clipping rect of the current ban. */
                    ban_top = cliprect->rc.top;
                    ban_first = cliprect;
                    while (ban_first && ban_top == ban_first->rc.top) {
                        ban_saved = ban_first;
                        ban_first = ban_first->prev;
                    }
                    /* ban_first is the last clipping rect of the previous ban */
                    if (ban_first) {
                        cliprect = ban_first;
                        ban_first = ban_first->next;
                    }
                    else {
                        ban_first = ban_saved;
                        cliprect = NULL;
                    }

                    while (ban_first && ban_top == ban_first->rc.top) {
                        if (IntersectRect (&eff_rc, &pddc->rc_output, &ban_first->rc)) {
                            SET_GAL_CLIPRECT (pddc, eff_rc);

                            src.x = sx; src.y = sy; src.w = sw; src.h = sh;
                            dst.x = dx; dst.y = dy; dst.w = sw; dst.h = sh;
                            GAL_BlitSurface (psdc->surface, &src, pddc->surface, &dst);
                        }
                        ban_first = ban_first->next;
                    }
                }
            }
        }
        else {
            cliprect = pddc->ecrgn.head;
            if (dx > sx) {
                int ban_top;
                PCLIPRECT ban_last = NULL, ban_saved;

                while (cliprect) {
                    /* to find the last clipping rect of the current ban. */
                    ban_top = cliprect->rc.top;
                    ban_last = cliprect;
                    while (ban_last && ban_top == ban_last->rc.top) {
                        ban_saved = ban_last;
                        ban_last = ban_last->next;
                    }
                    /* ban_last is the first clipping rect of the next ban */
                    if (ban_last) {
                        cliprect = ban_last;
                        ban_last = ban_last->prev;
                    }
                    else {
                        ban_last = ban_saved;
                        cliprect = NULL;
                    }

                    while (ban_last && ban_top == ban_last->rc.top) {
                        if (IntersectRect (&eff_rc, &pddc->rc_output, &ban_last->rc)) {
                            SET_GAL_CLIPRECT (pddc, eff_rc);

                            src.x = sx; src.y = sy; src.w = sw; src.h = sh;
                            dst.x = dx; dst.y = dy; dst.w = sw; dst.h = sh;
                            GAL_BlitSurface (psdc->surface, &src, pddc->surface, &dst);
                        }
                        ban_last = ban_last->prev;
                    }
                }
            }
            else {
                while (cliprect) {
                    if (IntersectRect (&eff_rc, &pddc->rc_output, &cliprect->rc)) {
                        SET_GAL_CLIPRECT (pddc, eff_rc);

                        src.x = sx; src.y = sy; src.w = sw; src.h = sh;
                        dst.x = dx; dst.y = dy; dst.w = sw; dst.h = sh;
                        GAL_BlitSurface (psdc->surface, &src, pddc->surface, &dst);
                    }
                    cliprect = cliprect->next;
                }
            }
        }
    }
    else {
        cliprect = pddc->ecrgn.head;
        while (cliprect) {
            if (IntersectRect (&eff_rc, &pddc->rc_output, &cliprect->rc)) {
                SET_GAL_CLIPRECT (pddc, eff_rc);

                src.x = sx; src.y = sy; src.w = sw; src.h = sh;
                dst.x = dx; dst.y = dy; dst.w = sw; dst.h = sh;
                GAL_BlitSurface (psdc->surface, &src, pddc->surface, &dst);
            }
            cliprect = cliprect->next;
        }
    }

    if (pddc->surface !=  psdc->surface && (psdc->surface ==
                __gal_screen))
        kernel_ShowCursorForGDI (TRUE, psdc);

    LEAVE_DRAWING (pddc);

empty_ret:
    UNLOCK_GCRINFO (pddc);
}


struct _SCALER_INFO_STRETCHBLT
{
    PDC pdc;
    int dst_x, dst_y;
    int dst_w, dst_h;
    BYTE* line_buff;
    BOOL bottom2top;
};

static void* _get_line_buff_stretchblt (void* context, int y, void** alpha_line_mask)
{
    struct _SCALER_INFO_STRETCHBLT* info = 
            (struct _SCALER_INFO_STRETCHBLT*) context;

    return info->line_buff;
}

static void _line_scaled_stretchblt (void* context, const void* line, int y)
{
    struct _SCALER_INFO_STRETCHBLT* info = 
            (struct _SCALER_INFO_STRETCHBLT*) context;

    if (info->bottom2top)
        info->pdc->cur_ban = NULL;

    if (_dc_which_region_ban (info->pdc, info->dst_y + y)) {
        int top;
        CLIPRECT* cliprect;
        RECT rc_output, eff_rc;

        rc_output.left = info->dst_x;
        rc_output.right = info->dst_x + info->dst_w;
        rc_output.top = info->dst_y + y;
        rc_output.bottom = rc_output.top + 1;

        /* draw in this ban */
        cliprect = info->pdc->cur_ban;
        top = cliprect->rc.top;
        while (cliprect && cliprect->rc.top == top) {
            if (IntersectRect (&eff_rc, &rc_output, &cliprect->rc)) {
                BYTE* row = (BYTE*)line;
                row += GAL_BytesPerPixel (info->pdc->surface) * 
                        (eff_rc.left - rc_output.left);
                info->pdc->move_to (info->pdc, eff_rc.left, eff_rc.top);
                info->pdc->draw_src_span (PDC_TO_COMP_CTXT(info->pdc),
                        row, info->pdc->bkmode, RECTW (eff_rc));
            }
            cliprect = cliprect->next;
        }
    }
}

void GUIAPI StretchBlt (HDC hsdc, int sx, int sy, int sw, int sh,
                       HDC hddc, int dx, int dy, int dw, int dh, DWORD dwRop)
{
    PDC psdc, pddc;
    RECT srcOutput, dstOutput;
    BITMAP bmp;
    struct _SCALER_INFO_STRETCHBLT info;

    psdc = dc_HDC2PDC (hsdc);
    if (!(pddc = __mg_check_ecrgn (hddc)))
        return;

    if (GAL_BitsPerPixel (psdc->surface) != GAL_BitsPerPixel (pddc->surface)) {
        goto error_ret;
    }

    if (sx < 0) sx = 0;
    if (sy < 0) sy = 0;
    if (dx < 0) dx = 0;
    if (dy < 0) dy = 0;

    if (sx >= RECTW(psdc->DevRC))
        goto error_ret;
    if (sy >= RECTH(psdc->DevRC))
        goto error_ret;
    if (dx >= RECTW(pddc->DevRC))
        goto error_ret;
    if (dy >= RECTH(pddc->DevRC))
        goto error_ret;

    if (sw <= 0 || sw > RECTW (psdc->DevRC) - sx) sw = RECTW (psdc->DevRC) - sx;
    if (sh <= 0 || sh > RECTH (psdc->DevRC) - sy) sh = RECTH (psdc->DevRC) - sy;
    if (dw <= 0 || dw > RECTW (pddc->DevRC) - dx) dw = RECTW (pddc->DevRC) - dx;
    if (dh <= 0 || dh > RECTH (pddc->DevRC) - dy) dh = RECTH (pddc->DevRC) - dy;


    /* The coordinates should be in device space. */
#if 0
    // Transfer logical to device to screen here.
    sw += sx; sh += sy;
    coor_LP2SP(psdc, &sx, &sy);
    coor_LP2SP(psdc, &sw, &sh);
    SetRect (&srcOutput, sx, sy, sw, sh);
    NormalizeRect (&srcOutput);
    sw = RECTW (srcOutput); sh = RECTH (srcOutput);

    dw += dx; dh += dy;
    coor_LP2SP (pddc, &dx, &dy);
    coor_LP2SP (pddc, &dw, &dh);
    SetRect (&dstOutput, dx, dy, dw, dh);
    NormalizeRect (&dstOutput);
    dw = RECTW (dstOutput); dh = RECTH (dstOutput);
#else
    coor_DP2SP (psdc, &sx, &sy);
    SetRect (&srcOutput, sx, sy, sx + sw, sy + sh);
/*
    if (!IsCovered (&srcOutput, &psdc->DevRC))
        goto error_ret;
*/

    coor_DP2SP (pddc, &dx, &dy);
    SetRect (&dstOutput, dx, dy, dx + dw, dy + dh);
#endif

    info.pdc = pddc;
    info.dst_x = dx; info.dst_y = dy;
    info.dst_w = dw; info.dst_h = dh;
    info.line_buff = malloc (GAL_BytesPerPixel (pddc->surface) * dw);

    if (info.line_buff == NULL) {
        goto error_ret;
    }

    memset (&bmp, 0, sizeof (bmp));
    bmp.bmType = BMP_TYPE_NORMAL;
    bmp.bmBitsPerPixel = GAL_BitsPerPixel (psdc->surface);
    bmp.bmBytesPerPixel = GAL_BytesPerPixel (psdc->surface);
    bmp.bmWidth = sw;
    bmp.bmHeight = sh;
    bmp.bmPitch = psdc->surface->pitch;
    bmp.bmBits = (unsigned char*)psdc->surface->pixels 
            + sy * psdc->surface->pitch 
            + sx * GAL_BytesPerPixel (psdc->surface);

    if (pddc->surface ==  psdc->surface)
        GetBoundRect (&pddc->rc_output, &srcOutput, &dstOutput);
    else
        pddc->rc_output = dstOutput;

    if (pddc->surface !=  psdc->surface && (psdc->surface == __gal_screen))
        psdc->rc_output = srcOutput;

    ENTER_DRAWING (pddc);

    if (pddc->surface !=  psdc->surface && (psdc->surface == __gal_screen))
        kernel_ShowCursorForGDI (FALSE, psdc);

#if 0
    PCLIPRECT cliprect;
    GAL_Rect src, dst;
    RECT eff_rc;

    if (pddc->surface == psdc->surface && dy > sy)
        cliprect = pddc->ecrgn.tail;
    else
        cliprect = pddc->ecrgn.head;
    while(cliprect) {
        if (IntersectRect (&eff_rc, &pddc->rc_output, &cliprect->rc)) {
            SET_GAL_CLIPRECT (pddc, eff_rc);

            src.x = sx; src.y = sy; src.w = sw; src.h = sh;
            dst.x = dx; dst.y = dy; dst.w = dw; dst.h = dh;
            GAL_SoftStretch (psdc->surface, &src, pddc->surface, &dst);
        }

        if (pddc->surface == psdc->surface && dy > sy)
            cliprect = cliprect->prev;
        else
            cliprect = cliprect->next;
    }
#else
    pddc->step = 1;
    pddc->cur_ban = NULL;
    if (pddc->surface == psdc->surface 
                    && DoesIntersect (&srcOutput, &dstOutput)) {
        info.bottom2top = TRUE;
        BitmapDDAScaler2 (&info, &bmp, dw, dh,
                _get_line_buff_stretchblt, _line_scaled_stretchblt);
    }
    else {
        info.bottom2top = FALSE;
        /*
        BitmapDDAScaler (&info, &bmp, dw, dh,
                _get_line_buff_stretchblt, _line_scaled_stretchblt);
                */
        if ( pddc->bitmap_scaler ) 
        {
            pddc->bitmap_scaler(&info, &bmp, dw, dh,
                    _get_line_buff_stretchblt, 
                    _line_scaled_stretchblt,
                    pddc->surface->format);
        }
        else
        {
            BitmapDDAScaler (&info, &bmp, dw, dh,
                    _get_line_buff_stretchblt, _line_scaled_stretchblt);
        }

    }
#endif

    if (pddc->surface !=  psdc->surface && (psdc->surface == __gal_screen))
        kernel_ShowCursorForGDI (TRUE, psdc);

    LEAVE_DRAWING (pddc);

    free (info.line_buff);

error_ret:
    UNLOCK_GCRINFO (pddc);
}

/* 
 * This function performs a fast box scaling.
 * This is a DDA-based algorithm; Iteration over target bitmap.
 *
 * This function comes from SVGALib, Copyright 1993 Harm Hanemaayer
 */

/* We use the 32-bit to 64-bit multiply and 64-bit to 32-bit divide of the
 * 386 (which gcc doesn't know well enough) to efficiently perform integer
 * scaling without having to worry about overflows.
 */

#if defined(__GNUC__) && defined(i386)

static inline int muldiv64(int m1, int m2, int d)
{
    /* int32 * int32 -> int64 / int32 -> int32 */
    int result;
    int dummy;
    __asm__ __volatile__ (
               "imull %%edx\n\t"
               "idivl %4\n\t"
  :            "=a"(result), "=d"(dummy)    /* out */
  :            "0"(m1), "1"(m2), "g"(d)     /* in */
        );
    return result;
}

#else

static inline int muldiv64 (int m1, int m2, int d)
{
    Sint64 mul = (Sint64) m1 * m2;

    return (int) (mul / d);
}

#endif

typedef signed int Int32;
typedef unsigned long Word32;

typedef struct  _Int64_
{
    Word32 lo;
    Word32 hi;
}Int64;

static inline void Neg64( Int64 *x )
{
    /* Remember that -(0x80000000) == 0x80000000 with 2-complement! */
    /* We take care of that here.                                   */

    x->hi ^= 0xFFFFFFFFUL;
    x->lo ^= 0xFFFFFFFFUL;
    x->lo++;

    if ( !x->lo )
    {
        x->hi++;
        if ( x->hi == 0x80000000UL )  /* Check -MaxInt32 - 1 */
        {
            x->lo--;
            x->hi--;  /* We return 0x7FFFFFFF! */
        }
    }
}

static inline Int32 Div64by32( Int64 *x, Int32 y )
{
    Int64 tmp;
    Int32 s;
    Word32 q, r, i, lo;

    tmp.hi = x->hi;
    tmp.lo = x->lo;

    s  = tmp.hi; 
    if ( s < 0 ) 
        Neg64( &tmp );
    s ^= y;
    y = ABS( y );

    /* Shortcut */
    if ( tmp.hi == 0 ){
        q = tmp.lo / y;
        return ( s < 0 ) ? -(Int32)q : (Int32)q;
    }

    r  = tmp.hi;
    lo = tmp.lo;

    if ( r >= (Word32)y )   /* we know y is to be treated as unsigned here */
        return ( s < 0 ) ? 0x80000001UL : 0x7FFFFFFFUL;
                            /* Return Max/Min Int32 if divide overflow */
                            /* This includes division by zero!         */
    q = 0;
    for ( i = 0; i < 32; i++ ){
        r <<= 1;
        q <<= 1;
        r  |= lo >> 31;

        if ( r >= (Word32)y ){
            r -= y;
            q |= 1;
        }
        lo <<= 1;
    }
    return ( s < 0 ) ? -(Int32)q : (Int32)q;
}

static void inline MulTo64( Int32  x, Int32  y, Int64*  z )
{
    Int32   s;
    Word32  lo1, hi1, lo2, hi2, lo, hi, i1, i2;

    s  = x; x = ABS( x );
    s ^= y; y = ABS( y );

    lo1 = x & 0x0000FFFF;  hi1 = x >> 16;
    lo2 = y & 0x0000FFFF;  hi2 = y >> 16;

    lo = lo1*lo2;
    i1 = lo1*hi2;
    i2 = lo2*hi1;
    hi = hi1*hi2;

    /* Check carry overflow of i1 + i2 */
    if ( i2 )
    {
        if ( i1 >= (Word32)-(Int32)i2 )
            hi += 1L << 16;
        i1 += i2;
    }

    i2 = i1 >> 16;
    i1 = i1 << 16;
    /* Check carry overflow of i1 + lo */
    if ( i1 )
    {
        if ( lo >= (Word32)-(Int32)i1 )
            hi++;
        lo += i1;
    }

    hi += i2;

    z->lo = lo;
    z->hi = hi;

    if ( s < 0 )
        Neg64( z );
}

static void inline Add64( Int64*  x, Int64*  y, Int64*  z )
{
    register Word32  lo, hi;

    lo = x->lo + y->lo;
    hi = x->hi + y->hi + ( lo < x->lo );

    z->lo = lo;
    z->hi = hi;
}

static void inline get_alpha_with_alphamask (BYTE *src, const BITMAP* src_bmp, Uint8 *pa)
{
    int x = (src - src_bmp->bmBits) % src_bmp->bmPitch;
    int y = (src - src_bmp->bmBits) / src_bmp->bmPitch;
    *pa = src_bmp->bmAlphaMask[y * src_bmp->bmAlphaPitch + x / src_bmp->bmBytesPerPixel];
}

/* 
 * f(i+u, j+v) = (1-u) * (1-v) * f(i, j) + (1-u)  * v * f(i, j+1) + 
 *                        u * (1-v) * f(i+1,j) + u * v * f(i+1, j+1) 
 * 
 */
static inline void get_linear_pt(BYTE *dst, int dst_off, BYTE *src, int sx, int sy, 
        const BITMAP* src_bmp, GAL_PixelFormat *format, BYTE* alpha_mask)
{
    int i = sx>>16;
    int j = sy>>16;
    int u = sx&0xFFFF;
    int v = sy&0xFFFF;

    int pitch = src_bmp->bmPitch;
    int bytesperpixel = src_bmp->bmBytesPerPixel;
    int max_x = src_bmp->bmWidth-1;
    int max_y = src_bmp->bmHeight-1;

    Uint8 p1_r, p1_g, p1_b, p1_a;
    Uint8 p2_r, p2_g, p2_b, p2_a;
    Uint8 p3_r, p3_g, p3_b, p3_a;
    Uint8 p4_r, p4_g, p4_b, p4_a;
    Uint16 nr, ng, nb;
    gal_pixel pt1, pt2, pt3, pt4;

    Int64 tmp1, tmp2, tmp3, tmp4; 
    int pm1, pm2, pm3, pm4;
    gal_pixel result;

    BYTE *src1, *src2, *src3, *src4;
    src1 = src +  i*bytesperpixel;
    src2 = j >= max_y ? src1 : src1 + pitch;
    src3 = i >= max_x ? src1 : src1 + bytesperpixel;
    if (i >= max_x)
        src4 = src2;
    else if (j >= max_y)
        src4 = src3;
    else
        src4 = src1 + bytesperpixel + pitch;

    /*RGB in point(i, j)*/
    pt1 = _mem_get_pixel (src1, bytesperpixel);

    /*RGB in point(i, j+1)*/
    pt2 = _mem_get_pixel (src2, bytesperpixel);

    /*RGB in point(i+1, j)*/
    pt3 = _mem_get_pixel (src3, bytesperpixel);

    /*RGB in point(i+1, j+1)*/
    pt4 = _mem_get_pixel (src4, bytesperpixel);

    if ((src_bmp->bmType & BMP_TYPE_COLORKEY) && 
         (pt1 == src_bmp->bmColorKey)) 
    {
        _mem_set_pixel(dst + dst_off*bytesperpixel, 
                                    bytesperpixel, pt1);
        return;
    }

    if (!format)
    { 
        PDC pdc = dc_HDC2PDC(HDC_SCREEN_SYS);
        format = pdc->surface->format;
    }

    if (src_bmp->bmType & BMP_TYPE_ALPHA ) {
        GAL_GetRGBA(pt1, format, &p1_r, &p1_g, &p1_b, &p1_a);
        GAL_GetRGBA(pt2, format, &p2_r, &p2_g, &p2_b, &p2_a);
        GAL_GetRGBA(pt3, format, &p3_r, &p3_g, &p3_b, &p3_a);
        GAL_GetRGBA(pt4, format, &p4_r, &p4_g, &p4_b, &p4_a);
        if (src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
            get_alpha_with_alphamask (src1, src_bmp, &p1_a);
            get_alpha_with_alphamask (src2, src_bmp, &p2_a);
            get_alpha_with_alphamask (src3, src_bmp, &p3_a);
            get_alpha_with_alphamask (src4, src_bmp, &p4_a);
        }
    }
    else {
        GAL_GetRGB(pt1, format, &p1_r, &p1_g, &p1_b);
        GAL_GetRGB(pt2, format, &p2_r, &p2_g, &p2_b);
        GAL_GetRGB(pt3, format, &p3_r, &p3_g, &p3_b);
        GAL_GetRGB(pt4, format, &p4_r, &p4_g, &p4_b);
    }


    MulTo64((65536-u), (65536-v), &tmp1);
    MulTo64( v, (65536-u), &tmp2);
    MulTo64( u, (65536-v), &tmp3);
    MulTo64( v, u, &tmp4);

    /* (1-u)*(1-v) */
    pm1 = Div64by32( &tmp1, 65536); 
    /* v*(1-u) */ 
    pm2 = Div64by32( &tmp2, 65536);
    /* u*(1-v) */
    pm3 = Div64by32( &tmp3, 65536);
    /* v*u */
    pm4 = Div64by32( &tmp4, 65536);

    nr = ( p1_r*pm1 + p2_r*pm2 + p3_r*pm3 + p4_r*pm4 )>>16;
    ng = ( p1_g*pm1 + p2_g*pm2 + p3_g*pm3 + p4_g*pm4 )>>16;
    nb = ( p1_b*pm1 + p2_b*pm2 + p3_b*pm3 + p4_b*pm4 )>>16;

    if (src_bmp->bmType & BMP_TYPE_ALPHA ) {
        Uint16 na;
        na = ( p1_a*pm1 + p2_a*pm2 + p3_a*pm3 + p4_a*pm4 )>>16;
        if (alpha_mask && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
            result = GAL_MapRGB (format, nr, ng, nb);
            alpha_mask[dst_off] = na;
        }
        else {
            result = GAL_MapRGBA(format, nr, ng, nb, na);
        }
    }
    else
        result = GAL_MapRGB(format, nr, ng, nb);

    _mem_set_pixel(dst + dst_off*bytesperpixel, bytesperpixel, result);

    return;
}

BOOL BitmapBilinerScaler(
    void* context, 
    const BITMAP* src_bmp, 
    int dst_w, int dst_h,
    CB_GET_LINE_BUFF cb_line_buff, 
    CB_LINE_SCALED cb_line_scaled, 
    GAL_PixelFormat *format)
{
    BYTE *dp1 = src_bmp->bmBits;
    BYTE *dp2;
    int xfactor;
    int yfactor;
    int y, sy=0;
    BYTE* dp4 = NULL;

    if (dst_w <= 0 || dst_h <= 0 || src_bmp == NULL)
        return FALSE;

    xfactor = muldiv64 (src_bmp->bmWidth, 65536, dst_w);    /* scaled by 65536 */
    yfactor = muldiv64 (src_bmp->bmHeight, 65536, dst_h);   /* scaled by 65536 */

    for (y = 0; y < dst_h;) {
        int sx = 0;
        int x;

        x = 0;
        dp2 = cb_line_buff (context, y, (void*)&dp4);
        while (x < dst_w) {
            get_linear_pt(dp2, x, dp1, sx, sy, src_bmp, format, dp4);
            sx += xfactor;
            x++;
        }
        cb_line_scaled (context, dp2, y);
        y++;
        sy += yfactor;
        dp1 = src_bmp->bmBits + (sy >> 16) * src_bmp->bmPitch;
    }
    return TRUE;
}

/**
 * \fn BOOL GUIAPI BitmapDDAScalerEx (void* context, \
                const BITMAP* src_bmp, int dst_w, int dst_h, \ 
                CB_GET_LINE_BUFF cb_get_line_buff, \
                CB_LINE_SCALED cb_line_scaled, 
                GAL_PixelFormat *format)
 * \brief A bitmap scaler using DDA algorithm.
 *
 * This function is a general bitmap scaler using DDA algorithm. 
 * This function scales the bitmap from bottom to top.
 *
 * MiniGUI implements ScaleBitmap, FillBoxWithBitmap, FillBoxWithBitmapPart, and 
 * StretchBlt functions by using this scaler.
 *
 * \param context The context will be passed to the callbacks.
 * \param src_bmp The source BITMAP object.
 * \param dst_w The width of the destination BITMAP object.
 * \param dst_h The height of the destination BITMAP object.
 * \param cb_get_line_buff The callback to get the line buffer of 
 *        the destination BITMAP object.
 * \param cb_line_scaled The callback to tell the line is scaled.
 * \param formt reserve.
 *
 * \sa ScaleBitmap, StretchBlt, BitmapDDAScaler2
 */
BOOL BitmapDDAScalerEx (void* context, const BITMAP* src_bmp, 
        int dst_w, int dst_h,
        CB_GET_LINE_BUFF cb_line_buff, CB_LINE_SCALED cb_line_scaled, 
        GAL_PixelFormat *format)
{
    BYTE *dp1 = src_bmp->bmBits;
    BYTE *dp2;
    int xfactor;
    int yfactor;

    if (dst_w <= 0 || dst_h <= 0)
        return FALSE;

    xfactor = muldiv64 (src_bmp->bmWidth, 65536, dst_w);    /* scaled by 65536 */
    yfactor = muldiv64 (src_bmp->bmHeight, 65536, dst_h);   /* scaled by 65536 */

    switch (src_bmp->bmBytesPerPixel) {
    case 1:
        {
            int y, sy;
            BYTE* dp3 = src_bmp->bmAlphaMask;
            BYTE* dp4 = NULL;
            sy = 0;
            for (y = 0; y < dst_h;) {
                int sx = 0;
                int x;

                x = 0;
                dp2 = cb_line_buff (context, y, (void*)&dp4);
                while (x < dst_w) {
                    *(dp2 + x) = *(dp1 + (sx >> 16));
                    if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        dp4[x] = dp3[(sx >> 16)];
                    }
                    sx += xfactor;
                    x++;
                }
                cb_line_scaled (context, dp2, y);
                y++;
                while (y < dst_h) {
                    int syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    cb_line_scaled (context, dp2, y);
                    y++;
                }
                dp1 = src_bmp->bmBits + (sy >> 16) * src_bmp->bmPitch;
                if (src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                    dp3 = src_bmp->bmAlphaMask + (sy >> 16) * src_bmp->bmAlphaPitch;
                }
            }
        }
    break;
    case 2:
        {
            int y, sy;
            BYTE* dp3 = src_bmp->bmAlphaMask;
            BYTE* dp4 = NULL;
            sy = 0;
            for (y = 0; y < dst_h;) {
                int sx = 0;
                int x;

                x = 0;
                dp2 = cb_line_buff (context, y, (void*)&dp4);
                /* This can be greatly optimized with loop */
                /* unrolling; omitted to save space. */
                while (x < dst_w) {
                    *(unsigned short *) (dp2 + x * 2) =
                        *(unsigned short *) (dp1 + (sx >> 16) * 2);
                    if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        dp4[x] = dp3[(sx >> 16)];
                    }
                    sx += xfactor;
                    x++;
                }
                cb_line_scaled (context, dp2, y);
                y++;
                while (y < dst_h) {
                    int syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    cb_line_scaled (context, dp2, y);
                    y++;
                }
                dp1 = src_bmp->bmBits + (sy >> 16) * src_bmp->bmPitch;
                if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                    dp3 = src_bmp->bmAlphaMask + (sy >> 16) * src_bmp->bmAlphaPitch;
                }
            }
        }
    break;
    case 3:
        {
            int y, sy;
            BYTE* dp3 = src_bmp->bmAlphaMask;
            BYTE* dp4 = NULL;
            sy = 0;
            for (y = 0; y < dst_h;) {
                int sx = 0;
                int x;

                x = 0;
                dp2 = cb_line_buff (context, y, (void*)&dp4);
                /* This can be greatly optimized with loop */
                /* unrolling; omitted to save space. */
                while (x < dst_w) {
#if 0
                    *(unsigned short *) (dp2 + x * 3) =
                        *(unsigned short *) (dp1 + (sx >> 16) * 3);
                    *(unsigned char *) (dp2 + x * 3 + 2) =
                        *(unsigned char *) (dp1 + (sx >> 16) * 3 + 2);
#else
                    _mem_set_pixel (dp2 + x * 3, 3, 
                            _mem_get_pixel (dp1 + (sx >> 16) * 3, 3));
#endif
                    if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        dp4[x] = dp3[(sx >> 16)];
                    }
                    sx += xfactor;
                    x++;
                }
                cb_line_scaled (context, dp2, y);
                y++;
                while (y < dst_h) {
                    int syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    cb_line_scaled (context, dp2, y);
                    y++;
                }
                dp1 = src_bmp->bmBits + (sy >> 16) * src_bmp->bmPitch;
                if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                    dp3 = src_bmp->bmAlphaMask + (sy >> 16) * src_bmp->bmAlphaPitch;
                }
            }
        }
    break;
    case 4:
        {
            int y, sy;
            BYTE* dp3 = src_bmp->bmAlphaMask;
            BYTE* dp4 = NULL;
            sy = 0;
            for (y = 0; y < dst_h;) {
                int sx = 0;
                int x;

                x = 0;
                dp2 = cb_line_buff (context, y, (void*)&dp4);
                /* This can be greatly optimized with loop */
                /* unrolling; omitted to save space. */
                while (x < dst_w) {
                    *(unsigned *) (dp2 + x * 4) =
                        *(unsigned *) (dp1 + (sx >> 16) * 4);
                    if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        dp4[x] = dp3[(sx >> 16)];
                    }
                    sx += xfactor;
                    x++;
                }
                cb_line_scaled (context, dp2, y);
                y++;
                while (y < dst_h) {
                    int syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    cb_line_scaled (context, dp2, y);
                    y++;
                }
                dp1 = src_bmp->bmBits + (sy >> 16) * src_bmp->bmPitch;
                if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                    dp3 = src_bmp->bmAlphaMask + (sy >> 16) * src_bmp->bmAlphaPitch;
                }
            }
        }
    break;
    }

    return TRUE;
    
}

/**
 * \fn BOOL BitmapDDAScaler2 (void* context, \
                const BITMAP* src_bmp, int dst_w, int dst_h, \
                CB_GET_LINE_BUFF cb_get_line_buff, \
                CB_LINE_SCALED cb_line_scaled)
 * \brief A bitmap scaler using DDA algorithm.
 *
 * This function is a general bitmap scaler using DDA algorithm. 
 * This function scales the bitmap from bottom to top.
 *
 * MiniGUI implements StretchBlt functions by using this scaler.
 *
 * \param context The context will be passed to the callbacks.
 * \param src_bmp The source BITMAP object.
 * \param dst_w The width of the destination BITMAP object.
 * \param dst_h The height of the destination BITMAP object.
 * \param cb_get_line_buff The callback to get the line buffer of 
 *        the destination BITMAP object.
 * \param cb_line_scaled The callback to tell the line is scaled.
 *
 * \sa ScaleBitmap, StretchBlt, BitmapDDAScaler
 */
BOOL BitmapDDAScaler2 (void* context, const BITMAP* src_bmp, int dst_w, int dst_h,
            CB_GET_LINE_BUFF cb_line_buff, CB_LINE_SCALED cb_line_scaled)
{
    BYTE *src_bits = src_bmp->bmBits + src_bmp->bmPitch * (src_bmp->bmHeight - 1);
    BYTE *dp1 = src_bits;
    BYTE *dp2;
    int xfactor;
    int yfactor;

    if (dst_w == 0 || dst_h == 0)
        return TRUE;

    xfactor = muldiv64 (src_bmp->bmWidth, 65536, dst_w);    /* scaled by 65536 */
    yfactor = muldiv64 (src_bmp->bmHeight, 65536, dst_h);   /* scaled by 65536 */

    switch (src_bmp->bmBytesPerPixel) {
    case 1:
        {
            int y, sy;
            BYTE* dp3 = src_bmp->bmAlphaMask;
            BYTE* dp4 = NULL;
            sy = 0;
            for (y = dst_h - 1; y >= 0;) {
                int sx = 0;
                int x;

                x = 0;
                dp2 = cb_line_buff (context, y, (void*)&dp4);
                while (x < dst_w) {
                    *(dp2 + x) = *(dp1 + (sx >> 16));
                    if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        dp4[x] = dp3[(sx >> 16)];
                    }
                    sx += xfactor;
                    x++;
                }
                cb_line_scaled (context, dp2, y);
                y--;
                while (y >= 0) {
                    int syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    cb_line_scaled (context, dp2, y);
                    y--;
                }
                dp1 = src_bits - (sy >> 16) * src_bmp->bmPitch;
                if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                    dp3 = src_bmp->bmAlphaMask + (sy >> 16) * src_bmp->bmAlphaPitch;
                }
            }
        }
    break;
    case 2:
        {
            int y, sy;
            BYTE* dp3 = src_bmp->bmAlphaMask;
            BYTE* dp4 = NULL;
            sy = 0;
            for (y = dst_h - 1; y >= 0;) {
                int sx = 0;
                int x;

                x = 0;
                dp2 = cb_line_buff (context, y, (void*)&dp4);
                /* This can be greatly optimized with loop */
                /* unrolling; omitted to save space. */
                while (x < dst_w) {
                    *(unsigned short *) (dp2 + x * 2) =
                        *(unsigned short *) (dp1 + (sx >> 16) * 2);
                    if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        dp4[x] = dp3[(sx >> 16)];
                    }
                    sx += xfactor;
                    x++;
                }
                cb_line_scaled (context, dp2, y);
                y--;
                while (y >= 0) {
                    int syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    cb_line_scaled (context, dp2, y);
                    y--;
                }
                dp1 = src_bits - (sy >> 16) * src_bmp->bmPitch;
                if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                    dp3 = src_bmp->bmAlphaMask + (sy >> 16) * src_bmp->bmAlphaPitch;
                }
            }
        }
    break;
    case 3:
        {
            int y, sy;
            BYTE* dp3 = src_bmp->bmAlphaMask;
            BYTE* dp4 = NULL;
            sy = 0;
            for (y = dst_h - 1; y >= 0;) {
                int sx = 0;
                int x;

                x = 0;
                dp2 = cb_line_buff (context, y, (void*)&dp4);
                /* This can be greatly optimized with loop */
                /* unrolling; omitted to save space. */
                while (x < dst_w) {
#if 0
                    *(unsigned short *) (dp2 + x * 3) =
                        *(unsigned short *) (dp1 + (sx >> 16) * 3);
                    *(unsigned char *) (dp2 + x * 3 + 2) =
                        *(unsigned char *) (dp1 + (sx >> 16) * 3 + 2);
#else
                    _mem_set_pixel (dp2 + x * 3, 3, 
                            _mem_get_pixel (dp1 + (sx >> 16) * 3, 3));
#endif
                    if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        dp4[x] = dp3[(sx >> 16)];
                    }
                    sx += xfactor;
                    x++;
                }
                cb_line_scaled (context, dp2, y);
                y--;
                while (y >= 0) {
                    int syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    cb_line_scaled (context, dp2, y);
                    y--;
                }
                dp1 = src_bits - (sy >> 16) * src_bmp->bmPitch;
                if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                    dp3 = src_bmp->bmAlphaMask + (sy >> 16) * src_bmp->bmAlphaPitch;
                }
            }
        }
    break;
    case 4:
        {
            int y, sy;
            BYTE* dp3 = src_bmp->bmAlphaMask;
            BYTE* dp4 = NULL;
            sy = 0;
            for (y = dst_h - 1; y >= 0;) {
                int sx = 0;
                int x;

                x = 0;
                dp2 = cb_line_buff (context, y, (void*)&dp4);
                /* This can be greatly optimized with loop */
                /* unrolling; omitted to save space. */
                while (x < dst_w) {
                    *(unsigned *) (dp2 + x * 4) =
                        *(unsigned *) (dp1 + (sx >> 16) * 4);
                    if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        dp4[x] = dp3[(sx >> 16)];
                    }
                    sx += xfactor;
                    x++;
                }
                cb_line_scaled (context, dp2, y);
                y--;
                while (y >= 0) {
                    int syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    cb_line_scaled (context, dp2, y);
                    y--;
                }
                dp1 = src_bits - (sy >> 16) * src_bmp->bmPitch;
                if (dp4 && src_bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                    dp3 = src_bmp->bmAlphaMask + (sy >> 16) * src_bmp->bmAlphaPitch;
                }
            }
        }
    break;
    }

    return TRUE;
    
}

struct _SCALER_INFO
{
    BITMAP* dst;
    int last_y;
};

static void* _get_line_buff_scalebitmap (void* context, int y, void** alpha_line_mask)
{
    BYTE* line;
    struct _SCALER_INFO* info = (struct _SCALER_INFO*) context;

    line = info->dst->bmBits;
    line += info->dst->bmPitch * y;
    info->last_y = y;

    if ((info->dst->bmType & BMP_TYPE_ALPHA_MASK)
            && info->dst->bmAlphaMask) {
        int alpha_pitch = info->dst->bmAlphaPitch;
        *(BYTE**)alpha_line_mask = info->dst->bmAlphaMask + alpha_pitch * y;
    }
    return line;
}

static void _line_scaled_scalebitmap (void* context, const void* line, int y)
{
    BYTE* dst_line;
    struct _SCALER_INFO* info = (struct _SCALER_INFO*) context;

    if (y != info->last_y) {
        dst_line = info->dst->bmBits + info->dst->bmPitch * y;
        memcpy (dst_line, line, info->dst->bmPitch);
    }
}

/*
BOOL ScaleBitmap (BITMAP *dst, const BITMAP *src)
{
    struct _SCALER_INFO info;

    info.dst = dst;
    info.last_y = 0;

    if (dst->bmWidth == 0 || dst->bmHeight == 0)
        return TRUE;

    if (dst->bmBytesPerPixel != src->bmBytesPerPixel)
        return FALSE;

    BitmapDDAScaler (&info, src, dst->bmWidth, dst->bmHeight,
            _get_line_buff_scalebitmap, _line_scaled_scalebitmap);

    return TRUE;
}
*/

BOOL ScaleBitmapEx(BITMAP *dst, const BITMAP *src, HDC ref_dc)
{
    struct _SCALER_INFO info;
        
    info.dst = dst;
    info.last_y = 0;

    if (dst->bmWidth == 0 || dst->bmHeight == 0)
        return TRUE;

    if (dst->bmBytesPerPixel != src->bmBytesPerPixel)
        return FALSE;

    if (ref_dc == HDC_SCREEN || ref_dc == HDC_SCREEN_SYS) {
        BitmapDDAScaler (&info, src, dst->bmWidth, dst->bmHeight,
                _get_line_buff_scalebitmap, _line_scaled_scalebitmap);
    }
    else
    {
        PDC pdc = dc_HDC2PDC(ref_dc);
        pdc->bitmap_scaler(&info, src, dst->bmWidth, dst->bmHeight,
                _get_line_buff_scalebitmap, _line_scaled_scalebitmap, 
                pdc->surface->format);
    }

    return TRUE;
}

int GUIAPI SetBitmapScalerType (HDC hdc,  int scaler_type)
{
    PDC pdc = dc_HDC2PDC(hdc);
    if ( scaler_type == BITMAP_SCALER_BILINEAR )
        pdc->bitmap_scaler = BitmapBilinerScaler;
    else 
        pdc->bitmap_scaler = BitmapDDAScalerEx;

    return TRUE;
}

gal_pixel GUIAPI GetPixelInBitmapEx (const BITMAP* bmp, int x, int y, Uint8* alpha)
{
    BYTE* dst;

    if (x < 0 || y < 0 || x >= bmp->bmWidth || y >= bmp->bmHeight)
        return 0;

    if (alpha && bmp->bmAlphaMask) {
        int alpha_pitch = bmp->bmAlphaPitch;
        *alpha = bmp->bmAlphaMask[y*alpha_pitch + x];
    }

    dst = bmp->bmBits + y * bmp->bmPitch + x * bmp->bmBytesPerPixel;
    return _mem_get_pixel (dst, bmp->bmBytesPerPixel);
}

BOOL GUIAPI SetPixelInBitmapEx (const BITMAP* bmp, int x, int y, 
        gal_pixel pixel, const Uint8* alpha)
{
    BYTE* dst;

    if (x < 0 || y < 0 || x >= bmp->bmWidth || y >= bmp->bmHeight)
        return FALSE;

    if (alpha && bmp->bmAlphaMask) {
        int alpha_pitch = bmp->bmAlphaPitch;
        bmp->bmAlphaMask[y*alpha_pitch + x] = *alpha;
    }

    dst = bmp->bmBits + y * bmp->bmPitch + x * bmp->bmBytesPerPixel;
    _mem_set_pixel (dst, bmp->bmBytesPerPixel, pixel);
    return TRUE;
}

#ifdef _FILL_MYBITMAP

/* This function expand monochorate bitmap. */
void GUIAPI ExpandPartMonoBitmap (HDC hdc, BYTE* bits, Uint32 pitch, 
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, Uint32 bg, Uint32 fg, 
                int stepx, CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp)
{
    Uint32 x, y;
    BYTE *dst, *dst_line;
    const BYTE *src, *src_line;
    Uint32 pixel;
    BYTE byte = 0;
    int  bnum,continum,i;

    dst_line = bits;
    if (flags & MYBMP_FLOW_UP)
        src_line = my_bits + my_pitch * (h - 1);
    else
        src_line = my_bits;

    for (y = 0; y < h; y++) {
        src = src_line;
        dst = dst_line;
        bnum =stepx/8;
        continum =stepx %8;
        src =src +bnum;

        i=0;
        for (x = 0; x < w +continum; x++) {
            if (x % 8 == 0)
                byte = *src++;
            if(i<continum){
                i++;
                continue;
            }

            if ((byte & (128 >> (x % 8))))   /* pixel */
                pixel = fg;
            else
                pixel = bg;

            if(cb_draw)
                cb_draw(hdc,mybmp,pixel,dst);
        }
        
        if (flags & MYBMP_FLOW_UP)
            src_line -= my_pitch;
        else
            src_line += my_pitch;

        dst_line += pitch;
    }
}

#endif

/* This function expand monochorate bitmap. */
void GUIAPI ExpandMonoBitmap (HDC hdc, BYTE* bits, Uint32 pitch, 
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, Uint32 bg, Uint32 fg)
{
    Uint32 x, y;
    BYTE *dst, *dst_line;
    const BYTE *src, *src_line;
    Uint32 pixel;
    int bpp = GAL_BytesPerPixel (dc_HDC2PDC (hdc)->surface);
    BYTE byte = 0;

    dst_line = bits;
    if (flags & MYBMP_FLOW_UP)
        src_line = my_bits + my_pitch * (h - 1);
    else
        src_line = my_bits;

    for (y = 0; y < h; y++) {
        src = src_line;
        dst = dst_line;

        for (x = 0; x < w; x++) {
            if (x % 8 == 0)
                byte = *src++;

            if ((byte & (128 >> (x % 8))))   /* pixel */
                pixel = fg;
            else
                pixel = bg;
            dst = _mem_set_pixel (dst, bpp, pixel);
        }
        
        if (flags & MYBMP_FLOW_UP)
            src_line -= my_pitch;
        else
            src_line += my_pitch;

        dst_line += pitch;
    }
}

static const RGB WindowsStdColor [] = {
    {0x00, 0x00, 0x00},     // black         --0
    {0x80, 0x00, 0x00},     // dark red      --1
    {0x00, 0x80, 0x00},     // dark green    --2
    {0x80, 0x80, 0x00},     // dark yellow   --3
    {0x00, 0x00, 0x80},     // dark blue     --4
    {0x80, 0x00, 0x80},     // dark magenta  --5
    {0x00, 0x80, 0x80},     // dark cyan     --6
    {0xC0, 0xC0, 0xC0},     // light gray    --7
    {0x80, 0x80, 0x80},     // dark gray     --8
    {0xFF, 0x00, 0x00},     // red           --9
    {0x00, 0xFF, 0x00},     // green         --10
    {0xFF, 0xFF, 0x00},     // yellow        --11
    {0x00, 0x00, 0xFF},     // blue          --12
    {0xFF, 0x00, 0xFF},     // magenta       --13
    {0x00, 0xFF, 0xFF},     // cyan          --14
    {0xFF, 0xFF, 0xFF},     // light white   --15
};

#ifdef _FILL_MYBITMAP
/* This function expand 16-color bitmap. */
void GUIAPI ExpandPart16CBitmapEx (HDC hdc, BYTE* bits, Uint32 pitch, const BYTE* my_bits, 
        Uint32 my_pitch, Uint32 w, Uint32 h, DWORD flags, 
        const RGB* pal, BYTE use_pal_alpha, BYTE alpha,
        int stepx, CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp)
{
    PDC pdc;
    Uint32 x, y;
    BYTE *dst, *dst_line;
    const BYTE *src, *src_line;
    int index;
    //int bpp;
    Uint32 pixel;
    BYTE byte = 0;
    int  bnum,continum,i;

    pdc = dc_HDC2PDC(hdc);
    // bpp = GAL_BytesPerPixel (pdc->surface);

    dst_line = bits;
    if (flags & MYBMP_FLOW_UP)
        src_line = my_bits + my_pitch * (h - 1);
    else
        src_line = my_bits;

    for (y = 0; y < h; y++) {
        src = src_line;
        dst = dst_line;
        bnum =stepx/2;
        continum =stepx %2;
        src =src +bnum;

        i =0;
        for (x = 0; x < w +continum; x++) {
            if (x % 2 == 0) {
                byte = *src++;
                index = (byte >> 4) & 0x0f;
            }
            else
                index = byte & 0x0f;
            if(i<continum){
                i++;
                continue;
            }

            if (pal)
                pixel = GAL_MapRGBA (pdc->surface->format, 
                                pal[index].r, pal[index].g, pal[index].b, 
                                use_pal_alpha?pal[index].a:0xFF);
            else
                pixel = GAL_MapRGBA (pdc->surface->format, 
                                WindowsStdColor[index].r, 
                                WindowsStdColor[index].g, 
                                WindowsStdColor[index].b,
                                alpha);
            if(cb_draw)
                cb_draw(hdc,mybmp,pixel,dst);

            //dst = _mem_set_pixel (dst, bpp, pixel);
        }

        if (flags & MYBMP_FLOW_UP)
            src_line -= my_pitch;
        else
            src_line += my_pitch;

        dst_line += pitch;
    }
}

#endif

/* This function expand 16-color bitmap. */
void GUIAPI Expand16CBitmapEx (HDC hdc, BYTE* bits, Uint32 pitch, 
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, 
                const RGB* pal, BYTE use_pal_alpha, BYTE alpha)
{
    PDC pdc;
    Uint32 x, y;
    BYTE *dst, *dst_line;
    const BYTE *src, *src_line;
    int index, bpp;
    Uint32 pixel;
    BYTE byte = 0;

    pdc = dc_HDC2PDC(hdc);
    bpp = GAL_BytesPerPixel (pdc->surface);

    dst_line = bits;
    if (flags & MYBMP_FLOW_UP)
        src_line = my_bits + my_pitch * (h - 1);
    else
        src_line = my_bits;

    for (y = 0; y < h; y++) {
        src = src_line;
        dst = dst_line;
        for (x = 0; x < w; x++) {
            if (x % 2 == 0) {
                byte = *src++;
                index = (byte >> 4) & 0x0f;
            }
            else
                index = byte & 0x0f;

            if (pal)
                pixel = GAL_MapRGBA (pdc->surface->format, 
                                pal[index].r, pal[index].g, pal[index].b,
                                use_pal_alpha?pal[index].a:0xFF);
            else
                pixel = GAL_MapRGBA (pdc->surface->format, 
                                WindowsStdColor[index].r, 
                                WindowsStdColor[index].g, 
                                WindowsStdColor[index].b,
                                alpha);

            dst = _mem_set_pixel (dst, bpp, pixel);
        }

        if (flags & MYBMP_FLOW_UP)
            src_line -= my_pitch;
        else
            src_line += my_pitch;

        dst_line += pitch;
    }
}

/* This function expands 256-color bitmap. */
void GUIAPI Expand256CBitmapEx (HDC hdc, BYTE* bits, Uint32 pitch, 
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, 
                const RGB* pal, BYTE use_pal_alpha, BYTE alpha,
                CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp)
{
    PDC pdc;
    Uint32 x, y;
    int bpp;
    BYTE *dst, *dst_line;
    const BYTE *src, *src_line;
    Uint32 pixel;
    BYTE byte;

    pdc = dc_HDC2PDC(hdc);
    bpp = GAL_BytesPerPixel (pdc->surface);

    dst_line = bits;
    if (flags & MYBMP_FLOW_UP)
        src_line = my_bits + my_pitch * (h - 1);
    else
        src_line = my_bits;

    for (y = 0; y < h; y++) {
        src = src_line;
        dst = dst_line;
        for (x = 0; x < w; x++) {
            byte = *src++;

            if (pal)
                pixel = GAL_MapRGBA (pdc->surface->format, 
                                pal[byte].r, pal[byte].g, pal[byte].b,
                                use_pal_alpha?pal[byte].a:0xFF);
            else if (bpp == 1)
                /* 
                 * Assume that the palette of the bitmap is the same as 
                 * the palette of the DC.
                 */
                pixel = byte;
            else
                /* 
                 * Treat the bitmap uses the dithered colorful palette.
                 */
                pixel = GAL_MapRGBA (pdc->surface->format, 
                                    (byte >> 5) & 0x07,
                                    (byte >> 2) & 0x07,
                                    byte & 0x03,
                                    alpha);

        if (cb_draw)
            cb_draw (hdc, mybmp, pixel, dst);
        else
            dst = _mem_set_pixel (dst, bpp, pixel);
        }

        if (flags & MYBMP_FLOW_UP)
            src_line -= my_pitch;
        else
            src_line += my_pitch;

        dst_line += pitch;
    }
}

/* This function compile a RGBA bitmap. */
void GUIAPI CompileRGBABitmap (HDC hdc, BYTE* bits, Uint32 pitch, 
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, void* pixel_format,
                CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp)
{
    PDC pdc;
    Uint32 x, y;
    int bpp;
    BYTE *dst, *dst_line;
    const BYTE *src, *src_line;
    Uint32 pixel;
    GAL_Color rgb;

    pdc = dc_HDC2PDC(hdc);
    bpp = GAL_BytesPerPixel (pdc->surface);

    dst_line = bits;
    if (flags & MYBMP_FLOW_UP)
        src_line = my_bits + my_pitch * (h - 1);
    else
        src_line = my_bits;

    /* expand bits here. */
    for (y = 0; y < h; y++) {
        src = src_line;
        dst = dst_line;
        for (x = 0; x < w; x++) {
            if ((flags & MYBMP_TYPE_MASK) == MYBMP_TYPE_BGR) {
                rgb.b = *src++;
                rgb.g = *src++;
                rgb.r = *src++;
            }
            else {
                rgb.r = *src++;
                rgb.g = *src++;
                rgb.b = *src++;
            }

            if (flags & MYBMP_RGBSIZE_4) {
                if (flags & MYBMP_ALPHA) {
                    rgb.a = *src;
                    pixel = GAL_MapRGBA (pdc->surface->format, 
                                    rgb.r, rgb.g, rgb.b, rgb.a);
                }
                else {
                    pixel = GAL_MapRGB (pdc->surface->format, 
                                    rgb.r, rgb.g, rgb.b);
                }
                src++;
            }
            else {
                pixel = GAL_MapRGB (pdc->surface->format, rgb.r, rgb.g, rgb.b);
            }

        if(cb_draw)
            cb_draw(hdc,mybmp,pixel,dst);
        else
            dst = _mem_set_pixel (dst, bpp, pixel);
        }

        if (flags & MYBMP_FLOW_UP)
            src_line -= my_pitch;
        else
            src_line += my_pitch;

        dst_line += pitch;
    }
}

void GUIAPI CompileRGBABitmapEx (HDC hdc, BYTE* bits, Uint32 pitch, 
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, void* pixel_format,
                CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp,
                BYTE* alpha_mask)
{
    PDC pdc;
    Uint32 x, y;
    int bpp;
    BYTE *dst, *dst_line;
    const BYTE *src, *src_line;
    Uint32 pixel;
    GAL_Color rgb;
    Uint32 alpha_index = 0;
    Uint32 alpha_pitch = (w +3) & (~3);

    pdc = dc_HDC2PDC(hdc);
    bpp = GAL_BytesPerPixel (pdc->surface);

    dst_line = bits;
    if (flags & MYBMP_FLOW_UP)
        src_line = my_bits + my_pitch * (h - 1);
    else
        src_line = my_bits;

    /* expand bits here. */
    for (y = 0; y < h; y++) {
        src = src_line;
        dst = dst_line;
        for (x = 0; x < w; x++) {
            if ((flags & MYBMP_TYPE_MASK) == MYBMP_TYPE_BGR) {
                rgb.b = *src++;
                rgb.g = *src++;
                rgb.r = *src++;
            }
            else {
                rgb.r = *src++;
                rgb.g = *src++;
                rgb.b = *src++;
            }

            if (flags & MYBMP_RGBSIZE_4) {
                if (flags & MYBMP_ALPHA) {
                    rgb.a = *src;
                    if (alpha_mask) {
                        alpha_mask[alpha_index + x] = rgb.a;
                        pixel = GAL_MapRGB(pdc->surface->format,
                                rgb.r, rgb.g, rgb.b);
                    } else {
                        pixel = GAL_MapRGBA (pdc->surface->format, 
                                rgb.r, rgb.g, rgb.b, rgb.a);
                    }
                }
                else {
                    pixel = GAL_MapRGB (pdc->surface->format, 
                                    rgb.r, rgb.g, rgb.b);
                }
                src++;
            }
            else {
                pixel = GAL_MapRGB (pdc->surface->format, rgb.r, rgb.g, rgb.b);
            }

        if(cb_draw)
            cb_draw(hdc,mybmp,pixel,dst);
        else
            dst = _mem_set_pixel (dst, bpp, pixel);
        }

        if (flags & MYBMP_FLOW_UP)
            src_line -= my_pitch;
        else
            src_line += my_pitch;

        dst_line += pitch;

        if (alpha_mask) {
            alpha_index += alpha_pitch;
        }
    }
}

/* This function replaces one color with specified color. */
void GUIAPI ReplaceBitmapColor (HDC hdc, BITMAP* bmp, 
                gal_pixel iOColor, gal_pixel iNColor)
{
    PDC pdc;
    int w, h, i;
    BYTE* line, *bits;
    int bpp;

    pdc = dc_HDC2PDC (hdc);
    bpp = GAL_BytesPerPixel (pdc->surface);

    h = bmp->bmHeight;
    w = bmp->bmWidth;
    line = bmp->bmBits;
    switch (bpp) {
        case 1:
            while (h--) {
                bits = line;
                for (i = 0; i < w; i++) {
                    if (*bits == iOColor)
                        *bits = iNColor;
                    bits++;
                }
                line += bmp->bmPitch;
            }
            break;
        case 2:
            while (h--) {
                bits = line;
                for (i = 0; i < w; i++) {
                    if( *(Uint16 *) bits == iOColor)
                        *(Uint16 *) bits = iNColor;
                    bits += 2;
                }
                line += bmp->bmPitch;
            }
            break;
        case 3: 
            while (h--) {
                bits = line;
                for (i = 0; i < w; i++) {
#if 0
                    if ((*(Uint16 *) bits == iOColor) 
                           && (*(bits + 2) == (iOColor >> 16)) )
                    {
                        *(Uint16 *) bits = iNColor;
                        *(bits + 2) = iNColor >> 16;
                    }
#else
                    if (_mem_get_pixel (bits, 3) == iOColor)
                        _mem_set_pixel (bits, 3, iNColor);
#endif
                    bits += 3;
                }
                line += bmp->bmPitch;
            }
            break;
        case 4:    
            while (h--) {
                bits = line;
                for (i = 0; i < w; i++) {
                    if( *(Uint32 *) bits == iOColor )
                        *(Uint32 *) bits = iNColor;
                    bits += 4;
                }
                line += bmp->bmPitch;
            }
            break;
    }
}

void GUIAPI HFlipBitmap (BITMAP* bmp, unsigned char* inter_buff)
{
    int bpp;
    int x, y;
    unsigned char* sline, *dline, *sp;
    gal_pixel pixel;

    if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
        int alpha_pitch = bmp->bmAlphaPitch;
        unsigned char* alpha_inter_buff = (unsigned char*)malloc(sizeof(char) * alpha_pitch);

        sline = bmp->bmAlphaMask;

        memset(alpha_inter_buff, 0, alpha_pitch);
        for (y = 0; y < bmp->bmHeight; y++) {

            sp = sline + bmp->bmWidth;
            dline = alpha_inter_buff;

            for (x = 0; x < bmp->bmWidth; x++) {
                *dline ++ = *sp--;
            }

            memcpy (sline, alpha_inter_buff, alpha_pitch);
            sline += alpha_pitch;
        }
        free(alpha_inter_buff);
    }

    bpp = bmp->bmBytesPerPixel;
    sline = bmp->bmBits;
    for (y = 0; y < bmp->bmHeight; y++) {

        sp = sline + bpp * bmp->bmWidth;
        dline = inter_buff;

        for (x = 0; x < bmp->bmWidth; x++) {
            sp -= bpp;
            pixel = _mem_get_pixel (sp, bpp);
            dline = _mem_set_pixel (dline, bpp, pixel);
        }

        memcpy (sline, inter_buff, bmp->bmPitch);

        sline += bmp->bmPitch;
    }
}

void GUIAPI VFlipBitmap (BITMAP* bmp, unsigned char* inter_buff)
{
    int y;
    unsigned char* sline, *dline;
    
    if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
        int alpha_pitch = bmp->bmAlphaPitch;
        unsigned char* alpha_inter_buff = (unsigned char*)malloc(sizeof(char) * alpha_pitch);

        dline = bmp->bmAlphaMask + alpha_pitch * bmp->bmHeight;
        sline = bmp->bmAlphaMask;

        for (y = 0; y < (bmp->bmHeight >> 1); y++) {
            dline -= alpha_pitch;

            memcpy (alpha_inter_buff, sline, alpha_pitch);
            memcpy (sline, dline, alpha_pitch);
            memcpy (dline, alpha_inter_buff, alpha_pitch);

            sline += alpha_pitch;
        }
        free(alpha_inter_buff);
    }

    sline = bmp->bmBits;
    dline = bmp->bmBits + bmp->bmPitch * bmp->bmHeight;

    for (y = 0; y < (bmp->bmHeight >> 1); y++) {

        dline -= bmp->bmPitch;

        memcpy (inter_buff, sline, bmp->bmPitch);
        memcpy (sline, dline, bmp->bmPitch);
        memcpy (dline, inter_buff, bmp->bmPitch);


        sline += bmp->bmPitch;
    }
}

