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
** unicode-ops.h: Declaration of Unicode operators.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2019/02/26
*/

#ifndef GUI_UNICODE_OPS_H
#define GUI_UNICODE_OPS_H

#define UCHAR_TAB               0x0009
#define UCHAR_SPACE             0x0020
#define UCHAR_SHY               0x00AD
#define UCHAR_IDSPACE           0x3000
#define UCHAR_LINE_SEPARATOR    0x2028

#define PAREN_STACK_DEPTH 128

typedef struct _ParenStackEntry {
    int pair_index;
    ScriptType script;
} ParenStackEntry;

typedef struct _ScriptIterator {
    const Uchar32* ucs_start;
    const Uchar32* ucs_end;
    const Uchar32* start;
    const Uchar32* end;
    int paren_sp;

    ScriptType script;
    ParenStackEntry paren_stack[PAREN_STACK_DEPTH];
} ScriptIterator;

typedef struct _WidthIterator {
    const Uchar32*  ucs_start;
    const Uchar32*  ucs_end;
    const Uchar32*  start;
    const Uchar32*  end;
    BOOL            upright;
} WidthIterator;

typedef struct _EmojiIterator {
    const Uchar32 *text_start;
    const Uchar32 *text_end;
    const Uchar32 *start;
    const Uchar32 *end;
    BOOL is_emoji;

    unsigned char *types;
    unsigned int n_chars;
    unsigned int cursor;
} EmojiIterator;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

static inline BOOL is_uchar_letter(Uchar32 uc,
        UCharGeneralCategory gc, UCharBreakType bt)
{
    if ((gc >= UCHAR_CATEGORY_LOWERCASE_LETTER
                && gc <= UCHAR_CATEGORY_UPPERCASE_LETTER)
            || (gc >= UCHAR_CATEGORY_DECIMAL_NUMBER
                && gc <= UCHAR_CATEGORY_OTHER_NUMBER))
        return TRUE;

    if (bt == UCHAR_BREAK_NUMERIC
            || bt == UCHAR_BREAK_ALPHABETIC
            || bt == UCHAR_BREAK_IDEOGRAPHIC
            || bt == UCHAR_BREAK_AMBIGUOUS)
        return TRUE;

    return FALSE;
}

static inline BOOL is_uchar_no_shaping(Uchar32 uc)
{
    UCharGeneralCategory gc;

    gc = UCharGetCategory (uc);
    if ((gc == UCHAR_CATEGORY_CONTROL ||
            gc == UCHAR_CATEGORY_FORMAT ||
            gc == UCHAR_CATEGORY_SURROGATE ||
            (gc == UCHAR_CATEGORY_SPACE_SEPARATOR &&
                uc != 0x1680u /* OGHAM SPACE MARK */) ||
            (uc >= 0xfe00u && uc <= 0xfe0fu) ||
            (uc >= 0xe0100u && uc <= 0xe01efu))) {
        return TRUE;
    }

    return FALSE;
}

static inline BOOL is_uchar_word_separator(Uchar32 uc)
{
    return (uc == 0x0020 || uc == 0x00A0 ||
        uc == 0x1361 ||
        uc == 0x10100 || uc == 0x10101 ||
        uc == 0x1039F || uc == 0x1091F);
}

BOOL _unicode_is_emoji(Uchar32 ch);
BOOL _unicode_is_emoji_presentation(Uchar32 ch);
BOOL _unicode_is_emoji_modifier(Uchar32 ch);
BOOL _unicode_is_emoji_modifier_base(Uchar32 ch);
BOOL _unicode_is_extended_pictographic(Uchar32 ch);

BOOL _unicode_is_emoji_text_default(Uchar32 ch);
BOOL _unicode_is_emoji_emoji_default(Uchar32 ch);
BOOL _unicode_is_emoji_keycap_base(Uchar32 ch);
BOOL _unicode_is_regional_indicator(Uchar32 ch);

ScriptIterator* __mg_script_iterator_init (ScriptIterator *iter,
        const Uchar32* ucs, int nr_ucs);
BOOL __mg_script_iterator_next (ScriptIterator* iter);

WidthIterator* __mg_width_iterator_init (WidthIterator* iter,
        const Uchar32* ucs, int nr_ucs);
void __mg_width_iterator_next(WidthIterator* iter);

EmojiIterator* __mg_emoji_iterator_init (EmojiIterator *iter,
        const Uchar32*  ucs, int nr_ucs, Uint8* types_buff);
BOOL __mg_emoji_iterator_next (EmojiIterator *iter);
void __mg_emoji_iterator_fini (EmojiIterator *iter);

BOOL __mg_language_includes_script(LanguageCode lc, ScriptType script);

void __mg_unicode_break_arabic(const Uchar32* ucs, int nr_ucs,
        BreakOppo* break_oppos);
void __mg_unicode_break_indic(ScriptType writing_system,
        const Uchar32* ucs, int nr_ucs, BreakOppo* break_oppos);
void __mg_unicode_break_thai(const Uchar32* ucs, int nr_ucs,
        BreakOppo* break_oppos);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_UNICODE_OPS_H

