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
** Some operators to check Emoji.
** Implementation of the functions is based on some code from HarfBuzz
** (licensed under MIT):
**
**  https://github.com/harfbuzz/harfbuzz
**
** Created by WEI Yongming at 2019/03/01
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"

#ifdef _MGCHARSET_UNICODE

#include "unicode-ops.h"
#include "unicode-emoji-tables.h"

static int interval_compare(const void *key, const void *elt)
{
    Uchar32 c = (Uchar32)(intptr_t)key;
    struct Interval *interval = (struct Interval *)elt;

    if (c < interval->start)
        return -1;
    if (c > interval->end)
        return +1;

    return 0;
}

#define DEFINE_unicode_is_(name) \
    BOOL _unicode_is_##name (Uchar32 ch) \
{ \
    if (bsearch ((void*)((intptr_t)ch), \
                _unicode_##name##_table, \
                TABLESIZE (_unicode_##name##_table), \
                sizeof _unicode_##name##_table[0], \
                interval_compare)) \
        return TRUE; \
    \
    return FALSE; \
}

DEFINE_unicode_is_(emoji)
DEFINE_unicode_is_(emoji_presentation)
DEFINE_unicode_is_(emoji_modifier)
DEFINE_unicode_is_(emoji_modifier_base)
DEFINE_unicode_is_(extended_pictographic)

BOOL _unicode_is_emoji_text_default(Uchar32 ch)
{
    return _unicode_is_emoji(ch) && !_unicode_is_emoji_presentation(ch);
}

BOOL _unicode_is_emoji_emoji_default(Uchar32 ch)
{
    return _unicode_is_emoji_presentation(ch);
}

BOOL _unicode_is_emoji_keycap_base(Uchar32 ch)
{
    return (ch >= '0' && ch <= '9') || ch == '#' || ch == '*';
}

BOOL _unicode_is_regional_indicator(Uchar32 ch)
{
    return (ch >= 0x1F1E6 && ch <= 0x1F1FF);
}

/*
 * Implementation of EmojiIterator is based on Chromium's Ragel-based
 * parser:
 *
 * https://chromium-review.googlesource.com/c/chromium/src/+/1264577
 *
 * The grammar file emoji_presentation_scanner.rl was just modified to
 * adapt the function signature and variables to our usecase.  The
 * grammar itself was NOT modified:
 *
 * https://chromium-review.googlesource.com/c/chromium/src/+/1264577/3/third_party/blink/renderer/platform/fonts/emoji_presentation_scanner.rl
 *
 * The emoji_presentation_scanner.c is generated from .rl file by
 * running ragel on it.
 *
 * The categorization is also based on:
 *
 * https://chromium-review.googlesource.com/c/chromium/src/+/1264577/3/third_party/blink/renderer/platform/fonts/utf16_ragel_iterator.h
 *
 * The iterator next() is based on:
 *
 * https://chromium-review.googlesource.com/c/chromium/src/+/1264577/3/third_party/blink/renderer/platform/fonts/symbols_iterator.cc
 *
 * // Copyright 2015 The Chromium Authors. All rights reserved.
 * // Use of this source code is governed by a BSD-style license that can be
 * // found in the LICENSE file.
 */

const Uchar32 kCombiningEnclosingCircleBackslashCharacter = 0x20E0;
const Uchar32 kCombiningEnclosingKeycapCharacter = 0x20E3;
const Uchar32 kVariationSelector15Character = 0xFE0E;
const Uchar32 kVariationSelector16Character = 0xFE0F;
const Uchar32 kZeroWidthJoinerCharacter = 0x200D;

enum EmojiScannerCategory {
    EMOJI = 0,
    EMOJI_TEXT_PRESENTATION = 1,
    EMOJI_EMOJI_PRESENTATION = 2,
    EMOJI_MODIFIER_BASE = 3,
    EMOJI_MODIFIER = 4,
    EMOJI_VS_BASE = 5,
    REGIONAL_INDICATOR = 6,
    KEYCAP_BASE = 7,
    COMBINING_ENCLOSING_KEYCAP = 8,
    COMBINING_ENCLOSING_CIRCLE_BACKSLASH = 9,
    ZWJ = 10,
    VS15 = 11,
    VS16 = 12,
    TAG_BASE = 13,
    TAG_SEQUENCE = 14,
    TAG_TERM = 15,
    kMaxEmojiScannerCategory = 16
};

static unsigned char emojiSegmentationCategory (Uchar32 codepoint)
{
    /* Specific ones first. */
    if (codepoint == kCombiningEnclosingKeycapCharacter)
        return COMBINING_ENCLOSING_KEYCAP;
    if (codepoint == kCombiningEnclosingCircleBackslashCharacter)
        return COMBINING_ENCLOSING_CIRCLE_BACKSLASH;
    if (codepoint == kZeroWidthJoinerCharacter)
        return ZWJ;
    if (codepoint == kVariationSelector15Character)
        return VS15;
    if (codepoint == kVariationSelector16Character)
        return VS16;
    if (codepoint == 0x1F3F4)
        return TAG_BASE;
    if ((codepoint >= 0xE0030 && codepoint <= 0xE0039) ||
            (codepoint >= 0xE0061 && codepoint <= 0xE007A))
        return TAG_SEQUENCE;
    if (codepoint == 0xE007F)
        return TAG_TERM;

    if (_unicode_is_emoji_modifier_base (codepoint))
        return EMOJI_MODIFIER_BASE;
    if (_unicode_is_emoji_modifier (codepoint))
        return EMOJI_MODIFIER;
    if (_unicode_is_regional_indicator (codepoint))
        return REGIONAL_INDICATOR;
    if (_unicode_is_emoji_keycap_base (codepoint))
        return KEYCAP_BASE;

    if (_unicode_is_emoji_emoji_default (codepoint))
        return EMOJI_EMOJI_PRESENTATION;
    if (_unicode_is_emoji_text_default (codepoint))
        return EMOJI_TEXT_PRESENTATION;
    if (_unicode_is_emoji (codepoint))
        return EMOJI;

    /* Ragel state machine will interpret unknown category as "any". */
    return kMaxEmojiScannerCategory;
}


typedef BOOL bool;
enum { false = FALSE, true = TRUE };
typedef unsigned char *emoji_text_iter_t;

#include "emoji_presentation_scanner.inc"

EmojiIterator * __mg_emoji_iterator_init (EmojiIterator *iter,
        const Uchar32* ucs, int nr_ucs, Uint8* types_buff)
{
    int i;
    Uint8 *types = types_buff;
    const Uchar32 *p;

    assert (nr_ucs > 0);

    p = ucs;
    for (i = 0; i < nr_ucs; i++) {
        types[i] = emojiSegmentationCategory (*p);
        p++;
    }

    iter->text_start = iter->start = iter->end = ucs;
    iter->text_end = ucs + nr_ucs;
    iter->is_emoji = FALSE;

    iter->types = types;
    iter->n_chars = nr_ucs;
    iter->cursor = 0;

    __mg_emoji_iterator_next (iter);
    return iter;
}

BOOL __mg_emoji_iterator_next (EmojiIterator *iter)
{
    unsigned int old_cursor, cursor;
    BOOL is_emoji;

    if (iter->end >= iter->text_end)
        return FALSE;

    iter->start = iter->end;

    old_cursor = cursor = iter->cursor;
    cursor = scan_emoji_presentation (iter->types + cursor,
            iter->types + iter->n_chars,
            &is_emoji) - iter->types;

    do {
        iter->cursor = cursor;
        iter->is_emoji = is_emoji;

        if (cursor == iter->n_chars)
            break;

        cursor = scan_emoji_presentation (iter->types + cursor,
                iter->types + iter->n_chars,
                &is_emoji) - iter->types;
    }
    while (iter->is_emoji == is_emoji);

    iter->end = iter->start + (iter->cursor - old_cursor);
    return TRUE;
}

#endif /* _MGCHARSET_UNICODE */
