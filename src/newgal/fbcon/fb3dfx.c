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
#include "fb3dfx.h"
#include "3dfx_mmio.h"

/* Wait for vertical retrace */
static void WaitVBL(_THIS)
{
    /* find start of retrace */
    tdfx_waitidle();
    while( (tdfx_in32(TDFX_STATUS) & STATUS_RETRACE) == STATUS_RETRACE )
        ;
    /* wait until we're past the start */
    while( (tdfx_in32(TDFX_STATUS) & STATUS_RETRACE) == 0 )
        ; 
}
static void WaitIdle(_THIS)
{
    tdfx_waitidle();
}

/* Sets video mem colorkey and accelerated blit function */
static int SetHWColorKey(_THIS, GAL_Surface *surface, Uint32 key)
{
    return(0);
}

static int FillHWRect(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
{
    int bpp;
    char *dst_base;
    Uint32 format;
    int dstX, dstY;

    /* Don't blit to the display surface when switched away */

    /* Set the destination pixel format */
    dst_base = (char *)((char *)dst->pixels - mapped_mem);
    bpp = dst->format->BitsPerPixel;
    format = dst->pitch | ((bpp+((bpp==8) ? 0 : 8)) << 13);

    /* Calculate source and destination base coordinates */
    dstX = rect->x;
    dstY = rect->y;

    /* Execute the fill command */
    tdfx_wait(6);
    tdfx_out32(DSTBASE, (UINT_PTR)dst_base);
    tdfx_out32(DSTFORMAT, format);
    tdfx_out32(COLORFORE, color);
    tdfx_out32(COMMAND_2D, COMMAND_2D_FILLRECT);
    tdfx_out32(DSTSIZE, rect->w | (rect->h << 16));
    tdfx_out32(LAUNCH_2D, dstX | (dstY << 16));

    FB_AddBusySurface(dst);

    return(0);
}

static int HWAccelBlit(GAL_Surface *src, GAL_Rect *srcrect,
                       GAL_Surface *dst, GAL_Rect *dstrect)
{
    GAL_VideoDevice *this = current_video;
    int bpp;
    Uint32 src_format;
    /* Uint32 dst_format; */
    char *src_base;
    char *dst_base;
    int srcX, srcY;
    int dstX, dstY;
    Uint32 blitop;
    Uint32 use_colorkey;

    /* Don't blit to the display surface when switched away */

    /* Set the source and destination pixel format */
    src_base = (char *)((char *)src->pixels - mapped_mem);
    bpp = src->format->BitsPerPixel;
    src_format = src->pitch | ((bpp+((bpp==8) ? 0 : 8)) << 13);
    dst_base = (char *)((char *)dst->pixels - mapped_mem);
    bpp = dst->format->BitsPerPixel;
    /* dst_format = dst->pitch | ((bpp+((bpp==8) ? 0 : 8)) << 13); */

    srcX = srcrect->x;
    srcY = srcrect->y;
    dstX = dstrect->x;
    dstY = dstrect->y;

    /* Assemble the blit operation */
    blitop = COMMAND_2D_BITBLT | (0xCC << 24);
    if ( srcX <= dstX ) {
        blitop |= BIT(14);
        srcX += (dstrect->w - 1);
        dstX += (dstrect->w - 1);
    }
    if ( srcY <= dstY ) {
        blitop |= BIT(15);
        srcY += (dstrect->h - 1);
        dstY += (dstrect->h - 1);
    }

    /* Perform the blit! */
    if ( (src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY ) {
            tdfx_wait(3);
            tdfx_out32(SRCCOLORKEYMIN, src->format->colorkey);
            tdfx_out32(SRCCOLORKEYMAX, src->format->colorkey);
            tdfx_out32(ROP_2D, 0xAA00);
        use_colorkey = 1;
    } else {
        use_colorkey = 0;
    }
    tdfx_wait(9);
    tdfx_out32(SRCBASE, (UINT_PTR)src_base);
    tdfx_out32(SRCFORMAT, src_format);
    tdfx_out32(DSTBASE, (UINT_PTR)dst_base);
    tdfx_out32(DSTFORMAT, src_format);
    tdfx_out32(COMMAND_2D, blitop);
    tdfx_out32(COMMANDEXTRA_2D, use_colorkey);
    tdfx_out32(DSTSIZE, dstrect->w | (dstrect->h << 16));
    tdfx_out32(DSTXY, dstX | (dstY << 16));
    tdfx_out32(LAUNCH_2D, srcX | (srcY << 16));

    FB_AddBusySurface(src);
    FB_AddBusySurface(dst);

    return(0);
}

static int CheckHWBlit(_THIS, GAL_Surface *src, GAL_Surface *dst)
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

    /* Check to see if final surface blit is accelerated */
    accelerated = !!(src->flags & GAL_HWACCEL);
    if ( accelerated ) {
        src->map->hw_blit = HWAccelBlit;
    }
    return(accelerated);
}

void FB_3DfxAccel(_THIS, __u32 card)
{
    /* We have hardware accelerated surface functions */
    this->CheckHWBlit = CheckHWBlit;
    wait_vbl = WaitVBL;
    wait_idle = WaitIdle;

    /* Reset the 3Dfx controller */
    tdfx_out32(BRESERROR0, 0);
    tdfx_out32(BRESERROR1, 0);

    /* The 3Dfx has an accelerated color fill */
    this->info.blit_fill = 1;
    this->FillHWRect = FillHWRect;

    /* The 3Dfx has accelerated normal and colorkey blits */
    this->info.blit_hw = 1;
    this->info.blit_hw_CC = 1;
    this->SetHWColorKey = SetHWColorKey;
}
