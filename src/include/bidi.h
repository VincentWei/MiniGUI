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
** charset-bidi.h: The charset Bidirectional Algorithm.
** 
** Current maintainer: Wei Yongming.
**
** Create date: 2008/01/23
*/

#ifndef GUI_FONT_CHARSET_BIDI_H
#define GUI_FONT_CHARSET_BIDI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

//#define ARABIC_DEBUG
#ifdef ARABIC_DEBUG
    #define DBGLOG(s)      do { if (1) { fprintf(stderr, s); } } while (0)
    #define DBGLOG2(s, t1)  do { if (1) { fprintf(stderr, s, t1); } } while (0)
    #define DBGLOG3(s, t1,t2)  do { if (1) { fprintf(stderr, s, t1,t2); } } while (0)
    #define DBGLOG4(s, t1,t2,t3)  do { if (1) { fprintf(stderr, s, t1,t2,t3); } } while (0)
#else
    #define DBGLOG(s)
    #define DBGLOG2(s, t1)
    #define DBGLOG3(s, t1, t2)
    #define DBGLOG4(s, t1, t2, t3)
#endif

typedef struct _TYPERUN TYPERUN;

typedef struct _BIDICHAR_MIRROR_MAP {
    Mchar32 chv;
    Mchar32 mirrored;
} BIDICHAR_MIRROR_MAP;

Glyph32* __mg_charset_bidi_glyphs_reorder (const CHARSETOPS* charset_ops,
        Glyph32* glyphs, int len, int pel,
        void* extra, CB_REVERSE_EXTRA cb_reverse_extra);

/*
Glyph32* __mg_charset_bidi_map_reorder (const CHARSETOPS* charset_ops,
        Glyph32* glyphs, int len, GLYPHMAPINFO* map, int pel);

Glyph32* __mg_charset_bidi_index_reorder (const CHARSETOPS* charset_ops,
        Glyph32* glyphs, int len, int* index_map, int pel);
*/

void __mg_charset_bidi_get_embeddlevels (const CHARSETOPS* charset_ops,
        Glyph32* glyphs, int len, int pel, Uint8* embedding_levels, Uint8 type);

Uint32 __mg_charset_bidi_str_base_dir (const CHARSETOPS* charset_ops,
        Glyph32* glyphs, int len);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FONT_CHARSET_BIDI_H
