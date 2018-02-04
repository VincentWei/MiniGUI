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
** palette.c: Palette operations of GDI.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/08/02
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

int GUIAPI GetPalette (HDC hdc, int start, int len, GAL_Color* cmap)
{
    PDC pdc;
    GAL_Palette* pal;

    pdc = dc_HDC2PDC (hdc);
    pal = pdc->surface->format->palette;

    return GetPaletteEntries ((HPALETTE)pal, start, len, cmap);
}

BOOL GUIAPI SetPalette (HDC hdc, int start, int len, GAL_Color* cmap)
{
    PDC pdc = dc_HDC2PDC (hdc);

    return GAL_SetPalette (pdc->surface, 
            dc_IsScreenDC(pdc) ? (GAL_LOGPAL|GAL_PHYSPAL) : GAL_LOGPAL, 
            cmap, start, len);
}

BOOL GUIAPI SetColorfulPalette (HDC hdc)
{
    PDC pdc;
    GAL_Color pal[256];
    pdc = dc_HDC2PDC (hdc);
    if (pdc->surface->format->BitsPerPixel == 8) {
        GAL_DitherColors (pal, 8);
        GAL_SetColors (pdc->surface, pal, 0, 256);
    }

    return FALSE;
}

HPALETTE GUIAPI CreatePalette (GAL_Palette *pal)
{
    GAL_Palette* new_pal;

    if (!pal) return 0; /* TODO, system palette? */

    new_pal = (GAL_Palette *) malloc(sizeof(GAL_Palette));

    new_pal->ncolors = pal->ncolors;
    new_pal->colors = malloc(new_pal->ncolors * sizeof(GAL_Color));
    memcpy (new_pal->colors, pal->colors, new_pal->ncolors * sizeof(GAL_Color));
    return (HPALETTE) new_pal;
}

void GUIAPI DestroyPalette (HPALETTE hpal)
{
    GAL_Palette* gal_pal = (GAL_Palette*)hpal;

    free (gal_pal);
}

HPALETTE GUIAPI GetDefaultPalette (void)
{
    PDC pdc;
    GAL_Palette* pal;

    pdc = dc_HDC2PDC (HDC_SCREEN);
    pal = pdc->surface->format->palette;

    return (HPALETTE) pal;
}

int GUIAPI GetPaletteEntries (HPALETTE hpal, int start, int len, 
                GAL_Color* cmap)
{
    GAL_Palette* pal;

    if (!hpal || !cmap || len <= 0) return 0;

    pal = (GAL_Palette *)hpal;

    if (start < 0) start = 0;
    if (start + len > pal->ncolors) len = pal->ncolors - start;

    memcpy (cmap, pal->colors + start, sizeof (GAL_Color) * len);
 
    return len;
}

int GUIAPI SetPaletteEntries (HPALETTE hpal, int start, int len, 
                GAL_Color* cmap)
{
    GAL_Palette* pal;

    if (!hpal || !cmap || len <= 0) return 0;

    pal = (GAL_Palette *)hpal;

    if (start < 0) start = 0;
    if (start + len > pal->ncolors) len = pal->ncolors - start;

    memcpy (pal->colors + start, cmap, sizeof (GAL_Color) * len);

    return len;
}

BOOL GUIAPI ResizePalette (HPALETTE hpal, int len)
{
    GAL_Palette* pal;

    if (!hpal || len <= 0) return 0;

    pal = (GAL_Palette *)hpal;

    pal->ncolors = len;
    pal->colors = realloc(pal->colors, len * sizeof(GAL_Color));

    return TRUE;
}

UINT GUIAPI GetNearestPaletteIndex (HPALETTE hpal, 
                Uint8 red, Uint8 green, Uint8 blue)
{
    GAL_Palette* pal;

    if (!hpal) return 0;

    pal = (GAL_Palette *)hpal;

    return GAL_FindColor (pal, red, green, blue);
}

RGBCOLOR GUIAPI GetNearestColor (HDC hdc, Uint8 red, Uint8 green, Uint8 blue)
{
    gal_pixel pixel = RGB2Pixel (hdc, red, green, blue);

    Pixel2RGB (hdc, pixel, &red, &green, &blue);

    return MakeRGB (red, green, blue);
}

BOOL GUIAPI SelectPalette (HDC hdc, HPALETTE hpal, BOOL reserved)
{
    PDC pdc = dc_HDC2PDC (hdc);
    GAL_Palette* pal;

    if (!hpal) return FALSE;

    pal = (GAL_Palette *)hpal;

    return GAL_SetPalette (pdc->surface, GAL_LOGPAL, 
            pal->colors, 0, pal->ncolors);
}

BOOL GUIAPI RealizePalette (HDC hdc)
{
    PDC pdc, pdc_screen;
    GAL_Palette* pal;

    pdc = dc_HDC2PDC (hdc);
    pal = pdc->surface->format->palette;

    if (pal == NULL || dc_IsMemDC (pdc)) return 0;

    pdc_screen = dc_HDC2PDC (HDC_SCREEN);
    return GAL_SetPalette (pdc_screen->surface, GAL_LOGPAL|GAL_PHYSPAL,
            pal->colors, 0, pal->ncolors);
}

