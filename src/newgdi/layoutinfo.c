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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

/*
** layoutinfo.c: The implementation of APIs related LAYOUTINFO
**
** Create by WEI Yongming at 2019/03/20
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
#include "layoutinfo.h"

LAYOUTINFO* GUIAPI CreateLayoutInfo(
        const TEXTRUNSINFO* truninfo, Uint32 render_flags,
        const BreakOppo* break_oppos, BOOL persist_lines,
        int letter_spacing, int word_spacing, int tab_size)
{
    LAYOUTINFO* layout;

    if (truninfo == NULL || truninfo->sei.inst == NULL) {
        return NULL;
    }

    layout = (LAYOUTINFO*)calloc(1, sizeof(LAYOUTINFO));
    if (layout == NULL) {
        return NULL;
    }

    layout->truninfo = truninfo;
    layout->bos = break_oppos;
    layout->rf = render_flags;
    layout->ls = letter_spacing;
    layout->ws = word_spacing;
    layout->ts = tab_size;

    INIT_LIST_HEAD(&layout->line_head);
    layout->left_ucs = truninfo->nr_ucs;

    layout->persist = persist_lines ? 1 : 0;

    return layout;
}

static void release_glyph_string(GLYPHSTRING* gs)
{
    if (gs->glyphs)
        free (gs->glyphs);
    if (gs->log_clusters);
        free (gs->log_clusters);

    free(gs);
}

static void release_run(GLYPHRUN* run)
{
    if (run->gs) {
        release_glyph_string(run->gs);
    }

    free(run);
}

static void release_line(LAYOUTLINE* line)
{
    while (!list_empty(&line->grun_head)) {
        GLYPHRUN* run = (GLYPHRUN*)line->grun_head.prev;
        list_del(line->grun_head.prev);
        release_run(run);
    }

    if (line->log_widths) {
        free(line->log_widths);
    }

    free(line);
}

BOOL GUIAPI DestroyLayoutInfo(LAYOUTINFO* info)
{
    while (!list_empty(&info->line_head)) {
        LAYOUTLINE* line = (LAYOUTLINE*)info->line_head.prev;
        list_del(info->line_head.prev);
        release_line(line);
    }

    free(info);
    return TRUE;
}

typedef struct _LayoutState LayoutState;

typedef enum {
    BREAK_NONE_FIT,
    BREAK_SOME_FIT,
    BREAK_ALL_FIT,
    BREAK_EMPTY_FIT,
    BREAK_LINE_SEPARATOR
} BreakResult;

struct _LayoutState {
    /* maintained per layout */
    /* is last line */
    Uint32 last_line:1;
    Uint32 shape_set:1;

    RECT shape_ink_rect, shape_logical_rect;

    /* maintained per paragraph */

    /* Current text run */
    TEXTRUN* item;

    /* Current resolved base direction */
    GlyphRunDir base_dir;

    /* Line of the paragraph, starting at 1 for first line */
    int line_of_par;

    /* Glyphs for the current text run */
    GLYPHSTRING* glyphs;

    /* Character offset of first item in state->item in layout->truninfo->ucs */
    int start_offset;

    /* Logical widths for the current text run */
    int *log_widths;

    /* Offset into log_widths to the point corresponding
     * to the remaining portion of the first item */
    int log_widths_offset;

    /* Start index of line in layout->truninfo->ucs */
    int line_start_index;

    /* maintained per line */
    /* Goal width of line currently processing; < 0 is infinite */
    int line_width;

    /* Amount of space remaining on line; < 0 is infinite */
    int remaining_width;
};

static BOOL should_ellipsize_current_line(LAYOUTINFO *layout,
        LayoutState *state)
{
    if (((layout->rf & GRF_OVERFLOW_ELLIPSIZE_MASK) ==
            GRF_OVERFLOW_ELLIPSIZE_NONE) || state->line_width < 0)
        return FALSE;

    if (!state->last_line) {
        return FALSE;
    }

    return TRUE;
}

static void shape_tab(LAYOUTLINE *line, GLYPHSTRING *glyphs)
{
}

static void shape_shape(const Uchar32* ucs, int nr_ucs,
        RECT* ink_rc, RECT* log_rc, GLYPHSTRING *glyphs)
{
    unsigned int i;

    __mg_glyph_string_set_size (glyphs, nr_ucs);

    for (i = 0; i < nr_ucs; i++) {
        glyphs->glyphs[i].gv = INV_GLYPH_VALUE;
        glyphs->glyphs[i].x_off = 0;
        glyphs->glyphs[i].y_off = 0;
        glyphs->glyphs[i].width = RECTWP(log_rc);
        glyphs->glyphs[i].is_cluster_start = 1;

        glyphs->log_clusters[i] = i;
    }
}

static void shape_full(const Uchar32* item_ucs, int nr_item_ucs,
        const Uchar32* para_ucs, int nr_para_ucs,
        const TEXTRUNSINFO* truninfo, const TEXTRUN* textrun,
        GLYPHSTRING* glyphs)
{
}

static void distribute_letter_spacing (int letter_spacing,
        int *space_left, int *space_right)
{
    *space_left = letter_spacing / 2;
    /* hinting */
    if (letter_spacing & 1) {
        *space_left += 1;
    }

    *space_right = letter_spacing - *space_left;
}

static GLYPHSTRING* shape_run(LAYOUTLINE *line, LayoutState *state,
        TEXTRUN *item)
{
    LAYOUTINFO *layout = line->layout;
    GLYPHSTRING *glyphs = __mg_glyph_string_new ();

    if (layout->truninfo->ucs[item->si] == '\t')
        shape_tab(line, glyphs);
    else {
        if (state->shape_set)
            shape_shape(layout->truninfo->ucs + item->si, item->len,
                    &state->shape_ink_rect,
                    &state->shape_logical_rect, glyphs);
        else
            shape_full(layout->truninfo->ucs + item->si, item->len,
                    layout->truninfo->ucs, layout->truninfo->nr_ucs,
                    layout->truninfo, item, glyphs);

        if (layout->ls) {
            GlyphItem glyph_item;
            int space_left, space_right;

            glyph_item.item = item;
            glyph_item.glyphs = glyphs;

            __mg_glyph_item_letter_space (&glyph_item,
                    layout->truninfo->ucs,
                    layout->bos + state->start_offset,
                    layout->ls);

            distribute_letter_spacing (layout->ls, &space_left, &space_right);

            glyphs->glyphs[0].width += space_left;
            glyphs->glyphs[0].x_off += space_left;
            glyphs->glyphs[glyphs->nr_glyphs - 1].width += space_right;
        }
    }

    return glyphs;
}

static GLYPHRUN* insert_run (LAYOUTLINE *line, LayoutState *state,
        TEXTRUN *text_run, int si, int len, BOOL last_run)
{
    GLYPHRUN *glyph_run = malloc (sizeof(GLYPHRUN));

    glyph_run->trun = text_run;

    if (last_run && state->log_widths_offset == 0)
        glyph_run->gs = state->glyphs;
    else
        glyph_run->gs = shape_run (line, state, text_run);

    if (last_run) {
        if (state->log_widths_offset > 0)
            __mg_glyph_string_free (state->glyphs);
        state->glyphs = NULL;
        free (state->log_widths);
        state->log_widths = NULL;
    }

    glyph_run->si = si;
    glyph_run->len = len;

    list_add_tail(&glyph_run->list, &line->grun_head);
    line->len += text_run->len;

    return glyph_run;
}

static inline BOOL can_break_at (LAYOUTINFO *layout,
        int offset, BOOL always_wrap_char)
{
    Uint32 wrap = layout->rf & GRF_OVERFLOW_WRAP_MASK;

    if (offset == layout->truninfo->nr_ucs)
        return TRUE;

    if (wrap == GRF_OVERFLOW_WRAP_NORMAL)
        wrap = always_wrap_char ? GRF_OVERFLOW_WRAP_ANYWHERE :
                GRF_OVERFLOW_WRAP_BREAK_WORD;

    else if (wrap == GRF_OVERFLOW_WRAP_BREAK_WORD)
        return layout->bos[offset] & BOV_LB_BREAK_FLAG;
    else if (wrap == GRF_OVERFLOW_WRAP_ANYWHERE)
        return layout->bos[offset] & BOV_GB_CHAR_BREAK;
    else {
        _WRN_PRINTF ("broken LayoutInfo\n");
    }

    return TRUE;
}

/*
 * Tries to insert as much as possible of the text runs at the head of
 * state->items onto @line. Five results are possible:
 *
 *  %BREAK_NONE_FIT: Couldn't fit anything.
 *  %BREAK_SOME_FIT: The item was broken in the middle.
 *  %BREAK_ALL_FIT: Everything fit.
 *  %BREAK_EMPTY_FIT: Nothing fit, but that was ok, as we can break at the first char.
 *  %BREAK_LINE_SEPARATOR: The text runs begins with a line separator.
 *
 * If @force_fit is %TRUE, then %BREAK_NONE_FIT will never
 * be returned, a run will be added even if inserting the minimum amount
 * will cause the line to overflow. This is used at the start of a line
 * and until we've found at least some place to break.
 *
 * If @no_break_at_end is %TRUE, then %BREAK_ALL_FIT will never be
 * returned even everything fits; the run will be broken earlier,
 * or %BREAK_NONE_FIT returned. This is used when the end of the
 * run is not a break position.
 */
BreakResult process_one_text_run(LAYOUTINFO *layout,
        LAYOUTLINE *line, LayoutState *state,
        BOOL force_fit, BOOL no_break_at_end)
{
    TEXTRUN *item = state->item;
    BOOL shape_set = FALSE;
    int width;
    int i;
    BOOL processing_new_item = FALSE;

    /* Only one character has type UCHAR_CATEGORY_LINE_SEPARATOR in
     * Unicode 12.0; update this if that changes. */
#define LINE_SEPARATOR 0x2028

    if (!state->glyphs) {
        state->glyphs = shape_run (line, state, item);

        state->log_widths = NULL;
        state->log_widths_offset = 0;

        processing_new_item = TRUE;
    }

    if (!layout->single_paragraph &&
            layout->truninfo->ucs[item->si] == LINE_SEPARATOR &&
            !should_ellipsize_current_line (layout, state)) {
        insert_run (line, state, item, 0, item->len, TRUE);
        state->log_widths_offset += item->len;
        return BREAK_LINE_SEPARATOR;
    }

    if (state->remaining_width < 0 && !no_break_at_end)  /* Wrapping off */
    {
        insert_run (line, state, item, 0, item->len, TRUE);

        return BREAK_ALL_FIT;
    }

    width = 0;
    if (processing_new_item) {
        width = __mg_glyph_string_get_width (state->glyphs);
    }
    else {
        for (i = 0; i < item->len; i++)
            width += state->log_widths[state->log_widths_offset + i];
    }

    if ((width <= state->remaining_width ||
                (item->len == 1 && list_empty(&line->grun_head))) &&
            !no_break_at_end) {
        state->remaining_width -= width;
        state->remaining_width = MAX (state->remaining_width, 0);
        insert_run (line, state, item, 0, item->len, TRUE);

        return BREAK_ALL_FIT;
    }
    else {
        int num_chars = item->len;
        int break_num_chars = num_chars;
        int break_width = width;
        int orig_width = width;
        BOOL retrying_with_char_breaks = FALSE;

        if (processing_new_item) {
            GlyphItem glyph_item = {item, state->glyphs};
            state->log_widths = malloc (sizeof (int) * item->len);
            __mg_glyph_item_get_logical_widths(&glyph_item, layout->truninfo->ucs,
                state->log_widths);
        }

retry_break:

        /* See how much of the item we can stuff in the line. */
        width = 0;
        for (num_chars = 0; num_chars < item->len; num_chars++) {
            if (width > state->remaining_width && break_num_chars < item->len)
                break;

            /* If there are no previous runs we have to take care to grab at least one char. */
            if (can_break_at (layout, state->start_offset + num_chars,
                        retrying_with_char_breaks) &&
                    (num_chars > 0 || !list_empty(&line->grun_head)))
            {
                break_num_chars = num_chars;
                break_width = width;
            }

            width += state->log_widths[state->log_widths_offset + num_chars];
        }

        /* If there's a space at the end of the line, include that also.
         * The logic here should match zero_line_final_space().
         * XXX Currently it doesn't quite match the logic there.  We don't check
         * the cluster here.  But should be fine in practice. */
        if (break_num_chars > 0 && break_num_chars < item->len &&
                layout->bos[state->start_offset + break_num_chars - 1] & BOV_WHITESPACE)
        {
            break_width -= state->log_widths[state->log_widths_offset + break_num_chars - 1];
        }

        if ((layout->rf & GRF_OVERFLOW_WRAP_MASK) == GRF_OVERFLOW_WRAP_NORMAL
                && force_fit
                && break_width > state->remaining_width
                && !retrying_with_char_breaks) {
            retrying_with_char_breaks = TRUE;
            num_chars = item->len;
            width = orig_width;
            break_num_chars = num_chars;
            break_width = width;
            goto retry_break;
        }

        if (force_fit || break_width <= state->remaining_width)	/* Successfully broke the item */
        {
            if (state->remaining_width >= 0) {
                state->remaining_width -= break_width;
                state->remaining_width = MAX (state->remaining_width, 0);
            }

            if (break_num_chars == item->len) {
                insert_run (line, state, item, 0, item->len, TRUE);
                return BREAK_ALL_FIT;
            }
            else if (break_num_chars == 0) {
                return BREAK_EMPTY_FIT;
            }
            else {
                GLYPHRUN *grun;

                /* Add the width back, to the line, reshape,
                   subtract the new width */
                state->remaining_width += break_width;
                grun = insert_run (line, state, item, 0, break_num_chars, FALSE);
                break_width = __mg_glyph_string_get_width (grun->gs);
                state->remaining_width -= break_width;

                state->log_widths_offset += break_num_chars;

                /* Shaped items should never be broken */
                assert (!shape_set);

                return BREAK_SOME_FIT;
            }
        }
        else {
            __mg_glyph_string_free (state->glyphs);
            state->glyphs = NULL;
            free (state->log_widths);
            state->log_widths = NULL;

            return BREAK_NONE_FIT;
        }
    }
}

LAYOUTLINE* GUIAPI LayoutNextLine(
        LAYOUTINFO* info, LAYOUTLINE* prev_Line,
        int x, int y, int max_extent, BOOL last_line, SIZE* line_size,
        CB_GLYPH_LAID_OUT cb_laid_out, GHANDLE ctxt)
{
    return NULL;
}

#if 0
int GUIAPI DrawShapedGlyphString(HDC hdc,
        LOGFONT* logfont_upright, LOGFONT* logfont_sideways,
        const SHAPEDGLYPHS* shaped_glyphs,
        const GLYPHPOS* glyph_pos, int nr_glyphs)
{
    int i;
    int n = 0;
    Uint32 old_ta;
    PLOGFONT old_lf;

    if (shaped_glyphs == NULL || glyph_pos == NULL || nr_glyphs <= 0)
        return 0;

    old_ta = SetTextAlign(hdc, TA_LEFT | TA_TOP | TA_UPDATECP);
    old_lf = GetCurFont(hdc);

    for (i = 0; i < nr_glyphs; i++) {
        if (glyph_pos[i].suppressed == 0 && glyph_pos[i].whitespace == 0) {
            Glyph32 gv = shaped_glyphs->cb_get_glyph_info(
                    shaped_glyphs->shaping_engine, shaped_glyphs->glyph_infos,
                    i, NULL);
            if (glyph_pos[i].orientation == GLYPH_ORIENTATION_UPRIGHT) {
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

            DrawGlyph(hdc, glyph_pos[i].x, glyph_pos[i].y, gv,
                NULL, NULL);

            n++;
        }
    }

error:
    SelectFont(hdc, old_lf);
    SetTextAlign(hdc, old_ta);

    return n;
}
#endif

#endif /*  _MGCHARSET_UNICODE */

