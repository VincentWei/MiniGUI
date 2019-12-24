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
** textrunsinfo.c: The implementation of APIs related to TEXTRUNS
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
#include "textruns.h"
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
    TEXTRUNS*   runinfo;
    const Uchar32*  text;
    const Uchar32*  end;
    TextRun*        run;

    const Uchar32*  run_start;
    const Uchar32*  run_end;

    BidiLevel*      els;
    const Uchar32*  emb_end;
    int             emb_end_offset;
    BidiLevel       emb_level;
    Uint8           upright;
#if 0
    GlyphGravity   grv_rsv;
    BOOL            centered_baseline;
#endif

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
        state->upright = state->width_iter.upright;
#if 0
        GlyphGravity gravity = state->runinfo->grv_base;
        GlyphGravityPolicy gravity_policy = state->runinfo->grv_plc;

        if (gravity == GLYPH_GRAVITY_AUTO)
            gravity = state->runinfo->grv_rsv;

        state->grv_rsv = ScriptGetGlyphGravityForWide(
                state->script_iter.script, state->width_iter.upright,
                gravity, gravity_policy);
#endif
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
        BOOL without_shaping = (state->run->flags & TEXTRUN_FLAG_NO_SHAPING);
        if (!force_break &&
                state->run->lc == state->derived_lang &&
                without_shaping == no_shaping) {
            state->run->len++;
            return;
        }
    }

    state->run = mg_slice_new(TextRun);
    state->run->fontname = NULL;
    state->run->si      = pos - state->text;
    state->run->len     = 1;
    state->run->lc      = state->derived_lang;
    state->run->st      = state->script_iter.script;
    state->run->el      = state->emb_level;
    state->run->flags   = 0;

    if (no_shaping) {
        state->run->flags |= TEXTRUN_FLAG_NO_SHAPING;
    }
    if (state->upright) {
        state->run->flags |= TEXTRUN_FLAG_UPRIGHT;
    }
    //state->run->dir = state->runinfo->run_dir;
    //state->run->ort = state->grv_rsv;

#if 0
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
#endif

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

static BOOL create_text_runs(TEXTRUNS* runinfo, BidiLevel* els)
{
    BOOL ok = FALSE;
    TextRunState state;
    Uint8 local_types_buff[LOCAL_ARRAY_SIZE];
    Uint8* types_buff = NULL;

    if (runinfo->nr_ucs <= LOCAL_ARRAY_SIZE) {
        types_buff = local_types_buff;
    }
    else {
        types_buff = malloc(sizeof(Uint8) * runinfo->nr_ucs);
    }

    if (!types_buff)
        goto out;

    state.runinfo = runinfo;
    state.text = runinfo->ucs;
    state.end = runinfo->ucs + runinfo->nr_ucs;
    state.run = NULL;
    state.els = els;
    state.upright = 0;
#if 0
    state.grv_rsv = runinfo->ort_rsv;
    state.centered_baseline = GLYPH_GRAVITY_IS_VERTICAL(runinfo->grv_rsv);
#endif

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

static inline Uint8 get_gravity_from_fontname (const char* fontname)
{
    switch (fontGetOrientFromName (fontname)) {
    case FONT_ORIENT_SIDEWAYS:
        return GLYPH_GRAVITY_EAST;

    case FONT_ORIENT_UPSIDE_DOWN:
        return GLYPH_GRAVITY_NORTH;

    case FONT_ORIENT_SIDEWAYS_LEFT:
        return GLYPH_GRAVITY_WEST;

    case FONT_ORIENT_UPRIGHT:
    default:
        return GLYPH_GRAVITY_SOUTH;
    }
}

#ifdef _MGDEVEL_MODE
TEXTRUN* GetNextTextRunInfo(TEXTRUNS* runinfo,
        TEXTRUN* prev,
        const char** fontname, int* start_index, int* length,
        LanguageCode* lang_code, ScriptType* script,
        BidiLevel* embedding_level, Uint8* flags)
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
#if 0
    if (gravity) *orient = run->ort;
#endif
    if (flags) *flags = run->flags;

    return run;
}
#endif /* _MGDEVEL_MODE */

static BOOL is_fontname_conformed(const char* fontname)
{
    LOGFONT* lf;
    DEVFONT *mbc_devfont;
    BOOL ok = FALSE;

    if ((lf = CreateLogFontByName(fontname)) == NULL)
        return FALSE;

    mbc_devfont = lf->devfonts[1];
    if (mbc_devfont == NULL)
        goto out;

    if (mbc_devfont->charset_ops->conv_to_uc32)
        goto out;

    ok = TRUE;

out:
    DestroyLogFont(lf);
    return ok;
}

TEXTRUNS* GUIAPI CreateTextRuns(const Uchar32* ucs, int nr_ucs,
        LanguageCode lang_code, ParagraphDir base_dir,
        const char* logfont_name, RGBCOLOR color, RGBCOLOR bg_color,
        BreakOppo* break_oppos)
{
    BOOL ok = FALSE;

    TEXTRUNS* runinfo;
    BidiLevel  local_els[LOCAL_ARRAY_SIZE];
    BidiLevel* els = NULL;

    if (ucs == NULL || nr_ucs <= 0 || logfont_name == NULL) {
        return NULL;
    }

    if (!is_fontname_conformed(logfont_name)) {
        _WRN_PRINTF("Please check your fontname; it must be in Unicode charset.\n");
        return NULL;
    }

    runinfo = (TEXTRUNS*)mg_slice_new0(TEXTRUNS);
    if (runinfo == NULL) {
        return NULL;
    }

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        els = local_els;
    else
        els = (BidiLevel*)malloc(nr_ucs * sizeof(BidiLevel));

    if (!els) {
        _ERR_PRINTF("%s: failed to allocate space for embedding levels.\n",
            __FUNCTION__);
        goto out;
    }

    UBidiGetParagraphEmbeddingLevelsAlt(ucs, nr_ucs, &base_dir, els);

    // Initialize other fields
    runinfo->ucs        = ucs;
    runinfo->fontname   = strdup(logfont_name);
    runinfo->nr_ucs     = nr_ucs;
    if (lang_code == LANGCODE_unknown)
        runinfo->lc     = LANGCODE_en;  // fallback to English
    else
        runinfo->lc     = lang_code;
    runinfo->base_dir = base_dir;

#if 0
    runinfo->base_level = (base_dir == BIDI_PGDIR_LTR) ? 0 : 1;
    runinfo->grv_base   = glyph_orient;
    runinfo->grv_plc    = gravity_policy;
    if (runinfo->grv_base == GLYPH_GRAVITY_AUTO)
        runinfo->grv_rsv = get_gravity_from_fontname (runinfo->fontname);
    else
        runinfo->grv_rsv = runinfo->ort_base;
#endif

    INIT_LIST_HEAD(&runinfo->fg_colors.list);
    runinfo->fg_colors.si = 0;
    runinfo->fg_colors.len = nr_ucs;
    runinfo->fg_colors.value = color;

    INIT_LIST_HEAD(&runinfo->bg_colors.list);
    runinfo->bg_colors.si = 0;
    runinfo->bg_colors.len = nr_ucs;
    runinfo->bg_colors.value = bg_color;

    INIT_LIST_HEAD(&runinfo->truns);
    runinfo->nr_runs = 0;

    if (!create_text_runs(runinfo, els)) {
        _ERR_PRINTF("%s: failed to call create_text_runs.\n",
            __FUNCTION__);
        goto out;
    }

    if (break_oppos) {
        struct list_head* i;
        list_for_each(i, &runinfo->truns) {
            TextRun* trun = (TextRun*)i;
            UStrTailorBreaks(trun->st, runinfo->ucs + trun->si, trun->len,
                break_oppos + trun->si);
        }
    }

    ok = TRUE;

out:
    if (els && els != local_els)
        free(els);

    if (ok)
        return runinfo;

    mg_slice_delete(TEXTRUNS, runinfo);

    return NULL;
}

TextRun* __mg_text_run_get_by_offset(TEXTRUNS* runinfo,
        int index, int *start_offset)
{
    struct list_head *i;
    TextRun* found = NULL;

    list_for_each(i, &runinfo->truns) {
        TextRun* trun = (TextRun*)i;
        if (index >= trun->si &&
                (index < trun->si + trun->len)) {
            found = trun;
            if (start_offset) {
                *start_offset = index - trun->si;
            }

            break;
        }
    }

    return found;
}

const TextRun* __mg_text_run_get_by_offset_const(
        const TEXTRUNS* runinfo, int index, int *start_offset)
{
    struct list_head *i;
    const TextRun* found = NULL;

    list_for_each(i, &runinfo->truns) {
        const TextRun* trun = (const TextRun*)i;
        if (index >= trun->si &&
                (index < trun->si + trun->len)) {
            found = trun;
            if (start_offset) {
                *start_offset = index - trun->si;
            }

            break;
        }
    }

    return found;
}

TextRun* __mg_text_run_copy(const TextRun *trun)
{
    TextRun *result;

    if (trun == NULL)
        return NULL;

    result = mg_slice_new(TextRun);
    memcpy(result, trun, sizeof(TextRun));

    return result;
}

TextRun* __mg_text_run_split(TextRun *orig, int split_index)
{
    TextRun *new_run;

    if (orig == NULL)
        return NULL;
    if (split_index <= 0)
        return NULL;
    if (split_index >= orig->len)
        return NULL;

    new_run = __mg_text_run_copy(orig);
    new_run->len = split_index;

    orig->si += split_index;
    orig->len -= split_index;

    return new_run;
}

BOOL GUIAPI SetFontNameInTextRuns(TEXTRUNS* runinfo,
        int start_index, int length, const char* logfont_name)
{
    TextRun* run;
    int start_offset;
    int left_ucs;;

    if (runinfo == NULL || logfont_name == NULL ||
            start_index < 0 || start_index >= runinfo->nr_ucs)
        return FALSE;

    // can not change font for empty runs
    if (list_empty(&runinfo->truns))
        return FALSE;

    if (!is_fontname_conformed(logfont_name)) {
        return FALSE;
    }

    // normalize length
    if (length <= 0)
        length = runinfo->nr_ucs;
    if ((start_index + length) > runinfo->nr_ucs)
        length = runinfo->nr_ucs - start_index;

    // change the default fontname
    if (start_index == 0 && length >= runinfo->nr_ucs) {
        struct list_head* i;

        // reset all runs fontname
        list_for_each(i, &runinfo->truns) {
            TextRun* run = (TextRun*)i;
            if (run->fontname) {
                free(run->fontname);
                run->fontname = NULL;
            }
        }

        // do not allow to set the same font as the default
        if (strcmp(logfont_name, runinfo->fontname) == 0)
            return TRUE;

        free(runinfo->fontname);
        runinfo->fontname = strdup(logfont_name);
        return TRUE;
    }

    left_ucs = length;
    while (left_ucs > 0) {
        run = __mg_text_run_get_by_offset(runinfo, start_index, &start_offset);

        if (run == NULL)
            return FALSE;

        // no need to change
        if (run->fontname && strcmp(logfont_name, run->fontname) == 0) {
            start_index = run->si + run->len;
            left_ucs -= run->len - start_offset;
            continue;
        }

        if (start_offset == 0) {
            if (left_ucs >= run->len) {
                // change the fontname of current run
                if (run->fontname) {
                    free(run->fontname);
                }
                run->fontname = strdup(logfont_name);

                start_index += run->len;
                left_ucs -= run->len;
            }
            else {
                TextRun* new_run;

                new_run = __mg_text_run_split(run, run->len - left_ucs);
                new_run->fontname = strdup(logfont_name);
                // insert the new run to the list.
                __list_add(&new_run->list, run->list.prev, &run->list);

                left_ucs = 0;
            }
        }
        else {
            TextRun* new_run;

            new_run = __mg_text_run_split(run, start_offset);
            if (new_run->fontname)
                new_run->fontname = strdup(new_run->fontname);

            // insert the new run to the list.
            __list_add(&new_run->list, run->list.prev, &run->list);
        }
    }

    return TRUE;
}

const char* GUIAPI GetFontNameInTextRuns(const TEXTRUNS* runinfo, int index)
{
    struct list_head *i;

    if (runinfo == NULL)
        return NULL;

    list_for_each(i, &runinfo->truns) {
        TextRun* run = (TextRun*)i;
        if (index >= run->si && (index < run->si + run->len)) {
            if (run->fontname == NULL)
                goto out;
            return run->fontname;
        }
    }

out:
    return runinfo->fontname;
}

BOOL GUIAPI SetTextColorInTextRuns(TEXTRUNS* runinfo,
        int start_index, int length, RGBCOLOR color)
{
    TextColorMap* color_entry = NULL;

    if (runinfo == NULL ||
            start_index < 0 || start_index >= runinfo->nr_ucs) {
        goto error;
    }

    // normalize length
    if (length <= 0)
        length = runinfo->nr_ucs;
    if ((start_index + length) > runinfo->nr_ucs)
        length = runinfo->nr_ucs - start_index;

    // change the default text color
    if (start_index == 0 && length >= runinfo->nr_ucs) {
        runinfo->fg_colors.value = color;

        while (!list_empty(&runinfo->fg_colors.list)) {
            TextColorMap* entry = (TextColorMap*)runinfo->fg_colors.list.prev;
            list_del(runinfo->fg_colors.list.prev);
            mg_slice_delete(TextColorMap, entry);
        }
        return TRUE;
    }

    color_entry = mg_slice_new0(TextColorMap);
    if (color_entry == NULL) {
        goto error;
    }

    color_entry->si = start_index;
    color_entry->len = length;
    color_entry->value = color;

    list_add(&color_entry->list, &runinfo->fg_colors.list);

    return TRUE;

error:
    return FALSE;
}

RGBCOLOR GetTextColorInTextRuns(const TEXTRUNS* runinfo, int index)
{
    struct list_head *i;

    list_for_each(i, &runinfo->fg_colors.list) {
        TextColorMap* color_entry;
        color_entry = (TextColorMap*)i;
        if (index >= color_entry->si &&
                (index < color_entry->si + color_entry->len)) {
            return color_entry->value;
        }
    }

    return runinfo->fg_colors.value;
}

BOOL GUIAPI SetBackgroundColorInTextRuns(TEXTRUNS* runinfo,
        int start_index, int length, RGBCOLOR color)
{
    TextColorMap* color_entry = NULL;

    if (runinfo == NULL ||
            start_index < 0 || start_index >= runinfo->nr_ucs) {
        goto error;
    }

    // normalize length
    if (length <= 0)
        length = runinfo->nr_ucs;
    if ((start_index + length) > runinfo->nr_ucs)
        length = runinfo->nr_ucs - start_index;

    // change the default background color
    if (start_index == 0 && length >= runinfo->nr_ucs) {
        runinfo->bg_colors.value = color;

        while (!list_empty(&runinfo->bg_colors.list)) {
            TextColorMap* entry = (TextColorMap*)runinfo->bg_colors.list.prev;
            list_del(runinfo->bg_colors.list.prev);
            mg_slice_delete(TextColorMap, entry);
        }
        return TRUE;
    }

    color_entry = mg_slice_new0(TextColorMap);
    if (color_entry == NULL) {
        goto error;
    }

    color_entry->si = start_index;
    color_entry->len = length;
    color_entry->value = color;

    list_add(&color_entry->list, &runinfo->bg_colors.list);

    return TRUE;

error:
    return FALSE;
}

RGBCOLOR GetBackgroundColorInTextRuns(const TEXTRUNS* runinfo, int index)
{
    struct list_head *i;

    list_for_each(i, &runinfo->bg_colors.list) {
        TextColorMap* color_entry;
        color_entry = (TextColorMap*)i;
        if (index >= color_entry->si &&
                (index < color_entry->si + color_entry->len)) {
            return color_entry->value;
        }
    }

    return runinfo->bg_colors.value;
}

BOOL GUIAPI DestroyTextRuns(TEXTRUNS* runinfo)
{
    if (runinfo == NULL)
        return FALSE;

    while (!list_empty(&runinfo->bg_colors.list)) {
        TextColorMap* entry = (TextColorMap*)runinfo->bg_colors.list.prev;
        list_del(runinfo->bg_colors.list.prev);
        mg_slice_delete(TextColorMap, entry);
    }

    while (!list_empty(&runinfo->fg_colors.list)) {
        TextColorMap* entry = (TextColorMap*)runinfo->fg_colors.list.prev;
        list_del(runinfo->fg_colors.list.prev);
        mg_slice_delete(TextColorMap, entry);
    }

    while (!list_empty(&runinfo->truns)) {
        TextRun* run = (TextRun*)runinfo->truns.prev;
        list_del(runinfo->truns.prev);
        if (run->fontname)
            free(run->fontname);
        mg_slice_delete(TextRun, run);
    }

    if (runinfo->sei.inst) {
        runinfo->sei.free(runinfo->sei.inst);
    }

    free(runinfo->fontname);
    mg_slice_delete(TEXTRUNS, runinfo);
    return TRUE;
}

#endif /*  _MGCHARSET_UNICODE */

