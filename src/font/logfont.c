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
** reflf.c: Log fonts management.
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

static inline int get_rotation (LOGFONT* reflf,
        DEVFONT* devfont, int rot_desired)
{
    if (devfont->font_ops->is_rotatable)
        return devfont->font_ops->is_rotatable(reflf, devfont, rot_desired);
    else
        return 0;
}

static int get_family_name_len (const char* family)
{
    int n = 0;
    while (family[n] && family[n] != ',') {
        n++;
    }

    return n;
}

/* Adjust the logical font information */
static void adjust_newlf_info (LOGFONT* newlf)
{
    int i;

    i = 0;
    newlf->ascent = 0;
    while (newlf->devfonts[i]) {
        int value = newlf->devfonts[i]->font_ops->get_font_ascent(
                        newlf, newlf->devfonts[i]);
        newlf->ascent = MAX (newlf->ascent, value);
        i++;
    }

    i = 0;
    newlf->descent = 0;
    while (newlf->devfonts[i]) {
        int value = newlf->devfonts[i]->font_ops->get_font_descent(
                        newlf, newlf->devfonts[i]);
        newlf->descent = MAX (newlf->descent, value);
        i++;
    }

    newlf->size = newlf->ascent + newlf->descent;
}

static PLOGFONT gdiCreateLogFont (const char* type, const char* family,
    const char* charset, DWORD style, int size, int rotation)
{
    PLOGFONT newlf;
    char name_field[LEN_LOGFONT_NAME_FIELD + 1];
    DEVFONT* devfonts[MAXNR_DEVFONTS] = {};
    const char* iter;
    int i, n;

    // is valid style?
    if (style == 0xFFFFFFFF)
        return INV_LOGFONT;

    // is supported charset?
    if (GetCharsetOps(charset) == NULL) {
        _ERR_PRINTF("FONT>LogFont: Not supprted charset: %s.\n", charset);
        return INV_LOGFONT;
    }

    if ((newlf = (PLOGFONT)calloc(sizeof (FONT_RES), 1)) == NULL)
        return INV_LOGFONT;

    ((FONT_RES *)newlf)->key = -1;

    newlf->style = style;
    if (type == NULL)
        type = FONT_TYPE_NAME_ALL;
    newlf->type = strdup(type);
    newlf->family = strdup(family);
    newlf->charset = strdup(charset);

    if (newlf->type == NULL || newlf->family == NULL || newlf->charset == NULL)
        goto error;

    if (size > FONT_MAX_SIZE)
        newlf->size = FONT_MAX_SIZE;
    else if (size < FONT_MIN_SIZE)
        newlf->size = FONT_MIN_SIZE;
    else
        newlf->size = size;
    newlf->size_request = newlf->size;

    newlf->rotation = rotation;

    _DBG_PRINTF ("%s: requested LOGFONT: %s-%s-%x-%d-%s.\n",
            __FUNCTION__,
            newlf->type, newlf->family,
            newlf->style, newlf->size, newlf->charset);

    iter = family;
    if ((n = get_family_name_len(iter)) <= 0 || n > LEN_LOGFONT_NAME_FIELD)
        goto error;

    strncpy (name_field, iter, n);
    name_field[n] = '\0';

    _DBG_PRINTF ("%s: try to create SBC Devfont for family(%s)\n",
            __FUNCTION__, name_field);

    newlf->scales[0] = 1;
    if ((devfonts[0] = font_GetMatchedSBDevFont(newlf, name_field)) == NULL)
        goto error;

    iter = family;
    for (i = 1; i < MAXNR_DEVFONTS; i++) {
        DEVFONT* df;

        if ((n = get_family_name_len(iter)) <= 0 || n > LEN_LOGFONT_NAME_FIELD)
            break;
        strncpy (name_field, iter, n);
        name_field[n] = '\0';

        _DBG_PRINTF("%s: try to create MBC Devfont for family(%s)\n",
                __FUNCTION__, name_field);

        newlf->scales[i] = 1;
        if ((df = font_GetMatchedMBDevFont (newlf, name_field, i))) {
            int j;
            // check duplicated.
            for (j = 1; j <= i; j++) {
                if (df == devfonts[j]) {
                    // duplicated
                    _DBG_PRINTF("%s: ignore the duplicated devfont (%s)\n",
                            __FUNCTION__, name_field);
                    break;
                }
                else if (devfonts[j] == NULL) {
                    devfonts[j] = df;
                    _DBG_PRINTF("%s: created new devfont for family(%s)\n",
                            __FUNCTION__, name_field);
                    break;
                }
            }
        }
        else {
            _DBG_PRINTF("%s: failed to created new devfont for family(%s)\n",
                            __FUNCTION__, name_field);
        }

        iter += n;
        if (*iter == ',' || *iter == ' ')
            iter++;
    }

    // check if all devfonts support rotation
    if (rotation) {
        for (i = 0; i < MAXNR_DEVFONTS; i++) {
            if (devfonts[i] &&
                    get_rotation(newlf, devfonts[i], rotation) != rotation) {
                newlf->rotation = 0;
                break;
            }
        }
    }

    // create new instance of devfont if need
    for (i = 0; i < MAXNR_DEVFONTS; i++) {
        if (devfonts[i] && devfonts[i]->font_ops->new_instance) {
            devfonts[i] = devfonts[i]->font_ops->new_instance(newlf,
                    devfonts[i], i == 0);
            if (devfonts[i] == NULL)
                goto error;
        }
    }

    memcpy(newlf->devfonts, devfonts, sizeof(devfonts));

    adjust_newlf_info(newlf);

    _DBG_PRINTF ("%s: NEW LOGFONT: %s-%s-%x-%d-%s.\n",
            __FUNCTION__,
            newlf->type, newlf->family, newlf->style,
            newlf->size, newlf->charset);

    return newlf;

error:
    for (i = 0; i < MAXNR_DEVFONTS; i++) {
        if (newlf->devfonts[i] && newlf->devfonts[i]->font_ops->delete_instance
                && newlf->devfonts[i] != devfonts[i]) {
            newlf->devfonts[i]->font_ops->delete_instance(newlf->devfonts[i]);
        }
    }

    if (newlf->type) free(newlf->type);
    if (newlf->family) free(newlf->family);
    if (newlf->charset) free(newlf->charset);

    free(newlf);

    return INV_LOGFONT;
}

PLOGFONT GUIAPI CreateLogFontIndirect (LOGFONT *reflf)
{
    PLOGFONT newlf;
    int i;

    if (!reflf)
        return NULL;

    // VincentWei: make sure the reflf has the key for resource manager.
    if ((newlf = (PLOGFONT)calloc (sizeof (FONT_RES), 1)) == NULL)
        return INV_LOGFONT;
    // VincentWei: make sure the reflf has an invalid key for resource manager.
    ((FONT_RES *)newlf)->key = -1;

    memcpy (newlf, reflf, sizeof(LOGFONT));
    newlf->type = strdup(reflf->type);
    newlf->family = strdup(reflf->family);
    newlf->charset = strdup(reflf->charset);
    if (newlf->type == NULL || newlf->family == NULL ||
            newlf->charset == NULL)
        goto error;


    // reset request size of newlf to reflf->size_request
    newlf->size = reflf->size_request;
    if (newlf->size > FONT_MAX_SIZE)
        newlf->size = FONT_MAX_SIZE;
    else if (newlf->size < FONT_MIN_SIZE)
        newlf->size = FONT_MIN_SIZE;

    // create new devfont instance if need
    for (i = 0; i < MAXNR_DEVFONTS; i++) {
        DEVFONT* df = reflf->devfonts[i];
        if (df == NULL) {
            goto error;
        }
        if (df->font_ops->new_instance)
            newlf->devfonts[i] = df->font_ops->new_instance(newlf,
                df, i == 0);
    }

    adjust_newlf_info(newlf);

    return newlf;

error:
    for (i = 0; i < MAXNR_DEVFONTS; i++) {
        if (newlf->devfonts[i] && newlf->devfonts[i]->font_ops->delete_instance
                && newlf->devfonts[i] != reflf->devfonts[i]) {
            newlf->devfonts[i]->font_ops->delete_instance(newlf->devfonts[i]);
        }
    }

    if (newlf->type) free(newlf->type);
    if (newlf->family) free(newlf->family);
    if (newlf->charset) free(newlf->charset);

    free(newlf);

    return INV_LOGFONT;
}

PLOGFONT GUIAPI CreateLogFontIndirectEx (LOGFONT *reflf, int rotation)
{
    PLOGFONT newlf;
    int i;

    if (!reflf)
        return NULL;

    // VincentWei: make sure the reflf has the key for resource manager.
    if ((newlf = (PLOGFONT)calloc (sizeof (FONT_RES), 1)) == NULL)
        return INV_LOGFONT;
    // VincentWei: make sure the reflf has an invalid key for resource manager.
    ((FONT_RES *)newlf)->key = -1;

    memcpy (newlf, reflf, sizeof(LOGFONT));
    newlf->type = strdup(reflf->type);
    newlf->family = strdup(reflf->family);
    newlf->charset = strdup(reflf->charset);
    if (newlf->type == NULL || newlf->family == NULL ||
            newlf->charset == NULL)
        goto error;

    // reset request size of newlf to reflf->size_request
    newlf->size = reflf->size_request;
    if (newlf->size > FONT_MAX_SIZE)
        newlf->size = FONT_MAX_SIZE;
    else if (newlf->size < FONT_MIN_SIZE)
        newlf->size = FONT_MIN_SIZE;
    newlf->rotation = rotation;

    // check if all devfonts support rotation
    if (rotation) {
        for (i = 0; i < MAXNR_DEVFONTS; i++) {
            DEVFONT* df = newlf->devfonts[i];
            if (df && get_rotation(newlf, df, rotation) != rotation) {
                newlf->rotation = 0;
                break;
            }
        }
    }

    // create new devfont instance if need
    for (i = 0; i < MAXNR_DEVFONTS; i++) {
        DEVFONT* df = reflf->devfonts[i];
        if (df && df->font_ops->new_instance) {
            newlf->devfonts[i] = df->font_ops->new_instance(newlf, df, i == 0);
            if (newlf->devfonts[i] == NULL) {
                goto error;
            }
        }
    }

    adjust_newlf_info(newlf);
    return newlf;

error:
    for (i = 0; i < MAXNR_DEVFONTS; i++) {
        if (newlf->devfonts[i] && newlf->devfonts[i]->font_ops->delete_instance
                && newlf->devfonts[i] != reflf->devfonts[i]) {
            newlf->devfonts[i]->font_ops->delete_instance(newlf->devfonts[i]);
        }
    }

    if (newlf->type) free(newlf->type);
    if (newlf->family) free(newlf->family);
    if (newlf->charset) free(newlf->charset);

    free(newlf);

    return INV_LOGFONT;
}

PLOGFONT GUIAPI CreateLogFont (const char* type, const char* family,
    const char* charset, char weight, char slant, char flip,
    char other, char underline, char struckout, int size, int rotation)
{
    char style_name [7];
    style_name[0] = weight;
    style_name[1] = slant;
    style_name[2] = flip;
    style_name[3] = other;
    style_name[5] = FONT_RENDER_ANY;

    if (weight == FONT_WEIGHT_BOOK) {
        style_name[0] = FONT_WEIGHT_REGULAR;
        style_name[5] = FONT_RENDER_GREY;
    }
    else if (weight == FONT_WEIGHT_SUBPIXEL) {
        style_name[0] = FONT_WEIGHT_REGULAR;
        style_name[5] = FONT_RENDER_SUBPIXEL;
    }

    if (underline == FONT_UNDERLINE_LINE) {
        if (struckout == FONT_STRUCKOUT_LINE) {
            style_name[4] = FONT_DECORATE_US;
        }
        else {
            style_name[4] = FONT_DECORATE_UNDERLINE;
        }
    }
    else {
        if (struckout == FONT_STRUCKOUT_LINE) {
            style_name[4] = FONT_DECORATE_STRUCKOUT;
        }
        else {
            style_name[4] = FONT_DECORATE_NONE;
        }
    }

    style_name[6] = 0;

    return gdiCreateLogFont (type, family, charset,
            fontConvertStyle (style_name), size, rotation);
}

PLOGFONT GUIAPI CreateLogFontEx (const char* type, const char* family,
        const char* charset, char weight, char slant, char flip,
        char other, char decoration, char rendering,
        int size, int rotation)
{
    char style_name [7];
    style_name[0] = weight;
    style_name[1] = slant;
    style_name[2] = flip;
    style_name[3] = other;
    style_name[4] = decoration;
    style_name[5] = rendering;
    style_name[6] = 0;

    return gdiCreateLogFont (type, family, charset,
            fontConvertStyle (style_name), size, rotation);
}

PLOGFONT GUIAPI CreateLogFontByName (const char* font_name)
{
    char type[LEN_LOGFONT_NAME_FIELD + 1];
    char family[LEN_LOGFONT_FAMILY_FIELD + 1];
    char charset[LEN_LOGFONT_NAME_FIELD + 1];
    DWORD style;
    int height;
    int rotation;

    if (!fontGetTypeNameFromName (font_name, type) ||
            !fontGetFamilyFromName (font_name, family) ||
            !fontGetCharsetFromName (font_name, charset) ||
            ((height = fontGetHeightFromName (font_name)) == -1) ||
            ((style = fontGetStyleFromName (font_name)) == 0xFFFFFFFF))
        return NULL;

    switch (fontGetOrientFromName (font_name)) {
    case FONT_ORIENT_SIDEWAYS:
        rotation = -900;
        break;

    case FONT_ORIENT_UPSIDE_DOWN:
        rotation = 1800;
        break;

    case FONT_ORIENT_SIDEWAYS_LEFT:
        rotation = 900;
        break;

    case FONT_ORIENT_UPRIGHT:
    default:
        rotation = 0;
        break;
    }

    return gdiCreateLogFont (type, family, charset, style, height, rotation);
}

PLOGFONT GUIAPI CreateLogFontForMChar2UChar(const char* charset)
{
    char my_fontname[LEN_LOGFONT_NAME_FULL + 1];

    memset(my_fontname, 0, LEN_LOGFONT_NAME_FULL + 1);
    strncpy(my_fontname, "nuf-dummy-rrncnn-U-1-", LEN_LOGFONT_NAME_FULL);
    strncat(my_fontname, charset, LEN_LOGFONT_NAME_FIELD);

    return CreateLogFontByName(my_fontname);
}

void GUIAPI DestroyLogFont (PLOGFONT logfont)
{
    int i;

    if (!logfont)
        return;

    for (i = 0; i < MAXNR_DEVFONTS; i++) {
        DEVFONT* df = logfont->devfonts[i];
        if (df && df->font_ops->delete_instance)
            df->font_ops->delete_instance(df);
    }

    free(logfont->type);
    free(logfont->family);
    free(logfont->charset);

    free(logfont);
}

void GUIAPI GetLogFontInfo (HDC hdc, LOGFONT* logfont)
{
    if (!logfont)
        return;

    /*
     * NOTE: change the size to copy when LOGFONT structure changed
     * The function only copy the opened fields of the LOGFONT.
     */
    memcpy (logfont, dc_HDC2PDC (hdc)->pLogFont, sizeof (LOGFONT));
}

void GUIAPI GetFontMetrics (LOGFONT* logfont, FONTMETRICS* font_metrics)
{
    int i;
    DEVFONT* df;

    if (!logfont) return;
    if (!font_metrics) return;

    font_metrics->font_height = logfont->size;
    font_metrics->ascent = logfont->ascent;
    font_metrics->descent = logfont->descent;

    i = 0;
    font_metrics->max_width = 0;
    while ((df = logfont->devfonts[i])) {
        int value = df->font_ops->get_max_width(logfont, df);
        font_metrics->max_width = MAX(font_metrics->max_width, value);
        i++;
    }

    /* FIXME: ave_width should depend on the glyph number of devfont */
    i = 0;
    font_metrics->ave_width = 0;
    while ((df = logfont->devfonts[i])) {
        int value = df->font_ops->get_max_width(logfont, df);
        font_metrics->ave_width += value;
        i++;
    }

    if (i > 0)
        font_metrics->ave_width /= i;
}

