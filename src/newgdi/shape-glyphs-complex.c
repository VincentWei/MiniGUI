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
** shaped-glyph.c: The implementation of APIs related shaped-glyphs
**
** Reference:
**
** https://docs.microsoft.com/en-us/typography/opentype/spec/
**
** Create by WEI Yongming at 2019/03/06
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#if defined(_MGCHARSET_UNICODE) && defined(_MGCOMPLEX_SCRIPTS)

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "unicode-ops.h"
#include "textruns.h"
#include "layout.h"
#include "glyph.h"

#include <hb.h>
#include <hb-ft.h>

static BOOL shape_layout_run(SEInstance* inst,
        const TEXTRUNS* info, const LayoutRun* run,
        GlyphString* gs)
{
    BOOL ok = FALSE;
    int dfi;
    unsigned int i, nr_glyphs;
    FT_Face face = NULL;
    hb_buffer_t *hb_buf = NULL;
    hb_font_t *hb_font = NULL;
    hb_glyph_info_t *glyph_info;
    hb_glyph_position_t *glyph_pos;
    int last_cluster;

    face = (FT_Face)__mg_ft2_get_face(run->lf, run->ucs[0], &dfi);
    if (face == NULL) {
        _WRN_PRINTF("Can not get FT2 face object for layout run: %p\n", run);
        return FALSE;
    }

    hb_buf = hb_buffer_create();
    if (hb_buf == NULL)
        goto error;

    hb_buffer_set_content_type(hb_buf, HB_BUFFER_CONTENT_TYPE_UNICODE);
    for (i = 0; i < run->len; i++) {
        hb_buffer_add(hb_buf, run->ucs[i], i);
    }

    hb_buffer_set_direction(hb_buf, run->dir + HB_DIRECTION_LTR);
    hb_buffer_set_script(hb_buf, ScriptTypeToISO15924(run->st));
    hb_buffer_set_language(hb_buf,
            hb_language_from_string(LanguageCodeToISO639s1(run->lc), -1));

    hb_font = hb_ft_font_create(face, NULL);
    if (hb_font == NULL)
        goto error;

    hb_shape(hb_font, hb_buf, NULL, 0);

    glyph_info = hb_buffer_get_glyph_infos(hb_buf, &nr_glyphs);
    glyph_pos  = hb_buffer_get_glyph_positions(hb_buf, &nr_glyphs);
    if (glyph_info == NULL || glyph_pos == NULL || nr_glyphs == 0) {
        _ERR_PRINTF("%s: bad shaping result: glyph_info(%p), glyph_pos(%p), nr_glyphs(%d)\n",
            __FUNCTION__, glyph_info, glyph_pos, nr_glyphs);
        goto error;
    }

    // generate result
    gs->glyphs = malloc(sizeof(ShapedGlyph) * nr_glyphs);
    if (gs->glyphs == NULL) {
        goto error;
    }
    gs->log_clusters = malloc(sizeof(int) * nr_glyphs);
    if (gs->log_clusters == NULL) {
        free(gs->glyphs);
        goto error;
    }

    last_cluster = -1;
    gs->nr_glyphs = nr_glyphs;
    for (i = 0 ; i < nr_glyphs; i++) {

        Glyph32 gv;

        gv = SET_GLYPH_DFI(glyph_info[i].codepoint, dfi);
        gs->glyphs[i].gv = gv;
        gs->log_clusters[i] = glyph_info[i].cluster;

        if (run->flags & LAYOUTRUN_FLAG_CENTERED_BASELINE) {
            gs->glyphs[i].width = run->lf->size;
            gs->glyphs[i].height
                = _font_get_glyph_log_width(run->lf, gv);
        }
        else {
            gs->glyphs[i].width
                = _font_get_glyph_log_width(run->lf, gv);
            gs->glyphs[i].height = run->lf->size;
        }

        gs->glyphs[i].x_off = ((glyph_pos[i].x_offset + 0x8000) >> 16);
        gs->glyphs[i].y_off = ((glyph_pos[i].y_offset + 0x8000) >> 16);

        gs->glyphs[i].is_cluster_start = gs->log_clusters[i] != last_cluster;
        last_cluster = gs->log_clusters[i];
    }

    ok = TRUE;

error:

    if (hb_font)
        hb_font_destroy(hb_font);
    if (hb_buf)
        hb_buffer_destroy(hb_buf);

    return ok;
}

struct _SEInstance {
    const char* name;
    int ref_count;
};

static struct _SEInstance shaping_engine_complex = {
    "Complex Shapping Engine", 0
};

static BOOL destroy_instance(SEInstance* instance)
{
    if (instance == &shaping_engine_complex) {
        shaping_engine_complex.ref_count--;
        return TRUE;
    }

    _WRN_PRINTF("you are destroying a non-complex shaping engine instance.");
    return FALSE;
}

BOOL GUIAPI InitComplexShapingEngine(TEXTRUNS* truns)
{
    shaping_engine_complex.ref_count++;

    truns->sei.inst = &shaping_engine_complex;
    truns->sei.shape = shape_layout_run;
    truns->sei.free = destroy_instance;
    return TRUE;
}

#endif /* defined(_MGCHARSET_UNICODE) && defined(_MGCOMPLEX_SCRIPTS) */

