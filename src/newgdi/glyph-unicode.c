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
** of CSS 3, UAX#29, and UAX#14.
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
#include "unicode-ops.h"

#ifdef _MGCHARSET_UNICODE

#define MIN_LEN_GLYPHS      4
#define INC_LEN_GLYPHS      4
#define UCHAR_BREAK_UNSET   0xFF

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

/* See Grapheme_Cluster_Break Property Values table of UAX#29 */
typedef enum {
    GB_Other,
    GB_ControlCRLF,
    GB_Extend,
    GB_ZWJ,
    GB_Prepend,
    GB_SpacingMark,
    GB_InHangulSyllable, /* Handles all of L, V, T, LV, LVT rules */
    /* Use state machine to handle emoji sequence */
    /* Rule GB12 and GB13 */
    GB_RI_Odd, /* Meets odd number of RI */
    GB_RI_Even, /* Meets even number of RI */
} _GBType;

/* See Word_Break Property Values table of UAX#29 */
typedef enum {
    WB_Other,
    WB_NewlineCRLF,
    WB_ExtendFormat,
    WB_Katakana,
    WB_Hebrew_Letter,
    WB_ALetter,
    WB_MidNumLet,
    WB_MidLetter,
    WB_MidNum,
    WB_Numeric,
    WB_ExtendNumLet,
    WB_RI_Odd,
    WB_RI_Even,
    WB_WSegSpace,
} _WBType;

/* See Sentence_Break Property Values table of UAX#29 */
typedef enum {
    SB_Other,
    SB_ExtendFormat,
    SB_ParaSep,
    SB_Sp,
    SB_Lower,
    SB_Upper,
    SB_OLetter,
    SB_Numeric,
    SB_ATerm,
    SB_SContinue,
    SB_STerm,
    SB_Close,
    /* Rules SB8 and SB8a */
    SB_ATerm_Close_Sp,
    SB_STerm_Close_Sp,
} _SBType;

/* An enum that works as the states of the Hangul syllables system.
 */
typedef enum {
    JAMO_L,     /* UCHAR_BREAK_HANGUL_L_JAMO */
    JAMO_V,     /* UCHAR_BREAK_HANGUL_V_JAMO */
    JAMO_T,     /* UCHAR_BREAK_HANGUL_T_JAMO */
    JAMO_LV,    /* UCHAR_BREAK_HANGUL_LV_SYLLABLE */
    JAMO_LVT,   /* UCHAR_BREAK_HANGUL_LVT_SYLLABLE */
    NO_JAMO     /* Other */
} _JamoType;

/* Previously "123foo" was two words. But in UAX#29,
 * we know don't break words between consecutive letters and numbers.
 */
typedef enum {
  WordNone,
  WordLetters,
  WordNumbers
} _WordType;

struct glyph_break_ctxt {
    LOGFONT* lf;
    DEVFONT* mbc_devfont;
    DEVFONT* sbc_devfont;
    Uchar32* ucs;
    Uint8*   bts;
    Uint16*  bos;
    Uint8*   ods;

    int      len_buff;
    int      n;

    LanguageCode cl;
    UCharScriptType ws;

    Uint8   wsr;
    Uint8   ctr;
    Uint8   wbr;
    Uint8   lbp;

    Uint8   base_bt;
    Uint8   curr_gc;
    Uint8   curr_od;

    /* UAX#29 boundaries */
    Uchar32 prev_uc;
    Uchar32 base_uc;
    Uchar32 last_word_letter;

    int     last_stc_start;
    int     last_non_space;
    int     prev_wb_index;
    int     prev_sb_index;

    _GBType prev_gbt;
    _WBType prev_wbt, prev_prev_wbt;
    _SBType prev_sbt, prev_prev_sbt;
    _JamoType prev_jamo;
    _WordType curr_wt;

    Uint8   makes_hangul_syllable:1;
    Uint8   met_extended_pictographic:1;
    Uint8   is_extended_pictographic:1;

    Uint8   is_grapheme_boundary:1;
    Uint8   is_word_boundary:1;
    Uint8   is_sentence_boundary:1;
};

static BOOL gbctxt_change_lbo_last(struct glyph_break_ctxt* gbctxt,
        Uint16 lbo)
{
    int i = gbctxt->n - 1;

    if (i < 0)
        return FALSE;

    if ((gbctxt->bos[i] & BOV_LB_MASK) == BOV_UNKNOWN) {
         gbctxt->bos[i] &= ~BOV_LB_MASK;
         gbctxt->bos[i] |= lbo;
         gbctxt->ods[i] = gbctxt->curr_od;
    }
    else if (gbctxt->bos[i] & BOV_LB_MANDATORY_FLAG) {
        _DBG_PRINTF("%s: ignore the change: old one is mandatory\n",
            __FUNCTION__);
    }
    else if (gbctxt->curr_od <= gbctxt->ods[i]) {
        _DBG_PRINTF("%s: changed: curr_od(%d), org_od(%d)\n",
            __FUNCTION__, gbctxt->curr_od, gbctxt->ods[i]);
         gbctxt->bos[i] &= ~BOV_LB_MASK;
         gbctxt->bos[i] |= lbo;
         gbctxt->ods[i] = gbctxt->curr_od;
    }
    else {
        _DBG_PRINTF("%s: ignore the change: curr_od(%d), org_od(%d)\n",
            __FUNCTION__, gbctxt->curr_od, gbctxt->ods[i]);
    }

    return TRUE;
}

static BOOL gbctxt_change_lbo_before_last(struct glyph_break_ctxt* gbctxt,
        Uint16 lbo)
{
    int i = gbctxt->n - 2;

    // do not allow to change the first break value
    if (i < 1) {
        return FALSE;
    }

    if ((gbctxt->bos[i] & BOV_LB_MASK) == BOV_UNKNOWN) {
         gbctxt->bos[i] &= ~BOV_LB_MASK;
         gbctxt->bos[i] |= lbo;
         gbctxt->ods[i] = gbctxt->curr_od;
    }
    else if (gbctxt->bos[i] & BOV_LB_MANDATORY_FLAG) {
        _DBG_PRINTF("%s: ignore the change: old one is mandatory\n",
            __FUNCTION__);
    }
    else if (gbctxt->curr_od <= gbctxt->ods[i]) {
        _DBG_PRINTF("%s: changed: curr_od(%d), org_od(%d)\n",
            __FUNCTION__, gbctxt->curr_od, gbctxt->ods[gbctxt->n - 2]);
         gbctxt->bos[i] &= ~BOV_LB_MASK;
         gbctxt->bos[i] |= lbo;
         gbctxt->ods[i] = gbctxt->curr_od;
    }
    else {
        _DBG_PRINTF("%s: ignore the change\n", __FUNCTION__);
    }

    return TRUE;
}

#if 0
static BOOL gbctxt_change_lbo_before_last_sp(struct glyph_break_ctxt* gbctxt,
        Uint16 lbo)
{
    // do not allow to change the first break value
    if (gbctxt->n < 3) {
        return FALSE;
    }

    if (REAL_GLYPH(gbctxt->gvs[gbctxt->n - 3]) == UCHAR_SPACE) {
        _DBG_PRINTF("%s: force changed: curr_od(%d), org_od(%d)\n",
            __FUNCTION__, gbctxt->curr_od, gbctxt->ods[gbctxt->n - 2]);
         gbctxt->bos[gbctxt->n - 2] = lbo;
         gbctxt->ods[gbctxt->n - 2] = gbctxt->curr_od;
    }
    else if (gbctxt->curr_od < gbctxt->ods[gbctxt->n - 2]) {
        _DBG_PRINTF("%s: changed: curr_od(%d), org_od(%d)\n",
            __FUNCTION__, gbctxt->curr_od, gbctxt->ods[gbctxt->n - 2]);
         gbctxt->bos[gbctxt->n - 2] = lbo;
         gbctxt->ods[gbctxt->n - 2] = gbctxt->curr_od;
    }
    else {
        _DBG_PRINTF("%s: ignore the change\n", __FUNCTION__);
    }

    return TRUE;
}
#endif

static int get_next_uchar(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32* uc)
{
    int mclen = 0;

    if (mstr_len <= 0 || *mstr == '\0')
        return 0;

    if (gbctxt->mbc_devfont) {
        mclen = gbctxt->mbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)mstr, mstr_len);

        if (mclen > 0) {
            Achar32 chv = gbctxt->mbc_devfont->charset_ops->get_char_value
                (NULL, 0, (Uint8*)mstr, mclen);

            if (gbctxt->mbc_devfont->charset_ops->conv_to_uc32)
                *uc = gbctxt->mbc_devfont->charset_ops->conv_to_uc32(chv);
            else
                *uc = chv;

            chv = SET_MBCHV(chv);
        }
    }

    if (mclen == 0) {
        mclen = gbctxt->sbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)mstr, mstr_len);

        if (mclen > 0) {
            Achar32 chv = gbctxt->sbc_devfont->charset_ops->get_char_value
                (NULL, 0, (Uint8*)mstr, mclen);

            if (gbctxt->sbc_devfont->charset_ops->conv_to_uc32)
                *uc = gbctxt->sbc_devfont->charset_ops->conv_to_uc32(chv);
            else
                *uc = chv;
        }
    }

    return mclen;
}

static UCharBreakType resolve_lbc(struct glyph_break_ctxt* gbctxt, Uchar32 uc)
{
    UCharBreakType bt;

    bt = UCharGetBreakType(uc);
    gbctxt->curr_gc = UCharGetCategory(uc);

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
        if (gbctxt->curr_gc == UCHAR_CATEGORY_NON_SPACING_MARK
                || gbctxt->curr_gc == UCHAR_CATEGORY_SPACING_MARK) {
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

    return bt;
}

/* Find the Grapheme Break Type of uc */
static _GBType resolve_gbt(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharGeneralCategory gc)
{
    _GBType gbt;

    gbt = GB_Other;
    switch ((Uint8)gc) {
    case UCHAR_CATEGORY_FORMAT:
        if (uc == 0x200C) {
            gbt = GB_Extend;
            break;
        }
        if (uc == 0x200D) {
            gbt = GB_ZWJ;
            break;
        }
        if ((uc >= 0x600 && uc <= 0x605) ||
                uc == 0x6DD || uc == 0x70F ||
                uc == 0x8E2 || uc == 0xD4E || uc == 0x110BD ||
                (uc >= 0x111C2 && uc <= 0x111C3)) {
            gbt = GB_Prepend;
            break;
        }

    case UCHAR_CATEGORY_CONTROL:
    case UCHAR_CATEGORY_LINE_SEPARATOR:
    case UCHAR_CATEGORY_PARAGRAPH_SEPARATOR:
    case UCHAR_CATEGORY_SURROGATE:
        /* fall through */
        gbt = GB_ControlCRLF;
        break;

    case UCHAR_CATEGORY_UNASSIGNED:
        /* Unassigned default ignorables */
        if ((uc >= 0xFFF0 && uc <= 0xFFF8) ||
                (uc >= 0xE0000 && uc <= 0xE0FFF)) {
            gbt = GB_ControlCRLF;
            break;
        }

    case UCHAR_CATEGORY_OTHER_LETTER:
        if (gbctxt->makes_hangul_syllable)
            gbt = GB_InHangulSyllable;
        break;

    case UCHAR_CATEGORY_MODIFIER_LETTER:
        if (uc >= 0xFF9E && uc <= 0xFF9F)
            gbt = GB_Extend; /* Other_Grapheme_Extend */
        break;

    case UCHAR_CATEGORY_SPACING_MARK:
        gbt = GB_SpacingMark; /* SpacingMark */
        if (uc >= 0x0900 &&
            (uc == 0x09BE || uc == 0x09D7 ||
                uc == 0x0B3E || uc == 0x0B57 ||
                uc == 0x0BBE || uc == 0x0BD7 ||
                uc == 0x0CC2 || uc == 0x0CD5 ||
                uc == 0x0CD6 || uc == 0x0D3E ||
                uc == 0x0D57 || uc == 0x0DCF ||
                uc == 0x0DDF || uc == 0x1D165 ||
                (uc >= 0x1D16E && uc <= 0x1D172))) {
            gbt = GB_Extend; /* Other_Grapheme_Extend */
        }
        break;

    case UCHAR_CATEGORY_ENCLOSING_MARK:
    case UCHAR_CATEGORY_NON_SPACING_MARK:
        gbt = GB_Extend; /* Grapheme_Extend */
        break;

    case UCHAR_CATEGORY_OTHER_SYMBOL:
        if (uc >= 0x1F1E6 && uc <= 0x1F1FF) {
            if (gbctxt->prev_gbt == GB_RI_Odd)
                gbt = GB_RI_Even;
            else if (gbctxt->prev_gbt == GB_RI_Even)
                gbt = GB_RI_Odd;
            else
                gbt = GB_RI_Odd;
            break;
        }
        break;

    case UCHAR_CATEGORY_MODIFIER_SYMBOL:
        if (uc >= 0x1F3FB && uc <= 0x1F3FF)
            gbt = GB_Extend;
        break;
    }

    return gbt;
}

/* Find the Word Break Type of uc */
static _WBType resolve_wbt(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharGeneralCategory gc, UCharBreakType bt)
{
    UCharScriptType script;
    _WBType wbt;

    script = UCharGetScriptType(uc);
    wbt = WB_Other;

    if (script == UCHAR_SCRIPT_KATAKANA)
        wbt = WB_Katakana;

    if (script == UCHAR_SCRIPT_HEBREW && gc == UCHAR_CATEGORY_OTHER_LETTER)
        wbt = WB_Hebrew_Letter;

    if (wbt == WB_Other) {
        switch (uc >> 8) {
        case 0x30:
            if ((uc >= 0x3031 && uc <= 0x3035) ||
                    uc == 0x309b || uc == 0x309c ||
                    uc == 0x30a0 || uc == 0x30fc)
                wbt = WB_Katakana; /* Katakana exceptions */
            break;
        case 0xFF:
            if (uc == 0xFF70)
                wbt = WB_Katakana; /* Katakana exceptions */
            else if (uc >= 0xFF9E && uc <= 0xFF9F)
                wbt = WB_ExtendFormat; /* Other_Grapheme_Extend */
            break;
        case 0x05:
            if (uc == 0x05F3)
                wbt = WB_ALetter; /* ALetter exceptions */
            break;
        }
    }

    if (wbt == WB_Other) {
        switch ((Uint8)bt) {
        case UCHAR_BREAK_NUMERIC:
            if (uc != 0x066C)
                wbt = WB_Numeric; /* Numeric */
            break;
        case UCHAR_BREAK_INFIX_SEPARATOR:
            if (uc != 0x003A && uc != 0xFE13 && uc != 0x002E)
                wbt = WB_MidNum; /* MidNum */
            break;
        }
    }

    if (wbt == WB_Other) {
        switch ((Uint8)gc) {
        case UCHAR_CATEGORY_CONTROL:
            if (uc != 0x000D && uc != 0x000A && uc != 0x000B &&
                    uc != 0x000C && uc != 0x0085)
                break;
            /* fall through */
        case UCHAR_CATEGORY_LINE_SEPARATOR:
        case UCHAR_CATEGORY_PARAGRAPH_SEPARATOR:
            wbt = WB_NewlineCRLF; /* CR, LF, Newline */
            break;

        case UCHAR_CATEGORY_FORMAT:
        case UCHAR_CATEGORY_SPACING_MARK:
        case UCHAR_CATEGORY_ENCLOSING_MARK:
        case UCHAR_CATEGORY_NON_SPACING_MARK:
            wbt = WB_ExtendFormat; /* Extend, Format */
            break;

        case UCHAR_CATEGORY_CONNECT_PUNCTUATION:
            wbt = WB_ExtendNumLet; /* ExtendNumLet */
            break;

        case UCHAR_CATEGORY_INITIAL_PUNCTUATION:
        case UCHAR_CATEGORY_FINAL_PUNCTUATION:
            if (uc == 0x2018 || uc == 0x2019)
                wbt = WB_MidNumLet; /* MidNumLet */
            break;
        case UCHAR_CATEGORY_OTHER_PUNCTUATION:
            if (uc == 0x0027 || uc == 0x002e || uc == 0x2024 ||
                    uc == 0xfe52 || uc == 0xff07 || uc == 0xff0e)
                wbt = WB_MidNumLet; /* MidNumLet */
            else if (uc == 0x00b7 || uc == 0x05f4 || uc == 0x2027 ||
                    uc == 0x003a || uc == 0x0387 ||
                    uc == 0xfe13 || uc == 0xfe55 || uc == 0xff1a)
                wbt = WB_MidLetter; /* MidLetter */
            else if (uc == 0x066c ||
                    uc == 0xfe50 || uc == 0xfe54 || uc == 0xff0c ||
                    uc == 0xff1b)
                wbt = WB_MidNum; /* MidNum */
            break;

        case UCHAR_CATEGORY_OTHER_SYMBOL:
            if (uc >= 0x24B6 && uc <= 0x24E9) /* Other_Alphabetic */
                goto Alphabetic;

            if (uc >=0x1F1E6 && uc <=0x1F1FF) {
                if (gbctxt->prev_wbt == WB_RI_Odd)
                    wbt = WB_RI_Even;
                else if (gbctxt->prev_wbt == WB_RI_Even)
                    wbt = WB_RI_Odd;
                else
                    wbt = WB_RI_Odd;
            }

            break;

        case UCHAR_CATEGORY_OTHER_LETTER:
        case UCHAR_CATEGORY_LETTER_NUMBER:
            if (uc == 0x3006 || uc == 0x3007 ||
                    (uc >= 0x3021 && uc <= 0x3029) ||
                    (uc >= 0x3038 && uc <= 0x303A) ||
                    (uc >= 0x3400 && uc <= 0x4DB5) ||
                    (uc >= 0x4E00 && uc <= 0x9FC3) ||
                    (uc >= 0xF900 && uc <= 0xFA2D) ||
                    (uc >= 0xFA30 && uc <= 0xFA6A) ||
                    (uc >= 0xFA70 && uc <= 0xFAD9) ||
                    (uc >= 0x20000 && uc <= 0x2A6D6) ||
                    (uc >= 0x2F800 && uc <= 0x2FA1D))
                break; /* ALetter exceptions: Ideographic */
            goto Alphabetic;

        case UCHAR_CATEGORY_LOWERCASE_LETTER:
        case UCHAR_CATEGORY_MODIFIER_LETTER:
        case UCHAR_CATEGORY_TITLECASE_LETTER:
        case UCHAR_CATEGORY_UPPERCASE_LETTER:
Alphabetic:
            if (bt != UCHAR_BREAK_COMPLEX_CONTEXT
                    && script != UCHAR_SCRIPT_HIRAGANA)
                wbt = WB_ALetter; /* ALetter */
            break;
        }
    }

    if (wbt == WB_Other) {
        if (gc == UCHAR_CATEGORY_SPACE_SEPARATOR &&
                bt != UCHAR_BREAK_NON_BREAKING_GLUE)
            wbt = WB_WSegSpace;
    }

    _DBG_PRINTF("%s: uc(%0X), script(%d), wbt(%d)\n",
        __FUNCTION__, uc, script, wbt);

    return wbt;
}

/* Find the Sentence Break Type of wc */
static _SBType resolve_sbt(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharGeneralCategory gc, UCharBreakType bt)
{
    _SBType sbt;

    sbt = SB_Other;
    if (bt == UCHAR_BREAK_NUMERIC)
        sbt = SB_Numeric; /* Numeric */

    if (sbt == SB_Other) {
        switch ((Uint8)gc) {
        case UCHAR_CATEGORY_CONTROL:
            if (uc == '\r' || uc == '\n')
                sbt = SB_ParaSep;
            else if (uc == 0x0009 || uc == 0x000B || uc == 0x000C)
                sbt = SB_Sp;
            else if (uc == 0x0085)
                sbt = SB_ParaSep;
            break;

        case UCHAR_CATEGORY_SPACE_SEPARATOR:
            if (uc == 0x0020 || uc == 0x00A0 || uc == 0x1680 ||
                    (uc >= 0x2000 && uc <= 0x200A) ||
                    uc == 0x202F || uc == 0x205F || uc == 0x3000)
                sbt = SB_Sp;
            break;

        case UCHAR_CATEGORY_LINE_SEPARATOR:
        case UCHAR_CATEGORY_PARAGRAPH_SEPARATOR:
            sbt = SB_ParaSep;
            break;

        case UCHAR_CATEGORY_FORMAT:
        case UCHAR_CATEGORY_SPACING_MARK:
        case UCHAR_CATEGORY_ENCLOSING_MARK:
        case UCHAR_CATEGORY_NON_SPACING_MARK:
            sbt = SB_ExtendFormat; /* Extend, Format */
            break;

        case UCHAR_CATEGORY_MODIFIER_LETTER:
            if (uc >= 0xFF9E && uc <= 0xFF9F)
                sbt = SB_ExtendFormat; /* Other_Grapheme_Extend */
            break;

        case UCHAR_CATEGORY_TITLECASE_LETTER:
            sbt = SB_Upper;
            break;

        case UCHAR_CATEGORY_DASH_PUNCTUATION:
            if (uc == 0x002D ||
                    (uc >= 0x2013 && uc <= 0x2014) ||
                    (uc >= 0xFE31 && uc <= 0xFE32) ||
                    uc == 0xFE58 || uc == 0xFE63 || uc == 0xFF0D)
                sbt = SB_SContinue;
            break;

        case UCHAR_CATEGORY_OTHER_PUNCTUATION:
            if (uc == 0x05F3)
                sbt = SB_OLetter;
            else if (uc == 0x002E || uc == 0x2024 ||
                    uc == 0xFE52 || uc == 0xFF0E)
                sbt = SB_ATerm;

            if (uc == 0x002C || uc == 0x003A || uc == 0x055D ||
                    (uc >= 0x060C && uc <= 0x060D) ||
                    uc == 0x07F8 || uc == 0x1802 ||
                    uc == 0x1808 || uc == 0x3001 ||
                    (uc >= 0xFE10 && uc <= 0xFE11) ||
                    uc == 0xFE13 ||
                    (uc >= 0xFE50 && uc <= 0xFE51) ||
                    uc == 0xFE55 || uc == 0xFF0C ||
                    uc == 0xFF1A || uc == 0xFF64)
                sbt = SB_SContinue;

            if (uc == 0x0021 || uc == 0x003F ||
                    uc == 0x0589 || uc == 0x061F || uc == 0x06D4 ||
                    (uc >= 0x0700 && uc <= 0x0702) ||
                    uc == 0x07F9 ||
                    (uc >= 0x0964 && uc <= 0x0965) ||
                    (uc >= 0x104A && uc <= 0x104B) ||
                    uc == 0x1362 ||
                    (uc >= 0x1367 && uc <= 0x1368) ||
                    uc == 0x166E ||
                    (uc >= 0x1735 && uc <= 0x1736) ||
                    uc == 0x1803 || uc == 0x1809 ||
                    (uc >= 0x1944 && uc <= 0x1945) ||
                    (uc >= 0x1AA8 && uc <= 0x1AAB) ||
                    (uc >= 0x1B5A && uc <= 0x1B5B) ||
                    (uc >= 0x1B5E && uc <= 0x1B5F) ||
                    (uc >= 0x1C3B && uc <= 0x1C3C) ||
                    (uc >= 0x1C7E && uc <= 0x1C7F) ||
                    (uc >= 0x203C && uc <= 0x203D) ||
                    (uc >= 0x2047 && uc <= 0x2049) ||
                    uc == 0x2E2E || uc == 0x2E3C ||
                    uc == 0x3002 || uc == 0xA4FF ||
                    (uc >= 0xA60E && uc <= 0xA60F) ||
                    uc == 0xA6F3 || uc == 0xA6F7 ||
                    (uc >= 0xA876 && uc <= 0xA877) ||
                    (uc >= 0xA8CE && uc <= 0xA8CF) ||
                    uc == 0xA92F ||
                    (uc >= 0xA9C8 && uc <= 0xA9C9) ||
                    (uc >= 0xAA5D && uc <= 0xAA5F) ||
                    (uc >= 0xAAF0 && uc <= 0xAAF1) ||
                    uc == 0xABEB ||
                    (uc >= 0xFE56 && uc <= 0xFE57) ||
                    uc == 0xFF01 || uc == 0xFF1F || uc == 0xFF61 ||
                    (uc >= 0x10A56 && uc <= 0x10A57) ||
                    (uc >= 0x11047 && uc <= 0x11048) ||
                    (uc >= 0x110BE && uc <= 0x110C1) ||
                    (uc >= 0x11141 && uc <= 0x11143) ||
                    (uc >= 0x111C5 && uc <= 0x111C6) ||
                    uc == 0x111CD ||
                    (uc >= 0x111DE && uc <= 0x111DF) ||
                    (uc >= 0x11238 && uc <= 0x11239) ||
                    (uc >= 0x1123B && uc <= 0x1123C) ||
                    uc == 0x112A9 ||
                    (uc >= 0x1144B && uc <= 0x1144C) ||
                    (uc >= 0x115C2 && uc <= 0x115C3) ||
                    (uc >= 0x115C9 && uc <= 0x115D7) ||
                    (uc >= 0x11641 && uc <= 0x11642) ||
                    (uc >= 0x1173C && uc <= 0x1173E) ||
                    (uc >= 0x11C41 && uc <= 0x11C42) ||
                    (uc >= 0x16A6E && uc <= 0x16A6F) ||
                    uc == 0x16AF5 ||
                    (uc >= 0x16B37 && uc <= 0x16B38) ||
                    uc == 0x16B44 || uc == 0x1BC9F || uc == 0x1DA88)
                sbt = SB_STerm;
            break;
        }
    }

    if (sbt == SB_Other) {
        if (IsUCharLowercase(uc))
            sbt = SB_Lower;
        else if (IsUCharUppercase(uc))
            sbt = SB_Upper;
        else if (IsUCharAlpha(uc))
            sbt = SB_OLetter;

        if (gc == UCHAR_CATEGORY_OPEN_PUNCTUATION ||
                gc == UCHAR_CATEGORY_CLOSE_PUNCTUATION ||
                bt == UCHAR_BREAK_QUOTATION)
            sbt = SB_Close;
    }

    return sbt;
}

/* There are Hangul syllables encoded as characters, that act like a
 * sequence of Jamos. For each character we define a JamoType
 * that the character starts with, and one that it ends with.  This
 * decomposes JAMO_LV and JAMO_LVT to simple other JAMOs.  So for
 * example, a character with LineBreak type
 * UCHAR_BREAK_HANGUL_LV_SYLLABLE has start=JAMO_L and end=JAMO_V.
 */
struct _CharJamoProps {
    _JamoType start, end;
};

/* Map from JamoType to CharJamoProps that hold only simple
 * JamoTypes (no LV or LVT) or none.
 */
static const struct _CharJamoProps HangulJamoProps[] = {
    {JAMO_L, JAMO_L},   /* JAMO_L */
    {JAMO_V, JAMO_V},   /* JAMO_V */
    {JAMO_T, JAMO_T},   /* JAMO_T */
    {JAMO_L, JAMO_V},   /* JAMO_LV */
    {JAMO_L, JAMO_T},   /* JAMO_LVT */
    {NO_JAMO, NO_JAMO}  /* NO_JAMO */
};

/* A character forms a syllable with the previous character if and only if:
 * JamoType(this) is not NO_JAMO and:
 *
 * HangulJamoProps[JamoType(prev)].end and
 * HangulJamoProps[JamoType(this)].start are equal,
 * or the former is one less than the latter.
 */

#define IS_JAMO(btype)                          \
    ((btype >= UCHAR_BREAK_HANGUL_L_JAMO) &&    \
     (btype <= UCHAR_BREAK_HANGUL_LVT_SYLLABLE))

#define JAMO_TYPE(btype)    \
    (IS_JAMO(btype) ? (btype - UCHAR_BREAK_HANGUL_L_JAMO) : NO_JAMO)

/* Determine wheter this forms a Hangul syllable with prev. */
static void check_hangul_syllable(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharBreakType bt)
{
    _JamoType jamo = JAMO_TYPE (bt);
    if (jamo == NO_JAMO)
        gbctxt->makes_hangul_syllable = 0;
    else {
        _JamoType prev_end   = HangulJamoProps[gbctxt->prev_jamo].end;
        _JamoType this_start = HangulJamoProps[jamo].start;

        /* See comments before IS_JAMO */
        gbctxt->makes_hangul_syllable = (prev_end == this_start) ||
            (prev_end + 1 == this_start);
    }

    if (bt != UCHAR_BREAK_SPACE)
        gbctxt->prev_jamo = jamo;
}

static Uint16 check_space(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharGeneralCategory gc)
{
    Uint16 bo;

    switch (gc) {
    case UCHAR_CATEGORY_SPACE_SEPARATOR:
    case UCHAR_CATEGORY_LINE_SEPARATOR:
    case UCHAR_CATEGORY_PARAGRAPH_SEPARATOR:
        bo = BOV_WHITESPACE;
        break;

    default:
        if (uc == '\t' || uc == '\n' || uc == '\r' || uc == '\f')
            bo = BOV_WHITESPACE;
        else
            bo = BOV_UNKNOWN;
        break;
    }

    /* Just few spaces have variable width. So explicitly mark them.
     */
    if (0x0020 == uc || 0x00A0 == uc) {
        bo |= BOV_EXPANDABLE_SPACE;
    }

    if (uc != 0x00AD && (
            gc == UCHAR_CATEGORY_NON_SPACING_MARK ||
            gc == UCHAR_CATEGORY_ENCLOSING_MARK ||
            gc == UCHAR_CATEGORY_FORMAT))
        bo |= BOV_ZERO_WIDTH;
    else if ((uc >= 0x1160 && uc < 0x1200) || uc == 0x200B)
        bo |= BOV_ZERO_WIDTH;

    return bo;
}

static inline
void check_emoji_extended_pictographic(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc)
{
    gbctxt->is_extended_pictographic =
        _unicode_is_emoji_extended_pictographic(uc);
}

/* Types of Japanese characters */
#define JAPANESE(uc) ((uc) >= 0x2F00 && (uc) <= 0x30FF)
#define KANJI(uc)    ((uc) >= 0x2F00 && (uc) <= 0x2FDF)
#define HIRAGANA(uc) ((uc) >= 0x3040 && (uc) <= 0x309F)
#define KATAKANA(uc) ((uc) >= 0x30A0 && (uc) <= 0x30FF)

#define LATIN(uc) (((uc) >= 0x0020 && (uc) <= 0x02AF) || ((uc) >= 0x1E00 && (uc) <= 0x1EFF))
#define CYRILLIC(uc) (((uc) >= 0x0400 && (uc) <= 0x052F))
#define GREEK(uc) (((uc) >= 0x0370 && (uc) <= 0x3FF) || ((uc) >= 0x1F00 && (uc) <= 0x1FFF))
#define KANA(uc) ((uc) >= 0x3040 && (uc) <= 0x30FF)
#define HANGUL(uc) ((uc) >= 0xAC00 && (uc) <= 0xD7A3)
#define BACKSPACE_DELETES_CHARACTER(uc) \
    (!LATIN (uc) && !CYRILLIC (uc) && !GREEK (uc) && !KANA(uc) && !HANGUL(uc))

static Uint16 check_grapheme_boundaries(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharGeneralCategory gc, UCharBreakType bt)
{
    _GBType gbt;
    Uint16 bo;

    gbt = resolve_gbt(gbctxt, uc, gc);

    /* Rule GB11 */
    if (gbctxt->met_extended_pictographic) {
        if (gbt == GB_Extend)
            gbctxt->met_extended_pictographic = 1;
        else if (_unicode_is_emoji_extended_pictographic(gbctxt->prev_uc) &&
                gbt == GB_ZWJ)
            gbctxt->met_extended_pictographic = 1;
        else if (gbctxt->prev_gbt == GB_Extend && gbt == GB_ZWJ)
            gbctxt->met_extended_pictographic = 1;
        else if (gbctxt->prev_gbt == GB_ZWJ && gbctxt->is_extended_pictographic)
            gbctxt->met_extended_pictographic = 1;
        else
            gbctxt->met_extended_pictographic = 0;
    }

    /* Grapheme Cluster Boundary Rules */
    gbctxt->is_grapheme_boundary = 1; /* Rule GB999 */

    /* We apply Rules GB1 and GB2 at the upper level of the function */

    if (uc == '\n' && gbctxt->prev_uc == '\r')
        gbctxt->is_grapheme_boundary = 0; /* Rule GB3 */
    else if (gbctxt->prev_gbt == GB_ControlCRLF || gbt == GB_ControlCRLF)
        gbctxt->is_grapheme_boundary = 1; /* Rules GB4 and GB5 */
    else if (gbt == GB_InHangulSyllable)
        gbctxt->is_grapheme_boundary = 0; /* Rules GB6, GB7, GB8 */
    else if (gbt == GB_Extend) {
        gbctxt->is_grapheme_boundary = 0; /* Rule GB9 */
    }
    else if (gbt == GB_ZWJ)
        gbctxt->is_grapheme_boundary = 0; /* Rule GB9 */
    else if (gbt == GB_SpacingMark)
        gbctxt->is_grapheme_boundary = 0; /* Rule GB9a */
    else if (gbctxt->prev_gbt == GB_Prepend)
        gbctxt->is_grapheme_boundary = 0; /* Rule GB9b */
    else if (gbctxt->is_extended_pictographic) {
        /* Rule GB11 */
        if (gbctxt->prev_gbt == GB_ZWJ && gbctxt->met_extended_pictographic)
            gbctxt->is_grapheme_boundary = 0;
    }
    else if (gbctxt->prev_gbt == GB_RI_Odd && gbt == GB_RI_Even)
        gbctxt->is_grapheme_boundary = 0; /* Rule GB12 and GB13 */

    if (gbctxt->is_extended_pictographic)
        gbctxt->met_extended_pictographic = 1;

    bo = 0;
    if (gbctxt->is_grapheme_boundary) {
        bo = BOV_GB_CURSOR_POS;
    }

    /* If this is a grapheme boundary, we have to decide if backspace
     * deletes a character or the whole grapheme cluster */
    if (gbctxt->is_grapheme_boundary) {
        bo |= BOV_GB_CHAR_BREAK;
        if (BACKSPACE_DELETES_CHARACTER (gbctxt->base_uc))
            bo |= BOV_GB_BACKSPACE_DEL_CH;
    }

    gbctxt->prev_gbt = gbt;

    return bo;
}

static Uint16 check_word_boundaries(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharGeneralCategory gc, UCharBreakType bt, int i)
{
    gbctxt->is_word_boundary = 0;

    /* Rules WB3 and WB4 */
    if (gbctxt->is_grapheme_boundary ||
            (uc >= 0x1F1E6 && uc <= 0x1F1FF)) {

        _WBType wbt = resolve_wbt(gbctxt, uc, gc, bt);

        /* We apply Rules WB1 and WB2 at the upper level of the function */

        if (gbctxt->prev_uc == 0x3031 && uc == 0x41)
            _DBG_PRINTF ("%s: Y %d %d\n",
                __FUNCTION__, gbctxt->prev_wbt, wbt);

        if (gbctxt->prev_wbt == WB_NewlineCRLF &&
                gbctxt->prev_wb_index + 1 == i) {
            /* The extra check for gbctxt->prev_wb_index is to
             * correctly handle sequences like
             * Newline ÷ Extend × Extend
             * since we have not skipped ExtendFormat yet.
             */
            gbctxt->is_word_boundary = 1; /* Rule WB3a */
        }
        else if (wbt == WB_NewlineCRLF)
            gbctxt->is_word_boundary = 1; /* Rule WB3b */
        else if (gbctxt->prev_uc == 0x200D &&
                gbctxt->is_extended_pictographic)
            gbctxt->is_word_boundary = 0; /* Rule WB3c */
        else if (gbctxt->prev_wbt == WB_WSegSpace &&
                wbt == WB_WSegSpace && gbctxt->prev_wb_index + 1 == i)
            gbctxt->is_word_boundary = 0; /* Rule WB3d */
        else if (wbt == WB_ExtendFormat)
            gbctxt->is_word_boundary = 0; /* Rules WB4? */
        else if ((gbctxt->prev_wbt == WB_ALetter  ||
                    gbctxt->prev_wbt == WB_Hebrew_Letter ||
                    gbctxt->prev_wbt == WB_Numeric) &&
                (wbt == WB_ALetter  ||
                 wbt == WB_Hebrew_Letter ||
                 wbt == WB_Numeric))
            gbctxt->is_word_boundary = 0; /* Rules WB5, WB8, WB9, WB10 */
        else if (gbctxt->prev_wbt == WB_Katakana && wbt == WB_Katakana)
            gbctxt->is_word_boundary = 0; /* Rule WB13 */
        else if ((gbctxt->prev_wbt == WB_ALetter ||
                    gbctxt->prev_wbt == WB_Hebrew_Letter ||
                    gbctxt->prev_wbt == WB_Numeric ||
                    gbctxt->prev_wbt == WB_Katakana ||
                    gbctxt->prev_wbt == WB_ExtendNumLet) &&
                wbt == WB_ExtendNumLet)
            gbctxt->is_word_boundary = 0; /* Rule WB13a */
        else if (gbctxt->prev_wbt == WB_ExtendNumLet &&
                (wbt == WB_ALetter ||
                 wbt == WB_Hebrew_Letter ||
                 wbt == WB_Numeric ||
                 wbt == WB_Katakana))
            gbctxt->is_word_boundary = 0; /* Rule WB13b */
        else if (((gbctxt->prev_prev_wbt == WB_ALetter ||
                        gbctxt->prev_prev_wbt == WB_Hebrew_Letter) &&
                    (wbt == WB_ALetter ||
                     wbt == WB_Hebrew_Letter)) &&
                (gbctxt->prev_wbt == WB_MidLetter ||
                 gbctxt->prev_wbt == WB_MidNumLet ||
                 gbctxt->prev_uc == 0x0027))
        {
            /* Rule WB6 */
            gbctxt->bos[gbctxt->prev_wb_index - 1] &= ~BOV_WB_WORD_BOUNDARY;
            gbctxt->is_word_boundary = 0; /* Rule WB7 */
        }
        else if (gbctxt->prev_wbt == WB_Hebrew_Letter && uc == 0x0027)
            gbctxt->is_word_boundary = 0; /* Rule WB7a */
        else if (gbctxt->prev_prev_wbt == WB_Hebrew_Letter &&
                gbctxt->prev_uc == 0x0022 &&
                wbt == WB_Hebrew_Letter) {

            /* Rule WB7b */
            gbctxt->bos[gbctxt->prev_wb_index - 1] &= ~BOV_WB_WORD_BOUNDARY;
            gbctxt->is_word_boundary = 0; /* Rule WB7c */
        }
        else if ((gbctxt->prev_prev_wbt == WB_Numeric &&
                wbt == WB_Numeric) &&
                (gbctxt->prev_wbt == WB_MidNum ||
                    gbctxt->prev_wbt == WB_MidNumLet ||
                    gbctxt->prev_uc == 0x0027)) {
            gbctxt->is_word_boundary = 0; /* Rule WB11 */

            /* Rule WB12 */
            gbctxt->bos[gbctxt->prev_wb_index - 1] &= ~BOV_WB_WORD_BOUNDARY;
        }
        else if (gbctxt->prev_wbt == WB_RI_Odd && wbt == WB_RI_Even)
            gbctxt->is_word_boundary = 0; /* Rule WB15 and WB16 */
        else
            gbctxt->is_word_boundary = 1; /* Rule WB999 */

        if (wbt != WB_ExtendFormat) {
            gbctxt->prev_prev_wbt = gbctxt->prev_wbt;
            gbctxt->prev_wbt = wbt;
            gbctxt->prev_wb_index = i;
        }
    }

    if (gbctxt->is_word_boundary)
        return BOV_WB_WORD_BOUNDARY;

    return 0;
}

#define IS_OTHER_TERM(sbt)                                      \
    /* not in (OLetter | Upper | Lower | ParaSep | SATerm) */   \
    !(sbt == SB_OLetter ||                                      \
            sbt == SB_Upper || sbt == SB_Lower ||               \
            sbt == SB_ParaSep ||                                \
            sbt == SB_ATerm || sbt == SB_STerm ||               \
            sbt == SB_ATerm_Close_Sp ||                         \
            sbt == SB_STerm_Close_Sp)

static Uint16 check_sentence_boundaries(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharGeneralCategory gc, UCharBreakType bt, int i)
{
    gbctxt->is_sentence_boundary = 0;

    /* Rules SB3 and SB5 */
    if (gbctxt->is_word_boundary || uc == '\r' || uc == '\n') {

        _SBType sbt = resolve_sbt(gbctxt, uc, gc, bt);

        /* Sentence Boundary Rules */

        /* We apply Rules SB1 and SB2 at the upper level of the function */

        if (uc == '\n' && gbctxt->prev_uc == '\r')
            gbctxt->is_sentence_boundary = 0; /* Rule SB3 */
        else if (gbctxt->prev_sbt == SB_ParaSep &&
                gbctxt->prev_sb_index + 1 == i) {
            /* The extra check for gbctxt->prev_sb_index is to correctly
             * handle sequences like
             * ParaSep ÷ Extend × Extend
             * since we have not skipped ExtendFormat yet.
             */

            gbctxt->is_sentence_boundary = 1; /* Rule SB4 */
        }
        else if (sbt == SB_ExtendFormat)
            gbctxt->is_sentence_boundary = 0; /* Rule SB5? */
        else if (gbctxt->prev_sbt == SB_ATerm && sbt == SB_Numeric)
            gbctxt->is_sentence_boundary = 0; /* Rule SB6 */
        else if ((gbctxt->prev_prev_sbt == SB_Upper ||
                    gbctxt->prev_prev_sbt == SB_Lower) &&
                gbctxt->prev_sbt == SB_ATerm &&
                sbt == SB_Upper)
            gbctxt->is_sentence_boundary = 0; /* Rule SB7 */
        else if (gbctxt->prev_sbt == SB_ATerm && sbt == SB_Close)
            sbt = SB_ATerm;
        else if (gbctxt->prev_sbt == SB_STerm && sbt == SB_Close)
            sbt = SB_STerm;
        else if (gbctxt->prev_sbt == SB_ATerm && sbt == SB_Sp)
            sbt = SB_ATerm_Close_Sp;
        else if (gbctxt->prev_sbt == SB_STerm && sbt == SB_Sp)
            sbt = SB_STerm_Close_Sp;
        /* Rule SB8 */
        else if ((gbctxt->prev_sbt == SB_ATerm ||
                gbctxt->prev_sbt == SB_ATerm_Close_Sp) &&
                sbt == SB_Lower)
            gbctxt->is_sentence_boundary = 0;
        else if ((gbctxt->prev_prev_sbt == SB_ATerm ||
                gbctxt->prev_prev_sbt == SB_ATerm_Close_Sp) &&
                IS_OTHER_TERM(gbctxt->prev_sbt) &&
                sbt == SB_Lower)
            gbctxt->bos[gbctxt->prev_sb_index - 1] &= ~BOV_SB_SENTENCE_BOUNDARY;
        else if ((gbctxt->prev_sbt == SB_ATerm ||
                    gbctxt->prev_sbt == SB_ATerm_Close_Sp ||
                    gbctxt->prev_sbt == SB_STerm ||
                    gbctxt->prev_sbt == SB_STerm_Close_Sp) &&
                (sbt == SB_SContinue ||
                    sbt == SB_ATerm || sbt == SB_STerm))
            gbctxt->is_sentence_boundary = 0; /* Rule SB8a */
        else if ((gbctxt->prev_sbt == SB_ATerm ||
                    gbctxt->prev_sbt == SB_STerm) &&
                (sbt == SB_Close || sbt == SB_Sp ||
                    sbt == SB_ParaSep))
            gbctxt->is_sentence_boundary = 0; /* Rule SB9 */
        else if ((gbctxt->prev_sbt == SB_ATerm ||
                    gbctxt->prev_sbt == SB_ATerm_Close_Sp ||
                    gbctxt->prev_sbt == SB_STerm ||
                    gbctxt->prev_sbt == SB_STerm_Close_Sp) &&
                (sbt == SB_Sp || sbt == SB_ParaSep))
            gbctxt->is_sentence_boundary = 0; /* Rule SB10 */
        else if ((gbctxt->prev_sbt == SB_ATerm ||
                    gbctxt->prev_sbt == SB_ATerm_Close_Sp ||
                    gbctxt->prev_sbt == SB_STerm ||
                    gbctxt->prev_sbt == SB_STerm_Close_Sp) &&
                sbt != SB_ParaSep)
            gbctxt->is_sentence_boundary = 1; /* Rule SB11 */
        else
            gbctxt->is_sentence_boundary = 0; /* Rule SB998 */

        if (sbt != SB_ExtendFormat &&
                !((gbctxt->prev_prev_sbt == SB_ATerm ||
                    gbctxt->prev_prev_sbt == SB_ATerm_Close_Sp) &&
                IS_OTHER_TERM(gbctxt->prev_sbt) &&
                IS_OTHER_TERM(sbt))) {
            gbctxt->prev_prev_sbt = gbctxt->prev_sbt;
            gbctxt->prev_sbt = sbt;
            gbctxt->prev_sb_index = i;
        }
    }

    if (i == 1)
        gbctxt->is_sentence_boundary = 1; /* Rules SB1 and SB2 */

    if (gbctxt->is_sentence_boundary)
        return BOV_SB_SENTENCE_BOUNDARY;

    return 0;
}

#undef IS_OTHER_TERM

/* ---- Word breaks ---- */
static void check_word_breaks(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharGeneralCategory gc, int i)
{
    /* default to not a word start/end */
    gbctxt->bos[i] &= ~BOV_WB_WORD_START;
    gbctxt->bos[i] &= ~BOV_WB_WORD_END;

    if (gbctxt->curr_wt != WordNone) {
        /* Check for a word end */
        switch (gc) {
        case UCHAR_CATEGORY_SPACING_MARK:
        case UCHAR_CATEGORY_ENCLOSING_MARK:
        case UCHAR_CATEGORY_NON_SPACING_MARK:
        case UCHAR_CATEGORY_FORMAT:
            /* nothing, we just eat these up as part of the word */
            break;

        case UCHAR_CATEGORY_LOWERCASE_LETTER:
        case UCHAR_CATEGORY_MODIFIER_LETTER:
        case UCHAR_CATEGORY_OTHER_LETTER:
        case UCHAR_CATEGORY_TITLECASE_LETTER:
        case UCHAR_CATEGORY_UPPERCASE_LETTER:
            if (gbctxt->curr_wt == WordLetters) {
                /* Japanese special cases for ending the word */
                if (JAPANESE (gbctxt->last_word_letter) ||
                        JAPANESE (uc)) {
                    if ((HIRAGANA (gbctxt->last_word_letter) &&
                                !HIRAGANA (uc)) ||
                            (KATAKANA (gbctxt->last_word_letter) &&
                             !(KATAKANA (uc) || HIRAGANA (uc))) ||
                            (KANJI (gbctxt->last_word_letter) &&
                             !(HIRAGANA (uc) || KANJI (uc))) ||
                            (JAPANESE (gbctxt->last_word_letter) &&
                             !JAPANESE (uc)) ||
                            (!JAPANESE (gbctxt->last_word_letter) &&
                             JAPANESE (uc)))
                        gbctxt->bos[i] |= BOV_WB_WORD_END;
                }
            }
            gbctxt->last_word_letter = uc;
            break;

        case UCHAR_CATEGORY_DECIMAL_NUMBER:
        case UCHAR_CATEGORY_LETTER_NUMBER:
        case UCHAR_CATEGORY_OTHER_NUMBER:
            gbctxt->last_word_letter = uc;
            break;

        default:
            /* Punctuation, control/format chars, etc. all end a word. */
            gbctxt->bos[i] |= BOV_WB_WORD_END;
            gbctxt->curr_wt = WordNone;
            break;
        }
    }
    else {
        /* Check for a word start */
        switch (gc) {
        case UCHAR_CATEGORY_LOWERCASE_LETTER:
        case UCHAR_CATEGORY_MODIFIER_LETTER:
        case UCHAR_CATEGORY_OTHER_LETTER:
        case UCHAR_CATEGORY_TITLECASE_LETTER:
        case UCHAR_CATEGORY_UPPERCASE_LETTER:
            gbctxt->curr_wt = WordLetters;
            gbctxt->last_word_letter = uc;
            gbctxt->bos[i] |= BOV_WB_WORD_START;
            break;

        case UCHAR_CATEGORY_DECIMAL_NUMBER:
        case UCHAR_CATEGORY_LETTER_NUMBER:
        case UCHAR_CATEGORY_OTHER_NUMBER:
            gbctxt->curr_wt = WordNumbers;
            gbctxt->last_word_letter = uc;
            gbctxt->bos[i] |= BOV_WB_WORD_START;
            break;

        default:
            /* No word here */
            break;
        }
    }
}

/* ---- Sentence breaks ---- */
static void check_sentence_breaks(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharGeneralCategory gc, int i)
{
    /* default to not a sentence start/end */
    gbctxt->bos[i] &= ~BOV_SB_SENTENCE_START;
    gbctxt->bos[i] &= ~BOV_SB_SENTENCE_END;

    /* maybe start sentence */
    if (gbctxt->last_stc_start == -1 && !gbctxt->is_sentence_boundary)
        gbctxt->last_stc_start = i - 1;

    /* remember last non space character position */
    if (i > 0 && !(gbctxt->bos[i - 1] & BOV_WHITESPACE))
        gbctxt->last_non_space = i;

    /* meets sentence end, mark both sentence start and end */
    if (gbctxt->last_stc_start != -1 && gbctxt->is_sentence_boundary) {
        if (gbctxt->last_non_space != -1) {
            gbctxt->bos[gbctxt->last_stc_start] |= BOV_SB_SENTENCE_START;
            gbctxt->bos[gbctxt->last_non_space] |= BOV_SB_SENTENCE_END;
        }

        gbctxt->last_stc_start = -1;
        gbctxt->last_non_space = -1;
    }

    /* meets space character, move sentence start */
    if (gbctxt->last_stc_start != -1 &&
            gbctxt->last_stc_start == i - 1 &&
            (gbctxt->bos[i - 1] & BOV_WHITESPACE))
        gbctxt->last_stc_start++;
}

#ifdef DEBUG
static void dbg_dump_gbctxt(struct glyph_break_ctxt* gbctxt,
        const char* func, Uchar32 uc, Uint16 gwsbo)
{
    _DBG_PRINTF("After calling %s (%06X):\n"
        "\tmakes_hangul_syllable: %d (prev_jamo: %d)\n"
        "\tmet_extended_pictographic: %d\n"
        "\tis_extended_pictographic: %d\n"
        "\tis_grapheme_boundary: %d\n"
        "\tis_word_boundary: %d\n"
        "\tis_sentence_boundary: %d\n"
        "\tBOV_WHITESPACE: %s\n"
        "\tBOV_EXPANDABLE_SPACE: %s\n"
        "\tBOV_GB_CHAR_BREAK: %s\n"
        "\tBOV_GB_CURSOR_POS: %s\n"
        "\tBOV_GB_BACKSPACE_DEL_CH: %s\n"
        "\tBOV_WB_WORD_BOUNDARY: %s\n"
        "\tBOV_WB_WORD_START: %s\n"
        "\tBOV_WB_WORD_END: %s\n"
        "\tBOV_SB_SENTENCE_BOUNDARY: %s\n"
        "\tBOV_SB_SENTENCE_START: %s\n"
        "\tBOV_SB_SENTENCE_END: %s\n",
        func, uc,
        gbctxt->makes_hangul_syllable, gbctxt->prev_jamo,
        gbctxt->met_extended_pictographic,
        gbctxt->is_extended_pictographic,
        gbctxt->is_grapheme_boundary,
        gbctxt->is_word_boundary,
        gbctxt->is_sentence_boundary,
        (gwsbo & BOV_WHITESPACE)?"TRUE":"FALSE",
        (gwsbo & BOV_EXPANDABLE_SPACE)?"TRUE":"FALSE",
        (gwsbo & BOV_GB_CHAR_BREAK)?"TRUE":"FALSE",
        (gwsbo & BOV_GB_CURSOR_POS)?"TRUE":"FALSE",
        (gwsbo & BOV_GB_BACKSPACE_DEL_CH)?"TRUE":"FALSE",
        (gwsbo & BOV_WB_WORD_BOUNDARY)?"TRUE":"FALSE",
        (gwsbo & BOV_WB_WORD_START)?"TRUE":"FALSE",
        (gwsbo & BOV_WB_WORD_END)?"TRUE":"FALSE",
        (gwsbo & BOV_SB_SENTENCE_BOUNDARY)?"TRUE":"FALSE",
        (gwsbo & BOV_SB_SENTENCE_START)?"TRUE":"FALSE",
        (gwsbo & BOV_SB_SENTENCE_END)?"TRUE":"FALSE");
}
#else
#define dbg_dump_gbctxt(gbctxt, func, uc, gwsbo)
#endif

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

static int gbctxt_init_spaces(struct glyph_break_ctxt* gbctxt, int size)
{
    // pre-allocate buffers
    gbctxt->len_buff = size;
    if (gbctxt->len_buff < MIN_LEN_GLYPHS)
        gbctxt->len_buff = MIN_LEN_GLYPHS;

    gbctxt->ucs = (Uchar32*)malloc(sizeof(Uchar32) * gbctxt->len_buff);
    gbctxt->bts = (Uint8*)malloc(sizeof(Uint8) * gbctxt->len_buff);
    gbctxt->bos = (Uint16*)malloc(sizeof(Uint16) * gbctxt->len_buff);
    gbctxt->ods = (Uint8*)malloc(sizeof(Uint8) * gbctxt->len_buff);
    if (gbctxt->bts == NULL || gbctxt->bos == NULL ||
            gbctxt->ods == NULL || gbctxt->ucs == NULL)
        return 0;

    return gbctxt->len_buff;
}

static int gbctxt_push_back(struct glyph_break_ctxt* gbctxt,
        Uchar32 uc, UCharBreakType bt, Uint16 lbo)
{
    /* realloc buffers if it needs */
    if ((gbctxt->n + 2) >= gbctxt->len_buff) {
        gbctxt->len_buff += INC_LEN_GLYPHS;
        gbctxt->ucs = (Uchar32*)realloc(gbctxt->ucs,
            sizeof(Uchar32) * gbctxt->len_buff);
        gbctxt->bts = (Uint8*)realloc(gbctxt->bts,
            sizeof(Uint8) * gbctxt->len_buff);
        gbctxt->bos = (Uint16*)realloc(gbctxt->bos,
            sizeof(Uint16) * gbctxt->len_buff);
        gbctxt->ods = (Uint8*)realloc(gbctxt->ods,
            sizeof(Uint8) * gbctxt->len_buff);

        if (gbctxt->bts == NULL || gbctxt->bos == NULL ||
                gbctxt->ods == NULL ||gbctxt->ucs == NULL)
            return 0;
    }

    if (gbctxt->n == 0) {
        // set the before line break opportunity
        gbctxt->bos[0] = lbo;
        gbctxt->ods[0] = LBLAST;
    }
    else {
        // break opportunities for grapheme, word, and sentence.
        UCharGeneralCategory gc;
        Uint16 gwsbo = 0;

        // set the after line break opportunity
        gbctxt->ucs[gbctxt->n - 1] = uc;
        gbctxt->bts[gbctxt->n - 1] = bt;
        gbctxt->bos[gbctxt->n] = lbo;
        if (lbo == BOV_UNKNOWN)
            gbctxt->ods[gbctxt->n] = LBLAST;
        else
            gbctxt->ods[gbctxt->n] = gbctxt->curr_od;

        // determine the grapheme, word, and sentence breaks
        gc = UCharGetCategory(uc);
        // use the original breaking class for GWS breaking test.
        bt = UCharGetBreakType(uc);

        check_hangul_syllable(gbctxt, uc, bt);
        //dbg_dump_gbctxt(gbctxt, "check_hangul_syllable", uc, gwsbo);

        gbctxt->bos[gbctxt->n] |= check_space(gbctxt, uc, gc);
        //dbg_dump_gbctxt(gbctxt, "check_space", uc, gwsbo);

        check_emoji_extended_pictographic(gbctxt, uc);
        //dbg_dump_gbctxt(gbctxt, "check_extended_pictographic", uc, gwsbo);

        gwsbo |= check_grapheme_boundaries(gbctxt, uc, gc, bt);
        //dbg_dump_gbctxt(gbctxt, "check_grapheme_boundaries", uc, gwsbo);

        gwsbo |= check_word_boundaries(gbctxt, uc, gc, bt, gbctxt->n);
        //dbg_dump_gbctxt(gbctxt, "check_word_boundaries", uc, gwsbo);

        gwsbo |= check_sentence_boundaries(gbctxt, uc, gc, bt, gbctxt->n);
        //dbg_dump_gbctxt(gbctxt, "check_sentence_boundaries", uc, gwsbo);

        gbctxt->bos[gbctxt->n - 1] |= gwsbo;

        check_word_breaks(gbctxt, uc, gc, gbctxt->n);
        //dbg_dump_gbctxt(gbctxt, "check_word_breaks", uc, gwsbo);

        check_sentence_breaks(gbctxt, uc, gc, gbctxt->n);
        dbg_dump_gbctxt(gbctxt, "check_sentence_breaks", uc, gwsbo);

        // Character Transformation
        // NOTE: Assume character transformation will not affect the breaks
        if (gbctxt->ctr && (is_uchar_letter(uc, gc, bt) || uc == 0x0020)) {
            Uchar32 new_uc = uc;

            switch(gbctxt->ctr & CTR_CASE_MASK) {
            case CTR_UPPERCASE:
                new_uc = UCharToUpper(uc);
                break;

            case CTR_LOWERCASE:
                new_uc = UCharToLower(uc);
                break;

            case CTR_CAPITALIZE:
                if (gbctxt->bos[gbctxt->n] & BOV_WB_WORD_START)
                    new_uc = UCharToUpper(uc);
                break;
            }

            if (gbctxt->ctr & CTR_FULL_WIDTH) {
                new_uc = UCharToFullWidth(new_uc);
            }

            if (gbctxt->ctr & CTR_FULL_SIZE_KANA) {
                new_uc = UCharToFullSizeKana(new_uc);
            }
        }

        gbctxt->prev_uc = uc;

        /* uc might not be a valid Unicode base character, but really all we
         * need to know is the last non-combining character */
        if (gc != UCHAR_CATEGORY_SPACING_MARK &&
                gc != UCHAR_CATEGORY_ENCLOSING_MARK &&
                gc != UCHAR_CATEGORY_NON_SPACING_MARK)
            gbctxt->base_uc = uc;
    }

    gbctxt->n++;
    return gbctxt->n;
}

static int collapse_space(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len)
{
    Uchar32 uc;
    UCharBreakType bt;
    int cosumed = 0;

    do {
        int mclen;

        mclen = get_next_uchar(gbctxt, mstr, mstr_len, &uc);
        if (mclen == 0)
            break;

        bt = resolve_lbc(gbctxt, uc);
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
        Uchar32 uc;

        mclen = get_next_uchar(gbctxt, mstr, mstr_len, &uc);
        if (mclen > 0) {
            mstr += mclen;
            mstr_len -= mclen;

            if (resolve_lbc(gbctxt, uc) == UCHAR_BREAK_SPACE) {
                cosumed += mclen;
                gbctxt_change_lbo_last(gbctxt, BOV_LB_NOTALLOWED);
                gbctxt_push_back(gbctxt, uc, UCHAR_BREAK_SPACE,
                    BOV_LB_NOTALLOWED);
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
    const char* mstr, int mstr_len, Uchar32* uc,
    UCharBreakType bt)
{
    int mclen;

    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0 && resolve_lbc(gbctxt, *uc) == bt)
        return mclen;

    return 0;
}

static int is_next_glyph_letter(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32* uc,
    UCharBreakType* pbt)
{
    int mclen;
    UCharBreakType bt;

    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        bt = resolve_lbc(gbctxt, *uc);
        if (is_uchar_letter(*uc, gbctxt->curr_gc, bt)) {
            if (pbt) *pbt = bt;
            return mclen;
        }
    }

    return 0;
}

static inline int is_next_glyph_lf(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32* uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
            UCHAR_BREAK_LINE_FEED);
}

static inline int is_next_glyph_sp(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32* uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
            UCHAR_BREAK_SPACE);
}

static inline int is_next_glyph_gl(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32* uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
        UCHAR_BREAK_NON_BREAKING_GLUE);
}

static inline int is_next_glyph_hl(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
        UCHAR_BREAK_HEBREW_LETTER);
}

static inline int is_next_glyph_in(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
        UCHAR_BREAK_INSEPARABLE);
}

static inline int is_next_glyph_nu(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
        UCHAR_BREAK_NUMERIC);
}

static inline int is_next_glyph_po(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
        UCHAR_BREAK_POSTFIX);
}

static inline int is_next_glyph_pr(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
        UCHAR_BREAK_PREFIX);
}

static inline int is_next_glyph_op(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
        UCHAR_BREAK_OPEN_PUNCTUATION);
}

static inline int is_next_glyph_jt(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
        UCHAR_BREAK_HANGUL_T_JAMO);
}

static inline int is_next_glyph_ri(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
        UCHAR_BREAK_REGIONAL_INDICATOR);
}

static inline int is_next_glyph_em(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    return is_next_glyph_bt(gbctxt, mstr, mstr_len, uc,
        UCHAR_BREAK_EMOJI_MODIFIER);
}

static int is_next_glyph_cm_zwj(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32* uc,
    UCharBreakType* pbt)
{
    int mclen;

    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = UCharGetBreakType(*uc);
        if (bt == UCHAR_BREAK_COMBINING_MARK
                || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER) {
            if (pbt) *pbt = bt;
            return mclen;
        }
    }

    return 0;
}

static int is_next_glyph_hy_ba(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc,
    UCharBreakType* pbt)
{
    int mclen;

    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
        if (bt == UCHAR_BREAK_HYPHEN
                || bt == UCHAR_BREAK_AFTER) {
            if (pbt) *pbt = bt;
            return mclen;
        }
    }

    return 0;
}

static int is_next_glyph_al_hl(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
        if (bt == UCHAR_BREAK_HEBREW_LETTER || bt == UCHAR_BREAK_ALPHABETIC)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_pr_po(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
        if (bt == UCHAR_BREAK_PREFIX || bt == UCHAR_BREAK_POSTFIX)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_id_eb_em(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
        if (bt == UCHAR_BREAK_IDEOGRAPHIC
                || bt == UCHAR_BREAK_EMOJI_BASE
                || bt == UCHAR_BREAK_EMOJI_MODIFIER)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_jl_jv_jt_h2_h3(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc,
    UCharBreakType* pbt)
{
    int mclen;
    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
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
    const char* mstr, int mstr_len, Uchar32 *uc, UCharBreakType* pbt)
{
    int mclen;
    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
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
    const char* mstr, int mstr_len, Uchar32 *uc,
    UCharBreakType* pbt)
{
    int mclen;
    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
        if (bt == UCHAR_BREAK_HANGUL_V_JAMO
                || bt == UCHAR_BREAK_HANGUL_T_JAMO) {
            if (pbt) *pbt = bt;
            return mclen;
        }
    }

    return 0;
}

static int is_next_glyph_al_hl_nu(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
        if (bt == UCHAR_BREAK_HEBREW_LETTER
                || bt == UCHAR_BREAK_ALPHABETIC
                || bt == UCHAR_BREAK_NUMERIC)
            return mclen;
    }

    return 0;
}

static int is_next_glyph_cl_cp_is_sy(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
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
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    int mclen;
    int next_mclen;
    Uchar32 next_uc;

    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
        if (bt == UCHAR_BREAK_NUMERIC) {
            return mclen;
        }
        else if ((bt == UCHAR_BREAK_OPEN_PUNCTUATION
                    || bt == UCHAR_BREAK_HYPHEN)
                && (next_mclen = is_next_glyph_nu(gbctxt,
                    mstr + mclen, mstr_len - mclen, &next_uc)) > 0) {
            return mclen + next_mclen;
        }
    }

    return 0;
}

static int is_next_glyph_nu_sy_is(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, Uchar32 *uc)
{
    int mclen;
    mclen = get_next_uchar(gbctxt, mstr, mstr_len, uc);
    if (mclen > 0) {
        UCharBreakType bt = resolve_lbc(gbctxt, *uc);
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
    Uchar32 uc;

    mclen = get_next_uchar(gbctxt, mstr, mstr_len, &uc);
    if (mclen > 0 && resolve_lbc(gbctxt, uc)
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
    Uchar32 uc;
    UCharBreakType bt;

    while ((mclen = is_next_glyph_cm_zwj(gbctxt, mstr, mstr_len,
            &uc, &bt)) > 0) {

        // CM/ZWJ should have the same break class as
        // its base character.
        gbctxt_push_back(gbctxt, uc, gbctxt->base_bt, BOV_LB_NOTALLOWED);

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
    Uchar32 uc;

    while ((mclen = is_next_glyph_sp(gbctxt, mstr, mstr_len, &uc)) > 0) {
        gbctxt_push_back(gbctxt, uc, UCHAR_BREAK_SPACE, BOV_LB_NOTALLOWED);
        mstr += mclen;
        mstr_len -= mclen;
        cosumed += mclen;
    }

    return cosumed;
}

static int is_subsequent_sps_and_end_bt(struct glyph_break_ctxt* gbctxt,
    const char* mstr, int mstr_len, UCharBreakType end_bt)
{
    Uchar32 uc;
    int mclen;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_uchar(gbctxt, mstr, mstr_len, &uc);
        if (mclen > 0) {
            UCharBreakType bt = resolve_lbc(gbctxt, uc);
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
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_uchar(gbctxt, mstr, mstr_len, &uc);
        if (mclen > 0) {
            UCharBreakType bt = resolve_lbc(gbctxt, uc);
            _DBG_PRINTF("%s: %04X (%d)\n", __FUNCTION__, uc, bt);
            if (bt == UCHAR_BREAK_SPACE) {
                mstr += mclen;
                mstr_len -= mclen;
                cosumed += mclen;
                if (!col_sp)
                    gbctxt_push_back(gbctxt, uc, bt, BOV_LB_NOTALLOWED);
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
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_uchar(gbctxt, mstr, mstr_len, &uc);
        mstr += mclen;
        mstr_len -= mclen;

        if (mclen > 0 && resolve_lbc(gbctxt, uc)
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
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_uchar(gbctxt, mstr, mstr_len, &uc);
        mstr += mclen;
        mstr_len -= mclen;

        if (mclen > 0 && resolve_lbc(gbctxt, uc)
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
    Uchar32 uc;

    while (mstr_len > 0 && *mstr != '\0') {
        mclen = get_next_uchar(gbctxt, mstr, mstr_len, &uc);
        if (mclen > 0 && resolve_lbc(gbctxt, uc)
                    == UCHAR_BREAK_REGIONAL_INDICATOR) {
            mstr += mclen;
            mstr_len -= mclen;
            cosumed += mclen;

            gbctxt_push_back(gbctxt, uc,
                UCHAR_BREAK_REGIONAL_INDICATOR, BOV_LB_NOTALLOWED);
            continue;
        }
        else
            break;
    }

    return cosumed;
}

int GUIAPI GetUCharsAndBreaks(LOGFONT* logfont, const char* mstr, int mstr_len,
            LanguageCode content_language, UCharScriptType writing_system,
            Uint8 wsr, Uint8 ctr, Uint8 wbr, Uint8 lbp,
            Uchar32** uchars, Uint16** break_oppos, int* nr_ucs)
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

    memset(&gbctxt, 0, sizeof(gbctxt));
    gbctxt.lf = logfont;
    gbctxt.mbc_devfont = logfont->devfonts[1];
    gbctxt.sbc_devfont = logfont->devfonts[0];
#if 0
    gbctxt.ucs = NULL;
    gbctxt.bts = NULL;
    gbctxt.bos = NULL;
    gbctxt.len_buff = 0;
    gbctxt.n = 0;
#endif
    gbctxt.base_bt = UCHAR_BREAK_UNSET;
    gbctxt.cl = content_language;
    gbctxt.ws = writing_system;
    gbctxt.wsr = wsr;
    gbctxt.ctr = ctr;
    gbctxt.wbr = wbr;
    gbctxt.lbp = lbp;

#if 0
    gbctxt.prev_uc  = 0;
    gbctxt.base_uc  = 0;
    gbctxt.last_word_letter = 0;
#endif

    // NOTE: the index 0 of break_oppos is the break opportunity
    // before the first glyph.
    gbctxt.last_stc_start = -1;
    gbctxt.last_non_space = -1;
    gbctxt.prev_wb_index = -1;
    gbctxt.prev_sb_index = -1;

    gbctxt.prev_gbt = GB_Other;
    gbctxt.prev_gbt = GB_Other;
    gbctxt.prev_wbt = gbctxt.prev_prev_wbt = WB_Other;
    gbctxt.prev_sbt = gbctxt.prev_prev_sbt = SB_Other;
    gbctxt.prev_jamo = NO_JAMO;
    gbctxt.curr_wt = WordNone;

#if 0
    gbctxt.makes_hangul_syllable = 0;
    gbctxt.is_grapheme_boundary = 0;
    gbctxt.is_word_boundary = 0;
    gbctxt.is_sentence_boundary = 0;
    gbctxt.met_extended_pictographic = 0;
    gbctxt.is_extended_pictographic = 0;
#endif

    *uchars = NULL;
    *break_oppos = NULL;
    *nr_ucs = 0;

    if (mstr_len == 0)
        return 0;

    if (gbctxt_init_spaces(&gbctxt, mstr_len >> 1) <= 0) {
        goto error;
    }

    while (TRUE) {
        Uchar32 uc;
        int mclen = 0;
        UCharBreakType bt;
        UCharGeneralCategory gc;
        // line break opportunity
        Uint16 lbo;

        Uchar32 next_uc;
        UCharBreakType next_bt;
        int next_mclen;
        int cosumed_one_loop = 0;

        mclen = get_next_uchar(&gbctxt, mstr, mstr_len, &uc);
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
        }

        /*
         * UNICODE LINE BREAKING ALGORITHM
         */

        // LB1 Resolve line breaking class
        gbctxt.curr_od = LB1;
        bt = resolve_lbc(&gbctxt, uc);
        gc = gbctxt.curr_gc;

        /* Start and end of text */
        // LB2 Never break at the start of text.
        if (gbctxt.n == 0) {
            _DBG_PRINTF ("LB2 Never break at the start of text\n");
            gbctxt.curr_od = LB2;
            if (gbctxt_push_back(&gbctxt, 0, 0, BOV_LB_NOTALLOWED) == 0)
                goto error;
        }

        // Only break at forced line breaks.
        if (wsr == WSR_PRE || wsr == WSR_NOWRAP) {
            // Set the default breaking manner is not allowed.
            gbctxt.curr_od = LBPRE;
            lbo = BOV_LB_NOTALLOWED;
        }
        else {
            // Set the default breaking manner is not set.
            gbctxt.curr_od = LBLAST;
            lbo = BOV_UNKNOWN;
        }

        // Set default break opportunity of the current glyph
        if (gbctxt_push_back(&gbctxt, uc, bt, lbo) == 0)
            goto error;

        // LB3 Always break at the end of text.
        if (get_next_uchar(&gbctxt, mstr, mstr_len, &next_uc) == 0) {
            _DBG_PRINTF ("LB3 Always break at the end of text\n");
            gbctxt.curr_od = LB3;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_MANDATORY);
        }

        /* Mandatory breaks */
        // LB4 Always break after hard line breaks
        // LB6 Do not break before hard line breaks
        if (bt == UCHAR_BREAK_MANDATORY) {
            _DBG_PRINTF ("LB4 Always break after hard line breaks\n");
            gbctxt.curr_od = LB4;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_MANDATORY);
            _DBG_PRINTF ("LB6 Do not break before hard line breaks\n");
            gbctxt.curr_od = LB6;
            gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);
        }
        // LB5 Treat CR followed by LF, as well as CR, LF,
        // and NL as hard line breaks.
        // LB6 Do not break before hard line breaks.
        else if (bt == UCHAR_BREAK_CARRIAGE_RETURN
                && (next_mclen = is_next_glyph_lf(&gbctxt,
                    mstr, mstr_len, &next_uc)) > 0) {
            cosumed_one_loop += next_mclen;

            _DBG_PRINTF ("LB5 Treat CR followed by LF, as well as CR, LF, and NL as hard line breaks.\n");
            gbctxt.curr_od = LB5;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            _DBG_PRINTF ("LB6 Do not break before hard line breaks\n");
            gbctxt.curr_od = LB6;
            gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);

            if (col_nl)
                // Collapse new lines
                lbo = BOV_LB_NOTALLOWED;
            else
                lbo = BOV_LB_MANDATORY;
            if (gbctxt_push_back(&gbctxt, next_uc,
                    UCHAR_BREAK_LINE_FEED, lbo) == 0)
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
                lbo = BOV_LB_NOTALLOWED;
            else
                lbo = BOV_LB_MANDATORY;
            _DBG_PRINTF ("LB5 Treat CR followed by LF, as well as CR, LF, and NL as hard line breaks.\n");
            gbctxt.curr_od = LB5;
            gbctxt_change_lbo_last(&gbctxt, lbo);
            _DBG_PRINTF ("LB6 Do not break before hard line breaks\n");
            gbctxt.curr_od = LB6;
            gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);
        }

        /* Explicit breaks and non-breaks */
        // LB7 Do not break before spaces or zero width space.
        else if (bt == UCHAR_BREAK_SPACE
                || bt == UCHAR_BREAK_ZERO_WIDTH_SPACE) {

            _DBG_PRINTF ("LB7 Do not break before spaces or zero width space\n");
            gbctxt.curr_od = LB7;
            gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);

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
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_ALLOWED);
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
                    mstr, mstr_len, &next_uc) > 0) {

            _DBG_PRINTF ("LB8a Do not break between a zero width joiner and ID, EB, EM\n");
            gbctxt.curr_od = LB8a;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
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
                    mstr, mstr_len, &next_uc, NULL) > 0) {

            _DBG_PRINTF ("LB9 Do not break a combining character sequence\n");
            gbctxt.curr_od = LB9;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);

            // LB10 Treat any remaining combining mark or ZWJ as AL.
            if ((bt == UCHAR_BREAK_COMBINING_MARK
                    || bt == UCHAR_BREAK_ZERO_WIDTH_JOINER)) {
                bt = UCHAR_BREAK_ALPHABETIC;
            }

            gbctxt.base_bt = bt;
            cosumed_one_loop += check_subsequent_cm_zwj(&gbctxt, mstr, mstr_len);
            gbctxt_change_lbo_last(&gbctxt, BOV_UNKNOWN);

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
            lbo = BOV_LB_NOTALLOWED;
            _DBG_PRINTF ("LB11 Do not break before or after Word joiner and ...\n");
            gbctxt.curr_od = LB11;
            gbctxt_change_lbo_last(&gbctxt, lbo);
            gbctxt_change_lbo_before_last(&gbctxt, lbo);
        }
        // LB12 Do not break after NBSP and related characters.
        else if (bt == UCHAR_BREAK_NON_BREAKING_GLUE) {
            _DBG_PRINTF ("LB12 Do not break after NBSP and related characters\n");
            gbctxt.curr_od = LB12;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
        }

        /* Breaking is forbidden within “words”: implicit soft wrap
         * opportunities between typographic letter units (or other
         * typographic character units belonging to the NU, AL, AI, or ID
         * Unicode line breaking classes) are suppressed, i.e. breaks are
         * prohibited between pairs of such characters (regardless of
         * line-break settings other than anywhere) except where opportunities
         * exist due to dictionary-based breaking.
         */
        if (wbr == WBR_KEEP_ALL && is_uchar_letter(uc, gc, bt)
                && (next_mclen = is_next_glyph_letter(&gbctxt,
                    mstr, mstr_len, &next_uc, &next_bt)) > 0) {
            _DBG_PRINTF ("WBR_KEEP_ALL.\n");
            gbctxt.curr_od = LB12a;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            if (gbctxt_push_back(&gbctxt, next_uc, next_bt,
                    BOV_UNKNOWN) == 0)
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
                    mstr, mstr_len, &next_uc) > 0) {
            _DBG_PRINTF ("LB12a Do not break before NBSP and related characters, except after SP and HY\n");
            gbctxt.curr_od = LB12a;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
        }

        /* Opening and closing */
        if (lbp == LBP_LOOSE) {
            // LB13 for LBP_LOOSE: Do not break before ‘!’, even after spaces.
            if (bt == UCHAR_BREAK_EXCLAMATION) {
                _DBG_PRINTF ("LB13 for LBP_LOOSE: Do not break before ‘!’, even after spaces.\n");
                gbctxt.curr_od = LB13;
                gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
        }
        else if (lbp == LBP_NORMAL) {
            // LB13 for LBP_NORMAL
            if (bt != UCHAR_BREAK_NUMERIC
                && is_next_glyph_cl_cp_is_sy(&gbctxt,
                    mstr, mstr_len, &next_uc) > 0) {

                _DBG_PRINTF ("LB13 for LBP_NORMAL: Do not break between non-number and ‘]’ or ‘;’ or ‘/’.\n");
                gbctxt.curr_od = LB13;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }

            if (bt == UCHAR_BREAK_EXCLAMATION) {
                _DBG_PRINTF ("LB13 for LBP_NORMAL: Do not break before ‘!’, even after spaces\n");
                gbctxt.curr_od = LB13;
                gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);
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
                gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
        }

        // LB14 Do not break after ‘[’, even after spaces.
        if (bt == UCHAR_BREAK_OPEN_PUNCTUATION) {
            _DBG_PRINTF ("LB14 Do not break after ‘[’, even after spaces\n");
            gbctxt.curr_od = LB14;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);

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
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);

            _DBG_PRINTF ("LB19 Do not break before or after quotation marks, such as ‘ ” ’\n");
            gbctxt.curr_od = LB19;
            gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);

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
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);

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
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);

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
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_ALLOWED);
        }
        /* Special case rules */
        // LB19 Do not break before or after quotation marks, such as ‘ ” ’.
        else if (bt == UCHAR_BREAK_QUOTATION) {
            _DBG_PRINTF ("LB19 Do not break before or after quotation marks, such as ‘ ” ’\n");
            gbctxt.curr_od = LB19;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);
        }
        // LB20 Break before and after unresolved CB.
        else if (bt == UCHAR_BREAK_CONTINGENT) {
            _DBG_PRINTF ("LB20 Break before and after unresolved CB.\n");
            gbctxt.curr_od = LB20;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_ALLOWED);
            gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_ALLOWED);
        }
        // LB21 Do not break before hyphen-minus, other hyphens,
        // fixed-width spaces, small kana, and other non-starters,
        // or after acute accents.
        else if (bt == UCHAR_BREAK_AFTER
                || bt == UCHAR_BREAK_HYPHEN
                || bt == UCHAR_BREAK_NON_STARTER) {
            _DBG_PRINTF ("LB21.1 Do not break before hyphen-minus, other hyphens...\n");
            gbctxt.curr_od = LB21;
            gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);
        }
        else if (bt == UCHAR_BREAK_BEFORE) {
            _DBG_PRINTF ("LB21.2 Do not break before hyphen-minus, other hyphens...\n");
            gbctxt.curr_od = LB21;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
        }
        // LB21a Don't break after Hebrew + Hyphen.
        else if (bt == UCHAR_BREAK_HEBREW_LETTER
                && (next_mclen = is_next_glyph_hy_ba(&gbctxt,
                    mstr, mstr_len, &next_uc, &next_bt)) > 0) {

            _DBG_PRINTF ("LB21a Don't break after Hebrew + Hyphen\n");
            gbctxt.curr_od = LB21a;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            if (gbctxt_push_back(&gbctxt,
                    next_uc, next_bt, BOV_LB_NOTALLOWED) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        // LB21b Don’t break between Solidus and Hebrew letters.
        else if (bt == UCHAR_BREAK_SYMBOL
                && is_next_glyph_hl(&gbctxt,
                    mstr, mstr_len, &next_uc) > 0) {
            _DBG_PRINTF ("LB21b Don’t break between Solidus and Hebrew letters\n");
            gbctxt.curr_od = LB21b;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
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
                    mstr, mstr_len, &next_uc) > 0) {

            _DBG_PRINTF ("LB22 Do not break between two ellipses, or between letters...\n");
            gbctxt.curr_od = LB22;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
        }

        /* Numbers */
        // LB23 Do not break between digits and letters.
        if (lbp != LBP_LOOSE) {
            if ((bt == UCHAR_BREAK_HEBREW_LETTER
                        || bt == UCHAR_BREAK_ALPHABETIC)
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB23 Do not break between digits and letters\n");
                gbctxt.curr_od = LB23;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_al_hl(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB23 Do not break between digits and letters\n");
                gbctxt.curr_od = LB23;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            // LB23a Do not break between numeric prefixes and ideographs,
            // or between ideographs and numeric postfixes.
            else if (bt == UCHAR_BREAK_PREFIX
                    && is_next_glyph_id_eb_em(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB23a.1 Do not break between numeric prefixes and ID...\n");
                gbctxt.curr_od = LB23a;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if ((bt == UCHAR_BREAK_IDEOGRAPHIC
                        || bt == UCHAR_BREAK_EMOJI_BASE
                        || bt == UCHAR_BREAK_EMOJI_MODIFIER)
                    && is_next_glyph_po(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB23a.2 Do not break between numeric prefixes and ID...\n");
                gbctxt.curr_od = LB23a;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            // LB24 Do not break between numeric prefix/postfix and letters,
            // or between letters and prefix/postfix.
            else if ((bt == UCHAR_BREAK_PREFIX
                        || bt == UCHAR_BREAK_POSTFIX)
                    && is_next_glyph_al_hl(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB24 Do not break between numeric prefix/postfix and letters\n");
                gbctxt.curr_od = LB24;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if ((bt == UCHAR_BREAK_ALPHABETIC
                        || bt == UCHAR_BREAK_HEBREW_LETTER)
                    && is_next_glyph_pr_po(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB24 Do not break between numeric prefix/postfix and letters\n");
                gbctxt.curr_od = LB24;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
        }

        // LB25 Do not break between the following pairs of classes
        // relevant to numbers
        if (lbp == LBP_LOOSE) {
            if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_po(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.5 for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_pr(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.6 for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_POSTFIX
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                gbctxt.curr_od = LB25;
                _DBG_PRINTF ("LB25.8 for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_PREFIX
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.a for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_HYPHEN
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.b for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.d for LBP_LOOSE: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
        }
        else if (lbp == LBP_NORMAL) {
            if ((bt == UCHAR_BREAK_PREFIX || bt == UCHAR_BREAK_POSTFIX)
                    && is_next_glyph_nu_OR_op_hy_followed_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.1 for LBP_NORMAL: (PR | PO) × ( OP | HY )? NU.\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if ((bt == UCHAR_BREAK_OPEN_PUNCTUATION
                        || bt == UCHAR_BREAK_HYPHEN)
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.2 for LBP_NORMAL: ( OP | HY ) × NU.\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_nu_sy_is(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.3 for LBP_NORMAL: NU × (NU | SY | IS).\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }

            if ((bt == UCHAR_BREAK_NUMERIC
                    || bt == UCHAR_BREAK_SYMBOL
                    || bt == UCHAR_BREAK_INFIX_SEPARATOR
                    || bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                    || bt == UCHAR_BREAK_CLOSE_PARANTHESIS)
                    && are_prev_glyphs_nu_AND_nu_sy_is(&gbctxt, FALSE)) {
                _DBG_PRINTF ("LB25.4 for LBP_NORMAL: NU (NU | SY | IS)* × (NU | SY | IS | CL | CP ).\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            if ((bt == UCHAR_BREAK_POSTFIX || bt == UCHAR_BREAK_PREFIX)
                    && are_prev_glyphs_nu_AND_nu_sy_is_AND_cl_cp(&gbctxt)) {
                _DBG_PRINTF ("LB25.5 for LBP_NORMAL: NU (NU | SY | IS)* (CL | CP)? × (PO | PR).\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_before_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
        }
        else {
            if (bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                    && is_next_glyph_po(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.1 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_CLOSE_PUNCTUATION
                    && is_next_glyph_pr(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.2 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                    && is_next_glyph_po(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                gbctxt.curr_od = LB25;
                _DBG_PRINTF ("LB25.3 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                    && is_next_glyph_pr(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.4 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_po(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.5 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_pr(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.6 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_POSTFIX
                    && is_next_glyph_op(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                gbctxt.curr_od = LB25;
                _DBG_PRINTF ("LB25.7 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_POSTFIX
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                gbctxt.curr_od = LB25;
                _DBG_PRINTF ("LB25.8 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_PREFIX
                    && is_next_glyph_op(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                gbctxt.curr_od = LB25;
                _DBG_PRINTF ("LB25.9 for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_PREFIX
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.a for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_HYPHEN
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.b for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_INFIX_SEPARATOR
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.c for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_NUMERIC
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.d for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
            else if (bt == UCHAR_BREAK_SYMBOL
                    && is_next_glyph_nu(&gbctxt,
                        mstr, mstr_len, &next_uc) > 0) {
                _DBG_PRINTF ("LB25.e for LBP_STRICT: Do not break between the certain pairs of classes\n");
                gbctxt.curr_od = LB25;
                gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            }
        }

        /* Korean syllable blocks */
        // LB26 Do not break a Korean syllable.
        if (bt == UCHAR_BREAK_HANGUL_L_JAMO
                && (next_mclen = is_next_glyph_jl_jv_h2_h3(&gbctxt,
                    mstr, mstr_len, &next_uc, &next_bt)) > 0) {
            _DBG_PRINTF ("LB26.1 Do not break a Korean syllable.\n");
            gbctxt.curr_od = LB26;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            if (gbctxt_push_back(&gbctxt, next_uc, next_bt,
                    BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        else if ((bt == UCHAR_BREAK_HANGUL_V_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE)
                && (next_mclen = is_next_glyph_jv_jt(&gbctxt,
                    mstr, mstr_len, &next_uc, &next_bt)) > 0) {
            _DBG_PRINTF ("LB26.2 Do not break a Korean syllable.\n");
            gbctxt.curr_od = LB26;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            if (gbctxt_push_back(&gbctxt, next_uc, next_bt,
                    BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        else if ((bt == UCHAR_BREAK_HANGUL_T_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE)
                && (next_mclen = is_next_glyph_jt(&gbctxt,
                    mstr, mstr_len, &next_uc)) > 0) {
            _DBG_PRINTF ("LB26.3 Do not break a Korean syllable.\n");
            gbctxt.curr_od = LB26;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            if (gbctxt_push_back(&gbctxt, next_uc,
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
                    mstr, mstr_len, &next_uc)) > 0) {
            _DBG_PRINTF ("LB27.1 Treat a Korean Syllable Block the same as ID.\n");
            gbctxt.curr_od = LB27;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            if (gbctxt_push_back(&gbctxt, next_uc,
                    UCHAR_BREAK_INSEPARABLE, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        else if ((bt == UCHAR_BREAK_HANGUL_L_JAMO
                    || bt == UCHAR_BREAK_HANGUL_V_JAMO
                    || bt == UCHAR_BREAK_HANGUL_T_JAMO
                    || bt == UCHAR_BREAK_HANGUL_LV_SYLLABLE
                    || bt == UCHAR_BREAK_HANGUL_LVT_SYLLABLE)
                && (next_mclen = is_next_glyph_po(&gbctxt,
                    mstr, mstr_len, &next_uc)) > 0) {
            _DBG_PRINTF ("LB27.2 Treat a Korean Syllable Block the same as ID.\n");
            gbctxt.curr_od = LB27;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            if (gbctxt_push_back(&gbctxt, next_uc,
                    UCHAR_BREAK_POSTFIX, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }
        else if (bt == UCHAR_BREAK_PREFIX
                && (next_mclen = is_next_glyph_jl_jv_jt_h2_h3(&gbctxt,
                    mstr, mstr_len, &next_uc, &next_bt)) > 0) {
            _DBG_PRINTF ("LB27.3 Treat a Korean Syllable Block the same as ID.\n");
            gbctxt.curr_od = LB27;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            if (gbctxt_push_back(&gbctxt, next_uc,
                    next_bt, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }

        /* Finally, join alphabetic letters into words
           and break everything else. */

        // LB28 Do not break between alphabetics (“at”).
        else if ((bt == UCHAR_BREAK_HEBREW_LETTER
                    || bt == UCHAR_BREAK_ALPHABETIC)
                && is_next_glyph_al_hl(&gbctxt,
                    mstr, mstr_len, &next_uc) > 0) {
            _DBG_PRINTF ("LB28 Do not break between alphabetics (“at”)\n");
            gbctxt.curr_od = LB28;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
        }

        // LB29 Do not break between numeric punctuation
        // and alphabetics (“e.g.”).
        else if (bt == UCHAR_BREAK_INFIX_SEPARATOR
                && is_next_glyph_al_hl(&gbctxt,
                    mstr, mstr_len, &next_uc) > 0) {
            _DBG_PRINTF ("LB29 Do not break between numeric punctuation\n");
            gbctxt.curr_od = LB29;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
        }

        // LB30 Do not break between letters, numbers, or ordinary symbols and
        // opening or closing parentheses.
        else if ((bt == UCHAR_BREAK_HEBREW_LETTER
                    || bt == UCHAR_BREAK_ALPHABETIC
                    || bt == UCHAR_BREAK_NUMERIC)
                && is_next_glyph_op(&gbctxt,
                    mstr, mstr_len, &next_uc) > 0) {
            _DBG_PRINTF ("LB30.1 Do not break between letters, numbers...\n");
            gbctxt.curr_od = LB30;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
        }
        else if (bt == UCHAR_BREAK_CLOSE_PARANTHESIS
                && is_next_glyph_al_hl_nu(&gbctxt,
                    mstr, mstr_len, &next_uc) > 0) {
            _DBG_PRINTF ("LB30.2 Do not break between letters, numbers...\n");
            gbctxt.curr_od = LB30;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
        }

        // LB30a Break between two regional indicator symbols if and only if
        // there are an even number of regional indicators preceding the
        // position of the break.
        else if (bt == UCHAR_BREAK_REGIONAL_INDICATOR
                && (next_mclen = is_next_glyph_ri(&gbctxt,
                    mstr, mstr_len, &next_uc)) > 0) {
            _DBG_PRINTF ("LB30a.1 Break between two regional indicator symbols...\n");
            gbctxt.curr_od = LB30a;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            if (gbctxt_push_back(&gbctxt, next_uc,
                    UCHAR_BREAK_REGIONAL_INDICATOR, BOV_UNKNOWN) == 0)
                goto error;
            cosumed_one_loop += next_mclen;
        }
        else if (bt != UCHAR_BREAK_REGIONAL_INDICATOR
                && (next_mclen = is_even_nubmer_of_subsequent_ri(&gbctxt,
                    mstr, mstr_len)) > 0) {

            _DBG_PRINTF ("LB30a.2 Break between two regional indicator symbols...\n");
            gbctxt.curr_od = LB30a;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            next_mclen = check_subsequent_ri(&gbctxt,
                mstr, mstr_len);
            gbctxt_change_lbo_last(&gbctxt, BOV_UNKNOWN);

            cosumed_one_loop += next_mclen;
        }

        // LB30b Do not break between an emoji base and an emoji modifier.
        else if (bt == UCHAR_BREAK_EMOJI_BASE
                && (next_mclen = is_next_glyph_em(&gbctxt,
                    mstr, mstr_len, &next_uc)) > 0) {
            _DBG_PRINTF ("LB30b Do not break between an emoji base and an emoji modifier\n");
            gbctxt.curr_od = LB30b;
            gbctxt_change_lbo_last(&gbctxt, BOV_LB_NOTALLOWED);
            if (gbctxt_push_back(&gbctxt, next_uc,
                    UCHAR_BREAK_EMOJI_MODIFIER, BOV_UNKNOWN) == 0)
                goto error;

            cosumed_one_loop += next_mclen;
        }

        gbctxt.base_bt = UCHAR_BREAK_UNSET;

next_glyph:
        mstr_len -= cosumed_one_loop;
        mstr += cosumed_one_loop;
        cosumed += cosumed_one_loop;

        // Return if we got any BK!
        if ((gbctxt.bos[gbctxt.n] & BOV_LB_MASK) == BOV_LB_MANDATORY) {
            break;
        }
    }

    if (gbctxt.n > 0) {
        // Rule GB1
        gbctxt.bos[0] |= BOV_GB_CHAR_BREAK | BOV_GB_CURSOR_POS;
        // Rule WB1
        gbctxt.bos[0] |= BOV_WB_WORD_BOUNDARY;
        // Ruel SB1
        gbctxt.bos[0] |= BOV_SB_SENTENCE_BOUNDARY;
        // Rule GB2
        gbctxt.bos[gbctxt.n - 1] |= BOV_GB_CHAR_BREAK | BOV_GB_CURSOR_POS;
        // Rule WB2
        gbctxt.bos[gbctxt.n - 1] |= BOV_WB_WORD_BOUNDARY;
        // Rule SB2
        gbctxt.bos[gbctxt.n - 1] |= BOV_SB_SENTENCE_BOUNDARY;

        // LB31 Break everywhere else.
        int n;
        for (n = 1; n < gbctxt.n; n++) {
            if ((gbctxt.bos[n] & BOV_LB_MASK) == BOV_UNKNOWN) {
                _DBG_PRINTF ("LB31 Break everywhere else: %d\n", n);
                gbctxt.bos[n] &= ~BOV_LB_MASK;
                gbctxt.bos[n] |= BOV_LB_ALLOWED;
            }
        }

        *break_oppos = gbctxt.bos;
        if (uchars)
            *uchars = gbctxt.ucs;
        *nr_ucs = gbctxt.n - 1;
    }
    else
        goto error;

    if (uchars == NULL && gbctxt.ucs) free(gbctxt.ucs);
    if (gbctxt.ods) free(gbctxt.ods);

    return cosumed;

error:
    if (gbctxt.ucs) free(gbctxt.ucs);
    if (gbctxt.bts) free(gbctxt.bts);
    if (gbctxt.bos) free(gbctxt.bos);
    if (gbctxt.ods) free(gbctxt.ods);
    return 0;
}

// internal use
typedef struct _MYGLYPHINFO {
    Uchar32 uc;
    Uint8 gc;
    Uint8 suppressed:1;
    Uint8 whitespace:1;
    Uint8 orientation:2;
    Uint8 hanged:2;
    Uint8 justify_word:1;
    Uint8 justify_char:1;
} MYGLYPHINFO;

typedef struct _MYGLYPHARGS {
    PLOGFONT lfur;
    PLOGFONT lfsw;
    const Uchar32* ucs;
    const Uint16* bos;
    Glyph32* gvs;
    int nr_ucs;
    Uint32 rf;

    int lw;
    int hanged_start;
    int hanged_end;
} MYGLYPHARGS;

#if 0
static LOGFONT* create_sideways_logfont(LOGFONT* logfont_upright)
{
    LOGFONT tmp;

    memcpy(&tmp, logfont_upright, sizeof(LOGFONT));
    tmp.rotation = -900;
    return CreateLogFontIndirect(&tmp);
}
#endif

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

    DEVFONT* devfont = SELECT_DEVFONT_BY_GLYPH(logfont, gv);
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

static void normalize_glyph_metrics(LOGFONT* logfont,
        Uint32 render_flags, const BBOX* bbox,
        int* adv_x, int* adv_y, int* line_adv, int* line_width)
{
    switch (render_flags & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        if (logfont->rotation == -900) {
            *line_adv = *adv_y;
            if (logfont->size > *line_width)
                *line_width = logfont->size;
        }
        else {
            if (*adv_x > *line_width)
                *line_width = *adv_x;

            *adv_x = 0;
            *adv_y = logfont->size;
            *line_adv = logfont->size;
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
        GLYPHEXTINFO* ges)
{
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        ges->extra_x = 0;
        ges->extra_y = extra_spacing;
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        ges->extra_x = extra_spacing;
        ges->extra_y = 0;
        break;
    }
}

static void increase_extra_spacing(MYGLYPHARGS* args, int extra_spacing,
        GLYPHEXTINFO* ges)
{
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        ges->extra_y += extra_spacing;
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        ges->extra_x += extra_spacing;
        break;
    }
}

static int find_breaking_pos_normal(MYGLYPHARGS* args, int n)
{
    int i;

    for (i = n - 1; i >= 0; i--) {
        if ((args->bos[i] & BOV_LB_MASK) == BOV_LB_ALLOWED)
            return i;
    }

    return -1;
}

static int find_breaking_pos_any(MYGLYPHARGS* args, int n)
{
    int i;

    for (i = n - 1; i >= 0; i--) {
        if (args->bos[i] & BOV_GB_CHAR_BREAK)
            return i;
    }

    return -1;
}

static int find_breaking_pos_word(MYGLYPHARGS* args, int n)
{
    int i;

    for (i = n - 1; i >= 0; i--) {
        if (args->bos[i] & BOV_WB_WORD_BOUNDARY)
            return i;
    }

    return -1;
}

static inline BOOL is_whitespace_glyph(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int i)
{
    return (args->bos[i] & BOV_WHITESPACE);
}

static inline BOOL is_zero_width_glyph(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int i)
{
    return (args->bos[i] & BOV_ZERO_WIDTH);
}

static inline BOOL is_word_separator(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int i)
{
#if 0
    return (args->bos[i] & BOV_WB_WORD_BOUNDARY);
#else
    return (
        gis[i].uc == 0x0020 || gis[i].uc == 0x00A0 ||
        gis[i].uc == 0x1361 ||
        gis[i].uc == 0x10100 || gis[i].uc == 0x10101 ||
        gis[i].uc == 0x1039F || gis[i].uc == 0x1091F
    );
#endif
}

/*
 * TODO: scripts and spacing:
 * https://www.w3.org/TR/css-text-3/#script-groups
 */
static inline BOOL is_typographic_char(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int i)
{
    return (args->bos[i - 1] & BOV_GB_CHAR_BREAK);
}

static void justify_glyphs_inter_word(MYGLYPHARGS* args,
        MYGLYPHINFO* gis, GLYPHEXTINFO* ges, int n, int error)
{
    int i;
    int nr_words = 0;
    int err_per_unit;
    int left;

    for (i = 0; i < n; i++) {
        if (gis[i].suppressed == 0 && gis[i].hanged == 0 &&
                is_word_separator(args, gis, i)) {
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
                increase_extra_spacing(args, err_per_unit, ges + i);
                nr_words--;
            }

            i++;
        } while (nr_words > 0);
    }

    if (left > 0) {
        i = 0;
        do {
            if (gis[i].justify_word) {
                increase_extra_spacing(args, 1, ges + i);
                if (--left == 0)
                    break;
            }

            i++;
        } while (1);
    }
}

static void justify_glyphs_inter_char(MYGLYPHARGS* args,
        MYGLYPHINFO* gis, GLYPHEXTINFO* ges, int n, int error)
{
    int i;
    int nr_chars = 0;
    int err_per_unit;
    int left;

    for (i = 0; i < n; i++) {
        if (gis[i].suppressed == 0 && gis[i].hanged == 0 &&
                !is_word_separator(args, gis, i) &&
                is_typographic_char(args, gis, i)) {
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
                increase_extra_spacing(args, err_per_unit, ges + i);
                nr_chars--;
            }

            i++;
        } while (nr_chars > 0);
    }

    if (left > 0) {
        for (i = 0; i < n; i++) {
            if (gis[i].justify_char) {
                increase_extra_spacing(args, 1, ges + i);
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
        MYGLYPHINFO* gis, GLYPHEXTINFO* ges, int n, int error)
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
        if (gis[i].suppressed == 0 && gis[i].hanged == 0) {
            if ((is_word_separator(args, gis, i) && i != 0) ||
                    IsUCharWideCJK(gis[i].uc)) {
                nr_words++;
                gis[i].justify_word = 1;
            }
            else if (is_typographic_char(args, gis, i)) {
                nr_chars++;
                gis[i].justify_char = 1;
            }
        }
    }

    _DBG_PRINTF("%s: nr_words(%d), nr_chars(%d)\n",
        __FUNCTION__, nr_words, nr_chars);

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
                    increase_extra_spacing(args, err_per_unit, ges + i);
                    compensated += err_per_unit;
                    nr_words--;
                }

                i++;
            } while (nr_words > 0);
        }

        if (nr_chars <= 0 && left > 0) {
            for (i = 0; i < n; i++) {
                if (gis[i].justify_word) {
                    increase_extra_spacing(args, 1, ges + i);
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
                    increase_extra_spacing(args, err_per_unit, ges + i);
                    nr_chars--;
                }

                i++;
            } while (nr_chars > 0);
        }

        if (left > 0) {
            for (i = 0; i < n; i++) {
                if (gis[i].justify_char) {
                    increase_extra_spacing(args, 1, ges + i);
                    if (--left == 0)
                        break;
                }
            }
        }
    }
}

static void adjust_glyph_position(MYGLYPHARGS* args,
        int x, int y, const MYGLYPHINFO* gi, const GLYPHEXTINFO* ge,
        GLYPHPOS* pos)
{
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
        if (gi->orientation == GLYPH_ORIENTATION_UPRIGHT) {
            x -= (args->lw + ge->bbox_w) / 2;
            x -= ge->bbox_x;
        }
        break;

    case GRF_WRITING_MODE_VERTICAL_LR:
        if (gi->orientation == GLYPH_ORIENTATION_SIDEWAYS) {
            x += args->lfsw->size;
        }
        else {
            x += (args->lw - ge->bbox_w) / 2;
            x -= ge->bbox_x;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        break;
    }

    pos->x += x;
    pos->y += y;
}

static void calc_unhanged_glyph_positions(MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, GLYPHEXTINFO* ges, int n,
        int x, int y, GLYPHPOS* pos)
{
    int i;
    int first = 0, stop = n;

    if (args->hanged_start >= 0)
        first = args->hanged_start + 1;
    if (args->hanged_end < n)
        stop = args->hanged_end;

    for (i = first; i < stop; i++) {
        if (i == first) {
            pos[i].x = 0;
            pos[i].y = 0;
        }
        else {
            pos[i].x = pos[i - 1].x + ges[i - 1].adv_x;
            pos[i].y = pos[i - 1].y + ges[i - 1].adv_y;
            pos[i].x += ges[i - 1].extra_x;
            pos[i].y += ges[i - 1].extra_y;
        }
    }

    for (i = first; i < stop; i++) {
        adjust_glyph_position(args, x, y, gis + i, ges + i, pos + i);

        ges[i].suppressed = gis[i].suppressed;
        ges[i].whitespace = gis[i].whitespace;
        ges[i].orientation = gis[i].orientation;

        pos[i].suppressed = gis[i].suppressed;
        pos[i].whitespace = gis[i].whitespace;
        pos[i].orientation = gis[i].orientation;
        pos[i].hanged = gis[i].hanged;
    }
}

static int calc_hanged_glyphs_extent(MYGLYPHARGS* args,
        const GLYPHEXTINFO* ges, int n)
{
    int i;
    int hanged_extent = 0;

    if (args->hanged_start >= 0) {
        for (i = 0; i <= args->hanged_start; i++) {
            hanged_extent += ges[i].line_adv;
        }
    }

    if (args->hanged_end < n) {
        for (i = args->hanged_end; i < n; i++) {
            hanged_extent += ges[i].line_adv;
        }
    }

    _DBG_PRINTF("%s: hanged_start(%d) hanged_end(%d) n(%d) hanged_extent(%d)\n",
        __FUNCTION__, args->hanged_start, args->hanged_end, n, hanged_extent);

    return hanged_extent;
}

static int calc_hanged_glyphs_start(MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, GLYPHEXTINFO* ges,
        GLYPHPOS* pos, int n, int x, int y)
{
    int i;
    int hanged_extent = 0;

    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        for (i = 0; i <= args->hanged_start; i++) {
            hanged_extent += ges[i].line_adv;
        }

        for (i = 0; i <= args->hanged_start; i++) {
            if (i == 0) {
                pos[i].x = 0;
                pos[i].y = -hanged_extent;
            }
            else {
                pos[i].x = pos[i - 1].x + ges[i - 1].adv_x;
                pos[i].y = pos[i - 1].y + ges[i - 1].adv_y;
                pos[i].x += ges[i - 1].extra_x;
                pos[i].y += ges[i - 1].extra_y;
            }
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        for (i = 0; i <= args->hanged_start; i++) {
            hanged_extent += ges[i].line_adv;
        }

        for (i = 0; i <= args->hanged_start; i++) {
            if (i == 0) {
                pos[i].x = -hanged_extent;
                pos[i].y = 0;
            }
            else {
                pos[i].x = pos[i - 1].x + ges[i - 1].adv_x;
                pos[i].y = pos[i - 1].y + ges[i - 1].adv_y;
                pos[i].x += ges[i - 1].extra_x;
                pos[i].y += ges[i - 1].extra_y;
            }
        }
        break;
    }

    for (i = 0; i <= args->hanged_start; i++) {
        adjust_glyph_position(args, x, y, gis + i, ges + i, pos + i);

        ges[i].suppressed = gis[i].suppressed;
        ges[i].whitespace = gis[i].whitespace;
        ges[i].orientation = gis[i].orientation;

        pos[i].suppressed = gis[i].suppressed;
        pos[i].whitespace = gis[i].whitespace;
        pos[i].orientation = gis[i].orientation;
        pos[i].hanged = gis[i].hanged;
    }

    return hanged_extent;
}

static int calc_hanged_glyphs_end(MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, GLYPHEXTINFO* ges,
        GLYPHPOS* pos, int n, int x, int y, int extent)
{
    int i;
    int hanged_extent = 0;

    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        for (i = args->hanged_end; i < n; i++) {
            if (i == args->hanged_end) {
                pos[i].x = 0;
                pos[i].y = extent;
            }
            else {
                pos[i].x = pos[i - 1].x + ges[i - 1].adv_x;
                pos[i].y = pos[i - 1].y + ges[i - 1].adv_y;
                pos[i].x += ges[i - 1].extra_x;
                pos[i].y += ges[i - 1].extra_y;
            }

            hanged_extent += ges[i].line_adv;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        for (i = args->hanged_end; i < n; i++) {
            if (i == args->hanged_end) {
                pos[i].x = extent;
                pos[i].y = 0;
            }
            else {
                pos[i].x = pos[i - 1].x + ges[i - 1].adv_x;
                pos[i].y = pos[i - 1].y + ges[i - 1].adv_y;
                pos[i].x += ges[i - 1].extra_x;
                pos[i].y += ges[i - 1].extra_y;
            }

            hanged_extent += ges[i].line_adv;
        }
        break;
    }

    for (i = args->hanged_end; i < n; i++) {
        adjust_glyph_position(args, x, y, gis + i, ges + i, pos + i);

        ges[i].suppressed = gis[i].suppressed;
        ges[i].whitespace = gis[i].whitespace;
        ges[i].orientation = gis[i].orientation;

        pos[i].suppressed = gis[i].suppressed;
        pos[i].whitespace = gis[i].whitespace;
        pos[i].orientation = gis[i].orientation;
        pos[i].hanged = gis[i].hanged;
    }

    _DBG_PRINTF("%s: hanged_start(%d) hanged_end(%d) n(%d) hanged_extent(%d)\n",
        __FUNCTION__, args->hanged_start, args->hanged_end, n, hanged_extent);

    return hanged_extent;
}

static void offset_unhanged_glyph_positions(MYGLYPHARGS* args,
        GLYPHPOS* pos, int n, int offset)
{
    int i;
    int first = 0, stop = n;

    if (args->hanged_start >= 0)
        first = args->hanged_start + 1;
    if (args->hanged_end < n)
        stop = args->hanged_end;

    _DBG_PRINTF("%s: offset(%d), first(%d), stop(%d)\n",
            __FUNCTION__, offset, first, stop);

    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        for (i = first; i < stop; i++) {
            pos[i].y += offset;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        for (i = first; i < stop; i++) {
            pos[i].x += offset;
        }
        break;
    }
}

static void align_unhanged_glyphs(MYGLYPHARGS* args,
        GLYPHPOS* pos, int n, int gap)
{
    _DBG_PRINTF("%s: args->rf(0x%08X), gap(%d)\n",
            __FUNCTION__, args->rf, gap);
    switch (args->rf & GRF_ALIGN_MASK) {
    case GRF_ALIGN_RIGHT:
    case GRF_ALIGN_END:
        offset_unhanged_glyph_positions(args, pos, n, gap);
        break;

    case GRF_ALIGN_CENTER:
        offset_unhanged_glyph_positions(args, pos, n, gap/2);
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
        gi->gc == UCHAR_CATEGORY_OPEN_PUNCTUATION ||
        gi->gc == UCHAR_CATEGORY_FINAL_PUNCTUATION ||
        gi->gc == UCHAR_CATEGORY_INITIAL_PUNCTUATION ||
        gi->uc == 0x0027 ||
        gi->uc == 0x0022
    );
}

static inline BOOL is_closing_punctation(const MYGLYPHINFO* gi)
{
    return (
        gi->gc == UCHAR_CATEGORY_CLOSE_PUNCTUATION ||
        gi->gc == UCHAR_CATEGORY_FINAL_PUNCTUATION ||
        gi->gc == UCHAR_CATEGORY_INITIAL_PUNCTUATION ||
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
    args->gvs[i] = GetGlyphValue(args->lfur, SET_MBCHV(args->ucs[i]));

    gi->uc = args->ucs[i];
    gi->gc = UCharGetCategory(gi->uc);
    gi->whitespace = 0;
    gi->suppressed = 0;
    gi->hanged = GLYPH_HANGED_NONE;
    gi->orientation = GLYPH_ORIENTATION_UPRIGHT;
}

static inline int shrink_total_extent(MYGLYPHARGS* args, int total_extent,
        const GLYPHEXTINFO* ges)
{
#if 0
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        total_extent -= ges->adv_y;
        total_extent -= ges->extra_y;
        break;
    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        total_extent -= ges->adv_x;
        total_extent -= ges->extra_x;
        break;
    }

    return total_extent;
#else
    return total_extent - ges->line_adv;
#endif

}

static int get_glyph_extent_info(MYGLYPHARGS* args, Glyph32 gv,
        MYGLYPHINFO* gi, GLYPHEXTINFO* ges)
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
                gi->orientation = GLYPH_ORIENTATION_SIDEWAYS;
            }
        }
        else if ((args->rf & GRF_TEXT_ORIENTATION_MASK)
               == GRF_TEXT_ORIENTATION_SIDEWAYS) {
            logfont = args->lfsw;
            gi->orientation = GLYPH_ORIENTATION_SIDEWAYS;
        }
    }

    font_get_glyph_metrics(logfont, gv, &adv_x, &adv_y, &bbox);
    normalize_glyph_metrics(logfont, args->rf, &bbox,
            &adv_x, &adv_y, &line_adv, &args->lw);

    ges->bbox_x = bbox.x;
    ges->bbox_y = bbox.y;
    ges->bbox_w = bbox.w;
    ges->bbox_h = bbox.h;
    ges->adv_x = adv_x;
    ges->adv_y = adv_y;

    return line_adv;
}

static int get_first_normal_glyph(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        if (gis[i].suppressed == 0 && gis[i].hanged == 0
                && (args->bos[i - 1] & BOV_GB_CHAR_BREAK))
            return i;
    }

    return -1;
}

static int get_last_normal_glyph(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int n)
{
    int i;
    for (i = n - 1; i >= 0; i--) {
        if (gis[i].suppressed == 0 && gis[i].hanged == 0
                && (args->bos[i - 1] & BOV_GB_CHAR_BREAK))
            return i;
    }

    return -1;
}

static inline BOOL is_logfont_unicode(LOGFONT* lf)
{
    DEVFONT* df = lf->devfonts[1];
    if (df && df->charset_ops->conv_to_uc32 == NULL)
        return TRUE;

    return FALSE;
}

int GUIAPI GetGlyphsExtentFromUChars(LOGFONT* logfont_upright,
        const Uchar32* ucs, int nr_ucs,
        const Uint16* break_oppos,
        Uint32 render_flags, int x, int y,
        int letter_spacing, int word_spacing, int tab_size, int max_extent,
        SIZE* line_size, Glyph32* glyphs, GLYPHEXTINFO* glyph_ext_info,
        GLYPHPOS* glyph_pos, LOGFONT** logfont_sideways)
{
    int n = 0;
    int total_extent = 0;
    int breaking_pos = -1;
    int gap;

    MYGLYPHARGS  args;
    MYGLYPHINFO* gis = NULL;
    GLYPHEXTINFO* ges = NULL;
    BOOL test_overflow = TRUE;
    BOOL lfsw_created = FALSE;

    /* Check logfont_upright and create logfont_sideways if need */
    if (logfont_upright == NULL || logfont_upright->rotation != 0)
        return 0;

    if (!is_logfont_unicode(logfont_upright) ||
            (*logfont_sideways != NULL &&
                !is_logfont_unicode(*logfont_sideways))) {
        _ERR_PRINTF("%s: the logfonts are not in Unicode charset.\n",
            __FUNCTION__);
        return 0;
    }

    if (*logfont_sideways == NULL) {
        if ((render_flags & GRF_WRITING_MODE_MASK)
                != GRF_WRITING_MODE_HORIZONTAL_TB) {
            switch (render_flags & GRF_TEXT_ORIENTATION_MASK) {
            case GRF_TEXT_ORIENTATION_MIXED:
            case GRF_TEXT_ORIENTATION_SIDEWAYS:
                *logfont_sideways
                    = CreateLogFontIndirectEx(logfont_upright, -900);
                if (*logfont_sideways == NULL
                        || (*logfont_sideways)->rotation != -900)
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
        if ((*logfont_sideways)->rotation != -900) {
            goto error;
        }
    }

    if (glyph_ext_info == NULL) {
        ges = (GLYPHEXTINFO*)calloc(sizeof(GLYPHEXTINFO), nr_ucs);
        if (ges == NULL)
            goto error;
    }
    else {
        ges = glyph_ext_info;
        memset(ges, 0, sizeof(GLYPHEXTINFO) * nr_ucs);
    }

    gis = (MYGLYPHINFO*)calloc(sizeof(MYGLYPHINFO), nr_ucs);
    if (gis == NULL)
        goto error;

    args.lfur = logfont_upright;
    args.lfsw = *logfont_sideways;
    args.ucs = ucs;
    args.gvs = glyphs;
    args.bos = break_oppos;
    args.nr_ucs = nr_ucs;
    args.rf = render_flags;
    args.lw = args.lfur->size;
    if (args.lfsw) {
        args.lw = MAX(args.lw, args.lfsw->size);
    }

    while (n < nr_ucs) {
        int extra_spacing;

        init_glyph_info(&args, n, gis + n);

        /*
         * NOTE: The collapsible spaces should be handled in GetGlyphsByRules.
         */
        if (gis[n].uc == UCHAR_TAB) {
            if (tab_size > 0) {
                int tabstops = total_extent / tab_size + 1;
                ges[n].line_adv = tabstops * tab_size- total_extent;

                // If this distance is less than 0.5ch, then the
                // subsequent tab stop is used instead.
                if (ges[n].line_adv < logfont_upright->size / 6) {
                    tabstops++;
                    ges[n].line_adv = tabstops * tab_size- total_extent;
                }

                switch (render_flags & GRF_WRITING_MODE_MASK) {
                case GRF_WRITING_MODE_VERTICAL_RL:
                case GRF_WRITING_MODE_VERTICAL_LR:
                    ges[n].adv_y = ges[n].line_adv;
                    break;
                case GRF_WRITING_MODE_HORIZONTAL_TB:
                default:
                    ges[n].adv_x = ges[n].line_adv;
                    break;
                }

                gis[n].whitespace = 1;
            }
            else {
                gis[n].suppressed = 1;
                ges[n].line_adv = 0;
            }
        }
        else if (is_whitespace_glyph(&args, gis, n)) {
            gis[n].whitespace = 1;
            ges[n].line_adv = logfont_upright->size / 6;
            switch (render_flags & GRF_WRITING_MODE_MASK) {
            case GRF_WRITING_MODE_VERTICAL_RL:
            case GRF_WRITING_MODE_VERTICAL_LR:
                ges[n].adv_y = ges[n].line_adv;
                break;
            case GRF_WRITING_MODE_HORIZONTAL_TB:
            default:
                ges[n].adv_x = ges[n].line_adv;
                break;
            }
        }
        else if (is_zero_width_glyph(&args, gis, n)) {
            gis[n].suppressed = 1;
            ges[n].line_adv = 0;
        }
        else {
            ges[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    ges + n);
        }

        // extra space for word and letter
        extra_spacing = 0;
        if (gis[n].suppressed == 0 && is_word_separator(&args, gis, n)) {
            extra_spacing = word_spacing;
        }
        else if (gis[n].suppressed == 0 && is_typographic_char(&args, gis, n)) {
            extra_spacing = letter_spacing;
        }

        if (extra_spacing > 0) {
            ges[n].line_adv += extra_spacing;
            set_extra_spacing(&args, extra_spacing, ges + n);
        }

        if (test_overflow && max_extent > 0
                && (total_extent + ges[n].line_adv) > max_extent) {
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

        total_extent += ges[n].line_adv;
        if ((break_oppos[n] & BOV_LB_MASK) == BOV_LB_MANDATORY) {
            // hard line breaking
            n++;
            break;
        }

        if (!test_overflow && max_extent > 0
                && ((break_oppos[n] & BOV_LB_MASK) == BOV_LB_ALLOWED)) {
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
            total_extent += ges[i].line_adv;
        }
    }

    args.hanged_start = -1;
    args.hanged_end = n + 1;

    // Trimming spaces at the start of the line
    if (render_flags & GRF_SPACES_REMOVE_START) {
        int i = 0;
        while (i < n && gis[i].uc == UCHAR_SPACE) {
            total_extent -= ges[i].line_adv;
            memset(ges + i, 0, sizeof(GLYPHEXTINFO));
            gis[i].suppressed = 1;
            i++;
        }
    }

    // Trimming or hanging spaces at the end of the line
    if (render_flags & GRF_SPACES_REMOVE_END) {
        int i = n - 1;
        while (i > 0 &&
                (gis[i].uc == UCHAR_SPACE || gis[i].uc == UCHAR_IDSPACE)) {
            total_extent -= ges[i].line_adv;
            memset(ges + i, 0, sizeof(GLYPHEXTINFO));
            gis[i].suppressed = 1;
            i--;
        }
    }
    else if (render_flags & GRF_SPACES_HANGE_END) {
        int i = n - 1;
        while (i > 0 &&
                (gis[i].uc == UCHAR_SPACE || gis[i].uc == UCHAR_IDSPACE)) {

            gis[i].hanged = GLYPH_HANGED_END;
            if (i < args.hanged_end) args.hanged_end = i;
            i--;
        }
    }

    if (n < nr_ucs) {
        init_glyph_info(&args, n, gis + n);
    }

    if (render_flags & GRF_HANGING_PUNC_OPEN) {
        int first = get_first_normal_glyph(&args, gis, n);
        if (first >= 0 && is_opening_punctation(gis + first)) {
            gis[first].hanged = GLYPH_HANGED_START;
            if (first > args.hanged_start) args.hanged_start = first;
        }
    }

    if (n > 1 && render_flags & GRF_HANGING_PUNC_CLOSE) {
        int last = get_last_normal_glyph(&args, gis, n);
        if (last > 0 && is_closing_punctation(gis + last)) {
            gis[last].hanged = GLYPH_HANGED_END;
            if (last < args.hanged_end) args.hanged_end = last;
        }
#if 0
        else if (n < nr_ucs && is_closing_punctation(gis + n)) {
            gis[n].hanged = GLYPH_HANGED_END;
            if (n < args.hanged_end) args.hanged_end = n;
            ges[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    ges + n);
            total_extent += ges[n].line_adv;
            n++;
        }
#endif
    }

    if (render_flags & GRF_HANGING_PUNC_FORCE_END) {
        // A stop or comma at the end of a line hangs.
        int last = get_last_normal_glyph(&args, gis, n);
        if (last > 0 && is_stop_or_common(gis + last)) {
            gis[last].hanged = GLYPH_HANGED_END;
            if (last < args.hanged_end) args.hanged_end = last;
        }
#if 0
        else if (n < nr_ucs && is_stop_or_common(gis + n)) {
            gis[n].hanged = GLYPH_HANGED_END;
            if (n < args.hanged_end) args.hanged_end = n;
            ges[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    ges + n);
            total_extent += ges[n].line_adv;
            n++;
        }
#endif
    }
    else if (render_flags & GRF_HANGING_PUNC_ALLOW_END) {
        // A stop or comma at the end of a line hangs
        // if it does not otherwise fit prior to justification.
        if (n < nr_ucs && is_stop_or_common(gis + n)) {
            gis[n].hanged = GLYPH_HANGED_END;
            if (n < args.hanged_end) args.hanged_end = n;
            ges[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    ges + n);
            total_extent += ges[n].line_adv;
            n++;
        }
    }

    total_extent -= calc_hanged_glyphs_extent(&args, ges, n);

    // calc positions of hanged glyphs
    if (args.hanged_start >= 0) {
        calc_hanged_glyphs_start(&args, gis, ges,
                glyph_pos, n, x, y);
    }

    if (args.hanged_end < n) {
        if (max_extent > 0) {
            calc_hanged_glyphs_end(&args, gis, ges,
                        glyph_pos, n, x, y, MAX(max_extent, total_extent));
        }
        else {
            calc_hanged_glyphs_end(&args, gis, ges,
                    glyph_pos, n, x, y, total_extent);
        }
    }

    gap = max_extent - total_extent;
    // justify the unhanged glyphs
    if ((render_flags & GRF_ALIGN_MASK) == GRF_ALIGN_JUSTIFY
            && gap > 0) {
        switch (render_flags & GRF_TEXT_JUSTIFY_MASK) {
        case GRF_TEXT_JUSTIFY_INTER_WORD:
            justify_glyphs_inter_word(&args, gis, ges, n, gap);
            break;
        case GRF_TEXT_JUSTIFY_INTER_CHAR:
            justify_glyphs_inter_char(&args, gis, ges, n, gap);
            break;
        case GRF_TEXT_JUSTIFY_AUTO:
        default:
            justify_glyphs_auto(&args, gis, ges, n, gap);
            break;
        }

    }

    // calcualte unhanged glyph positions according to the base point
    calc_unhanged_glyph_positions(&args, gis, ges, n, x, y, glyph_pos);

    // align unhanged glyphs
    align_unhanged_glyphs(&args, glyph_pos, n, gap);

    if (line_size) {
        if ((render_flags & GRF_WRITING_MODE_MASK)
                == GRF_WRITING_MODE_HORIZONTAL_TB) {
            line_size->cx = glyph_pos[n - 1].x - glyph_pos[0].x
                + ges[n - 1].adv_x + ges[n - 1].extra_x;
            line_size->cy = args.lw;
        }
        else {
            line_size->cy = glyph_pos[n - 1].y - glyph_pos[0].y
                + ges[n - 1].adv_y + ges[n - 1].extra_y;
            line_size->cx = args.lw;
        }
    }

    free(gis);
    if (glyph_ext_info == NULL)
        free(ges);

    return n;

error:
    if (ges) free(ges);
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
        const Glyph32* glyphs, const GLYPHPOS* glyph_pos,
        int nr_glyphs)
{
    int i;
    int n = 0;
    Uint32 old_ta;
    PLOGFONT old_lf;

    if (glyphs == NULL || glyph_pos == NULL || nr_glyphs <= 0)
        return 0;

    old_ta = SetTextAlign(hdc, TA_LEFT | TA_TOP | TA_UPDATECP);
    old_lf = GetCurFont(hdc);

    for (i = 0; i < nr_glyphs; i++) {
        if (glyph_pos[i].suppressed == 0 && glyph_pos[i].whitespace == 0) {
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

