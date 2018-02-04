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
** logfont.c: Log fonts management.
**
** Current maintainer: Wei Yongming.
** 
** Created by Wei Yongming
**
** Create date: 2000/07/07
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"

#include "sysfont.h"
#include "devfont.h"
#include "fontname.h"

CHARSETOPS* GetCharsetOps (const char* charset_name);

/************************** Exported functions ******************************/
static PLOGFONT gdiCreateLogFont ( const char* type, const char* family, 
    const char* charset, DWORD style, int size, int rotation)
{
    PLOGFONT log_font;
    int sbc_descent, mbc_descent = 0;
    char dev_family [LEN_FONT_NAME + 1];
    DEVFONT* sbc_devfont, *mbc_devfont;

    // is valid style?
    if (style == 0xFFFFFFFF)
        return INV_LOGFONT;

    // is supported charset?
    if (GetCharsetOps (charset) == NULL) {
        return INV_LOGFONT;
    }

    if ((log_font = malloc (sizeof (LOGFONT))) == NULL)
        return INV_LOGFONT;
    
    log_font->style = style;

#if 0
    fprintf (stderr, "LogFont: style: %x\n", log_font->style);
#endif

    if (type) {
        strncpy (log_font->type, type, LEN_FONT_NAME);
        log_font->type [LEN_FONT_NAME] = '\0';
    }
    else
        strcpy (log_font->type, FONT_TYPE_NAME_ALL);

    strncpy (log_font->family, family, LEN_FONT_NAME);
    log_font->family [LEN_FONT_NAME] = '\0';

    strncpy (log_font->charset, charset, LEN_FONT_NAME);
    log_font->charset [LEN_FONT_NAME] = '\0';

    if (size > FONT_MAX_SIZE)
        log_font->size = FONT_MAX_SIZE;
    else if (size < FONT_MIN_SIZE)
        log_font->size = FONT_MIN_SIZE;
    else
        log_font->size = size;

    log_font->rotation = rotation;

#if 0
    fprintf (stderr, "log_font: %s, %s, %s, %d.\n",
                    log_font->type, log_font->family, log_font->charset,
                    log_font->size);
#endif

    sbc_devfont = font_GetMatchedSBDevFont (log_font);
    if (sbc_devfont->font_ops->new_instance)
        sbc_devfont = (*sbc_devfont->font_ops->new_instance) (log_font, sbc_devfont, TRUE);
    if (sbc_devfont == NULL) {
        free (log_font);
        return INV_LOGFONT;
    }

    mbc_devfont = font_GetMatchedMBDevFont (log_font);
    if (mbc_devfont && mbc_devfont->font_ops->new_instance)
        mbc_devfont = (*mbc_devfont->font_ops->new_instance) (log_font, mbc_devfont, FALSE);

    log_font->sbc_devfont = sbc_devfont;
    log_font->mbc_devfont = mbc_devfont;

    /* 
     * Adjust the logical font information
     */

    // family name
    if (log_font->mbc_devfont) {
        fontGetFamilyFromName (log_font->mbc_devfont->name, dev_family);
        strncpy (log_font->family, dev_family, LEN_FONT_NAME);
        log_font->family [LEN_FONT_NAME] = '\0';
    }
    else {
        fontGetFamilyFromName (log_font->sbc_devfont->name, dev_family);
        strncpy (log_font->family, dev_family, LEN_FONT_NAME);
        log_font->family [LEN_FONT_NAME] = '\0';
    }

    // charset name
    if (log_font->mbc_devfont) {
        strncpy (log_font->charset, 
            log_font->mbc_devfont->charset_ops->name, LEN_FONT_NAME);
        log_font->charset [LEN_FONT_NAME] = '\0';
    }
    else {
        strncpy (log_font->charset, 
            log_font->sbc_devfont->charset_ops->name, LEN_FONT_NAME);
        log_font->charset [LEN_FONT_NAME] = '\0';
    }

    // size
    log_font->size = (*log_font->sbc_devfont->font_ops->get_font_height)
            (log_font, log_font->sbc_devfont);

    if (log_font->mbc_devfont) {
        int size = (*log_font->mbc_devfont->font_ops->get_font_height)
            (log_font, log_font->mbc_devfont);

        if (size > log_font->size)
            log_font->size = size;
    }

    sbc_descent = (*log_font->sbc_devfont->font_ops->get_font_descent)
            (log_font, log_font->sbc_devfont);

    if (log_font->mbc_devfont) {
        mbc_descent = (*log_font->mbc_devfont->font_ops->get_font_descent)
            (log_font, log_font->mbc_devfont);
    }
    else
        mbc_descent = sbc_descent;

    log_font->size += ABS (sbc_descent - mbc_descent);

    return log_font;
}

PLOGFONT GUIAPI CreateLogFontIndirect (LOGFONT *logfont)
{
    return gdiCreateLogFont (logfont->type, logfont->family, 
        logfont->charset, logfont->style, logfont->size, logfont->rotation);
}

PLOGFONT GUIAPI CreateLogFont (const char* type, const char* family, 
    const char* charset, char weight, char slant, char set_width, 
    char spacing, char underline, char struckout, int size, int rotation)
{
    DWORD style;
    char style_name [7];
    style_name[0] = weight;
    style_name[1] = slant;
    style_name[2] = set_width;
    style_name[3] = spacing;
    style_name[4] = underline;
    style_name[5] = struckout;
    style_name[6] = 0;
    
    if ((style = fontConvertStyle (style_name)) == 0xFFFFFFFF) {
        return INV_LOGFONT;
    }

    return gdiCreateLogFont (type, family, charset, style, size, rotation);
}

PLOGFONT GUIAPI CreateLogFontByName (const char* font_name)
{
    char type[LEN_FONT_NAME + 1];
    char family[LEN_FONT_NAME + 1];
    char charset[LEN_FONT_NAME + 1];
    DWORD style;
    int height;

    if (!fontGetTypeNameFromName (font_name, type) ||
            !fontGetFamilyFromName (font_name, family) ||
            !fontGetCharsetFromName (font_name, charset) ||
            ((height = fontGetHeightFromName (font_name)) == -1) ||
            ((style = fontGetStyleFromName (font_name)) == 0xFFFFFFFF))
        return NULL;

    return gdiCreateLogFont (type, family, charset, style, height, 0);
}

void GUIAPI DestroyLogFont (PLOGFONT log_font)
{
    LOGFONT* logfont = (PLOGFONT)log_font;

    if (log_font == NULL)
        return;

    if (logfont->sbc_devfont->font_ops->delete_instance)
        (*logfont->sbc_devfont->font_ops->delete_instance) (logfont->sbc_devfont);
    if (logfont->mbc_devfont && logfont->mbc_devfont->font_ops->delete_instance)
        (*logfont->mbc_devfont->font_ops->delete_instance) (logfont->mbc_devfont);

    free (logfont);
}

void GUIAPI GetLogFontInfo (HDC hdc, LOGFONT* log_font)
{
    memcpy (log_font, dc_HDC2PDC (hdc)->pLogFont, sizeof (LOGFONT));
}

void GUIAPI GetFontMetrics (LOGFONT* log_font, FONTMETRICS* font_metrics)
{
    int sbc_value, mbc_value;
    
    sbc_value = log_font->sbc_devfont->font_ops->get_font_height (log_font, log_font->sbc_devfont);
    if (log_font->mbc_devfont) {
        mbc_value = log_font->mbc_devfont->font_ops->get_font_height (log_font, log_font->mbc_devfont);
        font_metrics->font_height = MAX (sbc_value, mbc_value);
    }
    else {
        font_metrics->font_height = sbc_value;
    }

    sbc_value = log_font->sbc_devfont->font_ops->get_font_ascent (log_font, log_font->sbc_devfont);
    if (log_font->mbc_devfont) {
        mbc_value = log_font->mbc_devfont->font_ops->get_font_ascent (log_font, log_font->mbc_devfont);
        font_metrics->ascent = MAX (sbc_value, mbc_value);
    }
    else {
        font_metrics->ascent = sbc_value;
    }

    sbc_value = log_font->sbc_devfont->font_ops->get_font_descent (log_font, log_font->sbc_devfont);
    if (log_font->mbc_devfont) {
        mbc_value = log_font->mbc_devfont->font_ops->get_font_descent (log_font, log_font->mbc_devfont);
        font_metrics->descent = MAX (sbc_value, mbc_value);
    }
    else {
        font_metrics->descent = sbc_value;
    }

    sbc_value = log_font->sbc_devfont->font_ops->get_max_width (log_font, log_font->sbc_devfont);
    if (log_font->mbc_devfont) {
        mbc_value = log_font->mbc_devfont->font_ops->get_max_width (log_font, log_font->mbc_devfont);
        font_metrics->max_width = MAX (sbc_value, mbc_value);
    }
    else {
        font_metrics->max_width = sbc_value;
    }

    sbc_value = log_font->sbc_devfont->font_ops->get_ave_width (log_font, log_font->sbc_devfont);
    if (log_font->mbc_devfont) {
        mbc_value = log_font->mbc_devfont->font_ops->get_ave_width (log_font, log_font->mbc_devfont);
        font_metrics->ave_width = mbc_value;
    }
    else {
        font_metrics->ave_width = sbc_value;
    }
}

void GUIAPI GetGlyphBitmap (LOGFONT* log_font, const unsigned char* mchar, int mchar_len, 
                GLYPHBITMAP* glyph_bitmap)
{
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
    DEVFONT* devfont;

    if (mbc_devfont) {
        int mbc_pos;

        mbc_pos = mbc_devfont->charset_ops->pos_first_char(mchar, mchar_len);
        if (mbc_pos == 0) {
            devfont = mbc_devfont;
        }
        else {
            devfont = sbc_devfont;
        }
    }
    else {
        devfont = sbc_devfont;
    }

    if (devfont->font_ops->get_char_bbox) {
        glyph_bitmap->bbox_x = 0;
        glyph_bitmap->bbox_y = 0;
        devfont->font_ops->get_char_bbox (log_font, devfont, mchar, mchar_len,
                    &glyph_bitmap->bbox_x,
                    &glyph_bitmap->bbox_y,
                    &glyph_bitmap->bbox_w,
                    &glyph_bitmap->bbox_h);
    }
    else {
        glyph_bitmap->bbox_x = 0;
        glyph_bitmap->bbox_y = devfont->font_ops->get_font_descent (log_font, devfont);
        glyph_bitmap->bbox_w = devfont->font_ops->get_char_width (log_font, devfont, mchar, mchar_len);
        glyph_bitmap->bbox_h = devfont->font_ops->get_font_height (log_font, devfont);
    }

    if (devfont->font_ops->get_char_advance) {
        devfont->font_ops->get_char_advance (log_font, devfont, mchar, mchar_len,
                    &glyph_bitmap->advance_x,
                    &glyph_bitmap->advance_y);
    }
    else {
        glyph_bitmap->advance_x = glyph_bitmap->bbox_w;
        glyph_bitmap->advance_y = 0;
    }

    devfont->font_ops->get_char_advance (log_font, devfont, mchar, mchar_len,
                    &glyph_bitmap->advance_x,
                    &glyph_bitmap->advance_y);

    glyph_bitmap->bmp_size = devfont->font_ops->char_bitmap_size (log_font, devfont, mchar, mchar_len);

    glyph_bitmap->bmp_pitch = (glyph_bitmap->bbox_w + 7) >> 3;

    glyph_bitmap->bits = devfont->font_ops->get_char_bitmap (log_font, devfont, mchar, mchar_len);
}

