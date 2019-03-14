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
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "unicode-ops.h"

#ifdef _MGCHARSET_UNICODE

#if 0
static void bidi_reverse_shaped_glyphs (void* context, int len, int pos)
{
    int i;
    SHAPEDGLYPH* vgs = (SHAPEDGLYPH*)context + pos;

    for (i = 0; i < len / 2; i++) {
        SHAPEDGLYPH tmp = vgs[i];
        vgs[i] = vgs[len - 1 - i];
        vgs[len - 1 - i] = tmp;
    }
}
#endif

/* Local array size, used for stack-based local arrays */

#if SIZEOF_PTR == 8
#   define LOCAL_ARRAY_SIZE 256
#else
#   define LOCAL_ARRAY_SIZE 128
#endif

int GUIAPI GetShapedGlyphsBasic(LOGFONT* logfont,
        LanguageCode lang_code, UCharScriptType writing_system,
        const Uchar32* ucs, int nr_ucs,
        const BidiLevel embedding_levels, BidiType base_dir,
        SHAPEDGLYPHS* shaped_glyphs)
{
#if 0
    int ret_value = 0;
    int i, j, nr_reordered;
    BidiLevel max_level = 0;

    Uchar32 local_visual_ucs[LOCAL_ARRAY_SIZE];
    Uchar32* visual_ucs = NULL;

    BidiArabicProp local_ar_props[LOCAL_ARRAY_SIZE];
    BidiArabicProp *ar_props = NULL;

    BidiType local_bidi_ts[LOCAL_ARRAY_SIZE];
    BidiType *bidi_ts = NULL;

    BidiBracketType local_brk_ts[LOCAL_ARRAY_SIZE];
    BidiBracketType *brk_ts = NULL;

    BidiLevel local_els[LOCAL_ARRAY_SIZE];
    BidiLevel* els = NULL;

    int local_indics_map[LOCAL_ARRAY_SIZE];
    int* indics_map = NULL;

    if (nr_ucs == 0 || logical_ucs == NULL || break_oppos == NULL ||
            visual_glyphs == NULL || nr_glyphs == NULL) {
        return 0;
    }

    /* TODO: we may re-use visual_glyphs for visual ucs */
    if (nr_ucs < LOCAL_ARRAY_SIZE)
        visual_ucs = local_visual_ucs;
    else
        visual_ucs = malloc (nr_ucs * sizeof(Uchar32));

    if (!visual_ucs)
        goto out;

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        indics_map = local_indics_map;
    else
        indics_map = malloc (nr_ucs * sizeof(int));

    if (!indics_map)
        goto out;

    /* TODO: compose logical character here */
    memcpy(visual_ucs, logical_ucs, sizeof(Uchar32) * nr_ucs);

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        bidi_ts = local_bidi_ts;
    else
        bidi_ts = malloc (nr_ucs * sizeof(BidiType));

    if (!bidi_ts)
        goto out;

    UStrGetBidiTypes(logical_ucs, nr_ucs, bidi_ts);

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        brk_ts = local_brk_ts;
    else
        brk_ts = (BidiBracketType*)malloc (nr_ucs * sizeof(BidiBracketType));

    if (!brk_ts)
        goto out;

    UStrGetBracketTypes (logical_ucs, bidi_ts, nr_ucs, brk_ts);

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        els = local_els;
    else
        els = (BidiLevel*)malloc (nr_ucs * sizeof(BidiLevel));

    if (!els)
        goto out;

    max_level = UBidiGetParagraphEmbeddingLevels(bidi_ts, brk_ts, nr_ucs,
            paragraph_dir, els) - 1;
    if (max_level < 0)
        goto out;

    /* Set up the ordering array to identity order */
    for (i = 0; i < nr_ucs; i++)
        indics_map[i] = i;

    if (writing_system == UCHAR_SCRIPT_ARABIC) {
        /* Arabic joining */
        if (nr_ucs < LOCAL_ARRAY_SIZE)
            ar_props = local_ar_props;
        else
            ar_props = malloc(sizeof(BidiArabicProp) * nr_ucs);

        if (!ar_props)
            goto out;

        UStrGetJoiningTypes(logical_ucs, nr_ucs, ar_props);
        UBidiJoinArabic(bidi_ts, els, nr_ucs, ar_props);
        UBidiShape(BIDI_FLAG_SHAPE_MIRRORING | BIDI_FLAG_SHAPE_ARAB_PRES |
                BIDI_FLAG_SHAPE_ARAB_LIGA,
                els, nr_ucs, ar_props, visual_ucs);
    }

    if (UBidiReorderLine(BIDI_FLAGS_DEFAULT, bidi_ts, nr_ucs, 0,
                *paragraph_dir, els, visual_ucs, indics_map, NULL, NULL) == 0)
        goto out;

    // remove the unused characters and get the real length of the visual ucs.
    j = 0;
    for (i = 0; i < nr_ucs; i++) {
        if (!BIDI_IS_EXPLICIT_OR_BN (bidi_ts[indics_map[i]])) {
            visual_ucs[j] = visual_ucs[i];
            j++;
        }
    }
    nr_reordered = j;

    if (break_oppos) {
        // The breaking opportunities
        if (UStrGetBreaks(writing_system, ctr, wbr, lbp, visual_ucs, nr_reordered,
                &break_oppos) == 0)
            goto out;
    }

    // get shaped glyphs
    for (i = 0; i < nr_reordered; i++) {
        visual_glyphs[i] = GetGlyphValue(logfont, UCHAR2ACHAR(visual_ucs[i]));

        if (ar_props) {
            BidiArabicProp ar_prop = ar_props[indics_map[i]];
            if (ar_prop & BIDI_MASK_LIGATURED) {
                glyph_shaping_info[i].gt = GLYPH_TYPE_STDLIGATURE;
                glyph_shaping_info[i].gp = GLYPH_POS_BASELINE;
            }
            else if (UCharIsArabicVowel(visual_ucs[i])) {
                glyph_shaping_info[i].gt = GLYPH_TYPE_STDMARK;
                glyph_shaping_info[i].gp = GLYPH_POS_STDMARK_ABOVE;
            }
            else {
                glyph_shaping_info[i].gt = GLYPH_TYPE_STANDALONE;
                glyph_shaping_info[i].gp = GLYPH_POS_BASELINE;
            }
        }
        else {
            glyph_shaping_info[i].gt = GLYPH_TYPE_STANDALONE;
            glyph_shaping_info[i].gp = GLYPH_POS_BASELINE;
        }
    }

    *nr_glyphs = nr_reordered;

    if (map_v2l) {
        memcpy(map_v2l, indics_map, sizeof(int) * nr_reordered);
    }

    ret_value = nr_ucs;

out:
    if (indics_map && indics_map != local_indics_map)
        free (indics_map);

    if (visual_ucs && visual_ucs != local_visual_ucs)
        free (visual_ucs);

    if (els && els != local_els)
        free (els);

    if (ar_props && ar_props != local_ar_props)
        free (ar_props);

    if (bidi_ts && bidi_ts != local_bidi_ts)
        free (bidi_ts);

    if (brk_ts && brk_ts != local_brk_ts)
        free (brk_ts);

    return ret_value;
#endif
    return 0;
}

int GUIAPI GetShapedGlyphsComplex(LOGFONT* logfont,
        LanguageCode lang_code, UCharScriptType writing_system,
        Uint32 render_flags, const Uchar32* ucs, int nr_ucs,
        const BidiLevel embedding_levels, BidiType base_dir,
        SHAPEDGLYPHS* shaped_glyphs)
{
    return 0;
}

int GUIAPI GetGlyphsExtentInfo(LOGFONT* logfont,
        LanguageCode lang_code, UCharScriptType writing_system,
        Uint32 render_flags, const Uchar32* ucs, int nr_ucs,
        const BidiLevel embedding_levels, BidiType base_dir,
        const SHAPEDGLYPHS* shaped_glyphs,
        GLYPHEXTINFO* glyph_ext_info, LOGFONT** logfont_sideways)
{
    return 0;
}

int GUIAPI GetGlyphsPositionInfo(
        LOGFONT* logfont_upright, LOGFONT* logfont_sideways,
        const Uchar32* ucs, int nr_ucs,
        const Uint16* break_oppos,
        const BidiLevel embedding_levels, BidiType base_dir,
        const SHAPEDGLYPHS* shaped_glyphs,
        Uint32 render_flags, int x, int y,
        int letter_spacing, int word_spacing, int tab_size, int max_extent,
        GLYPHEXTINFO* glyph_ext_info, SIZE* line_size, GLYPHPOS* glyph_pos)
{
    return 0;
}

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

#endif /*  _MGCHARSET_UNICODE */

