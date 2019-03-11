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

#define MGBIDI_BRACKET_NONE       0
#define MGBIDI_BRACKET_CLOSE      1
#define MGBIDI_BRACKET_OPEN       2

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
        void* extra, CB_REVERSE_EXTRA cb_reverse_extra);

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

#ifdef _MGCHARSET_UNICODE

/**
 * \fn __mg_unicode_bidi_get_paragraph_dir
 * \brief get base paragraph direction
 *
 * This function finds the base direction of a single paragraph,
 * as defined by rule P2 of the Unicode Bidirectional Algorithm available at
 * http://www.unicode.org/reports/tr9/#P2.
 *
 * You typically do not need this function as
 * __mg_unicode_bidi_get_paragraph_els() knows how to compute base direction
 * itself, but you may need this to implement a more sophisticated paragraph
 * direction handling.  Note that you can pass more than a paragraph to this
 * function and the direction of the first non-neutral paragraph is returned,
 * which is a very good heuristic to set direction of the neutral paragraphs
 * at the beginning of text.  For other neutral paragraphs, you better use the
 * direction of the previous paragraph.
 *
 * \param bidi_types the pointer to the BidiType array as returned by
 *      __mg_unicode_bidi_get_bidi_types()
 * \param len The length of bidi_types
 *
 * \return Base pargraph direction. No weak paragraph direction is returned,
 * only BIDI_PGDIR_LTR, BIDI_PGDIR_RTL, or BIDI_PGDIR_ON.
 *
 */
int __mg_unicode_bidi_get_paragraph_dir(const BidiType *bidi_types, int len);

/**
 * \fn __mg_unicode_bidi_get_paragraph_els
 * \brief Get bidi embedding levels of a paragraph
 *
 * This function finds the bidi embedding levels of a single paragraph,
 * as defined by the Unicode Bidirectional Algorithm available at
 * http://www.unicode.org/reports/tr9/.
 *
 * This function implements rules P2 to I1 inclusive, and parts 1 to 3 of L1.
 * Part 4 of L1 is implemented in __mg_unicode_bidi_reorder_line().
 *
 * There are a few macros defined in mgbidi-bidi-types.h to work with this
 * embedding levels.
 *
 * \param bidi_types the pointer to the BidiType array as returned by
 *      __mg_unicode_bidi_get_bidi_types()
 * \param bracket_types The pointer to a Uint8 which contains the
        bracket types as returned by UCharGetBracketTypes()
 * \param len The length of the list.
 * \param base_dir requested and resolved paragraph base direction
 * \param embedding_levels The pointer to a buffer which will restore
 *      the embedding levels
 *
 * \return The Maximum level found plus one, or zero if any error occurred
 * (memory allocation failure most probably).
 */
BidiLevel __mg_unicode_bidi_get_paragraph_els(const BidiType *bidi_types,
    const BidiBracketType* bracket_types, int len,
    int *base_dir, BidiLevel *embedding_levels);

/**
 * \fn __mg_unicode_bidi_reorder_line
 * \brief Reorder a line of logical string to visual
 *
 * This function reorders the characters in a line of text from logical to
 * final visual order.
 *
 * This function implements part 4 of rule L1, and rules
 * L2 and L3 of the Unicode Bidirectional Algorithm available at
 * http://www.unicode.org/reports/tr9/#Reordering_Resolved_Levels.
 *
 * As a side effect it also sets position maps if not NULL.
 *
 * You should provide the resolved paragraph direction and embedding levels as
 * set by __mg_unicode_bidi_get_paragraph_els(). Also note that the embedding
 * levels may change a bit.  To be exact, the embedding level of any sequence
 * of white space at the end of line is reset to the paragraph embedding level
 * (That is part 4 of rule L1).
 *
 * Note that the bidi types and embedding levels are not reordered.
 * You can reorder these (or any other) arrays using the map later.
 * The user is responsible to initialize map to something sensible,
 * like an identity mapping, or pass NULL if no map is needed.
 *
 * There is an optional part to this function, which is whether non-spacing
 * marks for right-to-left parts of the text should be reordered to come after
 * their base characters in the visual string or not.
 *
 * Most rendering engines expect this behavior, but console-based systems
 * for example do not like it. This is controlled by the
 * BIDI_FLAG_REORDER_NSM flag. The flag is on in BIDI_FLAGS_DEFAULT.
 *
 * \param flags The reorder flags.
 * \param bidi_types the pointer to the BidiType array as returned by
 *      __mg_unicode_bidi_get_bidi_types()
 * \param bracket_types The pointer to a BidiBracketType array which
 *      contains the bracket types as returned by UCharGetBracketTypes()
 * \param len The length of the list.
 * \param off The input offset of the beginning of the line in the paragraph.
 * \param base_dir The resolved paragraph base direction.
 * \param embedding_levels The embedding levels, as returned by
        __mg_unicode_bidi_get_paragraph_els()
 * \param visual_str The Uchar32 string will be reordered.
 * \param extra The pointer to the extra array to reorder; can be NULL.
 * \param cb_reverse_extra The callback function to reverse the extra array.
 * \param map a map of string indices which is reordered to reflect
 *      where each glyph ends up.
 *
 * \return Maximum level found in this line plus one, or zero if any error
 * occurred (memory allocation failure most probably).
 */
BidiLevel __mg_unicode_bidi_reorder_line(Uint32 flags,
    const BidiType *bidi_types, int len, int off,
    int base_dir, BidiLevel *embedding_levels,
    Uchar32 *visual_str, int *map,
    void* extra, CB_REVERSE_EXTRA cb_reverse_extra);

#endif /* _MGCHARSET_UNICODE */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FONT_CHARSET_BIDI_H
