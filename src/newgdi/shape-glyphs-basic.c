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
** shaped-glyph-basic.c: The implementation of basic shaping engine.
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

#ifdef _MGCHARSET_UNICODE

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "unicode-ops.h"
#include "layout.h"
#include "glyph.h"

/* Local array size, used for stack-based local arrays */

#if SIZEOF_PTR == 8
#   define LOCAL_ARRAY_SIZE 256
#else
#   define LOCAL_ARRAY_SIZE 128
#endif

static BOOL shape_layout_run(SEInstance* inst,
        const TEXTRUNS* info, const LayoutRun* run,
        GlyphString* gs)
{
    BOOL ok = FALSE;
    int i, j;
    const Uchar32* logical_ucs = run->ucs;
    int nr_ucs = run->len;

    Uchar32 local_shaped_ucs[LOCAL_ARRAY_SIZE];
    Uchar32* shaped_ucs = NULL;

    BidiArabicProp local_ar_props[LOCAL_ARRAY_SIZE];
    BidiArabicProp *ar_props = NULL;

    BidiType local_bidi_ts[LOCAL_ARRAY_SIZE];
    BidiType *bidi_ts = NULL;

    BidiLevel local_els[LOCAL_ARRAY_SIZE];
    BidiLevel* els = NULL;

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        shaped_ucs = local_shaped_ucs;
    else
        shaped_ucs = malloc(nr_ucs * sizeof(Uchar32));

    if (!shaped_ucs)
        goto out;

    /* TODO: compose logical character here */
    memcpy(shaped_ucs, logical_ucs, sizeof(Uchar32) * nr_ucs);

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        els = local_els;
    else
        els = (BidiLevel*)malloc (nr_ucs * sizeof(BidiLevel));
    if (!els)
        goto out;

    // Get the embedding levels from run instead calling
    // UBidiGetParagraphEmbeddingLevels
    memset(els, run->el, nr_ucs);

    if (run->st == SCRIPT_ARABIC &&
            !(run->flags & LAYOUTRUN_FLAG_CENTERED_BASELINE)) {

        if (nr_ucs < LOCAL_ARRAY_SIZE)
            bidi_ts = local_bidi_ts;
        else
            bidi_ts = malloc (nr_ucs * sizeof(BidiType));

        if (!bidi_ts)
            goto out;

        UStrGetBidiTypes(logical_ucs, nr_ucs, bidi_ts);

        /* Arabic joining */
        if (run->len < LOCAL_ARRAY_SIZE)
            ar_props = local_ar_props;
        else
            ar_props = malloc(sizeof(BidiArabicProp) * run->len);

        if (!ar_props)
            goto out;

        UStrGetJoiningTypes(logical_ucs, nr_ucs, ar_props);
        UBidiJoinArabic(bidi_ts, els, nr_ucs, ar_props);
        UBidiShape(BIDI_FLAG_SHAPE_MIRRORING | BIDI_FLAG_SHAPE_ARAB_PRES |
                BIDI_FLAG_SHAPE_ARAB_LIGA,
                els, nr_ucs, ar_props, shaped_ucs);

        if (bidi_ts && bidi_ts != local_bidi_ts) {
            free (bidi_ts);
            bidi_ts = NULL;
        }
    }
#if 0
    else if (BIDI_LEVEL_IS_RTL(run->el)) {
        UBidiShapeMirroring(els, nr_ucs, shaped_ucs);
    }
#endif

    if (els && els != local_els) {
        free (els);
        els = NULL;
    }

    // generate the glyphs
#if 0
    gs->glyphs = malloc(sizeof(ShapedGlyph) * nr_ucs);
    if (gs->glyphs == NULL) {
        goto out;
    }
    gs->log_clusters = malloc(sizeof(int) * nr_ucs);
    if (gs->log_clusters == NULL) {
        free(gs->glyphs);
        goto out;
    }
#else
    // must use __mg_glyph_string_set_size
    __mg_glyph_string_set_size(gs, nr_ucs);
#endif

    j = 0;
    for (i = 0; i < nr_ucs; i++) {
        Glyph32 gv;

        // check cluster for ligatures and marks
        if (ar_props) {
            BidiArabicProp ar_prop = ar_props[i];
            if (ar_prop & BIDI_MASK_LIGATURED) {
                if (j > 0) {
                    gs->glyphs[j-1].is_cluster_start = 1;
                }
                else {
                    _WRN_PRINTF("ligatured at the first glyph?\n");
                }
            }
            else {
                gv = GetGlyphValueAlt(run->lf, UCHAR2ACHAR(shaped_ucs[i]));
                gs->glyphs[j].gv = gv;
                gs->glyphs[j].is_cluster_start = 0;

                if (UCharIsArabicVowel(logical_ucs[i])) {
                    // adjust offset
                    gs->glyphs[j].x_off = 0;
                    gs->glyphs[j].y_off -= run->lf->ascent;
                    gs->glyphs[j].width = 0;
                }
                else {
                    if (run->flags & LAYOUTRUN_FLAG_CENTERED_BASELINE) {
                        gs->glyphs[j].width = run->lf->size;
                        gs->glyphs[j].height
                            = _font_get_glyph_log_width(run->lf, gv);
                        gs->glyphs[j].x_off = 0;
                        gs->glyphs[j].y_off = 0;
                    }
                    else {
                        gs->glyphs[j].x_off = 0;
                        gs->glyphs[j].y_off = 0;
                        gs->glyphs[j].width
                            = _font_get_glyph_log_width(run->lf, gv);
                        gs->glyphs[j].height = run->lf->size;
                    }
                }

                gs->log_clusters[j] = i;
                j++;
            }
        }
        else {
            gv = GetGlyphValueAlt(run->lf, UCHAR2ACHAR(shaped_ucs[i]));
            gs->glyphs[j].gv = gv;
            gs->glyphs[j].is_cluster_start = 0;

            if (run->flags & LAYOUTRUN_FLAG_CENTERED_BASELINE) {
                gs->glyphs[j].width = run->lf->size;
                gs->glyphs[j].height
                    = _font_get_glyph_log_width(run->lf, gv);
                gs->glyphs[j].x_off = 0;
                gs->glyphs[j].y_off = 0;
            }
            else {
                gs->glyphs[j].x_off = 0;
                gs->glyphs[j].y_off = 0;
                gs->glyphs[j].width
                    = _font_get_glyph_log_width(run->lf, gv);
                gs->glyphs[j].height = run->lf->size;
            }

            _DBG_PRINTF("%s: shaped uchar: %c, width: %d\n",
                    __FUNCTION__, shaped_ucs[i], gs->glyphs[j].width);

            gs->log_clusters[j] = i;
            j++;
        }

    }

    gs->nr_glyphs = j;

    if (ar_props && ar_props != local_ar_props) {
        free (ar_props);
        ar_props = NULL;
    }

    // reorder glyphs
    if (BIDI_LEVEL_IS_RTL(run->el) && gs->nr_glyphs > 1) {
        __mg_reverse_shaped_glyphs(gs->glyphs, gs->nr_glyphs);
        __mg_reverse_log_clusters(gs->log_clusters, gs->nr_glyphs);
    }

    ok = TRUE;

out:
    if (shaped_ucs && shaped_ucs != local_shaped_ucs)
        free (shaped_ucs);

    if (els && els != local_els)
        free (els);

    if (ar_props && ar_props != local_ar_props)
        free (ar_props);

    if (bidi_ts && bidi_ts != local_bidi_ts)
        free (bidi_ts);

    return ok;
}

struct _SEInstance {
    const char* name;
    int ref_count;
};

static struct _SEInstance shaping_engine_basic = {
    "Basic Shapping Engine", 0
};

static BOOL destroy_instance(SEInstance* instance)
{
    if (instance == &shaping_engine_basic) {
        shaping_engine_basic.ref_count--;
        return TRUE;
    }

    _WRN_PRINTF("You are destroying a non-basic shaping engine instance\n");
    return FALSE;
}

BOOL GUIAPI InitBasicShapingEngine(TEXTRUNS* truns)
{
    shaping_engine_basic.ref_count++;

    truns->sei.inst = &shaping_engine_basic;
    truns->sei.shape = shape_layout_run;
    truns->sei.free = destroy_instance;
    return TRUE;
}

#ifdef _MGDEVEL_MODE
int GetBasicShapingEngineRefCount(void)
{
    return shaping_engine_basic.ref_count;
}
#endif /* _MGDEVEL_MODE */

#endif /*  _MGCHARSET_UNICODE */

