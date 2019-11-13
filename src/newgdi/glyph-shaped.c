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
#include "layout.h"

#ifdef _MGCHARSET_UNICODE

BOOL DrawShapedGlyph(HDC hdc, Glyph32 gv,
        const GLYPHPOS* glyph_pos, const RENDERDATA* render_data)
{
    RGBCOLOR fg_color, bg_color;

    if (glyph_pos == NULL || render_data == NULL)
        return FALSE;

    bg_color = GetBackgroundColorInTextRuns(render_data->truns,
        render_data->uc_index);

    if (glyph_pos->suppressed == 0 && glyph_pos->whitespace == 0) {
        int x_off, y_off;
        gal_pixel fg_pixel, bg_pixel;

        SelectFont(hdc, render_data->logfont);

        fg_color = GetTextColorInTextRuns(render_data->truns,
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
    int n, line_offset, line_adv = 0;
    struct list_head* i;
    const TEXTRUNS* truns;
    const LAYOUT* layout;
    Uint32 def_ta, up_ta;
    PLOGFONT old_lf = NULL;

    if (line == NULL)
        return 0;

    layout = line->layout;
    truns = layout->truns;

    switch (layout->rf & GRF_WRITING_MODE_MASK) {
    case GRF_WRITING_MODE_HORIZONTAL_BT:
        def_ta = TA_LEFT | TA_BOTTOM | TA_NOUPDATECP;
        up_ta = def_ta;
        break;

    case GRF_WRITING_MODE_VERTICAL_RL:
        def_ta = TA_RIGHT | TA_TOP | TA_NOUPDATECP;
        up_ta = def_ta;
        break;

    case GRF_WRITING_MODE_VERTICAL_LR:
        def_ta = TA_LEFT | TA_BOTTOM | TA_NOUPDATECP;
        up_ta = TA_LEFT | TA_TOP | TA_NOUPDATECP;
        break;

    case GRF_WRITING_MODE_HORIZONTAL_TB:
    default:
        def_ta = TA_LEFT | TA_TOP | TA_NOUPDATECP;
        up_ta = def_ta;
        break;
    }

    line_offset = __mg_layout_get_line_offset(layout, line);

    old_lf = GetCurFont(hdc);
    list_for_each(i, &line->gruns) {
        GlyphRun* run = (GlyphRun*)i;
        int j;
        int log_x, log_y;

        SelectFont(hdc, run->lrun->lf);
        for (j = 0; j < run->gstr->nr_glyphs; j++) {
            int x_off, y_off;
            Uint32 ta;

            ShapedGlyph* gi = run->gstr->glyphs + j;
            int log_index = run->lrun->si + run->gstr->log_clusters[j];
            RGBCOLOR bg_color, fg_color;

            // We might use an interator to optimize the color settings.
            fg_color = GetTextColorInTextRuns(truns, log_index);
            bg_color = GetBackgroundColorInTextRuns(truns, log_index);
            SetTextColor(hdc, DWORD2Pixel(hdc, fg_color));
            if (bg_color) {
                SetBkColor(hdc, DWORD2Pixel(hdc, bg_color));
                SetBkMode(hdc, BM_OPAQUE);
            }
            else {
                SetBkMode(hdc, BM_TRANSPARENT);
            }

            if (layout->rf & GRF_WRITING_MODE_VERTICAL_FLAG) {
                log_x = 0;
                log_y = line_adv;
                log_y += line_offset;
            }
            else {
                log_x = line_adv;
                log_x += line_offset;
                log_y = 0;
            }

            // vertical layout
            if (run->lrun->flags & LAYOUTRUN_FLAG_CENTERED_BASELINE) {
                ta = up_ta;
                if (run->lrun->ort == GLYPH_ORIENT_UPSIDE_DOWN) {
                    log_y += run->gstr->glyphs[j].width;
                    if ((layout->rf & GRF_WRITING_MODE_MASK) ==
                            GRF_WRITING_MODE_VERTICAL_RL)
                        log_x -= (line->height - gi->height) / 2;
                    else if ((layout->rf & GRF_WRITING_MODE_MASK) ==
                            GRF_WRITING_MODE_VERTICAL_LR)
                        log_x += (line->height + gi->height) / 2;
                }
                else if (run->lrun->ort == GLYPH_ORIENT_UPRIGHT) {
                    if ((layout->rf & GRF_WRITING_MODE_MASK) ==
                            GRF_WRITING_MODE_VERTICAL_RL)
                        log_x -= (line->height - gi->height) / 2;
                    else if ((layout->rf & GRF_WRITING_MODE_MASK) ==
                            GRF_WRITING_MODE_VERTICAL_LR)
                        log_x += (line->height - gi->height) / 2;
                }
            }
            else {
                ta = def_ta;
            }

            if (run->lrun->ort == GLYPH_ORIENT_SIDEWAYS_LEFT) {
                log_y += run->gstr->glyphs[j].width;
                if ((layout->rf & GRF_WRITING_MODE_MASK) ==
                        GRF_WRITING_MODE_VERTICAL_RL)
                    log_x -= line->height;
                else if ((layout->rf & GRF_WRITING_MODE_MASK) ==
                        GRF_WRITING_MODE_VERTICAL_LR)
                    log_x += line->height;
            }

            if (run->lrun->flags & LAYOUTRUN_FLAG_NO_SHAPING) {
                if (gi->width > 0 && bg_color) {
                // FIXME: draw background here
                }
            }
            else {
                SetTextAlign(hdc, ta);

                x_off = gi->x_off;
                y_off = gi->y_off;
                if (run->lrun->lf->rotation) {
                    _gdi_get_rotated_point(&x_off, &y_off,
                            run->lrun->lf->rotation);
                }

                DrawGlyph(hdc, x + log_x + x_off,
                               y + log_y + y_off, gi->gv, NULL, NULL);
                n++;
            }

            line_adv += gi->width;
        }
    }

    SelectFont(hdc, old_lf);
    return n;
}

#endif /* _MGCHARSET_UNICODE */
