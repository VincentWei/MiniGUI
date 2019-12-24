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
** drawtext.c: Implementation of DrawTextEx2
**
** Create date: 2008/02/02
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

static BOOL cb_drawtextex2 (void* context, Glyph32 glyph_value,
                unsigned int char_type)
{
    DRAWTEXTEX2_CTXT* ctxt = (DRAWTEXTEX2_CTXT*)context;
    int adv_x = 0, adv_y = 0;
    BBOX bbox;
    int bkmode;

    switch (char_type & ACHARTYPE_BASIC_MASK) {
        case ACHAR_BASIC_ZEROWIDTH:
        case ACHAR_BASIC_CR:
            adv_x = adv_y = 0;
            break;

        case ACHAR_BASIC_HT:
            if(!(ctxt->nFormat & DT_EXPANDTABS))
                return TRUE;
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
                bkmode = ctxt->pdc->bkmode;
                //ctxt->pdc->bkmode = BM_TRANSPARENT;
                _gdi_draw_one_glyph (ctxt->pdc, glyph_value,
                        (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                        ctxt->x, ctxt->y, &adv_x, &adv_y);
                ctxt->pdc->bkmode = bkmode;
                adv_x = adv_y = 0;
            }
            break;

        default:
            if (ctxt->only_extent) {
                ctxt->advance += _gdi_get_glyph_advance (ctxt->pdc,
                    glyph_value,
                    (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                    ctxt->x, ctxt->y, &adv_x, &adv_y, &bbox);
            }
            else {
                ctxt->advance += _gdi_draw_one_glyph (ctxt->pdc, glyph_value,
                        (ctxt->pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                        ctxt->x, ctxt->y, &adv_x, &adv_y);
            }
            break;
    }

    ctxt->x += adv_x;
    ctxt->y += adv_y;

    return TRUE;
}

int _gdi_get_drawtext_extent (PDC pdc, const unsigned char* text, int len,
                void* context, SIZE* size)
{
    DRAWTEXTEX2_CTXT  ctxt;
    DRAWTEXTEX2_CTXT* _tmp = (DRAWTEXTEX2_CTXT*)context;

    ctxt.pdc = pdc;
    ctxt.x = 0;
    ctxt.y = 0;
    ctxt.advance = 0;
    ctxt.only_extent = TRUE;
    ctxt.nFormat = _tmp->nFormat;
    ctxt.tab_width = _tmp->tab_width;

    _gdi_start_new_line (pdc);
    _gdi_reorder_text (pdc, text, len,
        (pdc->ta_flags & TA_X_MASK) != TA_RIGHT, cb_drawtextex2, &ctxt);

    if (size) {
        size->cx = ABS (ctxt.x);
        size->cy = ABS (ctxt.y) + pdc->pLogFont->size;
    }

    return ctxt.advance;
}

int DrawTextEx2 (HDC hdc, const char* pText, int nCount,
                RECT* pRect, int indent, UINT nFormat, DTFIRSTLINE *firstline)
{
    DRAWTEXTEX2_CTXT ctxt;
    PDC pdc;
    int nLines = 0;
    int x = 0, y;
    RECT rcDraw;
    int nTabWidth;
    int line_height, line_len = 0;
    int nr_delim_newline = 0;
    const unsigned char* pline = NULL;
    int old_ta = TA_LEFT;

    if (pText == NULL || nCount == 0 || pRect == NULL) return -1;
    if (RECTWP(pRect) == 0 || RECTHP(pRect) == 0) return -1;

    pdc = dc_HDC2PDC(hdc);
    if (pdc->pLogFont->rotation)
        return -1;

    if (nCount < 0) nCount = __mg_strlen (pdc->pLogFont, pText);
    if (nCount <= 0) return -1;

    if (indent < 0) indent = 0;

    line_height = pdc->pLogFont->size + pdc->alExtra + pdc->blExtra;
    if (nFormat & DT_TABSTOP)
        nTabWidth = HIWORD (nFormat) *
                    (*pdc->pLogFont->devfonts[0]->font_ops->get_ave_width)
                    (pdc->pLogFont, pdc->pLogFont->devfonts[0]);
    else {
        nTabWidth = pdc->tabstop *
                    (*pdc->pLogFont->devfonts[0]->font_ops->get_ave_width)
                    (pdc->pLogFont, pdc->pLogFont->devfonts[0]);
    }

    /* Transfer logical to device to screen here. */
    rcDraw = *pRect;
    coor_LP2SP(pdc, &rcDraw.left, &rcDraw.top);
    coor_LP2SP(pdc, &rcDraw.right, &rcDraw.bottom);
    NormalizeRect (&rcDraw);

#if 0
    /* If output rect is zero, we shouldn't output any text.*/
    if (RECTW(rcDraw) < pdc->pLogFont->size
            && RECTH(rcDraw) < pdc->pLogFont->size) {
        _WRN_PRINTF ("Output rect is too small, we won't output any text.\n");
        return -1;
    }
#endif

    /* nFormat surpport follow. */
    if ((nFormat & DT_CALCRECT) || firstline){
        ctxt.only_extent = TRUE;
    }
    else{
        ctxt.only_extent = FALSE;
    }

    if ((nFormat & DT_NOCLIP))
        pdc->rc_output = pdc->DevRC;
    else
        pdc->rc_output = rcDraw;

    y = rcDraw.top;
    if (nFormat & DT_SINGLELINE) {
        if (nFormat & DT_BOTTOM)
            y = rcDraw.bottom - pdc->pLogFont->size;
        else if (nFormat & DT_VCENTER)
            y = rcDraw.top + ((RECTH (rcDraw) - pdc->pLogFont->size) >> 1);
    }

    /* set the start_x pos.*/
    if (nFormat & DT_RIGHT) {
        x = rcDraw.right;
        old_ta = SetTextAlign(hdc, TA_RIGHT);
    }
    else {
        old_ta = SetTextAlign(hdc, TA_LEFT);
        x = rcDraw.left;
    }

    if (nFormat & DT_CALCRECT) {
        *pRect = rcDraw;
        NormalizeRect (pRect);
        pRect->top  = y;
        pRect->left = x;
    }

    /* init the drawtext context.*/
    ctxt.start_x = x;
    ctxt.start_y = y;
    ctxt.nFormat = nFormat;
    ctxt.tab_width   = nTabWidth;
    ctxt.line_height = line_height;
    ctxt.max_advance = 0;
    ctxt.pdc = pdc;
    ctxt.x   = x;
    ctxt.y   = y;

    while (nCount > 0) {
        int line_x, maxwidth;

        line_len = __mg_substrlen (pdc->pLogFont,
                pText, nCount, '\n', &nr_delim_newline);

        /* line_len == 0 means the first char is '\n' */
        if(nFormat & DT_SINGLELINE) {
            line_len = nCount;
        }
        else if (line_len == 0) {
            y += ctxt.line_height;
            nLines += 1;
        }

        pline = (const unsigned char*) pText;
        nCount -= line_len + nr_delim_newline;
        pText = pText + line_len + nr_delim_newline;

        while(line_len){
            if (nLines == 0) {
                line_x = indent;
                maxwidth = rcDraw.right - rcDraw.left;
                if ((maxwidth - indent) <= 0) {
                    y += line_height;
                    nLines ++;
                    continue;
                }
            }
            else {
                line_x = 0;
                maxwidth = rcDraw.right - rcDraw.left;
            }
            /* set the max limit width.*/
            if(nFormat & DT_SINGLELINE){
                ctxt.max_extent= -1;
            }
            else
                ctxt.max_extent= maxwidth - line_x;

            ctxt.nCount = 0;
            ctxt.x = ctxt.start_x + line_x;
            ctxt.y = y;
            ctxt.advance = 0;
            _gdi_start_new_line (pdc);

            /* convert to the start point on baseline. */
            _gdi_get_baseline_point(pdc, &ctxt.x, &ctxt.y);

            if (nFormat & DT_CENTER){
                BOOL old_set = ctxt.only_extent;

                /* only get text extent.*/
                ctxt.only_extent = TRUE;
                _gdi_reorder_text_break (pdc, pline, line_len,
                        (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                        cb_drawtextex2, &ctxt);

                /* calc text start pos under DT_CENTER.*/
                ctxt.nCount = 0;
                x = rcDraw.left + ((RECTW (rcDraw) - ctxt.advance) >> 1);
                ctxt.x = x + line_x;
                ctxt.advance = 0;
                ctxt.only_extent = old_set;
                if((nFormat & DT_CALCRECT) && (pRect->left > ctxt.x))
                    pRect->left = ctxt.x;
                _gdi_reorder_text_break (pdc, pline, line_len,
                        (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                        cb_drawtextex2, &ctxt);
            }
            else {
                _gdi_reorder_text_break (pdc, pline, line_len,
                        (pdc->ta_flags & TA_X_MASK) != TA_RIGHT,
                        cb_drawtextex2, &ctxt);
                if ((nFormat & DT_CALCRECT) && (nFormat & DT_RIGHT)){
                    if(pRect->left > (pRect->right - ctxt.advance))
                        pRect->left = pRect->right - ctxt.advance;
                }
            }
            if(ctxt.nCount <= 0)
                return 0;
            if (ctxt.max_advance < ctxt.advance) {
                ctxt.max_advance = ctxt.advance;
            }

            if (firstline) {
                firstline->nr_chars = ctxt.nCount;
                firstline->startx = x;
                firstline->starty = y;
                firstline->width  = ctxt.advance;
                firstline->height = ctxt.line_height;
                goto down;
            }

            line_len -= ctxt.nCount;
            pline  +=  ctxt.nCount;
            y += ctxt.line_height;
            nLines ++;
        }

        /* continuous multiline '\n'.*/
        if ((nr_delim_newline-1) > 0){
            y += ctxt.line_height * (nr_delim_newline-1);
            nLines += (nr_delim_newline-1);
        }

    }

down:
    if (nFormat & DT_RIGHT){
        SetTextAlign(hdc, old_ta);
    }

    if (firstline) {
        coor_SP2LP (pdc, &firstline->startx, &firstline->starty);
        return 0;
    }

    if (nFormat & DT_CALCRECT) {
        pRect->right  = pRect->left + ctxt.max_advance;
        pRect->bottom = pRect->top + nLines * ctxt.line_height;
        coor_SP2LP (pdc, &pRect->left, &pRect->top);
        coor_SP2LP (pdc, &pRect->right, &pRect->bottom);
    }
    else {
        /* update text out position */
        coor_SP2LP (pdc, &ctxt.x, &ctxt.y);
        pdc->CurTextPos.x = ctxt.x;
        pdc->CurTextPos.y = ctxt.y;
    }

    return line_height * nLines;
}


