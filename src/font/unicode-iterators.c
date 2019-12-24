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
 *   Copyright (C) 2019, Beijing FMSoft Technologies Co., Ltd.
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
** unicode-iterators.c: The implementation of the uchar string iterators.
**
** Create by WEI Yongming at 2019/03/18
**
** Implementation of WidthIterator is derived from LGPL'd Pango.
**
** Implementation of ScriptIterator is derived from ICU:
**
**  icu/sources/common/usc_impl.c
**
***********************************************************************
**   Copyright (C) 1999-2002, International Business Machines
**   Corporation and others.  All Rights Reserved.
***********************************************************************
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, and/or sell copies of the Software, and to permit persons
** to whom the Software is furnished to do so, provided that the above
** copyright notice(s) and this permission notice appear in all copies of
** the Software and that both the above copyright notice(s) and this
** permission notice appear in supporting documentation.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
** OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
** OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
** HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
** INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
** NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
** WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
** Except as contained in this notice, the name of a copyright holder
** shall not be used in advertising or otherwise to promote the sale, use
** or other dealings in this Software without prior written authorization
** of the copyright holder.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#if defined(_MGCHARSET_UNICODE)

#include "minigui.h"
#include "gdi.h"
#include "unicode-ops.h"

ScriptIterator* __mg_script_iterator_init(ScriptIterator *iter,
        const Uchar32* ucs, int nr_ucs)
{
    assert (nr_ucs >= 0);

    iter->ucs_start = ucs;
    iter->ucs_end = ucs + nr_ucs;
    iter->start = ucs;
    iter->end = ucs;
    iter->script = SCRIPT_COMMON;

    iter->paren_sp = -1;

    __mg_script_iterator_next (iter);
    return iter;
}

static const Uchar32 paired_chars[] = {
    0x0028, 0x0029, /* ascii paired punctuation */
    0x003c, 0x003e,
    0x005b, 0x005d,
    0x007b, 0x007d,
    0x00ab, 0x00bb, /* guillemets */
    0x2018, 0x2019, /* general punctuation */
    0x201c, 0x201d,
    0x2039, 0x203a,
    0x3008, 0x3009, /* chinese paired punctuation */
    0x300a, 0x300b,
    0x300c, 0x300d,
    0x300e, 0x300f,
    0x3010, 0x3011,
    0x3014, 0x3015,
    0x3016, 0x3017,
    0x3018, 0x3019,
    0x301a, 0x301b
};

static int get_pair_index (Uchar32 ch)
{
    int lower = 0;
    int upper = TABLESIZE (paired_chars) - 1;

    while (lower <= upper) {
        int mid = (lower + upper) / 2;

        if (ch < paired_chars[mid])
            upper = mid - 1;
        else if (ch > paired_chars[mid])
            lower = mid + 1;
        else
            return mid;
    }

    return -1;
}

#define REAL_SCRIPT(script) \
    ((script) > SCRIPT_INHERITED && (script) != SCRIPT_UNKNOWN)

#define SAME_SCRIPT(script1, script2) \
    (!REAL_SCRIPT (script1) || !REAL_SCRIPT (script2) || (script1) == (script2))

#define IS_OPEN(pair_index) (((pair_index) & 1) == 0)

/**
 * __mg_script_iterator_next:
 *
 * Advances a #ScriptIterator to the next range. If @iter
 * is already at the end, it is left unchanged and %FALSE
 * is returned.
 *
 * Return value: %TRUE if @iter was successfully advanced.
 **/
BOOL __mg_script_iterator_next (ScriptIterator *iter)
{
    int start_sp;

    if (iter->end == iter->ucs_end)
        return FALSE;

    start_sp = iter->paren_sp;
    iter->script = SCRIPT_COMMON;
    iter->start = iter->end;

    for (; iter->end < iter->ucs_end; iter->end++) {
        Uchar32 ch = *iter->end;
        ScriptType sc;
        int pair_index;

        sc = UCharGetScriptType(ch);
        if (sc != SCRIPT_COMMON)
            pair_index = -1;
        else
            pair_index = get_pair_index (ch);

        /*
         * Paired character handling:
         *
         * if it's an open character, push it onto the stack.
         * if it's a close character, find the matching open on the
         * stack, and use that script code. Any non-matching open
         * characters above it on the stack will be poped.
         */
        if (pair_index >= 0) {
            if (IS_OPEN (pair_index)) {
                /*
                 * If the paren stack is full, empty it. This
                 * means that deeply nested paired punctuation
                 * characters will be ignored, but that's an unusual
                 * case, and it's better to ignore them than to
                 * write off the end of the stack...
                 */
                if (++iter->paren_sp >= PAREN_STACK_DEPTH)
                    iter->paren_sp = 0;

                iter->paren_stack[iter->paren_sp].pair_index = pair_index;
                iter->paren_stack[iter->paren_sp].script =
                        iter->script;
            }
            else if (iter->paren_sp >= 0) {
                int pi = pair_index & ~1;

                while (iter->paren_sp >= 0 &&
                        iter->paren_stack[iter->paren_sp].pair_index != pi)
                    iter->paren_sp--;

                if (iter->paren_sp < start_sp)
                    start_sp = iter->paren_sp;

                if (iter->paren_sp >= 0)
                    sc = iter->paren_stack[iter->paren_sp].script;
            }
        }

        if (SAME_SCRIPT (iter->script, sc)) {
            if (!REAL_SCRIPT (iter->script) && REAL_SCRIPT (sc)) {
                iter->script = sc;

                /*
                 * now that we have a final script code, fix any open
                 * characters we pushed before we knew the script code.
                 */
                while (start_sp < iter->paren_sp)
                    iter->paren_stack[++start_sp].script = iter->script;
            }

            /*
             * if this character is a close paired character,
             * pop it from the stack
             */
            if (pair_index >= 0 && !IS_OPEN (pair_index) &&
                    iter->paren_sp >= 0) {
                iter->paren_sp--;

                if (iter->paren_sp < start_sp)
                    start_sp = iter->paren_sp;
            }
        }
        else {
            /* Different script, we're done */
            break;
        }
    }

    return TRUE;
}

static inline BOOL width_iterator_is_upright(Uchar32 ch)
{
    return (UCharGetVerticalOrientation(ch) == UCHAR_VOP_U);
}

void __mg_width_iterator_next(WidthIterator* iter)
{
    BOOL met_joiner = FALSE;
    iter->start = iter->end;

    if (iter->end < iter->ucs_end) {
        iter->upright = width_iterator_is_upright(*iter->end);
    }

    while (iter->end < iter->ucs_end) {
        Uchar32 ch = *iter->end;

        /* for zero width joiner */
        if (ch == 0x200D) {
            iter->end++;
            met_joiner = TRUE;
            continue;
        }

        /* ignore the upright check if met joiner */
        if (met_joiner) {
            iter->end++;
            met_joiner = FALSE;
            continue;
        }

        /* for variation selector, tag and emoji modifier. */
        if (ch == 0xFE0EU || ch == 0xFE0FU ||
                (ch >= 0xE0020 && ch <= 0xE007F) ||
                (ch >= 0x1F3FB && ch <= 0x1F3FF)) {
            iter->end++;
            continue;
        }

        if (width_iterator_is_upright (ch) != iter->upright)
            break;

        iter->end++;
    }
}

WidthIterator* __mg_width_iterator_init (WidthIterator* iter,
        const Uchar32* ucs, int nr_ucs)
{
    iter->ucs_start = ucs;
    iter->ucs_end = ucs + nr_ucs;
    iter->start = iter->end = ucs;

    __mg_width_iterator_next (iter);
    return iter;
}

#endif /*  _MGCHARSET_UNICODE */

