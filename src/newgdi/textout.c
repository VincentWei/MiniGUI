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
    mbc_devfont = pdc->pLogFont->devfonts[1];
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
    int x, y;
    int last_x, last_y;
    int advance;
    int last_adv;
} DRAW_GLYPHS_CTXT;

static BOOL cb_draw_glyph (void* context, Glyph32 glyph_value, unsigned int char_type)
{
    DRAW_GLYPHS_CTXT* ctxt = (DRAW_GLYPHS_CTXT*)context;
    int adv_x, adv_y;

    if (check_zero_width(char_type)) {
        adv_x = adv_y = 0;
    }
    else if (check_vowel(char_type)) {
        int bkmode = GetBkMode (ctxt->hdc);
        SetBkMode (ctxt->hdc, BM_TRANSPARENT);
#if 0
        DrawGlyph (ctxt->hdc, ctxt->x, ctxt->y, glyph_value, &adv_x, &adv_y);
#else
        DrawVowel (ctxt->hdc, ctxt->last_x, ctxt->last_y, glyph_value,
                ctxt->last_adv);
#endif
        SetBkMode (ctxt->hdc, bkmode);
        adv_x = 0;
        adv_y = 0;
    }
    else {
        ctxt->advance += DrawGlyph (ctxt->hdc, ctxt->x, ctxt->y,
                glyph_value, &adv_x, &adv_y);
    }

    ctxt->last_x = ctxt->x;
    ctxt->last_y = ctxt->y;
    ctxt->last_adv = adv_x;
    ctxt->x += adv_x;
    ctxt->y += adv_y;

    return TRUE;
}

typedef struct _TEXTOUT_CTXT
{
    PDC pdc;
    int x, y;
    int last_x, last_y;
    int advance;
    int last_adv;
    BOOL only_extent;
} TEXTOUT_CTXT;

static BOOL cb_textout (void* context, Glyph32 glyph_value,
        unsigned int char_type)
{
    TEXTOUT_CTXT* ctxt = (TEXTOUT_CTXT*)context;
    int adv_x, adv_y;

    if (check_zero_width (char_type)) {
        adv_x = adv_y = 0;
    }
    else if (check_vowel(char_type)) {
        if (!ctxt->only_extent) {
#if 0
            int bkmode = ctxt->pdc->bkmode;
            ctxt->pdc->bkmode = BM_TRANSPARENT;
            _gdi_draw_one_glyph (ctxt->pdc, glyph_value,
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    ctxt->x, ctxt->y, &adv_x, &adv_y);
            ctxt->pdc->bkmode = bkmode;
#else
            _gdi_draw_one_vowel (ctxt->pdc, glyph_value,
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    ctxt->last_x, ctxt->last_y, ctxt->last_adv);
#endif
        }
        adv_x = adv_y = 0;
    }
    else {
        if (ctxt->only_extent)
            ctxt->advance += _gdi_get_glyph_advance (ctxt->pdc, glyph_value,
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                0, 0, &adv_x, &adv_y, NULL);
        else {
            int bkmode = ctxt->pdc->bkmode;
            ctxt->pdc->bkmode = ctxt->pdc->bkmode_set;
            ctxt->advance += _gdi_draw_one_glyph (ctxt->pdc, glyph_value,
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                ctxt->x, ctxt->y, &adv_x, &adv_y);
            ctxt->pdc->bkmode = bkmode;
        }
    }

    ctxt->last_x = ctxt->x;
    ctxt->last_y = ctxt->y;
    ctxt->last_adv = adv_x;
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
    ctxt.last_x = x;
    ctxt.last_y = y;
    ctxt.advance = 0;
    ctxt.last_adv = 0;
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
    ctxt.last_x = 0;
    ctxt.last_y = 0;
    ctxt.advance = 0;
    ctxt.last_adv = 0;
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
    int x, y;
    int last_x, last_y;
    int advance;
    int last_adv;
    Uint32 max_extent;
} TEXTOUTOMITTED_CTXT;

static BOOL
cb_textout_omitted (void* context, Glyph32 glyph_value, unsigned int char_type)
{
    TEXTOUTOMITTED_CTXT* ctxt = (TEXTOUTOMITTED_CTXT*)context;
    int adv_x, adv_y;
    int glyph_advance = 0;

    if (check_zero_width(char_type)) {
        adv_x = adv_y = 0;
    }
    else if (check_vowel(char_type)) {
#if 0
        int bkmode = ctxt->pdc->bkmode;
        ctxt->pdc->bkmode = BM_TRANSPARENT;
        _gdi_draw_one_glyph (ctxt->pdc, glyph_value,
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                ctxt->x, ctxt->y, &adv_x, &adv_y);
        ctxt->pdc->bkmode = bkmode;
#else
        _gdi_draw_one_vowel (ctxt->pdc, glyph_value,
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                ctxt->last_x, ctxt->last_y, ctxt->last_adv);
#endif
        adv_x = adv_y = 0;
    }
    else {
        /* if this glyph can be visible. */
        glyph_advance = _gdi_get_glyph_advance (ctxt->pdc, glyph_value,
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                0, 0, &adv_x, &adv_y, NULL);

        if((ctxt->advance + glyph_advance) > ctxt->max_extent)
            return FALSE;

        int bkmode = ctxt->pdc->bkmode;
        ctxt->pdc->bkmode = ctxt->pdc->bkmode_set;
        ctxt->advance += _gdi_draw_one_glyph (ctxt->pdc, glyph_value,
                (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                ctxt->x, ctxt->y, &adv_x, &adv_y);
        ctxt->pdc->bkmode = bkmode;
    }

    ctxt->last_x = ctxt->x;
    ctxt->last_y = ctxt->y;
    ctxt->last_adv = adv_x;
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
    ctxt.last_x = x;
    ctxt.last_y = y;
    ctxt.advance = 0;
    ctxt.last_adv = 0;
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

#define SZ_BUFF_IN_STACK 256

static int get_text_extent_point_for_bidi(HDC hdc,
        const char* text, int len, int max_extent,
        int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size)
{
    PDC pdc = dc_HDC2PDC(hdc);
    LOGFONT *log_font = pdc->pLogFont;
    Achar32 achars_buff[SZ_BUFF_IN_STACK];
    ACHARMAPINFO achars_map_buff[SZ_BUFF_IN_STACK];
    int dx_achars_buff[SZ_BUFF_IN_STACK];
    Achar32 *achars;
    ACHARMAPINFO* achars_map;
    int *dx_achars = NULL;

    if (len < 0)
        len = strlen(text);
    if (len <= SZ_BUFF_IN_STACK) {
        achars = achars_buff;
        achars_map = achars_map_buff;
    }
    else {
        achars = NULL;
        achars_map = NULL;
    }

    int nr_fit_achars = 0;

    int nr_achars = BIDIGetTextVisualAChars(log_font, text, len,
            &achars, &achars_map);
    if (nr_achars <= 0) {
        goto done;
    }

    if (nr_achars <= SZ_BUFF_IN_STACK) {
        dx_achars = dx_achars_buff;
    }
    else {
        dx_achars = malloc(sizeof(int) * nr_achars);
    }

    if (dx_chars == NULL || achars == NULL || achars_map == NULL)
        goto done;

    nr_fit_achars = GetACharsExtentPointEx(hdc, achars, nr_achars,
            max_extent, dx_achars, size);

    if (fit_chars) {
        *fit_chars = nr_fit_achars;
    }

    if (pos_chars || dx_chars) {
        for (int i = 0; i < nr_fit_achars; i++) {
            if (pos_chars) {
                pos_chars[i] = achars_map[i].byte_index;
            }

            if (dx_chars) {
                dx_chars[i] = dx_achars[i];
            }
        }
    }

done:
    if (achars != NULL && achars != achars_buff)
        free(achars);
    if (achars_map != NULL && achars_map != achars_map_buff)
        free(achars_map);
    if (dx_achars != NULL && dx_achars != dx_achars_buff)
        free(dx_achars);
    return nr_fit_achars;
}

int GUIAPI GetTextExtentPoint (HDC hdc,
        const char* text, int len, int max_extent,
        int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size)
{
    PDC pdc = dc_HDC2PDC (hdc);
    LOGFONT* log_font = pdc->pLogFont;
    DEVFONT* sbc_devfont = log_font->devfonts[0];
    DEVFONT* mbc_devfont = log_font->devfonts[1];
    int len_cur_char, advance_cur_char;
    int left_bytes = len;
    int char_count = 0;
    Achar32 chv;
    Glyph32 gv;

    /* set size to zero first */
    size->cx = size->cy = 0;

    /* This function does not support BIDI */
    if (mbc_devfont &&
            (mbc_devfont->charset_ops->legacy_bidi || pdc->bidi_flags) &&
            mbc_devfont->charset_ops->bidi_char_type) {
        get_text_extent_point_for_bidi(pdc, text, len, max_extent,
                fit_chars, pos_chars, dx_chars, size);
        return -1;
    }

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

            chv = (*mbc_devfont->charset_ops->get_char_value)(NULL,
                    0, (const unsigned char*)text, 0);
            chv = SET_MBCHV(chv);

            gv = GetGlyphValueAlt(pdc->pLogFont, chv);
            advance_cur_char = _gdi_get_glyph_advance (pdc, gv,
                (pdc->ta_flags & TA_X_MASK) == TA_LEFT,
                0, 0, NULL, NULL, NULL);
        }
        else {
            if ((len_cur_char = sbc_devfont->charset_ops->len_first_char
                        ((const unsigned char*)text, left_bytes)) > 0) {

                chv = (*sbc_devfont->charset_ops->get_char_value)(
                        NULL, 0, (const unsigned char*)text, 0);
                gv = GetGlyphValueAlt(pdc->pLogFont, chv);
                advance_cur_char = _gdi_get_glyph_advance (pdc, gv,
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

int GUIAPI DrawACharString (HDC hdc, int startx, int starty,
        Achar32* achars, int len, int* adv_x, int* adv_y)
{
    DRAW_GLYPHS_CTXT ctxt = {hdc, startx, starty, startx, starty, 0};

    if ((((PDC)hdc)->ta_flags & TA_X_MASK) == TA_LEFT)
        _gdi_output_visual_achars((PDC)hdc, achars,
                len, TRUE, cb_draw_glyph, &ctxt);
    else
        _gdi_output_visual_achars((PDC)hdc, achars,
                len, FALSE, cb_draw_glyph, &ctxt);

    if (adv_x)
        *adv_x = ctxt.x - startx;

    if (adv_y)
        *adv_y = ctxt.y - starty;

    return ctxt.advance;
}

int GUIAPI GetACharsExtentPointEx (HDC hdc, Achar32* achars, int nr_achars,
        int max_extent, int *dx_achars, SIZE* size)
{
    int i = 0;
    int advance = 0;
    int adv_x, adv_y;
    PDC pdc = dc_HDC2PDC(hdc);
    PLOGFONT log_font = pdc->pLogFont;
    DEVFONT* devfont = NULL;
    unsigned int char_type = 0;

    size->cx = 0;
    size->cy = 0;

    while (i < nr_achars){
        if (dx_achars) {
            dx_achars[i] = advance;
        }

        devfont = SELECT_DEVFONT_BY_ACHAR(log_font, achars[i]);
        char_type = devfont->charset_ops->char_type(achars[i]);

        if (check_zero_width(char_type)) {
            adv_x = adv_y = 0;
        }
        else {
            Glyph32 gv;

            if ((gv = GetGlyphValueAlt(log_font, achars[i])) == INV_GLYPH_VALUE) {
                _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                    __FUNCTION__, achars[i]);
                break;
            }

            advance += _gdi_get_glyph_advance (pdc, gv,
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    0, 0, &adv_x, &adv_y, NULL);
        }

        if (max_extent > 0 && advance > max_extent)
            break;

        size->cx += adv_x;
        size->cy += adv_y;
        i++;
    }

    _gdi_calc_glyphs_size_from_two_points (pdc, 0, 0,
            size->cx, size->cy, size);

    return i;
}

int GUIAPI GetACharsExtentPoint(HDC hdc, Achar32* achars, int nr_achars,
        int max_extent, SIZE* size)
{
    return GetACharsExtentPointEx(hdc, achars, nr_achars, max_extent,
            NULL, size);
}

int GUIAPI GetACharsExtent(HDC hdc, Achar32* achars, int nr_achars, SIZE* size)
{
    int i = 0;
    int advance = 0;
    int adv_x, adv_y;
    unsigned int char_type;
    PDC pdc = dc_HDC2PDC(hdc);
    PLOGFONT log_font = pdc->pLogFont;
    DEVFONT* devfont;

    size->cx = 0;
    size->cy = 0;
    while (i < nr_achars) {
        devfont = SELECT_DEVFONT_BY_GLYPH(log_font, achars[i]);
        char_type = devfont->charset_ops->char_type(achars[i]);

        if (check_zero_width(char_type) || check_vowel(char_type)) {
            adv_x = adv_y = 0;
        }
        else {
            Glyph32 gv;

            if ((gv = GetGlyphValueAlt(log_font, achars[i])) == INV_GLYPH_VALUE) {
                _DBG_PRINTF("%s: got a bad glyph value from achar: %x\n",
                    __FUNCTION__, achars[i]);
                break;
            }

            advance += _gdi_get_glyph_advance (pdc, gv,
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

