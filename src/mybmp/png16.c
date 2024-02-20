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
/*
** png16.c: Low-level PNG file read routines (based on LibPNG 1.6)
**
** Current maintainer: Wei Yongming
**
** Create date: 2019/04/12
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "gdi.h"

#ifdef _MGIMAGE_PNG
#if _MGLIBPNG_VER == 16

#include <png.h>

typedef struct {
    png_structp png_ptr;
    png_infop info_ptr;
    BOOL      interlaced;
} PngInitInfo;


typedef unsigned char uch;
typedef unsigned long ulg;

static void my_read_data_fn (png_structp png_ptr, png_bytep data,
                png_size_t length)
{
    png_size_t check;
    png_voidp io_ptr = png_get_io_ptr(png_ptr);

    check = (png_size_t)MGUI_RWread ((MG_RWops*)io_ptr,
            data, (png_size_t)1, length);

    if (check != length)
        png_error(png_ptr, "Read Error");
}

#define _PNG_SIG_ERROR  1
#define _PNG_MEM_ALLOC  2
#define _PNG_LIB_ERROR  3

#define _PNG_INIT_OK    0

static int readpng_init (png_structpp png_pptr, png_infopp info_pptr,
                MG_RWops* src, png_uint_32* width, png_uint_32* height,
                int* bitdepth, int* colortype)
{
    uch sig[9];

    MGUI_RWread (src, sig, 8, 1);
    if (!png_check_sig (sig, 8)) {
#ifdef PNG_DEBUG
        sig[8] = '\0';
        fprintf (stderr, "%s: check error!\n", sig);
#endif
        return _PNG_SIG_ERROR;
    } else {
#ifdef PNG_DEBUG
        sig[8]='\0';
        fprintf (stderr, "png sig is %s.\n", sig);
#endif
    }

    *png_pptr = png_create_read_struct (PNG_LIBPNG_VER_STRING,
                    NULL, NULL, NULL );
    if (!*png_pptr){
        return _PNG_MEM_ALLOC;
    }

    *info_pptr = png_create_info_struct ( *png_pptr );
    if ( !*info_pptr ) {
        png_destroy_read_struct (png_pptr, NULL,NULL);
        return _PNG_MEM_ALLOC;
    }

#ifdef PNG_SETJMP_SUPPORTED
    if (setjmp(png_jmpbuf(*png_pptr))) {
        png_destroy_read_struct (png_pptr, info_pptr, NULL);
        return _PNG_LIB_ERROR;
    }
#endif

    png_set_read_fn (*png_pptr, (void*)src, my_read_data_fn);

    png_set_sig_bytes ( *png_pptr, 8 );

    png_read_info ( *png_pptr, *info_pptr );

    png_get_IHDR ( *png_pptr, *info_pptr, width, height, bitdepth,
         colortype, NULL, NULL, NULL );

    return _PNG_INIT_OK;
}

#if 0
static void get_transparent_info (png_structp png_ptr, png_infop info_ptr,
                int bit_depth, int color_type, MYBITMAP* bmp)
{
    png_bytep trans;
    int num_trans;
    png_color_16p trans_values;

    if (png_get_tRNS (png_ptr, info_ptr, &trans, &num_trans, &trans_values)
                        && num_trans) {
        gal_uint8 r = 0, g = 0, b = 0;

        switch (color_type) {
            case PNG_COLOR_TYPE_GRAY:
                if (bit_depth < 8)
                    trans_values->gray = trans_values->gray << (8 - bit_depth);
                r = g = b = trans_values->gray;
                break;

            case PNG_COLOR_TYPE_PALETTE:
            {
                png_colorp pal;
                int num_pal;

                if (png_get_PLTE (png_ptr, info_ptr, &pal, &num_pal) && *trans < num_pal) {
                    r = pal [*trans].red;
                    g = pal [*trans].green;
                    b = pal [*trans].blue;
                }
                break;
            }

            case PNG_COLOR_TYPE_RGB:
                r = trans_values->red >> 8;
                g = trans_values->green >> 8;
                b = trans_values->blue >> 8;
                break;
        }

        bmp->flags |= MYBMP_TRANSPARENT;
        bmp->transparent = MakeRGB (r, g, b);
   }
}
#endif

void* __mg_init_png(MG_RWops * fp, MYBITMAP * mybmp, RGB * pal)
{
    int rc;
    png_structpp png_ptr;
    png_infopp info_ptr;
    png_uint_32 width;
    png_uint_32 height;
    int bit_depth, intent, color_type;
    double screen_gamma;
    PngInitInfo * png_data = NULL;

    png_data = (PngInitInfo *)malloc(sizeof(PngInitInfo));
    if (!png_data) {
        _ERR_PRINTF ("__mg_init_png: failed to allocate memory for PngInitInfo\n");
        return NULL;
    }

    png_ptr = &png_data->png_ptr;
    info_ptr = &png_data->info_ptr;

    if ((rc = readpng_init(png_ptr, info_ptr, fp, &width, &height,
                                    &bit_depth, &color_type)) != 0) {
        switch (rc) {
        case _PNG_SIG_ERROR:
        case _PNG_MEM_ALLOC:
        case _PNG_LIB_ERROR:
        default:
            _ERR_PRINTF ("__mg_init_png: failed to call readpng_init: %d\n",
                rc);
            goto err;
        }
    }

    mybmp->w = width;
    mybmp->h = height;
    mybmp->frames = 1;

    /* tell libpng to strip 16 bit/color files down to 8 bits/color */
    if (bit_depth == 16)
        png_set_strip_16(*png_ptr);

    /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
     * byte into separate bytes (useful for paletted and grayscale images).
     */
    if (bit_depth < 8)
        png_set_packing(*png_ptr);

    /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
    if ((color_type == PNG_COLOR_TYPE_GRAY
            || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(*png_ptr);
    }

    /* Expand paletted colors into true RGB triplets */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(*png_ptr);
    else if (color_type == PNG_COLOR_TYPE_GRAY
            || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(*png_ptr);

    if (png_get_valid(*png_ptr, *info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha (*png_ptr);
        mybmp->flags |= MYBMP_FLOW_DOWN | MYBMP_ALPHA | MYBMP_TYPE_RGBA;
        color_type = PNG_COLOR_TYPE_RGBA;
    }

    if (color_type & PNG_COLOR_MASK_ALPHA) {
        mybmp->flags |= MYBMP_FLOW_DOWN | MYBMP_ALPHA | MYBMP_TYPE_RGBA;
    }
    else {
        mybmp->flags |= MYBMP_FLOW_DOWN | MYBMP_TYPE_RGB;
    }

    screen_gamma = 2.2;

    if (png_get_sRGB (*png_ptr, *info_ptr, &intent))
        png_set_gamma (*png_ptr, screen_gamma, 0.45455);
    else {
        double image_gamma;
        if (png_get_gAMA (*png_ptr, *info_ptr, &image_gamma))
            png_set_gamma (*png_ptr, screen_gamma, image_gamma);
        else
            png_set_gamma (*png_ptr, screen_gamma, 0.45455);
    }

    png_read_update_info (*png_ptr, *info_ptr);
    mybmp->depth = png_get_bit_depth (*png_ptr, *info_ptr);
    mybmp->depth *= png_get_channels (*png_ptr, *info_ptr);
    if (mybmp->depth == 24)
        mybmp->flags |= MYBMP_RGBSIZE_3;
    else if (mybmp->depth == 32)
        mybmp->flags |= MYBMP_RGBSIZE_4;
    else {
        _ERR_PRINTF ("__mg_init_png: bad pixel_depth: %d\n", mybmp->depth);
        goto err;
    }

    mybmp->pitch = png_get_rowbytes (*png_ptr, *info_ptr);

    if (png_get_interlace_type(*png_ptr, *info_ptr) == PNG_INTERLACE_ADAM7)
        png_data->interlaced = TRUE;
    else
        png_data->interlaced = FALSE;

    return png_data;

err:
    _ERR_PRINTF ("__mg_init_png error!\n");
    free (png_data);
    return NULL;
}

void __mg_cleanup_png(void * init_info)
{
    PngInitInfo * info = (PngInitInfo *)init_info;

    if (!info)
        return;

    if (info->png_ptr && info->info_ptr) {
        png_destroy_read_struct(&info->png_ptr, &info->info_ptr, NULL);
        info->png_ptr = NULL;
        info->info_ptr = NULL;
    }

    free(init_info);
}

int __mg_load_png (MG_RWops * fp, void * init_info, MYBITMAP * my_bmp,
                CB_ONE_SCANLINE cb, void * context)
{
    PngInitInfo * info = (PngInitInfo *)init_info;
    png_bytep rp, bits = (png_bytep)my_bmp->bits;
    png_uint_32 i, image_height, p, pass;

    pass = png_set_interlace_handling(info->png_ptr);
    if (my_bmp->flags & MYBMP_LOAD_ALLOCATE_ONE)
        p = 0;
    else
        p = my_bmp->pitch;

    image_height = png_get_image_height(info->png_ptr, info->info_ptr);
    //info->png_ptr->num_rows = image_height;

    if (info->interlaced) {
        bits = (png_bytep)malloc(my_bmp->pitch * image_height);
        if (!bits)
            return ERR_BMP_MEM;
        p = my_bmp->pitch;
    }

#ifdef PNG_SETJMP_SUPPORTED
    if (setjmp(png_jmpbuf (info->png_ptr))) {
        if (info->interlaced) {
            free (bits);
        }
        return ERR_BMP_LOAD;
    }
#endif

    while (pass--) {
        rp = bits;
        for (i = 0; i < image_height; i++, rp += p) {
            png_read_row(info->png_ptr, rp, NULL);
            if (cb && !p) cb(context, my_bmp, i);
        }
    }

    png_read_end(info->png_ptr, NULL);

    if (info->interlaced) {
        rp = (png_bytep)my_bmp->bits;
        my_bmp->bits = (BYTE *)bits;
        if (cb) for (i = 0; i < image_height; i++, my_bmp->bits += p)
            cb(context, my_bmp, i);
        else
            memcpy(rp, bits, my_bmp->pitch * image_height);
        my_bmp->bits = rp;
        free(bits);
    }

    return ERR_BMP_OK;
}

BOOL __mg_check_png (MG_RWops* fp)
{
    unsigned char sig [9];

    MGUI_RWread (fp, sig, 8, 1);
    return png_check_sig (sig, 8);
}

static void png_error_callback (png_structp png, png_const_charp error_msg)
{
    int *error = png_get_error_ptr (png);

    *error = ERR_BMP_CANT_SAVE;

#ifdef PNG_SETJMP_SUPPORTED
    longjmp (png_jmpbuf (png), 1);
#endif
}

static void png_warning_callback (png_structp png, png_const_charp error_msg)
{
}

static void png_output_flush_fn (png_structp png_ptr)
{
}

static void my_write_data_fn (png_structp png_ptr, png_bytep data,
                png_size_t length)
{
    png_size_t check;
    png_voidp io_ptr = png_get_io_ptr(png_ptr);

    if (MGUI_RWwrite((MG_RWops*)io_ptr, data, 1, length) !=
            (size_t) length) {
        png_error(png_ptr, "Write Error");
    }
}

typedef BYTE* (*MYBITMAP_get_pixel_row)(unsigned int cinfo,
        MYBITMAP* mybmp,RGB* pal);

static BYTE* MYBITMAP_get_pixel_row_pal16(unsigned int next_scanline,
                       MYBITMAP* mybmp, RGB* pal)
{
    RGB rgb0, rgb1;

    BYTE *bits = mybmp->bits + next_scanline * mybmp->pitch;
    int nr_data = mybmp->w * 4;
    BYTE *data = (BYTE *)malloc(nr_data);
    uint8_t alpha = 0xFF;

    for (int i = 0, j = 0, end_i = (mybmp->w + 1) >> 1; i < end_i; ++i) {

        rgb0 = pal[ (bits[ i ] & 0XF0) >> 4 ];
        rgb1 = pal[  bits[ i ] & 0X0F ];

        data[ j ++ ] = rgb0.r;
        data[ j ++ ] = rgb0.g;
        data[ j ++ ] = rgb0.b;
        data[ j ++ ] = alpha;

        data[ j ++ ] = rgb1.r;
        data[ j ++ ] = rgb1.g;
        data[ j ++ ] = rgb1.b;
        data[ j ++ ] = alpha;
    }

    return data;
}

static BYTE* MYBITMAP_get_pixel_row_pal256(unsigned int next_scanline,
                       MYBITMAP* mybmp, RGB* pal)
{
    RGB rgb;
    BYTE *bits = mybmp->bits + next_scanline * mybmp->pitch;

    int nr_data = mybmp->w * 4;
    BYTE *data = (BYTE *)malloc(nr_data);
    uint8_t alpha = 0xFF;

    for (int i = 0, j = 0; i < mybmp->w; i++, j += 4) {
        rgb = pal [bits[i] ];

        data[0] = rgb.r;
        data[1] = rgb.g;
        data[2] = rgb.b;
        data[3] = alpha;
    }

    return data;
}

#define RGB_FROM_RGB565(pixel, r, g, b)                         \
{                                                               \
    r = (((pixel&0xF800)>>11)<<3);                              \
    g = (((pixel&0x07E0)>>5)<<2);                               \
    b = ((pixel&0x001F)<<3);                                    \
}

static BYTE* MYBITMAP_get_pixel_row_RGB565(unsigned int next_scanline,
                       MYBITMAP* mybmp, RGB* pal)
{
    Uint16* bits = (Uint16*)(mybmp->bits + next_scanline * mybmp->pitch);

    int nr_data = mybmp->w * 4;
    BYTE* data = (BYTE *)malloc(nr_data);
    uint8_t alpha = 0xFF;
    for (int i = 0, j = 0; i < nr_data; i += 4, j++) {
        uint8_t *b = data + i;
        Uint16 *pixel = bits + j;
        RGB_FROM_RGB565(pixel[j], b[0], b[1], b[2])
        b[3] = alpha;
    }

    return data;
}

static BYTE* MYBITMAP_get_pixel_row_RGB(unsigned int next_scanline,
                       MYBITMAP* mybmp, RGB* pal)
{
    BYTE *bits = mybmp->bits + next_scanline * mybmp->pitch;

    int nr_data = mybmp->w * 4;
    BYTE *data = (BYTE *)malloc(nr_data);
    uint8_t alpha = 0xFF;

    for (int i = 0, j = 0; i < nr_data; i += 4, j += 3) {
        uint8_t *b = data + i;
        BYTE *pixel = bits + j;
        b[0] = pixel[2];
        b[1] = pixel[1];
        b[2] = pixel[0];
        b[3] = alpha;
    }

    return data;
}

static BYTE* MYBITMAP_get_pixel_row_BGR(unsigned int next_scanline,
                       MYBITMAP* mybmp, RGB* pal)
{
    BYTE *bits = mybmp->bits + next_scanline * mybmp->pitch;

    int nr_data = mybmp->w * 4;
    BYTE *data = (BYTE *)malloc(nr_data);
    uint8_t alpha = 0xFF;

    for (int i = 0, j = 0; i < nr_data; i += 4, j += 3) {
        uint8_t *b = data + i;
        BYTE *pixel = bits + j;
        b[0] = pixel[0];
        b[1] = pixel[1];
        b[2] = pixel[2];
        b[3] = alpha;
    }

    return data;
}

static BYTE* MYBITMAP_get_pixel_row_RGBA(unsigned int next_scanline,
                       MYBITMAP* mybmp, RGB* pal)
{
    BYTE *bits = mybmp->bits + next_scanline * mybmp->pitch;

    int nr_data = mybmp->w * 4;
    BYTE *data = (BYTE *)malloc(nr_data);

    for (int i = 0; i < nr_data; i += 4) {
        uint8_t *b = data + i;
        BYTE *pixel = bits + i;

        uint8_t alpha = pixel[3];
        if (alpha == 0) {
            b[0] = b[1] = b[2] = b[3] = 0;
        } else {
            b[0] = (pixel[2] * 255 + alpha / 2) / alpha; /* red */
            b[1] = (pixel[1] * 255 + alpha / 2) / alpha; /* green */
            b[2] = (pixel[0] * 255 + alpha / 2) / alpha; /* blue */
            b[3] = alpha;
        }
    }

    return data;
}

int __mg_save_png (MG_RWops* fp, MYBITMAP* bmp, RGB* pal)
{
    png_struct *png;
    png_info *info;
    png_byte **volatile rows = NULL;
    MYBITMAP_get_pixel_row get_row;
    png_color_16 white;
    int png_color_type;
    int retcode = ERR_BMP_CANT_SAVE;
    int bpc;
    int bmp_type;

    rows = calloc(bmp->h, sizeof(png_byte*));
    if (rows == NULL) {
        retcode = ERR_BMP_CANT_SAVE;
        goto failed;
    }

    png = png_create_write_struct (PNG_LIBPNG_VER_STRING, &retcode,
            png_error_callback, png_warning_callback);
    if (png == NULL) {
        retcode = ERR_BMP_MEM;
        goto failed_clear_rows;
    }

    info = png_create_info_struct (png);
    if (info == NULL) {
        retcode = ERR_BMP_MEM;
        goto failed_clear_struct;
    }

#ifdef PNG_SETJMP_SUPPORTED
    if (setjmp (png_jmpbuf (png))) {
        goto failed_clear_struct;
    }
#endif

    png_set_write_fn (png, fp, my_write_data_fn, png_output_flush_fn);

    bmp_type = bmp->flags & MYBMP_TYPE_MASK;

    switch(bmp->depth) {
    case 4:
        get_row = MYBITMAP_get_pixel_row_pal16;
        break;
    case 8:
        get_row = MYBITMAP_get_pixel_row_pal256;
        break;
    case 16:
        get_row = MYBITMAP_get_pixel_row_RGB565;
        break;
    case 24:
        if(MYBMP_TYPE_RGB == bmp_type)
            get_row = MYBITMAP_get_pixel_row_RGB;
        else
            get_row = MYBITMAP_get_pixel_row_BGR;
        break;
    case 32:
        get_row = MYBITMAP_get_pixel_row_RGBA;
        break;
    default:
        _ERR_PRINTF("MYBMP>PNG: invalid MYBITMAP.depth = %d\n", bmp->depth);
        goto failed_clear_struct;
    }

    for (int i = 0; i < bmp->h; i++) {
        rows[i] = get_row(i, bmp, pal);
    }
    bpc = 8;
    png_color_type = PNG_COLOR_TYPE_RGB_ALPHA;

    png_set_IHDR (png, info,
            bmp->w,
            bmp->h, bpc,
            png_color_type,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT);

    white.gray = (1 << bpc) - 1;
    white.red = white.blue = white.green = white.gray;
    png_set_bKGD (png, info, &white);

    png_write_info (png, info);

    png_write_image (png, rows);
    png_write_end (png, info);

    retcode = ERR_BMP_OK;

failed_clear_struct:
    png_destroy_write_struct (&png, &info);
failed_clear_rows:
    for (int i = 0; i < bmp->h; i++) {
        if (rows[i]) {
            free(rows[i]);
        }
    }
    free (rows);
failed:
    return retcode;
}

#endif /* _MGLIBPNG_VER == 16 */
#endif /* _MGIMAGE_PNG */

