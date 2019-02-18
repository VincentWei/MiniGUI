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

#define DEBUG

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
#define UCHAR_BREAK_UNSET   -1

#define UCHAR_SPACE         0x0020
#define UCHAR_TAB           0x0009

enum _LBOrder {
    LB1, LB2, LB3, LB4, LB5, LB6, LB7, LB8, LB8a, LB9, LB10,
    LB11, LB12, LB12a, LB13, LB14, LB15, LB16, LB17, LB18, LB19,
    LB20, LB21, LB21a, LB21b, LB22, LB23, LB23a, LB24, LB25, LB26,
    LB27, LB28, LB29, LB30, LB30a, LB30b, LB31,
};

struct glyph_break_ctxt {
    DEVFONT* mbc_devfont;
    DEVFONT* sbc_devfont;
    Glyph32* gs;
    Uint8*   bts;
    Uint8*   bos;
    Uint8*   od;

    int      len_buff;
    int      n;
    int      base_bt;

    LanguageCode cl;
    UCharScriptType ws;

    Uint8   curr_od;
    Uint8   wsr;
    Uint8   ctr;
    Uint8   wbr;
    Uint8   lbp;
};

static int gbctxt_init(struct glyph_break_ctxt* gbctxt, int size)
{
    // pre-allocate buffers
    gbctxt->len_buff = size;
    if (gbctxt->len_buff < MIN_LEN_GLYPHS)
        gbctxt->len_buff = MIN_LEN_GLYPHS;

    gbctxt->gs = (Glyph32*)malloc(sizeof(Glyph32) * gbctxt->len_buff);
    gbctxt->bts = (Uint8*)malloc(sizeof(Uint8) * gbctxt->len_buff);
    gbctxt->bos = (Uint8*)malloc(sizeof(Uint8) * gbctxt->len_buff);
    gbctxt->od = (Uint8*)malloc(sizeof(Uint8) * gbctxt->len_buff);
    if (gbctxt->gs == NULL || gbctxt->bts == NULL
            || gbctxt->bos == NULL || gbctxt->od == NULL)
        return 0;

    return gbctxt->len_buff;
}

static int gbctxt_push_back(struct glyph_break_ctxt* gbctxt,
        Glyph32 gv, UCharBreakType bt, Uint8 bo)
{
    /* realloc buffers if it needs */
    if ((gbctxt->n + 2) >= gbctxt->len_buff) {
        gbctxt->len_buff += INC_LEN_GLYPHS;
        gbctxt->gs = (Glyph32*)realloc(gbctxt->gs,
            sizeof(Glyph32) * gbctxt->len_buff);
        gbctxt->bts = (Uint8*)realloc(gbctxt->bts,
            sizeof(Uint8) * gbctxt->len_buff);
        gbctxt->bos = (Uint8*)realloc(gbctxt->bos,
            sizeof(Uint8) * gbctxt->len_buff);
        gbctxt->od = (Uint8*)realloc(gbctxt->od,
            sizeof(Uint8) * gbctxt->len_buff);

        if (gbctxt->gs == NULL || gbctxt->bts == NULL
                || gbctxt->bos == NULL || gbctxt->od == NULL)
            return 0;
    }

    if (gbctxt->n == 0) {
        // set the before break opportunity
        gbctxt->bos[0] = bo;
        gbctxt->od[0] = 0xFF;
    }
    else {
        // set the after break opportunity
        gbctxt->gs[gbctxt->n - 1] = gv;
        gbctxt->bts[gbctxt->n - 1] = (Uint8)bt;
        gbctxt->bos[gbctxt->n] = bo;
        if (bo == BOV_UNKNOWN)
            gbctxt->od[gbctxt->n] = 0xFF;
        else
            gbctxt->od[gbctxt->n] = gbctxt->curr_od;
    }

    gbctxt->n++;
    return gbctxt->n;
}

static BOOL gbctxt_change_bo_last(struct glyph_break_ctxt* gbctxt,
        Uint8 bo)
{
    if (gbctxt->n < 1)
        return FALSE;

    if (gbctxt->bos[gbctxt->n - 1] == BOV_UNKNOWN) {
         gbctxt->bos[gbctxt->n - 1] = bo;
         gbctxt->od[gbctxt->n - 1] = gbctxt->curr_od;
    }
    else if (gbctxt->bos[gbctxt->n - 1] & BOV_MANDATORY_FLAG) {
        _DBG_PRINTF("%s: ignore the change: old one is mandatory\n",
            __FUNCTION__);
    }
    else if (gbctxt->curr_od <= gbctxt->od[gbctxt->n - 1]) {
        _DBG_PRINTF("%s: changed: curr_od(%d), org_od(%d)\n",
            __FUNCTION__, gbctxt->curr_od, gbctxt->od[gbctxt->n - 1]);
         gbctxt->bos[gbctxt->n - 1] = bo;
         gbctxt->od[gbctxt->n - 1] = gbctxt->curr_od;
    }
    else {
        _DBG_PRINTF("%s: ignore the change: curr_od(%d), org_od(%d)\n",
            __FUNCTION__, gbctxt->curr_od, gbctxt->od[gbctxt->n - 1]);
    }

    return TRUE;
}

static BOOL gbctxt_change_bo_before_last(struct glyph_break_ctxt* gbctxt,
        Uint8 bo)
{
    // do not allow to change the first break value
    if (gbctxt->n < 3) {
        return FALSE;
    }

#if 0
    // do not apply this bo to the char before last one
    if (gbctxt->base_bt != UCHAR_BREAK_UNSET) {
        return FALSE;
    }
#endif

    if (gbctxt->bos[gbctxt->n - 2] == BOV_UNKNOWN) {
         gbctxt->bos[gbctxt->n - 2] = bo;
         gbctxt->od[gbctxt->n - 2] = gbctxt->curr_od;
    }
    else if (gbctxt->bos[gbctxt->n - 2] & BOV_MANDATORY_FLAG) {
        _DBG_PRINTF("%s: ignore the change: old one is mandatory\n",
            __FUNCTION__);
    }
#if 0
    // Because rules are applied in order, allowing breaks following SP
    // in rule LB18 implies that any prohibited break in rules
    // LB19–LB30 is equivalent to an indirect break.
    else if (gbctxt->curr_od > LB18
            && GLYPH2UCHAR(gbctxt->gs[gbctxt->n - 3]) == UCHAR_SPACE) {
        _DBG_PRINTF("%s: ignore the change\n", __FUNCTION__);
    }
#endif
    else if (gbctxt->curr_od <= gbctxt->od[gbctxt->n - 2]) {
        _DBG_PRINTF("%s: changed: curr_od(%d), org_od(%d)\n",
            __FUNCTION__, gbctxt->curr_od, gbctxt->od[gbctxt->n - 2]);
         gbctxt->bos[gbctxt->n - 2] = bo;
         gbctxt->od[gbctxt->n - 2] = gbctxt->curr_od;
    }
    else {
        _DBG_PRINTF("%s: ignore the change\n", __FUNCTION__);
    }

    return TRUE;
}

#if 0
static BOOL gbctxt_change_bo_before_last_sp(struct glyph_break_ctxt* gbctxt,
        Uint8 bo)
{
    // do not allow to change the first break value
    if (gbctxt->n < 3) {
        return FALSE;
    }

    if (GLYPH2UCHAR(gbctxt->gs[gbctxt->n - 3]) == UCHAR_SPACE) {
        _DBG_PRINTF("%s: force changed: curr_od(%d), org_od(%d)\n",
            __FUNCTION__, gbctxt->curr_od, gbctxt->od[gbctxt->n - 2]);
         gbctxt->bos[gbctxt->n - 2] = bo;
         gbctxt->od[gbctxt->n - 2] = gbctxt->curr_od;
    }
    else if (gbctxt->curr_od < gbctxt->od[gbctxt->n - 2]) {
        _DBG_PRINTF("%s: changed: curr_od(%d), org_od(%d)\n",
            __FUNCTION__, gbctxt->curr_od, gbctxt->od[gbctxt->n - 2]);
         gbctxt->bos[gbctxt->n - 2] = bo;
         gbctxt->od[gbctxt->n - 2] = gbctxt->curr_od;
    }
    else {
        _DBG_PRINTF("%s: ignore the change\n", __FUNCTION__);
    }

    return TRUE;
}
#endif

static int get_next_glyph(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc)
{
    int mclen = 0;

    if (mstr_len <= 0 || *mstr == '\0')
        return 0;

    if (gbctxt->mbc_devfont) {
        mclen = gbctxt->mbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)mstr, mstr_len);

        if (mclen > 0) {
            *gv = gbctxt->mbc_devfont->charset_ops->char_glyph_value
                (NULL, 0, (Uint8*)mstr, mclen);
            *gv = SET_MBC_GLYPH(*gv);

            if (gbctxt->mbc_devfont->charset_ops->conv_to_uc32)
                *uc = gbctxt->mbc_devfont->charset_ops->conv_to_uc32(*gv);
            else
                *uc = GLYPH2UCHAR(*gv);
        }
    }

    if (*gv == INV_GLYPH_VALUE) {
        mclen = gbctxt->sbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)mstr, mstr_len);

        if (mclen > 0) {
            *gv = gbctxt->sbc_devfont->charset_ops->char_glyph_value
                (NULL, 0, (Uint8*)mstr, mclen);
            if (gbctxt->sbc_devfont->charset_ops->conv_to_uc32)
                *uc = gbctxt->sbc_devfont->charset_ops->conv_to_uc32(*gv);
            else
                *uc = GLYPH2UCHAR(*gv);
        }
    }

    return mclen;
}

static UCharBreakType resolve_lbc(struct glyph_break_ctxt* gbctxt, Uchar32 uc,
        UCharGeneralCategory* pgc)
{
    UCharGeneralCategory gc = UCharGetType(uc);
    UCharBreakType bt = UCharGetBreak(uc);

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

    case UCHAR_BREAK_COMBINING_MARK:
    case UCHAR_BREAK_ZERO_WIDTH_JOINER:
        if (gbctxt && gbctxt->base_bt != UCHAR_BREAK_UNSET) {
            _DBG_PRINTF ("CM as if it has the line breaking class of the base character(%d)\n", gbctxt->base_bt);
            // CM/ZWJ should have the same break class as
            // its base character.
            bt = gbctxt->base_bt;
        }
        break;

    default:
        break;
    }

    /*
     * Breaking is allowed within “words”: specifically, in addition to
     * soft wrap opportunities allowed for normal, any typographic character
     * units resolving to the NU (“numeric”), AL (“alphabetic”), or
     * SA (“Southeast Asian”) line breaking classes are instead treated
     * as ID (“ideographic characters”) for the purpose of line-breaking.
     */
    if (gbctxt->wbr == WBR_BREAK_ALL &&
            (bt == UCHAR_BREAK_NUMERIC || bt == UCHAR_BREAK_ALPHABETIC
                || bt == UCHAR_BREAK_COMPLEX_CONTEXT)) {
        bt = UCHAR_BREAK_IDEOGRAPHIC;
    }

    if (pgc)
        *pgc = gc;

    return bt;
}

static int collapse_space(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len)
{
    Uchar32 uc;
    UCharBreakType bt;
    int cosumed = 0;

    do {
        int mclen;
        Glyph32 gv;

        mclen = get_next_glyph(gbctxt, mstr, mstr_len, &gv, &uc);
        if (mclen == 0)
            break;

        mstr += mclen;
        mstr_len -= mclen;
        cosumed += mclen;

        bt = resolve_lbc(gbctxt, uc, NULL);
    } while (bt == UCHAR_BREAK_SPACE || uc == UCHAR_TAB);

    return cosumed;
}

static int check_glyphs_following_zw(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len)
{
    int cosumed = 0;

    do {
        int mclen;
        Glyph32 gv;
        Uchar32 uc;

        mclen = get_next_glyph(gbctxt, mstr, mstr_len, &gv, &uc);
        if (mclen > 0) {
            mstr += mclen;
            mstr_len -= mclen;

            if (resolve_lbc(gbctxt, uc, NULL) == UCHAR_BREAK_SPACE) {
                cosumed += mclen;
                gbctxt_change_bo_last(gbctxt, BOV_NOTALLOWED_DEFINITELY);
                gbctxt_push_back(gbctxt, gv, UCHAR_BREAK_SPACE,
                    BOV_NOTALLOWED_DEFINITELY);
            }
            else {
                break;
            }
        }
        else
            break;

    } while (TRUE);

    return cosumed;
}

static int is_next_glyph_bt(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc,
    UCharBreakType bt)
{
    int mclen;

    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0 && resolve_lbc(gbctxt, *uc, NULL) == bt)
        return mclen;

    return 0;
}

static BOOL is_glyph_letter(UCharGeneralCategory gc, UCharBreakType bt)
{
    if ((gc >= UCHAR_TYPE_LOWERCASE_LETTER
                && gc <= UCHAR_TYPE_UPPERCASE_LETTER)
            || (gc >= UCHAR_TYPE_DECIMAL_NUMBER
                && gc <= UCHAR_TYPE_OTHER_NUMBER))
        return TRUE;

    if (bt == UCHAR_BREAK_NUMERIC
            || bt == UCHAR_BREAK_ALPHABETIC
            || bt == UCHAR_BREAK_IDEOGRAPHIC
            || bt == UCHAR_BREAK_AMBIGUOUS)
        return TRUE;

    return FALSE;
}

static int is_next_glyph_letter(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc,
    UCharBreakType* pbt)
{
    int mclen;
    UCharGeneralCategory gc;
    UCharBreakType bt;

    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        bt = resolve_lbc(gbctxt, *uc, &gc);
        if (is_glyph_letter(gc, bt)) {
            if (pbt) *pbt = bt;
            return mclen;
        }
    }

    return 0;
}

static inline int is_next_glyph_lf(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
            UCHAR_BREAK_LINE_FEED);
}

static inline int is_next_glyph_sp(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
            UCHAR_BREAK_SPACE);
}

static inline int is_next_glyph_gl(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_NON_BREAKING_GLUE);
}

static inline int is_next_glyph_hl(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_HEBREW_LETTER);
}

static inline int is_next_glyph_in(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_INSEPARABLE);
}

static inline int is_next_glyph_nu(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_NUMERIC);
}

static inline int is_next_glyph_po(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_POSTFIX);
}

static inline int is_next_glyph_pr(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_PREFIX);
}

static inline int is_next_glyph_op(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_OPEN_PUNCTUATION);
}

static inline int is_next_glyph_jt(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_HANGUL_T_JAMO);
}

static inline int is_next_glyph_ri(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_REGIONAL_INDICATOR);
}

static inline int is_next_glyph_em(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, gv, uc,
        UCHAR_BREAK_EMOJI_MODIFIER);
}

static int is_next_glyph_cm_zwj(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32* uc,
    UCharBreakType* pbt)
{
    int mclen;

    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreak(*uc);
        if (bt == UCHAR_BREAK_COMBINING_MARK
                || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER) {
            if (pbt) *pbt = bt;
            return mclen;
        }
    }

    return 0;
}

static int is_next_glyph_hy_ba(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc,
    UCharBreakType* pbt)
{
    int mclen;

    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_HYPHEN
                || bt == UCHAR_BREAK_AFTER) {
            if (pbt) *pbt = bt;
            return mclen;
        }
    }

    return 0;
}

static int is_next_glyph_al_hl(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_HEBREW_LETTER || bt == UCHAR_BREAK_ALPHABETIC)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_pr_po(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_PREFIX || bt == UCHAR_BREAK_POSTFIX)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_id_eb_em(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_IDEOGRAPHIC
                || bt == UCHAR_BREAK_EMOJI_BASE
                || bt == UCHAR_BREAK_EMOJI_MODIFIER)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_jl_jv_jt_h2_h3(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc,
    UCharBreakType* pbt)
{
    int mclen;
    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_HANGUL_L_JAMO
                || bt == UCHAR_BREAK_HANGUL_V_JAMO
                || bt == UCHAR_BREAK_HANGUL_T_JAMO
                || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE
                || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE) {
            if (pbt) *pbt = bt;
            return mclen;
        }
    }

    return 0;
}

static int is_next_glyph_jl_jv_h2_h3(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc,
    UCharBreakType* pbt)
{
    int mclen;
    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_HANGUL_L_JAMO
                || bt == UCHAR_BREAK_HANGUL_V_JAMO
                || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE
                || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE) {
            if (pbt) *pbt = bt;
            return mclen;
        }
    }

    return 0;
}

static int is_next_glyph_jv_jt(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc,
    UCharBreakType* pbt)
{
    int mclen;
    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_HANGUL_V_JAMO
                || bt == UCHAR_BREAK_HANGUL_T_JAMO) {
            if (pbt) *pbt = bt;
            return mclen;
        }
    }

    return 0;
}

static int is_next_glyph_al_hl_nu(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_HEBREW_LETTER
                || bt == UCHAR_BREAK_ALPHABETIC
                || bt == UCHAR_BREAK_NUMERIC)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_cl_cp_is_sy(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                || bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                || bt == UCHAR_BREAK_INFIX_SEPARATOR
                || bt == UCHAR_BREAK_SYMBOL)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_nu_OR_op_hy_followed_nu(
    struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    int mclen;
    int next_mclen;
    Glyph32 next_gv;
    Uchar32 next_uc;

    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_NUMERIC) {
            return mclen;
        }
        else if ((bt == UCHAR_BREAK_OPEN_PUNCTUATION
                    || bt == UCHAR_BREAK_HYPHEN)
                && (next_mclen = is_next_glyph_nu(gbctxt,
                    mstr + mclen, mstr_len - mclen, &next_gv, &next_uc)) > 0) {
            return mclen + next_mclen;
        }
    }

    return 0;
}

static int is_next_glyph_nu_sy_is(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Glyph32* gv, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_glyph(gbctxt, mstr, mstr_len, gv, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc, NULL);
        if (bt == UCHAR_BREAK_NUMERIC
                || bt == UCHAR_BREAK_SYMBOL
                || bt == UCHAR_BREAK_INFIX_SEPARATOR)
            return mclen;
    }

    return 0;
}

static BOOL are_prev_glyphs_nu_AND_nu_sy_is(
        const struct glyph_break_ctxt* gbctxt, BOOL before_last)
{
    int last, i;

    if (before_last)
        last = gbctxt->n - 4;
    else
        last = gbctxt->n - 3;

    if (last < 0)
        return FALSE;

    _DBG_PRINTF("%s: break type of last (%d/%d): %d\n",
            __FUNCTION__, last, gbctxt->n, gbctxt->bts[last]);
    if (gbctxt->bts[last] == UCHAR_BREAK_NUMERIC)
        return TRUE;

    i = last;
    while (i > 0) {
        UCharBreakType bt = gbctxt->bts[i];
        if (bt == UCHAR_BREAK_NUMERIC
                || bt == UCHAR_BREAK_SYMBOL
                || bt == UCHAR_BREAK_INFIX_SEPARATOR)
            i--;
        else
            break;
    }

    _DBG_PRINTF("%s: break type of first (%d/%d): %d\n",
            __FUNCTION__, i, gbctxt->n, gbctxt->bts[i]);
    if (i == last)
        return FALSE;

    if (gbctxt->bts[i + 1] == UCHAR_BREAK_NUMERIC) {
        return TRUE;
    }

    return FALSE;
}

static BOOL are_prev_glyphs_nu_AND_nu_sy_is_AND_cl_cp(
    const struct glyph_break_ctxt* gbctxt)
{
    int last = gbctxt->n - 3;
    UCharBreakType bt = gbctxt->bts[last];

    if (bt == UCHAR_BREAK_CLOSE_PUNCTUATION
            || bt == UCHAR_BREAK_CLOSE_PARANTHESIS) {
        return are_prev_glyphs_nu_AND_nu_sy_is(gbctxt, TRUE);
    }

    return are_prev_glyphs_nu_AND_nu_sy_is(gbctxt, FALSE);
}

#if 0
static BOOL is_next_glyph_zw(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len)
{
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    mclen = get_next_glyph(gbctxt, mstr, mstr_len, &gv, &uc);
    if (mclen > 0 && resolve_lbc(gbctxt, uc, NULL)
            == UCHAR_BREAK_ZERO_WIDTH_SPACE)
        return TRUE;

    return FALSE;
}
#endif

static int check_subsequent_cm_zwj(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len)
{
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;
    UCharBreakType bt;

    while ((mclen = is_next_glyph_cm_zwj(gbctxt, mstr, mstr_len,
            &gv, &uc, &bt)) > 0) {

        // CM/ZWJ should have the same break class as
        // its base character.
        gbctxt_push_back(gbctxt, gv, gbctxt->base_bt,
                BOV_NOTALLOWED_DEFINITELY);

        mstr += mclen;
        mstr_len -= mclen;
        cosumed += mclen;
    }

    return cosumed;
}

static int check_subsequent_sp(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len)
{
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while ((mclen = is_next_glyph_sp(gbctxt, mstr, mstr_len, &gv, &uc)) > 0) {
        gbctxt_push_back(gbctxt, gv,
            UCHAR_BREAK_SPACE, BOV_NOTALLOWED_UNCERTAINLY);
        mstr += mclen;
        mstr_len -= mclen;
        cosumed += mclen;
    }

    return cosumed;
}

static int is_subsequent_sps_and_end_bt(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, UCharBreakType end_bt)
{
    Glyph32 gv;
    Uchar32 uc;
    int mclen;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_glyph(gbctxt, mstr, mstr_len, &gv, &uc);
        if (mclen > 0) {
            UCharBreakType bt = resolve_lbc(gbctxt, uc, NULL);
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

static int check_subsequent_sps_and_end_bt(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, BOOL col_sp, UCharBreakType end_bt)
{
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_glyph(gbctxt, mstr, mstr_len, &gv, &uc);
        if (mclen > 0) {
            UCharBreakType bt = resolve_lbc(gbctxt, uc, NULL);
            _DBG_PRINTF("%s: %04X (%d)\n", __FUNCTION__, uc, bt);
            if (bt == UCHAR_BREAK_SPACE) {
                mstr += mclen;
                mstr_len -= mclen;
                cosumed += mclen;
                if (!col_sp)
                    gbctxt_push_back(gbctxt,
                        gv, bt, BOV_NOTALLOWED_UNCERTAINLY);
                continue;
            }
            else if (bt == end_bt) {
                return cosumed;
            }
        }

        break;
    }

    return cosumed;
}

#if 0
static BOOL is_odd_nubmer_of_subsequent_ri(struct glyph_break_ctxt* gbctxt,
        const char* mstr, int mstr_len)
{
    int nr = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_glyph(gbctxt, mstr, mstr_len, &gv, &uc);
        mstr += mclen;
        mstr_len -= mclen;

        if (mclen > 0 && resolve_lbc(gbctxt, uc, NULL)
                    == UCHAR_BREAK_REGIONAL_INDICATOR) {
            nr++;
            continue;
        }
        else
            break;
    }

    if (nr > 0 && nr % 2 != 0)
        return TRUE;

    return FALSE;
}
#endif

static BOOL is_even_nubmer_of_subsequent_ri(struct glyph_break_ctxt* gbctxt,
        const char* mstr, int mstr_len)
{
    int nr = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_glyph(gbctxt, mstr, mstr_len, &gv, &uc);
        mstr += mclen;
        mstr_len -= mclen;

        if (mclen > 0 && resolve_lbc(gbctxt, uc, NULL)
                == UCHAR_BREAK_REGIONAL_INDICATOR) {
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

static int check_subsequent_ri(struct glyph_break_ctxt* gbctxt,
        const char* mstr, int mstr_len)
{
    int cosumed = 0;
    int mclen;
    Glyph32 gv;
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_glyph(gbctxt, mstr, mstr_len, &gv, &uc);
        if (mclen > 0 && resolve_lbc(gbctxt, uc, NULL)
                    == UCHAR_BREAK_REGIONAL_INDICATOR) {
            mstr += mclen;
            mstr_len -= mclen;
            cosumed += mclen;

            gbctxt_push_back(gbctxt, gv,
                UCHAR_BREAK_REGIONAL_INDICATOR, BOV_NOTALLOWED_UNCERTAINLY);
            continue;
        }
        else
            break;
    }

    return cosumed;
}

int GUIAPI GetGlyphsByRules(LOGFONT* logfont, const char* mstr, int mstr_len,
            LanguageCode content_language, UCharScriptType writing_system,
            Uint8 wsr, Uint8 ctr, Uint8 wbr, Uint8 lbp,
            Glyph32** glyphs, Uint8** break_oppos, int* nr_glyphs)
{
    struct glyph_break_ctxt gbctxt;
    int cosumed = 0;
    BOOL col_sp = FALSE;

    // CSS: collapses space according to space rule
    if (wsr == WSR_NORMAL || wsr == WSR_NOWRAP)
        col_sp = TRUE;

    gbctxt.mbc_devfont = logfont->mbc_devfont;
    gbctxt.sbc_devfont = logfont->sbc_devfont;
    gbctxt.gs = NULL;
    gbctxt.bts = NULL;
    gbctxt.bos = NULL;
    gbctxt.len_buff = 0;
    gbctxt.n = 0;
    gbctxt.base_bt = UCHAR_BREAK_UNSET;
    gbctxt.cl = content_language;
    gbctxt.ws = writing_system;
    gbctxt.wsr = wsr;
    gbctxt.ctr = ctr;
    gbctxt.wbr = wbr;
    gbctxt.lbp = lbp;

    *glyphs = NULL;
    *break_oppos = NULL;
    *nr_glyphs = 0;

    if (mstr_len == 0)
        return 0;

    if (gbctxt_init(&gbctxt, mstr_len >> 1) <= 0) {
        goto error;
    }

    while (TRUE) {
        Glyph32 gv;
        Uchar32 uc;
        int mclen = 0;
        UCharBreakType bt;
        UCharGeneralCategory gc;
        Uint8 bo;

        Glyph32 next_gv;
        Uchar32 next_uc;
        UCharBreakType next_bt;
        int next_mclen;
        int cosumed_one_loop = 0;

        mclen = get_next_glyph(&gbctxt, mstr, mstr_len, &gv, &uc);
        if (mclen == 0) {
            // badly encoded or end of text
            break;
        }
        mstr += mclen;
        mstr_len -= mclen;
        cosumed += mclen;

        gbctxt.base_bt = UCHAR_BREAK_UNSET;

        _DBG_PRINTF ("Got a glyph: %04X\n", uc);

        if ((wsr == WSR_NORMAL || wsr == WSR_NOWRAP
                || wsr == WSR_PRE_LINE) && uc == UCHAR_TAB) {
            // Every tab is converted to a space (U+0020).
            _DBG_PRINTF ("CSS: Every tab is converted to a space (U+0020)\n");
            uc = UCHAR_SPACE;
            if (IS_MBC_GLYPH(gv))
                gv = SET_MBC_GLYPH(UCHAR_SPACE);
            else
                gv = UCHAR_SPACE;
        }

        /*
         * UNICODE LINE BREAKING ALGORITHM
         */

        // LB1 Resolve line breaking class
        gbctxt.curr_od = LB1;
        bt = resolve_lbc(&gbctxt, uc, &gc);

        /* Start and end of text */
        // LB2 Never break at the start of text.
        if (gbctxt.n == 0) {
            _DBG_PRINTF ("LB2 Never break at the start of text\n");
            gbctxt.curr_od = LB2;
            if (gbctxt_push_back(&gbctxt, 0, 0, BOV_NOTALLOWED_DEFINITELY) == 0)
                goto error;
        }

        // Only break at forced line breaks.
        if (wsr == WSR_PRE || wsr == WSR_NOWRAP)
            // Set the default breaking manner is not allowed.
            bo = BOV_NOTALLOWED_DEFINITELY;
        else
            // Set the default breaking manner is not set.
            bo = BOV_UNKNOWN;

        // Set default break opportunity of the current glyph
        if (gbctxt_push_back(&gbctxt, gv, bt, bo) == 0)
            goto error;

        // LB3 Always break at the end of text.
        if (get_next_glyph(&gbctxt, mstr, mstr_len, &next_gv, &next_uc) == 0) {
            _DBG_PRINTF ("LB3 Always break at the end of text\n");
            gbctxt.curr_od = LB3;
            gbctxt_change_bo_last(&gbctxt, BOV_MANDATORY);
        }

        /* Mandatory breaks */
        // LB4 Always break after hard line breaks
        // LB6 Do not break before hard line breaks
        if (bt == UCHAR_BREAK_MANDATORY) {
            _DBG_PRINTF ("LB4 Always break after hard line breaks\n");
            gbctxt.curr_od = LB4;
            gbctxt_change_bo_last(&gbctxt, BOV_MANDATORY);
            _DBG_PRINTF ("LB6 Do not break before hard line breaks\n");
            gbctxt.curr_od = LB6;
            gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
        }
        // LB5 Treat CR followed by LF, as well as CR, LF,
        // and NL as hard line breaks.
        // LB6 Do not break before hard line breaks.
        else if (bt == UCHAR_BREAK_CARRIAGE_RETURN
                && (next_mclen = is_next_glyph_lf(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc)) > 0) {
            cosumed_one_loop += next_mclen;

            _DBG_PRINTF ("LB5 Treat CR followed by LF, as well as CR, LF, and NL as hard line breaks.\n");
            gbctxt.curr_od = LB5;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);

            _DBG_PRINTF ("LB6 Do not break before hard line breaks\n");
            gbctxt.curr_od = LB6;
            gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);

            if (wsr == WSR_NORMAL || wsr == WSR_NOWRAP) {
                // Collapse new lines
            }
            else {
                if (col_sp)
                    bo = BOV_NOTALLOWED_DEFINITELY;
                else
                    bo = BOV_MANDATORY;
                if (gbctxt_push_back(&gbctxt, next_gv,
                        UCHAR_BREAK_LINE_FEED, bo) == 0)
                    goto error;
            }
        }
        // LB5 Treat CR followed by LF, as well as CR, LF,
        // and NL as hard line breaks.
        // LB6 Do not break before hard line breaks.
        else if (bt == UCHAR_BREAK_CARRIAGE_RETURN
                || bt == UCHAR_BREAK_LINE_FEED
                || bt == UCHAR_BREAK_NEXT_LINE) {

            if (col_sp)
                // Collapse new lines
                bo = BOV_NOTALLOWED_DEFINITELY;
            else
                bo = BOV_MANDATORY;
            _DBG_PRINTF ("LB5 Treat CR followed by LF, as well as CR, LF, and NL as hard line breaks.\n");
            gbctxt.curr_od = LB5;
            gbctxt_change_bo_last(&gbctxt, bo);
            _DBG_PRINTF ("LB6 Do not break before hard line breaks\n");
            gbctxt.curr_od = LB6;
            gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
        }

        /* Explicit breaks and non-breaks */
        // LB7 Do not break before spaces or zero width space.
        else if (bt == UCHAR_BREAK_SPACE
                || bt == UCHAR_BREAK_ZERO_WIDTH_SPACE) {

            _DBG_PRINTF ("LB7 Do not break before spaces or zero width space\n");
            gbctxt.curr_od = LB7;
            gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);

            if (col_sp) {
                cosumed_one_loop += collapse_space(&gbctxt, mstr, mstr_len);
            }
        }

        // LB8: Break before any character following a zero-width space,
        // even if one or more spaces intervene.
        if (bt == UCHAR_BREAK_ZERO_WIDTH_SPACE) {
            _DBG_PRINTF ("LB8: Break before any character following a zero-width space...\n");
            if (col_sp) {
                // CSS: collapses space according to space rule
                cosumed_one_loop += collapse_space(&gbctxt, mstr, mstr_len);
            }
            else {
                gbctxt.curr_od = LB8;
                cosumed_one_loop += check_glyphs_following_zw(&gbctxt,
                    mstr, mstr_len);
                gbctxt_change_bo_last(&gbctxt, BOV_ALLOWED);
                goto next_glyph;
            }
        }

        // Only break at forced line breaks.
        if (wsr == WSR_PRE || wsr == WSR_NOWRAP) {
            // ignore the following breaking rules.
            goto next_glyph;
        }
        else if (lbp == LBP_ANYWHERE) {
            // ignore the following breaking rules.
            goto next_glyph;
        }

        // LB8a Do not break between a zero width joiner and an ideograph,
        // emoji base or emoji modifier.
        if (bt == UCHAR_BREAK_ZERO_WIDTH_JOINER
                && is_next_glyph_id_eb_em(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc) > 0) {

            _DBG_PRINTF ("LB8a Do not break between a zero width joiner and ID, EB, EM\n");
            gbctxt.curr_od = LB8a;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
        }

        /* Combining marks */
        // LB9 Do not break a combining character sequence;
        // treat it as if it has the line breaking class of
        // the base character in all of the following rules.
        // Treat ZWJ as if it were CM.
        // CM/ZWJ should have the same break class as
        // its base character.
        if (bt != UCHAR_BREAK_MANDATORY
                && bt != UCHAR_BREAK_CARRIAGE_RETURN
                && bt != UCHAR_BREAK_LINE_FEED
                && bt != UCHAR_BREAK_NEXT_LINE
                && bt != UCHAR_BREAK_SPACE
                && bt != UCHAR_BREAK_ZERO_WIDTH_SPACE
                && is_next_glyph_cm_zwj(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc, NULL) > 0) {

            _DBG_PRINTF ("LB9 Do not break a combining character sequence\n");
            gbctxt.curr_od = LB9;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);

            // LB10 Treat any remaining combining mark or ZWJ as AL.
            if ((bt == UCHAR_BREAK_COMBINING_MARK
                    || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER)) {
                bt = UCHAR_BREAK_ALPHABETIC;
            }

            gbctxt.base_bt = bt;
            cosumed_one_loop += check_subsequent_cm_zwj(&gbctxt, mstr, mstr_len);
            gbctxt_change_bo_last(&gbctxt, BOV_UNKNOWN);

            mstr += cosumed_one_loop;
            mstr_len -= cosumed_one_loop;
            cosumed_one_loop = 0;

            gbctxt.base_bt = UCHAR_BREAK_UNSET;
        }
        // LB10 Treat any remaining combining mark or ZWJ as AL.
        else if ((bt == UCHAR_BREAK_COMBINING_MARK
                    || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER)) {
            _DBG_PRINTF ("LB10 Treat any remaining combining mark or ZWJ as AL\n");
            bt = UCHAR_BREAK_ALPHABETIC;
        }
#if 0
        else if ((bt == UCHAR_BREAK_COMBINING_MARK
                    || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER)
                && gbctxt.base_bt != UCHAR_BREAK_UNSET) {
            _DBG_PRINTF ("LB9 CM/ZWJ should have the same break class as its base character\n");
            bt = gbctxt.base_bt;
        }
#endif

        /* Word joiner */
        // LB11 Do not break before or after Word joiner
        // and related characters.
        if (bt == UCHAR_BREAK_WORD_JOINER) {
            bo = BOV_NOTALLOWED_DEFINITELY;
            _DBG_PRINTF ("LB11 Do not break before or after Word joiner and ...\n");
            gbctxt.curr_od = LB11;
            gbctxt_change_bo_last(&gbctxt, bo);
            gbctxt_change_bo_before_last(&gbctxt, bo);
        }
        // LB12 Do not break after NBSP and related characters.
        else if (bt == UCHAR_BREAK_NON_BREAKING_GLUE) {
            _DBG_PRINTF ("LB12 Do not break after NBSP and related characters\n");
            gbctxt.curr_od = LB12;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
        }

        /* Breaking is forbidden within “words”: implicit soft wrap
         * opportunities between typographic letter units (or other
         * typographic character units belonging to the NU, AL, AI, or ID
         * Unicode line breaking classes) are suppressed, i.e. breaks are
         * prohibited between pairs of such characters (regardless of
         * line-break settings other than anywhere) except where opportunities
         * exist due to dictionary-based breaking.
         */
        if (wbr == WBR_KEEP_ALL && is_glyph_letter(gc, bt)
                && (next_mclen = is_next_glyph_letter(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc, &next_bt)) > 0) {
            _DBG_PRINTF ("WBR_KEEP_ALL.\n");
            gbctxt.curr_od = LB12a;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            if (gbctxt_push_back(&gbctxt, next_gv, next_bt, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
            goto next_glyph;
        }

        /*
         * Tailorable Line Breaking Rules
         */

        /* Non-breaking characters */
        // LB12a Do not break before NBSP and related characters,
        // except after spaces and hyphens.
        else if (bt != UCHAR_BREAK_SPACE
                && bt != UCHAR_BREAK_AFTER
                && bt != UCHAR_BREAK_HYPHEN
                && is_next_glyph_gl(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc) > 0) {
            _DBG_PRINTF ("LB12a Do not break before NBSP and related characters, except after SP and HY\n");
            gbctxt.curr_od = LB12a;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
        }

        /* Opening and closing */
        if (lbp == LBP_LOOSE) {
            // LB13 for LBP_LOOSE: Do not break before ‘!’, even after spaces.
            if (bt == UCHAR_BREAK_EXCLAMATION) {
                _DBG_PRINTF ("LB13 for LBP_LOOSE: Do not break before ‘!’, even after spaces.\n");
                gbctxt.curr_od = LB13;
                gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
        }
        else if (lbp == LBP_NORMAL) {
            // LB13 for LBP_NORMAL
            if (bt != UCHAR_BREAK_NUMERIC
                && is_next_glyph_cl_cp_is_sy(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc) > 0) {

                _DBG_PRINTF ("LB13 for LBP_NORMAL: Do not break between non-number and ‘]’ or ‘;’ or ‘/’.\n");
                gbctxt.curr_od = LB13;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }

            if (bt == UCHAR_BREAK_EXCLAMATION) {
                _DBG_PRINTF ("LB13 for LBP_NORMAL: Do not break before ‘!’, even after spaces\n");
                gbctxt.curr_od = LB13;
                gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
        }
        else {
            // LB13 for LBP_STRICT: Do not break before ‘]’ or ‘!’ or ‘;’ or ‘/’, even after spaces.
            if (bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                    || bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                    || bt == UCHAR_BREAK_EXCLAMATION
                    || bt == UCHAR_BREAK_INFIX_SEPARATOR
                    || bt == UCHAR_BREAK_SYMBOL) {
                _DBG_PRINTF ("LB13  for LBP_STRICT: Do not break before ‘]’ or ‘!’ or ‘;’ or ‘/’, even after spaces\n");
                gbctxt.curr_od = LB13;
                gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
        }

        // LB14 Do not break after ‘[’, even after spaces.
        if (bt == UCHAR_BREAK_OPEN_PUNCTUATION) {
            _DBG_PRINTF ("LB14 Do not break after ‘[’, even after spaces\n");
            gbctxt.curr_od = LB14;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);

            // For any possible subsequent space.
            if (col_sp) {
                // CSS: collapses space according to space rule
                cosumed_one_loop += collapse_space(&gbctxt, mstr, mstr_len);
            }
            else {
                cosumed_one_loop += check_subsequent_sp(&gbctxt, mstr, mstr_len);
            }
        }
        // LB15 Do not break within ‘”[’, even with intervening spaces.
        else if (bt == UCHAR_BREAK_QUOTATION
                && (is_subsequent_sps_and_end_bt(&gbctxt, mstr, mstr_len,
                    UCHAR_BREAK_OPEN_PUNCTUATION))) {

            _DBG_PRINTF ("LB15 Do not break within ‘”[’, even with intervening spaces\n");
            gbctxt.curr_od = LB15;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);

            _DBG_PRINTF ("LB19 Do not break before or after quotation marks, such as ‘ ” ’\n");
            gbctxt.curr_od = LB19;
            gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);

            gbctxt.curr_od = LB15;
            // For subsequent spaces and OP.
            cosumed_one_loop += check_subsequent_sps_and_end_bt(&gbctxt,
                    mstr, mstr_len, col_sp,
                    UCHAR_BREAK_OPEN_PUNCTUATION);
        }
        // LB16 Do not break between closing punctuation and a nonstarter
        // (lb=NS), even with intervening spaces.
        if ((bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                    || bt == UCHAR_BREAK_CLOSE_PARANTHESIS)
                && (is_subsequent_sps_and_end_bt(&gbctxt, mstr, mstr_len,
                    UCHAR_BREAK_NON_STARTER))) {

            _DBG_PRINTF ("LB16 Do not break between closing punctuation and NS, even...\n");
            gbctxt.curr_od = LB16;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);

            // For subsequent spaces and NS.
            cosumed_one_loop += check_subsequent_sps_and_end_bt(&gbctxt,
                    mstr, mstr_len, col_sp,
                    UCHAR_BREAK_NON_STARTER);
        }
        // LB17 Do not break within ‘——’, even with intervening spaces.
        else if (bt == UCHAR_BREAK_BEFORE_AND_AFTER
                && (is_subsequent_sps_and_end_bt(&gbctxt, mstr, mstr_len,
                    UCHAR_BREAK_BEFORE_AND_AFTER))) {

            _DBG_PRINTF ("LB17 Do not break within ‘——’, even with intervening spaces\n");
            gbctxt.curr_od = LB17;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);

            // For subsequent spaces and B2.
            cosumed_one_loop += check_subsequent_sps_and_end_bt(&gbctxt,
                    mstr, mstr_len, col_sp,
                    UCHAR_BREAK_BEFORE_AND_AFTER);
        }
        /* Spaces */
        // LB18 Break after spaces.
        else if (bt == UCHAR_BREAK_SPACE) {
            _DBG_PRINTF ("LB18 Break after spaces\n");
            gbctxt.curr_od = LB18;
            gbctxt_change_bo_last(&gbctxt, BOV_ALLOWED);
        }
        /* Special case rules */
        // LB19 Do not break before or after quotation marks, such as ‘ ” ’.
        else if (bt == UCHAR_BREAK_QUOTATION) {
            _DBG_PRINTF ("LB19 Do not break before or after quotation marks, such as ‘ ” ’\n");
            gbctxt.curr_od = LB19;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
        }
        // LB20 Break before and after unresolved CB.
        else if (bt == UCHAR_BREAK_CONTINGENT) {
            _DBG_PRINTF ("LB20 Break before and after unresolved CB.\n");
            gbctxt.curr_od = LB20;
            gbctxt_change_bo_last(&gbctxt, BOV_ALLOWED);
            gbctxt_change_bo_before_last(&gbctxt, BOV_ALLOWED);
        }
        // LB21 Do not break before hyphen-minus, other hyphens,
        // fixed-width spaces, small kana, and other non-starters,
        // or after acute accents.
        else if (bt == UCHAR_BREAK_AFTER
                || bt == UCHAR_BREAK_HYPHEN
                || bt == UCHAR_BREAK_NON_STARTER) {
            _DBG_PRINTF ("LB21.1 Do not break before hyphen-minus, other hyphens...\n");
            gbctxt.curr_od = LB21;
            gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
        }
        else if (bt == UCHAR_BREAK_BEFORE) {
            _DBG_PRINTF ("LB21.2 Do not break before hyphen-minus, other hyphens...\n");
            gbctxt.curr_od = LB21;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
        }
        // LB21a Don't break after Hebrew + Hyphen.
        else if (bt == UCHAR_BREAK_HEBREW_LETTER
                && (next_mclen = is_next_glyph_hy_ba(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc, &next_bt)) > 0) {

            _DBG_PRINTF ("LB21a Don't break after Hebrew + Hyphen\n");
            gbctxt.curr_od = LB21a;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            if (gbctxt_push_back(&gbctxt,
                    next_gv, next_bt, BOV_NOTALLOWED_UNCERTAINLY) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        // LB21b Don’t break between Solidus and Hebrew letters.
        else if (bt == UCHAR_BREAK_SYMBOL
                && is_next_glyph_hl(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc) > 0) {
            _DBG_PRINTF ("LB21b Don’t break between Solidus and Hebrew letters\n");
            gbctxt.curr_od = LB21b;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
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
                && is_next_glyph_in(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc) > 0) {

            _DBG_PRINTF ("LB22 Do not break between two ellipses, or between letters...\n");
            gbctxt.curr_od = LB22;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
        }

        /* Numbers */
        // LB23 Do not break between digits and letters.
        if (lbp != LBP_LOOSE) {
            if ((bt == UCHAR_BREAK_HEBREW_LETTER
                        || bt == UCHAR_BREAK_ALPHABETIC)
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB23 Do not break between digits and letters\n");
                gbctxt.curr_od = LB23;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_al_hl(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB23 Do not break between digits and letters\n");
                gbctxt.curr_od = LB23;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            // LB23a Do not break between numeric prefixes and ideographs,
            // or between ideographs and numeric postfixes.
            else if (bt == UCHAR_BREAK_PREFIX
                    && is_next_glyph_id_eb_em(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB23a.1 Do not break between numeric prefixes and ID...\n");
                gbctxt.curr_od = LB23a;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            else if ((bt == UCHAR_BREAK_IDEOGRAPHIC
                        || bt == UCHAR_BREAK_EMOJI_BASE
                        || bt == UCHAR_BREAK_EMOJI_MODIFIER)
                    && is_next_glyph_po(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB23a.2 Do not break between numeric prefixes and ID...\n");
                gbctxt.curr_od = LB23a;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            // LB24 Do not break between numeric prefix/postfix and letters,
            // or between letters and prefix/postfix.
            else if ((bt == UCHAR_BREAK_PREFIX
                        || bt == UCHAR_BREAK_POSTFIX)
                    && is_next_glyph_al_hl(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB24 Do not break between numeric prefix/postfix and letters\n");
                gbctxt.curr_od = LB24;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            else if ((bt == UCHAR_BREAK_ALPHABETIC
                        || bt == UCHAR_BREAK_HEBREW_LETTER)
                    && is_next_glyph_pr_po(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB24 Do not break between numeric prefix/postfix and letters\n");
                gbctxt.curr_od = LB24;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
        }

        // LB25 Do not break between the following pairs of classes
        // relevant to numbers
        if (lbp == LBP_LOOSE) {
            if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_po(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.5 for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_pr(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.6 for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            else if (bt == UCHAR_BREAK_POSTFIX
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                gbctxt.curr_od = LB25;
                _DBG_PRINTF ("LB25.8 for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            else if (bt == UCHAR_BREAK_PREFIX
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.a for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            else if (bt == UCHAR_BREAK_HYPHEN
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.b for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.d for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
        }
        else if (lbp == LBP_NORMAL) {
            if ((bt == UCHAR_BREAK_PREFIX || bt == UCHAR_BREAK_POSTFIX)
                    && is_next_glyph_nu_OR_op_hy_followed_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.1 for LBP_NORMAL: (PR | PO) × ( OP | HY )? NU.\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            else if ((bt == UCHAR_BREAK_OPEN_PUNCTUATION
                        || bt == UCHAR_BREAK_HYPHEN)
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.2 for LBP_NORMAL: ( OP | HY ) × NU.\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_nu_sy_is(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.3 for LBP_NORMAL: NU × (NU | SY | IS).\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }

            if ((bt == UCHAR_BREAK_NUMERIC
                    || bt == UCHAR_BREAK_SYMBOL
                    || bt == UCHAR_BREAK_INFIX_SEPARATOR
                    || bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                    || bt == UCHAR_BREAK_CLOSE_PARANTHESIS)
                    && are_prev_glyphs_nu_AND_nu_sy_is(&gbctxt, FALSE)) {
                _DBG_PRINTF ("LB25.4 for LBP_NORMAL: NU (NU | SY | IS)* × (NU | SY | IS | CL | CP ).\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
            if ((bt == UCHAR_BREAK_POSTFIX || bt == UCHAR_BREAK_PREFIX)
                    && are_prev_glyphs_nu_AND_nu_sy_is_AND_cl_cp(&gbctxt)) {
                _DBG_PRINTF ("LB25.5 for LBP_NORMAL: NU (NU | SY | IS)* (CL | CP)? × (PO | PR).\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_before_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            }
        }
        else {
            if (bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                    && is_next_glyph_po(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.1 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                    && is_next_glyph_pr(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.2 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                    && is_next_glyph_po(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                gbctxt.curr_od = LB25;
                _DBG_PRINTF ("LB25.3 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                    && is_next_glyph_pr(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.4 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_po(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.5 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_pr(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.6 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_POSTFIX
                    && is_next_glyph_op(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                gbctxt.curr_od = LB25;
                _DBG_PRINTF ("LB25.7 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);

#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_POSTFIX
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                gbctxt.curr_od = LB25;
                _DBG_PRINTF ("LB25.8 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_PREFIX
                    && is_next_glyph_op(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                gbctxt.curr_od = LB25;
                _DBG_PRINTF ("LB25.9 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_PREFIX
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.a for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_HYPHEN
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.b for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_INFIX_SEPARATOR
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.c for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.d for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
            else if (bt == UCHAR_BREAK_SYMBOL
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_gv, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.e for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
                if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                    goto error;
                cosumed_one_loop += next_mclen;
#endif
            }
        }

        /* Korean syllable blocks */
        // LB26 Do not break a Korean syllable.
        if (bt == UCHAR_BREAK_HANGUL_L_JAMO
                && (next_mclen = is_next_glyph_jl_jv_h2_h3(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc, &next_bt)) > 0) {
            _DBG_PRINTF ("LB26.1 Do not break a Korean syllable.\n");
            gbctxt.curr_od = LB26;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
            if (gbctxt_push_back(&gbctxt, next_gv, next_bt, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        else if ((bt == UCHAR_BREAK_HANGUL_V_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE)
                && (next_mclen = is_next_glyph_jv_jt(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc, &next_bt)) > 0) {
            _DBG_PRINTF ("LB26.2 Do not break a Korean syllable.\n");
            gbctxt.curr_od = LB26;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
            if (gbctxt_push_back(&gbctxt, next_gv, next_bt, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        else if ((bt == UCHAR_BREAK_HANGUL_T_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE)
                && (next_mclen = is_next_glyph_jt(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc)) > 0) {
            _DBG_PRINTF ("LB26.3 Do not break a Korean syllable.\n");
            gbctxt.curr_od = LB26;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
            if (gbctxt_push_back(&gbctxt, next_gv,
                    UCHAR_BREAK_HANGUL_T_JAMO, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        // LB27 Treat a Korean Syllable Block the same as ID.
        else if ((bt == UCHAR_BREAK_HANGUL_L_JAMO
                    || bt == UCHAR_BREAK_HANGUL_V_JAMO
                    || bt == UCHAR_BREAK_HANGUL_T_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE
                    || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE)
                && (next_mclen = is_next_glyph_in(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc)) > 0) {
            _DBG_PRINTF ("LB27.1 Treat a Korean Syllable Block the same as ID.\n");
            gbctxt.curr_od = LB27;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
            if (gbctxt_push_back(&gbctxt,
                    next_gv, UCHAR_BREAK_INSEPARABLE, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        else if ((bt == UCHAR_BREAK_HANGUL_L_JAMO
                    || bt == UCHAR_BREAK_HANGUL_V_JAMO
                    || bt == UCHAR_BREAK_HANGUL_T_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE
                    || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE)
                && (next_mclen = is_next_glyph_po(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc)) > 0) {
            _DBG_PRINTF ("LB27.2 Treat a Korean Syllable Block the same as ID.\n");
            gbctxt.curr_od = LB27;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
            if (gbctxt_push_back(&gbctxt,
                    next_gv, UCHAR_BREAK_POSTFIX, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        else if (bt == UCHAR_BREAK_PREFIX
                && (next_mclen = is_next_glyph_jl_jv_jt_h2_h3(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc, &next_bt)) > 0) {
            _DBG_PRINTF ("LB27.3 Treat a Korean Syllable Block the same as ID.\n");
            gbctxt.curr_od = LB27;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
            if (gbctxt_push_back(&gbctxt, next_gv, next_bt, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }

        /* Finally, join alphabetic letters into words
           and break everything else. */

        // LB28 Do not break between alphabetics (“at”).
        else if ((bt == UCHAR_BREAK_HEBREW_LETTER
                    || bt == UCHAR_BREAK_ALPHABETIC)
                && is_next_glyph_al_hl(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc) > 0) {
            _DBG_PRINTF ("LB28 Do not break between alphabetics (“at”)\n");
            gbctxt.curr_od = LB28;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
        }

        // LB29 Do not break between numeric punctuation
        // and alphabetics (“e.g.”).
        else if (bt == UCHAR_BREAK_INFIX_SEPARATOR
                && is_next_glyph_al_hl(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc) > 0) {
            _DBG_PRINTF ("LB29 Do not break between numeric punctuation\n");
            gbctxt.curr_od = LB29;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
        }

        // LB30 Do not break between letters, numbers, or ordinary symbols and
        // opening or closing parentheses.
        else if ((bt == UCHAR_BREAK_HEBREW_LETTER
                    || bt == UCHAR_BREAK_ALPHABETIC
                    || bt == UCHAR_BREAK_NUMERIC)
                && is_next_glyph_op(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc) > 0) {
            _DBG_PRINTF ("LB30.1 Do not break between letters, numbers...\n");
            gbctxt.curr_od = LB30;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
        }
        else if (bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                && is_next_glyph_al_hl_nu(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc) > 0) {
            _DBG_PRINTF ("LB30.2 Do not break between letters, numbers...\n");
            gbctxt.curr_od = LB30;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
#if 0
            if (gbctxt_push_back(&gbctxt, next_gv, BOV_UNKNOWN) == 0)
                goto error;
            cosumed_one_loop += next_mclen;
#endif
        }

        // LB30a Break between two regional indicator symbols if and only if
        // there are an even number of regional indicators preceding the
        // position of the break.
        else if (bt == UCHAR_BREAK_REGIONAL_INDICATOR
                && (next_mclen = is_next_glyph_ri(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc)) > 0) {
            _DBG_PRINTF ("LB30a.1 Break between two regional indicator symbols...\n");
            gbctxt.curr_od = LB30a;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            if (gbctxt_push_back(&gbctxt,
                    next_gv, UCHAR_BREAK_REGIONAL_INDICATOR, BOV_UNKNOWN) == 0)
                goto error;
            cosumed_one_loop += next_mclen;
#if 0
            next_mclen = check_subsequent_ri(&gbctxt,
                mstr, mstr_len);
            gbctxt_change_bo_last(&gbctxt, BOV_UNKNOWN);
            cosumed_one_loop += next_mclen;
#endif
        }
        else if (bt != UCHAR_BREAK_REGIONAL_INDICATOR
                && (next_mclen = is_even_nubmer_of_subsequent_ri(&gbctxt,
                    mstr, mstr_len)) > 0) {

            _DBG_PRINTF ("LB30a.2 Break between two regional indicator symbols...\n");
            gbctxt.curr_od = LB30a;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_UNCERTAINLY);
            next_mclen = check_subsequent_ri(&gbctxt,
                mstr, mstr_len);
            gbctxt_change_bo_last(&gbctxt, BOV_UNKNOWN);

            cosumed_one_loop += next_mclen;
        }

        // LB30b Do not break between an emoji base and an emoji modifier.
        else if (bt == UCHAR_BREAK_EMOJI_BASE
                && (next_mclen = is_next_glyph_em(&gbctxt,
                    mstr, mstr_len, &next_gv, &next_uc)) > 0) {
            _DBG_PRINTF ("LB30b Do not break between an emoji base and an emoji modifier\n");
            gbctxt.curr_od = LB30b;
            gbctxt_change_bo_last(&gbctxt, BOV_NOTALLOWED_DEFINITELY);
            if (gbctxt_push_back(&gbctxt,
                    next_gv, UCHAR_BREAK_EMOJI_MODIFIER, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }

        gbctxt.base_bt = UCHAR_BREAK_UNSET;

next_glyph:
        mstr_len -= cosumed_one_loop;
        mstr += cosumed_one_loop;
        cosumed += cosumed_one_loop;

        // Return if we got any BK!
        if (gbctxt.bos[gbctxt.n] == BOV_MANDATORY) {
            break;
        }
    }

    if (gbctxt.n > 0) {
        // LB31 Break everywhere else.
        int n;
        for (n = 1; n < gbctxt.n; n++) {
            if (gbctxt.bos[n] == BOV_UNKNOWN) {
                _DBG_PRINTF ("LB31 Break everywhere else: %d\n", n);
                gbctxt.bos[n] = BOV_ALLOWED;
            }
        }

        *glyphs = gbctxt.gs;
        *break_oppos = gbctxt.bos;
        *nr_glyphs = gbctxt.n - 1;
    }
    else
        goto error;

    if (gbctxt.bts) free(gbctxt.bts);
    if (gbctxt.od) free(gbctxt.od);

    return cosumed;

error:
    if (gbctxt.gs) free(gbctxt.gs);
    if (gbctxt.bts) free(gbctxt.bts);
    if (gbctxt.bos) free(gbctxt.bos);
    if (gbctxt.od) free(gbctxt.od);
    return 0;
}

static int font_get_glyph_metrics (LOGFONT* logfont, DEVFONT* devfont,
        Glyph32 rg, int* adv_x, int* adv_y, BBOX* bbox)
{
    int bold = 0;
    int tmp_x = 0;
    int tmp_y = 0;
    int bbox_x = 0, bbox_y = 0;
    int bbox_w = 0, bbox_h = 0;
    int gbt;

    gbt = devfont->font_ops->get_glyph_bmptype (logfont, devfont)
            & DEVFONTGLYPHTYPE_MASK_BMPTYPE;

    devfont->font_ops->get_glyph_bbox (logfont, devfont,
            rg, &bbox_x, &bbox_y, &bbox_w, &bbox_h);

    if ((logfont->style & FS_WEIGHT_MASK) > FS_WEIGHT_MEDIUM
            && (devfont->style & FS_WEIGHT_MASK) < FS_WEIGHT_DEMIBOLD
            && (gbt == DEVFONTGLYPHTYPE_MONOBMP)) {
        bold = GET_DEVFONT_SCALE (logfont, devfont);
        bbox_w += bold;
    }

    if (bbox) {
        bbox->x = bbox_x;
        bbox->y = bbox_y;
        bbox->w = bbox_w;
        bbox->h = bbox_h;
    }

    devfont->font_ops->get_glyph_advance (logfont, devfont, rg, &tmp_x, &tmp_y);

    tmp_x += bold;
    if (gbt == DEVFONTGLYPHTYPE_MONOBMP) {
        if ((logfont->style & FS_RENDER_MASK) == FS_RENDER_GREY ||
                (logfont->style & FS_DECORATE_OUTLINE)) {
            tmp_x += 1;
        }
    }

    if (adv_x) *adv_x = tmp_x;
    if (adv_y) *adv_y = tmp_y;

    return 0;
}

#define IS_UPRIGHT(rf)  \
    ((render_flags & GRF_TEXT_ORIENTATION_MASK) == GRF_TEXT_ORIENTATION_UPRIGHT)
#define IS_SIDEWAYS(rf)  \
    ((render_flags & GRF_TEXT_ORIENTATION_MASK) == GRF_TEXT_ORIENTATION_SIDEWAYS)

static int normalize_advance(Uint32 render_flags, int* adv_x, int* adv_y)
{
    int line_adv;
    int dir = 1;

    switch (render_flags & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
        if (IS_SIDEWAYS(render_flags)) {
        }
        else {
            int tmp;
            tmp = *adv_x;
            *adv_x = *adv_y;
            *adv_y = tmp;
        }
        line_adv = *adv_y;
        *adv_y *= dir;
        break;

    case GRF_WRITING_MODE_VERTICAL_LR:
        if (IS_SIDEWAYS(render_flags)) {
        }
        else {
            int tmp;
            tmp = *adv_x;
            *adv_x = *adv_y;
            *adv_y = tmp;
        }
        line_adv = *adv_y;
        *adv_y *= dir;
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        if (IS_SIDEWAYS(render_flags)) {
            int tmp;
            tmp = *adv_x;
            *adv_x = *adv_y;
            *adv_y = tmp;
        }
        else {
        }
        line_adv = *adv_x;
        *adv_x *= dir;
        break;
    }

    return line_adv;
}

static void advance_extra_spacing(Uint32 render_flags, int extra_spacing,
        int* adv_x, int *adv_y)
{
    int dir = 1;

    switch (render_flags & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
        if (IS_SIDEWAYS(render_flags)) {
        }
        else {
            *adv_y += dir * extra_spacing;
        }
        break;

    case GRF_WRITING_MODE_VERTICAL_LR:
        if (IS_SIDEWAYS(render_flags)) {
        }
        else {
            *adv_y += dir * extra_spacing;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        if (IS_SIDEWAYS(render_flags)) {
            *adv_x += dir * extra_spacing;
        }
        else {
        }
        break;
    }
}

static int find_breaking_pos_any(const Glyph32* glyphs,
        const Uint8* break_oppos, int n, Uint32 rf)
{
    return 0;
}

static int find_breaking_pos_word(const Glyph32* glyphs,
        const Uint8* break_oppos, int n, Uint32 rf)
{
    return 0;
}

static int find_breaking_pos_normal(const Glyph32* glyphs,
        const Uint8* break_oppos, int n, Uint32 rf)
{
    return 0;
}

int GUIAPI GetGlyphsExtentPointEx(LOGFONT* logfont, int x, int y,
            const Glyph32* glyphs, const Uint8* break_oppos, int nr_glyphs,
            Uint32 render_flags,
            int letter_spacing, int word_spacing, int tab_size, int max_extent,
            SIZE* line_size, GLYPHEXTINFO* glyph_ext_info, GLYPHPOS* glyph_pos)
{
    int n = 0;
    int total_extent = 0;
    int nr_letter_spaces = 0;
    int nr_word_spaces = 0;
    int breaking_pos = -1;

    if (line_size) {
        line_size->cx = 0;
        line_size->cy = 0;
    }

    /* check the rotation of the logfont is compatible with render flags */
    switch (render_flags & GRF_TEXT_ORIENTATION_MASK) {
    case GRF_TEXT_ORIENTATION_UPRIGHT:
        if (logfont->rotation != 0)
            return 0;
        break;
    case GRF_TEXT_ORIENTATION_SIDEWAYS:
        if (logfont->rotation != 900)
            return 0;
        break;
    default:
        return 0;
    }

    while (n < nr_glyphs) {
        DEVFONT* devfont;
        Glyph32 gv;
        Uchar32 uc;
        UCharGeneralCategory gc;
        UCharBreakType bt;

        BBOX bbox;
        int adv_x, adv_y;
        int line_adv;

        gv = glyphs[n];
        uc = GLYPH2UCHAR(gv);
        bt = resolve_lbc(NULL, uc, &gc);

        if (uc == UCHAR_TAB) {
            bbox.x = 0; bbox.y = 0;
            bbox.w = 0; bbox.h = 0;
            adv_x = 0; adv_y = 0;
            line_adv = tab_size;
        }
        else if (bt == UCHAR_BREAK_ZERO_WIDTH_SPACE
                || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER) {
            bbox.x = 0; bbox.y = 0;
            bbox.w = 0; bbox.h = 0;
            adv_x = 0; adv_y = 0;
            line_adv = 0;
        }
        else {
            devfont = SELECT_DEVFONT(logfont, gv);
            font_get_glyph_metrics (logfont, devfont, REAL_GLYPH(gv),
                    &adv_x, &adv_y, &bbox);

            line_adv = normalize_advance(render_flags, &adv_x, &adv_y);
        }

        if (glyph_ext_info) {
            glyph_ext_info[n].bbox_x = bbox.x;
            glyph_ext_info[n].bbox_y = bbox.y;
            glyph_ext_info[n].bbox_w = bbox.w;
            glyph_ext_info[n].bbox_h = bbox.h;
            glyph_ext_info[n].advance_x = adv_x;
            glyph_ext_info[n].advance_y = adv_y;
        }

        if (n > 0 && break_oppos[n+1] == BOV_MANDATORY) {
            n++;
            // hard line breaking
            break;
        }

        if (max_extent > 0 && (total_extent + line_adv) > max_extent) {
            // overflow
            switch (render_flags & GRF_OVERFLOW_WRAP_MASK) {
            case GRF_OVERFLOW_WRAP_BREAK_WORD:
                breaking_pos = find_breaking_pos_word(glyphs, break_oppos,
                    n, render_flags);
                break;
            case GRF_OVERFLOW_WRAP_ANYWHERE:
                breaking_pos = find_breaking_pos_any(glyphs, break_oppos,
                    n, render_flags);
                break;
            case GRF_OVERFLOW_WRAP_NORMAL:
            default:
                breaking_pos = find_breaking_pos_normal(glyphs, break_oppos,
                    n, render_flags);
                break;
            }

            break;
        }

        if ((render_flags & GRF_ALIGN_MASK) != GRF_ALIGN_JUSTIFY) {
            int extra_spacing = 0;
            // extra space for word and letter
            if (uc == UCHAR_SPACE) {
                extra_spacing = word_spacing;
            }
            else if (gc != UCHAR_TYPE_FORMAT) {
                extra_spacing = letter_spacing;
            }

            line_adv += extra_spacing;
            advance_extra_spacing(render_flags, extra_spacing, &adv_x, &adv_y);
        }
        else {
            if (uc == UCHAR_SPACE) {
                nr_word_spaces++;
            }
            else if (gc != UCHAR_TYPE_FORMAT) {
                nr_letter_spaces++;
            }
        }

        total_extent += line_adv;

        if (glyph_pos) {
            if (n == 0) {
                glyph_pos[n].x = x;
                glyph_pos[n].y = y;
            }
            else {
                glyph_pos[n].x = glyph_pos[n - 1].x + adv_x;
                glyph_pos[n].y = glyph_pos[n - 1].y + adv_y;
            }

            if (adv_x == 0 && adv_y == 0) {
                glyph_pos[n].suppressed = 1;
            }
        }

        if (line_size) {
            line_size->cx += adv_x;
            line_size->cy += adv_y;
        }

        n++;
    }

    if (breaking_pos != -1 && breaking_pos != n) {
        // wrapped due to overflow
    }

    if ((render_flags & GRF_ALIGN_MASK) == GRF_ALIGN_JUSTIFY) {
    }

    return n;
}

#endif /*  _MGCHARSET_UNICODE */

int GUIAPI DrawGlyphStringEx(HDC hdc, const Glyph32* glyphs, int nr_glyphs,
        const GLYPHPOS* glyph_pos)
{
    int i;
    int n = 0;

    if (glyph_pos) {
        for (i = 0; i < nr_glyphs; i++) {
            if (glyph_pos[i].suppressed == 0) {
                DrawGlyph(hdc, glyph_pos[i].x, glyph_pos[i].y, glyphs[i],
                    NULL, NULL);
                n++;
            }
        }
    }

    return n;
}

