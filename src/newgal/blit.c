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
static int GAL_SoftBlit(GAL_VideoDevice *video,
        GAL_Surface *src, GAL_Rect *srcrect,
        GAL_Surface *dst, GAL_Rect *dstrect)
{
    (void)video;

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
#include <pixman.h>

static int GAL_PixmanBlit(GAL_VideoDevice *video,
        struct GAL_Surface *src, GAL_Rect *srcrect,
        struct GAL_Surface *dst, GAL_Rect *dstrect);
#endif

/* Figure out which of many blit routines to set up on a surface */
int GAL_CalculateBlit(GAL_Surface *surface, const GAL_Rect *srcrc,
        const GAL_Rect *dstrc, DWORD op)
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
            video->CheckHWBlit (this, surface, srcrc,
                    surface->map->dst, dstrc, op);
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
        else if (surface->pix_img && surface->map->dst->pix_img &&
                surface != surface->map->dst) {
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

int GAL_SetupBlitting (GAL_Surface *src, GAL_Surface *dst, DWORD ops)
{
    if (GAL_CheckPixmanFormats (src, dst)) {
        if ((src->flags & GAL_SRCALPHA) && src->format->alpha != GAL_ALPHA_OPAQUE) {
            memset (&src->pix_alpha_bits, src->format->alpha, sizeof(uint32_t));
            src->msk_img = pixman_image_create_bits_no_clear (PIXMAN_a8, 1, 1,
                    &src->pix_alpha_bits, 4);
            if (src->msk_img)
                pixman_image_set_repeat (src->msk_img, PIXMAN_REPEAT_NORMAL);
        }
        else
            src->msk_img = NULL;

        ops &= COLOR_BLEND_PIXMAN_MASK;
        if (ops == COLOR_BLEND_LEGACY) {
            if ((src->flags & GAL_SRCPIXELALPHA) && src->format->Amask && src != dst) {
                src->pix_op = PIXMAN_OP_OVER;
            }
            else {
                src->pix_op = PIXMAN_OP_SRC;
            }
        }
        else {
            src->pix_op = ops;
        }

        if (src->pix_op > PIXMAN_OP_HSL_LUMINOSITY ||
                src->pix_op < PIXMAN_OP_CLEAR) {
            src->pix_op = PIXMAN_OP_SRC;
        }

        /* reset transform and filter */
        pixman_image_set_transform (src->pix_img, NULL);
        pixman_image_set_filter (src->pix_img, PIXMAN_FILTER_NEAREST, NULL, 0);
    }

    return 0;
}

int GAL_CleanupBlitting (GAL_Surface *src, GAL_Surface *dst)
{
    if (src->msk_img) {
        pixman_image_unref (src->msk_img);
        src->msk_img = NULL;
    }

    pixman_image_set_clip_region32 (dst->pix_img, NULL);

    return 0;
}

static int GAL_PixmanBlit (GAL_VideoDevice *video,
        struct GAL_Surface *src, GAL_Rect *srcrect,
        struct GAL_Surface *dst, GAL_Rect *dstrect)
{
    (void)video;

    pixman_image_t *src_img = src->pix_img, *dst_img = dst->pix_img;
    pixman_image_t *msk_img;
    pixman_op_t op;

    msk_img = src->msk_img;
    op = (pixman_op_t)src->pix_op;

#if 0
    pixman_region32_t clip_region;
    pixman_region32_init_rect (&clip_region,
            dst->clip_rect.x, dst->clip_rect.y, dst->clip_rect.w, dst->clip_rect.h);
    pixman_image_set_clip_region32 (dst_img, &clip_region);

    // pixman_blt does not work
    if (pixman_image_get_format (src_img) == pixman_image_get_format (dst_img) &&
            op == PIXMAN_OP_SRC && msk_img == NULL) {
        pixman_blt ((uint32_t *)((char*)src->pixels + src->pixels_off),
                    (uint32_t *)((char*)dst->pixels + dst->pixels_off),
                    src->pitch, dst->pitch,
                    GAL_BytesPerPixel (src),
                    GAL_BytesPerPixel (dst),
                    srcrect->x, srcrect->y,
                    dstrect->x, dstrect->y,
                    srcrect->w, srcrect->h);
    }
    else
#endif
    {
        pixman_image_composite32 (op, src_img, msk_img, dst_img,
                srcrect->x, srcrect->y,
                0, 0,
                dstrect->x, dstrect->y,
                srcrect->w, srcrect->h);
    }

    //pixman_region32_fini (&clip_region);

    return 0;
}
#endif // _MGUSE_PIXMAN

