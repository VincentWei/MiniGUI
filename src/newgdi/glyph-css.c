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

static int get_next_glyph(DEVFONT* mbc_devfont, DEVFONT* sbc_devfont,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc)
{
    int mclen = 0;

    if (mstr_len <= 0)
        return 0;

    if (mbc_devfont) {
        mclen = mbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)mstr, mstr_len);

        if (mclen > 0) {
            *gv = mbc_devfont->charset_ops->char_glyph_value
                (NULL, 0, (Uint8*)mstr, mclen);
            *gv = SET_MBC_GLYPH(*gv);

            if (mbc_devfont->charset_ops->conv_to_uc32)
                *uc = mbc_devfont->charset_ops->conv_to_uc32(*gv);
            else
                *uc = GLYPH2UCHAR(*gv);
        }
    }

    if (*gv == INV_GLYPH_VALUE) {
        mclen = sbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)mstr, mstr_len);

        if (mclen > 0) {
            *gv = sbc_devfont->charset_ops->char_glyph_value
                (NULL, 0, (Uint8*)mstr, mclen);
            if (sbc_devfont->charset_ops->conv_to_uc32)
                *uc = sbc_devfont->charset_ops->conv_to_uc32(*gv);
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

static int collapse_space(DEVFONT* mbc_devfont, DEVFONT* sbc_devfont,
    const char* mstr, int mstr_len)
{
    UCharBreakType bt;
    int cosumed = 0;

    do {
        int mclen;
        Glyph32 gv;
        Uchar32 uc;

        mclen = get_next_glyph(mbc_devfont, sbc_devfont, mstr, mstr_len,
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

static int collapse_line_feed(DEVFONT* mbc_devfont, DEVFONT* sbc_devfont,
    const char* mstr, int mstr_len)
{
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    mclen = get_next_glyph(mbc_devfont, sbc_devfont, mstr, mstr_len,
                    &gv, &uc);
    if (mclen > 0 && UCharGetBreak(uc) == UCHAR_BREAK_LINE_FEED)
        cosumed = mclen;

    return cosumed;
}

static BOOL is_next_glyph_zw(DEVFONT* mbc_devfont, DEVFONT* sbc_devfont,
    const char* mstr, int mstr_len)
{
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    mclen = get_next_glyph(mbc_devfont, sbc_devfont, mstr, mstr_len,
                    &gv, &uc);
    if (mclen > 0 && UCharGetBreak(uc) == UCHAR_BREAK_ZERO_WIDTH_SPACE)
        return TRUE;

    return FALSE;
}

static int is_next_glyph_cm_or_zwj(DEVFONT* mbc_devfont, DEVFONT* sbc_devfont,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc)
{
    int mclen;

    mclen = get_next_glyph(mbc_devfont, sbc_devfont, mstr, mstr_len,
                    gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_COMBINING_MARK
                || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER)
        return mclen;
    }

    return 0;
}

#define MIN_LEN_GLYPHS      4
#define INC_LEN_GLYPHS      4

struct glyph_break_info {
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

static int gbinfo_push_back(struct glyph_break_info* gbinfo, Glyph32 gv, Uint8 bt)
{
    /* realloc buffers if it needs */
    if ((gbinfo->n + 2) >= gbinfo->len_buff) {
        gbinfo->len_buff += INC_LEN_GLYPHS;
        gbinfo->gs = (Glyph32*)realloc(gbinfo->gs, sizeof(Glyph32) * gbinfo->len_buff);
        gbinfo->bs = (Uint8*)realloc(gbinfo->bs, sizeof(Uint8) * gbinfo->len_buff);
        if (gbinfo->gs == NULL || gbinfo->bs == NULL)
            return 0;
    }

    gbinfo->gs[gbinfo->n] = gv;
    gbinfo->bs[gbinfo->n] = bt;
    gbinfo->n++;
    return gbinfo->n;
}

static int check_subsequent_cm_or_zwj(DEVFONT* mbc_devfont, DEVFONT* sbc_devfont,
    const char* mstr, int mstr_len, struct glyph_break_info* gbinfo)
{
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while ((mclen = is_next_glyph_cm_or_zwj(mbc_devfont, sbc_devfont,
        mstr, mstr_len, &gv, &uc)) > 0) {

        // FIXME: CM/ZWJ should have the same break class as
        // its base character.
        gbinfo_push_back(gbinfo, gv, BOV_NOT_ALLOWED_BEFORE);

        cosumed += mclen;
    }

    return cosumed;
}

int GUIAPI GetGlyphsByRules(LOGFONT* logfont, const char* mstr, int mstr_len,
            LanguageCode content_language, UCharScriptType writing_system,
            Uint32 space_rule, Uint32 trans_rule,
            Glyph32** glyphs, Uint8** break_oppos, int* nr_glyphs)
{
    struct glyph_break_info gbinfo = {0, 0, NULL, NULL};
    int cosumed = 0;
    DEVFONT* sbc_devfont  = logfont->sbc_devfont;
    DEVFONT* mbc_devfont = logfont->mbc_devfont;

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

        Glyph32 next_gv;
        Uchar32 next_uc;
        UCharBasicType next_gc;
        UCharBreakType next_bt;
        int next_mclen;

        mclen = get_next_glyph(mbc_devfont, sbc_devfont, mstr, mstr_len,
                        &gv, &uc);
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

        // LB2 Never break at the start of text.
        if (gbinfo.n == 0) {
            Uint8 bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB3 Always break at the end of text.
        else if (*mstr == '\0') {
            break;
        }
        // LB4 Always break after hard line breaks
        // LB6 Do not break before hard line breaks.
        else if (bt == UCHAR_BREAK_MANDATORY) {
            Uint8 bo = BOV_AFTER_MANDATORY | BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB5 Treat CR followed by LF, as well as CR, LF,
        // and NL as hard line breaks.
        // LB6 Do not break before hard line breaks.
        else if (bt == UCHAR_BREAK_CARRIAGE_RETURN) {
            mclen += collapse_line_feed(mbc_devfont, sbc_devfont, mstr,
                    mstr_len);
            Uint8 bo = BOV_AFTER_MANDATORY | BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB5 Treat CR followed by LF, as well as CR, LF,
        // and NL as hard line breaks.
        // LB6 Do not break before hard line breaks.
        else if (bt == UCHAR_BREAK_LINE_FEED
                || bt == UCHAR_BREAK_NEXT_LINE) {
            Uint8 bo = BOV_AFTER_MANDATORY | BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB7: Do not break before spaces or zero width space.
        else if (bt == UCHAR_BREAK_SPACE) {
            Uint8 bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;

            if (space_rule == WSR_NORMAL || space_rule == WSR_NOWRAP) {
                // CSS: collapses space according to space rule
                mclen += collapse_space(mbc_devfont, sbc_devfont, mstr,
                    mstr_len);
            }
        }
        // LB7: Do not break before spaces or zero width space.
        else if (bt == UCHAR_BREAK_ZERO_WIDTH_SPACE) {
            Uint8 bo = BOV_BEFORE_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB8: Break before any character following a zero-width space,
        // even if one or more spaces intervene.
        else if (is_next_glyph_zw(mbc_devfont, sbc_devfont, mstr, mstr_len)) {
            Uint8 bo = BOV_BEFORE_MANDATORY;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }
        // LB8a Do not break between a zero width joiner and an ideograph,
        // emoji base or emoji modifier.
        else if (bt == UCHAR_BREAK_ZERO_WIDTH_JOINER) {
            next_mclen = get_next_glyph(mbc_devfont, sbc_devfont,
                            mstr, mstr_len, &next_gv, &next_uc);
            Uint8 bo;

            if (next_mclen > 0) {
                next_gc = UCharGetType(next_uc);
                next_bt = UCharGetBreak(next_uc);
                next_bt = resolve_line_breaking_class(content_language,
                        writing_system, next_gc, next_bt);
                if (next_bt == UCHAR_BREAK_IDEOGRAPHIC
                        || next_bt == UCHAR_BREAK_EMOJI_BASE
                        || next_bt == UCHAR_BREAK_EMOJI_MODIFIER) {

                    mclen += next_mclen;

                    bo = BOV_AFTER_NOTALLOWED;
                    if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                        goto error;

                    bo = BOV_BEFORE_NOTALLOWED;
                    if (gbinfo_push_back(&gbinfo, next_gv, bo) == 0)
                        goto error;
                }
            }
        }
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
                && (next_mclen = is_next_glyph_cm_or_zwj(mbc_devfont,
                    sbc_devfont, mstr, mstr_len, &next_gv, &next_uc)) > 0) {
            Uint8 bo;

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
            mclen += check_subsequent_cm_or_zwj(mbc_devfont,
                    sbc_devfont, mstr + next_mclen, mstr_len - next_mclen,
                    &gbinfo);

        }
        // LB10 Treat any remaining combining mark or ZWJ as AL.
        else if (bt == UCHAR_BREAK_COMBINING_MARK
                || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER) {
            bt = UCHAR_BREAK_ALPHABETIC;
        }
        // LB11 Do not break before or after Word joiner
        // and related characters.
        else if (bt == UCHAR_BREAK_WORD_JOINER) {
            Uint8 bo = BOV_BEFORE_NOTALLOWED | BOV_AFTER_NOTALLOWED;
            if (gbinfo_push_back(&gbinfo, gv, bo) == 0)
                goto error;
        }

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

