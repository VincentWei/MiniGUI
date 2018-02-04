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
** textout.c: Implementation of TextOut and related functions.
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
#include "cursor.h"

#ifdef _MGCHARSET_UNICODE
extern size_t __mg_strlen (PLOGFONT log_font, const char* mstr);
extern char* __mg_strnchr (PLOGFONT log_font, const char* s, 
                size_t n, int c, int* cl);
extern int __mg_substrlen (PLOGFONT log_font, const char* text, int len, 
                int delimiter, int* nr_delim);

static inline BOOL is_utf16_logfont (PDC pdc)
{
    DEVFONT* mbc_devfont; 
    mbc_devfont = pdc->pLogFont->mbc_devfont;
    if (mbc_devfont && strstr (mbc_devfont->charset_ops->name, "UTF-16")) {
        return TRUE;
    }

    return FALSE;
}

#else

static inline size_t __mg_strlen (PLOGFONT log_font, const char* mstr)
{
    return strlen (mstr);
}

static inline char* __mg_strnchr (PLOGFONT logfont, const char* s, 
                size_t n, int c, int* cl)
{
    *cl = 1;
    return strnchr (s, n, c);
}

static inline int __mg_substrlen (PLOGFONT logfont, const char* text, int len, 
                int delimiter, int* nr_delim)
{
    return substrlen (text, len, delimiter, nr_delim);
}

static inline BOOL is_utf16_logfont (PDC pdc)
{
    return FALSE;
}

#endif

typedef struct _DRAW_GLYPHS_CTXT {
    HDC hdc; 
    int x; 
    int y;  
    int advance;
} DRAW_GLYPHS_CTXT;

static BOOL cb_draw_glyph (void* context, Glyph32 glyph_value, int glyph_type)
{
    DRAW_GLYPHS_CTXT* ctxt = (DRAW_GLYPHS_CTXT*)context;
    int adv_x, adv_y;
    int bkmode;

    if (glyph_type == MCHAR_TYPE_ZEROWIDTH) {
        adv_x = adv_y = 0;
    }
    else if (glyph_type == MCHAR_TYPE_VOWEL){
        bkmode = GetBkMode (ctxt->hdc);
        //SetBkMode (ctxt->hdc, BM_TRANSPARENT);
        DrawGlyph (ctxt->hdc, ctxt->x, ctxt->y, glyph_value, &adv_x, &adv_y);
        SetBkMode (ctxt->hdc, bkmode);
        adv_x = 0;
        adv_y = 0;
    }
    else {
        ctxt->advance += DrawGlyph (ctxt->hdc, ctxt->x, ctxt->y, glyph_value, &adv_x, &adv_y);
    }

    ctxt->x += adv_x;
    ctxt->y += adv_y;

    return TRUE;
}

int DrawGlyphString (HDC hdc, int startx, int starty, Glyph32* glyph_string, int len, int* adv_x, int* adv_y)
{
    DRAW_GLYPHS_CTXT ctxt = {hdc, startx, starty, 0};

    if ((((PDC)hdc)->ta_flags & TA_X_MASK) == TA_LEFT)
        _gdi_output_visual_glyphs((PDC)hdc, glyph_string, 
                len, TRUE, cb_draw_glyph, &ctxt);
    else 
        _gdi_output_visual_glyphs((PDC)hdc, glyph_string, 
                len, FALSE, cb_draw_glyph, &ctxt);

    if (adv_x)
        *adv_x = ctxt.x - startx;

    if (adv_y)
        *adv_y = ctxt.y - starty;

    return ctxt.advance;
}

typedef struct _TEXTOUT_CTXT
{
    PDC pdc;
    int x; 
    int y; 
    int advance;
    BOOL only_extent;
} TEXTOUT_CTXT;

static BOOL cb_textout (void* context, Glyph32 glyph_value, int glyph_type)
{
    TEXTOUT_CTXT* ctxt = (TEXTOUT_CTXT*)context;
    int adv_x, adv_y;
    int bkmode;

    if (glyph_type == MCHAR_TYPE_ZEROWIDTH) {
        adv_x = adv_y = 0;
    }
    else if (glyph_type == MCHAR_TYPE_VOWEL){
        if (!ctxt->only_extent)
        {
            bkmode = ctxt->pdc->bkmode;
            //ctxt->pdc->bkmode = BM_TRANSPARENT;
            _gdi_draw_one_glyph (ctxt->pdc, glyph_value, 
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT, 
                    ctxt->x, ctxt->y, &adv_x, &adv_y);
            ctxt->pdc->bkmode = bkmode;

        }
            //adv_x = adv_y = 0;
            adv_x = 0;
    }
    else {
        if (ctxt->only_extent)
            ctxt->advance += _gdi_get_glyph_advance (ctxt->pdc, glyph_value, 
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT, 
                0, 0, &adv_x, &adv_y, NULL);
        else 
            ctxt->advance += _gdi_draw_one_glyph (ctxt->pdc, glyph_value, 
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT, 
                ctxt->x, ctxt->y, &adv_x, &adv_y);
    }

    ctxt->x += adv_x;
    ctxt->y += adv_y;

    return TRUE;
}

int _gdi_text_out (PDC pdc, int x, int y, 
                const unsigned char* text, int len, POINT* cur_pos)
{
    TEXTOUT_CTXT ctxt;

    /* convert to the start point on baseline. */
    _gdi_get_baseline_point (pdc, &x, &y);

    ctxt.pdc = pdc;
    ctxt.x = x;
    ctxt.y = y;
    ctxt.advance = 0;
    ctxt.only_extent = FALSE;

    _gdi_start_new_line (pdc);
    _gdi_reorder_text (pdc, text, len, 
        (pdc->ta_flags & TA_X_MASK) != TA_RIGHT, cb_textout, &ctxt);

    if (cur_pos) {
        cur_pos->x = ctxt.x;
        cur_pos->y = ctxt.y;
    }

    return ctxt.advance;
}

int GUIAPI TextOutLen (HDC hdc, int x, int y, const char* spText, int len)
{
    PDC pdc;
    POINT cur_pos;
    int advance;

    if (!spText) return 0;

    pdc = dc_HDC2PDC (hdc);

    if (len < 0) len = __mg_strlen (pdc->pLogFont, spText);
    if (len == 0) return 0;

    if ((pdc->ta_flags & TA_CP_MASK) == TA_UPDATECP) {
        x = pdc->CurTextPos.x;
        y = pdc->CurTextPos.y;
    }

    /* Transfer logical to device to screen here. */
    coor_LP2SP (pdc, &x, &y);

    pdc->rc_output = pdc->DevRC;
    advance = _gdi_text_out (pdc, x, y, 
                    (const unsigned char*)spText, len, &cur_pos);

    coor_SP2LP (pdc, &cur_pos.x, &cur_pos.y);
    pdc->CurTextPos.x = cur_pos.x;
    pdc->CurTextPos.y = cur_pos.y;

    return advance;
}

int _gdi_get_text_extent (PDC pdc, const unsigned char* text, int len, 
                SIZE* size)
{
    TEXTOUT_CTXT ctxt;

    ctxt.pdc = pdc;
    ctxt.x = 0;
    ctxt.y = 0;
    ctxt.advance = 0;
    ctxt.only_extent = TRUE;

    _gdi_start_new_line (pdc);
    _gdi_reorder_text (pdc, text, len, 
        (pdc->ta_flags & TA_X_MASK) != TA_RIGHT, cb_textout, &ctxt);

    if (size) {
        size->cx = ABS (ctxt.x);
        size->cy = ABS (ctxt.y) + pdc->pLogFont->size;
    }

    return ctxt.advance;
}

int GUIAPI GetTextExtent (HDC hdc, const char* spText, int len, SIZE* pSize)
{
    PDC pdc;

    /* set size to zero first */
    pSize->cx = pSize->cy = 0;

    if (!spText) return 0;

    pdc = dc_HDC2PDC(hdc);

    if (len < 0) len = __mg_strlen (pdc->pLogFont, spText);
    if (len == 0) return 0;

    return _gdi_get_text_extent (pdc, (const unsigned char*)spText, len, pSize);
}

static const char *strdot = "...";
#define STRDOT_LEN 3

#if 0
static int str_omitted_3dot (char *dest, const char *src, 
                             int *pos_chars, int reserve)
{
    int nbytes = 0;
    if (reserve >= 1) {
        nbytes = *(pos_chars + reserve);
        memcpy (dest, src, nbytes);
    }
    strcpy ((dest + nbytes), strdot);
    return nbytes + 3;
}
#endif

typedef struct _TEXTOUTOMITTED_CTXT
{
    PDC pdc;
    int x; 
    int y; 
    int advance;
    Uint32 max_extent;
} TEXTOUTOMITTED_CTXT;

static BOOL cb_textout_omitted (void* context, Glyph32 glyph_value, int glyph_type)
{
    TEXTOUTOMITTED_CTXT* ctxt = (TEXTOUTOMITTED_CTXT*)context;
    int adv_x, adv_y;
    int glyph_advance = 0; 
    //BBOX bbox;
    int bkmode = ctxt->pdc->bkmode;

    if (glyph_type == MCHAR_TYPE_ZEROWIDTH) {
        adv_x = adv_y = 0;
    }
    else if(glyph_type == MCHAR_TYPE_VOWEL) {
        //ctxt->pdc->bkmode = BM_TRANSPARENT;
        _gdi_draw_one_glyph (ctxt->pdc, glyph_value, 
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT, 
                ctxt->x, ctxt->y, &adv_x, &adv_y);
        ctxt->pdc->bkmode = bkmode;
        adv_x = adv_y = 0;
    }
    else {
        /* if this glyph can be visible. */
        glyph_advance = _gdi_get_glyph_advance (ctxt->pdc, glyph_value, 
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT, 
                0, 0, &adv_x, &adv_y, NULL);

        if((ctxt->advance + glyph_advance) > ctxt->max_extent)
            return FALSE;

        ctxt->advance += _gdi_draw_one_glyph (ctxt->pdc, glyph_value, 
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT, 
                ctxt->x, ctxt->y, &adv_x, &adv_y);
    }

    ctxt->x += adv_x;
    ctxt->y += adv_y;

    return TRUE;
}

int _gdi_textout_omitted (PDC pdc, int x, int y, 
                const unsigned char* text, int len, int max_extent, POINT* cur_pos)
{
    TEXTOUTOMITTED_CTXT ctxt;

    /* convert to the start point on baseline. */
    _gdi_get_baseline_point (pdc, &x, &y);

    ctxt.pdc = pdc;
    ctxt.x = x;
    ctxt.y = y;
    ctxt.advance = 0;
    ctxt.max_extent = max_extent;

    _gdi_start_new_line (pdc);
    _gdi_reorder_text (pdc, text, len, 
        (pdc->ta_flags & TA_X_MASK) != TA_RIGHT, cb_textout_omitted, &ctxt);

    if (cur_pos) {
        cur_pos->x = ctxt.x;
        cur_pos->y = ctxt.y;
    }

    return ctxt.advance;
}

int GUIAPI TextOutOmitted (HDC hdc, int x, int y, 
                           const char *mtext, int len, int max_extent)
{
    PDC pdc;
    POINT cur_pos;
    int advance;
    int orig_y = y;
    SIZE size_dot;
    SIZE size_text;

    if (!mtext) return 0;

    pdc = dc_HDC2PDC (hdc);

    if (len < 0) len = __mg_strlen (pdc->pLogFont, mtext);
    if (len == 0) return 0;

    if ((pdc->ta_flags & TA_CP_MASK) == TA_UPDATECP) {
        x = pdc->CurTextPos.x;
        y = pdc->CurTextPos.y;
    }

    GetTextExtent (hdc, mtext, len, &size_text);
    if (max_extent >= size_text.cx) {
        return TextOutLen (hdc, x, y, mtext, len);
    }

    /* Transfer logical to device to screen here. */
    coor_LP2SP (pdc, &x, &y);
    pdc->rc_output = pdc->DevRC;

    GetTextExtent (hdc, strdot, STRDOT_LEN, &size_dot);
    if (max_extent <= size_dot.cx) {
        if (max_extent == size_dot.cx) {
            return TextOutLen (hdc, x, y, strdot, STRDOT_LEN);
        }
        /* nothing can display */
        return 0;
    }
    max_extent -= size_dot.cx;
    advance = _gdi_textout_omitted(pdc, x, y, 
                    (const unsigned char*)mtext, len, max_extent, &cur_pos);

    coor_SP2LP (pdc, &cur_pos.x, &cur_pos.y);
    /* output the dot, note: coor_SP2LP should call first.*/
    TextOutLen (hdc, cur_pos.x, orig_y, strdot, STRDOT_LEN);

    pdc->CurTextPos.x = cur_pos.x;
    pdc->CurTextPos.y = cur_pos.y;

    return advance + size_dot.cx;
}

#undef STRDOT_LEN

int GUIAPI GetTextExtentPoint (HDC hdc, const char* text, int len, 
                int max_extent, 
                int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size)
{
    PDC pdc = dc_HDC2PDC (hdc);
    LOGFONT* log_font = pdc->pLogFont;
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    int len_cur_char, advance_cur_char;
    int left_bytes = len;
    int char_count = 0;
    Glyph32 glyph_value;

    /* set size to zero first */
    size->cx = size->cy = 0;

    /* bidi is not supported */
    if (mbc_devfont && mbc_devfont->charset_ops->bidi_reorder)
        return -1;

    _gdi_start_new_line(pdc);

    size->cy = log_font->size + pdc->alExtra + pdc->blExtra;
    size->cx = 0;
    while (left_bytes > 0) {
        if (pos_chars)
            pos_chars[char_count] = len - left_bytes;
        if (dx_chars)
            dx_chars[char_count] = size->cx;

        if (mbc_devfont && 
                (len_cur_char = mbc_devfont->charset_ops->len_first_char 
                    ((const unsigned char*)text, left_bytes)) > 0) {
            
            glyph_value = (*mbc_devfont->charset_ops->char_glyph_value)(NULL,
                    0, (const unsigned char*)text, 0);

            advance_cur_char = _gdi_get_glyph_advance (pdc, glyph_value | 0x80000000, 
                (pdc->ta_flags & TA_X_MASK) == TA_LEFT, 
                0, 0, NULL, NULL, NULL);
        }
        else {
            if ((len_cur_char = sbc_devfont->charset_ops->len_first_char
                        ((const unsigned char*)text, left_bytes)) > 0) {

                glyph_value = (*sbc_devfont->charset_ops->char_glyph_value)(
                        NULL, 0, (const unsigned char*)text, 0);
                advance_cur_char = _gdi_get_glyph_advance (pdc, glyph_value, 
                        (pdc->ta_flags & TA_X_MASK) == TA_LEFT, 
                        0, 0, NULL, NULL, NULL);
            }
            else
                break;
        }

        advance_cur_char += pdc->cExtra;

        if (max_extent > 0 && (size->cx + advance_cur_char) > max_extent) {
            goto ret;
        }

        char_count ++;
        size->cx += advance_cur_char;
        left_bytes -= len_cur_char;
        text += len_cur_char;
    }

ret:
    if (fit_chars) *fit_chars = char_count;
    return len - left_bytes;
}

