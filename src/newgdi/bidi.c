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
** textout.c: Hight level textout drawing.
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

static Glyph32* _gdi_get_glyphs_string(PDC pdc, const unsigned char* text,
        int text_len, int* nr_glyphs)
{
    int i = 0;
    int len_cur_char;
    int left_bytes = text_len;
    int glyph_type;
    Glyph32  glyph_value;
    DEVFONT* sbc_devfont = pdc->pLogFont->sbc_devfont;
    DEVFONT* mbc_devfont = pdc->pLogFont->mbc_devfont;
    Glyph32 *logical_glyphs = NULL;

    if (mbc_devfont) {
        int prev_len = 0;
        const unsigned char* prev_mchar = NULL;
        logical_glyphs = malloc(text_len * sizeof (Glyph32));
        while (left_bytes > 0){
            len_cur_char = mbc_devfont->charset_ops->len_first_char
                ((const unsigned char*)text, left_bytes);
            if (len_cur_char > 0) {
                glyph_value = mbc_devfont->charset_ops->char_glyph_value
                    (prev_mchar, prev_len, text, left_bytes);

                logical_glyphs[i++] = SET_MBC_GLYPH(glyph_value);
                glyph_type = (*mbc_devfont->charset_ops->glyph_type)
                    (glyph_value);
                if(!(glyph_type & MCHAR_TYPE_NOSPACING_MARK)){
                    prev_mchar = text;
                    prev_len = len_cur_char;
                }
                else{
                    prev_len += len_cur_char;
                }

                left_bytes -= len_cur_char;
                text += len_cur_char;
                continue;
            }

            len_cur_char = sbc_devfont->charset_ops->len_first_char
                ((const unsigned char*)text, left_bytes);

            if (len_cur_char > 0) {
                glyph_value = sbc_devfont->charset_ops->char_glyph_value
                    (NULL, 0, text, left_bytes);
                logical_glyphs[i++] = glyph_value;
                text += len_cur_char;
                left_bytes -= len_cur_char;
            }
            else
                break;
        }
        if(nr_glyphs) *nr_glyphs = i;
    }
    return logical_glyphs;
}

static  int inline get_glyph_type (LOGFONT* logfont, Glyph32 glyph_value)
{
    if (IS_MBC_GLYPH (glyph_value))
        return logfont->mbc_devfont->charset_ops->glyph_type 
            (REAL_GLYPH (glyph_value));
    else
        return logfont->sbc_devfont->charset_ops->glyph_type 
            (glyph_value);
}

static int inline get_glyph_advance_in_dc (PDC pdc, Glyph32 glyph_value)
{
    return _gdi_get_glyph_advance(pdc, glyph_value, TRUE, 0, 0, NULL, NULL, NULL);
}


static int jump_vowels_ltr (PDC pdc, Glyph32* glyphs, int glyph_num, int* biggest_vowel)
{
    int i = 0;
    int max_advance = 0;
    int cur_advance = 0;
    for (i=0; i<glyph_num; i++)
    {
        if (get_glyph_type (pdc->pLogFont, glyphs[i]) != MCHAR_TYPE_VOWEL) 
            return i;
        cur_advance = get_glyph_advance_in_dc (pdc, glyphs[i]);
        if (cur_advance > max_advance)
        {
            max_advance = cur_advance;
            *biggest_vowel = glyphs [i];
        }
    }
    return glyph_num;
}

static int output_visual_glyphs_ltr (PDC pdc, Glyph32* visual_glyphs, 
    int nr_glyphs, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    int i = 0;
    int j = 0;
    int glyph_type;
    Glyph32 glyph_value;
    Glyph32 biggest_vowel = 0;

    int vowel_num;
    int outed_num = 0;

    int old_text_align = pdc->ta_flags;
    int bkmode = pdc->bkmode;

    while (i < nr_glyphs) {
        /*jump vowels*/
        vowel_num = jump_vowels_ltr (pdc, visual_glyphs, 
                nr_glyphs-i, &biggest_vowel);

        visual_glyphs += vowel_num;

        /*output the letter after vowels, 
         * the letter is the owner of jumped vowels
         * if there is a letter on the vowel's right*/
        if (vowel_num < nr_glyphs - i)
        {
            glyph_value = *(visual_glyphs);
            glyph_type = get_glyph_type (pdc->pLogFont, glyph_value);
            if (!cb_one_glyph (context, glyph_value, glyph_type)){
                goto end;
            }
            outed_num++;

            /*set vowel aligns width letter's right*/
            pdc->ta_flags = (old_text_align & ~TA_LEFT) | TA_RIGHT;
            pdc->bkmode = BM_TRANSPARENT;
        }

        /*first output the biggest vowel*/
        if (vowel_num)
        {
            glyph_type = get_glyph_type (pdc->pLogFont, biggest_vowel);
            if (!cb_one_glyph (context, biggest_vowel, glyph_type)){
                goto end;
            }
            outed_num++;
            /*transparent from other vowels*/
            pdc->bkmode = BM_TRANSPARENT;
        }

        /*output other vowels from visual_glyphs[i]*/
        for (j = 1; j<=vowel_num; j++)
        {
            glyph_value = *(visual_glyphs - j);
            if (glyph_value != biggest_vowel)
            {
                glyph_type = get_glyph_type (pdc->pLogFont, glyph_value);
                if (!cb_one_glyph (context, glyph_value, glyph_type)){
                    goto end;
                }
                outed_num++;
            }
        }

        pdc->ta_flags = old_text_align;
        pdc->bkmode = bkmode;

        i += vowel_num + 1;
        visual_glyphs ++;
    }

end:
    pdc->bkmode = bkmode;
    return outed_num;
}

static int output_vowels_rtl (PDC pdc, Glyph32* end_glyph, int left_num,
     CB_ONE_GLYPH cb_one_glyph, void* context, int* outed_num)
{
    int i = 0;
    Glyph32 glyph_value;
    int glyph_type;
    int old_text_align = pdc->ta_flags;
    int old_bkmode = pdc->bkmode;

    pdc->ta_flags = (old_text_align & ~TA_RIGHT) | TA_LEFT;
    pdc->bkmode = BM_TRANSPARENT;
    *outed_num = 0;
    for (i=0; i<left_num; i++)
    {
        glyph_value = *end_glyph--;
        glyph_type = get_glyph_type (pdc->pLogFont, glyph_value);

        if (glyph_type == MCHAR_TYPE_VOWEL) 
        {
            if (cb_one_glyph(context, glyph_value, glyph_type))
            {
                (*outed_num) ++;
            }
        }
        else break;
    }

    pdc->ta_flags = old_text_align;
    pdc->bkmode = old_bkmode;
    return i;
}

static int output_unowned_vowels_rtl (PDC pdc, Glyph32* end_glyph, int left_num,
     CB_ONE_GLYPH cb_one_glyph, void* context, int* outed_num)
{
    int i = 0;
    Glyph32 glyph_value   = INV_GLYPH_VALUE;
    Glyph32 biggest_vowel = INV_GLYPH_VALUE;
    int glyph_type;
    int cur_advance = 0;
    int max_advance = 0;
    int bkmode = pdc->bkmode;
    int old_text_align = pdc->ta_flags;

    *outed_num = 0;
    if (get_glyph_type (pdc->pLogFont, *end_glyph) != MCHAR_TYPE_VOWEL)
        return 0;

    pdc->ta_flags = (old_text_align & ~TA_RIGHT) | TA_LEFT;
    for (i=0; i<left_num; i++)
    {
        glyph_value = end_glyph [-i];
        glyph_type = get_glyph_type (pdc->pLogFont, glyph_value);

        if (glyph_type != MCHAR_TYPE_VOWEL) 
            break;

        cur_advance = get_glyph_advance_in_dc (pdc, glyph_value);
        if (cur_advance > max_advance)
        {
            max_advance = cur_advance;
            biggest_vowel = glyph_value;
        }
    }

    /*output the biggest_vowel*/
    if(max_advance){
        glyph_type = get_glyph_type (pdc->pLogFont, biggest_vowel);
        if (cb_one_glyph(context, biggest_vowel, glyph_type)){
            (*outed_num) ++;
        }
    }

    pdc->bkmode = BM_TRANSPARENT;

    left_num = i;
    for (i=0; i<left_num; i++)
    {
        glyph_value = *end_glyph--;
        if (glyph_value != biggest_vowel)
        {
            glyph_type = get_glyph_type (pdc->pLogFont, glyph_value);
            if (cb_one_glyph(context, glyph_value, glyph_type))
            {
                (*outed_num) ++;
            }
        }
    }
    pdc->bkmode = bkmode;
    pdc->ta_flags = old_text_align;
    return left_num;
}

static int output_visual_glyphs_rtl (PDC pdc, Glyph32* visual_glyphs, 
    int nr_glyphs, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    int cur = nr_glyphs - 1;
    int glyph_type;
    Glyph32 glyph_value;
    int vowel_num;
    int outed_vowel_num;
    int outed_glyph_num = 0;

    /*out the vowels on the rightest*/
    vowel_num = output_unowned_vowels_rtl (pdc, visual_glyphs+cur, cur+1,
            cb_one_glyph, context, &outed_vowel_num);
    if (outed_vowel_num < vowel_num)
        return outed_vowel_num;

    cur -= vowel_num;
    outed_glyph_num = outed_vowel_num;

    while (cur >= 0) {
        glyph_value = visual_glyphs[cur];
        glyph_type = get_glyph_type (pdc->pLogFont, glyph_value);

        /*output the letter*/
        if (!cb_one_glyph(context, glyph_value, glyph_type)){
            return outed_glyph_num;
        }
        outed_glyph_num ++;

        /*output the vowels of current letter, align with the letter's left*/
        vowel_num = output_vowels_rtl (pdc, visual_glyphs+cur-1, cur,
                cb_one_glyph, context, &outed_vowel_num);
        outed_glyph_num += outed_vowel_num;

        if (outed_vowel_num < vowel_num)
            return outed_glyph_num;

        cur -= vowel_num + 1;
    }

    return outed_glyph_num;
}

int _gdi_output_visual_glyphs(PDC pdc, Glyph32* visual_glyphs, 
    int nr_glyphs, BOOL direction, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    /* call cb_one_glyph to show text. */
    if (!direction) { /* right to left.*/
        return  output_visual_glyphs_rtl (pdc, visual_glyphs, 
                nr_glyphs, cb_one_glyph, context);
    }
    else {
        return  output_visual_glyphs_ltr (pdc, visual_glyphs, 
                nr_glyphs, cb_one_glyph, context);
    }

}

static int _gdi_output_glyphs_direct(PDC pdc, const unsigned char* text, 
        int text_len, CB_ONE_GLYPH cb_one_glyph, void* context,
        BOOL if_break, BOOL if_draw)
{
    int glyph_type;
    Glyph32  glyph_value;
    DEVFONT* sbc_devfont = pdc->pLogFont->sbc_devfont;
    DEVFONT* mbc_devfont = pdc->pLogFont->mbc_devfont;
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
        if(mbc_devfont){
            len_cur_char = mbc_devfont->charset_ops->len_first_char
                ((const unsigned char*)text, left_bytes);
            if (len_cur_char > 0) {
                glyph_value = mbc_devfont->charset_ops->char_glyph_value
                    (prev_mchar, prev_len, text, left_bytes);

                glyph_value = SET_MBC_GLYPH(glyph_value);
                goto do_glyph;
            }
        }

        len_cur_char = sbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)text, left_bytes);

        if (len_cur_char > 0) {
            glyph_value = sbc_devfont->charset_ops->char_glyph_value
                (NULL, 0, text, left_bytes);
        }
        else break;

do_glyph:
        if (IS_MBC_GLYPH (glyph_value))
            glyph_type = mbc_devfont->charset_ops->glyph_type 
                (REAL_GLYPH (glyph_value));
        else
            glyph_type = sbc_devfont->charset_ops->glyph_type 
                (glyph_value);

        if(if_break){
            DRAWTEXTEX2_CTXT _txt;
            _txt.pdc         = pdc;
            _txt.x           = 0;
            _txt.y           = 0;
            _txt.advance     = 0;
            _txt.nFormat     = ctxt->nFormat;
            _txt.only_extent = TRUE;
            _txt.tab_width   = ctxt->tab_width;
            if (!cb_one_glyph(&_txt, glyph_value, glyph_type)){
                break;
            }
            line_width += _txt.advance; 
            if(line_width > ctxt->max_extent){
                break;
            }
        }

        if(if_draw) {
            if (!cb_one_glyph(ctxt, glyph_value, glyph_type)){
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

Glyph32* _gdi_bidi_reorder (PDC pdc, const unsigned char* text, int text_len,
        int* nr_glyphs)
{
    DEVFONT* mbc_devfont = pdc->pLogFont->mbc_devfont;
    Glyph32 *logical_glyphs = NULL;

    if (mbc_devfont && mbc_devfont->charset_ops->bidi_reorder) {
        logical_glyphs = _gdi_get_glyphs_string(pdc, text, text_len,
                nr_glyphs);
        if (*nr_glyphs > 0)
            mbc_devfont->charset_ops->bidi_reorder (logical_glyphs, *nr_glyphs);
    }

    return logical_glyphs;
}

int _gdi_reorder_text (PDC pdc, const unsigned char* text, int text_len, 
                BOOL direction, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    int i = 0;
    int glyph_type;
    Glyph32  glyph_value;
    DEVFONT* sbc_devfont = pdc->pLogFont->sbc_devfont;
    DEVFONT* mbc_devfont = pdc->pLogFont->mbc_devfont;

    if (mbc_devfont) {
        int nr_glyphs = 0;
        Glyph32 *logical_glyphs = NULL;

        if(mbc_devfont->charset_ops->bidi_reorder){

            logical_glyphs = _gdi_bidi_reorder(pdc, text, text_len, &nr_glyphs);
            if(!logical_glyphs || nr_glyphs <= 0) 
                return 0;

            i = _gdi_output_visual_glyphs(pdc, logical_glyphs, nr_glyphs, 
                    direction, cb_one_glyph, context);
        }
        else if(!direction){ 
            /* not need reorder glyphs, right to left, should alloc 
             * glyphs range. */
            logical_glyphs = _gdi_get_glyphs_string(pdc, text, text_len,
                    &nr_glyphs);
            if(!logical_glyphs || nr_glyphs <= 0) return 0;

            i = _gdi_output_visual_glyphs(pdc, logical_glyphs, nr_glyphs, 
                    direction, cb_one_glyph, context);
        }
        else{ 
            /* not need reorder glyphs, output left to right.*/
            i = _gdi_output_glyphs_direct(pdc, text, text_len,
                    cb_one_glyph, context, FALSE, TRUE);
        }

        if (logical_glyphs) {
            free(logical_glyphs);
        }
    }
    else{
        if (!direction) { /* right to left, reverse text.*/
            for (i = text_len - 1; i >= 0; i--) {
                if (!(glyph_value = sbc_devfont->charset_ops->char_glyph_value
                    (NULL, 0, text + i, 1)))
                    return (text_len-i-1);

                glyph_type = sbc_devfont->charset_ops->glyph_type 
                    (glyph_value);

                if(!cb_one_glyph (context, glyph_value, glyph_type))
                    break;
            }
        }
        else {
            for (i = 0; i < text_len; i++) {
                if (!(glyph_value = sbc_devfont->charset_ops->char_glyph_value
                    (NULL, 0, text + i, 1)))
                    return i;

                glyph_type = sbc_devfont->charset_ops->glyph_type 
                    (glyph_value);

                if(!cb_one_glyph (context, glyph_value, glyph_type))
                    break;
            }
        }
    }
    return i;
}

static Glyph32* 
_gdi_get_glyphs_string_charbreak(PDC pdc, const unsigned char* text,
        int text_len, int* nr_glyphs, void* context)
{
    int i = 0;
    int len_cur_char;
    int left_bytes = text_len;
    int glyph_type;
    Glyph32  glyph_value;
    DEVFONT* sbc_devfont = pdc->pLogFont->sbc_devfont;
    DEVFONT* mbc_devfont = pdc->pLogFont->mbc_devfont;
    Glyph32 *logical_glyphs = NULL;

    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    int line_width = 0;
    BBOX bbox;

    int prev_len = 0;
    const unsigned char* prev_mchar = NULL;

    logical_glyphs = malloc(text_len * sizeof (Glyph32));

    while (left_bytes > 0){
        if (mbc_devfont) {
            len_cur_char = mbc_devfont->charset_ops->len_first_char
                ((const unsigned char*)text, left_bytes);
        }
        else len_cur_char = 0;

        if (len_cur_char > 0) {
            glyph_value = mbc_devfont->charset_ops->char_glyph_value
                (prev_mchar, prev_len, text, left_bytes);

            prev_mchar = text;
            prev_len = len_cur_char;
            logical_glyphs[i++] = SET_MBC_GLYPH(glyph_value);

            glyph_type = (*mbc_devfont->charset_ops->glyph_type)
                (glyph_value);
            if(!(glyph_type & MCHAR_TYPE_NOSPACING_MARK)){
                prev_mchar = text;
            }
            else{
                prev_len += len_cur_char;
            }

            line_width += _gdi_get_glyph_advance (pdc, logical_glyphs[i-1], 
                    TRUE, 0, 0, NULL, NULL, &bbox);
            left_bytes -= len_cur_char;
            text += len_cur_char;
        }
        else{
            len_cur_char = sbc_devfont->charset_ops->len_first_char
                ((const unsigned char*)text, left_bytes);

            if (len_cur_char > 0) {
                glyph_value = sbc_devfont->charset_ops->char_glyph_value
                    (NULL, 0, text, left_bytes);
                logical_glyphs[i++] = glyph_value;
                
                line_width += _gdi_get_glyph_advance (pdc, logical_glyphs[i-1], 
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
    if(nr_glyphs) *nr_glyphs = i;

    return logical_glyphs;
}


static int _gdi_get_nextword_width (PDC pdc, const unsigned char* pText, 
                int nCount, int* nChars, void* context)
{
    SIZE size;
    DEVFONT* sbc_devfont = pdc->pLogFont->sbc_devfont;
    DEVFONT* mbc_devfont = pdc->pLogFont->mbc_devfont;
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
_gdi_get_glyphs_string_pos_wordbreak(PDC pdc, const unsigned char* text,
        int text_len, int* nr_glyphs, void* context)
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
                Glyph32* logical_glyphs = 
                    _gdi_get_glyphs_string_charbreak(pdc, text,
                        text_len, nr_glyphs, ctxt);
                if(logical_glyphs) free(logical_glyphs);
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

static Glyph32* 
_gdi_get_glyphs_string_wordbreak(PDC pdc, const unsigned char* text,
        int text_len, int* nr_glyphs, void* context)
{
    DEVFONT* mbc_devfont = pdc->pLogFont->mbc_devfont;
    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    Glyph32 *logical_glyphs = NULL;
    
    if (mbc_devfont) {
        _gdi_get_glyphs_string_pos_wordbreak(pdc, text,
                text_len, nr_glyphs, context);
        logical_glyphs = _gdi_get_glyphs_string(pdc, text, ctxt->nCount,
                nr_glyphs);
    }
    return logical_glyphs;
}

static Glyph32* _gdi_get_glyphs_string_break(PDC pdc, const unsigned char* text,
        int text_len, int* nr_glyphs, void* context)
{
    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    if((ctxt->nFormat & DT_CHARBREAK) || (ctxt->nFormat & DT_SINGLELINE)){
        return _gdi_get_glyphs_string_charbreak(pdc, text,
                text_len, nr_glyphs, ctxt);
    }
    //else if(ctxt->nFormat & DT_WORDBREAK){
    else {
        return _gdi_get_glyphs_string_wordbreak(pdc, text,
                text_len, nr_glyphs, ctxt);
    }
}

static int 
_gdi_output_glyphs_direct_break(PDC pdc, const unsigned char* text, 
        int text_len, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    int nr_glyphs = 0;

    if(ctxt->nFormat & DT_WORDBREAK){
        _gdi_get_glyphs_string_pos_wordbreak(pdc, text,
                text_len, &nr_glyphs, context);

        return _gdi_output_glyphs_direct(pdc, text, ctxt->nCount,
                    cb_one_glyph, ctxt, FALSE, TRUE);
    }
    else {
        return _gdi_output_glyphs_direct(pdc, text, text_len,
                    cb_one_glyph, ctxt, TRUE, TRUE);
    }
}


static void _gdi_get_glyphs_string_sbc_rtol_wordbreak(PDC pdc,
      const unsigned char* text, int text_len, int* nr_glyphs, void* context)
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
_gdi_output_glyphs_direct_sbc_rtol_break(PDC pdc, const unsigned char* text, 
        int text_len, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    int nr_glyphs = 0;
    int i = 0;
    int glyph_type, glyph_value;
    DEVFONT* sbc_devfont = pdc->pLogFont->sbc_devfont;

    if(ctxt->nFormat & DT_WORDBREAK){
        _gdi_get_glyphs_string_sbc_rtol_wordbreak(pdc,
                text, text_len, &nr_glyphs, ctxt);
    }
    else {
        /* get break char's pos, only count no draw chars out. */
        _gdi_output_glyphs_direct(pdc, text, text_len,
                    cb_one_glyph, ctxt, TRUE, FALSE);
        /*
        return _gdi_output_glyphs_direct(pdc, text, ctxt->nCount,
                    cb_one_glyph, ctxt, TRUE, TRUE);
        */
    }

    if(ctxt->nCount <= 0) return 0;

    for (i = ctxt->nCount - 1; i >= 0; i--) {
        glyph_value = sbc_devfont->charset_ops->char_glyph_value
            (NULL, 0, text + i, 1);

        glyph_type = sbc_devfont->charset_ops->glyph_type 
            (glyph_value);

        if(!cb_one_glyph (context, glyph_value, glyph_type))
            break;
    }
    return nr_glyphs;
}

int _gdi_reorder_text_break (PDC pdc, const unsigned char* text,
        int text_len, BOOL direction, CB_ONE_GLYPH cb_one_glyph, void* context)
{
    int i = 0;
    DEVFONT* mbc_devfont = pdc->pLogFont->mbc_devfont;

    if (mbc_devfont) {
        int nr_glyphs = text_len;
        Glyph32 *logical_glyphs = NULL;

        if(mbc_devfont->charset_ops->bidi_reorder){

            logical_glyphs = _gdi_get_glyphs_string_break(pdc, text, text_len,
                    &nr_glyphs, context);
            mbc_devfont->charset_ops->bidi_reorder(logical_glyphs, nr_glyphs);

            if(!logical_glyphs) 
                return 0;

            i = _gdi_output_visual_glyphs(pdc, logical_glyphs, nr_glyphs, 
                    direction, cb_one_glyph, context);
        }
        /* not need reorder glyphs, right to left, should 
         * alloc glyphs range. */
        else if(!direction){ 
            logical_glyphs = _gdi_get_glyphs_string_break(pdc, text, text_len,
                    &nr_glyphs, context);
            if(!logical_glyphs) return 0;
            i = _gdi_output_visual_glyphs(pdc, logical_glyphs, nr_glyphs, 
                    direction, cb_one_glyph, context);
        }
        /* not need reorder glyphs, output left to right.*/
        else { 
            i = _gdi_output_glyphs_direct_break(pdc, text, text_len,
                    cb_one_glyph, context);
        }

        if (logical_glyphs) {
            free(logical_glyphs);
        }
    }
    else{
        /* note: follow sbc_devfont surpport not need to 
         * alloc mem for glyphs.*/
        if (!direction) { 
            /* right to left, reverse text.*/
            i = _gdi_output_glyphs_direct_sbc_rtol_break(pdc, text, 
                    text_len, cb_one_glyph, context);
        }
        else {
            i = _gdi_output_glyphs_direct_break(pdc, text, text_len,
                    cb_one_glyph, context);
        }
    }
    return i;
}

/* \bref: Get the logical glyph string and the glyphs map table, if
 * caller not malloc space for glyphs or glyphs_map, it will malloc
 * space inner.
 * 
 * \param  LOGFONT* log_font: The logical font.
 * \param  unsigned char* text: Input logical string, input.
 * \param  int text_len: Input logical string len, input.
 * \param  Glyph32** glyphs: logical glyph string, input/output.
 * \param  GLYPHMAPINFO* glyph_map: position mapping from Logical
 * glyphs strin to logical text, output.
 *
 * \param  int        : return logical glyph string len, output.
 */
int  GUIAPI BIDIGetTextLogicalGlyphs(
        LOGFONT*       log_font, 
        const char*    text,
        int            text_len, 
        Glyph32**      glyphs, 
        GLYPHMAPINFO** glyphs_map)
{
    int i = 0;
    int left_bytes = text_len;
    int prev_len = 0, len_cur_char = 0;
    const char* prev_mchar = NULL;
    Glyph32  glyph_value = INV_GLYPH_VALUE;
    Glyph32* glyph_string = NULL;
    GLYPHMAPINFO* map = NULL;

    if(glyphs && *glyphs == NULL){
        *glyphs = malloc(text_len * sizeof (Glyph32));
        memset(*glyphs, 0, text_len * sizeof(Glyph32));
    }

    if(glyphs_map && *glyphs_map == NULL){
        *glyphs_map = malloc(text_len * sizeof (GLYPHMAPINFO));
        memset(*glyphs_map, 0, text_len * sizeof(GLYPHMAPINFO));
    }
    if(glyphs)     glyph_string = *glyphs;
    if(glyphs_map) map = *glyphs_map;
    if(!map && !glyph_string) return 0;

    while (left_bytes > 0){
        len_cur_char = GetFirstMCharLen(log_font, text, left_bytes);

        glyph_value = GetGlyphValue(log_font, text, left_bytes,
                prev_mchar, prev_len);

        if(glyph_string) glyph_string[i] = glyph_value;
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

/* \bref: Get the visual glyph string and the glyphs map table, if
 * caller not malloc space for glyphs or glyphs_map, it will malloc
 * space inner.
 * 
 * \param  LOGFONT*   log_font: The logical font.
 * \param  unsigned char* text: Input logical string, input.
 * \param  int        text_len: Input logical string len, input.
 * \param  Glyph32**    glyphs: visual glyph string, input/output.
 * \param  GLYPHMAPINFO* glyph_map: position mapping from Visual glyphs 
 * string to Logical text, output.
 *
 * \param  return int : return visual glyph string len, output.
 */
int  GUIAPI BIDIGetTextVisualGlyphs(
        LOGFONT*       log_font,
        const char*    text,
        int            text_len, 
        Glyph32**      glyphs,
        GLYPHMAPINFO** glyphs_map)
{
    int nr_glyphs = 0;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;

    if(glyphs == NULL || glyphs_map == NULL)
        return nr_glyphs;

    nr_glyphs = BIDIGetTextLogicalGlyphs(log_font, text, text_len,
            glyphs, glyphs_map);

    if(mbc_devfont && mbc_devfont->charset_ops->bidi_reorder){
        bidi_map_reorder(mbc_devfont->charset_ops->name, *glyphs, 
                nr_glyphs, *glyphs_map);

        mbc_devfont->charset_ops->bidi_reorder(*glyphs, nr_glyphs);
    }
    return nr_glyphs;
}

/* bref: reorder the logical glyphs string to visual glyphs string.
 * if glyphs_map not NULL, get the visual glyphs map info.
 * 
 * \param  LOGFONT* log_font: The logical font.
 * \param  Glyph32* glyphs: visual glyph string, Input.
 * \param  int      nr_glyphs: glyph string len, Input.
 * \param  GLYPHMAPINFO* glyph_map: position mapping from logical glyphs.
 * string to Logical text, output.
 *
 * \param  Glyph32* : return visual glyph string, output.
 */
Glyph32* GUIAPI BIDILogGlyphs2VisGlyphs(
        LOGFONT* log_font,
        Glyph32* glyphs,
        int nr_glyphs,
        GLYPHMAPINFO* glyphs_map)
{
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    
    if(mbc_devfont && mbc_devfont->charset_ops->bidi_reorder){
        /* get the visual glyphs map from the logical glyphs map. */
        if(glyphs_map){
            bidi_map_reorder((const char*)mbc_devfont->charset_ops->name,
                    glyphs, nr_glyphs, glyphs_map);
        }
        mbc_devfont->charset_ops->bidi_reorder(glyphs, nr_glyphs);
    }

    return glyphs;
}


/*
 * \param  HDC*     hdc: The device context.
 * \param  Glyph32* glyph: Input glyph string, input.
 * \param  int      nr_glyphs: Input glyph string len, input.
 * \param  int      max_extent: Input glyph string extent value, input.
 * \param  int      size: Ouput the fit glyph's visual extent value, Output.
 *
 * \param  return   int return the fit glyph's visual_glyph_index.
 */
int GUIAPI GetGlyphsExtentPoint(
        HDC      hdc, 
        Glyph32* glyphs,
        int      nr_glyphs, 
        int      max_extent, 
        SIZE*    size)
{
    int i = 0;
    int advance = 0;
    int adv_x, adv_y;
    PDC pdc = dc_HDC2PDC(hdc);
    PLOGFONT log_font = pdc->pLogFont;
    DEVFONT* devfont = NULL; 
    int glyph_type = 0;

    size->cx = 0;
    size->cy = 0;

    while(i < nr_glyphs){
        devfont = SELECT_DEVFONT(log_font, glyphs[i]);
        glyph_type = devfont->charset_ops->glyph_type
            (glyphs[i]);


        if (glyph_type == MCHAR_TYPE_ZEROWIDTH 
                || (glyph_type & MCHAR_TYPE_NOSPACING_MARK)) {
            adv_x = adv_y = 0;
        }
        else {
            advance += _gdi_get_glyph_advance (pdc, glyphs[i], 
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    0, 0, &adv_x, &adv_y, NULL);

        }

        if(max_extent > 0 && advance > max_extent)
            break;

        size->cx += adv_x;
        size->cy += adv_y;
        i ++;
    }


    _gdi_calc_glyphs_size_from_two_points (pdc, 0, 0, 
            size->cx, size->cy, size);


    return i;
}

/*
 * \param  HDC*     hdc: The device context.
 * \param  Glyph32* glyphs: Input glyph string, input.
 * \param  int      nr_glyphs: Input glyph string len, input.
 * \param  SIZE     size: Ouput the fit glyph's extent value, Output.
 *
 * \return int: return the nr_glyphs glyph_string extent.
 */
int GUIAPI GetGlyphsExtent(
        HDC      hdc, 
        Glyph32* glyphs, 
        int      nr_glyphs, 
        SIZE*    size)
{
    int i = 0;
    int advance = 0;
    int adv_x, adv_y;
    int glyph_type;
    Glyph32 glyph_value = INV_GLYPH_VALUE;
    PDC pdc = dc_HDC2PDC(hdc);
    PLOGFONT log_font = pdc->pLogFont;
    DEVFONT* devfont = SELECT_DEVFONT(log_font, glyph_value);

    size->cx = 0;
    size->cy = 0;
    while(i < nr_glyphs){
        devfont = SELECT_DEVFONT(log_font, glyphs[i]);
        glyph_type = devfont->charset_ops->glyph_type
            (glyphs[i]);

        if (glyph_type == MCHAR_TYPE_ZEROWIDTH 
                || (glyph_type & MCHAR_TYPE_NOSPACING_MARK)) {
            adv_x = adv_y = 0;
        }
        else {
            advance += _gdi_get_glyph_advance (pdc, glyphs[i], 
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    0, 0, &adv_x, &adv_y, NULL);
        }
        size->cx += adv_x;
        size->cy += adv_y;
        i ++;
    }

    _gdi_calc_glyphs_size_from_two_points (pdc, 0, 0, 
            size->cx, size->cy, size);

    return advance;
}

typedef struct _RANGEINFO{
    int start;
    int end;
    int level;
    struct _RANGEINFO* next;
}RANGEINFO;

extern int GUIAPI GetLastMCharLen (PLOGFONT log_font, const char* mstr, int len);

static BOOL  check_sel_range (PLOGFONT log_font, const char* text,
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

static int* generate_ranges(PLOGFONT log_font, Glyph32* log_glyphs,
        int nr_glyphs, GLYPHMAPINFO* l_map,
        int l_start_index, int l_end_index, int* nr_ranges)
{
    int m = 0, i = 0, j = 0;
    int* p = NULL;
    int max_ranges = DEF_RANGES * 2;
    GLYPHMAPINFO* v_map = NULL;
    Uint8* embedding_level_list = NULL;

    BIDIGetLogicalEmbeddLevels(log_font, log_glyphs,
            nr_glyphs, &embedding_level_list);

    /* get all the logical ranges in the same language.
     * default malloc DEF_RANGES ranges.*/
    p = (int*) malloc(max_ranges * sizeof(int));
    memset(p, 0, max_ranges * sizeof(int));
    m = 0;
    p[m++] = l_start_index;
    for(i = l_start_index; i <= l_end_index; i++){
        if((i+1) <= l_end_index 
                && embedding_level_list[i] != embedding_level_list[i+1]){
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
    BIDILogGlyphs2VisGlyphs (log_font, log_glyphs,
                nr_glyphs, v_map);   

    *nr_ranges = m;

    /* translate the logical bytes index to visual glyph index. */
    for(j = 0; j < *nr_ranges; j++){
        for(i = 0; i < nr_glyphs; i++) {
            if(p[j] == v_map[i].byte_index){
                p[j] = i;
                break;
            }
        }
    }

    free(embedding_level_list);
    return p;
}


static BOOL check_range_edge(int index, int nr_glyphs, GLYPHMAPINFO* v_map)
{
    if (index < 0 || index > nr_glyphs)
        return FALSE;

    if(index >= nr_glyphs-1 || index == 0){
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

    /* set the visual glyphs range start_index < end_index.*/
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
    Glyph32* l_glyphs = NULL;
    GLYPHMAPINFO* v_map = NULL;
    GLYPHMAPINFO* l_map = NULL;
    int nr_glyphs = 0;

    *nr_ranges = 0;

    if(!check_sel_range(log_font, text, text_len,
                &start_index, &end_index)){
        return;
    }
    
    nr_glyphs = BIDIGetTextLogicalGlyphs(log_font, text, text_len,
            &l_glyphs, &l_map);

    v_map = l_map;
    BIDILogGlyphs2VisGlyphs (log_font, l_glyphs,
        nr_glyphs, v_map);   

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
    for(i = 0; i < nr_glyphs; i++){
        /* deal the start_index and end_index.*/
        if(v_map[i].byte_index <= start_index &&
                (v_map[i].byte_index+v_map[i].char_len) > start_index){
            p[m++] = i;
        }
        if(v_map[i].byte_index <= end_index &&
                (v_map[i].byte_index+v_map[i].char_len) > end_index){
            p[m++] = i;
        }

        if((i+1) >= nr_glyphs){
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
    free(l_glyphs);
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
void GUIAPI GetTextRangesLog2Vis(
        LOGFONT* log_font,
        char*    text, 
        int      text_len, 
        int      start_index, 
        int      end_index, 
        int**    ranges, 
        int*     nr_ranges)
{
    int  i = 0;
    int* p = NULL;
    Glyph32* l_glyphs = NULL;
    GLYPHMAPINFO* l_map = NULL;
    int l_start_index = start_index, l_end_index = end_index;
    int nr_glyphs = 0;

    *nr_ranges = 0;
    /* it is not bidi string. */
    if(!log_font->mbc_devfont || (log_font->mbc_devfont && 
                !log_font->mbc_devfont->charset_ops->bidi_reorder)){
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
    
    nr_glyphs = BIDIGetTextLogicalGlyphs(log_font, text, text_len,
            &l_glyphs, &l_map);

    for(i = 0; i < nr_glyphs; i++) {
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

    p = generate_ranges(log_font, l_glyphs, nr_glyphs, l_map,
            l_start_index, l_end_index, nr_ranges);

    computer_ranges(p, nr_ranges);

    /* check the ranges edges, +1 when edges.
     * after generate_ranges, l_map is translate to v_map*/
    for(i = 1; i < *nr_ranges; i+=2){
        if(p[i] != 0 && check_range_edge(p[i], nr_glyphs, l_map)){
            p[i] += 1;
        }
    }

    *ranges = p;
    *nr_ranges /= 2;

    free(l_map);
    free(l_glyphs);
}

/*
 * bref: get the logical embedding levels for the logical glyph string
 * and generate runs by embedding levels, then for reorder to get
 * visual glyph string.
 *
 * \param  LOGFONT* log_font: The logical font.
 * \param  Glyph32* glyphs: Input logical glyph string, input.
 * \param  int      nr_glyphs: Input logical string len, input.
 * \param  Uint8**  embedding_level_list: embedding level Logical, output.
 *
 * \param  return void.
 */
void GUIAPI BIDIGetLogicalEmbeddLevels(
        LOGFONT*  log_font, 
        Glyph32*  glyphs,
        int       nr_glyphs,
        Uint8**   embedding_level_list)
{
    int i = 0;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;

    if(*embedding_level_list == NULL)
        *embedding_level_list = malloc(nr_glyphs * sizeof (Uint8));

    if (mbc_devfont && mbc_devfont->charset_ops->bidi_reorder) {
        bidi_get_embeddlevels(mbc_devfont->charset_ops->name, glyphs, nr_glyphs,
                *embedding_level_list, 0);
    }
    else{
        for(i = 0; i < nr_glyphs; i++){
            (*embedding_level_list)[i] = i;
        }
    }
}

/*
 * \param  LOGFONT*  log_font: The logical font.
 * \param  Glyph32*  glyphs: Input logical glyph string, input.
 * \param  int       nr_glyphs: Input logical string len, input.
 * \param  Uint8     *embedding_level_list: embedding level logical 
 * to visual, output.
 *
 * \param  return void.
 */
void GUIAPI BIDIGetVisualEmbeddLevels(
        LOGFONT* log_font, 
        Glyph32* glyphs,
        int      nr_glyphs,
        Uint8**  embedding_level_list)
{
    int i = 0;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;

    if(*embedding_level_list == NULL)
        *embedding_level_list = malloc(nr_glyphs * sizeof (Uint8));

    if (mbc_devfont && mbc_devfont->charset_ops->bidi_reorder) {
        bidi_get_embeddlevels(mbc_devfont->charset_ops->name, glyphs, nr_glyphs,
                *embedding_level_list, 1);
    }
    else{
        for(i = 0; i < nr_glyphs; i++){
            (*embedding_level_list)[i] = i;
        }
    }
}
