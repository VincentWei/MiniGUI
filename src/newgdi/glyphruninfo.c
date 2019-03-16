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

static BOOL init_glyph_runs(GLYPHRUNINFO* runinfo)
{
    return FALSE;
}

GLYPHRUNINFO* GUIAPI CreateGlyphRunInfo(Uchar32* ucs, int nr_ucs,
        const char* lang_tag, const char* script_tag,
        GlyphRunDir run_dir, GlyphOrient glyph_orient, ParagraphDir base_dir,
        Uint8 ctr, Uint8 wbr, Uint8 lbp, LOGFONT* logfont, RGBCOLOR color)
{
    BOOL ok = FALSE;

    GLYPHRUNINFO* runinfo;
    BidiLevel* els = NULL;
    BreakOppo* bos = NULL;

    BidiLevel max_level = 0;

    BidiType local_bidi_ts[LOCAL_ARRAY_SIZE];
    BidiType *bidi_ts = NULL;

    BidiBracketType local_brk_ts[LOCAL_ARRAY_SIZE];
    BidiBracketType *brk_ts = NULL;

    BidiLevel level_or, level_and;

    ScriptType script_type = SCRIPT_COMMON;

    int i, j;

    runinfo = (GLYPHRUNINFO*)calloc(1, sizeof(GLYPHRUNINFO));
    if (ucs == NULL || nr_ucs <= 0 || runinfo == NULL) {
        return NULL;
    }

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        bidi_ts = local_bidi_ts;
    else
        bidi_ts = malloc (nr_ucs * sizeof(BidiType));

    if (!bidi_ts) {
        _DBG_PRINTF("%s: failed to allocate space for bidi types.\n");
        goto out;
    }

    UStrGetBidiTypes(ucs, nr_ucs, bidi_ts);

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        brk_ts = local_brk_ts;
    else
        brk_ts = (BidiBracketType*)malloc (nr_ucs * sizeof(BidiBracketType));

    if (!brk_ts) {
        _DBG_PRINTF("%s: failed to allocate space for bracket types.\n");
        goto out;
    }

    UStrGetBracketTypes (ucs, bidi_ts, nr_ucs, brk_ts);

    els = (BidiLevel*)malloc (nr_ucs * sizeof(BidiLevel));
    if (!els) {
        _DBG_PRINTF("%s: failed to allocate space for embedding levels.\n");
        goto out;
    }

    max_level = UBidiGetParagraphEmbeddingLevels(bidi_ts, brk_ts, nr_ucs,
            &base_dir, els) - 1;
    if (max_level < 0) {
        _DBG_PRINTF("%s: failed to get paragraph embedding levels.\n");
        goto out;
    }

    // Calculate the breaking opportunities
    if (UStrGetBreaks(script_type, ctr, wbr, lbp,
            ucs, nr_ucs, &bos) == 0) {
        _DBG_PRINTF("%s: failed to get breaking opportunities.\n");
        goto out;
    }

    // Initialize other fields
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

    runinfo->run_head.lt = lang_tag;
    runinfo->run_head.st = script_type;
    runinfo->run_head.level = BIDI_DIR_TO_LEVEL(base_dir);
    runinfo->run_head.dir = run_dir;
    runinfo->run_head.ort = glyph_orient;

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
    runinfo->run_head.all_even = (level_or & 0x1) == 0;
    /* If all of the levels had the LSB set, all chars were odd. */
    runinfo->run_head.all_odd = (level_and & 0x1) == 1;

    if (!init_glyph_runs(runinfo)) {
        _DBG_PRINTF("%s: failed to call init_glyph_runs.\n");
        goto out;
    }

    runinfo->ucs = ucs;
    runinfo->els = els;
    runinfo->bos = bos;
    ok = TRUE;

out:
    if (bidi_ts && bidi_ts != local_bidi_ts)
        free (bidi_ts);

    if (brk_ts && brk_ts != local_brk_ts)
        free (brk_ts);

    if (ok)
        return runinfo;

    if (runinfo->l2g) free(runinfo->l2g);
    if (els) free(els);
    if (bos) free(bos);
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

    return init_glyph_runs(runinfo);
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

BOOL GUIAPI ResetBreaksInGlyphRuns(GLYPHRUNINFO* runinfo,
    Uint8 ctr, Uint8 wbr, Uint8 lbp)
{
    if (runinfo == NULL)
        return FALSE;

    if (runinfo->bos) {
        free (runinfo->bos);
        runinfo->bos = NULL;
    }

    // Re-calculate the breaking opportunities
    if (UStrGetBreaks(runinfo->run_head.st, ctr, wbr, lbp,
            runinfo->ucs, runinfo->run_head.nr_ucs, &runinfo->bos) == 0) {
        _DBG_PRINTF("%s: failed to get breaking opportunities.\n");
        return FALSE;
    }

    return TRUE;
}

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
    if (runinfo->els) free(runinfo->els);
    if (runinfo->bos) free(runinfo->bos);

    free(runinfo);
    return TRUE;
}

#endif /*  _MGCHARSET_UNICODE */

