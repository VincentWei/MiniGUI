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
 *   Copyright (C) 2002~2019, Beijing FMSoft Technologies Co., Ltd.
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
** layout-utils.c: The implementation of utilities related LAYOUT
**
** Create by WEI Yongming at 2019/03/21
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGCHARSET_UNICODE

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "unicode-ops.h"
#include "layout.h"
#include "fontname.h"

static BOOL fontname_get_from_orient(char* fontname, GlyphOrient ort)
{
    int orient_pos;

    orient_pos = fontGetOrientPosFromName(fontname);
    if (orient_pos < 0)
        return FALSE;

    switch (ort) {
    case GLYPH_ORIENT_UPRIGHT:
        fontname[orient_pos] = FONT_ORIENT_UPRIGHT;
        break;
    case GLYPH_ORIENT_SIDEWAYS:
        fontname[orient_pos] = FONT_ORIENT_SIDEWAYS;
        break;
    case GLYPH_ORIENT_UPSIDE_DOWN:
        fontname[orient_pos] = FONT_ORIENT_UPSIDE_DOWN;
        break;
    case GLYPH_ORIENT_SIDEWAYS_LEFT:
        fontname[orient_pos] = FONT_ORIENT_SIDEWAYS_LEFT;
        break;
    default:
        _WRN_PRINTF("bad orientation param: %d\n", ort);
        return FALSE;
    }

    return TRUE;
}

/* Use MiniGUI resource manager to avoid duplicated logfonts */
LOGFONT* __mg_create_logfont_for_layout(const LAYOUT* layout,
        const char* fontname, GlyphOrient ort)
{
    char my_fontname[LEN_LOGFONT_NAME_FULL + 1];
    LOGFONT* lf;

    if (fontname == NULL)
        fontname = layout->truns->fontname;

    memset (my_fontname, 0, LEN_LOGFONT_NAME_FULL + 1);
    strncpy (my_fontname, fontname, LEN_LOGFONT_NAME_FULL);
    if (!fontname_get_from_orient(my_fontname, ort)) {
        return NULL;
    }

    lf = (LOGFONT*)LoadResource(my_fontname, RES_TYPE_FONT, 0);
    if (lf == NULL) {
        _ERR_PRINTF("%s: failed to create LOGFONT for layout: %p\n",
            __FUNCTION__, layout);
    }

    return lf;
}

void __mg_release_logfont_for_layout(const LAYOUT* layout,
        const char* fontname, GlyphOrient ort)
{
    char my_fontname[LEN_LOGFONT_NAME_FULL + 1];
    if (fontname == NULL)
        fontname = layout->truns->fontname;

    memset (my_fontname, 0, LEN_LOGFONT_NAME_FULL + 1);
    strncpy (my_fontname, fontname, LEN_LOGFONT_NAME_FULL);
    if (!fontname_get_from_orient(my_fontname, ort)) {
        return;
    }

    ReleaseRes(Str2Key(my_fontname));
}

static GlyphOrient resolve_glyph_orient(const LAYOUT* layout,
        const TextRun* trun)
{
    GlyphGravity gravity;

    if (layout->grv_plc == GLYPH_GRAVITY_POLICY_STRONG) {
        assert(layout->grv_base != GLYPH_GRAVITY_AUTO);
        return layout->grv_base;
    }

    gravity = ScriptGetGlyphGravityForWide(trun->st,
            layout->rf & GRF_WRITING_MODE_VERTICAL_FLAG,
            trun->flags & TEXTRUN_FLAG_UPRIGHT,
            layout->grv_base, layout->grv_plc);

    return (GlyphOrient)gravity;
}

static void resolve_layout_run_dir(const LAYOUT* layout,
        LayoutRun* lrun)
{
    /* The level vs. gravity dance:
     *  If gravity is SOUTH, leave level untouched.
     *  If gravity is NORTH, step level one up, to
     *    not get mirrored upside-down text.
     *  If gravity is EAST, step up to an even level, as
     *    it's a clockwise-rotated layout, so the rotated
     *     top is unrotated left.
     *  If gravity is WEST, step up to an odd level, as
     *    it's a counter-clockwise-rotated layout, so the rotated
     *    top is unrotated right.
     */
    switch (lrun->ort) {
    case GLYPH_GRAVITY_SOUTH:
    default:
        break;
    case GLYPH_GRAVITY_NORTH:
        lrun->el++;
        break;
    case GLYPH_GRAVITY_EAST:
        lrun->el += 1;
        lrun->el &= ~1;
        break;
    case GLYPH_GRAVITY_WEST:
        lrun->el |= 1;
        break;
    }

    if (layout->rf & GRF_WRITING_MODE_VERTICAL_FLAG) {
        lrun->dir = (lrun->el & 1) ? GLYPH_RUN_DIR_BTT : GLYPH_RUN_DIR_TTB;

        if (lrun->ort == GLYPH_ORIENT_UPRIGHT ||
                lrun->ort == GLYPH_ORIENT_UPSIDE_DOWN)
            lrun->flags |= LAYOUTRUN_FLAG_CENTERED_BASELINE;
    }
    else {
        lrun->dir = (lrun->el & 1) ? GLYPH_RUN_DIR_RTL : GLYPH_RUN_DIR_LTR;
    }
}

/*
 * Not like Pango, we simply use the properties of the referenced TextRun
 * for the new orphan LayoutRun, including the direction, the orientation,
 * the embedding level, and the language, except the script type.
 *
 * Therefore, we assume that the text of the new LayoutRun are all
 * in the same script, same direction, and same orientation. This is
 * enough for the ellipsis.
 */
LayoutRun* __mg_layout_run_new_ellipsis(const LAYOUT* layout,
        const TextRun* trun, const Uchar32* ucs, int nr_ucs)
{
    GlyphOrient ort;
    LOGFONT* lf;
    LayoutRun* lrun;

    ort = resolve_glyph_orient(layout, trun);
    lf = __mg_create_logfont_for_layout(layout, trun->fontname, ort);
    if (lf == NULL)
        return NULL;

    lrun = mg_slice_new(LayoutRun);
    lrun->lf = lf;
    lrun->ucs = ucs;
    lrun->si = trun->si;
    lrun->len = nr_ucs;
    lrun->lc = trun->lc;
    lrun->st = UCharGetScriptType(ucs[0]);
    lrun->el = trun->el;
    lrun->ort = ort;
    lrun->flags = trun->flags | LAYOUTRUN_FLAG_ELLIPSIS;

    resolve_layout_run_dir(layout, lrun);
    return lrun;
}

LayoutRun* __mg_layout_run_new_from(const LAYOUT* layout,
        const TextRun* trun)
{
    GlyphOrient ort;
    LOGFONT* lf;
    LayoutRun* lrun;

    ort = resolve_glyph_orient(layout, trun);
    lf = __mg_create_logfont_for_layout(layout, trun->fontname, ort);
    if (lf == NULL)
        return NULL;

    lrun = mg_slice_new(LayoutRun);
    lrun->lf = lf;
    lrun->si = trun->si;
    lrun->ucs = layout->truns->ucs + lrun->si;
    lrun->len = trun->len;
    lrun->lc = trun->lc;
    lrun->st = trun->st;
    lrun->el = trun->el;
    lrun->ort = ort;
    lrun->flags = trun->flags;

    resolve_layout_run_dir(layout, lrun);
    return lrun;
}

LayoutRun* __mg_layout_run_new_from_offset(const LAYOUT* layout,
        const TextRun* trun, int offset)
{
    GlyphOrient ort;
    LOGFONT* lf;
    LayoutRun* lrun;

    if (offset >= trun->len) {
        _WRN_PRINTF("offset(%d) >= trun->len(%p, %d)\n",
            offset, trun, trun->len);
        return NULL;
    }

    ort = resolve_glyph_orient(layout, trun);
    lf = __mg_create_logfont_for_layout(layout, trun->fontname, ort);
    if (lf == NULL)
        return NULL;

    lrun = mg_slice_new(LayoutRun);

    lrun->lf = lf;
    lrun->si = trun->si + offset;
    lrun->ucs = layout->truns->ucs + lrun->si;
    lrun->len = trun->len - offset;
    lrun->lc = trun->lc;
    lrun->st = trun->st;
    lrun->el = trun->el;
    lrun->ort = ort;
    lrun->flags = trun->flags;

    resolve_layout_run_dir(layout, lrun);
    return lrun;
}

void __mg_layout_run_free(LayoutRun* lrun)
{
    if (lrun->lf) {
        FONT_RES* font_res = (FONT_RES*)lrun->lf;
        if (font_res->key)
            ReleaseRes(font_res->key);
    }

    mg_slice_delete(LayoutRun, lrun);
}

LayoutRun* __mg_layout_run_copy(const LayoutRun* lrun)
{
    LayoutRun *result;

    if (lrun == NULL)
        return NULL;

    result = mg_slice_new(LayoutRun);
    memcpy(result, lrun, sizeof(LayoutRun));

    // must increase the reference count of LOGFONT
    if (result->lf) {
        FONT_RES* font_res = (FONT_RES*)result->lf;
        AddResRef(font_res->key);
    }
    return result;
}

LayoutRun* __mg_layout_run_split(LayoutRun *orig, int split_index)
{
    LayoutRun *new_run;

    if (orig == NULL)
        return NULL;
    if (split_index <= 0)
        return NULL;
    if (split_index >= orig->len)
        return NULL;

    new_run = __mg_layout_run_copy(orig);
    new_run->len = split_index;

    orig->si += split_index;
    orig->len -= split_index;

    return new_run;
}

GlyphString* __mg_glyph_string_new (void)
{
    GlyphString *string = mg_slice_new0(GlyphString);
    return string;
}

void __mg_glyph_string_free (GlyphString *string)
{
    if (string == NULL)
        return;

    if (string->glyphs)
        free (string->glyphs);
    if (string->log_clusters)
        free (string->log_clusters);

    mg_slice_delete(GlyphString, string);
}

void __mg_glyph_string_set_size (GlyphString* string, int new_len)
{
    if (new_len < 0)
        return;

    while (new_len > string->space) {
        if (string->space == 0) {
            string->space = 4;
        }
        else {
            const unsigned int max_space =
                MIN (INT_MAX,
                    UINT_MAX / MAX (sizeof(ShapedGlyph), sizeof(int)));

            unsigned int more_space = (unsigned int)string->space * 2;

            if (more_space > max_space) {
                more_space = max_space;

                if ((unsigned int)new_len > max_space) {
                    _WRN_PRINTF("Too large glyph string of length %d\n", new_len);
                }
            }

            string->space = more_space;
        }
    }

    string->glyphs = realloc (string->glyphs,
            string->space * sizeof (ShapedGlyph));
    string->log_clusters = realloc (string->log_clusters,
            string->space * sizeof (int));
    string->nr_glyphs = new_len;
}

int __mg_glyph_string_get_width(const GlyphString *glyphs)
{
    int i;
    int width = 0;

    for (i = 0; i < glyphs->nr_glyphs; i++)
        width += glyphs->glyphs[i].width;

    return width;
}

void __mg_layout_line_free_runs(LAYOUTLINE* line)
{
    while (!list_empty(&line->gruns)) {
        GlyphRun* run = (GlyphRun*)line->gruns.prev;
        list_del(line->gruns.prev);
        __mg_glyph_run_free(run);
    }
}

void __mg_glyph_run_free(GlyphRun* run)
{
    if (run->lrun) {
        __mg_layout_run_free(run->lrun);
    }

    if (run->gstr) {
        __mg_glyph_string_free(run->gstr);
    }

    mg_slice_delete(GlyphRun, run);
}

#define LTR(glyph_run) (((glyph_run)->lrun->el & 1) == 0)

/**
 * __mg_glyph_run_split:
 * @orig: a #GlyphRun
 * @split_index: byte index of position to split item, relative to the start of the item
 *
 * Modifies @orig to cover only the text after @split_index, and
 * returns a new item that covers the text before @split_index that
 * used to be in @orig. You can think of @split_index as the length of
 * the returned item. @split_index may not be 0, and it may not be
 * greater than or equal to the length of @orig (that is, there must
 * be at least one byte assigned to each item, you can't create a
 * zero-length item).
 *
 * Return value: the newly allocated item representing text before
 *               @split_index, which should be freed
 *               with __mg_glyph_run_free().
 **/
GlyphRun *__mg_glyph_run_split (GlyphRun *orig, int split_index)
{
    GlyphRun *new_grun;
    int i;
    int nr_glyphs;
    int num_remaining;

    if (orig == NULL)
        return NULL;
    if (orig->lrun->len <= 0)
        return NULL;
    if (split_index <= 0)
        return NULL;
    if (split_index >= orig->lrun->len)
        return NULL;

    _DBG_PRINTF("%s: called, orig: %d, %d, split at: %d\n",
        __FUNCTION__, orig->lrun->si, orig->lrun->len, split_index);

    if (LTR (orig)) {
        for (i = 0; i < orig->gstr->nr_glyphs; i++) {
            if (orig->gstr->log_clusters[i] >= split_index)
                break;
        }

        if (i == orig->gstr->nr_glyphs)
            // No splitting necessary
            return NULL;

        split_index = orig->gstr->log_clusters[i];
        nr_glyphs = i;
    }
    else {
        for (i = orig->gstr->nr_glyphs - 1; i >= 0; i--) {
            if (orig->gstr->log_clusters[i] >= split_index)
                break;
        }

        if (i < 0)
            // No splitting necessary
            return NULL;

        split_index = orig->gstr->log_clusters[i];
        nr_glyphs = orig->gstr->nr_glyphs - 1 - i;
    }

    num_remaining = orig->gstr->nr_glyphs - nr_glyphs;

    new_grun = mg_slice_new(GlyphRun);
    new_grun->lrun = __mg_layout_run_split(orig->lrun, split_index);

    new_grun->gstr = __mg_glyph_string_new ();
    __mg_glyph_string_set_size (new_grun->gstr, nr_glyphs);

    if (LTR (orig)) {
        memcpy(new_grun->gstr->glyphs,
                orig->gstr->glyphs,
                nr_glyphs * sizeof(ShapedGlyph));
        memcpy(new_grun->gstr->log_clusters,
                orig->gstr->log_clusters,
                nr_glyphs * sizeof(int));

        memmove (orig->gstr->glyphs,
                orig->gstr->glyphs + nr_glyphs,
                num_remaining * sizeof(ShapedGlyph));
        for (i = nr_glyphs; i < orig->gstr->nr_glyphs; i++)
            orig->gstr->log_clusters[i - nr_glyphs] =
                    orig->gstr->log_clusters[i] - split_index;
    }
    else {
        memcpy (new_grun->gstr->glyphs,
            orig->gstr->glyphs + num_remaining,
            nr_glyphs * sizeof(ShapedGlyph));
        memcpy (new_grun->gstr->log_clusters,
            orig->gstr->log_clusters + num_remaining,
            nr_glyphs * sizeof(int));

        for (i = 0; i < num_remaining; i++)
            orig->gstr->log_clusters[i] =
                    orig->gstr->log_clusters[i] - split_index;
    }

    __mg_glyph_string_set_size (orig->gstr,
            orig->gstr->nr_glyphs - nr_glyphs);

    return new_grun;
}

BOOL __mg_glyph_run_iter_next_cluster (GlyphRunIter *iter)
{
    int glyph_index = iter->end_glyph;
    GlyphString *glyphs = iter->glyph_run->gstr;
    int cluster;
    const LayoutRun *item = iter->glyph_run->lrun;

    if (LTR (iter->glyph_run)) {
        if (glyph_index == glyphs->nr_glyphs)
            return FALSE;
    }
    else {
        if (glyph_index < 0)
            return FALSE;
    }

    iter->start_glyph = iter->end_glyph;
    iter->start_index = iter->end_index;
    iter->start_char = iter->end_char;

    if (LTR (iter->glyph_run)) {
        cluster = glyphs->log_clusters[glyph_index];
        while (TRUE) {
            glyph_index++;

            if (glyph_index == glyphs->nr_glyphs) {
                iter->end_index = item->si + item->len;
                iter->end_char = item->len;
                break;
            }

            if (glyphs->log_clusters[glyph_index] > cluster) {
                iter->end_index = item->si + glyphs->log_clusters[glyph_index];
                iter->end_char += iter->end_index - iter->start_index;
                break;
            }
        }
    }
    else {  /* RTL */
        cluster = glyphs->log_clusters[glyph_index];
        while (TRUE) {
            glyph_index--;

            if (glyph_index < 0) {
                iter->end_index = item->si + item->len;
                iter->end_char = item->len;
                break;
            }

            if (glyphs->log_clusters[glyph_index] > cluster) {
                iter->end_index = item->si + glyphs->log_clusters[glyph_index];
                iter->end_char += iter->end_index - iter->start_index;
                break;
            }
        }
    }

    iter->end_glyph = glyph_index;

    assert (iter->start_char < iter->end_char);
    assert (iter->end_char <= item->len);
    return TRUE;
}

BOOL __mg_glyph_run_iter_prev_cluster (GlyphRunIter *iter)
{
    int glyph_index = iter->start_glyph;
    GlyphString *glyphs = iter->glyph_run->gstr;
    int cluster;
    const LayoutRun *item = iter->glyph_run->lrun;

    if (LTR (iter->glyph_run)) {
        if (glyph_index == 0)
            return FALSE;
    }
    else {
        if (glyph_index == glyphs->nr_glyphs - 1)
            return FALSE;
    }

    iter->end_glyph = iter->start_glyph;
    iter->end_index = iter->start_index;
    iter->end_char = iter->start_char;

    if (LTR (iter->glyph_run)) {

        cluster = glyphs->log_clusters[glyph_index - 1];
        while (TRUE) {
            if (glyph_index == 0) {
                iter->start_index = item->si;
                iter->start_char = 0;
                break;
            }

            glyph_index--;

            if (glyphs->log_clusters[glyph_index] < cluster) {
                glyph_index++;
                iter->start_index = item->si + glyphs->log_clusters[glyph_index];
                iter->start_char -= iter->end_index - iter->start_index;
                break;
            }
        }
    }
    else { /* RTL */

        cluster = glyphs->log_clusters[glyph_index + 1];
        while (TRUE) {
            if (glyph_index == glyphs->nr_glyphs - 1) {
                iter->start_index = item->si;
                iter->start_char = 0;
                break;
            }

            glyph_index++;

            if (glyphs->log_clusters[glyph_index] < cluster) {
                glyph_index--;
                iter->start_index = item->si + glyphs->log_clusters[glyph_index];
                iter->start_char -= iter->end_index - iter->start_index;
                break;
            }
        }
    }

    iter->start_glyph = glyph_index;

    assert (iter->start_char < iter->end_char);
    assert (0 <= iter->start_char);

    return TRUE;
}

BOOL __mg_glyph_run_iter_init_start (GlyphRunIter  *iter,
        const GlyphRun *glyph_run, const Uchar32 *text)
{
    iter->glyph_run = glyph_run;
    iter->text = text;

    if (LTR (glyph_run))
        iter->end_glyph = 0;
    else
        iter->end_glyph = glyph_run->gstr->nr_glyphs - 1;

    iter->end_index = glyph_run->lrun->si;
    iter->end_char = 0;

    iter->start_glyph = iter->end_glyph;
    iter->start_index = iter->end_index;
    iter->start_char = iter->end_char;

    /* Advance onto the first cluster of the glyph item */
    return __mg_glyph_run_iter_next_cluster (iter);
}

BOOL __mg_glyph_run_iter_init_end (GlyphRunIter *iter,
        const GlyphRun *glyph_run, const Uchar32 *text)
{
    iter->glyph_run = glyph_run;
    iter->text = text;

    if (LTR (glyph_run))
        iter->start_glyph = glyph_run->gstr->nr_glyphs;
    else
        iter->start_glyph = -1;

    iter->start_index = glyph_run->lrun->si + glyph_run->lrun->len;
    iter->start_char = glyph_run->lrun->len;

    iter->end_glyph = iter->start_glyph;
    iter->end_index = iter->start_index;
    iter->end_char = iter->start_char;

    /* Advance onto the first cluster of the glyph item */
    return __mg_glyph_run_iter_prev_cluster (iter);
}

void __mg_glyph_run_get_logical_widths (const GlyphRun *glyph_run,
        const Uchar32 *text, int *logical_widths)
{
    GlyphRunIter iter;
    BOOL has_cluster;
    int dir;

    dir = glyph_run->lrun->el % 2 == 0 ? +1 : -1;
    for (has_cluster = __mg_glyph_run_iter_init_start (&iter, glyph_run, text);
            has_cluster;
            has_cluster = __mg_glyph_run_iter_next_cluster (&iter))
    {
        int glyph_index, char_index, num_chars, cluster_width = 0, char_width;

        for (glyph_index  = iter.start_glyph;
                glyph_index != iter.end_glyph;
                glyph_index += dir) {
            cluster_width += glyph_run->gstr->glyphs[glyph_index].width;
        }

        num_chars = iter.end_char - iter.start_char;
        if (num_chars) /* pedantic */
        {
            char_width = cluster_width / num_chars;

            for (char_index = iter.start_char;
                    char_index < iter.end_char;
                    char_index++)
            {
                logical_widths[char_index] = char_width;
            }

            /* add any residues to the first char */
            logical_widths[iter.start_char] += cluster_width - (char_width * num_chars);
        }
    }
}

void __mg_glyph_run_letter_space(const GlyphRun* glyph_run,
        const Uchar32* ucs, const BreakOppo* bos, int letter_spacing)
{
    GlyphRunIter iter;
    ShapedGlyph *glyphs = glyph_run->gstr->glyphs;
    BOOL have_cluster;
    int space_left, space_right;

    space_left = letter_spacing / 2;

    /* hinting */
    if ((letter_spacing & 1) == 0) {
        space_left += 1;
    }

    space_right = letter_spacing - space_left;

    for (have_cluster = __mg_glyph_run_iter_init_start(&iter, glyph_run, ucs);
            have_cluster;
            have_cluster = __mg_glyph_run_iter_next_cluster(&iter)) {

        if (!(bos[iter.start_char] & BOV_GB_CURSOR_POS))
            continue;

        if (iter.start_glyph < iter.end_glyph) {
            /* LTR */

            if (iter.start_char > 0) {
                glyphs[iter.start_glyph].width += space_left ;
                glyphs[iter.start_glyph].x_off += space_left ;
            }

            if (iter.end_char < glyph_run->lrun->len) {
                glyphs[iter.end_glyph-1].width += space_right;
            }
        }
        else {
            /* RTL */

            if (iter.start_char > 0) {
                glyphs[iter.start_glyph].width += space_right;
            }

            if (iter.end_char < glyph_run->lrun->len) {
                glyphs[iter.end_glyph+1].x_off += space_left ;
                glyphs[iter.end_glyph+1].width += space_left ;
            }
        }
    }
}

void __mg_reverse_shaped_glyphs(ShapedGlyph* glyphs, int len)
{
    int i;
    for (i = 0; i < len / 2; i++) {
        ShapedGlyph tmp = glyphs[i];
        glyphs[i] = glyphs[len - 1 - i];
        glyphs[len - 1 - i] = tmp;
    }
}

void __mg_reverse_log_clusters(int* clusters, int len)
{
    int i;
    for (i = 0; i < len / 2; i++) {
        int tmp = clusters[i];
        clusters[i] = clusters[len - 1 - i];
        clusters[len - 1 - i] = tmp;
    }
}

#endif /*  _MGCHARSET_UNICODE */

