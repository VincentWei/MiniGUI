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

#include "common.h"
#include "newgal.h"
#include "blit.h"
#include "fbvideo.h"

#ifdef FB_ACCEL_NEOMAGIC_NM2070

#include "fbneomagic.h"
#include "neomagic_mmio.h"

typedef volatile struct {
  __u32 bltStat;
  __u32 bltCntl;
  __u32 xpColor;
  __u32 fgColor;
  __u32 bgColor;
  __u32 pitch;
  __u32 clipLT;
  __u32 clipRB;
  __u32 srcBitOffset;
  __u32 srcStart;
  __u32 reserved0;
  __u32 dstStart;
  __u32 xyExt;

  __u32 reserved1[19];

  __u32 pageCntl;
  __u32 pageBase;
  __u32 postBase;
  __u32 postPtr;
  __u32 dataPtr;
} Neo2200;

static Neo2200 *neo2200 = NULL;

/* Wait for vertical retrace */
static void WaitVBL(_THIS)
{
    while (neo2200->bltStat & 1)
        ;
}

static void WaitIdle(_THIS)
{
    while (neo2200->bltStat & 1)
        ;
}

/* Sets video mem colorkey and accelerated blit function */
static int SetHWColorKey(_THIS, GAL_Surface *surface, Uint32 key)
{
    return 0;
}

static inline void validate_dst (_THIS, GAL_Surface* dst)
{
    int bltMode = 0;

    switch (dst->format->BitsPerPixel) {
    case 8:
        bltMode |= NEO_MODE1_DEPTH8;
        break;
    case 15:
    case 16:
        bltMode |= NEO_MODE1_DEPTH16;
        break;
    default:
        break;
    }

  neo2200->bltStat = bltMode << 16;
  neo2200->pitch = (dst->pitch << 16) | (dst->pitch & 0xFFFF);
}

static int FillHWRect(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
{
    char *dst_base;

    dst_base = (char *)((char *)dst->pixels - mapped_mem);

    /* Execute the fill command */
    WaitIdle (this);

    /* Set the destination pixel format */
    validate_dst (this, dst);

    neo2200->fgColor = color;
    neo2200->bltCntl = NEO_BC3_FIFO_EN      |
                       NEO_BC0_SRC_IS_FG    |
                       NEO_BC3_SKIP_MAPPING |  0x0c0000;

    neo2200->dstStart = (UINT_PTR) dst_base +
                        (rect->y * dst->pitch) + 
                        (rect->x * dst->format->BytesPerPixel);

    neo2200->xyExt    = (rect->h << 16) | (rect->w & 0xffff);

    WaitIdle (this);

    return(0);
}

static void HWAccelBlit_helper (GAL_Surface *src, GAL_Rect *srcrect,
                       GAL_Surface *dst, GAL_Rect *dstrect)
{
    GAL_VideoDevice *this = current_video;
    Uint32 src_base;
    Uint32 dst_base;

    /* Set the source and destination pixel format */
    src_base = (Uint32)((char *)src->pixels - mapped_mem);
    dst_base = (Uint32)((char *)dst->pixels - mapped_mem);

    /* Perform the blit! */
    neo2200->pitch = (dst->pitch << 16) | (src->pitch & 0xFFFF);
    neo2200->bltCntl = NEO_BC3_FIFO_EN |
                       NEO_BC3_SKIP_MAPPING | 0x0c0000;
    if ( (src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY ) {
        neo2200->bltCntl |= NEO_BC0_SRC_TRANS;
        neo2200->xpColor = src->format->colorkey;
    }

    neo2200->srcStart = (Uint32) src_base + 
            srcrect->y * src->pitch + 
            srcrect->x * src->format->BytesPerPixel;

    neo2200->dstStart =  (Uint32) dst_base +
                 (dstrect->y * dst->pitch) +
                 (dstrect->x * dst->format->BytesPerPixel);

    neo2200->xyExt = (srcrect->h << 16) | (srcrect->w & 0xffff);

    WaitIdle (this);
}

static void make_rects (GAL_Rect *src, GAL_Rect* dst, const RECT* rc, Uint32 x, Uint32 y)
{
    src->x = rc->left;
    src->y = rc->top;
    src->w = rc->right - rc->left;
    src->h = rc->bottom - rc->top;

    dst->x = src->x + x;
    dst->y = src->y + y;
    dst->w = src->w;
    dst->h = src->h;
}

static int HWAccelBlit (GAL_Surface *src, GAL_Rect *srcrect,
                       GAL_Surface *dst, GAL_Rect *dstrect)
{
    RECT rc_src = {srcrect->x, srcrect->y, srcrect->x + srcrect->w, srcrect->y + srcrect->h};
    RECT rc_dst = {dstrect->x, dstrect->y, dstrect->x + dstrect->w, dstrect->y + dstrect->h};
    RECT rc_inter, rc_sub [4];
    
    if (rc_src.top < rc_dst.top && IntersectRect (&rc_inter, &rc_src, &rc_dst)) {
        int i, sub_count, inter_h;
        Sint32 off_x = dstrect->x - srcrect->x;
        Sint32 off_y = dstrect->y - srcrect->y;
        GAL_Rect tmp_src, tmp_dst;

        inter_h = rc_inter.bottom - rc_inter.top;
        for (i = 0; i < inter_h; i++) {
            rc_inter.top = rc_inter.bottom - 1;
            make_rects (&tmp_src, &tmp_dst, &rc_inter, off_x, off_y);
            HWAccelBlit_helper (src, &tmp_src, dst, &tmp_dst);
            rc_inter.bottom --;
        }

        sub_count = SubtractRect (rc_sub, &rc_src, &rc_dst);
        for (i = 0; i < sub_count; i++) {
            make_rects (&tmp_src, &tmp_dst, rc_sub + i, off_x, off_y);
            HWAccelBlit_helper (src, &tmp_src, dst, &tmp_dst);
        }
    }
    else if (rc_src.top == rc_dst.top && rc_src.left < rc_dst.left) {
        int i;
        GAL_Rect tmp_src, tmp_dst;
        Sint32 off_x = dstrect->x - srcrect->x;
        RECT rc = rc_src;

        for (i = 0; i < rc_src.right - rc_src.left; i++) {
            rc.left = rc.right - 1;
            make_rects (&tmp_src, &tmp_dst, &rc, off_x, 0);
            HWAccelBlit_helper (src, &tmp_src, dst, &tmp_dst);
            rc.right --;
        }
    }
    else
        HWAccelBlit_helper (src, srcrect, dst, dstrect);

    return(0);
}

static int CheckHWBlit (_THIS, GAL_Surface *src, GAL_Surface *dst)
{
    int accelerated;

    /* Set initial acceleration on */
    src->flags |= GAL_HWACCEL;

    /* Set the surface attributes */
    if ( (src->flags & GAL_SRCALPHA) == GAL_SRCALPHA ) {
        if ( ! this->info.blit_hw_A ) {
            src->flags &= ~GAL_HWACCEL;
        }
    }
    if ( (src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY ) {
        if ( ! this->info.blit_hw_CC ) {
            src->flags &= ~GAL_HWACCEL;
        }
    }

    if (src->format->BitsPerPixel > 16)
        src->flags &= ~GAL_HWACCEL;

    /* Check to see if final surface blit is accelerated */
    accelerated = !!(src->flags & GAL_HWACCEL);
    if ( accelerated ) {
        src->map->hw_blit = HWAccelBlit;
    }
    return(accelerated);
}

void FB_NeoMagicAccel(_THIS, __u32 card)
{
    neo2200 = (Neo2200*) mapped_io;

    /* We have hardware accelerated surface functions */
    this->CheckHWBlit = CheckHWBlit;
    wait_vbl = WaitVBL;
    wait_idle = WaitIdle;

    /* The NeoMagic has an accelerated color fill */
    this->info.blit_fill = 1;
    this->FillHWRect = FillHWRect;

    /* The NeoMagic has accelerated normal and colorkey blits */
    this->info.blit_hw = 1;
    this->info.blit_hw_CC = 1;
    this->SetHWColorKey = SetHWColorKey;
}

#endif /* FB_ACCEL_NEOMAGIC_NM2070 */

