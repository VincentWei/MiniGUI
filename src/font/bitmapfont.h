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
** bitmapfont.h: the head file of Bitmap Font operation set.
**
** Create date: 2007/09/22
*/

#ifndef GUI_FONT_BITMAP_H
    #define GUI_FONT_BITMAP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _GLYPHTREENODE {
    unsigned int min, max;  
    int nr_glyphs; 
    int glyph_width;
    BITMAP *data;
    
    struct _GLYPHTREENODE *left;
    struct _GLYPHTREENODE *right;
        
} GLYPHTREENODE;

extern FONTOPS __mg_bitmap_font_ops;

typedef struct _BMPFONTINFO
{
    char name[LEN_UNIDEVFONT_NAME + 1]; /* font name */
    int height;                 /* character height. */
    int ave_width;              /* average width of character */
    int max_width;              /* max width of character */
    
    GLYPHTREENODE* tree;        /* The pointer to glyph tree */
    int node_cnt;               /* The node count of AVL tree */
          
} BMPFONTINFO;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_FONT_BITMAP_H */


