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
** textrunsinfo.c: The implementation of APIs related to TEXTRUNSINFO
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
#include "textrunsinfo.h"
#include "fontname.h"

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

typedef struct _TextRunState {
    TEXTRUNSINFO*   runinfo;
    const Uchar32*  text;
    const Uchar32*  end;
    TextRun*        run;

    const Uchar32*  run_start;
    const Uchar32*  run_end;

    BidiLevel*      els;
    const Uchar32*  emb_end;
    int             emb_end_offset;
    BidiLevel       emb_level;

    GlyphOrient     ort_rsv;
    BOOL            centered_baseline;

    Uint8           changed;

    ScriptIterator  script_iter;
    WidthIterator   width_iter;
    EmojiIterator   emoji_iter;

    LanguageCode    derived_lang;

} TextRunState;

static void update_embedding_end(TextRunState *state)
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

static void update_end (TextRunState *state)
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

static void state_update_for_new_run (TextRunState *state)
{
    if (state->changed & (SCRIPT_CHANGED | WIDTH_CHANGED)) {
        GlyphOrient orient = state->runinfo->ort_base;
        GlyphOrientPolicy orient_policy = state->runinfo->ort_plc;

        if (orient == GLYPH_ORIENT_AUTO)
            orient = state->runinfo->ort_rsv;

        state->ort_rsv = GetWideGlyphOrientationForScript(
                state->script_iter.script, state->width_iter.upright,
                orient, orient_policy);
    }

    if (state->changed & (SCRIPT_CHANGED | LANG_CHANGED)) {
        LanguageCode old_derived_lang = state->derived_lang;
        state->derived_lang = compute_derived_language (state->runinfo->lc,
                state->script_iter.script);
        if (old_derived_lang != state->derived_lang)
            state->changed |= DERIVED_LANG_CHANGED;
    }
}

static void state_add_character(TextRunState *state,
        BOOL no_shaping, BOOL force_break, const Uchar32* pos)
{
    if (state->run) {
        BOOL without_shaping = (state->run->flags & TEXTRUN_FLAG_NOT_SHAPING);
        if (!force_break &&
                state->run->lc == state->derived_lang &&
                without_shaping == no_shaping) {
            state->run->len++;
            return;
        }
    }

    state->run = malloc(sizeof(TextRun));
    state->run->fontname = NULL;
    state->run->si  = pos - state->text;
    state->run->len = 1;
    state->run->lc  = state->derived_lang;
    state->run->st  = state->script_iter.script;
    state->run->el  = state->emb_level;
    state->run->dir = state->runinfo->run_dir;
    state->run->ort = state->ort_rsv;
    state->run->flags = 0;

    if (no_shaping) {
        state->run->flags |= TEXTRUN_FLAG_NOT_SHAPING;
    }

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
    switch (state->run->ort) {
    case GLYPH_GRAVITY_SOUTH:
    default:
        break;
    case GLYPH_GRAVITY_NORTH:
        state->run->el++;
        break;
    case GLYPH_GRAVITY_EAST:
        state->run->el += 1;
        state->run->el &= ~1;
        break;
    case GLYPH_GRAVITY_WEST:
        state->run->el |= 1;
        break;
    }

    state->run->flags |= state->centered_baseline ?
            TEXTRUN_FLAG_CENTERED_BASELINE : 0;

    list_add_tail(&state->run->list, &state->runinfo->truns);
    state->runinfo->nr_runs++;
}

static BOOL state_process_run (TextRunState *state)
{
    const Uchar32* p;
    BOOL last_was_forced_break = FALSE;

    state_update_for_new_run (state);

    for (p = state->run_start; p < state->run_end; p++) {
        Uchar32 uc = *p;

        BOOL is_forced_break = (uc == UCHAR_TAB || uc == UCHAR_LINE_SEPARATOR);
        BOOL no_shaping;

        no_shaping = is_uchar_no_shaping(uc);

        state_add_character(state, no_shaping,
                is_forced_break || last_was_forced_break, p);

        last_was_forced_break = is_forced_break;
    }

    /* Finish the final item from the current segment */
    state->run = NULL;

    return TRUE;
}

static BOOL state_next (TextRunState *state)
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

static BOOL create_text_runs(TEXTRUNSINFO* runinfo, BidiLevel* els)
{
    BOOL ok = FALSE;
    TextRunState state;
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
    state.run = NULL;
    state.els = els;
    state.ort_rsv = runinfo->ort_rsv;

    /* check font gravity here */
    state.centered_baseline = GLYPH_ORIENT_IS_VERTICAL(runinfo->ort_rsv);

    state.run_start = state.text;
    state.changed = 0;

    state.emb_end_offset = 0;
    state.emb_end = runinfo->ucs;
    update_embedding_end(&state);

    __mg_script_iterator_init (&state.script_iter, state.text, runinfo->nr_ucs);
    __mg_width_iterator_init (&state.width_iter, state.text, runinfo->nr_ucs);
    __mg_emoji_iterator_init (&state.emoji_iter, state.text, runinfo->nr_ucs,
        types_buff);

    if (types_buff && types_buff != local_types_buff) {
        free (types_buff);
        types_buff = NULL;
    }

    update_end (&state);
    state.changed = EMBEDDING_CHANGED | SCRIPT_CHANGED | LANG_CHANGED |
            FONT_CHANGED | WIDTH_CHANGED | EMOJI_CHANGED;

    do {

        if (!state_process_run (&state))
            goto out;

    } while (state_next (&state));

    ok = TRUE;

out:
    if (types_buff && types_buff != local_types_buff)
        free (types_buff);

    return ok;
}

static inline Uint8 get_glyph_orient_from_fontname (const char* fontname)
{
    switch (fontGetOrientFromName (fontname)) {
    case FONT_ORIENT_SIDEWAYS:
        return GLYPH_ORIENT_SIDEWAYS;

    case FONT_ORIENT_UPSIDE_DOWN:
        return GLYPH_ORIENT_UPSIDE_DOWN;

    case FONT_ORIENT_SIDEWAYS_LEFT:
        return GLYPH_ORIENT_SIDEWAYS_LEFT;

    case FONT_ORIENT_UPRIGHT:
    default:
        return GLYPH_ORIENT_UPRIGHT;
    }
}

#ifdef _MGDEVEL_MODE
void* GetNextTextRunInfo(TEXTRUNSINFO* runinfo,
        void* prev,
        const char** fontname, int* start_index, int* length,
        LanguageCode* lang_code, ScriptType* script,
        BidiLevel* embedding_level, GlyphRunDir* run_dir,
        GlyphOrient* orient, Uint8* flags)
{
    TextRun* run = NULL;

    if (prev == NULL) {
        if (list_empty(&runinfo->truns))
            return NULL;
        run = (TextRun*)runinfo->truns.next;
    }
    else {
        struct list_head* list_entry = (struct list_head*)prev;
        if (list_entry->next == &runinfo->truns)
            return NULL;

        run = (TextRun*)list_entry->next;
    }

    if (fontname) *fontname = run->fontname;
    if (start_index) *start_index = run->si;
    if (length) *length = run->len;
    if (lang_code) *lang_code = run->lc;
    if (script) *script = run->st;
    if (embedding_level) *embedding_level = run->el;
    if (run_dir) *run_dir = run->dir;
    if (orient) *orient = run->ort;
    if (flags) *flags = run->flags;

    return run;
}
#endif /* _MGDEVEL_MODE */

static BOOL is_fontname_conformed(const char* fontname,
        GlyphOrient glyph_orient, GlyphOrientPolicy orient_policy)
{
    LOGFONT* lf;
    DEVFONT *sbc_devfont, *mbc_devfont;
    BOOL ok = FALSE;

    if ((lf = CreateLogFontByName(fontname)) == NULL)
        return FALSE;

    sbc_devfont = lf->devfonts[0];
    mbc_devfont = lf->devfonts[1];
    if (mbc_devfont == NULL)
        goto out;

    if (mbc_devfont->charset_ops->conv_to_uc32)
        goto out;

    /* TODO: check more */
    if (glyph_orient != GLYPH_ORIENT_UPRIGHT) {

        if (sbc_devfont->font_ops->is_rotatable(lf, sbc_devfont, 100) != 100
                || mbc_devfont->font_ops->is_rotatable(lf, mbc_devfont, 100)
                    != 100)
            goto out;
    }

    ok = TRUE;

out:
    DestroyLogFont(lf);
    return ok;
}

TEXTRUNSINFO* GUIAPI CreateTextRunsInfo(Uchar32* ucs, int nr_ucs,
        LanguageCode lang_code, ParagraphDir base_dir, GlyphRunDir run_dir,
        GlyphOrient glyph_orient, GlyphOrientPolicy orient_policy,
        const char* logfont_name, RGBCOLOR color)
{
    BOOL ok = FALSE;

    TEXTRUNSINFO* runinfo;
    BidiLevel  local_els[LOCAL_ARRAY_SIZE];
    BidiLevel* els = NULL;

    if (ucs == NULL || nr_ucs <= 0 || logfont_name == NULL) {
        return NULL;
    }

    if (!is_fontname_conformed(logfont_name, glyph_orient, orient_policy)) {
        _WRN_PRINTF("Please check your fontname; it must conform to the glyph orientation specified.");
        return NULL;
    }

    runinfo = (TEXTRUNSINFO*)calloc(1, sizeof(TEXTRUNSINFO));
    if (runinfo == NULL) {
        return NULL;
    }

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        els = local_els;
    else
        els = (BidiLevel*)malloc (nr_ucs * sizeof(BidiLevel));

    if (!els) {
        _ERR_PRINTF("%s: failed to allocate space for embedding levels.\n",
            __FUNCTION__);
        goto out;
    }

    UBidiGetParagraphEmbeddingLevelsAlt(ucs, nr_ucs, &base_dir, els);

    /* the breaking opportunities should be passed to the layout function.
    // Calculate the breaking opportunities
    if (UStrGetBreaks(script_type, ctr, wbr, lbp,
            ucs, nr_ucs, &bos) == 0) {
        _ERR_PRINTF("%s: failed to get breaking opportunities.\n",
            __FUNCTION__);
        goto out;
    }
    */

    // Initialize other fields
    runinfo->ucs        = ucs;
    runinfo->fontname   = strdup(logfont_name);
    runinfo->nr_ucs     = nr_ucs;
    if (lang_code == LANGCODE_unknown)
        runinfo->lc     = LANGCODE_en;  // fallback to English
    else
        runinfo->lc     = lang_code;
    runinfo->base_level = (base_dir == BIDI_PGDIR_LTR) ? 0 : 1;
    runinfo->run_dir    = run_dir;
    runinfo->ort_base   = glyph_orient;
    runinfo->ort_plc    = orient_policy;
    if (runinfo->ort_base == GLYPH_ORIENT_AUTO)
        runinfo->ort_rsv = get_glyph_orient_from_fontname (runinfo->fontname);
    else
        runinfo->ort_rsv = runinfo->ort_base;

    INIT_LIST_HEAD(&runinfo->attrs.list);
    runinfo->attrs.si = 0;
    runinfo->attrs.len = nr_ucs;
    runinfo->attrs.type = TEXT_ATTR_TEXT_COLOR;
    runinfo->attrs.value = color;

    INIT_LIST_HEAD(&runinfo->truns);
    runinfo->nr_runs = 0;

    if (!create_text_runs(runinfo, els)) {
        _ERR_PRINTF("%s: failed to call create_text_runs.\n",
            __FUNCTION__);
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

BOOL GUIAPI SetFontInTextRuns(TEXTRUNSINFO* runinfo,
        int start_index, int length, const char* logfont_name)
{
    if (runinfo == NULL)
        return FALSE;

    // can not change font for empty runs
    if (list_empty(&runinfo->truns))
        return FALSE;

    // TODO
    return FALSE;
}

RGBCOLOR __mg_textruns_get_text_color(const TEXTRUNSINFO* runinfo, int index)
{
    struct list_head *i;

    list_for_each(i, &runinfo->attrs.list) {
        TextAttrMap* color_entry;
        color_entry = (TextAttrMap*)i;
        if (index >= color_entry->si &&
                (index < color_entry->si + color_entry->len) &&
            color_entry->type == TEXT_ATTR_TEXT_COLOR) {
            return color_entry->value;
        }
    }

    return runinfo->attrs.value;
}

const TextRun* __mg_textruns_get_by_offset(const TEXTRUNSINFO* runinfo,
        int offset, int *start_index)
{
    return NULL;
}

BOOL GUIAPI SetTextColorInTextRuns(TEXTRUNSINFO* runinfo,
        int start_index, int length, RGBCOLOR color)
{
    TextAttrMap* color_entry = NULL;

    if (runinfo == NULL || start_index < 0 || length < 0 ||
            start_index > runinfo->nr_ucs ||
            (start_index + length) > runinfo->nr_ucs) {
        goto error;
    }

    color_entry = calloc(1, sizeof(TextAttrMap));
    if (color_entry == NULL) {
        goto error;
    }

    color_entry->si = start_index;
    color_entry->len = length;
    color_entry->type = TEXT_ATTR_TEXT_COLOR;
    color_entry->value = color;

    list_add(&color_entry->list, &runinfo->attrs.list);

    return TRUE;

error:
    return FALSE;
}

BOOL GUIAPI DestroyTextRunsInfo(TEXTRUNSINFO* runinfo)
{
    if (runinfo == NULL)
        return FALSE;

    while (!list_empty(&runinfo->attrs.list)) {
        TextAttrMap* entry = (TextAttrMap*)runinfo->attrs.list.prev;
        list_del(runinfo->attrs.list.prev);
        free(entry);
    }

    while (!list_empty(&runinfo->truns)) {
        TextRun* run = (TextRun*)runinfo->truns.prev;
        list_del(runinfo->truns.prev);
        if (run->fontname)
            free(run->fontname);
        free(run);
    }

    if (runinfo->sei.inst) {
        runinfo->sei.free(runinfo->sei.inst);
    }

    free(runinfo->fontname);
    free(runinfo);
    return TRUE;
}

#endif /*  _MGCHARSET_UNICODE */

