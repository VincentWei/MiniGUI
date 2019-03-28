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

#ifdef _MGCHARSET_UNICODE

BOOL DrawShapedGlyph(HDC hdc, LOGFONT* lf, RGBCOLOR color,
        Glyph32 gv, const GLYPHPOS* pos)
{
    return FALSE;
}

int DrawShapedGlyphLine(HDC hdc, const LAYOUTLINE* line,
        int x, int y)
{
    return 0;
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
