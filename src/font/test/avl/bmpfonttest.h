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
** $Id$
**
** bitmapfont.h: Head file of bitmap font AVL tree unit test .
**
** Copyright (C) 2003 ~ 2007 Feynman Software
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2007/09/22
*/



#ifndef _BMPFONT_TEST_H
#define _BMPFONT_TEST_H

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>

typedef struct  _ELEMENT {
    char* start_mchar;
    int nr_glyphs;
    int glyph_width;
    BITMAP *data;  
} ELEMENT;

/* Insertion order. */
enum insert_order
{
        INS_RANDOM,                        /* Random order. */
        INS_ASCENDING,                   /* Ascending order. */
        INS_DESCENDING,                /* Descending order. */
        INS_CUSTOM,                        /* Custom order. */
        INS_CNT                               /* Number of insertion orders. */
};

/* Charset */
enum charset
{
        ISO8859,                        
        GB2312                   
};

/* Program options. */
struct test_options
{
    enum insert_order insert_order;     /* Insertion order. */
    enum charset charset;
    int node_cnt;
    int seed;
};


#endif
