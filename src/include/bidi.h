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

#ifdef BIDI_DEBUG
    #define DBGLOG(s)      \
        do { if (1) { fprintf(stderr, s); } } while (0)
    #define DBGLOG2(s, t1)  \
        do { if (1) { fprintf(stderr, s, t1); } } while (0)
    #define DBGLOG3(s, t1,t2) \
        do { if (1) { fprintf(stderr, s, t1, t2); } } while (0)
    #define DBGLOG4(s, t1,t2,t3) \
        do { if (1) { fprintf(stderr, s, t1, t2, t3); } } while (0)
#else
    #define DBGLOG(s)
    #define DBGLOG2(s, t1)
    #define DBGLOG3(s, t1, t2)
    #define DBGLOG4(s, t1, t2, t3)
#endif

typedef struct _TYPERUN TYPERUN;

typedef struct _BIDICHAR_TYPE_MAP {
    // Starting code point of Unicode character
    Uchar32   chv;
    // Total number of Unicode characters of same type starting from chv
    Uint16    count;
    // Type of Unicode characters
    Uint16    type;
} BIDICHAR_TYPE_MAP;

#define _BIDI_BRACKET_NONE       0
#define _BIDI_BRACKET_CLOSE      1
#define _BIDI_BRACKET_OPEN       2

// NOTE: It is enough to use Uint16 for Unicode bracket table
typedef struct _BIDICHAR_BRACKET {
    Uint16 chv;
    Sint8  bracket_off;
    Uint8  type;
} BIDICHAR_BRACKET;

// NOTE: It is enough to use Uint16 for Unicode mirror map
typedef struct _BIDICHAR_MIRROR_MAP {
    Uint16 chv;
    Uint16 mirrored;
} BIDICHAR_MIRROR_MAP;

Achar32* __mg_legacy_bidi_achars_reorder (const CHARSETOPS* charset_ops,
        Achar32* achars, int len, int pel,
        void* extra, CB_REVERSE_ARRAY cb_reverse_extra);

void __mg_legacy_bidi_get_embeddlevels (const CHARSETOPS* charset_ops,
        Achar32* achars, int len, int pel, Uint8* embedding_levels, Uint8 type);

Uint32 __mg_legacy_bidi_str_base_dir (const CHARSETOPS* charset_ops,
        Achar32* achars, int len);

#if 0
// Deprecated
Achar32* __mg_legacy_bidi_map_reorder (const CHARSETOPS* charset_ops,
        Achar32* achars, int len, GLYPHMAPINFO* map, int pel);

Achar32* __mg_legacy_bidi_index_reorder (const CHARSETOPS* charset_ops,
        Achar32* achars, int len, int* index_map, int pel);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FONT_CHARSET_BIDI_H
