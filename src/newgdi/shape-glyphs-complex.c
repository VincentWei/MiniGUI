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
#include "list.h"

#include <hb.h>
#include <hb-ft.h>

typedef struct _FtFontInfo {
    struct list_head    list;
    LOGFONT*            lf;
    ScriptType          st;
    FT_Face             face;
    hb_font_t*          hb_font;
    int                 dfi;
} FtFontInfo;

struct _SEInstance {
    int                 signature;
    int                 nr_fonts;
    struct list_head    cached_fonts;
};

static hb_font_t* get_hb_font_for_script(SEInstance* inst,
        LOGFONT* lf, ScriptType st, Uchar32 uc, int* dfi)
{
    struct list_head* f;
    FT_Face face;
    hb_font_t* hb_font;
    FtFontInfo *new_fi;
    FONT_RES* font_res;

    list_for_each(f, &inst->cached_fonts) {
        FtFontInfo *fi = (FtFontInfo*)f;

        if (fi->lf == lf && fi->st == st) {
            *dfi = fi->dfi;
            return fi->hb_font;
        }
    }

    face = (FT_Face)__mg_ft2_get_face(lf, uc, dfi);
    if (face == NULL) {
        _WRN_PRINTF("Cannot get FT2 face for logfont (%p) and uc (0x%x)\n",
            lf, uc);
        return NULL;
    }

    hb_font = hb_ft_font_create_referenced(face);
    //hb_font = hb_ft_font_create(face, NULL);
    if (hb_font == NULL)
        return NULL;

    new_fi = mg_slice_new(FtFontInfo);
    new_fi->lf = lf;
    new_fi->st = st;
    new_fi->face = face;
    new_fi->hb_font = hb_font;
    new_fi->dfi = *dfi;

    font_res = (FONT_RES*)lf;
    AddResRef(font_res->key);

    list_add_tail(&new_fi->list, &inst->cached_fonts);
    inst->nr_fonts++;
    return hb_font;
}

static SEInstance* create_instance(void)
{
    SEInstance* inst;

    inst = mg_slice_new(SEInstance);

    inst->signature = 0x77520388;
    inst->nr_fonts = 0;
    INIT_LIST_HEAD(&inst->cached_fonts);

    _DBG_PRINTF("%s: new complex shaping engine instance: %p\n",
        __FUNCTION__, inst);
    return inst;
}

static BOOL destroy_instance(SEInstance* inst)
{
    _DBG_PRINTF("%s: destroy complex shaping engine instance: %p\n",
        __FUNCTION__, inst);

    if (inst->signature != 0x77520388) {
        _WRN_PRINTF("you passed a non-complex shaping engine instance.");
        return FALSE;
    }

    while (!list_empty(&inst->cached_fonts)) {
        FONT_RES* font_res;

        FtFontInfo *fi = (FtFontInfo*)inst->cached_fonts.prev;
        list_del(inst->cached_fonts.prev);

        hb_font_destroy(fi->hb_font);

        font_res = (FONT_RES*)fi->lf;
        if (font_res->key) {
            int rc = ReleaseRes(font_res->key);
            _DBG_PRINTF("%s: ref count of logfont (%p): %d\n",
                __FUNCTION__, fi->lf, rc);
        }

        mg_slice_delete(FtFontInfo, fi);
        inst->nr_fonts--;
    }

    assert(inst->nr_fonts == 0);

    mg_slice_delete(SEInstance, inst);
    return TRUE;
}

static BOOL shape_layout_run(SEInstance* inst,
        const TEXTRUNS* info, const LayoutRun* run,
        GlyphString* gs)
{
    BOOL ok = FALSE;
    int dfi;
    unsigned int i, nr_glyphs;
    hb_buffer_t *hb_buf = NULL;
    hb_font_t *hb_font = NULL;
    hb_glyph_info_t *glyph_info;
    hb_glyph_position_t *glyph_pos;
    int last_cluster;

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

    hb_font = get_hb_font_for_script(inst, run->lf, run->st, run->ucs[0], &dfi);
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

    if (hb_buf)
        hb_buffer_destroy(hb_buf);

    return ok;
}

BOOL GUIAPI InitComplexShapingEngine(TEXTRUNS* truns)
{
    truns->sei.inst = create_instance();
    truns->sei.shape = shape_layout_run;
    truns->sei.free = destroy_instance;
    return TRUE;
}

#endif /* defined(_MGCHARSET_UNICODE) && defined(_MGCOMPLEX_SCRIPTS) */

