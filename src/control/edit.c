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
** edit.c: the Single Line Edit Control module.
**
** Create date: 1999/8/26
**
** TODO:
**    * Replace
**    * Undo.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

#ifdef _MGCTRL_SLEDIT

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/edit.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "clipboard.h"

#include "text.h"
#include "edit_impl.h"

static void set_line_width (HWND hWnd, PSLEDITDATA sled);
static void mySetCaretPos (HWND hWnd, PSLEDITDATA sled, int x);
static LRESULT SLEditCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void sledit_refresh_caret (HWND hWnd, PSLEDITDATA sled, BOOL bInited);
static BOOL make_charpos_visible (HWND hWnd, PSLEDITDATA sled, int charPos, int *cx);

/* 
 * for bug 4894, when select text don't hide caret, because this will close
 * the ime window. by humingming 2010.8.17
 */
/*#define check_caret() \
            if(sled->selStart != sled->selEnd) \
                HideCaret(hWnd); \
            else \
                ShowCaret(hWnd);
*/
#define check_caret() ShowCaret(hWnd);


#define shift_one_char_r(pos) \
        { \
            int len = CHLENNEXT((sled->content.string + sled->pos), (sled->content.txtlen - sled->pos)); \
            sled->pos += len; \
        }

#define shift_one_char_l(pos) \
        { \
            int len = CHLENPREV((const char* )(sled->content.string),(const char* )(sled->content.string + sled->pos) ); \
            sled->pos -= len; \
        }

#define PASSWORD_REPLACEMENT '*'
BOOL RegisterSLEditControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_SLEDIT;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_IBEAM);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = SLEditCtrlProc;

    if (AddNewControlClass (&WndClass) != ERR_OK)
        return FALSE;

    WndClass.spClassName = CTRL_EDIT;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

/* -------------------------------------------------------------------------- */

static void setup_dc (HWND hWnd, SLEDITDATA *sled, HDC hdc, BOOL bSel)
{
    DWORD dwStyle = GetWindowStyle (hWnd);
    if (!bSel) {
        SetBkMode (hdc, BM_TRANSPARENT);
        if (dwStyle & WS_DISABLED)
            SetTextColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_DISABLED_ITEM));
        else
            SetTextColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_WINDOW));
        SetBkColor (hdc, GetWindowBkColor (hWnd));
    }
    else {
        SetBkMode (hdc, BM_OPAQUE);

        if (dwStyle & WS_DISABLED)
            SetTextColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_DISABLED_ITEM));
        else
            SetTextColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_SELECTED_ITEM));

        if (sled->status & EST_FOCUSED)
            SetBkColor (hdc, GetWindowElementPixel (hWnd, WE_BGC_SELECTED_ITEM));
        else
            SetBkColor (hdc, GetWindowElementPixel (hWnd, WE_BGC_SELECTED_LOSTFOCUS));
    }
}

/* -------------------------------------------------------------------------- */

static int sledit_settext (PSLEDITDATA sled, const char *newtext)
{
    int len, txtlen;

    txtlen = strlen (newtext);
    len = (txtlen <= 0) ? sled->nBlockSize : txtlen;

    if (sled->hardLimit >= 0 && txtlen > sled->hardLimit) {
        return -1;
    }
 
    /* free the old text */
    if (sled->content.string)
        testr_free (&sled->content);

    if (!testr_alloc (&sled->content, len, sled->nBlockSize))
        return -1;

    if (newtext && txtlen > 0)
        testr_setstr (&sled->content, newtext, txtlen);
    else
        sled->content.txtlen = 0;

    return 0;
}

static void get_content_width (HWND hWnd, PSLEDITDATA sled)
{
    GetClientRect (hWnd, &sled->rcVis);

    sled->rcVis.left += sled->leftMargin;
    sled->rcVis.top += sled->topMargin;
    sled->rcVis.right -= sled->rightMargin;
    sled->rcVis.bottom -= sled->bottomMargin;

    sled->nVisW = sled->rcVis.right - sled->rcVis.left;
}

static void recalcSize (HWND hWnd, PSLEDITDATA sled, BOOL bInited)
{
    get_content_width (hWnd, sled);

    sled->nContX = 0;
    sled->nContW = sled->rcVis.right - sled->rcVis.left;

    set_line_width (hWnd, sled);

    sled->starty  = sled->topMargin + ( sled->rcVis.bottom - 
                    sled->rcVis.top - GetWindowFont (hWnd)->size - 1 ) / 2;

    sledit_refresh_caret (hWnd, sled, bInited);
}

static int sledit_init (HWND hWnd, PSLEDITDATA sled)
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

    sled->passwdChar     = PASSWORD_REPLACEMENT;

    sled->changed        = FALSE;
    sled->bShowCaret     = FALSE;

    sled->nBlockSize = DEF_LINE_BLOCK_SIZE;
    sled->hardLimit      = -1;
    
    if (pCtrl->dwStyle & ES_TIP) {
        sled->tiptext = FixStrAlloc (DEF_TIP_LEN + 1);
        sled->tiptext[0] = 0;
    }
    else
        sled->tiptext = NULL;

    sled->content.string = NULL;
    sled->content.buffsize = 0;
    sled->content.txtlen = 0;

    sledit_settext (sled, pCtrl->spCaption);

    if (pCtrl->dwStyle & ES_PASSWORD) {
        pCtrl->pLogFont = GetSystemFont (SYSLOGFONT_DEFAULT);
    }

    CreateCaret (hWnd, NULL, 1, pCtrl->pLogFont->size);
    recalcSize (hWnd, sled, TRUE);

    return 0;
}

static void sledit_destroy (HWND hWnd, PSLEDITDATA sled)
{
    DestroyCaret (hWnd);
    if ( (GetWindowStyle(hWnd) & ES_TIP) && sled->tiptext)
        FreeFixStr (sled->tiptext);
    testr_free (&sled->content);
}

static inline BOOL sleContentToVisible (PSLEDITDATA sled, int *x)
{
    if (x)
        *x -= sled->nContX;
    else
        return -1;

    if (*x < 0 || *x > sled->nVisW)
        return -1;
    return 0;
}

static inline BOOL sleVisibleToWindow (PSLEDITDATA sled, int *x)
{
    if (x)
        *x += sled->leftMargin;
    return TRUE;
}

static inline void sleContentToWindow (PSLEDITDATA sled, int *x)
{
    sleContentToVisible (sled, x);
    sleVisibleToWindow (sled, x);
}

static inline void sleWindowToContent (PSLEDITDATA sled, int *x)
{
    *x -= sled->leftMargin;
    *x += sled->nContX;
}

/* sets the current caret position in the virtual content window */
static void mySetCaretPos (HWND hWnd, PSLEDITDATA sled, int x)
{
    if (sleContentToVisible(sled, &x) < 0) {
        HideCaret (hWnd);
    } else {
        sleVisibleToWindow (sled, &x);
        SetCaretPos (hWnd, x, sled->starty);
        ActiveCaret (hWnd);
        /* fix bug 4349: CENTER style does not work when startup*/ 
        if (sled->status & EST_FOCUSED) {
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

static void slePaint (HWND hWnd, HDC hdc, PSLEDITDATA sled)
{
    char*   dispBuffer, *passwdBuffer = NULL;
    DWORD   dwStyle = GetWindowStyle(hWnd);
    StrBuffer *content = &sled->content;
    int starty = sled->starty;
    int outw = 0;
    int startx;
   
//FIXME, normal case of ES_TIP?
    if (dwStyle & ES_TIP && content->txtlen <= 0 && 
                    GetFocus(GetParent(hWnd)) != hWnd) {
        setup_dc (hWnd, sled, hdc, FALSE);
        TextOut (hdc, sled->leftMargin, starty, 
                        sled->tiptext);
        return;
    }

    if (dwStyle & ES_PASSWORD) {
        dispBuffer = FixStrAlloc (content->txtlen);
        memset (dispBuffer, sled->passwdChar, content->txtlen);
        passwdBuffer = dispBuffer;
    }
    else {
        dispBuffer = (char*)content->string;
    }

    if (dwStyle & ES_BASELINE) {
        SetPenColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_WINDOW));
#ifdef _PHONE_WINDOW_STYLE
        MoveTo (hdc, sled->leftMargin, sled->rcVis.bottom);
        LineTo (hdc, sled->rcVis.right, sled->rcVis.bottom);
#else
        DrawHDotLine (hdc, 
                    sled->leftMargin, 
                    sled->rcVis.bottom,
                    sled->rcVis.right - sled->rcVis.left);
#endif
    }

    ClipRectIntersect (hdc, &sled->rcVis);

    if (sled->selStart != sled->selEnd) {//select chars
        startx = sled->startx;
        sleContentToWindow (sled, &startx);
        /* draw first normal chars */
        if (sled->selStart > 0) {
            setup_dc (hWnd, sled, hdc, FALSE);
            outw += TextOutLen (hdc, startx, starty,
                        dispBuffer, sled->selStart);
            dispBuffer += sled->selStart;
        }

        /* draw selected chars */
        setup_dc (hWnd, sled, hdc, TRUE);
        outw += TextOutLen (hdc, startx + outw, starty, dispBuffer, 
                        sled->selEnd - sled->selStart);
        dispBuffer += sled->selEnd - sled->selStart;
        
        /* draw others */
        if (sled->selEnd < content->txtlen) {
            setup_dc (hWnd, sled, hdc, FALSE);
            outw += TextOutLen (hdc, startx + outw, starty, dispBuffer, 
                    content->txtlen - sled->selEnd);
        }
    }
    else {
        setup_dc (hWnd, sled, hdc, FALSE);
        startx = sled->startx;
        sleContentToWindow (sled, &startx);
        outw += TextOutLen (hdc, startx, starty, dispBuffer, content->txtlen);
    }

    if (dwStyle & ES_PASSWORD)
        FreeFixStr (passwdBuffer);
}

static int sleSetSel (HWND hWnd, PSLEDITDATA sled, int sel_start, int sel_end)
{
    if (sled->content.txtlen <= 0)
        return -1;

    if (sel_start < 0)
        sel_start = 0;
    if (sel_end < 0)
        sel_end = sled->content.txtlen;
    if (sel_start == sel_end)
        return -1;

    sled->selStart = sel_start;
    sled->selEnd = sel_end;

    /* houhh 20101129, set cart position to sled->selEnd. */
    if (sled->selEnd >= sled->selStart) {
        int cx = 0;
        make_charpos_visible (hWnd, sled, sled->selEnd, &cx);
        mySetCaretPos (hWnd, sled, cx);
    }

    /* 
     * for bug 4894, when select text don't hide caret, because this will close
     * the ime window. by humingming 2010.8.17
     */
    //HideCaret(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);

    return sled->selEnd - sled->selStart;
}

/* 
 * set caret position according to the new desired x coordinate.
 */
static void set_caret_pos (HWND hWnd, PSLEDITDATA sled, int x, BOOL bSel)
{
    int out_chars;
    HDC hdc;
    SIZE txtsize;
    hdc = GetClientDC (hWnd);

    sleWindowToContent (sled, &x);
    if (x - sled->startx <= 0) {
        out_chars = 0;
        txtsize.cx = 0;
    }
    else {
        /* DK [10/05/07] for fix bug4542: Replace content of edit to a same length 
         * string which is made up of '*' character when the edit control include 
         * ES_PASSWORD style, for take the right content width. */
       if ((GetWindowStyle(hWnd) & ES_PASSWORD)) {
            int sled_width = x - sled->startx;
            int max_char_len = 0;
            GetTextExtent(hdc, "*", 1, &txtsize);
            max_char_len = sled_width / txtsize.cx;
            out_chars = sled->content.txtlen > max_char_len ? 
                max_char_len : sled->content.txtlen;
            txtsize.cx *= out_chars;
        }
        else {
            const char* sled_string = (const char*)sled->content.string;
            out_chars = GetTextExtentPoint (hdc, sled_string,
                    sled->content.txtlen, 
                    x - sled->startx,
                    NULL, NULL, NULL, &txtsize);
        }
    }
    if (!bSel) {
        sled->editPos = out_chars;
        sled->selStart = sled->selEnd = sled->editPos;
        //mySetCaretPos (hWnd, sled, txtsize.cx + sled->startx);
    }
    else {
        if (out_chars > sled->editPos) {
            sled->selStart = sled->editPos;
            sled->selEnd = out_chars;
        }
        else {
            sled->selEnd = sled->editPos;
            sled->selStart = out_chars;
        }
    }

    /* move caret with select end. */
    if (sled->editPos == sled->selStart)
        make_charpos_visible (hWnd, sled, sled->selEnd, NULL);
    else
        make_charpos_visible (hWnd, sled, sled->selStart, NULL);
    mySetCaretPos (hWnd, sled, txtsize.cx + sled->startx);

    ReleaseDC (hdc);
}

static BOOL make_pos_visible (HWND hWnd, PSLEDITDATA sled, int x)
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
make_charpos_visible (HWND hWnd, PSLEDITDATA sled, int charPos, int *cx)
{
    SIZE txtsize;
    HDC hdc;

    if (charPos <= 0)
        txtsize.cx = 0;
    else {
        char* string = (char*)sled->content.string;
        int dwStyle  = GetWindowStyle(hWnd);
        hdc = GetClientDC (hWnd);
        if (dwStyle & ES_PASSWORD) {
            string = FixStrAlloc (sled->content.txtlen);
            memset (string, sled->passwdChar, sled->content.txtlen);
        }
        GetTextExtent (hdc, (const char*)string, 
                charPos, &txtsize);
        if (dwStyle & ES_PASSWORD)
            FreeFixStr (string);
        ReleaseDC (hdc);
    }
    if (cx)
        *cx = sled->startx + txtsize.cx;
    return make_pos_visible (hWnd, sled, sled->startx + txtsize.cx);
}

static void 
calc_content_width(HWND hWnd, PSLEDITDATA sled, int charPos, int *cx)
{
    SIZE txtsize;
    HDC hdc;

    if (charPos <= 0)
        txtsize.cx = 0;
    else {
        hdc = GetClientDC (hWnd);
        GetTextExtent (hdc, (const char*)sled->content.string, 
                charPos, &txtsize);
        ReleaseDC (hdc);
    }
    if (cx)
        *cx = txtsize.cx;
        
}

static void 
calc_charpos_cx (HWND hWnd, PSLEDITDATA sled, int charPos, int *cx)
{
    SIZE txtsize;
    HDC hdc;

    if (charPos <= 0)
        txtsize.cx = 0;
    else {
        hdc = GetClientDC (hWnd);
        GetTextExtent (hdc, (const char*)sled->content.string, 
                charPos, &txtsize);
        ReleaseDC (hdc);
    }
    if (cx)
        *cx = sled->startx + txtsize.cx;
        
}

/*
 * set caret position according to the current edit position.
 */
static BOOL edtSetCaretPos (HWND hWnd, PSLEDITDATA sled)
{
    BOOL bRefresh;
    int cx;

    bRefresh = make_charpos_visible (hWnd, sled, sled->editPos, &cx);

    mySetCaretPos (hWnd, sled, cx);
    if (bRefresh) {
        InvalidateRect (hWnd, NULL, TRUE);
    }
    return bRefresh;
}
static void set_edit_caret_pos(HWND hWnd, PSLEDITDATA sled)
{
    int cx;

    make_charpos_visible (hWnd, sled, sled->editPos, &cx);
    mySetCaretPos (hWnd, sled, cx);
}

static int sleMouseMove (HWND hWnd, PSLEDITDATA sled, LPARAM lParam)
{
    int selStart, selEnd;
    int mouseX, mouseY;
    RECT rcClient;

    mouseX = LOSWORD(lParam);
    mouseY = HISWORD(lParam);

    ScreenToClient(hWnd, &mouseX, &mouseY);
    GetClientRect(hWnd, &rcClient);
    
    selStart = sled->selStart;
    selEnd = sled->selEnd;
    if (mouseX < 0) {
        mouseX = 0;
        if (sled->selStart > 0) {
            shift_one_char_l (selStart);
            make_charpos_visible (hWnd, sled, sled->selStart, NULL);
        }
        else {
            return -1;
        }
    }
    if (mouseX > RECTW(rcClient)) {
        mouseX = RECTW(rcClient) - 1;
        if (sled->selEnd < sled->content.txtlen) {
            shift_one_char_r (selEnd);
            make_charpos_visible (hWnd, sled, sled->selEnd, NULL);
        }
        else {
            return -1;
        }
    }
    set_caret_pos (hWnd, sled, mouseX, TRUE);

    check_caret();
    if (selStart != sled->selStart || selEnd != sled->selEnd)
        InvalidateRect(hWnd, NULL, TRUE);

    return 0;
}

static void set_line_width (HWND hWnd, PSLEDITDATA sled)
{
    SIZE txtsize;
    HDC hdc;
    DWORD dwStyle = GetWindowStyle(hWnd);
    int old_w = sled->nContW;

    hdc = GetClientDC (hWnd);
    SelectFont (hdc, GetWindowFont(hWnd));
    GetTextExtent(hdc, (const char*)(sled->content.string), 
            sled->content.txtlen, &txtsize);
    ReleaseDC (hdc);

    if (txtsize.cx > sled->nVisW)
        sled->nContW = txtsize.cx;
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
        sled->startx = sled->nVisW - txtsize.cx;
    }
    else if (dwStyle & ES_CENTER) {
        sled->startx = (sled->nVisW - txtsize.cx) >> 1;
    }
    else {
        sled->startx = 0;
    }
}

static inline void edtChangeCont (HWND hWnd, PSLEDITDATA sled)
{
    set_line_width (hWnd, sled);
}

/* check input validity for password style */
static void check_valid_passwd (char *newtext)
{
    char *ptmp = newtext;

    while (*ptmp) {
        if (!isprint(*ptmp))
            *ptmp = PASSWORD_REPLACEMENT;
        ptmp ++;
    }
}

static int
sleInsertText (HWND hWnd, PSLEDITDATA sled, char *newtext, int inserting)
{
    int  deleted;
    unsigned char *pIns, *content = sled->content.string;

    if ( (GetWindowStyle(hWnd) & ES_PASSWORD) && newtext && inserting > 0 ) {
        check_valid_passwd (newtext);
    }

    //delete the seleted
    if(sled->selStart != sled->selEnd) {
        deleted = sled->selEnd - sled->selStart;
        memmove (content + sled->selStart, content + sled->selEnd,
                        sled->content.txtlen - sled->selEnd);
        sled->content.txtlen -= deleted;

        sled->editPos = sled->selStart;
        sled->selEnd = sled->selStart;
        ShowCaret(hWnd);

        if (!newtext)
            inserting = 0;
    }

    if ( !(sled->status & EST_REPLACE) && inserting > 0 
            && sled->hardLimit >= 0 
            && ((sled->content.txtlen + inserting) > sled->hardLimit)) {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = sled->hardLimit - sled->content.txtlen;
        return 0;
    }
    else if ( (sled->status & EST_REPLACE) && inserting > 0 
                && sled->hardLimit >= 0
                && (sled->editPos + inserting > sled->hardLimit) ) {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = sled->hardLimit - sled->editPos;
    }

    if (inserting > 0) {
        content = testr_realloc (&sled->content, sled->content.txtlen + 
                                  inserting);
        pIns = content + sled->editPos;
        if ( !(sled->status & EST_REPLACE) )
            memmove (pIns + inserting, pIns, 
                    sled->content.txtlen+1 - sled->editPos);
        memcpy (pIns, newtext, inserting);
    }
    else if (inserting < 0) {
        pIns = content + sled->editPos;
        memmove (pIns + inserting, pIns, 
                sled->content.txtlen+1 - sled->editPos);
        content = testr_realloc (&sled->content, sled->content.txtlen + 
                                 inserting);
    }

    if (inserting <= 0 || !(sled->status & EST_REPLACE))
        sled->content.txtlen += inserting;
    else {
        int add_len = inserting - sled->content.txtlen + sled->editPos;
        if (add_len > 0) {
            sled->content.txtlen += add_len;
            sled->content.string[sled->content.txtlen] = '\0';
        }
    }
    sled->editPos += inserting;
    sled->selStart = sled->selEnd = sled->editPos;

//FIXME
    edtChangeCont (hWnd, sled);
    InvalidateRect (hWnd, NULL, TRUE);

    if (!edtSetCaretPos (hWnd, sled)) {
        InvalidateRect (hWnd, NULL, TRUE);
    }
    sled->changed = TRUE;
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);

    return 0;
}

static int
sleInsertText_refresh (HWND hWnd, 
        PSLEDITDATA sled, char *newtext, int inserting)
{
    int  deleted;
    unsigned char *pIns, *content = sled->content.string;

    if ( (GetWindowStyle(hWnd) & ES_PASSWORD) && newtext && inserting > 0 ) {
        check_valid_passwd (newtext);
    }

    //delete the seleted
    if(sled->selStart != sled->selEnd) {
        deleted = sled->selEnd - sled->selStart;
        memmove (content + sled->selStart, content + sled->selEnd,
                        sled->content.txtlen - sled->selEnd);
        sled->content.txtlen -= deleted;

        sled->editPos = sled->selStart;
        sled->selEnd = sled->selStart;
        ShowCaret(hWnd);
        if (!newtext)
            inserting = 0;
    }

    if ( !(sled->status & EST_REPLACE) && inserting > 0 && sled->hardLimit >= 0 
                && ((sled->content.txtlen + inserting) > sled->hardLimit)) {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = sled->hardLimit - sled->content.txtlen;
        //return 0;
    }
    else if ( (sled->status & EST_REPLACE) && inserting > 0 
            && sled->hardLimit >= 0
            && (sled->editPos + inserting > sled->hardLimit) ) {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = sled->hardLimit - sled->editPos;
    }

    if (inserting > 0) {
        content = testr_realloc (&sled->content, sled->content.txtlen + 
                                  inserting);
        pIns = content + sled->editPos;
        if ( !(sled->status & EST_REPLACE) )
            memmove (pIns + inserting, pIns, 
                    sled->content.txtlen+1 - sled->editPos);
        memcpy (pIns, newtext, inserting);
    }
    else if (inserting < 0) {
        pIns = content + sled->editPos;
        memmove (pIns + inserting, pIns, 
                sled->content.txtlen+1 - sled->editPos);
        content = testr_realloc (&sled->content, sled->content.txtlen + 
                                 inserting);
    }

    if (inserting <= 0 || !(sled->status & EST_REPLACE))
        sled->content.txtlen += inserting;
    else {
        int add_len = inserting - sled->content.txtlen + sled->editPos;
        if (add_len > 0) {
            sled->content.txtlen += add_len;
            sled->content.string[sled->content.txtlen] = '\0';
        }
    }
    sled->editPos += inserting;
    sled->selStart = sled->selEnd = sled->editPos;

//FIXME
    edtChangeCont (hWnd, sled);

    return 0;
}

static int sleCaseText (char* txt, BOOL bupper)
{
    char *str;
    char (*tocase)(char);

    if (NULL == txt)
        return -1;

    if (bupper)
        tocase = (char(*)(char))toupper;
    else
        tocase = (char(*)(char))tolower;

    str = txt;
    while (*str != '\0') {
        str[0] = tocase(str[0]);
        str ++;
    }

    return 0;
}

#ifdef _MGHAVE_CLIPBOARD
static int sleInsertCbText (HWND hWnd, PSLEDITDATA sled)
{
    int  inserting;
    unsigned char *txtBuffer;

    if (GetWindowStyle(hWnd) & ES_READONLY) {
        return 0;
    }

    inserting = GetClipBoardDataLen (CBNAME_TEXT);
    txtBuffer = ALLOCATE_LOCAL (inserting);
    GetClipBoardData (CBNAME_TEXT, txtBuffer, inserting);
    txtBuffer[inserting]=0;

    if (GetWindowStyle(hWnd) & ES_UPPERCASE) 
        sleCaseText((char *)txtBuffer, TRUE);
    else if (GetWindowStyle(hWnd) & ES_LOWERCASE) 
        sleCaseText((char *)txtBuffer, FALSE);

    sleInsertText (hWnd, sled, (char *)txtBuffer, inserting);
    DEALLOCATE_LOCAL(txtBuffer);

    return 0;
}
#endif

static void esright_backspace_refresh(HWND hWnd, PSLEDITDATA sled, int del)
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

            calc_charpos_cx(hWnd, sled, sled->selStart, &sel_start_x);
            calc_charpos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
            if (sled->selStart < sled->selEnd){
                scroll_rc.right = sel_end_x + 1;
            }
            else{
                scroll_rc.right = sel_start_x + 1;
            }

            scroll_rc.left = sled->rcVis.left;
            sleInsertText_refresh (hWnd, sled, NULL, del);

            scroll_len = abs(sel_start_x - sel_end_x);
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
            edtSetCaretPos (hWnd, sled);
        }
        else{
            calc_charpos_cx(hWnd, sled, sled->editPos, &old_editpos_x);
            scroll_rc.left = sled->rcVis.left;
            calc_charpos_cx(hWnd, sled, sled->editPos + del, &cur_editpos_x);
            
            sleInsertText_refresh (hWnd, sled, NULL, del);

            edtSetCaretPos (hWnd, sled);
            scroll_rc.right = old_editpos_x + 1;

            scroll_len = abs(cur_editpos_x - old_editpos_x);
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

        }

    }
    else{

        if (sled->nContX <= 0){//left scroll window while the head of text is in the sled->rcVis 
            if (sled->selStart != sled->selEnd){

                refresh_rc.top = sled->rcVis.top;
                refresh_rc.bottom = sled->rcVis.bottom;
                refresh_rc.right = sled->rcVis.right;

                scroll_rc.right = sled->rcVis.right;

                calc_charpos_cx(hWnd, sled, sled->selStart, &sel_start_x);
                calc_charpos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
                if (sled->selStart < sled->selEnd){
                    scroll_rc.left = sel_start_x + 1;
                }
                else{
                    scroll_rc.left = sel_end_x + 1;
                }

                sleInsertText_refresh (hWnd, sled, NULL, del);
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

                sleInsertText_refresh (hWnd, sled, NULL, del);
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

                calc_charpos_cx(hWnd, sled, sled->selStart, &sel_start_x);
                calc_charpos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
                if (sled->selStart < sled->selEnd){
                    scroll_rc.right = sel_end_x + 1 - sled->nContX;
                }
                else{
                    scroll_rc.right = sel_start_x + 1 - sled->nContX;
                }

                scroll_rc.left = sled->rcVis.left;
                sleInsertText_refresh (hWnd, sled, NULL, del);

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
                sleInsertText_refresh (hWnd, sled, NULL, del);
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

static void esright_del_refresh(HWND hWnd, PSLEDITDATA sled, int del)
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

            calc_charpos_cx(hWnd, sled, sled->selStart, &sel_start_x);
            calc_charpos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
            if (sled->selStart < sled->selEnd){
                scroll_rc.right = sel_end_x + 1;
            }
            else{
                scroll_rc.right = sel_start_x + 1;
            }

            scroll_rc.left = sled->rcVis.left;
            sleInsertText_refresh (hWnd, sled, NULL, del);

            scroll_len = abs(sel_start_x - sel_end_x);
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);
            edtSetCaretPos (hWnd, sled);
        }
        else{
            old_caret_x = get_caretpos_x(hWnd);
            scroll_rc.left = sled->rcVis.left;
            sleInsertText_refresh (hWnd, sled, NULL, del);
            edtSetCaretPos (hWnd, sled);
            cur_caret_x = get_caretpos_x(hWnd);    
            scroll_rc.right = cur_caret_x;

            scroll_len = cur_caret_x - old_caret_x;
            ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

        }

    }
    else{
        if (sled->nContX <=0){//left scroll window while the head of text is in the sled->rcVis 
            if (sled->selStart != sled->selEnd){

                refresh_rc.top = sled->rcVis.top;
                refresh_rc.bottom = sled->rcVis.bottom;
                refresh_rc.right = sled->rcVis.right;

                scroll_rc.right = sled->rcVis.right;

                calc_charpos_cx(hWnd, sled, sled->selStart, &sel_start_x);
                calc_charpos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
                if (sled->selStart < sled->selEnd){
                    scroll_rc.left = sel_start_x + 1;
                }
                else{
                    scroll_rc.left = sel_end_x + 1;
                }

                sleInsertText_refresh (hWnd, sled, NULL, del);
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
                calc_charpos_cx(hWnd, sled, sled->editPos, &old_editpos_x);

                sleInsertText_refresh (hWnd, sled, NULL, del);
                calc_charpos_cx(hWnd, sled, sled->editPos, &cur_editpos_x);
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

                calc_charpos_cx(hWnd, sled, sled->selStart, &sel_start_x);
                calc_charpos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
                if (sled->selStart < sled->selEnd){
                    scroll_rc.right = sel_end_x + 1 - sled->nContX;
                }
                else{
                    scroll_rc.right = sel_start_x + 1 - sled->nContX;
                }

                scroll_rc.left = sled->rcVis.left;
                sleInsertText_refresh (hWnd, sled, NULL, del);

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
                sleInsertText_refresh (hWnd, sled, NULL, del);
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

static void esleft_del_refresh(HWND hWnd, PSLEDITDATA sled, int del)
{
    int old_nContX = sled->nContX;
    RECT scroll_rc, del_rc, refresh_rc;
    int txtlen_x;    
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
              calc_charpos_cx(hWnd, sled, sled->selEnd, &sel_end_x);
              scroll_rc.left = sel_end_x;
         }
         else{
             calc_charpos_cx(hWnd, sled, sled->selStart, &sel_start_x);
             scroll_rc.left = sel_start_x;
         }

         scroll_rc.right = sled->rcVis.right;
        }
     else{
         calc_charpos_cx(hWnd, sled, sled->editPos, &scroll_rc.left);
         scroll_rc.left = scroll_rc.left - old_nContX;
         scroll_rc.right = sled->rcVis.right;
     }

     old_caret_x = get_caretpos_x(hWnd);
     calc_charpos_cx(hWnd, sled, sled->content.txtlen, &txtlen_x);

     sleInsertText_refresh (hWnd, sled, NULL, del);

     calc_charpos_cx(hWnd, sled, sled->editPos, &del_rc.left);
     del_rc.left = del_rc.left - old_nContX;
     del_rc.right = scroll_rc.left; 

     scroll_len = del_rc.left - scroll_rc.left;

     if (abs(sel_start_x - sel_end_x) >= sled->nVisW){
         InvalidateRect(hWnd, NULL, TRUE);
     }
     else{
         if ((old_nContX > 0) && ((txtlen_x - old_nContX) <= sled->rcVis.right)){
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
             calc_charpos_cx(hWnd, sled, sled->content.txtlen, &rl);

             if (((sled->nContW - sled->nContX) == sled->nVisW) && (old_nContX > 0)){
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
    PSLEDITDATA sled = (PSLEDITDATA) (pCtrl->dwAddData2);

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
            //InvalidateRect (hWnd, NULL, TRUE);
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
            //InvalidateRect (hWnd, NULL, TRUE);
            InvalidateRect (hWnd, &sled->rcVis, TRUE);


        return 0;
    }
   
    case SCANCODE_END:
    {
        BOOL refresh = FALSE;
       
        if(lParam & KS_SHIFT) {
            sled->selStart = sled->editPos;
            sled->selEnd = sled->content.txtlen;
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

        sled->editPos = sled->content.txtlen;
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
                    shift_one_char_l(selStart);
            }
            else {
                shift_one_char_l(selEnd);
            }

            check_caret ();
            make_charpos_visible (hWnd, sled, sled->selStart, NULL);
            InvalidateRect (hWnd, NULL, TRUE);

            NotifyParent (hWnd, pCtrl->id, EN_SELCHANGED);
            return 0;
        }

        if (sled->selStart != sled->selEnd) {
            if (sled->editPos != sled->selEnd)
                sled->editPos = sled->selEnd;
            else
                sled->editPos = sled->selStart;
        }

        if (sled->editPos > 0) {
            shift_one_char_l (editPos);
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
                if (sled->selEnd < sled->content.txtlen)
                    shift_one_char_r (selEnd);
            }
            else {
                shift_one_char_r (selStart);
            }

            check_caret ();
            make_charpos_visible (hWnd, sled, sled->selEnd, NULL);
            InvalidateRect (hWnd, NULL, TRUE);
            NotifyParent (hWnd, pCtrl->id, EN_SELCHANGED);
            return 0;
        }

        if (sled->selStart != sled->selEnd) {
            if (sled->editPos != sled->selEnd)
                sled->editPos = sled->selEnd;
            else
                sled->editPos = sled->selStart;
        }

        if (sled->editPos < sled->content.txtlen) {
            shift_one_char_r (editPos);
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
        if (sled->editPos == sled->content.txtlen 
                && sled->selStart == sled->selEnd)
            return 0;

        if (sled->selStart == sled->selEnd && 
                        sled->editPos < sled->content.txtlen) {
            shift_one_char_r(editPos);
        }

        del = oldpos - sled->editPos;
        if (dwStyle & ES_LEFT) {
            esleft_del_refresh(hWnd, sled, del);

            if (!edtSetCaretPos (hWnd, sled)) {
                InvalidateRect (hWnd, NULL, TRUE);
            }

            sled->changed = TRUE;
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
        } 
        else if (dwStyle & ES_RIGHT) {
            esright_del_refresh(hWnd, sled, del);
            //sleInsertText (hWnd, sled, NULL, del);
        }
        else if (dwStyle & ES_CENTER) {
            sleInsertText (hWnd, sled, NULL, del);
        }
        else {
            sleInsertText (hWnd, sled, NULL, del);
        }

    }
    break;


    case SCANCODE_BACKSPACE:
    {
        int del;

        if (dwStyle & ES_READONLY)
            return 0;
        if (sled->editPos == 0 && sled->selStart == sled->selEnd)
            return 0;

        del = - CHLENPREV( sled->content.string, 
                (sled->content.string + sled->editPos) );
        sleInsertText (hWnd, sled, NULL, del);
    }
    break;

    case SCANCODE_A:
    {
        if (lParam & KS_CTRL) {
            sleSetSel (hWnd, sled, 0, sled->content.txtlen);
        }
        return 0;
    }

#ifdef _MGHAVE_CLIPBOARD
    case SCANCODE_C:
    case SCANCODE_X:
    {
        if ((lParam & KS_CTRL) && (sled->selEnd - sled->selStart > 0)) {
            SetClipBoardData (CBNAME_TEXT, sled->content.string + sled->selStart, 
                        sled->selEnd - sled->selStart, CBOP_NORMAL);
            if (wParam == SCANCODE_X && !(GetWindowStyle(hWnd) & ES_READONLY)) {
                sleInsertText (hWnd, sled, NULL, 0);
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
        PSLEDITDATA sled, char *charBuffer, int chars)
{
    RECT scroll_rc;
    int scroll_len;
    int old_ncontw, cur_ncontw;
    int cur_caret_x;

    if (sled->selStart != sled->selEnd) {
        sleInsertText (hWnd, sled, (char* )charBuffer, chars);
    }
    else {//left scroll window
        scroll_rc.top = sled->rcVis.top;
        scroll_rc.bottom = sled->rcVis.bottom;
        calc_content_width(hWnd, sled, sled->content.txtlen, &old_ncontw);
        sleInsertText_refresh (hWnd, sled, (char* )charBuffer, chars);

        calc_content_width(hWnd, sled, sled->content.txtlen, &cur_ncontw);
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
        PSLEDITDATA sled, char *charBuffer, int chars)
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
    int txtlen_x;

    old_sel_start = sled->selStart;
    old_sel_end = sled->selEnd; 

    old_caretpos_x = get_caretpos_x(hWnd);
    calc_charpos_cx(hWnd, sled, sled->editPos, &old_edit_pos_x);
    calc_charpos_cx(hWnd, sled, sled->content.txtlen, &txtlen_x);
    old_nContX= sled->nContX;
    calc_charpos_cx(hWnd, sled, old_sel_start, &old_sel_start_x);
    calc_charpos_cx(hWnd, sled, old_sel_end, &old_sel_end_x);
    old_sel_start_x = old_sel_start_x - sled->nContX;
    old_sel_end_x = old_sel_end_x - sled->nContX;

    sleInsertText_refresh (hWnd, sled, (char* )charBuffer, chars);

    calc_charpos_cx(hWnd, sled, sled->editPos, &cur_edit_pos_x);
    calc_charpos_cx(hWnd, sled, old_sel_start, &cur_sel_start_x);

    /* Not used var
    int cur_caretpos_x, cur_ncontx;
    cur_ncontx = sled->nContX;
    cur_caretpos_x = get_caretpos_x(hWnd);
    */

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
                    if ((old_nContX > 0) 
                            && ((txtlen_x - old_nContX) <= sled->rcVis.right)) {
                        scroll_len = sel_size - char_size;  
                        scroll_rc.left = sled->rcVis.left;		 
                        scroll_rc.right = old_sel_start_x < old_sel_end_x
                                ? old_sel_start_x : old_sel_end_x;

                        ScrollWindow(hWnd, scroll_len, 0, &scroll_rc, NULL);

                        refresh_rc.left = scroll_rc.right;
                        refresh_rc.right = refresh_rc.left + sel_size + scroll_len;

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
            calc_charpos_cx(hWnd, sled, sled->content.txtlen, &right);
            if (old_caretpos_x < sled->rcVis.right){//right scroll window 
                scroll_len = cur_edit_pos_x - old_edit_pos_x;     
                if ((sled->rcVis.right - old_caretpos_x) < scroll_len){//
                    scroll_len = -(scroll_len - (sled->rcVis.right- old_caretpos_x)); 
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

static void sledit_refresh_caret (HWND hWnd, PSLEDITDATA sled, BOOL bInited)
{
    DWORD   dwStyle = GetWindowStyle(hWnd);
    SIZE    txtsize;
    int     outchar;
    HDC     hdc;

    if (!sled)
        return;

    hdc= GetClientDC (hWnd);
    outchar = GetTextExtentPoint (hdc, (const char*)sled->content.string, 
                    sled->content.txtlen, 
                    0,
                    NULL, NULL, NULL, &txtsize);
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

static int sledit_reset_text (HWND hWnd, PSLEDITDATA sled, char* str)
{
    DWORD   dwStyle = GetWindowStyle(hWnd);
    char*   buffer  = str;

    if (!sled || !buffer)
        return -1;

    if (dwStyle & ES_PASSWORD) {
        buffer = strdup (buffer);
        check_valid_passwd (buffer);
    }

	//we must retain the status of this control, it include is ETS_FOCUS state, that 
	//tell this control whether the caret is need
    //sled->status = 0;
    sled->selStart = 0;
    sled->selEnd = 0;

    if (sled->content.string) {
        free (sled->content.string);
        sled->content.string = NULL;
        sled->content.buffsize = 0;
        sled->content.txtlen = 0;
    }

    sledit_settext (sled, buffer);
    recalcSize (hWnd, sled, TRUE);

    if (dwStyle & ES_PASSWORD) { free (buffer);
    }

    //sledit_refresh_caret (hWnd, sled, TRUE);

    sled->changed = TRUE;
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_UPDATE);
    return 0;
}

static int 
sledit_insert_text (HWND hWnd, PSLEDITDATA sled, const char* newtext, int len)
{
    int     i, realen = strlen (newtext);
    char*   tempstr;
    char*   newbuffer;
    DWORD   dwStyle = GetWindowStyle(hWnd);

    if (len > realen)
        len = realen;
    if (!newtext || len <= 0)
        return -1;

    if (dwStyle & ES_READONLY)
        return 0;

	tempstr = (char*) malloc(realen+1);

    for (i = 0; i < len && newtext[i] != '\0'; i ++)
    {
        if (dwStyle & ES_UPPERCASE) {
            tempstr[i] = toupper (newtext[i]); 
        }
        else if (dwStyle & ES_LOWERCASE) {
            tempstr[i] = tolower (newtext[i]);
        }
        else
            tempstr[i] = newtext[i];
    }
    newbuffer = tempstr;

    sleInsertText (hWnd, sled, (char* )newbuffer, len);

	free(tempstr);
    return 0;
}

static LRESULT
SLEditCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    DWORD       dwStyle;
    HDC         hdc;
    PSLEDITDATA sled = NULL;
    PMAINWIN    pWin = (PMAINWIN)hWnd;
    RECT        rcCaret;

    dwStyle     = GetWindowStyle(hWnd);

    if (message != MSG_CREATE)
        sled = (PSLEDITDATA) GetWindowAdditionalData2 (hWnd);

    switch (message) {

    case MSG_CREATE:
        if ( !(sled = calloc (1, sizeof (SLEDITDATA))) )
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

        if (sled->bShowCaret)
            ShowCaret (hWnd);
        else
            HideCaret (hWnd);
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

            //HideCaret (hWnd);
            if (pWin->pCaretInfo) {
                sled->bShowCaret = FALSE;
                SetRect (&rcCaret, pWin->pCaretInfo->x, pWin->pCaretInfo->y, 
                        pWin->pCaretInfo->x + pWin->pCaretInfo->nWidth, 
                        pWin->pCaretInfo->y + pWin->pCaretInfo->nHeight);
                InvalidateRect (hWnd, &rcCaret, FALSE);
            }

            if ( sled->selStart != sled->selEnd 
                    || (dwStyle & ES_TIP && sled->content.txtlen <= 0)) {
                refresh = TRUE;
            }
            if (sled->selStart != sled->selEnd && !(dwStyle & ES_NOHIDESEL))
            {
                sled->selStart = sled->selEnd = sled->editPos;
            }

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

        /*[humingming/2010/10/19]:bug 5158:
         * don't draw the caret here, should in MSG_PAINT. */
        ActiveCaret (hWnd);
        //ShowCaret(hWnd);
        if (pWin->pCaretInfo) {
            sled->bShowCaret = TRUE;
            SetRect (&rcCaret, pWin->pCaretInfo->x, pWin->pCaretInfo->y, 
                    pWin->pCaretInfo->x + pWin->pCaretInfo->nWidth, 
                    pWin->pCaretInfo->y + pWin->pCaretInfo->nHeight);
            InvalidateRect (hWnd, &rcCaret, FALSE);
        }

        if (dwStyle & ES_AUTOSELECT) {
            if (sleSetSel (hWnd, sled, 0, 
                        sled->content.txtlen) <= 0 && dwStyle & ES_TIP)
                InvalidateRect (hWnd, NULL, TRUE);
            if (lParam == 1)
                sled->status |= EST_TMP;
        }
        else if ( (dwStyle & ES_NOHIDESEL && sled->selStart != sled->selEnd)
                    || (dwStyle & ES_TIP && sled->content.txtlen <= 0)) {
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
        return sled->content.txtlen;
    
    case MSG_GETTEXT:
    {
        char*   buffer = (char*)lParam;
        int     len;

        len = MIN ((int)wParam, sled->content.txtlen);
        memcpy (buffer, sled->content.string, len);
        buffer [len] = '\0';
        return len;
    }

    case MSG_SETTEXT:
        return sledit_reset_text (hWnd, sled, (char*)lParam);

    case MSG_KEYDOWN:
    {
        return sleKeyDown (hWnd, wParam, lParam);
    }

    case MSG_UTF8CHAR:
        _ERR_PRINTF ("CONTROL>EDIT: MSG_UTF8CHAR is not implemented.\n");
        break;

    case MSG_CHAR:
    {
        unsigned char charBuffer [4];
        int chars;

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

        if (wParam == '\b') { //backspace
            int del;

            if (sled->editPos == 0 && sled->selStart == sled->selEnd)
                 return 0;
            del = -CHLENPREV((const char *)(sled->content.string), 
                    (sled->content.string + sled->editPos) );

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
                sleInsertText (hWnd, sled, NULL, del);
            }
            else {
                sleInsertText (hWnd, sled, NULL, del);
            }
        }
        else{
            if (dwStyle & ES_LEFT) {
	            esleft_input_char_refresh(hWnd, sled, (char*)charBuffer, chars);
                set_edit_caret_pos(hWnd, sled);

                sled->changed = TRUE;
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
            }
            else if (dwStyle & ES_RIGHT) {
	            esright_input_char_refresh(hWnd, sled, (char*)charBuffer, chars);
            }
            else if (dwStyle & ES_CENTER) {
                sleInsertText (hWnd, sled, (char* )charBuffer, chars);

            }
            else {
                sleInsertText (hWnd, sled, (char* )charBuffer, chars);
            }
        }
        return 0;
    }

    case MSG_LBUTTONDBLCLK:
        sleSetSel (hWnd, sled, 0, sled->content.txtlen);
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
            return IME_WINDOW_TYPE_EDITABLE;
    
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
                         (const char* )(sled->content.string), pos, NULL);

        if (line_pos) *line_pos = 0;
        if (char_pos) *char_pos = nr_chars;

        return pos;
    }

    case EM_SETCARETPOS:
    case EM_SETSEL:
    {
        int char_pos = (int)lParam;
        int nr_chars, pos;
        int *pos_chars;

        if (char_pos < 0)
            return -1;

        pos_chars = ALLOCATE_LOCAL (sled->content.txtlen * sizeof(int));
        nr_chars = GetTextMCharInfo (GetWindowFont (hWnd), 
            (const char* )sled->content.string, sled->content.txtlen, pos_chars);
        if (char_pos > nr_chars) {
            DEALLOCATE_LOCAL (pos_chars);
            return -1;
        }

        if (char_pos == nr_chars)
            pos = sled->content.txtlen;
        else
            pos = pos_chars[char_pos];

        DEALLOCATE_LOCAL (pos_chars);

        if (message == EM_SETCARETPOS) {
            sled->editPos = pos;
            sled->selStart = sled->selEnd = 0;
            edtSetCaretPos (hWnd, sled);
            return sled->editPos;
        }
        else {
            int start, end;
            if (sled->editPos < pos) {
                start = sled->editPos;
                end = pos;
            }
            else {
                start = pos;
                end = sled->editPos;
            }
            return sleSetSel (hWnd, sled, start, end);
        }
    }

    case EM_INSERTTEXT:
    {
        return sledit_insert_text (hWnd, sled, (const char*)lParam, wParam);
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
            if (sled->content.txtlen > newLimit) {
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

        if (sled->content.txtlen <= 0)
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
            (const char*)(sled->content.string + sled->selStart), len); 

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
                    sled->content.string + sled->selStart, 
                    sled->selEnd - sled->selStart, CBOP_NORMAL);
            if (message == EM_CUTTOCB 
                    && !(GetWindowStyle(hWnd) & ES_READONLY)) {
                sleInsertText (hWnd, sled, NULL, 0);
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

#endif /* _MGCTRL_SLEDIT */
