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
** varbitmap.h: the head file of raw bitmap font operation set.
** 
*/

#ifndef GUI_FONT_VARBITMAP_H
    #define GUI_FONT_VARBITMAP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


typedef struct _VBF_BBOX
{
        char x, y, w, h;
} VBF_BBOX;

typedef struct
{
    char ver_info [4];  /* font version*/
    const char* name;         /* font name*/

    char max_width;     /* The max width in pixels. */
    char ave_width;     /* The average width in pixels. */
    char height;        /* The height in pixels. */
    char descent;       /* The pixels below the base line. */

    int first_glyph;    /* The glyph number of the first character in this font.*/
    int last_glyph;     /* The glyph number of the last character in this font.*/
    int def_glyph;      /* The glyph number of the default character in this font.*/

    unsigned int font_size; /* used by mmap. It should be zero for in-core vbfs. */

    const VBF_BBOX* bbox;     /* The pointer to the glyph bounding box array or NULL. */

    const char* advance_x;    /* The pointer to the glyph advance x array. 
                           If bbox is NULL, the array contains the width of each glyph.
                           If advance_x is NULL, the glyph has the universal width, 
                           that's max_width. */
    const char* advance_y;    /* The pointer to the glyph advance y array.
                           If bbox is NULL, advance_y should be NULL.
                           If bbox is not NULL and advance_y is NULL, 
                           all glyph has the universal advance_y, that is, zero. */

    const unsigned int* bits_offset;      /* The pointer to the glyph bitmap offset array 
                                       whose number will be used to fetch glyph bitmap 
                                       from bits array or NULL. */
    const unsigned char* all_glyph_bits;  /* The 8-bit right-padded bitmap data for all glyphs. */
} VBFINFO;

#define VBF_LEN_VERSION_INFO 10
#define VBF_LEN_VENDER_INFO  12

#define VBF_VERSION3       "vbf-3.0**"

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FONT_VARBITMAP_H

