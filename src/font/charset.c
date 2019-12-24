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
** charset.c: The charset operation set.
**
** Create date: 2000/06/13
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "devfont.h"
#include "charset.h"

/*************** Common Operations for single byte charsets ******************/
static int sb_len_first_char (const unsigned char* mstr, int len)
{
    if (len < 1) return 0;
    if (*mstr != '\0')  return 1;
    return 0;
}

static Achar32 sb_get_char_value (const unsigned char* pre_mchar,  int pre_len,
                const unsigned char* cur_mchar, int cur_len)
{
    return (Achar32)(*cur_mchar);
}

#define sb_char_type __mg_sb_char_type

unsigned int __mg_sb_char_type (Achar32 chv)
{
    unsigned int ch_type = ACHAR_BASIC_UNKNOWN;

    switch (chv) {
        case '\0':
            ch_type = ACHAR_BASIC_NUL;
            break;
        case '\a':
            ch_type = ACHAR_BASIC_BEL;
            break;
        case '\b':
            ch_type = ACHAR_BASIC_BS;
            break;
        case '\t':
            ch_type = ACHAR_BASIC_HT;
            break;
        case '\n':
            ch_type = ACHAR_BASIC_LF;
            break;
        case '\v':
            ch_type = ACHAR_BASIC_VT;
            break;
        case '\f':
            ch_type = ACHAR_BASIC_FF;
            break;
        case '\r':
            ch_type = ACHAR_BASIC_CR;
            break;
        case ' ':
            ch_type = ACHAR_BASIC_SPACE;
            break;
    }

    if (ch_type == ACHAR_BASIC_UNKNOWN) {
        if (chv < '\a')
            ch_type = ACHAR_BASIC_CTRL1;
        else if (chv < ' ')
            ch_type = ACHAR_BASIC_CTRL2;
        else
            ch_type = ACHAR_BASIC_GENERIC;
    }

    return ch_type;
}

static int sb_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    return mstrlen;
}

static int sb_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    return mstrlen;
}

static int sb_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    return 0;
}

static const unsigned char* sb_get_next_word (const unsigned char* mstr,
                int mstrlen, WORDINFO* word_info)
{
    int i;

    word_info->len = 0;
    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    if (mstrlen == 0) return NULL;

    for (i = 0; i < mstrlen; i++) {
        if (mstr[i] > 127) {
            if (word_info->len > 0) {
                word_info->delimiter = mstr[i];
                word_info->nr_delimiters ++;
            }
            else {
                word_info->len ++;
                word_info->delimiter = ' ';
                word_info->nr_delimiters ++;
            }
            break;
        }

        switch (mstr[i]) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            if (word_info->delimiter == '\0') {
                word_info->delimiter = mstr[i];
                word_info->nr_delimiters ++;
            }
            else if (word_info->delimiter == mstr[i])
                word_info->nr_delimiters ++;
            else
                return mstr + word_info->len + word_info->nr_delimiters;
        break;

        default:
            if (word_info->delimiter != '\0')
                break;

            word_info->len ++;
        }
    }

    return mstr + word_info->len + word_info->nr_delimiters;
}

/************************* US-ASCII Specific Operations **********************/
static int ascii_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ASCII"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 ascii_conv_to_uc32 (Achar32 chv)
{
    if (chv < 128)
        return chv;
    else
        return '?';
}

static int ascii_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    if (wc < 128) {
        *mchar = (unsigned char) wc;
        return 1;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_ascii = {
    128,
    1,
    FONT_CHARSET_US_ASCII,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    ascii_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    ascii_conv_to_uc32,
    ascii_conv_from_uc32
#endif
};

/************************* ISO8859-1 Specific Operations **********************/
static int iso8859_1_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];
    char* sub_enc;

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && (sub_enc = strstr (name, "8859-1"))
            && (sub_enc [6] == '\0'))
        return 0;

    if ((sub_enc = strstr (name, "LATIN1")) && (sub_enc [6] == '\0'))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 iso8859_1_conv_to_uc32 (Achar32 chv)
{
    return chv;
}

static int iso8859_1_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    if (wc < 256) {
        *mchar = (unsigned char) wc;
        return 1;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_1 = {
    256,
    1,
    FONT_CHARSET_ISO8859_1,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_1_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_1_conv_to_uc32,
    iso8859_1_conv_from_uc32
#endif
};

#ifdef _MGCHARSET_LATIN2

/************************* ISO8859-2 Specific Operations **********************/
static int iso8859_2_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-2"))
        return 0;

    if (strstr (name, "LATIN2"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static const unsigned short iso8859_2_unicode_map [] =
{
    0x0104, 0x02D8,
    0x0141, 0x00A4, 0x013D,
    0x015A, 0x00A7, 0x00A8,
    0x0160, 0x015E, 0x0164,
    0x0179, 0x00AD, 0x017D,
    0x017B, 0x00B0, 0x0105,
    0x02DB, 0x0142, 0x00B4,
    0x013E, 0x015B, 0x02C7,
    0x00B8, 0x0161, 0x015F,
    0x0165, 0x017A, 0x02DD,
    0x017E, 0x017C, 0x0154,
    0x00C1, 0x00C2, 0x0102,
    0x00C4, 0x0139, 0x0106,
    0x00C7, 0x010C, 0x00C9,
    0x0118, 0x00CB, 0x011A,
    0x00CD, 0x00CE, 0x010E,
    0x0110, 0x0143, 0x0147,
    0x00D3, 0x00D4, 0x0150,
    0x00D6, 0x00D7, 0x0158,
    0x016E, 0x00DA, 0x0170,
    0x00DC, 0x00DD, 0x0162,
    0x00DF, 0x0155, 0x00E1,
    0x00E2, 0x0103, 0x00E4,
    0x013A, 0x0107, 0x00E7,
    0x010D, 0x00E9, 0x0119,
    0x00EB, 0x011B, 0x00ED,
    0x00EE, 0x010F, 0x0111,
    0x0144, 0x0148, 0x00F3,
    0x00F4, 0x0151, 0x00F6,
    0x00F7, 0x0159, 0x016F,
    0x00FA, 0x0171, 0x00FC,
    0x00FD, 0x0163, 0x02D9
};

static Uchar32 iso8859_2_conv_to_uc32 (Achar32 chv)
{
    if (chv < 0xA1)
        return chv;
    else
        return (Uchar32) iso8859_2_unicode_map [chv - 0xA1];
}

static int iso8859_2_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    int i;

    if (wc < 0xA1) {
        *mchar = (unsigned char)wc;
        return 1;
    }

    for (i = 0; i < TABLESIZE (iso8859_2_unicode_map); i++) {
        if (((Uchar32)iso8859_2_unicode_map [i]) == wc) {
            *mchar = 0xA1 + i;
            return 1;
        }
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_2 = {
    256,
    1,
    FONT_CHARSET_ISO8859_2,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_2_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_2_conv_to_uc32,
    iso8859_2_conv_from_uc32,
#endif
};

#endif /* _LATIN2 */

#ifdef _MGCHARSET_LATIN3

/************************* ISO8859-3 Specific Operations **********************/
static int iso8859_3_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-3"))
        return 0;

    if (strstr (name, "LATIN3"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static const unsigned short iso8859_3_unicode_map [] =
{
    0x0126, 0x02D8, 0x00A3,
    0x00A4, 0x00A5, 0x0124,
    0x00A7, 0x00A8, 0x0130,
    0x015E, 0x011E, 0x0134,
    0x00AD, 0x00AE, 0x017B,
    0x00B0, 0x0127, 0x00B2,
    0x00B3, 0x00B4, 0x00B5,
    0x0125, 0x00B7, 0x00B8,
    0x0131, 0x015F, 0x011F,
    0x0135, 0x00BD, 0x00BE,
    0x017C, 0x00C0, 0x00C1,
    0x00C2, 0x00C3, 0x00C4,
    0x010A, 0x0108, 0x00C7,
    0x00C8, 0x00C9, 0x00CA,
    0x00CB, 0x00CC, 0x00CD,
    0x00CE, 0x00CF, 0x00D0,
    0x00D1, 0x00D2, 0x00D3,
    0x00D4, 0x0120, 0x00D6,
    0x00D7, 0x011C, 0x00D9,
    0x00DA, 0x00DB, 0x00DC,
    0x016C, 0x015C, 0x00DF,
    0x00E0, 0x00E1, 0x00E2,
    0x00E3, 0x00E4, 0x010B,
    0x0109, 0x00E7, 0x00E8,
    0x00E9, 0x00EA, 0x00EB,
    0x00EC, 0x00ED, 0x00EE,
    0x00EF, 0x00F0, 0x00F1,
    0x00F2, 0x00F3, 0x00F4,
    0x0121, 0x00F6, 0x00F7,
    0x011D, 0x00F9, 0x00FA,
    0x00FB, 0x00FC, 0x016D,
    0x015D, 0x02D9
};

static Uchar32 iso8859_3_conv_to_uc32 (Achar32 chv)
{
    if (chv < 0xA1)
        return chv;
    else
        return (Uchar32) iso8859_3_unicode_map [chv - 0xA1];
}

static int iso8859_3_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    int i;

    if (wc < 0xA1) {
        *mchar = (unsigned char) wc;
        return 1;
    }

    for (i = 0; i < TABLESIZE (iso8859_3_unicode_map); i++) {
        if (((Uchar32)iso8859_3_unicode_map [i]) == wc) {
            *mchar = 0xA1 + i;
            return 1;
        }
    }
    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_3 = {
    256,
    1,
    FONT_CHARSET_ISO8859_3,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_3_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_3_conv_to_uc32,
    iso8859_3_conv_from_uc32,
#endif
};

#endif /* _LATIN4 */

#ifdef _MGCHARSET_LATIN4

/************************* ISO8859-4 Specific Operations **********************/
static int iso8859_4_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-4"))
        return 0;

    if (strstr (name, "LATIN4"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static const unsigned short iso8859_4_unicode_map [] =
{
    0x0104, 0x0138, 0x0156,
    0x00A4, 0x0128, 0x013B,
    0x00A7, 0x00A8, 0x0160,
    0x0112, 0x0122, 0x0166,
    0x00AD, 0x017D, 0x00AF,
    0x00B0, 0x0105, 0x02DB,
    0x0157, 0x00B4, 0x0129,
    0x013C, 0x02C7, 0x00B8,
    0x0161, 0x0113, 0x0123,
    0x0167, 0x014A, 0x017E,
    0x014B, 0x0100, 0x00C1,
    0x00C2, 0x00C3, 0x00C4,
    0x00C5, 0x00C6, 0x012E,
    0x010C, 0x00C9, 0x0118,
    0x00CB, 0x0116, 0x00CD,
    0x00CE, 0x012A, 0x0110,
    0x0145, 0x014C, 0x0136,
    0x00D4, 0x00D5, 0x00D6,
    0x00D7, 0x00D8, 0x0172,
    0x00DA, 0x00DB, 0x00DC,
    0x0168, 0x016A, 0x00DF,
    0x0101, 0x00E1, 0x00E2,
    0x00E3, 0x00E4, 0x00E5,
    0x00E6, 0x012F, 0x010D,
    0x00E9, 0x0119, 0x00EB,
    0x0117, 0x00ED, 0x00EE,
    0x012B, 0x0111, 0x0146,
    0x014D, 0x0137, 0x00F4,
    0x00F5, 0x00F6, 0x00F7,
    0x00F8, 0x0173, 0x00FA,
    0x00FB, 0x00FC, 0x0169,
    0x016B, 0x02D9
};

static Uchar32 iso8859_4_conv_to_uc32 (Achar32 chv)
{
    if (chv < 0xA1)
        return chv;
    else
        return (Uchar32) iso8859_4_unicode_map [chv - 0xA1];
}

static int iso8859_4_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    int i;

    if (wc < 0xA1) {
        *mchar = (unsigned char) wc;
        return 1;
    }

    for (i = 0; i < TABLESIZE (iso8859_4_unicode_map); i++) {
        if (((Uchar32)iso8859_4_unicode_map [i]) == wc) {
            *mchar = 0xA1 + i;
            return 1;
        }
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_4 = {
    256,
    1,
    FONT_CHARSET_ISO8859_4,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_4_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_4_conv_to_uc32,
    iso8859_4_conv_from_uc32
#endif
};

#endif /* _LATIN4 */

#ifdef _MGCHARSET_CYRILLIC

/************************* ISO8859-5 Specific Operations **********************/
static int iso8859_5_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-5"))
        return 0;

    if (strstr (name, "CYRILLIC"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 iso8859_5_conv_to_uc32 (Achar32 chv)
{
    if (chv < 0xA1)
        return chv;

    if (chv == 0xAD)
        return 0x00AD;  /* SOFT HYPHEN */

    if (chv == 0xF0)
        return 0x2116;  /* NUMERO SIGN */

    if (chv == 0xFD)
        return 0x00A7;  /* SECTION SIGN */

    return chv + (0x0401 - 0xA1);
}

static int iso8859_5_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    if (wc < 0xA1) {
        *mchar = (unsigned char) wc;
        return 1;
    }

    switch (wc) {
        case 0x00A7:
            *mchar = 0xFD;
            return 1;
        case 0x00AD:
            *mchar = 0xAD;
            return 1;
        case 0x2116:
            *mchar = 0xF0;
            return 1;
    }

    if (wc >= 0x0401 && wc <= (0x0401 + 0xFF - 0xA1)) {
        *mchar = (unsigned char) (wc - 0x0401 + 0xA1);
        return 1;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_5 = {
    256,
    1,
    FONT_CHARSET_ISO8859_5,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_5_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_5_conv_to_uc32,
    iso8859_5_conv_from_uc32,
#endif
};

#endif /* _CYRILLIC */

#if defined (_MGCHARSET_HEBREW) || defined (_MGCHARSET_ARABIC)
#include "legacy-bidi-tables.inc"
#endif

#if defined (_MGCHARSET_HEBREW) || defined (_MGCHARSET_ARABIC) || defined (_MGCHARSET_UNICODE)

#include "bidi.h"

#define get_mirror_char __mg_get_mirror_char

BOOL __mg_get_mirror_char (const BIDICHAR_MIRROR_MAP* map, int n,
        Achar32 chv, Achar32* mirrored)
{
    int pos, step;
    BOOL found = FALSE;
    Achar32 mbc_mask;

    pos = step = (n / 2) + 1;

    mbc_mask = chv & ACHAR_MBC_FLAG;
    chv = REAL_ACHAR(chv);

    while (step > 1) {
        Achar32 cmp_char = map[pos].chv;
        step = (step + 1) / 2;

        if (cmp_char < chv) {
            pos += step;
            if (pos > n - 1)
                pos = n - 1;
        }
        else if (cmp_char > chv) {
            pos -= step;
            if (pos < 0)
                pos = 0;
        }
        else
            break;
    }

    found = map[pos].chv == chv;

    if (mirrored){
        *mirrored = found ? map[pos].mirrored : chv;
        *mirrored |= mbc_mask;
    }

    return found;
}

#endif /* definede (_MGCHARSET_HEBREW) || defined (_MGCHARSET_ARABIC) || defined (_MGCHARSET_UNICODE) */

/* houhh 20080123 */
#ifdef _MGCHARSET_ARABIC
#include "charset-arabic.c"
#endif /* _ARABIC */

#ifdef _MGCHARSET_GREEK

/************************* ISO8859-7 Specific Operations **********************/
static int iso8859_7_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-7"))
        return 0;

    if (strstr (name, "GREEK"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static const unsigned short iso8859_7_unicode_map [] =
{
    0x2018, 0x2019, 0x00A3,
    0x00A4, 0x00A5, 0x00A6,
    0x00A7, 0x00A8, 0x00A9,
    0x00AA, 0x00AB, 0x00AC,
    0x00AD, 0x00AE, 0x2015,
    0x00B0, 0x00B1, 0x00B2,
    0x00B3, 0x0384, 0x0385,
    0x0386, 0x00B7, 0x0388,
    0x0389, 0x038A, 0x00BB,
    0x038C, 0x00BD
};

static Uchar32 iso8859_7_conv_to_uc32 (Achar32 chv)
{
    if (chv >= 0xBE && chv <= 0xFE) {
        return chv - 0xBE + 0x038E;
    }
    else if (chv >= 0xA1 && chv <= 0xBD) {
        return (Uchar32)iso8859_7_unicode_map [chv - 0xA1];
    }
    else {
        return chv;
    }
}

static int iso8859_7_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    int i;

    if (wc < 0xA1) {
        *mchar = (unsigned char) wc;
        return 1;
    }

    if (wc >= 0x038E && wc <= (0x038E + 0xFE - 0xBE)) {
        *mchar = (unsigned char) (wc - 0x038E + 0xBE);
        return 1;
    }

    for (i = 0; i < TABLESIZE (iso8859_7_unicode_map); i++) {
        if (((Uchar32)iso8859_7_unicode_map [i]) == wc) {
            *mchar = 0xA1 + i;
            return 1;
        }
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_7 = {
    256,
    1,
    FONT_CHARSET_ISO8859_7,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_7_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_7_conv_to_uc32,
    iso8859_7_conv_from_uc32
#endif
};

#endif /* _GREEK */

#ifdef _MGCHARSET_HEBREW

/************************* ISO8859-8 Specific Operations **********************/
static int iso8859_8_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-8"))
        return 0;

    if (strstr (name, "HEBREW"))
        return 0;

    return 1;
}

static const unsigned char* iso8859_8_get_next_word (const unsigned char* mstr,
        int mstrlen, WORDINFO* word_info)
{
    int i;

    word_info->len = 0;
    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    if (mstrlen == 0) return NULL;

    for (i = 0; i < mstrlen; i++) {
        switch (mstr[i]) {
            case 0xa0:
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if (word_info->delimiter == '\0') {
                    word_info->delimiter = mstr[i];
                    word_info->nr_delimiters ++;
                }
                else if (word_info->delimiter == mstr[i])
                    word_info->nr_delimiters ++;
                else
                    return mstr + word_info->len + word_info->nr_delimiters;
                break;

            default:
                if (word_info->delimiter != '\0')
                    break;

                word_info->len++;
        }
    }

    return mstr + word_info->len + word_info->nr_delimiters;

    //return sb_get_next_word(mstr, mstrlen, word_info);
}

static BidiType __mg_iso8859_8_bidi_char_type_map[] = {
#if 0
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
#endif

    /*0x80~0x8f*/
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,

    /*0x90~0x9f*/
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,

    /*0xa0~0xaf*/
    BIDI_TYPE_CS,  BIDI_TYPE_BN,  BIDI_TYPE_ET,  BIDI_TYPE_ET,
    BIDI_TYPE_ET,  BIDI_TYPE_ET,  BIDI_TYPE_ON,  BIDI_TYPE_ON,
    BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ON,
    BIDI_TYPE_ON,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_ON,

    /*0xb0~0xbf*/
    BIDI_TYPE_ET,  BIDI_TYPE_ET,  BIDI_TYPE_EN,  BIDI_TYPE_EN,
    BIDI_TYPE_ON,  BIDI_TYPE_LTR, BIDI_TYPE_ON,  BIDI_TYPE_ON,
    BIDI_TYPE_ON,  BIDI_TYPE_EN,  BIDI_TYPE_ON,  BIDI_TYPE_ON,
    BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_ON,  BIDI_TYPE_BN,

    /*0xc0~0xcf*/
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,

    /*0xd0~0xdf*/
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_ON,

    /*0xe0~0xef*/
    BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,
    BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,
    BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,
    BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_ON,

    /*0xf0~0xff*/
    BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,
    BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,
    BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,
    BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_RTL,  BIDI_TYPE_BN,
    BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,  BIDI_TYPE_BN,
};

static BidiType iso8859_8_bidi_char_type (Achar32 chv)
{
    chv = REAL_ACHAR(chv);
    if (chv > 0x7F)
        return (BidiType)__mg_iso8859_8_bidi_char_type_map[chv - 0x80];
    else
        return (BidiType)__mg_iso8859_1_bidi_char_type_map[chv];
}

static const BIDICHAR_MIRROR_MAP __mg_iso8859_8_mirror_table [] =
{
    {0x0028, 0x0029},
    {0x0029, 0x0028},
    {0x003C, 0x003E},
    {0x003E, 0x003C},
    {0x005B, 0x005D},
    {0x005D, 0x005B},
    {0x007B, 0x007D},
    {0x007D, 0x007B},
//  {0x00AB, 0x00BB},
//  {0x00BB, 0x00AB}
};

static BOOL iso8859_8_bidi_mirror_char (Achar32 chv, Achar32* mirrored)
{
    return get_mirror_char (__mg_iso8859_8_mirror_table,
            TABLESIZE (__mg_iso8859_8_mirror_table), chv, mirrored);
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 iso8859_8_conv_to_uc32 (Achar32 chv)
{
    chv = REAL_ACHAR(chv);

    if (chv >= 0xE0 && chv <= 0xFA) {
        return  chv - 0xE0 + 0x05D0;
    }
    else {
        switch (chv) {
            case 0xAA:
                return 0x00D7;
            case 0xBA:
                return 0x00F7;
            case 0xDF:
                return 0x2017;
            case 0xFD:
                return 0x200E;
            case 0xFE:
                return 0x200F;
        }
        return chv;
    }
}

static int iso8859_8_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{

    switch (wc) {
        case 0x00D7C:
            *mchar = 0xAA;
            return 1;
        case 0x203E:
            *mchar = 0xAF;
            return 1;
        case 0x0F7:
            *mchar = 0xBA;
            return 1;
        case 0x2017:
            *mchar = 0xBF;
            return 1;
    }

    if (wc < 0x5D0) {
        *mchar = (unsigned char) wc;
        return 1;
    }

    if (wc >= 0x5D0 && wc <= 0x05EA) {
        *mchar = (unsigned char) (wc - 0x5D0 + 0xE0);
        return 1;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_8 = {
    256,
    2, // This is a trick for bidi reorder only implemented for MBC.
    FONT_CHARSET_ISO8859_8,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_8_is_this_charset,
    sb_len_first_substr,
    iso8859_8_get_next_word,
    sb_pos_first_char,
    iso8859_8_bidi_char_type,
    iso8859_8_bidi_mirror_char,
#ifdef _MGCHARSET_UNICODE
    iso8859_8_conv_to_uc32,
    iso8859_8_conv_from_uc32
#endif
};

#endif /* _GREEK */

#ifdef _MGCHARSET_LATIN5

/************************* ISO8859-9 Specific Operations **********************/
static int iso8859_9_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-9"))
        return 0;

    if (strstr (name, "LATIN5"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 iso8859_9_conv_to_uc32 (Achar32 chv)
{
    switch (chv) {
        case 0xD0:
                return 0x011E;
        case 0xDD:
                return 0x0130;
        case 0xDE:
                return 0x015E;
        case 0xF0:
                return 0x011F;
        case 0xFD:
                return 0x0131;
        case 0xFE:
                return 0x015F;
    }

    return chv;
}

static int iso8859_9_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    switch (wc) {
        case 0x011E:
            *mchar = 0xD0;
            return 1;
        case 0x0130:
            *mchar = 0xDD;
            return 1;
        case 0x015E:
            *mchar = 0xDE;
            return 1;
        case 0x011F:
            *mchar = 0xF0;
            return 1;
        case 0x0131:
            *mchar = 0xFD;
            return 1;
        case 0x015F:
            *mchar = 0xFE;
            return 1;
    }

    if (wc <= 0xFF) {
        *mchar = (unsigned char)wc;
        return 1;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_9 = {
    256,
    1,
    FONT_CHARSET_ISO8859_9,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_9_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_9_conv_to_uc32,
    iso8859_9_conv_from_uc32
#endif
};

#endif /* _LATIN5 */

#ifdef _MGCHARSET_LATIN6

/************************* ISO8859-10 Specific Operations **********************/
static int iso8859_10_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-10"))
        return 0;

    if (strstr (name, "LATIN6"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static const unsigned short iso8859_10_unicode_map [] =
{
    0x0104, 0x0112, 0x0122,
    0x012A, 0x0128, 0x0136,
    0x00A7, 0x013B, 0x0110,
    0x0160, 0x0166, 0x017D,
    0x00AD, 0x016A, 0x014A,
    0x00B0, 0x0105, 0x0113,
    0x0123, 0x012B, 0x0129,
    0x0137, 0x00B7, 0x013C,
    0x0111, 0x0161, 0x0167,
    0x017E, 0x2015, 0x016B,
    0x014B, 0x0100, 0x00C1,
    0x00C2, 0x00C3, 0x00C4,
    0x00C5, 0x00C6, 0x012E,
    0x010C, 0x00C9, 0x0118,
    0x00CB, 0x0116, 0x00CD,
    0x00CE, 0x00CF, 0x00D0,
    0x0145, 0x014C, 0x00D3,
    0x00D4, 0x00D5, 0x00D6,
    0x0168, 0x00D8, 0x0172,
    0x00DA, 0x00DB, 0x00DC,
    0x00DD, 0x00DE, 0x00DF,
    0x0101, 0x00E1, 0x00E2,
    0x00E3, 0x00E4, 0x00E5,
    0x00E6, 0x012F, 0x010D,
    0x00E9, 0x0119, 0x00EB,
    0x0117, 0x00ED, 0x00EE,
    0x00EF, 0x00F0, 0x0146,
    0x014D, 0x00F3, 0x00F4,
    0x00F5, 0x00F6, 0x0169,
    0x00F8, 0x0173, 0x00FA,
    0x00FB, 0x00FC, 0x00FD,
    0x00FE, 0x0138
};

static Uchar32 iso8859_10_conv_to_uc32 (Achar32 chv)
{
    if (chv < 0xA1)
        return chv;
    else
        return (Uchar32)iso8859_10_unicode_map [chv - 0xA1];
}

static int iso8859_10_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    int i;

    if (wc < 0xA1) {
        *mchar = (unsigned char)wc;
        return 1;
    }

    for (i = 0; i < TABLESIZE (iso8859_10_unicode_map); i++) {
        if (((Uchar32)iso8859_10_unicode_map [i]) == wc) {
            *mchar = 0xA1 + i;
            return 1;
        }
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_10 = {
    256,
    1,
    FONT_CHARSET_ISO8859_10,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_10_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_10_conv_to_uc32,
    iso8859_10_conv_from_uc32
#endif
};

#endif /* _LATIN6 */

#ifdef _MGCHARSET_THAI

/************************* ISO8859-11 Specific Operations **********************/
static int iso8859_11_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-11"))
        return 0;

    if (strstr (name, "TIS") && strstr (name, "620"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static const unsigned short iso8859_11_unicode_map [] =
{
    0x0E01, 0x0E02, 0x0E03,
    0x0E04, 0x0E05, 0x0E06,
    0x0E07, 0x0E08, 0x0E09,
    0x0E0A, 0x0E0B, 0x0E0C,
    0x0E0D, 0x0E0E, 0x0E0F,
    0x0E10, 0x0E11, 0x0E12,
    0x0E13, 0x0E14, 0x0E15,
    0x0E16, 0x0E17, 0x0E18,
    0x0E19, 0x0E1A, 0x0E1B,
    0x0E1C, 0x0E1D, 0x0E1E,
    0x0E1F, 0x0E20, 0x0E21,
    0x0E22, 0x0E23, 0x0E24,
    0x0E25, 0x0E26, 0x0E27,
    0x0E28, 0x0E29, 0x0E2A,
    0x0E2B, 0x0E2C, 0x0E2D,
    0x0E2E, 0x0E2F, 0x0E30,
    0x0E31, 0x0E32, 0x0E33,
    0x0E34, 0x0E35, 0x0E36,
    0x0E37, 0x0E38, 0x0E39,
    0x0E3A, 0x00DB, 0x00DC,
    0x00DD, 0x00DE, 0x0E3F,
    0x0E40, 0x0E41, 0x0E42,
    0x0E43, 0x0E44, 0x0E45,
    0x0E46, 0x0E47, 0x0E48,
    0x0E49, 0x0E4A, 0x0E4B,
    0x0E4C, 0x0E4D, 0x0E4E,
    0x0E4F, 0x0E50, 0x0E51,
    0x0E52, 0x0E53, 0x0E54,
    0x0E55, 0x0E56, 0x0E57,
    0x0E58, 0x0E59, 0x0E5A,
    0x0E5B, 0x00FC, 0x00FD,
    0x00FE, 0x00FF
};

static Uchar32 iso8859_11_conv_to_uc32 (Achar32 chv)
{
    if (chv < 0xA1)
        return chv;
    else
        return (Uchar32) iso8859_11_unicode_map [chv - 0xA1];
}

static int iso8859_11_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    int i;

    if (wc < 0xA1) {
        *mchar = (unsigned char)wc;
        return 1;
    }

    for (i = 0; i < TABLESIZE (iso8859_11_unicode_map); i++) {
        if (((Uchar32)iso8859_11_unicode_map [i]) == wc) {
            *mchar = 0xA1 + i;
            return 1;
        }
    }
    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_11 = {
    256,
    1,
    FONT_CHARSET_ISO8859_11,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_11_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_11_conv_to_uc32,
    iso8859_11_conv_from_uc32
#endif
};

#endif /* _THAI */

#ifdef _MGCHARSET_LATIN7

/************************* ISO8859-13 Specific Operations **********************/
static int iso8859_13_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-13"))
        return 0;

    if (strstr (name, "LATIN7"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static const unsigned short iso8859_13_unicode_map [] =
{
    0x201D, 0x00A2, 0x00A3,
    0x00A4, 0x201E, 0x00A6,
    0x00A7, 0x00D8, 0x00A9,
    0x0156, 0x00AB, 0x00AC,
    0x00AD, 0x00AE, 0x00C6,
    0x00B0, 0x00B1, 0x00B2,
    0x00B3, 0x201C, 0x00B5,
    0x00B6, 0x00B7, 0x00F8,
    0x00B9, 0x0157, 0x00BB,
    0x00BC, 0x00BD, 0x00BE,
    0x00E6, 0x0104, 0x012E,
    0x0100, 0x0106, 0x00C4,
    0x00C5, 0x0118, 0x0112,
    0x010C, 0x00C9, 0x0179,
    0x0116, 0x0122, 0x0136,
    0x012A, 0x013B, 0x0160,
    0x0143, 0x0145, 0x00D3,
    0x014C, 0x00D5, 0x00D6,
    0x00D7, 0x0172, 0x0141,
    0x015A, 0x016A, 0x00DC,
    0x017B, 0x017D, 0x00DF,
    0x0105, 0x012F, 0x0101,
    0x0107, 0x00E4, 0x00E5,
    0x0119, 0x0113, 0x010D,
    0x00E9, 0x017A, 0x0117,
    0x0123, 0x0137, 0x012B,
    0x013C, 0x0161, 0x0144,
    0x0146, 0x00F3, 0x014D,
    0x00F5, 0x00F6, 0x00F7,
    0x0173, 0x0142, 0x015B,
    0x016B, 0x00FC, 0x017C,
    0x017E, 0x2019
};

static Uchar32 iso8859_13_conv_to_uc32 (Achar32 chv)
{
    if (chv < 0xA1)
        return chv;
    else
        return (Uchar32) iso8859_13_unicode_map [chv - 0xA1];
}

static int iso8859_13_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    int i;

    if (wc < 0xA1) {
        *mchar = (unsigned char)wc;
        return 1;
    }

    for (i = 0; i < TABLESIZE (iso8859_13_unicode_map); i++) {
        if (((Uchar32)iso8859_13_unicode_map [i]) == wc) {
            *mchar = 0xA1 + i;
            return 1;
        }
    }
    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_13 = {
    256,
    1,
    FONT_CHARSET_ISO8859_13,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_13_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_13_conv_to_uc32,
    iso8859_13_conv_from_uc32
#endif
};

#endif /* _LATIN7 */

#ifdef _MGCHARSET_LATIN8

/************************* ISO8859-14 Specific Operations **********************/
static int iso8859_14_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-14"))
        return 0;

    if (strstr (name, "LATIN8"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static const unsigned short iso8859_14_unicode_map [] =
{
    0x1E02, 0x1E03, 0x00A3,
    0x010A, 0x010B, 0x1E0A,
    0x00A7, 0x1E80, 0x00A9,
    0x1E82, 0x1E0B, 0x1EF2,
    0x00AD, 0x00AE, 0x0178,
    0x1E1E, 0x1E1F, 0x0120,
    0x0121, 0x1E40, 0x1E41,
    0x00B6, 0x1E56, 0x1E81,
    0x1E57, 0x1E83, 0x1E60,
    0x1EF3, 0x1E84, 0x1E85,
    0x1E61, 0x00C0, 0x00C1,
    0x00C2, 0x00C3, 0x00C4,
    0x00C5, 0x00C6, 0x00C7,
    0x00C8, 0x00C9, 0x00CA,
    0x00CB, 0x00CC, 0x00CD,
    0x00CE, 0x00CF, 0x0174,
    0x00D1, 0x00D2, 0x00D3,
    0x00D4, 0x00D5, 0x00D6,
    0x1E6A, 0x00D8, 0x00D9,
    0x00DA, 0x00DB, 0x00DC,
    0x00DD, 0x0176, 0x00DF,
    0x00E0, 0x00E1, 0x00E2,
    0x00E3, 0x00E4, 0x00E5,
    0x00E6, 0x00E7, 0x00E8,
    0x00E9, 0x00EA, 0x00EB,
    0x00EC, 0x00ED, 0x00EE,
    0x00EF, 0x0175, 0x00F1,
    0x00F2, 0x00F3, 0x00F4,
    0x00F5, 0x00F6, 0x1E6B,
    0x00F8, 0x00F9, 0x00FA,
    0x00FB, 0x00FC, 0x00FD,
    0x0177, 0x00FF
};

static Uchar32 iso8859_14_conv_to_uc32 (Achar32 chv)
{
    if (chv < 0xA1)
        return chv;
    else
        return (Uchar32) iso8859_14_unicode_map [chv - 0xA1];
}

static int iso8859_14_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    int i;

    if (wc < 0xA1) {
        *mchar = (unsigned char)wc;
        return 1;
    }

    for (i = 0; i < TABLESIZE (iso8859_14_unicode_map); i++) {
        if (((Uchar32)iso8859_14_unicode_map [i]) == wc) {
            *mchar = 0xA1 + i;
            return 1;
        }
    }
    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_14 = {
    256,
    1,
    FONT_CHARSET_ISO8859_14,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_14_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_14_conv_to_uc32,
    iso8859_14_conv_from_uc32
#endif
};

#endif /* _LATIN8 */

#ifdef _MGCHARSET_LATIN9

/************************* ISO8859-15 Specific Operations **********************/
static int iso8859_15_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-15"))
        return 0;

    if (strstr (name, "LATIN") && strstr (name, "9"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 iso8859_15_conv_to_uc32 (Achar32 chv)
{
    switch (chv) {
        case 0xA4:
            return 0x20AC;  /* EURO SIGN */
        case 0xA6:
            return 0x0160;  /* LATIN CAPITAL LETTER S WITH CARO */
        case 0xA8:
            return 0x0161;  /* LATIN SMALL LETTER S WITH CARON */
        case 0xB4:
            return 0x017D;  /* LATIN CAPITAL LETTER Z WITH CARON */
        case 0xB8:
            return 0x017E;  /* LATIN SMALL LETTER Z WITH CARON */
        case 0xBC:
            return 0x0152;  /* LATIN CAPITAL LIGATURE OE */
        case 0xBD:
            return 0x0153;  /* LATIN SMALL LIGATURE OE */
        case 0xBE:
            return 0x0178;  /* LATIN CAPITAL LETTER Y WITH DIAERESIS */
        default:
            return chv;
    }
}

static int iso8859_15_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    switch (wc) {
        case 0x20AC:  /* EURO SIGN */
            *mchar = 0xA4;
            return 1;
        case 0x0160:  /* LATIN CAPITAL LETTER S WITH CARO */
            *mchar = 0xA6;
            return 1;
        case 0x0161:  /* LATIN SMALL LETTER S WITH CARON */
            *mchar = 0xA8;
            return 1;
        case 0x017D:  /* LATIN CAPITAL LETTER Z WITH CARON */
            *mchar = 0xB4;
            return 1;
        case 0x017E:  /* LATIN SMALL LETTER Z WITH CARON */
            *mchar = 0xB8;
            return 1;
        case 0x0152:  /* LATIN CAPITAL LIGATURE OE */
            *mchar = 0xBC;
            return 1;
        case 0x0153:  /* LATIN SMALL LIGATURE OE */
            *mchar = 0xBD;
            return 1;
        case 0x0178:  /* LATIN CAPITAL LETTER Y WITH DIAERESIS */
            *mchar = 0xBE;
            return 1;
    }

    if (wc <= 0xFF) {
        *mchar = (unsigned char)wc;
        return 1;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_15 = {
    256,
    1,
    FONT_CHARSET_ISO8859_15,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_15_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_15_conv_to_uc32,
    iso8859_15_conv_from_uc32
#endif
};
#endif /* _LATIN9 */

#ifdef _MGCHARSET_LATIN10

/************************* ISO8859-16 Specific Operations **********************/
static int iso8859_16_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-16"))
        return 0;

    if (strstr (name, "LATIN") && strstr (name, "10"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 iso8859_16_conv_to_uc32 (Achar32 chv)
{
    switch (chv) {
        case 0xA1: return 0x0104;
        case 0xA2: return 0x0105;
        case 0xA3: return 0x0141;
        case 0xA4: return 0x20AC;
        case 0xA5: return 0x201E;
        case 0xA6: return 0x0160;
        case 0xA8: return 0x0161;
        case 0xAA: return 0x0218;
        case 0xAC: return 0x0179;
        case 0xAE: return 0x017A;
        case 0xAF: return 0x017B;
        case 0xB2: return 0x010C;
        case 0xB3: return 0x0142;
        case 0xB4: return 0x017D;
        case 0xB5: return 0x201D;
        case 0xB8: return 0x017E;
        case 0xB9: return 0x010D;
        case 0xBA: return 0x0219;
        case 0xBC: return 0x0152;
        case 0xBD: return 0x0153;
        case 0xBE: return 0x0178;
        case 0xBF: return 0x017C;
        case 0xC3: return 0x0102;
        case 0xC5: return 0x0106;
        case 0xD0: return 0x0110;
        case 0xD1: return 0x0143;
        case 0xD5: return 0x0150;
        case 0xD7: return 0x015A;
        case 0xD8: return 0x0170;
        case 0xDD: return 0x0118;
        case 0xDE: return 0x021A;
        case 0xE3: return 0x0103;
        case 0xE5: return 0x0107;
        case 0xF0: return 0x0111;
        case 0xF1: return 0x0144;
        case 0xF5: return 0x0151;
        case 0xF7: return 0x015B;
        case 0xF8: return 0x0171;
        case 0xFD: return 0x0119;
        case 0xFE: return 0x021B;
    }

    return chv;
}

static int iso8859_16_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    switch (wc) {
        case 0x0104: *mchar = 0xA1; return 1;
        case 0x0105: *mchar = 0xA2; return 1;
        case 0x0141: *mchar = 0xA3; return 1;
        case 0x20AC: *mchar = 0xA4; return 1;
        case 0x201E: *mchar = 0xA5; return 1;
        case 0x0160: *mchar = 0xA6; return 1;
        case 0x0161: *mchar = 0xA8; return 1;
        case 0x0218: *mchar = 0xAA; return 1;
        case 0x0179: *mchar = 0xAC; return 1;
        case 0x017A: *mchar = 0xAE; return 1;
        case 0x017B: *mchar = 0xAF; return 1;
        case 0x010C: *mchar = 0xB2; return 1;
        case 0x0142: *mchar = 0xB3; return 1;
        case 0x017D: *mchar = 0xB4; return 1;
        case 0x201D: *mchar = 0xB5; return 1;
        case 0x017E: *mchar = 0xB8; return 1;
        case 0x010D: *mchar = 0xB9; return 1;
        case 0x0219: *mchar = 0xBA; return 1;
        case 0x0152: *mchar = 0xBC; return 1;
        case 0x0153: *mchar = 0xBD; return 1;
        case 0x0178: *mchar = 0xBE; return 1;
        case 0x017C: *mchar = 0xBF; return 1;
        case 0x0102: *mchar = 0xC3; return 1;
        case 0x0106: *mchar = 0xC5; return 1;
        case 0x0110: *mchar = 0xD0; return 1;
        case 0x0143: *mchar = 0xD1; return 1;
        case 0x0150: *mchar = 0xD5; return 1;
        case 0x015A: *mchar = 0xD7; return 1;
        case 0x0170: *mchar = 0xD8; return 1;
        case 0x0118: *mchar = 0xDD; return 1;
        case 0x021A: *mchar = 0xDE; return 1;
        case 0x0103: *mchar = 0xE3; return 1;
        case 0x0107: *mchar = 0xE5; return 1;
        case 0x0111: *mchar = 0xF0; return 1;
        case 0x0144: *mchar = 0xF1; return 1;
        case 0x0151: *mchar = 0xF5; return 1;
        case 0x015B: *mchar = 0xF7; return 1;
        case 0x0171: *mchar = 0xF8; return 1;
        case 0x0119: *mchar = 0xFD; return 1;
        case 0x021B: *mchar = 0xFE; return 1;
    }

    if (wc <= 0xFF) {
        *mchar = (unsigned char)wc;
        return 1;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_iso8859_16 = {
    256,
    1,
    FONT_CHARSET_ISO8859_16,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    iso8859_16_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    iso8859_16_conv_to_uc32,
    iso8859_16_conv_from_uc32
#endif
};

#endif /* _LATIN10 */

/*************** Common Operations for double bytes charsets ******************/
#if defined(_MGCHARSET_GB) | defined(_MGCHARSET_GBK) | defined(_MGCHARSET_GB18030) \
        | defined(_MGCHARSET_BIG5) | defined(_MGCHARSET_EUCKR) | defined(_MGCHARSET_EUCJP) \
        | defined(_MGCHARSET_SHIFTJIS) \
        | defined(_MGCHARSET_UNICODE)

static unsigned int mb_char_type (Achar32 chv)
{
    /* TODO: get the subtype of the char */
    return ACHAR_BASIC_GENERIC;
}

static int db_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    assert ((mstrlen % 2) == 0);
    return mstrlen >> 1;
}

static const unsigned char* db_get_next_word (const unsigned char* mstr,
                int mstrlen, WORDINFO* word_info)
{
    assert ((mstrlen % 2) == 0);

    word_info->len = 0;
    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    if (mstrlen < 2) return NULL;

    word_info->len = 2;

    return mstr + 2;
}

#endif

#ifdef _MGCHARSET_GB
/************************* GB2312 Specific Operations ************************/
#define IS_GB2312_CHAR(ch1, ch2) \
        if (((ch1 >= 0xA1 && ch1 <= 0xA9) || (ch1 >= 0xB0 && ch1 <= 0xF7)) \
                        && ch2 >= 0xA1 && ch2 <= 0xFE)

static int gb2312_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    IS_GB2312_CHAR (ch1, ch2)
        return 2;

    return 0;
}

static Achar32 gb2312_0_get_char_value (const unsigned char* pre_mchar,
                int pre_len, const unsigned char* cur_mchar, int cur_len)
{
    int area = cur_mchar [0] - 0xA1;

    if (area < 9) {
        return (area * 94 + cur_mchar [1] - 0xA1);
    }
    else if (area >= 15)
        return ((area - 6)* 94 + cur_mchar [1] - 0xA1);

    return 0;
}

static int gb2312_0_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "GB") && strstr (name, "2312"))
        return 0;

    if (strstr (name, "EUC") && strstr (name, "CN"))
        return 0;

    return 1;
}

static int gb2312_0_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        IS_GB2312_CHAR (ch1, ch2)
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int gb2312_0_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        IS_GB2312_CHAR (ch1, ch2)
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 gb2312_0_conv_to_uc32 (Achar32 chv)
{
    return (Uchar32)__mg_gbunicode_map [REAL_ACHAR(chv)];
}

const unsigned char* __mg_map_uc16_to_gb (unsigned short uc16);

static int gb2312_0_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    const unsigned char* got;

    if (wc > 0xFFFF)
        return 0;

    got = __mg_map_uc16_to_gb ((unsigned short)wc);

    if (got) {
        mchar [0] = got [0];
        mchar [1] = got [1];
        return 2;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_gb2312_0 = {
    (0xFF-0xA1)*(72 + 9),
    2,
    FONT_CHARSET_GB2312_0,
    0,
    gb2312_0_len_first_char,
    gb2312_0_get_char_value,
    NULL,
    mb_char_type,
    db_nr_chars_in_str,
    gb2312_0_is_this_charset,
    gb2312_0_len_first_substr,
    db_get_next_word,
    gb2312_0_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    gb2312_0_conv_to_uc32,
    gb2312_0_conv_from_uc32
#endif
};
#endif /* _GB */

#ifdef _MGCHARSET_GBK
/************************* GBK Specific Operations ************************/

#define IS_GBK_CHAR(ch1, ch2) \
    if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F)

static int gbk_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    IS_GBK_CHAR(ch1, ch2)
        return 2;

    return 0;
}

static Achar32 gbk_get_char_value (const unsigned char* pre_mchar,  int pre_len,
                const unsigned char* cur_mchar, int cur_len)
{
    if (cur_mchar [1] > 0x7F)
        return ((cur_mchar [0] - 0x81) * 190 + cur_mchar [1] - 0x41);
    else
        return ((cur_mchar [0] - 0x81) * 190 + cur_mchar [1] - 0x40);
}

static int gbk_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "GBK"))
        return 0;

    if (strstr (name, "GB") && strstr (name, "2312"))
        return 0;

    if (strstr (name, "EUC") && strstr (name, "CN"))
        return 0;

    return 1;
}

static int gbk_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        IS_GBK_CHAR(ch1, ch2)
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int gbk_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        IS_GBK_CHAR(ch1, ch2)
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 gbk_conv_to_uc32 (Achar32 chv)
{
    return (Uchar32)__mg_gbkunicode_map[REAL_ACHAR(chv)];
}

const unsigned char* __mg_map_uc16_to_gbk (unsigned short uc16);
static int gbk_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    const unsigned char* got;

    if (wc > 0xFFFF)
        return 0;

    got = __mg_map_uc16_to_gbk ((unsigned short)wc);

    if (got) {
        mchar [0] = got [0];
        mchar [1] = got [1];
        return 2;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_gbk = {
    (0xFE - 0x80) * 190,
    2,
    FONT_CHARSET_GBK,
    0,
    gbk_len_first_char,
    gbk_get_char_value,
    NULL,
    mb_char_type,
    db_nr_chars_in_str,
    gbk_is_this_charset,
    gbk_len_first_substr,
    db_get_next_word,
    gbk_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    gbk_conv_to_uc32,
    gbk_conv_from_uc32,
#endif
};

#endif /* _GBK */

#ifdef _MGCHARSET_GB18030

/************************* GBK Specific Operations ************************/
static int gb18030_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;
    unsigned char ch3;
    unsigned char ch4;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F)
        return 2;

    if (len < 4) return 0;

    ch3 = mstr [2];
    ch4 = mstr [3];
    if (ch2 >= 0x30 && ch2 <= 0x39 && ch4 >= 0x30 && ch4 <= 0x39
            && ch1 >= 0x81 && ch1 <= 0xFE && ch3 >= 0x81 && ch3 <= 0xFE)
        return 4;

    return 0;
}

static Achar32 gb18030_0_get_char_value (const unsigned char* pre_mchar,
                int pre_len, const unsigned char* cur_mchar, int cur_len)
{
    unsigned char ch1;
    unsigned char ch2;
    unsigned char ch3;
    unsigned char ch4;

    ch1 = cur_mchar [0];
    ch2 = cur_mchar [1];
    if (ch2 >= 0x40)
        return ((ch1 - 0x81) * 192 + (ch2 - 0x40));

    ch3 = cur_mchar [2];
    ch4 = cur_mchar [3];
    return ((126 * 192) +
            ((ch1 - 0x81) * 12600 + (ch2 - 0x30) * 1260 +
             (ch3 - 0x81) * 10 + (ch4 - 0x30)));
}

static int gb18030_0_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "GB") && strstr (name, "18030"))
        return 0;

    if (strstr (name, "GBK"))
        return 0;

    if (strstr (name, "GB") && strstr (name, "2312"))
        return 0;

    return 1;
}

static int gb18030_0_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1, ch2, ch3, ch4;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F) {
            sub_len += 2;
            left -= 2;
        }
        else if (left >= 4) {

            ch3 = mstr [i + 2];
            ch4 = mstr [i + 3];
            if (ch2 >= 0x30 && ch2 <= 0x39 && ch4 >= 0x30 && ch4 <= 0x39
                    && ch1 >= 0x81 && ch1 <= 0xFE && ch3 >= 0x81 && ch3 <= 0xFE) {
                sub_len += 4;
                left -= 4;
                i += 2;
            }
        }
        else
            return sub_len;
    }

    return sub_len;
}

static int gb18030_0_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1, ch2, ch3, ch4;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        if (ch1 >= 0x81 && ch1 <= 0xFE
                && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F)
            return i;

        if (left < 4) return -1;

        ch3 = mstr [i + 2];
        ch4 = mstr [i + 3];
        if (ch2 >= 0x30 && ch2 <= 0x39 && ch4 >= 0x30 && ch4 <= 0x39
                && ch1 >= 0x81 && ch1 <= 0xFE && ch3 >= 0x81 && ch3 <= 0xFE)
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

static int gb18030_0_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    int i, left;
    int n;

    assert ((mstrlen % 2) == 0);

    n = 0;
    i = 0;
    left = mstrlen;
    while (left) {
        if (left >= 2 && mstr [i + 1] >= 0x40) {
            left -= 2;
            i += 2;
        }
        else if (left >= 4) {
            left -= 4;
            i += 4;
        }
        else
            break;

        n++;
    }

    return n;

}

static const unsigned char* gb18030_0_get_next_word (const unsigned char* mstr,
                int mstrlen, WORDINFO* word_info)
{
    assert ((mstrlen % 2) == 0);

    word_info->len = 0;
    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    if (mstrlen < 2) return NULL;

    if (mstrlen >= 2 && mstr [1] >= 0x40) {
        word_info->len = 2;
    }
    else if (mstrlen >= 4) {
        word_info->len = 4;
    }

    return mstr + word_info->len;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 gb18030_0_conv_to_uc32 (Achar32 chv)
{
    chv = REAL_ACHAR(chv);

    /* from 0x90308130 to 0xE3329A35 */
    if (chv > 63611) {
        int m1, n1, m2, n2, m3, n3;

        m1 = (chv - 24192) / 12600;
        n1 = (chv - 24192) % 12600;
        m2 = n1 / 1260;
        n2 = n1 % 1260;
        m3 = n2 / 10;
        n3 = n2 % 10;

        return ((m1 + 129 - 144) * 12600) +
                (m2 * 1260) + (m3 * 10) + n3 + 65536;
    }

    return (Uchar32)__mg_gb18030_0_unicode_map [chv];
}

int __mg_map_ucs_to_gb18030 (int wc, unsigned char* mchar);
static int gb18030_0_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    if (wc > 0x10FFFF)
        return 0;
    else if (wc >= 0x10000 && wc <= 0x10FFFF) {
        /* from 0x10000 - 0x10FFFF */
        Uchar32 m1, n1, m2, n2, m3, n3;
        m1 = (wc - 65536) / 12600;
        n1 = (wc - 65536) % 12600;
        m2 = n1 / 1260;
        n2 = n1 % 1260;
        m3 = n2 / 10;
        n3 = n2 % 10;

        mchar [0] = m1 + 144;
        mchar [1] = m2 + 48;
        mchar [2] = m3 + 129;
        mchar [3] = n3 + 48;
        return 4;
    }

    return __mg_map_ucs_to_gb18030 (wc, mchar);
}

#endif

static CHARSETOPS CharsetOps_gb18030_0 = {
    1587600 + 23940,
    4,
    FONT_CHARSET_GB18030_0,
    0,
    gb18030_0_len_first_char,
    gb18030_0_get_char_value,
    NULL,
    mb_char_type,
    gb18030_0_nr_chars_in_str,
    gb18030_0_is_this_charset,
    gb18030_0_len_first_substr,
    gb18030_0_get_next_word,
    gb18030_0_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    gb18030_0_conv_to_uc32,
    gb18030_0_conv_from_uc32
#endif
};

#endif /* _GB18030 */

#ifdef _MGCHARSET_BIG5

/************************** BIG5 Specific Operations ************************/
static int big5_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 >= 0xA1 && ch1 <= 0xFE &&
            ((ch2 >=0x40 && ch2 <= 0x7E) || (ch2 >= 0xA1 && ch2 <= 0xFE)))
        return 2;

    return 0;
}

static Achar32 big5_get_char_value (const unsigned char* pre_mchar,  int pre_len,
                const unsigned char* cur_mchar, int cur_len)
{
    if (cur_mchar [1] & 0x80)
        return (cur_mchar [0] - 0xA1) * 94 + cur_mchar [1] - 0xA1;
    else
        return 94 * 94 + (cur_mchar [0] - 0xa1) * 63 + (cur_mchar [1] - 0x40);
}

static int big5_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "BIG5"))
        return 0;

    return 1;
}

static int big5_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE &&
                ((ch2 >=0x40 && ch2 <= 0x7E) || (ch2 >= 0xA1 && ch2 <= 0xFE)))
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int big5_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE &&
                ((ch2 >=0x40 && ch2 <= 0x7E) || (ch2 >= 0xA1 && ch2 <= 0xFE)))
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 big5_conv_to_uc32 (Achar32 chv)
{
    unsigned short ucs_code = __mg_big5_unicode_map[REAL_ACHAR(chv)];

    if (ucs_code == 0)
        return '?';

    return ucs_code;
}

const unsigned char* __mg_map_uc16_to_big5 (unsigned short uc16);
static int big5_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    const unsigned char* got;

    if (wc > 0xFFFF)
        return 0;

    got = __mg_map_uc16_to_big5 ((unsigned short)wc);

    if (got) {
        mchar [0] = got [0];
        mchar [1] = got [1];
        return 2;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_big5 = {
    14758,
    2,
    FONT_CHARSET_BIG5,
    0,
    big5_len_first_char,
    big5_get_char_value,
    NULL,
    mb_char_type,
    db_nr_chars_in_str,
    big5_is_this_charset,
    big5_len_first_substr,
    db_get_next_word,
    big5_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    big5_conv_to_uc32,
    big5_conv_from_uc32,
#endif
};

#endif /* _BIG5 */

#ifdef _MGCHARSET_EUCKR

/************************* EUCKR Specific Operations ************************/
static int ksc5601_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
        return 2;

    return 0;
}

static Achar32 ksc5601_0_get_char_value (const unsigned char* pre_mchar,
                int pre_len, const unsigned char* cur_mchar, int cur_len)
{
#if 1
    return ((cur_mchar [0] - 0xA1) * 94 + cur_mchar [1] - 0xA1);
#else
    if(cur_mchar [0] > 0xAD)
        return ((cur_mchar [0] - 0xA4) * 94 + cur_mchar [1] - 0xA1 - 0x8E);
    else
        return ((cur_mchar [0] - 0xA1) * 94 + cur_mchar [1] - 0xA1 - 0x8E);
#endif
}

static int ksc5601_0_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if ((strstr (name, "KSC5601") && strstr (name, "-0")) ||
            (strstr (name, "EUC") && strstr (name, "KR")))
        return 0;

    return 1;
}

static int ksc5601_0_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int ksc5601_0_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 ksc5601_0_conv_to_uc32 (Achar32 chv)
{
    unsigned short ucs_code = __mg_ksc5601_0_unicode_map[REAL_ACHAR(chv)];

    if (ucs_code == 0)
        return '?';

    return ucs_code;
}

static int ksc5601_0_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    /* TODO */
    return 0;
}
#endif

static CHARSETOPS CharsetOps_ksc5601_0 = {
    8836,
    2,
    FONT_CHARSET_EUCKR,
    0,
    ksc5601_0_len_first_char,
    ksc5601_0_get_char_value,
    NULL,
    mb_char_type,
    db_nr_chars_in_str,
    ksc5601_0_is_this_charset,
    ksc5601_0_len_first_substr,
    db_get_next_word,
    ksc5601_0_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    ksc5601_0_conv_to_uc32,
    ksc5601_0_conv_from_uc32,
#endif
};
/************************* End of EUCKR *************************************/

#endif  /* _EUCKR */

#ifdef _MGCHARSET_EUCJP

/************************* EUCJP Specific Operations ************************/
static int jisx0201_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 == 0x8E && ch2 >= 0xA1 && ch2 <= 0xDF)
        return 2;
    else
        return 1;

    return 0;
}

static Achar32 jisx0201_0_get_char_value (const unsigned char* pre_mchar,
                int pre_len, const unsigned char* cur_mchar, int cur_len)
{
    if (cur_mchar [0] == 0x8E)
        return cur_mchar [1];
    else
        return cur_mchar [0];
}

static int jisx0201_0_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int left;
    int nr_chars = 0;

    left = mstrlen;
    while (left > 0) {

        ch1 = mstr [0];
        if (ch1 == '\0') return nr_chars;

        ch2 = mstr [1];
        if (ch1 == 0x8E && ch2 >= 0xA1 && ch2 <= 0xDF) {
            nr_chars ++;
            left -= 2;
            mstr += 2;
        }
        else {
            nr_chars ++;
            left -= 1;
            mstr += 1;
        }
    }

    return nr_chars;
}

static int jisx0201_0_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "JISX0201") && strstr (name, "-0"))
        return 0;

    return 1;
}


#ifdef _MGCHARSET_UNICODE
static Uchar32 jisx0201_0_conv_to_uc32 (Achar32 chv)
{
    if (chv >= 0xA1 && chv <= 0xDF)
        return 0xFF61 + chv - 0xA1;
    else if (chv == 0x5C)
        return 0x00A5;
    else if (chv == 0x80)
        return 0x005C;

    return chv;

}

static int jisx0201_0_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    /* TODO */
    return 0;
}
#endif

static CHARSETOPS CharsetOps_jisx0201_0 = {
    190,
    1,
    FONT_CHARSET_JISX0201_0,
    0,
    jisx0201_0_len_first_char,
    jisx0201_0_get_char_value,
    NULL,
    sb_char_type,
    jisx0201_0_nr_chars_in_str,
    jisx0201_0_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    jisx0201_0_conv_to_uc32,
    jisx0201_0_conv_from_uc32,
#endif
};

static int jisx0208_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
        return 2;

    return 0;
}

static Achar32 jisx0208_0_get_char_value (const unsigned char* pre_mchar,
                int pre_len, const unsigned char* cur_mchar, int cur_len)
{
#if 1
    return ((cur_mchar [0] - 0xA1) * 94 + cur_mchar [1] - 0xA1);
#else
    if(cur_mchar [0] > 0xAA)
        return ((cur_mchar [0] - 0xA6 - 0x02) * 94 + cur_mchar [1] - 0xC1 - 0xC4);
    else
        return ((cur_mchar [0] - 0xA1 - 0x02) * 83 + cur_mchar [1] - 0xA1 + 0x7E);
#endif
}

static int jisx0208_0_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if ((strstr (name, "JISX0208") && strstr (name, "-0")) ||
            (strstr (name, "EUC") && strstr (name, "JP")))
        return 0;

    return 1;
}

static int jisx0208_0_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int jisx0208_0_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 jisx0208_0_conv_to_uc32 (Achar32 chv)
{
    unsigned short ucs_code;

    ucs_code = __mg_jisx0208_0_unicode_map[REAL_ACHAR(chv)];

    if (ucs_code == 0)
        return '?';

    return ucs_code;
}

static int jisx0208_0_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    /* TODO */
    return 0;
}
#endif

static CHARSETOPS CharsetOps_jisx0208_0 = {
    8836,
    2,
    FONT_CHARSET_JISX0208_0,
    0,
    jisx0208_0_len_first_char,
    jisx0208_0_get_char_value,
    NULL,
    mb_char_type,
    db_nr_chars_in_str,
    jisx0208_0_is_this_charset,
    jisx0208_0_len_first_substr,
    db_get_next_word,
    jisx0208_0_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    jisx0208_0_conv_to_uc32,
    jisx0208_0_conv_from_uc32,
#endif
};
/************************* End of EUCJP *************************************/

#endif /* _EUCJP */

#ifdef _MGCHARSET_SHIFTJIS

/************************* SHIFTJIS Specific Operations ************************/
static int jisx0201_1_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "JISX0201") && strstr (name, "-1"))
        return 0;

    return 1;
}


#ifdef _MGCHARSET_UNICODE
static Uchar32 jisx0201_1_conv_to_uc32 (Achar32 chv)
{
    if (chv >= 0xA1 && chv <= 0xDF)
        return 0xFF61 + chv - 0xA1;
    else if (chv == 0x5C)
        return 0x00A5;
    else if (chv == 0x80)
        return 0x005C;

    return chv;
}

static int jisx0201_1_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    if (wc < 0xA1) {
        *mchar = (unsigned char)wc;
        return 1;
    }

    switch (wc) {
        case 0x00A5:
            *mchar = 0x5C;
            return 1;
        case 0x005C:
            *mchar = 0x80;
            return 1;
    }

    if (wc >= 0xFF61 && wc <= (0xFF61 + 0xDF - 0xA1)) {
        *mchar = (unsigned char) (wc - 0xFF61 + 0xA1);
        return 1;
    }

    return 0;
}
#endif

static CHARSETOPS CharsetOps_jisx0201_1 = {
    190,
    1,
    FONT_CHARSET_JISX0201_1,
    0,
    sb_len_first_char,
    sb_get_char_value,
    NULL,
    sb_char_type,
    sb_nr_chars_in_str,
    jisx0201_1_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    jisx0201_1_conv_to_uc32,
    jisx0201_1_conv_from_uc32,
#endif
};

static int jisx0208_1_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (((ch1 >= 0x81 && ch1 <= 0x9F) || (ch1 >= 0xE0 && ch1 <= 0xEF)) &&
            ((ch2 >= 0x40 && ch2 <= 0x7E) || (ch2 >= 0x80 && ch2 <= 0xFC)))
        return 2;

    return 0;
}

static Achar32 jisx0208_1_get_char_value (const unsigned char* pre_mchar,
                int pre_len, const unsigned char* cur_mchar, int cur_len)
{
    unsigned char ch1 = cur_mchar [0];
    unsigned char ch2 = cur_mchar [1];

#if 0
    if (ch1 >= 0x81 && ch1 <= 0x9F) {
        if (ch2 >= 0x40 && ch2 <= 0x7E) {
            return ((ch1 - 0x81) * 0x3F + ch2 - 0x40);
        }
        else {
            return (ch1 - 0x81) * 0x7D + ch2 - 0x80 + (0x1F * 0x3F);
        }
    }
    else {
        if (ch2 >= 0x40 && ch2 <= 0x7E) {
            return ((ch1 - 0xE0) * 0x3F + ch2 - 0x40) + 0x1F * (0x3F + 0x7D);
        }
        else {
            return ((ch1 - 0xE0) * 0x7D + ch2 - 0x80) + 0x1F * (0x3F + 0x7D) + 0x10 * 0x3F;
        }
    }
#else
    int adjust = ch2 < 159;
    int rowOffset = ch1 < 160 ? 112 : 176;
    int cellOffset = adjust ? (ch2 > 127 ? 32 : 31) : 126;

    ch1 = ((ch1 - rowOffset) << 1) - adjust;
    ch2 -= cellOffset;

    ch1 += 128;
    ch2 += 128;

    return ((ch1 - 0xA1) * 94 + ch2 - 0xA1);
#endif
}

static int jisx0208_1_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if ((strstr (name, "JISX0208") && strstr (name, "-1")) ||
            (strstr (name, "SHIFT") && strstr (name, "JIS")))
        return 0;

    return 1;
}

static int jisx0208_1_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        if (((ch1 >= 0x81 && ch1 <= 0x9F) || (ch1 >= 0xE0 && ch1 <= 0xEF)) &&
                ((ch2 >= 0x40 && ch2 <= 0x7E) || (ch2 >= 0x80 && ch2 <= 0xFC)))
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int jisx0208_1_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        if (((ch1 >= 0x81 && ch1 <= 0x9F) || (ch1 >= 0xE0 && ch1 <= 0xEF)) &&
                ((ch2 >= 0x40 && ch2 <= 0x7E) || (ch2 >= 0x80 && ch2 <= 0xFC)))
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _MGCHARSET_UNICODE
static Uchar32 jisx0208_1_conv_to_uc32 (Achar32 chv)
{
    unsigned short ucs_code;

    ucs_code  = __mg_jisx0208_1_unicode_map[REAL_ACHAR(chv)];

    if (ucs_code == 0)
        return '?';

    return ucs_code;
}

static int jisx0208_1_conv_from_uc32 (Uchar32 wc, unsigned char *mchar)
{
    /* TODO */
    return 0;
}
#endif

static CHARSETOPS CharsetOps_jisx0208_1 = {
    8836,
    2,
    FONT_CHARSET_JISX0208_1,
    0,
    jisx0208_1_len_first_char,
    jisx0208_1_get_char_value,
    NULL,
    mb_char_type,
    db_nr_chars_in_str,
    jisx0208_1_is_this_charset,
    jisx0208_1_len_first_substr,
    db_get_next_word,
    jisx0208_1_pos_first_char,
    NULL,
    NULL,
#ifdef _MGCHARSET_UNICODE
    jisx0208_1_conv_to_uc32,
    jisx0208_1_conv_from_uc32
#endif
};
/************************* End of SHIFTJIS ************************************/

#endif /* _SHIFTJIS */

#ifdef _MGCHARSET_UNICODE
extern CHARSETOPS __mg_CharsetOps_utf8;
extern CHARSETOPS __mg_CharsetOps_utf16le;
extern CHARSETOPS __mg_CharsetOps_utf16be;
#endif /*_MGCHARSET_UNICODE */

static CHARSETOPS* Charsets [] =
{
    &CharsetOps_ascii,
#ifdef _MGCHARSET_LATIN10
    &CharsetOps_iso8859_16,
#endif
#ifdef _MGCHARSET_LATIN9
    &CharsetOps_iso8859_15,
#endif
#ifdef _MGCHARSET_LATIN8
    &CharsetOps_iso8859_14,
#endif
#ifdef _MGCHARSET_LATIN7
    &CharsetOps_iso8859_13,
#endif
#ifdef _MGCHARSET_THAI
    &CharsetOps_iso8859_11,
#endif
#ifdef _MGCHARSET_LATIN6
    &CharsetOps_iso8859_10,
#endif
#ifdef _MGCHARSET_LATIN5
    &CharsetOps_iso8859_9,
#endif
#ifdef _MGCHARSET_HEBREW
    &CharsetOps_iso8859_8,
#endif
#ifdef _MGCHARSET_GREEK
    &CharsetOps_iso8859_7,
#endif
#ifdef _MGCHARSET_ARABIC
    &CharsetOps_iso8859_6,
#endif
#ifdef _MGCHARSET_CYRILLIC
    &CharsetOps_iso8859_5,
#endif
#ifdef _MGCHARSET_LATIN4
    &CharsetOps_iso8859_4,
#endif
#ifdef _MGCHARSET_LATIN3
    &CharsetOps_iso8859_3,
#endif
#ifdef _MGCHARSET_LATIN2
    &CharsetOps_iso8859_2,
#endif
    &CharsetOps_iso8859_1,
#ifdef _MGCHARSET_GB18030
    &CharsetOps_gb18030_0,
#endif
#ifdef _MGCHARSET_GBK
    &CharsetOps_gbk,
#endif
#ifdef _MGCHARSET_GB
    &CharsetOps_gb2312_0,
#endif
#ifdef _MGCHARSET_BIG5
    &CharsetOps_big5,
#endif
#ifdef _MGCHARSET_EUCKR
    &CharsetOps_ksc5601_0,
#endif
#ifdef _MGCHARSET_EUCJP
    &CharsetOps_jisx0201_0,
    &CharsetOps_jisx0208_0,
#endif
#ifdef _MGCHARSET_SHIFTJIS
    &CharsetOps_jisx0201_1,
    &CharsetOps_jisx0208_1,
#endif
#ifdef _MGCHARSET_UNICODE
    &__mg_CharsetOps_utf8,
    &__mg_CharsetOps_utf16le,
    &__mg_CharsetOps_utf16be,
#endif
};

#define NR_CHARSETS     (sizeof(Charsets)/sizeof(CHARSETOPS*))

CHARSETOPS* GetCharsetOps (const char* charset)
{
    int i;

    for (i = 0; i < NR_CHARSETS; i++) {
        if ((*Charsets [i]->is_this_charset)
                        ((const unsigned char*)charset) == 0)
            return Charsets [i];
    }

    return NULL;
}

CHARSETOPS* GetCharsetOpsEx (const char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    for (i = 0; i < NR_CHARSETS; i++) {
        if (strcmp (Charsets [i]->name, name) == 0)
            return Charsets [i];
    }

    return NULL;
}

BOOL IsCompatibleCharset (const char* charset, CHARSETOPS* ops)
{
    int i;

    for (i = 0; i < NR_CHARSETS; i++) {
        if ((*Charsets [i]->is_this_charset)
                        ((const unsigned char*)charset) == 0)
            if (Charsets [i] == ops)
                return TRUE;
    }

    return FALSE;
}

