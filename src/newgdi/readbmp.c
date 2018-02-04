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
** Top-level bitmap file read/save function.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/08/26, derived from original bitmap.c
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
#include "misc.h"
#include "readbmp.h"
#include "bitmap.h"

inline static BOOL device_has_alpha(HDC hdc);

/* Init a bitmap as a normal bitmap */
BOOL GUIAPI InitBitmap (HDC hdc, Uint32 w, Uint32 h, Uint32 pitch, BYTE* bits,
                        PBITMAP bmp)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    if (w == 0 || h == 0)
        return FALSE;

    if (bits && pitch) {
        if (pdc->surface->format->BytesPerPixel * w > pitch)
            return FALSE;
        bmp->bmBits = bits;
        bmp->bmPitch = pitch;
    }
    else if (!bits) {

        size_t size = GAL_GetBoxSize (pdc->surface, w, h, &bmp->bmPitch);
        if (!(bmp->bmBits = malloc (size)))
            return FALSE;
    }
    else /* bits is not zero, but pitch is zero */
        return FALSE;

    bmp->bmType = BMP_TYPE_NORMAL;
    bmp->bmAlpha = 0;
    bmp->bmColorKey = 0;
    bmp->bmWidth = w;
    bmp->bmHeight = h;
    bmp->bmBitsPerPixel = pdc->surface->format->BitsPerPixel;
    bmp->bmBytesPerPixel = pdc->surface->format->BytesPerPixel;
    bmp->bmAlphaMask = NULL;
    bmp->bmAlphaPitch = 0;

    return TRUE;
}

/* Init a standard pixel format with alpha of a bitmap */
BOOL GUIAPI InitBitmapPixelFormat (HDC hdc, PBITMAP bmp)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    bmp->bmBitsPerPixel = pdc->surface->format->BitsPerPixel;
    bmp->bmBytesPerPixel = pdc->surface->format->BytesPerPixel;

    if (bmp->bmType & BMP_TYPE_ALPHA) {
        if (device_has_alpha(hdc) || bmp->bmBitsPerPixel <= 8) {
            bmp->bmType &= ~BMP_TYPE_ALPHA_MASK;
            bmp->bmType &= ~BMP_TYPE_PRIV_PIXEL;
            return TRUE;
        }

        bmp->bmType |= BMP_TYPE_ALPHA_MASK;
        bmp->bmAlphaPitch = (bmp->bmWidth + 3) & (~3);
        bmp->bmAlphaMask = calloc(1, bmp->bmHeight * bmp->bmAlphaPitch);
    }
    return TRUE;
}

static BOOL find_color_key (GAL_PixelFormat* format, const MYBITMAP* my_bmp, 
                          const RGB* pal, RGB* trans)
{
    BOOL ret = FALSE;
    int i, j;
    int color_num = 0;
    Uint32* pixels = NULL;
    Uint32 pixel_key;

    if (!(my_bmp->flags & MYBMP_TRANSPARENT))
        goto free_and_ret;

    if (my_bmp->depth <= 1 || my_bmp->depth > 8)
        goto free_and_ret;

    color_num = 1 << my_bmp->depth;
    pixels = malloc (color_num * sizeof (Uint32));
    for (i = 0; i < color_num; i++)
        pixels[i] = GAL_MapRGB (format, pal[i].r, pal[i].g, pal[i].b);
    pixel_key = pixels [my_bmp->transparent];

    for (i = 0; i < color_num; i++) {
        if (pixels[i] == pixel_key && i != my_bmp->transparent)
                break;
    }

    if (i == color_num)
        goto free_and_ret;

    srand (__mg_os_get_random_seed ());
    /* we try 256 times. */
    for (j = 0; j < 256; j++) {
        trans->r = rand () % 256;
        trans->g = rand () % 256;
        trans->b = rand () % 256;
        pixel_key = GAL_MapRGB (format, trans->r, trans->g, trans->b);

        for (i = 0; i < color_num; i++) {
            if (pixels[i] == pixel_key)
                break;
        }

        if (i == color_num) {
            ret = TRUE;
            break;
        }
    }

free_and_ret:
    if (pixels)
        free (pixels);

    return ret;
}

inline static BOOL device_has_alpha(HDC hdc)
{
    PDC pdc = dc_HDC2PDC(hdc);
    if (pdc->surface->format->Amask) {
        int max = MAX(MAX(pdc->surface->format->Rloss,
                    pdc->surface->format->Gloss), pdc->surface->format->Bloss);
        if (pdc->surface->format->Aloss > max) {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

/* Initializes a bitmap object with a mybitmap object */
static int init_bitmap_from_mybmp (HDC hdc, PBITMAP bmp, 
                const MYBITMAP* my_bmp, RGB* pal, BOOL alloc_all)
{
    int ret = ERR_BMP_OK;
    unsigned int size;
    Uint32 alpha_pitch;
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    bmp->bmBitsPerPixel = pdc->surface->format->BitsPerPixel;
    bmp->bmBytesPerPixel = pdc->surface->format->BytesPerPixel;
    bmp->bmAlphaMask = NULL;
    bmp->bmAlphaPitch = 0;

    size = GAL_GetBoxSize (pdc->surface, my_bmp->w, my_bmp->h, &bmp->bmPitch);

    if (!(bmp->bmBits = malloc (alloc_all?size:bmp->bmPitch))) {
        ret = ERR_BMP_MEM;
        goto cleanup_and_ret;
    }

    bmp->bmWidth = my_bmp->w;
    bmp->bmHeight = my_bmp->h;

    bmp->bmType = BMP_TYPE_NORMAL;
    if (my_bmp->flags & MYBMP_TRANSPARENT) {
        RGB trans;

        bmp->bmType |= BMP_TYPE_COLORKEY;
        if (pal && my_bmp->depth <= 8) {
            if (find_color_key (pdc->surface->format, my_bmp, pal, &trans))
                pal [my_bmp->transparent] = trans;
            else
                trans = pal [my_bmp->transparent];
        }
        else {
            if ((my_bmp->flags & MYBMP_TYPE_MASK) == MYBMP_TYPE_BGR) {
                trans.b = GetRValue (my_bmp->transparent);
                trans.g = GetGValue (my_bmp->transparent);
                trans.r = GetBValue (my_bmp->transparent);
            }
            else {
                trans.r = GetRValue (my_bmp->transparent);
                trans.g = GetGValue (my_bmp->transparent);
                trans.b = GetBValue (my_bmp->transparent);
            }
        }
        bmp->bmColorKey = GAL_MapRGB (pdc->surface->format, 
                        trans.r, trans.g, trans.b);
    }
    else
        bmp->bmColorKey = 0;

    if (my_bmp->flags & MYBMP_ALPHACHANNEL) {
        bmp->bmType |= BMP_TYPE_ALPHACHANNEL;
        bmp->bmAlpha = my_bmp->alpha;
    }
    else
        bmp->bmAlpha = 0;

    if (my_bmp->flags & MYBMP_ALPHA && my_bmp->depth == 32) {
        bmp->bmType |= BMP_TYPE_ALPHA;
        if (!device_has_alpha(hdc)) {
            bmp->bmType |= BMP_TYPE_ALPHA_MASK;
            alpha_pitch = (bmp->bmWidth + 3) & (~3);
            
            if (alloc_all) {
                size = bmp->bmHeight * alpha_pitch;
            } else {
                size = alpha_pitch;
            }
            bmp->bmAlphaMask = calloc(1, size);
            bmp->bmAlphaPitch = alpha_pitch;
            
            if (bmp->bmAlphaMask == NULL) {
                ret = ERR_BMP_MEM;
                goto cleanup_and_ret;
            }
        }
    }

    return ERR_BMP_OK;

cleanup_and_ret:
    if (ret != ERR_BMP_OK) {
        DeleteBitmapAlphaPixel (bmp);
        if (bmp->bmBits) {
            free (bmp->bmBits);
            bmp->bmBits = NULL;
        }
    }

    return ret;
}

static void compilergba_bitmap_sl (HDC hdc, BYTE* bits, BYTE* alpha_mask, MYBITMAP* my_bmp)
{
    GAL_Color rgb;
    Uint32 pixel;
    DWORD flags;
    PDC pdc;
    BYTE *src, *dst;
    int x = 0, w = 0;
    int bpp;

    pdc = dc_HDC2PDC(hdc);
    bpp = GAL_BytesPerPixel (pdc->surface);
    dst = bits;
    src = my_bmp->bits;
    flags = my_bmp->flags;
    w = my_bmp->w;

    for (x = 0; x < w; x++) {
        if ((flags & MYBMP_TYPE_MASK) == MYBMP_TYPE_BGR) {
            rgb.b = *src++;
            rgb.g = *src++;
            rgb.r = *src++;
        }
        else {
            rgb.r = *src++;
            rgb.g = *src++;
            rgb.b = *src++;
        }

        if (flags & MYBMP_RGBSIZE_4) {
            if (flags & MYBMP_ALPHA) {
                rgb.a = *src;
                if (alpha_mask) {
                    alpha_mask[x] = rgb.a;
                    pixel = GAL_MapRGB (pdc->surface->format,
                            rgb.r, rgb.g, rgb.b);
                } else {
                    pixel = GAL_MapRGBA (pdc->surface->format,
                            rgb.r, rgb.g, rgb.b, rgb.a);
                }
            } else {
                pixel = GAL_MapRGB (pdc->surface->format, rgb.r, rgb.g, rgb.b);
            }
            src++;
        }
        else {
            pixel = GAL_MapRGB (pdc->surface->format, rgb.r, rgb.g, rgb.b);
        }

        dst = _mem_set_pixel (dst, bpp, pixel);

    }

}

/*
 * This function expand a mybitmap to a compiled bitmap.
 */
int GUIAPI ExpandMyBitmap (HDC hdc, PBITMAP bmp, const MYBITMAP* my_bmp, 
                           const RGB* pal, int frame)
{
    Uint8* bits;
    PDC pdc;
    RGB *new_pal = NULL;
    int ret;
    BYTE *alpha_mask = NULL;

    pdc = dc_HDC2PDC (hdc);

    if (pal && my_bmp->depth <= 8) {
        int nr_colors = 1 << my_bmp->depth;

        new_pal = calloc (nr_colors, sizeof (RGB));
        if (new_pal == NULL)
            return ERR_BMP_MEM;

        memcpy (new_pal, pal, sizeof (RGB) * nr_colors);
    }

    ret = init_bitmap_from_mybmp (hdc, bmp, my_bmp, new_pal, TRUE);
    if (ret) return ret;

    pal = new_pal;

    if (frame <= 0 || frame >= my_bmp->frames)
        bits = my_bmp->bits;
    else
        bits = my_bmp->bits + frame * my_bmp->size;

    if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
        alpha_mask= bmp->bmAlphaMask;
    }

    switch (my_bmp->depth) {
    case 1:
        ExpandMonoBitmap (hdc, bmp->bmBits, bmp->bmPitch, bits, my_bmp->pitch, 
                        my_bmp->w, my_bmp->h, my_bmp->flags, 
                        GAL_MapRGB (pdc->surface->format, 
                                pal[0].r, pal[0].g, pal[0].b),
                        GAL_MapRGB (pdc->surface->format, 
                                pal[1].r, pal[1].g, pal[1].b));
        break;
    case 4:
        Expand16CBitmap (hdc, bmp->bmBits, bmp->bmPitch, bits, my_bmp->pitch, 
                        my_bmp->w, my_bmp->h, my_bmp->flags, pal);
        break;
    case 8:
        Expand256CBitmap (hdc, bmp->bmBits, bmp->bmPitch, bits, my_bmp->pitch, 
                        my_bmp->w, my_bmp->h, my_bmp->flags, pal, NULL, NULL);
        break;
    case 24:
        CompileRGBABitmap (hdc, bmp->bmBits, bmp->bmPitch, bits, my_bmp->pitch, 
                        my_bmp->w, my_bmp->h, my_bmp->flags & ~MYBMP_RGBSIZE_4, 
                        pdc->surface->format, NULL, NULL);
        break;
    case 32:
        CompileRGBABitmapEx (hdc, bmp->bmBits, bmp->bmPitch, bits, my_bmp->pitch, 
                        my_bmp->w, my_bmp->h, my_bmp->flags | MYBMP_RGBSIZE_4, 
                        pdc->surface->format, NULL, NULL, alpha_mask);
        break;
    default:
        ret = ERR_BMP_NOT_SUPPORTED;
        break;
    }

    if (new_pal) {
        free (new_pal);
    }

    if (ret) {
        DeleteBitmapAlphaPixel (bmp);
        if (bmp->bmBits) {
            free (bmp->bmBits);
            bmp->bmBits = NULL;
        }
    }

    return ret;
}

typedef struct {
    HDC hdc;
    BITMAP* bmp;
    RGB* pal;
} LOAD_BITMAP_INFO;

static void cb_load_bitmap_sl (void* context, MYBITMAP* my_bmp, int y)
{
    PDC pdc;
    LOAD_BITMAP_INFO* info = (LOAD_BITMAP_INFO*)context;
    Uint8 *src_bits, *dst_bits, *dst_alpha_mask;

    pdc = dc_HDC2PDC (info->hdc);
    src_bits = my_bmp->bits;
    dst_bits = info->bmp->bmBits + info->bmp->bmPitch * y;
    
    if (info->bmp->bmType & BMP_TYPE_ALPHA_MASK) {
        dst_alpha_mask= info->bmp->bmAlphaMask + info->bmp->bmAlphaPitch * y;
    } else {
        dst_alpha_mask = NULL;
    }

    switch (my_bmp->depth) {
    case 1:
        ExpandMonoBitmap (info->hdc, dst_bits, info->bmp->bmPitch, 
                        src_bits, my_bmp->pitch, my_bmp->w, 1, my_bmp->flags, 
                        GAL_MapRGB (pdc->surface->format, 
                            info->pal[0].r, info->pal[0].g, info->pal[0].b),
                        GAL_MapRGB (pdc->surface->format, 
                            info->pal[1].r, info->pal[1].g, info->pal[1].b));
        break;
    case 4:
        Expand16CBitmap (info->hdc, dst_bits, info->bmp->bmPitch, 
                        src_bits, my_bmp->pitch, 
                        my_bmp->w, 1, my_bmp->flags, info->pal);
        break;
    case 8:
        Expand256CBitmap (info->hdc, dst_bits, info->bmp->bmPitch, 
                        src_bits, my_bmp->pitch, 
                        my_bmp->w, 1, my_bmp->flags, info->pal, NULL, NULL);
        break;
    case 24:
        CompileRGBABitmap (info->hdc, dst_bits, info->bmp->bmPitch, 
                        src_bits, my_bmp->pitch, 
                        my_bmp->w, 1, my_bmp->flags & ~MYBMP_RGBSIZE_4, 
                        pdc->surface->format, NULL, NULL);
        break;
    case 32:
        compilergba_bitmap_sl (info->hdc, dst_bits, dst_alpha_mask, my_bmp);
        break;
    default:
        break;
    }
}

int GUIAPI LoadBitmapEx (HDC hdc, PBITMAP bmp, MG_RWops* area, const char* ext)
{
    MYBITMAP my_bmp;
    RGB pal [256];
    int ret;
    void* load_info;
    LOAD_BITMAP_INFO info;

    info.hdc = hdc;
    info.bmp = bmp;
    info.pal = pal;

    load_info = InitMyBitmapSL (area, ext, &my_bmp, pal);
    if (load_info == NULL) {
        return ERR_BMP_IMAGE_TYPE;
    }

    ret = init_bitmap_from_mybmp (hdc, bmp, &my_bmp, pal, TRUE);
    if (ret) {
        CleanupMyBitmapSL (&my_bmp, load_info);
        return ret;
    }

    ret = LoadMyBitmapSL (area, load_info, &my_bmp, cb_load_bitmap_sl, &info);

    CleanupMyBitmapSL (&my_bmp, load_info);

    return ret;
}

int GUIAPI LoadBitmapFromFile (HDC hdc, PBITMAP bmp, const char* file_name)
{
    int ret;
    MG_RWops* area;
    const char* ext;
    
    /* format, png, jpg etc. */
    if ((ext = get_extension (file_name)) == NULL) {
        return ERR_BMP_UNKNOWN_TYPE;
    }

    if (!(area = MGUI_RWFromFile (file_name, "rb"))) {
        fprintf(stderr, "LoadBitmapFromFile:%d error!\n", ERR_BMP_FILEIO);
        return ERR_BMP_FILEIO;
    }

    ret = LoadBitmapEx (hdc, bmp, area, ext);

    MGUI_RWclose (area);

    return ret;
}

int GUIAPI LoadBitmapFromMem (HDC hdc, PBITMAP bmp, const void* mem, int size, 
                              const char* ext)
{
    int ret;
    MG_RWops* area;

    if (!(area = MGUI_RWFromMem ((void*)mem, size))) {
        return ERR_BMP_MEM;
    }

    ret = LoadBitmapEx (hdc, bmp, area, ext);

    MGUI_RWclose (area);

    return ret;
}

/* this function delete the pixel format of a bitmap */
void GUIAPI DeleteBitmapAlphaPixel (PBITMAP bmp)
{
	if (bmp->bmType & BMP_TYPE_ALPHA_MASK) {
       free (bmp->bmAlphaMask);
	   bmp->bmType &= ~BMP_TYPE_ALPHA_MASK;
	   bmp->bmAlphaMask = NULL;
	}
}

/* this function unloads bitmap */
void GUIAPI UnloadBitmap (PBITMAP bmp)
{
    DeleteBitmapAlphaPixel (bmp);

    free (bmp->bmBits);
    bmp->bmBits = NULL;
}

typedef struct {
    HDC hdc;
    PDC pdc;
    BITMAP* bmp;
    RGB* pal;
    FILLINFO fill_info;
} FILL_BITMAP_INFO;

static void cb_paint_image_sl (void* context, MYBITMAP* my_bmp, int y)
{
    FILL_BITMAP_INFO* info = (FILL_BITMAP_INFO*)context;
    Uint8 *src_bits, *dst_bits;
    PDC pdc = dc_HDC2PDC(info->hdc);

    src_bits = my_bmp->bits;
    dst_bits = info->bmp->bmBits;


    switch (my_bmp->depth) {
    case 1:
        ExpandMonoBitmap (info->hdc, dst_bits, info->bmp->bmPitch, 
                        src_bits, my_bmp->pitch, my_bmp->w, 1, my_bmp->flags, 
                        GAL_MapRGB (info->pdc->surface->format, 
                            info->pal[0].r, info->pal[0].g, info->pal[0].b),
                        GAL_MapRGB (info->pdc->surface->format, 
                            info->pal[1].r, info->pal[1].g, info->pal[1].b));
        break;
    case 4:
        Expand16CBitmap (info->hdc, dst_bits, info->bmp->bmPitch, 
                        src_bits, my_bmp->pitch, 
                        my_bmp->w, 1, my_bmp->flags, info->pal);
        break;
    case 8:
        Expand256CBitmap (info->hdc, dst_bits, info->bmp->bmPitch, 
                        src_bits, my_bmp->pitch, 
                        my_bmp->w, 1, my_bmp->flags, info->pal, NULL, NULL);
        break;
    case 24:
        CompileRGBABitmap (info->hdc, dst_bits, info->bmp->bmPitch, 
                        src_bits, my_bmp->pitch, 
                        my_bmp->w, 1, my_bmp->flags & ~MYBMP_RGBSIZE_4, 
                        pdc->surface->format, NULL, NULL);
        break;
    case 32:
     	compilergba_bitmap_sl (info->hdc, dst_bits, info->bmp->bmAlphaMask, my_bmp);
        break;
    default:
        break;
    }

    _fill_bitmap_scanline (info->pdc, info->bmp, &info->fill_info, y);
}

int GUIAPI PaintImageEx (HDC hdc, int x, int y, MG_RWops* area, const char *ext)
{
    MYBITMAP my_bmp;
    BITMAP bmp;
    RGB pal[256];
    int ret;
    void* load_info;
    FILL_BITMAP_INFO info;
        
    info.hdc = hdc;
    info.pdc = NULL;
    info.bmp = &bmp;
    info.pal = (void*) pal;

    load_info = InitMyBitmapSL (area, ext, &my_bmp, pal);
    if (load_info == NULL) {
        return ERR_BMP_IMAGE_TYPE;
    }

    ret = init_bitmap_from_mybmp (hdc, &bmp, &my_bmp, pal, FALSE);
    if (ret)
        goto fail1;

    info.pdc = _begin_fill_bitmap (hdc, x, y, 0, 0, &bmp, &info.fill_info);
    if (info.pdc == NULL) {
        ret = ERR_BMP_OTHER;
        goto fail2;
    }

    bmp.bmHeight = 1;
    ret = LoadMyBitmapSL (area, load_info, &my_bmp, cb_paint_image_sl, &info);

    _end_fill_bitmap (info.pdc, &bmp, &info.fill_info);

fail2:
    UnloadBitmap (&bmp);
fail1:
    CleanupMyBitmapSL (&my_bmp, load_info);

    return ret;
}

int GUIAPI PaintImageFromFile (HDC hdc, int x, int y, const char *file_name)
{
    int ret;
    MG_RWops* area;
    const char* ext;

    if ((ext = get_extension (file_name)) == NULL) 
        return ERR_BMP_UNKNOWN_TYPE;

    if (!(area = MGUI_RWFromFile (file_name, "rb"))) {
        return ERR_BMP_FILEIO;
    }

    ret = PaintImageEx (hdc, x, y, area, ext);

    MGUI_RWclose (area);

    return ret;
}

int GUIAPI PaintImageFromMem (HDC hdc, int x, int y, const void* mem, 
                int size, const char *ext)
{
    int ret;
    MG_RWops* area;

    if (!(area = MGUI_RWFromMem ((void*)mem, size))) {
        return ERR_BMP_MEM;
    }

    ret = PaintImageEx (hdc, x, y, area, ext);

    MGUI_RWclose (area);

    return ret;
}

typedef struct {
    HDC hdc;
    BITMAP* bmp;
    RGB* pal;
    FILLINFO fill_info;
    
    PDC pdc;
    int *dda_ys;
    Uint8* expanded_line;
	Uint8* expanded_alpha_line;
    int expanded_len;
} STRETCH_PAINT_IMAGE_INFO;

static void _cb_stretch_paint_image_sl (void* context, MYBITMAP* my_bmp, int yy)
{
    STRETCH_PAINT_IMAGE_INFO* info = (STRETCH_PAINT_IMAGE_INFO*)context;
    BYTE* expanded_line;
	BYTE* expanded_alpha_line;
    int expanded_len;
    int nr_ident_lines;

    if (info->dda_ys 
                    && (info->dda_ys [yy + 1] - info->dda_ys [yy]) == 0 
                    && yy != 0)
        return;

    if (info->expanded_line) {
        expanded_line = info->expanded_line;
        expanded_len = info->expanded_len;
		expanded_alpha_line = info->expanded_alpha_line;
    }
    else {
        expanded_line = info->bmp->bmBits;
        expanded_len = info->bmp->bmPitch;
		expanded_alpha_line = info->bmp->bmAlphaMask;
    }

    /* expand the line into expanded_line first */
    switch (my_bmp->depth) {
    case 1:
        ExpandMonoBitmap (info->hdc, expanded_line, expanded_len,
                        my_bmp->bits, my_bmp->pitch, my_bmp->w, 1, 
                        my_bmp->flags, 
                        GAL_MapRGB (info->pdc->surface->format, 
                            info->pal[0].r, info->pal[0].g, info->pal[0].b),
                        GAL_MapRGB (info->pdc->surface->format, 
                            info->pal[1].r, info->pal[1].g, info->pal[1].b));
        break;

    case 4:
        Expand16CBitmap (info->hdc, expanded_line, expanded_len, 
                        my_bmp->bits, my_bmp->pitch, 
                        my_bmp->w, 1, my_bmp->flags, info->pal);
        break;

    case 8:
        Expand256CBitmap (info->hdc, expanded_line, expanded_len,
                        my_bmp->bits, my_bmp->pitch, 
                        my_bmp->w, 1, my_bmp->flags, info->pal, NULL, NULL);
        break;

    case 24:
        CompileRGBABitmap (info->hdc, expanded_line, expanded_len,
                        my_bmp->bits, my_bmp->pitch, 
                        my_bmp->w, 1, my_bmp->flags & ~MYBMP_RGBSIZE_4, 
                        info->pdc->surface->format, NULL, NULL);
        break;

    case 32:
    	compilergba_bitmap_sl (info->hdc, expanded_line, expanded_alpha_line, my_bmp);
        break;

    default:
        break;
    }

    if (info->expanded_line) {
        int x, sx, xfactor;
        int index;
        BYTE *dp1, *dp2, *dp3;

        /* scaled by 65536 */
        xfactor = (int)(((Sint64) (my_bmp->w * 65536)) / info->bmp->bmWidth);

        /* scale the line */
        x = 0; sx = 0;
        dp1 = info->expanded_line;
        dp2 = info->bmp->bmBits;
		dp3 = info->expanded_alpha_line;

        switch (info->bmp->bmBytesPerPixel) {
        case 1:
            {
                while (x < info->bmp->bmWidth) {
                    index = sx >> 16;
                    *(dp2 + x) = *(dp1 + index);
                    if (info->bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        info->bmp->bmAlphaMask[x] = dp3[index];
                    }
                    sx += xfactor;
                    x++;
                }
            }
        break;
    
        case 2:
            {
                while (x < info->bmp->bmWidth) {
                    index = (sx >> 16) * 2;
                    *(unsigned short *) (dp2 + x * 2) 
                            = *(unsigned short *) (dp1 + index);
                    if (info->bmp->bmType & BMP_TYPE_ALPHA_MASK) {
						info->bmp->bmAlphaMask[x] = dp3[index >> 1];
					}
                    sx += xfactor;
                    x++;
                }
            }
        break;
        case 3:
            {
                while (x < info->bmp->bmWidth) {
                    index = (sx >> 16) * 3;
#if 0
                    *(unsigned short *) (dp2 + x * 3) 
                            = *(unsigned short *) (dp1 + (sx >> 16) * 3);
                    *(unsigned char *) (dp2 + x * 3 + 2) 
                            = *(unsigned char *) (dp1 + (sx >> 16) * 3 + 2);
#else
                    dp2 [x*3] = dp1 [index];
                    dp2 [x*3 + 1] = dp1 [index + 1];
                    dp2 [x*3 + 2] = dp1 [index + 2];
#endif
                    if (info->bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        info->bmp->bmAlphaMask[x] = dp3[index/3];
                    }
                    sx += xfactor;
                    x++;
                }
            }
        break;
        case 4:
            {
                while (x < info->bmp->bmWidth) {
                    index = (sx >> 16) * 4;
                    *(unsigned *) (dp2 + x * 4) 
                            = *(unsigned *) (dp1 + index);
                    if (info->bmp->bmType & BMP_TYPE_ALPHA_MASK) {
                        info->bmp->bmAlphaMask[x] = dp3[index >> 2];
                    }
                    sx += xfactor;
                    x++;
                }
            }
        break;
        }
    }
    
    /* ok to output the lines */
    if (info->dda_ys) {
        if (info->dda_ys [yy] < info->fill_info.dst_rect.h) {
       	    int i = 0;
            nr_ident_lines = info->dda_ys [yy + 1] - info->dda_ys [yy];
            for (i = 0; i < nr_ident_lines; i++) {
                _fill_bitmap_scanline (info->pdc, info->bmp, &info->fill_info, 
                                info->dda_ys [yy] + i);
            }
        }
    }
    else
        _fill_bitmap_scanline (info->pdc, info->bmp, &info->fill_info, yy);
}

static void _deinit_stretch_paint_info (STRETCH_PAINT_IMAGE_INFO* info)
{
    free (info->dda_ys);
    free (info->expanded_line);
    if (info->expanded_alpha_line)
        free (info->expanded_alpha_line);
}

static void _init_stretch_paint_info (STRETCH_PAINT_IMAGE_INFO* info, 
                const MYBITMAP* mybmp, const BITMAP* bmp)
{
    if (mybmp->w == bmp->bmWidth) {
        info->expanded_len = 0;
        info->expanded_line = NULL;
        info->expanded_alpha_line = NULL;
    }
    else {
        info->expanded_len = bmp->bmBytesPerPixel * mybmp->w;
        info->expanded_line = malloc (info->expanded_len);
        if (bmp->bmType & BMP_TYPE_ALPHA_MASK)
            info->expanded_alpha_line = malloc (info->expanded_len / bmp->bmBytesPerPixel);
        else
            info->expanded_alpha_line = NULL;
    }

    if (mybmp->h == bmp->bmHeight) {
        info->dda_ys = NULL;
    }
    else {
        int sy, yy, dy;
        int yfactor;
        int last_y;

        /* scaled by 65536 */
        yfactor = (int)(((Sint64) (mybmp->h * 65536)) / bmp->bmHeight);

        info->dda_ys = calloc (mybmp->h + 1, sizeof(int));
        if (info->dda_ys == NULL) {
            return;
        }

        sy = 0; yy = 0;
        for (dy = 0; dy < bmp->bmHeight;) {

            info->dda_ys [yy] = dy;
            dy++;
            while (dy < bmp->bmHeight) {
                int syint = sy >> 16;
                sy += yfactor;
    
                if ((sy >> 16) != syint)
                    break;
    
                info->dda_ys [yy] = dy;
                dy++;
            }
            yy = (sy >> 16);
        }

        info->dda_ys [0] = 0;
        info->dda_ys [mybmp->h] = bmp->bmHeight;

        last_y = 0;
        for (sy = 0; sy < mybmp->h; sy++) {
            if (info->dda_ys [sy] == 0) {
                info->dda_ys [sy] = last_y;
            }
            else {
                last_y = info->dda_ys [sy];
            }
        }
    }

}

int GUIAPI StretchPaintImageEx (HDC hdc, int x, int y, int w, int h, 
                MG_RWops* area, const char *ext)
{
    MYBITMAP my_bmp;
    BITMAP bmp;
    RGB pal[256];
    int ret;
    void* load_info;
    Uint32 tmp_mybmp_w, tmp_mybmp_h;
    STRETCH_PAINT_IMAGE_INFO info;
    info.hdc = hdc;
    info.bmp = &bmp;
    info.pal = pal;

    if (w <= 0 || h <= 0)
        return ERR_BMP_OK;

    load_info = InitMyBitmapSL (area, ext, &my_bmp, pal);
    if (load_info == NULL) {
        return ERR_BMP_IMAGE_TYPE;
    }

    tmp_mybmp_w = my_bmp.w;
    tmp_mybmp_h = my_bmp.h;

    my_bmp.w = w; my_bmp.h = h;
    ret = init_bitmap_from_mybmp (hdc, &bmp, &my_bmp, pal, FALSE);
    my_bmp.w = tmp_mybmp_w; my_bmp.h = tmp_mybmp_h;

    if (ret)
        goto fail1;

    _init_stretch_paint_info (&info, &my_bmp, &bmp);

    info.pdc = _begin_fill_bitmap (hdc, x, y, 0, 0, &bmp, &info.fill_info);
    if (info.pdc == NULL) {	    
        free (info.dda_ys);
        ret = ERR_BMP_OTHER;
        goto fail2;
    }

    bmp.bmHeight = 1;
    ret = LoadMyBitmapSL (area, load_info, &my_bmp, 
                    _cb_stretch_paint_image_sl, &info);

    _end_fill_bitmap (info.pdc, &bmp, &info.fill_info);

    _deinit_stretch_paint_info (&info);

fail2:
    UnloadBitmap (&bmp);
fail1:
    CleanupMyBitmapSL (&my_bmp, load_info);
    return ret;
}

int GUIAPI StretchPaintImageFromFile (HDC hdc, int x, int y, int w, int h, 
                const char *file_name)
{
    int ret;
    MG_RWops* area;
    const char* ext;

    if ((ext = get_extension (file_name)) == NULL) 
        return ERR_BMP_UNKNOWN_TYPE;

    if (!(area = MGUI_RWFromFile (file_name, "rb"))) {
        return ERR_BMP_FILEIO;
    }

    ret = StretchPaintImageEx (hdc, x, y, w, h, area, ext);

    MGUI_RWclose (area);

    return ret;
}

int GUIAPI StretchPaintImageFromMem (HDC hdc, int x, int y, int w, int h, 
                const void* mem, int size, const char *ext)
{
    int ret;
    MG_RWops* area;

    if (!(area = MGUI_RWFromMem ((void*)mem, size))) {
        return ERR_BMP_MEM;
    }

    ret = StretchPaintImageEx (hdc, x, y, w, h, area, ext);

    MGUI_RWclose (area);

    return ret;
}

#ifdef  _FILL_MYBITMAP
static BYTE*  cb_directdraw_pixel(HDC hdc, MYBITMAP_CONTXT* mybmp, Uint32 pixel, BYTE* dest)
{
    PDC pdc =dc_HDC2PDC(hdc);
    GAL_Surface *dst =pdc->surface;
    MYBITMAP* my_bmp =mybmp->mybmp;

    if (pdc->rop == ROP_SET) {
        if (mybmp->AlphaPixelFormat != NULL 
                && mybmp->AlphaPixelFormat != dst->format) {
            if ((my_bmp->flags & MYBMP_ALPHA && my_bmp->depth == 32) && dst->format->BitsPerPixel > 8
                    && (my_bmp->flags & MYBMP_TRANSPARENT)) {
                dest = gal_PutPixelKeyAlpha (dst, dest, pixel, mybmp);
                return dest;
            }
            else if ((my_bmp->flags & MYBMP_ALPHA && my_bmp->depth == 32) && 
                    dst->format->BitsPerPixel > 8) {
                dest = gal_PutPixelAlpha (dst, dest, pixel, mybmp);
                return dest;
            }
        }
        else if ((my_bmp->flags & MYBMP_ALPHACHANNEL) && (my_bmp->flags & MYBMP_TRANSPARENT)) {
            dest = gal_PutPixelKeyAlphaChannel (dst, dest, pixel, mybmp);
            return dest;
        }
        else if (my_bmp->flags & MYBMP_TRANSPARENT) {
            dest = gal_PutPixelKey (dst, dest, pixel, mybmp);
            return dest;
        }
        else if (my_bmp->flags & MYBMP_ALPHACHANNEL) {
            dest = gal_PutPixelAlphaChannel (dst, dest, pixel, mybmp);
            return dest;
        }

        dest =_mem_set_pixel (dest, dst->format->BytesPerPixel, pixel);
    }
    else{
        pdc->draw_src_span (PDC_TO_COMP_CTXT(pdc),
                (gal_uint8*)&pixel, pdc->bkmode, 1);
        dest +=dst->format->BytesPerPixel;
    }

    return dest;
}

static void cb_direct_paint_mybmp_sl (void* context, int stepx, 
        int y, MYBITMAP_CONTXT* mybmp_contxt)
{
    FILL_BITMAP_INFO* info = (FILL_BITMAP_INFO*)context;
    Uint8 *src_bits, *dst_bits;
    //GAL_Rect dst_rect,my_dstrect;
    PDC  pdc;
    MYBITMAP *my_bmp =mybmp_contxt->mybmp;
    //Uint32  off_x, off_y;

    pdc =info->pdc;
    /*dst_rect.x = info->fill_info.dst_rect.x;
    dst_rect.y = info->fill_info.dst_rect.y +y;
    dst_rect.w = info->fill_info.dst_rect.w;
    dst_rect.h = 1;

    if (!GAL_IntersectRect (&dst_rect, &pdc->dst->clip_rect, &my_dstrect)) {
        return ;
    }

    off_x =my_dstrect.x -dst_rect.x;
    off_y =my_dstrect.y -dst_rect.y;*/
    if (my_bmp->flags & MYBMP_FLOW_UP){
        src_bits = my_bmp->bits +my_bmp->pitch *(my_bmp->h -y -1);
    }
    else{
        src_bits = my_bmp->bits +my_bmp->pitch *y;
    }
    dst_bits = pdc->cur_dst;

    switch(my_bmp->depth){
        case 1:
            ExpandPartMonoBitmap (info->hdc, dst_bits, pdc->surface->pitch, 
                    src_bits, my_bmp->pitch, pdc->surface->clip_rect.w, 1, 
                    my_bmp->flags, GAL_MapRGB (pdc->surface->format, 
                        info->pal[0].r, info->pal[0].g, info->pal[0].b),
                    GAL_MapRGB (pdc->surface->format, info->pal[1].r, 
                        info->pal[1].g, info->pal[1].b),stepx,cb_directdraw_pixel,
                    mybmp_contxt);
            break;
        case 4:
            ExpandPart16CBitmap (info->hdc, dst_bits, pdc->surface->pitch, 
                    src_bits, my_bmp->pitch, pdc->surface->clip_rect.w,1, 
                    my_bmp->flags, info->pal, stepx,cb_directdraw_pixel,mybmp_contxt);
            break;
        case 8:
            src_bits +=stepx;
            Expand256CBitmap (info->hdc, dst_bits, pdc->surface->pitch, src_bits, my_bmp->pitch, 
                        pdc->surface->clip_rect.w, 1, my_bmp->flags, 
                        info->pal,cb_directdraw_pixel,mybmp_contxt);
            break;
        case 24:
            src_bits +=stepx*3;
            CompileRGBABitmap (info->hdc, dst_bits, pdc->surface->pitch, src_bits, my_bmp->pitch, 
                        pdc->surface->clip_rect.w, 1, my_bmp->flags & ~MYBMP_RGBSIZE_4, 
                        mybmp_contxt->AlphaPixelFormat,cb_directdraw_pixel,mybmp_contxt);
            break;
        case 32:
            src_bits +=stepx*4;
            CompileRGBABitmap (info->hdc, dst_bits, pdc->surface->pitch, src_bits, my_bmp->pitch, 
                    pdc->surface->clip_rect.w, 1, my_bmp->flags |MYBMP_RGBSIZE_4, 
                    mybmp_contxt->AlphaPixelFormat,cb_directdraw_pixel,mybmp_contxt);
    }
}

int GUIAPI FillBoxWithMyBitmap (HDC hdc, int x, int y, MYBITMAP* mybmp, RGB* pal)
{
    int i = 0,right,bottom,w,h,oldbkmode,stepy,stepx;
    RECT rc_tmp;
    GAL_Rect dst_rect;
    PCLIPRECT cliprect;
    RECT eff_rc;
    PDC pdc;
    RGB lpal[256];
    Uint32  colorKey,pitch;
    GAL_PixelFormat* AlphaPixelFormat =NULL;
    FILL_BITMAP_INFO info;
    //const BYTE* bits =NULL;
    MYBITMAP_CONTXT mybmp_contxt;

    if (!mybmp || mybmp->w == 0 || mybmp->h == 0 || mybmp->bits == NULL)
        return -1;

    pdc = dc_HDC2PDC (hdc);
    rc_tmp = pdc->rc_output;

    /* houhh 20081204, check dc's ecrgn first.*/
    if (!(pdc = __mg_check_ecrgn (hdc))) {
        return -1;
    }

    if (mybmp->depth <= 8 && !pal)
        GetPalette (HDC_SCREEN, 0, 256, (GAL_Color*)lpal);
    if(!pal)
        info.pal = (void*) lpal;
    else
        info.pal =(void*) pal;

    GAL_GetBoxSize (pdc->surface, mybmp->w, mybmp->h, &pitch);

    if (mybmp->flags & MYBMP_TRANSPARENT) {
        RGB trans;

        //bmp->bmType |= BMP_TYPE_COLORKEY;
        if (info.pal && mybmp->depth <= 8) {
            if (find_color_key (pdc->surface->format, mybmp, info.pal, &trans))
                info.pal [mybmp->transparent] = trans;
            else
                trans = info.pal [mybmp->transparent];
        }
        else {
            if ((mybmp->flags & MYBMP_TYPE_MASK) == MYBMP_TYPE_BGR) {
                trans.b = GetRValue (mybmp->transparent);
                trans.g = GetGValue (mybmp->transparent);
                trans.r = GetBValue (mybmp->transparent);
            }
            else {
                trans.r = GetRValue (mybmp->transparent);
                trans.g = GetGValue (mybmp->transparent);
                trans.b = GetBValue (mybmp->transparent);
            }
        }
        colorKey = GAL_MapRGB (pdc->surface->format, 
                trans.r, trans.g, trans.b);
    }
    else
        colorKey = 0;

    if (mybmp->flags & MYBMP_ALPHA && mybmp->depth == 32) {
        Uint32 Rmask = 0, Gmask = 0, Bmask = 0, Amask = 0;
        GAL_PixelFormat * format = pdc->surface->format;

        if (pdc->surface->format->Amask) {
            Amask = format->Amask;
            Rmask = format->Rmask;
            Gmask = format->Gmask;
            Bmask = format->Bmask;
        }
        else switch (pdc->surface->format->BytesPerPixel) {
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
        }

        if (pdc->surface->format->BitsPerPixel > 8) {

            AlphaPixelFormat 
                    = GAL_AllocFormat (pdc->surface->format->BytesPerPixel << 3, 
                                        Rmask, Gmask, Bmask, Amask);
            if (!AlphaPixelFormat)
            {
                //ret = ERR_BMP_MEM;
                //goto cleanup_and_ret;
                goto fail;
            }

            //bmp->bmType |= BMP_TYPE_ALPHA;
            //bmp->bmType |= BMP_TYPE_PRIV_PIXEL;
        }
        else { /* for bpp <= 8, just strip alpha */
            AlphaPixelFormat = pdc->surface->format;
        }
    }
    else
        AlphaPixelFormat = NULL;

    mybmp_contxt.colorKey = colorKey;
    mybmp_contxt.pitch = pitch;
    mybmp_contxt.AlphaPixelFormat = AlphaPixelFormat;
    mybmp_contxt.mybmp = mybmp;


    /* Transfer logical to device to screen here. */
    right = x + mybmp->w; bottom = y + mybmp->h;
    coor_LP2SP (pdc, &x, &y);
    coor_LP2SP (pdc, &right, &bottom);
    SetRect (&pdc->rc_output, x, y, right, bottom);
    NormalizeRect (&pdc->rc_output);

    w = RECTW (pdc->rc_output); h = RECTH (pdc->rc_output);
    dst_rect.x = x; dst_rect.y = y; dst_rect.w = w; dst_rect.h = h;

    pdc->step = 1;
    pdc->cur_ban = NULL;
    pdc->cur_pixel = pdc->brushcolor;
    pdc->skip_pixel = colorKey;
    //fill_info->old_bkmode = pdc->bkmode;
    oldbkmode =pdc->bkmode;
    if (mybmp->flags & MYBMP_TRANSPARENT) {
        pdc->bkmode = BM_TRANSPARENT;
    }

    if(__mg_enter_drawing (pdc) < 0) {
        goto fail;
    }
    //bits = mybmp->bits;

    info.hdc = hdc;
    info.pdc = pdc;
    info.bmp = NULL;
    
    info.fill_info.dst_rect =dst_rect;
    cliprect = pdc->ecrgn.head;
    while(cliprect){
        if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)) {
            SET_GAL_CLIPRECT (pdc, eff_rc);
            stepy =0;
            stepx =eff_rc.left -dst_rect.x;
            for(i=(eff_rc.top -dst_rect.y);i < (eff_rc.bottom -dst_rect.y);i++){
                pdc->move_to(pdc,eff_rc.left,eff_rc.top + stepy);
                stepy ++;
                cb_direct_paint_mybmp_sl (&info,stepx, i,&mybmp_contxt);
            }
        }

        cliprect = cliprect->next;
    }

    pdc->bkmode =oldbkmode; 
    __mg_leave_drawing (pdc);
    pdc->rc_output = rc_tmp;
    UNLOCK_GCRINFO (pdc);

    return 0;

fail:
    pdc->rc_output = rc_tmp;
    UNLOCK_GCRINFO (pdc);

    return -1;
}
#endif

#ifdef _MGMISC_SAVEBITMAP
int GUIAPI SaveBitmapToFile (HDC hdc, PBITMAP bmp, const char* file_name)
{
    PDC pdc;
    MYBITMAP myBitmap;
    int i;
    RGB pal [256];
    GAL_Palette* palette;

    pdc = dc_HDC2PDC (hdc);
    palette = pdc->surface->format->palette;
    switch (GAL_BitsPerPixel (pdc->surface)) {
    case 4:
        for (i = 0; i < 16; i++) {
            pal [i].r = palette->colors [i].r;
            pal [i].g = palette->colors [i].g;
            pal [i].b = palette->colors [i].b;
        }
        break;
    case 8:
        for (i = 0; i < 256; i++) {
            pal [i].r = palette->colors [i].r;
            pal [i].g = palette->colors [i].g;
            pal [i].b = palette->colors [i].b;
        }
        break;
    }

    myBitmap.flags  = MYBMP_TYPE_NORMAL;
    myBitmap.frames = 1;
    myBitmap.depth  = GAL_BitsPerPixel (pdc->surface);
    myBitmap.w      = bmp->bmWidth;
    myBitmap.h      = bmp->bmHeight;
    myBitmap.pitch  = bmp->bmPitch;
    myBitmap.size   = myBitmap.h * bmp->bmPitch;
    myBitmap.bits   = bmp->bmBits;

    return SaveMyBitmapToFile (&myBitmap, pal, file_name);
}
#endif

int GUIAPI SetBitmapKeyColor (HDC hdc, PBITMAP bmp, Uint8 r, Uint8 g, Uint8 b)
{
    int ret = ERR_BMP_UNKNOWN_TYPE; 

	if (bmp) {
        PDC pdc = dc_HDC2PDC (hdc);
        bmp->bmColorKey = GAL_MapRGB (pdc->surface->format, r, g, b);

        bmp->bmType |= BMP_TYPE_COLORKEY;
        ret = ERR_BMP_OK;
	}		

    return ret;
}

