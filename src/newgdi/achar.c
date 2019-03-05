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
** achar.c: API implementation for abstract character.
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
    DEVFONT* devfont = SELECT_DEVFONT_BY_ACHAR(logfont, chv);
    return devfont->charset_ops->char_type (REAL_ACHAR (chv));
}

Uint32 GetGlyphBIDIType (LOGFONT* log_font, Achar32 chv)
{
    DEVFONT* devfont = SELECT_DEVFONT_BY_ACHAR(log_font, chv);

    if (devfont == NULL ||
            devfont->charset_ops->bidi_char_type == NULL)
        return BIDI_TYPE_INVALID;

    return devfont->charset_ops->bidi_char_type (REAL_ACHAR (chv));
}

#ifdef _MGCHARSET_UNICODE

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

#endif /* _MGCHARSET_UNICODE */
