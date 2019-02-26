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
** Some operators to check Emoji.
** Implementation of the functions is based on some code from LGPL'd Pango.
**
** Created by WEI Yongming at 2019/02/26
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"

#ifdef _MGCHARSET_UNICODE

#include "unicode-ops.h"
#include "unicode-emoji-table.h"

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
DEFINE_unicode_is_(emoji_extended_pictographic)

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

#endif /* _MGCHARSET_UNICODE */
