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
** unicode-shape-arabic.c:
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

typedef struct _PairMap {
    Uchar32 pair[2], to;
} PairMap;

#define UNIBIDI_ACCESS_SHAPE_TABLE(table,min,max,x,shape) (table), (min), (max)
# define UNIBIDI_ACCESS_SHAPE_TABLE_REAL(table,min,max,x,shape) \
    (((x)<(min)||(x)>(max))?(x):(table)[(x)-(min)][(shape)])

#include "unicode-arabic-shaping-table.inc"
#include "unicode-arabic-misc-table.inc"

static void unibidi_shape_arabic_joining (const Uchar32 table[][4],
        Uchar32 min, Uchar32 max, const int len,
        const BidiArabicProp *ar_props,
        Uchar32 *str)
{
    register int i;

    for (i = 0; i < len; i++) {
        if (BIDI_ARAB_SHAPES(ar_props[i]))
            str[i] = UNIBIDI_ACCESS_SHAPE_TABLE_REAL(table, min, max, str[i],
                BIDI_JOIN_SHAPE (ar_props[i]));
    }
}

static int comp_PairMap (const void *pa, const void *pb)
{
    PairMap *a = (PairMap *)pa;
    PairMap *b = (PairMap *)pb;

    if (a->pair[0] != b->pair[0])
        return a->pair[0] < b->pair[0] ? -1 : +1;
    else
        return a->pair[1] < b->pair[1] ? -1 :
            a->pair[1] > b->pair[1] ? +1 : 0;
}


static Uchar32 find_pair_match (const PairMap *table, int size,
        Uchar32 first, Uchar32 second)
{
    PairMap *match;
    PairMap x;
    x.pair[0] = first;
    x.pair[1] = second;
    x.to = 0;
    match = bsearch (&x, table, size, sizeof (table[0]), comp_PairMap);
    return match ? match->to : 0;
}

#define PAIR_MATCH(table,len,first,second) \
    ((first)<(table[0].pair[0])||(first)>(table[len-1].pair[0])?0: \
     find_pair_match(table, len, first, second))

static void unibidi_shape_arabic_ligature(const PairMap *table,
        int size, const BidiLevel *embedding_levels, int len,
        BidiArabicProp *ar_props, Uchar32 *str)
{
    /* TODO: This doesn't form ligatures for even-level Arabic text.
     * no big problem though. */
    register int i;

    for (i = 0; i < len - 1; i++) {
        register Uchar32 c;
        if (BIDI_LEVEL_IS_RTL(embedding_levels[i]) &&
                embedding_levels[i] == embedding_levels[i+1] &&
                (c = PAIR_MATCH(table, size, str[i], str[i+1])))
        {
            str[i] = BIDI_CHAR_FILL;
            BIDI_SET_BITS(ar_props[i], BIDI_MASK_LIGATURED);
            str[i+1] = c;
        }
    }
}

#define DO_LIGATURING(table, levels, len, ar_props, str) \
    unibidi_shape_arabic_ligature ((table), \
        sizeof(table)/sizeof((table)[0]), levels, len, ar_props, str)

#define DO_SHAPING(tablemacro, len, ar_props, str) \
    unibidi_shape_arabic_joining (tablemacro(,), len, ar_props, str)

void UBidiShapeArabic(Uint32 flags, const BidiLevel *embedding_levels, int len,
        BidiArabicProp *ar_props, Uchar32 *str)
{
    if (len == 0 || !str) return;

    if (BIDI_TEST_BITS (flags, BIDI_FLAG_SHAPE_ARAB_PRES)) {
        DO_SHAPING (UNIBIDI_GET_ARABIC_SHAPE_PRES, len, ar_props, str);
    }

    if (BIDI_TEST_BITS (flags, BIDI_FLAG_SHAPE_ARAB_LIGA)) {
        DO_LIGATURING (mandatory_liga_table, embedding_levels, len,
            ar_props, str);
    }

    if (BIDI_TEST_BITS (flags, BIDI_FLAG_SHAPE_ARAB_CONSOLE)) {
        DO_LIGATURING (console_liga_table, embedding_levels, len,
            ar_props, str);
        DO_SHAPING (UNIBIDI_GET_ARABIC_SHAPE_NSM, len, ar_props, str);
    }
}

#endif /* _MGCHARSET_UNICODE */
