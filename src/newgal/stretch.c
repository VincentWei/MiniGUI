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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#include <stdio.h>

#include "common.h"
#include "newgal.h"
#include "blit.h"

/* This isn't ready for general consumption yet - it should be folded
   into the general blitting mechanism.
*/

#if (defined(WIN32) && !defined(_M_ALPHA) && !defined(_WIN32_WCE)) || \
    defined(i386) && defined(__GNUC__) && defined(USE_ASMBLIT)
#define USE_ASM_STRETCH
#endif

#ifdef USE_ASM_STRETCH

#if defined(WIN32) || defined(i386)
#define PREFIX16    0x66
#define STORE_BYTE    0xAA
#define STORE_WORD    0xAB
#define LOAD_BYTE    0xAC
#define LOAD_WORD    0xAD
#define RETURN        0xC3
#else
#error Need assembly opcodes for this architecture
#endif

#if defined(__ELF__) && defined(__GNUC__)
extern unsigned char _copy_row[4096] __attribute__ ((alias ("copy_row")));
#endif
static unsigned char copy_row[4096];

static int generate_rowbytes(int src_w, int dst_w, int bpp)
{
    static struct {
        int bpp;
        int src_w;
        int dst_w;
    } last;

    int i;
    int pos, inc;
    unsigned char *eip;
    unsigned char load, store;

    /* See if we need to regenerate the copy buffer */
    if ( (src_w == last.src_w) &&
         (dst_w == last.src_w) && (bpp == last.bpp) ) {
        return(0);
    }
    last.bpp = bpp;
    last.src_w = src_w;
    last.dst_w = dst_w;

    switch (bpp) {
        case 1:
        load = LOAD_BYTE;
        store = STORE_BYTE;
        break;
        case 2:
        case 4:
        load = LOAD_WORD;
        store = STORE_WORD;
        break;
        default:
        GAL_SetError("NEWGAL: ASM stretch of %d bytes isn't supported.\n", bpp);
        return(-1);
    }
    pos = 0x10000;
    inc = (src_w << 16) / dst_w;
    eip = copy_row;
    for ( i=0; i<dst_w; ++i ) {
        while ( pos >= 0x10000L ) {
            if ( bpp == 2 ) {
                *eip++ = PREFIX16;
            }
            *eip++ = load;
            pos -= 0x10000L;
        }
        if ( bpp == 2 ) {
            *eip++ = PREFIX16;
        }
        *eip++ = store;
        pos += inc;
    }
    *eip++ = RETURN;

    /* Verify that we didn't overflow (too late) */
    if ( eip > (copy_row+sizeof(copy_row)) ) {
        GAL_SetError("NEWGAL: Copy buffer overflow.\n");
        return(-1);
    }
    return(0);
}

#else

#define DEFINE_COPY_ROW(name, type)                             \
static void name(type *src, int src_w, type *dst, int dst_w)    \
{                                                               \
    int i;                                                      \
    int pos, inc;                                               \
    type pixel = 0;                                             \
                                                                \
    pos = 0x10000;                                              \
    inc = (src_w << 16) / dst_w;                                \
    for ( i=dst_w; i>0; --i ) {                                 \
        while ( pos >= 0x10000L ) {                             \
            pixel = *src++;                                     \
            pos -= 0x10000L;                                    \
        }                                                       \
        *dst++ = pixel;                                         \
        pos += inc;                                             \
    }                                                           \
}

DEFINE_COPY_ROW(copy_row1, Uint8)
DEFINE_COPY_ROW(copy_row2, Uint16)
DEFINE_COPY_ROW(copy_row4, Uint32)

#endif /* USE_ASM_STRETCH */

/* The ASM code doesn't handle 24-bpp stretch blits */
static void copy_row3(Uint8 *src, int src_w, Uint8 *dst, int dst_w)
{
    int i;
    int pos, inc;
    Uint8 pixel[3] = {};

    pos = 0x10000;
    inc = (src_w << 16) / dst_w;
    for ( i=dst_w; i>0; --i ) {
        while ( pos >= 0x10000L ) {
            pixel[0] = *src++;
            pixel[1] = *src++;
            pixel[2] = *src++;
            pos -= 0x10000L;
        }
        *dst++ = pixel[0];
        *dst++ = pixel[1];
        *dst++ = pixel[2];
        pos += inc;
    }
}

/* Perform a stretch blit between two surfaces of the same format.
   NOTE:  This function is not safe to call from multiple threads!
*/
static int GAL_SoftStretch(GAL_Surface *src, GAL_Rect *srcrect,
                    GAL_Surface *dst, GAL_Rect *dstrect)
{
    int pos, inc;
    int dst_maxrow;
    int src_row, dst_row;
    Uint8 *srcp = NULL;
    Uint8 *dstp;
    GAL_Rect full_src;
    GAL_Rect full_dst;
#if defined(USE_ASM_STRETCH) && defined(__GNUC__)
    int u1, u2;
#endif
    const int bpp = dst->format->BytesPerPixel;

    if ( src->format->BitsPerPixel != dst->format->BitsPerPixel ) {
        GAL_SetError("NEWGAL: Only works with same format surfaces.\n");
        return(-1);
    }

    /* Verify the blit rectangles */
    if ( srcrect ) {
        if ( (srcrect->x < 0) || (srcrect->y < 0) ||
             ((srcrect->x+srcrect->w) > src->w) ||
             ((srcrect->y+srcrect->h) > src->h) ) {
            GAL_SetError("NEWGAL: Invalid source blit rectangle.\n");
            return(-1);
        }
    } else {
        full_src.x = 0;
        full_src.y = 0;
        full_src.w = src->w;
        full_src.h = src->h;
        srcrect = &full_src;
    }
    if ( dstrect ) {
        if ( (dstrect->x < 0) || (dstrect->y < 0) ||
             ((dstrect->x+dstrect->w) > dst->w) ||
             ((dstrect->y+dstrect->h) > dst->h) ) {
            GAL_SetError("NEWGAL: Invalid destination blit rectangle (%d, %d, %d x %d) to surface (%d x %d)\n",
                    dstrect->x, dstrect->y, dstrect->w, dstrect->h, dst->w, dst->h);
            return(-1);
        }
    } else {
        full_dst.x = 0;
        full_dst.y = 0;
        full_dst.w = dst->w;
        full_dst.h = dst->h;
        dstrect = &full_dst;
    }

    /* Set up the data... */
    pos = 0x10000;
    inc = (srcrect->h << 16) / dstrect->h;
    src_row = srcrect->y;
    dst_row = dstrect->y;

#ifdef USE_ASM_STRETCH
    /* Write the opcodes for this stretch */
    if ( (bpp != 3) &&
         (generate_rowbytes(srcrect->w, dstrect->w, bpp) < 0) ) {
        return(-1);
    }
#endif

    /* Perform the stretch blit */
    for ( dst_maxrow = dst_row+dstrect->h; dst_row<dst_maxrow; ++dst_row ) {
        dstp = (Uint8 *)dst->pixels + (dst_row*dst->pitch)
                                    + (dstrect->x*bpp);
        while ( pos >= 0x10000L ) {
            srcp = (Uint8 *)src->pixels + (src_row*src->pitch)
                                        + (srcrect->x*bpp);
            ++src_row;
            pos -= 0x10000L;
        }
#ifdef USE_ASM_STRETCH
        switch (bpp) {
            case 3:
            copy_row3(srcp, srcrect->w, dstp, dstrect->w);
            break;
            default:
#ifdef __GNUC__
            __asm__ __volatile__ (
                        " call _copy_row "
            : "=&D" (u1), "=&S" (u2)
            : "0" (dstp), "1" (srcp)
            : "memory" );
#else
#ifdef WIN32
        { void *code = &copy_row;
            __asm {
                push edi
                push esi

                mov edi, dstp
                mov esi, srcp
                call dword ptr code

                pop esi
                pop edi
            }
        }
#else
#error Need inline assembly for this compiler
#endif
#endif /* __GNUC__ */
            break;
        }
#else
        switch (bpp) {
            case 1:
            copy_row1(srcp, srcrect->w, dstp, dstrect->w);
            break;
            case 2:
            copy_row2((Uint16 *)srcp, srcrect->w,
                      (Uint16 *)dstp, dstrect->w);
            break;
            case 3:
            copy_row3(srcp, srcrect->w, dstp, dstrect->w);
            break;
            case 4:
            copy_row4((Uint32 *)srcp, srcrect->w,
                      (Uint32 *)dstp, dstrect->w);
            break;
        }
#endif
        pos += inc;
    }
    return(0);
}

static void GAL_SoftStretchHelper (GAL_Surface *src, GAL_Rect *srcrect,
        GAL_Surface *dst, GAL_Rect *dstrect, DWORD op)
{
    GAL_Surface *new_surf;
    GAL_Rect new_rc = { 0, 0, dstrect->w, dstrect->h };

    new_surf = GAL_CreateRGBSurface (GAL_SWSURFACE, dstrect->w, dstrect->h,
            GAL_BitsPerPixel(src),
            GAL_RMask(src), GAL_GMask(src), GAL_BMask(src), GAL_AMask(src));
    if (new_surf == NULL) {
        _WRN_PRINTF ("Failed to create a software surface\n");
        return;
    }

    GAL_SoftStretch (src, srcrect, new_surf, &new_rc);

    if ((src->flags & GAL_SRCALPHA) == GAL_SRCALPHA) {
        new_surf->flags |= GAL_SRCALPHA;
        new_surf->format->alpha = src->format->alpha;
    }
    if ((src->flags & GAL_SRCPIXELALPHA) == GAL_SRCPIXELALPHA) {
        new_surf->flags |= GAL_SRCPIXELALPHA;
    }
    if ((src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
        new_surf->flags |= GAL_SRCCOLORKEY;
        new_surf->format->colorkey = src->format->colorkey;
    }
    GAL_UpperBlit (new_surf, &new_rc, dst, dstrect, op);
    GAL_FreeSurface (new_surf);
}

static int GAL_StretchBltLegacy (GAL_Surface *src, GAL_Rect *srcrect,
        GAL_Surface *dst, GAL_Rect *dstrect, DWORD op)
{
    GAL_Rect clipped_srcrect, clipped_dstrect;

    if (dstrect->x >= dst->clip_rect.x &&
            dstrect->y >= dst->clip_rect.y &&
            dstrect->x + dstrect->w <= dst->clip_rect.x + dst->clip_rect.w &&
            dstrect->y + dstrect->h <= dst->clip_rect.y + dst->clip_rect.h) {
        clipped_srcrect = *srcrect;
        clipped_dstrect = *dstrect;
    }
    else if (GAL_IntersectRect (dstrect, &dst->clip_rect, &clipped_dstrect)) {
        float ratio_x = (dstrect->x + dstrect->w - clipped_dstrect.x) * 1.0f / dstrect->w;
        float ratio_y = (dstrect->y + dstrect->h - clipped_dstrect.y) * 1.0f / dstrect->h;

        clipped_srcrect.x = srcrect->x + srcrect->w * (1.0f - ratio_x);
        clipped_srcrect.y = srcrect->y + srcrect->h * (1.0f - ratio_y);

        ratio_x = (clipped_dstrect.w) * 1.0f / dstrect->w;
        ratio_y = (clipped_dstrect.h) * 1.0f / dstrect->h;
        clipped_srcrect.w = (srcrect->w * ratio_x);
        clipped_srcrect.h = (srcrect->h * ratio_y);

        if (clipped_srcrect.w <= 0 || clipped_srcrect.h <= 0 ||
                clipped_dstrect.w <= 0 || clipped_dstrect.h <= 0)
            return -1;
    }
    else {
        return -1;
    }

    if (GAL_RMask (src) != GAL_RMask (dst)
            || GAL_GMask (src) != GAL_GMask (dst)
            || GAL_BMask (src) != GAL_BMask (dst)
            || GAL_AMask (src) != GAL_AMask (dst)) {
        GAL_SoftStretchHelper (src, &clipped_srcrect, dst, &clipped_dstrect, op);
        return 0;
    }

    GAL_SoftStretch (src, &clipped_srcrect, dst, &clipped_dstrect);
    return 0;
}

#ifdef _MGUSE_PIXMAN
#include <pixman.h>
#include <math.h>

int GAL_StretchBlt (GAL_Surface *src, GAL_Rect *srcrect,
        GAL_Surface *dst, GAL_Rect *dstrect, DWORD cop)
{
    pixman_image_t *src_img = NULL, *dst_img = NULL, *msk_img = NULL;
    pixman_op_t op;
    uint32_t alpha_bits;
    int retv = -1;
    
    if (!GAL_CheckPixmanFormat (src, dst) || (src->flags & GAL_SRCCOLORKEY))
        return GAL_StretchBltLegacy (src, srcrect, dst, dstrect, cop);

    assert (src->tmp_data);
    assert (dst->tmp_data);

    if ((src->flags & GAL_SRCALPHA) && src->format->alpha != GAL_ALPHA_OPAQUE) {
        memset (&alpha_bits, src->format->alpha, sizeof(alpha_bits));
        msk_img = pixman_image_create_bits_no_clear (PIXMAN_a8, 1, 1, &alpha_bits, 4);
        if (msk_img)
            pixman_image_set_repeat (msk_img, PIXMAN_REPEAT_NORMAL);
    }

    if (cop == COLOR_BLEND_LEGACY) {
        if ((src->flags & GAL_SRCPIXELALPHA) && src->format->Amask && src != dst) {
            op = PIXMAN_OP_OVER;
        }
        else {
            op = PIXMAN_OP_SRC;
        }
    }
    else {
        op = cop & ~COLOR_BLEND_FLAGS_MASK;
    }

    src_img = pixman_image_create_bits_no_clear ((pixman_format_code_t)src->tmp_data,
            src->w, src->h,
            (uint32_t *)((char*)src->pixels + src->pixels_off),
            src->pitch);
    if (src_img == NULL)
        goto out;

    if (dst != src) {
        dst_img = pixman_image_create_bits_no_clear ((pixman_format_code_t)dst->tmp_data,
                dst->w, dst->h,
                (uint32_t *)((char*)dst->pixels + dst->pixels_off),
                dst->pitch);
        if (dst_img == NULL)
            goto out;
    }
    else {
        dst_img = src_img;
    }

    {
        double fscale_x = dstrect->w * 1.0 / srcrect->w;
        double fscale_y = dstrect->h * 1.0 / srcrect->h;
        pixman_f_transform_t ftransform;
        pixman_transform_t transform;
        pixman_fixed_t *params;
        int n_params;
        pixman_region32_t clip_region;

        _DBG_PRINTF ("srcrect: %d, %d, %dx%d; dstrect: %d, %d, %dx%d; scale: %f x %f; cliprect: %d, %d, %dx%d\n",
                srcrect->x, srcrect->y, srcrect->w, srcrect->h,
                dstrect->x, dstrect->y, dstrect->w, dstrect->h,
                fscale_x, fscale_y,
                dst->clip_rect.x, dst->clip_rect.y, dst->clip_rect.w, dst->clip_rect.h);

        pixman_f_transform_init_identity (&ftransform);
        pixman_f_transform_scale (&ftransform, NULL, 1.0/fscale_x, 1.0/fscale_y);
        pixman_transform_from_pixman_f_transform (&transform, &ftransform);
        pixman_image_set_transform (src_img, &transform);

        fscale_x = hypot (ftransform.m[0][0], ftransform.m[0][1]) / ftransform.m[2][2];
        fscale_y = hypot (ftransform.m[1][0], ftransform.m[1][1]) / ftransform.m[2][2];
        params = pixman_filter_create_separable_convolution (
            &n_params,
            pixman_double_to_fixed(fscale_x),
            pixman_double_to_fixed(fscale_y),
            PIXMAN_KERNEL_IMPULSE,
            PIXMAN_KERNEL_IMPULSE,
            PIXMAN_KERNEL_BOX,
            PIXMAN_KERNEL_BOX,
            4, 4);
        pixman_image_set_filter (src_img, PIXMAN_FILTER_SEPARABLE_CONVOLUTION, params, n_params);
        free (params);

        pixman_region32_init_rect (&clip_region,
                dst->clip_rect.x, dst->clip_rect.y, dst->clip_rect.w, dst->clip_rect.h);
        pixman_image_set_clip_region32 (dst_img, &clip_region);

        retv = 0;
        pixman_image_composite32 (op, src_img, msk_img, dst_img,
                srcrect->x, srcrect->y,
                0, 0,
                dstrect->x, dstrect->y, 
                srcrect->w, srcrect->h);

        pixman_region32_fini (&clip_region);
    }

out:
    if (msk_img)
        pixman_image_unref (msk_img);

    if (src_img)
        pixman_image_unref (src_img);

    if (dst_img != src_img && dst_img)
        pixman_image_unref (dst_img);

    return retv;
}

#else   /* defined _MGUSE_PIXMAN */

int GAL_StretchBlt (GAL_Surface *src, GAL_Rect *srcrect,
        GAL_Surface *dst, GAL_Rect *dstrect, DWORD op)
{
    return GAL_StretchBltLegacy (src, srcrect, dst, dstrect, op);
}

#endif   /* not defined  _MGUSE_PIXMAN */
