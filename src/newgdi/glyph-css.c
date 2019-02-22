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
#define UCHAR_SHY           0x00AD
#define UCHAR_IDSPACE       0x3000
#define UCHAR_TAB           0x0009

enum _LBOrder {
    LB1, LB2, LB3, LB4, LB5, LB6, LB7, LB8, LBPRE, LB8a, LB9, LB10,
    LB11, LB12, LB12a, LB13, LB14, LB15, LB16, LB17, LB18, LB19,
    LB20, LB21, LB21a, LB21b, LB22, LB23, LB23a, LB24, LB25, LB26,
    LB27, LB28, LB29, LB30, LB30a, LB30b, LB31,
    LBLAST = 0xFF,
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
        gbctxt->od[0] = LBLAST;
    }
    else {
        // set the after break opportunity
        gbctxt->gs[gbctxt->n - 1] = gv;
        gbctxt->bts[gbctxt->n - 1] = (Uint8)bt;
        gbctxt->bos[gbctxt->n] = bo;
        if (bo == BOV_UNKNOWN)
            gbctxt->od[gbctxt->n] = LBLAST;
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
    UCharGeneralCategory gc = UCharGetCategory(uc);
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

        bt = resolve_lbc(gbctxt, uc, NULL);
        if (bt != UCHAR_BREAK_SPACE && uc != UCHAR_TAB)
            break;

        mstr += mclen;
        mstr_len -= mclen;
        cosumed += mclen;
    } while (1);

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
            Glyph32** glyphs, Uint8** break_oppos, Uint8** break_classes,
            int* nr_glyphs)
{
    struct glyph_break_ctxt gbctxt;
    int cosumed = 0;
    BOOL col_sp = FALSE;
    BOOL col_nl = FALSE;

    // CSS: collapses space according to space rule
    if (wsr == WSR_NORMAL || wsr == WSR_NOWRAP || wsr == WSR_PRE_LINE)
        col_sp = TRUE;
    // CSS: collapses new lines acoording to space rule
    if (wsr == WSR_NORMAL || wsr == WSR_NOWRAP)
        col_nl = TRUE;

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
    if (break_classes)
        *break_classes = NULL;
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
        if (wsr == WSR_PRE || wsr == WSR_NOWRAP) {
            // Set the default breaking manner is not allowed.
            gbctxt.curr_od = LBPRE;
            bo = BOV_NOTALLOWED_DEFINITELY;
        }
        else {
            // Set the default breaking manner is not set.
            gbctxt.curr_od = LBLAST;
            bo = BOV_UNKNOWN;
        }

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

            if (col_nl)
                // Collapse new lines
                bo = BOV_NOTALLOWED_DEFINITELY;
            else
                bo = BOV_MANDATORY;
            if (gbctxt_push_back(&gbctxt, next_gv,
                    UCHAR_BREAK_LINE_FEED, bo) == 0)
                goto error;
        }
        // LB5 Treat CR followed by LF, as well as CR, LF,
        // and NL as hard line breaks.
        // LB6 Do not break before hard line breaks.
        else if (bt == UCHAR_BREAK_CARRIAGE_RETURN
                || bt == UCHAR_BREAK_LINE_FEED
                || bt == UCHAR_BREAK_NEXT_LINE) {

            if (col_nl)
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
        if (break_classes)
            *break_classes = gbctxt.bts;
        *nr_glyphs = gbctxt.n - 1;
    }
    else
        goto error;

    if (break_classes == NULL && gbctxt.bts) free(gbctxt.bts);
    if (gbctxt.od) free(gbctxt.od);

    return cosumed;

error:
    if (gbctxt.gs) free(gbctxt.gs);
    if (gbctxt.bts) free(gbctxt.bts);
    if (gbctxt.bos) free(gbctxt.bos);
    if (gbctxt.od) free(gbctxt.od);
    return 0;
}

// internal use
typedef struct _MYGLYPHINFO {
    Uchar32 uc;
    Uint8 bt;
    Uint8 gc;
    Uint8 ignored:1;
    Uint8 hanged:2;
    Uint8 ort:2;
    Uint8 justify_word:1;
    Uint8 justify_char:1;
} MYGLYPHINFO;

typedef struct _MYGLYPHARGS {
    PLOGFONT lfur;
    PLOGFONT lfsw;
    const Glyph32* gvs;
    const Uint8* bcs;
    const Uint8* bos;
    Uint32 rf;
    int nr_gvs;
} MYGLYPHARGS;

static LOGFONT* create_sideways_logfont(LOGFONT* logfont_upright)
{
    LOGFONT tmp;

    memcpy(&tmp, logfont_upright, sizeof(LOGFONT));
    tmp.rotation = 900;
    return CreateLogFontIndirect(&tmp);
}

#define SCRIPT_ORIENTATION_HORIZONTAL   0x01
#define SCRIPT_ORIENTATION_VERTICAL     0x02
#define SCRIPT_ORIENTATION_BI           (0x01 | 0x02)

typedef struct _VERT_SCRIPT_ORT {
    UCharScriptType st;
    int             rot;
    const char*     vid;
} VERT_SCRIPT_ORT;

static VERT_SCRIPT_ORT vertical_scripts [] = {
    {UCHAR_SCRIPT_BOPOMOFO, 0, "ttb"},
    {UCHAR_SCRIPT_EGYPTIAN_HIEROGLYPHS, 0, "ttb"},
    {UCHAR_SCRIPT_HIRAGANA, 0, "ttb"},
    {UCHAR_SCRIPT_KATAKANA, 0, "ttb"},
    {UCHAR_SCRIPT_HAN, 0, "ttb"},
    {UCHAR_SCRIPT_HANGUL, 0, "ttb"},
    {UCHAR_SCRIPT_MEROITIC_CURSIVE, 0, "ttb"},
    {UCHAR_SCRIPT_MEROITIC_HIEROGLYPHS, 0, "ttb"},
    {UCHAR_SCRIPT_MONGOLIAN, 90, "ttb"},
    {UCHAR_SCRIPT_OGHAM, -90, "btt"},
    {UCHAR_SCRIPT_OLD_TURKIC, -90, "ttb"},
    {UCHAR_SCRIPT_PHAGS_PA, 90, "ttb"},
    {UCHAR_SCRIPT_YI, 0, "ttb"},
};

static BOOL is_horizontal_only_script(Uchar32 uc)
{
    size_t i;
    UCharScriptType st = UCharGetScriptType(uc);

    for (i = 0; i < TABLESIZE(vertical_scripts); i++) {
        if (st == vertical_scripts[i].st)
            return FALSE;
    }

    return TRUE;
}

static int font_get_glyph_metrics(LOGFONT* logfont,
        Glyph32 gv, int* adv_x, int* adv_y, BBOX* bbox)
{
    int bold = 0;
    int tmp_x = 0;
    int tmp_y = 0;
    int bbox_x = 0, bbox_y = 0;
    int bbox_w = 0, bbox_h = 0;
    int gbt;

    DEVFONT* devfont = SELECT_DEVFONT(logfont, gv);
    gbt = devfont->font_ops->get_glyph_bmptype (logfont, devfont)
            & DEVFONTGLYPHTYPE_MASK_BMPTYPE;

    devfont->font_ops->get_glyph_bbox (logfont, devfont,
            REAL_GLYPH(gv), &bbox_x, &bbox_y, &bbox_w, &bbox_h);

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

    devfont->font_ops->get_glyph_advance (logfont, devfont, REAL_GLYPH(gv),
        &tmp_x, &tmp_y);

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
    ((rf & GRF_TEXT_ORIENTATION_MASK) == GRF_TEXT_ORIENTATION_UPRIGHT)
#define IS_SIDEWAYS(rf)  \
    ((rf & GRF_TEXT_ORIENTATION_MASK) == GRF_TEXT_ORIENTATION_SIDEWAYS)

static void normalize_glyph_metrics(LOGFONT* logfont,
        Uint32 render_flags, const BBOX* bbox,
        int* adv_x, int* adv_y, int* line_adv, int* line_width)
{
    switch (render_flags & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        if (IS_SIDEWAYS(render_flags)) {
            *line_adv = *adv_y;
            if (logfont->size > *line_width)
                *line_width = logfont->size;
        }
        else {
            int tmp;
            tmp = *adv_x;
            *adv_x = *adv_y;
            *adv_y = tmp;

            *line_adv = logfont->size;
            if (*adv_x > *line_width)
                *line_width = *adv_x;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        *line_adv = *adv_x;
        if (logfont->size > *line_width)
            *line_width = logfont->size;
        break;
    }
}

static void set_extra_spacing(MYGLYPHARGS* args, int extra_spacing,
        GLYPHEXTINFO* gei)
{
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        gei->extra_x = 0;
        gei->extra_y = extra_spacing;
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        gei->extra_x = extra_spacing;
        gei->extra_y = 0;
        break;
    }
}

static void increase_extra_spacing(MYGLYPHARGS* args, int extra_spacing,
        GLYPHEXTINFO* gei)
{
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        gei->extra_y += extra_spacing;
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        gei->extra_x += extra_spacing;
        break;
    }
}

static int find_breaking_pos_normal(MYGLYPHARGS* args, int n)
{
    int i;

    for (i = n - 1; i >= 0; i--) {
        if (args->bos[i] == BOV_ALLOWED)
            return i;
    }

    return -1;
}

static int find_breaking_pos_any(MYGLYPHARGS* args, int n)
{
    int i;

    for (i = n - 1; i >= 0; i--) {
        if (args->bos[i] == BOV_ALLOWED
                || args->bos[i] == BOV_NOTALLOWED_UNCERTAINLY)
            return i;
    }

    return -1;
}

static int find_breaking_pos_word(MYGLYPHARGS* args, int n)
{
    int i;

    for (i = n - 1; i >= 0; i--) {
        if (args->bos[i] == BOV_ALLOWED
                && args->bcs[i] == UCHAR_BREAK_SPACE)
            return i;
    }

    return -1;
}

static inline BOOL is_invisible_glyph(const MYGLYPHINFO* gi)
{
    return (
        gi->gc == UCHAR_TYPE_CONTROL ||
        gi->gc == UCHAR_TYPE_FORMAT ||
        gi->bt == UCHAR_BREAK_ZERO_WIDTH_SPACE ||
        gi->bt == UCHAR_BREAK_ZERO_WIDTH_JOINER
    );
}

static inline BOOL is_word_separator(const MYGLYPHINFO* gi)
{
    return (
        gi->uc == 0x0020 || gi->uc == 0x00A0 ||
        gi->uc == 0x1361 ||
        gi->uc == 0x10100 || gi->uc == 0x10101 ||
        gi->uc == 0x1039F || gi->uc == 0x1091F
    );
}

/*
 * TODO: scripts and spacing:
 * https://www.w3.org/TR/css-text-3/#script-groups
 */
static inline BOOL is_typographic_char(const MYGLYPHINFO* gi)
{
    return (
        gi->gc != UCHAR_TYPE_FORMAT &&
        gi->gc != UCHAR_TYPE_CONTROL
    );
}

static void justify_glyphs_inter_word(MYGLYPHARGS* args,
        MYGLYPHINFO* gis, GLYPHEXTINFO* gei, int n, int error)
{
    int i;
    int nr_words = 0;
    int err_per_unit;
    int left;

    for (i = 0; i < n; i++) {
        if (gis[i].ignored == 0 && gis[i].hanged == 0 &&
                is_word_separator(gis + i)) {
            nr_words++;
            gis[i].justify_word = 1;
        }
        else {
            gis[i].justify_word = 0;
        }
    }

    if (nr_words <= 0)
        return;

    err_per_unit = error / nr_words;
    left = error % nr_words;
    if (err_per_unit > 0) {

        i = 0;
        do {
            if (gis[i].justify_word) {
                increase_extra_spacing(args, err_per_unit, gei + i);
                nr_words--;
            }

            i++;
        } while (nr_words > 0);
    }

    if (left > 0) {
        i = 0;
        do {
            if (gis[i].justify_word) {
                increase_extra_spacing(args, 1, gei + i);
                if (--left == 0)
                    break;
            }

            i++;
        } while (1);
    }
}

static void justify_glyphs_inter_char(MYGLYPHARGS* args,
        MYGLYPHINFO* gis, GLYPHEXTINFO* gei, int n, int error)
{
    int i;
    int nr_chars = 0;
    int err_per_unit;
    int left;

    for (i = 0; i < n; i++) {
        if (gis[i].ignored == 0 && gis[i].hanged == 0 &&
                !is_word_separator(gis + i) &&
                is_typographic_char(gis + i)) {
            nr_chars++;
            gis[i].justify_char = 1;
        }
        else {
            gis[i].justify_char = 0;
        }
    }

    nr_chars--;
    if (nr_chars <= 0)
        return;

    err_per_unit = error / nr_chars;
    left = error % nr_chars;
    if (err_per_unit > 0) {
        i = 0;
        do {
            if (gis[i].justify_char) {
                increase_extra_spacing(args, err_per_unit, gei + i);
                nr_chars--;
            }

            i++;
        } while (nr_chars > 0);
    }

    if (left > 0) {
        for (i = 0; i < n; i++) {
            if (gis[i].justify_char) {
                increase_extra_spacing(args, 1, gei + i);
                if (--left == 0)
                    break;
            }
        }
    }
}

/*
 * For auto justification, we use the following policy:
 * Primarily expanding word separators and between CJK typographic
 * letter units along with secondarily expanding between other
 * typographic character units.
 */
static void justify_glyphs_auto(MYGLYPHARGS* args,
        MYGLYPHINFO* gis, GLYPHEXTINFO* gei, int n, int error)
{
    int i;
    int total_error = error;
    int nr_words = 0;
    int nr_chars = 0;
    int err_per_unit;
    int compensated = 0;
    int left;

    for (i = 0; i < n; i++) {
        gis[i].justify_word = 0;
        gis[i].justify_char = 0;
        if (gis[i].ignored == 0 && gis[i].hanged == 0) {
            if (is_word_separator(gis + i) || IsUCharWideCJK(gis[i].uc)) {
                nr_words++;
                gis[i].justify_word = 1;
            }
            else if (is_typographic_char(gis + i)) {
                nr_chars++;
                gis[i].justify_char = 1;
            }
        }
    }

    nr_chars--;

    /* most error for words and CJK letters */
    if (nr_chars > 0)
        error = error * 2 / 3;

    if (nr_words > 0) {
        err_per_unit = error / nr_words;
        left = error % nr_words;
        if (err_per_unit > 0) {

            i = 0;
            do {
                if (gis[i].justify_word) {
                    increase_extra_spacing(args, err_per_unit, gei + i);
                    compensated += err_per_unit;
                    nr_words--;
                }

                i++;
            } while (nr_words > 0);
        }

        if (nr_chars <= 0 && left > 0) {
            for (i = 0; i < n; i++) {
                if (gis[i].justify_word) {
                    increase_extra_spacing(args, 1, gei + i);
                    if (--left == 0)
                        break;
                }
            }

            return;
        }
    }

    if (nr_chars > 0) {
        /* left error for other chars */
        error = total_error - compensated;
        err_per_unit = error / nr_chars;
        left = error % nr_chars;
        if (err_per_unit > 0) {
            i = 0;
            do {
                if (gis[i].justify_char) {
                    increase_extra_spacing(args, err_per_unit, gei + i);
                    nr_chars--;
                }

                i++;
            } while (nr_chars > 0);
        }

        if (left > 0) {
            for (i = 0; i < n; i++) {
                if (gis[i].justify_char) {
                    increase_extra_spacing(args, 1, gei + i);
                    if (--left == 0)
                        break;
                }
            }
        }
    }
}

static void adjust_glyph_position(MYGLYPHARGS* args,
        int x, int y, int lw, const GLYPHEXTINFO* gei, GLYPHPOS* pos)
{
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
        if (IS_SIDEWAYS(args->rf)) {
            x -= args->lfsw->size;
        }
        else {
            x -= (lw + gei->bbox_w) / 2;
        }
        break;

    case GRF_WRITING_MODE_VERTICAL_LR:
        if (IS_SIDEWAYS(args->rf)) {
        }
        else {
            x += (lw - gei->bbox_w) / 2;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        break;
    }

    pos->x += x;
    pos->y += y;
}

static void calc_glyph_positions(MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, const GLYPHEXTINFO* gei, int n,
        int x, int y, int lw, GLYPHPOS* pos)
{
    int i;

    for (i = 0; i < n; i++) {
        if (i == 0) {
            pos[i].x = 0;
            pos[i].y = 0;
        }
        else {
            pos[i].x = pos[i - 1].x + gei[i - 1].adv_x;
            pos[i].y = pos[i - 1].y + gei[i - 1].adv_y;
            pos[i].x += gei[i - 1].extra_x;
            pos[i].y += gei[i - 1].extra_y;
        }
    }

    for (i = 0; i < n; i++) {
        adjust_glyph_position(args, x, y, lw, gei + i, pos + i);

        pos[i].suppressed = gis[i].ignored;
        pos[i].hanged = gis[i].hanged;
        pos[i].orientation = gis[i].ort;
    }
}

static int adjust_hanged_glyphs_start(MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, const GLYPHEXTINFO* gei,
        GLYPHPOS* pos, int n, int start_x, int start_y)
{
    int i;
    int hanged_extent = 0;
    int last_hanged = -1;

    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        for (i = 0; i < n; i++) {
            if (gis[i].hanged == GLYPH_HANGED_START) {
                last_hanged = i;
                hanged_extent += gei[i].adv_y;
                hanged_extent += gei[i].extra_y;
            }
            else {
                break;
            }
        }

        if (hanged_extent > 0) {
            start_y -= hanged_extent;

            hanged_extent = 0;
            for (i = 0; i < last_hanged; i++) {
                pos[i].y = start_y + hanged_extent;
                hanged_extent += gei[i].adv_y;
                hanged_extent += gei[i].extra_y;
            }
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        for (i = 0; i < n; i++) {
            if (gis[i].hanged == GLYPH_HANGED_START) {
                last_hanged = i;
                hanged_extent += gei[i].adv_x;
                hanged_extent += gei[i].extra_x;
            }
            else {
                break;
            }
        }

        if (hanged_extent > 0) {
            start_x -= hanged_extent;

            hanged_extent = 0;
            for (i = 0; i < last_hanged; i++) {
                pos[i].x = start_x + hanged_extent;
                hanged_extent += gei[i].adv_x;
                hanged_extent += gei[i].extra_x;
            }
        }
        break;
    }

    return hanged_extent;
}

static int adjust_hanged_glyphs_end(MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, const GLYPHEXTINFO* gei,
        GLYPHPOS* pos, int n, int end_x, int end_y)
{
    int i;
    int hanged_extent = 0;
    int first_hanged = -1;

    for (i = n - 1; i > 0; i--) {
        if (gis[i].hanged == GLYPH_HANGED_END) {
            first_hanged = i;
        }
        else {
            break;
        }
    }

    if (first_hanged == -1)
        return 0;

    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        hanged_extent = 0;
        for (i = first_hanged; i < n; i++) {
            pos[i].y = end_y + hanged_extent;
            hanged_extent += gei[i].adv_y;
            hanged_extent += gei[i].extra_y;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        hanged_extent = 0;
        for (i = first_hanged; i < n; i++) {
            pos[i].x = end_x + hanged_extent;
            hanged_extent += gei[i].adv_x;
            hanged_extent += gei[i].extra_x;
        }
        break;
    }

    return hanged_extent;
}

static void offset_glyph_positions(MYGLYPHARGS* args,
        GLYPHPOS* pos, int n, int offset)
{
    int i;

    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        for (i = 0; i < n; i++) {
            if (pos[i].hanged == 0)
                pos[i].y += offset;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        for (i = 0; i < n; i++) {
            if (pos[i].hanged == 0)
                pos[i].x += offset;
        }
        break;
    }
}

static void align_unhanged_glyphs(MYGLYPHARGS* args,
        GLYPHPOS* pos, int n, int gap)
{
    switch (args->rf & GRF_ALIGN_MASK) {
    case GRF_ALIGN_RIGHT:
    case GRF_ALIGN_END:
        offset_glyph_positions(args, pos, n, gap);
        break;

    case GRF_ALIGN_CENTER:
        offset_glyph_positions(args, pos, n, gap/2);
        break;

    case GRF_ALIGN_LEFT:
    case GRF_ALIGN_START:
    case GRF_ALIGN_JUSTIFY:
    default:
        break;
    }
}

static inline BOOL is_opening_punctation(const MYGLYPHINFO* gi)
{
    return (
        gi->gc == UCHAR_TYPE_OPEN_PUNCTUATION ||
        gi->gc == UCHAR_TYPE_FINAL_PUNCTUATION ||
        gi->gc == UCHAR_TYPE_INITIAL_PUNCTUATION ||
        gi->uc == 0x0027 ||
        gi->uc == 0x0022
    );
}

static inline BOOL is_closing_punctation(const MYGLYPHINFO* gi)
{
    return (
        gi->gc == UCHAR_TYPE_CLOSE_PUNCTUATION ||
        gi->gc == UCHAR_TYPE_FINAL_PUNCTUATION ||
        gi->gc == UCHAR_TYPE_INITIAL_PUNCTUATION ||
        gi->uc == 0x0027 ||
        gi->uc == 0x0022
    );
}

static inline BOOL is_stop_or_common(const MYGLYPHINFO* gi)
{
    return (
        gi->uc == 0x002C || //  ,   COMMA
        gi->uc == 0x002E || //  .   FULL STOP
        gi->uc == 0x060C || //  ،   ARABIC COMMA
        gi->uc == 0x06D4 || //  ۔   ARABIC FULL STOP
        gi->uc == 0x3001 || //  、  IDEOGRAPHIC COMMA
        gi->uc == 0x3002 || //  。  IDEOGRAPHIC FULL STOP
        gi->uc == 0xFF0C || //  ，  FULLWIDTH COMMA
        gi->uc == 0xFF0E || //  ．  FULLWIDTH FULL STOP
        gi->uc == 0xFE50 || //  ﹐  SMALL COMMA
        gi->uc == 0xFE51 || //  ﹑  SMALL IDEOGRAPHIC COMMA
        gi->uc == 0xFE52 || //  ﹒  SMALL FULL STOP
        gi->uc == 0xFF61 || //  ｡   HALFWIDTH IDEOGRAPHIC FULL STOP
        gi->uc == 0xFF64    //  ､   HALFWIDTH IDEOGRAPHIC COMMA
    );
}

static void init_glyph_info(MYGLYPHARGS* args, int i,
        MYGLYPHINFO* gi)
{
    DEVFONT* devfont = SELECT_DEVFONT(args->lfur, args->gvs[i]);
    if (devfont->charset_ops->conv_to_uc32)
        gi->uc = devfont->charset_ops->conv_to_uc32(args->gvs[i]);
    else
        gi->uc = GLYPH2UCHAR(args->gvs[i]);
    gi->gc = UCharGetCategory(gi->uc);
    gi->bt = args->bcs[i];
    gi->ignored = 0;
    gi->hanged = GLYPH_HANGED_NONE;
    gi->ort = GLYPH_ORIENTATION_UPRIGHT;
}

static inline int shrink_total_extent(MYGLYPHARGS* args, int total_extent,
        const GLYPHEXTINFO* gei)
{
#if 0
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        total_extent -= gei->adv_y;
        total_extent -= gei->extra_y;
        break;
    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        total_extent -= gei->adv_x;
        total_extent -= gei->extra_x;
        break;
    }

    return total_extent;
#else
    return total_extent - gei->line_adv;
#endif

}

static int get_glyph_extent_info(MYGLYPHARGS* args, Glyph32 gv,
        MYGLYPHINFO* gi, GLYPHEXTINFO* gei, int* line_width)
{
    LOGFONT* logfont = args->lfur;
    BBOX bbox;
    int adv_x = 0, adv_y = 0;
    int line_adv;

    if ((args->rf & GRF_WRITING_MODE_MASK)
            != GRF_WRITING_MODE_HORIZONTAL_TB) {
        if ((args->rf & GRF_TEXT_ORIENTATION_MASK)
               == GRF_TEXT_ORIENTATION_MIXED) {
            if (is_horizontal_only_script(gi->uc)) {
                logfont = args->lfsw;
                gi->ort = GLYPH_ORIENTATION_SIDEWAYS;
            }
        }
        else if ((args->rf & GRF_TEXT_ORIENTATION_MASK)
               == GRF_TEXT_ORIENTATION_SIDEWAYS) {
            logfont = args->lfsw;
            gi->ort = GLYPH_ORIENTATION_SIDEWAYS;
        }
    }

    font_get_glyph_metrics(logfont, gv, &adv_x, &adv_y, &bbox);
    normalize_glyph_metrics(logfont, args->rf, &bbox,
            &adv_x, &adv_y, &line_adv, line_width);

    gei->bbox_x = bbox.x;
    gei->bbox_y = bbox.y;
    gei->bbox_w = bbox.w;
    gei->bbox_h = bbox.h;
    gei->adv_x = adv_x;
    gei->adv_y = adv_y;

    return line_adv;
}

static int get_first_normal_glyph(MYGLYPHINFO* gis, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        if (gis[i].ignored == 0 && gis[i].hanged == 0)
            return i;
    }

    return -1;
}

static int get_last_normal_glyph(MYGLYPHINFO* gis, int n)
{
    int i;
    for (i = n - 1; i >= 0; i--) {
        if (gis[i].ignored == 0 && gis[i].hanged == 0)
            return i;
    }

    return -1;
}

int GUIAPI GetGlyphsExtentPointEx(LOGFONT* logfont_upright,
        const Glyph32* glyphs, int nr_glyphs,
        const Uint8* break_oppos, const Uint8* break_classes,
        Uint32 render_flags, int x, int y,
        int letter_spacing, int word_spacing, int tab_size, int max_extent,
        SIZE* line_size, GLYPHEXTINFO* glyph_ext_info, GLYPHPOS* glyph_pos,
        LOGFONT** logfont_sideways)
{
    int n = 0;
    int total_extent = 0;
    int breaking_pos = -1;
    int line_width;
    int gap;

    MYGLYPHARGS  args;
    MYGLYPHINFO* gis = NULL;
    GLYPHEXTINFO* gei = NULL;
    BOOL test_overflow = TRUE;
    BOOL lfsw_created = FALSE;

    /* Check logfont_upright and create logfont_sideways if need */
    if (logfont_upright == NULL || logfont_upright->rotation != 0)
        return 0;

    if (*logfont_sideways == NULL) {
        if ((render_flags & GRF_WRITING_MODE_MASK)
                != GRF_WRITING_MODE_HORIZONTAL_TB) {
            switch (render_flags & GRF_TEXT_ORIENTATION_MASK) {
            case GRF_TEXT_ORIENTATION_MIXED:
            case GRF_TEXT_ORIENTATION_SIDEWAYS:
                *logfont_sideways = create_sideways_logfont(logfont_upright);
                if (*logfont_sideways == NULL
                        || (*logfont_sideways)->rotation != 900)
                    goto error;
                lfsw_created = TRUE;
                break;
            default:
                *logfont_sideways = NULL;
                break;
            }
        }
    }
    else {
        if ((*logfont_sideways)->rotation != 900) {
            goto error;
        }
    }

    if (glyph_ext_info == NULL) {
        gei = (GLYPHEXTINFO*)calloc(sizeof(GLYPHEXTINFO), nr_glyphs);
        if (gei == NULL)
            goto error;
    }
    else {
        gei = glyph_ext_info;
        memset(gei, 0, sizeof(GLYPHEXTINFO) * nr_glyphs);
    }

    gis = (MYGLYPHINFO*)calloc(sizeof(MYGLYPHINFO), nr_glyphs);
    if (gis == NULL)
        goto error;

    // skip the breaking opportunity before the first glyph
    // break_oppos++;

    args.lfur = logfont_upright;
    args.lfsw = *logfont_sideways;
    args.gvs = glyphs;
    args.bcs = break_classes;
    args.bos = break_oppos;
    args.rf = render_flags;
    args.nr_gvs = nr_glyphs;

    line_width = args.lfur->size;
    if (args.lfsw) {
        line_width = MAX(line_width, args.lfsw->size);
    }

    while (n < nr_glyphs) {
        int extra_spacing;

        init_glyph_info(&args, n, gis + n);

        /*
         * NOTE: The collapsible spaces should be handled in GetGlyphsByRules.
         */
        if (gis[n].uc == UCHAR_TAB) {
            if (tab_size > 0) {
                int tabstops = total_extent / tab_size + 1;
                gei[n].line_adv = tabstops * tab_size- total_extent;

                // If this distance is less than 0.5ch, then the
                // subsequent tab stop is used instead.
                if (gei[n].line_adv < logfont_upright->size / 6) {
                    tabstops++;
                    gei[n].line_adv = tabstops * tab_size- total_extent;
                }

                switch (render_flags & GRF_WRITING_MODE_MASK) {
                case GRF_WRITING_MODE_VERTICAL_RL:
                case GRF_WRITING_MODE_VERTICAL_LR:
                    gei[n].adv_y = gei[n].line_adv;
                    break;
                case GRF_WRITING_MODE_HORIZONTAL_TB:
                default:
                    gei[n].adv_x = gei[n].line_adv;
                    break;
                }
            }
            else {
                gis[n].ignored = 1;
                gei[n].line_adv = 0;
            }
        }
        else if (is_invisible_glyph (gis + n)) {
            gis[n].ignored = 1;
            gei[n].line_adv = 0;
        }
        else {
            gei[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    gei + n, &line_width);
        }

        // extra space for word and letter
        extra_spacing = 0;
        if (gis[n].ignored == 0 && is_word_separator(gis + n)) {
            extra_spacing = word_spacing;
        }
        else if (gis[n].ignored == 0 && is_typographic_char(gis + n)) {
            extra_spacing = letter_spacing;
        }

        if (extra_spacing > 0) {
            gei[n].line_adv += extra_spacing;
            set_extra_spacing(&args, extra_spacing, gei + n);
        }

        if (test_overflow && max_extent > 0
                && (total_extent + gei[n].line_adv) > max_extent) {
            // overflow
            switch (render_flags & GRF_OVERFLOW_WRAP_MASK) {
            case GRF_OVERFLOW_WRAP_BREAK_WORD:
                breaking_pos = find_breaking_pos_word(&args, n);
                break;
            case GRF_OVERFLOW_WRAP_ANYWHERE:
                breaking_pos = find_breaking_pos_any(&args, n);
                break;
            case GRF_OVERFLOW_WRAP_NORMAL:
            default:
                breaking_pos = find_breaking_pos_normal(&args, n);
                break;
            }

            if (breaking_pos >= 0) {
                // a valid breaking position found before current glyph
                break;
            }

            breaking_pos = -1;
            test_overflow = FALSE;
        }

        total_extent += gei[n].line_adv;
        if (break_oppos[n] == BOV_MANDATORY) {
            // hard line breaking
            n++;
            break;
        }

        if (!test_overflow && max_extent > 0
                && (break_oppos[n] == BOV_ALLOWED)) {
            n++;
            break;
        }

        n++;
    }

    if (breaking_pos >= 0 && breaking_pos != n) {
        // wrapped due to overflow
        int i;

        n = breaking_pos + 1;

        total_extent = 0;
        for (i = 0; i < n; i++) {
            total_extent += gei[i].line_adv;
        }
    }

    // Trimming spaces at the start of the line
    if (render_flags & GRF_SPACES_REMOVE_START) {
        int i = 0;
        while (i < n && gis[i].uc == UCHAR_SPACE) {
            gis[i].ignored = 1;
            memset(gei + i, 0, sizeof(GLYPHEXTINFO));
            i++;
        }
    }

    // Trimming or hanging spaces at the end of the line
    if (render_flags & GRF_SPACES_REMOVE_END) {
        int i = n - 1;
        while (i > 0 &&
                (gis[i].uc == UCHAR_SPACE || gis[i].uc == UCHAR_IDSPACE)) {

            gis[i].ignored = 1;
            memset(gei + i, 0, sizeof(GLYPHEXTINFO));
            i--;
        }
    }
    else if (render_flags & GRF_SPACES_HANGE_END) {
        int i = n - 1;
        while (i > 0 &&
                (gis[i].uc == UCHAR_SPACE || gis[i].uc == UCHAR_IDSPACE)) {

            gis[i].hanged = GLYPH_HANGED_END;
            i--;
        }
    }

    if (n < nr_glyphs) {
        init_glyph_info(&args, n, gis + n);
    }

    if (render_flags & GRF_HANGING_PUNC_OPEN) {
        int first = get_first_normal_glyph(gis, n);
        if (first >= 0 && is_opening_punctation(gis + first)) {
            gis[first].hanged = GLYPH_HANGED_START;
        }
    }

    if (n > 1 && render_flags & GRF_HANGING_PUNC_CLOSE) {
        int last = get_last_normal_glyph(gis, n);
        if (last >= 0 && is_closing_punctation(gis + last)) {
            gis[last].hanged = GLYPH_HANGED_END;
        }
        else if (n < nr_glyphs && is_closing_punctation(gis + n)) {
            gis[n].hanged = GLYPH_HANGED_END;
            gei[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    gei + n, &line_width);
            n++;
        }
    }

    if (render_flags & GRF_HANGING_PUNC_FORCE_END) {
        // A stop or comma at the end of a line hangs.
        int last = get_last_normal_glyph(gis, n);
        if (last >= 0 && is_stop_or_common(gis + last)) {
            gis[last].hanged = GLYPH_HANGED_END;
        }
        else if (n < nr_glyphs && is_stop_or_common(gis + n)) {
            gis[n].hanged = GLYPH_HANGED_END;
            gei[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    gei + n, &line_width);
            n++;
        }
    }
    else if (render_flags & GRF_HANGING_PUNC_ALLOW_END) {
        // A stop or comma at the end of a line hangs
        // if it does not otherwise fit prior to justification.
        if (n < nr_glyphs && is_stop_or_common(gis + n)) {
            gis[n].hanged = GLYPH_HANGED_END;
            gei[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    gei + n, &line_width);
            n++;
        }
    }

    if ((render_flags & GRF_HANGING_PUNC_MASK) ||
            (render_flags & GRF_SPACES_MASK)) {
        int i;

        // recalculate total extent
        total_extent = 0;
        for (i = 0; i < n; i++) {
            if (gis[i].hanged == 0 && gis[i].ignored == 0)
                total_extent += gei[i].line_adv;
        }
    }

    gap = max_extent - total_extent;
    // justify the glyphs
    if ((render_flags & GRF_ALIGN_MASK) == GRF_ALIGN_JUSTIFY
            && gap > 0) {
        switch (render_flags & GRF_TEXT_JUSTIFY_MASK) {
        case GRF_TEXT_JUSTIFY_INTER_WORD:
            justify_glyphs_inter_word(&args, gis, gei, n, gap);
            break;
        case GRF_TEXT_JUSTIFY_INTER_CHAR:
            justify_glyphs_inter_char(&args, gis, gei, n, gap);
            break;
        case GRF_TEXT_JUSTIFY_AUTO:
        default:
            justify_glyphs_auto(&args, gis, gei, n, gap);
            break;
        }
    }

    // calcualte glyph positions according to the base point
    calc_glyph_positions(&args, gis, gei, n, x, y, line_width, glyph_pos);

    // align unhanged glyphs
    align_unhanged_glyphs(&args, glyph_pos, n, gap);

    // adjust positions of hanged glyphs
    adjust_hanged_glyphs_start(&args, gis, gei, glyph_pos, n, x, y);
    if (max_extent > 0) {
        adjust_hanged_glyphs_end(&args, gis, gei,
                glyph_pos, n, x + max_extent, y + max_extent);
    }
    else {
        adjust_hanged_glyphs_end(&args, gis, gei,
                glyph_pos, n, x + total_extent, y + total_extent);
    }

    if (line_size) {
        if ((render_flags & GRF_WRITING_MODE_MASK)
                == GRF_WRITING_MODE_HORIZONTAL_TB) {
            line_size->cx = glyph_pos[n - 1].x - glyph_pos[0].x
                + gei[n - 1].adv_x + gei[n - 1].extra_x;
            line_size->cy = line_width;
        }
        else {
            line_size->cx = glyph_pos[n - 1].y - glyph_pos[0].y
                + gei[n - 1].adv_y + gei[n - 1].extra_y;
            line_size->cx = line_width;
        }
    }

    free(gis);
    if (glyph_ext_info == NULL)
        free(gei);

    return n;

error:
    if (gei) free(gei);
    if (gis) free(gis);
    if (lfsw_created) {
        DestroyLogFont(*logfont_sideways);
        *logfont_sideways = NULL;
    }
    return 0;
}

#endif /*  _MGCHARSET_UNICODE */

int GUIAPI DrawGlyphStringEx(HDC hdc,
        LOGFONT* logfont_upright, LOGFONT* logfont_sideways,
        const Glyph32* glyphs, int nr_glyphs,
        const GLYPHPOS* glyph_pos)
{
    int i;
    int n = 0;
    Uint32 old_ta;
    PLOGFONT old_lf;

    if (glyph_pos == NULL || nr_glyphs <= 0)
        return 0;

    old_ta = SetTextAlign(hdc, TA_LEFT | TA_TOP | TA_UPDATECP);
    old_lf = GetCurFont(hdc);

    for (i = 0; i < nr_glyphs; i++) {
        if (glyph_pos[i].suppressed == 0) {
            if (glyph_pos[i].orientation == GLYPH_ORIENTATION_UPRIGHT) {
                if (logfont_upright)
                    SelectFont(hdc, logfont_upright);
                else
                    goto error;
            }
            else {
                if (logfont_sideways)
                    SelectFont(hdc, logfont_sideways);
                else
                    goto error;
            }

            DrawGlyph(hdc, glyph_pos[i].x, glyph_pos[i].y, glyphs[i],
                NULL, NULL);

            n++;
        }
    }

error:
    SelectFont(hdc, old_lf);
    SetTextAlign(hdc, old_ta);

    return n;
}

