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

#ifndef _GAL_blit_h
#define _GAL_blit_h

#include "common.h"

/* The structure passed to the low level blit functions */
typedef struct {
    Uint8 *s_pixels;
    int s_width;
    int s_height;
    int s_skip;
    Uint8 *d_pixels;
    int d_width;
    int d_height;
    int d_skip;
    void *aux_data;
    GAL_PixelFormat *src;
    Uint8 *table;
    GAL_PixelFormat *dst;
} GAL_BlitInfo;

/* The type definition for the low level blit functions */
typedef void (*GAL_loblit)(GAL_BlitInfo *info);

/* This is the private info structure for software accelerated blits */
struct private_swaccel {
    GAL_loblit blit;
    void *aux_data;
};

/* Blit mapping definition */
typedef struct GAL_BlitMap {
    GAL_Surface *dst;
    int identity;
    Uint8 *table;
    GAL_blit hw_blit;
    GAL_blit sw_blit;
    struct private_hwaccel *hw_data;
    struct private_swaccel *sw_data;

    /* the version count matches the destination; mismatch indicates
       an invalid mapping */
        unsigned int format_version;
} GAL_BlitMap;


/* Definitions for special global blit functions */
#include "blit_A.h"

/* Functions found in GAL_blit.c */
extern int GAL_CalculateBlit(GAL_Surface *surface);

/* Functions found in GAL_blit_{0,1,N,A}.c */
extern GAL_loblit GAL_CalculateBlit0(GAL_Surface *surface, int complex);
extern GAL_loblit GAL_CalculateBlit1(GAL_Surface *surface, int complex);
extern GAL_loblit GAL_CalculateBlitN(GAL_Surface *surface, int complex);
extern GAL_loblit GAL_CalculateAlphaBlit(GAL_Surface *surface, int complex);

/*
 * Useful macros for blitting routines
 */

#define FORMAT_EQUAL(A, B)                                      \
    ((A)->BitsPerPixel == (B)->BitsPerPixel                     \
     && ((A)->Rmask == (B)->Rmask) && ((A)->Amask == (B)->Amask))

/* scale value in (0 ... (2<<(8-loss) - 1) to (0 ... 255) */
#define RESCALE_RGBA_VALUE(value, loss)                         \
{                                                               \
	switch(loss) {                                              \
		case 1: value <<= 1; break;                             \
		case 2: value <<= 2; break;                             \
		case 3: value <<= 3; break;                             \
		case 4: value *= 17; break;                             \
		case 5: value *= 36; break;                             \
		case 6: value *= 85; break;                             \
		case 7: value *= 255; break;                            \
	}                                                           \
}

/* Load pixel of the specified format from a buffer and get its R-G-B values */

/* Q: rescale values to 0..255 here? 
 * A: The loss of RGB component almost is less than 4, so there is no 
 * need to rescale the values for RGB component. Please see the 
 * above scale method. */
#define RGB_FROM_PIXEL(pixel, fmt, r, g, b)                     \
{                                                               \
    r = (((pixel&fmt->Rmask)>>fmt->Rshift)<<fmt->Rloss);        \
    g = (((pixel&fmt->Gmask)>>fmt->Gshift)<<fmt->Gloss);        \
    b = (((pixel&fmt->Bmask)>>fmt->Bshift)<<fmt->Bloss);        \
}

#define RGB_FROM_RGB565(pixel, r, g, b)                         \
{                                                               \
    r = (((pixel&0xF800)>>11)<<3);                              \
    g = (((pixel&0x07E0)>>5)<<2);                               \
    b = ((pixel&0x001F)<<3);                                    \
}

#define RGB_FROM_RGB555(pixel, r, g, b)                         \
{                                                               \
    r = (((pixel&0x7C00)>>10)<<3);                              \
    g = (((pixel&0x03E0)>>5)<<3);                               \
    b = ((pixel&0x001F)<<3);                                    \
}

#define RGB_FROM_RGB888(pixel, r, g, b)                         \
{                                                               \
    r = ((pixel&0xFF0000)>>16);                                 \
    g = ((pixel&0xFF00)>>8);                                    \
    b = (pixel&0xFF);                                           \
}

#define RETRIEVE_RGB_PIXEL(buf, bpp, pixel)                     \
do {                                                            \
    switch (bpp) {                                              \
        case 2:                                                 \
            pixel = *((Uint16 *)(buf));                         \
        break;                                                  \
                                                                \
        case 3: {                                               \
            Uint8 *B = (Uint8 *)(buf);                          \
            if(GAL_BYTEORDER == GAL_LIL_ENDIAN) {               \
                pixel = B[0] + (B[1] << 8) + (B[2] << 16);      \
            } else {                                            \
                pixel = (B[0] << 16) + (B[1] << 8) + B[2];      \
            }                                                   \
        }                                                       \
        break;                                                  \
                                                                \
        case 4:                                                 \
            pixel = *((Uint32 *)(buf));                         \
        break;                                                  \
                                                                \
        default:                                                \
            pixel = 0; /* appease gcc */                        \
        break;                                                  \
    }                                                           \
} while(0)

#define DISEMBLE_RGB(buf, bpp, fmt, pixel, r, g, b)             \
do {                                                            \
    switch (bpp) {                                              \
        case 2:                                                 \
            pixel = *((Uint16 *)(buf));                         \
        break;                                                  \
                                                                \
        case 3: {                                               \
            Uint8 *B = (Uint8 *)(buf);                          \
            if (GAL_BYTEORDER == GAL_LIL_ENDIAN) {              \
                pixel = B[0] + (B[1] << 8) + (B[2] << 16);      \
            } else {                                            \
                pixel = (B[0] << 16) + (B[1] << 8) + B[2];      \
            }                                                   \
        }                                                       \
        break;                                                  \
                                                                \
        case 4:                                                 \
            pixel = *((Uint32 *)(buf));                         \
        break;                                                  \
                                                                \
        default:                                                \
            pixel = 0;    /* prevent gcc from complaining */    \
        break;                                                  \
    }                                                           \
    RGB_FROM_PIXEL(pixel, fmt, r, g, b);                        \
} while(0)

/* Assemble R-G-B values into a specified pixel format and store them */
#define PIXEL_FROM_RGB(pixel, fmt, r, g, b)                     \
{                                                               \
    pixel = ((r>>fmt->Rloss)<<fmt->Rshift)|                     \
        ((g>>fmt->Gloss)<<fmt->Gshift)|                         \
        ((b>>fmt->Bloss)<<fmt->Bshift);                         \
}

#define RGB565_FROM_RGB(pixel, r, g, b)                         \
{                                                               \
    pixel = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);                    \
}

#define RGB555_FROM_RGB(pixel, r, g, b)                         \
{                                                               \
    pixel = ((r>>3)<<10)|((g>>3)<<5)|(b>>3);                    \
}

#define RGB888_FROM_RGB(pixel, r, g, b)                         \
{                                                               \
    pixel = (r<<16)|(g<<8)|b;                                   \
}

#define ASSEMBLE_RGB(buf, bpp, fmt, r, g, b)                    \
{                                                               \
    switch (bpp) {                                              \
        case 2: {                                               \
            Uint16 pixel;                                       \
                                                                \
            PIXEL_FROM_RGB(pixel, fmt, r, g, b);                \
            *((Uint16 *)(buf)) = pixel;                         \
        }                                                       \
        break;                                                  \
                                                                \
        case 3: {                                               \
            Uint32 pixel;                                       \
            PIXEL_FROM_RGB(pixel, fmt, r, g, b);                \
            if (GAL_BYTEORDER == GAL_LIL_ENDIAN) {              \
                (buf)[0] = pixel;                               \
                (buf)[1] = pixel >> 8;                          \
                (buf)[2] = pixel >> 16;                         \
            } else {                                            \
                (buf)[0] = pixel >> 16;                         \
                (buf)[1] = pixel >> 8;                          \
                (buf)[2] = pixel;                               \
            }                                                   \
        }                                                       \
        break;                                                  \
                                                                \
        case 4: {                                               \
            Uint32 pixel;                                       \
                                                                \
            PIXEL_FROM_RGB(pixel, fmt, r, g, b);                \
            *((Uint32 *)(buf)) = pixel;                         \
        }                                                       \
        break;                                                  \
    }                                                           \
}

#define ASSEMBLE_RGB_AMASK(buf, bpp, fmt, r, g, b, Amask)       \
{                                                               \
    switch (bpp) {                                              \
        case 2: {                                               \
            Uint16 *bufp;                                       \
            Uint16 pixel;                                       \
                                                                \
            bufp = (Uint16 *)buf;                               \
            PIXEL_FROM_RGB(pixel, fmt, r, g, b);                \
            *bufp = pixel | (*bufp & Amask);                    \
        }                                                       \
        break;                                                  \
                                                                \
        case 3: {                                               \
            Uint32 pixel;                                       \
            PIXEL_FROM_RGB(pixel, fmt, r, g, b);                \
            pixel |= Amask;                                     \
            if (GAL_BYTEORDER == GAL_LIL_ENDIAN) {              \
                (buf)[0] = pixel;                               \
                (buf)[1] = pixel >> 8;                          \
                (buf)[2] = pixel >> 16;                         \
            } else {                                            \
                (buf)[0] = pixel >> 16;                         \
                (buf)[1] = pixel >> 8;                          \
                (buf)[2] = pixel;                               \
            }                                                   \
        }                                                       \
        break;                                                  \
                                                                \
        case 4: {                                               \
            Uint32 *bufp;                                       \
            Uint32 pixel;                                       \
                                                                \
            bufp = (Uint32 *)buf;                               \
            PIXEL_FROM_RGB(pixel, fmt, r, g, b);                \
            *bufp = pixel | (*bufp & Amask);                    \
        }                                                       \
        break;                                                  \
    }                                                           \
}

#define RGBA_FROM_PIXEL(pixel, fmt, r, g, b, a)                 \
{                                                               \
    r = ((pixel&fmt->Rmask)>>fmt->Rshift)<<fmt->Rloss;          \
    g = ((pixel&fmt->Gmask)>>fmt->Gshift)<<fmt->Gloss;          \
    b = ((pixel&fmt->Bmask)>>fmt->Bshift)<<fmt->Bloss;          \
    a = ((pixel&fmt->Amask)>>fmt->Ashift);                      \
    RESCALE_RGBA_VALUE(a, fmt->Aloss)                           \
}

#define RGBA_FROM_8888(pixel, fmt, r, g, b, a)                  \
{                                                               \
    r = (pixel&fmt->Rmask)>>fmt->Rshift;                        \
    g = (pixel&fmt->Gmask)>>fmt->Gshift;                        \
    b = (pixel&fmt->Bmask)>>fmt->Bshift;                        \
    a = (pixel&fmt->Amask)>>fmt->Ashift;                        \
}
#define RGBA_FROM_RGBA8888(pixel, r, g, b, a)                   \
{                                                               \
    r = (pixel>>24);                                            \
    g = ((pixel>>16)&0xFF);                                     \
    b = ((pixel>>8)&0xFF);                                      \
    a = (pixel&0xFF);                                           \
}
#define RGBA_FROM_ARGB8888(pixel, r, g, b, a)                   \
{                                                               \
    r = ((pixel>>16)&0xFF);                                     \
    g = ((pixel>>8)&0xFF);                                      \
    b = (pixel&0xFF);                                           \
    a = (pixel>>24);                                            \
}

#define RGBA_FROM_ABGR8888(pixel, r, g, b, a)                   \
{                                                               \
    r = (pixel&0xFF);                                           \
    g = ((pixel>>8)&0xFF);                                      \
    b = ((pixel>>16)&0xFF);                                     \
    a = (pixel>>24);                                            \
}

#define DISEMBLE_RGBA(buf, bpp, fmt, pixel, r, g, b, a)         \
do {                                                            \
    switch (bpp) {                                              \
        case 2:                                                 \
            pixel = *((Uint16 *)(buf));                         \
        break;                                                  \
                                                                \
        case 3:    {                                            \
            Uint8 *b = (Uint8 *)(buf);                          \
            if(GAL_BYTEORDER == GAL_LIL_ENDIAN) {               \
                pixel = b[0] + (b[1] << 8) + (b[2] << 16);      \
            } else {                                            \
                pixel = (b[0] << 16) + (b[1] << 8) + b[2];      \
            }                                                   \
        }                                                       \
        break;                                                  \
                                                                \
        case 4:                                                 \
            pixel = *((Uint32 *)(buf));                         \
        break;                                                  \
                                                                \
        default:                                                \
            pixel = 0; /* stop gcc complaints */                \
        break;                                                  \
    }                                                           \
    RGBA_FROM_PIXEL(pixel, fmt, r, g, b, a);                    \
    pixel &= ~fmt->Amask;                                       \
} while(0)

/* FIXME: this isn't correct, especially for Alpha (maximum != 255) */
#define PIXEL_FROM_RGBA(pixel, fmt, r, g, b, a)                 \
{                                                               \
    pixel = ((r>>fmt->Rloss)<<fmt->Rshift)|                     \
        ((g>>fmt->Gloss)<<fmt->Gshift)|                         \
        ((b>>fmt->Bloss)<<fmt->Bshift)|                         \
        ((a>>fmt->Aloss)<<fmt->Ashift);                         \
}

#define ASSEMBLE_RGBA(buf, bpp, fmt, r, g, b, a)                \
{                                                               \
    switch (bpp) {                                              \
        case 2: {                                               \
            Uint16 pixel;                                       \
            PIXEL_FROM_RGBA(pixel, fmt, r, g, b, a);            \
            *((Uint16 *)(buf)) = pixel;                         \
        }                                                       \
        break;                                                  \
                                                                \
        case 3: {                                               \
            Uint32 pixel;                                       \
            PIXEL_FROM_RGBA(pixel, fmt, r, g, b, a);            \
            if (GAL_BYTEORDER == GAL_LIL_ENDIAN) {              \
                (buf)[0] = pixel;                               \
                (buf)[1] = pixel >> 8;                          \
                (buf)[2] = pixel >> 16;                         \
            } else {                                            \
                (buf)[0] = pixel >> 16;                         \
                (buf)[1] = pixel >> 8;                          \
                (buf)[2] = pixel;                               \
            }                                                   \
        }                                                       \
        break;                                                  \
                                                                \
        case 4: {                                               \
            Uint32 pixel;                                       \
            PIXEL_FROM_RGBA(pixel, fmt, r, g, b, a);            \
            *((Uint32 *)(buf)) = pixel;                         \
        }                                                       \
        break;                                                  \
    }                                                           \
}

/* Blend the RGB values of two pixels based on a source alpha value */
#define ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB)                  \
do {                                                            \
    dR = (((sR-dR)*(A))>>8)+dR;                                 \
    dG = (((sG-dG)*(A))>>8)+dG;                                 \
    dB = (((sB-dB)*(A))>>8)+dB;                                 \
} while(0)

/* This is a very useful loop for optimizing blitters */
#define USE_DUFFS_LOOP
#ifdef USE_DUFFS_LOOP

/* 8-times unrolled loop */
#define DUFFS_LOOP8(pixel_copy_increment, width)                \
{ int n = (width+7)/8;                                          \
    switch (width & 7) {                                        \
    case 0: do {    pixel_copy_increment;                       \
    case 7:        pixel_copy_increment;                        \
    case 6:        pixel_copy_increment;                        \
    case 5:        pixel_copy_increment;                        \
    case 4:        pixel_copy_increment;                        \
    case 3:        pixel_copy_increment;                        \
    case 2:        pixel_copy_increment;                        \
    case 1:        pixel_copy_increment;                        \
        } while ( --n > 0 );                                    \
    }                                                           \
}

/* 4-times unrolled loop */
#define DUFFS_LOOP4(pixel_copy_increment, width)                \
{ int n = (width+3)/4;                                          \
    switch (width & 3) {                                        \
    case 0: do {    pixel_copy_increment;                       \
    case 3:        pixel_copy_increment;                        \
    case 2:        pixel_copy_increment;                        \
    case 1:        pixel_copy_increment;                        \
        } while ( --n > 0 );                                    \
    }                                                           \
}

/* Use the 8-times version of the loop by default */
#define DUFFS_LOOP(pixel_copy_increment, width)                 \
    DUFFS_LOOP8(pixel_copy_increment, width)

#else

/* Don't use Duff's device to unroll loops */
#define DUFFS_LOOP(pixel_copy_increment, width)                 \
{ int n;                                                        \
    for ( n=width; n > 0; --n ) {                               \
        pixel_copy_increment;                                   \
    }                                                           \
}

#define DUFFS_LOOP8(pixel_copy_increment, width)                \
    DUFFS_LOOP(pixel_copy_increment, width)

#define DUFFS_LOOP4(pixel_copy_increment, width)                \
    DUFFS_LOOP(pixel_copy_increment, width)

#endif /* USE_DUFFS_LOOP */

#endif /* _GAL_blit_h */

