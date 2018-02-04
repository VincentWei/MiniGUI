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

/* General (mostly internal) pixel/color manipulation routines for GAL */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "newgal.h"
#include "sysvideo.h"
#include "blit.h"
#include "pixels_c.h"
#include "RLEaccel_c.h"

/* Helper functions */
/*
 * Allocate a pixel format structure and fill it according to the given info.
 */
GAL_PixelFormat *GAL_AllocFormat(int bpp,
            Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    GAL_PixelFormat *format;
    Uint32 mask;

    /* Allocate an empty pixel format structure */
    format = (GAL_PixelFormat*)calloc(1, sizeof(*format));//malloc(sizeof(*format));
    if ( format == NULL ) {
        GAL_OutOfMemory();
        return(NULL);
    }
    //memset(format, 0, sizeof(*format));
    format->alpha = GAL_ALPHA_OPAQUE;

    /* Set up the format */
    format->BitsPerPixel = bpp;
    format->BytesPerPixel = (bpp+7)/8;
    format->DitheredPalette = FALSE;
    switch (bpp) {
        case 1:
            /* Create the 2 color black-white palette */
            format->palette = (GAL_Palette *)malloc(
                            sizeof(GAL_Palette));
            if ( format->palette == NULL ) {
                GAL_FreeFormat(format);
                GAL_OutOfMemory();
                return(NULL);
            }
            (format->palette)->ncolors = 2;
            (format->palette)->colors = (GAL_Color *)malloc(
                (format->palette)->ncolors*sizeof(GAL_Color));
            if ( (format->palette)->colors == NULL ) {
                GAL_FreeFormat(format);
                GAL_OutOfMemory();
                return(NULL);
            }
            format->palette->colors[0].r = 0xFF;
            format->palette->colors[0].g = 0xFF;
            format->palette->colors[0].b = 0xFF;
            format->palette->colors[1].r = 0x00;
            format->palette->colors[1].g = 0x00;
            format->palette->colors[1].b = 0x00;
            format->Rloss = 8;
            format->Gloss = 8;
            format->Bloss = 8;
            format->Aloss = 8;
            format->Rshift = 0;
            format->Gshift = 0;
            format->Bshift = 0;
            format->Ashift = 0;
            format->Rmask = 0;
            format->Gmask = 0;
            format->Bmask = 0;
            format->Amask = 0;
            break;

        case 4:
            /* Create the 16 color VGA palette */
            format->palette = (GAL_Palette *)malloc(
                            sizeof(GAL_Palette));
            if ( format->palette == NULL ) {
                GAL_FreeFormat(format);
                GAL_OutOfMemory();
                return(NULL);
            }
            (format->palette)->ncolors = 16;
            (format->palette)->colors = (GAL_Color *)malloc(
                (format->palette)->ncolors*sizeof(GAL_Color));
            if ( (format->palette)->colors == NULL ) {
                GAL_FreeFormat(format);
                GAL_OutOfMemory();
                return(NULL);
            }
            /* Punt for now, will this ever be used? */
            memset((format->palette)->colors, 0,
                (format->palette)->ncolors*sizeof(GAL_Color));

            /* Palettized formats have no mask info */
            format->Rloss = 8;
            format->Gloss = 8;
            format->Bloss = 8;
            format->Aloss = 8;
            format->Rshift = 0;
            format->Gshift = 0;
            format->Bshift = 0;
            format->Ashift = 0;
            format->Rmask = 0;
            format->Gmask = 0;
            format->Bmask = 0;
            format->Amask = 0;
            break;

        case 8:
            /* Create an empty 256 color palette */
            format->palette = (GAL_Palette *)malloc(
                            sizeof(GAL_Palette));
            if ( format->palette == NULL ) {
                GAL_FreeFormat(format);
                GAL_OutOfMemory();
                return(NULL);
            }
            (format->palette)->ncolors = 256;
            (format->palette)->colors = (GAL_Color *)malloc(
                (format->palette)->ncolors*sizeof(GAL_Color));
            if ( (format->palette)->colors == NULL ) {
                GAL_FreeFormat(format);
                GAL_OutOfMemory();
                return(NULL);
            }
            memset((format->palette)->colors, 0,
                (format->palette)->ncolors*sizeof(GAL_Color));

            /* Palettized formats have no mask info */
            format->Rloss = 8;
            format->Gloss = 8;
            format->Bloss = 8;
            format->Aloss = 8;
            format->Rshift = 0;
            format->Gshift = 0;
            format->Bshift = 0;
            format->Ashift = 0;
            format->Rmask = 0;
            format->Gmask = 0;
            format->Bmask = 0;
            format->Amask = 0;
            break;

        default:
            /* No palette, just packed pixel info */
            format->palette = NULL;
            format->Rshift = 0;
            format->Rloss = 8;
            if ( Rmask ) {
                for ( mask = Rmask; !(mask&0x01); mask >>= 1 )
                    ++format->Rshift;
                for ( ; (mask&0x01); mask >>= 1 )
                    --format->Rloss;
            }
            format->Gshift = 0;
            format->Gloss = 8;
            if ( Gmask ) {
                for ( mask = Gmask; !(mask&0x01); mask >>= 1 )
                    ++format->Gshift;
                for ( ; (mask&0x01); mask >>= 1 )
                    --format->Gloss;
            }
            format->Bshift = 0;
            format->Bloss = 8;
            if ( Bmask ) {
                for ( mask = Bmask; !(mask&0x01); mask >>= 1 )
                    ++format->Bshift;
                for ( ; (mask&0x01); mask >>= 1 )
                    --format->Bloss;
            }
            format->Ashift = 0;
            format->Aloss = 8;

            if ( Amask ) {
                for ( mask = Amask; !(mask&0x01); mask >>= 1 )
                    ++format->Ashift;
                for ( ; (mask&0x01); mask >>= 1 )
                    --format->Aloss;
            }
            format->Rmask = Rmask;
            format->Gmask = Gmask;
            format->Bmask = Bmask;
            format->Amask = Amask;
            break;
    }

    /* Calculate some standard bitmasks, if necessary 
     * Note:  This could conflict with an alpha mask, if given.
     */
    if ( (bpp > 8) && !format->Rmask && !format->Gmask && !format->Bmask ) {
        /* R-G-B */
        if ( bpp > 24 )
            bpp = 24;
        format->Rloss = 8-(bpp/3);
        format->Gloss = 8-(bpp/3)-(bpp%3);
        format->Bloss = 8-(bpp/3);
        format->Rshift = ((bpp/3)+(bpp%3))+(bpp/3);
        format->Gshift = (bpp/3);
        format->Bshift = 0;
        format->Rmask = ((0xFF>>format->Rloss)<<format->Rshift);
        format->Gmask = ((0xFF>>format->Gloss)<<format->Gshift);
        format->Bmask = ((0xFF>>format->Bloss)<<format->Bshift);

    }
    return(format);
}
GAL_PixelFormat *GAL_ReallocFormat(GAL_Surface *surface, int bpp,
            Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    if ( surface->format ) {
        GAL_FreeFormat(surface->format);
        GAL_FormatChanged(surface);
    }
    surface->format = GAL_AllocFormat(bpp, Rmask, Gmask, Bmask, Amask);
    return surface->format;
}

/*
 * Change any previous mappings from/to the new surface format
 */
void GAL_FormatChanged(GAL_Surface *surface)
{
    surface->format_version++;
    GAL_InvalidateMap(surface->map);
}
/*
 * Free a previously allocated format structure
 */
void GAL_FreeFormat(GAL_PixelFormat *format)
{
    if ( format ) {
        if ( format->palette ) {
            if ( format->palette->colors ) {
                free(format->palette->colors);
            }
            free(format->palette);
        }
        free(format);
    }
}
/*
 * Calculate an 8-bit (3 red, 3 green, 2 blue) dithered palette of colors
 */
#define RGB332  1

void GAL_DitherColors(GAL_Color *colors, int bpp)
{
    int i;
    if(bpp != 8)
        return;        /* only 8bpp supported right now */

    for(i = 0; i < 256; i++) {
#if RGB332
        int r, g, b;
        /* map each bit field to the full [0, 255] interval,
           so 0 is mapped to (0, 0, 0) and 255 to (255, 255, 255) */
        r = i & 0xe0;
        r |= r >> 3 | r >> 6;
        colors[i].r = r;
        g = (i << 3) & 0xe0;
        g |= g >> 3 | g >> 6;
        colors[i].g = g;
        b = i & 0x3;
        b |= b << 2;
        b |= b << 4;
        colors[i].b = b;
#else
        colors[i].r = ((i & 192) >> 6) * (64);        /* 2 bits */
        colors[i].g = ((i & 56) >> 3) * (64 / 2);    /* 3 bits */
        colors[i].b = (i & 7) * (64 / 2);        /* 3 bits */
#endif
    }
}

Uint8 GAL_FindDitheredColor (int bpp, Uint8 r, Uint8 g, Uint8 b)
{
    if (bpp != 8)
        return 0;        /* only 8bpp supported right now */

#if RGB332
    r &= 0xe0;
    g &= 0xe0; g >>= 3;
    b &= 0xc0; b >>= 6;
#else
    r &= 0xc0;
    g &= 0xe0; g >>= 2;
    b &= 0xe0; b >>= 5;
#endif

    return r | g | b;
}

/* 
 * Calculate the pad-aligned scanline width of a surface
 */
Uint32 GAL_CalculatePitch(GAL_Surface *surface)
{
    Uint32 pitch;

    /* Box should be 4-byte aligned for speed */
    pitch = surface->w * surface->format->BytesPerPixel;
    switch (surface->format->BitsPerPixel) {
        case 1:
            pitch = (pitch+7)/8;
            break;
        case 4:
            pitch = (pitch+1)/2;
            break;
        default:
            break;
    }
    pitch = (pitch + 3) & ~3;    /* 4-byte aligning */
    return(pitch);
}

/*
 * Match an RGB value to a particular palette index
 */
Uint8 GAL_FindColor(GAL_Palette *pal, Uint8 r, Uint8 g, Uint8 b)
{
    /* Do colorspace distance matching */
    unsigned int smallest;
    unsigned int distance;
    int rd, gd, bd;
    int i;
    Uint8 pixel=0;
        
    smallest = ~0;
    for ( i=0; i<pal->ncolors; ++i ) {
        rd = pal->colors[i].r - r;
        gd = pal->colors[i].g - g;
        bd = pal->colors[i].b - b;
        distance = (rd*rd)+(gd*gd)+(bd*bd);
        if ( distance < smallest ) {
            pixel = i;
            if ( distance == 0 ) { /* Perfect match! */
                break;
            }
            smallest = distance;
        }
    }
    return(pixel);
}

/* Find the opaque pixel value corresponding to an RGB triple */
Uint32 GAL_MapRGB(GAL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b)
{
    if ( format->palette == NULL ) {
        Uint32 pixv = (r >> format->Rloss) << format->Rshift
               | (g >> format->Gloss) << format->Gshift
               | (b >> format->Bloss) << format->Bshift
               | format->Amask;
#ifdef _NEWGAL_SWAP16
        pixv = (pixv & 0xFFFF0000) | ArchSwap16((unsigned short)pixv);
#endif
#if defined(_EM86_IAL) || defined (_EM85_IAL)
        if (pixv == 0) pixv ++;
#endif
        return pixv;
    } else {
        if (format->DitheredPalette)
            return GAL_FindDitheredColor (format->BitsPerPixel, r, g, b);
        else
            return GAL_FindColor(format->palette, r, g, b);
    }
}

/* Find the pixel value corresponding to an RGBA quadruple */
Uint32 GAL_MapRGBA(GAL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if ( format->palette == NULL ) {
        Uint32 pixv = (r >> format->Rloss) << format->Rshift
            | (g >> format->Gloss) << format->Gshift
            | (b >> format->Bloss) << format->Bshift
            | ((a >> format->Aloss) << format->Ashift & format->Amask);
#ifdef _NEWGAL_SWAP16
        pixv = (pixv & 0xFFFF0000) | ArchSwap16((unsigned short)pixv);
#endif
        return pixv;
    } else {
        if (format->DitheredPalette)
            return GAL_FindDitheredColor (format->BitsPerPixel, r, g, b);
        else
            return GAL_FindColor(format->palette, r, g, b);
    }
}

void GAL_GetRGBA(Uint32 pixel, GAL_PixelFormat *fmt,
         Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
    if ( fmt->palette == NULL ) {
            unsigned rv, gv, bv, av;
#ifdef _NEWGAL_SWAP16
                pixel = (pixel & 0xFFFF0000) | ArchSwap16((unsigned short)pixel);
#endif
            /*
         * This makes sure that the result is mapped to the
         * interval [0..255], and the maximum value for each
         * component is 255. This is important to make sure
         * that white is indeed reported as (255, 255, 255),
         * and that opaque alpha is 255.
         * This only works for RGB bit fields at least 4 bit
         * wide, which is almost always the case.
         */
        rv = (pixel & fmt->Rmask) >> fmt->Rshift;
        *r = (rv << fmt->Rloss) + (rv >> (8 - fmt->Rloss));
        gv = (pixel & fmt->Gmask) >> fmt->Gshift;
        *g = (gv << fmt->Gloss) + (gv >> (8 - fmt->Gloss));
        bv = (pixel & fmt->Bmask) >> fmt->Bshift;
        *b = (bv << fmt->Bloss) + (bv >> (8 - fmt->Bloss));
        if(fmt->Amask) {
                av = (pixel & fmt->Amask) >> fmt->Ashift;
            *a = (av << fmt->Aloss) + (av >> (8 - fmt->Aloss));
        } else
                *a = GAL_ALPHA_OPAQUE;
    } else {
        *r = fmt->palette->colors[pixel].r;
        *g = fmt->palette->colors[pixel].g;
        *b = fmt->palette->colors[pixel].b;
        *a = GAL_ALPHA_OPAQUE;
    }
}

void GAL_GetRGB(Uint32 pixel, GAL_PixelFormat *fmt, Uint8 *r,Uint8 *g,Uint8 *b)
{
    if ( fmt->palette == NULL ) {
            /* the note for GAL_GetRGBA above applies here too */
            unsigned rv, gv, bv;
#ifdef _NEWGAL_SWAP16
                pixel = (pixel & 0xFFFF0000) | ArchSwap16((unsigned short)pixel);
#endif
        rv = (pixel & fmt->Rmask) >> fmt->Rshift;
        *r = (rv << fmt->Rloss) + (rv >> (8 - fmt->Rloss));
        gv = (pixel & fmt->Gmask) >> fmt->Gshift;
        *g = (gv << fmt->Gloss) + (gv >> (8 - fmt->Gloss));
        bv = (pixel & fmt->Bmask) >> fmt->Bshift;
        *b = (bv << fmt->Bloss) + (bv >> (8 - fmt->Bloss));
    } else {
        *r = fmt->palette->colors[pixel].r;
        *g = fmt->palette->colors[pixel].g;
        *b = fmt->palette->colors[pixel].b;
    }
}

/* Apply gamma to a set of colors - this is easy. :) */
void GAL_ApplyGamma(Uint16 *gamma, GAL_Color *colors, GAL_Color *output,
                            int ncolors)
{
    int i;

    for ( i=0; i<ncolors; ++i ) {
        output[i].r = gamma[0*256 + colors[i].r] >> 8;
        output[i].g = gamma[1*256 + colors[i].g] >> 8;
        output[i].b = gamma[2*256 + colors[i].b] >> 8;
    }
}

/* Map from Palette to Palette */
static Uint8 *Map1to1(GAL_Palette *src, GAL_Palette *dst, int *identical)
{
    Uint8 *map;
    int i;

    if ( identical ) {
        if ( src->ncolors <= dst->ncolors ) {
            /* If an identical palette, no need to map */
            if ( memcmp(src->colors, dst->colors, src->ncolors*
                        sizeof(GAL_Color)) == 0 ) {
                *identical = 1;
                return(NULL);
            }
        }
        *identical = 0;
    }
    map = (Uint8 *)malloc(src->ncolors);
    if ( map == NULL ) {
        GAL_OutOfMemory();
        return(NULL);
    }
    for ( i=0; i<src->ncolors; ++i ) {
        map[i] = GAL_FindColor(dst,
            src->colors[i].r, src->colors[i].g, src->colors[i].b);
    }
    return(map);
}
/* Map from Palette to BitField */
static Uint8 *Map1toN(GAL_Palette *src, GAL_PixelFormat *dst)
{
    Uint8 *map;
    int i;
    int  bpp;
    unsigned alpha;

    bpp = ((dst->BytesPerPixel == 3) ? 4 : dst->BytesPerPixel);
    map = (Uint8 *)malloc(src->ncolors*bpp);
    if ( map == NULL ) {
        GAL_OutOfMemory();
        return(NULL);
    }

    alpha = dst->Amask ? GAL_ALPHA_OPAQUE : 0;
    /* We memory copy to the pixel map so the endianness is preserved */
    for ( i=0; i<src->ncolors; ++i ) {
        ASSEMBLE_RGBA(&map[i*bpp], dst->BytesPerPixel, dst,
                  src->colors[i].r, src->colors[i].g,
                  src->colors[i].b, alpha);
    }
    return(map);
}
/* Map from BitField to Dithered-Palette to Palette */
static Uint8 *MapNto1(GAL_PixelFormat *src, GAL_Palette *dst, int *identical)
{
    /* Generate a 256 color dither palette */
    GAL_Palette dithered;
    GAL_Color colors[256];

    dithered.ncolors = 256;
    GAL_DitherColors(colors, 8);
    dithered.colors = colors;
    return(Map1to1(&dithered, dst, identical));
}

GAL_BlitMap *GAL_AllocBlitMap(void)
{
    GAL_BlitMap *map;

    /* Allocate the empty map */
    map = (GAL_BlitMap *)calloc(1, sizeof(*map));//malloc(sizeof(*map));
    if ( map == NULL ) {
        GAL_OutOfMemory();
        return(NULL);
    }
    //memset(map, 0, sizeof(*map));

    /* Allocate the software blit data */
    map->sw_data = (struct private_swaccel *)calloc(1, sizeof(*map->sw_data));//malloc(sizeof(*map->sw_data));
    if ( map->sw_data == NULL ) {
        GAL_FreeBlitMap(map);
        GAL_OutOfMemory();
        return(NULL);
    }
    //memset(map->sw_data, 0, sizeof(*map->sw_data));

    /* It's ready to go */
    return(map);
}
void GAL_InvalidateMap(GAL_BlitMap *map)
{
    if ( ! map ) {
        return;
    }
    map->dst = NULL;
    map->format_version = (unsigned int)-1;
    if ( map->table ) {
        free(map->table);
        map->table = NULL;
    }
}
int GAL_MapSurface (GAL_Surface *src, GAL_Surface *dst)
{
    GAL_PixelFormat *srcfmt;
    GAL_PixelFormat *dstfmt;
    GAL_BlitMap *map;

    /* Clear out any previous mapping */
    map = src->map;
    if ( (src->flags & GAL_RLEACCEL) == GAL_RLEACCEL ) {
        GAL_UnRLESurface(src, 1);
    }
    GAL_InvalidateMap(map);

    /* Figure out what kind of mapping we're doing */
    map->identity = 0;
    srcfmt = src->format;
    dstfmt = dst->format;
    switch (srcfmt->BytesPerPixel) {
        case 1:
        switch (dstfmt->BytesPerPixel) {
            case 1:
            /* Palette --> Palette */
            /* If both GAL_HWSURFACE, assume have same palette */
            if ( ((src->flags & GAL_HWSURFACE) == GAL_HWSURFACE) &&
                 ((dst->flags & GAL_HWSURFACE) == GAL_HWSURFACE) ) {
                map->identity = 1;
            } else {
                map->table = Map1to1(srcfmt->palette,
                    dstfmt->palette, &map->identity);
            }
            if ( ! map->identity ) {
                if ( map->table == NULL ) {
                    return(-1);
                }
            }
            if (srcfmt->BitsPerPixel!=dstfmt->BitsPerPixel)
                map->identity = 0;
            break;

            default:
            /* Palette --> BitField */
            map->table = Map1toN(srcfmt->palette, dstfmt);
            if ( map->table == NULL ) {
                return(-1);
            }
            break;
        }
        break;
    default:
        switch (dstfmt->BytesPerPixel) {
            case 1:
            /* BitField --> Palette */
            map->table = MapNto1(srcfmt,
                    dstfmt->palette, &map->identity);
            if ( ! map->identity ) {
                if ( map->table == NULL ) {
                    return(-1);
                }
            }
            map->identity = 0;    /* Don't optimize to copy */
            break;
            default:
            /* BitField --> BitField */
            if (
#ifdef _MGGAL_S3C6410
                    (((src->flags & GAL_HWSURFACE) == GAL_HWSURFACE) && ((dst->flags & GAL_HWSURFACE) == GAL_HWSURFACE))
                    ||
#endif
                    FORMAT_EQUAL(srcfmt, dstfmt)
                ) {
                map->identity = 1;
            }
            break;
        }
        break;
    }

    map->dst = dst;
    map->format_version = (++dst->format_version);

    /* Choose your blitters wisely */
    return(GAL_CalculateBlit(src));
}
void GAL_FreeBlitMap(GAL_BlitMap *map)
{
    if ( map ) {
        GAL_InvalidateMap(map);
        if ( map->sw_data != NULL ) {
            free(map->sw_data);
        }
        free(map);
    }
}

