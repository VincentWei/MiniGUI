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
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
/*
** icon.c: Icon operations of GDI.
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
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "inline.h"
#include "memops.h"
#include "dc.h"
#include "icon.h"
#include "readbmp.h"
#include "misc.h"

#define align_32_bits(b) (((b) + 3) & -4)

#if ((defined(__THREADX__) && defined(__TARGET_VFANVIL__)) || \
        (defined(__NUCLEUS__) && defined(__TARGET_MONACO__)))
static int my_fgetc (FILE *stream)
{
    int c;
    while (!feof (stream)) {
        if (fread (&c, 1, 1, stream) == 1)
            return c;
        else
            break;
    }
    return EOF;
}

#define fgetc my_fgetc

#endif /* (__THREADX__ && __TARGET_VFANVIL__)... */

void _dc_restore_alpha_in_bitmap (const GAL_PixelFormat* format,
                void* dst_bits, unsigned int nr_bytes)
{
    int i;
    Uint8* dst8 = (Uint8*) dst_bits;
    Uint16* dst16 = (Uint16*) dst_bits;
    Uint32* dst32 = (Uint32*) dst_bits;
    switch (format->BytesPerPixel) {
    case 1:
        for (i = 0; i < nr_bytes; i++)
            dst8 [i] |= format->Amask;
        break;

    case 2:
        for (i = 0; i < nr_bytes >> 1; i++)
            dst16 [i] |= format->Amask;
        break;

    case 3:
        break;

    case 4:
        for (i = 0; i < nr_bytes >> 2; i++)
            dst32 [i] |= format->Amask;
        break;
    }
}

/************************* Icon support **************************************/

/* read_bmicolors:
 *  Loads the color palette for 1,4,8 bit formats.
 */
static void read_bmicolors_f (int ncols, RGB *pal, FILE* f)
{
   int i;

   for (i = 0; i < ncols; i++) {
      pal[i].b = fgetc (f);
      pal[i].g = fgetc (f);
      pal[i].r = fgetc (f);
      fgetc (f);
   }
}

static void read_bmicolors_m (int ncols, RGB *pal, const Uint8 *p)
{
   int i;

   for (i = 0; i < ncols; i++) {
      pal[i].b = *p++;
      pal[i].g = *p++;
      pal[i].r = *p++;
      p++;
   }
}

/* Icon creating and destroying.*/
HICON GUIAPI LoadIconFromFile (HDC hdc, const char* filename, int which)
{
    FILE* fp;
    WORD16 wTemp;
    BYTE bTemp;
    int  w, h, colornum;
    DWORD32 offset;
    DWORD32 imagesize;
    BYTE* image;
    HICON icon = 0;
    RGB pal[256];

    if (!(fp = fopen (filename, "rb"))) return 0;

    fseek (fp, sizeof (WORD16), SEEK_SET);

    /* the cbType of struct ICONDIR.*/
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp != 1) goto error;

    /* get ICON images count.*/
    wTemp = MGUI_ReadLE16FP (fp);
    if (which >= wTemp)
        which = wTemp - 1;
    if (which < 0)
        which = 0;

    /* seek to the right ICONDIRENTRY if needed.*/
    if (which != 0)
        fseek (fp, SIZEOF_ICONDIRENTRY * which, SEEK_CUR);

    /* cursor info, read the members of struct ICONDIRENTRY.*/
    w = fgetc (fp);       /* the width of first cursor*/
    h = fgetc (fp);       /* the height of first cursor*/
    if ((w%16) != 0 || (h%16) != 0) goto error;
    bTemp = fgetc (fp);   /* the bColorCount*/
    if(bTemp != 2 && bTemp != 16 && bTemp != 0) goto error;
    fseek(fp, sizeof(BYTE), SEEK_CUR); /* skip the bReserved*/

    wTemp = MGUI_ReadLE16FP (fp);   /* the wPlanes*/
    if(wTemp != 0 && wTemp != 1) goto error;

    wTemp = MGUI_ReadLE16FP (fp);   /* the wBitCount*/
    if(wTemp > 8) goto error;

#if 0
    DWORD32 size;
    size = MGUI_ReadLE32FP (fp);
#else
    fseek(fp, sizeof(DWORD32), SEEK_CUR); /* skip the wSize member.*/
#endif
    offset = MGUI_ReadLE32FP (fp);

    /* read the cursor image info. */
    fseek(fp, offset, SEEK_SET);
    fseek(fp, sizeof(DWORD32), SEEK_CUR); /* skip the biSize member.*/
#if 0
    DWORD32 imagew, imageh;
    imagew = MGUI_ReadLE32FP (fp);
    imageh = MGUI_ReadLE32FP (fp);
#else
    fseek(fp, sizeof(DWORD32), SEEK_CUR); /* skip the biWidth member.*/
    fseek(fp, sizeof(DWORD32), SEEK_CUR); /* skip the biHeight member.*/
#endif

    /* check the biPlanes member;*/
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp != 1) goto error;

    /* check the biBitCount member;*/
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp > 8) goto error;

    colornum = (int)wTemp;
    fseek(fp, sizeof(DWORD32), SEEK_CUR); /* skip the biCompression members.*/

    if (colornum == 1)
        imagesize = align_32_bits(w>>3) * h;
    else if (colornum == 4)
        imagesize = align_32_bits(w>>1) * h;
    else
        imagesize = align_32_bits(w) * h;

    imagesize += align_32_bits(w>>3) * h;
    fseek(fp, sizeof(DWORD32), SEEK_CUR);

    /* skip the rest members and the color table.*/
    fseek(fp, sizeof(DWORD32)*4, SEEK_CUR);
    read_bmicolors_f (1 << colornum, pal, fp);

    /* allocate memory for image.*/
#ifdef HAVE_ALLOCA
    /* Use alloca, the stack may be enough for the image.*/
    if ((image = (BYTE*) alloca (imagesize)) == NULL)
#else
    if ((image = (BYTE*) malloc (imagesize)) == NULL)
#endif
        goto error;

    /* read image*/
    if (fread (image, 1, imagesize, fp) < imagesize) {
        goto error_free;
    }

    icon = CreateIconEx (hdc, w, h,
                   image + (imagesize - (align_32_bits(w>>3) * h)), image,
                   colornum, pal);

error_free:
#ifndef HAVE_ALLOCA
    free (image);
#endif

error:
    fclose (fp);
    return icon;
}

HICON GUIAPI LoadIconFromMem (HDC hdc, const void* area, int which)
{
    const Uint8* p = (Uint8*)area;
    WORD16 wTemp;
    BYTE bTemp;

    int  w, h, colornum;
    DWORD32 offset;
    DWORD32 imagesize;
    RGB pal[256];

    p += sizeof (WORD16);

    /* the cbType of struct ICONDIR.*/
    wTemp = MGUI_ReadLE16Mem (&p);
    if (wTemp != 1) goto error;

    /* get ICON images count.*/
    wTemp = MGUI_ReadLE16Mem (&p);
    if (which >= wTemp)
        which = wTemp - 1;
    if (which < 0)
        which = 0;

    /* seek to the right ICONDIRENTRY if needed.*/
    if (which != 0)
        p += SIZEOF_ICONDIRENTRY * which;

    /* cursor info, read the members of struct ICONDIRENTRY. */
    w = *p++;       /* the width of first cursor */
    h = *p++;       /* the height of first cursor */
    if ((w%16) != 0 || (h%16) != 0)
        goto error;
    bTemp = *p++;   /* the bColorCount */
    if(bTemp != 2 && bTemp != 16 && bTemp != 0) goto error;

    /* skip the bReserved */
    p ++;
    wTemp = MGUI_ReadLE16Mem (&p);   /* the wPlanes */
    if (wTemp != 0 && wTemp != 1) goto error;
    wTemp = MGUI_ReadLE16Mem (&p);   /* the wBitCount */
    if (wTemp > 8) goto error;
#if 0
    DWORD32 size;
    size = MGUI_ReadLE32Mem (&p);
#else
    p += sizeof(DWORD32);
#endif
    offset = MGUI_ReadLE32Mem (&p);

    /* read the cursor image info. */
    p = (Uint8*)area + offset;
    /* skip the biSize member. */
    p += sizeof(DWORD32);
#if 0
    DWORD32 imagew, imageh;
    imagew = MGUI_ReadLE32Mem (&p);
    imageh = MGUI_ReadLE32Mem (&p);
#else
    p += sizeof(DWORD32);
    p += sizeof(DWORD32);
#endif
    /* check the biPlanes member; */
    wTemp = MGUI_ReadLE16Mem (&p);
    if (wTemp != 1) goto error;
    /* check the biBitCount member; */
    wTemp = MGUI_ReadLE16Mem (&p);
    if (wTemp > 8) goto error;
    colornum = (int)wTemp;
    /* skip the biCompression members. */
    p += sizeof (DWORD32);

    if (colornum == 1)
        imagesize = align_32_bits(w>>3) * h;
    else if (colornum == 4)
        imagesize = align_32_bits(w>>1) * h;
    else
        imagesize = align_32_bits(w) * h;

    imagesize += align_32_bits(w>>3) * h;
    p += sizeof (DWORD32);

    /* skip the rest members and the color table. */
    p += sizeof(DWORD32)*4;
    read_bmicolors_m (1 << colornum, pal, p);
    p += sizeof(BYTE)*(4<<colornum);

    return CreateIconEx (hdc, w, h,
                    p + (imagesize - (align_32_bits(w>>3) * h)), p,
                    colornum, pal);

error:
    return 0;
}

HICON GUIAPI CreateIconEx (HDC hdc, int w, int h, const BYTE* pANDBits,
                        const BYTE* pXORBits, int colornum, const RGB *pal)
{
    PDC pdc;
    PICON picon;
    Uint32 image_size;
    gal_pixel trans_pixel;

    /* Since 5.0.0, we always decode an icon into RGBA8888 compliant pixels */
    hdc = __mg_get_common_rgba8888_dc ();
    if (!(pdc = dc_HDC2PDC (hdc)))
        return 0;

    if ((w % 16) != 0 || (h % 16) != 0 || w <= 0 || h <= 0)
        return 0;

    /* allocate memory. */
    if (!(picon = mg_slice_new (ICON)))
        return 0;

    image_size = GAL_GetBoxSize (pdc->surface, w, h, &picon->pitch);
    if (!(picon->pixels = malloc (image_size)))
        goto error;

    picon->width = w;
    picon->height = h;

    trans_pixel = GAL_MapRGBA (pdc->surface->format, 0x00, 0x00, 0x00, 0x00);
    if (colornum == 1) {
        int x, y;
        int mono_pitch = align_32_bits (w >> 3);
        BYTE and_byte = 0;
        BYTE xor_byte = 0;
        gal_pixel white_pixel = GAL_MapRGBA (pdc->surface->format,
                0xFF, 0xFF, 0xFF, 0xFF);
        gal_pixel black_pixel = GAL_MapRGBA (pdc->surface->format,
                0x00, 0x00, 0x00, 0xFF);

        pANDBits += mono_pitch * (h - 1);
        pXORBits += mono_pitch * (h - 1);
        Uint32* pixels = (Uint32*)picon->pixels;

        for (y = 0; y < h; y++) {
            const BYTE* and_bytes = pANDBits;
            const BYTE* xor_bytes = pXORBits;

            for (x = 0; x < w; x++) {
                if (x % 8 == 0) {
                    and_byte = *and_bytes++;
                    xor_byte = *xor_bytes++;
                }

                if (and_byte & (0x80 >> (x % 8))) {
                    pixels [x] = trans_pixel;
                }
                else {
                    if (xor_byte & (0x80 >> (x % 8))) {
                        pixels [x] = white_pixel;
                    }
                    else {
                        pixels [x] = black_pixel;
                    }
                }
            }

            pANDBits -= mono_pitch;
            pXORBits -= mono_pitch;
            pixels += (picon->pitch >> 2);
        }
    }
    else if (colornum == 4) {
        int x, y;
        int pitch_mono = align_32_bits (w >> 3);
        int pitch_16c = align_32_bits (w >> 1);
        BYTE and_byte = 0;
        BYTE xor_byte = 0;
        int idx_16c = 0;
        const RGB* std16c_rgb = __mg_bmp_get_std_16c ();

        pANDBits += pitch_mono * (h - 1);
        pXORBits += pitch_16c * (h - 1);
        Uint32* pixels = (Uint32*)picon->pixels;

        for (y = 0; y < h; y++) {
            const BYTE* and_bytes = pANDBits;
            const BYTE* xor_bytes = pXORBits;

            for (x = 0; x < w; x++) {
                if (x % 8 == 0) {
                    and_byte = *and_bytes++;
                }
                if (x % 2 == 0) {
                    xor_byte = *xor_bytes++;
                    idx_16c = (xor_byte >> 4) & 0x0F;
                }
                else {
                    idx_16c = xor_byte & 0x0F;
                }

                if (and_byte & (0x80 >> (x % 8))) {
                    pixels [x] = trans_pixel;
                }
                else if (pal) {
                    pixels [x] = GAL_MapRGBA (pdc->surface->format,
                            pal[idx_16c].r, pal[idx_16c].g,
                            pal[idx_16c].b, 0xFF);
                }
                else {
                    pixels [x] = GAL_MapRGBA (pdc->surface->format,
                            std16c_rgb[idx_16c].r, std16c_rgb[idx_16c].g,
                            std16c_rgb[idx_16c].b, 0xFF);
                }
            }

            pANDBits -= pitch_mono;
            pXORBits -= pitch_16c;
            pixels += (picon->pitch >> 2);
        }
    }
    else if (colornum == 8) {
        int x, y;
        int pitch_mono = align_32_bits (w >> 3);
        int pitch_256c = align_32_bits (w);
        BYTE and_byte = 0;
        BYTE xor_byte = 0;

        pANDBits += pitch_mono * (h - 1);
        pXORBits += pitch_256c * (h - 1);
        Uint32* pixels = (Uint32*)picon->pixels;

        for (y = 0; y < h; y++) {
            const BYTE* and_bytes = pANDBits;
            const BYTE* xor_bytes = pXORBits;

            for (x = 0; x < w; x++) {
                if (x % 8 == 0) {
                    and_byte = *and_bytes++;
                }

                xor_byte = *xor_bytes++;

                if (and_byte & (0x80 >> (x % 8))) {
                    pixels [x] = trans_pixel;
                }
                else if (pal) {
                    pixels [x] = GAL_MapRGBA (pdc->surface->format,
                            pal[xor_byte].r, pal[xor_byte].g,
                            pal[xor_byte].b, 0xFF);
                }
                else {
                    /* Treat the bitmap uses the dithered colorful palette. */
                    pixels [x] = GAL_MapRGBA (pdc->surface->format,
                            (xor_byte >> 5) & 0x07, (xor_byte >> 2) & 0x07,
                            xor_byte & 0x03, 0xFF);
                }
            }

            pANDBits -= pitch_mono;
            pXORBits -= pitch_256c;
            pixels += (picon->pitch >> 2);
        }
    }

    return (HICON)picon;

error:
    free (picon);
    return 0;
}

HICON GUIAPI LoadBitmapIconEx (HDC hdc, MG_RWops* area, const char* ext)
{
    BITMAP bmp;
    PICON ico;
    int ret;

    if ((hdc = __mg_get_common_rgba8888_dc()) == HDC_INVALID)
        return 0;

    if (!(ico = mg_slice_new (ICON)))
        return 0;

    ret = LoadBitmapEx (hdc, &bmp, area, ext);
    if (ret) {
        goto error;
    }

    ico->width = bmp.bmWidth;
    ico->height = bmp.bmHeight;
    ico->pitch = bmp.bmPitch;
    ico->pixels = bmp.bmBits;
    return (HICON)ico;

error:
    mg_slice_delete (ICON, ico);
    return 0;
}

HICON GUIAPI LoadBitmapIconFromFile (HDC hdc, const char* file_name)
{
    PICON ico;
    MG_RWops* area;
    const char* ext;

    /* format, png, jpg etc. */
    if ((ext = __mg_get_extension (file_name)) == NULL) {
        return 0;
    }

    if (!(area = MGUI_RWFromFile (file_name, "rb"))) {
        _WRN_PRINTF ("Failed to make RWOps\n");
        return 0;
    }

    ico = LoadBitmapIconEx (hdc, area, ext);
    MGUI_RWclose (area);
    return ico;
}

HICON GUIAPI LoadBitmapIconFromMem (HDC hdc, const void* mem, size_t size,
        const char* ext)
{
    PICON ico;
    MG_RWops* area;

    if (!(area = MGUI_RWFromMem ((void*)mem, size))) {
        _WRN_PRINTF ("Failed to make RWOps\n");
        return NULL;
    }

    ico = LoadBitmapIconEx (hdc, area, ext);

    MGUI_RWclose (area);
    return ico;
}

BOOL GUIAPI DestroyIcon (HICON hicon)
{
    PICON picon = (PICON)hicon;

    if (!picon)
        return FALSE;

    free (picon->pixels);
    mg_slice_delete (ICON, picon);
    return TRUE;
}

BOOL GUIAPI GetIconSize (HICON hicon, int* w, int* h)
{
    PICON picon = (PICON)hicon;

    if (!picon)
        return FALSE;

    if (w) *w = picon->width;
    if (h) *h = picon->height;
    return TRUE;
}

void GUIAPI DrawIcon (HDC hdc, int x, int y, int w, int h, HICON hicon)
{
    HDC icon_dc;
    PICON picon = (PICON)hicon;

    icon_dc = __mg_get_common_rgba8888_dc();
    if (icon_dc == HDC_INVALID)
        return;

    if (!__mg_reset_common_rgba8888_dc (picon->width, picon->height,
                picon->pitch, picon->pixels))
        return;

    if (w <= 0) w = picon->width;
    if (h <= 0) h = picon->height;

    SetMemDCAlpha (icon_dc, MEMDC_FLAG_SRCPIXELALPHA, 0);
    if (w != picon->width || h != picon->height) {
        StretchBlt (icon_dc, 0, 0, picon->width, picon->height,
                hdc, x, y, w, h, 0);
    }
    else {
        BitBlt (icon_dc, 0, 0, picon->width, picon->height,
                hdc, x, y, 0);
    }
}

