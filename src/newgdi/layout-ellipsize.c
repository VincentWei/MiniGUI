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
 *   For more layoutrmation about the commercial license, please refer to
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

/*
** layout-ellipsize.c: Routine to ellipsize layout lines.
**
** Create by WEI Yongming at 2019/03/22
**
** This implementation is derived from LGPL'd Pango:
**
** Copyright (C) 2004 Red Hat Software
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA 02111-1307, USA.
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

typedef struct _EllipsizeState EllipsizeState;
typedef struct _RunInfo        RunInfo;
typedef struct _LineIter       LineIter;

/* Overall, the way we ellipsize is we grow a "gap" out from an original
 * gap center position until:
 *
 *  line_width - gap_width + ellipsize_width <= goal_width
 *
 * Line:  [-------------------------------------------]
 * Runs:  [------)[---------------)[------------------]
 * Gap center:                 *
 * Gap:             [----------------------]
 *
 * The gap center may be at the start or end in which case the gap grows
 * in only one direction.
 *
 * Note the line and last run are logically closed at the end; this allows
 * us to use a gap position at x=line_width and still have it be part of
 * of a run.
 *
 * We grow the gap out one "span" at a time, where a span is simply a
 * consecutive run of clusters that we can't interrupt with an ellipsis.
 *
 * When choosing whether to grow the gap at the start or the end, we
 * calculate the next span to remove in both directions and see which
 * causes the smaller increase in:
 *
 *  MAX (gap_end - gap_center, gap_start - gap_center)
 *
 * All computations are done using logical order; the ellipsization
 * process occurs before the runs are ordered into visual order.
 */

/* Keeps information about a single run */
struct _RunInfo
{
    GlyphRun *run;
    int start_offset;   /* Character offset of run start */
    int width;          /* Width of run */
};

/* Iterator to a position within the ellipsized line */
struct _LineIter
{
    GlyphRunIter run_iter;
    int run_index;
};

/* State of ellipsization process */
struct _EllipsizeState
{
    LAYOUT *layout;     /* Layout being ellipsized */

    RunInfo *run_info;      /* Array of information about each run */
    int nr_runs;

    int total_width;        /* Original width of line in pixels */
    int gap_center;         /* Goal for center of gap */

    LayoutRun *ellipsis_lrun; /* Layout run created to hold ellipsis */
    GlyphRun *ellipsis_grun; /* Glyph run created to hold ellipsis */
    int ellipsis_width;     /* Width of ellipsis, in pixels */
    int ellipsis_is_cjk;    /* Whether the first character in the ellipsized
                             * is wide; this triggers us to try to use a
                             * mid-line ellipsis instead of a baseline
                             */
    const char* fontname;   /* fontname of gap TextRun */

    LineIter gap_start_iter; /* Iteratator pointing to the first cluster in gap */
    int gap_start_x;        /* x position of start of gap, in pixels */

    LineIter gap_end_iter;  /* Iterator pointing to last cluster in gap */
    int gap_end_x;          /* x position of end of gap, in pixels */
};

/* Compute global information needed for the itemization process
 */
static void init_state (EllipsizeState *state, LAYOUTLINE *line)
{
    struct list_head *l;
    int i;
    int start_offset;

    state->layout = line->layout;

    state->nr_runs = line->nr_runs;
    state->run_info = malloc(sizeof(RunInfo) * state->nr_runs);

    start_offset = line->si;

    state->total_width = 0;
    i = 0;
    list_for_each (l, &line->gruns) {
        GlyphRun *run = (GlyphRun*)l;
        int width = __mg_glyph_string_get_width (run->gstr);
        state->run_info[i].run = run;
        state->run_info[i].width = width;
        state->run_info[i].start_offset = start_offset;
        state->total_width += width;

        start_offset += run->lrun->len;
        i++;
    }

    state->ellipsis_lrun = NULL;
    state->ellipsis_grun = NULL;
    state->ellipsis_is_cjk = FALSE;
    state->fontname = "";   // force to reset the fontname from gap
}

/* Cleanup memory allocation
 */
static void free_state (EllipsizeState *state)
{
    free (state->run_info);
}

/* Computes the width of a single cluster
 */
static int get_cluster_width (LineIter *iter)
{
    GlyphRunIter *run_iter = &iter->run_iter;
    GlyphString *glyphs = run_iter->glyph_run->gstr;
    int width = 0;
    int i;

    if (run_iter->start_glyph < run_iter->end_glyph) {
        /* LTR */
        for (i = run_iter->start_glyph; i < run_iter->end_glyph; i++)
            width += glyphs->glyphs[i].width;
    }
    else {
        /* RTL */
        for (i = run_iter->start_glyph; i > run_iter->end_glyph; i--)
            width += glyphs->glyphs[i].width;
    }

    return width;
}

/* Move forward one cluster. Returns %FALSE if we were already at the end
 */
static BOOL line_iter_next_cluster (EllipsizeState *state, LineIter *iter)
{
    if (!__mg_glyph_run_iter_next_cluster (&iter->run_iter)) {
        if (iter->run_index == state->nr_runs - 1)
            return FALSE;
        else {
            iter->run_index++;
            __mg_glyph_run_iter_init_start (&iter->run_iter,
                    state->run_info[iter->run_index].run,
                    state->layout->truns->ucs);
        }
    }

    return TRUE;
}

/* Move backward one cluster. Returns %FALSE if we were already at the end
 */
static BOOL line_iter_prev_cluster (EllipsizeState *state, LineIter *iter)
{
    if (!__mg_glyph_run_iter_prev_cluster (&iter->run_iter)) {
        if (iter->run_index == 0)
            return FALSE;
        else {
            iter->run_index--;
            __mg_glyph_run_iter_init_end (&iter->run_iter,
                    state->run_info[iter->run_index].run,
                    state->layout->truns->ucs);
        }
    }

    return TRUE;
}

/*
 * An ellipsization boundary is defined by two things
 *
 * - Starts a cluster - forced by structure of code
 * - Starts a grapheme - checked here
 *
 * In the future we'd also like to add a check for cursive connectivity here.
 * This should be an addition to #PangoGlyphVisAttr
 *
 */

/* Check if there is a ellipsization boundary before
 * the cluster @iter points to
 */
static BOOL starts_at_ellipsization_boundary (EllipsizeState *state,
        LineIter *iter)
{
    int index;
    RunInfo *run_info = &state->run_info[iter->run_index];

    if (iter->run_iter.start_char == 0 && iter->run_index == 0)
        return TRUE;

    index = run_info->start_offset + iter->run_iter.start_char;
    return state->layout->bos[index] & BOV_GB_CURSOR_POS;
}

/* Check if there is a ellipsization boundary after the cluster @iter points to
 */
static BOOL ends_at_ellipsization_boundary(EllipsizeState *state,
        LineIter *iter)
{
    int index;
    RunInfo *run_info = &state->run_info[iter->run_index];

    if (iter->run_iter.end_char == run_info->run->lrun->len &&
            iter->run_index == state->nr_runs - 1)
        return TRUE;

    index = run_info->start_offset + iter->run_iter.end_char;

    /*
     * FIXME: Pango uses index + 1 here, but will overflow the bos array.
     */
    assert(index < state->layout->truns->nr_ucs);

    return state->layout->bos[index] & BOV_GB_CURSOR_POS;
}

/* Shapes the ellipsis using the font and is_cjk information computed by
 * update_ellipsis_shape() from the first character in the gap.
 */

/* U+22EF: MIDLINE HORIZONTAL ELLIPSIS, used for CJK: "\342\213\257" */
static const Uchar32 _ellipsis_midline[1] = {0x22EF};
/* U+2026: HORIZONTAL ELLIPSIS: "\342\200\246" */
static const Uchar32 _ellipsis_baseline[1] = {0x2026};
static const Uchar32 _ellipsis_fallback[3] = {'.', '.', '.'};

static void shape_ellipsis (EllipsizeState *state)
{
    Glyph32 ellipsis_gv;
    const TextRun* text_run;
    LayoutRun *layout_run;
    GlyphString *glyphs;
    const Uchar32* ellipsis_ucs;
    int i;

    /* Create/reset state->ellipsis_grun
     */
    if (!state->ellipsis_grun) {
        state->ellipsis_grun = mg_slice_new(GlyphRun);
        state->ellipsis_grun->gstr = __mg_glyph_string_new ();
        state->ellipsis_grun->lrun = NULL;
    }

    if (state->ellipsis_lrun) {
        __mg_layout_run_free (state->ellipsis_lrun);
        state->ellipsis_lrun = NULL;
    }

    /* First try using a specific ellipsis character in the best matching font
     */
    if (state->ellipsis_is_cjk) {
        ellipsis_ucs = _ellipsis_midline;
    }
    else {
        ellipsis_ucs = _ellipsis_baseline;
    }

    text_run = __mg_text_run_get_by_offset_const(state->layout->truns,
        state->gap_start_iter.run_iter.start_index, NULL);

    layout_run = __mg_layout_run_new_ellipsis (state->layout, text_run,
            ellipsis_ucs, 1);

    ellipsis_gv = GetGlyphValueAlt(layout_run->lf,
            UCHAR2ACHAR(ellipsis_ucs[0]));
    /* If the devfont of the glyph value of the specific ellipsis character
     * is SBC devfont, we use "...".
     */
    if (DFI_IN_GLYPH(ellipsis_gv) == 0) {
        __mg_layout_run_free (layout_run);
        layout_run = __mg_layout_run_new_ellipsis (state->layout,
                text_run, _ellipsis_fallback, 3);
    }

    state->ellipsis_lrun = layout_run;

    /* Now shape */
    glyphs = state->ellipsis_grun->gstr;
    __mg_shape_layout_run(state->layout->truns, layout_run, glyphs);

    state->ellipsis_width = 0;
    for (i = 0; i < glyphs->nr_glyphs; i++)
        state->ellipsis_width += glyphs->glyphs[i].width;
}

/* Updates the shaping of the ellipsis if necessary when we move the
 * position of the start of the gap.
 *
 * The shaping of the ellipsis is determined by two things:
 *
 * - The font attributes applied to the first character in the gap
 * - Whether the first character in the gap is wide or not. If the
 *   first character is wide, then we assume that we are ellipsizing
 *   East-Asian text, so prefer a mid-line ellipsizes to a baseline
 *   ellipsis, since that's typical practice for Chinese/Japanese/Korean.
 */
static void update_ellipsis_shape (EllipsizeState *state)
{
    BOOL recompute = FALSE;
    Uchar32 start_wc;
    BOOL is_cjk;
    const TextRun* text_run;

    text_run = __mg_text_run_get_by_offset_const(state->layout->truns,
        state->gap_start_iter.run_iter.start_index, NULL);

    if (state->fontname != text_run->fontname) {
        state->fontname = text_run->fontname;
        recompute = TRUE;
    }

    /* Check whether we need to recompute the ellipsis because we switch
     * from CJK to not or vice-versa
     */
    start_wc = state->layout->truns->ucs[state->gap_start_iter.run_iter.start_index];
    is_cjk = IsUCharWide (start_wc);

    if (is_cjk != state->ellipsis_is_cjk) {
        state->ellipsis_is_cjk = is_cjk;
        recompute = TRUE;
    }

    if (recompute)
        shape_ellipsis (state);
}

/* Computes the position of the gap center and finds the smallest span
 * containing it
 */
static void find_initial_span (EllipsizeState *state)
{
    GlyphRun *glyph_run;
    GlyphRunIter *run_iter;
    BOOL have_cluster;
    int i;
    int x;
    int cluster_width;

    switch (state->layout->rf & GRF_OVERFLOW_ELLIPSIZE_MASK) {
    case GRF_OVERFLOW_ELLIPSIZE_NONE:
    default:
        assert(0);
    case GRF_OVERFLOW_ELLIPSIZE_START:
        state->gap_center = 0;
        break;
    case GRF_OVERFLOW_ELLIPSIZE_MIDDLE:
        state->gap_center = state->total_width / 2;
        break;
    case GRF_OVERFLOW_ELLIPSIZE_END:
        state->gap_center = state->total_width;
        break;
    }

    /* Find the run containing the gap center
     */
    x = 0;
    for (i = 0; i < state->nr_runs; i++) {
        if (x + state->run_info[i].width > state->gap_center)
            break;

        x += state->run_info[i].width;
    }

    /* Last run is a closed interval, so back off one run */
    if (i == state->nr_runs) {
        i--;
        x -= state->run_info[i].width;
    }

    /* Find the cluster containing the gap center */
    state->gap_start_iter.run_index = i;
    run_iter = &state->gap_start_iter.run_iter;
    glyph_run = state->run_info[i].run;

    cluster_width = 0; // Quiet GCC, the line must have at least one cluster
    for (have_cluster = __mg_glyph_run_iter_init_start(run_iter, glyph_run,
                state->layout->truns->ucs);
            have_cluster;
            have_cluster = __mg_glyph_run_iter_next_cluster (run_iter))
    {
        cluster_width = get_cluster_width (&state->gap_start_iter);

        if (x + cluster_width > state->gap_center)
            break;

        x += cluster_width;
    }

    /* Last cluster is a closed interval, so back off one cluster */
    if (!have_cluster)
        x -= cluster_width;

    state->gap_end_iter = state->gap_start_iter;
    state->gap_start_x = x;
    state->gap_end_x = x + cluster_width;

    /* Expand the gap to a full span
     */
    while (!starts_at_ellipsization_boundary (state, &state->gap_start_iter)) {
        line_iter_prev_cluster (state, &state->gap_start_iter);
        state->gap_start_x -= get_cluster_width (&state->gap_start_iter);
    }

    while (!ends_at_ellipsization_boundary (state, &state->gap_end_iter)) {
        line_iter_next_cluster (state, &state->gap_end_iter);
        state->gap_end_x += get_cluster_width (&state->gap_end_iter);
    }

    update_ellipsis_shape (state);
}

/* Removes one run from the start or end of the gap. Returns FALSE
 * if there's nothing left to remove in either direction.
 */
static BOOL remove_one_span (EllipsizeState *state)
{
    LineIter new_gap_start_iter;
    LineIter new_gap_end_iter;
    int new_gap_start_x;
    int new_gap_end_x;
    int width;

    /* Find one span backwards and forward from the gap
     */
    new_gap_start_iter = state->gap_start_iter;
    new_gap_start_x = state->gap_start_x;

    do {
        if (!line_iter_prev_cluster (state, &new_gap_start_iter))
            break;
        width = get_cluster_width (&new_gap_start_iter);
        new_gap_start_x -= width;
    }
    while (!starts_at_ellipsization_boundary (state, &new_gap_start_iter) ||
            width == 0);

    new_gap_end_iter = state->gap_end_iter;
    new_gap_end_x = state->gap_end_x;
    do {
        if (!line_iter_next_cluster (state, &new_gap_end_iter))
            break;
        width = get_cluster_width (&new_gap_end_iter);
        new_gap_end_x += width;
    }
    while (!ends_at_ellipsization_boundary (state, &new_gap_end_iter) ||
            width == 0);

    if (state->gap_end_x == new_gap_end_x &&
            state->gap_start_x == new_gap_start_x)
        return FALSE;

    /* In the case where we could remove a span from either end of the
     * gap, we look at which causes the smaller increase in the
     * MAX (gap_end - gap_center, gap_start - gap_center)
     */
    if (state->gap_end_x == new_gap_end_x ||
            (state->gap_start_x != new_gap_start_x &&
             state->gap_center - new_gap_start_x <
                new_gap_end_x - state->gap_center)) {
        state->gap_start_iter = new_gap_start_iter;
        state->gap_start_x = new_gap_start_x;

        update_ellipsis_shape (state);
    }
    else {
        state->gap_end_iter = new_gap_end_iter;
        state->gap_end_x = new_gap_end_x;
    }

    return TRUE;
}

/* Fixes up the properties of the ellipsis run once we've determined the
 * final extents of the gap
 */
static void fixup_ellipsis_grun (EllipsizeState *state)
{
    GlyphString *glyphs = state->ellipsis_grun->gstr;
    LayoutRun *layout_run = state->ellipsis_lrun;
    int level;
    int i;

    /* Make the entire glyphstring into a single logical cluster */
    for (i = 0; i < glyphs->nr_glyphs; i++) {
        glyphs->log_clusters[i] = 0;
        glyphs->glyphs[i].is_cluster_start = FALSE;
    }

    glyphs->glyphs[0].is_cluster_start = TRUE;

    /* Fix up the layout_run to point to the entire elided text */
    layout_run->si = state->gap_start_iter.run_iter.start_index;
    layout_run->len = state->gap_end_iter.run_iter.end_index - layout_run->si;

    /* The level for the layout_run is the minimum level of the elided text */
    level = INT_MAX;
    for (i = state->gap_start_iter.run_index;
            i <= state->gap_end_iter.run_index; i++)
        level = MIN (level, state->run_info[i].run->lrun->el);

    layout_run->el = level;
    state->ellipsis_grun->lrun = layout_run;
}

/* Computes the new list of runs for the line
 */
static void get_run_list (EllipsizeState *state, LAYOUTLINE* line)
{
    GlyphRun *partial_start_run = NULL;
    GlyphRun *partial_end_run = NULL;
    RunInfo *run_info;
    GlyphRunIter *run_iter;
    int i;

    /* We first cut out the pieces of the starting and ending runs we want to
     * preserve; we do the end first in case the end and the start are
     * the same. Doing the start first would disturb the indices for the end.
     */
    run_info = &state->run_info[state->gap_end_iter.run_index];
    run_iter = &state->gap_end_iter.run_iter;
    if (run_iter->end_char != run_info->run->lrun->len) {
        partial_end_run = run_info->run;
        run_info->run = __mg_glyph_run_split (run_info->run,
                run_iter->end_index - run_info->run->lrun->si);
    }

    run_info = &state->run_info[state->gap_start_iter.run_index];
    run_iter = &state->gap_start_iter.run_iter;
    if (run_iter->start_char != 0) {
        partial_start_run = __mg_glyph_run_split (run_info->run,
                run_iter->start_index - run_info->run->lrun->si);
    }

    /* Now assemble the new list of runs
     */
    for (i = 0; i < state->gap_start_iter.run_index; i++) {
        list_add_tail(&state->run_info[i].run->list, &line->gruns);
        line->nr_runs++;
    }

    if (partial_start_run) {
        list_add_tail(&partial_start_run->list, &line->gruns);
        line->nr_runs++;
    }

    list_add_tail(&state->ellipsis_grun->list, &line->gruns);
    line->nr_runs++;

    if (partial_end_run) {
        list_add_tail(&partial_end_run->list, &line->gruns);
        line->nr_runs++;
    }

    for (i = state->gap_end_iter.run_index + 1; i < state->nr_runs; i++) {
        list_add_tail(&state->run_info[i].run->list, &line->gruns);
        line->nr_runs++;
    }

    /* And free the ones we didn't use
     */
    for (i = state->gap_start_iter.run_index;
            i <= state->gap_end_iter.run_index; i++)
        __mg_glyph_run_free(state->run_info[i].run);
}

/* Computes the width of the line as currently ellipsized
 */
static int current_width (EllipsizeState *state)
{
    return state->total_width - (state->gap_end_x - state->gap_start_x) +
            state->ellipsis_width;
}

/**
 * _pango_layout_line_ellipsize:
 * @line: a #LAYOUTLINE
 *
 * Given a #LAYOUTLINE with the runs still in logical order, ellipsize
 * it according the layout's policy to fit within the set width of the layout.
 *
 * Return value: whether the line had to be ellipsized
 **/
BOOL __mg_layout_line_ellipsize (LAYOUTLINE *line, int goal_width)
{
    EllipsizeState state;
    BOOL is_ellipsized = FALSE;

    if ((line->layout->rf & GRF_OVERFLOW_ELLIPSIZE_MASK) ==
            GRF_OVERFLOW_ELLIPSIZE_NONE || goal_width < 0)
        return is_ellipsized;

    init_state (&state, line);

    if (state.total_width <= goal_width)
        goto out;

    find_initial_span (&state);

    while (current_width (&state) > goal_width) {
        if (!remove_one_span (&state))
            break;
    }

    fixup_ellipsis_grun (&state);

    //__mg_layout_line_free_runs(line);
    INIT_LIST_HEAD(&line->gruns);
    line->nr_runs = 0;

    get_run_list(&state, line);
    is_ellipsized = TRUE;

out:
    free_state (&state);
    return is_ellipsized;
}

#endif /*  _MGCHARSET_UNICODE */

