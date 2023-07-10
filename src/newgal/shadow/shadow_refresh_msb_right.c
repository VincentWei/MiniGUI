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
#include <stdlib.h>
#include <string.h>
#include "common.h"

#ifdef _MGGAL_SHADOW

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "error.h"
#include "shadow.h"

#if 0 /* use in-stack buffer since 5.0.14 */
gal_uint8* __gal_a_line;

int refresh_init (int pitch)
{
    __gal_a_line = malloc (pitch);
    if (__gal_a_line == NULL)
        return 0;
    return 0;
}

void refresh_destroy (void)
{
    if (__gal_a_line != NULL)
        free(__gal_a_line);
}
#endif

static unsigned char pixel_bit [] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

#if 0

/* Version for 1=White 0=Black */
static void put_one_line_1bpp (const BYTE* src_bits, BYTE* dst_line, int left, int width)
{
    int x, i;
    dst_line += left >> 3;

    for (x = 0; x < width; x += 8) {
        *dst_line = 0;
        for (i = 0; i < 8; i++) {
            if (*src_bits < 128)
                *dst_line |= pixel_bit [i];
            src_bits ++;
        }
        dst_line ++;
    }
}

#else

/* Version for 1=Black 0=White */
static void put_one_line_1bpp (const BYTE* src_bits, BYTE* dst_line, int left, int width)
{
    int x, i;
    dst_line += left >> 3;

    for (x = 0; x < width; x += 8) {
        *dst_line = 0;
        for (i = 0; i < 8; i++) {
            if (*src_bits >= 128)
                *dst_line |= pixel_bit [i];
            src_bits ++;
        }
        dst_line ++;
    }
}

#endif

static void put_one_line_2bpp (const BYTE* src_bits, BYTE* dst_line,
                int left, int width)
{
    int x;
    dst_line += left >> 2;

    for (x = 0; x < width; x += 4) {
        *dst_line = ((*src_bits) & 0xC0) >> 6 | ((*(src_bits+1))  & 0xC0) >> 4 |
                        ((*(src_bits+2))  & 0xC0) >> 2 | ((*(src_bits+3)) & 0xC0);
        src_bits += 4;
        dst_line ++;
    }
}

static void put_one_line_4bpp (const BYTE* src_bits, BYTE* dst_line,
                int left, int width)
{
    int x;
    dst_line += left >> 1;

    for (x = 0; x < width; x+=2) {
        *dst_line = (src_bits [0] >> 4) | (src_bits [1] & 0xF0);
        src_bits += 2;
        dst_line ++;
    }
}

void put_one_line_8bpp (const BYTE* src_bits, BYTE* dst_line, int left,
                int width, RealFBInfo *realfb_info)
{
    dst_line += left * realfb_info->depth/8;
    memcpy (dst_line, src_bits, width * (realfb_info->depth/8));
}

void round_rect(int depth, PRECT update_rect){
    if (depth == 1) {
        /* round x to be 8-aligned */
        update_rect->left = update_rect->left & ~0x07;
        update_rect->right = (update_rect->right + 7) & ~0x07;
    }
    else if (depth == 2) {
        /* round x to be 4-aligned */
        update_rect->left = update_rect->left & ~0x03;
        update_rect->right = (update_rect->right + 3) & ~0x03;
    }
    else if (depth == 4) {
        /* round x to be 2-aligned */
        update_rect->left = update_rect->left & ~0x01;
        update_rect->right = (update_rect->right + 1) & ~0x01;
    }else
        return;
}

void refresh_normal_msb_right (ShadowFBHeader * shadowfb_header, RealFBInfo *realfb_info,
                void* update)
{
    RECT src_update ;
    const BYTE* src_bits;
    BYTE* dst_line, *dst_bits;
    int width, height;
    int y;
    const BYTE* src_line;

    src_update.left = ((RECT*)update)->left;
    src_update.right = ((RECT*)update)->right;
    src_update.top = ((RECT*)update)->top;
    src_update.bottom = ((RECT*)update)->bottom;

    /* Round the update rectangle.  */
    round_rect(realfb_info->depth, &src_update);

    width = RECTW (src_update);
    height = RECTH (src_update);

    if (width <= 0 || height <= 0){
       return;
    }

    /* Copy the bits from Shadow FrameBuffer to console FrameBuffer */
    dst_bits = (gal_uint8*)realfb_info->fb;
    src_bits = (gal_uint8*)shadowfb_header + shadowfb_header->fb_offset;
    src_bits += src_update.top * shadowfb_header->pitch + src_update.left * shadowfb_header->depth/8;

    if (realfb_info->depth == 1) {
        dst_bits += src_update.top * realfb_info->pitch;

        for (y = 0; y < height; y++) {
            src_line = src_bits;
            dst_line = dst_bits;
            put_one_line_1bpp (src_line, dst_line, src_update.left, width);
            src_bits += shadowfb_header->pitch;
            dst_bits += realfb_info->pitch;
        }
        return;
    }
    else if (realfb_info->depth == 2){
        dst_bits += src_update.top * realfb_info->pitch;
        for (y = 0; y < height; y++) {
            src_line = src_bits;
            dst_line = dst_bits;
            put_one_line_2bpp (src_line, dst_line, src_update.left, width);
            src_bits += shadowfb_header->pitch;
            dst_bits += realfb_info->pitch;
        }
        return;
    }
    else if (realfb_info->depth == 4){
        dst_bits += src_update.top * realfb_info->pitch;
        for (y = 0; y < height; y++) {
            src_line = src_bits;
            dst_line = dst_bits;
            put_one_line_4bpp (src_line, dst_line, src_update.left, width);
            src_bits += shadowfb_header->pitch;
            dst_bits += realfb_info->pitch;
        }
        return;
    }
    else{
        dst_bits += src_update.top * realfb_info->pitch;

        for (y = 0; y < height; y++) {
            put_one_line_8bpp (src_bits, dst_bits, src_update.left, width, realfb_info);
            src_bits += shadowfb_header->pitch;
            dst_bits += realfb_info->pitch;
        }
    }
}

void _get_dst_rect_cw (RECT* dst_rect, const RECT* src_rect, RealFBInfo *realfb_info)
{
    dst_rect->left = realfb_info->width - src_rect->bottom;
    dst_rect->top = src_rect->left;
    dst_rect->right = realfb_info->width - src_rect->top;
    dst_rect->bottom = src_rect->right;
}

void _get_src_rect_cw (const RECT* dst_rect, RECT* src_rect, RealFBInfo *realfb_info)
{
    src_rect->left = dst_rect->top;
    src_rect->top = realfb_info->width - dst_rect->right;
    src_rect->right = dst_rect->bottom;
    src_rect->bottom = realfb_info->width - dst_rect->left;
}

void _get_dst_rect_ccw (RECT* dst_rect, const RECT* src_rect, RealFBInfo *realfb_info)
{
    dst_rect->left = src_rect->top;
    dst_rect->bottom = realfb_info->height - src_rect->left;
    dst_rect->right = src_rect->bottom;
    dst_rect->top = realfb_info->height - src_rect->right;
}

void _get_src_rect_ccw (const RECT* dst_rect, RECT* src_rect, RealFBInfo *realfb_info)
{
    src_rect->left = realfb_info->height - dst_rect->bottom;
    src_rect->top = dst_rect->left;
    src_rect->right = realfb_info->height - dst_rect->top;
    src_rect->bottom = dst_rect->right;
}

void refresh_cw_msb_right (ShadowFBHeader *shadowfb_header,
        RealFBInfo *realfb_info, void* update)
{
    RECT src_update = *(RECT*)update;
    RECT dst_update;
    const BYTE* src_bits;
    BYTE* dst_line;
    int dst_width, dst_height;
    int x, y;
    BYTE line_pixels[realfb_info->pitch];

    _get_dst_rect_cw (&dst_update, &src_update, realfb_info);

    /* Round the update rectangle.  */
    round_rect(realfb_info->depth, &dst_update);

    dst_width = RECTW (dst_update);
    dst_height = RECTH (dst_update);

    if (dst_width <= 0 || dst_height <= 0)
        return;

    _get_src_rect_cw (&dst_update, &src_update, realfb_info);

    /* Copy the bits from Shadow FrameBuffer to console FrameBuffer */
    src_bits = (gal_uint8*)shadowfb_header + shadowfb_header->fb_offset;
    src_bits += (src_update.bottom -1) * shadowfb_header->pitch
                + src_update.left * (shadowfb_header->depth / 8);

    dst_line = (gal_uint8*)realfb_info->fb + dst_update.top * realfb_info->pitch;

    for (x = 0; x < dst_height; x++) {
        /* Copy the bits from vertical line to horizontal line */
        const BYTE* ver_bits = src_bits;

        BYTE* hor_bits = line_pixels;

        switch (realfb_info->depth) {
            case 32:
                for (y = 0; y < dst_width; y++) {
                    *(gal_uint32*) hor_bits = *(gal_uint32*) ver_bits;
                    ver_bits -= shadowfb_header->pitch;
                    hor_bits += realfb_info->depth/8;
                }
                break;

           case 24:
                for (y = 0; y < dst_width; y++) {
                    hor_bits [0] = ver_bits [0];
                    hor_bits [1] = ver_bits [1];
                    hor_bits [2] = ver_bits [2];
                    ver_bits -= shadowfb_header->pitch;
                    hor_bits += realfb_info->depth/8;
                }
                break;

           case 15:
           case 16:
                for (y = 0; y < dst_width; y++) {
                    *(gal_uint16*) hor_bits = *(gal_uint16*) ver_bits;
                    ver_bits -= shadowfb_header->pitch;
                    hor_bits += (realfb_info->depth/8);
                }
                break;

           default:
                for (y = 0; y < dst_width; y++) {
                    *hor_bits = *ver_bits;
                    ver_bits -= shadowfb_header->pitch;
                    hor_bits += 1;
                }
                break;
        }

        switch (realfb_info->depth) {
            case 1:
                put_one_line_1bpp (line_pixels, dst_line, dst_update.left, dst_width);
                src_bits += 1;
                dst_line += realfb_info->pitch;
                break;

            case 2:
                put_one_line_2bpp (line_pixels, dst_line, dst_update.left, dst_width);
                src_bits +=1;
                dst_line += realfb_info->pitch;
                break;

            case 4:
                put_one_line_4bpp (line_pixels, dst_line, dst_update.left, dst_width);
                src_bits +=1;
                dst_line += realfb_info->pitch;
                break;

            default:
                put_one_line_8bpp (line_pixels, dst_line, dst_update.left, dst_width, realfb_info);
                src_bits += realfb_info->depth/8;
                dst_line += realfb_info->pitch;
                break;

        }
    }
}

void refresh_ccw_msb_right(ShadowFBHeader* shadowfb_header,
        RealFBInfo* realfb_info, void* update)
{
    RECT src_update = *(RECT*)update;
    RECT dst_update;
    const BYTE* src_bits;
    BYTE* dst_line;
    BYTE line_pixels[realfb_info->pitch];

    int dst_width, dst_height;
    int x, y;

    _get_dst_rect_ccw (&dst_update, &src_update, realfb_info);

    /* Round the update rectangle.  */
    round_rect(realfb_info->depth, &dst_update);

    dst_width = RECTW (dst_update);
    dst_height = RECTH (dst_update);

    if (dst_width <= 0 || dst_height <= 0)
        return;

    _get_src_rect_ccw (&dst_update, &src_update, realfb_info);

    /* Copy the bits from Shadow FrameBuffer to console FrameBuffer */
    src_bits = (gal_uint8*)shadowfb_header + shadowfb_header->fb_offset;
    src_bits += src_update.top * shadowfb_header->pitch
                + src_update.left * shadowfb_header->depth/8;

    dst_line = (gal_uint8*)realfb_info->fb + (dst_update.bottom - 1) * realfb_info->pitch;

    for (x = 0; x < dst_height; x++) {
        /* Copy the bits from vertical line to horizontal line */
        const BYTE* ver_bits = src_bits;
        BYTE* hor_bits = line_pixels;

        switch (realfb_info->depth) {
            case 32:
                for (y = 0; y < dst_width; y++) {
                    *(gal_uint32*) hor_bits = *(gal_uint32*) ver_bits;
                    ver_bits += shadowfb_header->pitch;
                    hor_bits += realfb_info->depth/8;
                }
                break;

            case 24:
                for (y = 0; y < dst_width; y++) {
                    hor_bits [0] = ver_bits [0];
                    hor_bits [1] = ver_bits [1];
                    hor_bits [2] = ver_bits [2];
                    ver_bits += shadowfb_header->pitch;
                    hor_bits += realfb_info->depth/8;
                }
                break;

            case 15:
            case 16:
                for (y = 0; y < dst_width; y++) {

                    *(gal_uint16*) hor_bits = *(gal_uint16*) ver_bits;
                    ver_bits += shadowfb_header->pitch;
                    hor_bits += realfb_info->depth/8;
                }
                break;

           default:
                for (y = 0; y < dst_width; y++) {
                    *hor_bits = *ver_bits;
                    ver_bits += shadowfb_header->pitch;
                    hor_bits += shadowfb_header->depth/8;
                }
                break;
        }

        switch (realfb_info->depth) {
            case 1:
                put_one_line_1bpp (line_pixels, dst_line, dst_update.left, dst_width);
                src_bits += 1;
                dst_line -= realfb_info->pitch;
                break;

            case 2:
                put_one_line_2bpp (line_pixels, dst_line, dst_update.left, dst_width);
                src_bits += 1;
                dst_line -= realfb_info->pitch;
                break;

            case 4:
                put_one_line_4bpp (line_pixels, dst_line, dst_update.left, dst_width);
                src_bits += 1;
                dst_line -= realfb_info->pitch;
                break;

            default:
                put_one_line_8bpp (line_pixels, dst_line, dst_update.left, dst_width,\
                                realfb_info);
                src_bits += realfb_info->depth/8;
                dst_line -= realfb_info->pitch;
                break;
        }
    }
}

void _get_dst_rect_hflip (RECT* src_rect, RealFBInfo *realfb_info)
{
    int old_left;

    old_left = src_rect->left;
    src_rect->left = realfb_info->width - src_rect->right;
    src_rect->right = realfb_info->width - old_left;
}

void refresh_hflip_msb_right (ShadowFBHeader* shadowfb_header, RealFBInfo* realfb_info, void* update)
{
    RECT src_update = *(RECT*)update;
    const BYTE* src_bits;
    BYTE* dst_line;
    int src_width, src_height;
    int x, y;
    BYTE line_pixels[realfb_info->pitch];

    /* Round the update rectangle.  */
    round_rect(realfb_info->depth, &src_update);

    src_width = RECTW (src_update);
    src_height = RECTH (src_update);

    /* Copy the bits from Shadow FrameBuffer to console FrameBuffer */
    src_bits = (gal_uint8*)shadowfb_header + shadowfb_header->fb_offset;

    src_bits += src_update.top * shadowfb_header->pitch + (src_update.right - 1)
                    * (shadowfb_header->depth / 8);

    dst_line = (gal_uint8 *)realfb_info->fb + src_update.top * realfb_info->pitch;

    for (x = 0; x < src_height; x++) {
        /* Copy the bits from vertical line to horizontal line */
        const BYTE* ver_bits = src_bits;
        BYTE* hor_bits = line_pixels;

        switch (realfb_info->depth) {
            case 32:
                for (y = 0; y < src_width; y++) {
                    *(gal_uint32*) hor_bits = *(gal_uint32*) ver_bits;
                    ver_bits -= realfb_info->depth/8;
                    hor_bits += realfb_info->depth/8;
                }
                break;

            case 24:
                for (y = 0; y < src_width; y++) {
                    hor_bits [0] = ver_bits [0];
                    hor_bits [1] = ver_bits [1];
                    hor_bits [2] = ver_bits [2];
                    ver_bits -= shadowfb_header->pitch;
                    hor_bits += realfb_info->depth/8;
                }
                break;

            case 15:
            case 16:
                for (y = 0; y < src_width; y++) {
                    *(gal_uint16*) hor_bits = *(gal_uint16*) ver_bits;
                    ver_bits -= realfb_info->depth/8;
                    hor_bits += realfb_info->depth/8;
                }
                break;

            default:

                 for (y = 0; y < src_width; y++) {
                    *hor_bits = *ver_bits;
                    ver_bits -= 1;
                    hor_bits += 1;
                 }
                break;
        }
        switch (realfb_info->depth) {
            case 1:
                put_one_line_1bpp (line_pixels, dst_line, realfb_info->width - src_update.right, src_width);
                src_bits += shadowfb_header->pitch;
                dst_line += realfb_info->pitch;
                break;

            case 2:
                put_one_line_2bpp (line_pixels, dst_line, realfb_info->width - src_update.right, src_width);
                src_bits += shadowfb_header->pitch;
                dst_line += realfb_info->pitch;
                break;

            case 4:
                put_one_line_4bpp (line_pixels, dst_line, realfb_info->width - src_update.right, src_width);
                src_bits += shadowfb_header->pitch;/* realfb_info->depth;*/
                dst_line += realfb_info->pitch;
                break;

            default:
                put_one_line_8bpp (line_pixels, dst_line, realfb_info->width - src_update.right,
                                src_width, realfb_info);
                src_bits += realfb_info->pitch;
                dst_line += realfb_info->pitch;
                break;
        }
    }
}

void _get_dst_rect_vflip (RECT* src_rect, RealFBInfo *realfb_info)
{
    int old_top;

    old_top = src_rect->top;
    src_rect->top = realfb_info->height - src_rect->bottom;
    src_rect->bottom = realfb_info->height - old_top;//src_rect->top;
}

void refresh_vflip_msb_right (ShadowFBHeader* shadowfb_header, RealFBInfo* realfb_info, void* update)
{
    RECT src_update = *(RECT*)update;
    const BYTE* src_bits;
    BYTE* dst_line;
    int src_width, src_height;
    int x, y;
    BYTE line_pixels[realfb_info->pitch];

    /* Round the update rectangle.  */
    round_rect (realfb_info->depth, &src_update);

    src_width = RECTW (src_update);
    src_height = RECTH (src_update);

    /* Copy the bits from Shadow FrameBuffer to console FrameBuffer */
    src_bits = (gal_uint8*)shadowfb_header + shadowfb_header->fb_offset;

    src_bits += (src_update.bottom - 1) * shadowfb_header->pitch + src_update.left
                    * shadowfb_header->depth / 8;

    dst_line = (gal_uint8*)realfb_info->fb + (realfb_info->height - src_update.bottom) * realfb_info->pitch;

    for (x = 0; x < src_height; x++) {
        /* Copy the bits from vertical line to horizontal line */
        const BYTE* ver_bits = src_bits;
        BYTE* hor_bits = line_pixels;

        switch (realfb_info->depth) {
            case 32:
                for (y = 0; y < src_width; y++) {
                    *(gal_uint32*) hor_bits = *(gal_uint32*) ver_bits;
                    ver_bits += realfb_info->depth/8;
                    hor_bits += realfb_info->depth/8;
                }
                break;


            case 24:
                for (y = 0; y < src_width; y++) {
                    hor_bits [0] = ver_bits [0];
                    hor_bits [1] = ver_bits [1];
                    hor_bits [2] = ver_bits [2];
                    ver_bits += shadowfb_header->pitch;
                    hor_bits += realfb_info->depth/8;
                }
                break;

            case 15:
            case 16:
                for (y = 0; y < src_width; y++) {
                    *(gal_uint16*) hor_bits = *(gal_uint16*) ver_bits;
                    ver_bits += realfb_info->depth/8;
                    hor_bits += realfb_info->depth/8;
                }
                break;

            default:
                for (y = 0; y < src_width; y++) {
                    *hor_bits = *ver_bits;
                    ver_bits += 1;
                    hor_bits += 1;
                }
                break;
        }

        switch (realfb_info->depth) {
            case 1:
                put_one_line_1bpp(line_pixels, dst_line, src_update.left,
                        src_width);
                src_bits -= shadowfb_header->pitch;
                dst_line += realfb_info->pitch;
                break;

            case 2:
                put_one_line_2bpp(line_pixels, dst_line, src_update.left,
                        src_width);
                src_bits -= shadowfb_header->pitch;
                dst_line += realfb_info->pitch;
                break;

            case 4:
                put_one_line_4bpp(line_pixels, dst_line, src_update.left,
                        src_width);
                src_bits -= shadowfb_header->pitch;/* realfb_info->depth;*/
                dst_line += realfb_info->pitch;
                break;

            default:
                put_one_line_8bpp(line_pixels, dst_line, src_update.left,
                        src_width, realfb_info);
                src_bits -= realfb_info->pitch;
                dst_line += realfb_info->pitch;
                break;
        }
    }
}

#endif /* _MGGAL_SHADOW */
