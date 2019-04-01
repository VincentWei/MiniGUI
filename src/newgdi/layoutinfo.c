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
**
** This implementation is derived from LGPL'd Pango. However, we optimize
** and simplify the original implementation in the following respects:
**
**  - We split the layout process into two stages. We get the text runs
**      (Pango items) in the first stage, and the text runs will keep as
**      constants for subsequent different layouts. In the seconde stage,
**      we create a layout context for a set of specific layout parameters,
**      and generates the lines one by one for the caller. This is useful
**      for an app like browser, it can reuse the text runs if the output
**      width or height changed, and it is no need to re-generate the text
**      runs because of the size change of the output rectangle.
**
**  - We use MiniGUI's fontname for the font attributes of text, and leave
**      the font selection and the glyph generating to MiniGUI's LOGFONT.
**      In this way, we simplify the layout process greatly.
**
**  - We always use Uchar32 string for the whole layout process. So the
**      code and the structures are clearer than original implementation.
**
**  - We provide two shaping engines for rendering the text. One is a
**      basic shaping engine and other is the complex shaping engine based
**      on HarzBuff. The former can be used for some simple applications.
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
#include "glyph.h"

LAYOUTINFO* GUIAPI CreateLayoutInfo(
        const TEXTRUNSINFO* truninfo, Uint32 render_flags,
        const BreakOppo* break_oppos, BOOL persist_lines,
        int max_line_extent, int indent,
        int letter_spacing, int word_spacing, int tab_size,
        int* tabs, int nr_tabs)
{
    LAYOUTINFO* layout;

    if (truninfo == NULL || truninfo->sei.inst == NULL) {
        return NULL;
    }

    layout = (LAYOUTINFO*)calloc(1, sizeof(LAYOUTINFO));
    if (layout == NULL) {
        return NULL;
    }

    layout->truninfo    = truninfo;
    layout->bos         = break_oppos;
    layout->rf          = render_flags;
    layout->ls          = letter_spacing;
    layout->ws          = word_spacing;
    layout->ts          = tab_size;
    layout->max_ext     = max_line_extent;
    layout->indent      = indent;
    layout->tabs        = tabs;
    layout->nr_tabs     = nr_tabs;

    layout->lf_upright  = __mg_create_logfont_for_layout(layout,
            NULL, GLYPH_ORIENT_UPRIGHT);
    if (layout->lf_upright == NULL) {
        free(layout);
        return NULL;
    }

    INIT_LIST_HEAD(&layout->lines);
    layout->nr_left_ucs = truninfo->nr_ucs;

    switch (render_flags & GRF_WRITING_MODE_MASK) {
    default:
    case GRF_WRITING_MODE_HORIZONTAL_TB:
        layout->grv_base = LAYOUT_GRAVITY_SOUTH;
        break;
    case GRF_WRITING_MODE_HORIZONTAL_BT:
        layout->grv_base = LAYOUT_GRAVITY_NORTH;
        break;
    case GRF_WRITING_MODE_VERTICAL_RL:
        layout->grv_base = LAYOUT_GRAVITY_WEST;
        break;
    case GRF_WRITING_MODE_VERTICAL_LR:
        layout->grv_base = LAYOUT_GRAVITY_EAST;
        break;
    }

    switch (render_flags & GRF_TEXT_ORIENTATION_MASK) {
    case GRF_TEXT_ORIENTATION_AUTO:
        layout->grv_plc = LAYOUT_GRAVITY_POLICY_NATURAL;
        layout->orient = GLYPH_ORIENT_UPRIGHT;
        break;
    case GRF_TEXT_ORIENTATION_MIXED:
        layout->grv_plc = LAYOUT_GRAVITY_POLICY_LINE;
        layout->orient = GLYPH_ORIENT_UPRIGHT;
        break;
    case GRF_TEXT_ORIENTATION_UPRIGHT:
        layout->grv_plc = LAYOUT_GRAVITY_POLICY_STRONG;
        layout->orient = GLYPH_ORIENT_UPRIGHT;
        break;
    case GRF_TEXT_ORIENTATION_SIDEWAYS:
        layout->grv_plc = LAYOUT_GRAVITY_POLICY_STRONG;
        layout->orient = GLYPH_ORIENT_SIDEWAYS;
        break;
    case GRF_TEXT_ORIENTATION_UPSIDE_DOWN:
        layout->grv_plc = LAYOUT_GRAVITY_POLICY_STRONG;
        layout->orient = GLYPH_ORIENT_UPSIDE_DOWN;
        break;
    case GRF_TEXT_ORIENTATION_SIDEWAYS_LEFT:
        layout->grv_plc = LAYOUT_GRAVITY_POLICY_STRONG;
        layout->orient = GLYPH_ORIENT_SIDEWAYS_LEFT;
        break;
    }

    layout->persist = persist_lines ? 1 : 0;

    return layout;
}

static void release_line(LAYOUTLINE* line)
{
    while (!list_empty(&line->gruns)) {
        GlyphRun* run = (GlyphRun*)line->gruns.prev;
        list_del(line->gruns.prev);
        __mg_glyph_run_free(run);
    }

    if (line->log_widths) {
        free(line->log_widths);
    }

    free(line);
}

BOOL GUIAPI DestroyLayoutInfo(LAYOUTINFO* layout)
{
    if (layout->lf_upright) {
        FONT_RES* font_res = (FONT_RES*)layout->lf_upright;
        if (font_res->key)
            ReleaseRes(font_res->key);
    }

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

/* Local array size, used for stack-based local arrays */
#if SIZEOF_PTR == 8
#   define LOCAL_ARRAY_SIZE 256
#else
#   define LOCAL_ARRAY_SIZE 128
#endif

struct _LayoutState {
    /* maintained per layout */
    // is last line
    Uint32 last_line:1;
    Uint32 shape_set:1;

    RECT shape_ink_rect, shape_logical_rect;

    /* maintained per paragraph */
    // Current resolved base direction
    ParagraphDir base_dir;
    // Line of the paragraph, starting at 1 for first line
    int line_of_par;
    // Glyphs for the current glyph run
    GlyphString* glyphs;
    // Logical widths for the current text run */
    int *log_widths;
    // Offset into log_widths to the point corresponding
    // to the remaining portion of the first lrun
    int log_widths_offset;

    // Local array for logical widths
    int local_log_widths[LOCAL_ARRAY_SIZE];

    // Character offset of first lrun in state->lrun in layout->truninfo->ucs
    int start_offset;

    /* maintained per line */
    // Start index of line in layout->text
    int line_start_index;
    // Current text run
    const TextRun* trun;
    // Current layout run
    LayoutRun* lrun;
    // Start index of layout run in current text run */
    int start_index_in_trun;

    // Goal width of line currently processing; < 0 is infinite
    int line_width;
    // Amount of space remaining on line; < 0 is infinite
    int remaining_width;
};

static inline void state_log_widths_new(LayoutState* state, int n)
{
    if (n <= LOCAL_ARRAY_SIZE) {
        state->log_widths = state->local_log_widths;
        return;
    }

    state->log_widths = malloc(sizeof(int) * n);
}

static inline void state_log_widths_free(LayoutState* state)
{
    if (state->log_widths && state->log_widths != state->local_log_widths)
        free(state->log_widths);

    state->log_widths = NULL;
}

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

int __mg_shape_layout_run(const TEXTRUNSINFO* info, const LayoutRun* run,
        GlyphString* glyphs)
{
    if (!info->sei.shape(info->sei.inst, info, run, glyphs))
        return 0;

    return glyphs->nr_glyphs;
}

static void ensure_tab_width(LAYOUTINFO *layout)
{
    if (layout->ts == -1) {
        /* Find out how wide 8 spaces are in the context's default
         * font.
         */

        // TODO

        /* We need to make sure the ts is > 0 so finding tab positions
         * terminates. This check should be necessary only under extreme
         * problems with the font.
         */
        if (layout->ts <= 0)
            layout->ts = 50; /* pretty much arbitrary */
    }
}

/* For now we only need the tab position, we assume
 * all tabs are left-aligned.
 */
static int get_tab_pos(LAYOUTINFO *layout, int index, BOOL *is_default)
{
    int n_tabs;

    if (layout->tabs) {
        n_tabs = layout->nr_tabs;
        if (is_default)
            *is_default = FALSE;
    }
    else {
        n_tabs = 0;
        if (is_default)
            *is_default = TRUE;
    }

    if (index < n_tabs) {
        return layout->tabs[index];
    }

    if (n_tabs > 0) {
        /* Extrapolate tab position, repeating the last tab gap to
         * infinity.
         */
        int last_pos = 0;
        int next_to_last_pos = 0;
        int ts;

        last_pos = layout->tabs[n_tabs - 1];

        if (n_tabs > 1)
            next_to_last_pos = layout->tabs[n_tabs - 2];
        else
            next_to_last_pos = 0;

        if (last_pos > next_to_last_pos) {
            ts = last_pos - next_to_last_pos;
        }
        else {
            ts = layout->ts;
        }

        return last_pos + ts * (index - n_tabs + 1);
    }
    else {
        /* No tab array set, so use default tab width
         */
        return layout->ts * index;
    }
}

static int get_line_width(const LAYOUTLINE *line)
{
    struct list_head *l;
    int i;
    int width = 0;

    /* Compute the width of the line currently - inefficient, but easier
     * than keeping the current width of the line up to date everywhere
     */
    list_for_each(l, &line->gruns) {
        GlyphRun *run = (GlyphRun*)l;

        for (i = 0; i < run->gstr->nr_glyphs; i++)
            width += run->gstr->glyphs[i].width;
    }

    return width;
}

static int get_line_height(const LAYOUTLINE *line)
{
    struct list_head *l;
    int i;
    int height = 0;

    list_for_each(l, &line->gruns) {
        GlyphRun *run = (GlyphRun*)l;

        for (i = 0; i < run->gstr->nr_glyphs; i++)
            if (run->gstr->glyphs[i].height > height)
                height = run->gstr->glyphs[i].height;
    }

    return height;
}

static void shape_tab(LAYOUTLINE *line, GlyphString *glyphs)
{
    int i, space_width;

    int current_width = get_line_width(line);

    __mg_glyph_string_set_size (glyphs, 1);

    glyphs->glyphs[0].gv = INV_GLYPH_VALUE;
    glyphs->glyphs[0].x_off = 0;
    glyphs->glyphs[0].y_off = 0;
    glyphs->glyphs[0].is_cluster_start = 1;

    glyphs->log_clusters[0] = 0;

    ensure_tab_width (line->layout);
    space_width = line->layout->ts / 8;

    for (i=0; ; i++) {
        BOOL is_default;
        int tab_pos = get_tab_pos (line->layout, i, &is_default);
        /* Make sure there is at least a space-width of space between
         * tab-aligned text and the text before it.  However, only do
         * this if no tab array is set on the layout, ie. using default
         * tab positions.  If use has set tab positions, respect it to
         * the pixel.
         */
        if (tab_pos >= current_width + (is_default ? space_width : 1)) {
            glyphs->glyphs[0].width = tab_pos - current_width;
            break;
        }
    }
}

static void shape_space(const LAYOUTINFO* layout, const LayoutRun* lrun,
        GlyphString* gstr)
{
    unsigned int i;

    __mg_glyph_string_set_size (gstr, lrun->len);

    for (i = 0; i < lrun->len; i++) {
        UCharGeneralCategory gc;

        gc = UCharGetCategory(lrun->ucs[i]);

        gstr->glyphs[i].gv = INV_GLYPH_VALUE;
        gstr->glyphs[i].x_off = 0;
        gstr->glyphs[i].y_off = 0;

        if (gc == UCHAR_CATEGORY_SPACE_SEPARATOR) {
            Glyph32 space_gv = GetGlyphValue(lrun->lf, UCHAR_SPACE);
            gstr->glyphs[i].width
                        = _font_get_glyph_log_width(lrun->lf, space_gv);

            if (IsUCharWide(lrun->ucs[i])) {
                gstr->glyphs[i].width *= 2;
            }
        }
        else {
            gstr->glyphs[i].width = 0;
        }

        gstr->glyphs[i].is_cluster_start = 1;
        gstr->log_clusters[i] = i;

        if (BIDI_LEVEL_IS_RTL(lrun->el) && gstr->nr_glyphs > 1) {
            __mg_reverse_shaped_glyphs(gstr->glyphs, gstr->nr_glyphs);
            __mg_reverse_log_clusters(gstr->log_clusters, gstr->nr_glyphs);
        }
    }
}

static void shape_shape(const Uchar32* ucs, int nr_ucs,
        RECT* ink_rc, RECT* log_rc, GlyphString *glyphs)
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

static void shape_full(const Uchar32* lrun_ucs, int nr_lrun_ucs,
        const Uchar32* para_ucs, int nr_para_ucs,
        const TEXTRUNSINFO* info, const LayoutRun* lrun,
        GlyphString* glyphs)
{
    // TODO
    info->sei.shape(info->sei.inst, info, lrun, glyphs);
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

static GlyphString* shape_run(LAYOUTLINE *line, LayoutState *state,
        LayoutRun *lrun)
{
    LAYOUTINFO *layout = line->layout;
    GlyphString *glyphs = __mg_glyph_string_new ();

    if (layout->truninfo->ucs[lrun->si] == UCHAR_TAB) {
        shape_tab(line, glyphs);
    }
    else if (lrun->flags & LAYOUTRUN_FLAG_NO_SHAPING) {
        // no need shaping
        shape_space(layout, lrun, glyphs);
    }
    else {
        if (state->shape_set)
            shape_shape(layout->truninfo->ucs + lrun->si, lrun->len,
                    &state->shape_ink_rect,
                    &state->shape_logical_rect, glyphs);
        else
            shape_full(layout->truninfo->ucs + lrun->si, lrun->len,
                    layout->truninfo->ucs, layout->truninfo->nr_ucs,
                    layout->truninfo, lrun, glyphs);

        if (layout->ls) {
            GlyphRun glyph_run;
            int space_left, space_right;

            glyph_run.lrun = lrun;
            glyph_run.gstr = glyphs;

            __mg_glyph_run_letter_space (&glyph_run,
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

static void free_glyph_run (GlyphRun *grun)
{
    if (grun->lrun) {
        __mg_layout_run_free(grun->lrun);
        grun->lrun = NULL;
    }

    __mg_glyph_string_free(grun->gstr);
    free(grun);
}

#ifdef _DEBUG
static inline void print_text_runs(const TEXTRUNSINFO* info, const char* func)
{
    int j = 0;
    struct list_head* i;

    _DBG_PRINTF("Text runs in content when calling %s (nr_runs: %d):\n",
            func, info->nr_runs);

    list_for_each(i, &info->truns) {
        TextRun* run = (TextRun*)i;
        _DBG_PRINTF("RUN NO.:               %d\n", j);
        _DBG_PRINTF("   ADDRESS:        %p\n", run);
        _DBG_PRINTF("   INDEX:          %d\n", run->si);
        _DBG_PRINTF("   LENGHT:         %d\n", run->len);
        _DBG_PRINTF("   EMBEDDING LEVEL:%d\n", run->el);
        _DBG_PRINTF("   NO SHAPING     :%s\n",
            (run->flags & TEXTRUN_FLAG_NO_SHAPING) ? "YES" : "NO");
        j++;
    }
}

static inline void print_line_runs(const LAYOUTLINE* line, const char* func)
{
    int j = 0;
    struct list_head* i;

    _DBG_PRINTF("Runs in line when calling %s (line length: %d, runs: %d):\n",
            func, line->len, line->nr_runs);

    list_for_each(i, &line->gruns) {
        GlyphRun* run = (GlyphRun*)i;
        _DBG_PRINTF("RUN NO.:               %d\n", j);
        _DBG_PRINTF("   ADDRESS:        %p\n", run);
        _DBG_PRINTF("   INDEX:          %d\n", run->lrun->si);
        _DBG_PRINTF("   LENGHT:         %d\n", run->lrun->len);
        _DBG_PRINTF("   EMBEDDING LEVEL:%d\n", run->lrun->el);
        _DBG_PRINTF("   NO SHAPING     :%s\n",
            (run->lrun->noshape) ? "YES" : "NO");
        _DBG_PRINTF("   NR GLYPHS:      %d\n", run->gstr->nr_glyphs);
        j++;
    }
}

static inline void print_run(const GlyphRun* run, const char* func)
{
    _DBG_PRINTF("Run in %s:\n", func);
    _DBG_PRINTF("   ADDRESS:        %p\n", run);
    _DBG_PRINTF("   INDEX:          %d\n", run->lrun->si);
    _DBG_PRINTF("   LENGHT:         %d\n", run->lrun->len);
    _DBG_PRINTF("   EMBEDDING LEVEL:%d\n", run->lrun->el);
    _DBG_PRINTF("   NR GLYPHS:      %d\n", run->gstr->nr_glyphs);
}

static inline void list_print(struct list_head* head, const char* desc)
{
    int i = 0;
    struct list_head* e;

    printf ("entries in list (%p, next: %p, prev: %p) %s\n",
            head, head->next, head->prev, desc);
    list_for_each(e, head) {
        printf ("%d: %p\n", i, e);
        i++;
    }
}

#else

static inline void print_text_runs(const TEXTRUNSINFO* info, const char* func)
{
    // do nothing.
}

static inline void print_line_runs(const LAYOUTLINE* line, const char* func)
{
    // do nothing.
}

static inline void print_run(const GlyphRun* run, const char* func)
{
    // do nothing.
}

static inline void list_print(struct list_head* head, const char* desc)
{
    // do nothing.
}

#endif

static void uninsert_run(LAYOUTLINE *line)
{
    GlyphRun *grun;

    grun = (GlyphRun*)line->gruns.prev;

    list_del(line->gruns.prev);
    line->len -= grun->lrun->len;
    line->nr_runs--;
    free_glyph_run(grun);

    print_line_runs(line, __FUNCTION__);
}

static GlyphRun* insert_run(LAYOUTLINE *line, LayoutState *state,
        LayoutRun *layout_run, BOOL last_run)
{
    GlyphRun *glyph_run = malloc(sizeof(GlyphRun));

    glyph_run->lrun = layout_run;

    if (last_run && state->log_widths_offset == 0)
        glyph_run->gstr = state->glyphs;
    else
        glyph_run->gstr = shape_run(line, state, layout_run);

    if (last_run) {
        if (state->log_widths_offset > 0)
            __mg_glyph_string_free(state->glyphs);
        state->glyphs = NULL;
        state_log_widths_free(state);
    }

    list_add_tail(&glyph_run->list, &line->gruns);
    line->len += layout_run->len;
    line->nr_runs++;

    print_line_runs(line, __FUNCTION__);

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
 * Tries to insert as much as possible of the current layout run onto @line.
 * Five results are possible:
 *
 *  %BREAK_NONE_FIT: Couldn't fit anything.
 *  %BREAK_SOME_FIT: The lrun was broken in the middle.
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
BreakResult process_layout_run(LAYOUTINFO *layout,
        LAYOUTLINE *line, LayoutState *state,
        BOOL force_fit, BOOL no_break_at_end)
{
    LayoutRun *lrun = state->lrun;
    BOOL shape_set = FALSE;
    int width;
    int i;
    BOOL processing_new_lrun = FALSE;

    if (!state->glyphs) {
        state->glyphs = shape_run (line, state, lrun);

        state->log_widths = NULL;
        state->log_widths_offset = 0;

        processing_new_lrun = TRUE;
    }

    if (!layout->single_paragraph &&
            layout->truninfo->ucs[lrun->si] == UCHAR_LINE_SEPARATOR &&
            !should_ellipsize_current_line (layout, state)) {
        insert_run(line, state, lrun, TRUE);
        state->log_widths_offset += lrun->len;
        return BREAK_LINE_SEPARATOR;
    }

    if (state->remaining_width < 0 && !no_break_at_end) {
        /* Wrapping off */
        insert_run (line, state, lrun, TRUE);
        return BREAK_ALL_FIT;
    }

    width = 0;
    if (processing_new_lrun) {
        width = __mg_glyph_string_get_width (state->glyphs);
    }
    else {
        for (i = 0; i < lrun->len; i++)
            width += state->log_widths[state->log_widths_offset + i];
    }

    if ((width <= state->remaining_width ||
                (lrun->len == 1 && list_empty(&line->gruns))) &&
            !no_break_at_end) {
        state->remaining_width -= width;
        state->remaining_width = MAX (state->remaining_width, 0);
        insert_run (line, state, lrun, TRUE);

        return BREAK_ALL_FIT;
    }
    else {
        int num_chars = lrun->len;
        int break_num_chars = num_chars;
        int break_width = width;
        int orig_width = width;
        BOOL retrying_with_char_breaks = FALSE;

        if (processing_new_lrun) {

            GlyphRun glyph_run;
            glyph_run.lrun = lrun;
            glyph_run.gstr = state->glyphs;

            assert(state->log_widths == NULL);
            state_log_widths_new(state, lrun->len);
            __mg_glyph_run_get_logical_widths(&glyph_run, layout->truninfo->ucs,
                state->log_widths);
        }

retry_break:

        /* See how much of the lrun we can stuff in the line. */
        width = 0;
        for (num_chars = 0; num_chars < lrun->len; num_chars++) {
            if (width > state->remaining_width && break_num_chars < lrun->len)
                break;

            /* If there are no previous runs we have to take care to
             * grab at least one char. */
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
        if (break_num_chars > 0 && break_num_chars < lrun->len &&
                layout->bos[state->start_offset + break_num_chars - 1] &
                    BOV_WHITESPACE) {
            break_width -= state->log_widths[state->log_widths_offset +
                                             break_num_chars - 1];
        }

        if ((layout->rf & GRF_OVERFLOW_WRAP_MASK) == GRF_OVERFLOW_WRAP_NORMAL
                && force_fit
                && break_width > state->remaining_width
                && !retrying_with_char_breaks) {
            retrying_with_char_breaks = TRUE;
            num_chars = lrun->len;
            width = orig_width;
            break_num_chars = num_chars;
            break_width = width;
            goto retry_break;
        }

        if (force_fit || break_width <= state->remaining_width) {
            /* Successfully broke the lrun */
            if (state->remaining_width >= 0) {
                state->remaining_width -= break_width;
                state->remaining_width = MAX (state->remaining_width, 0);
            }

            if (break_num_chars == lrun->len) {
                insert_run (line, state, lrun, TRUE);
                return BREAK_ALL_FIT;
            }
            else if (break_num_chars == 0) {
                return BREAK_EMPTY_FIT;
            }
            else {
                LayoutRun* new_lrun;
                GlyphRun *grun;

                new_lrun = __mg_layout_run_split(lrun, break_num_chars);
                state->lrun = new_lrun;
                /* we must free the original layout run */
                __mg_layout_run_free(lrun);

                /* Add the width back, to the line, reshape,
                   subtract the new width */
                state->remaining_width += break_width;
                grun = insert_run(line, state, new_lrun, FALSE);
                break_width = __mg_glyph_string_get_width(grun->gstr);
                state->remaining_width -= break_width;

                state->log_widths_offset += break_num_chars;

                /* Shaped lruns should never be broken */
                assert (!shape_set);

                return BREAK_SOME_FIT;
            }
        }
        else {
            __mg_glyph_string_free(state->glyphs);
            state->glyphs = NULL;
            state_log_widths_free(state);

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
static void line_set_resolved_dir(LAYOUTLINE *line, ParagraphDir direction)
{
    switch (direction) {
    default:
    case BIDI_PGDIR_LTR:
    case BIDI_PGDIR_WLTR:
    case BIDI_PGDIR_ON:
        line->resolved_dir = GLYPH_RUN_DIR_LTR;
        break;
    case BIDI_PGDIR_RTL:
    case BIDI_PGDIR_WRTL:
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
    switch (line->layout->grv_base) {
        default:
        case LAYOUT_GRAVITY_AUTO:
        case LAYOUT_GRAVITY_SOUTH:
            break;
        case LAYOUT_GRAVITY_NORTH:
            line->resolved_dir = GLYPH_RUN_DIR_LTR
                + GLYPH_RUN_DIR_RTL
                - line->resolved_dir;
            break;
        case LAYOUT_GRAVITY_EAST:
            line->resolved_dir = GLYPH_RUN_DIR_LTR;
            break;
        case LAYOUT_GRAVITY_WEST:
            line->resolved_dir = GLYPH_RUN_DIR_RTL;
            break;
    }
}

static void reorder_runs_recurse(struct list_head* result,
        GlyphRun** runs, int n_items)
{
    GlyphRun **tmp, **level_start_node;
    int i, level_start_i;
    int min_level = INT_MAX;

    INIT_LIST_HEAD(result);

    if (n_items == 0)
        return;

    for (i = 0; i < n_items; i++) {
        GlyphRun *run = runs[i];
        min_level = MIN (min_level, run->lrun->el);
    }

    level_start_i = 0;
    level_start_node = runs;
    tmp = runs;
    for (i = 0; i < n_items; i++) {
        GlyphRun* run = runs[i];

        if (run->lrun->el == min_level) {
            if (min_level % 2) {
                if (i > level_start_i) {
                    struct list_head sub_result;
                    reorder_runs_recurse (&sub_result, level_start_node,
                            i - level_start_i);
                    list_concat(&sub_result, result);
                    list_move(result, &sub_result); // result = sub_result;
                }
                list_add(&run->list, result);
            }
            else {
                if (i > level_start_i) {
                    struct list_head sub_result;
                    reorder_runs_recurse (&sub_result, level_start_node,
                            i - level_start_i);
                    list_concat(result, &sub_result);
                }
                list_add_tail(&run->list, result);
            }

            level_start_i = i + 1;
            level_start_node = tmp + 1;
        }

        tmp++;
    }

    if (min_level % 2) {
        if (i > level_start_i) {
            struct list_head sub_result;
            reorder_runs_recurse(&sub_result, level_start_node,
                    i - level_start_i);
            list_concat(&sub_result, result);
            list_move(result, &sub_result); // result = sub_result;
        }
    }
    else {
        if (i > level_start_i) {
            struct list_head sub_result;
            reorder_runs_recurse(&sub_result, level_start_node,
                    i - level_start_i);
            list_concat(result, &sub_result);
        }
    }

    return;
}

static void reverse_runs(struct list_head* result,
        GlyphRun** runs, int n_items)
{
    int i;

    INIT_LIST_HEAD(result);

    for (i = n_items - 1; i >= 0; i--) {
        GlyphRun* run = runs[i];

        list_add_tail(&run->list, result);
    }
}

static inline void list_reverse(struct list_head* head)
{
    struct list_head tmp_head;

    INIT_LIST_HEAD(&tmp_head);

    while (!list_empty(head)) {
        struct list_head* entry = head->prev;
        list_del(head->prev);
        list_add_tail(entry, &tmp_head);
    }

    list_move(head, &tmp_head);
}

static void layout_line_reorder(LAYOUTLINE *line)
{
    struct list_head *i;
    BOOL all_even, all_odd;
    Uint8 level_or = 0, level_and = 1;
    int length = 0;

    GlyphRun** runs = NULL;
    GlyphRun* local_runs[LOCAL_ARRAY_SIZE];

    if (length > LOCAL_ARRAY_SIZE) {
        runs = malloc(sizeof(GlyphRun*) * length);
    }
    else {
        runs = local_runs;
    }

    /* Check if all gruns are in the same direction, in that case, the
     * line does not need modification and we can avoid the expensive
     * reorder runs recurse procedure.
     */

    list_for_each(i, &line->gruns) {
        GlyphRun *grun = (GlyphRun*)i;

        runs[length] = grun;

        level_or |= grun->lrun->el;
        level_and &= grun->lrun->el;
        length++;
    }

    /* If none of the levels had the LSB set, all numbers were even. */
    all_even = (level_or & 0x1) == 0;

    /* If all of the levels had the LSB set, all numbers were odd. */
    all_odd = (level_and & 0x1) == 1;

    if (!all_even && !all_odd) {
        reorder_runs_recurse(&line->gruns, runs, length);
    }
    else if (all_odd) {
        reverse_runs(&line->gruns, runs, length);
    }

    print_line_runs(line, __FUNCTION__);

#if 0 /* test code for list_reverse and reverse_runs */
    list_reverse(&line->gruns);
    print_line_runs(line, "list_reverse");

    length = 0;
    list_for_each(i, &line->gruns) {
        GlyphRun *grun = (GlyphRun*)i;
        runs[length] = grun;
        length++;
    }

    reverse_runs(&line->gruns, runs, length);
    print_line_runs(line, "reverse_runs");
#endif

    if (runs && runs != local_runs)
        free(runs);
}

static void zero_line_final_space (LAYOUTLINE *line,
        LayoutState *state, GlyphRun *run)
{
    LAYOUTINFO *layout = line->layout;
    LayoutRun *lrun = run->lrun;
    GlyphString *glyphs = run->gstr;
    int glyph = lrun->el % 2 ? 0 : glyphs->nr_glyphs - 1;

    /* if the final char of line forms a cluster, and it's
     * a whitespace char, zero its glyph's width as it's been wrapped
     */
    if (glyphs->nr_glyphs < 1 || state->start_offset == 0 ||
            !(layout->bos[state->start_offset - 1] & BOV_WHITESPACE))
        return;

    if (glyphs->nr_glyphs >= 2 &&
            glyphs->log_clusters[glyph] ==
                glyphs->log_clusters[glyph + (lrun->el % 2 ? 1 : -1)])
        return;

    state->remaining_width += glyphs->glyphs[glyph].width;
    glyphs->glyphs[glyph].width = 0;
}

static void pad_glyphstring_right (GlyphString *glyphs,
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

static void pad_glyphstring_left (GlyphString *glyphs,
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

static inline BOOL is_tab_run(LAYOUTINFO *layout, GlyphRun *grun)
{
    return (layout->truninfo->ucs[grun->lrun->si] == '\t');
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
    GlyphRun *last_run;
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
            if (is_tab_run (layout, (GlyphRun*)l)) {
                list_reverse(&line->gruns);
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
        GlyphRun *run = (GlyphRun*)l;
        GlyphRun *next_run;

        if (l->next == &line->gruns) {
            next_run = NULL;
        }
        else {
            next_run = (GlyphRun*)l->next;
        }

        if (is_tab_run (layout, run)) {
            pad_glyphstring_right (run->gstr, state, tab_adjustment);
            tab_adjustment = 0;
        }
        else {
            GlyphRun *visual_next_run = reversed ? last_run : next_run;
            GlyphRun *visual_last_run = reversed ? next_run : last_run;
            int run_spacing = line->layout->ls;
            int space_left, space_right;

            distribute_letter_spacing (run_spacing, &space_left, &space_right);

            if (run->gstr->glyphs[0].width == 0) {
                /* we've zeroed this space glyph at the end of line, now remove
                 * the letter spacing added to its adjacent glyph */
                pad_glyphstring_left (run->gstr, state, -space_left);
            }
            else if (!visual_last_run || is_tab_run(layout, visual_last_run)) {
                pad_glyphstring_left(run->gstr, state, -space_left);
                tab_adjustment += space_left;
            }

            if (run->gstr->glyphs[run->gstr->nr_glyphs - 1].width == 0) {
                /* we've zeroed this space glyph at the end of line, now remove
                 * the letter spacing added to its adjacent glyph */
                pad_glyphstring_right (run->gstr, state, -space_right);
            }
            else if (!visual_next_run || is_tab_run(layout, visual_next_run)) {
                pad_glyphstring_right(run->gstr, state, - space_right);
                tab_adjustment += space_right;
            }
        }

        last_run = run;
    }

    if (reversed)
        list_reverse(&line->gruns);
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
        GlyphString *rightmost_glyphs = NULL;
        int rightmost_space = 0;
        int residual = 0;

        added_so_far = 0;
        gaps_so_far = 0;

        list_for_each(run_iter, &line->gruns) {
            GlyphRun *run = (GlyphRun*)run_iter;
            GlyphString *glyphs = run->gstr;
            GlyphRunIter cluster_iter;
            BOOL have_cluster;
            int dir;
            int offset;

            dir = run->lrun->el % 2 == 0 ? +1 : -1;

            assert(run->lrun->si >= state->line_start_index);
            offset = run->lrun->si;

            if ((have_cluster = (dir > 0)))
                __mg_glyph_run_iter_init_start(&cluster_iter, run, text);
            else
                __mg_glyph_run_iter_init_end(&cluster_iter, run, text);

            for (; have_cluster;
                    have_cluster = dir > 0 ?
                    __mg_glyph_run_iter_next_cluster (&cluster_iter) :
                    __mg_glyph_run_iter_prev_cluster (&cluster_iter)) {
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
            GlyphRun *run = (GlyphRun *)run_iter;
            GlyphString *glyphs = run->gstr;
            GlyphRunIter cluster_iter;
            BOOL have_cluster;
            int offset;

            assert(run->lrun->si >= state->line_start_index);
            offset = run->lrun->si;

            for (have_cluster = __mg_glyph_run_iter_init_start(&cluster_iter,
                        run, text);
                    have_cluster;
                    have_cluster = __mg_glyph_run_iter_next_cluster(
                        &cluster_iter))
            {
                int i;
                int dir;

                if (!(bos[offset + cluster_iter.start_char] &
                        BOV_EXPANDABLE_SPACE))
                    continue;

                dir = (cluster_iter.start_glyph < cluster_iter.end_glyph) ?
                        1 : -1;
                for (i = cluster_iter.start_glyph;
                        i != cluster_iter.end_glyph; i += dir)
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

        if (mode == MEASURE) {
            total_space_width = spaces_so_far;

            if (total_space_width == 0) {
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
        GlyphRun *run = (GlyphRun*)i;
        width += __mg_glyph_string_get_width (run->gstr);
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
        zero_line_final_space (line, state, (GlyphRun*)line->gruns.prev);

    /* Ellipsize the line if necessary
     */
    if (state->line_width >= 0 &&
            should_ellipsize_current_line (line->layout, state)) {
        ellipsized = __mg_layout_line_ellipsize(line, state->line_width);

    }

    _DBG_PRINTF("%s: ellipsized: %s\n",
            __FUNCTION__, ellipsized ? "TRUE" : "FALSE");

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
            state->remaining_width = state->line_width -
                    layout_line_get_width (line);

        justify_words (line, state);
    }

    line->is_wrapped |= wrapped;
    line->is_ellipsized |= ellipsized;
}

static LAYOUTLINE* check_next_line(LAYOUTINFO* layout, LayoutState* state)
{
    LAYOUTLINE *line;

    BOOL have_break = FALSE;    // If we've seen a possible break yet
    int break_remaining_width = 0;// Left width before adding run with break
    int break_start_offset = 0; // Start offset before adding run with break
    struct list_head *break_link = NULL;  // Link holding run before break
    BOOL wrapped = FALSE;       // If we had to wrap the line

    line = layout_line_new (layout);
    line->si = state->line_start_index;
    line->is_paragraph_start = state->line_of_par == 1;
    line_set_resolved_dir(line, state->base_dir);

    line->width = -1;
    line->height = -1;

    if (should_ellipsize_current_line (layout, state))
        state->remaining_width = -1;
    else
        state->remaining_width = state->line_width;

    while (state->trun) {
        BreakResult result;
        int old_num_chars;
        int old_remaining_width;
        BOOL first_lrun_in_line;

        if (state->start_index_in_trun > 0) {
            state->lrun = __mg_layout_run_new_from_offset(layout,
                state->trun, state->start_index_in_trun);
        }
        else {
            state->lrun = __mg_layout_run_new_from(layout, state->trun);
        }

        old_num_chars = state->lrun->len;
        old_remaining_width = state->remaining_width;
        first_lrun_in_line = list_empty(&line->gruns);

        result = process_layout_run(layout, line, state, !have_break, FALSE);

        switch (result) {
        case BREAK_ALL_FIT:
            if (can_break_in (layout, state->start_offset,
                    old_num_chars, first_lrun_in_line)) {
                have_break = TRUE;
                break_remaining_width = old_remaining_width;
                break_start_offset = state->start_offset;
                break_link = line->gruns.next;
            }

            state->trun = (const TextRun*)state->trun->list.next;
            state->start_index_in_trun = 0;
            state->start_offset += old_num_chars;
            if (&state->trun->list == &layout->truninfo->truns)
                state->trun = NULL;
            break;

        case BREAK_EMPTY_FIT:
            __mg_layout_run_free(state->lrun);
            state->lrun = NULL;
            wrapped = TRUE;
            goto done;

        case BREAK_SOME_FIT:
            state->start_offset += old_num_chars - state->lrun->len;
            wrapped = TRUE;
            goto done;

        case BREAK_NONE_FIT:
            __mg_layout_run_free(state->lrun);

            /* Back up over unused runs to run where there is a break */
            while (!list_empty(&line->gruns) &&
                    line->gruns.next != break_link) {
                uninsert_run(line);
            }

            state->start_offset = break_start_offset;
            state->remaining_width = break_remaining_width;

            /* determine start text run again */
            state->trun = __mg_text_run_get_by_offset_const(layout->truninfo,
                    state->start_offset, &state->start_index_in_trun);
            if (state->start_index_in_trun > 0) {
                state->lrun = __mg_layout_run_new_from_offset(layout,
                    state->trun, state->start_index_in_trun);
            }
            else {
                state->lrun = __mg_layout_run_new_from(layout, state->trun);
            }

            /* Reshape run to break */
            old_num_chars = state->lrun->len;
            result = process_layout_run(layout, line, state, TRUE, TRUE);

            assert(result == BREAK_SOME_FIT || result == BREAK_EMPTY_FIT);

            state->start_offset += old_num_chars - state->lrun->len;
            wrapped = TRUE;
            goto done;

        case BREAK_LINE_SEPARATOR:
            state->trun = (const TextRun*)state->trun->list.next;
            state->start_index_in_trun = 0;
            state->start_offset += old_num_chars;
            if (&state->trun->list == &layout->truninfo->truns)
                state->trun = NULL;
            /* A line-separate is just a forced break.  Set wrapped, so we do
             * justification */
            wrapped = TRUE;
            goto done;
        }
    }

done:
    layout_line_postprocess(line, state, wrapped);
    state->line_of_par++;
    state->line_start_index += line->len;
    return line;
}

static int traverse_line_glyphs(const LAYOUTINFO* layout,
        const LAYOUTLINE* line,
        CB_GLYPH_LAID_OUT cb_laid_out, GHANDLE ctxt)
{
    int j;
    struct list_head* i;
    int line_adv = 0;
    RENDERDATA extra;

    extra.truninfo  = layout->truninfo;
    extra.layout    = layout;
    extra.line      = line;

    list_for_each(i, &line->gruns) {
        GlyphRun* run = (GlyphRun*)i;
        for (j = 0; j < run->gstr->nr_glyphs; j++) {
            GLYPHPOS pos;
            ShapedGlyph* glyph_info;

            extra.logfont   = run->lrun->lf;
#if 0
            extra.fg_color  = GetTextColorInTextRuns(layout->truninfo,
                                extra.uc_index);
            extra.bg_color  = GetBackgroundColorInTextRuns(layout->truninfo,
                                extra.uc_index);
#endif
            extra.uc        = run->lrun->ucs[run->gstr->log_clusters[j]];
            extra.uc_index  = run->lrun->si + run->gstr->log_clusters[j];

            glyph_info = run->gstr->glyphs + j;

            pos.x = line_adv;
            pos.y = 0;
            pos.x_off = glyph_info->x_off;
            pos.y_off = glyph_info->y_off;
            pos.advance = glyph_info->width;

            pos.suppressed = 0;
            pos.whitespace = 0;
            pos.ellipsis = 0;
            pos.orientation = run->lrun->ort;
            pos.hanged = glyph_info->hanged;

            if (run->lrun->flags & LAYOUTRUN_FLAG_ELLIPSIS) {
                pos.ellipsis = 1;
            }

            if (run->lrun->flags & LAYOUTRUN_FLAG_NO_SHAPING) {
                if (glyph_info->width > 0) {
                    pos.whitespace = 1;
                }
                else {
                    pos.suppressed = 1;
                }
            }

            if (!cb_laid_out(ctxt, glyph_info->gv, &pos, &extra))
                return j;

            line_adv += glyph_info->width;
        }
    }

    return j;
}

LAYOUTLINE* GUIAPI LayoutNextLine(
        LAYOUTINFO* layout, LAYOUTLINE* prev_line,
        int max_extent, BOOL last_line,
        CB_GLYPH_LAID_OUT cb_laid_out, GHANDLE ctxt)
{
    LAYOUTLINE* next_line = NULL;
    LayoutState state;

    if (prev_line && prev_line->list.next &&
            prev_line->list.next != &layout->lines) {
        // must be a line persisted and not the last line.
        next_line = (LAYOUTLINE*)prev_line->list.next;
        goto out;
    }

    if (layout->persist && layout->nr_left_ucs == 0) {
        // already laid out

        if (list_empty(&layout->lines))
            return NULL;

        if (prev_line == NULL) {
            next_line = (LAYOUTLINE*)layout->lines.next;
            goto out;
        }
        else if (prev_line->list.next != &layout->lines) {
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

    state.base_dir = layout->truninfo->base_dir;
    state.line_of_par = 1;
    state.glyphs = NULL;
    state.log_widths = NULL;
    state.log_widths_offset = 0;

    if (prev_line == NULL) {
        if (list_empty(&layout->truninfo->truns)) {
            // empty line
            next_line = NULL;
            goto out;
        }

        state.line_start_index = 0;
        state.start_offset = 0;
        state.start_index_in_trun = 0;
        state.trun = (TextRun*)layout->truninfo->truns.next;
    }
    else {
        state.line_start_index = layout->truninfo->nr_ucs - layout->nr_left_ucs;
        state.start_offset = state.line_start_index;
        state.start_index_in_trun = 0;
        state.trun = __mg_text_run_get_by_offset_const(layout->truninfo,
                state.line_start_index, &state.start_index_in_trun);

        _DBG_PRINTF("%s: line_start_index: %d, start_index_in_trun: %d(%p)\n",
            __FUNCTION__, state.line_start_index, state.start_index_in_trun,
            state.trun);

        if (state.trun == NULL) {
            next_line = NULL;
            goto out;
        }
    }

    if ((layout->rf & GRF_LINE_EXTENT_MASK) == GRF_LINE_EXTENT_FIXED) {
        max_extent = layout->max_ext;

        switch (layout->rf & GRF_INDENT_MASK) {
        case GRF_INDENT_FIRST_LINE:
            if (prev_line == NULL)
                max_extent -= layout->indent;
            break;

        case GRF_INDENT_HANGING:
            if (prev_line)
                max_extent -= layout->indent;
            break;

        default:
            break;
        }
    }

    state.line_width = max_extent;
    state.remaining_width = max_extent;

    next_line = check_next_line(layout, &state);

    if (state.glyphs) {
        __mg_glyph_string_free(state.glyphs);
    }
    if (state.log_widths) {
        state_log_widths_free(&state);
    }

    if (next_line) {
        next_line->max_extent = max_extent;
        next_line->is_last_line = last_line;
        if (prev_line == 0)
            next_line->line_no = 0;
        else
            next_line->line_no = prev_line->line_no + 1;

        if (layout->persist) {
            list_add_tail(&next_line->list, &layout->lines);
        }
        else {
            // list->next == NULL for not persisted.
            next_line->list.next = NULL;
        }

        layout->nr_lines++;
        layout->nr_left_ucs -= next_line->len;
    }

    // Release the previous line after got the next line.
    // This will avoid releasing the LOGFONT objects earlier.
    if (layout->persist == 0 && prev_line) {
        release_line(prev_line);
        prev_line = NULL;
    }

out:
    if (layout->persist == 0 && prev_line) {
        release_line(prev_line);
    }

    if (next_line && cb_laid_out) {
        traverse_line_glyphs(layout, next_line, cb_laid_out, ctxt);
    }

    return next_line;
}

BOOL GUIAPI GetLayoutLineSize(LAYOUTLINE* line,
        SIZE* line_size)
{
    if (line == NULL)
        return FALSE;

    if (line->width >= 0 && line->height >= 0) {
        line_size->cx = line->width;
        line_size->cy = line->height;
    }
    else {
        line->width = line_size->cx = get_line_width(line);
        line->height = line_size->cy = get_line_height(line);
    }

    return TRUE;
}

int GUIAPI CalcLayoutBoundingRect(LAYOUTINFO* layout,
        int max_line_extent, int max_height, int line_height,
        int x, int y, RECT* bounding)
{
    int nr_lines = 0;
    int remaining_height = max_height;
    LAYOUTLINE* line = NULL;
    BOOL last_line = FALSE;
    SIZE line_size;

    if (line_height <= 0) {
        // determine the default line height
        Uint32 writing_mode = layout->rf & GRF_WRITING_MODE_MASK;
        FONTMETRICS font_metrics;

        GetFontMetrics(layout->lf_upright, &font_metrics);

        if (writing_mode == GRF_WRITING_MODE_VERTICAL_LR ||
                writing_mode == GRF_WRITING_MODE_VERTICAL_RL) {
            line_height = font_metrics.max_width;
        }
        else {
            line_height = font_metrics.font_height;
        }
    }

    SetRect(bounding, 0, 0, 0, 0);

    do {
        if (max_height > 0) {
            remaining_height -= line_height;
            if (remaining_height <= 0)
                last_line = TRUE;
        }

        line = LayoutNextLine(layout, line, max_line_extent, last_line,
            NULL, NULL);

        if (line == NULL) {
            break;
        }

        GetLayoutLineSize(line, &line_size);
        if (line_size.cy > line_height)
            line_height = line_size.cy;

        bounding->bottom += line_height;
        if (line_size.cx > bounding->right)
            bounding->right = line_size.cx;
        nr_lines++;

    } while (1);

    // TODO: transform the bouding rectangle according to the wrting mode
    OffsetRect(bounding, x, y);

    return nr_lines;
}

#ifdef _MGDEVEL_MODE
BOOL GUIAPI GetLayoutLineInfo(LAYOUTLINE* line,
        int* line_no, int* max_extent, int* nr_chars, int* nr_glyphs,
        int** log_widths, int* width, int* height,
        BOOL* is_ellipsized, BOOL* is_wrapped)
{
    if (line == NULL)
        return FALSE;

    if (line_no) *line_no = line->line_no;
    if (max_extent) *max_extent = line->max_extent;
    if (nr_chars) *nr_chars = line->len;
    if (log_widths) *log_widths = line->log_widths;

    if (nr_glyphs) {
        struct list_head* i;

        *nr_glyphs = 0;
        list_for_each(i, &line->gruns) {
            GlyphRun* run = (GlyphRun*)i;
            *nr_glyphs += run->gstr->nr_glyphs;
        }
    }

    if (width) *width = line->width;
    if (height) *height = line->height;
    if (is_ellipsized) *is_ellipsized = line->is_ellipsized;
    if (is_wrapped) *is_wrapped = line->is_wrapped;

    return TRUE;
}
#endif

#endif /*  _MGCHARSET_UNICODE */

