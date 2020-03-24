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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"

#ifdef _MGCHARSET_UNICODE

#include "unicode-decomp.h"
#include "unicode-comp.h"

#define CC_PART1(Page, Char) \
    ((combining_class_table_part1[Page] >= UCHAR_MAX_TABLE_INDEX) \
     ? (combining_class_table_part1[Page] - UCHAR_MAX_TABLE_INDEX) \
     : (cclass_data[combining_class_table_part1[Page]][Char]))

#define CC_PART2(Page, Char) \
    ((combining_class_table_part2[Page] >= UCHAR_MAX_TABLE_INDEX) \
     ? (combining_class_table_part2[Page] - UCHAR_MAX_TABLE_INDEX) \
     : (cclass_data[combining_class_table_part2[Page]][Char]))

#define COMBINING_CLASS(Char) \
    (((Char) <= UCHAR_LAST_CHAR_PART1) \
     ? CC_PART1 ((Char) >> 8, (Char) & 0xff) \
     : (((Char) >= 0xe0000 && (Char) <= UCHAR_LAST_CHAR) \
         ? CC_PART2 (((Char) - 0xe0000) >> 8, (Char) & 0xff) \
         : 0))

int UCharCombiningClass (Uchar32 uc)
{
    return COMBINING_CLASS (uc);
}

/* constants for hangul syllable [de]composition */
#define SBase 0xAC00
#define LBase 0x1100
#define VBase 0x1161
#define TBase 0x11A7
#define LCount 19
#define VCount 21
#define TCount 28
#define NCount (VCount * TCount)
#define SCount (LCount * NCount)

void UCharCanonicalOrdering (Uchar32 *string, int len)
{
    int i;
    int swap = 1;

    while (swap)
    {
        int last;
        swap = 0;
        last = COMBINING_CLASS (string[0]);
        for (i = 0; i < len - 1; ++i)
        {
            int next = COMBINING_CLASS (string[i + 1]);
            if (next != 0 && last > next)
            {
                int j;
                /* Percolate item leftward through string.  */
                for (j = i + 1; j > 0; --j)
                {
                    Uchar32 t;
                    if (COMBINING_CLASS (string[j - 1]) <= next)
                        break;
                    t = string[j];
                    string[j] = string[j - 1];
                    string[j - 1] = t;
                    swap = 1;
                }
                /* We're re-entering the loop looking at the old
                   character again.  */
                next = last;
            }
            last = next;
        }
    }
}

/* http://www.unicode.org/unicode/reports/tr15/#Hangul
 * r should be null or have sufficient space. Calling with r == NULL will
 * only calculate the result_len; however, a buffer with space for three
 * characters will always be big enough. */
static void decompose_hangul (Uchar32 s, Uchar32 *r, int *result_len)
{
    int SIndex = s - SBase;
    int TIndex = SIndex % TCount;

    if (r)
    {
        r[0] = LBase + SIndex / NCount;
        r[1] = VBase + (SIndex % NCount) / TCount;
    }

    if (TIndex)
    {
        if (r)
            r[2] = TBase + TIndex;
        *result_len = 3;
    }
    else
        *result_len = 2;
}

/* returns a pointer to a null-terminated UTF-8 string */
static const unsigned char * find_decomposition (Uchar32 ch, BOOL compat)
{
    int start = 0;
    int end = TABLESIZE (decomp_table);

    if (ch >= decomp_table[start].ch &&
            ch <= decomp_table[end - 1].ch) {
        while (TRUE) {
            int half = (start + end) / 2;
            if (ch == decomp_table[half].ch) {
                int offset;

                if (compat) {
                    offset = decomp_table[half].compat_offset;
                    if (offset == UCHAR_NOT_PRESENT_OFFSET)
                        offset = decomp_table[half].canon_offset;
                }
                else {
                    offset = decomp_table[half].canon_offset;
                    if (offset == UCHAR_NOT_PRESENT_OFFSET)
                        return NULL;
                }

                return &(decomp_expansion_string[offset]);
            }
            else if (half == start)
                break;
            else if (ch > decomp_table[half].ch)
                start = half;
            else
                end = half;
        }
    }

    return NULL;
}

/* L,V => LV and LV,T => LVT  */
static BOOL combine_hangul (Uchar32 a, Uchar32 b, Uchar32 *result)
{
    int LIndex = a - LBase;
    int SIndex = a - SBase;

    int VIndex = b - VBase;
    int TIndex = b - TBase;

    if (0 <= LIndex && LIndex < LCount
            && 0 <= VIndex && VIndex < VCount) {
        *result = SBase + (LIndex * VCount + VIndex) * TCount;
        return TRUE;
    }
    else if (0 <= SIndex && SIndex < SCount && (SIndex % TCount) == 0
            && 0 < TIndex && TIndex < TCount) {
        *result = a + TIndex;
        return TRUE;
    }

    return FALSE;
}

#define CI(Page, Char) \
    ((compose_table[Page] >= UCHAR_MAX_TABLE_INDEX) \
     ? (compose_table[Page] - UCHAR_MAX_TABLE_INDEX) \
     : (compose_data[compose_table[Page]][Char]))

#define COMPOSE_INDEX(Char) \
    (((Char >> 8) > (COMPOSE_TABLE_LAST)) ? 0 : CI((Char) >> 8, (Char) & 0xff))

static BOOL combine (Uchar32  a, Uchar32  b, Uchar32 *result)
{
    unsigned short index_a, index_b;

    if (combine_hangul (a, b, result))
        return TRUE;

    index_a = COMPOSE_INDEX(a);

    if (index_a >= COMPOSE_FIRST_SINGLE_START && index_a < COMPOSE_SECOND_START)
    {
        if (b == compose_first_single[index_a - COMPOSE_FIRST_SINGLE_START][0])
        {
            *result = compose_first_single[index_a - COMPOSE_FIRST_SINGLE_START][1];
            return TRUE;
        }
        else
            return FALSE;
    }

    index_b = COMPOSE_INDEX(b);

    if (index_b >= COMPOSE_SECOND_SINGLE_START)
    {
        if (a == compose_second_single[index_b - COMPOSE_SECOND_SINGLE_START][0])
        {
            *result = compose_second_single[index_b - COMPOSE_SECOND_SINGLE_START][1];
            return TRUE;
        }
        else
            return FALSE;
    }

    if (index_a >= COMPOSE_FIRST_START && index_a < COMPOSE_FIRST_SINGLE_START &&
            index_b >= COMPOSE_SECOND_START && index_b < COMPOSE_SECOND_SINGLE_START)
    {
        Uchar32 res = compose_array[index_a - COMPOSE_FIRST_START][index_b - COMPOSE_SECOND_START];

        if (res)
        {
            *result = res;
            return TRUE;
        }
    }

    return FALSE;
}

static BOOL decompose_hangul_step (Uchar32  ch, Uchar32 *a, Uchar32 *b)
{
    int SIndex, TIndex;

    if (ch < SBase || ch >= SBase + SCount)
        return FALSE;  /* not a hangul syllable */

    SIndex = ch - SBase;
    TIndex = SIndex % TCount;

    if (TIndex) {
        /* split LVT -> LV,T */
        *a = ch - TIndex;
        *b = TBase + TIndex;
    }
    else {
        /* split LV -> L,V */
        *a = LBase + SIndex / NCount;
        *b = VBase + (SIndex % NCount) / TCount;
    }

    return TRUE;
}

BOOL UCharDecompose (Uchar32 ch, Uchar32 *a, Uchar32 *b)
{
    int start = 0;
    int end = TABLESIZE (decomp_step_table);

    if (decompose_hangul_step (ch, a, b))
        return TRUE;

    /* TODO use bsearch() */
    if (ch >= decomp_step_table[start].ch &&
            ch <= decomp_step_table[end - 1].ch)
    {
        while (TRUE)
        {
            int half = (start + end) / 2;
            const decomposition_step *p = &(decomp_step_table[half]);
            if (ch == p->ch)
            {
                *a = p->a;
                *b = p->b;
                return TRUE;
            }
            else if (half == start)
                break;
            else if (ch > p->ch)
                start = half;
            else
                end = half;
        }
    }

    *a = ch;
    *b = 0;

    return FALSE;
}

BOOL UCharCompose (Uchar32 a, Uchar32 b, Uchar32 *ch)
{
    if (combine (a, b, ch))
        return TRUE;

    *ch = 0;
    return FALSE;
}

#define UTF8_COMPUTE(Char, Mask, Len)       \
if (Char < 128)                             \
{                                           \
    Len = 1;                                \
    Mask = 0x7f;                            \
}                                           \
else if ((Char & 0xe0) == 0xc0)             \
{                                           \
    Len = 2;                                \
    Mask = 0x1f;                            \
}                                           \
else if ((Char & 0xf0) == 0xe0)             \
{                                           \
    Len = 3;                                \
    Mask = 0x0f;                            \
}                                           \
else if ((Char & 0xf8) == 0xf0)             \
{                                           \
    Len = 4;                                \
    Mask = 0x07;                            \
}                                           \
else if ((Char & 0xfc) == 0xf8)             \
{                                           \
    Len = 5;                                \
    Mask = 0x03;                            \
}                                           \
else if ((Char & 0xfe) == 0xfc)             \
{                                           \
    Len = 6;                                \
    Mask = 0x01;                            \
}                                           \
else                                        \
Len = -1;

#define UTF8_LENGTH(Char)                   \
    ((Char) < 0x80 ? 1 :                    \
     ((Char) < 0x800 ? 2 :                  \
      ((Char) < 0x10000 ? 3 :               \
       ((Char) < 0x200000 ? 4 :             \
        ((Char) < 0x4000000 ? 5 : 6)))))


#define UTF8_GET(Result, Chars, Count, Mask, Len)       \
    (Result) = (Chars)[0] & (Mask);                     \
for ((Count) = 1; (Count) < (Len); ++(Count))           \
{                                                       \
    if (((Chars)[(Count)] & 0xc0) != 0x80)              \
    {                                                   \
        (Result) = -1;                                  \
        break;                                          \
    }                                                   \
    (Result) <<= 6;                                     \
    (Result) |= ((Chars)[(Count)] & 0x3f);              \
}

static Uchar32 utf8_to_uchar32 (const unsigned char *p)
{
    int i, mask = 0, len;
    Uchar32 result;
    unsigned char c = (unsigned char) *p;

    UTF8_COMPUTE (c, mask, len);
    if (len == -1)
        return (Uchar32)-1;
    UTF8_GET (result, p, i, mask, len);

    return result;
}

#define UNICODE_VALID(Char)                   \
    ((Char) < 0x110000 &&                     \
     (((Char) & 0xFFFFF800) != 0xD800))

static const unsigned char utf8_skip_data[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

#define utf8_next_char(p) ((p) + utf8_skip_data[*(p)])

static int utf8_strlen (const unsigned char *p, int max)
{
    int len = 0;
    const unsigned char *start = p;
    if (p == NULL && max == 0)
        return 0;

    if (max < 0) {
        while (*p) {
            p = utf8_next_char (p);
            ++len;
        }
    }
    else {
        if (max == 0 || !*p)
            return 0;

        p = utf8_next_char (p);

        while (p - start < max && *p) {
            ++len;
            p = utf8_next_char (p);
        }

        /* only do the last len increment if we got a complete
         * char (don't count partial chars)
         */
        if (p - start <= max)
            ++len;
    }

    return len;
}

int UCharFullyDecompose (Uchar32  ch, BOOL compat, Uchar32 *result, int result_len)
{
    const unsigned char *decomp;
    const unsigned char *p;

    /* Hangul syllable */
    if (ch >= SBase && ch < SBase + SCount) {
        int len, i;
        Uchar32 buffer[3];
        decompose_hangul (ch, result ? buffer : NULL, &len);
        if (result)
            for (i = 0; i < len && i < result_len; i++)
                result[i] = buffer[i];
        return len;
    }
    else if ((decomp = find_decomposition (ch, compat)) != NULL)
    {
        /* Found it.  */
        int len, i;

        len = utf8_strlen (decomp, -1);

        for (p = decomp, i = 0; i < len && i < result_len; p = utf8_next_char (p), i++)
            result[i] = utf8_to_uchar32 (p);

        return len;
    }

    /* Does not decompose */
    if (result && result_len >= 1)
        *result = ch;
    return 1;
}

#endif /* _MGCHARSET_UNICODE */

