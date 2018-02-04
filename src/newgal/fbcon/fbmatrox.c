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
#include "fbmatrox.h"
#include "matrox_mmio.h"


/* Wait for vertical retrace - taken from the XFree86 Matrox driver */
static void WaitVBL(_THIS)
{
    int count;

    /* find start of retrace */
    mga_waitidle();
    while (  (mga_in8(0x1FDA) & 0x08) )
        ;
    while ( !(mga_in8(0x1FDA) & 0x08) )
        ; 
    /* wait until we're past the start */
    count = mga_in32(0x1E20) + 2;
    while ( mga_in32(0x1E20) < count )
        ;
}
static void WaitIdle(_THIS)
{
    mga_waitidle();
}

/* Sets video mem colorkey and accelerated blit function */
static int SetHWColorKey(_THIS, GAL_Surface *surface, Uint32 key)
{
    return(0);
}

/* Sets per surface hardware alpha value */
#if 0
static int SetHWAlpha(_THIS, GAL_Surface *surface, Uint8 value)
{
    return(0);
}
#endif

static int FillHWRect(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
{
    int dstX, dstY;
    Uint32 fxbndry;
    Uint32 ydstlen;
    Uint32 fillop;

    switch (dst->format->BytesPerPixel) {
        case 1:
        color |= (color<<8);
        case 2:
        color |= (color<<16);
        break;
    }

    /* Set up the X/Y base coordinates */
    FB_dst_to_xy(this, dst, &dstX, &dstY);

    /* Adjust for the current rectangle */
    dstX += rect->x;
    dstY += rect->y;

    /* Set up the X boundaries */
    fxbndry = (dstX | ((dstX+rect->w) << 16));

    /* Set up the Y boundaries */
    ydstlen = (rect->h | (dstY << 16));

    /* Set up for color fill operation */
    fillop = MGADWG_TRAP | MGADWG_SOLID |
             MGADWG_ARZERO | MGADWG_SGNZERO | MGADWG_SHIFTZERO;

    /* Execute the operations! */
    mga_wait(5);
    mga_out32(MGAREG_DWGCTL, fillop | MGADWG_REPLACE);
    mga_out32(MGAREG_FCOL, color);
    mga_out32(MGAREG_FXBNDRY, fxbndry);
    mga_out32(MGAREG_YDSTLEN + MGAREG_EXEC, ydstlen);

    FB_AddBusySurface(dst);

    return(0);
}

static int HWAccelBlit(GAL_Surface *src, GAL_Rect *srcrect,
                       GAL_Surface *dst, GAL_Rect *dstrect)
{
    GAL_VideoDevice *this = current_video;
    int pitch, w, h;
    int srcX, srcY;
    int dstX, dstY;
    Uint32 sign;
    Uint32 start, stop;
    int skip;
    Uint32 blitop;

    /* FIXME: For now, only blit to display surface */
    if ( dst->pitch != GAL_VideoSurface->pitch ) {
        return(src->map->sw_blit(src, srcrect, dst, dstrect));
    }

    /* Calculate source and destination base coordinates (in pixels) */
    w = dstrect->w;
    h = dstrect->h;
    FB_dst_to_xy(this, src, &srcX, &srcY);
    FB_dst_to_xy(this, dst, &dstX, &dstY);

    /* Adjust for the current blit rectangles */
    srcX += srcrect->x;
    srcY += srcrect->y;
    dstX += dstrect->x;
    dstY += dstrect->y;
    pitch = dst->pitch/dst->format->BytesPerPixel;

    /* Set up the blit direction (sign) flags */
    sign = 0;
    if ( srcX < dstX ) {
        sign |= 1;
    }
    if ( srcY < dstY ) {
        sign |= 4;
        srcY += (h - 1);
        dstY += (h - 1);
    }

    /* Set up the blit source row start, end, and skip (in pixels) */
    stop = start = (srcY * pitch) + srcX;
    if ( srcX < dstX ) {
        start += (w - 1);
    } else {
        stop  += (w - 1);
    }
    if ( srcY < dstY ) {
        skip = -pitch;
    } else {
        skip = pitch;
    }

    /* Set up the blit operation */
    if ( (src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY ) {
        Uint32 colorkey;

        blitop = MGADWG_BFCOL | MGADWG_BITBLT |
                 MGADWG_SHIFTZERO | MGADWG_RSTR | (0x0C << 16) |
                 MGADWG_TRANSC;

        colorkey = src->format->colorkey;
        switch (dst->format->BytesPerPixel) {
            case 1:
            colorkey |= (colorkey<<8);
            case 2:
            colorkey |= (colorkey<<16);
            break;
        }
        mga_wait(2);
        mga_out32(MGAREG_FCOL, colorkey);
        mga_out32(MGAREG_BCOL, 0xFFFFFFFF);
    } else {
        blitop = MGADWG_BFCOL | MGADWG_BITBLT |
                 MGADWG_SHIFTZERO | MGADWG_RSTR | (0x0C << 16);
    }
    mga_wait(7);
    mga_out32(MGAREG_SGN, sign);
    mga_out32(MGAREG_AR3, start);
    mga_out32(MGAREG_AR0, stop);
    mga_out32(MGAREG_AR5, skip);
    mga_out32(MGAREG_FXBNDRY, (dstX | ((dstX + w-1) << 16)));
    mga_out32(MGAREG_YDSTLEN, (dstY << 16) | h);
    mga_out32(MGAREG_DWGCTL + MGAREG_EXEC, blitop);

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

void FB_MatroxAccel(_THIS, __u32 card)
{
    /* We have hardware accelerated surface functions */
    this->CheckHWBlit = CheckHWBlit;
    wait_vbl = WaitVBL;
    wait_idle = WaitIdle;

    /* The Matrox has an accelerated color fill */
    this->info.blit_fill = 1;
    this->FillHWRect = FillHWRect;

    /* The Matrox has accelerated normal and colorkey blits. */
    this->info.blit_hw = 1;
    /* The Millenium I appears to do the colorkey test a word
       at a time, and the transparency is intverted. (?)
     */
    if ( card != FB_ACCEL_MATROX_MGA2064W ) {
        this->info.blit_hw_CC = 1;
        this->SetHWColorKey = SetHWColorKey;
    }

#if 0 /* Not yet implemented? */
    /* The Matrox G200/G400 has an accelerated alpha blit */
    if ( (card == FB_ACCEL_MATROX_MGAG200)
      || (card == FB_ACCEL_MATROX_MGAG400)
    ) {
        this->info.blit_hw_A = 1;
        this->SetHWAlpha = SetHWAlpha;
    }
#endif
}

