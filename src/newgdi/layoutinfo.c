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
        const TEXTRUNSINFO* runinfo, Uint32 render_flags,
        const BreakOppo* break_oppos, BOOL persist_lines,
        int letter_spacing, int word_spacing, int tab_size)
{
    LAYOUTINFO* layoutinfo;

    if (runinfo == NULL || runinfo->sei.inst == NULL) {
        return NULL;
    }

    layoutinfo = (LAYOUTINFO*)calloc(1, sizeof(LAYOUTINFO));
    if (layoutinfo == NULL) {
        return NULL;
    }

    layoutinfo->runinfo = runinfo;
    layoutinfo->bos = break_oppos;
    layoutinfo->rf = render_flags;
    layoutinfo->ls = letter_spacing;
    layoutinfo->ws = word_spacing;
    layoutinfo->ts = tab_size;

    INIT_LIST_HEAD(&layoutinfo->line_head);
    layoutinfo->left_ucs = runinfo->nr_ucs;

    layoutinfo->persist = persist_lines ? 1 : 0;

    return layoutinfo;
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
    while (!list_empty(&line->run_head)) {
        GLYPHRUN* run = (GLYPHRUN*)line->run_head.prev;
        list_del(line->run_head.prev);
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

LAYOUTLINE* GUIAPI LayoutNextLine(LAYOUTINFO* info,
        LAYOUTLINE* prev_Line,
        int* x, int* y, int max_extent, SIZE* line_size,
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

