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
#ifndef QPF2C_FOR_MINIGUI3_H
#define QPF2C_FOR_MINIGUI3_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    typedef unsigned char Uint8;
    typedef signed char Sint8;
    typedef int BOOL;
    typedef unsigned char uchar;

#define FALSE   0
#define TRUE    1 


#ifdef __NOUNIX__
    typedef struct _QPF_GLYPHMETRICS 
#else
    typedef struct __attribute__ ((packed)) _QPF_GLYPHMETRICS 
#endif
    {
        Uint8 linestep;
        Uint8 width;
        Uint8 height;
        Uint8 padding;

        Sint8 bearingx;     /* Difference from pen position to glyph's left bbox */
        Uint8 advance;      /* Difference between pen positions */
        Sint8 bearingy;     /* Used for putting characters on baseline */

        Sint8 reserved;     /* Do not use */
    } QPF_GLYPHMETRICS;

    typedef struct _QPF_GLYPH
    {
        const QPF_GLYPHMETRICS* metrics;
        const unsigned char* data;
    } QPF_GLYPH;

    typedef struct _QPF_GLYPHTREE
    {
        unsigned int min, max;
        int less_tree_index;
        int more_tree_index;
        int glyph_index;
    } QPF_GLYPHTREE;

#ifdef __NOUNIX__
    typedef struct _QPFMETRICS
#else
    typedef struct __attribute__ ((packed)) _QPFMETRICS
#endif
    {
        Sint8 ascent, descent;
        Sint8 leftbearing, rightbearing;
        Uint8 maxwidth;
        Sint8 leading;
        Uint8 flags;
        Uint8 underlinepos;
        Uint8 underlinewidth;
        Uint8 reserved3;
    } QPFMETRICS;

    typedef struct 
    {
        unsigned int height;
        unsigned int width;

        unsigned int file_size;
        QPFMETRICS* fm;

        QPF_GLYPHTREE* tree;
    } QPFINFO;

    typedef struct _ARRAY{
        void* array;
        int item_size;
        int item_num;
        int alloced_num;
    } ARRAY;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
