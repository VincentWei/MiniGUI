/*
** $Id: logfont.c 13674 2010-12-06 06:45:01Z wanzheng $
** 
** logfont.c: Log fonts management.
**
** Copyright (C) 2003 ~ 2008 Feynman Software
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** All right reserved by Feynman Software.
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

static inline int get_rotation (LOGFONT* logfont,
        DEVFONT* devfont, int rot_desired)
{
    if (devfont->font_ops->is_rotatable)
        return devfont->font_ops->is_rotatable (logfont, devfont, rot_desired);
    else
        return 0;
}

/************************** Exported functions ******************************/
static PLOGFONT gdiCreateLogFont (const char* type, const char* family, 
    const char* charset, DWORD style, int size, int rotation)
{
    PLOGFONT log_font;
    int sbc_value, mbc_value = 0;
    char dev_family [LEN_FONT_NAME + 1];
    DEVFONT* sbc_devfont, *mbc_devfont;
    DWORD glyph_bmptype;

    // is valid style?
    if (style == 0xFFFFFFFF)
        return INV_LOGFONT;

    // is supported charset?
    if (GetCharsetOps (charset) == NULL) {
        _MG_PRINTF ("FONT>LogFont: Not supprted charset: %s.\n", charset);
        return INV_LOGFONT;
    }

    /*
     * fix "Invalid read of size 4" in mgncs
     */
    //if ((log_font = (malloc (sizeof (LOGFONT))) == NULL)
    if ((log_font = (PLOGFONT)malloc (sizeof (FONT_RES))) == NULL)
        return INV_LOGFONT;

    /*
     * fix "Invalid read of size 4" in mgncs
     */
    ((FONT_RES *)log_font)->key = -1;

    log_font->style = style;

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
    log_font->sbc_scale = 1;
    log_font->mbc_scale = 1;

    _MG_PRINTF ("FONT>LogFont: requested info: type: %s, family: %s, style: %p, charset: %s, size: %d.\n",
                    log_font->type, log_font->family, (PVOID)log_font->style, log_font->charset,
                    log_font->size);

    sbc_devfont = font_GetMatchedSBDevFont (log_font);
    mbc_devfont = font_GetMatchedMBDevFont (log_font);

    /* return error */
    if (sbc_devfont == NULL) {
        free (log_font);
        return INV_LOGFONT;
    }

    /*check if sbc_devfont and mbc_devfont support rotation*/
    if (get_rotation(log_font, sbc_devfont, rotation) == rotation
            && (!mbc_devfont ||
            get_rotation(log_font, mbc_devfont, rotation) == rotation))
        log_font->rotation = rotation;
    else
        log_font->rotation = 0;

    if (sbc_devfont->font_ops->new_instance)
        sbc_devfont = (*sbc_devfont->font_ops->new_instance) (log_font, sbc_devfont, TRUE);
    if (sbc_devfont == NULL) {
        free (log_font);
        return INV_LOGFONT;
    }

    if (mbc_devfont && mbc_devfont->font_ops->new_instance)
        mbc_devfont = (*mbc_devfont->font_ops->new_instance) (log_font, mbc_devfont, FALSE);

    log_font->sbc_devfont = sbc_devfont;
    log_font->mbc_devfont = mbc_devfont;

    /* 
     * Adjust the logical font information
     */

    /*reset type name of logfont*/
    if (log_font->mbc_devfont) {
        fontGetTypeNameFromName (log_font->mbc_devfont->name, dev_family);
        strncpy (log_font->type, dev_family, LEN_FONT_NAME);
        log_font->type[LEN_FONT_NAME] = '\0';
    }
    else {
        fontGetTypeNameFromName (log_font->sbc_devfont->name, dev_family);
        strncpy (log_font->type, dev_family, LEN_FONT_NAME);
        log_font->type[LEN_FONT_NAME] = '\0';
    }

    /*reset family name of logfont*/
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

    /*reset charset name of logfont*/
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

    /*reset ascent of logfont*/
    sbc_value = log_font->sbc_devfont->font_ops->get_font_ascent (log_font, 
                    log_font->sbc_devfont);
    if (log_font->mbc_devfont) {
        mbc_value = log_font->mbc_devfont->font_ops->get_font_ascent (log_font,
                        log_font->mbc_devfont);
        log_font->ascent = MAX (sbc_value, mbc_value);
    }
    else {
        log_font->ascent = sbc_value;
    }

    /*reset descent of logfont*/
    sbc_value = log_font->sbc_devfont->font_ops->get_font_descent (log_font, 
                    log_font->sbc_devfont);
    if (log_font->mbc_devfont) {
        mbc_value = log_font->mbc_devfont->font_ops->get_font_descent (log_font,
                        log_font->mbc_devfont);
        log_font->descent = MAX (sbc_value, mbc_value);
    }
    else {
        log_font->descent = sbc_value;
    }

    /*reset size of logfont*/
    log_font->size = log_font->ascent + log_font->descent;

    /* reset style of logfont */
    /* remove HORZVERT style if weight is subpixel */
    if (log_font->style & FS_WEIGHT_SUBPIXEL) {
        log_font->style &= ~FS_FLIP_HORZVERT;
        log_font->style &= ~FS_SLANT_ITALIC;
    }

    /* remove HORZVERT style if the glyph bmptype is subpixel */
    glyph_bmptype = log_font->sbc_devfont->font_ops->get_glyph_type 
                (log_font, log_font->sbc_devfont) & DEVFONTGLYPHTYPE_MASK_BMPTYPE;
    if (glyph_bmptype == GLYPHBMP_TYPE_SUBPIXEL) {
        log_font->style &= ~FS_FLIP_HORZVERT;
    }
    else if (log_font->mbc_devfont) {
        glyph_bmptype = log_font->mbc_devfont->font_ops->get_glyph_type 
                    (log_font, log_font->mbc_devfont) & DEVFONTGLYPHTYPE_MASK_BMPTYPE;
        if (glyph_bmptype == GLYPHBMP_TYPE_SUBPIXEL)
            log_font->style &= ~FS_FLIP_HORZVERT;
    }

    _MG_PRINTF ("FONT>LogFont: created info: type: %s, family: %s, style: %p, charset: %s, size: %d.\n",
                    log_font->type, log_font->family, (PVOID)log_font->style, log_font->charset,
                    log_font->size);

    return log_font;
}

PLOGFONT GUIAPI CreateLogFontIndirect (LOGFONT *logfont)
{
    PLOGFONT font;
    DEVFONT* sbc_devfont, *mbc_devfont;

    if (!logfont) return NULL;

    if ((font = malloc (sizeof (LOGFONT))) == NULL)
        return INV_LOGFONT;
    
    memcpy (font, logfont, sizeof(LOGFONT));

    sbc_devfont = logfont->sbc_devfont;
    if (sbc_devfont->font_ops->new_instance)
        sbc_devfont = (*sbc_devfont->font_ops->new_instance) (logfont, sbc_devfont, TRUE);
    if (sbc_devfont == NULL) {
        free (font);
        return INV_LOGFONT;
    }

    mbc_devfont = logfont->mbc_devfont;
    if (mbc_devfont && mbc_devfont->font_ops->new_instance)
        mbc_devfont = (*mbc_devfont->font_ops->new_instance) (logfont, mbc_devfont, FALSE);

    font->sbc_devfont = sbc_devfont;
    font->mbc_devfont = mbc_devfont;

    return font;
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
    style_name[4] = underline;
    style_name[5] = struckout;
    style_name[6] = 0;
    
    return gdiCreateLogFont (type, family, charset, fontConvertStyle (style_name), size, rotation);
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

    if (!log_font) return;

    if (logfont->sbc_devfont->font_ops->delete_instance)
        logfont->sbc_devfont->font_ops->delete_instance (logfont->sbc_devfont);
    if (logfont->mbc_devfont && logfont->mbc_devfont->font_ops->delete_instance)
        logfont->mbc_devfont->font_ops->delete_instance (logfont->mbc_devfont);

    free (logfont);
}

void GUIAPI GetLogFontInfo (HDC hdc, LOGFONT* log_font)
{
    if (!log_font) return;
    memcpy (log_font, dc_HDC2PDC (hdc)->pLogFont, sizeof (LOGFONT));
}

void GUIAPI GetFontMetrics (LOGFONT* log_font, FONTMETRICS* font_metrics)
{
    int sbc_value, mbc_value;
    
    if (!log_font) return;
    if (!font_metrics) return;

    font_metrics->font_height = log_font->size;

    sbc_value = log_font->sbc_devfont->font_ops->get_font_ascent (log_font, 
                    log_font->sbc_devfont);
    if (log_font->mbc_devfont) {
        mbc_value = log_font->mbc_devfont->font_ops->get_font_ascent (log_font,
                        log_font->mbc_devfont);
        font_metrics->ascent = MAX (sbc_value, mbc_value);
    }
    else {
        font_metrics->ascent = sbc_value;
    }

    font_metrics->descent = font_metrics->font_height - font_metrics->ascent;

    sbc_value = log_font->sbc_devfont->font_ops->get_max_width (log_font, 
                    log_font->sbc_devfont);
    if (log_font->mbc_devfont) {
        mbc_value = log_font->mbc_devfont->font_ops->get_max_width (log_font, 
                        log_font->mbc_devfont);
        font_metrics->max_width = MAX (sbc_value, mbc_value);
    }
    else {
        font_metrics->max_width = sbc_value;
    }

    sbc_value = log_font->sbc_devfont->font_ops->get_ave_width (log_font, 
                    log_font->sbc_devfont);
    if (log_font->mbc_devfont) {
        mbc_value = log_font->mbc_devfont->font_ops->get_ave_width (log_font, 
                        log_font->mbc_devfont);
        font_metrics->ave_width = mbc_value;
    }
    else {
        font_metrics->ave_width = sbc_value;
    }
}

#if 0
void GUIAPI GetGlyphBitmap (LOGFONT* log_font, 
                const unsigned char* mchar, int mchar_len, 
                GLYPHBITMAP* glyph_bitmap)
{
    DEVFONT* sbc_devfont;
    DEVFONT* mbc_devfont;
    DEVFONT* devfont;
    Glyph32 glyph_value;

    if (!log_font) return;
    if (!glyph_bitmap) return;
    if (!mchar) return;

    sbc_devfont = log_font->sbc_devfont;
    mbc_devfont = log_font->mbc_devfont;

    if (mbc_devfont) {
        int mbc_len;

        mbc_len = mbc_devfont->charset_ops->len_first_char(mchar, mchar_len);
        if (mbc_len == 0) {
            devfont = sbc_devfont;
        }
        else {
            devfont = mbc_devfont;
        }
    }
    else {
        devfont = sbc_devfont;
    }
    glyph_value = devfont->charset_ops->char_glyph_value(NULL, 0, mchar, mchar_len); 

    if (devfont->font_ops->get_glyph_bbox) {
        glyph_bitmap->bbox_x = 0;
        glyph_bitmap->bbox_y = 0;
        devfont->font_ops->get_glyph_bbox (log_font, devfont, glyph_value,
                    &glyph_bitmap->bbox_x,
                    &glyph_bitmap->bbox_y,
                    &glyph_bitmap->bbox_w,
                    &glyph_bitmap->bbox_h);
    }
   else {
        glyph_bitmap->bbox_x = 0;
        glyph_bitmap->bbox_w = 0;

        glyph_bitmap->bbox_y = devfont->font_ops->get_font_descent (log_font, 
                        devfont);
        devfont->font_ops->get_glyph_advance (log_font, 
                        devfont, glyph_value, &glyph_bitmap->bbox_w, 0);
        glyph_bitmap->bbox_h = devfont->font_ops->get_font_height (log_font, 
                        devfont);
    }

    if (devfont->font_ops->get_glyph_advance) {
        glyph_bitmap->advance_x = 0;
        glyph_bitmap->advance_y = 0;
        if (devfont->font_ops->start_str_output)
            devfont->font_ops->start_str_output (log_font, devfont);
        devfont->font_ops->get_glyph_advance (log_font, devfont, 
                        glyph_value, 
                        &glyph_bitmap->advance_x, &glyph_bitmap->advance_y);
    }
    else {
        glyph_bitmap->advance_x = glyph_bitmap->bbox_w;
        glyph_bitmap->advance_y = 0;
    }

    /* restore to the original value if the scale factor is there. */
    if (GET_DEVFONT_SCALE (log_font, devfont) > 1) {
        int scale = GET_DEVFONT_SCALE (log_font, devfont);

        glyph_bitmap->bbox_x /= scale;
        glyph_bitmap->bbox_y /= scale;
        glyph_bitmap->bbox_w /= scale;
        glyph_bitmap->bbox_h /= scale;
        glyph_bitmap->advance_x /= scale;
        glyph_bitmap->advance_y /= scale;
    }

    glyph_bitmap->bmp_pitch = (glyph_bitmap->bbox_w + 7) >> 3;

    glyph_bitmap->bmp_size = glyph_bitmap->bmp_pitch * glyph_bitmap->bbox_h;

    glyph_bitmap->bits = devfont->font_ops->get_glyph_monobitmap (log_font, 
                    devfont, glyph_value, NULL);
}

#endif
