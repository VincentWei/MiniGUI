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
** simple-glyph-renderer.c: The implementation of GetGlyphsExtentFromUChars
**  and DrawGlyphStringEx.
**
** Reference:
**
**  https://www.w3.org/TR/css-text-3/
**  https://www.w3.org/TR/css-writing-modes-3/
**
** Create by WEI Yongming at 2019/01/16
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "pixel_ops.h"
#include "cursor.h"
#include "drawtext.h"
#include "fixedmath.h"
#include "glyph.h"
#include "unicode-ops.h"

#ifdef _MGCHARSET_UNICODE

// internal use
typedef struct _MYGLYPHINFO {
    Uchar32 uc;
    Uint8 gc;
    Uint8 suppressed:1;
    Uint8 whitespace:1;
    Uint8 orientation:2;
    Uint8 hanged:2;
    Uint8 justify_word:1;
    Uint8 justify_char:1;
} MYGLYPHINFO;

typedef struct _MYGLYPHARGS {
    PLOGFONT lfur;
    PLOGFONT lfsw;
    const Uchar32* ucs;
    const Uint16* bos;
    Glyph32* gvs;
    int nr_ucs;
    Uint32 rf;

    int lw;
    int hanged_start;
    int hanged_end;
} MYGLYPHARGS;

#if 0
static LOGFONT* create_sideways_logfont(LOGFONT* logfont_upright)
{
    LOGFONT tmp;

    memcpy(&tmp, logfont_upright, sizeof(LOGFONT));
    tmp.rotation = -900;
    return CreateLogFontIndirect(&tmp);
}
#endif

#define SCRIPT_ORIENTATION_HORIZONTAL   0x01
#define SCRIPT_ORIENTATION_VERTICAL     0x02
#define SCRIPT_ORIENTATION_BI           (0x01 | 0x02)

typedef struct _VERT_SCRIPT_ORT {
    ScriptType st;
    int             rot;
    const char*     vid;
} VERT_SCRIPT_ORT;

static VERT_SCRIPT_ORT vertical_scripts [] = {
    {SCRIPT_BOPOMOFO, 0, "ttb"},
    {SCRIPT_EGYPTIAN_HIEROGLYPHS, 0, "ttb"},
    {SCRIPT_HIRAGANA, 0, "ttb"},
    {SCRIPT_KATAKANA, 0, "ttb"},
    {SCRIPT_HAN, 0, "ttb"},
    {SCRIPT_HANGUL, 0, "ttb"},
    {SCRIPT_MEROITIC_CURSIVE, 0, "ttb"},
    {SCRIPT_MEROITIC_HIEROGLYPHS, 0, "ttb"},
    {SCRIPT_MONGOLIAN, 90, "ttb"},
    {SCRIPT_OGHAM, -90, "btt"},
    {SCRIPT_OLD_TURKIC, -90, "ttb"},
    {SCRIPT_PHAGS_PA, 90, "ttb"},
    {SCRIPT_YI, 0, "ttb"},
};

static BOOL is_horizontal_only_script(Uchar32 uc)
{
    size_t i;
    ScriptType st = UCharGetScriptType(uc);

    for (i = 0; i < TABLESIZE(vertical_scripts); i++) {
        if (st == vertical_scripts[i].st)
            return FALSE;
    }

    return TRUE;
}

static void normalize_glyph_metrics(LOGFONT* logfont,
        Uint32 render_flags, const BBOX* bbox,
        int* adv_x, int* adv_y, int* line_adv, int* line_width)
{
    switch (render_flags & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        if (logfont->rotation == -900) {
            *line_adv = *adv_y;
            if (logfont->size > *line_width)
                *line_width = logfont->size;
        }
        else {
            if (*adv_x > *line_width)
                *line_width = *adv_x;

            *adv_x = 0;
            *adv_y = logfont->size;
            *line_adv = logfont->size;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        *line_adv = *adv_x;
        if (logfont->size > *line_width)
            *line_width = logfont->size;
        break;
    }
}

static void set_extra_spacing(MYGLYPHARGS* args, int extra_spacing,
        GLYPHEXTINFO* ges)
{
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        ges->extra_x = 0;
        ges->extra_y = extra_spacing;
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        ges->extra_x = extra_spacing;
        ges->extra_y = 0;
        break;
    }
}

static void increase_extra_spacing(MYGLYPHARGS* args, int extra_spacing,
        GLYPHEXTINFO* ges)
{
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        ges->extra_y += extra_spacing;
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        ges->extra_x += extra_spacing;
        break;
    }
}

static int find_breaking_pos_normal(MYGLYPHARGS* args, int n)
{
    int i;

    for (i = n - 1; i >= 0; i--) {
        if ((args->bos[i] & BOV_LB_MASK) == BOV_LB_ALLOWED)
            return i;
    }

    return -1;
}

static int find_breaking_pos_any(MYGLYPHARGS* args, int n)
{
    int i;

    for (i = n - 1; i >= 0; i--) {
        if (args->bos[i] & BOV_GB_CHAR_BREAK)
            return i;
    }

    return -1;
}

static int find_breaking_pos_word(MYGLYPHARGS* args, int n)
{
    int i;

    for (i = n - 1; i >= 0; i--) {
        if (args->bos[i] & BOV_WB_WORD_BOUNDARY)
            return i;
    }

    return -1;
}

static inline BOOL is_whitespace_glyph(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int i)
{
    return (args->bos[i] & BOV_WHITESPACE);
}

static inline BOOL is_zero_width_glyph(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int i)
{
    return (args->bos[i] & BOV_ZERO_WIDTH);
}

static inline BOOL is_word_separator(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int i)
{
#if 0
    return (args->bos[i] & BOV_WB_WORD_BOUNDARY);
#else
    return (
        gis[i].uc == 0x0020 || gis[i].uc == 0x00A0 ||
        gis[i].uc == 0x1361 ||
        gis[i].uc == 0x10100 || gis[i].uc == 0x10101 ||
        gis[i].uc == 0x1039F || gis[i].uc == 0x1091F
    );
#endif
}

/*
 * TODO: scripts and spacing:
 * https://www.w3.org/TR/css-text-3/#script-groups
 */
static inline BOOL is_typographic_char(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int i)
{
    return (args->bos[i - 1] & BOV_GB_CHAR_BREAK);
}

static void justify_glyphs_inter_word(MYGLYPHARGS* args,
        MYGLYPHINFO* gis, GLYPHEXTINFO* ges, int n, int error)
{
    int i;
    int nr_words = 0;
    int err_per_unit;
    int left;

    for (i = 0; i < n; i++) {
        if (gis[i].suppressed == 0 && gis[i].hanged == 0 &&
                is_word_separator(args, gis, i)) {
            nr_words++;
            gis[i].justify_word = 1;
        }
        else {
            gis[i].justify_word = 0;
        }
    }

    if (nr_words <= 0)
        return;

    err_per_unit = error / nr_words;
    left = error % nr_words;
    if (err_per_unit > 0) {

        i = 0;
        do {
            if (gis[i].justify_word) {
                increase_extra_spacing(args, err_per_unit, ges + i);
                nr_words--;
            }

            i++;
        } while (nr_words > 0);
    }

    if (left > 0) {
        i = 0;
        do {
            if (gis[i].justify_word) {
                increase_extra_spacing(args, 1, ges + i);
                if (--left == 0)
                    break;
            }

            i++;
        } while (1);
    }
}

static void justify_glyphs_inter_char(MYGLYPHARGS* args,
        MYGLYPHINFO* gis, GLYPHEXTINFO* ges, int n, int error)
{
    int i;
    int nr_chars = 0;
    int err_per_unit;
    int left;

    for (i = 0; i < n; i++) {
        if (gis[i].suppressed == 0 && gis[i].hanged == 0 &&
                !is_word_separator(args, gis, i) &&
                is_typographic_char(args, gis, i)) {
            nr_chars++;
            gis[i].justify_char = 1;
        }
        else {
            gis[i].justify_char = 0;
        }
    }

    nr_chars--;
    if (nr_chars <= 0)
        return;

    err_per_unit = error / nr_chars;
    left = error % nr_chars;
    if (err_per_unit > 0) {
        i = 0;
        do {
            if (gis[i].justify_char) {
                increase_extra_spacing(args, err_per_unit, ges + i);
                nr_chars--;
            }

            i++;
        } while (nr_chars > 0);
    }

    if (left > 0) {
        for (i = 0; i < n; i++) {
            if (gis[i].justify_char) {
                increase_extra_spacing(args, 1, ges + i);
                if (--left == 0)
                    break;
            }
        }
    }
}

/*
 * For auto justification, we use the following policy:
 * Primarily expanding word separators and between CJK typographic
 * letter units along with secondarily expanding between other
 * typographic character units.
 */
static void justify_glyphs_auto(MYGLYPHARGS* args,
        MYGLYPHINFO* gis, GLYPHEXTINFO* ges, int n, int error)
{
    int i;
    int total_error = error;
    int nr_words = 0;
    int nr_chars = 0;
    int err_per_unit;
    int compensated = 0;
    int left;

    for (i = 0; i < n; i++) {
        gis[i].justify_word = 0;
        gis[i].justify_char = 0;
        if (gis[i].suppressed == 0 && gis[i].hanged == 0) {
            if ((is_word_separator(args, gis, i) && i != 0) ||
                    IsUCharWideCJK(gis[i].uc)) {
                nr_words++;
                gis[i].justify_word = 1;
            }
            else if (is_typographic_char(args, gis, i)) {
                nr_chars++;
                gis[i].justify_char = 1;
            }
        }
    }

    _DBG_PRINTF("%s: nr_words(%d), nr_chars(%d)\n",
        __FUNCTION__, nr_words, nr_chars);

    nr_chars--;

    /* most error for words and CJK letters */
    if (nr_chars > 0)
        error = error * 2 / 3;

    if (nr_words > 0) {
        err_per_unit = error / nr_words;
        left = error % nr_words;
        if (err_per_unit > 0) {

            i = 0;
            do {
                if (gis[i].justify_word) {
                    increase_extra_spacing(args, err_per_unit, ges + i);
                    compensated += err_per_unit;
                    nr_words--;
                }

                i++;
            } while (nr_words > 0);
        }

        if (nr_chars <= 0 && left > 0) {
            for (i = 0; i < n; i++) {
                if (gis[i].justify_word) {
                    increase_extra_spacing(args, 1, ges + i);
                    if (--left == 0)
                        break;
                }
            }

            return;
        }
    }

    if (nr_chars > 0) {
        /* left error for other chars */
        error = total_error - compensated;
        err_per_unit = error / nr_chars;
        left = error % nr_chars;
        if (err_per_unit > 0) {
            i = 0;
            do {
                if (gis[i].justify_char) {
                    increase_extra_spacing(args, err_per_unit, ges + i);
                    nr_chars--;
                }

                i++;
            } while (nr_chars > 0);
        }

        if (left > 0) {
            for (i = 0; i < n; i++) {
                if (gis[i].justify_char) {
                    increase_extra_spacing(args, 1, ges + i);
                    if (--left == 0)
                        break;
                }
            }
        }
    }
}

static void adjust_glyph_position(MYGLYPHARGS* args,
        int x, int y, const MYGLYPHINFO* gi, const GLYPHEXTINFO* ge,
        GLYPHPOS* pos)
{
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
        if (gi->orientation == GLYPH_ORIENT_UPRIGHT) {
            x -= (args->lw + ge->bbox_w) / 2;
            x -= ge->bbox_x;
        }
        break;

    case GRF_WRITING_MODE_VERTICAL_LR:
        if (gi->orientation == GLYPH_ORIENT_SIDEWAYS) {
            x += args->lfsw->size;
        }
        else {
            x += (args->lw - ge->bbox_w) / 2;
            x -= ge->bbox_x;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        break;
    }

    pos->x += x;
    pos->y += y;
}

static void calc_unhanged_glyph_positions(MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, GLYPHEXTINFO* ges, int n,
        int x, int y, GLYPHPOS* pos)
{
    int i;
    int first = 0, stop = n;

    if (args->hanged_start >= 0)
        first = args->hanged_start + 1;
    if (args->hanged_end < n)
        stop = args->hanged_end;

    for (i = first; i < stop; i++) {
        if (i == first) {
            pos[i].x = 0;
            pos[i].y = 0;
        }
        else {
            pos[i].x = pos[i - 1].x + ges[i - 1].adv_x;
            pos[i].y = pos[i - 1].y + ges[i - 1].adv_y;
            pos[i].x += ges[i - 1].extra_x;
            pos[i].y += ges[i - 1].extra_y;
        }
    }

    for (i = first; i < stop; i++) {
        adjust_glyph_position(args, x, y, gis + i, ges + i, pos + i);

        ges[i].suppressed = gis[i].suppressed;
        ges[i].whitespace = gis[i].whitespace;
        ges[i].orientation = gis[i].orientation;

        pos[i].suppressed = gis[i].suppressed;
        pos[i].whitespace = gis[i].whitespace;
        pos[i].orientation = gis[i].orientation;
        pos[i].hanged = gis[i].hanged;
    }
}

static int calc_hanged_glyphs_extent(MYGLYPHARGS* args,
        const GLYPHEXTINFO* ges, int n)
{
    int i;
    int hanged_extent = 0;

    if (args->hanged_start >= 0) {
        for (i = 0; i <= args->hanged_start; i++) {
            hanged_extent += ges[i].line_adv;
        }
    }

    if (args->hanged_end < n) {
        for (i = args->hanged_end; i < n; i++) {
            hanged_extent += ges[i].line_adv;
        }
    }

    _DBG_PRINTF("%s: hanged_start(%d) hanged_end(%d) n(%d) hanged_extent(%d)\n",
        __FUNCTION__, args->hanged_start, args->hanged_end, n, hanged_extent);

    return hanged_extent;
}

static int calc_hanged_glyphs_start(MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, GLYPHEXTINFO* ges,
        GLYPHPOS* pos, int n, int x, int y)
{
    int i;
    int hanged_extent = 0;

    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        for (i = 0; i <= args->hanged_start; i++) {
            hanged_extent += ges[i].line_adv;
        }

        for (i = 0; i <= args->hanged_start; i++) {
            if (i == 0) {
                pos[i].x = 0;
                pos[i].y = -hanged_extent;
            }
            else {
                pos[i].x = pos[i - 1].x + ges[i - 1].adv_x;
                pos[i].y = pos[i - 1].y + ges[i - 1].adv_y;
                pos[i].x += ges[i - 1].extra_x;
                pos[i].y += ges[i - 1].extra_y;
            }
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        for (i = 0; i <= args->hanged_start; i++) {
            hanged_extent += ges[i].line_adv;
        }

        for (i = 0; i <= args->hanged_start; i++) {
            if (i == 0) {
                pos[i].x = -hanged_extent;
                pos[i].y = 0;
            }
            else {
                pos[i].x = pos[i - 1].x + ges[i - 1].adv_x;
                pos[i].y = pos[i - 1].y + ges[i - 1].adv_y;
                pos[i].x += ges[i - 1].extra_x;
                pos[i].y += ges[i - 1].extra_y;
            }
        }
        break;
    }

    for (i = 0; i <= args->hanged_start; i++) {
        adjust_glyph_position(args, x, y, gis + i, ges + i, pos + i);

        ges[i].suppressed = gis[i].suppressed;
        ges[i].whitespace = gis[i].whitespace;
        ges[i].orientation = gis[i].orientation;

        pos[i].suppressed = gis[i].suppressed;
        pos[i].whitespace = gis[i].whitespace;
        pos[i].orientation = gis[i].orientation;
        pos[i].hanged = gis[i].hanged;
    }

    return hanged_extent;
}

static int calc_hanged_glyphs_end(MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, GLYPHEXTINFO* ges,
        GLYPHPOS* pos, int n, int x, int y, int extent)
{
    int i;
    int hanged_extent = 0;

    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        for (i = args->hanged_end; i < n; i++) {
            if (i == args->hanged_end) {
                pos[i].x = 0;
                pos[i].y = extent;
            }
            else {
                pos[i].x = pos[i - 1].x + ges[i - 1].adv_x;
                pos[i].y = pos[i - 1].y + ges[i - 1].adv_y;
                pos[i].x += ges[i - 1].extra_x;
                pos[i].y += ges[i - 1].extra_y;
            }

            hanged_extent += ges[i].line_adv;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        for (i = args->hanged_end; i < n; i++) {
            if (i == args->hanged_end) {
                pos[i].x = extent;
                pos[i].y = 0;
            }
            else {
                pos[i].x = pos[i - 1].x + ges[i - 1].adv_x;
                pos[i].y = pos[i - 1].y + ges[i - 1].adv_y;
                pos[i].x += ges[i - 1].extra_x;
                pos[i].y += ges[i - 1].extra_y;
            }

            hanged_extent += ges[i].line_adv;
        }
        break;
    }

    for (i = args->hanged_end; i < n; i++) {
        adjust_glyph_position(args, x, y, gis + i, ges + i, pos + i);

        ges[i].suppressed = gis[i].suppressed;
        ges[i].whitespace = gis[i].whitespace;
        ges[i].orientation = gis[i].orientation;

        pos[i].suppressed = gis[i].suppressed;
        pos[i].whitespace = gis[i].whitespace;
        pos[i].orientation = gis[i].orientation;
        pos[i].hanged = gis[i].hanged;
    }

    _DBG_PRINTF("%s: hanged_start(%d) hanged_end(%d) n(%d) hanged_extent(%d)\n",
        __FUNCTION__, args->hanged_start, args->hanged_end, n, hanged_extent);

    return hanged_extent;
}

static void offset_unhanged_glyph_positions(MYGLYPHARGS* args,
        GLYPHPOS* pos, int n, int offset)
{
    int i;
    int first = 0, stop = n;

    if (args->hanged_start >= 0)
        first = args->hanged_start + 1;
    if (args->hanged_end < n)
        stop = args->hanged_end;

    _DBG_PRINTF("%s: offset(%d), first(%d), stop(%d)\n",
            __FUNCTION__, offset, first, stop);

    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        for (i = first; i < stop; i++) {
            pos[i].y += offset;
        }
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        for (i = first; i < stop; i++) {
            pos[i].x += offset;
        }
        break;
    }
}

static void align_unhanged_glyphs(MYGLYPHARGS* args,
        GLYPHPOS* pos, int n, int gap)
{
    _DBG_PRINTF("%s: args->rf(0x%08X), gap(%d)\n",
            __FUNCTION__, args->rf, gap);
    switch (args->rf & GRF_ALIGN_MASK) {
    case GRF_ALIGN_RIGHT:
    case GRF_ALIGN_END:
        offset_unhanged_glyph_positions(args, pos, n, gap);
        break;

    case GRF_ALIGN_CENTER:
        offset_unhanged_glyph_positions(args, pos, n, gap/2);
        break;

    case GRF_ALIGN_LEFT:
    case GRF_ALIGN_START:
    case GRF_ALIGN_JUSTIFY:
    default:
        break;
    }
}

static inline BOOL is_opening_punctation(const MYGLYPHINFO* gi)
{
    return (
        gi->gc == UCHAR_CATEGORY_OPEN_PUNCTUATION ||
        gi->gc == UCHAR_CATEGORY_FINAL_PUNCTUATION ||
        gi->gc == UCHAR_CATEGORY_INITIAL_PUNCTUATION ||
        gi->uc == 0x0027 ||
        gi->uc == 0x0022
    );
}

static inline BOOL is_closing_punctation(const MYGLYPHINFO* gi)
{
    return (
        gi->gc == UCHAR_CATEGORY_CLOSE_PUNCTUATION ||
        gi->gc == UCHAR_CATEGORY_FINAL_PUNCTUATION ||
        gi->gc == UCHAR_CATEGORY_INITIAL_PUNCTUATION ||
        gi->uc == 0x0027 ||
        gi->uc == 0x0022
    );
}

static inline BOOL is_stop_or_common(const MYGLYPHINFO* gi)
{
    return (
        gi->uc == 0x002C || //  ,   COMMA
        gi->uc == 0x002E || //  .   FULL STOP
        gi->uc == 0x060C || //  ،   ARABIC COMMA
        gi->uc == 0x06D4 || //  ۔   ARABIC FULL STOP
        gi->uc == 0x3001 || //  、  IDEOGRAPHIC COMMA
        gi->uc == 0x3002 || //  。  IDEOGRAPHIC FULL STOP
        gi->uc == 0xFF0C || //  ，  FULLWIDTH COMMA
        gi->uc == 0xFF0E || //  ．  FULLWIDTH FULL STOP
        gi->uc == 0xFE50 || //  ﹐  SMALL COMMA
        gi->uc == 0xFE51 || //  ﹑  SMALL IDEOGRAPHIC COMMA
        gi->uc == 0xFE52 || //  ﹒  SMALL FULL STOP
        gi->uc == 0xFF61 || //  ｡   HALFWIDTH IDEOGRAPHIC FULL STOP
        gi->uc == 0xFF64    //  ､   HALFWIDTH IDEOGRAPHIC COMMA
    );
}

static void init_glyph_info(MYGLYPHARGS* args, int i,
        MYGLYPHINFO* gi)
{
    args->gvs[i] = GetGlyphValueAlt(args->lfur, UCHAR2ACHAR(args->ucs[i]));

    gi->uc = args->ucs[i];
    gi->gc = UCharGetCategory(gi->uc);
    gi->whitespace = 0;
    gi->suppressed = 0;
    gi->hanged = GLYPH_HANGED_NONE;
    gi->orientation = GLYPH_ORIENT_UPRIGHT;
}

static inline int shrink_total_extent(MYGLYPHARGS* args, int total_extent,
        const GLYPHEXTINFO* ges)
{
#if 0
    switch (args->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_VERTICAL_RL:
    case GRF_WRITING_MODE_VERTICAL_LR:
        total_extent -= ges->adv_y;
        total_extent -= ges->extra_y;
        break;
    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        total_extent -= ges->adv_x;
        total_extent -= ges->extra_x;
        break;
    }

    return total_extent;
#else
    return total_extent - ges->line_adv;
#endif

}

static int get_glyph_extent_info(MYGLYPHARGS* args, Glyph32 gv,
        MYGLYPHINFO* gi, GLYPHEXTINFO* ges)
{
    LOGFONT* logfont = args->lfur;
    BBOX bbox;
    int adv_x = 0, adv_y = 0;
    int line_adv;

    if ((args->rf & GRF_WRITING_MODE_MASK)
            != GRF_WRITING_MODE_HORIZONTAL_TB) {
        if ((args->rf & GRF_TEXT_ORIENTATION_MASK)
               == GRF_TEXT_ORIENTATION_MIXED) {
            if (is_horizontal_only_script(gi->uc)) {
                logfont = args->lfsw;
                gi->orientation = GLYPH_ORIENT_SIDEWAYS;
            }
        }
        else if ((args->rf & GRF_TEXT_ORIENTATION_MASK)
               == GRF_TEXT_ORIENTATION_SIDEWAYS) {
            logfont = args->lfsw;
            gi->orientation = GLYPH_ORIENT_SIDEWAYS;
        }
    }

    _font_get_glyph_metrics(logfont, gv, &adv_x, &adv_y, &bbox);
    normalize_glyph_metrics(logfont, args->rf, &bbox,
            &adv_x, &adv_y, &line_adv, &args->lw);

    ges->bbox_x = bbox.x;
    ges->bbox_y = bbox.y;
    ges->bbox_w = bbox.w;
    ges->bbox_h = bbox.h;
    ges->adv_x = adv_x;
    ges->adv_y = adv_y;

    return line_adv;
}

static int get_first_normal_glyph(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        if (gis[i].suppressed == 0 && gis[i].hanged == 0
                && (args->bos[i - 1] & BOV_GB_CHAR_BREAK))
            return i;
    }

    return -1;
}

static int get_last_normal_glyph(const MYGLYPHARGS* args,
        const MYGLYPHINFO* gis, int n)
{
    int i;
    for (i = n - 1; i >= 0; i--) {
        if (gis[i].suppressed == 0 && gis[i].hanged == 0
                && (args->bos[i - 1] & BOV_GB_CHAR_BREAK))
            return i;
    }

    return -1;
}

static inline BOOL is_logfont_unicode(LOGFONT* lf)
{
    DEVFONT* df = lf->devfonts[1];
    if (df && df->charset_ops->conv_to_uc32 == NULL)
        return TRUE;

    return FALSE;
}

int GUIAPI GetGlyphsExtentFromUChars(LOGFONT* logfont_upright,
        const Uchar32* ucs, int nr_ucs,
        const Uint16* break_oppos,
        Uint32 render_flags, int x, int y,
        int letter_spacing, int word_spacing, int tab_size, int max_extent,
        SIZE* line_size, Glyph32* glyphs, GLYPHEXTINFO* glyph_ext_info,
        GLYPHPOS* glyph_pos, LOGFONT** logfont_sideways)
{
    int n = 0;
    int total_extent = 0;
    int breaking_pos = -1;
    int gap;

    MYGLYPHARGS  args;
    MYGLYPHINFO* gis = NULL;
    GLYPHEXTINFO* ges = NULL;
    BOOL test_overflow = TRUE;
    BOOL lfsw_created = FALSE;

    /* Check logfont_upright and create logfont_sideways if need */
    if (logfont_upright == NULL || logfont_upright->rotation != 0)
        return 0;

    if (!is_logfont_unicode(logfont_upright) ||
            (*logfont_sideways != NULL &&
                !is_logfont_unicode(*logfont_sideways))) {
        _ERR_PRINTF("%s: the logfonts are not in Unicode charset.\n",
            __FUNCTION__);
        return 0;
    }

    if (*logfont_sideways == NULL) {
        if ((render_flags & GRF_WRITING_MODE_MASK)
                != GRF_WRITING_MODE_HORIZONTAL_TB) {
            switch (render_flags & GRF_TEXT_ORIENTATION_MASK) {
            case GRF_TEXT_ORIENTATION_MIXED:
            case GRF_TEXT_ORIENTATION_SIDEWAYS:
                *logfont_sideways
                    = CreateLogFontIndirectEx(logfont_upright, -900);
                if (*logfont_sideways == NULL
                        || (*logfont_sideways)->rotation != -900)
                    goto error;
                lfsw_created = TRUE;
                break;
            default:
                *logfont_sideways = NULL;
                break;
            }
        }
    }
    else {
        if ((*logfont_sideways)->rotation != -900) {
            goto error;
        }
    }

    if (glyph_ext_info == NULL) {
        ges = (GLYPHEXTINFO*)calloc(sizeof(GLYPHEXTINFO), nr_ucs);
        if (ges == NULL)
            goto error;
    }
    else {
        ges = glyph_ext_info;
        memset(ges, 0, sizeof(GLYPHEXTINFO) * nr_ucs);
    }

    gis = (MYGLYPHINFO*)calloc(sizeof(MYGLYPHINFO), nr_ucs);
    if (gis == NULL)
        goto error;

    args.lfur = logfont_upright;
    args.lfsw = *logfont_sideways;
    args.ucs = ucs;
    args.gvs = glyphs;
    args.bos = break_oppos;
    args.nr_ucs = nr_ucs;
    args.rf = render_flags;
    args.lw = args.lfur->size;
    if (args.lfsw) {
        args.lw = MAX(args.lw, args.lfsw->size);
    }

    while (n < nr_ucs) {
        int extra_spacing;

        init_glyph_info(&args, n, gis + n);

        /*
         * NOTE: The collapsible spaces should be handled in GetGlyphsByRules.
         */
        if (gis[n].uc == UCHAR_TAB) {
            if (tab_size > 0) {
                int tabstops = total_extent / tab_size + 1;
                ges[n].line_adv = tabstops * tab_size- total_extent;

                // If this distance is less than 0.5ch, then the
                // subsequent tab stop is used instead.
                if (ges[n].line_adv < logfont_upright->size / 6) {
                    tabstops++;
                    ges[n].line_adv = tabstops * tab_size- total_extent;
                }

                switch (render_flags & GRF_WRITING_MODE_MASK) {
                case GRF_WRITING_MODE_VERTICAL_RL:
                case GRF_WRITING_MODE_VERTICAL_LR:
                    ges[n].adv_y = ges[n].line_adv;
                    break;
                case GRF_WRITING_MODE_HORIZONTAL_TB:
                default:
                    ges[n].adv_x = ges[n].line_adv;
                    break;
                }

                gis[n].whitespace = 1;
            }
            else {
                gis[n].suppressed = 1;
                ges[n].line_adv = 0;
            }
        }
        else if (is_whitespace_glyph(&args, gis, n)) {
            Glyph32 space_gv = GetGlyphValueAlt(logfont_upright,
                    UCHAR2ACHAR(UCHAR_SPACE));

            gis[n].whitespace = 1;
            ges[n].line_adv = _font_get_glyph_log_width(logfont_upright,
                                space_gv);

            switch (render_flags & GRF_WRITING_MODE_MASK) {
            case GRF_WRITING_MODE_VERTICAL_RL:
            case GRF_WRITING_MODE_VERTICAL_LR:
                ges[n].adv_y = ges[n].line_adv;
                break;
            case GRF_WRITING_MODE_HORIZONTAL_TB:
            default:
                ges[n].adv_x = ges[n].line_adv;
                break;
            }
        }
        else if (is_zero_width_glyph(&args, gis, n)) {
            gis[n].suppressed = 1;
            ges[n].line_adv = 0;
        }
        else {
            ges[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    ges + n);
        }

        // extra space for word and letter
        extra_spacing = 0;
        if (gis[n].suppressed == 0 && is_word_separator(&args, gis, n)) {
            extra_spacing = word_spacing;
        }
        else if (gis[n].suppressed == 0 && is_typographic_char(&args, gis, n)) {
            extra_spacing = letter_spacing;
        }

        if (extra_spacing > 0) {
            ges[n].line_adv += extra_spacing;
            set_extra_spacing(&args, extra_spacing, ges + n);
        }

        if (test_overflow && max_extent > 0
                && (total_extent + ges[n].line_adv) > max_extent) {
            // overflow
            switch (render_flags & GRF_OVERFLOW_WRAP_MASK) {
            case GRF_OVERFLOW_WRAP_BREAK_WORD:
                breaking_pos = find_breaking_pos_word(&args, n);
                break;
            case GRF_OVERFLOW_WRAP_ANYWHERE:
                breaking_pos = find_breaking_pos_any(&args, n);
                break;
            case GRF_OVERFLOW_WRAP_NORMAL:
            default:
                breaking_pos = find_breaking_pos_normal(&args, n);
                break;
            }

            if (breaking_pos >= 0) {
                // a valid breaking position found before current glyph
                break;
            }

            breaking_pos = -1;
            test_overflow = FALSE;
        }

        total_extent += ges[n].line_adv;
        if ((break_oppos[n] & BOV_LB_MASK) == BOV_LB_MANDATORY) {
            // hard line breaking
            n++;
            break;
        }

        if (!test_overflow && max_extent > 0
                && ((break_oppos[n] & BOV_LB_MASK) == BOV_LB_ALLOWED)) {
            n++;
            break;
        }

        n++;
    }

    if (breaking_pos >= 0 && breaking_pos != n) {
        // wrapped due to overflow
        int i;

        n = breaking_pos + 1;

        total_extent = 0;
        for (i = 0; i < n; i++) {
            total_extent += ges[i].line_adv;
        }
    }

    args.hanged_start = -1;
    args.hanged_end = n + 1;

    // Trimming spaces at the start of the line
    if (render_flags & GRF_SPACES_REMOVE_START) {
        int i = 0;
        while (i < n && gis[i].uc == UCHAR_SPACE) {
            total_extent -= ges[i].line_adv;
            memset(ges + i, 0, sizeof(GLYPHEXTINFO));
            gis[i].suppressed = 1;
            i++;
        }
    }

    // Trimming or hanging spaces at the end of the line
    if (render_flags & GRF_SPACES_REMOVE_END) {
        int i = n - 1;
        while (i > 0 &&
                (gis[i].uc == UCHAR_SPACE || gis[i].uc == UCHAR_IDSPACE)) {
            total_extent -= ges[i].line_adv;
            memset(ges + i, 0, sizeof(GLYPHEXTINFO));
            gis[i].suppressed = 1;
            i--;
        }
    }
    else if (render_flags & GRF_SPACES_HANGE_END) {
        int i = n - 1;
        while (i > 0 &&
                (gis[i].uc == UCHAR_SPACE || gis[i].uc == UCHAR_IDSPACE)) {

            gis[i].hanged = GLYPH_HANGED_END;
            if (i < args.hanged_end) args.hanged_end = i;
            i--;
        }
    }

    if (n < nr_ucs) {
        init_glyph_info(&args, n, gis + n);
    }

    if (render_flags & GRF_HANGING_PUNC_OPEN) {
        int first = get_first_normal_glyph(&args, gis, n);
        if (first >= 0 && is_opening_punctation(gis + first)) {
            gis[first].hanged = GLYPH_HANGED_START;
            if (first > args.hanged_start) args.hanged_start = first;
        }
    }

    if (n > 1 && render_flags & GRF_HANGING_PUNC_CLOSE) {
        int last = get_last_normal_glyph(&args, gis, n);
        if (last > 0 && is_closing_punctation(gis + last)) {
            gis[last].hanged = GLYPH_HANGED_END;
            if (last < args.hanged_end) args.hanged_end = last;
        }
#if 0
        else if (n < nr_ucs && is_closing_punctation(gis + n)) {
            gis[n].hanged = GLYPH_HANGED_END;
            if (n < args.hanged_end) args.hanged_end = n;
            ges[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    ges + n);
            total_extent += ges[n].line_adv;
            n++;
        }
#endif
    }

    if (render_flags & GRF_HANGING_PUNC_FORCE_END) {
        // A stop or comma at the end of a line hangs.
        int last = get_last_normal_glyph(&args, gis, n);
        if (last > 0 && is_stop_or_common(gis + last)) {
            gis[last].hanged = GLYPH_HANGED_END;
            if (last < args.hanged_end) args.hanged_end = last;
        }
#if 0
        else if (n < nr_ucs && is_stop_or_common(gis + n)) {
            gis[n].hanged = GLYPH_HANGED_END;
            if (n < args.hanged_end) args.hanged_end = n;
            ges[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    ges + n);
            total_extent += ges[n].line_adv;
            n++;
        }
#endif
    }
    else if (render_flags & GRF_HANGING_PUNC_ALLOW_END) {
        // A stop or comma at the end of a line hangs
        // if it does not otherwise fit prior to justification.
        if (n < nr_ucs && is_stop_or_common(gis + n)) {
            gis[n].hanged = GLYPH_HANGED_END;
            if (n < args.hanged_end) args.hanged_end = n;
            ges[n].line_adv = get_glyph_extent_info(&args, glyphs[n], gis + n,
                    ges + n);
            total_extent += ges[n].line_adv;
            n++;
        }
    }

    total_extent -= calc_hanged_glyphs_extent(&args, ges, n);

    // calc positions of hanged glyphs
    if (args.hanged_start >= 0) {
        calc_hanged_glyphs_start(&args, gis, ges,
                glyph_pos, n, x, y);
    }

    if (args.hanged_end < n) {
        if (max_extent > 0) {
            calc_hanged_glyphs_end(&args, gis, ges,
                        glyph_pos, n, x, y, MAX(max_extent, total_extent));
        }
        else {
            calc_hanged_glyphs_end(&args, gis, ges,
                    glyph_pos, n, x, y, total_extent);
        }
    }

    gap = max_extent - total_extent;
    // justify the unhanged glyphs
    if ((render_flags & GRF_ALIGN_MASK) == GRF_ALIGN_JUSTIFY
            && gap > 0) {
        switch (render_flags & GRF_TEXT_JUSTIFY_MASK) {
        case GRF_TEXT_JUSTIFY_INTER_WORD:
            justify_glyphs_inter_word(&args, gis, ges, n, gap);
            break;
        case GRF_TEXT_JUSTIFY_INTER_CHAR:
            justify_glyphs_inter_char(&args, gis, ges, n, gap);
            break;
        case GRF_TEXT_JUSTIFY_AUTO:
        default:
            justify_glyphs_auto(&args, gis, ges, n, gap);
            break;
        }

    }

    // calcualte unhanged glyph positions according to the base point
    calc_unhanged_glyph_positions(&args, gis, ges, n, x, y, glyph_pos);

    // align unhanged glyphs
    align_unhanged_glyphs(&args, glyph_pos, n, gap);

    if (line_size) {
        if ((render_flags & GRF_WRITING_MODE_MASK)
                == GRF_WRITING_MODE_HORIZONTAL_TB) {
            line_size->cx = glyph_pos[n - 1].x - glyph_pos[0].x
                + ges[n - 1].adv_x + ges[n - 1].extra_x;
            line_size->cy = args.lw;
        }
        else {
            line_size->cy = glyph_pos[n - 1].y - glyph_pos[0].y
                + ges[n - 1].adv_y + ges[n - 1].extra_y;
            line_size->cx = args.lw;
        }
    }

    free(gis);
    if (glyph_ext_info == NULL)
        free(ges);

    return n;

error:
    if (ges) free(ges);
    if (gis) free(gis);
    if (lfsw_created) {
        DestroyLogFont(*logfont_sideways);
        *logfont_sideways = NULL;
    }

    return 0;
}

#endif /*  _MGCHARSET_UNICODE */

int GUIAPI DrawGlyphStringEx(HDC hdc,
        LOGFONT* logfont_upright, LOGFONT* logfont_sideways,
        const Glyph32* glyphs, const GLYPHPOS* glyph_pos,
        int nr_glyphs)
{
    int i;
    int n = 0;
    Uint32 old_ta;
    PLOGFONT old_lf;

    if (glyphs == NULL || glyph_pos == NULL || nr_glyphs <= 0)
        return 0;

    old_ta = SetTextAlign(hdc, TA_LEFT | TA_TOP | TA_UPDATECP);
    old_lf = GetCurFont(hdc);

    for (i = 0; i < nr_glyphs; i++) {
        if (glyph_pos[i].suppressed == 0 && glyph_pos[i].whitespace == 0) {
            if (glyph_pos[i].orientation == GLYPH_ORIENT_UPRIGHT) {
                if (logfont_upright)
                    SelectFont(hdc, logfont_upright);
                else
                    goto error;
            }
            else {
                if (logfont_sideways)
                    SelectFont(hdc, logfont_sideways);
                else
                    goto error;
            }

            DrawGlyph(hdc, glyph_pos[i].x, glyph_pos[i].y, glyphs[i],
                NULL, NULL);

            n++;
        }
    }

error:
    SelectFont(hdc, old_lf);
    SetTextAlign(hdc, old_ta);

    return n;
}

