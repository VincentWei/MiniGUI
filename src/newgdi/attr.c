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
** Drawing attributes of GDI
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/06/12, derived from original gdi.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "gal.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"

/******************* General drawing attributes *******************************/
Uint32 GUIAPI GetDCAttr (HDC hdc, int attr)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    if (attr < NR_DC_ATTRS && attr >= 0) {
        Uint32* attrs = (Uint32*) (&pdc->bkcolor);

        return attrs [attr];
    }

    return 0;
}

static int make_alpha_pixel_format (PDC pdc)
{
    Uint32 Rmask, Gmask, Bmask, Amask;

    if (pdc->alpha_pixel_format == NULL) {
        switch (pdc->surface->format->BytesPerPixel) {
        case 2:
            Rmask = 0x0000F000;
            Gmask = 0x00000F00;
            Bmask = 0x000000F0;
            Amask = 0x0000000F;
            break;
        case 3:
            Rmask = 0x00FC0000;
            Gmask = 0x0003F000;
            Bmask = 0x00000FC0;
            Amask = 0x0000003F;
            break;
        case 4:
            Rmask = 0xFF000000;
            Gmask = 0x00FF0000;
            Bmask = 0x0000FF00;
            Amask = 0x000000FF;
            break;
        default:
            return -1;
        }

        pdc->alpha_pixel_format = GAL_AllocFormat (
                        pdc->surface->format->BytesPerPixel << 3,
                        Rmask, Gmask, Bmask, Amask);
        if (pdc->alpha_pixel_format == NULL)
            return -1;
    }

    return 0;
}

static void make_gray_pixels (PDC pdc)
{
    if (pdc->bkmode == BM_TRANSPARENT
                    && !make_alpha_pixel_format (pdc)) {
        int i;
        Uint8 r, g, b, a;

        GAL_GetRGB (pdc->textcolor, pdc->surface->format, &r, &g, &b);

        a = 0;
        for (i = 0; i < 16; i++) {
            pdc->gray_pixels [i] = GAL_MapRGBA (pdc->alpha_pixel_format, 
                            r, g, b, a);
            a += 16;
        }

        pdc->gray_pixels [16] = GAL_MapRGBA (pdc->alpha_pixel_format, 
                        r, g, b, 255);
    }
    else {
        int i;

        if (pdc->surface->format->BitsPerPixel > 8) {
            int delta_r, delta_g, delta_b;
            RGB pal [17];

            GAL_GetRGB (pdc->bkcolor, pdc->surface->format, 
                            &pal->r, &pal->g, &pal->b);
            GAL_GetRGB (pdc->textcolor, pdc->surface->format, 
                            &pal[16].r, &pal[16].g, &pal[16].b);

            delta_r = ((int)pal[16].r - (int)pal[0].r) / 16;
            delta_g = ((int)pal[16].g - (int)pal[0].g) / 16;
            delta_b = ((int)pal[16].b - (int)pal[0].b) / 16;

            for (i = 1; i < 16; i++) {
                pal[i].r = pal[i - 1].r + delta_r;
                pal[i].g = pal[i - 1].g + delta_g;
                pal[i].b = pal[i - 1].b + delta_b;
                pdc->gray_pixels [i] = GAL_MapRGB (pdc->surface->format, 
                                pal[i].r, pal[i].g, pal[i].b);
            }
            pdc->gray_pixels [0] = pdc->bkcolor;
            pdc->gray_pixels [16] = pdc->textcolor;
        }
        else {
            for (i = 0; i < 16; i++) {
                pdc->gray_pixels [i] = pdc->bkcolor;
            }
            pdc->gray_pixels [16] = pdc->textcolor;
        }
    }
}

static void make_filter_pixels (PDC pdc)
{
    if (pdc->pLogFont->style & FS_WEIGHT_BOOK) {
        int i;
        Uint8 r, g, b, a;

        if (make_alpha_pixel_format (pdc))
            return;

        GAL_GetRGB (pdc->textcolor, pdc->surface->format, &r, &g, &b);

        a = 0;
        for (i = 0; i < 16; i++) {
            pdc->filter_pixels [i] = GAL_MapRGBA (pdc->alpha_pixel_format, 
                            r, g, b, a);
            a += 16;
        }

        pdc->filter_pixels [16] = GAL_MapRGBA (pdc->alpha_pixel_format, 
                        r, g, b, 255);

    }
    else if (pdc->pLogFont->style & FS_WEIGHT_LIGHT) {
        gal_pixel trans = pdc->bkcolor ^ 1;
        if (trans == pdc->textcolor)
            trans = pdc->bkcolor ^ 3;

        pdc->filter_pixels [0] = trans;
        pdc->filter_pixels [1] = pdc->bkcolor;
        pdc->filter_pixels [2] = pdc->textcolor;
    }
}

Uint32 GUIAPI SetDCAttr (HDC hdc, int attr, Uint32 value)
{
    Uint32 old_value = 0;
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    if (attr < NR_DC_ATTRS && attr >= 0) {
        Uint32* attrs = (Uint32*) (&pdc->bkcolor);
        old_value = attrs [attr];
        attrs [attr] = value;

        if (attr == DC_ATTR_TEXT_COLOR
                || attr == DC_ATTR_BK_COLOR
                || attr == DC_ATTR_BK_MODE) {
            make_gray_pixels (pdc);
            make_filter_pixels (pdc);
        }
#ifdef _MG_ADV_2DAPI
        else if (attr == DC_ATTR_PEN_TYPE && !pdc->dash_list) {
            pdc->dash_offset = 0;
            pdc->dash_list = (const unsigned char*) "\xa\xa";
            pdc->dash_list_len = 2;
        }
#endif

        return old_value;
    }

    return 0;
}

PLOGFONT GUIAPI GetCurFont (HDC hdc)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);
    return pdc->pLogFont;
}

PLOGFONT GUIAPI SelectFont (HDC hdc, PLOGFONT log_font)
{
    PLOGFONT old;
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);
    old = pdc->pLogFont;
    if (log_font == INV_LOGFONT)
        pdc->pLogFont = g_SysLogFont [1] ? g_SysLogFont [1] : g_SysLogFont [0];
    else
        pdc->pLogFont = log_font;
    
    make_gray_pixels (pdc);
    make_filter_pixels (pdc);
    return old;
}

