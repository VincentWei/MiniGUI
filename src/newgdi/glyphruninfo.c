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
    GLYPHRUNINFO*   context;
    const Uchar32*  text;
    const Uchar32*  end;

    const Uchar32*  run_start;
    const Uchar32*  run_end;

    BidiLevel*      embedding_levels;
    const Uchar32*  embedding_end;
    int             embedding_end_offset;
    BidiLevel       embedding;

    GlyphOrient     orient;
    GlyphOrientPolicy   orient_policy;
    GlyphOrient     resolved_gravity;
    BOOL            centered_baseline;

    Uint8           changed;

    ScriptIterator  script_iter;
    const Uchar32*  script_end;
    ScriptType      script;

    WidthIterator   width_iter;
    EmojiIterator   emoji_iter;

    LanguageCode    derived_lang;
} GLYPHRUNSTATE;

static BOOL init_glyph_runs(GLYPHRUNINFO* runinfo, BidiLevel* els)
{
#if 0
    GLYPHRUNSTATE state;

    BidiLevel el = runinfo->els[0];
    ScriptType st = UCharGetScriptType(runinfo->ucs[0]);
#endif

    return FALSE;
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

    INIT_LIST_HEAD(&runinfo->cm_head.list);
    runinfo->cm_head.si = 0;
    runinfo->cm_head.len = nr_ucs;
    runinfo->cm_head.color = color;

    INIT_LIST_HEAD(&runinfo->run_head.list);
    runinfo->run_head.lf = logfont;
    runinfo->run_head.gs = NULL;
    runinfo->run_head.nr_gs = 0;

    runinfo->run_head.si = 0;
    runinfo->run_head.nr_ucs = nr_ucs;

    runinfo->run_head.lc = lang_code;
    runinfo->run_head.st = script_type;
    runinfo->run_head.level = BIDI_DIR_TO_LEVEL(base_dir);
    runinfo->run_head.dir = run_dir;
    runinfo->run_head.ort = glyph_orient;

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

    if (!init_glyph_runs(runinfo, els)) {
        _DBG_PRINTF("%s: failed to call init_glyph_runs.\n");
        goto out;
    }

    ok = TRUE;

out:
    if (els && els != local_els)
        free(els);

    if (ok)
        return runinfo;

    if (runinfo->l2g) free(runinfo->l2g);
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
    if (list_empty(&runinfo->run_head.list))
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
            start_index > runinfo->run_head.nr_ucs ||
            (start_index + length) > runinfo->run_head.nr_ucs) {
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

    while (!list_empty(&runinfo->run_head.list)) {
        GLYPHRUN* run = (GLYPHRUN*)runinfo->run_head.list.prev;
        list_del(runinfo->run_head.list.prev);
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

    runinfo->run_head.lf = logfont;

    return init_glyph_runs(runinfo, NULL);
}

static void set_run_dir(GLYPHRUNINFO* runinfo, GLYPHRUN* run,
        GlyphRunDir run_dir, GlyphOrient glyph_orient)
{
}

BOOL GUIAPI ResetDirectionInGlyphRuns(GLYPHRUNINFO* runinfo,
        GlyphRunDir run_dir, GlyphOrient glyph_orient)
{
    struct list_head *i;

    if (runinfo == NULL)
        return FALSE;

    runinfo->run_head.dir = run_dir;
    runinfo->run_head.ort = glyph_orient;

    list_for_each(i, &runinfo->run_head.list) {
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

    while (!list_empty(&runinfo->run_head.list)) {
        GLYPHRUN* run = (GLYPHRUN*)runinfo->run_head.list.prev;
        list_del(runinfo->run_head.list.prev);
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

