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
** tabbedtextout.c: Implementaion of TabbedTextOut and related functions.
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


typedef struct _TABBEDTEXTOUT_CTXT
{
    PDC pdc;
    int start_x, start_y;
    int max_advance;
    int max_x, max_y;
    int tab_width;
    int line_height;

    int x, y;
    int advance;

    BOOL only_extent;
} TABBEDTEXTOUT_CTXT;

static BOOL cb_tabbedtextout (void* context, Glyph32 glyph_value,
                unsigned int char_type)
{
    TABBEDTEXTOUT_CTXT* ctxt = (TABBEDTEXTOUT_CTXT*)context;
    int adv_x, adv_y;
    BBOX bbox;

    switch (char_type & ACHARTYPE_BASIC_MASK) {
        case ACHAR_BASIC_ZEROWIDTH:
        case ACHAR_BASIC_CTRL1:
        case ACHAR_BASIC_CTRL2:
            adv_x = adv_y = 0;
            break;

        case ACHAR_BASIC_HT:
            _gdi_start_new_line (ctxt->pdc);
            if (ctxt->only_extent) {
                ctxt->advance += _gdi_get_null_glyph_advance (ctxt->pdc,
                    ctxt->tab_width,
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    ctxt->x, ctxt->y, &adv_x, &adv_y);
            }
            else {
                ctxt->advance += _gdi_draw_null_glyph (ctxt->pdc,
                    ctxt->tab_width,
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    ctxt->x, ctxt->y, &adv_x, &adv_y);
            }
            break;

        case ACHAR_BASIC_VOWEL:
            if (!ctxt->only_extent) {
                int bkmode = ctxt->pdc->bkmode;
                ctxt->pdc->bkmode = BM_TRANSPARENT;
                _gdi_draw_one_glyph (ctxt->pdc, glyph_value,
                        (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                        ctxt->x, ctxt->y, &adv_x, &adv_y);
                ctxt->pdc->bkmode = bkmode;
            }
            adv_x = adv_y = 0;
            break;

        default:
            if (ctxt->only_extent) {
                ctxt->advance += _gdi_get_glyph_advance (ctxt->pdc,
                    glyph_value,
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    ctxt->x, ctxt->y, &adv_x, &adv_y, &bbox);
            }
            else {
                int bkmode = ctxt->pdc->bkmode;
                ctxt->pdc->bkmode = ctxt->pdc->bkmode_set;
                ctxt->advance += _gdi_draw_one_glyph (ctxt->pdc, glyph_value,
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    ctxt->x, ctxt->y, &adv_x, &adv_y);
                ctxt->pdc->bkmode = bkmode;
            }
            break;
    }

    ctxt->x += adv_x;
    ctxt->y += adv_y;

    return TRUE;
}

typedef struct _TABBEDTEXTOUTEX_CTXT
{
    PDC pdc;
    /* para for new line. */
    int start_x, start_y;
    int line_height;
    int max_advance;

    /* para for tab. */
    int tab_width;
    int nr_tab;
    int nTabs, nTabOrig;
    int *pTabPos;

    /* para for current char. */
    int x, y;
    int advance;
} TABBEDTEXTOUTEX_CTXT;

static BOOL cb_tabbedtextoutex (void* context, Glyph32 glyph_value,
                unsigned int char_type)
{
    TABBEDTEXTOUTEX_CTXT* ctxt = (TABBEDTEXTOUTEX_CTXT*)context;
    int adv_x, adv_y;
    int tab_pos  = ctxt->nTabOrig;

    switch (char_type & ACHARTYPE_BASIC_MASK) {
        case ACHAR_BASIC_ZEROWIDTH:
        case ACHAR_BASIC_CTRL1:
        case ACHAR_BASIC_CTRL2:
            adv_x = adv_y = 0;
            break;

        case ACHAR_BASIC_HT:
            _gdi_start_new_line (ctxt->pdc);
            /* use some tabs to move current x. */
            if (ctxt->advance >= tab_pos) {
                while (ctxt->advance >= tab_pos)
                    tab_pos += (ctxt->nr_tab >= ctxt->nTabs) ?
                        ctxt->tab_width : ctxt->pTabPos[ctxt->nr_tab++];
            }
            else {
                tab_pos += (ctxt->nr_tab >= ctxt->nTabs) ?
                    ctxt->tab_width : ctxt->pTabPos[ctxt->nr_tab++];
            }

            _gdi_draw_null_glyph (ctxt->pdc, tab_pos - ctxt->advance,
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    ctxt->x, ctxt->y, &adv_x, &adv_y);

            ctxt->advance  = tab_pos;
            ctxt->nTabOrig = tab_pos;
            break;

        case ACHAR_BASIC_VOWEL: {
            int bkmode = ctxt->pdc->bkmode;
            ctxt->pdc->bkmode = BM_TRANSPARENT;
            _gdi_draw_one_glyph (ctxt->pdc, glyph_value,
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    ctxt->x, ctxt->y, &adv_x, &adv_y);
            ctxt->pdc->bkmode = bkmode;
            adv_x = adv_y = 0;
            break;
        }

        default: {
            int bkmode = ctxt->pdc->bkmode;
            ctxt->pdc->bkmode = ctxt->pdc->bkmode_set;
            ctxt->advance += _gdi_draw_one_glyph (ctxt->pdc, glyph_value,
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    ctxt->x, ctxt->y, &adv_x, &adv_y);
            ctxt->pdc->bkmode = bkmode;
            break;
        }
    }

    ctxt->x += adv_x;
    ctxt->y += adv_y;

    return TRUE;
}

int _gdi_tabbed_text_out (PDC pdc, int x, int y,
                const unsigned char* text, int len, int tab_width,
                BOOL only_extent, POINT* cur_pos, SIZE* size)
{
    TABBEDTEXTOUT_CTXT ctxt;
    int nr_delim_newline = 0, line_len = 0;

    ctxt.pdc = pdc;
    ctxt.start_x = x;
    ctxt.start_y = y;
    ctxt.max_advance = 0;
    ctxt.max_x = x;
    ctxt.max_y = y;
    ctxt.tab_width = tab_width;
    ctxt.line_height = pdc->pLogFont->size + pdc->alExtra + pdc->blExtra;

    ctxt.x = x;
    ctxt.y = y;
    ctxt.advance = 0;
    ctxt.only_extent = only_extent;

    if (!only_extent)
        pdc->rc_output = pdc->DevRC;

    while (len > 0) {
        line_len = __mg_substrlen (pdc->pLogFont,
                (const char*)text, len, '\n', &nr_delim_newline);

        ctxt.x = ctxt.start_x;
        ctxt.advance = 0;

        if(nr_delim_newline){
            _gdi_start_new_line (pdc);
            _gdi_reorder_text (pdc, text, line_len,
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    cb_tabbedtextout, &ctxt);
            if (ctxt.max_advance < ctxt.advance) {
                ctxt.max_advance = ctxt.advance;
                ctxt.max_x = ctxt.x;
            }
            len -= line_len + nr_delim_newline;
            text = text + line_len + nr_delim_newline;
            ctxt.y += ctxt.line_height * nr_delim_newline;
            ctxt.max_y = ctxt.y;
        }
        else{
            /* output the final line. */
            _gdi_start_new_line (pdc);
            _gdi_reorder_text (pdc, text, len,
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    cb_tabbedtextout, &ctxt);

            if (ctxt.max_advance < ctxt.advance) {
                ctxt.max_advance = ctxt.advance;
                ctxt.max_x = ctxt.x;
            }
            ctxt.y += ctxt.line_height;
            ctxt.max_y = ctxt.y;
            break;
        }
    }

    if (cur_pos) {
        cur_pos->x = ctxt.x;
        cur_pos->y = ctxt.y;
    }

    if (size) {
        size->cx = ABS (ctxt.max_x - ctxt.start_x);
        size->cy = ABS (ctxt.max_y - ctxt.start_y);// + ctxt.line_height;
    }

    return ctxt.max_advance;
}

int GUIAPI TabbedTextOutLen (HDC hdc, int x, int y,
                const char* spText, int len)
{
    PDC pdc;
    int advance;
    int tab_width;
    POINT cur_pos;

    if (!spText) return 0;
    pdc = dc_HDC2PDC(hdc);
    if (len < 0) len = __mg_strlen (pdc->pLogFont, spText);
    if (len == 0) return 0;

    tab_width = pdc->pLogFont->devfonts[0]->font_ops->get_ave_width
            (pdc->pLogFont, pdc->pLogFont->devfonts[0]) * pdc->tabstop;

    /* override start point by current text position */
    if ((pdc->ta_flags & TA_CP_MASK) == TA_UPDATECP) {
        x = pdc->CurTextPos.x;
        y = pdc->CurTextPos.y;
    }

    /* Transfer logical to device to screen here. */
    coor_LP2SP (pdc, &x, &y);

    /* convert to the start point on baseline. */
    _gdi_get_baseline_point (pdc, &x, &y);

    advance = _gdi_tabbed_text_out (pdc, x, y, (const unsigned char*) spText,
                    len, tab_width, FALSE, &cur_pos, NULL);

    coor_SP2LP (pdc, &cur_pos.x, &cur_pos.y);
    pdc->CurTextPos.x = cur_pos.x;
    pdc->CurTextPos.y = cur_pos.y;

    return advance;
}

int GUIAPI GetTabbedTextExtent (HDC hdc, const char* spText, int len,
                SIZE* pSize)
{
    PDC pdc;
    SIZE size;
    int advance;
    int tab_width;

    /* set size to zero first */
    pSize->cx = pSize->cy = 0;

    if (!spText)
        return 0;

    pdc = dc_HDC2PDC(hdc);
    if (len < 0) len = __mg_strlen (pdc->pLogFont, spText);
    if (len == 0) {
        pSize->cx = 0;
        pSize->cy = 0;
        return 0;
    }

    tab_width = pdc->pLogFont->devfonts[0]->font_ops->get_ave_width
            (pdc->pLogFont, pdc->pLogFont->devfonts[0]) * pdc->tabstop;

    advance = _gdi_tabbed_text_out (pdc, 0, 0, (const unsigned char*) spText,
                    len, tab_width, TRUE, NULL, &size);

    if (pSize) *pSize = size;

    return advance;
}

static int _gdi_tabbedex_text_out (PDC pdc, int x, int y,
                const unsigned char* text, int len, int tab_width,
                int nTabs, int *pTabPos, int nTabOrig, POINT* cur_pos)
{
    TABBEDTEXTOUTEX_CTXT ctxt;
    int nr_delim_newline = 0, line_len = 0;

    ctxt.pdc = pdc;
    ctxt.start_x = x;
    ctxt.start_y = y;
    ctxt.max_advance = 0;
    ctxt.tab_width = tab_width;
    ctxt.line_height = pdc->pLogFont->size + pdc->alExtra + pdc->blExtra;

    ctxt.x = x;
    ctxt.y = y;
    ctxt.advance = 0;

    /* init the tab relative info.*/
    ctxt.nTabs = nTabs;
    ctxt.nTabOrig = nTabOrig;
    ctxt.pTabPos = pTabPos;
    ctxt.nr_tab = 0;

    pdc->rc_output = pdc->DevRC;

    while (len > 0) {
        line_len = __mg_substrlen (pdc->pLogFont,
                (const char*)text, len, '\n', &nr_delim_newline);

        ctxt.x = ctxt.start_x;
        ctxt.advance = 0;
        /* reset the tab relative info for new line. */
        ctxt.nTabOrig = nTabOrig;
        ctxt.nr_tab = 0;

        if(nr_delim_newline){
            _gdi_start_new_line (pdc);
            _gdi_reorder_text (pdc, text, line_len,
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    cb_tabbedtextoutex, &ctxt);
            if (ctxt.max_advance < ctxt.advance) {
                ctxt.max_advance = ctxt.advance;
            }
            len -= line_len + nr_delim_newline;
            text = text + line_len + nr_delim_newline;
            ctxt.y += ctxt.line_height * nr_delim_newline;
        }
        else{
            /* output the final line. */
            _gdi_start_new_line (pdc);
            _gdi_reorder_text (pdc, text, len,
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    cb_tabbedtextoutex, &ctxt);

            if (ctxt.max_advance < ctxt.advance) {
                ctxt.max_advance = ctxt.advance;
            }
            break;
        }
    }

    if (cur_pos) {
        cur_pos->x = ctxt.x;
        cur_pos->y = ctxt.y;
    }
    return ctxt.max_advance;
}

int GUIAPI TabbedTextOutEx (HDC hdc, int x, int y, const char* spText,
        int nCount, int nTabs, int *pTabPos, int nTabOrig)
{
    PDC pdc;
    int advance;
    int def_tab;
    POINT cur_pos;

    if (!spText) return 0;
    pdc = dc_HDC2PDC(hdc);

    /* UTF16 is not supported */
    if (is_utf16_logfont (pdc)) return 0;

    if (nCount < 0) nCount = __mg_strlen (pdc->pLogFont, spText);
    if (nCount == 0) return 0;

    if (nTabs == 0 || pTabPos == NULL) {
        int ave_width = (*pdc->pLogFont->devfonts[0]->font_ops->get_ave_width)
                        (pdc->pLogFont, pdc->pLogFont->devfonts[0]);
        def_tab = ave_width * pdc->tabstop;
    }
    else
        def_tab = pTabPos [nTabs - 1];

    /* override start point by current text position */
    if ((pdc->ta_flags & TA_CP_MASK) == TA_UPDATECP) {
        x = pdc->CurTextPos.x;
        y = pdc->CurTextPos.y;
    }

    /* Transfer logical to device to screen here. */
    coor_LP2SP (pdc, &x, &y);

    /* convert to the start point on baseline. */
    _gdi_get_baseline_point (pdc, &x, &y);

    advance = _gdi_tabbedex_text_out (pdc, x, y, (const unsigned char*) spText,
                    nCount, def_tab, nTabs, pTabPos, nTabOrig, &cur_pos);

    coor_SP2LP (pdc, &cur_pos.x, &cur_pos.y);
    pdc->CurTextPos.x = cur_pos.x;
    pdc->CurTextPos.y = cur_pos.y;

    return advance;
}

#define SZ_BUFF_IN_STACK 256

static int get_tabbed_text_extent_point_for_bidi(HDC hdc,
        const char* text, int len, int max_extent,
        int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size)
{
    PDC pdc = dc_HDC2PDC(hdc);
    LOGFONT *log_font = pdc->pLogFont;
    DEVFONT* sbc_devfont = log_font->devfonts[0];
    Achar32 achars_buff[SZ_BUFF_IN_STACK];
    ACHARMAPINFO achars_map_buff[SZ_BUFF_IN_STACK];
    Achar32 *achars;
    ACHARMAPINFO* achars_map;

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

    _gdi_start_new_line(pdc);

    int tab_width = sbc_devfont->font_ops->get_ave_width(log_font, sbc_devfont)
                    * pdc->tabstop;
    int line_height = log_font->size + pdc->alExtra + pdc->blExtra;

    size->cx = 0;
    size->cy = line_height;

    int left_achars = nr_achars;
    int last_line_width = 0;
    while (left_achars > 0) {
        if (pos_chars)
            pos_chars[nr_fit_achars] = achars_map[nr_fit_achars].byte_index;
        if (dx_chars)
            dx_chars[nr_fit_achars] = last_line_width;
        size->cx = last_line_width;

        Uint32 achar_type = GetACharType(log_font, achars[nr_fit_achars]);
        Glyph32 gv = GetGlyphValueAlt(log_font, achars[nr_fit_achars]);

        int adv_x = 0, adv_y = 0;
        switch (achar_type & ACHARTYPE_BASIC_MASK) {
            case ACHAR_BASIC_ZEROWIDTH:
            case ACHAR_BASIC_CTRL1:
            case ACHAR_BASIC_CTRL2:
                adv_x = adv_y = 0;
                break;
            case ACHAR_BASIC_LF:
                adv_y = line_height;
            case ACHAR_BASIC_CR:
                adv_x = 0;
                if (last_line_width > size->cx) {
                    size->cx = last_line_width;
                }
                last_line_width = 0;
                _gdi_start_new_line(pdc);
                break;

            case ACHAR_BASIC_HT:
                adv_x = tab_width;
                last_line_width += tab_width;
                _gdi_start_new_line(pdc);
                break;

            default:
               last_line_width  += _gdi_get_glyph_advance (pdc, gv,
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    0, 0, &adv_x, &adv_y, NULL);
                last_line_width += pdc->cExtra;
                break;
        }

        if (max_extent > 0 && last_line_width > max_extent) {
            break;
        }

        size->cx += adv_x;
        size->cy += adv_y;
        if (last_line_width > size->cx)
            size->cx = last_line_width;
        left_achars--;
        nr_fit_achars++;
    }

    if (fit_chars)
        *fit_chars = nr_fit_achars;

done:
    if (achars != NULL && achars != achars_buff)
        free(achars);
    if (achars_map != NULL && achars_map != achars_map_buff)
        free(achars_map);
    return nr_fit_achars;
}

int GUIAPI GetTabbedTextExtentPoint (HDC hdc, const char* text,
                int len, int max_extent,
                int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size)
{
    PDC pdc = dc_HDC2PDC (hdc);
    LOGFONT* log_font = pdc->pLogFont;
    DEVFONT* sbc_devfont = log_font->devfonts[0];
    DEVFONT* mbc_devfont = log_font->devfonts[1];
    DEVFONT* devfont;
    int left_bytes = len;
    int len_cur_char;
    int tab_width, line_height;
    int last_line_width = 0;
    int char_count = 0;
    Achar32 ach;
    Glyph32 gv;

    /* set size to zero first */
    size->cx = size->cy = 0;

    if (mbc_devfont &&
            (mbc_devfont->charset_ops->legacy_bidi || pdc->bidi_flags) &&
            mbc_devfont->charset_ops->bidi_char_type) {
        return get_tabbed_text_extent_point_for_bidi(hdc, text, len,
                max_extent, fit_chars, pos_chars, dx_chars, size);
    }

    _gdi_start_new_line (pdc);

    tab_width = sbc_devfont->font_ops->get_ave_width (log_font, sbc_devfont)
                    * pdc->tabstop;
    line_height = log_font->size + pdc->alExtra + pdc->blExtra;
    size->cy = line_height;

    while (left_bytes > 0) {
        if (pos_chars)
            pos_chars [char_count] = len - left_bytes;
        if (dx_chars)
            dx_chars [char_count] = last_line_width;
        size->cx = last_line_width;

        devfont = NULL;
        len_cur_char = 0;

        if (mbc_devfont)
            len_cur_char = mbc_devfont->charset_ops->len_first_char
                                ((const unsigned char*)text, left_bytes);

        if (len_cur_char > 0)
            devfont = mbc_devfont;
        else {
            len_cur_char = sbc_devfont->charset_ops->len_first_char
                                ((const unsigned char*)text, left_bytes);
            if (len_cur_char > 0)
                devfont = sbc_devfont;
        }

        if (devfont) {
            int char_type;

            ach = devfont->charset_ops->get_char_value
                            (NULL, 0, (const unsigned char*)text, len_cur_char);
            char_type = devfont->charset_ops->char_type (ach);

            if (devfont == mbc_devfont)
                ach = SET_MBCHV(ach);
            gv = GetGlyphValueAlt(log_font, ach);

            switch (char_type & ACHARTYPE_BASIC_MASK) {
                case ACHAR_BASIC_ZEROWIDTH:
                case ACHAR_BASIC_CTRL1:
                case ACHAR_BASIC_CTRL2:
                    break;
                case ACHAR_BASIC_LF:
                    size->cy += line_height;
                case ACHAR_BASIC_CR:
                    if (last_line_width > size->cx)
                        size->cx = last_line_width;
                    last_line_width = 0;
                    _gdi_start_new_line (pdc);
                    break;

                case ACHAR_BASIC_HT:
                    last_line_width += tab_width;
                    _gdi_start_new_line (pdc);
                    break;

                default:
                   last_line_width  += _gdi_get_glyph_advance (pdc, gv,
                        (pdc->ta_flags & TA_X_MASK) == TA_LEFT,
                        0, 0, NULL, NULL, NULL);
                    last_line_width += pdc->cExtra;
                    break;
            }
        }
        else
            break;

        if (max_extent > 0 && last_line_width > max_extent) {
            goto ret;
        }

        if (last_line_width > size->cx)
            size->cx = last_line_width;
        char_count ++;
        left_bytes -= len_cur_char;
        text += len_cur_char;
    }

ret:
    if (fit_chars) *fit_chars = char_count;
    return len - left_bytes;
}

int GUIAPI GetTabbedACharsExtentPointEx(HDC hdc,
        Achar32* achars, int nr_achars, int max_extent,
        int* dx_achars, int* dy_achars, SIZE* size)
{
    PDC pdc = dc_HDC2PDC(hdc);
    LOGFONT *log_font = pdc->pLogFont;
    DEVFONT* sbc_devfont = log_font->devfonts[0];
    int nr_fit_achars = 0;

    _gdi_start_new_line(pdc);

    int tab_width = sbc_devfont->font_ops->get_ave_width(log_font, sbc_devfont)
                    * pdc->tabstop;
    int line_height = log_font->size + pdc->alExtra + pdc->blExtra;

    size->cx = 0;
    size->cy = 0;

    int left_achars = nr_achars;
    int last_line_width = 0;
    while (left_achars > 0) {
        if (dx_achars)
            dx_achars[nr_fit_achars] = last_line_width;
        if (dy_achars)
            dy_achars[nr_fit_achars] = size->cy;
        size->cx = last_line_width;

        Uint32 achar_type = GetACharType(log_font, achars[nr_fit_achars]);
        Glyph32 gv = GetGlyphValueAlt(log_font, achars[nr_fit_achars]);

        int adv_x = 0, adv_y = 0;
        switch (achar_type & ACHARTYPE_BASIC_MASK) {
            case ACHAR_BASIC_ZEROWIDTH:
            case ACHAR_BASIC_CTRL1:
            case ACHAR_BASIC_CTRL2:
                adv_x = adv_y = 0;
                break;
            case ACHAR_BASIC_LF:
                adv_y = line_height;
            case ACHAR_BASIC_CR:
                adv_x = 0;
                if (last_line_width > size->cx) {
                    size->cx = last_line_width;
                }
                last_line_width = 0;
                _gdi_start_new_line(pdc);
                break;

            case ACHAR_BASIC_HT:
                adv_x = tab_width;
                last_line_width += tab_width;
                _gdi_start_new_line(pdc);
                break;

            default:
               last_line_width  += _gdi_get_glyph_advance (pdc, gv,
                    (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    0, 0, &adv_x, &adv_y, NULL);
                last_line_width += pdc->cExtra;
                break;
        }

        if (max_extent > 0 && last_line_width > max_extent) {
            break;
        }

        size->cx += adv_x;
        size->cy += adv_y;
        if (last_line_width > size->cx)
            size->cx = last_line_width;
        left_achars--;
        nr_fit_achars++;
    }

    size->cy += line_height;
    return nr_fit_achars;
}

int GUIAPI GetTabbedACharsExtentPoint(HDC hdc, Achar32* achars,
        int nr_achars, int max_extent, SIZE* size)
{
    return GetTabbedACharsExtentPointEx(hdc,
        achars, nr_achars, max_extent, NULL, NULL, size);
}

int GUIAPI GetTabbedACharsExtent(HDC hdc, Achar32* achars,
        int nr_achars, SIZE* size)
{
    GetTabbedACharsExtentPointEx(hdc,
        achars, nr_achars, -1, NULL, NULL, size);
    return size->cx;
}

