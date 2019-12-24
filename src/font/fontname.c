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
** fontname.c: Font name parser.
**
** Current maintainer: Wei Yongming.
**
** Created by Wei Yongming, 2000/07/11
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "fontname.h"

/*
 * Font name format:
 * type-family-style-width-height-charset-encoding1[,encoding2,...]
 */

int fontConvertFontType (const char* type)
{
    if (strcasecmp (type, FONT_TYPE_NAME_BITMAP_RAW) == 0)
        return FONT_TYPE_BITMAP_RAW;
    if (strcasecmp (type, FONT_TYPE_NAME_BITMAP_VAR) == 0)
        return FONT_TYPE_BITMAP_VAR;
    if (strcasecmp (type, FONT_TYPE_NAME_BITMAP_QPF) == 0)
        return FONT_TYPE_BITMAP_QPF;
    if (strcasecmp (type, FONT_TYPE_NAME_BITMAP_UPF) == 0)
        return FONT_TYPE_BITMAP_UPF;
    if (strcasecmp (type, FONT_TYPE_NAME_BITMAP_BMP) == 0)
        return FONT_TYPE_BITMAP_BMP;
    if (strcasecmp (type, FONT_TYPE_NAME_SCALE_TTF) == 0)
        return FONT_TYPE_SCALE_TTF;
    if (strcasecmp (type, FONT_TYPE_NAME_SCALE_T1F) == 0)
        return FONT_TYPE_SCALE_T1F;
    if (strcasecmp (type, FONT_TYPE_NAME_ALL) == 0)
        return FONT_TYPE_ANY;

    return -1;
}

BOOL fontGetTypeNameFromName (const char* name, char* type)
{
    int i = 0;
    while (name [i]) {
        if (name [i] == '-') {
            type [i] = '\0';
            break;
        }

        type [i] = name [i];
        i++;
    }

    if (name [i] == '\0')
        return FALSE;

    return TRUE;
}

int fontGetFontTypeFromName (const char* name)
{
    char type [LEN_LOGFONT_NAME_FIELD + 1];

    if (!fontGetTypeNameFromName (name, type))
        return -1;

    return fontConvertFontType (type);
}

BOOL fontGetFamilyFromName (const char* name, char* family)
{
    int i = 0;
    const char* family_part;

    if ((family_part = strchr (name, '-')) == NULL)
        return FALSE;
    if (*(++family_part) == '\0')
        return FALSE;

    while (family_part [i] && i <= LEN_LOGFONT_FAMILY_FIELD) {
        if (family_part [i] == '-'/* || family_part [i] == ',' */) {
            family [i] = '\0';
            break;
        }

        family [i] = family_part [i];
        i++;
    }

    return TRUE;
}

static BOOL get_family_part_lower (const char* name, char* family)
{
    int i = 0;
    const char* family_part;

    if ((family_part = strchr (name, '-')) == NULL)
        return FALSE;
    if (*(++family_part) == '\0')
        return FALSE;

    while (family_part [i] && i <= LEN_UNIDEVFONT_NAME) {
        if (family_part [i] == '-') {
            family [i] = '\0';
            break;
        }

        family [i] = tolower(family_part [i]);
        i++;
    }

    return TRUE;
}

#if 0
static const char *my_strstr(const char *haystack, const char *needle)
{
    int i = 0, j = 0;
    int tmp = i;
    int str_len = strlen(haystack);
    int substr_len = strlen(needle);

    for (i = 0; i < str_len - substr_len; i++) {
        tmp = i;

        for (j = 0; j < substr_len; j++) {
            if (haystack[tmp] == needle[j]) {
                if (j == substr_len - 1)
                    return haystack + i;
                tmp++;
            }
            else
                break;
        }
    }

    return NULL;
}
#endif

/* devfont family specification:
 * family[,alias]...
 */
BOOL fontDoesMatchFamily (const char* name, const char* family)
{
    // make sure there is a redundant space for the head and tail characters.
    char family_part[LEN_LOGFONT_FAMILY_FIELD + 3];
    char family_request[LEN_LOGFONT_NAME_FIELD + 3];
    int i;
    size_t len;

    // add ',' to the head
    family_part[0] = ',';
    if (!get_family_part_lower(name, family_part + 1)) {
        return FALSE;
    }

    // add ',' to the tail
    len = strlen(family_part);
    family_part[len] = ',';
    family_part[len + 1] = '\0';

    // make family (lowercase)
    family_request[0] = ',';
    i = 0;
    while (family[i] && i < LEN_LOGFONT_NAME_FIELD) {
        family_request[i + 1] = tolower(family[i]);
        i++;
    }
    family_request[i + 1] = '\0';

    // add ',' to the tail
    len = strlen(family_request);
    family_request[len] = ',';
    family_request[len + 1] = '\0';

    // try to match "<family_request>,"
    if (strstr(family_part, family_request)) {
        return TRUE;
    }

    return FALSE;
}

DWORD fontConvertStyle (const char* style_part)
{
    DWORD style = 0;

    style &= ~FS_WEIGHT_MASK;
    switch (style_part [0]) {
    case FONT_WEIGHT_BLACK:
        style |= FS_WEIGHT_BLACK;
        break;
    case FONT_WEIGHT_EXTRA_BOLD:
        style |= FS_WEIGHT_EXTRA_BOLD;
        break;
    case FONT_WEIGHT_BOLD:
        style |= FS_WEIGHT_BOLD;
        break;
    /* deprecated since v4.0.0
    case FONT_WEIGHT_BOOK:
        style |= FS_WEIGHT_BOOK;
        break;
    */
    case FONT_WEIGHT_DEMIBOLD:
        style |= FS_WEIGHT_DEMIBOLD;
        break;
    case FONT_WEIGHT_MEDIUM:
        style |= FS_WEIGHT_MEDIUM;
        break;
    case FONT_WEIGHT_REGULAR:
        style |= FS_WEIGHT_REGULAR;
        break;
    case FONT_WEIGHT_NORMAL:
        style |= FS_WEIGHT_NORMAL;
        break;
    case FONT_WEIGHT_LIGHT:
        style |= FS_WEIGHT_LIGHT;
        break;
    case FONT_WEIGHT_EXTRA_LIGHT:
        style |= FS_WEIGHT_EXTRA_LIGHT;
        break;
    case FONT_WEIGHT_THIN:
        style |= FS_WEIGHT_THIN;
        break;
    /* deprecated since v4.0.0
    case FONT_WEIGHT_SUBPIXEL:
        style |= FS_WEIGHT_SUBPIXEL;
        break;
    case FONT_WEIGHT_ALL:
        style |= FS_WEIGHT_MASK;
        break;
    */
    case FONT_WEIGHT_ANY:
    default:
        style |= FS_WEIGHT_ANY;
        break;
    }

    style &= ~FS_SLANT_MASK;
    switch (style_part [1]) {
    case FONT_SLANT_ITALIC:
        style |= FS_SLANT_ITALIC;
        break;
    case FONT_SLANT_OBLIQUE:
        style |= FS_SLANT_OBLIQUE;
        break;
    case FONT_SLANT_ROMAN:
        style |= FS_SLANT_ROMAN;
        break;
    case FONT_SLANT_ANY:
    default:
        style |= FS_SLANT_ANY;
        break;
    }

    style &= ~FS_FLIP_MASK;
    switch (style_part [2]) {
    case FONT_FLIP_HORZ:
        style |= FS_FLIP_HORZ;
        break;
    case FONT_FLIP_VERT:
        style |= FS_FLIP_VERT;
        break;
    case FONT_FLIP_HORZVERT:
        style |= FS_FLIP_HORZVERT;
        break;
    default:
        //style |= FS_FLIP_NONE;
        break;
    }

    style &= ~FS_OTHER_MASK;
    switch (style_part [3]) {
    case FONT_OTHER_AUTOSCALE:
        style |= FS_OTHER_AUTOSCALE;
        break;
    case FONT_OTHER_TTFNOCACHE:
        style |= FS_OTHER_TTFNOCACHE;
        break;
    case FONT_OTHER_TTFKERN:
        style |= FS_OTHER_TTFKERN;
        break;
    case FONT_OTHER_TTFNOCACHEKERN:
        style |= FS_OTHER_TTFNOCACHEKERN;
        break;
    default:
        //style |= FS_OTHER_NONE;
        break;
    }

    style &= ~FS_DECORATE_MASK;
    switch (style_part [4]) {
    case FONT_DECORATE_NONE:
        break;
    case FONT_DECORATE_UNDERLINE:
        style |= FS_DECORATE_UNDERLINE;
        break;
    case FONT_DECORATE_STRUCKOUT:
        style |= FS_DECORATE_STRUCKOUT;
        break;
    case FONT_DECORATE_REVERSE:
        style |= FS_DECORATE_REVERSE;
        break;
    case FONT_DECORATE_OUTLINE:
        style |= FS_DECORATE_OUTLINE;
        break;
    case FONT_DECORATE_US:
        style |= FS_DECORATE_UNDERLINE;
        style |= FS_DECORATE_STRUCKOUT;
        break;
    default:
        //style |= FS_DECORATE_NONE;
        break;
    }

    style &= ~FS_RENDER_MASK;
    switch (style_part [5]) {
    case FONT_RENDER_MONO:
        style |= FS_RENDER_MONO;
        break;
    case FONT_RENDER_GREY:
        style |= FS_RENDER_GREY;
        break;
    case FONT_RENDER_SUBPIXEL:
        style |= FS_RENDER_SUBPIXEL;
        break;
    default:
        style |= FS_RENDER_MONO;
        break;
    }

    /* deprecated since v3.2.1
    switch (style_part [4]) {
    case FONT_UNDERLINE_LINE:
        style |= FS_UNDERLINE_LINE;
        break;
    case FONT_UNDERLINE_ALL:
        style |= FS_UNDERLINE_MASK;
        break;
    case FONT_UNDERLINE_NONE:
        style &= ~FS_UNDERLINE_MASK;
        break;
    default:
        return 0xFFFFFFFF;
    }

    switch (style_part [5]) {
    case FONT_STRUCKOUT_LINE:
        style |= FS_STRUCKOUT_LINE;
        break;
    case FONT_STRUCKOUT_ALL:
        style |= FS_STRUCKOUT_MASK;
        break;
    case FONT_STRUCKOUT_NONE:
        style &= ~FS_STRUCKOUT_MASK;
        break;
    default:
        return 0xFFFFFFFF;
    }
    */

    return style;
}

BOOL fontCopyStyleFromName (const char* name, char* style)
{
    int i;
    const char* style_part = name;

    for (i = 0; i < NR_LOOP_FOR_STYLE; i++) {
        if ((style_part = strchr (style_part, '-')) == NULL)
            return 0xFFFFFFFF;

        if (*(++style_part) == '\0')
            return 0xFFFFFFFF;
    }

    strncpy (style, style_part, 6);
    style[6] = '\0';

    return TRUE;
}

DWORD fontGetStyleFromName (const char* name)
{
    int i;
    const char* style_part = name;
    char style_name[7];

    for (i = 0; i < NR_LOOP_FOR_STYLE; i++) {
        if ((style_part = strchr (style_part, '-')) == NULL)
            return 0xFFFFFFFF;

        if (*(++style_part) == '\0')
            return 0xFFFFFFFF;
    }

    strncpy (style_name, style_part, 6);
    style_name[6] = '\0';

    return fontConvertStyle (style_name);
}

int fontGetWidthFromName (const char* name)
{
    int i;
    const char* width_part = name;
    char width [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < NR_LOOP_FOR_WIDTH; i++) {
        if ((width_part = strchr (width_part, '-')) == NULL)
            return -1;

        if (*(++width_part) == '\0')
            return -1;
    }

    i = 0;
    while (width_part [i]) {
        if (width_part [i] == '-') {
            width [i] = '\0';
            break;
        }

        width [i] = width_part [i];
        i++;
    }

    if (width_part [i] == '\0')
        return -1;

    return atoi (width);
}

/* Since 4.0.0; only for LOGFONT name */
char fontGetOrientFromName (const char* name)
{
    int i;
    const char* orient_part = name;
    char orient [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < NR_LOOP_FOR_ORIENT; i++) {
        if ((orient_part = strchr (orient_part, '-')) == NULL)
            return FONT_ORIENT_UPRIGHT;

        if (*(++orient_part) == '\0')
            return FONT_ORIENT_UPRIGHT;
    }

    i = 0;
    while (orient_part [i]) {
        if (orient_part [i] == '-') {
            orient [i] = '\0';
            break;
        }

        orient [i] = orient_part [i];
        i++;
    }

    if (orient_part [i] == '\0')
        return FONT_ORIENT_UPRIGHT;

    return orient[0];
}

int fontGetOrientPosFromName (const char* name)
{
    int i;
    const char* orient_part = name;

    for (i = 0; i < NR_LOOP_FOR_ORIENT; i++) {
        if ((orient_part = strchr (orient_part, '-')) == NULL)
            return -1;

        if (*(++orient_part) == '\0')
            return -1;
    }

    return orient_part - name;
}

int fontGetHeightFromName (const char* name)
{
    int i;
    const char* height_part = name;
    char height [LEN_LOGFONT_NAME_FIELD + 1];

    for (i = 0; i < NR_LOOP_FOR_HEIGHT; i++) {
        if ((height_part = strchr (height_part, '-')) == NULL)
            return -1;
        if (*(++height_part) == '\0')
            return -1;
    }

    i = 0;
    while (height_part [i]) {
        if (height_part [i] == '-') {
            height [i] = '\0';
            break;
        }

        height [i] = height_part [i];
        i++;
    }

    if (height_part [i] == '\0')
        return -1;

    return atoi (height);
}

BOOL fontGetCharsetFromName (const char* name, char* charset)
{
    int i;
    char* delim;
    const char* charset_part = name;

    for (i = 0; i < NR_LOOP_FOR_CHARSET; i++) {
        if ((charset_part = strchr (charset_part, '-')) == NULL)
            return FALSE;
        if (*(++charset_part) == '\0')
            return FALSE;
    }

    if ((delim = strchr (charset_part, ','))) {
        int len;
        len = delim - charset_part;
        strncpy (charset, charset_part, len);
        charset [len] = '\0';
        return TRUE;
    }

    strncpy (charset, charset_part, LEN_LOGFONT_NAME_FIELD);
    charset [LEN_LOGFONT_NAME_FIELD] = '\0';
    return TRUE;
}

BOOL fontGetCompatibleCharsetFromName (const char* name, char* charset)
{
    int i;
    const char* charset_part = name;

    for (i = 0; i < NR_LOOP_FOR_CHARSET; i++) {
        if ((charset_part = strchr (charset_part, '-')) == NULL)
            return FALSE;
        if (*(++charset_part) == '\0')
            return FALSE;
    }

    if ((charset_part = strchr (charset_part, ',')) == NULL)
        return FALSE;

    if (*(++charset_part) == '\0')
        return FALSE;

    strncpy (charset, charset_part, LEN_LOGFONT_NAME_FIELD);
    charset [LEN_LOGFONT_NAME_FIELD] = '\0';
    return TRUE;
}

BOOL fontGetCharsetPartFromName (const char* name, char* charset)
{
    int i;
    const char* charset_part = name;

    for (i = 0; i < NR_LOOP_FOR_CHARSET; i++) {
        if ((charset_part = strchr (charset_part, '-')) == NULL)
            return FALSE;
        if (*(++charset_part) == '\0')
            return FALSE;
    }

    strncpy (charset, charset_part, LEN_DEVFONT_NAME);
    charset [LEN_DEVFONT_NAME] = '\0';
    return TRUE;
}

int charsetGetCharsetsNumber (const char* charsets)
{
    int n = 1;

    while (1) {
        if ((charsets = strchr (charsets, ',')) == NULL)
            break;

        charsets ++;
        n ++;
    }

    return n;
}

BOOL charsetGetSpecificCharset (const char* charsets, int _index, char* charset)
{
    int i;
    char* delim;

    for (i = 0; i < _index && charsets; i++) {
        charsets = strchr (charsets, ',');
        if (charsets)
            charsets ++;
    }

    if (charsets == NULL)
        return FALSE;

    if ((delim = strchr (charsets, ','))) {
        int len;
        len = delim - charsets;
        strncpy (charset, charsets, len);
        charset [len] = '\0';
        return TRUE;
    }

    strncpy (charset, charsets, LEN_LOGFONT_NAME_FIELD);
    charset [LEN_LOGFONT_NAME_FIELD] = '\0';
    return TRUE;
}

#if 0
 /* type-family-style-width-height-charset-encoding1[,encoding2,...] */
BOOL fontCreateFontName (char* devfont_name, const char* type,
        const char* family_name,  const char *style, int w, int h,
        const char* charset_encoding)
{
    const char def_type [] = FONT_TYPE_NAME_ALL;
    const char def_family_name [] = "*";
    const char def_style [] = "*";
    const char def_charset_encoding [] = "ISO8859-1";

    if (devfont_name == NULL)
        return FALSE;

    if (type == NULL)
        type = def_type;

    if (family_name == NULL)
        family_name = def_family_name;

    if (file_style == NULL)
        file_style = def_file_style;

    if (style == NULL)
        style = def_style;

    if (charset_encoding == NULL)
        charset_encoding = def_charset_encoding;

    sprintf(devfont_name, "%s-%s-%s-%d-%d-%s",
            type, family_name, style, w, h, charset_encoding);

    return TRUE;
}
#endif

