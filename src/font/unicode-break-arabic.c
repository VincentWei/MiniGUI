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
** unicode-break-arabic.c:
**      The implementation to tailor Arabic breaks
**
** Created by WEI Yongming at 2019/03/27
**
** This implementation is derived from LGPL'd Pango.
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
#include "unicode-ops.h"

#define ALEF_WITH_MADDA_ABOVE   0x0622
#define YEH_WITH_HAMZA_ABOVE    0x0626
#define ALEF                    0x0627
#define WAW                     0x0648
#define YEH                     0x064A

#define MADDAH_ABOVE            0x0653
#define HAMZA_ABOVE             0x0654
#define HAMZA_BELOW             0x0655

/*
 * Arabic characters with canonical decompositions that are not just
 * ligatures.  The characters U+06C0, U+06C2, and U+06D3 are intentionally
 * excluded as they are marked as "not an independent letter" in Unicode
 * Character Database's NamesList.txt
 */
#define IS_COMPOSITE(c) (ALEF_WITH_MADDA_ABOVE <= (c) && (c) <= YEH_WITH_HAMZA_ABOVE)

/* If a character is the second part of a composite Arabic character with an Alef */
#define IS_COMPOSITE_WITH_ALEF(c) (MADDAH_ABOVE <= (c) && (c) <= HAMZA_BELOW)

void __mg_unicode_break_arabic(const Uchar32* ucs, int nr_ucs,
        BreakOppo* break_oppos)
{
    int i;
    Uchar32 prev_wc, this_wc;

    for (i = 0, prev_wc = 0; i < nr_ucs; i++, prev_wc = this_wc) {
        this_wc = ucs[i];

        /*
         * Unset backspace_deletes_character for various combinations.
         *
         * A few more combinations may need to be handled here, but are not
         * handled yet, as expectations of users is not known or may differ
         * among different languages or users:
         * some letters combined with U+0658 ARABIC MARK NOON GHUNNA;
         * combinations considered one letter in Azerbaijani (WAW+SUKUN and
         * FARSI_YEH+HAMZA_ABOVE); combinations of YEH and ALEF_MAKSURA with
         * HAMZA_BELOW (Qur'anic); TATWEEL+HAMZA_ABOVE (Qur'anic).
         *
         * FIXME: Ordering these in some other way may lower the time spent here, or not.
         */
        if (IS_COMPOSITE (this_wc) ||
                (prev_wc == ALEF && IS_COMPOSITE_WITH_ALEF (this_wc)) ||
                (this_wc == HAMZA_ABOVE && (prev_wc == WAW || prev_wc == YEH)))
            break_oppos[i + 1] &= ~BOV_GB_BACKSPACE_DEL_CH;
    }
}

#endif /* _MGCHARSET_UNICODE */

