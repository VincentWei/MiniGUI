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
** unicode-break-indic.c:
**      The implementation to tailor Indic breaks
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

#define DEV_RRA     0x0931 /* 0930 + 093c */
#define DEV_QA      0x0958 /* 0915 + 093c */
#define DEV_YA      0x095F /* 092f + 003c */
#define DEV_KHHA    0x0959
#define DEV_GHHA    0x095A
#define DEV_ZA      0x095B
#define DEV_DDDHA   0x095C
#define DEV_RHA     0x095D
#define DEV_FA      0x095E
#define DEV_YYA     0x095F

/* Bengali */
/* for split matras in all brahmi based script */
#define BENGALI_SIGN_O  0x09CB  /* 09c7 + 09be */
#define BENGALI_SIGN_AU 0x09CC /* 09c7 + 09d7 */
#define BENGALI_RRA     0x09DC
#define BENGALI_RHA     0x09DD
#define BENGALI_YYA     0x09DF

/* Gurumukhi */
#define GURUMUKHI_LLA   0x0A33
#define GURUMUKHI_SHA   0x0A36
#define GURUMUKHI_KHHA  0x0A59
#define GURUMUKHI_GHHA  0x0A5A
#define GURUMUKHI_ZA    0x0A5B
#define GURUMUKHI_RRA   0x0A5C
#define GURUMUKHI_FA    0x0A5E

/* Oriya */
#define ORIYA_AI        0x0B48
#define ORIYA_O         0x0B4B
#define ORIYA_AU        0x0B4C

/* Telugu */
#define TELUGU_EE       0x0C47
#define TELUGU_AI       0x0C48

/* Tamil */
#define TAMIL_O         0x0BCA
#define TAMIL_OO        0x0BCB
#define TAMIL_AU        0x0BCC

/* Kannada */
#define KNDA_EE         0x0CC7
#define KNDA_AI         0x0CC8
#define KNDA_O          0x0CCA
#define KNDA_OO         0x0CCB

/* Malayalam */
#define MLYM_O          0x0D4A
#define MLYM_OO         0x0D4B
#define MLYM_AU         0x0D4C

#define IS_COMPOSITE_WITH_BRAHMI_NUKTA(c) ( \
        (c >= BENGALI_RRA  && c <= BENGALI_YYA) || \
        (c >= DEV_QA  && c <= DEV_YA) || (c == DEV_RRA) || (c >= DEV_KHHA  && c <= DEV_YYA) || \
        (c >= KNDA_EE  && c <= KNDA_AI) ||(c >= KNDA_O  && c <= KNDA_OO) || \
        (c == TAMIL_O) || (c == TAMIL_OO) || (c == TAMIL_AU) || \
        (c == TELUGU_EE) || (c == TELUGU_AI) || \
        (c == ORIYA_AI) || (c == ORIYA_O) || (c == ORIYA_AU) || \
        (c >= GURUMUKHI_KHHA  && c <= GURUMUKHI_RRA) || (c == GURUMUKHI_FA)|| (c == GURUMUKHI_LLA)|| (c == GURUMUKHI_SHA) || \
        FALSE)
#define IS_SPLIT_MATRA_BRAHMI(c) ( \
        (c == BENGALI_SIGN_O) || (c == BENGALI_SIGN_AU) || \
        (c >= MLYM_O  && c <= MLYM_AU) || \
        FALSE)

static void
not_cursor_position (BreakOppo *bov)
{
    *bov &= ~BOV_GB_CURSOR_POS; // attr->is_cursor_position = FALSE;
    *bov &= ~BOV_GB_CHAR_BREAK; // attr->is_char_break = FALSE;
    *bov &= ~BOV_LB_MASK;       // attr->is_line_break = FALSE;
    *bov |= BOV_LB_NOTALLOWED;  // attr->is_mandatory_break = FALSE;
    //*bov &= ~BOV_LB_MANDATORY_FLAG;
}

void __mg_unicode_break_indic(ScriptType writing_system,
        const Uchar32 *ucs, int nr_ucs, BreakOppo* bos)
{
    const Uchar32 *p, *next = NULL, *next_next;
    Uchar32 prev_wc, this_wc, next_wc, next_next_wc;
    BOOL is_conjunct = FALSE;
    int i;

    for (p = ucs, prev_wc = 0, i = 0;
            p != NULL && p < (ucs + nr_ucs);
            p = next, prev_wc = this_wc, i++)
    {
        this_wc = p[0];
        next = p + 1;

        if (IS_COMPOSITE_WITH_BRAHMI_NUKTA(this_wc) ||
                IS_SPLIT_MATRA_BRAHMI(this_wc)) {
            // attrs[i+1].backspace_deletes_character = FALSE;
            bos[i + 1] &= ~BOV_GB_BACKSPACE_DEL_CH;
        }

        if (next != NULL && next < (ucs + nr_ucs)) {
            next_wc = next[0];
            next_next = next + 1;
        }
        else {
            next_wc = 0;
            next_next = NULL;
        }

        if (next_next != NULL && next_next < (ucs + nr_ucs))
            next_next_wc = *next_next;
        else
            next_next_wc = 0;

        switch (writing_system) {
        case SCRIPT_SINHALA:
            /*
             * TODO: The cursor position should be based on the state table.
             *       This is the wrong place to be doing this.
             */

            /*
             * The cursor should treat as a single glyph:
             * SINHALA CONS + 0x0DCA + 0x200D + SINHALA CONS
             * SINHALA CONS + 0x200D + 0x0DCA + SINHALA CONS
             */
            if ((this_wc == 0x0DCA && next_wc == 0x200D)
                    || (this_wc == 0x200D && next_wc == 0x0DCA)) {
                not_cursor_position(bos + i);
                not_cursor_position(bos + i + 1);
                is_conjunct = TRUE;
            }
            else if (is_conjunct
                    && (prev_wc == 0x200D || prev_wc == 0x0DCA)
                    && this_wc >= 0x0D9A
                    && this_wc <= 0x0DC6) {
                not_cursor_position(bos + i);
                is_conjunct = FALSE;
            }
            /*
             * Consonant clusters do NOT result in implicit conjuncts
             * in SINHALA orthography.
             */
            else if (!is_conjunct && prev_wc == 0x0DCA && this_wc != 0x200D) {
                // attrs[i].is_cursor_position = TRUE;
                bos[i] |= BOV_GB_CURSOR_POS;
            }

            break;

        default:
            if (prev_wc != 0 && (this_wc == 0x200D || this_wc == 0x200C)) {
                not_cursor_position(bos + i);
                if (next_wc != 0) {
                    not_cursor_position(bos + i+1);
                    if ((next_next_wc != 0) &&
                            (next_wc == 0x09CD ||    /* Bengali */
                             next_wc == 0x0ACD ||    /* Gujarati */
                             next_wc == 0x094D ||    /* Hindi */
                             next_wc == 0x0CCD ||    /* Kannada */
                             next_wc == 0x0D4D ||    /* Malayalam */
                             next_wc == 0x0B4D ||    /* Oriya */
                             next_wc == 0x0A4D ||    /* Punjabi */
                             next_wc == 0x0BCD ||    /* Tamil */
                             next_wc == 0x0C4D))    /* Telugu */
                    {
                        not_cursor_position(bos + i + 2);
                    }
                }
            }

            break;
        }
    }
}

#endif /* _MGCHARSET_UNICODE */

