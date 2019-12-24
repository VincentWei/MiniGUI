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
** achar-uchar.c: API implementation for conversion between
**  abstract characters and Unicode characters.
**
** Create date: 2019/03/05
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"

Achar32 GUIAPI GetACharValue (LOGFONT* logfont, const char* mchar,
        int mchar_len, const char* pre_mchar, int pre_len)
{
    int len_cur_char;
    Achar32  chv = INV_ACHAR_VALUE;
    DEVFONT* sbc_devfont = logfont->devfonts[0];
    DEVFONT* mbc_devfont = logfont->devfonts[1];

    if (mbc_devfont) {
        len_cur_char = mbc_devfont->charset_ops->len_first_char
            ((const unsigned char*)mchar, mchar_len);

        if (len_cur_char > 0) {
            chv = mbc_devfont->charset_ops->get_char_value
                ((Uint8*)pre_mchar, pre_len, (Uint8*)mchar, mchar_len);

            return SET_MBCHV(chv);
        }
    }

    len_cur_char = sbc_devfont->charset_ops->len_first_char
        ((const unsigned char*)mchar, mchar_len);

    if (len_cur_char > 0) {
        chv = sbc_devfont->charset_ops->get_char_value
            (NULL, 0, (Uint8*)mchar, mchar_len);
    }

    return chv;
}

Achar32 GUIAPI GetShapedAChar (LOGFONT* logfont, const char* mchar,
        int mchar_len, ACHARSHAPETYPE shape_type)
{
    int len_cur_char;

    Achar32  chv = INV_ACHAR_VALUE;
    DEVFONT* sbc_devfont = logfont->devfonts[0];
    DEVFONT* mbc_devfont = logfont->devfonts[1];

    if (mbc_devfont) {
        len_cur_char = mbc_devfont->charset_ops->len_first_char (
                (const unsigned char*)mchar, mchar_len);

        if (len_cur_char > 0) {
            if (mbc_devfont->charset_ops->get_shaped_char_value)
                chv = mbc_devfont->charset_ops->get_shaped_char_value(
                        (const unsigned char*)mchar, mchar_len, shape_type);
            else
                chv = mbc_devfont->charset_ops->get_char_value
                (NULL, 0, (Uint8*)mchar, len_cur_char);

            return SET_MBCHV(chv);
        }
    }

    len_cur_char = sbc_devfont->charset_ops->len_first_char
        ((const unsigned char*)mchar, mchar_len);

    if (len_cur_char > 0) {
        if (sbc_devfont->charset_ops->get_shaped_char_value)
            chv = sbc_devfont->charset_ops->get_shaped_char_value(
                    (const unsigned char*)mchar, mchar_len, shape_type);
        else
            chv = sbc_devfont->charset_ops->get_char_value
                (NULL, 0, (Uint8*)mchar, len_cur_char);

    }

    return chv;
}

BOOL GUIAPI GetMirrorAChar (LOGFONT* logfont, Achar32 chv, Achar32* mirrored)
{
    DEVFONT* devfont = SELECT_DEVFONT_BY_ACHAR(logfont, chv);

    *mirrored = INV_ACHAR_VALUE;
    if (devfont->charset_ops->bidi_mirror_char) {
        return devfont->charset_ops->bidi_mirror_char(REAL_ACHAR(chv),
                mirrored);
    }

    return FALSE;
}

Uint32 GUIAPI GetACharType (LOGFONT* logfont, Achar32 chv)
{
    Uint32 t = ACHARTYPE_UNKNOWN;
    DEVFONT* devfont = SELECT_DEVFONT_BY_ACHAR(logfont, chv);

    if (devfont) {
        t = devfont->charset_ops->char_type (REAL_ACHAR (chv));

#ifdef _MGCHARSET_UNICODE
        // get general category and break class from Unicode
        if (devfont->charset_ops->conv_to_uc32) {
            Uchar32 uc;
            Uint32 gc, bt;

            uc = devfont->charset_ops->conv_to_uc32(chv);
            gc = UCharGetCategory(uc);
            bt = UCharGetBreakType(uc);
            t = t | (gc << 16) | (bt << 24);
        }
#endif /* _MGCHARSET_UNICODE */
    }

    return t;
}

Uint16 GUIAPI GetACharBidiType (LOGFONT* log_font, Achar32 chv)
{
    Uint16 t = BIDI_TYPE_INVALID;
    DEVFONT* devfont = SELECT_DEVFONT_BY_ACHAR(log_font, chv);

    if (devfont) {
        if (devfont->charset_ops->bidi_char_type)
            t = devfont->charset_ops->bidi_char_type(REAL_ACHAR(chv));
        else {
#ifdef _MGCHARSET_UNICODE
            // get Bidi type from Unicode
            if (devfont->charset_ops->conv_to_uc32) {
                Uchar32 uc;
                uc = devfont->charset_ops->conv_to_uc32(chv);
                t = UCharGetBidiType(uc);
            }
#else
            t = BIDI_TYPE_INVALID;
#endif
        }
    }

    return t;
}

#ifdef _MGCHARSET_UNICODE

#include "unicode-ops.h"

Uchar32 GUIAPI Achar2UChar(LOGFONT* logfont, Achar32 chv)
{
    Uchar32 uc;
    DEVFONT* devfont = SELECT_DEVFONT_BY_ACHAR(logfont, chv);

    if (devfont) {
        chv = REAL_ACHAR(chv);
        if (devfont->charset_ops->conv_to_uc32)
            uc = devfont->charset_ops->conv_to_uc32(chv);
        else
            uc = chv;

        return uc;
    }

    return 0;
}

int GUIAPI AChars2UChars(LOGFONT* logfont, const Achar32* chs,
        Uchar32* ucs, int n)
{
    int i;
    Uchar32 uc;
    DEVFONT* devfont;

    for (i = 0; i < n; i++) {
        Achar32 chv = chs[i];
        devfont = SELECT_DEVFONT_BY_ACHAR(logfont, chv);
        if (devfont) {
            chv = REAL_ACHAR(chv);
            if (devfont->charset_ops->conv_to_uc32)
                uc = devfont->charset_ops->conv_to_uc32(chv);
            else
                uc = chv;

            ucs[i] = uc;
        }
        else
            return i + 1;
    }

    return TRUE;
}

BOOL GUIAPI UChar2AChar(LOGFONT* logfont, Uchar32 uc, Achar32* ac)
{
    DEVFONT* mbc_devfont;
    DEVFONT* sbc_devfont;
    unsigned char mchar [16];
    int l = 0;

    if (logfont == NULL || uc == 0 || ac == NULL)
        return FALSE;

    mbc_devfont = logfont->devfonts[1];
    sbc_devfont = logfont->devfonts[0];
    if (mbc_devfont) {
        if (mbc_devfont->charset_ops->conv_to_uc32) {
            // not in Unicode encoding
            l = mbc_devfont->charset_ops->conv_from_uc32(uc, mchar);
            if (l > 0) {
                *ac = mbc_devfont->charset_ops->get_char_value(NULL, 0,
                        mchar, l);
                *ac = SET_MBCHV(*ac);
            }
        }
        else {
            l = 1;
            *ac = SET_MBCHV(uc);
        }
    }

    if (l <= 0) {
        if (sbc_devfont->charset_ops->conv_to_uc32) {
            // not in Unicode encoding
            l = sbc_devfont->charset_ops->conv_from_uc32(uc, mchar);
            if (l > 0) {
                *ac = mbc_devfont->charset_ops->get_char_value(NULL, 0,
                    mchar, l);
            }
        }
        else {
            l = 1;
            *ac = uc;
        }
    }

    if (l <= 0)
        return FALSE;

    return TRUE;
}

int GUIAPI UChars2AChars(LOGFONT* logfont, const Uchar32* ucs,
        Achar32* acs, int n)
{
    DEVFONT* mbc_devfont;
    DEVFONT* sbc_devfont;
    unsigned char mchar [16];
    int i, l = 0;

    if (logfont == NULL || ucs == NULL || acs == NULL)
        return FALSE;

    mbc_devfont = logfont->devfonts[1];
    sbc_devfont = logfont->devfonts[0];

    for (i = 0; i < n; i++) {
        if (mbc_devfont) {
            if (mbc_devfont->charset_ops->conv_to_uc32) {
                // not in Unicode encoding
                l = mbc_devfont->charset_ops->conv_from_uc32(ucs[i], mchar);
                if (l > 0) {
                    acs[i] = mbc_devfont->charset_ops->get_char_value(NULL, 0,
                            mchar, l);
                    acs[i] = SET_MBCHV(acs[i]);
                }
            }
            else {
                l = 1;
                acs[i] = SET_MBCHV(ucs[i]);
            }

        }

        if (l <= 0) {
            if (sbc_devfont->charset_ops->conv_to_uc32) {
                // not in Unicode encoding
                l = sbc_devfont->charset_ops->conv_from_uc32(ucs[i], mchar);
                if (l > 0) {
                    acs[i] = mbc_devfont->charset_ops->get_char_value(NULL, 0,
                        mchar, l);
                }
            }
            else {
                l = 1;
                acs[i] = ucs[i];
            }
        }

        if (l <= 0)
            return i;
    }

    return i;
}

#define MIN_LEN_UCHARS      4
#define INC_LEN_UCHARS      8

struct ustr_ctxt {
    Uchar32* ucs;
    int      len_buff;
    int      n;
    Uint8    wsr;
};

static int usctxt_init_spaces(struct ustr_ctxt* ctxt, int size)
{
    // pre-allocate buffers
    ctxt->len_buff = size;
    if (ctxt->len_buff < MIN_LEN_UCHARS)
        ctxt->len_buff = MIN_LEN_UCHARS;

    ctxt->ucs = (Uchar32*)malloc(sizeof(Uchar32) * ctxt->len_buff);
    if (ctxt->ucs == NULL)
        return 0;

    ctxt->n = 0;
    return ctxt->len_buff;
}

static int usctxt_push_back(struct ustr_ctxt* ctxt, Uchar32 uc)
{
    /* realloc buffers if it needs */
    if ((ctxt->n + 2) >= ctxt->len_buff) {
        ctxt->len_buff += INC_LEN_UCHARS;
        ctxt->ucs = (Uchar32*)realloc(ctxt->ucs,
            sizeof(Uchar32) * ctxt->len_buff);

        if (ctxt->ucs == NULL)
            return 0;
    }

    ctxt->ucs[ctxt->n] = uc;
    ctxt->n++;
    return ctxt->n;
}

int GetNextUChar(LOGFONT* lf, const char* mstr, int mstr_len,
        Uchar32* uc)
{
    int mclen = 0;

    if (mstr_len <= 0 || *mstr == '\0')
        return 0;

    if (lf->devfonts[1]) {
        mclen = lf->devfonts[1]->charset_ops->len_first_char
            ((const unsigned char*)mstr, mstr_len);

        if (mclen > 0) {
            Achar32 chv = lf->devfonts[1]->charset_ops->get_char_value
                (NULL, 0, (Uint8*)mstr, mclen);

            if (lf->devfonts[1]->charset_ops->conv_to_uc32)
                *uc = lf->devfonts[1]->charset_ops->conv_to_uc32(chv);
            else
                *uc = chv;

            chv = SET_MBCHV(chv);
        }
    }

    if (mclen == 0) {
        mclen = lf->devfonts[0]->charset_ops->len_first_char
            ((const unsigned char*)mstr, mstr_len);

        if (mclen > 0) {
            Achar32 chv = lf->devfonts[0]->charset_ops->get_char_value
                (NULL, 0, (Uint8*)mstr, mclen);

            if (lf->devfonts[0]->charset_ops->conv_to_uc32)
                *uc = lf->devfonts[0]->charset_ops->conv_to_uc32(chv);
            else
                *uc = chv;
        }
    }

    return mclen;
}

static int is_next_mchar_bt(LOGFONT* lf,
        const char* mstr, int mstr_len, Uchar32* uc,
        UCharBreakType bt)
{
    int mclen;

    mclen = GetNextUChar(lf, mstr, mstr_len, uc);
    if (mclen > 0 && UCharGetBreakType(*uc) == bt)
        return mclen;

    return 0;
}

static inline int is_next_mchar_lf(LOGFONT* lf,
        const char* mstr, int mstr_len, Uchar32* uc)
{
    return is_next_mchar_bt(lf, mstr, mstr_len, uc,
            UCHAR_BREAK_LINE_FEED);
}

static int collapse_space(LOGFONT* lf, const char* mstr, int mstr_len)
{
    Uchar32 uc;
    UCharBreakType bt;
    int cosumed = 0;

    do {
        int mclen;

        mclen = GetNextUChar(lf, mstr, mstr_len, &uc);
        if (mclen == 0)
            break;

        bt = UCharGetBreakType(uc);
        if (bt != UCHAR_BREAK_SPACE && uc != UCHAR_TAB)
            break;

        mstr += mclen;
        mstr_len -= mclen;
        cosumed += mclen;
    } while (1);

    return cosumed;
}

/*
    Reference:
    [CSS Text Module Level 3](https://www.w3.org/TR/css-text-3/)
 */
int GUIAPI GetUCharsUntilParagraphBoundary(LOGFONT* logfont,
        const char* mstr, int mstr_len, Uint8 wsr,
        Uchar32** uchars, int* nr_uchars)
{
    struct ustr_ctxt ctxt;
    int cosumed = 0;
    BOOL col_sp = FALSE;
    BOOL col_nl = FALSE;

    // CSS: collapses space according to space rule
    if (wsr == WSR_NORMAL || wsr == WSR_NOWRAP || wsr == WSR_PRE_LINE)
        col_sp = TRUE;
    // CSS: collapses new lines acoording to space rule
    if (wsr == WSR_NORMAL || wsr == WSR_NOWRAP)
        col_nl = TRUE;

    *uchars = NULL;
    *nr_uchars = 0;

    if (mstr_len == 0)
        return 0;

    ctxt.wsr = wsr;
    if (usctxt_init_spaces(&ctxt, mstr_len >> 1) <= 0) {
        goto error;
    }

    while (TRUE) {
        Uchar32 uc, next_uc;
        UCharBreakType bt;

        int mclen = 0;
        int next_mclen;
        int cosumed_one_loop = 0;

        mclen = GetNextUChar(logfont, mstr, mstr_len, &uc);
        if (mclen == 0) {
            // badly encoded or end of text
            break;
        }

        mstr += mclen;
        mstr_len -= mclen;
        cosumed += mclen;

        if ((wsr == WSR_NORMAL || wsr == WSR_NOWRAP
                || wsr == WSR_PRE_LINE) && uc == UCHAR_TAB) {
            _DBG_PRINTF ("CSS: Every tab is converted to a space (U+0020)\n");
            uc = UCHAR_SPACE;
        }

        bt = UCharGetBreakType(uc);
        if (usctxt_push_back(&ctxt, uc) == 0)
            goto error;

        /* Check mandatory breaks */
        if (bt == UCHAR_BREAK_MANDATORY) {
            break;
        }
        else if (bt == UCHAR_BREAK_CARRIAGE_RETURN
                && (next_mclen = is_next_mchar_lf(logfont,
                    mstr, mstr_len, &next_uc)) > 0) {
            cosumed_one_loop += next_mclen;

            if (col_nl) {
                ctxt.n--;
            }
            else {
                if (usctxt_push_back(&ctxt, next_uc) == 0)
                    goto error;
                break;
            }
        }
        else if (bt == UCHAR_BREAK_CARRIAGE_RETURN
                || bt == UCHAR_BREAK_LINE_FEED
                || bt == UCHAR_BREAK_NEXT_LINE) {

            if (col_nl) {
                ctxt.n--;
            }
            else {
                break;
            }
        }
        /* collapse spaces */
        else if (col_sp && (bt == UCHAR_BREAK_SPACE
                || bt == UCHAR_BREAK_ZERO_WIDTH_SPACE)) {
            cosumed_one_loop += collapse_space(logfont, mstr, mstr_len);
        }

        mstr_len -= cosumed_one_loop;
        mstr += cosumed_one_loop;
        cosumed += cosumed_one_loop;
    }

    if (ctxt.n > 0) {
        *uchars = ctxt.ucs;
        *nr_uchars = ctxt.n;
    }
    else
        goto error;

    return cosumed;

error:
    if (ctxt.ucs) free(ctxt.ucs);
    return 0;
}

#endif /* _MGCHARSET_UNICODE */

