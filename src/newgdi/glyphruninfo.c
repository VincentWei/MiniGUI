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

GLYPHRUNINFO* GUIAPI CreateGlyphRunInfo(
        const char* lang_tag, const char* script_tag,
        Uint8 ctr, Uint8 wbr, Uint8 lbp,
        Uchar32* ucs, int nr_ucs, ParagraphDir base_dir)
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

    if (!bidi_ts)
        goto out;

    UStrGetBidiTypes(ucs, nr_ucs, bidi_ts);

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        brk_ts = local_brk_ts;
    else
        brk_ts = (BidiBracketType*)malloc (nr_ucs * sizeof(BidiBracketType));

    if (!brk_ts)
        goto out;

    UStrGetBracketTypes (ucs, bidi_ts, nr_ucs, brk_ts);

    els = (BidiLevel*)malloc (nr_ucs * sizeof(BidiLevel));
    if (!els) {
        goto out;
    }

    max_level = UBidiGetParagraphEmbeddingLevels(bidi_ts, brk_ts, nr_ucs,
            &base_dir, els) - 1;
    if (max_level < 0) {
        goto out;
    }

    // remove the bidi marks from embedding levels and original string.
    j = 0;
    for (i = 0; i < nr_ucs; i++) {
        if (!BIDI_IS_EXPLICIT_OR_BN (bidi_ts[i])) {
            els[j] = els[i];
            ucs[j] = ucs[i];
            j++;
        }
    }
    nr_ucs = j;

    // Calculate the breaking opportunities
    if (UStrGetBreaks(script_type, ctr, wbr, lbp,
            ucs, nr_ucs, &bos) == 0) {
        goto out;
    }

    runinfo->ucs = ucs;
    runinfo->nr_ucs = nr_ucs;
    runinfo->els = els;
    runinfo->bos = bos;
    runinfo->base_dir = base_dir;
    runinfo->lang_tag = lang_tag;
    runinfo->script_tag = script_tag;
    ok = TRUE;

out:
    if (bidi_ts && bidi_ts != local_bidi_ts)
        free (bidi_ts);

    if (brk_ts && brk_ts != local_brk_ts)
        free (brk_ts);

    if (ok)
        return runinfo;

    if (runinfo->runs) free(runinfo->runs);
    if (els) free(els);
    if (bos) free(bos);
    free(runinfo);

    return NULL;
}

BOOL GUIAPI ResetGlyphRunInfo(GLYPHRUNINFO* run_info)
{
    return FALSE;
}

BOOL GUIAPI DestroyGlyphRunInfo(GLYPHRUNINFO* run_info)
{
    return FALSE;
}

#endif /*  _MGCHARSET_UNICODE */

