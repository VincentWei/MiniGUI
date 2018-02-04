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
#include "cursor.h"
#include "readbmp.h"

#define align_32_bits(b) (((b) + 3) & -4)

/*-------------------------*/
#if ((defined(__THREADX__) && defined(__TARGET_VFANVIL__)) || (defined(__NUCLEUS__) && defined(__TARGET_MONACO__)))
#define fgetc my_fgetc
int my_fgetc(FILE *stream)
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
#endif
/*-------------------------*/

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

HICON GUIAPI CreateIconEx (HDC hdc, int w, int h, const BYTE* pAndBits, 
                        const BYTE* pXorBits, int colornum, const RGB *pal)
{
    PDC pdc;
    PICON picon;
    Uint32 image_size;

    pdc = dc_HDC2PDC (hdc);
    
    if( (w%16) != 0 || (h%16) != 0 ) return 0;

    /* allocate memory. */
    if (!(picon = (PICON)malloc (sizeof(ICON))))
        return 0;

    image_size = GAL_GetBoxSize (pdc->surface, w, h, &picon->pitch);

    if (!(picon->AndBits = malloc (image_size)))
        goto error1;
    if (!(picon->XorBits = malloc (image_size)))
        goto error2;

    picon->width = w;
    picon->height = h;

    if (colornum == 1) {
        ExpandMonoBitmap (hdc, picon->AndBits, picon->pitch, pAndBits, 
                align_32_bits (w >> 3), w, h, MYBMP_FLOW_UP, 0, 0xFFFFFFFF);
        ExpandMonoBitmap (hdc, picon->XorBits, picon->pitch, pXorBits, 
                align_32_bits (w >> 3), w, h, MYBMP_FLOW_UP, 0, 0xFFFFFFFF);
    }
    else if (colornum == 4) {
        ExpandMonoBitmap (hdc, picon->AndBits, picon->pitch, pAndBits, 
                align_32_bits (w >> 3), w, h, MYBMP_FLOW_UP, 0, 0xFFFFFFFF);
        Expand16CBitmap (hdc, picon->XorBits, picon->pitch, pXorBits, 
                align_32_bits (w >> 1), w, h, MYBMP_FLOW_UP, pal);
    }
    else if (colornum == 8){
        ExpandMonoBitmap (hdc, picon->AndBits, picon->pitch, pAndBits, 
                align_32_bits (w >> 3), w, h, MYBMP_FLOW_UP, 0, 0xFFFFFFFF);
        Expand256CBitmap (hdc, picon->XorBits, picon->pitch, pXorBits, 
                align_32_bits (w), w, h, MYBMP_FLOW_UP, pal, NULL, NULL);
    }

    return (HICON)picon;

error2:
    free(picon->AndBits);
error1:
    free (picon);

    return 0;
}

BOOL GUIAPI DestroyIcon (HICON hicon)
{
    PICON picon = (PICON)hicon;

    if (!picon)
        return FALSE;

    free(picon->AndBits);
    free(picon->XorBits);
    free(picon);
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
    PDC pdc;
    PCLIPRECT cliprect;
    PICON picon = (PICON)hicon;
    Uint32 imagesize, pitch;
    BYTE* iconimage;
    BITMAP bitmap;
    BYTE* andbits = NULL;
    BYTE* xorbits = NULL;
    RECT eff_rc;
    GAL_Rect rect;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    if (w <= 0) w = picon->width;
    if (h <= 0) h = picon->height;

    /* Transfer logical to device to screen here. */
    w += x; h += y;
    coor_LP2SP (pdc, &x, &y);
    coor_LP2SP (pdc, &w, &h);
    SetRect (&pdc->rc_output, x, y, w, h);
    NormalizeRect (&pdc->rc_output);
    w = RECTW (pdc->rc_output); h = RECTH (pdc->rc_output);

    imagesize = GAL_GetBoxSize (pdc->surface, w, h, &pitch);

    if ((iconimage = malloc (imagesize)) == NULL)
        goto free_ret;

    if (w != picon->width || h != picon->height) {
        BITMAP unscaled, scaled;

        andbits = malloc (imagesize);
        xorbits = malloc (imagesize);
        if (andbits == NULL || xorbits == NULL)
            goto free_ret;

        unscaled.bmType = BMP_TYPE_NORMAL;
        unscaled.bmBytesPerPixel = GAL_BytesPerPixel (pdc->surface);
        unscaled.bmWidth = picon->width;
        unscaled.bmHeight = picon->height;
        unscaled.bmPitch = picon->pitch;
        unscaled.bmBits = picon->AndBits;

        unscaled.bmType = BMP_TYPE_NORMAL;
        scaled.bmBytesPerPixel = unscaled.bmBytesPerPixel;
        scaled.bmWidth = w;
        scaled.bmHeight = h;
        scaled.bmPitch = pitch;
        scaled.bmBits = andbits;

        ScaleBitmap (&scaled, &unscaled);

        unscaled.bmBits = picon->XorBits;
        scaled.bmBits = xorbits;
        ScaleBitmap (&scaled, &unscaled);
    }
    else {
        andbits = picon->AndBits;
        xorbits = picon->XorBits;
        pitch = picon->pitch;
    }

    ENTER_DRAWING (pdc);

    rect.x = x; rect.y = y; rect.w = w; rect.h = h;
    bitmap.bmPitch = pitch;
    bitmap.bmBits = iconimage;
    GAL_GetBox (pdc->surface, &rect, &bitmap);

#ifdef ASM_memandcpy4
    ASM_memandcpy4 (iconimage, andbits, imagesize >> 2);
    ASM_memxorcpy4 (iconimage, xorbits, imagesize >> 2);
#else
    {
        int i;
        Uint32* dst = (Uint32*)iconimage;
        Uint32* src1 = (Uint32*)andbits;
        Uint32* src2 = (Uint32*)xorbits;
        for (i = 0; i < imagesize>>2; i++) {
            dst [i] &= src1 [i];
            dst [i] ^= src2 [i];
        }
    }
#endif

    if (pdc->surface->format->Amask) {
        _dc_restore_alpha_in_bitmap (pdc->surface->format,
                iconimage, imagesize);
    }

    cliprect = pdc->ecrgn.head;
    while (cliprect) {
        if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)) {
            SET_GAL_CLIPRECT(pdc, eff_rc);

            GAL_PutBox (pdc->surface, &rect, &bitmap);
        }

        cliprect = cliprect->next;
    }

    LEAVE_DRAWING (pdc);

free_ret:
    UNLOCK_GCRINFO (pdc);
    free (iconimage);
    if (w != picon->width || h != picon->height) {
        free (andbits);
        free (xorbits);
    }
}

