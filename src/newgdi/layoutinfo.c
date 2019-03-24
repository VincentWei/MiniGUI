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
 *   For more layoutrmation about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

/*
** layoutlayout.c: The implementation of APIs related LAYOUTINFO
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

    INIT_LIST_HEAD(&layout->lines);
    layout->nr_left_ucs = truninfo->nr_ucs;

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
    while (!list_empty(&line->gruns)) {
        GLYPHRUN* run = (GLYPHRUN*)line->gruns.prev;
        list_del(line->gruns.prev);
        release_run(run);
    }

    if (line->log_widths) {
        free(line->log_widths);
    }

    free(line);
}

BOOL GUIAPI DestroyLayoutInfo(LAYOUTINFO* layout)
{
    while (!list_empty(&layout->lines)) {
        LAYOUTLINE* line = (LAYOUTLINE*)layout->lines.prev;
        list_del(layout->lines.prev);
        release_line(line);
    }

    free(layout);
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
    // is last line
    Uint32 last_line:1;
    Uint32 shape_set:1;

    RECT shape_ink_rect, shape_logical_rect;

    /* maintained per paragraph */
    // Current resolved base direction
    GlyphRunDir base_dir;
    // Line of the paragraph, starting at 1 for first line
    int line_of_par;
    // Glyphs for the current glyph run
    GLYPHSTRING* glyphs;
    // Logical widths for the current text run */
    int *log_widths;
    // Offset into log_widths to the point corresponding
    // to the remaining portion of the first item
    int log_widths_offset;

    // Character offset of first item in state->item in layout->truninfo->ucs
    int start_offset;

    /* maintained per line */
    // Start index of line in layout->text
    int line_start_index;
    // Current text run
    const TEXTRUN* item;
    // Start index of line in current item */
    int start_index_in_item;
    // the number of not fit uchars in current text run
    int left_ucs_in_item;

    // Goal width of line currently processing; < 0 is infinite
    int line_width;
    // Amount of space remaining on line; < 0 is infinite
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
        const TEXTRUN *item, int offset, int len)
{
    LAYOUTINFO *layout = line->layout;
    GLYPHSTRING *glyphs = __mg_glyph_string_new ();

    if (layout->truninfo->ucs[item->si + offset] == '\t')
        shape_tab(line, glyphs);
    else {
        if (state->shape_set)
            shape_shape(layout->truninfo->ucs + item->si + offset, len,
                    &state->shape_ink_rect,
                    &state->shape_logical_rect, glyphs);
        else
            shape_full(layout->truninfo->ucs + item->si + offset, len,
                    layout->truninfo->ucs, layout->truninfo->nr_ucs,
                    layout->truninfo, item, glyphs);

        if (layout->ls) {
            GlyphItem glyph_item;
            int space_left, space_right;

            glyph_item.trun = item;
            glyph_item.gs = glyphs;
            glyph_item.so = offset;
            glyph_item.len = len;

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

static void free_run (GLYPHRUN *run)
{
    __mg_glyph_string_free(run->gs);
    free(run);
}

static const TEXTRUN *uninsert_run (LAYOUTLINE *line)
{
    GLYPHRUN *run;
    const TEXTRUN *item;

    run = (GLYPHRUN*)&line->gruns.next;
    item = run->trun;

    list_del(line->gruns.next);
    line->len -= run->len;

    free_run(run);
    return item;
}

static GLYPHRUN* insert_run(LAYOUTLINE *line, LayoutState *state,
        const TEXTRUN *text_run, int so, int len, BOOL last_run)
{
    GLYPHRUN *glyph_run = malloc(sizeof(GLYPHRUN));

    glyph_run->trun = text_run;
    glyph_run->so = so;
    glyph_run->len = len;

    if (last_run && state->log_widths_offset == 0)
        glyph_run->gs = state->glyphs;
    else
        glyph_run->gs = shape_run(line, state, text_run, so, len);

    if (last_run) {
        if (state->log_widths_offset > 0)
            __mg_glyph_string_free(state->glyphs);
        state->glyphs = NULL;
        free(state->log_widths);
        state->log_widths = NULL;
    }

    list_add_tail(&glyph_run->list, &line->gruns);
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
        _WRN_PRINTF ("broken LayoutInfo");
    }

    return TRUE;
}

static inline BOOL can_break_in (LAYOUTINFO *layout,
        int  start_offset, int  num_chars, BOOL allow_break_at_start)
{
    int i;

    for (i = allow_break_at_start ? 0 : 1; i < num_chars; i++)
        if (can_break_at (layout, start_offset + i, FALSE))
            return TRUE;

    return FALSE;
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
BreakResult process_text_run(LAYOUTINFO *layout,
        LAYOUTLINE *line, LayoutState *state,
        BOOL force_fit, BOOL no_break_at_end)
{
    const TEXTRUN *item = state->item;
    BOOL shape_set = FALSE;
    int width;
    int i;
    BOOL processing_new_item = FALSE;

    /* Only one character has type UCHAR_CATEGORY_LINE_SEPARATOR in
     * Unicode 12.0; update this if that changes. */
#define LINE_SEPARATOR 0x2028

    if (!state->glyphs) {
        state->glyphs = shape_run (line, state, item, 0, item->len);

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
                (item->len == 1 && list_empty(&line->gruns))) &&
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

            GlyphItem glyph_item;
            glyph_item.trun = item;
            glyph_item.gs = state->glyphs;

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
                    (num_chars > 0 || !list_empty(&line->gruns)))
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

static LAYOUTLINE *layout_line_new(LAYOUTINFO *layout)
{
    LAYOUTLINE *line = calloc (1, sizeof (LAYOUTLINE));

    line->layout = layout;
    INIT_LIST_HEAD(&line->gruns);

    return (LAYOUTLINE*)line;
}

/* The resolved direction for the line is always one
 * of LTR/RTL; not a week or neutral directions
 */
static void line_set_resolved_dir(LAYOUTLINE *line, GlyphRunDir direction)
{
    switch (direction) {
    default:
    case GLYPH_RUN_DIR_LTR:
    case GLYPH_RUN_DIR_WEAK_LTR:
    case GLYPH_RUN_DIR_NEUTRAL:
        line->resolved_dir = GLYPH_RUN_DIR_LTR;
        break;
    case GLYPH_RUN_DIR_RTL:
    case GLYPH_RUN_DIR_WEAK_RTL:
        line->resolved_dir = GLYPH_RUN_DIR_RTL;
        break;
    }

    /* The direction vs. gravity dance:
     *  If gravity is SOUTH, leave direction untouched.
     *  If gravity is NORTH, switch direction.
     *  If gravity is EAST, set to LTR, as
     *      it's a clockwise-rotated layout, so the rotated
     *      top is unrotated left.
     *  If gravity is WEST, set to RTL, as
     *      it's a counter-clockwise-rotated layout, so the rotated
     *      top is unrotated right.
     *
     * A similar dance is performed in textrunsinfo.c:
     * state_add_character().  Keep in sync.
     */
    switch (line->layout->truninfo->ort_rsv) {
        default:
        case GLYPH_GRAVITY_AUTO:
        case GLYPH_GRAVITY_SOUTH:
            break;
        case GLYPH_GRAVITY_NORTH:
            line->resolved_dir = GLYPH_RUN_DIR_LTR
                + GLYPH_RUN_DIR_RTL
                - line->resolved_dir;
            break;
        case GLYPH_GRAVITY_EAST:
            line->resolved_dir = GLYPH_RUN_DIR_LTR;
            break;
        case GLYPH_GRAVITY_WEST:
            line->resolved_dir = GLYPH_RUN_DIR_RTL;
            break;
    }
}

static void reorder_runs(LAYOUTLINE *line, int nr_runs)
{
}

static void reverse_runs(LAYOUTLINE *line)
{
}

static void layout_line_reorder(LAYOUTLINE *line)
{
    struct list_head *i;
    BOOL all_even, all_odd;
    Uint8 level_or = 0, level_and = 1;
    int length = 0;

    /* Check if all items are in the same direction, in that case, the
     * line does not need modification and we can avoid the expensive
     * reorder runs recurse procedure.
     */

    list_for_each(i, &line->gruns) {
        GLYPHRUN *grun = (GLYPHRUN*)i;

        level_or |= grun->trun->el;
        level_and &= grun->trun->el;
        length++;
    }

    /* If none of the levels had the LSB set, all numbers were even. */
    all_even = (level_or & 0x1) == 0;

    /* If all of the levels had the LSB set, all numbers were odd. */
    all_odd = (level_and & 0x1) == 1;

    if (!all_even && !all_odd) {
        reorder_runs(line, length);
    }
    else if (all_odd)
        reverse_runs(line);
}

static void zero_line_final_space (LAYOUTLINE *line,
        LayoutState *state, GLYPHRUN *run)
{
    LAYOUTINFO *layout = line->layout;
    const TEXTRUN *item = run->trun;
    GLYPHSTRING *glyphs = run->gs;
    int glyph = item->el % 2 ? 0 : glyphs->nr_glyphs - 1;

    /* if the final char of line forms a cluster, and it's
     * a whitespace char, zero its glyph's width as it's been wrapped
     */
    if (glyphs->nr_glyphs < 1 || state->start_offset == 0 ||
            !(layout->bos[state->start_offset - 1] & BOV_WHITESPACE))
        return;

    if (glyphs->nr_glyphs >= 2 &&
            glyphs->log_clusters[glyph] ==
                glyphs->log_clusters[glyph + (item->el % 2 ? 1 : -1)])
        return;

    state->remaining_width += glyphs->glyphs[glyph].width;
    glyphs->glyphs[glyph].width = 0;
}

static void pad_glyphstring_right (GLYPHSTRING *glyphs,
        LayoutState *state, int adjustment)
{
    int glyph = glyphs->nr_glyphs - 1;

    while (glyph >= 0 && glyphs->glyphs[glyph].width == 0)
        glyph--;

    if (glyph < 0)
        return;

    state->remaining_width -= adjustment;
    glyphs->glyphs[glyph].width += adjustment;
    if (glyphs->glyphs[glyph].width < 0) {
        state->remaining_width += glyphs->glyphs[glyph].width;
        glyphs->glyphs[glyph].width = 0;
    }
}

static void pad_glyphstring_left (GLYPHSTRING *glyphs,
        LayoutState *state, int adjustment)
{
    int glyph = 0;

    while (glyph < glyphs->nr_glyphs && glyphs->glyphs[glyph].width == 0)
        glyph++;

    if (glyph == glyphs->nr_glyphs)
        return;

    state->remaining_width -= adjustment;
    glyphs->glyphs[glyph].width += adjustment;
    glyphs->glyphs[glyph].x_off += adjustment;
}

static inline BOOL is_tab_run(LAYOUTINFO *layout, GLYPHRUN *grun)
{
    return (layout->truninfo->ucs[grun->trun->si] == '\t');
}

/* When doing shaping, we add the letter spacing value for a
 * run after every grapheme in the run. This produces ugly
 * asymmetrical results, so what this routine is redistributes
 * that space to the beginning and the end of the run.
 *
 * We also trim the letter spacing from runs adjacent to
 * tabs and from the outside runs of the lines so that things
 * line up properly. The line breaking and tab positioning
 * were computed without this trimming so they are no longer
 * exactly correct, but this won't be very noticeable in most
 * cases.
 */
static void adjust_line_letter_spacing(LAYOUTLINE *line, LayoutState *state)
{
    LAYOUTINFO *layout = line->layout;
    BOOL reversed;
    GLYPHRUN *last_run;
    int tab_adjustment;
    struct list_head *l;

    /* If we have tab stops and the resolved direction of the
     * line is RTL, then we need to walk through the line
     * in reverse direction to figure out the corrections for
     * tab stops.
     */
    reversed = FALSE;
    if (line->resolved_dir == GLYPH_RUN_DIR_RTL) {
        list_for_each(l, &line->gruns) {
            if (is_tab_run (layout, (GLYPHRUN*)l)) {
                reverse_runs(line);
                reversed = TRUE;
                break;
            }
        }
    }

    /* Walk over the runs in the line, redistributing letter
     * spacing from the end of the run to the start of the
     * run and trimming letter spacing from the ends of the
     * runs adjacent to the ends of the line or tab stops.
     *
     * We accumulate a correction factor from this trimming
     * which we add onto the next tab stop space to keep the
     * things properly aligned.
     */

    last_run = NULL;
    tab_adjustment = 0;
    list_for_each(l, &line->gruns) {
        GLYPHRUN *run = (GLYPHRUN*)l;
        GLYPHRUN *next_run;

        if (l->next == &line->gruns) {
            next_run = NULL;
        }
        else {
            next_run = (GLYPHRUN*)l->next;
        }

        if (is_tab_run (layout, run)) {
            pad_glyphstring_right (run->gs, state, tab_adjustment);
            tab_adjustment = 0;
        }
        else {
            GLYPHRUN *visual_next_run = reversed ? last_run : next_run;
            GLYPHRUN *visual_last_run = reversed ? next_run : last_run;
            int run_spacing = line->layout->ls;
            int space_left, space_right;

            distribute_letter_spacing (run_spacing, &space_left, &space_right);

            if (run->gs->glyphs[0].width == 0) {
                /* we've zeroed this space glyph at the end of line, now remove
                 * the letter spacing added to its adjacent glyph */
                pad_glyphstring_left (run->gs, state, -space_left);
            }
            else if (!visual_last_run || is_tab_run(layout, visual_last_run)) {
                pad_glyphstring_left(run->gs, state, -space_left);
                tab_adjustment += space_left;
            }

            if (run->gs->glyphs[run->gs->nr_glyphs - 1].width == 0) {
                /* we've zeroed this space glyph at the end of line, now remove
                 * the letter spacing added to its adjacent glyph */
                pad_glyphstring_right (run->gs, state, -space_right);
            }
            else if (!visual_next_run || is_tab_run(layout, visual_next_run)) {
                pad_glyphstring_right(run->gs, state, - space_right);
                tab_adjustment += space_right;
            }
        }

        last_run = run;
    }

    if (reversed)
        reverse_runs (line);
}

static void justify_clusters (LAYOUTLINE *line, LayoutState *state)
{
    const Uchar32 *text = line->layout->truninfo->ucs;
    const BreakOppo *bos = line->layout->bos;

    int total_remaining_width, total_gaps = 0;
    int added_so_far, gaps_so_far;
    BOOL is_hinted;
    struct list_head *run_iter;
    enum {
        MEASURE,
        ADJUST
    } mode;

    total_remaining_width = state->remaining_width;
    if (total_remaining_width <= 0)
        return;

    /* hint to full pixel if total remaining width was so */
    is_hinted = (total_remaining_width & 1) == 0;

    for (mode = MEASURE; mode <= ADJUST; mode++) {
        BOOL leftedge = TRUE;
        GLYPHSTRING *rightmost_glyphs = NULL;
        int rightmost_space = 0;
        int residual = 0;

        added_so_far = 0;
        gaps_so_far = 0;

        list_for_each(run_iter, &line->gruns) {
            GLYPHRUN *run = (GLYPHRUN*)run_iter;
            GLYPHSTRING *glyphs = run->gs;
            GlyphItemIter cluster_iter;
            BOOL have_cluster;
            int dir;
            int offset;

            dir = run->trun->el % 2 == 0 ? +1 : -1;

            /* We need character offset of the start of the run.
             * We don't have this.
             * Compute by counting from the beginning of the line.
             * The naming is confusing.  Note that:
             *
             * run->trun->si is the index of start of run.
             * state->line_start_index is the index of start of line.
             */
            assert(run->trun->si >= state->line_start_index);
            offset = state->line_start_index + run->trun->si;

            if ((have_cluster = (dir > 0)))
                __mg_glyph_item_iter_init_start(&cluster_iter, run, text);
            else
                __mg_glyph_item_iter_init_end(&cluster_iter, run, text);

            for (; have_cluster;
                    have_cluster = dir > 0 ?
                    __mg_glyph_item_iter_next_cluster (&cluster_iter) :
                    __mg_glyph_item_iter_prev_cluster (&cluster_iter)) {
                int i;
                int width = 0;

                /* don't expand in the middle of graphemes */
                if (!(bos[offset + cluster_iter.start_char] & BOV_GB_CURSOR_POS))
                    continue;

                for (i = cluster_iter.start_glyph;
                        i != cluster_iter.end_glyph; i += dir)
                    width += glyphs->glyphs[i].width;

                /* also don't expand zero-width clusters. */
                if (width == 0)
                    continue;

                gaps_so_far++;

                if (mode == ADJUST) {
                    int leftmost, rightmost;
                    int adjustment, space_left, space_right;

                    adjustment = total_remaining_width / total_gaps + residual;
                    if (is_hinted) {
                        int old_adjustment = adjustment;
                        residual = old_adjustment - adjustment;
                    }
                    /* distribute to before/after */
                    distribute_letter_spacing(adjustment,
                            &space_left, &space_right);

                    if (cluster_iter.start_glyph < cluster_iter.end_glyph) {
                        /* LTR */
                        leftmost  = cluster_iter.start_glyph;
                        rightmost = cluster_iter.end_glyph - 1;
                    }
                    else {
                        /* RTL */
                        leftmost  = cluster_iter.end_glyph + 1;
                        rightmost = cluster_iter.start_glyph;
                    }
                    /* Don't add to left-side of left-most glyph of
                       left-most non-zero run. */
                    if (leftedge)
                        leftedge = FALSE;
                    else {
                        glyphs->glyphs[leftmost].width += space_left;
                        glyphs->glyphs[leftmost].x_off += space_left;
                        added_so_far += space_left;
                    }
                    /* Don't add to right-side of right-most glyph of
                       right-most non-zero run. */
                    {
                        /* Save so we can undo later. */
                        rightmost_glyphs = glyphs;
                        rightmost_space = space_right;

                        glyphs->glyphs[rightmost].width  += space_right;
                        added_so_far += space_right;
                    }
                }
            }
        }

        if (mode == MEASURE) {
            total_gaps = gaps_so_far - 1;

            if (total_gaps == 0) {
                /* a single cluster, can't really justify it */
                return;
            }
        }
        else {
            /* mode == ADJUST */
            if (rightmost_glyphs) {
                rightmost_glyphs->glyphs[rightmost_glyphs->nr_glyphs - 1].width -= rightmost_space;
                added_so_far -= rightmost_space;
            }
        }
    }

    state->remaining_width -= added_so_far;
}

static void justify_words (LAYOUTLINE *line,
        LayoutState  *state)
{
    const Uchar32 *text = line->layout->truninfo->ucs;
    const BreakOppo *bos = line->layout->bos;

    int total_remaining_width, total_space_width = 0;
    int added_so_far, spaces_so_far;
    BOOL is_hinted;
    struct list_head *run_iter;
    enum {
        MEASURE,
        ADJUST
    } mode;

    total_remaining_width = state->remaining_width;
    if (total_remaining_width <= 0)
        return;

    /* hint to full pixel if total remaining width was so */
    is_hinted = (total_remaining_width & 1) == 0;

    for (mode = MEASURE; mode <= ADJUST; mode++) {
        added_so_far = 0;
        spaces_so_far = 0;

        list_for_each(run_iter, &line->gruns) {
            GLYPHRUN *run = (GLYPHRUN *)run_iter;
            GLYPHSTRING *glyphs = run->gs;
            GlyphItemIter cluster_iter;
            BOOL have_cluster;
            int offset;

            assert(run->trun->si >= state->line_start_index);
            offset = state->line_start_index + run->trun->si;

            for (have_cluster = __mg_glyph_item_iter_init_start (&cluster_iter,
                        run, text);
                    have_cluster;
                    have_cluster = __mg_glyph_item_iter_next_cluster (&cluster_iter))
            {
                int i;
                int dir;

                if (!(bos[offset + cluster_iter.start_char] &
                        BOV_EXPANDABLE_SPACE))
                    continue;

                dir = (cluster_iter.start_glyph < cluster_iter.end_glyph) ? 1 : -1;
                for (i = cluster_iter.start_glyph; i != cluster_iter.end_glyph; i += dir)
                {
                    int glyph_width = glyphs->glyphs[i].width;

                    if (glyph_width == 0)
                        continue;

                    spaces_so_far += glyph_width;

                    if (mode == ADJUST)
                    {
                        int adjustment;

                        adjustment = ((Uint64)spaces_so_far *
                            total_remaining_width) /
                            total_space_width - added_so_far;

                        if (is_hinted)
                            adjustment = adjustment + 1;

                        glyphs->glyphs[i].width += adjustment;
                        added_so_far += adjustment;
                    }
                }
            }
        }

        if (mode == MEASURE)
        {
            total_space_width = spaces_so_far;

            if (total_space_width == 0)
            {
                justify_clusters (line, state);
                return;
            }
        }
    }

    state->remaining_width -= added_so_far;
}

static int layout_line_get_width (LAYOUTLINE *line)
{
    int width = 0;
    struct list_head* i;

    list_for_each(i, &line->gruns) {
        GLYPHRUN *run = (GLYPHRUN*)i;
        width += __mg_glyph_string_get_width (run->gs);
    }

    return width;
}

static void layout_line_postprocess (LAYOUTLINE *line,
        LayoutState *state, BOOL wrapped)
{
    BOOL ellipsized = FALSE;

    /* Truncate the logical-final whitespace in the line
     * if we broke the line at it */
    if (wrapped)
        /* The runs are in reverse order at this point,
         * since we prepended them to the list.
         * So, the first run is the last logical run. */
        zero_line_final_space (line, state, (GLYPHRUN*)&line->gruns.next);

    /*
     * Reverse the runs
     */
    //line->runs = g_slist_reverse (line->runs);

    /* Ellipsize the line if necessary
     */
    if (state->line_width >= 0 &&
            should_ellipsize_current_line (line->layout, state)) {
        ellipsized = __mg_layout_line_ellipsize(line, state->line_width);
    }

    /* Now convert logical to visual order
     */
    layout_line_reorder (line);

    /* Fixup letter spacing between runs */
    adjust_line_letter_spacing (line, state);

    /*
     * Distribute extra space between words if justifying and line was wrapped
     */
    if ((line->layout->rf & GRF_ALIGN_MASK) == GRF_ALIGN_JUSTIFY &&
            (wrapped || ellipsized)) {
        /* if we ellipsized, we don't have remaining_width set */
        if (state->remaining_width < 0)
            state->remaining_width = state->line_width - layout_line_get_width (line);

        justify_words (line, state);
    }

    line->layout->is_wrapped |= wrapped;
    line->layout->is_ellipsized |= ellipsized;
}

static void add_line (LAYOUTLINE *line, LayoutState  *state)
{
#if 0
    LAYOUTINFO *layout = line->layout;

    list_add_tail(&line->list, &layout->lines);
    layout->nr_lines++;

    if (layout->height >= 0) {
        PangoRectangle logical_rect;
        pango_layout_line_get_extents (line, NULL, &logical_rect);
        state->remaining_height -= logical_rect.height;
        state->remaining_height -= layout->spacing;
        state->line_height = logical_rect.height;
    }
#else
    // do nothing
#endif
}

static LAYOUTLINE* check_next_line(LAYOUTINFO* layout, LayoutState* state)
{
    LAYOUTLINE *line;

    BOOL have_break = FALSE;    // If we've seen a possible break yet
    int break_remaining_width = 0;// Remaining width before adding run with break
    int break_start_offset = 0; // Start offset before adding run with break
    struct list_head *break_link = NULL;  // Link holding run before break
    BOOL wrapped = FALSE;       // If we had to wrap the line

    line = layout_line_new (layout);
    line->si = state->line_start_index;
    line->is_paragraph_start = state->line_of_par == 1;
    line_set_resolved_dir(line, state->base_dir);

    if (should_ellipsize_current_line (layout, state))
        state->remaining_width = -1;
    else
        state->remaining_width = state->line_width;

    while (state->item) {
        const TEXTRUN *item = state->item;
        BreakResult result;
        int old_num_chars;
        int old_remaining_width;
        BOOL first_item_in_line;

        old_num_chars = item->len;
        old_remaining_width = state->remaining_width;
        first_item_in_line = !list_empty(&line->gruns);

        result = process_text_run(layout, line, state, !have_break, FALSE);
        switch (result) {
        case BREAK_ALL_FIT:
            if (can_break_in (layout, state->start_offset, old_num_chars, first_item_in_line))
            {
                have_break = TRUE;
                break_remaining_width = old_remaining_width;
                break_start_offset = state->start_offset;
                break_link = line->gruns.next;
            }

            // FIXME
            // state->items = g_list_delete_link (state->items, state->items);
            state->start_offset += old_num_chars;

            break;

        case BREAK_EMPTY_FIT:
            wrapped = TRUE;
            goto done;

        case BREAK_SOME_FIT:
            state->start_offset += old_num_chars - item->len;
            wrapped = TRUE;
            goto done;

        case BREAK_NONE_FIT:
            /* Back up over unused runs to run where there is a break */
            while (!list_empty(&line->gruns) && line->gruns.next != break_link)
            {
                state->item = uninsert_run (line);
            }

            state->start_offset = break_start_offset;
            state->remaining_width = break_remaining_width;

            /* Reshape run to break */
            item = state->item;

            old_num_chars = item->len;
            result = process_text_run (layout, line, state, TRUE, TRUE);
            assert(result == BREAK_SOME_FIT || result == BREAK_EMPTY_FIT);

            state->start_offset += old_num_chars - item->len;

            wrapped = TRUE;
            goto done;

        case BREAK_LINE_SEPARATOR:
            // FIXME:
            // state->items = g_list_delete_link (state->items, state->items);

            state->start_offset += old_num_chars;
            /* A line-separate is just a forced break.  Set wrapped, so we do
             * justification */
            wrapped = TRUE;
            goto done;
        }
    }

done:
    layout_line_postprocess (line, state, wrapped);
    add_line (line, state);
    state->line_of_par++;
    state->line_start_index += line->len;
    return line;
}

static int traverse_line_glyphs(LAYOUTLINE* line, int x, int y,
        CB_GLYPH_LAID_OUT cb_laid_out, GHANDLE ctxt)
{
    return 0;
}

LAYOUTLINE* GUIAPI LayoutNextLine(
        LAYOUTINFO* layout, LAYOUTLINE* prev_line,
        int x, int y, int max_extent, BOOL last_line, SIZE* line_size,
        CB_GLYPH_LAID_OUT cb_laid_out, GHANDLE ctxt)
{
    LAYOUTLINE* next_line = NULL;
    LayoutState state;

    if (layout->persist && layout->nr_left_ucs == 0) {
        // already laid out

        if (prev_line && &prev_line->list != &layout->lines) {
            next_line = (LAYOUTLINE*)prev_line->list.next;
            goto out;
        }

        return NULL;
    }

    // init state here

    state.last_line = last_line ? 1 : 0;
    state.shape_set = 0;
    //state.shape_ink_rect;
    //state.shape_logical_rect;

    state.base_dir = layout->truninfo->run_dir;
    state.line_of_par = 1;
    state.glyphs = NULL;
    state.log_widths = NULL;
    state.log_widths_offset = 0;

    if (prev_line == NULL) {
        state.line_start_index = 0;
        if (list_empty(&layout->truninfo->truns)) {
            next_line = layout_line_new(layout);
            next_line->si = state.line_start_index;
            next_line->is_paragraph_start = TRUE;
            line_set_resolved_dir(next_line, layout->truninfo->run_dir);
            goto next_line;
        }

        state.item = (TEXTRUN*)&layout->truninfo->truns.next;
        state.start_index_in_item = 0;
        state.left_ucs_in_item = state.item->len;
    }
    else {
        state.start_offset = layout->truninfo->nr_ucs - layout->nr_left_ucs;
        state.item = __mg_textruns_get_by_offset(layout->truninfo,
                state.start_offset, &state.start_index_in_item);
        if (state.item == NULL) {
            return NULL;
        }
        state.left_ucs_in_item = state.item->len - state.start_index_in_item;
    }

    state.line_width = max_extent;
    state.remaining_width = max_extent;

    next_line = check_next_line (layout, &state);

next_line:
    if (next_line) {
        if (layout->persist) {
            list_add_tail(&next_line->list, &layout->lines);
        }
        else {
            release_line(prev_line);
        }

        layout->nr_lines ++;
        layout->nr_left_ucs -= next_line->len;
    }

out:
    if (next_line && cb_laid_out) {
        traverse_line_glyphs(next_line, x, y, cb_laid_out, ctxt);
    }

    return next_line;
}

#endif /*  _MGCHARSET_UNICODE */

