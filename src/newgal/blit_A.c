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

/* Functions to perform alpha blended blitting */

/* N->1 blending with per-surface alpha */
static void BlitNto1SurfaceAlpha(GAL_BlitInfo *info)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint8 *src = info->s_pixels;
        int srcskip = info->s_skip;
        Uint8 *dst = info->d_pixels;
        int dstskip = info->d_skip;
        Uint8 *palmap = info->table;
        GAL_PixelFormat *srcfmt = info->src;
        GAL_PixelFormat *dstfmt = info->dst;
        int srcbpp = srcfmt->BytesPerPixel;

        const unsigned A = srcfmt->alpha;

        while ( height-- ) {
            DUFFS_LOOP4(
            {
                Uint32 pixel;
                unsigned sR;
                unsigned sG;
                unsigned sB;
                unsigned dR;
                unsigned dG;
                unsigned dB;
                DISEMBLE_RGB(src, srcbpp, srcfmt, pixel, sR, sG, sB);
                dR = dstfmt->palette->colors[*dst].r;
                dG = dstfmt->palette->colors[*dst].g;
                dB = dstfmt->palette->colors[*dst].b;
                ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
                dR &= 0xff;
                dG &= 0xff;
                dB &= 0xff;
                /* Pack RGB into 8bit pixel */
                if ( palmap == NULL ) {
                    *dst =((dR>>5)<<(3+2))|
                          ((dG>>5)<<(2))|
                          ((dB>>6)<<(0));
                } else {
                    *dst = palmap[((dR>>5)<<(3+2))|
                                  ((dG>>5)<<(2))  |
                                  ((dB>>6)<<(0))];
                }
                dst++;
                src += srcbpp;
            },
            width);
            src += srcskip;
            dst += dstskip;
        }
}

/* N->1 blending with pixel alpha */
static void BlitNto1PixelAlpha(GAL_BlitInfo *info)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint8 *src = info->s_pixels;
        int srcskip = info->s_skip;
        Uint8 *dst = info->d_pixels;
        int dstskip = info->d_skip;
        Uint8 *palmap = info->table;
        GAL_PixelFormat *srcfmt = info->src;
        GAL_PixelFormat *dstfmt = info->dst;
        int srcbpp = srcfmt->BytesPerPixel;

        /* FIXME: fix alpha bit field expansion here too? */
        while ( height-- ) {
            DUFFS_LOOP4(
            {
                Uint32 pixel;
                unsigned sR;
                unsigned sG;
                unsigned sB;
                unsigned sA;
                unsigned dR;
                unsigned dG;
                unsigned dB;
                DISEMBLE_RGBA(src,srcbpp,srcfmt,pixel,sR,sG,sB,sA);
                dR = dstfmt->palette->colors[*dst].r;
                dG = dstfmt->palette->colors[*dst].g;
                dB = dstfmt->palette->colors[*dst].b;
                ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
                dR &= 0xff;
                dG &= 0xff;
                dB &= 0xff;
                /* Pack RGB into 8bit pixel */
                if ( palmap == NULL ) {
                    *dst =((dR>>5)<<(3+2))|
                          ((dG>>5)<<(2))|
                          ((dB>>6)<<(0));
                } else {
                    *dst = palmap[((dR>>5)<<(3+2))|
                                  ((dG>>5)<<(2))  |
                                  ((dB>>6)<<(0))  ];
                }
                dst++;
                src += srcbpp;
            },
            width);
            src += srcskip;
            dst += dstskip;
        }
}

/* colorkeyed N->1 blending with per-surface alpha */
static void BlitNto1SurfaceAlphaKey(GAL_BlitInfo *info)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint8 *src = info->s_pixels;
        int srcskip = info->s_skip;
        Uint8 *dst = info->d_pixels;
        int dstskip = info->d_skip;
        Uint8 *palmap = info->table;
        GAL_PixelFormat *srcfmt = info->src;
        GAL_PixelFormat *dstfmt = info->dst;
        int srcbpp = srcfmt->BytesPerPixel;
        Uint32 ckey = srcfmt->colorkey;

        const int A = srcfmt->alpha;

        while ( height-- ) {
            DUFFS_LOOP(
            {
                Uint32 pixel;
                unsigned sR;
                unsigned sG;
                unsigned sB;
                unsigned dR;
                unsigned dG;
                unsigned dB;
                DISEMBLE_RGB(src, srcbpp, srcfmt, pixel, sR, sG, sB);
                if ( pixel != ckey ) {
                    dR = dstfmt->palette->colors[*dst].r;
                    dG = dstfmt->palette->colors[*dst].g;
                    dB = dstfmt->palette->colors[*dst].b;
                    ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
                    dR &= 0xff;
                    dG &= 0xff;
                    dB &= 0xff;
                    /* Pack RGB into 8bit pixel */
                    if ( palmap == NULL ) {
                        *dst =((dR>>5)<<(3+2))|
                              ((dG>>5)<<(2)) |
                              ((dB>>6)<<(0));
                    } else {
                        *dst = palmap[((dR>>5)<<(3+2))|
                                      ((dG>>5)<<(2))  |
                                      ((dB>>6)<<(0))  ];
                    }
                }
                dst++;
                src += srcbpp;
            },
            width);
            src += srcskip;
            dst += dstskip;
        }
}

/* fast RGB888->(A)RGB888 blending with surface alpha=128 special case */
static void BlitRGBtoRGBSurfaceAlpha128(GAL_BlitInfo *info)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint32 *srcp = (Uint32 *)info->s_pixels;
        int srcskip = info->s_skip >> 2;
        Uint32 *dstp = (Uint32 *)info->d_pixels;
        int dstskip = info->d_skip >> 2;

        while(height--) {
            DUFFS_LOOP4({
                    Uint32 s = *srcp++;
                    Uint32 d = *dstp;
                    *dstp++ = ((((s & 0x00fefefe) + (d & 0x00fefefe)) >> 1)
                               + (s & d & 0x00010101)) | 0xff000000;
            }, width);
            srcp += srcskip;
            dstp += dstskip;
        }
}

/* fast RGB888->(A)RGB888 blending with surface alpha */
static void BlitRGBtoRGBSurfaceAlpha(GAL_BlitInfo *info)
{
        unsigned alpha = info->src->alpha;
        if(alpha == 128) {
                BlitRGBtoRGBSurfaceAlpha128(info);
        } else {
                int width = info->d_width;
                int height = info->d_height;
                Uint32 *srcp = (Uint32 *)info->s_pixels;
                int srcskip = info->s_skip >> 2;
                Uint32 *dstp = (Uint32 *)info->d_pixels;
                int dstskip = info->d_skip >> 2;

                while(height--) {
                        DUFFS_LOOP4({
                                Uint32 s;
                                Uint32 d;
                                Uint32 s1;
                                Uint32 d1;
                                s = *srcp;
                                d = *dstp;
                                s1 = s & 0xff00ff;
                                d1 = d & 0xff00ff;
                                d1 = (d1 + (((s1 - d1) * alpha) >> 8))
                                     & 0xff00ff;
                                s &= 0xff00;
                                d &= 0xff00;
                                d = (d + (((s - d) * alpha) >> 8)) & 0xff00;
                                *dstp = d1 | d | 0xff000000;
                                ++srcp;
                                ++dstp;
                        }, width);
                        srcp += srcskip;
                        dstp += dstskip;
                }
        }
}

/* fast ARGB888->(A)RGB888 blending with pixel alpha */
static void BlitRGBtoRGBPixelAlpha(GAL_BlitInfo *info)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint32 *srcp = (Uint32 *)info->s_pixels;
        int srcskip = info->s_skip >> 2;
        Uint32 *dstp = (Uint32 *)info->d_pixels;
        int dstskip = info->d_skip >> 2;

        while(height--) {
            DUFFS_LOOP4({
                Uint32 dalpha;
                Uint32 d;
                Uint32 s1;
                Uint32 d1;
                Uint32 s = *srcp;
                Uint32 alpha = s >> 24;
                /* FIXME: Here we special-case opaque alpha since the
                   compositioning used (>>8 instead of /255) doesn't handle
                   it correctly. Also special-case alpha=0 for speed?
                   Benchmark this! */
                if (alpha == GAL_ALPHA_OPAQUE) {
                    *dstp = s;
                } else {
                    /*
                     * take out the middle component (green), and process
                     * the other two in parallel. One multiply less.
                     */
                    d = *dstp;
                    dalpha = d >> 24;

                    s1 = s & 0xff00ff;
                    d1 = d & 0xff00ff;
                    d1 = (d1 + (((s1 - d1) * alpha) >> 8)) & 0xff00ff;
                    s &= 0xff00;
                    d &= 0xff00;
                    d = (d + (((s - d) * alpha) >> 8)) & 0xff00;

                    /* should not change dest_alpha
                    dalpha = (dalpha + alpha - (((dalpha * alpha)) >> 8));
                    */
                    *dstp = d1 | (d & 0xff00) | (dalpha << 24);
                }
                ++srcp;
                ++dstp;
            }, width);
            srcp += srcskip;
            dstp += dstskip;
        }
}

/* 16bpp special case for per-surface alpha=50%: blend 2 pixels in parallel */

/* blend a single 16 bit pixel at 50% */
#define BLEND16_50(d, s, mask)                                                \
        ((((s & mask) + (d & mask)) >> 1) + (s & d & (~mask & 0xffff)))

/* blend two 16 bit pixels at 50% */
#define BLEND2x16_50(d, s, mask)                                             \
        (((s & (mask | mask << 16)) >> 1) + ((d & (mask | mask << 16)) >> 1) \
         + (s & d & (~(mask | mask << 16))))

static void Blit16to16SurfaceAlpha128(GAL_BlitInfo *info, Uint16 mask)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint16 *srcp = (Uint16 *)info->s_pixels;
        int srcskip = info->s_skip >> 1;
        Uint16 *dstp = (Uint16 *)info->d_pixels;
        int dstskip = info->d_skip >> 1;

        while(height--) {
                if(((unsigned long)srcp ^ (unsigned long)dstp) & 2) {
                        /*
                         * Source and destination not aligned, pipeline it.
                         * This is mostly a win for big blits but no loss for
                         * small ones
                         */
                        Uint32 prev_sw;
                        int w = width;

                        /* handle odd destination */
                        if((unsigned long)dstp & 2) {
                                Uint16 d = *dstp, s = *srcp;
                                *dstp = BLEND16_50(d, s, mask);
                                dstp++;
                                srcp++;
                                w--;
                        }
                        srcp++;        /* srcp is now 32-bit aligned */

                        /* bootstrap pipeline with first halfword */
                        prev_sw = ((Uint32 *)srcp)[-1];

                        while(w > 1) {
                                Uint32 sw, dw, s;
                                sw = *(Uint32 *)srcp;
                                dw = *(Uint32 *)dstp;
                                if(GAL_BYTEORDER == GAL_BIG_ENDIAN)
                                        s = (prev_sw << 16) + (sw >> 16);
                                else
                                        s = (prev_sw >> 16) + (sw << 16);
                                prev_sw = sw;
                                *(Uint32 *)dstp = BLEND2x16_50(dw, s, mask);
                                dstp += 2;
                                srcp += 2;
                                w -= 2;
                        }

                        /* final pixel if any */
                        if(w) {
                                Uint16 d = *dstp, s;
                                if(GAL_BYTEORDER == GAL_BIG_ENDIAN)
                                        s = prev_sw;
                                else
                                        s = prev_sw >> 16;
                                *dstp = BLEND16_50(d, s, mask);
                                srcp++;
                                dstp++;
                        }
                        srcp += srcskip - 1;
                        dstp += dstskip;
                } else {
                        /* source and destination are aligned */
                        int w = width;

                        /* first odd pixel? */
                        if((unsigned long)srcp & 2) {
                                Uint16 d = *dstp, s = *srcp;
                                *dstp = BLEND16_50(d, s, mask);
                                srcp++;
                                dstp++;
                                w--;
                        }
                        /* srcp and dstp are now 32-bit aligned */

                        while(w > 1) {
                                Uint32 sw = *(Uint32 *)srcp;
                                Uint32 dw = *(Uint32 *)dstp;
                                *(Uint32 *)dstp = BLEND2x16_50(dw, sw, mask);
                                srcp += 2;
                                dstp += 2;
                                w -= 2;
                        }

                        /* last odd pixel? */
                        if(w) {
                                Uint16 d = *dstp, s = *srcp;
                                *dstp = BLEND16_50(d, s, mask);
                                srcp++;
                                dstp++;
                        }
                        srcp += srcskip;
                        dstp += dstskip;
                }
        }
}

/* fast RGB565->RGB565 blending with surface alpha */
static void Blit565to565SurfaceAlpha(GAL_BlitInfo *info)
{
        unsigned alpha = info->src->alpha;
        if(alpha == 128) {
                Blit16to16SurfaceAlpha128(info, 0xf7de);
        } else {
                int width = info->d_width;
                int height = info->d_height;
                Uint16 *srcp = (Uint16 *)info->s_pixels;
                int srcskip = info->s_skip >> 1;
                Uint16 *dstp = (Uint16 *)info->d_pixels;
                int dstskip = info->d_skip >> 1;
                alpha >>= 3;        /* downscale alpha to 5 bits */

                while(height--) {
                        DUFFS_LOOP4({
                                Uint32 s = *srcp++;
                                Uint32 d = *dstp;
                                /*
                                 * shift out the middle component (green) to
                                 * the high 16 bits, and process all three RGB
                                 * components at the same time.
                                 */
                                s = (s | s << 16) & 0x07e0f81f;
                                d = (d | d << 16) & 0x07e0f81f;
                                d += (s - d) * alpha >> 5;
                                d &= 0x07e0f81f;
                                *dstp++ = d | d >> 16;
                        }, width);
                        srcp += srcskip;
                        dstp += dstskip;
                }
        }
}

/* fast RGB555->RGB555 blending with surface alpha */
static void Blit555to555SurfaceAlpha(GAL_BlitInfo *info)
{
        unsigned alpha = info->src->alpha; /* downscale alpha to 5 bits */
        if(alpha == 128) {
                Blit16to16SurfaceAlpha128(info, 0xfbde);
        } else {
                int width = info->d_width;
                int height = info->d_height;
                Uint16 *srcp = (Uint16 *)info->s_pixels;
                int srcskip = info->s_skip >> 1;
                Uint16 *dstp = (Uint16 *)info->d_pixels;
                int dstskip = info->d_skip >> 1;
                alpha >>= 3;                /* downscale alpha to 5 bits */

                while(height--) {
                        DUFFS_LOOP4({
                                Uint32 s = *srcp++;
                                Uint32 d = *dstp;
                                /*
                                 * shift out the middle component (green) to
                                 * the high 16 bits, and process all three RGB
                                 * components at the same time.
                                 */
                                s = (s | s << 16) & 0x03e07c1f;
                                d = (d | d << 16) & 0x03e07c1f;
                                d += (s - d) * alpha >> 5;
                                d &= 0x03e07c1f;
                                *dstp++ = d | d >> 16;
                        }, width);
                        srcp += srcskip;
                        dstp += dstskip;
                }
        }
}

/* fast ARGB8888->RGB565 blending with pixel alpha */
static void BlitARGBto565PixelAlpha(GAL_BlitInfo *info)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint32 *srcp = (Uint32 *)info->s_pixels;
        int srcskip = info->s_skip >> 2;
        Uint16 *dstp = (Uint16 *)info->d_pixels;
        int dstskip = info->d_skip >> 1;

        while(height--) {
            DUFFS_LOOP4({
                Uint32 s = *srcp;
                unsigned alpha = s >> 27; /* downscale alpha to 5 bits */
                /* FIXME: Here we special-case opaque alpha since the
                   compositioning used (>>8 instead of /255) doesn't handle
                   it correctly. Also special-case alpha=0 for speed?
                   Benchmark this! */
                if(alpha == (GAL_ALPHA_OPAQUE >> 3)) {
                    *dstp = (s >> 8 & 0xf800) + (s >> 5 & 0x7e0)
                          + (s >> 3  & 0x1f);
                } else {
                    Uint32 d = *dstp;
                    /*
                     * convert source and destination to G0RAB65565
                     * and blend all components at the same time
                     */
                    s = ((s & 0xfc00) << 11) + (s >> 8 & 0xf800)
                      + (s >> 3 & 0x1f);
                    d = (d | d << 16) & 0x07e0f81f;
                    d += (s - d) * alpha >> 5;
                    d &= 0x07e0f81f;
                    *dstp = d | d >> 16;
                }
                srcp++;
                dstp++;
            }, width);
            srcp += srcskip;
            dstp += dstskip;
        }
}

/* fast ARGB8888->RGB555 blending with pixel alpha */
static void BlitARGBto555PixelAlpha(GAL_BlitInfo *info)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint32 *srcp = (Uint32 *)info->s_pixels;
        int srcskip = info->s_skip >> 2;
        Uint16 *dstp = (Uint16 *)info->d_pixels;
        int dstskip = info->d_skip >> 1;

        while(height--) {
            DUFFS_LOOP4({
                unsigned alpha;
                Uint32 s = *srcp;
                alpha = s >> 27; /* downscale alpha to 5 bits */
                /* FIXME: Here we special-case opaque alpha since the
                   compositioning used (>>8 instead of /255) doesn't handle
                   it correctly. Also special-case alpha=0 for speed?
                   Benchmark this! */
                if(alpha == (GAL_ALPHA_OPAQUE >> 3)) {
                    *dstp = (s >> 9 & 0x7c00) + (s >> 6 & 0x3e0)
                          + (s >> 3  & 0x1f);
                } else {
                    Uint32 d = *dstp;
                    /*
                     * convert source and destination to G0RAB65565
                     * and blend all components at the same time
                     */
                    s = ((s & 0xf800) << 10) + (s >> 9 & 0x7c00)
                      + (s >> 3 & 0x1f);
                    d = (d | d << 16) & 0x03e07c1f;
                    d += (s - d) * alpha >> 5;
                    d &= 0x03e07c1f;
                    *dstp = d | d >> 16;
                }
                srcp++;
                dstp++;
            }, width);
            srcp += srcskip;
            dstp += dstskip;
        }
}

/* General (slow) N->N blending with per-surface alpha */
static void BlitNtoNSurfaceAlpha(GAL_BlitInfo *info)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint8 *src = info->s_pixels;
        int srcskip = info->s_skip;
        Uint8 *dst = info->d_pixels;
        int dstskip = info->d_skip;
        GAL_PixelFormat *srcfmt = info->src;
        GAL_PixelFormat *dstfmt = info->dst;
        int srcbpp = srcfmt->BytesPerPixel;
        int dstbpp = dstfmt->BytesPerPixel;
        unsigned sA = srcfmt->alpha;
        unsigned dA = dstfmt->Amask ? GAL_ALPHA_OPAQUE : 0;

        while ( height-- ) {
            DUFFS_LOOP4(
            {
                Uint32 pixel;
                unsigned sR;
                unsigned sG;
                unsigned sB;
                unsigned dR;
                unsigned dG;
                unsigned dB;
                DISEMBLE_RGB(src, srcbpp, srcfmt, pixel, sR, sG, sB);
                DISEMBLE_RGB(dst, dstbpp, dstfmt, pixel, dR, dG, dB);
                ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
                ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
                src += srcbpp;
                dst += dstbpp;
            },
            width);
            src += srcskip;
            dst += dstskip;
        }
}

/* General (slow) colorkeyed N->N blending with per-surface alpha */
static void BlitNtoNSurfaceAlphaKey(GAL_BlitInfo *info)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint8 *src = info->s_pixels;
        int srcskip = info->s_skip;
        Uint8 *dst = info->d_pixels;
        int dstskip = info->d_skip;
        GAL_PixelFormat *srcfmt = info->src;
        GAL_PixelFormat *dstfmt = info->dst;
        Uint32 ckey = srcfmt->colorkey;
        int srcbpp = srcfmt->BytesPerPixel;
        int dstbpp = dstfmt->BytesPerPixel;
        unsigned sA = srcfmt->alpha;
        unsigned dA = dstfmt->Amask ? GAL_ALPHA_OPAQUE : 0;

        while ( height-- ) {
            DUFFS_LOOP4(
            {
                Uint32 pixel;
                unsigned sR;
                unsigned sG;
                unsigned sB;
                unsigned dR;
                unsigned dG;
                unsigned dB;
                RETRIEVE_RGB_PIXEL(src, srcbpp, pixel);
                if(pixel != ckey) {
                    RGB_FROM_PIXEL(pixel, srcfmt, sR, sG, sB);
                    DISEMBLE_RGB(dst, dstbpp, dstfmt, pixel, dR, dG, dB);
                    ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
                    ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
                }
                src += srcbpp;
                dst += dstbpp;
            },
            width);
            src += srcskip;
            dst += dstskip;
        }
}

/* General (slow) N->N blending with pixel alpha */
static void BlitNtoNPixelAlpha(GAL_BlitInfo *info)
{
        int width = info->d_width;
        int height = info->d_height;
        Uint8 *src = info->s_pixels;
        int srcskip = info->s_skip;
        Uint8 *dst = info->d_pixels;
        int dstskip = info->d_skip;
        GAL_PixelFormat *srcfmt = info->src;
        GAL_PixelFormat *dstfmt = info->dst;

        int  srcbpp;
        int  dstbpp;

        /* Set up some basic variables */
        srcbpp = srcfmt->BytesPerPixel;
        dstbpp = dstfmt->BytesPerPixel;

        /* FIXME: for 8bpp source alpha, this doesn't get opaque values
           quite right. for <8bpp source alpha, it gets them very wrong
           (check all macros!)
           It is unclear whether there is a good general solution that doesn't
           need a branch (or a divide). */
        if(dstfmt->Amask) {
                while ( height-- ) {
                        DUFFS_LOOP4(
                        {
                        Uint32 pixel;
                        unsigned sR;
                        unsigned sG;
                        unsigned sB;
                        unsigned dR;
                        unsigned dG;
                        unsigned dB;
                        unsigned sA;
                        unsigned dA;
                        DISEMBLE_RGBA(src, srcbpp, srcfmt, pixel, sR, sG, sB, sA);
                        DISEMBLE_RGBA(dst, dstbpp, dstfmt, pixel, dR, dG, dB, dA);
                        ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
                        dA = sA + dA - ((sA * dA) >> 8);
                        if(dA > 255) dA = 255; //alpha may be greater than 255
                        ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
                        src += srcbpp;
                        dst += dstbpp;
                        },
                        width);
                        src += srcskip;
                        dst += dstskip;
                }
        } else {
                while ( height-- ) {
                        DUFFS_LOOP4(
                        {
                        Uint32 pixel;
                        unsigned sR;
                        unsigned sG;
                        unsigned sB;
                        unsigned dR;
                        unsigned dG;
                        unsigned dB;
                        unsigned sA;
                        unsigned dA;
                        DISEMBLE_RGBA(src, srcbpp, srcfmt, pixel, sR, sG, sB, sA);
                        DISEMBLE_RGBA(dst, dstbpp, dstfmt, pixel, dR, dG, dB, dA);
                        ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
                        ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
                        src += srcbpp;
                        dst += dstbpp;
                        },
                        width);
                        src += srcskip;
                        dst += dstskip;
                }
        }
}


GAL_loblit GAL_CalculateAlphaBlit(GAL_Surface *surface, int blit_index)
{
    GAL_PixelFormat *sf = surface->format;
    GAL_PixelFormat *df = surface->map->dst->format;

    if (sf->Amask == 0) {
        if ((surface->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
            if(df->BytesPerPixel == 1)
                return BlitNto1SurfaceAlphaKey;
            else
                return BlitNtoNSurfaceAlphaKey;
        }
        else {
            /* Per-surface alpha blits */
            switch(df->BytesPerPixel) {
            case 1:
                return BlitNto1SurfaceAlpha;

            case 2:
                if(surface->map->identity) {
                    if(df->Gmask == 0x7e0)
                        return Blit565to565SurfaceAlpha;
                    else if(df->Gmask == 0x3e0)
                        return Blit555to555SurfaceAlpha;
                }
                return BlitNtoNSurfaceAlpha;

            case 4:
                if(sf->Rmask == df->Rmask
                   && sf->Gmask == df->Gmask
                   && sf->Bmask == df->Bmask
                   && (sf->Rmask | sf->Gmask | sf->Bmask) == 0xffffff
                   && sf->BytesPerPixel == 4)
                    return BlitRGBtoRGBSurfaceAlpha;
                else
                    return BlitNtoNSurfaceAlpha;

            case 3:
            default:
                return BlitNtoNSurfaceAlpha;
            }
        }
    }
    else {
        if (surface->flags & GAL_SRCALPHA) {
            if ((surface->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
                if(df->BytesPerPixel == 1)
                    return BlitNto1SurfaceAlphaKey;
                else
                    return BlitNtoNSurfaceAlphaKey;
            }
            else if (df->BytesPerPixel == 1)
                return BlitNto1SurfaceAlpha;
            else
                return BlitNtoNSurfaceAlpha;
        }

        /* Per-pixel alpha blits */
        switch(df->BytesPerPixel) {
        case 1:
            return BlitNto1PixelAlpha;

        case 2:
            if(sf->BytesPerPixel == 4 && sf->Amask == 0xff000000
               && sf->Gmask == 0xff00
               && ((sf->Rmask == 0xff && df->Rmask == 0x1f)
                   || (sf->Bmask == 0xff && df->Bmask == 0x1f))) {
                if(df->Gmask == 0x7e0)
                    return BlitARGBto565PixelAlpha;
                else if(df->Gmask == 0x3e0 && df->Amask != 0x8000)
                    return BlitARGBto555PixelAlpha;
            }
            return BlitNtoNPixelAlpha;

        case 4:
            if(sf->Amask == 0xff000000
               && sf->Rmask == df->Rmask
               && sf->Gmask == df->Gmask
               && sf->Bmask == df->Bmask
               && sf->BytesPerPixel == 4)
                return BlitRGBtoRGBPixelAlpha;
            return BlitNtoNPixelAlpha;

        case 3:
        default:
            return BlitNtoNPixelAlpha;
        }
    }
}

