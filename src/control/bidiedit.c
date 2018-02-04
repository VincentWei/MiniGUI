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
** bidiedit.c: the BIDI Single Line Edit Control module.
**
** Create date: 2008/03/10
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <common.h>

#ifdef _MGCTRL_BIDISLEDIT


#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/edit.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "clipboard.h"
#include "devfont.h"
#include "bidi.h"

#include "text.h"
#include "edit_impl.h"
#include "bidiedit_impl.h"


static void set_line_width (HWND hWnd, PBIDISLEDITDATA sled);
static void mySetCaretPos (HWND hWnd, PBIDISLEDITDATA sled, int x);
static LRESULT SLEditCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void sledit_refresh_caret (HWND hWnd, PBIDISLEDITDATA sled, BOOL bInited);

#define check_caret() \
            if(sled->selStart != sled->selEnd) \
                HideCaret(hWnd); \
            else \
                ShowCaret(hWnd);

/* simple macro to access glyphs string.*/
#define GLYPHS    (sled->glyph_content.glyphs)
#define GLYPHSMAP  (sled->glyph_content.glyph_map)
#define GLYPHSLEN  (sled->glyph_content.glyphs_len)

/* simple macro to access text string.*/
#define TEXT      (sled->str_content.string)
#define TEXTLEN   (sled->str_content.txtlen)


BOOL RegisterBIDISLEditControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_BIDISLEDIT;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_IBEAM);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = SLEditCtrlProc;

    if (AddNewControlClass (&WndClass) != ERR_OK)
        return FALSE;

    return TRUE;

}

static int get_line_rl_type (GLYPHMAPINFO* map, int len);

static int get_glyph_text_index (PBIDISLEDITDATA sled, int glyph_index)
{
    if (glyph_index < 0 || glyph_index > GLYPHSLEN)
        return -1;

    if(glyph_index >= GLYPHSLEN){
        int line_type = get_line_rl_type (GLYPHSMAP, GLYPHSLEN);
        if(!line_type){
            return TEXTLEN;
        }
        else{
            return 0;
        }
        //return (GLYPHSMAP[glyph_index - 1].byte_index);
        //return (GLYPHSMAP[glyph_index - 1].byte_index
        //      + GLYPHSMAP[glyph_index - 1].char_len);
    }
        
    return (*(GLYPHSMAP + glyph_index)).byte_index;
}

static int get_text_glyph_index (PBIDISLEDITDATA sled, int text_index)
{
    int min, max;
    int i = 0;
    GLYPHMAPINFO* map = GLYPHSMAP;
      
    if (text_index < 0 || text_index > TEXTLEN)
        return -1;
    
    for (; i < GLYPHSLEN; i++) {
        min = (map + i)->byte_index;
        max = (map + i)->byte_index + ((map + i)->char_len - 1);

        if ( text_index >= min && text_index <= max)
            return i;        
    }
    return -1;
}

int get_glyph_char_len(PBIDISLEDITDATA sled, int glyph_index)
{
    if (glyph_index<0 || glyph_index > GLYPHSLEN)
        return -1;
        
    return (*(GLYPHSMAP + glyph_index)).char_len;
}



/* Update glyph sring and glyph map info */
static void update_glyph_info (HWND hWnd, PBIDISLEDITDATA sled) 
{
    PLOGFONT log_font = GetWindowFont(hWnd);

    if (TEXT == NULL || TEXTLEN <= 0){
        GLYPHSLEN = 0;
        return; 
    }

    if (!glyphbuf_realloc (&sled->glyph_content, TEXTLEN))
        return;

    GLYPHSLEN = BIDIGetTextVisualGlyphs (log_font, (const char*)TEXT, TEXTLEN, 
                                    &GLYPHS,  &GLYPHSMAP);   
    return;
}

#define GLYPH_TYPE_RTL      1    
#define GLYPH_TYPE_LTR      0
#define GLYPH_TYPE_UNKKOWN  -1

static int get_glyph_type (PBIDISLEDITDATA sled, int glyph_index)
{
    if (glyph_index < 0 || glyph_index >= GLYPHSLEN)
        return GLYPH_TYPE_UNKKOWN;
 
    if (GLYPHSMAP[glyph_index].is_rtol)
        return GLYPH_TYPE_RTL;
    else 
        return GLYPH_TYPE_LTR;
}


static void setup_dc (HWND hWnd, BIDISLEDITDATA *sled, HDC hdc, BOOL bSel)
{
    DWORD dwStyle = GetWindowStyle (hWnd);

    if (!bSel) {

        if (dwStyle & WS_DISABLED)
            SetTextColor (hdc, 
                    GetWindowElementPixel (hWnd, WE_FGC_DISABLED_ITEM));
        else
            SetTextColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_WINDOW));

        SetBkColor (hdc, GetWindowBkColor (hWnd));
    }
    else {

        SetBkMode (hdc, BM_OPAQUE);

        if (dwStyle & WS_DISABLED)
            SetTextColor (hdc, 
                    GetWindowElementPixel (hWnd, WE_FGC_DISABLED_ITEM));
        else
            SetTextColor (hdc, 
                    GetWindowElementPixel (hWnd, WE_FGC_SELECTED_ITEM));

        if (sled->status & EST_FOCUSED)
            SetBkColor (hdc,
                    GetWindowElementPixel (hWnd, WE_BGC_SELECTED_ITEM));
        else
            SetBkColor (hdc, 
                    GetWindowElementPixel (hWnd, WE_BGC_SELECTED_LOSTFOCUS));
    }
}

static int sledit_settext (HWND hWnd, PBIDISLEDITDATA sled, const char *newtext)
{
    int len, txtlen;

    txtlen = strlen (newtext);
    len = (txtlen <= 0) ? sled->nBlockSize : txtlen;

    if (sled->hardLimit >= 0 && txtlen > sled->hardLimit) {
        return -1;
    }
 
    /* free the old text */
    if (TEXT){
        testr_free (&sled->str_content);
        glyphbuf_free (&sled->glyph_content);
    }

    if (!testr_alloc (&sled->str_content, len, sled->nBlockSize))
        return -1;
    
    if (!glyphbuf_alloc (&sled->glyph_content, len, sled->nBlockSize))
        return -1;

    if (newtext && txtlen > 0){
        testr_setstr (&sled->str_content, newtext, txtlen);
        update_glyph_info(hWnd, sled);
    }
    else {
        TEXTLEN = 0;
        GLYPHSLEN = 0;
    }

    return 0;
}

static void get_str_content_width (HWND hWnd, PBIDISLEDITDATA sled)
{
    GetClientRect (hWnd, &sled->rcVis);

    sled->rcVis.left += sled->leftMargin;
    sled->rcVis.top += sled->topMargin;
    sled->rcVis.right -= sled->rightMargin;
    sled->rcVis.bottom -= sled->bottomMargin;

    sled->nVisW = sled->rcVis.right - sled->rcVis.left;
}

static void recalcSize (HWND hWnd, PBIDISLEDITDATA sled, BOOL bInited)
{
    get_str_content_width (hWnd, sled);

    sled->nContX = 0;
    sled->nContW = sled->rcVis.right - sled->rcVis.left;

    set_line_width (hWnd, sled);

    sled->starty  = sled->topMargin + ( sled->rcVis.bottom - 
                    sled->rcVis.top - GetWindowFont (hWnd)->size - 1 ) / 2;

    sledit_refresh_caret (hWnd, sled, bInited);
}

static int sledit_init (HWND hWnd, PBIDISLEDITDATA sled)
{
    PCONTROL pCtrl;

    if (!sled)
        return -1;

    pCtrl = gui_Control(hWnd);

    pCtrl->dwAddData2 = (DWORD)sled;

    sled->status = 0;
    sled->editPos = 0;
    sled->selStart = 0;
    sled->selEnd = 0;

    sled->leftMargin     = MARGIN_EDIT_LEFT;
    sled->topMargin      = MARGIN_EDIT_TOP;
    sled->rightMargin    = MARGIN_EDIT_RIGHT;
    sled->bottomMargin   = MARGIN_EDIT_BOTTOM;

    sled->passwdChar     = '*';

    sled->changed        = FALSE;

    sled->nBlockSize = DEF_LINE_BLOCK_SIZE;
    sled->hardLimit      = -1;
    
    if (pCtrl->dwStyle & ES_TIP) {
        sled->tiptext = FixStrAlloc (DEF_TIP_LEN + 1);
        sled->tiptext[0] = 0;
    }
    else
        sled->tiptext = NULL;

    sled->str_content.string = NULL;
    sled->str_content.buffsize = 0;
    sled->str_content.txtlen = 0;
    
    sled->glyph_content.glyphs = NULL;
    sled->glyph_content.glyph_map = NULL;
    sled->glyph_content.glyphs_len = 0;
    sled->glyph_content.glyphs_buffsize = 0;
    sled->glyph_content.glyphmap_buffsize = 0;

    sledit_settext (hWnd, sled, pCtrl->spCaption);

    if (pCtrl->dwStyle & ES_PASSWORD) {
        pCtrl->pLogFont = GetSystemFont (SYSLOGFONT_DEFAULT);
    }

    CreateCaret (hWnd, NULL, 1, pCtrl->pLogFont->size);
    recalcSize (hWnd, sled, TRUE);

    return 0;
}

static void sledit_destroy (HWND hWnd, PBIDISLEDITDATA sled)
{
    DestroyCaret (hWnd);

    if ( (GetWindowStyle(hWnd) & ES_TIP) && sled->tiptext)
        FreeFixStr (sled->tiptext);
    testr_free (&sled->str_content);
    glyphbuf_free (&sled->glyph_content);

    return;
}

static inline BOOL sleContentToVisible (PBIDISLEDITDATA sled, int *x)
{
    if (x)
        *x -= sled->nContX;
    else
        return -1;

    if (*x < 0 || *x > sled->nVisW)
        return -1;

    return 0;
}

static inline BOOL sleVisibleToWindow (PBIDISLEDITDATA sled, int *x)
{
    if (x)
        *x += sled->leftMargin;

    return TRUE;
}

static inline void sleContentToWindow (PBIDISLEDITDATA sled, int *x)
{
    sleContentToVisible (sled, x);
    sleVisibleToWindow (sled, x);
}

static inline void sleWindowToContent (PBIDISLEDITDATA sled, int *x)
{
    *x -= sled->leftMargin;
    *x += sled->nContX;
}

/* sets the current caret position in the virtual str_content window */
static void mySetCaretPos (HWND hWnd, PBIDISLEDITDATA sled, int x)
{
    if (sleContentToVisible(sled, &x) < 0) {
        HideCaret (hWnd);
    }
    else {
        sleVisibleToWindow (sled, &x);
        SetCaretPos (hWnd, x, sled->starty);
        if (sled->status & EST_FOCUSED) {
            ActiveCaret (hWnd);
            ShowCaret (hWnd);
        }
    }
}

static int get_caretpos_x(HWND hWnd)
{
    PCONTROL pWin = (PCONTROL)hWnd;

    if (!pWin->pCaretInfo)
        return FALSE;

    return pWin->pCaretInfo->x;
}

static BOOL check_sel_edge(int* p, int* num, PBIDISLEDITDATA sled)
{
    int i = 0;
    if(p && sled->selStart == 0 && sled->selEnd >= GLYPHSLEN){
        p[0] = 0;
        p[1] = sled->selEnd;
        *num = 1;
        return TRUE;
    }

    /* check the ranges. */
    if(!p || *num <= 0) return FALSE;
    for(i = 0; i < *num; i++){
        if(p[i] < 0 || p[i] > GLYPHSLEN)
            return FALSE;
        if(p[i+1] < 0 || p[i+1] > GLYPHSLEN)
            return FALSE;
    }

    if (!(*num > 1)) return TRUE;
    if(sled->selStart == p[0] && (sled->selEnd+1) == p[1]){
        p[1] -= 1;
    }

    if(p[1] == GLYPHSLEN){
        sled->selEnd = GLYPHSLEN;
    }
    return TRUE;
}

#if _DEBUG
static void print_ranges(int* ranges, int nr_ranges, PBIDISLEDITDATA sled)
{
    int i = 0;
    int start_byte_index = get_glyph_text_index(sled, sled->selStart);
    int end_byte_index = get_glyph_text_index (sled, sled->selEnd);
    if(ranges){
        _MG_PRINTF("sel_glyph-[%d,%d], sel_byte = [%d, %d]\n",
                sled->selStart, sled->selEnd,
                start_byte_index, end_byte_index);

        _MG_PRINTF("nr_ranges = %d.\n ranges = ", nr_ranges);
        for (i=0; i<nr_ranges; i++)
        {
            _MG_PRINTF ("[%d, %d)-",
                    ranges[i<<1], ranges[(i<<1)+1]);
        }
        _MG_PRINTF("\n");
    }
}
#endif 

static void slePaint (HWND hWnd, HDC hdc, PBIDISLEDITDATA sled)
{
    char*   dispBuffer, *passwdBuffer = NULL;
    DWORD   dwStyle = GetWindowStyle(hWnd);
    int starty = sled->starty;
    int outw = 0;
    int startx = 0;
    int* ranges = NULL;
    int  nr_ranges = 0;
    int* glyph_string = GLYPHS;
    int  i = 0;
    int* p = NULL;
   
    if (dwStyle & ES_TIP && TEXTLEN <= 0 && 
        GetFocus(GetParent(hWnd)) != hWnd) {
        setup_dc (hWnd, sled, hdc, FALSE);
        TextOut (hdc, sled->leftMargin, starty, sled->tiptext);
        return;
    }

    if (dwStyle & ES_PASSWORD) {
        dispBuffer = FixStrAlloc (TEXTLEN);
        memset (dispBuffer, sled->passwdChar, TEXTLEN);
        passwdBuffer = dispBuffer;
    }

    if (dwStyle & ES_BASELINE) {
        SetPenColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_WINDOW));
#ifdef _PHONE_WINDOW_STYLE
        MoveTo (hdc, sled->leftMargin, sled->rcVis.bottom);
        LineTo (hdc, sled->rcVis.right, sled->rcVis.bottom);
#else
        DrawHDotLine (hdc, 
                    sled->leftMargin, sled->rcVis.bottom,
                    sled->rcVis.right - sled->rcVis.left);
#endif
    }

    /*some glyphs of arabic font have pixels out of them widths*/
    //sled->rcVis.right += 4;
    ClipRectIntersect (hdc, &sled->rcVis);

    if (sled->selStart != sled->selEnd) { 
        PLOGFONT log_font = GetWindowFont(hWnd);
        int start_byte_index = get_glyph_text_index(sled, sled->selStart);
        int end_byte_index = get_glyph_text_index (sled, sled->selEnd);

        GetTextRangesLog2Vis(log_font, (char*)TEXT, TEXTLEN,
                start_byte_index, end_byte_index, 
                &ranges, &nr_ranges);

        p = ranges;
        if (!check_sel_edge(p, &nr_ranges, sled)){
            if(p) free(ranges);
            ranges = p = NULL;
        }

#ifdef _DEBUG 
        print_ranges (p, nr_ranges, sled);
#endif 
        startx = sled->startx;
        sleContentToWindow (sled, &startx);
        while (p && i < nr_ranges){
            int len = 0;
            if (i == 0)
                len = *p;
            else
                len = *p - *(p-1);

            /* draw normal string. */
            setup_dc (hWnd, sled, hdc, FALSE);
            outw += DrawGlyphString (hdc, startx + outw, starty,
                    glyph_string, len, NULL, NULL);
            /* draw selected string.*/
            setup_dc (hWnd, sled, hdc, TRUE);
            outw += DrawGlyphString (hdc, startx + outw, starty, 
                    glyph_string + len, *(p + 1) - *p, NULL, NULL);

            glyph_string = GLYPHS + *(p + 1);

            p += 2; 
            i++;       
        }

        /* draw the last rest normal string if necessary.*/
        if (p && GLYPHSLEN > *(p-1)) {
            setup_dc (hWnd, sled, hdc, FALSE);
            outw += DrawGlyphString (hdc, startx + outw, starty,
                    glyph_string, GLYPHSLEN - *(p - 1), NULL, NULL);
        }
        else if(!p || nr_ranges == 0){
            /* draw first normal chars */
            if (sled->selStart > 0) {
                setup_dc (hWnd, sled, hdc, FALSE);
                outw += DrawGlyphString (hdc, startx + outw, starty,
                        glyph_string, sled->selStart, NULL, NULL);
                glyph_string += sled->selStart;
            }

            /* draw selected chars */
            setup_dc (hWnd, sled, hdc, TRUE);
                outw += DrawGlyphString (hdc, startx + outw, starty,
                        glyph_string, sled->selEnd - sled->selStart, NULL, NULL);
            glyph_string += sled->selEnd - sled->selStart;

            /* draw others */
            if (sled->selEnd < GLYPHSLEN) {
                setup_dc (hWnd, sled, hdc, FALSE);
                outw += DrawGlyphString (hdc, startx + outw, starty,
                        glyph_string, GLYPHSLEN - sled->selEnd, NULL, NULL);
            }

        }

        if (ranges)
            free (ranges);
    }
    else { /* select nothing */
        setup_dc (hWnd, sled, hdc, FALSE);
        startx = sled->startx;
        sleContentToWindow (sled, &startx);
        DrawGlyphString (hdc, startx, starty, glyph_string, GLYPHSLEN, NULL, NULL);
    }

    if (dwStyle & ES_PASSWORD)
        FreeFixStr (passwdBuffer);
}

static int 
sleSetSel (HWND hWnd, PBIDISLEDITDATA sled, int sel_start, int sel_end)
{
    if (GLYPHSLEN <= 0)
        return -1;

    if (sel_start < 0)
        sel_start = 0;
    if (sel_end < 0)
        sel_end = GLYPHSLEN;
    if (sel_start == sel_end)
        return -1;

    sled->selStart = sel_start;
    sled->selEnd = sel_end;
    HideCaret(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);

    return sled->selEnd - sled->selStart;
}

/* 
 * set caret position according to the new desired x coordinate.
 */
static void set_caret_pos (HWND hWnd, PBIDISLEDITDATA sled, int x, BOOL bSel)
{
#ifdef _DEBUG
    int out_chars = 0;
#endif
    int out_glyphs = 0;
    HDC hdc;
    SIZE txtsize;
    
    hdc = GetClientDC (hWnd);

    sleWindowToContent (sled, &x);

    if (x - sled->startx <= 0) {
#ifdef _DEBUG
        out_chars = 0;
#endif
        txtsize.cx = 0;
    }
    else {
        out_glyphs = GetGlyphsExtentPoint (hdc, GLYPHS, GLYPHSLEN,
                                          x - sled->startx, &txtsize);
#ifdef _DEBUG
        if(out_glyphs <= GLYPHSLEN){
            out_chars = get_glyph_text_index(sled, out_glyphs);
        }
#endif
        _MG_PRINTF ("editPos=%d,out_chars=%d\n", out_glyphs, out_chars);
    }
    if (!bSel) {
        sled->selStart = sled->selEnd = 0;
        sled->editPos = out_glyphs;
        mySetCaretPos (hWnd, sled, txtsize.cx + sled->startx);
    }
    else {
        if (out_glyphs > sled->editPos) {
            sled->selStart = sled->editPos;
            sled->selEnd = out_glyphs;
        }
        else {
            sled->selEnd = sled->editPos;
            sled->selStart = out_glyphs;
        }
    }

    ReleaseDC (hdc);
}

static BOOL make_pos_visible (HWND hWnd, PBIDISLEDITDATA sled, int x)
{
    if (x - sled->nContX > sled->nVisW) {
        sled->nContX = x - sled->nVisW;
        return TRUE;
    }
    else if (x < sled->nContX) {
        sled->nContX = x;
        return TRUE;
    }
    return FALSE;
}


static BOOL 
make_charpos_visible (HWND hWnd, PBIDISLEDITDATA sled, int glyph_pos, int *cx)
{
    SIZE glyphs_size;
    HDC hdc;

    if (glyph_pos <= 0)
        glyphs_size.cx = 0;
    else {
        /* check the glyph_pos. */
        if(glyph_pos > GLYPHSLEN)
            glyph_pos = GLYPHSLEN;

        hdc = GetClientDC (hWnd);
        GetGlyphsExtent (hdc, GLYPHS, glyph_pos, &glyphs_size);
        ReleaseDC (hdc);
    }
    if (cx)
        *cx = sled->startx + glyphs_size.cx;

    return make_pos_visible (hWnd, sled, sled->startx + glyphs_size.cx);
}

static void 
calc_content_width(HWND hWnd, PBIDISLEDITDATA sled, int charPos, int *cx)
{
    SIZE txtsize;
    HDC hdc;

    if (charPos <= 0)
        txtsize.cx = 0;
    else {
        hdc = GetClientDC (hWnd);
        GetTextExtent (hdc, (const char*)TEXT, charPos, &txtsize);
        ReleaseDC (hdc);
    }
    if (cx)
        *cx = txtsize.cx;

    return;
}

static void 
calc_glyph_pos_cx (HWND hWnd, PBIDISLEDITDATA sled, int glyph_pos, int *cx)
{
    SIZE glyphs_size;
    HDC hdc;

    if (glyph_pos <= 0)
        glyphs_size.cx = 0;
    else {
        hdc = GetClientDC (hWnd);
        GetTextExtent (hdc, (const char*)GLYPHS, glyph_pos, &glyphs_size);
        ReleaseDC (hdc);
    }
    if (cx)
        *cx = sled->startx + glyphs_size.cx;
        
}

/*
 * set caret position according to the current edit position.
 */
static BOOL edtSetCaretPos (HWND hWnd, PBIDISLEDITDATA sled)
{
    BOOL bRefresh;
    int cx;

    bRefresh = make_charpos_visible (hWnd, sled, sled->editPos, &cx);

    mySetCaretPos (hWnd, sled, cx);
    if (bRefresh)
        InvalidateRect (hWnd, NULL, TRUE);

    return bRefresh;
}

static void set_edit_caret_pos(HWND hWnd, PBIDISLEDITDATA sled)
{
    int cx;

    make_charpos_visible (hWnd, sled, sled->editPos, &cx);
    mySetCaretPos (hWnd, sled, cx);
}

static int sleMouseMove (HWND hWnd, PBIDISLEDITDATA sled, LPARAM lParam)
{
    int mouseX, mouseY;
    RECT rcClient;
    BOOL refresh = TRUE;

    mouseX = LOSWORD(lParam);
    mouseY = HISWORD(lParam);

    ScreenToClient(hWnd, &mouseX, &mouseY);
    GetClientRect(hWnd, &rcClient);
    
    if(mouseX >= 0 && mouseX < rcClient.right
       && mouseY >= 0 && mouseY < rcClient.bottom) {//in edit window
        set_caret_pos (hWnd, sled, mouseX, TRUE);
        if (sled->editPos == sled->selStart)
            make_charpos_visible (hWnd, sled, sled->selEnd, NULL);
        else
            make_charpos_visible (hWnd, sled, sled->selStart, NULL);
    }
    else if (mouseY < 0) {
        sled->nContX = 0;
        sled->selStart = 0;
        sled->selEnd = sled->editPos;
    }
    else if(mouseY >= rcClient.bottom) {
        /* not surpport select all when mouse bellow edit bottom.*/
#if 0
        HDC hdc;
        SIZE size;
        sled->selStart = sled->editPos;
        sled->selEnd = GLYPHSLEN;
        //sled->nContX = sled->nContW - (sled->rcVis.right - sled->rcVis.left); 
        hdc = GetClientDC (hWnd);
        sled->nContX = GetGlyphsExtent (hdc, GLYPHS, GLYPHSLEN, &size) - (sled->rcVis.right - sled->rcVis.left);
        ReleaseDC (hdc);
        printf ("nContX = %d. nContW = %d, rcVis.right = %d, rcVis.left = %d.\n", 
                sled->nContX, sled->nContW, sled->rcVis.right, sled->rcVis.left);
#endif
    }
    else if (mouseX < 0) {
        if (sled->selEnd == GLYPHSLEN) {
            set_caret_pos (hWnd, sled, 0, TRUE);
            make_charpos_visible (hWnd, sled, sled->selStart, NULL);
            goto quit;
        }

        if (sled->selStart == 0)
            return 0;

        sled->selStart --;
        refresh = make_charpos_visible (hWnd, sled, sled->selStart, NULL);
    }
    else if (mouseX >= rcClient.right) {
        if (sled->selStart == 0) 
            set_caret_pos (hWnd, sled, rcClient.right - 1, TRUE);

        if (sled->selEnd == GLYPHSLEN)
            return 0;

        sled->selEnd++;
        refresh = make_charpos_visible (hWnd, sled, sled->selEnd, NULL);
    }
quit:
    check_caret();

    if (refresh)
        InvalidateRect(hWnd, NULL, TRUE);

    return 0;
}

static void set_line_width (HWND hWnd, PBIDISLEDITDATA sled)
{
    SIZE glyphs_size;
    HDC hdc;
    DWORD dwStyle = GetWindowStyle(hWnd);
    int old_w = sled->nContW;

    hdc = GetClientDC (hWnd);

    SelectFont (hdc, GetWindowFont(hWnd));
    update_glyph_info(hWnd, sled);

    GetGlyphsExtent (hdc, GLYPHS, GLYPHSLEN, &glyphs_size);
    ReleaseDC (hdc);

    if (glyphs_size.cx > sled->nVisW)
        sled->nContW = glyphs_size.cx;
    else
        sled->nContW = sled->nVisW;

    if (dwStyle & ES_RIGHT) {
        sled->nContX += (sled->nContW - old_w);
    }
    else if (dwStyle & ES_CENTER) {
        sled->nContX += (sled->nContW - old_w) >> 1;
    }

    if (sled->nContX + sled->nVisW > sled->nContW) {
        sled->nContX = sled->nContW - sled->nVisW;
    }
    else if (sled->nContX < 0)
        sled->nContX = 0;

    if (sled->nContW > sled->nVisW)
        sled->startx = 0;
    else if (dwStyle & ES_RIGHT) {
        sled->startx = sled->nVisW - glyphs_size.cx;
    }
    else if (dwStyle & ES_CENTER) {
        sled->startx = (sled->nVisW - glyphs_size.cx) >> 1;
    }
    else {
        sled->startx = 0;
    }

    return;
}

static inline void edtChangeCont (HWND hWnd, PBIDISLEDITDATA sled)
{
    update_glyph_info (hWnd, sled);
    set_line_width (hWnd, sled);
}

/* check input validity for password style */
static void check_valid_passwd (char *newtext)
{
    char *ptmp = newtext;

    while (*ptmp) {
        *ptmp = '*';
        ptmp ++;
    }
}

static int get_glyph_bytes (PBIDISLEDITDATA sled, int glyph_index)
{
    if (glyph_index < 0 || glyph_index > GLYPHSLEN)
        return 0;

    if(glyph_index >= GLYPHSLEN){
        return 0;
    }
        
    return (*(GLYPHSMAP + glyph_index)).char_len;
}

static void del_selected_text (HWND hWnd, PBIDISLEDITDATA sled)
{
    int s = 0,e = 0;
    int start_byte_index = 0, end_byte_index = 0;
    int deleted = 0;
    unsigned char *str_content = TEXT;

    if(sled->selStart != sled->selEnd) {

        if (sled->selStart < 0 || sled->selEnd < 0)
            return;

        if(sled->selStart == 0 && sled->selEnd == GLYPHSLEN){
            s = 0; 
            e = TEXTLEN; 
        }
        else{
            s = get_glyph_text_index(sled, sled->selStart);
            e = get_glyph_text_index(sled, sled->selEnd);
            /*add end glyph char_len.*/
            if(s > e)
                end_byte_index += get_glyph_bytes(sled, sled->selStart);
            else
                end_byte_index += get_glyph_bytes(sled, sled->selEnd);
        }

        start_byte_index = MIN(s, e);
        end_byte_index = MAX (s, e);


        _MG_PRINTF("delete: sel_glyph-[%d,%d], sel_byte = [%d, %d]\n",
                sled->selStart, sled->selEnd,
                start_byte_index, end_byte_index);

        deleted = end_byte_index - start_byte_index;

        memmove (str_content + start_byte_index, str_content + end_byte_index,
                        TEXTLEN - end_byte_index);
        TEXTLEN -= deleted;

        sled->editPos = sled->selStart;
        sled->selEnd = sled->selStart;
        ShowCaret(hWnd);
   }

   return; 

}

static int 
sleBackspaceText (HWND hWnd, PBIDISLEDITDATA sled, int del_char_num, 
        BOOL refresh, BOOL is_del)
{
    int del_pos;
    unsigned char *pIns, *str_content = TEXT;
    int old_glyphs_len = GLYPHSLEN;
    int edit_pos_offset = 0;

    if(sled->selStart != sled->selEnd) {
        del_selected_text (hWnd, sled);
        del_char_num = 0;
   }

    del_pos = get_glyph_text_index(sled, sled->editPos - 1) + 1;

    if (del_char_num > 0) {
        pIns = str_content + del_pos;
        memmove (pIns - del_char_num, pIns, TEXTLEN+1 - del_pos);
        str_content = testr_realloc (&sled->str_content,
                             TEXTLEN - del_char_num);
    }
    TEXTLEN -= del_char_num;

    edtChangeCont (hWnd, sled);

    if (is_del) {
        if (del_char_num > 0 && sled->editPos >0 )
            sled->editPos --;
    }
    else {
        edit_pos_offset = old_glyphs_len - GLYPHSLEN;
        if (del_char_num > 0 && edit_pos_offset > 0
            && sled->editPos >= edit_pos_offset )
            sled->editPos -= edit_pos_offset;
    }

    sled->selStart = sled->selEnd = sled->editPos;

    DBGLOG2("editPos = %d. \n", sled->editPos);

    if (!refresh) {
        InvalidateRect (hWnd, NULL, TRUE);

        if (!edtSetCaretPos (hWnd, sled)) {
            //InvalidateRect (hWnd, NULL, TRUE);
        }
        sled->changed = TRUE;
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
    }

    return 0;
}

static int ascii_bidi_type[] = {
    /*0x00~0x0f*/ 
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN, 
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN, 
    BIDI_TYPE_BN,  BIDI_TYPE_SS,  BIDI_TYPE_BS,  BIDI_TYPE_SS, 
    BIDI_TYPE_WS,  BIDI_TYPE_BS,  BIDI_TYPE_BN,  BIDI_TYPE_BN,

    /*0x10~0x1f*/  
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN, 
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN, 
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN, 
    BIDI_TYPE_BS,  BIDI_TYPE_BS,  BIDI_TYPE_BS,  BIDI_TYPE_SS,

    /*0x20~0x2f*/  
    BIDI_TYPE_WS,  BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ET, 
    BIDI_TYPE_ET,  BIDI_TYPE_ET,  BIDI_TYPE_ON,  BIDI_TYPE_ON, 
    BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ES, 
    BIDI_TYPE_CS,  BIDI_TYPE_ES,  BIDI_TYPE_CS,  BIDI_TYPE_CS,

    /*0x30~0x3f*/  
    BIDI_TYPE_EN,  BIDI_TYPE_EN,  BIDI_TYPE_EN,  BIDI_TYPE_EN, 
    BIDI_TYPE_EN,  BIDI_TYPE_EN,  BIDI_TYPE_EN,  BIDI_TYPE_EN, 
    BIDI_TYPE_EN,  BIDI_TYPE_EN,  BIDI_TYPE_CS,  BIDI_TYPE_ON, 
    BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ON,

    /*0x40~0x4f*/  
    BIDI_TYPE_ON,  BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,

    /*0x50~0x6f*/  
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_ON, 
    BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ON,
    /*0x60~0x6f*/  
    BIDI_TYPE_ON,  BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,

    /*0x70~0x7f*/  
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR,
    BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_LTR, BIDI_TYPE_ON, 
    BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_BN,
};

static int inline get_glyph_bidi_type (LOGFONT* logfont, Glyph32 glyph)
{
    DEVFONT* devfont;

    if (IS_MBC_GLYPH(glyph)) {
        devfont = logfont->mbc_devfont;

        if (devfont->charset_ops->bidi_glyph_type)
        {
            return devfont->charset_ops->bidi_glyph_type (glyph);
        }
        else
        {
            return BIDI_TYPE_LTR;
        }
    }
    else {
        return ascii_bidi_type[glyph];
    }

}
static int get_text_rl_type (PLOGFONT logfont, char* newtext, int l_glyph_type,
        int r_glyph_type, int line_type)
{
    int bidi_type;
    Glyph32 glyph;
    glyph = GetGlyphValue (logfont, newtext, strlen(newtext), NULL, 0);

    if (glyph == INV_GLYPH_VALUE)
        return -1;

    bidi_type = get_glyph_bidi_type (logfont, glyph);

    if (BIDI_IS_STRONG(bidi_type))
    {
        if (BIDI_IS_RTL(bidi_type))
        {
            return GLYPH_TYPE_RTL;
        }
        else
        {
            return GLYPH_TYPE_LTR;
        }
    }

    if (BIDI_IS_NUMBER(bidi_type))
    {
        return GLYPH_TYPE_LTR;
    }

    if (l_glyph_type == r_glyph_type)
    {
        return l_glyph_type;
    }
    /*no left glyph*/
    else if (l_glyph_type == -1)
    {
        if (BIDI_IS_WEAK (bidi_type))
            return r_glyph_type;
        else
            return line_type;
    }
    /*no right glyph*/
    else if (r_glyph_type == -1)
    {
        if (BIDI_IS_WEAK (bidi_type))
            return l_glyph_type;
        else
            return line_type;
    }
    else
    {
        return line_type;
    }

}

static int get_line_rl_type (GLYPHMAPINFO* map, int len)
{
    int i;
    for (i=0; i<len; i++)
    {
        if (map->byte_index == 0)
            return map->is_rtol;
        map++;
    }
    return GLYPH_TYPE_UNKKOWN;
}

#define INSERTPOS_BEFORE(sled, cur_glyph_index) \
    ( (sled)->glyph_content.glyph_map[cur_glyph_index].byte_index )

#define INSERTPOS_AFTER(sled, cur_glyph_index) \
    ( (sled)->glyph_content.glyph_map[cur_glyph_index].byte_index +  \
     (sled)->glyph_content.glyph_map[cur_glyph_index].char_len )

#define INSERTPOS_LINE_END(sled) \
    ((sled)->str_content.txtlen)
   

static int 
get_insert_pos (PLOGFONT logfont, PBIDISLEDITDATA sled, char *newtext, 
        int* cur_type)
{
    int l_glyph_index = sled->editPos - 1;
    int r_glyph_index = sled->editPos;

    int l_glyph_type = get_glyph_type (sled, l_glyph_index);
    int r_glyph_type = get_glyph_type (sled, r_glyph_index);

    int line_type = get_line_rl_type (GLYPHSMAP, GLYPHSLEN);

    *cur_type = get_text_rl_type (logfont, newtext, l_glyph_type,
            r_glyph_type, line_type);

    if (*cur_type == GLYPH_TYPE_RTL) {
        if (l_glyph_type == GLYPH_TYPE_RTL) {
            return INSERTPOS_BEFORE (sled, l_glyph_index);
        }
        else if (r_glyph_type == GLYPH_TYPE_RTL) {
            return INSERTPOS_AFTER (sled, r_glyph_index);
        }
        else {
            /*L|L, or L|0(must in a L line)*/
            if (l_glyph_type == GLYPH_TYPE_LTR) {
                return INSERTPOS_AFTER (sled, l_glyph_index);
            }
            /*0|L*/
            else if (r_glyph_type == GLYPH_TYPE_LTR) {
                /*logical head in L line*/
                if (line_type == GLYPH_TYPE_LTR)
                {
                    return 0;
                }
                /*logical tail in R Line*/
                else
                {
                    return INSERTPOS_LINE_END(sled);
                }
            }
            else {
                return 0;
            }
        }
    }
    else {
        if (l_glyph_type == GLYPH_TYPE_LTR) {
            return INSERTPOS_AFTER (sled, l_glyph_index);
        }
        else if (r_glyph_type == GLYPH_TYPE_LTR) {
            return INSERTPOS_BEFORE (sled, r_glyph_index);
        }
        else {
            /*R|R  or 0|R(must in R line)*/
            if (r_glyph_type == GLYPH_TYPE_RTL) {
                return INSERTPOS_AFTER (sled, r_glyph_index);
            }
            /*R|0*/
            else if (l_glyph_type == GLYPH_TYPE_RTL) {
                /*logical head in R line*/
                if (line_type == GLYPH_TYPE_RTL) {
                    return 0;
                }
                /*logical tail in L line*/
                else {
                    return INSERTPOS_LINE_END(sled);
                }
            }
            else {
                return 0;
            }
        }
    }
}

#ifdef _DEBUG
static void print_text(PBIDISLEDITDATA sled)
{
    int i;
    _MG_PRINTF("sled->text = [");
    for (i=0; i<TEXTLEN; i++)
        _MG_PRINTF("%c", TEXT[i]);
    _MG_PRINTF ("]\n");
    /* output hexstring.*/
    _MG_PRINTF("sled->text = [");
    for (i=0; i<TEXTLEN; i++)
        _MG_PRINTF("%0x ", TEXT[i]);
    _MG_PRINTF ("]\n");
}
#endif

static int 
sleInsertText (HWND hWnd, PBIDISLEDITDATA sled, char *newtext, 
                int inserting, BOOL refresh)
{
    int insert_pos;
    unsigned char *pIns, *str_content = TEXT;
    int cur_type;

    if (inserting <= 0)
        return 0;

    if ( (GetWindowStyle(hWnd) & ES_PASSWORD) && newtext) {
        check_valid_passwd (newtext);
    }

    //delete the seleted
    if(sled->selStart != sled->selEnd) {
        del_selected_text (hWnd, sled);
        update_glyph_info (hWnd, sled);
    }

    insert_pos = get_insert_pos (GetWindowFont(hWnd), sled, newtext, &cur_type);

#ifdef _DEBUG 
    print_text (sled);
#endif 

    /* insert  and total char len > hardLimit in INSERT state */
    if ( !(sled->status & EST_REPLACE) && 
           sled->hardLimit >= 0 &&
           ((TEXTLEN + inserting) > sled->hardLimit)) {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = sled->hardLimit - TEXTLEN;

        if (!refresh)
            return 0;
    }      /* insert  and total char len > hardLimit in REPLACE state */
    else if ( (sled->status & EST_REPLACE) && 
               sled->hardLimit >= 0 &&
               (insert_pos + inserting > sled->hardLimit) ) {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = sled->hardLimit - insert_pos;
    }

    str_content = testr_realloc (&sled->str_content, TEXTLEN + inserting);
    pIns = str_content + insert_pos;
    if ( !(sled->status & EST_REPLACE) )
        memmove (pIns + inserting, pIns, TEXTLEN+1 - insert_pos);
    memcpy (pIns, newtext, inserting);

    if (!(sled->status & EST_REPLACE))
        TEXTLEN += inserting;
    else {
        int add_len = inserting - TEXTLEN + insert_pos;
        if (add_len > 0) {
            TEXTLEN += add_len;
            TEXT[TEXTLEN] = '\0';
        }
    }

#ifdef _DEBUG
    print_text (sled);
#endif

    edtChangeCont (hWnd, sled);

    /* RTL */
    if (cur_type == GLYPH_TYPE_RTL) {
        sled->editPos = get_text_glyph_index (sled, insert_pos);
    }
    else { /*LTR*/
        sled->editPos = get_text_glyph_index (sled, insert_pos) + 1;    
    }

    sled->selStart = sled->selEnd = sled->editPos;

#ifdef _DEBUG
    {
        int i = 0;
        _MG_PRINTF ("string = %s.\n", TEXT);
        _MG_PRINTF ("editPos = %d.\n", sled->editPos);
        for (i = 0; i < sled->glyph_content.glyphs_len; i++) {
            _MG_PRINTF ("map[%d] = %d,", i, 
                    sled->glyph_content.glyph_map[i].byte_index);
            _MG_PRINTF ("\n");
        }
    }
#endif

    InvalidateRect (hWnd, NULL, TRUE);

    if (!refresh) {
        edtSetCaretPos (hWnd, sled);
        sled->changed = TRUE;
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
    }

    return 0;
}

#ifdef _MGHAVE_CLIPBOARD
static int sleInsertCbText (HWND hWnd, PBIDISLEDITDATA sled)
{
    int  inserting;
    unsigned char *txtBuffer;

    if (GetWindowStyle(hWnd) & ES_READONLY) {
        return 0;
    }

    inserting = GetClipBoardDataLen (CBNAME_TEXT);
    txtBuffer = ALLOCATE_LOCAL (inserting);
    GetClipBoardData (CBNAME_TEXT, txtBuffer, inserting);
    sleInsertText (hWnd, sled, (char *)txtBuffer, inserting, FALSE);
    DEALLOCATE_LOCAL(txtBuffer);

    return 0;
}
#endif

static void esright_backspace_refresh(HWND hWnd, PBIDISLEDITDATA sled, int del)
{
    RECT scroll_rc, refresh_rc;
    int sel_start_x, sel_end_x;
    int old_caret_x, cur_caret_x;
    int scroll_len;
    int old_editpos_x, cur_editpos_x;

    scroll_rc.top = sled->rcVis.top;
    scroll_rc.bottom = sled->rcVis.bottom;

    if (sled->nContW <= sled->nVisW){
        if (sled->selStart != sled->selEnd){

            calc_glyph_pos_cx(hWnd, sled, sled->selStart, &sel_start_x);
            calc_glyph_pos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
            if (sled->selStart < sled->selEnd){
                scroll_rc.right = sel_end_x + 1;
            }
            else{
                scroll_rc.right = sel_start_x + 1;
            }

            scroll_rc.left = sled->rcVis.left;
            sleBackspaceText (hWnd, sled, del, TRUE, FALSE);

            scroll_len = abs(sel_start_x - sel_end_x);
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

            /* FIXME, repaint.*/
            InvalidateRect(hWnd, NULL, TRUE);

            edtSetCaretPos (hWnd, sled);
        }
        else{
            calc_glyph_pos_cx(hWnd, sled, sled->editPos, &old_editpos_x);
            scroll_rc.left = sled->rcVis.left;
            calc_glyph_pos_cx(hWnd, sled, sled->editPos + del, &cur_editpos_x);
            
            sleBackspaceText (hWnd, sled, del,TRUE, FALSE);

            edtSetCaretPos (hWnd, sled);
            scroll_rc.right = old_editpos_x + 1;

            scroll_len = abs(cur_editpos_x - old_editpos_x);
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

            /* Add for redraw glyph string. ES_RIGHT backspace/del error  */
            InvalidateRect(hWnd, NULL, TRUE);
        }

    }
    else{

        if (sled->nContX <= 0){
/* left scroll window while the head of text is in the sled->rcVis */
            if (sled->selStart != sled->selEnd){

                refresh_rc.top = sled->rcVis.top;
                refresh_rc.bottom = sled->rcVis.bottom;
                refresh_rc.right = sled->rcVis.right;

                scroll_rc.right = sled->rcVis.right;

                calc_glyph_pos_cx(hWnd, sled, sled->selStart, &sel_start_x);
                calc_glyph_pos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
                if (sled->selStart < sled->selEnd){
                    scroll_rc.left = sel_start_x + 1;
                }
                else{
                    scroll_rc.left = sel_end_x + 1;
                }

                sleBackspaceText (hWnd, sled, del, TRUE, FALSE);
                edtSetCaretPos (hWnd, sled);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(hWnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (sled->nContW <= sled->nVisW){
                    InvalidateRect(hWnd, NULL, TRUE);
                }

            }
            else{
                refresh_rc.top = sled->rcVis.top;
                refresh_rc.bottom = sled->rcVis.bottom;
                refresh_rc.right = sled->rcVis.right;

                scroll_rc.right = sled->rcVis.right;
                old_caret_x = get_caretpos_x(hWnd);


                sleBackspaceText (hWnd, sled, del, TRUE, FALSE);
                edtSetCaretPos (hWnd, sled);
                cur_caret_x = get_caretpos_x(hWnd);
                scroll_rc.left = cur_caret_x;

                scroll_len = abs(cur_caret_x - old_caret_x);
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(hWnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (sled->nContW <= sled->nVisW){
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }

        }
        else{//right scroll window while sled->nContX > 0
            if (sled->selStart != sled->selEnd){

                refresh_rc.top = sled->rcVis.top;
                refresh_rc.bottom = sled->rcVis.bottom;
                refresh_rc.left = sled->rcVis.left;

                calc_glyph_pos_cx(hWnd, sled, sled->selStart, &sel_start_x);
                calc_glyph_pos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
                if (sled->selStart < sled->selEnd){
                    scroll_rc.right = sel_end_x + 1 - sled->nContX;
                }
                else{
                    scroll_rc.right = sel_start_x + 1 - sled->nContX;
                }

                scroll_rc.left = sled->rcVis.left;
                sleBackspaceText (hWnd, sled, del, TRUE, FALSE);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.right = refresh_rc.left + scroll_len;
                ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
                edtSetCaretPos (hWnd, sled);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (sled->nContW <= sled->nVisW){
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                if (sled->nContX <= 0){
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            else{
                int old_ncontw;

                refresh_rc.top = sled->rcVis.top;
                refresh_rc.bottom = sled->rcVis.bottom;
                refresh_rc.left = sled->rcVis.left;

                scroll_rc.left = sled->rcVis.left;
                old_ncontw = sled->nContW;
                sleBackspaceText (hWnd, sled, del, TRUE, FALSE);
                edtSetCaretPos (hWnd, sled);
                cur_caret_x = get_caretpos_x(hWnd);    
                scroll_rc.right = cur_caret_x;

                scroll_len = abs(sled->nContW - old_ncontw);
                refresh_rc.right = refresh_rc.left + abs(scroll_len);
                ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (sled->nContW <= sled->nVisW){
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                if (sled->nContX <= 0){
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }

        }

    }

    sled->changed = TRUE;
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
}

static void esright_del_refresh(HWND hWnd, PBIDISLEDITDATA sled, int del)
{
    RECT scroll_rc, refresh_rc;
    int sel_start_x, sel_end_x;
    int old_caret_x, cur_caret_x;
    int scroll_len;
    int old_editpos_x, cur_editpos_x;

    scroll_rc.top = sled->rcVis.top;
    scroll_rc.bottom = sled->rcVis.bottom;

    if (sled->nContW <= sled->nVisW){
        if (sled->selStart != sled->selEnd){

            calc_glyph_pos_cx(hWnd, sled, sled->selStart, &sel_start_x);
            calc_glyph_pos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
            if (sled->selStart < sled->selEnd){
                scroll_rc.right = sel_end_x + 1;
            }
            else{
                scroll_rc.right = sel_start_x + 1;
            }

            scroll_rc.left = sled->rcVis.left;
            sleBackspaceText (hWnd, sled, del, TRUE, TRUE);

            scroll_len = abs(sel_start_x - sel_end_x);
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
            edtSetCaretPos (hWnd, sled);

            /* Add for redraw glyph string. ES_RIGHT backspace/del error  */
            InvalidateRect(hWnd, NULL, TRUE);
 
        }
        else{
            old_caret_x = get_caretpos_x(hWnd);
            scroll_rc.left = sled->rcVis.left;
            sleBackspaceText (hWnd, sled, del, TRUE, TRUE);
            edtSetCaretPos (hWnd, sled);
            cur_caret_x = get_caretpos_x(hWnd);    
            scroll_rc.right = cur_caret_x;

            scroll_len = cur_caret_x - old_caret_x;
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

            /* Add to fix bug2741 */
            InvalidateRect(hWnd, NULL, TRUE);
        }

    }
    else{
        if (sled->nContX <=0){
/* left scroll window while the head of text is in the sled->rcVis */
            if (sled->selStart != sled->selEnd){

                refresh_rc.top = sled->rcVis.top;
                refresh_rc.bottom = sled->rcVis.bottom;
                refresh_rc.right = sled->rcVis.right;

                scroll_rc.right = sled->rcVis.right;

                calc_glyph_pos_cx(hWnd, sled, sled->selStart, &sel_start_x);
                calc_glyph_pos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
                if (sled->selStart < sled->selEnd){
                    scroll_rc.left = sel_start_x + 1;
                }
                else{
                    scroll_rc.left = sel_end_x + 1;
                }

                sleBackspaceText (hWnd, sled, del, TRUE, TRUE);
                edtSetCaretPos (hWnd, sled);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(hWnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (sled->nContW <= sled->nVisW){
                    InvalidateRect(hWnd, NULL, TRUE);
                }

            }
            else{
                refresh_rc.top = sled->rcVis.top;
                refresh_rc.bottom = sled->rcVis.bottom;
                refresh_rc.right = sled->rcVis.right;

                scroll_rc.right = sled->rcVis.right;

                scroll_rc.left = get_caretpos_x(hWnd); 
                calc_glyph_pos_cx(hWnd, sled, sled->editPos, &old_editpos_x);

                sleBackspaceText (hWnd, sled, del, TRUE, TRUE);
                calc_glyph_pos_cx(hWnd, sled, sled->editPos, &cur_editpos_x);
                edtSetCaretPos (hWnd, sled);

                scroll_len = old_editpos_x - cur_editpos_x;
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(hWnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (sled->nContW <= sled->nVisW){
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }

        }
        else{//right scroll window while sled->nContX > 0
            if (sled->selStart != sled->selEnd){

                refresh_rc.top = sled->rcVis.top;
                refresh_rc.bottom = sled->rcVis.bottom;
                refresh_rc.left = sled->rcVis.left;

                calc_glyph_pos_cx(hWnd, sled, sled->selStart, &sel_start_x);
                calc_glyph_pos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
                if (sled->selStart < sled->selEnd){
                    scroll_rc.right = sel_end_x + 1 - sled->nContX;
                }
                else{
                    scroll_rc.right = sel_start_x + 1 - sled->nContX;
                }

                scroll_rc.left = sled->rcVis.left;
                sleBackspaceText (hWnd, sled, del, TRUE, TRUE);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.right = refresh_rc.left + scroll_len;
                ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
                edtSetCaretPos (hWnd, sled);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (scroll_len >= sled->nVisW){
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                if (sled->nContW <= sled->nVisW){
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                if (sled->nContX <= 0){
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            else{
                refresh_rc.top = sled->rcVis.top;
                refresh_rc.bottom = sled->rcVis.bottom;
                refresh_rc.left = sled->rcVis.left;

                old_caret_x = get_caretpos_x(hWnd);
                scroll_rc.left = sled->rcVis.left;
                sleBackspaceText (hWnd, sled, del, TRUE, TRUE);
                edtSetCaretPos (hWnd, sled);
                cur_caret_x = get_caretpos_x(hWnd);    
                scroll_rc.right = cur_caret_x;

                scroll_len = cur_caret_x - old_caret_x;
                refresh_rc.right = refresh_rc.left + abs(scroll_len);
                ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(hWnd, &refresh_rc, TRUE);

                if (sled->nContW <= sled->nVisW){
                    InvalidateRect(hWnd, NULL, TRUE);
                }

                if (sled->nContX <= 0){
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }

        }

    }
    sled->changed = TRUE;
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
}

static void esleft_del_refresh(HWND hWnd, PBIDISLEDITDATA sled, int del)
{
    int old_nContX = sled->nContX;
    RECT scroll_rc, del_rc, refresh_rc;
    int glyphs_len_x;    
    int sel_start_x, sel_end_x;
    int old_caret_x, cur_caret_x;
    int scroll_len;

    scroll_rc.top = sled->rcVis.top;
    scroll_rc.bottom = sled->rcVis.bottom;
    del_rc.bottom = sled->rcVis.bottom;
    del_rc.top = sled->rcVis.top;  
    refresh_rc.top = sled->rcVis.top;
    refresh_rc.bottom = sled->rcVis.bottom;

    if (sled->selStart != sled->selEnd){
        if (sled->selStart < sled->selEnd){
            calc_glyph_pos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
            scroll_rc.left = sel_end_x;
        }
        else{
            calc_glyph_pos_cx(hWnd, sled, sled->selStart, &sel_start_x);
            scroll_rc.left = sel_start_x;
        }

         scroll_rc.right = sled->rcVis.right;
     }
     else{
         calc_glyph_pos_cx(hWnd, sled, sled->editPos, &scroll_rc.left);
         scroll_rc.left = scroll_rc.left - old_nContX;
         scroll_rc.right = sled->rcVis.right;
     }

     old_caret_x = get_caretpos_x(hWnd);
     calc_glyph_pos_cx(hWnd, sled, GLYPHSLEN, &glyphs_len_x);

     sleBackspaceText (hWnd, sled, del, TRUE, TRUE);

     calc_glyph_pos_cx(hWnd, sled, sled->editPos, &del_rc.left);
     del_rc.left = del_rc.left - old_nContX;
     del_rc.right = scroll_rc.left; 

     scroll_len = del_rc.left - scroll_rc.left;

     if (abs(sel_start_x - sel_end_x) >= sled->nVisW){
         InvalidateRect(hWnd, NULL, TRUE);
     }
     else{
         if ((old_nContX > 0) && 
             ((glyphs_len_x - old_nContX) <= sled->rcVis.right)){

             cur_caret_x = get_caretpos_x(hWnd);    
             if (cur_caret_x != old_caret_x){
                 scroll_len = cur_caret_x - old_caret_x;   
                 scroll_rc.left = sled->rcVis.left;
                 scroll_rc.right = old_caret_x;

                 ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
                
                 refresh_rc.left = old_caret_x;
                 refresh_rc.right = cur_caret_x;

                 InvalidateRect(hWnd, &refresh_rc, TRUE);
             }
             else if (sled->selStart != sled->selEnd) {
                 scroll_len = sel_start_x - sel_end_x;   
                 scroll_rc.left = sled->rcVis.left;
                 scroll_rc.right = old_caret_x;

                 ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
                
                 refresh_rc.left = old_caret_x;
                 refresh_rc.right = cur_caret_x;

                 InvalidateRect(hWnd, &refresh_rc, TRUE);
             }
             else{
                 InvalidateRect(hWnd, NULL, TRUE);    
             }
         }
         else{
             int rl;

             InvalidateRect(hWnd, &del_rc, TRUE);
             ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
             calc_glyph_pos_cx(hWnd, sled, GLYPHSLEN, &rl);

             if (((sled->nContW - sled->nContX) == sled->nVisW) && 
                 (old_nContX > 0)){
                 InvalidateRect(hWnd, NULL, TRUE);    
             }
             else if (rl < sled->nVisW){
                refresh_rc.left = rl;
                refresh_rc.right = sled->rcVis.right;

                InvalidateRect(hWnd, &refresh_rc, TRUE);

             }
             else{
                 refresh_rc.left = sled->rcVis.right - abs(scroll_len);
                 refresh_rc.right = sled->rcVis.right;

                 InvalidateRect(hWnd, &refresh_rc, TRUE);
            }
        }
    }

}

static int sleKeyDown (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL    bChange = FALSE;
    DWORD   dwStyle = GetWindowStyle (hWnd);
    PCONTROL pCtrl   = gui_Control(hWnd);
    PBIDISLEDITDATA sled = (PBIDISLEDITDATA) (pCtrl->dwAddData2);

    switch (LOWORD (wParam)) {

    case SCANCODE_KEYPADENTER:
    case SCANCODE_ENTER:
        NotifyParent (hWnd, pCtrl->id, EN_ENTER);
        return 0;

    case SCANCODE_HOME:
    {
        BOOL refresh = FALSE;

        if(lParam & KS_SHIFT) {
            make_pos_visible (hWnd, sled, sled->startx);
            sled->selStart = 0;
            sled->selEnd = sled->editPos;
            check_caret();
            InvalidateRect (hWnd, &sled->rcVis, TRUE);

            NotifyParent (hWnd, pCtrl->id, EN_SELCHANGED);

            return 0;
        }
            
        if (sled->selStart != sled->selEnd) {
            ShowCaret(hWnd);
            refresh = TRUE;
        }

        sled->editPos  = 0;
        sled->selStart = sled->selEnd = 0;

        if (!edtSetCaretPos (hWnd, sled) && refresh)
            InvalidateRect (hWnd, &sled->rcVis, TRUE);


        return 0;
    }
   
    case SCANCODE_END:
    {
        BOOL refresh = FALSE;
       
        if(lParam & KS_SHIFT) {
            sled->selStart = sled->editPos;
            sled->selEnd = GLYPHSLEN;
            check_caret();
            make_charpos_visible (hWnd, sled, sled->selEnd, NULL);
            InvalidateRect(hWnd, NULL, TRUE);

            NotifyParent (hWnd, pCtrl->id, EN_SELCHANGED);

            return 0;
        }

        if (sled->selStart != sled->selEnd) {
            ShowCaret(hWnd);
            refresh = TRUE;
        }

        sled->editPos = GLYPHSLEN;
        sled->selStart = sled->selEnd = sled->editPos;

        if (!edtSetCaretPos (hWnd, sled) && refresh)
            InvalidateRect (hWnd, NULL, TRUE);


        return 0;
    }

    case SCANCODE_CURSORBLOCKLEFT:
    {
        BOOL refresh = FALSE;

        if(lParam & KS_SHIFT) {
            if (sled->selStart == sled->selEnd) {
                sled->selStart = sled->selEnd = sled->editPos;
            }

            if (sled->selStart < sled->editPos ||
                        sled->selStart == sled->selEnd) {
                if (sled->selStart > 0)
                    sled->selStart--;
            }
            else {
                sled->selEnd--;
            }

            check_caret ();
            make_charpos_visible (hWnd, sled, sled->selStart, NULL);
            InvalidateRect (hWnd, NULL, TRUE);

            NotifyParent (hWnd, pCtrl->id, EN_SELCHANGED);
            return 0;
        }

        if (sled->editPos > 0) {
            sled->editPos--; 
        }

        if (sled->selStart != sled->selEnd) {
            ShowCaret (hWnd);
            ActiveCaret (hWnd);
            refresh = TRUE;
        }

        sled->selStart = sled->selEnd = sled->editPos;
        if (!edtSetCaretPos (hWnd, sled) && refresh) {
            InvalidateRect (hWnd, NULL, TRUE);
        }

        return 0;
    }
    
    case SCANCODE_CURSORBLOCKRIGHT:
    {
        BOOL refresh = FALSE;

        if(lParam & KS_SHIFT) {
            if(sled->selStart == sled->selEnd)
                sled->selStart = sled->selEnd = sled->editPos;

            if (sled->selStart == sled->selEnd || 
                            sled->selEnd > sled->editPos) {
                if (sled->selEnd < GLYPHSLEN)
                    sled->selEnd++; 
            }
            else {
                sled->selStart++; 
            }

            check_caret ();
            make_charpos_visible (hWnd, sled, sled->selEnd, NULL);
            InvalidateRect (hWnd, NULL, TRUE);

            NotifyParent (hWnd, pCtrl->id, EN_SELCHANGED);
            return 0;
        }

        if (sled->editPos < GLYPHSLEN) {
            sled->editPos++; 
        }

        if (sled->selStart != sled->selEnd) {
            ShowCaret (hWnd);
            ActiveCaret (hWnd);
            refresh = TRUE;
        }

        sled->selStart = sled->selEnd = sled->editPos;
        if (!edtSetCaretPos (hWnd, sled) && refresh)
            InvalidateRect (hWnd, NULL, TRUE);
    }
    return 0;
    
    case SCANCODE_REMOVE:
    {
        int del;
        int oldpos = sled->editPos;

        if ((dwStyle & ES_READONLY))
            return 0;

        if (sled->editPos == GLYPHSLEN && sled->selStart == sled->selEnd)
            return 0;

        if (sled->selStart == sled->selEnd && sled->editPos < GLYPHSLEN) {
            sled->editPos ++; 
        }

        del = sled->editPos - oldpos;

        if (dwStyle & ES_LEFT) {
            esleft_del_refresh(hWnd, sled, del);

            if (!edtSetCaretPos (hWnd, sled)) {
            //InvalidateRect (hWnd, NULL, TRUE);
            }

            sled->changed = TRUE;
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);

        } 
        else if (dwStyle & ES_RIGHT) {
            esright_del_refresh(hWnd, sled, del);
        }
        else if (dwStyle & ES_CENTER) {
            sleBackspaceText (hWnd, sled, del, FALSE, TRUE);
        }
        else {
            sleBackspaceText (hWnd, sled, del, FALSE, TRUE);
        }

    }
    break;

    case SCANCODE_A:
    {
        if (lParam & KS_CTRL) {
            sleSetSel (hWnd, sled, 0, TEXTLEN);
        }
        return 0;
    }

#ifdef _MGHAVE_CLIPBOARD
    case SCANCODE_C:
    case SCANCODE_X:
    {
        if ((lParam & KS_CTRL) && (sled->selEnd - sled->selStart > 0)) {
            SetClipBoardData (CBNAME_TEXT, TEXT + sled->selStart, 
                        sled->selEnd - sled->selStart, CBOP_NORMAL);
            if (wParam == SCANCODE_X && !(GetWindowStyle(hWnd) & ES_READONLY)) {
                sleInsertText (hWnd, sled, NULL, 0, FALSE);
            }
        }
        return 0;
    }

    case SCANCODE_V:
    {
        if (!(lParam & KS_CTRL))
            return 0;
        return sleInsertCbText (hWnd, sled);
    }
#endif /* _MGHAVE_CLIPBOARD */

    case SCANCODE_INSERT:
        sled->status ^= EST_REPLACE;
        break;

    default:
        break;
    } //end switch
       
    if (bChange){
        sled->changed = TRUE;
        NotifyParent (hWnd, pCtrl->id, EN_CHANGE);
    }

    return 0;
}
static void 
esright_input_char_refresh (HWND hWnd, 
        PBIDISLEDITDATA sled, char *charBuffer, int chars)
{
    RECT scroll_rc;
    int scroll_len;
    int old_ncontw, cur_ncontw;
    int cur_caret_x;

    if (sled->selStart != sled->selEnd) {
        sleInsertText (hWnd, sled, (char* )charBuffer, chars, FALSE);
    }
    else {//left scroll window
        scroll_rc.top = sled->rcVis.top;
        scroll_rc.bottom = sled->rcVis.bottom;
        calc_content_width(hWnd, sled, TEXTLEN, &old_ncontw);
        sleInsertText (hWnd, sled, (char* )charBuffer, chars,TRUE);

        calc_content_width(hWnd, sled, TEXTLEN, &cur_ncontw);
        scroll_len = abs(old_ncontw- cur_ncontw); 
        cur_caret_x = get_caretpos_x(hWnd);
        scroll_rc.left = sled->rcVis.left;
        scroll_rc.right = cur_caret_x;

        ScrollWindow(hWnd, -scroll_len, 0, &scroll_rc, NULL);
        UpdateWindow(hWnd, FALSE);
        edtSetCaretPos (hWnd, sled);

        sled->changed = TRUE;
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
    }
}

static void 
esleft_input_char_refresh (HWND hWnd, 
        PBIDISLEDITDATA sled, char *charBuffer, int chars)
{
    int old_caretpos_x;
    int old_sel_start, old_sel_end; 
    int old_sel_start_x, old_sel_end_x, cur_sel_start_x;
    RECT scroll_rc, refresh_rc;
    int scroll_len;
    int old_edit_pos_x, cur_edit_pos_x;
    int old_nContX;
    //char_size is width of input chars; sel_size is width of the selected area 
    int char_size, sel_size;
    int glyphs_len_x;

    old_sel_start = sled->selStart;
    old_sel_end = sled->selEnd; 

    old_caretpos_x = get_caretpos_x(hWnd);
    calc_glyph_pos_cx(hWnd, sled, sled->editPos, &old_edit_pos_x);
    calc_glyph_pos_cx(hWnd, sled, GLYPHSLEN, &glyphs_len_x);
    old_nContX= sled->nContX;
    calc_glyph_pos_cx(hWnd, sled, old_sel_start, &old_sel_start_x);
    calc_glyph_pos_cx(hWnd, sled, old_sel_end, &old_sel_end_x);
    old_sel_start_x = old_sel_start_x - sled->nContX;
    old_sel_end_x = old_sel_end_x - sled->nContX;

    sleInsertText (hWnd, sled, (char* )charBuffer, chars, TRUE);

    calc_glyph_pos_cx(hWnd, sled, sled->editPos, &cur_edit_pos_x);
    calc_glyph_pos_cx(hWnd, sled, old_sel_start, &cur_sel_start_x);
    char_size = cur_edit_pos_x - cur_sel_start_x;
    sel_size = abs(old_sel_start_x - old_sel_end_x);

    scroll_rc.top = sled->rcVis.top;		 
    scroll_rc.bottom = sled->rcVis.bottom;		 
    refresh_rc.top = sled->rcVis.top;
    refresh_rc.bottom = sled->rcVis.bottom;

    if (char_size >= sled->nVisW){
        InvalidateRect(hWnd, NULL, TRUE);   
    }
    else{
        if (old_sel_start != old_sel_end){

            if (sel_size >= sled->nVisW){   
                InvalidateRect(hWnd, NULL, TRUE);
            }
            else{
                if (char_size < sel_size){
                    //right scroll window 
                    if ((old_nContX > 0) && 
                        ((glyphs_len_x - old_nContX) <= sled->rcVis.right)) {
                        scroll_len = sel_size - char_size;  
                        scroll_rc.left = sled->rcVis.left;		 
                        scroll_rc.right = old_sel_start_x < old_sel_end_x
                                ? old_sel_start_x : old_sel_end_x;

                        ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                        refresh_rc.left = scroll_rc.right;
                        refresh_rc.right = refresh_rc.left + sel_size + 
                                    scroll_len;

                        InvalidateRect(hWnd, &refresh_rc, TRUE);
                    } 
                    else{//left scroll window 
                        scroll_len = -(sel_size - char_size);  
                        scroll_rc.left = old_sel_start_x < old_sel_end_x 
                            ? old_sel_end_x : old_sel_start_x;		 
                        scroll_rc.right = sled->rcVis.right;

                        ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                        refresh_rc.left = scroll_rc.left - sel_size; 
                        refresh_rc.right = scroll_rc.left; 

                        InvalidateRect(hWnd, &refresh_rc, TRUE);
                    }
                }
                else{//when char_size > sel_size right scroll window 
                    scroll_len = char_size - sel_size;  
                    scroll_rc.left = old_sel_start_x < old_sel_end_x 
                        ? old_sel_end_x : old_sel_start_x;		 
                    scroll_rc.right = sled->rcVis.right;		 

                    ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                    refresh_rc.left = scroll_rc.left - sel_size;
                    refresh_rc.right = refresh_rc.left + char_size + 1; 

                    InvalidateRect(hWnd, &refresh_rc, TRUE);
                }
            }
        }
        else{
            int right;   
            char_size = cur_edit_pos_x - old_edit_pos_x; 

            calc_glyph_pos_cx(hWnd, sled, GLYPHSLEN, &right);

            if (old_caretpos_x < sled->rcVis.right){//right scroll window 
                scroll_len = cur_edit_pos_x - old_edit_pos_x;     
                if ((sled->rcVis.right - old_caretpos_x) < scroll_len){//
                    scroll_len = -(scroll_len - 
                                   (sled->rcVis.right- old_caretpos_x)); 
                    scroll_rc.left = sled->rcVis.left;   
                    scroll_rc.right = old_caretpos_x; 
                    ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                    refresh_rc.left = sled->rcVis.right - char_size;
                    refresh_rc.right = sled->rcVis.right;

                    InvalidateRect(hWnd, &refresh_rc, TRUE);
                }
                else{
                    scroll_rc.left = old_caretpos_x;   
                    scroll_rc.right = (right < sled->rcVis.right) 
                        ? right+1 : sled->rcVis.right; 
                    ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                    refresh_rc.left = sled->rcVis.right - char_size;
                    refresh_rc.right = sled->rcVis.right;

                    InvalidateRect(hWnd, &refresh_rc, TRUE);
                }
            }
            else{//left scroll window 
                scroll_len = old_edit_pos_x - cur_edit_pos_x;     
                ScrollWindow(hWnd, scroll_len, 0, &sled->rcVis, NULL);

                refresh_rc.left = sled->rcVis.right - char_size;
                refresh_rc.right = sled->rcVis.right;
                
                InvalidateRect(hWnd, &refresh_rc, TRUE);
            }

            if (((sled->nContW - sled->nContX) == sled->nVisW) 
                    && (old_nContX > 0)){
                InvalidateRect(hWnd, NULL, TRUE);    
            }
        }
    }
}

static void sledit_refresh_caret (HWND hWnd, PBIDISLEDITDATA sled, BOOL bInited)
{
    DWORD   dwStyle = GetWindowStyle(hWnd);
    SIZE    glyphs_size;
    int     outchar;
    HDC     hdc;

    if (!sled)
        return;

    hdc = GetClientDC (hWnd);
    outchar = GetGlyphsExtentPoint (hdc, GLYPHS, GLYPHSLEN, 0, &glyphs_size);
    ReleaseDC (hdc);

    if (bInited) {
        if (dwStyle & ES_RIGHT)
            sled->editPos = outchar;
        else if (dwStyle & ES_CENTER)
            sled->editPos = outchar/2;
        else
            sled->editPos = 0;
    }
    if (!edtSetCaretPos (hWnd, sled)) {
        InvalidateRect (hWnd, NULL, TRUE);
    }
}

static int sledit_reset_text (HWND hWnd, PBIDISLEDITDATA sled, char* str)
{
    DWORD   dwStyle = GetWindowStyle(hWnd);
    char*   buffer  = str;

    if (!sled || !buffer)
        return -1;

    if (dwStyle & ES_PASSWORD) {
        buffer = strdup (buffer);
        check_valid_passwd (buffer);
    }

    sled->status = 0;
    sled->selStart = 0;
    sled->selEnd = 0;

    if (TEXT) {
        //free (TEXT);
        //TEXT = NULL;
        sled->str_content.buffsize = 0;
        TEXTLEN = 0;
        sled->glyph_content.glyphs_buffsize = 0;
        sled->glyph_content.glyphmap_buffsize = 0;
        sled->glyph_content.glyphs_len = 0;
    }

    sledit_settext (hWnd, sled, buffer);
    recalcSize (hWnd, sled, TRUE);

    if (dwStyle & ES_PASSWORD) {
        free (buffer);
    }

    sledit_refresh_caret (hWnd, sled, TRUE);

    sled->changed = TRUE;
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_UPDATE);
    return 0;
}

static LRESULT
SLEditCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    DWORD       dwStyle;
    HDC         hdc;
    PBIDISLEDITDATA sled = NULL;

    dwStyle     = GetWindowStyle(hWnd);

    if (message != MSG_CREATE)
        sled = (PBIDISLEDITDATA) GetWindowAdditionalData2 (hWnd);

    switch (message) {

    case MSG_CREATE:
        if ( !(sled = calloc (1, sizeof (BIDISLEDITDATA))) )
            return -1;
        if (sledit_init (hWnd, sled) < 0)
            return -1;
        break;

    case MSG_DESTROY:
        sledit_destroy (hWnd, sled);
        free (sled);
        break;

    case MSG_PAINT:
    {
        hdc = BeginPaint (hWnd);
        slePaint (hWnd, hdc, sled);
        EndPaint (hWnd, hdc);
        return 0;
    }

    case MSG_SIZECHANGED:
        recalcSize (hWnd, sled, FALSE);
        return 0;

    case MSG_FONTCHANGING:
        if (dwStyle & ES_PASSWORD)
            return -1;
        return 0;

    case MSG_FONTCHANGED:
    {
        sled->starty  = sled->topMargin + (sled->rcVis.bottom 
                - sled->rcVis.top - GetWindowFont(hWnd)->size - 1)/2;

        DestroyCaret (hWnd);
        CreateCaret (hWnd, NULL, 1, GetWindowFont (hWnd)->size);

        recalcSize (hWnd, sled, FALSE);

        return 0;
    }

    case MSG_SETCURSOR:
        if (dwStyle & WS_DISABLED) {
            SetCursor (GetSystemCursor (IDC_ARROW));
            return 0;
        }
        break;

    case MSG_KILLFOCUS:
        if (sled->status & EST_FOCUSED) {
            BOOL refresh = FALSE;

            sled->status &= ~EST_FOCUSED;
            HideCaret (hWnd);
            if ( sled->selStart != sled->selEnd 
                    || (dwStyle & ES_TIP && TEXTLEN <= 0)) {
                refresh = TRUE;
            }
            if (sled->selStart != sled->selEnd && !(dwStyle & ES_NOHIDESEL))
                sled->selStart = sled->selEnd = sled->editPos;

            if (refresh)
                InvalidateRect (hWnd, NULL, TRUE);

            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_KILLFOCUS);

            if (sled->changed){
                sled->changed = FALSE;
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CONTCHANGED);
            }
        }
        break;
 
    case MSG_SETFOCUS:
        if (sled->status & EST_FOCUSED)
            break;
        
        sled->status |= EST_FOCUSED;

        ActiveCaret (hWnd);
        ShowCaret(hWnd);

        if (dwStyle & ES_AUTOSELECT) {
            if (sleSetSel (hWnd, sled, 0, TEXTLEN) <= 0 && dwStyle & ES_TIP)
                InvalidateRect (hWnd, NULL, TRUE);
            if (lParam == 1)
                sled->status |= EST_TMP;
        }
        else if ( (dwStyle & ES_NOHIDESEL && sled->selStart != sled->selEnd)
                    || (dwStyle & ES_TIP && TEXTLEN <= 0)) {
            InvalidateRect (hWnd, NULL, TRUE);
        }

        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_SETFOCUS);
        break;
        
    case MSG_ENABLE:
        if ( (!(dwStyle & WS_DISABLED) && !wParam)
                || ((dwStyle & WS_DISABLED) && wParam) ) {
            if (wParam)
                ExcludeWindowStyle(hWnd,WS_DISABLED);
            else
                IncludeWindowStyle(hWnd,WS_DISABLED);

            InvalidateRect (hWnd, NULL, TRUE);
        }
        return 0;

    case MSG_GETTEXTLENGTH:
        return TEXTLEN;
    
    case MSG_GETTEXT:
    {
        char*   buffer = (char*)lParam;
        int     len;

        len = MIN ((int)wParam, TEXTLEN);
        memcpy (buffer, TEXT, len);
        buffer [len] = '\0';
        return len;
    }

    case MSG_SETTEXT:
        return sledit_reset_text (hWnd, sled, (char*)lParam);

    case MSG_KEYDOWN:
    {
        return sleKeyDown (hWnd, wParam, lParam);
    }

    case MSG_CHAR:
    {
        unsigned char charBuffer [4];
        int chars;

        _MG_PRINTF("get char-----------------char----%p\n", (PVOID)wParam);

        if (dwStyle & ES_READONLY)
            return 0;

        charBuffer [0] = FIRSTBYTE (wParam);
        charBuffer [1] = SECONDBYTE (wParam);
        charBuffer [2] = THIRDBYTE (wParam);
        charBuffer [3] = FOURTHBYTE (wParam);

        if (charBuffer [3]) {
            chars = 4;
        }
        else if (charBuffer [2]) {
            chars = 3;
        }
        else if (charBuffer [1]) {
            chars = 2;
        }
        else {
            chars = 1;

            if (charBuffer [0] == 127) // BS
                charBuffer [0] = '\b';

            if (dwStyle & ES_UPPERCASE) {
                charBuffer [0] = toupper (charBuffer[0]);
            }
            else if (dwStyle & ES_LOWERCASE) {
                charBuffer [0] = tolower (charBuffer[0]);
            }
        }
    
        if (chars == 1) {
            if (charBuffer [0] < 0x20 && charBuffer[0] != '\b') //FIXME
                return 0;
        }

        if (wParam == '\b') { // backspace
            int del = 1;

            if (sled->editPos == 0 && sled->selStart == sled->selEnd)
                 return 0;

            if (dwStyle & ES_LEFT) {
                esleft_del_refresh(hWnd, sled, del);
                set_edit_caret_pos(hWnd, sled);
                sled->changed = TRUE;
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
            }
            else if (dwStyle & ES_RIGHT) {
                esright_backspace_refresh(hWnd, sled, del);
            }
            else if (dwStyle & ES_CENTER) {
                sleBackspaceText (hWnd, sled, del, FALSE, FALSE);
            }
            else {
                sleBackspaceText (hWnd, sled, del, FALSE, FALSE);
            }
        }
        else{
            if (dwStyle & ES_LEFT) {
            //if ((dwStyle&0xffff) ==  ES_LEFT) {
	            esleft_input_char_refresh(hWnd, sled, (char*)charBuffer, chars);
                set_edit_caret_pos(hWnd, sled);

                sled->changed = TRUE;
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
            }
            else if (dwStyle & ES_RIGHT) {
              esright_del_refresh(hWnd, sled, 0);
              esright_input_char_refresh(hWnd, sled, (char*)charBuffer, 
                                chars);
            }
            else if (dwStyle & ES_CENTER) {
                sleInsertText (hWnd, sled, (char* )charBuffer, chars,FALSE);

            }
            else {
                sleInsertText (hWnd, sled, (char* )charBuffer, chars, FALSE);
            }
        }
        return 0;
    }

    case MSG_LBUTTONDBLCLK:
        sleSetSel (hWnd, sled, 0, GLYPHSLEN);
        NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_DBLCLK);
        break;
        
    case MSG_LBUTTONDOWN:
    {
        if ( sled->status & EST_TMP) {
            sled->status &= ~EST_TMP;
            break;
        }

        if (HISWORD(lParam) < sled->rcVis.top 
                || HISWORD(lParam) > sled->rcVis.bottom)
            break;

        set_caret_pos (hWnd, sled, LOSWORD(lParam), FALSE);

        ActiveCaret (hWnd);
        ShowCaret(hWnd);
        SetCapture(hWnd);

        InvalidateRect(hWnd, NULL, TRUE);

        break;
    }
 
    case MSG_NCLBUTTONUP:
    case MSG_LBUTTONUP:
        if (GetCapture() == hWnd)
            ReleaseCapture();
        NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_CLICKED);

        if (sled->selStart != sled->selEnd)
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_SELCHANGED);

        break;

    case MSG_MOUSEMOVE:
    {
        if (GetCapture () == hWnd)
            sleMouseMove (hWnd, sled, lParam);
        return 0;
    }
        
    case MSG_GETDLGCODE:
        return DLGC_WANTCHARS| DLGC_HASSETSEL| DLGC_WANTARROWS| DLGC_WANTENTER;

    case MSG_DOESNEEDIME:
        if (dwStyle & ES_READONLY)
            return IME_WINDOW_TYPE_READONLY;
        else if (dwStyle & ES_PASSWORD)
            return IME_WINDOW_TYPE_PASSWORD;
        else
            return IME_WINDOW_TYPE_NOT_EDITABLE;
    
    case EM_GETCARETPOS:
    case EM_GETSELPOS:
    {
        int nr_chars, pos;
        int* line_pos = (int *)wParam;
        int* char_pos = (int *)lParam;

        if (message == EM_GETSELPOS)
            pos = (sled->editPos == sled->selStart) 
                ? sled->selEnd : sled->selStart;
        else
            pos = sled->editPos;
        nr_chars = GetTextMCharInfo (GetWindowFont (hWnd), 
                         (const char* )(TEXT), pos, NULL);

        if (line_pos) *line_pos = 0;
        if (char_pos) *char_pos = nr_chars;

        return pos;
    }

    case EM_SETCARETPOS:
    case EM_SETSEL:
    {
        int glyph_pos = (int)lParam;

        if (glyph_pos < 0)
            return -1;

        if (glyph_pos > GLYPHSLEN)
            glyph_pos = GLYPHSLEN;

        if (message == EM_SETCARETPOS) {
            sled->editPos = glyph_pos;
            sled->selStart = sled->selEnd = 0;
            edtSetCaretPos (hWnd, sled);
            return sled->editPos;
        }
        else {
            int start, end;
            if (sled->editPos < glyph_pos) {
                start = sled->editPos;
                end = glyph_pos;
            }
            else {
                start = glyph_pos;
                end = sled->editPos;
            }
            return sleSetSel (hWnd, sled, start, end);
        }
    }

    case EM_SETREADONLY:
        if (wParam)
            IncludeWindowStyle (hWnd, ES_READONLY);
        else
            ExcludeWindowStyle (hWnd, ES_READONLY);
        return 0;
 
    case EM_SETPASSWORDCHAR:
        if (sled->passwdChar != (int)wParam) {
            sled->passwdChar = (int)wParam;
            if (dwStyle & ES_PASSWORD) {
                InvalidateRect (hWnd, NULL, TRUE);
            }
        }
        return 0;
 
    case EM_GETPASSWORDCHAR:
    {
        if (lParam)
            *((int*)lParam) = sled->passwdChar;
        return 0;
    }
    
    case EM_GETMAXLIMIT:
        return sled->hardLimit;

    case EM_LIMITTEXT:
    {
        int newLimit = (int)wParam;

        if (newLimit >= 0) {
            if (TEXTLEN > newLimit) {
                Ping ();
                return -1;
            }
            else {
                sled->hardLimit = newLimit;
                return 0;
            }
        }
        return -1;
    }
    
    case EM_GETTIPTEXT:
    {
        int len, tip_len;
        char *buffer = (char *)lParam;

        if (!(dwStyle & ES_TIP))
            return -1;

        tip_len = strlen (sled->tiptext);

        if (!buffer)
            return tip_len;

        if (wParam >= 0)
            len = (wParam > DEF_TIP_LEN) ? DEF_TIP_LEN : wParam;
        else
            len = DEF_TIP_LEN;

        strncpy (buffer, sled->tiptext, len);
        buffer[len] = '\0';

        return tip_len;
    }

    case EM_SETTIPTEXT:
    {
        int len;

        if (!(dwStyle & ES_TIP) || !lParam)
            return -1;

        if (wParam >= 0)
            len = (wParam > DEF_TIP_LEN) ? DEF_TIP_LEN : wParam;
        else
            len = DEF_TIP_LEN;
        strncpy (sled->tiptext, (char *)lParam, len);
        sled->tiptext[len] = '\0';

        if (TEXTLEN <= 0)
            InvalidateRect (hWnd, NULL, TRUE);

        return strlen(sled->tiptext);
    }

    case EM_GETSEL:
    {
        char *buffer = (char *)lParam;
        int len;

        if (!buffer || sled->selEnd - sled->selStart <= 0)
            return 0;

        if (wParam < 0)
            len = sled->selEnd - sled->selStart;
        else
            len = MIN(sled->selEnd - sled->selStart, (int)wParam);

        strncpy (buffer, 
            (const char*)(TEXT + sled->selStart), len); 

        return len;
    }

    case EM_INSERTCBTEXT:
    {
#ifdef _CLIPBOAR_SUPPORT
        return sleInsertCbText (hWnd, sled);
#endif
    }

    case EM_COPYTOCB:
    case EM_CUTTOCB:
    {
#ifdef _MGHAVE_CLIPBOARD
        if (sled->selEnd - sled->selStart > 0) {
            SetClipBoardData (CBNAME_TEXT, 
                    TEXT + sled->selStart, 
                    sled->selEnd - sled->selStart, CBOP_NORMAL);
            if (message == EM_CUTTOCB 
                    && !(GetWindowStyle(hWnd) & ES_READONLY)) {
                sleInsertText (hWnd, sled, NULL, 0, FALSE);
            }
            return sled->selEnd - sled->selStart;
        }
#endif
        return 0;
    }

    default:
        break;

    } // end switch

    return DefaultControlProc (hWnd, message, wParam, lParam);
}

#endif /* _MGCTRL_BIDISLEDIT */
