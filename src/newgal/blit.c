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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "newgal.h"
#include "sysvideo.h"
#include "blit.h"
#include "RLEaccel_c.h"
#include "pixels_c.h"
#include "memops.h"

/* The general purpose software blit routine */
static int GAL_SoftBlit(GAL_Surface *src, GAL_Rect *srcrect,
            GAL_Surface *dst, GAL_Rect *dstrect)
{
    int okay;

    /* Everything is okay at the beginning...  */
    okay = 1;

    /* Unencode the destination if it's RLE encoded */
    if ( dst->flags & GAL_RLEACCEL ) {
        GAL_UnRLESurface(dst, 1);
        dst->flags |= GAL_RLEACCEL;    /* save accel'd state */
    }

    /* Set up source and destination buffer pointers, and BLIT! */
    if ( okay  && srcrect->w && srcrect->h ) {
        GAL_BlitInfo info;
        GAL_loblit RunBlit;

        /* Set up the blit information */
        info.s_pixels = (Uint8 *)src->pixels +
                (Uint16)srcrect->y*src->pitch +
                (Uint16)srcrect->x*src->format->BytesPerPixel;
        info.s_width = srcrect->w;
        info.s_height = srcrect->h;
        info.s_skip=src->pitch-info.s_width*src->format->BytesPerPixel;
        info.d_pixels = (Uint8 *)dst->pixels +
                (Uint16)dstrect->y*dst->pitch +
                (Uint16)dstrect->x*dst->format->BytesPerPixel;
        info.d_width = dstrect->w;
        info.d_height = dstrect->h;
        info.d_skip=dst->pitch-info.d_width*dst->format->BytesPerPixel;
        info.aux_data = src->map->sw_data->aux_data;
        info.src = src->format;
        info.table = src->map->table;
        info.dst = dst->format;
        RunBlit = src->map->sw_data->blit;

        /* Run the actual software blit */
        RunBlit(&info);
    }

    /* Re-encode the destination if it's RLE encoded */
    if ( dst->flags & GAL_RLEACCEL ) {
            dst->flags &= ~GAL_RLEACCEL; /* stop lying */
        GAL_RLESurface(dst);
    }

    /* Blit is done! */
    return(okay ? 0 : -1);
}

static void GAL_BlitCopy(GAL_BlitInfo *info)
{
    Uint8 *src, *dst;
    int w, h;
    int srcskip, dstskip;

    w = info->d_width*info->dst->BytesPerPixel;
    h = info->d_height;
    src = info->s_pixels;
    dst = info->d_pixels;
    srcskip = w+info->s_skip;
    dstskip = w+info->d_skip;
    while ( h-- ) {
        if (((DWORD)dst & 0x03) == 0
                && ((DWORD)src & 0x03) == 0
                && (w & 0x03) == 0)
            GAL_memcpy4 (dst, src, w >> 2);
        else
            GAL_memcpy(dst, src, w);
        src += srcskip;
        dst += dstskip;
    }
}

static void GAL_BlitCopyOverlap(GAL_BlitInfo *info)
{
    Uint8 *src, *dst;
    int w, h;
    int srcskip, dstskip;

    w = info->d_width*info->dst->BytesPerPixel;
    h = info->d_height;
    src = info->s_pixels;
    dst = info->d_pixels;
    srcskip = w+info->s_skip;
    dstskip = w+info->d_skip;
    if ( dst < src ) {
        while ( h-- ) {
            GAL_memcpy(dst, src, w);
            src += srcskip;
            dst += dstskip;
        }
    } else {
        src += ((h-1) * srcskip);
        dst += ((h-1) * dstskip);
        while ( h-- ) {
            GAL_revcpy(dst, src, w);
            src -= srcskip;
            dst -= dstskip;
        }
    }
}

#ifdef _MGUSE_PIXMAN
BOOL GAL_CheckPixmanFormat (struct GAL_Surface *src, struct GAL_Surface *dst);
static int GAL_PixmanBlit (struct GAL_Surface *src, GAL_Rect *srcrect,
        struct GAL_Surface *dst, GAL_Rect *dstrect);
#endif

/* Figure out which of many blit routines to set up on a surface */
int GAL_CalculateBlit(GAL_Surface *surface)
{
    int blit_index;
    GAL_VideoDevice *src_video, *dst_video, *cur_video = NULL;

    src_video = (GAL_VideoDevice *) surface->video;
    dst_video = (GAL_VideoDevice *) surface->map->dst->video;

    /* Clean everything out to start */
    if ((surface->flags & GAL_RLEACCEL) == GAL_RLEACCEL) {
        GAL_UnRLESurface(surface, 1);
    }
    surface->map->sw_blit = NULL;

    /* Figure out if an accelerated hardware blit is possible */
    surface->flags &= ~GAL_HWACCEL;
    if ( surface->map->identity ) {
        int hw_blit_ok;

        if (src_video && dst_video && (src_video != dst_video)) {
            /*
             * We do not support accelerated blitting bettween two
             * diffrent videos.
             */
            hw_blit_ok = 0;
        }
        else if ((surface->flags & GAL_HWSURFACE) == GAL_HWSURFACE) {
            /* We only support accelerated blitting to hardware */
            if (surface->map->dst->flags & GAL_HWSURFACE) {
                /* dst and src are in the same video memory */
                hw_blit_ok = src_video->info.blit_hw;
                cur_video = src_video;
            }
            else {
                /* src is in video memory and dst is in system memory */
                hw_blit_ok = 0;
            }

            if (hw_blit_ok && (surface->flags & GAL_SRCCOLORKEY)) {
                hw_blit_ok = src_video->info.blit_hw_CC;
            }

            if (hw_blit_ok && (surface->flags & GAL_SRCALPHA)) {
                hw_blit_ok = src_video->info.blit_hw_A;
            }

        }
        else {
            /* We only support accelerated blitting to hardware */
            if (surface->map->dst->flags & GAL_HWSURFACE) {
                /* dst is in video memory and src is in system memory */
                hw_blit_ok = dst_video->info.blit_sw;
                cur_video = dst_video;
            }
            else {
                /* dst and src are all in system memory */
                hw_blit_ok = 0;
            }

            if (hw_blit_ok && (surface->flags & GAL_SRCCOLORKEY)) {
                hw_blit_ok = dst_video->info.blit_sw_CC;
            }

            if (hw_blit_ok && (surface->flags & GAL_SRCALPHA)) {
                hw_blit_ok = dst_video->info.blit_sw_A;
            }
        }

        if (hw_blit_ok && cur_video) {
            GAL_VideoDevice *video = cur_video;
            GAL_VideoDevice *this  = cur_video;
            video->CheckHWBlit (this, surface, surface->map->dst);
        }
    }

    /* Get the blit function index, based on surface mode */
    /* { 0 = nothing, 1 = colorkey, 2 = alpha, 3 = colorkey+alpha } */
    blit_index = 0;
    blit_index |= (!!(surface->flags & GAL_SRCCOLORKEY)) << 0;
    if (((surface->flags & GAL_SRCALPHA) && surface->format->alpha != GAL_ALPHA_OPAQUE)
         ||((surface->flags & GAL_SRCPIXELALPHA) && surface->format->Amask)) {
        if(surface != surface->map->dst){
            blit_index |= 2;
        }
    }

    /* Check for special "identity" case -- copy blit */
    if ( surface->map->identity && blit_index == 0 ) {
            surface->map->sw_data->blit = GAL_BlitCopy;

        /* Handle overlapping blits on the same surface */
        if ( surface == surface->map->dst ) {
                surface->map->sw_data->blit = GAL_BlitCopyOverlap;
        }
    } else {
        if ( surface->format->BitsPerPixel < 8 ) {
            surface->map->sw_data->blit =
                GAL_CalculateBlit0(surface, blit_index);
        } else {
            switch ( surface->format->BytesPerPixel ) {
                case 1:
                surface->map->sw_data->blit =
                    GAL_CalculateBlit1(surface, blit_index);
                break;
                case 2:
                case 3:
                case 4:
                surface->map->sw_data->blit =
                    GAL_CalculateBlitN(surface, blit_index);
                break;
                default:
                surface->map->sw_data->blit = NULL;
                break;
            }
        }
    }
    /* Make sure we have a blit function */
    if ( surface->map->sw_data->blit == NULL ) {
        GAL_InvalidateMap(surface->map);
        GAL_SetError("NEWGAL: Blit combination not supported.\n");
        return(-1);
    }

    /* Choose software blitting function */
    if(surface->flags & GAL_RLEACCELOK
       && (surface->flags & GAL_HWACCEL) != GAL_HWACCEL) {

            if(surface->map->identity && (blit_index == 1
               || (blit_index == 3 && !surface->format->Amask))) {
                if ( GAL_RLESurface(surface) == 0 )
                    surface->map->sw_blit = GAL_RLEBlit;
        } else if(blit_index == 2 && surface->format->Amask) {
                if ( GAL_RLESurface(surface) == 0 )
                    surface->map->sw_blit = GAL_RLEAlphaBlit;
        }
    }

    if ( surface->map->sw_blit == NULL ) {
#ifdef _MGUSE_PIXMAN
        if (blit_index & 1) {
            // have colorkey
            surface->map->sw_blit = GAL_SoftBlit;
        }
        else if (GAL_CheckPixmanFormat (surface, surface->map->dst)) {
            surface->map->sw_blit = GAL_PixmanBlit;
        }
        else {
            surface->map->sw_blit = GAL_SoftBlit;
        }
#else
        surface->map->sw_blit = GAL_SoftBlit;
#endif
    }
    return(0);
}

#ifdef _MGUSE_PIXMAN
#include <pixman.h>

struct rgbamasks_pixman_format_map {
    uint32_t pixman_format;
    Uint32 Rmask, Gmask, Bmask, Amask;
};

static struct rgbamasks_pixman_format_map _format_map_8bpp [] = {
    { PIXMAN_r3g3b2,    0xE0, 0x1C, 0x03, 0x00 },
    { PIXMAN_b2g3r3,    0x0E, 0x38, 0xC0, 0x00 },
    { PIXMAN_a2r2g2b2,  0x30, 0x0C, 0x03, 0xC0 },
    { PIXMAN_a2b2g2r2,  0x03, 0x0C, 0x03, 0xC0 },
};

static struct rgbamasks_pixman_format_map _format_map_16bpp [] = {
    { PIXMAN_x4r4g4b4,  0x0F00, 0x00F0, 0x000F, 0x0000 },
    { PIXMAN_x4b4g4r4,  0x000F, 0x00F0, 0x0F00, 0x0000 },
    // { PIXMAN_r4g4b4x4,  0xF000, 0x0F00, 0x00F0, 0x0000 },
    // { PIXMAN_b4g4r4x4,  0x00F0, 0x0F00, 0xF000, 0x0000 },
    { PIXMAN_a4r4g4b4,  0x0F00, 0x00F0, 0x000F, 0xF000 },
    { PIXMAN_a4b4g4r4,  0x000F, 0x00F0, 0x0F00, 0xF000 },
    // { PIXMAN_r4g4b4a4,  0xF000, 0x0F00, 0x00F0, 0x000F },
    // { PIXMAN_b4g4r4a4,  0x00F0, 0x0F00, 0xF000, 0x000F },
    { PIXMAN_x1r5g5b5,  0x7C00, 0x03E0, 0x001F, 0x0000 },
    { PIXMAN_x1b5g5r5,  0x001F, 0x03E0, 0x7C00, 0x0000 },
    // { PIXMAN_r5g5b5x1,  0xF800, 0x07C0, 0x003E, 0x0000 },
    // { PIXMAN_b5g5r5x1,  0x003E, 0x07C0, 0xF800, 0x0000 },
    { PIXMAN_a1r5g5b5,  0x7C00, 0x03E0, 0x001F, 0x8000 },
    { PIXMAN_a1b5g5r5,  0x001F, 0x03E0, 0x7C00, 0x8000 },
    // { PIXMAN_r5g5b5a1,  0xF800, 0x07C0, 0x003E, 0x0001 },
    // { PIXMAN_b5g5r5a1,  0x003E, 0x07C0, 0xF800, 0x0001 },
    { PIXMAN_r5g6b5,    0xF800, 0x07E0, 0x001F, 0x0000 },
    { PIXMAN_b5g6r5,    0x001F, 0x07E0, 0xF800, 0x0000 },
};

static struct rgbamasks_pixman_format_map _format_map_24bpp [] = {
    { PIXMAN_r8g8b8,    0xFF0000, 0x00FF00, 0x0000FF, 0x000000 },
    { PIXMAN_b8g8r8,    0x0000FF, 0x00FF00, 0xFF0000, 0x000000 },
};

static struct rgbamasks_pixman_format_map _format_map_32bpp [] = {
    { PIXMAN_x8r8g8b8,  0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { PIXMAN_x8b8g8r8,  0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000 },
    { PIXMAN_r8g8b8x8,  0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000 },
    { PIXMAN_b8g8r8x8,  0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000 },
    { PIXMAN_a8r8g8b8,  0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 },
    { PIXMAN_a8b8g8r8,  0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 },
    { PIXMAN_r8g8b8a8,  0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF },
    { PIXMAN_b8g8r8a8,  0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF },
};

static uint32_t translate_gal_format(const GAL_PixelFormat *gal_format)
{
    struct rgbamasks_pixman_format_map* map;
    size_t i, n;

    // XXX: ignore format with palette
    if (gal_format->BitsPerPixel <= 8 && gal_format->palette)
        return 0;

    switch (gal_format->BitsPerPixel) {
    case 8:
        map = _format_map_8bpp;
        n = TABLESIZE(_format_map_8bpp);
        break;

    case 16:
        map = _format_map_16bpp;
        n = TABLESIZE(_format_map_16bpp);
        break;

    case 24:
        map = _format_map_24bpp;
        n = TABLESIZE(_format_map_24bpp);
        break;

    case 32:
        map = _format_map_32bpp;
        n = TABLESIZE(_format_map_32bpp);
        break;

    default:
        return 0;
    }

    for (i = 0; i < n; i++) {
        if (gal_format->Rmask == map[i].Rmask &&
                gal_format->Gmask == map[i].Gmask &&
                gal_format->Bmask == map[i].Bmask &&
                gal_format->Amask == map[i].Amask) {
            return map[i].pixman_format;
        }
    }

    return 0;
}

BOOL GAL_CheckPixmanFormat (struct GAL_Surface *src, struct GAL_Surface *dst)
{
    src->tmp_data = translate_gal_format (src->format);
    if (dst != src && dst != NULL) {
        dst->tmp_data = translate_gal_format (dst->format);
        return src->tmp_data && dst->tmp_data;
    }

    return src->tmp_data != 0;
}

static int GAL_PixmanBlit (struct GAL_Surface *src, GAL_Rect *srcrect,
        struct GAL_Surface *dst, GAL_Rect *dstrect)
{
    pixman_image_t *src_img = NULL, *dst_img = NULL, *msk_img = NULL;
    pixman_op_t op;
    uint32_t alpha_bits;
    int retv = -1;
    //pixman_region32_t clip_region;
 
    assert (src->tmp_data);
    assert (dst->tmp_data);

    if ((src->flags & GAL_SRCALPHA) && src->format->alpha != GAL_ALPHA_OPAQUE) {
        memset (&alpha_bits, src->format->alpha, sizeof(alpha_bits));
        msk_img = pixman_image_create_bits_no_clear (PIXMAN_a8, 1, 1, &alpha_bits, 4);
        if (msk_img)
            pixman_image_set_repeat (msk_img, PIXMAN_REPEAT_NORMAL);
    }
    
    if (src->map->cop == COLOR_BLEND_LEGACY) {
        if ((src->flags & GAL_SRCPIXELALPHA) && src->format->Amask && src != dst) {
            op = PIXMAN_OP_OVER;
        }
        else {
            op = PIXMAN_OP_SRC;
        }
    }
    else {
        op = src->map->cop & ~COLOR_BLEND_FLAGS_MASK;
    }

    if (op > PIXMAN_OP_HSL_LUMINOSITY || op < PIXMAN_OP_CLEAR) {
        op = PIXMAN_OP_SRC;
    }

    src_img = pixman_image_create_bits_no_clear ((pixman_format_code_t)src->tmp_data,
            src->w, src->h,
            (uint32_t *)((char*)src->pixels + src->pixels_off),
            src->pitch);
    if (src_img == NULL)
        goto out;

    if (dst != src) {
        dst_img = pixman_image_create_bits_no_clear ((pixman_format_code_t)dst->tmp_data,
                dst->w, dst->h,
                (uint32_t *)((char*)dst->pixels + dst->pixels_off),
                dst->pitch);
        if (dst_img == NULL)
            goto out;
    }
    else {
        dst_img = src_img;
    }

    _DBG_PRINTF ("srcrect: %d, %d, %dx%d; dstrect: %d, %d, %dx%d\n",
            srcrect->x, srcrect->y, srcrect->w, srcrect->h,
            dstrect->x, dstrect->y, dstrect->w, dstrect->h);

#if 0
    pixman_region32_init_rect (&clip_region,
            dst->clip_rect.x, dst->clip_rect.y, dst->clip_rect.w, dst->clip_rect.h);
    pixman_image_set_clip_region32 (dst_img, &clip_region);
#endif

    retv = 0;
    pixman_image_composite32 (op, src_img, msk_img, dst_img,
            srcrect->x, srcrect->y,
            0, 0,
            dstrect->x, dstrect->y, 
            srcrect->w, srcrect->h);

    //pixman_region32_fini (&clip_region);

out:
    if (msk_img)
        pixman_image_unref (msk_img);

    if (src_img)
        pixman_image_unref (src_img);

    if (dst_img != src_img && dst_img)
        pixman_image_unref (dst_img);

    return retv;
}
#endif // _MGUSE_PIXMAN

