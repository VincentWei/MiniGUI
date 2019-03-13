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
** unicode-ops.h: Declaration of Unicode operators.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2019/02/26
*/

#ifndef GUI_UNICODE_OPS_H
#define GUI_UNICODE_OPS_H

#define UCHAR_SPACE         0x0020
#define UCHAR_SHY           0x00AD
#define UCHAR_IDSPACE       0x3000
#define UCHAR_TAB           0x0009

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

BOOL _unicode_is_emoji(Uchar32 ch);
BOOL _unicode_is_emoji_presentation(Uchar32 ch);
BOOL _unicode_is_emoji_modifier(Uchar32 ch);
BOOL _unicode_is_emoji_modifier_base(Uchar32 ch);
BOOL _unicode_is_extended_pictographic(Uchar32 ch);

BOOL _unicode_is_emoji_text_default(Uchar32 ch);
BOOL _unicode_is_emoji_emoji_default(Uchar32 ch);
BOOL _unicode_is_emoji_keycap_base(Uchar32 ch);
BOOL _unicode_is_regional_indicator(Uchar32 ch);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_UNICODE_OPS_H

