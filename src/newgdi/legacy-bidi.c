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
** bidi.c
**
** Current maintainer: houhuihua.
**
** Create date: 2008/02/01
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "devfont.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "drawtext.h"
#include "glyph.h"
#include "bidi.h"
#include "cursor.h"

static Achar32* _gdi_get_achars_string(PDC pdc, const unsigned char* text,
        int text_len, int* nr_achars)
{
    int i = 0;
    int len_cur_char;
    int left_bytes = text_len;
    unsigned int char_type;
    Achar32  chv;
    DEVFONT* sbc_devfont = pdc->pLogFont->devfonts[0];
    DEVFONT* mbc_devfont = pdc->pLogFont->devfonts[1];
    Achar32 *logical_achars = NULL;

    if (mbc_devfont) {
        int prev_len = 0;
        const unsigned char* prev_mchar = NULL;
        logical_achars = malloc(text_len * sizeof (Achar32));
        while (left_bytes > 0) {
            len_cur_char = mbc_devfont->charset_ops->len_first_char
                ((const unsigned char*)text, left_bytes);
            if (len_cur_char > 0) {
                chv = mbc_devfont->charset_ops->get_char_value
                    (prev_mchar, prev_len, text, left_bytes);

                logical_achars[i++] = SET_MBCHV(chv);
                char_type = (*mbc_devfont->charset_ops->char_type)
                    (chv);
                if (!(char_type & ACHAR_BASIC_NOSPACING_MARK)){
                    prev_mchar = text;
                    prev_len = len_cur_char;
                }
                else {
                    prev_len += len_cur_char;
                }

                left_bytes -= len_cur_char;
                text += len_cur_char;
                continue;
            }

            len_cur_char = sbc_devfont->charset_ops->len_first_char
                ((const unsigned char*)text, left_bytes);

            if (len_cur_char > 0) {
                chv = sbc_devfont->charset_ops->get_char_value
                    (NULL, 0, text, left_bytes);
                logical_achars[i++] = chv;
                text += len_cur_char;
                left_bytes -= len_cur_char;
            }
            else
                break;
        }

        if (nr_achars)
            *nr_achars = i;
    }

    return logical_achars;
}

static unsigned int inline get_char_type(LOGFONT* logfont, Achar32 chv)
{
    if (IS_MBCHV (chv))
        return logfont->devfonts[1]->charset_ops->char_type(chv);
    else
        return logfont->devfonts[0]->charset_ops->char_type(chv);
}

static int inline get_glyph_advance_in_dc (PDC pdc, Glyph32 gv)
{
    return _gdi_get_glyph_advance(pdc, gv, TRUE, 0, 0, NULL, NULL, NULL);
}

static int jump_vowels_ltr (PDC pdc, Achar32* achs, int nr_achs,
        Achar32* biggest_vowel)
{
    int i = 0;
    int max_advance = 0;
    int cur_advance = 0;

    for (i = 0; i < nr_achs; i++) {
        Glyph32 gv;

        if (!check_vowel(get_char_type (pdc->pLogFont, achs[i])))
            return i;

        if ((gv = GetGlyphValueAlt(pdc->pLogFont, achs[i])) == INV_GLYPH_VALUE) {
            _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                __FUNCTION__, achs[i]);
            return i;
        }
        cur_advance = get_glyph_advance_in_dc (pdc, gv);
        if (cur_advance > max_advance) {
            max_advance = cur_advance;
            *biggest_vowel = achs [i];
        }
    }

    return nr_achs;
}

static int output_visual_achars_ltr (PDC pdc, Achar32* visual_achars,
    int nr_achars, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    int i = 0;
    int j = 0;
    unsigned int char_type;
    Achar32 chv;
    Achar32 biggest_vowel = 0;
    Glyph32 gv;

    int vowel_num;
    int outed_num = 0;

    int old_text_align = pdc->ta_flags;
    int bkmode = pdc->bkmode;

    while (i < nr_achars) {
        /*jump vowels*/
        vowel_num = jump_vowels_ltr (pdc, visual_achars,
                nr_achars-i, &biggest_vowel);

        visual_achars += vowel_num;

        /*output the letter after vowels,
         * the letter is the owner of jumped vowels
         * if there is a letter on the vowel's right*/
        if (vowel_num < nr_achars - i) {

            chv = *(visual_achars);
            char_type = get_char_type (pdc->pLogFont, chv);
            if ((gv = GetGlyphValueAlt(pdc->pLogFont, chv)) == INV_GLYPH_VALUE) {
                _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                    __FUNCTION__, chv);
                goto end;
            }

            if (!cb_one_glyph (context, gv, char_type)) {
                goto end;
            }

            outed_num++;

            /*set vowel aligns width letter's right*/
            pdc->ta_flags = (old_text_align & ~TA_LEFT) | TA_RIGHT;
            pdc->bkmode = BM_TRANSPARENT;
        }

        /*first output the biggest vowel*/
        if (vowel_num) {

            char_type = get_char_type (pdc->pLogFont, biggest_vowel);
            gv = GetGlyphValueAlt(pdc->pLogFont, biggest_vowel);
            if ((gv = GetGlyphValueAlt(pdc->pLogFont, biggest_vowel)) ==
                    INV_GLYPH_VALUE) {
                _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                    __FUNCTION__, biggest_vowel);
                goto end;
            }

            if (!cb_one_glyph (context, gv, char_type)){
                goto end;
            }

            outed_num++;
            /*transparent from other vowels*/
            pdc->bkmode = BM_TRANSPARENT;
        }

        /*output other vowels from visual_achars[i]*/
        for (j = 1; j <= vowel_num; j++) {

            chv = *(visual_achars - j);
            if (chv != biggest_vowel) {
                char_type = get_char_type (pdc->pLogFont, chv);
                if ((gv = GetGlyphValueAlt(pdc->pLogFont, chv)) ==
                        INV_GLYPH_VALUE) {
                    _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                        __FUNCTION__, chv);
                    goto end ;
                }

                if (!cb_one_glyph (context, gv, char_type)){
                    goto end;
                }
                outed_num++;
            }
        }

        pdc->ta_flags = old_text_align;
        pdc->bkmode = bkmode;

        i += vowel_num + 1;
        visual_achars ++;
    }

end:
    pdc->bkmode = bkmode;
    return outed_num;
}

static int output_vowels_rtl (PDC pdc, Achar32* end_achar, int left_num,
     CB_ONE_GLYPH cb_one_glyph, void* context, int* outed_num)
{
    int i = 0;
    Achar32 chv;
    unsigned int char_type;
    int old_text_align = pdc->ta_flags;
    int old_bkmode = pdc->bkmode;

    pdc->ta_flags = (old_text_align & ~TA_RIGHT) | TA_LEFT;
    pdc->bkmode = BM_TRANSPARENT;
    *outed_num = 0;
    for (i = 0; i < left_num; i++) {
        chv = *end_achar--;
        char_type = get_char_type (pdc->pLogFont, chv);

        if (check_vowel(char_type)) {
            Glyph32 gv;

            if ((gv = GetGlyphValueAlt(pdc->pLogFont, chv)) == INV_GLYPH_VALUE) {
                _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                    __FUNCTION__, chv);
                break;
            }

            if (cb_one_glyph(context, gv, char_type)) {
                (*outed_num) ++;
            }
            else
                break;
        }
        else
            break;
    }

    pdc->ta_flags = old_text_align;
    pdc->bkmode = old_bkmode;
    return i;
}

static int output_unowned_vowels_rtl (PDC pdc, Achar32* end_achar, int left_num,
     CB_ONE_GLYPH cb_one_glyph, void* context, int* outed_num)
{
    int i = 0;
    Achar32 chv = INV_ACHAR_VALUE;
    Achar32 biggest_vowel = INV_ACHAR_VALUE;
    unsigned int char_type;
    int cur_advance = 0;
    int max_advance = 0;
    int bkmode = pdc->bkmode;
    int old_text_align = pdc->ta_flags;
    Glyph32 gv;

    *outed_num = 0;
    if (!check_vowel(get_char_type (pdc->pLogFont, *end_achar)))
        return 0;

    pdc->ta_flags = (old_text_align & ~TA_RIGHT) | TA_LEFT;
    for (i = 0; i < left_num; i++) {
        chv = end_achar [-i];
        char_type = get_char_type (pdc->pLogFont, chv);

        if (!check_vowel(char_type))
            break;

        if ((gv = GetGlyphValueAlt(pdc->pLogFont, chv)) ==
                INV_GLYPH_VALUE) {
            _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                __FUNCTION__, chv);
            break;
        }

        cur_advance = get_glyph_advance_in_dc (pdc, gv);
        if (cur_advance > max_advance) {
            max_advance = cur_advance;
            biggest_vowel = chv;
        }
    }

    /*output the biggest_vowel*/
    if (max_advance) {
        char_type = get_char_type (pdc->pLogFont, biggest_vowel);
        if ((gv = GetGlyphValueAlt(pdc->pLogFont, biggest_vowel)) ==
                INV_GLYPH_VALUE) {
            _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                __FUNCTION__, biggest_vowel);
        }
        else if (cb_one_glyph(context, gv, char_type)) {
            (*outed_num) ++;
        }
    }

    pdc->bkmode = BM_TRANSPARENT;

    left_num = i;
    for (i = 0; i < left_num; i++) {
        chv = *end_achar--;
        if (chv != biggest_vowel) {
            Glyph32 gv;

            char_type = get_char_type (pdc->pLogFont, chv);
            if ((gv = GetGlyphValueAlt(pdc->pLogFont, chv)) == INV_GLYPH_VALUE) {
                _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                    __FUNCTION__, chv);
                break;
            }
            if (cb_one_glyph(context, gv, char_type)) {
                (*outed_num) ++;
            }
        }
    }

    pdc->bkmode = bkmode;
    pdc->ta_flags = old_text_align;
    return left_num;
}

static int output_visual_achars_rtl (PDC pdc, Achar32* visual_achars,
    int nr_achars, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    int cur = nr_achars - 1;
    unsigned int char_type;
    Achar32 chv;
    int vowel_num;
    int outed_vowel_num;
    int outed_glyph_num = 0;

    /*out the vowels on the rightest*/
    vowel_num = output_unowned_vowels_rtl (pdc, visual_achars+cur, cur+1,
            cb_one_glyph, context, &outed_vowel_num);
    if (outed_vowel_num < vowel_num)
        return outed_vowel_num;

    cur -= vowel_num;
    outed_glyph_num = outed_vowel_num;

    while (cur >= 0) {
        Glyph32 gv;

        chv = visual_achars[cur];
        char_type = get_char_type (pdc->pLogFont, chv);
        if ((gv = GetGlyphValueAlt(pdc->pLogFont, chv)) == INV_GLYPH_VALUE) {
            _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                __FUNCTION__, chv);
            return outed_glyph_num;
        }

        /*output the letter*/
        if (!cb_one_glyph(context, gv, char_type)){
            return outed_glyph_num;
        }
        outed_glyph_num ++;

        /*output the vowels of current letter, align with the letter's left*/
        vowel_num = output_vowels_rtl (pdc, visual_achars+cur-1, cur,
                cb_one_glyph, context, &outed_vowel_num);
        outed_glyph_num += outed_vowel_num;

        if (outed_vowel_num < vowel_num)
            return outed_glyph_num;

        cur -= vowel_num + 1;
    }

    return outed_glyph_num;
}

int _gdi_output_visual_achars(PDC pdc, Achar32* visual_achars,
    int nr_achars, BOOL direction, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    /* call cb_one_glyph to show text. */
    if (!direction) { /* right to left.*/
        return  output_visual_achars_rtl (pdc, visual_achars,
                nr_achars, cb_one_glyph, context);
    }
    else {
        return  output_visual_achars_ltr (pdc, visual_achars,
                nr_achars, cb_one_glyph, context);
    }

}

static int _gdi_output_achars_direct(PDC pdc, const unsigned char* text,
        int text_len, CB_ONE_GLYPH cb_one_glyph, void* context,
        BOOL if_break, BOOL if_draw)
{
    unsigned int char_type;
    Achar32  chv;
    Glyph32  gv;
    DEVFONT* sbc_devfont = pdc->pLogFont->devfonts[0];
    DEVFONT* mbc_devfont = pdc->pLogFont->devfonts[1];
    int len_cur_char = 0;
    int left_bytes = text_len;
    int prev_len = 0;
    const unsigned char* prev_mchar = NULL;
    int i = 0;
    int line_width = 0;
    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;

#if 0
    int limit_width = -1;
    if(if_break){
        limit_width = ctxt->max_extent;
    }
#endif

    while (left_bytes > 0){
        if (mbc_devfont) {
            len_cur_char = mbc_devfont->charset_ops->len_first_char
                ((const unsigned char*)text, left_bytes);
            if (len_cur_char > 0) {
                chv = mbc_devfont->charset_ops->get_char_value
                    (prev_mchar, prev_len, text, left_bytes);

                chv = SET_MBCHV(chv);
                goto do_glyph;
            }
        }

        len_cur_char = sbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)text, left_bytes);

        if (len_cur_char > 0) {
            chv = sbc_devfont->charset_ops->get_char_value
                (NULL, 0, text, left_bytes);
        }
        else
            break;

do_glyph:
        if (IS_MBCHV (chv))
            char_type = mbc_devfont->charset_ops->char_type (chv);
        else
            char_type = sbc_devfont->charset_ops->char_type (chv);

        gv = GetGlyphValueAlt(pdc->pLogFont, chv);
        if (gv == INV_GLYPH_VALUE) {
            continue;
        }

        if (if_break) {
            DRAWTEXTEX2_CTXT _txt;
            _txt.pdc         = pdc;
            _txt.x           = 0;
            _txt.y           = 0;
            _txt.advance     = 0;
            _txt.nFormat     = ctxt->nFormat;
            _txt.only_extent = TRUE;
            _txt.tab_width   = ctxt->tab_width;
            if (!cb_one_glyph(&_txt, gv, char_type)) {
                break;
            }
            line_width += _txt.advance;
            if (line_width > ctxt->max_extent) {
                break;
            }
        }

        if (if_draw) {
            if (!cb_one_glyph(ctxt, gv, char_type)){
                break;
            }
        }

        i++;
        text += len_cur_char;
        left_bytes -= len_cur_char;
        if(if_break){
            ctxt->nCount = text_len - left_bytes;
        }
    }

    return i;
}

Achar32* _gdi_bidi_reorder (PDC pdc, const unsigned char* text, int text_len,
        int* nr_achars)
{
    DEVFONT* mbc_devfont = pdc->pLogFont->devfonts[1];
    Achar32 *logical_achars = NULL;

    if (mbc_devfont && mbc_devfont->charset_ops->bidi_char_type) {
        logical_achars = _gdi_get_achars_string (pdc, text, text_len, nr_achars);
        if (*nr_achars > 0)
            __mg_legacy_bidi_achars_reorder (mbc_devfont->charset_ops,
                logical_achars, *nr_achars, -1, NULL, NULL);
    }

    return logical_achars;
}

int _gdi_reorder_text (PDC pdc, const unsigned char* text, int text_len,
                BOOL direction, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    int i = 0;
    unsigned int char_type;
    Achar32  chv;
    DEVFONT* sbc_devfont = pdc->pLogFont->devfonts[0];
    DEVFONT* mbc_devfont = pdc->pLogFont->devfonts[1];

    if (mbc_devfont) {
        int nr_achars = 0;
        Achar32 *logical_achars = NULL;

        if (mbc_devfont->charset_ops->bidi_char_type) {

            logical_achars = _gdi_bidi_reorder (pdc, text, text_len, &nr_achars);
            if (!logical_achars || nr_achars <= 0)
                return 0;

            i = _gdi_output_visual_achars (pdc, logical_achars, nr_achars,
                    direction, cb_one_glyph, context);
        }
        else if (!direction) {
            /* not need reorder achs, right to left, should alloc
             * achs range. */
            logical_achars = _gdi_get_achars_string(pdc, text, text_len,
                    &nr_achars);
            if(!logical_achars || nr_achars <= 0) return 0;

            i = _gdi_output_visual_achars(pdc, logical_achars, nr_achars,
                    direction, cb_one_glyph, context);
        }
        else{
            /* not need reorder achs, output left to right.*/
            i = _gdi_output_achars_direct(pdc, text, text_len,
                    cb_one_glyph, context, FALSE, TRUE);
        }

        if (logical_achars) {
            free(logical_achars);
        }
    }
    else {
        if (!direction) { /* right to left, reverse text.*/
            for (i = text_len - 1; i >= 0; i--) {
                Glyph32 gv;

                if (!(chv = sbc_devfont->charset_ops->get_char_value
                        (NULL, 0, text + i, 1)))
                    return (text_len-i-1);

                char_type = sbc_devfont->charset_ops->char_type
                    (chv);

                gv = GetGlyphValueAlt (pdc->pLogFont, chv);
                if (gv == INV_GLYPH_VALUE)
                    continue;

                if (!cb_one_glyph (context, gv, char_type))
                    break;
            }
        }
        else {
            for (i = 0; i < text_len; i++) {
                Glyph32 gv;

                if (!(chv = sbc_devfont->charset_ops->get_char_value
                    (NULL, 0, text + i, 1)))
                    return i;

                char_type = sbc_devfont->charset_ops->char_type
                    (chv);

                gv = GetGlyphValueAlt (pdc->pLogFont, chv);
                if (gv == INV_GLYPH_VALUE)
                    continue;

                if (!cb_one_glyph (context, gv, char_type))
                    break;
            }
        }
    }
    return i;
}

static Achar32*
_gdi_get_achars_string_charbreak(PDC pdc, const unsigned char* text,
        int text_len, int* nr_achars, void* context)
{
    int i = 0;
    int len_cur_char;
    int left_bytes = text_len;
    unsigned int char_type;
    Achar32  chv;
    DEVFONT* sbc_devfont = pdc->pLogFont->devfonts[0];
    DEVFONT* mbc_devfont = pdc->pLogFont->devfonts[1];
    Achar32 *logical_achars = NULL;

    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    int line_width = 0;
    BBOX bbox;

    int prev_len = 0;
    const unsigned char* prev_mchar = NULL;

    logical_achars = malloc(text_len * sizeof (Achar32));

    while (left_bytes > 0){
        Glyph32 gv;

        if (mbc_devfont) {
            len_cur_char = mbc_devfont->charset_ops->len_first_char
                ((const unsigned char*)text, left_bytes);
        }
        else len_cur_char = 0;

        if (len_cur_char > 0) {
            chv = mbc_devfont->charset_ops->get_char_value
                (prev_mchar, prev_len, text, left_bytes);

            prev_mchar = text;
            prev_len = len_cur_char;
            logical_achars[i++] = SET_MBCHV(chv);

            char_type = (*mbc_devfont->charset_ops->char_type)
                (chv);
            if (!(char_type & ACHAR_BASIC_NOSPACING_MARK)){
                prev_mchar = text;
            }
            else{
                prev_len += len_cur_char;
            }

            gv = GetGlyphValueAlt(pdc->pLogFont, chv);
            line_width += _gdi_get_glyph_advance (pdc, gv,
                    TRUE, 0, 0, NULL, NULL, &bbox);
            left_bytes -= len_cur_char;
            text += len_cur_char;
        }
        else{
            len_cur_char = sbc_devfont->charset_ops->len_first_char
                ((const unsigned char*)text, left_bytes);

            if (len_cur_char > 0) {
                chv = sbc_devfont->charset_ops->get_char_value
                    (NULL, 0, text, left_bytes);
                logical_achars[i++] = chv;

                gv = GetGlyphValueAlt(pdc->pLogFont, chv);
                line_width += _gdi_get_glyph_advance (pdc, gv,
                        TRUE, 0, 0, NULL, NULL, &bbox);
                left_bytes -= len_cur_char;
                text += len_cur_char;
            }
            else
                break;
        }

        if(line_width > ctxt->max_extent){
            break;
        }
        ctxt->nCount = text_len - left_bytes;
    }
    if(nr_achars) *nr_achars = i;

    return logical_achars;
}


static int _gdi_get_nextword_width (PDC pdc, const unsigned char* pText,
                int nCount, int* nChars, void* context)
{
    SIZE size;
    DEVFONT* sbc_devfont = pdc->pLogFont->devfonts[0];
    DEVFONT* mbc_devfont = pdc->pLogFont->devfonts[1];
    WORDINFO word_info = {0};

    *nChars = 0;
    if (nCount == 0) return 0;

    if (mbc_devfont) {
        int mbc_pos, sub_len;

        mbc_pos = (*mbc_devfont->charset_ops->pos_first_char)
                ((const unsigned char*)pText, nCount);
        if (mbc_pos == 0) {
            sub_len = (*mbc_devfont->charset_ops->len_first_substr)
                    ((const unsigned char*)pText, nCount);

            (*mbc_devfont->charset_ops->get_next_word)
                    ((const unsigned char*)pText, sub_len, &word_info);

            if (word_info.len == 0) {
                *nChars = 0;
                return 0;
            }
        }
        else if (mbc_pos > 0)
            nCount = mbc_pos;
    }

    if (word_info.len == 0)
        (*sbc_devfont->charset_ops->get_next_word)
            ((const unsigned char*)pText, nCount, &word_info);

    _gdi_get_drawtext_extent (pdc, pText, word_info.len, context, &size);

    *nChars = word_info.len;

    /* Fixed bug of italic font */
    return size.cx - _gdi_get_italic_added_width (pdc->pLogFont);
}

static void
_gdi_get_achars_string_pos_wordbreak(PDC pdc, const unsigned char* text,
        int text_len, int* nr_achars, void* context)
{
    int len_cur_char = 0;
    int left_bytes = text_len;

    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    int line_width = 0;
    int wordLen = 0;

    while (left_bytes > 0){
        line_width += _gdi_get_nextword_width (pdc, text, left_bytes,
                &wordLen, context);
        if(line_width > ctxt->max_extent){
            if(ctxt->nCount == 0){
                Achar32* logical_achars =
                    _gdi_get_achars_string_charbreak(pdc, text,
                        text_len, nr_achars, ctxt);
                if(logical_achars) free(logical_achars);
            }
            break;
        }

        if(wordLen == 0){
            SIZE size;
            len_cur_char = GetFirstMCharLen (pdc->pLogFont,
                    (const char*)(text + wordLen), left_bytes);
            line_width += _gdi_get_drawtext_extent (pdc, text + wordLen, len_cur_char,
                    context, &size);
#if 0
            /* wangjian fixed bug 3002, 2008-8-29.*/
            if (*(const unsigned char*)(text) == '\t')
                line_width += ctxt->tab_width;
            else
                line_width += size.cx;
#endif

            if(line_width > ctxt->max_extent){
                break;
            }
        }
        else
            len_cur_char = 0;

        left_bytes -= (wordLen + len_cur_char);
        text += (wordLen + len_cur_char);
        ctxt->nCount += (wordLen + len_cur_char);
    }
}

static Achar32*
_gdi_get_achars_string_wordbreak(PDC pdc, const unsigned char* text,
        int text_len, int* nr_achars, void* context)
{
    DEVFONT* mbc_devfont = pdc->pLogFont->devfonts[1];
    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    Achar32 *logical_achars = NULL;

    if (mbc_devfont) {
        _gdi_get_achars_string_pos_wordbreak(pdc, text,
                text_len, nr_achars, context);
        logical_achars = _gdi_get_achars_string(pdc, text, ctxt->nCount,
                nr_achars);
    }
    return logical_achars;
}

static Achar32* _gdi_get_achars_string_break(PDC pdc, const unsigned char* text,
        int text_len, int* nr_achars, void* context)
{
    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    if((ctxt->nFormat & DT_CHARBREAK) || (ctxt->nFormat & DT_SINGLELINE)){
        return _gdi_get_achars_string_charbreak(pdc, text,
                text_len, nr_achars, ctxt);
    }
    //else if(ctxt->nFormat & DT_WORDBREAK){
    else {
        return _gdi_get_achars_string_wordbreak(pdc, text,
                text_len, nr_achars, ctxt);
    }
}

static int
_gdi_output_achars_direct_break(PDC pdc, const unsigned char* text,
        int text_len, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    int nr_achars = 0;

    if(ctxt->nFormat & DT_WORDBREAK){
        _gdi_get_achars_string_pos_wordbreak(pdc, text,
                text_len, &nr_achars, context);

        return _gdi_output_achars_direct(pdc, text, ctxt->nCount,
                    cb_one_glyph, ctxt, FALSE, TRUE);
    }
    else {
        return _gdi_output_achars_direct(pdc, text, text_len,
                    cb_one_glyph, ctxt, TRUE, TRUE);
    }
}


static void _gdi_get_achars_string_sbc_rtol_wordbreak(PDC pdc,
      const unsigned char* text, int text_len, int* nr_achars, void* context)
{
    int len_cur_char = 0;
    int left_bytes = text_len;

    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    int line_width = 0;
    int wordLen = 0;

    while (left_bytes > 0){
        line_width += _gdi_get_nextword_width (pdc, text, left_bytes,
                &wordLen, context);
        if(line_width > ctxt->max_extent){
            if (0 == ctxt->nCount && ctxt->max_extent > 0) {
                ctxt->nCount = wordLen * ((float)ctxt->max_extent / line_width);
            }
            break;
        }

        if(wordLen == 0){
            SIZE size;
            len_cur_char = GetFirstMCharLen (pdc->pLogFont,
                    (const char*)(text + wordLen), left_bytes);
            _gdi_get_drawtext_extent (pdc, text + wordLen, len_cur_char,
                    context, &size);
            line_width += size.cx;
            if(line_width > ctxt->max_extent){
                break;
            }
        }
        else
            len_cur_char = 0;

        left_bytes -= (wordLen + len_cur_char);
        text += (wordLen + len_cur_char);
        ctxt->nCount += (wordLen + len_cur_char);
    }
}

static int
_gdi_output_achars_direct_sbc_rtol_break(PDC pdc, const unsigned char* text,
        int text_len, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    int nr_achars = 0;
    int i = 0;
    unsigned int char_type;
    Achar32 chv;
    DEVFONT* sbc_devfont = pdc->pLogFont->devfonts[0];

    if(ctxt->nFormat & DT_WORDBREAK){
        _gdi_get_achars_string_sbc_rtol_wordbreak(pdc,
                text, text_len, &nr_achars, ctxt);
    }
    else {
        /* get break char's pos, only count no draw chars out. */
        _gdi_output_achars_direct(pdc, text, text_len,
                    cb_one_glyph, ctxt, TRUE, FALSE);
        /*
        return _gdi_output_achars_direct(pdc, text, ctxt->nCount,
                    cb_one_glyph, ctxt, TRUE, TRUE);
        */
    }

    if(ctxt->nCount <= 0) return 0;

    for (i = ctxt->nCount - 1; i >= 0; i--) {
        Glyph32 gv;
        chv = sbc_devfont->charset_ops->get_char_value
            (NULL, 0, text + i, 1);

        char_type = sbc_devfont->charset_ops->char_type
            (chv);

        gv = GetGlyphValueAlt(pdc->pLogFont, chv);
        if (!cb_one_glyph (context, gv, char_type))
            break;
    }
    return nr_achars;
}

int _gdi_reorder_text_break (PDC pdc, const unsigned char* text,
        int text_len, BOOL direction, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    int i = 0;
    DEVFONT* mbc_devfont = pdc->pLogFont->devfonts[1];

    if (mbc_devfont) {
        int nr_achars = text_len;
        Achar32 *logical_achars = NULL;

        if (mbc_devfont->charset_ops->bidi_char_type){

            logical_achars = _gdi_get_achars_string_break(pdc, text, text_len,
                    &nr_achars, context);
            __mg_legacy_bidi_achars_reorder (mbc_devfont->charset_ops,
                logical_achars, nr_achars, -1, NULL, NULL);

            if(!logical_achars)
                return 0;

            i = _gdi_output_visual_achars(pdc, logical_achars, nr_achars,
                    direction, cb_one_glyph, context);
        }
        /* not need reorder achs, right to left, should
         * alloc achs range. */
        else if(!direction){
            logical_achars = _gdi_get_achars_string_break(pdc, text, text_len,
                    &nr_achars, context);
            if(!logical_achars) return 0;
            i = _gdi_output_visual_achars(pdc, logical_achars, nr_achars,
                    direction, cb_one_glyph, context);
        }
        /* not need reorder achs, output left to right.*/
        else {
            i = _gdi_output_achars_direct_break(pdc, text, text_len,
                    cb_one_glyph, context);
        }

        if (logical_achars) {
            free(logical_achars);
        }
    }
    else{
        /* note: follow sbc_devfont surpport not need to
         * alloc mem for achs.*/
        if (!direction) {
            /* right to left, reverse text.*/
            i = _gdi_output_achars_direct_sbc_rtol_break(pdc, text,
                    text_len, cb_one_glyph, context);
        }
        else {
            i = _gdi_output_achars_direct_break(pdc, text, text_len,
                    cb_one_glyph, context);
        }
    }
    return i;
}

/* \bref: Get the logical glyph string and the achs map table, if
 * caller not malloc space for achs or achs_map, it will malloc
 * space inner.
 *
 * \param  LOGFONT* log_font: The logical font.
 * \param  unsigned char* text: Input logical string, input.
 * \param  int text_len: Input logical string len, input.
 * \param  Achar32** achs: logical glyph string, input/output.
 * \param  ACHARMAPINFO* glyph_map: position mapping from Logical
 * achs strin to logical text, output.
 *
 * \param  int        : return logical glyph string len, output.
 */
int  GUIAPI BIDIGetTextLogicalAChars(
        LOGFONT*       log_font,
        const char*    text,
        int            text_len,
        Achar32**      achs,
        ACHARMAPINFO** achs_map)
{
    int i = 0;
    int left_bytes = text_len;
    int nr_chars;
    int prev_len = 0, len_cur_char = 0;
    const char* prev_mchar = NULL;
    Achar32  chv = INV_ACHAR_VALUE;
    Achar32* achar_str = NULL;
    ACHARMAPINFO* map = NULL;

    /* use the actual characters count for optimized memory usage. */
    nr_chars = GetTextMCharInfo (log_font, text, text_len, NULL);

    if (achs && *achs == NULL){
        *achs = malloc (nr_chars * sizeof (Achar32));
        memset (*achs, 0, nr_chars * sizeof(Achar32));
    }

    if(achs_map && *achs_map == NULL){
        *achs_map = malloc (nr_chars * sizeof (ACHARMAPINFO));
        memset (*achs_map, 0, nr_chars * sizeof(ACHARMAPINFO));
    }

    if (achs)     achar_str = *achs;
    if (achs_map) map = *achs_map;
    if (!map && !achar_str)
        return 0;

    while (left_bytes > 0){
        len_cur_char = GetFirstMCharLen(log_font, text, left_bytes);

        chv = GetACharValue(log_font, text, left_bytes,
                prev_mchar, prev_len);

        if(achar_str) achar_str[i] = chv;
        /* set the glyph map info. */
        if(map) {
            map[i].char_len   = len_cur_char;
            map[i].byte_index = text_len - left_bytes;
        }

        prev_mchar = text;
        prev_len   = len_cur_char;
        left_bytes -= len_cur_char;
        text += len_cur_char;

        i ++;
    }

    return i;
}

typedef struct _RANGEINFO{
    int start;
    int end;
    int level;
    struct _RANGEINFO* next;
}RANGEINFO;

extern int GUIAPI GetLastMCharLen (PLOGFONT log_font, const char* mstr, int len);

static BOOL check_sel_range (PLOGFONT log_font, const char* text,
        int text_len, int* start_index, int* end_index)
{
    BOOL ret = TRUE;
    if(*start_index < 0 || *start_index > text_len){
        if(*start_index < 0) *start_index = 0;
        else if(*start_index >= text_len){
            //int prev_len = GetLastMCharLen (log_font, text, text_len);
            //*start_index = text_len - prev_len;
            *start_index = text_len;
        }
    }
    if(*end_index < 0 || *end_index >= text_len){
        if(*end_index < 0) *end_index = 0;
        else if(*end_index >= text_len){
            //int prev_len = GetLastMCharLen (log_font, text, text_len);
            //*end_index = text_len - prev_len;
            *end_index = text_len;
        }
    }
    if(*end_index == *start_index) ret = FALSE;
    DBGLOG3("start_index=%d,end_index=%d\n", *start_index, *end_index);
    return ret;
}

#define DEF_RANGES 8

static int* generate_ranges(PLOGFONT log_font, Achar32* log_achars,
        int nr_achars, ACHARMAPINFO* l_map,
        int l_start_index, int l_end_index, int* nr_ranges)
{
    int m = 0, i = 0, j = 0;
    int* p = NULL;
    int max_ranges = DEF_RANGES * 2;
    ACHARMAPINFO* v_map = NULL;
    Uint8* embedding_levels = NULL;

    BIDIGetLogicalEmbeddLevels(log_font, log_achars,
            nr_achars, &embedding_levels);

    /* get all the logical ranges in the same language.
     * default malloc DEF_RANGES ranges.*/
    p = (int*) malloc(max_ranges * sizeof(int));
    memset(p, 0, max_ranges * sizeof(int));
    m = 0;
    p[m++] = l_start_index;
    for(i = l_start_index; i <= l_end_index; i++){
        if((i+1) <= l_end_index
                && embedding_levels[i] != embedding_levels[i+1]){
            if(m > (max_ranges-2)) {
                max_ranges += DEF_RANGES*2;
                p = realloc(p, max_ranges * sizeof(int));
                memset(p+m+1, 0, DEF_RANGES*2);
            }
            p[m++] = i;
            p[m++] = i+1;
        }
    }
    p[m++] = l_end_index;

    /* translate the logical glyph index to logical bytes index. */
    for(i = 0; i < m; i++){
        p[i] = l_map[p[i]].byte_index;
    }

    v_map = l_map;
    BIDILogAChars2VisAChars (log_font, log_achars, nr_achars, v_map);

    *nr_ranges = m;

    /* translate the logical bytes index to visual glyph index. */
    for(j = 0; j < *nr_ranges; j++){
        for(i = 0; i < nr_achars; i++) {
            if(p[j] == v_map[i].byte_index){
                p[j] = i;
                break;
            }
        }
    }

    free(embedding_levels);
    return p;
}


static BOOL check_range_edge(int index, int nr_achars, ACHARMAPINFO* v_map)
{
    if (index < 0 || index > nr_achars)
        return FALSE;

    if(index >= nr_achars-1 || index == 0){
        return TRUE;
    }

    if(v_map[index].is_rtol != v_map[index+1].is_rtol)
        return TRUE;

    return FALSE;
}

/* note: ranges is such as follow:
 * p(int*): 3,8,12,10,26,20 (for EN first).
 *
 * p(int*): 26,20,12,10,3,8 (for AR first).
 *
 * ranges must sort asce, final is:
 * p(int*) (3,8),(10,12),(20,26).
 */
static void computer_ranges(int* p, int* nr_ranges)
{
    int j = 0;
    int m = *nr_ranges;

    /* set the visual achs range start_index < end_index.*/
    for(j = 0; j < m; j+=2){
        if((j+1) < m && p[j] > p[j+1]){
            int tmp = p[j];
            p[j] = p[j+1];
            p[j+1] = tmp;
        }
    }

    /* reset the ranges order, such as (53,66),(37,47),(12,26), need swap
     * to (12,26),(37,47),(53,66).*/
    {
        int i = 0;
        for(i = 0; i < m-1; i+= 2){
            for(j = i+2; j < m-1; j+=2){
                if(p[i] > p[j]){
                    int tmp = p[j];
                    p[j] = p[i];
                    p[i] = tmp;

                    tmp = p[j+1];
                    p[j+1] = p[i+1];
                    p[i+1] = tmp;
                }
            }
        }
    }
    for(j = 1; j < m; j+=2){
        if((j+1) < m && p[j] && p[j+1]
                && (p[j] == p[j+1] || p[j]+1 == p[j+1])){
            memcpy(p+j, p+j+2, (m-j-1)*sizeof(int));
            memset(p+m-2, 0, 2 * sizeof(int));
            /* do next also from j.*/
            j -= 2;
        }
    }

    /* computer the ranges num.*/
    for(j = 0; j < m; j++){
        if(p[j] != 0){
            *nr_ranges = (j+1);
        }
    }
}

void GUIAPI GetTextRangesLog2VisTest(
        LOGFONT* log_font,
        char*    text,
        int      text_len,
        int      start_index,
        int      end_index,
        int**    ranges,
        int*     nr_ranges)
{
    int m = 0, i = 0;
    int* p = NULL;
    Achar32* l_achars = NULL;
    ACHARMAPINFO* v_map = NULL;
    ACHARMAPINFO* l_map = NULL;
    int nr_achars = 0;

    *nr_ranges = 0;

    if(!check_sel_range(log_font, text, text_len,
                &start_index, &end_index)){
        return;
    }

    nr_achars = BIDIGetTextLogicalAChars(log_font, text, text_len,
            &l_achars, &l_map);

    v_map = l_map;
    BIDILogAChars2VisAChars (log_font, l_achars, nr_achars, v_map);

    /* get all the logical ranges in the same language.
     * default malloc 5 ranges.*/
    p = (int*) malloc( 2 * 8 * sizeof(int));
    memset(p, 0, 16*sizeof(int));
    m = 0;

    if(start_index > end_index){
        int a = start_index;
        start_index = end_index;
        end_index = a;
    }
    for(i = 0; i < nr_achars; i++){
        /* deal the start_index and end_index.*/
        if(v_map[i].byte_index <= start_index &&
                (v_map[i].byte_index+v_map[i].char_len) > start_index){
            p[m++] = i;
        }
        if(v_map[i].byte_index <= end_index &&
                (v_map[i].byte_index+v_map[i].char_len) > end_index){
            p[m++] = i;
        }

        if((i+1) >= nr_achars){
            if(v_map[i].byte_index > start_index
                    && v_map[i].byte_index < end_index){
                p[m++] = i+1;
            }
            break;
        }
        /* deal middle index with the logical ranges.*/
        if(v_map[i].is_rtol != v_map[i+1].is_rtol
               && v_map[i].byte_index > start_index
               && v_map[i].byte_index < end_index){
            p[m++] = i+1;
        }
    }

    *nr_ranges = m;
    computer_ranges(p, nr_ranges);

    *ranges = p;
    *nr_ranges /= 2;

    free(l_map);
    free(l_achars);
}

/*
 * \param  bref: Get a list of visual ranges corresponding to a given
 * logical range.
 *
 * \param  text:        Input logical text string, Input.
 * \param  text_len:    Input logical text string len, Input.
 * \param  start_index: Input Start byte index of the logical range, Input.
 * \param  end_index: Input Ending byte index of the logical range, Input.
 * \param  ranges:    Output location to store a pointer to an array of
 * arranges, Output
 * \param  nr_ranges: Output The number of ranges stored in ranges, Output
 *
 * \return  void .
 */
void GUIAPI BIDIGetTextRangesLog2Vis(LOGFONT* log_font,
        const char* text, int text_len, int start_index, int end_index,
        int** ranges, int* nr_ranges)
{
    int  i = 0;
    int* p = NULL;
    Achar32* l_achars = NULL;
    ACHARMAPINFO* l_map = NULL;
    int l_start_index = start_index, l_end_index = end_index;
    int nr_achars = 0;

    *nr_ranges = 0;
    /* it is not bidi string. */
    if (!log_font->devfonts[1] || (log_font->devfonts[1] &&
                !log_font->devfonts[1]->charset_ops->bidi_char_type)) {
        *ranges = NULL;
        return;
    }

    /*
    if(text_len == 1){
        *nr_ranges = 1;
        p = (int*) malloc(2* sizeof(int));
        memset(p, 0, 2* sizeof(int));
        if(start_index > end_index){
            int a = start_index;
            start_index = end_index;
            end_index = a;
        }
        p[0] = start_index;
        p[1] = end_index;
        *ranges = p;
        return;
    }
    */

    if(!check_sel_range(log_font, text, text_len,
                &start_index, &end_index)){
        return;
    }

    nr_achars = BIDIGetTextLogicalAChars(log_font, text, text_len,
            &l_achars, &l_map);

    for(i = 0; i < nr_achars; i++) {
        int len = l_map[i].char_len;
        if(l_map[i].byte_index <= start_index
                && start_index <= (l_map[i].byte_index + len)){
            l_start_index = i;
            continue;
        }

        if(l_map[i].byte_index <= end_index
                && end_index <= (l_map[i].byte_index + len))
            l_end_index = i;
    }

    if(l_start_index > l_end_index) {
        int tmp = l_start_index;
        l_start_index = l_end_index;
        l_end_index = tmp;
    }

    p = generate_ranges(log_font, l_achars, nr_achars, l_map,
            l_start_index, l_end_index, nr_ranges);

    computer_ranges(p, nr_ranges);

    /* check the ranges edges, +1 when edges.
     * after generate_ranges, l_map is translate to v_map*/
    for(i = 1; i < *nr_ranges; i+=2){
        if(p[i] != 0 && check_range_edge(p[i], nr_achars, l_map)){
            p[i] += 1;
        }
    }

    *ranges = p;
    *nr_ranges /= 2;

    free(l_map);
    free(l_achars);
}

static void bidi_reverse_map (void* context, int len, int pos)
{
    ACHARMAPINFO* str = (ACHARMAPINFO*)context + pos;
    int i;
    for (i = 0; i < len / 2; i++)
    {
        ACHARMAPINFO tmp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = tmp;
    }
}

int GUIAPI BIDIGetTextVisualAChars(LOGFONT* log_font,
        const char*    text, int text_len,
        Achar32**      achs,
        ACHARMAPINFO** achs_map)
{
    int nr_achars = 0;
    DEVFONT* mbc_devfont = log_font->devfonts[1];

    if (achs == NULL || achs_map == NULL)
        return nr_achars;

    nr_achars = BIDIGetTextLogicalAChars(log_font, text, text_len,
            achs, achs_map);

    if (nr_achars > 0 && mbc_devfont
                && mbc_devfont->charset_ops->bidi_char_type) {
        __mg_legacy_bidi_achars_reorder (mbc_devfont->charset_ops,
                *achs, nr_achars, -1,
                *achs_map, bidi_reverse_map);
    }

    return nr_achars;
}

Achar32* GUIAPI BIDILogAChars2VisAChars(LOGFONT* log_font,
        Achar32* achs, int nr_achars, ACHARMAPINFO* achs_map)
{
    DEVFONT* mbc_devfont = log_font->devfonts[1];

    if (nr_achars > 0 && mbc_devfont
                && mbc_devfont->charset_ops->bidi_char_type) {
        __mg_legacy_bidi_achars_reorder (mbc_devfont->charset_ops,
                achs, nr_achars, -1,
                achs_map, bidi_reverse_map);

        return achs;
    }

    return NULL;
}

BOOL GUIAPI BIDILogAChars2VisACharsEx(LOGFONT* log_font,
        Achar32* achs, int nr_achars, int pel,
        void* extra, CB_REVERSE_ARRAY cb_reorder_extra)
{
    DEVFONT* mbc_devfont = log_font->devfonts[1];

    if (nr_achars > 0 && mbc_devfont
                && mbc_devfont->charset_ops->bidi_char_type) {
        __mg_legacy_bidi_achars_reorder (mbc_devfont->charset_ops,
                achs, nr_achars, pel, extra, cb_reorder_extra);

        return TRUE;
    }

    return FALSE;
}

void GUIAPI BIDIGetLogicalEmbedLevelsEx(LOGFONT* log_font,
        Achar32* achs, int nr_achars, int pel,
        Uint8**  embedding_levels)
{
    DEVFONT* mbc_devfont = log_font->devfonts[1];

    if (*embedding_levels == NULL)
        *embedding_levels = malloc(nr_achars * sizeof (Uint8));

    if (mbc_devfont && mbc_devfont->charset_ops->bidi_char_type) {
        __mg_legacy_bidi_get_embeddlevels (mbc_devfont->charset_ops,
                achs, nr_achars, pel, *embedding_levels, 0);
    }
    else {
        memset (*embedding_levels, 0, sizeof(Uint8) * nr_achars);
    }
}

void GUIAPI BIDIGetVisualEmbedLevelsEx(LOGFONT* log_font,
        Achar32* achs, int nr_achars, int pel,
        Uint8**  embedding_levels)
{
    DEVFONT* mbc_devfont = log_font->devfonts[1];

    if (*embedding_levels == NULL)
        *embedding_levels = malloc(nr_achars * sizeof (Uint8));

    if (mbc_devfont && mbc_devfont->charset_ops->bidi_char_type) {
        __mg_legacy_bidi_get_embeddlevels (mbc_devfont->charset_ops,
                achs, nr_achars, pel, *embedding_levels, 1);
    }
    else {
        memset (*embedding_levels, 0, sizeof(Uint8) * nr_achars);
    }
}

