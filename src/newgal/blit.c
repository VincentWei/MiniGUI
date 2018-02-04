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
        info.s_pixels = (Uint8 *)src->pixels + src->offset +
                (Uint16)srcrect->y*src->pitch +
                (Uint16)srcrect->x*src->format->BytesPerPixel;
        info.s_width = srcrect->w;
        info.s_height = srcrect->h;
        info.s_skip=src->pitch-info.s_width*src->format->BytesPerPixel;
        info.d_pixels = (Uint8 *)dst->pixels + dst->offset +
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

            if(surface->map->identity
           && (blit_index == 1
               || (blit_index == 3 && !surface->format->Amask))) {
                if ( GAL_RLESurface(surface) == 0 )
                    surface->map->sw_blit = GAL_RLEBlit;
        } else if(blit_index == 2 && surface->format->Amask) {
                if ( GAL_RLESurface(surface) == 0 )
                    surface->map->sw_blit = GAL_RLEAlphaBlit;
        }
    }
    
    if ( surface->map->sw_blit == NULL ) {
        surface->map->sw_blit = GAL_SoftBlit;
    }
    return(0);
}

