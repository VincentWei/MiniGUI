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
** textruns.h: Internal interface related to TEXTRUNS.
**
** Create by WEI Yongming at 2019/03/15
*/

#ifndef _MG_NEWGDI_TEXTRUNS_H
    #define _MG_NEWGDI_TEXTRUNS_H

#include "list.h"

typedef struct _LayoutRun       LayoutRun;
typedef struct _TextRun         TextRun;
typedef struct _GlyphString     GlyphString;
typedef struct _SEInstance      SEInstance;
typedef struct _TextColorMap    TextColorMap;

typedef BOOL (*CB_SHAPE_LAYOUT_RUN)(SEInstance* instance,
        const TEXTRUNS* info, const LayoutRun* run,
        GlyphString* gs);

typedef BOOL (*CB_DESTROY_INSTANCE)(SEInstance* instance);

typedef struct _ShapingEngineInfo ShapingEngineInfo;

struct _ShapingEngineInfo {
    /* The pointer to the shaping engine instance */
    SEInstance*         inst;

    /* callback to shap a text run */
    CB_SHAPE_LAYOUT_RUN shape;

    /* callback to destroy the shaping engine instance */
    CB_DESTROY_INSTANCE free;
};

#define TEXTRUN_FLAG_NO_SHAPING     0x01
#define TEXTRUN_FLAG_UPRIGHT        0x02

struct _TextRun {
    struct list_head list;
    char*       fontname;   // the logfont name for this run; NULL for default

    int         si;         // start index in the uchar string
    int         len;        // the length in uchars

    Uint32      lc:8;       // language code
    Uint32      st:8;       // script type
    Uint32      el:8;       // the bidi embedding level
    Uint32      flags:2;    // other flags
#if 0
    Uint32      dir:2;  // the run direction; value range: [0, 3]
    Uint32      ort:2;  // the glyph orientation; value range: [0, 3]
#endif
};

struct _TextColorMap {
    struct list_head    list;
    int                 si;
    int                 len;
    RGBCOLOR            value;  // attribute value
};

struct _TEXTRUNS {
    /* The following fields will be initialized by CreateGlyphRunInfo. */
    const Uchar32*      ucs;        // the uchars
    char*               fontname;   // the default logfont name specified

    TextColorMap        fg_colors;  // the head of foreground color map (list)
    TextColorMap        bg_colors;  // the head of background color map (list)
    struct list_head    truns;      // the head of text runs (list)

    int                 nr_ucs;     // number of uchars
    int                 nr_runs;    // number of runs
    LanguageCode        lc;         // language code specified
    ParagraphDir        base_dir;   // paragraph base direction

#if 0
    Uint32      lc:8;           // language code specified
    Uint32      grv_base:4;     // the gravity specified
    Uint32      grv_plc:2;      // the gravity policy specified
    Uint32      grv_rsv:4;      // the gravity resolved
    Uint32      base_level:1;   // the paragraph direction; 0 for LTR, 1 for RTL
#endif

    /* The following fields will be initialized by the shaping engine. */
    ShapingEngineInfo   sei;    // the shaping engine information
};

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

TextRun* __mg_text_run_copy(const TextRun *orig);
TextRun* __mg_text_run_split(TextRun *orig, int split_index);
TextRun* __mg_text_run_get_by_offset(TEXTRUNS* runinfo,
        int index, int *start_offset);
const TextRun* __mg_text_run_get_by_offset_const(const TEXTRUNS* runinfo,
        int index, int *start_offset);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // _MG_NEWGDI_TEXTRUNS_H

