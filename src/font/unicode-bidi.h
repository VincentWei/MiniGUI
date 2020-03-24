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
 *   Copyright (C) 2002~2019, Beijing FMSoft Technologies Co., Ltd.
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
** unicode-bidi.h:
**
** A general implementation of UNICODE BIDIRECTIONAL ALGORITHM for MiniGUI
**
**  https://www.unicode.org/reports/tr9/
**
** Created by WEI Yongming at 2019/03/08
**
** This implementation is based on LGPL'd FriBidi:
**
**  https://github.com/fribidi/fribidi
*/

#ifndef _MGFONT_UNICODE_BIDI_H
#define _MGFONT_UNICODE_BIDI_H

#ifdef _MGCHARSET_UNICODE_FULL
#define UNIBIDI_UNICODE_CHARS   0x110000
#else
#define UNIBIDI_UNICODE_CHARS   0xFFFE
#endif

/* Some special Unicode characters */

/* Bidirectional marks */
#define BIDI_CHAR_LRM        0x200E
#define BIDI_CHAR_RLM        0x200F
#define BIDI_CHAR_LRE        0x202A
#define BIDI_CHAR_RLE        0x202B
#define BIDI_CHAR_PDF        0x202C
#define BIDI_CHAR_LRO        0x202D
#define BIDI_CHAR_RLO        0x202E
#define BIDI_CHAR_LRI        0x2066
#define BIDI_CHAR_RLI        0x2067
#define BIDI_CHAR_FSI        0x2068
#define BIDI_CHAR_PDI        0x2069

/* Line and Paragraph Separators */
#define BIDI_CHAR_LS         0x2028
#define BIDI_CHAR_PS         0x2029

/* Arabic Joining marks */
#define BIDI_CHAR_ZWNJ       0x200C
#define BIDI_CHAR_ZWJ        0x200D

/* Hebrew and Arabic */
#define BIDI_CHAR_HEBREW_ALEF    0x05D0
#define BIDI_CHAR_ARABIC_ALEF    0x0627
#define BIDI_CHAR_ARABIC_ZERO    0x0660
#define BIDI_CHAR_PERSIAN_ZERO   0x06F0

/* Misc */
#define BIDI_CHAR_ZWNBSP         0xFEFF

/* Char we place for a deleted slot, to delete later */
#define BIDI_CHAR_FILL        BIDI_CHAR_ZWNBSP

/* some general macros */
#define STRINGIZE(symbol) #symbol

/* A few macros for working with bits */
#define BIDI_TEST_BITS(x, mask)     (((x) & (mask)) ? 1 : 0)

#define BIDI_INCLUDE_BITS(x, mask)  ((x) | (mask))

#define BIDI_EXCLUDE_BITS(x, mask)  ((x) & ~(mask))

#define BIDI_SET_BITS(x, mask)      ((x) |= (mask))

#define BIDI_UNSET_BITS(x, mask)    ((x) &= ~(mask))

#define BIDI_ADJUST_BITS(x, mask, cond)    \
    ((x) = ((x) & ~(mask)) | ((cond) ? (mask) : 0))

#define BIDI_ADJUST_AND_TEST_BITS(x, mask, cond)    \
    BIDI_TEST_BITS(BIDI_ADJUST_BITS((x), (mask), (cond)), (mask))

/* The maximum embedding level value assigned by explicit marks */
#define BIDI_MAX_EXPLICIT_LEVEL         125

/* The maximum *number* of different resolved embedding levels: 0-126 */
#define BIDI_MAX_RESOLVED_LEVELS        127

/* The maximum *number* of nested brackets: 0-63 */
#define BIDI_MAX_NESTED_BRACKET_PAIRS   63

#define BIDI_SENTINEL                   -1

enum _BidiJoiningTypeEnum {
# define _UNIBIDI_ADD_TYPE(TYPE,SYMBOL) \
    BIDI_JOINING_TYPE_##TYPE = BIDI_JOINING_TYPE_##TYPE##_VAL,
# include "unicode-joining-types-list.inc"
# undef _UNIBIDI_ADD_TYPE
    _UNIBIDI_JOINING_TYPE_JUNK    /* Don't use this */
};

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _BidiRun BidiRun;

struct _BidiRun {
    BidiRun *prev;
    BidiRun *next;

    /* Additional links for connecting the isolate tree */
    BidiRun *prev_isolate;
    BidiRun *next_isolate;

    int pos, len;
    BidiBracketType bracket_type;
    BidiType type;
    BidiLevel level;
    BidiLevel isolate_level;
};

#define swap(a,b) \
    do { \
        void *t; \
        (t) = (a); \
        (a) = (b); \
        (b) = (t); \
    } while(0)

#define merge_lists(a,b) \
    do { \
        swap((a)->prev->next, (b)->prev->next); \
        swap((a)->prev, (b)->prev); \
    } while(0)

#define delete_node(x) \
    do { \
        (x)->prev->next = (x)->next; \
        (x)->next->prev = (x)->prev; \
    } while(0)

#define insert_node_before(x, list) \
    do { \
        (x)->prev = (list)->prev; \
        (list)->prev->next = (x); \
        (x)->next = (list); \
        (list)->prev = (x); \
    } while(0)

#define move_node_before(x, list) \
    do { \
        if ((x)->prev) { \
          delete_node(x); \
        } \
        insert_node_before((x), (list)); \
    } while(0)

#define for_run_list(x, list) \
    for ((x) = (list)->next; (x)->type != BIDI_TYPE_SENTINEL; (x) = (x)->next)

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* !_MGFONT_UNICODE_BIDI_H */

