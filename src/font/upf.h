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
** upf.h: the head file of Unicode Prerendered Font operation set.
**
** Create date: 2008/01/26 mgwang
*/

#ifndef GUI_FONT_UPF_H
    #define GUI_FONT_UPF_H

#define LEN_VERSION_MAX       10
#define LEN_VENDER_NAME_MAX   12
#define LEN_DEVFONT_NAME_MAX  127

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define FM_SMOOTH   1

typedef struct 
{
    /* BBOX information of the glyph */
    Sint8 bearingx;
    Sint8 bearingy;
    Uint8 width;
    Uint8 height;
    /* The pitch of the bitmap (bytes of one scan line) */
    Uint8 pitch;
    /* Paddings for alignment */
    Uint8 padding1;
    /* advance value of the glyph */
    Uint8 advance;
    /* Paddings for alignment */
    Uint8 padding2;
    /* the offset of the glyph bitmap from the start of this font file. */
    Uint32 bitmap_offset;
}UPFGLYPH;

typedef struct 
{
    Uint32 min;
    Uint32 max;
    Uint32 less_offset;
    Uint32 more_offset;
    Uint32 glyph_offset;
} UPFNODE;

typedef struct 
{
    Uint8      width;
    Uint8      height;
    Sint8      ascent;
    Sint8      descent;
    Uint8      max_width;
    Uint8      underline_pos;
    Uint8      underline_width;
    Sint8      leading;
    Uint8      mono_bitmap;
    Uint8      reserved[3];
    void*      root_dir;
    Uint32     file_size;
} UPFINFO;

typedef struct 
{
    char     ver_info [LEN_VERSION_MAX];
    char     vender_name [LEN_VENDER_NAME_MAX];
    Uint16   endian;

    char     font_name [LEN_DEVFONT_NAME_MAX + 1];

    Uint8    width;
    Uint8    height;
    Sint8    ascent;
    Sint8    descent;

    Uint8    max_width;
    Uint8    min_width;
    Sint8    left_bearing;
    Sint8    right_bearing;

    Uint8    underline_pos;
    Uint8    underline_width;
    Sint8    leading;
    Uint8    mono_bitmap;

    Uint32   off_nodes;
    Uint32   off_glyphs;
    Uint32   off_bitmaps;
    Uint32   len_nodes;
    Uint32   len_glyphs;
    Uint32   len_bitmaps;
    Uint32   nr_glyph;
    Uint32   nr_zones;
    Uint32   font_size;

} UPFV1_FILE_HEADER;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_FONT_QPF_H */


