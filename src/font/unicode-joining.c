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
** unicode-joining.c:
**      The implementation of Arabic joining algorithm
**
** Created by WEI Yongming at 2019/03/11
**
** This implementation is based on LGPL'd FriBidi:
**
**  https://github.com/fribidi/fribidi
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"

#ifdef _MGCHARSET_UNICODE

#include "minigui.h"
#include "gdi.h"

#include "unicode-bidi.h"

#ifdef _DEBUG
/*======================================================================
 *  For debugging, define some functions for printing joining types and
 *  properties.
 *----------------------------------------------------------------------*/

static char get_char_from_joining_type(BidiJoiningType j, BOOL visual)
{
    /* switch left and right if on visual run */
    if (visual & ((BIDI_JOINS_RIGHT (j) && !BIDI_JOINS_LEFT (j)) |
                (!BIDI_JOINS_RIGHT (j) && BIDI_JOINS_LEFT (j))))
        j ^= BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT;

#define _UNIBIDI_ADD_TYPE(TYPE,SYMBOL)  \
    if (BIDI_IS_JOINING_TYPE_##TYPE(j)) return SYMBOL;
#include "unicode-joining-types-list.inc"
#undef _UNIBIDI_ADD_TYPE

    return '?';
}

static void print_joining_types(const BidiLevel *embedding_levels,
        int len, const BidiJoiningType *jtypes)
{
    register int i;

    _DBG_PRINTF ("  Join. types: ");
    for (i = 0; i < len; i++)
        _DBG_PRINTF ("%c", get_char_from_joining_type (jtypes[i],
                    !BIDI_LEVEL_IS_RTL(embedding_levels[i])));
    _DBG_PRINTF ("\n");
}

#endif /* _DEBUG */

#define UNIBIDI_CONSISTENT_LEVEL(i) \
    (BIDI_IS_EXPLICIT_OR_BN (bidi_types[(i)]) \
     ? BIDI_SENTINEL \
     : embedding_levels[(i)])

#define BIDI_LEVELS_MATCH(i, j)    \
    ((i) == (j) || (i) == BIDI_SENTINEL || (j) == BIDI_SENTINEL)

void GUIAPI UBidiJoinArabic(const BidiType *bidi_types,
        const BidiLevel *embedding_levels, int len,
        BidiArabicProp *ar_props)
{
    if (len == 0) return;

#ifdef _DEBUG
    print_joining_types (embedding_levels, len, ar_props);
#endif /* _DEBUG */

    /* The joining algorithm turned out very very dirty :(.  That's what happens
     * when you follow the standard which has never been implemented closely
     * before.
     */

    /* 8.2 Arabic - Cursive Joining */
    _DBG_PRINTF ("Arabic cursive joining");
    {
        /* The following do not need to be initialized as long as joins is
         * initialized to FALSE.  We just do to turn off compiler warnings. */
        register int saved = 0;
        register BidiLevel saved_level = BIDI_SENTINEL;
        register BOOL saved_shapes = FALSE;
        register BidiArabicProp saved_joins_following_mask = 0;

        register BOOL joins = FALSE;
        register int i;

        for (i = 0; i < len; i++)
            if (!BIDI_IS_JOINING_TYPE_G (ar_props[i]))
            {
                register BOOL disjoin = FALSE;
                register BOOL shapes = BIDI_ARAB_SHAPES (ar_props[i]);
                register BidiLevel level = UNIBIDI_CONSISTENT_LEVEL (i);

                if (joins && !BIDI_LEVELS_MATCH (saved_level, level))
                {
                    disjoin = TRUE;
                    joins = FALSE;
                }

                if (!BIDI_IS_JOIN_SKIPPED (ar_props[i]))
                {
                    register const BidiArabicProp joins_preceding_mask =
                        BIDI_JOINS_PRECEDING_MASK (level);

                    if (!joins)
                    {
                        if (shapes)
                            BIDI_UNSET_BITS (ar_props[i], joins_preceding_mask);
                    }
                    else if (!BIDI_TEST_BITS (ar_props[i], joins_preceding_mask))
                    {
                        disjoin = TRUE;
                    }
                    else
                    {
                        register int j;
                        /* This is a FriBidi extension:  we set joining properties
                         * for skipped characters in between, so we can put NSMs on tatweel
                         * later if we want.  Useful on console for example.
                         */
                        for (j = saved + 1; j < i; j++)
                            BIDI_SET_BITS (ar_props[j], joins_preceding_mask | saved_joins_following_mask);
                    }
                }

                if (disjoin && saved_shapes)
                    BIDI_UNSET_BITS (ar_props[saved], saved_joins_following_mask);

                if (!BIDI_IS_JOIN_SKIPPED (ar_props[i]))
                {
                    saved = i;
                    saved_level = level;
                    saved_shapes = shapes;
                    saved_joins_following_mask =
                        BIDI_JOINS_FOLLOWING_MASK (level);
                    joins =
                        BIDI_TEST_BITS (ar_props[i], saved_joins_following_mask);
                }
            }
        if ((joins) && saved_shapes)
            BIDI_UNSET_BITS (ar_props[saved], saved_joins_following_mask);

    }

#ifdef _DEBUG
     print_joining_types (embedding_levels, len, ar_props);
#endif /* _DEBUG */
}

#endif /* _MGCHARSET_UNICODE */

