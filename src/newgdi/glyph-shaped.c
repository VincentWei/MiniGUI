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
** glyph-shaped.c:
**  Implementation of DrawShapedGlyph and DrawLayoutLine.
**
** Create by WEI Yongming at 2019/03/21
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "pixel_ops.h"
#include "cursor.h"
#include "drawtext.h"
#include "glyph.h"
#include "layoutinfo.h"

#ifdef _MGCHARSET_UNICODE

BOOL DrawShapedGlyph(HDC hdc, Glyph32 gv,
        const GLYPHPOS* glyph_pos, const RENDERDATA* render_data)
{
    RGBCOLOR fg_color, bg_color;

    if (glyph_pos == NULL || render_data == NULL)
        return FALSE;

    bg_color = GetBackgroundColorInTextRuns(render_data->truninfo,
        render_data->uc_index);

    if (glyph_pos->suppressed == 0 && glyph_pos->whitespace == 0) {
        int x_off, y_off;
        gal_pixel fg_pixel, bg_pixel;

        SelectFont(hdc, render_data->logfont);

        fg_color = GetTextColorInTextRuns(render_data->truninfo,
            render_data->uc_index);
        fg_pixel = DWORD2Pixel(hdc, fg_color);
        SetTextColor(hdc, (DWORD)fg_pixel);

        if (bg_color) {
            bg_pixel = DWORD2Pixel(hdc, bg_color);
            SetBkColor(hdc, (DWORD)bg_pixel);
            SetBkMode(hdc, BM_OPAQUE);
        }
        else {
            SetBkMode(hdc, BM_TRANSPARENT);
        }

        SetTextAlign(hdc, render_data->ta);
        x_off = glyph_pos->x_off;
        y_off = glyph_pos->y_off;
        if (render_data->logfont->rotation) {
            _gdi_get_rotated_point(&x_off, &y_off,
                    render_data->logfont->rotation);
        }

        DrawGlyph(hdc, glyph_pos->x + x_off,
                       glyph_pos->y + y_off, gv, NULL, NULL);
    }
    else if (glyph_pos->whitespace && bg_color) {
        // TODO: draw background for whitespace.
    }

    return TRUE;
}

int DrawLayoutLine(HDC hdc, const LAYOUTLINE* line, int x, int y)
{
    int n, log_x = 0, log_y = 0;
    struct list_head* i;
    const TEXTRUNSINFO* truninfo;
    const LAYOUTINFO* layout;
    Uint32 old_ta;
    PLOGFONT old_lf = NULL;

    if (line == NULL)
        return 0;

    layout = line->layout;
    truninfo = layout->truninfo;

    if ((layout->rf & GRF_WRITING_MODE_MASK) ==
            GRF_WRITING_MODE_HORIZONTAL_BT)
        old_ta = SetTextAlign(hdc, TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
    else if ((layout->rf & GRF_WRITING_MODE_MASK) ==
            GRF_WRITING_MODE_VERTICAL_RL)
        old_ta = SetTextAlign(hdc, TA_RIGHT | TA_TOP | TA_NOUPDATECP);
    else
        old_ta = SetTextAlign(hdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
    old_lf = GetCurFont(hdc);

    list_for_each(i, &line->gruns) {
        GlyphRun* run = (GlyphRun*)i;
        int j;

        SelectFont(hdc, run->lrun->lf);
        for (j = 0; j < run->gstr->nr_glyphs; j++) {
            int dev_x, dev_y;

            ShapedGlyph* gi = run->gstr->glyphs + j;
            int log_index = run->lrun->si + run->gstr->log_clusters[j];
            RGBCOLOR bg_color, fg_color;

            // We might use an interator to optimize the color settings.
            fg_color = GetTextColorInTextRuns(truninfo, log_index);
            bg_color = GetBackgroundColorInTextRuns(truninfo, log_index);
            SetTextColor(hdc, DWORD2Pixel(hdc, fg_color));
            if (bg_color) {
                SetBkColor(hdc, DWORD2Pixel(hdc, bg_color));
                SetBkMode(hdc, BM_OPAQUE);
            }
            else {
                SetBkMode(hdc, BM_TRANSPARENT);
            }

            if (run->lrun->dir == GLYPH_RUN_DIR_TTB ||
                    run->lrun->dir == GLYPH_RUN_DIR_BTT) {
                dev_y = log_x + gi->x_off;
                dev_x = log_y + gi->y_off;
            }
            else {
                dev_x = log_x + gi->x_off;
                dev_y = log_y + gi->y_off;
            }

            if ((run->lrun->flags & LAYOUTRUN_FLAG_NO_SHAPING) &&
                    gi->width > 0 && bg_color) {
                // FIXME: draw background here
            }
            else {
                DrawGlyph(hdc, x + dev_x, y + dev_y, gi->gv, NULL, NULL);
                n++;
            }

            log_x += gi->width;
        }
    }

    SelectFont(hdc, old_lf);
    SetTextAlign(hdc, old_ta);
    return n;
}

#if 0
int GUIAPI DrawShapedGlyphString(HDC hdc,
        LOGFONT* logfont_upright, LOGFONT* logfont_sideways,
        const ShapedGlyphS* shaped_glyphs,
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
            if (glyph_pos[i].orientation == GLYPH_ORIENT_UPRIGHT) {
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

#endif /* _MGCHARSET_UNICODE */
