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
** glyphruninfo.c: The implementation of APIs related GLYPHRUNINFO
**
** Create by WEI Yongming at 2019/03/15
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#if defined(_MGCHARSET_UNICODE)

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "unicode-ops.h"
#include "glyphruninfo.h"

/* Local array size, used for stack-based local arrays */
#if SIZEOF_PTR == 8
#   define LOCAL_ARRAY_SIZE 256
#else
#   define LOCAL_ARRAY_SIZE 128
#endif

enum {
    EMBEDDING_CHANGED    = 1 << 0,
    SCRIPT_CHANGED       = 1 << 1,
    LANG_CHANGED         = 1 << 2,
    DERIVED_LANG_CHANGED = 1 << 3,
    FONT_CHANGED         = 1 << 4,
    WIDTH_CHANGED        = 1 << 5,
    EMOJI_CHANGED        = 1 << 6,
};

typedef struct _GLYPHRUNSTATE {
    GLYPHRUNINFO*   runinfo;
    const Uchar32*  text;
    const Uchar32*  end;

    const Uchar32*  run_start;
    const Uchar32*  run_end;

    BidiLevel*      els;
    const Uchar32*  emb_end;
    int             emb_end_offset;
    BidiLevel       emb_level;

    LanguageCode    lang;
    ScriptType      script;

    GlyphOrient     orient;
    GlyphOrientPolicy   orient_policy;
    GlyphOrient     ort_rsv;
    BOOL            centered_baseline;

    Uint8           changed;

    ScriptIterator  script_iter;
    WidthIterator   width_iter;
    EmojiIterator   emoji_iter;

    LanguageCode    derived_lang;
} GLYPHRUNSTATE;

static void update_embedding_end(GLYPHRUNSTATE *state)
{
    state->emb_level = state->els[state->emb_end_offset];
    while (state->emb_end < state->end &&
            state->els[state->emb_end_offset] == state->emb_level)
    {
        state->emb_end_offset++;
        state->emb_end++;
    }

    state->changed |= EMBEDDING_CHANGED;
}

static void update_end (GLYPHRUNSTATE *state)
{
    state->run_end = state->emb_end;
    if (state->script_iter.end < state->run_end)
        state->run_end = state->script_iter.end;
    if (state->width_iter.end < state->run_end)
        state->run_end = state->width_iter.end;
    if (state->emoji_iter.end < state->run_end)
        state->run_end = state->emoji_iter.end;
}

static LanguageCode compute_derived_language (LanguageCode lang,
        ScriptType script)
{
    LanguageCode derived_lang;

    /* Make sure the language tag is consistent with the derived
     * script. There is no point in marking up a section of
     * Arabic text with the "en" language tag.
     */
    if ((lang != LANGCODE_unknown) &&
            __mg_language_includes_script(lang, script))
        derived_lang = lang;
    else {
        derived_lang = GetSampleLanguageForScript(script);
    }

    return derived_lang;
}

static void state_update_for_new_run (GLYPHRUNSTATE *state)
{
    if (state->changed & (SCRIPT_CHANGED | WIDTH_CHANGED)) {
        GlyphOrient orient = state->orient;
        GlyphOrientPolicy orient_policy = state->runinfo->ort_plc;

        if (orient == GLYPH_ORIENT_AUTO)
            orient = state->runinfo->ort_rsv;

        state->ort_rsv = ScriptGetWideGlyphOrientation(
                state->script_iter.script, state->width_iter.upright,
                orient, orient_policy);
    }

    if (state->changed & (SCRIPT_CHANGED | LANG_CHANGED))
    {
        LanguageCode old_derived_lang = state->derived_lang;
        state->derived_lang = compute_derived_language (state->lang,
                state->script);
        if (old_derived_lang != state->derived_lang)
            state->changed |= DERIVED_LANG_CHANGED;
    }

#if 0
    if ((state->changed & DERIVED_LANG_CHANGED) || !state->lang_engine) {
        state->lang_engine = _pango_get_language_engine ();
    }
#endif
}

static void state_process_run (GLYPHRUNSTATE *state)
{
    const Uchar32 *p;
    BOOL last_was_forced_break = FALSE;

    /* Only one character has the category LINE_SEPARATOR in Unicode 12.0;
     * update this if that changes. */
#define LINE_SEPARATOR 0x2028

    state_update_for_new_run (state);

    for (p = state->run_start; p < state->run_end; p++) {
        Uchar32 wc = *p;
        BOOL is_forced_break = (wc == '\t' || wc == LINE_SEPARATOR);
        UCharGeneralCategory type;

        /* We don't want space characters to affect font selection; in general,
         * it's always wrong to select a font just to render a space.
         * We assume that all fonts have the ASCII space, and for other space
         * characters if they don't, HarfBuzz will compatibility-decompose them
         * to ASCII space...
         */
        type = UCharGetCategory (wc);
        if ((type == UCHAR_CATEGORY_CONTROL ||
                type == UCHAR_CATEGORY_FORMAT ||
                type == UCHAR_CATEGORY_SURROGATE ||
                (type == UCHAR_CATEGORY_SPACE_SEPARATOR &&
                    wc != 0x1680u /* OGHAM SPACE MARK */) ||
                (wc >= 0xfe00u && wc <= 0xfe0fu) ||
                (wc >= 0xe0100u && wc <= 0xe01efu))) {
        }
        else {
        }

        is_forced_break = last_was_forced_break; // supress warning
        last_was_forced_break = is_forced_break;
    }

#if 0
    /* Finish the final item from the current segment */
    state->item->length = (p - state->text) - state->item->offset;
    if (!state->item->analysis.shape_engine) {
        PangoEngineShape *shape_engine;
        PangoFont *font;

        if ((!get_shaper_and_font (state, ' ', &shape_engine, &font))) {
            /* If no shaper was found, warn only once per fontmap/script pair */
            PangoFontMap *fontmap = state->context->font_map;
            const char *script_name = string_from_script (get_script (state));

            if (!g_object_get_data (G_OBJECT (fontmap), script_name))
            {
                G_GNUC_BEGIN_IGNORE_DEPRECATIONS
                    g_warning ("failed to choose a font, expect ugly output. engine-type='%s', script='%s'",
                            pango_font_map_get_shape_engine_type (fontmap),
                            script_name);
                G_GNUC_END_IGNORE_DEPRECATIONS

                    g_object_set_data_full (G_OBJECT (fontmap), script_name,
                            GINT_TO_POINTER (1), NULL);
            }

            shape_engine = _pango_get_fallback_shaper ();
            font = NULL;
        }

        itemize_state_fill_shaper (state, shape_engine, font);
    }

    state->item = NULL;
#endif
}

static BOOL state_next (GLYPHRUNSTATE *state)
{
    if (state->run_end == state->end)
        return FALSE;

    state->changed = 0;
    state->run_start = state->run_end;

    if (state->run_end == state->emb_end) {
        update_embedding_end (state);
    }

    if (state->run_end == state->script_iter.end) {
        __mg_script_iterator_next (&state->script_iter);
        state->changed |= SCRIPT_CHANGED;
    }

    if (state->run_end == state->width_iter.end) {
        __mg_width_iterator_next (&state->width_iter);
        state->changed |= WIDTH_CHANGED;
    }

    if (state->run_end == state->emoji_iter.end)
    {
        __mg_emoji_iterator_next (&state->emoji_iter);
        state->changed |= EMOJI_CHANGED;
    }

    update_end (state);
    return TRUE;
}

static inline Uint8 get_glyph_orient_from_logfont (LOGFONT* lf)
{
    return GLYPH_ORIENT_SOUTH;
}

static void update_resolved_orientation (GLYPHRUNINFO *runinfo)
{
    if (runinfo->ort_base == GLYPH_ORIENT_AUTO)
        runinfo->ort_rsv = get_glyph_orient_from_logfont (runinfo->lf);
    else
        runinfo->ort_rsv = runinfo->ort_base;
}

static BOOL create_glyph_runs(GLYPHRUNINFO* runinfo, BidiLevel* els)
{
    BOOL ok = FALSE;
    GLYPHRUNSTATE state;
    Uint8 local_types_buff[LOCAL_ARRAY_SIZE];
    Uint8* types_buff = NULL;

    if (runinfo->nr_ucs <= LOCAL_ARRAY_SIZE) {
        types_buff = local_types_buff;
    }
    else {
        types_buff = malloc(sizeof(Uint8)* runinfo->nr_ucs);
    }

    if (!types_buff)
        goto out;

    state.runinfo = runinfo;
    state.text = runinfo->ucs;
    state.end = runinfo->ucs + runinfo->nr_ucs;
    state.els = els;

    state.changed = 0;

    state.emb_end_offset = 0;
    state.emb_end = runinfo->ucs;
    update_embedding_end(&state);

    __mg_script_iterator_init (&state.script_iter, state.text, runinfo->nr_ucs);
    __mg_width_iterator_init (&state.width_iter, state.text, runinfo->nr_ucs);
    __mg_emoji_iterator_init (&state.emoji_iter, state.text, runinfo->nr_ucs,
        types_buff);

    update_end (&state);

    do {
        state_process_run (&state);
    } while (state_next (&state));

out:
    if (types_buff && types_buff != local_types_buff)
        free (types_buff);

    return ok;
}

GLYPHRUNINFO* GUIAPI CreateGlyphRunInfo(Uchar32* ucs, int nr_ucs,
        LanguageCode lang_code, ScriptType script_type,
        ParagraphDir base_dir, GlyphRunDir run_dir,
        GlyphOrient glyph_orient, GlyphOrientPolicy orient_policy,
        LOGFONT* logfont, RGBCOLOR color)
{
    BOOL ok = FALSE;

    GLYPHRUNINFO* runinfo;
    BidiLevel  local_els[LOCAL_ARRAY_SIZE];
    BidiLevel* els = NULL;

    runinfo = (GLYPHRUNINFO*)calloc(1, sizeof(GLYPHRUNINFO));
    if (ucs == NULL || nr_ucs <= 0 || runinfo == NULL) {
        return NULL;
    }

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        els = local_els;
    else
        els = (BidiLevel*)malloc (nr_ucs * sizeof(BidiLevel));

    if (!els) {
        _DBG_PRINTF("%s: failed to allocate space for embedding levels.\n");
        goto out;
    }

    UBidiGetParagraphEmbeddingLevelsAlt(ucs, nr_ucs, &base_dir, els);

    /* the breaking opportunities should be passed to the layout function.
    // Calculate the breaking opportunities
    if (UStrGetBreaks(script_type, ctr, wbr, lbp,
            ucs, nr_ucs, &bos) == 0) {
        _DBG_PRINTF("%s: failed to get breaking opportunities.\n");
        goto out;
    }
    */

    // Initialize other fields
    runinfo->ucs = ucs;
    runinfo->lf = logfont;
    runinfo->nr_ucs = nr_ucs;
    runinfo->lc = lang_code;
    runinfo->st = script_type;
    runinfo->base_dir = (base_dir == BIDI_PGDIR_LTR) ? 0 : 1;
    runinfo->run_dir = run_dir;
    runinfo->ort_base = glyph_orient;
    runinfo->ort_plc = orient_policy;

    INIT_LIST_HEAD(&runinfo->cm_head.list);
    runinfo->cm_head.si = 0;
    runinfo->cm_head.len = nr_ucs;
    runinfo->cm_head.color = color;

    INIT_LIST_HEAD(&runinfo->run_head);

    update_resolved_orientation(runinfo);

#if 0
    int i, j;
    BidiLevel max_level = 0;
    BidiLevel level_or, level_and;

    // make the embedding levels of the bidi marks to be -1.
    level_or = 0, level_and = 1;
    j = 0;
    for (i = 0; i < nr_ucs; i++) {
        if (BIDI_IS_EXPLICIT_OR_BN (bidi_ts[i])) {
            els[i] = -1;
        }
        else {
            level_or |= els[j];
            level_and &= els[j];
            j++;
        }
    }

    // check for all even or odd
    /* If none of the levels had the LSB set, all chars were even. */
    runinfo->all_even = (level_or & 0x1) == 0;
    /* If all of the levels had the LSB set, all chars were odd. */
    runinfo->all_odd = (level_and & 0x1) == 1;
#endif

    if (!create_glyph_runs(runinfo, els)) {
        _DBG_PRINTF("%s: failed to call create_glyph_runs.\n");
        goto out;
    }

    ok = TRUE;

out:
    if (els && els != local_els)
        free(els);

    if (ok)
        return runinfo;

    free(runinfo);

    return NULL;
}

BOOL GUIAPI SetPartFontInGlyphRuns(GLYPHRUNINFO* runinfo,
        int start_index, int length, LOGFONT* logfont)
{
    if (runinfo == NULL)
        return FALSE;

    // can not change font after shaped the glyphs
    if (runinfo->se.engine != NULL)
        return FALSE;

    // can not change font for empty runs
    if (list_empty(&runinfo->run_head))
        return FALSE;

    return FALSE;
}

RGBCOLOR __mg_glyphruns_get_color(const GLYPHRUNINFO* runinfo, int index)
{
    struct list_head *i;

    list_for_each(i, &runinfo->cm_head.list) {
        UCHARCOLORMAP* color_entry;
        color_entry = (UCHARCOLORMAP*)i;
        if (index >= color_entry->si &&
                (index < color_entry->si + color_entry->len)) {
            return color_entry->color;
        }
    }

    return runinfo->cm_head.color;
}

BOOL GUIAPI SetPartColorInGlyphRuns(GLYPHRUNINFO* runinfo,
        int start_index, int length, RGBCOLOR color)
{
    UCHARCOLORMAP* color_entry = NULL;

    if (runinfo == NULL || start_index < 0 || length < 0 ||
            start_index > runinfo->nr_ucs ||
            (start_index + length) > runinfo->nr_ucs) {
        goto error;
    }

    color_entry = calloc(1, sizeof(UCHARCOLORMAP));
    if (color_entry == NULL) {
        goto error;
    }

    color_entry->si = start_index;
    color_entry->len = length;
    color_entry->color = color;

    list_add(&color_entry->list, &runinfo->cm_head.list);

    return TRUE;

error:
    return FALSE;
}

BOOL GUIAPI ResetFontInGlyphRuns(GLYPHRUNINFO* runinfo, LOGFONT* logfont)
{
    if (runinfo == NULL)
        return FALSE;

    while (!list_empty(&runinfo->run_head)) {
        GLYPHRUN* run = (GLYPHRUN*)runinfo->run_head.prev;
        list_del(runinfo->run_head.prev);
        runinfo->se.destroy_glyphs(runinfo->se.engine, run->gs);
        free(run);
    }

    if (runinfo->se.engine) {
        runinfo->se.destroy_engine(runinfo->se.engine);
    }

    if (runinfo->l2g) {
        free(runinfo->l2g);
        runinfo->l2g = NULL;
    }

    if (runinfo->ges) {
        free(runinfo->ges);
        runinfo->ges = NULL;
    }

    runinfo->lf = logfont;

    return create_glyph_runs(runinfo, NULL);
}

static void set_run_dir(GLYPHRUNINFO* runinfo, GLYPHRUN* run,
        GlyphRunDir run_dir, GlyphOrient glyph_orient)
{
}

BOOL GUIAPI ResetDirectionInGlyphRuns(GLYPHRUNINFO* runinfo,
        GlyphRunDir run_dir, GlyphOrient glyph_orient,
        GlyphOrientPolicy orient_policy)
{
    struct list_head *i;

    if (runinfo == NULL)
        return FALSE;

    runinfo->run_dir = run_dir;
    runinfo->ort_base = glyph_orient;
    runinfo->ort_plc = orient_policy;

    list_for_each(i, &runinfo->run_head) {
        GLYPHRUN* run = (GLYPHRUN*)i;
        if (runinfo->se.engine && run->gs) {
            runinfo->se.destroy_glyphs(runinfo->se.engine, run->gs);
        }

        set_run_dir(runinfo, run, run_dir, glyph_orient);
    }

    if (runinfo->l2g) {
        free(runinfo->l2g);
        runinfo->l2g = NULL;
    }

    if (runinfo->ges) {
        free(runinfo->ges);
        runinfo->ges = NULL;
    }

    if (runinfo->se.engine) {
        runinfo->se.destroy_engine(runinfo->se.engine);
        runinfo->se.engine = NULL;
    }

    return TRUE;
}

BOOL GUIAPI ResetColorInGlyphRuns(GLYPHRUNINFO* runinfo, RGBCOLOR color)
{
    if (runinfo == NULL)
        return FALSE;

    while (!list_empty(&runinfo->cm_head.list)) {
        UCHARCOLORMAP* entry = (UCHARCOLORMAP*)runinfo->cm_head.list.prev;
        list_del(runinfo->cm_head.list.prev);
        free(entry);
    }

    runinfo->cm_head.color = color;
    return TRUE;
}

#if 0
BOOL GUIAPI ResetBreaksInGlyphRuns(GLYPHRUNINFO* runinfo,
    Uint8 ctr, Uint8 wbr, Uint8 lbp)
{
    if (runinfo == NULL)
        return FALSE;

    // Re-calculate the breaking opportunities
    if (UStrGetBreaks(runinfo->run_head.st, ctr, wbr, lbp,
            runinfo->ucs, runinfo->run_head.nr_ucs, &runinfo->bos) == 0) {
        _DBG_PRINTF("%s: failed to get breaking opportunities.\n");
        return FALSE;
    }

    return TRUE;
}
#endif

BOOL GUIAPI DestroyGlyphRunInfo(GLYPHRUNINFO* runinfo)
{
    if (runinfo == NULL)
        return FALSE;

    while (!list_empty(&runinfo->cm_head.list)) {
        UCHARCOLORMAP* entry = (UCHARCOLORMAP*)runinfo->cm_head.list.prev;
        list_del(runinfo->cm_head.list.prev);
        free(entry);
    }

    while (!list_empty(&runinfo->run_head)) {
        GLYPHRUN* run = (GLYPHRUN*)runinfo->run_head.prev;
        list_del(runinfo->run_head.prev);
        runinfo->se.destroy_glyphs(runinfo->se.engine, run->gs);
        free(run);
    }

    if (runinfo->se.engine) {
        runinfo->se.destroy_engine(runinfo->se.engine);
    }

    if (runinfo->l2g) free(runinfo->l2g);
    if (runinfo->ges) free(runinfo->ges);

    free(runinfo);
    return TRUE;
}

#endif /*  _MGCHARSET_UNICODE */

