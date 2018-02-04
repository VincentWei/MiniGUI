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
** bidiedit_impl.h: the head file of BidiEdit Control module.
**
** Create date: 2008/03/10
*/

#ifndef BIDI_EDIT_IMPL_H_
#define BIDI_EDIT_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

typedef struct _GLYPHBUF
{   
    GLYPHMAPINFO* glyph_map;     /* glyph info array */ 
    int*  glyphs;                /* glyph array  */
    int   glyphs_len;            /* glyph array len or buffer used size */
        
    int   blocksize;             /* block size, a block is an allocate unit */
    int   glyphs_buffsize;       /* glyph string buffer size */        
    int   glyphmap_buffsize;     /* glyph map buffer size */        
} GLYPHBUF;

typedef struct tagBIDISLEDITDATA
{
    StrBuffer str_content;  // string text buffer
    GLYPHBUF glyph_content; // glyph string buffer

    int     editPos;        // current edit position
    int     selStart;       // selection start position
    int     selEnd;         // selection end position

    DWORD   status;         // status of box

    int     nContX;         // content x offset position
    int     nContW;         // content width
    RECT    rcVis;          // content rect
    int     nVisW;          // visible area width
    int     startx;         // text x display position
    int     starty;         // text y display position

    int     leftMargin;     // left margin
    int     topMargin;      // top margin
    int     rightMargin;    // right margin
    int     bottomMargin;   // bottom margin

    int     nBlockSize;     /* string buffer block size */
    char    *tiptext;       // tip text

    int     hardLimit;      // hard limit

    int     passwdChar;     // password character

    int     changed;        // change flag

}BIDISLEDITDATA;

typedef BIDISLEDITDATA* PBIDISLEDITDATA;

static inline void* 
glyphbuf_alloc (GLYPHBUF* glyph_buff, int len, int block_size)
{
    glyph_buff->glyphs_buffsize = (len + (block_size - len % block_size))
                                     * sizeof (int);
    glyph_buff->glyphs = malloc (glyph_buff->glyphs_buffsize);

    glyph_buff->glyphmap_buffsize = (len + (block_size - len % block_size)) 
                                    * sizeof (GLYPHMAPINFO);
    glyph_buff->glyph_map = malloc (glyph_buff->glyphmap_buffsize);

    glyph_buff->glyphs_len = len;
    glyph_buff->blocksize = block_size;

    return glyph_buff->glyphs;
}

static inline void glyphbuf_free (GLYPHBUF *glyph_buff)
{
    if (glyph_buff) {
        free (glyph_buff->glyphs);
        free (glyph_buff->glyph_map);
    }
}

static inline void* glyphbuf_realloc (GLYPHBUF *glyph_buff, int len)
{
    if (len * sizeof(int) > glyph_buff->glyphs_buffsize || 
        len * sizeof(int) < glyph_buff->glyphs_buffsize - glyph_buff->blocksize)    {
        /* realloc glyph string mem */
        glyph_buff->glyphs_buffsize = (len + (glyph_buff->blocksize - 
                            len % glyph_buff->blocksize))* sizeof(int);
        glyph_buff->glyphs = realloc (glyph_buff->glyphs, 
                                glyph_buff->glyphs_buffsize);

        /* realloc glyph map mem */
        glyph_buff->glyphmap_buffsize = (len + (glyph_buff->blocksize - 
                        len % glyph_buff->blocksize))* sizeof(GLYPHMAPINFO);
        glyph_buff->glyph_map = realloc (glyph_buff->glyph_map, 
                        glyph_buff->glyphmap_buffsize);
    }

    return glyph_buff->glyphs;
}

BOOL RegisterBIDISLEditControl (void);

#ifdef __cplusplus
}
#endif

#endif /* BIDI_EDIT_IMPL_H_ */

