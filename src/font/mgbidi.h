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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

/*
** mgbidi.h:
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

#ifndef _MGFONT_GENERAL_BIDI_H
#define _MGFONT_GENERAL_BIDI_H

#ifdef _MGCHARSET_UNICODE_FULL
#define MGBIDI_UNICODE_CHARS   0x110000
#else
#define MGBIDI_UNICODE_CHARS   0xFFFE
#endif

/* some general macros */
#define STRINGIZE(symbol) #symbol

/* A few macros for working with bits */
#define BIDI_TEST_BITS(x, mask) (((x) & (mask)) ? 1 : 0)

#define BIDI_INCLUDE_BITS(x, mask) ((x) | (mask))

#define BIDI_EXCLUDE_BITS(x, mask) ((x) & ~(mask))

#define BIDI_SET_BITS(x, mask)	((x) |= (mask))

#define BIDI_UNSET_BITS(x, mask)	((x) &= ~(mask))

#define BIDI_ADJUST_BITS(x, mask, cond)	\
	((x) = ((x) & ~(mask)) | ((cond) ? (mask) : 0))

#define BIDI_ADJUST_AND_TEST_BITS(x, mask, cond)	\
	BIDI_TEST_BITS(BIDI_ADJUST_BITS((x), (mask), (cond)), (mask))

/* The maximum embedding level value assigned by explicit marks */
#define BIDI_MAX_EXPLICIT_LEVEL         125

/* The maximum *number* of different resolved embedding levels: 0-126 */
#define BIDI_MAX_RESOLVED_LEVELS        127

/* The maximum *number* of nested brackets: 0-63 */
#define BIDI_MAX_NESTED_BRACKET_PAIRS   63

#define BIDI_SENTINEL               -1

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
    BidiType type;
    BidiLevel level;
    BidiLevel isolate_level;
    Uint8 bracket_type;
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

/**
 * \fn gbidi_get_paragraph_dir
 * \brief get base paragraph direction
 *
 * This function finds the base direction of a single paragraph,
 * as defined by rule P2 of the Unicode Bidirectional Algorithm available at
 * http://www.unicode.org/reports/tr9/#P2.
 *
 * You typically do not need this function as
 * gbidi_get_paragraph_els() knows how to compute base direction
 * itself, but you may need this to implement a more sophisticated paragraph
 * direction handling.  Note that you can pass more than a paragraph to this
 * function and the direction of the first non-neutral paragraph is returned,
 * which is a very good heuristic to set direction of the neutral paragraphs
 * at the beginning of text.  For other neutral paragraphs, you better use the
 * direction of the previous paragraph.
 *
 * \param bidi_types the pointer to the BidiType array as returned by
 *      gbidi_get_bidi_types()
 * \param len The length of bidi_types
 *
 * \return Base pargraph direction. No weak paragraph direction is returned,
 * only BIDI_PGDIR_LTR, BIDI_PGDIR_RTL, or BIDI_PGDIR_ON.
 *
 */
int gbidi_get_paragraph_dir(const BidiType *bidi_types, int len);

/* gbidi_get_paragraph_els_ex - get bidi embedding levels of a paragraph
 *
 * This function finds the bidi embedding levels of a single paragraph,
 * as defined by the Unicode Bidirectional Algorithm available at
 * http://www.unicode.org/reports/tr9/.
 *
 * This function implements rules P2 to I1 inclusive, and parts 1 to 3 of L1,
 * except for rule X9 which is implemented in gbidi_remove_bidi_marks().
 * Part 4 of L1 is implemented in gbidi_reorder_line().
 *
 * There are a few macros defined in gbidi-bidi-types.h to work with this
 * embedding levels.
 *
 * \param bidi_types the pointer to the BidiType array as returned by
 *      gbidi_get_bidi_types()
 * \param bracket_types The pointer to a Uint8 which contains the
        bracket types as returned by gbidi_get_bracket_types()
 * \param len The length of the list.
 * \param base_dir requested and resolved paragraph base direction
 * \param embedding_levels The pointer to a buffer which will restore
 *      the embedding levels
 *
 * \return The Maximum level found plus one, or zero if any error occurred
 * (memory allocation failure most probably).
 */
BidiLevel gbidi_get_paragraph_els_ex(const BidiType *bidi_types,
    const Uint8* bracket_types, int len,
    int *base_dir, BidiLevel *embedding_levels);

/* gbidi_reorder_line - reorder a line of logical string to visual
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
 * set by gbidi_get_paragraph_els().  Also note that the embedding
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
 *      gbidi_get_bidi_types()
 * \param bracket_types The pointer to a Uint8 which contains the
        bracket types as returned by gbidi_get_bracket_types()
 * \param len The length of the list.
 * \param off The input offset of the beginning of the line in the paragraph.
 * \param base_dir The resolved paragraph base direction.
 * \param embedding_levels The embedding levels, as returned by
        gbidi_get_paragraph_els()
 * \param visual_str
 * \param extra The pointer to the extra array to reorder; can be NULL.
 * \param cb_reverse_extra The callback function to reverse the extra array.
 * \param map a map of string indices which is reordered to reflect
 *      where each glyph ends up.
 *
 * \return Maximum level found in this line plus one, or zero if any error
 * occurred (memory allocation failure most probably).
 */
BidiLevel gbidi_reorder_line(Uint32 flags, const BidiType *bidi_types,
    int len, int off, int base_dir, BidiLevel *embedding_levels,
    Achar32 *visual_str, int *map,
    void* extra, CB_REVERSE_EXTRA cb_reverse_extra);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* !_MGFONT_GENERAL_BIDI_H */

