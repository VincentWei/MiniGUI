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
** unicode-shape.c:
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

void GUIAPI UBidiShapeMirroring(const BidiLevel *embedding_levels, int len,
        Uchar32* str)
{
    register int i;

    if (len == 0 || !str) return;

    /* L4. Mirror all characters that are in odd levels and have mirrors. */
    for (i = len - 1; i >= 0; i--) {
        if (BIDI_LEVEL_IS_RTL(embedding_levels[i])) {
            Uchar32 mirrored_ch;

            if (UCharGetMirror(str[i], &mirrored_ch)) {
                str[i] = mirrored_ch;
            }
        }
    }
}

void GUIAPI UBidiShape(Uint32 flags,
        const BidiLevel *embedding_levels, int len,
        BidiArabicProp *ar_props, Uchar32 *str)
{
    if (len == 0 || !str)
        return;

    if (ar_props)
        UBidiShapeArabic(flags, embedding_levels, len, ar_props, str);

    if (BIDI_TEST_BITS (flags, BIDI_FLAG_SHAPE_MIRRORING))
        UBidiShapeMirroring (embedding_levels, len, str);
}

#endif /* _MGCHARSET_UNICODE */
