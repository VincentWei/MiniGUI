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

#ifdef _MGCHARSET_UNICODE

#include "minigui.h"
#include "gdi.h"
#include "devfont.h"
#include "charset.h"
#include "bidi.h"

/************************* UTF-8 Specific Operations ************************/
static int utf8_len_first_char (const unsigned char* mstr, int len)
{
    int t, c = *((unsigned char *)(mstr++));

    if (c & 0x80) {
        int n = 1, ch_len = 0;
        while (c & (0x80 >> n))
            n++;

        if (n > len)
            return 0;

        ch_len = n;
        while (--n > 0) {
            t = *((unsigned char *)(mstr++));

            if ((!(t & 0x80)) || (t & 0x40))
                return 0;
        }

        return ch_len;
    }

    /* for ascii character */
    return 1;
}

static Achar32 utf8_get_char_value (const unsigned char* pre_mchar, int pre_len,
        const unsigned char* cur_mchar, int cur_len)
{
    Uchar32 wc = *((unsigned char *)(cur_mchar++));
    int n, t;

    if (wc & 0x80) {
        n = 1;
        while (wc & (0x80 >> n))
            n++;

        wc &= (1 << (8-n)) - 1;
        while (--n > 0) {
            t = *((unsigned char *)(cur_mchar++));

            wc = (wc << 6) | (t & 0x3F);
        }
    }

    return wc;
}

#include "unicode-tables.h"
#include "unicode-break-tables.h"

#define ATTR_TABLE(Page) (((Page) <= UCHAR_LAST_PAGE_PART1) \
                          ? attr_table_part1[Page] \
                          : attr_table_part2[(Page) - 0xe00])

#define ATTTABLE(Page, Char) \
  ((ATTR_TABLE(Page) == UCHAR_MAX_TABLE_INDEX) ? 0 : (attr_data[ATTR_TABLE(Page)][Char]))

#define TTYPE_PART1(Page, Char) \
  ((type_table_part1[Page] >= UCHAR_MAX_TABLE_INDEX) \
   ? (type_table_part1[Page] - UCHAR_MAX_TABLE_INDEX) \
   : (type_data[type_table_part1[Page]][Char]))

#define TTYPE_PART2(Page, Char) \
  ((type_table_part2[Page] >= UCHAR_MAX_TABLE_INDEX) \
   ? (type_table_part2[Page] - UCHAR_MAX_TABLE_INDEX) \
   : (type_data[type_table_part2[Page]][Char]))

#define TYPE(Char) \
  (((Char) <= UCHAR_LAST_CHAR_PART1) \
   ? TTYPE_PART1 ((Char) >> 8, (Char) & 0xff) \
   : (((Char) >= 0xe0000 && (Char) <= UCHAR_LAST_CHAR) \
      ? TTYPE_PART2 (((Char) - 0xe0000) >> 8, (Char) & 0xff) \
      : UCHAR_CATEGORY_UNASSIGNED))

#define IS(Type, Class) (((unsigned int)1 << (Type)) & (Class))
#define OR(Type, Rest)  (((unsigned int)1 << (Type)) | (Rest))

#define ISALPHA(Type)   IS ((Type),             \
                OR (UCHAR_CATEGORY_LOWERCASE_LETTER, \
                OR (UCHAR_CATEGORY_UPPERCASE_LETTER, \
                OR (UCHAR_CATEGORY_TITLECASE_LETTER, \
                OR (UCHAR_CATEGORY_MODIFIER_LETTER,  \
                OR (UCHAR_CATEGORY_OTHER_LETTER,     0))))))

#define ISALDIGIT(Type) IS ((Type),             \
                OR (UCHAR_CATEGORY_DECIMAL_NUMBER,   \
                OR (UCHAR_CATEGORY_LETTER_NUMBER,    \
                OR (UCHAR_CATEGORY_OTHER_NUMBER,     \
                OR (UCHAR_CATEGORY_LOWERCASE_LETTER, \
                OR (UCHAR_CATEGORY_UPPERCASE_LETTER, \
                OR (UCHAR_CATEGORY_TITLECASE_LETTER, \
                OR (UCHAR_CATEGORY_MODIFIER_LETTER,  \
                OR (UCHAR_CATEGORY_OTHER_LETTER,     0)))))))))

#define ISMARK(Type)    IS ((Type),             \
                OR (UCHAR_CATEGORY_NON_SPACING_MARK, \
                OR (UCHAR_CATEGORY_SPACING_MARK, \
                OR (UCHAR_CATEGORY_ENCLOSING_MARK,   0))))

#define ISZEROWIDTHTYPE(Type)   IS ((Type),         \
                OR (UCHAR_CATEGORY_NON_SPACING_MARK, \
                OR (UCHAR_CATEGORY_ENCLOSING_MARK,   \
                OR (UCHAR_CATEGORY_FORMAT,       0))))

#define TPROP_PART1(Page, Char) \
  ((break_property_table_part1[Page] >= UCHAR_MAX_TABLE_INDEX) \
   ? (break_property_table_part1[Page] - UCHAR_MAX_TABLE_INDEX) \
   : (break_property_data[break_property_table_part1[Page]][Char]))

#define TPROP_PART2(Page, Char) \
  ((break_property_table_part2[Page] >= UCHAR_MAX_TABLE_INDEX) \
   ? (break_property_table_part2[Page] - UCHAR_MAX_TABLE_INDEX) \
   : (break_property_data[break_property_table_part2[Page]][Char]))

#define PROP(Char) \
  (((Char) <= UCHAR_LAST_CHAR_PART1) \
   ? TPROP_PART1 ((Char) >> 8, (Char) & 0xff) \
   : (((Char) >= 0xe0000 && (Char) <= UCHAR_LAST_CHAR) \
      ? TPROP_PART2 (((Char) - 0xe0000) >> 8, (Char) & 0xff) \
      : UCHAR_BREAK_UNKNOWN))

extern unsigned int __mg_sb_char_type(Achar32);

static unsigned int unicode_char_type (Achar32 chv)
{
    unsigned int mchar_type = ACHAR_BASIC_UNKNOWN;
    unsigned int basic_type = 0, break_type = 0;

    chv = REAL_ACHAR(chv);
    basic_type = TYPE(chv);
    break_type = PROP(chv);

    if (chv < 0x80)
        mchar_type = __mg_sb_char_type(chv);
    else if (break_type == UCHAR_BREAK_CARRIAGE_RETURN)
        mchar_type = ACHAR_BASIC_CR;
    else if (break_type == UCHAR_BREAK_LINE_FEED)
        mchar_type = ACHAR_BASIC_LF;
    else if (break_type == UCHAR_BREAK_NEXT_LINE)
        mchar_type = ACHAR_BASIC_LF;
    else if (break_type == UCHAR_BREAK_SPACE)
        mchar_type = ACHAR_BASIC_SPACE;
    else if (chv != 0x00AD && ISZEROWIDTHTYPE (basic_type))
        mchar_type = ACHAR_BASIC_ZEROWIDTH;
    else if ((chv >= 0x1160 && chv < 0x1200)
            || chv == 0x200B)
        mchar_type = ACHAR_BASIC_ZEROWIDTH;

    return (break_type << 24) | (basic_type << 16) | mchar_type;
}

#define _USE_UNIBIDI

#ifdef _USE_UNIBIDI

enum _BidiCharTypeLinearEnum {
#define _UNIBIDI_ADD_TYPE(TYPE,SYMBOL) TYPE,
#include "unicode-bidi-types-list.inc"
#undef _UNIBIDI_ADD_TYPE
    _UNIBIDI_NUM_TYPES
};

#include "unicode-bidi-type-table.inc"

/* Map _BidiCharTypeLinearEnum to BidiType. */
static const BidiType linear_enum_to_bidi_type[] = {
#define _UNIBIDI_ADD_TYPE(TYPE,SYMBOL) BIDI_TYPE_##TYPE,
#include "unicode-bidi-types-list.inc"
#undef _UNIBIDI_ADD_TYPE
};

static BidiType unicode_bidi_char_type(Achar32 ch)
{
    ch = REAL_ACHAR(ch);
    return linear_enum_to_bidi_type[UNIBIDI_GET_BIDI_TYPE(ch)];
}

BidiType GUIAPI UCharGetBidiType(Uchar32 uc)
{
    return linear_enum_to_bidi_type[UNIBIDI_GET_BIDI_TYPE(uc)];
}

void GUIAPI UStrGetBidiTypes(const Uchar32 *str, int len, BidiType *btypes)
{
    register int i = len;
    for (; i; i--) {
        *btypes++ = linear_enum_to_bidi_type[UNIBIDI_GET_BIDI_TYPE(*str)];
        str++;
    }
}

#include "unicode-bidi-brackets-table.inc"
#include "unicode-bidi-brackets-type-table.inc"

#define UNIBIDI_TYPE_BRACKET_OPEN 2

BidiBracketType UCharGetBracketType(Uchar32 ch)
{
    BidiBracketType bracket_type;
    register Uint8 char_type = UNIBIDI_GET_BRACKET_TYPE (ch);

    /* The bracket type from the table may be:
       0 - Not a bracket
       1 - a bracket
       2 - closing.

       This will be recodeded into the BidiBracketType as having a
       bracket_id = 0 if the character is not a bracket.
     */
    BOOL is_open = FALSE;

    if (char_type == 0)
        bracket_type = BIDI_BRACKET_NONE;
    else {
        is_open = (char_type & UNIBIDI_TYPE_BRACKET_OPEN) != 0;
        bracket_type = UNIBIDI_GET_BRACKETS (ch) & BIDI_BRACKET_CHAR_MASK;
    }

    if (is_open)
        bracket_type |= BIDI_BRACKET_OPEN_MASK;

    return bracket_type;
}

#include "unicode-bidi-mirroring-table.inc"

static BOOL unicode_bidi_mirror_char(Achar32 ch, Achar32* mirrored_ch)
{
    register Achar32 result;
    register Uint32 mbc_mask;

    mbc_mask = ch & ACHAR_MBC_FLAG;
    ch = REAL_ACHAR(ch);
    result = UNIBIDI_GET_MIRRORING(ch);
    if (mirrored_ch)
        *mirrored_ch = result;
    result |= mbc_mask;

    return ch != result ? TRUE : FALSE;
}

BOOL GUIAPI UCharGetMirror(Uchar32 ch, Uchar32* mirrored_ch)
{
    register Achar32 result;

    result = UNIBIDI_GET_MIRRORING(ch);
    if (mirrored_ch)
        *mirrored_ch = result;
    return ch != result ? TRUE : FALSE;
}

#else

#include "unicode-bidi-tables.h"

static BidiType unicode_bidi_char_type (Achar32 chv)
{
    Achar32 chv_first = 0;
    Achar32 chv_last = (Achar32)TABLESIZE (unicode_bidi_char_type_map);
    Achar32 chv_mid;

    chv = REAL_ACHAR(chv);
    while (chv_last >= chv_first) {
        chv_mid = (chv_first + chv_last)/2;

        if ((unicode_bidi_char_type_map[chv_mid].chv <= chv)
                && ((unicode_bidi_char_type_map[chv_mid].chv + unicode_bidi_char_type_map[chv_mid].count) > chv)) {
            return unicode_bidi_char_type_map[chv_mid].type;
        }

        if (chv >= (unicode_bidi_char_type_map[chv_mid].chv + unicode_bidi_char_type_map[chv_mid].count)) {
            chv_first = chv_mid + 1;
        }
        else {
            if (chv < unicode_bidi_char_type_map[chv_mid].chv)
                chv_last = chv_mid - 1;
            else
                chv_last = chv_mid;
        }
    }

    return BIDI_TYPE_LTR;
}

BOOL __mg_get_mirror_char (const BIDICHAR_MIRROR_MAP* map, int n,
        Achar32 chv, Achar32* mirrored);

static BOOL unicode_bidi_mirror_char (Achar32 chv, Achar32* mirrored)
{
    return __mg_get_mirror_char (unicode_mirror_table,
            TABLESIZE (unicode_mirror_table), chv, mirrored);
}

BidiType GUIAPI UCharGetBidiType(Uchar32 uc)
{
    return unicode_bidi_char_type(uc);
}

BidiBracketType GUIAPI UCharGetBracketType(Uchar32 uc)
{
    BidiBracketType bracket_type;
    unsigned int lower = 0;
    unsigned int upper = TABLESIZE (unicode_bracket_table) - 1;
    int mid = TABLESIZE (unicode_bracket_table) / 2;

    if (uc < unicode_bracket_table[lower].chv ||
            uc > unicode_bracket_table[upper].chv)
        return BIDI_BRACKET_NONE;

    do {
        if (uc < unicode_bracket_table[mid].chv)
            upper = mid - 1;
        else if (uc > unicode_bracket_table[mid].chv)
            lower = mid + 1;
        else
            goto found;

        mid = (lower + upper) / 2;

    } while (lower <= upper);

    return BIDI_BRACKET_NONE;

found:
    if (unicode_bracket_table[mid].type == _BIDI_BRACKET_OPEN) {
        bracket_type = uc | BIDI_BRACKET_OPEN_MASK;
    }
    else {
        bracket_type = unicode_bracket_table[mid].chv +
                       unicode_bracket_table[mid].bracket_off;
        bracket_type &= BIDI_BRACKET_CHAR_MASK;
    }

    return bracket_type;
}

void GUIAPI UStrGetBidiTypes(const Uchar32 *str, int len, BidiType *btypes)
{
    register int i = len;
    for (; i; i--) {
        *btypes++ = UCharGetBracketType(*str);
        str++;
    }
}

/** The function returns the mirror character of a UNICODE character. */
BOOL GUIAPI UCharGetMirror(Uchar32 uc, Uchar32* mirrored)
{
    return unicode_bidi_mirror_char (uc, mirrored);
}

#endif /* !_USE_UNIBIDI */

void UStrGetBracketTypes(const Uchar32 *str,
        const BidiType *types, int len, BidiBracketType *btypes)
{
    int i;
    for (i = 0; i < len; i++) {
        /* Optimization that bracket must be of types ON */
        if (*types == BIDI_TYPE_ON)
            *btypes = UCharGetBracketType (*str);
        else
            *btypes = BIDI_BRACKET_NONE;

        btypes++;
        types++;
        str++;
    }
}

static int utf8_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    int charlen;
    int n = 0;
    int left = mstrlen;

    while (left >= 0) {
        charlen = utf8_len_first_char (mstr, left);
        if (charlen > 0)
            n ++;

        left -= charlen;
        mstr += charlen;
    }

    return n;
}

static int utf8_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "UTF") && strstr (name, "8"))
        return 0;

    return 1;
}

static int utf8_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    int ch_len;
    int sub_len = 0;

    while (mstrlen > 0) {
        ch_len = utf8_len_first_char (mstr, mstrlen);

        if (ch_len == 0)
            break;

        sub_len += ch_len;
        mstrlen -= ch_len;
        mstr += ch_len;
    }

    return sub_len;
}

static const unsigned char* utf8_get_next_word (const unsigned char* mstr,
                int mstrlen, WORDINFO* word_info)
{
    const unsigned char* mchar = mstr;
    int ch_len;
    Uchar32 wc;

    word_info->len = 0;
    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    if (mstrlen == 0) return NULL;

    while (mstrlen > 0) {
        ch_len = utf8_len_first_char (mchar, mstrlen);

        if (ch_len == 0)
            break;

        wc = utf8_get_char_value (NULL, 0, mchar, 0);
        if (wc > 0x0FFF) {
            if (word_info->len)
                return mstr + word_info->len + word_info->nr_delimiters;
            else { /* Treate the first char as a word */
                word_info->len = ch_len;
                return mstr + word_info->len + word_info->nr_delimiters;
            }
        }

        switch (mchar[0]) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            if (word_info->delimiter == '\0') {
                word_info->delimiter = mchar[0];
                word_info->nr_delimiters ++;
            }
            else if (word_info->delimiter == mchar[0])
                word_info->nr_delimiters ++;
            else
                return mstr + word_info->len + word_info->nr_delimiters;

            break;

        default:
            if (word_info->delimiter != '\0')
                return mstr + word_info->len + word_info->nr_delimiters;

            word_info->len += ch_len;
            break;
        }

        mchar += ch_len;
        mstrlen -= ch_len;
    }

    return mstr + word_info->len + word_info->nr_delimiters;
}

static int utf8_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    int ch_len;
    int pos = 0;

    while (mstrlen > 0) {
        ch_len = utf8_len_first_char (mstr, mstrlen);

        /*charset encoding mismatch*/
        if (ch_len == 0)
            return -1;

        if (ch_len > 0)
            return pos;

        pos += ch_len;
        mstrlen -= ch_len;
        mstr += ch_len;
    }

    return -1;
}

static int utf8_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    int first, len;

    if (wc < 0x80) {
        first = 0;
        len = 1;
    }
    else if (wc < 0x800) {
        first = 0xC0;
        len = 2;
    }
    else if (wc < 0x10000) {
        first = 0xE0;
        len = 3;
    }
    else if (wc < 0x200000) {
        first = 0xF0;
        len = 4;
    }
    else if (wc < 0x400000) {
        first = 0xF8;
        len = 5;
    }
    else {
        first = 0xFC;
        len = 6;
    }

    switch (len) {
        case 6:
            mchar [5] = (wc & 0x3f) | 0x80; wc >>= 6; /* Fall through */
        case 5:
            mchar [4] = (wc & 0x3f) | 0x80; wc >>= 6; /* Fall through */
        case 4:
            mchar [3] = (wc & 0x3f) | 0x80; wc >>= 6; /* Fall through */
        case 3:
            mchar [2] = (wc & 0x3f) | 0x80; wc >>= 6; /* Fall through */
        case 2:
            mchar [1] = (wc & 0x3f) | 0x80; wc >>= 6; /* Fall through */
        case 1:
            mchar [0] = wc | first;
    }

    return len;
}

CHARSETOPS __mg_CharsetOps_utf8 = {
    0x7FFFFFFF,
    6,
    FONT_CHARSET_UTF8,
    0,
    utf8_len_first_char,
    utf8_get_char_value,
    NULL,
    unicode_char_type,
    utf8_nr_chars_in_str,
    utf8_is_this_charset,
    utf8_len_first_substr,
    utf8_get_next_word,
    utf8_pos_first_char,
    unicode_bidi_char_type,
    unicode_bidi_mirror_char,
    NULL,
    utf8_conv_from_uc32,
};

/************************* UTF-16LE Specific Operations ***********************/
static int utf16le_len_first_char (const unsigned char* mstr, int len)
{
    Uchar16 w1, w2;

    if (len < 2)
        return 0;

    w1 = MAKEWORD16 (mstr[0], mstr[1]);

    if (w1 < 0xD800 || w1 > 0xDFFF)
        return 2;

    if (w1 >= 0xD800 && w1 <= 0xDBFF) {
        if (len < 4)
            return 0;
        w2 = MAKEWORD16 (mstr[2], mstr[3]);
        if (w2 < 0xDC00 || w2 > 0xDFFF)
            return 0;
    }

    return 4;
}

static Achar32 utf16le_get_char_value (const unsigned char* pre_mchar,
                int pre_len, const unsigned char* cur_mchar, int cur_len)
{
    Uchar16 w1, w2;
    Uchar32 wc;

    w1 = MAKEWORD16 (cur_mchar[0], cur_mchar[1]);

    if (w1 < 0xD800 || w1 > 0xDFFF)
        return w1;

    w2 = MAKEWORD16 (cur_mchar[2], cur_mchar[3]);

    wc = w1;
    wc <<= 10;
    wc |= (w2 & 0x03FF);
    wc += 0x10000;

    return wc;
}

static int utf16le_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    int charlen;
    int n = 0;
    int left = mstrlen;

    while (left >= 0) {
        charlen = utf16le_len_first_char (mstr, left);
        if (charlen > 0)
            n ++;

        left -= charlen;
        mstr += charlen;
    }

    return n;
}

static int utf16le_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "UTF") && strstr (name, "16LE"))
        return 0;

    return 1;
}

static int utf16le_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    int ch_len;
    int sub_len = 0;

    while (mstrlen > 0) {
        ch_len = utf16le_len_first_char (mstr, mstrlen);

        if (ch_len == 0)
            break;

        sub_len += ch_len;
        mstrlen -= ch_len;
        mstr += ch_len;
    }

    return sub_len;
}

static const unsigned char* utf16le_get_next_word (const unsigned char* mstr,
                int mstrlen, WORDINFO* word_info)
{
    const unsigned char* mchar = mstr;
    Uchar32 wc;
    int ch_len;

    word_info->len = 0;
    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    if (mstrlen == 0) return NULL;

    while (mstrlen > 0) {
        ch_len = utf16le_len_first_char (mchar, mstrlen);

        if (ch_len == 0)
            break;

        wc = utf16le_get_char_value (NULL, 0, mchar, 0);
        if (wc > 0x0FFF) {
            if (word_info->len)
                return mstr + word_info->len + word_info->nr_delimiters;
            else { /* Treate the first char as a word */
                word_info->len = ch_len;
                return mstr + word_info->len + word_info->nr_delimiters;
            }
        }

        switch (wc) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            if (word_info->delimiter == '\0') {
                word_info->delimiter = (unsigned char)wc;
                word_info->nr_delimiters += ch_len;
            }
            else if (word_info->delimiter == (unsigned char)wc)
                word_info->nr_delimiters += ch_len;
            else
                return mstr + word_info->len + word_info->nr_delimiters;
            break;

        default:
            if (word_info->delimiter != '\0')
                return mstr + word_info->len + word_info->nr_delimiters;

            word_info->len += ch_len;
            break;
        }

        mstrlen -= ch_len;
        mchar += ch_len;
    }

    return mstr + word_info->len + word_info->nr_delimiters;
}

static int utf16le_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    int ch_len;
    int pos = 0;

    while (mstrlen > 0) {
        ch_len = utf16le_len_first_char (mstr, mstrlen);

        if (ch_len > 0)
            return pos;

        pos += ch_len;
        mstrlen -= ch_len;
        mstr += ch_len;
    }

    return -1;
}

static int utf16le_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    Uchar16 w1, w2;

    if (wc > 0x10FFFF) {
        return 0;
    }

    if (wc < 0x10000) {
        mchar [0] = LOBYTE (wc);
        mchar [1] = HIBYTE (wc);
        return 2;
    }

    wc -= 0x10000;
    w1 = 0xD800;
    w2 = 0xDC00;

    w1 |= (wc >> 10);
    w2 |= (wc & 0x03FF);

    mchar [0] = LOBYTE (w1);
    mchar [1] = HIBYTE (w1);
    mchar [2] = LOBYTE (w2);
    mchar [3] = HIBYTE (w2);
    return 4;
}

CHARSETOPS __mg_CharsetOps_utf16le = {
    0x7FFFFFFF,
    4,
    FONT_CHARSET_UTF16LE,
    0,
    utf16le_len_first_char,
    utf16le_get_char_value,
    NULL,
    unicode_char_type,
    utf16le_nr_chars_in_str,
    utf16le_is_this_charset,
    utf16le_len_first_substr,
    utf16le_get_next_word,
    utf16le_pos_first_char,
    unicode_bidi_char_type,
    unicode_bidi_mirror_char,
    NULL,
    utf16le_conv_from_uc32
};

/************************* UTF-16BE Specific Operations ***********************/
static int utf16be_len_first_char (const unsigned char* mstr, int len)
{
    Uchar16 w1, w2;

    if (len < 2)
        return 0;

    w1 = MAKEWORD16 (mstr[1], mstr[0]);

    if (w1 < 0xD800 || w1 > 0xDFFF)
        return 2;

    if (w1 >= 0xD800 && w1 <= 0xDBFF) {
        if (len < 4)
            return 0;
        w2 = MAKEWORD16 (mstr[3], mstr[2]);
        if (w2 < 0xDC00 || w2 > 0xDFFF)
            return 0;
    }

    return 4;
}

static Achar32 utf16be_get_char_value (const unsigned char* pre_mchar,
                int pre_len, const unsigned char* cur_mchar, int cur_len)
{
    Uchar16 w1, w2;
    Uchar32 wc;

    w1 = MAKEWORD16 (cur_mchar[1], cur_mchar[0]);

    if (w1 < 0xD800 || w1 > 0xDFFF)
        return w1;

    w2 = MAKEWORD16 (cur_mchar[3], cur_mchar[2]);

    wc = w1;
    wc <<= 10;
    wc |= (w2 & 0x03FF);
    wc += 0x10000;

    return wc;
}

static int utf16be_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    int charlen;
    int n = 0;
    int left = mstrlen;

    while (left >= 0) {
        charlen = utf16be_len_first_char (mstr, left);
        if (charlen > 0)
            n ++;

        left -= charlen;
        mstr += charlen;
    }

    return n;
}

static int utf16be_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < LEN_LOGFONT_NAME_FIELD + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "UTF") && strstr (name, "16BE"))
        return 0;

    return 1;
}

static int utf16be_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    int ch_len;
    int sub_len = 0;

    while (mstrlen > 0) {
        ch_len = utf16be_len_first_char (mstr, mstrlen);

        if (ch_len == 0)
            break;

        sub_len += ch_len;
        mstrlen -= ch_len;
        mstr += ch_len;
    }

    return sub_len;
}

static const unsigned char* utf16be_get_next_word (const unsigned char* mstr,
                int mstrlen, WORDINFO* word_info)
{
    const unsigned char* mchar = mstr;
    Uchar32 wc;
    int ch_len;

    word_info->len = 0;
    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    if (mstrlen == 0) return NULL;

    while (mstrlen > 0) {
        ch_len = utf16be_len_first_char (mchar, mstrlen);

        if (ch_len == 0)
            break;

        wc = utf16be_get_char_value (NULL, 0, mchar, 0);
        if (wc > 0x0FFF) {
            if (word_info->len)
                return mstr + word_info->len + word_info->nr_delimiters;
            else { /* Treate the first char as a word */
                word_info->len = ch_len;
                return mstr + word_info->len + word_info->nr_delimiters;
            }
        }

        switch (wc) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            if (word_info->delimiter == '\0') {
                word_info->delimiter = (unsigned char)wc;
                word_info->nr_delimiters += ch_len;
            }
            else if (word_info->delimiter == (unsigned char)wc)
                word_info->nr_delimiters += ch_len;
            else
                return mstr + word_info->len + word_info->nr_delimiters;
            break;

        default:
            if (word_info->delimiter != '\0')
                return mstr + word_info->len + word_info->nr_delimiters;

            word_info->len += ch_len;
            break;
        }

        mstrlen -= ch_len;
        mchar += ch_len;
    }

    return mstr + word_info->len + word_info->nr_delimiters;
}

static int utf16be_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    int ch_len;
    int pos = 0;

    while (mstrlen > 0) {
        ch_len = utf16be_len_first_char (mstr, mstrlen);

        if (ch_len > 0)
            return pos;

        pos += ch_len;
        mstrlen -= ch_len;
        mstr += ch_len;
    }

    return -1;
}

static int utf16be_conv_from_uc32 (Uchar32 wc, unsigned char* mchar)
{
    Uchar16 w1, w2;

    if (wc > 0x10FFFF) {
        return 0;
    }

    if (wc < 0x10000) {
        mchar [1] = LOBYTE (wc);
        mchar [0] = HIBYTE (wc);
        return 2;
    }

    wc -= 0x10000;
    w1 = 0xD800;
    w2 = 0xDC00;

    w1 |= (wc >> 10);
    w2 |= (wc & 0x03FF);

    mchar [1] = LOBYTE (w1);
    mchar [0] = HIBYTE (w1);
    mchar [3] = LOBYTE (w2);
    mchar [2] = HIBYTE (w2);
    return 4;
}

CHARSETOPS __mg_CharsetOps_utf16be = {
    0x7FFFFFFF,
    4,
    FONT_CHARSET_UTF16BE,
    0,
    utf16be_len_first_char,
    utf16be_get_char_value,
    NULL,
    unicode_char_type,
    utf16be_nr_chars_in_str,
    utf16be_is_this_charset,
    utf16be_len_first_substr,
    utf16be_get_next_word,
    utf16be_pos_first_char,
    unicode_bidi_char_type,
    unicode_bidi_mirror_char,
    NULL,
    utf16be_conv_from_uc32
};

UCharGeneralCategory GUIAPI UCharGetCategory(Uchar32 uc)
{
    return (UCharGeneralCategory)TYPE(uc);
}

/** The function determines the break type of a UNICODE character. */
UCharBreakType GUIAPI UCharGetBreakType(Uchar32 uc)
{
    return (UCharBreakType)PROP(uc);
}

BOOL GUIAPI IsUCharAlnum(Uchar32 uc)
{
    return ISALDIGIT(TYPE(uc));
}

BOOL GUIAPI IsUCharAlpha(Uchar32 uc)
{
    return ISALPHA (TYPE(uc));
}

BOOL GUIAPI IsUCharControl(Uchar32 uc)
{
    return TYPE(uc) == UCHAR_CATEGORY_CONTROL;
}

BOOL GUIAPI IsUCharDigit(Uchar32 uc)
{
    return TYPE(uc) == UCHAR_CATEGORY_DECIMAL_NUMBER;
}

BOOL GUIAPI IsUCharGraph(Uchar32 uc)
{
    return !IS (TYPE(uc),
            OR (UCHAR_CATEGORY_CONTROL,
            OR (UCHAR_CATEGORY_FORMAT,
            OR (UCHAR_CATEGORY_UNASSIGNED,
            OR (UCHAR_CATEGORY_SURROGATE,
            OR (UCHAR_CATEGORY_SPACE_SEPARATOR,
            0))))));
}

BOOL GUIAPI IsUCharLowercase(Uchar32 uc)
{
    return TYPE(uc) == UCHAR_CATEGORY_LOWERCASE_LETTER;
}

BOOL GUIAPI IsUCharPrint(Uchar32 uc)
{
    return !IS (TYPE(uc),
            OR (UCHAR_CATEGORY_CONTROL,
            OR (UCHAR_CATEGORY_FORMAT,
            OR (UCHAR_CATEGORY_UNASSIGNED,
            OR (UCHAR_CATEGORY_SURROGATE,
            0)))));
}

BOOL GUIAPI IsUCharUppercase(Uchar32 uc)
{
    return TYPE(uc) == UCHAR_CATEGORY_UPPERCASE_LETTER;
}

BOOL GUIAPI IsUCharPunct(Uchar32 uc)
{
    return IS (TYPE(uc),
            OR (UCHAR_CATEGORY_CONNECT_PUNCTUATION,
            OR (UCHAR_CATEGORY_DASH_PUNCTUATION,
            OR (UCHAR_CATEGORY_CLOSE_PUNCTUATION,
            OR (UCHAR_CATEGORY_FINAL_PUNCTUATION,
            OR (UCHAR_CATEGORY_INITIAL_PUNCTUATION,
            OR (UCHAR_CATEGORY_OTHER_PUNCTUATION,
            OR (UCHAR_CATEGORY_OPEN_PUNCTUATION,
            OR (UCHAR_CATEGORY_CURRENCY_SYMBOL,
            OR (UCHAR_CATEGORY_MODIFIER_SYMBOL,
            OR (UCHAR_CATEGORY_MATH_SYMBOL,
            OR (UCHAR_CATEGORY_OTHER_SYMBOL,
            0))))))))))));
}

BOOL GUIAPI IsUCharSpace(Uchar32 uc)
{
    switch (uc) {
    /* special-case these since Unicode thinks they are not spaces */
    case '\t':
    case '\n':
    case '\r':
    case '\f':
        return TRUE;

    default: {
        if (IS (TYPE(uc),
               OR (UCHAR_CATEGORY_SPACE_SEPARATOR,
               OR (UCHAR_CATEGORY_LINE_SEPARATOR,
               OR (UCHAR_CATEGORY_PARAGRAPH_SEPARATOR,
                0)))))
            return TRUE;
        }
        break;
    }

    return FALSE;
}

BOOL GUIAPI IsUCharMark(Uchar32 uc)
{
    return ISMARK (TYPE(uc));
}

BOOL GUIAPI IsUCharTitle(Uchar32 uc)
{
    unsigned int i;

    for (i = 0; i < TABLESIZE (title_table); ++i) {
        if (title_table[i][0] == uc) {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL GUIAPI IsUCharXDigit(Uchar32 uc)
{
    return ((uc >= 'a' && uc <= 'f')
            || (uc >= 'A' && uc <= 'F')
            || (TYPE(uc) == UCHAR_CATEGORY_DECIMAL_NUMBER));
}

BOOL GUIAPI IsUCharDefined(Uchar32 uc)
{
    return !IS (TYPE(uc),
              OR (UCHAR_CATEGORY_UNASSIGNED,
              OR (UCHAR_CATEGORY_SURROGATE,
             0)));
}

BOOL GUIAPI IsUCharZeroWidth(Uchar32 uc)
{
    if (uc != 0x00AD && ISZEROWIDTHTYPE (TYPE(uc)))
        return TRUE;
    else if ((uc >= 0x1160 && uc < 0x1200)
            || uc == 0x200B)
        return TRUE;

    return FALSE;
}

static inline BOOL g_unichar_iswide_bsearch (Uchar32 ch)
{
    unsigned int lower = 0;
    unsigned int upper = TABLESIZE (unicode_width_table_wide) - 1;
    int mid = TABLESIZE (unicode_width_table_wide) / 2;

    do {
        if (ch < unicode_width_table_wide[mid].start)
            upper = mid - 1;
        else if (ch > unicode_width_table_wide[mid].end)
            lower = mid + 1;
        else
            return TRUE;

        mid = (lower + upper) / 2;
    }
    while (lower <= upper);

    return FALSE;
}

BOOL GUIAPI IsUCharWide(Uchar32 uc)
{
    if (uc < unicode_width_table_wide[0].start)
        return FALSE;
    else
        return g_unichar_iswide_bsearch (uc);
}

static int interval_compare (const void *key, const void *elt)
{
    Uchar32 uc = (Uchar32)((intptr_t)key);
    struct Interval *interval = (struct Interval *)elt;

    if (uc < interval->start)
        return -1;
    if (uc > interval->end)
        return +1;

    return 0;
}

BOOL GUIAPI IsUCharWideCJK (Uchar32 uc)
{
    if (IsUCharWide(uc))
        return TRUE;

    /* bsearch() is declared attribute(nonnull(1)) so we can't validly search
     * for a NULL key */
    if (uc == 0)
        return FALSE;

    if (bsearch((void*)((intptr_t)uc),
                unicode_width_table_ambiguous,
                TABLESIZE (unicode_width_table_ambiguous),
                sizeof unicode_width_table_ambiguous[0],
                interval_compare))
        return TRUE;

    return FALSE;
}

/**
 * Converts a character to uppercase.
 */
Uchar32 UCharToUpper (Uchar32 uc)
{
    int t = TYPE (uc);
    if (t == UCHAR_CATEGORY_LOWERCASE_LETTER) {
        Uchar32 val = ATTTABLE (uc >> 8, uc & 0xff);
        if (val >= 0x1000000) {
            const unsigned char *p = special_case_table + val - 0x1000000;
            val = utf8_get_char_value (NULL, 0, p, -1);
        }
        /* Some lowercase letters, e.uc., U+000AA, FEMININE ORDINAL INDICATOR,
         * do not have an uppercase equivalent, in which case val will be
         * zero.
         */
        return val ? val : uc;
    }
    else if (t == UCHAR_CATEGORY_TITLECASE_LETTER) {
        unsigned int i;
        for (i = 0; i < TABLESIZE (title_table); ++i) {
            if (title_table[i][0] == uc)
                return title_table[i][1] ? title_table[i][1] : uc;
        }
    }

    return uc;
}

/**
 * Converts a character to lower case.
 */
Uchar32 GUIAPI UCharToLower (Uchar32 uc)
{
    int t = TYPE (uc);
    if (t == UCHAR_CATEGORY_UPPERCASE_LETTER) {
        Uchar32 val = ATTTABLE (uc >> 8, uc & 0xff);
        if (val >= 0x1000000) {
            const unsigned char *p = special_case_table + val - 0x1000000;
            return utf8_get_char_value (NULL, 0, p, -1);
        }
        else {
            /* Not all uppercase letters are guaranteed to have a lowercase
             * equivalent.  If this is the case, val will be zero. */
            return val ? val : uc;
        }
    }
    else if (t == UCHAR_CATEGORY_TITLECASE_LETTER) {
        unsigned int i;
        for (i = 0; i < TABLESIZE (title_table); ++i) {
            if (title_table[i][0] == uc)
                return title_table[i][2];
        }
    }
    return uc;
}

/**
 * Converts a chv to the titlecase.
 */
Uchar32 GUIAPI UCharToTitle (Uchar32 uc)
{
    unsigned int i;
    for (i = 0; i < TABLESIZE (title_table); ++i) {
        if (title_table[i][0] == uc || title_table[i][1] == uc
                || title_table[i][2] == uc)
            return title_table[i][0];
    }

    if (TYPE (uc) == UCHAR_CATEGORY_LOWERCASE_LETTER)
        return UCharToUpper (uc);

    return uc;
}

/** Converts a chv to full-width. */
Uchar32 GUIAPI UCharToFullWidth (Uchar32 uc)
{
    if (uc == 0x20)
        return 0x3000;

    if (uc >= 0x21 && uc <= 0x7E) {
        return uc + (0xFF01 - 0x21);
    }

    return uc;
}

/** Converts a chv to single-width. */
Uchar32 GUIAPI UCharToSingleWidth (Uchar32 uc)
{
    if (uc == 0x3000)
        return 0x20;

    if (uc >= 0xFF01 && uc <= 0xFF5E) {
        return uc - (0xFF01 - 0x21);
    }

    return uc;
}

struct UCharMap {
    Uchar32 one, other;
};

static const struct UCharMap kana_small_to_full_size_table [] = {
    { /*ぁ*/ 0x3041, /*あ*/ 0x3042},
    { /*ぃ*/ 0x3043, /*い*/ 0x3044},
    { /*ぅ*/ 0x3045, /*う*/ 0x3046},
    { /*ぇ*/ 0x3047, /*え*/ 0x3048},
    { /*ぉ*/ 0x3049, /*お*/ 0x304A},
    { /*ゕ*/ 0x3095, /*か*/ 0x304B},
    { /*ゖ*/ 0x3096, /*け*/ 0x3051},
    { /*っ*/ 0x3063, /*つ*/ 0x3064},
    { /*ゃ*/ 0x3083, /*や*/ 0x3084},
    { /*ゅ*/ 0x3085, /*ゆ*/ 0x3086},
    { /*ょ*/ 0x3087, /*よ*/ 0x3088},
    { /*ゎ*/ 0x308E, /*わ*/ 0x308F},
    { /*ァ*/ 0x30A1, /*ア*/ 0x30A2},
    { /*ィ*/ 0x30A3, /*イ*/ 0x30A4},
    { /*ゥ*/ 0x30A5, /*ウ*/ 0x30A6},
    { /*ェ*/ 0x30A7, /*エ*/ 0x30A8},
    { /*ォ*/ 0x30A9, /*オ*/ 0x30AA},
    { /*ヵ*/ 0x30F5, /*カ*/ 0x30AB},
    { /*ㇰ*/ 0x31F0, /*ク*/ 0x30AF},
    { /*ヶ*/ 0x30F6, /*ケ*/ 0x30B1},
    { /*ㇱ*/ 0x31F1, /*シ*/ 0x30B7},
    { /*ㇲ*/ 0x31F2, /*ス*/ 0x30B9},
    { /*ッ*/ 0x30C3, /*ツ*/ 0x30C4},
    { /*ㇳ*/ 0x31F3, /*ト*/ 0x30C8},
    { /*ㇴ*/ 0x31F4, /*ヌ*/ 0x30CC},
    { /*ㇵ*/ 0x31F5, /*ハ*/ 0x30CF},
    { /*ㇶ*/ 0x31F6, /*ヒ*/ 0x30D2},
    { /*ㇷ*/ 0x31F7, /*フ*/ 0x30D5},
    { /*ㇸ*/ 0x31F8, /*ヘ*/ 0x30D8},
    { /*ㇹ*/ 0x31F9, /*ホ*/ 0x30DB},
    { /*ㇺ*/ 0x31FA, /*ム*/ 0x30E0},
    { /*ャ*/ 0x30E3, /*ヤ*/ 0x30E4},
    { /*ュ*/ 0x30E5, /*ユ*/ 0x30E6},
    { /*ョ*/ 0x30E7, /*ヨ*/ 0x30E8},
    { /*ㇻ*/ 0x31FB, /*ラ*/ 0x30E9},
    { /*ㇼ*/ 0x31FC, /*リ*/ 0x30EA},
    { /*ㇽ*/ 0x31FD, /*ル*/ 0x30EB},
    { /*ㇾ*/ 0x31FE, /*レ*/ 0x30EC},
    { /*ㇿ*/ 0x31FF, /*ロ*/ 0x30ED},
    { /*ヮ*/ 0x30EE, /*ワ*/ 0x30EF},
    { /*ｧ*/ 0xFF67, /*ｱ*/ 0xFF71},
    { /*ｨ*/ 0xFF68, /*ｲ*/ 0xFF72},
    { /*ｩ*/ 0xFF69, /*ｳ*/ 0xFF73},
    { /*ｪ*/ 0xFF6A, /*ｴ*/ 0xFF74},
    { /*ｫ*/ 0xFF6B, /*ｵ*/ 0xFF75},
    { /*ｯ*/ 0xFF6F, /*ﾂ*/ 0xFF82},
    { /*ｬ*/ 0xFF6C, /*ﾔ*/ 0xFF94},
    { /*ｭ*/ 0xFF6D, /*ﾕ*/ 0xFF95},
    { /*ｮ*/ 0xFF6E, /*ﾖ*/ 0xFF96},
};

/** Converts a chv to full-size Kana. */
Uchar32 GUIAPI UCharToFullSizeKana (Uchar32 uc)
{
    unsigned int lower = 0;
    unsigned int upper = TABLESIZE (kana_small_to_full_size_table) - 1;
    int mid = TABLESIZE (kana_small_to_full_size_table) / 2;

    if (uc < kana_small_to_full_size_table[lower].one
            || uc > kana_small_to_full_size_table[upper].one)
        return uc;

    do {
        if (uc < kana_small_to_full_size_table[mid].one)
            upper = mid - 1;
        else if (uc > kana_small_to_full_size_table[mid].one)
            lower = mid + 1;
        else
            return kana_small_to_full_size_table[mid].other;

        mid = (lower + upper) / 2;

    } while (lower <= upper);

    return uc;
}

#if 0 // VincentWei: use kana_small_to_full_size_table instead
static const struct UCharMap kana_full_size_to_small_table [] = {
    { /*あ*/ 0x3041, /*ぁ*/ 0x3041},
    { /*い*/ 0x3043, /*ぃ*/ 0x3043},
    { /*う*/ 0x3045, /*ぅ*/ 0x3045},
    { /*え*/ 0x3047, /*ぇ*/ 0x3047},
    { /*お*/ 0x3049, /*ぉ*/ 0x3049},
    { /*か*/ 0x304A, /*ゕ*/ 0x3095},
    { /*け*/ 0x3050, /*ゖ*/ 0x3096},
    { /*つ*/ 0x3063, /*っ*/ 0x3063},
    { /*や*/ 0x3083, /*ゃ*/ 0x3083},
    { /*ゆ*/ 0x3085, /*ゅ*/ 0x3085},
    { /*よ*/ 0x3087, /*ょ*/ 0x3087},
    { /*わ*/ 0x308E, /*ゎ*/ 0x308E},
    { /*ア*/ 0x30A1, /*ァ*/ 0x30A1},
    { /*イ*/ 0x30A3, /*ィ*/ 0x30A3},
    { /*ウ*/ 0x30A5, /*ゥ*/ 0x30A5},
    { /*エ*/ 0x30A7, /*ェ*/ 0x30A7},
    { /*オ*/ 0x30A9, /*ォ*/ 0x30A9},
    { /*カ*/ 0x30AA, /*ヵ*/ 0x30F5},
    { /*ク*/ 0x30AE, /*ㇰ*/ 0x31F0},
    { /*ケ*/ 0x30B0, /*ヶ*/ 0x30F6},
    { /*シ*/ 0x30B6, /*ㇱ*/ 0x31F1},
    { /*ス*/ 0x30B8, /*ㇲ*/ 0x31F2},
    { /*ツ*/ 0x30C3, /*ッ*/ 0x30C3},
    { /*ト*/ 0x30C7, /*ㇳ*/ 0x31F3},
    { /*ヌ*/ 0x30CB, /*ㇴ*/ 0x31F4},
    { /*ハ*/ 0x30CE, /*ㇵ*/ 0x31F5},
    { /*ヒ*/ 0x30D1, /*ㇶ*/ 0x31F6},
    { /*フ*/ 0x30D4, /*ㇷ*/ 0x31F7},
    { /*ヘ*/ 0x30D7, /*ㇸ*/ 0x31F8},
    { /*ホ*/ 0x30DA, /*ㇹ*/ 0x31F9},
    { /*ム*/ 0x30DF, /*ㇺ*/ 0x31FA},
    { /*ヤ*/ 0x30E3, /*ャ*/ 0x30E3},
    { /*ユ*/ 0x30E5, /*ュ*/ 0x30E5},
    { /*ヨ*/ 0x30E7, /*ョ*/ 0x30E7},
    { /*ラ*/ 0x30E8, /*ㇻ*/ 0x31FB},
    { /*リ*/ 0x30E9, /*ㇼ*/ 0x31FC},
    { /*ル*/ 0x30EA, /*ㇽ*/ 0x31FD},
    { /*レ*/ 0x30EB, /*ㇾ*/ 0x31FE},
    { /*ロ*/ 0x30EC, /*ㇿ*/ 0x31FF},
    { /*ワ*/ 0x30EE, /*ヮ*/ 0x30EE},
    { /*ｱ*/ 0xFF70, /*ｧ*/ 0xFF67},
    { /*ｲ*/ 0xFF71, /*ｨ*/ 0xFF68},
    { /*ｳ*/ 0xFF72, /*ｩ*/ 0xFF69},
    { /*ｴ*/ 0xFF73, /*ｪ*/ 0xFF6A},
    { /*ｵ*/ 0xFF74, /*ｫ*/ 0xFF6B},
    { /*ﾂ*/ 0xFF81, /*ｯ*/ 0xFF6F},
    { /*ﾔ*/ 0xFF93, /*ｬ*/ 0xFF6C},
    { /*ﾕ*/ 0xFF94, /*ｭ*/ 0xFF6D},
    { /*ﾖ*/ 0xFF95, /*ｮ*/ 0xFF6E},
};

/** Converts a chv to small Kana. */
Uchar32 GUIAPI UCharToSmallKana (Uchar32 uc)
{
    unsigned int lower = 0;
    unsigned int upper = TABLESIZE (kana_full_size_to_small_table) - 1;
    int mid = TABLESIZE (kana_full_size_to_small_table) / 2;

    if (uc < kana_full_size_to_small_table[lower].one
            || uc < kana_full_size_to_small_table[upper].one)
        return uc;

    do {
        if (uc < kana_full_size_to_small_table[mid].one)
            upper = mid - 1;
        else if (uc > kana_full_size_to_small_table[mid].one)
            lower = mid + 1;
        else
            return kana_full_size_to_small_table[mid].other;

        mid = (lower + upper) / 2;

    } while (lower <= upper);

    return uc;
}
#endif // VincentWei: use kana_small_to_full_size_table instead

/** Converts a chv to small Kana. */
Uchar32 GUIAPI UCharToSmallKana (Uchar32 uc)
{
    unsigned int lower = 0;
    unsigned int upper = TABLESIZE (kana_small_to_full_size_table) - 1;
    int mid = TABLESIZE (kana_small_to_full_size_table) / 2;

    if (uc < kana_small_to_full_size_table[lower].other
            || uc > kana_small_to_full_size_table[upper].other)
        return uc;

    do {
        if (uc < kana_small_to_full_size_table[mid].other)
            upper = mid - 1;
        else if (uc > kana_small_to_full_size_table[mid].other)
            lower = mid + 1;
        else
            return kana_small_to_full_size_table[mid].one;

        mid = (lower + upper) / 2;

    } while (lower <= upper);

    return uc;
}

BOOL GUIAPI UCharIsArabicVowel(Uchar32 uc)
{
    if ((uc >= 0x064B) && (uc <= 0x0655))
        return TRUE;

    if ((uc >= 0xFC5E) && (uc <= 0xFC63))
        return TRUE;

    if ((uc >= 0xFE70) && (uc <= 0xFE7F))
        return TRUE;

    return 0;
}

#endif /* _MGCHARSET_UNICODE */
