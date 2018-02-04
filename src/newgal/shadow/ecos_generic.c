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
**  ecos_generic.c: A subdriver of Shadow NEWGAL engine for eCos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_SHADOW 

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "shadow.h"

#ifdef __ECOS__

#include <cyg/hal/drv_api.h>
#include <cyg/io/io.h>
#include <cyg/hal/lcd_support.h>

static struct ecoslcd_info {
    short height, width;  // Pixels
    short depth;          // Depth (bits/pixel)
    short type;           // pixel type
    short rlen;           // Length of one raster line in bytes
    gal_uint8* fb;        // Frame buffer

    short Bpp;            // bytes per pixel
    gal_uint8* a_line;
    void (*put_one_line) (const BYTE* src_line, BYTE* dst_line, int x, int width);
} ecoslcd_info = {-1, -1};

static unsigned char pixel_bit [] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

static void put_one_line_1bpp (const BYTE* src_bits, BYTE* dst_line,
                int left, int width)
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

static void put_one_line_2bpp (const BYTE* src_bits, BYTE* dst_line,
                int left, int width)
{
    int x;
    dst_line += left >> 2;

    for (x = 0; x < width; x += 4) {
        *dst_line = (*src_bits >> 6); src_bits ++;
        *dst_line |= ((*src_bits >> 4) & 0x0C); src_bits ++;
        *dst_line |= ((*src_bits >> 2) & 0x30); src_bits ++;
        *dst_line |= ((*src_bits) & 0xC0); src_bits ++;
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

static void put_one_line_8bpp (const BYTE* src_bits, BYTE* dst_line,
                int left, int width)
{
    dst_line += left * ecoslcd_info.Bpp;

    memcpy (dst_line, src_bits, width * ecoslcd_info.Bpp);
}

#ifdef _COOR_TRANS

#   if _ROT_DIR_CW == 1

static void _get_dst_rect (RECT* dst_rect, const RECT* src_rect)
{
    dst_rect->left = ecoslcd_info.width - src_rect->bottom;
    dst_rect->top = src_rect->left;
    dst_rect->right = ecoslcd_info.width - src_rect->top;
    dst_rect->bottom = src_rect->right;
}

static void a_refresh_cw (_THIS, const RECT* update)
{
    RECT src_update = *update;
    RECT dst_update;
    const BYTE* src_bits;
    BYTE* dst_line;
    int dst_width, dst_height;
    int x, y;

    _get_dst_rect (&dst_update, &src_update);

    /* Round the update rectangle.  */
    if (ecoslcd_info.depth == 1) {
        /* round x to be 8-aligned */
        dst_update.left = dst_update.left & ~0x07;
        dst_update.right = (dst_update.right + 7) & ~0x07;
    }
    else if (ecoslcd_info.depth == 2) {
        /* round x to be 4-aligned */
        dst_update.left = dst_update.left & ~0x03;
        dst_update.right = (dst_update.right + 3) & ~0x03;
    }
    else if (ecoslcd_info.depth == 4) {
        /* round x to be 2-aligned */
        dst_update.left = dst_update.left & ~0x01;
        dst_update.right = (dst_update.right + 1) & ~0x01;
    }

    dst_width = RECTW (dst_update);
    dst_height = RECTH (dst_update);

    if (dst_width <= 0 || dst_height <= 0)
        return;

    /* Copy the bits from Shadow FrameBuffer to console FrameBuffer */
    src_bits = this->hidden->fb;
    src_bits += (src_update.bottom - 1) * this->hidden->pitch
                + src_update.left * ecoslcd_info.Bpp;

    dst_line = ecoslcd_info.fb + dst_update.top * ecoslcd_info.rlen;

    for (x = 0; x < dst_height; x++) {
        /* Copy the bits from vertical line to horizontal line */
        const BYTE* ver_bits = src_bits;
        BYTE* hor_bits = ecoslcd_info.a_line;

        switch (ecoslcd_info.depth) {
            case 32:
                for (y = 0; y < dst_width; y++) {
                    *(gal_uint32*) hor_bits = *(gal_uint32*) ver_bits;
                    ver_bits -= this->hidden->pitch;
                    hor_bits += ecoslcd_info.Bpp;
                }
                break;

           case 24:
                for (y = 0; y < dst_width; y++) {
                    hor_bits [0] = ver_bits [0];
                    hor_bits [1] = ver_bits [1];
                    hor_bits [2] = ver_bits [2];
                    ver_bits -= this->hidden->pitch;
                    hor_bits += ecoslcd_info.Bpp;
                }
                break;

           case 15:
           case 16:
                for (y = 0; y < dst_width; y++) {
                    *(gal_uint16*) hor_bits = *(gal_uint16*) ver_bits;
                    ver_bits -= this->hidden->pitch;
                    hor_bits += ecoslcd_info.Bpp;
                }
                break;

           default:
                for (y = 0; y < dst_width; y++) {
                    *hor_bits = *ver_bits;
                    ver_bits -= this->hidden->pitch;
                    hor_bits += ecoslcd_info.Bpp;
                }
                break;
        }

        ecoslcd_info.put_one_line (ecoslcd_info.a_line, dst_line, 
                        dst_update.left, dst_width);

        src_bits += ecoslcd_info.Bpp;
        dst_line += ecoslcd_info.rlen;
    }
}

#   else /* _ROT_DIR_CW == 1 */

static void _get_dst_rect (RECT* dst_rect, const RECT* src_rect)
{
    dst_rect->left = src_rect->top;
    dst_rect->bottom = ecoslcd_info.height - src_rect->left;
    dst_rect->right = src_rect->bottom;
    dst_rect->top = ecoslcd_info.height - src_rect->right;
}

static void a_refresh_ccw (_THIS, const RECT* update)
{
    RECT src_update = *update;
    RECT dst_update;
    const BYTE* src_bits;
    BYTE* dst_line;
    int dst_width, dst_height;
    int x, y;

    _get_dst_rect (&dst_update, &src_update);

    /* Round the update rectangle.  */
    if (ecoslcd_info.depth == 1) {
        /* round x to be 8-aligned */
        dst_update.left = dst_update.left & ~0x07;
        dst_update.right = (dst_update.right + 7) & ~0x07;
    }
    else if (ecoslcd_info.depth == 2) {
        /* round x to be 4-aligned */
        dst_update.left = dst_update.left & ~0x03;
        dst_update.right = (dst_update.right + 3) & ~0x03;
    }
    else if (ecoslcd_info.depth == 4) {
        /* round x to be 2-aligned */
        dst_update.left = dst_update.left & ~0x01;
        dst_update.right = (dst_update.right + 1) & ~0x01;
    }

    dst_width = RECTW (dst_update);
    dst_height = RECTH (dst_update);

    if (dst_width <= 0 || dst_height <= 0)
        return;

    /* Copy the bits from Shadow FrameBuffer to console FrameBuffer */
    src_bits = this->hidden->fb;
    src_bits += src_update.top * this->hidden->pitch 
                + src_update.left * ecoslcd_info.Bpp;

    dst_line = ecoslcd_info.fb + (dst_update.bottom - 1) * ecoslcd_info.rlen;

    for (x = 0; x < dst_height; x++) {
        /* Copy the bits from vertical line to horizontal line */
        const BYTE* ver_bits = src_bits;
        BYTE* hor_bits = ecoslcd_info.a_line;

        switch (ecoslcd_info.depth) {
            case 32:
                for (y = 0; y < dst_width; y++) {
                    *(gal_uint32*) hor_bits = *(gal_uint32*) ver_bits;
                    ver_bits += this->hidden->pitch;
                    hor_bits += ecoslcd_info.Bpp;
                }
                break;

           case 24:
                for (y = 0; y < dst_width; y++) {
                    hor_bits [0] = ver_bits [0];
                    hor_bits [1] = ver_bits [1];
                    hor_bits [2] = ver_bits [2];
                    ver_bits += this->hidden->pitch;
                    hor_bits += ecoslcd_info.Bpp;
                }
                break;

           case 15:
           case 16:
                for (y = 0; y < dst_width; y++) {
                    *(gal_uint16*) hor_bits = *(gal_uint16*) ver_bits;
                    ver_bits += this->hidden->pitch;
                    hor_bits += ecoslcd_info.Bpp;
                }
                break;

           default:
                for (y = 0; y < dst_width; y++) {
                    *hor_bits = *ver_bits;
                    ver_bits += this->hidden->pitch;
                    hor_bits += ecoslcd_info.Bpp;
                }
                break;

        }

        ecoslcd_info.put_one_line (ecoslcd_info.a_line, dst_line, 
                        dst_update.left, dst_width);

        src_bits += ecoslcd_info.Bpp;
        dst_line -= ecoslcd_info.rlen;
    }
}

#   endif /* _ROT_DIR_CW == 0 */

#else /* _COOR_TRANS */

static void a_refresh_normal (_THIS, const RECT* update)
{
    RECT src_update = *update;
    const BYTE* src_bits;
    BYTE* dst_line;
    int width, height;
    int y;

    if (ecoslcd_info.depth >= 8) {
        return;
    }

    /* Round the update rectangle.  */
    if (ecoslcd_info.depth == 1) {
        /* round x to be 8-aligned */
        src_update.left = src_update.left & ~0x07;
        src_update.right = (src_update.right + 7) & ~0x07;
    }
    else if (ecoslcd_info.depth == 2) {
        /* round x to be 4-aligned */
        src_update.left = src_update.left & ~0x03;
        src_update.right = (src_update.right + 3) & ~0x03;
    }
    else if (ecoslcd_info.depth == 4) {
        /* round x to be 2-aligned */
        src_update.left = src_update.left & ~0x01;
        src_update.right = (src_update.right + 1) & ~0x01;
    }

    width = RECTW (src_update);
    height = RECTH (src_update);

    if (width <= 0 || height <= 0)
        return;

    /* Copy the bits from Shadow FrameBuffer to console FrameBuffer */

    src_bits = this->hidden->fb;
    src_bits += src_update.top * this->hidden->pitch + src_update.left;

    dst_line = ecoslcd_info.fb + src_update.top * ecoslcd_info.rlen;

    for (y = 0; y < height; y++) {
        ecoslcd_info.put_one_line (src_bits, dst_line, src_update.left, width);
        src_bits += this->hidden->pitch;
        dst_line += ecoslcd_info.rlen;
    }
}

#endif /* !_COOR_TRANS */

static int a_getinfo (struct shadowlcd_info* lcd_info)
{
#ifdef _COOR_TRANS
    lcd_info->width = ecoslcd_info.height;
    lcd_info->height = ecoslcd_info.width;
    lcd_info->fb = NULL;

    ecoslcd_info.a_line = malloc (ecoslcd_info.width * ecoslcd_info.Bpp);
    if (ecoslcd_info.a_line == NULL)
        return 1;

#   if _ROT_DIR_CW == 1
    __mg_shadow_lcd_ops.refresh = a_refresh_cw;
#   else
    __mg_shadow_lcd_ops.refresh = a_refresh_ccw;
#   endif

#else /*  _COOR_TRANS */
    lcd_info->width = ecoslcd_info.width;
    lcd_info->height = ecoslcd_info.height;
    if (ecoslcd_info.depth >= 8) {
        lcd_info->fb = ecoslcd_info.fb;
        lcd_info->rlen = ecoslcd_info.rlen;
    }
    else
        lcd_info->fb = NULL;

    __mg_shadow_lcd_ops.refresh = a_refresh_normal;
#endif /* !_COOR_TRANS */

    lcd_info->bpp = ecoslcd_info.depth;

    switch (ecoslcd_info.depth) {
        case 1:
            ecoslcd_info.put_one_line = put_one_line_1bpp;
            break;
        case 2:
            ecoslcd_info.put_one_line = put_one_line_2bpp;
            break;
        case 4:
            ecoslcd_info.put_one_line = put_one_line_4bpp;
            break;
        default:
            ecoslcd_info.put_one_line = put_one_line_8bpp;
            break;
    }

    lcd_info->type = 0;
    return 0;
}

static int a_init (void)
{
    struct lcd_info lcd_info;

    lcd_init (16);
    if (lcd_getinfo (&lcd_info))
        return 1;

    memcpy (&ecoslcd_info, &lcd_info, sizeof (lcd_info));
    switch (ecoslcd_info.depth) {
        case 32: ecoslcd_info.Bpp = 4; break;
        case 24: ecoslcd_info.Bpp = 3; break;
        case 16: ecoslcd_info.Bpp = 2; break;
        case 15: ecoslcd_info.Bpp = 2; break;
        default: ecoslcd_info.Bpp = 1; break;
    }

    /* TODO: save original palette*/
    return 0;    /* success*/
}


static int a_release (void)
{
    /* TODO: reset hw palette */
  
    if (ecoslcd_info.a_line) {
        free (ecoslcd_info.a_line);
        ecoslcd_info.a_line = NULL;
    }

    return 0;
}

static void a_sleep (void)
{
    /* 20ms */
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 20 * 1000000;
    nanosleep (&ts, NULL);
}

struct shadow_lcd_ops __mg_shadow_lcd_ops = {
    a_init,
    a_getinfo,
    a_release,
    NULL,
    a_sleep,
    NULL
};

#endif /* __ECOS__ */

#endif /* _MGGAL_SHADOW */

