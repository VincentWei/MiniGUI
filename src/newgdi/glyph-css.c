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
** glyph-css.c: The implementation of APIs which conform to the specifiction
** of CSS 3
**
** Reference:
**
**  https://www.w3.org/TR/css-text-3/
**  https://www.w3.org/TR/css-writing-modes-3/
**
** Create by WEI Yongming at 2019/01/16
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "pixel_ops.h"
#include "cursor.h"
#include "drawtext.h"
#include "fixedmath.h"
#include "glyph.h"

#ifdef _MGCHARSET_UNICODE

#define MIN_LEN_GLYPHS      4
#define INC_LEN_GLYPHS      4

struct glyph_break_info {
    DEVFONT* mbc_devfont;
    DEVFONT* sbc_devfont;
    int      len_buff;
    int      n;
    Glyph32* gs;
    Uint8*   bs;
};

static int gbinfo_init(struct glyph_break_info* gbinfo, int size)
{
    // pre-allocate buffers
    gbinfo->len_buff = size;
    if (gbinfo->len_buff < MIN_LEN_GLYPHS)
        gbinfo->len_buff = MIN_LEN_GLYPHS;

    gbinfo->gs = (Glyph32*)malloc(sizeof(Glyph32) * gbinfo->len_buff);
    gbinfo->bs = (Uint8*)malloc(sizeof(Uint8) * gbinfo->len_buff);
    if (gbinfo->gs == NULL || gbinfo->bs == NULL)
        return 0;

    return gbinfo->len_buff;
}

static int gbinfo_push_back(struct glyph_break_info* gbinfo,
        Glyph32 gv, Uint8 bt)
{
    /* realloc buffers if it needs */
    if ((gbinfo->n + 2) >= gbinfo->len_buff) {
        gbinfo->len_buff += INC_LEN_GLYPHS;
        gbinfo->gs = (Glyph32*)realloc(gbinfo->gs,
            sizeof(Glyph32) * gbinfo->len_buff);
        gbinfo->bs = (Uint8*)realloc(gbinfo->bs,
            sizeof(Uint8) * gbinfo->len_buff);
        if (gbinfo->gs == NULL || gbinfo->bs == NULL)
            return 0;
    }

    gbinfo->gs[gbinfo->n] = gv;
    gbinfo->bs[gbinfo->n] = bt;
    gbinfo->n++;
    return gbinfo->n;
}

static BOOL gbinfo_change_bt(struct glyph_break_info* gbinfo, int i, Uint8 bt)
{
    Uint8 bb, ba;

    if (i >= gbinfo->n)
        return FALSE;
    else if (i < 0)
        i = gbinfo->n + i;

    if (i < 0)
        return FALSE;

    bb = bt & BOV_BEFORE_MASK;
    if (bb) {
        gbinfo->bs[i] = (gbinfo->bs[i] & ~BOV_BEFORE_MASK) | bb;
    }

    ba = bt & BOV_AFTER_MASK;
    if (ba) {
        gbinfo->bs[i] = (gbinfo->bs[i] & ~BOV_AFTER_MASK) | ba;
    }

    return TRUE;
}

static int get_next_glyph(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc)
{
    int mclen = 0;

    if (mstr_len <= 0)
        return 0;

    if (gbinfo->mbc_devfont) {
        mclen = gbinfo->mbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)mstr, mstr_len);

        if (mclen > 0) {
            *gv = gbinfo->mbc_devfont->charset_ops->char_glyph_value
                (NULL, 0, (Uint8*)mstr, mclen);
            *gv = SET_MBC_GLYPH(*gv);

            if (gbinfo->mbc_devfont->charset_ops->conv_to_uc32)
                *uc = gbinfo->mbc_devfont->charset_ops->conv_to_uc32(*gv);
            else
                *uc = GLYPH2UCHAR(*gv);
        }
    }

    if (*gv == INV_GLYPH_VALUE) {
        mclen = gbinfo->sbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)mstr, mstr_len);

        if (mclen > 0) {
            *gv = gbinfo->sbc_devfont->charset_ops->char_glyph_value
                (NULL, 0, (Uint8*)mstr, mclen);
            if (gbinfo->sbc_devfont->charset_ops->conv_to_uc32)
                *uc = gbinfo->sbc_devfont->charset_ops->conv_to_uc32(*gv);
            else
                *uc = GLYPH2UCHAR(*gv);
        }
    }

    return mclen;
}

static UCharBreakType resolve_line_breaking_class(
        LanguageCode content_language, UCharScriptType writing_system,
        UCharBasicType gc, UCharBreakType bt)
{
    /*
     * TODO: according to the content language and the writing system
     * to resolve AI, CB, CJ, SA, SG, and XX into other line breaking classes.
     */

    // default handling.
    switch (bt) {
    case UCHAR_BREAK_AMBIGUOUS:
    case UCHAR_BREAK_SURROGATE:
    case UCHAR_BREAK_UNKNOWN:
        bt = UCHAR_BREAK_ALPHABETIC;
        break;

    case UCHAR_BREAK_COMPLEX_CONTEXT:
        if (gc == UCHAR_TYPE_NON_SPACING_MARK
                || gc == UCHAR_TYPE_SPACING_MARK) {
            bt = UCHAR_BREAK_COMBINING_MARK;
        }
        else {
            bt = UCHAR_BREAK_ALPHABETIC;
        }
        break;

    case UCHAR_BREAK_CONDITIONAL_JAPANESE_STARTER:
        bt = UCHAR_BREAK_NON_STARTER;
        break;

    default:
        break;
    }

    return bt;
}

static int collapse_space(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len)
{
    UCharBreakType bt;
    int cosumed = 0;

    do {
        int mclen;
        Glyph32 gv;
        Uchar32 uc;

        mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                        &gv, &uc);
        if (mclen == 0)
            break;

        mstr += mclen;
        mstr_len -= mclen;
        cosumed += mclen;

        bt = UCharGetBreak(uc);
    } while (bt == UCHAR_BREAK_SPACE);

    return cosumed;
}

static int collapse_line_feed(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len)
{
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    &gv, &uc);
    if (mclen > 0 && UCharGetBreak(uc) == UCHAR_BREAK_LINE_FEED)
        cosumed = mclen;

    return cosumed;
}

static BOOL is_next_glyph_zw(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len)
{
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    &gv, &uc);
    if (mclen > 0 && UCharGetBreak(uc) == UCHAR_BREAK_ZERO_WIDTH_SPACE)
        return TRUE;

    return FALSE;
}

static int is_next_glyph_bt(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc,
    UCharBreakType bt)
{
    int mclen;

    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    gv, uc);
    if (mclen > 0 && UCharGetBreak(*uc) == bt)
        return mclen;

    return 0;
}

static inline int is_next_glyph_sp(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc)
{
    return is_next_glyph_bt(gbinfo, mstr, mstr_len, gv, uc,
            UCHAR_BREAK_SPACE);
}

static inline int is_next_glyph_gl(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc)
{
    return is_next_glyph_bt(gbinfo, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_NON_BREAKING_GLUE);
}

static inline int is_next_glyph_hl(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbinfo, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_HEBREW_LETTER);
}

static inline int is_next_glyph_in(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbinfo, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_INSEPARABLE);
}

static inline int is_next_glyph_nu(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbinfo, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_NUMERIC);
}

static inline int is_next_glyph_po(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbinfo, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_POSTFIX);
}

static inline int is_next_glyph_pr(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbinfo, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_PREFIX);
}

static inline int is_next_glyph_op(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbinfo, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_OPEN_PUNCTUATION);
}

static inline int is_next_glyph_jt(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbinfo, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_HANGUL_T_JAMO);
}

static int is_next_glyph_em(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbinfo, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_EMOJI_MODIFIER);
}

static int is_next_glyph_cm_zwj(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc)
{
    int mclen;

    mclen = get_next_glyph(gbinfo, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_COMBINING_MARK
                || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_hy_or_ba(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    int mclen;

    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_HYPHEN
                || bt == UCHAR_BREAK_AFTER)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_al_hl(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_HEBREW_LETTER || bt == UCHAR_BREAK_ALPHABETIC)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_pr_po(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_PREFIX || bt == UCHAR_BREAK_POSTFIX)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_id_eb_em(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_IDEOGRAPHIC
                || bt == UCHAR_BREAK_EMOJI_BASE
                || bt == UCHAR_BREAK_EMOJI_MODIFIER)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_jl_jv_jt_h2_h3(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_HANGUL_L_JAMO
                || bt == UCHAR_BREAK_HANGUL_V_JAMO
                || bt == UCHAR_BREAK_HANGUL_T_JAMO
                || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE
                || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_jl_jv_h2_h3(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_HANGUL_L_JAMO
                || bt == UCHAR_BREAK_HANGUL_V_JAMO
                || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE
                || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_jv_jt(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_HANGUL_V_JAMO
                || bt == UCHAR_BREAK_HANGUL_T_JAMO)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_al_hl_nu(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, Glyph32*gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_HEBREW_LETTER
                || bt == UCHAR_BREAK_ALPHABETIC
                || bt == UCHAR_BREAK_NUMERIC)
            return mclen;
    }

    return 0;
}

static int check_subsequent_cm_or_zwj(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len)
{
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while ((mclen = is_next_glyph_cm_zwj(gbinfo,
        mstr, mstr_len, &gv, &uc)) > 0) {

        // FIXME: CM/ZWJ should have the same break class as
        // its base character.
        gbinfo_push_back(gbinfo, gv, BOV_BEFORE_NOTALLOWED);

        cosumed += mclen;
    }

    return cosumed;
}

static int check_subsequent_sp(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len)
{
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while ((mclen = is_next_glyph_sp(gbinfo,
        mstr, mstr_len, &gv, &uc)) > 0) {

        gbinfo_push_back(gbinfo, gv, BOV_AFTER_NOTALLOWED);
        cosumed += mclen;
    }

    return cosumed;
}

static int is_subsequent_sps_and_end_bt(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, UCharBreakType end_bt)
{
    Glyph32 gv;
    Uchar32 uc;
    int mclen;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    &gv, &uc);
        if (mclen > 0) {
            UCharBreakType bt = UCharGetBreak(uc);
            if (bt == UCHAR_BREAK_SPACE) {
                mstr += mclen;
                mstr_len -= mclen;
                continue;
            }
            else if (bt == end_bt) {
                return TRUE;
            }
        }

        break;
    }

    return FALSE;
}

static int check_subsequent_sps_and_end_bt(struct glyph_break_info* gbinfo,
    const char* mstr, int mstr_len, UCharBreakType end_bt)
{
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_glyph(gbinfo, mstr, mstr_len,
                    &gv, &uc);
        if (mclen > 0) {
            UCharBreakType bt = UCharGetBreak(uc);
            if (bt == UCHAR_BREAK_SPACE) {
                mstr += mclen;
                mstr_len -= mclen;
                cosumed += mclen;

                gbinfo_push_back(gbinfo, gv,
                    BOV_BEFORE_NOTALLOWED | BOV_AFTER_NOTALLOWED);
                continue;
            }
            else if (bt == end_bt) {
                cosumed += mclen;

                gbinfo_push_back(gbinfo, gv, BOV_BEFORE_NOTALLOWED);
            }
        }

        break;
    }

    return cosumed;
}

static BOOL is_even_nubmer_of_subsequent_ri(struct glyph_break_info* gbinfo,
        const char* mstr, int mstr_len)
{
    int nr = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_glyph(gbinfo, mstr, mstr_len, &gv, &uc);
        mstr_len -= mclen;

        if (mclen > 0 && UCharGetBreak(uc) == UCHAR_BREAK_REGIONAL_INDICATOR) {
            nr++;
            continue;
        }
        else
            break;
    }

    if (nr > 0 && nr % 2 == 0)
        return TRUE;

    return FALSE;
}

static int check_even_nubmer_of_subsequent_ri(struct glyph_break_info* gbinfo,
        const char* mstr, int mstr_len)
{
    int nr = 0;
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_glyph(gbinfo, mstr, mstr_len, &gv, &uc);
        if (mclen > 0 && UCharGetBreak(uc) == UCHAR_BREAK_REGIONAL_INDICATOR) {
            mstr += mclen;
            mstr_len -= mclen;
            cosumed += mclen;
            nr++;

            gbinfo_push_back(gbinfo, gv,
                BOV_BEFORE_UNKNOWN | BOV_BEFORE_UNKNOWN);
            continue;
        }
        else
            break;
    }

    if (nr > 0 && nr % 2 == 0) {
        gbinfo_change_bt(gbinfo, -2, BOV_AFTER_NOTALLOWED);
        gbinfo_change_bt(gbinfo, -1, BOV_BEFORE_NOTALLOWED);
        return cosumed;
    }

    return 0;
}

int GUIAPI GetGlyphsByRules(LOGFONT* logfont, const char* mstr, int mstr_len,
            LanguageCode content_language, UCharScriptType writing_system,
            Uint32 space_rule, Uint32 trans_rule,
            Glyph32** glyphs, Uint8** break_oppos, int* nr_glyphs)
{
    struct glyph_break_info gbinfo;
    int cosumed = 0;

    gbinfo.mbc_devfont = logfont->mbc_devfont;
    gbinfo.sbc_devfont = logfont->sbc_devfont;
    gbinfo.len_buff = 0;
    gbinfo.n = 0;
    gbinfo.gs = NULL;
    gbinfo.bs = NULL;

    *glyphs = NULL;
    *break_oppos = NULL;
    *nr_glyphs = 0;

    if (mstr_len == 0)
        return 0;

    if (gbinfo_init(&gbinfo, mstr_len >> 1) <= 0) {
        goto error;
    }

    while (mstr_len > 0) {
        Glyph32 gv;
        Uchar32 uc;
        int mclen;
        UCharBasicType gc;
        UCharBreakType bt;
        Uint8 bo = 0;

        Glyph32 next_gv;
        Uchar32 next_uc;
        int next_mclen;

        mclen = get_next_glyph(&gbinfo, mstr, mstr_len, &gv, &uc);
        if (mclen == 0) {
            // badly encoded or end of text
            break;
        }

        if (space_rule == WSR_PRE || space_rule == WSR_NOWRAP) {
            // TODO: only break at forced line breaks.
        }
        else {
            // mark all breaking opportunities
        }

        // LB1 Resolve line breaking class
        gc = UCharGetType(uc);
        bt = UCharGetBreak(uc);
        bt = resolve_line_breaking_class(content_language, writing_system,
                gc, bt);

        /* Start and end of text */
        // LB2 Never break at the start of text.
        if (gbinfo.n == 0) {
            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB3 Always break at the end of text.
        else if (*mstr == '\0') {
            break;
        }

        /* Mandatory breaks */
        // LB4 Always break after hard line breaks
        // LB6 Do not break before hard line breaks.
        else if (bt == UCHAR_BREAK_MANDATORY) {
            bo = BOV_AFTER_MANDATORY | BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB5 Treat CR followed by LF, as well as CR, LF,
        // and NL as hard line breaks.
        // LB6 Do not break before hard line breaks.
        else if (bt == UCHAR_BREAK_CARRIAGE_RETURN) {
            mclen += collapse_line_feed(&gbinfo, mstr, mstr_len);
            bo = BOV_AFTER_MANDATORY | BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB5 Treat CR followed by LF, as well as CR, LF,
        // and NL as hard line breaks.
        // LB6 Do not break before hard line breaks.
        else if (bt == UCHAR_BREAK_LINE_FEED
                || bt == UCHAR_BREAK_NEXT_LINE) {
            bo = BOV_AFTER_MANDATORY | BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }

        /* Explicit breaks and non-breaks */
        // LB7: Do not break before spaces or zero width space.
        else if (bt == UCHAR_BREAK_SPACE
                || bt == UCHAR_BREAK_ZERO_WIDTH_SPACE) {
            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            if (space_rule == WSR_NORMAL || space_rule == WSR_NOWRAP) {
                // CSS: collapses space according to space rule
                mclen += collapse_space(&gbinfo, mstr, mstr_len);
            }
        }
        // LB7: Do not break before spaces or zero width space.
        else if (bt == UCHAR_BREAK_ZERO_WIDTH_SPACE) {
            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB8: Break before any character following a zero-width space,
        // even if one or more spaces intervene.
        else if (is_next_glyph_zw(&gbinfo, mstr, mstr_len)) {
            bo = BOV_BEFORE_MANDATORY;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB8a Do not break between a zero width joiner and an ideograph,
        // emoji base or emoji modifier.
        else if (bt == UCHAR_BREAK_ZERO_WIDTH_JOINER
                && (next_mclen = is_next_glyph_id_eb_em(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {

            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }

        /* Combining marks */
        // LB9: Do not break a combining character sequence;
        // treat it as if it has the line breaking class of
        // the base character in all of the following rules.
        // Treat ZWJ as if it were CM.
        else if (bt != UCHAR_BREAK_MANDATORY
                && bt != UCHAR_BREAK_CARRIAGE_RETURN
                && bt != UCHAR_BREAK_LINE_FEED
                && bt != UCHAR_BREAK_NEXT_LINE
                && bt != UCHAR_BREAK_SPACE
                && bt != UCHAR_BREAK_ZERO_WIDTH_SPACE
                && (next_mclen = is_next_glyph_cm_zwj(&gbinfo,
                    mstr, mstr_len, &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            // FIXME: CM/ZWJ should have the same break class as
            // its base character.
            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;

            // For any possible subsequent CM/ZWJ characters.
            mclen += check_subsequent_cm_or_zwj(&gbinfo,
                    mstr + mclen + next_mclen,
                    mstr_len - mclen - next_mclen);

        }
        // LB10 Treat any remaining combining mark or ZWJ as AL.
        else if (bt == UCHAR_BREAK_COMBINING_MARK
                || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER) {
            bt = UCHAR_BREAK_ALPHABETIC;
        }

        /* Word joiner */
        // LB11 Do not break before or after Word joiner
        // and related characters.
        else if (bt == UCHAR_BREAK_WORD_JOINER) {
            bo = BOV_BEFORE_NOTALLOWED | BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB12 Do not break after NBSP and related characters.
        else if (bt == UCHAR_BREAK_NON_BREAKING_GLUE) {
            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }

        /* Non-breaking characters */
        // LB12a Do not break before NBSP and related characters,
        // except after spaces and hyphens.
        if (bt != UCHAR_BREAK_SPACE
                && bt != UCHAR_BREAK_AFTER
                && bt != UCHAR_BREAK_HYPHEN
                && (next_mclen = is_next_glyph_gl(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo |= BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo |= BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }

        /* Opening and closing */
        // LB13 Do not break before ‘]’ or ‘!’ or ‘;’ or ‘/’, even after spaces.
        if (bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                || bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                || bt == UCHAR_BREAK_EXCLAMATION
                || bt == UCHAR_BREAK_INFIX_SEPARATOR
                || bt == UCHAR_BREAK_SYMBOL) {
            bo |= BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB14 Do not break after ‘[’, even after spaces.
        else if (bt == UCHAR_BREAK_OPEN_PUNCTUATION) {
            bo |= BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            // For any possible subsequent space.
            mclen += check_subsequent_sp(&gbinfo,
                    mstr + mclen, mstr_len - mclen);
        }
        // LB15 Do not break within ‘”[’, even with intervening spaces.
        else if (bt == UCHAR_BREAK_QUOTATION
                && (is_subsequent_sps_and_end_bt(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    UCHAR_BREAK_OPEN_PUNCTUATION))) {

            bo |= BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            // For subsequent spaces and OP.
            mclen += check_subsequent_sps_and_end_bt(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    UCHAR_BREAK_OPEN_PUNCTUATION);
        }
        // LB16 Do not break between closing punctuation and a nonstarter
        // (lb=NS), even with intervening spaces.
        else if ((bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                    || bt == UCHAR_BREAK_CLOSE_PARANTHESIS)
                && (is_subsequent_sps_and_end_bt(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    UCHAR_BREAK_NON_STARTER))) {

            bo |= BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            // For subsequent spaces and NS.
            mclen += check_subsequent_sps_and_end_bt(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    UCHAR_BREAK_NON_STARTER);
        }
        // LB17 Do not break within ‘——’, even with intervening spaces.
        else if (bt == UCHAR_BREAK_BEFORE_AND_AFTER
                && (is_subsequent_sps_and_end_bt(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    UCHAR_BREAK_BEFORE_AND_AFTER))) {

            bo |= BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            // For subsequent spaces and B2.
            mclen += check_subsequent_sps_and_end_bt(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    UCHAR_BREAK_BEFORE_AND_AFTER);
        }

        /* Spaces */
        // LB18 Break after spaces.
        if (bt == UCHAR_BREAK_SPACE) {
            bo |= BOV_AFTER_ALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }

        /* Special case rules */
        // LB19 Do not break before or after quotation marks, such as ‘ ” ’.
        if (bt == UCHAR_BREAK_QUOTATION) {
            bo |= BOV_BEFORE_NOTALLOWED | BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_CONTINGENT) {
            bo |= BOV_BEFORE_ALLOWED | BOV_AFTER_ALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB21 Do not break before hyphen-minus, other hyphens,
        // fixed-width spaces, small kana, and other non-starters,
        // or after acute accents.
        else if (bt == UCHAR_BREAK_AFTER
                || bt == UCHAR_BREAK_HYPHEN
                || bt == UCHAR_BREAK_NON_STARTER) {
            bo |= BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_BEFORE) {
            bo |= BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB21a Don't break after Hebrew + Hyphen.
        else if (bt == UCHAR_BREAK_HEBREW_LETTER
                && (next_mclen = is_next_glyph_hy_or_ba(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo |= BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo |= BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        // LB21b Don’t break between Solidus and Hebrew letters.
        else if (bt == UCHAR_BREAK_SYMBOL
                && (next_mclen = is_next_glyph_hl(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        // LB22 Do not break between two ellipses, or between letters,
        // numbers or exclamations and ellipsis.
        else if ((bt == UCHAR_BREAK_HEBREW_LETTER
                    || bt == UCHAR_BREAK_ALPHABETIC
                    || bt == UCHAR_BREAK_EXCLAMATION
                    || bt == UCHAR_BREAK_IDEOGRAPHIC
                    || bt == UCHAR_BREAK_EMOJI_BASE
                    || bt == UCHAR_BREAK_EMOJI_MODIFIER
                    || bt == UCHAR_BREAK_INSEPARABLE
                    || bt == UCHAR_BREAK_NUMERIC)
                && (next_mclen = is_next_glyph_in(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }

        /* Numbers */
        // LB23 Do not break between digits and letters.
        if ((bt == UCHAR_BREAK_HEBREW_LETTER
                    || bt == UCHAR_BREAK_ALPHABETIC)
                && (next_mclen = is_next_glyph_nu(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_NUMERIC
                && (next_mclen = is_next_glyph_al_hl(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        // LB23a Do not break between numeric prefixes and ideographs,
        // or between ideographs and numeric postfixes.
        else if (bt == UCHAR_BREAK_PREFIX
                && (next_mclen = is_next_glyph_id_eb_em(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if ((bt == UCHAR_BREAK_IDEOGRAPHIC
                    || bt == UCHAR_BREAK_EMOJI_BASE
                    || bt == UCHAR_BREAK_EMOJI_MODIFIER)
                && (next_mclen = is_next_glyph_po(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        // LB24 Do not break between numeric prefix/postfix and letters,
        // or between letters and prefix/postfix.
        else if ((bt == UCHAR_BREAK_PREFIX
                    || bt == UCHAR_BREAK_POSTFIX)
                && (next_mclen = is_next_glyph_al_hl(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if ((bt == UCHAR_BREAK_ALPHABETIC
                    || bt == UCHAR_BREAK_HEBREW_LETTER)
                && (next_mclen = is_next_glyph_pr_po(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        // LB25 Do not break between the following pairs of classes
        // relevant to numbers
        else if (bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                && (next_mclen = is_next_glyph_po(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                && (next_mclen = is_next_glyph_po(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                && (next_mclen = is_next_glyph_pr(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                && (next_mclen = is_next_glyph_pr(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_NUMERIC
                && (next_mclen = is_next_glyph_pr(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_NUMERIC
                && (next_mclen = is_next_glyph_pr(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_POSTFIX
                && (next_mclen = is_next_glyph_op(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_POSTFIX
                && (next_mclen = is_next_glyph_nu(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_PREFIX
                && (next_mclen = is_next_glyph_op(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_PREFIX
                && (next_mclen = is_next_glyph_nu(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_HYPHEN
                && (next_mclen = is_next_glyph_nu(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_INFIX_SEPARATOR
                && (next_mclen = is_next_glyph_nu(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_NUMERIC
                && (next_mclen = is_next_glyph_nu(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_SYMBOL
                && (next_mclen = is_next_glyph_nu(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }

        /* Korean syllable blocks */
        // LB26 Do not break a Korean syllable.
        else if (bt == UCHAR_BREAK_HANGUL_L_JAMO
                && (next_mclen = is_next_glyph_jl_jv_h2_h3(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if ((bt == UCHAR_BREAK_HANGUL_V_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE)
                && (next_mclen = is_next_glyph_jv_jt(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if ((bt == UCHAR_BREAK_HANGUL_T_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE)
                && (next_mclen = is_next_glyph_jt(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        // LB27 Treat a Korean Syllable Block the same as ID.
        else if ((bt == UCHAR_BREAK_HANGUL_L_JAMO
                    || bt == UCHAR_BREAK_HANGUL_V_JAMO
                    || bt == UCHAR_BREAK_HANGUL_T_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE
                    || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE)
                && (next_mclen = is_next_glyph_in(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if ((bt == UCHAR_BREAK_HANGUL_L_JAMO
                    || bt == UCHAR_BREAK_HANGUL_V_JAMO
                    || bt == UCHAR_BREAK_HANGUL_T_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE
                    || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE)
                && (next_mclen = is_next_glyph_po(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_PREFIX
                && (next_mclen = is_next_glyph_jl_jv_jt_h2_h3(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }

        /* Finally, join alphabetic letters into words
           and break everything else. */

        // LB28 Do not break between alphabetics (“at”).
        if ((bt == UCHAR_BREAK_HEBREW_LETTER
                    || bt == UCHAR_BREAK_ALPHABETIC)
                && (next_mclen = is_next_glyph_al_hl(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }

        // LB29 Do not break between numeric punctuation
        // and alphabetics (“e.g.”).
        else if (bt == UCHAR_BREAK_INFIX_SEPARATOR
                && (next_mclen = is_next_glyph_al_hl(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }

        // LB30 Do not break between letters, numbers, or ordinary symbols and
        // opening or closing parentheses.
        else if ((bt == UCHAR_BREAK_HEBREW_LETTER
                    || bt == UCHAR_BREAK_ALPHABETIC
                    || bt == UCHAR_BREAK_NUMERIC)
                && (next_mclen = is_next_glyph_op(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }
        else if (bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                && (next_mclen = is_next_glyph_al_hl_nu(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }

        // LB30a Break between two regional indicator symbols if and only if
        // there are an even number of regional indicators preceding the
        // position of the break.
        if (gbinfo.n == 0 && bt == UCHAR_BREAK_REGIONAL_INDICATOR
                && is_even_nubmer_of_subsequent_ri(&gbinfo,
                    mstr - mclen, mstr_len + mclen)) {
            mclen = check_even_nubmer_of_subsequent_ri(&gbinfo,
                mstr - mclen, mstr_len + mclen);
        }
        else if (bt != UCHAR_BREAK_REGIONAL_INDICATOR
                && is_even_nubmer_of_subsequent_ri(&gbinfo,
                    mstr + mclen, mstr_len - mclen)) {

            bo = BOV_BEFORE_UNKNOWN | BOV_AFTER_ALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            mclen += check_even_nubmer_of_subsequent_ri(&gbinfo,
                mstr + mclen, mstr_len - mclen);
        }

        // LB30b Do not break between an emoji base and an emoji modifier.
        if (bt == UCHAR_BREAK_EMOJI_BASE
                && (next_mclen = is_next_glyph_em(&gbinfo,
                    mstr + mclen, mstr_len - mclen,
                    &next_gv, &next_uc)) > 0) {
            mclen += next_mclen;

            bo = BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                goto error;
        }

        // LB31 Break everywhere else.

        mstr_len -= mclen;
        mstr += mclen;
        cosumed += mclen;

        // Return if we got any BK!
        if ((gbinfo.bs[gbinfo.n] & BOV_AFTER_MASK) == BOV_AFTER_MANDATORY) {
            break;
        }
    }

    if (gbinfo.n == 0) {
        if (gbinfo.gs) free(gbinfo.gs);
        if (gbinfo.bs) free(gbinfo.bs);
        return 0;
    }
    else {
        *glyphs = gbinfo.gs;
        *break_oppos = gbinfo.bs;
        *nr_glyphs = gbinfo.n;
    }

    return cosumed;

error:
    if (gbinfo.gs) free(gbinfo.gs);
    if (gbinfo.bs) free(gbinfo.bs);
    return 0;
}

PLOGFONT GUIAPI GetGlyphsExtentPointEx (LOGFONT* logfont, int x, int y,
            const Glyph32* glyphs, const Uint8* break_oppos, int nr_glyphs,
            Uint32 reander_flags, Uint32 space_rule,
            int letter_spacing, int word_spacing, int tab_size, int max_extent,
            SIZE* line_size, GLYPHEXTINFO* glyph_ext_info, GLYPHPOSORT* pos_orts,
            int* nr_to_fit)
{
    return NULL;
}

#endif /*  _MGCHARSET_UNICODE */

BOOL GUIAPI DrawGlyphStringEx (HDC hdc, const Glyph32* glyphs, int nr_glyphs,
        const GLYPHPOSORT* pos_orts, PLOGFONT logfont_sideways)
{
    return TRUE;
}

