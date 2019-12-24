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
** nullfont.c: The Null Font operation set.
**
** Created by WEI Yongming at 2019/03/27
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"

#include "minigui.h"
#include "gdi.h"

#include "devfont.h"
#include "charset.h"
#include "fontname.h"

#define NUF_WIDTH   8
#define NUF_HEIGHT  1

static DWORD get_glyph_bmptype (LOGFONT* logfont, DEVFONT* devfont)
{
    return DEVFONTGLYPHTYPE_MONOBMP;
}

static int get_max_width (LOGFONT* logfont, DEVFONT* devfont)
{
   return NUF_WIDTH;
}

static int get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
    return NUF_WIDTH;
}

static int get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
    return GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect, int df_slot)
{
    if (df_slot >= 0 && df_slot < MAXNR_DEVFONTS)
        SET_DEVFONT_SCALE(logfont, df_slot, expect);

    return expect;
}

static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
    return GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_descent (LOGFONT* logfont, DEVFONT* devfont)
{
    return 0;
}

static int get_glyph_bbox (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value,
            int* px, int* py, int* pwidth, int* pheight)
{
    int scale = GET_DEVFONT_SCALE (logfont, devfont);

    if (pwidth)
        *pwidth     = NUF_WIDTH;
    if (pheight)
        *pheight    = NUF_HEIGHT;
    if (py)
        *py        -= scale;

    return NUF_WIDTH;
}

static int get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont,
                Glyph32 glyph_value, int* px, int* py)
{
    if (px)
        *px += NUF_WIDTH;
    return NUF_WIDTH;
}

static unsigned char null_bitmap = 0x18;

static const void* get_glyph_monobitmap (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value, SIZE* sz, int* pitch, unsigned short* scale)
{
    if (sz) {
        sz->cx = 8;
        sz->cy = 1;
    }

    if (pitch)
        *pitch = 1;

    if (scale)
        *scale = GET_DEVFONT_SCALE (logfont, devfont);

    return &null_bitmap;
}

static BOOL is_glyph_existed (LOGFONT* logfont, DEVFONT* devfont, Glyph32 gv)
{
    return TRUE;
}

static int is_rotatable (LOGFONT* logfont, DEVFONT* devfont, int rot_desired)
{
    return 0;
}

static void* load_font_data (DEVFONT* devfont, const char* fontname,
        const char* filename)
{
    return &null_bitmap;
}

static void unload_font_data (DEVFONT* devfont, void* data)
{
    if (data != &null_bitmap) {
        _WRN_PRINTF("data is not null_bitmap\n");
    }
}

FONTOPS __mg_null_font_ops = {
    get_glyph_bmptype,
    get_ave_width,
    get_max_width,
    get_font_height,
    get_font_size,
    get_font_ascent,
    get_font_descent,

    is_glyph_existed,
    get_glyph_advance,
    get_glyph_bbox,

    get_glyph_monobitmap,
    NULL,
    NULL,

    NULL,
    NULL,
    NULL,
    is_rotatable,
    load_font_data,
    unload_font_data
};

