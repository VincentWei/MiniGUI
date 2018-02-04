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
** type1.h: Type1 font support based on t1lib.
**
** Created by Song Lixin, 2000/8/29
*/

#ifndef GUI_TYPE1_H
    #define GUI_TYPE1_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct
{
  char *encfilename;
  char **encoding;
} encstruct;

typedef struct tagTYPE1GLYPHINFO {
    int font_id;
    //BBox 	font_bbox;
    //int 	ave_width;
    BOOL valid;
} TYPE1INFO, *PTYPE1INFO;

typedef struct tagTYPE1INSTANCEINFO {
    PTYPE1INFO  type1_info;
    int		rotation;/*in tenthdegrees*/
    T1_TMATRIX  * pmatrix;
    int 	size;
    int 	font_height;
    int 	font_ascent;
    int 	font_descent;

    int 	max_width;
    int 	ave_width;

    double 	csUnit2Pixel;
    /* 
     * last char or string's info
     * T1_SetChar, T1_SetString, T1_AASetSting, T1_AASetString all return a static
     * glyph pointer, we save the related infomation here for later use.
     * */
    char 	last_bitmap_char;
    char 	last_pixmap_char;
    char 	* last_bitmap_str;
    char 	* last_pixmap_str;
    int 	last_ascent;
    int 	last_descent;
    int 	last_leftSideBearing;
    int 	last_rightSideBearing;
    int 	last_advanceX;
    int 	last_advanceY;
    unsigned long 	last_bpp;
    char  *	last_bits;
    
} TYPE1INSTANCEINFO, *PTYPE1INSTANCEINFO;

#define TYPE1_INFO_P(devfont) ((TYPE1INFO*)(devfont->data))
#define TYPE1_INST_INFO_P(devfont) ((TYPE1INSTANCEINFO*)(devfont->data))

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_TYPE1_H

