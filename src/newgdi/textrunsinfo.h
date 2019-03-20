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
** textrunsinfo.h: Internal interface related to TEXTRUNSINFO.
**
** Create by WEI Yongming at 2019/03/15
*/

#ifndef _MG_NEWGDI_TEXTRUNSINFO_H
    #define _MG_NEWGDI_TEXTRUNSINFO_H

#include "list.h"

typedef struct _TEXTRUN         TEXTRUN;
typedef struct _GLYPHSTRING     GLYPHSTRING;
typedef struct _SEINSTANCE      SEINSTANCE;
typedef struct _TEXTCOLORMAP    TEXTCOLORMAP;

typedef BOOL (*CB_SHAPE_TEXT_RUN)(SEINSTANCE* instance,
        const TEXTRUNSINFO* info, const TEXTRUN* run,
        GLYPHSTRING* gs);

typedef BOOL (*CB_DESTROY_INSTANCE)(SEINSTANCE* instance);

typedef struct _SHAPINGENGINEINFO SHAPINGENGINEINFO;

struct _SHAPINGENGINEINFO {
    /* The pointer to the shaping engine instance */
    SEINSTANCE*         inst;

    /* callback to shap a text run */
    CB_SHAPE_TEXT_RUN   shape;

    /* callback to destroy the shaping engine instance */
    CB_DESTROY_INSTANCE free;
};

struct _TEXTRUN {
    struct list_head list;
    LOGFONT*    lf;     // the logfont for this run

    int         idx;    // start index in the Unicode string
    int         len;    // the number of Unicode characters

    Uint32      lc:8;   // language code
    Uint32      st:8;   // script type
    Uint32      el:8;   // the bidi embedding level
    Uint32      dir:4;  // the run direction
    Uint32      ort:2;  // the glyph orientation
};

struct _TEXTCOLORMAP {
    struct list_head    list;
    int                 si;
    int                 len;
    RGBCOLOR            color;
};

struct _TEXTRUNSINFO {
    /* The following fields will be initialized by CreateGlyphRunInfo. */
    const Uchar32*      ucs;    // the uchars
    char*               fontname;// the logfont name specified

    TEXTCOLORMAP        cm_head; // the head of color map list of the characters
    struct list_head    run_head;// glyph runs (list)

    int         nr_ucs;         // number of uchars
    int         nr_runs;        // number of runs
    Uint32      lc:8;           // language code specified
    Uint32      ort_base:3;     // the glyph orientation specified
    Uint32      ort_rsv:3;      // the glyph orientation resolved
    Uint32      ort_plc:2;      // the glyph orientation policy specified
    Uint32      run_dir:4;      // the run direction specified
    Uint32      base_level:1;   // the paragraph direction; 0 for LTR, 1 for RTL

    /* The following fields will be initialized by the shaping engine. */
    SHAPINGENGINEINFO   sei;    // the shaping engine information
};

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

RGBCOLOR __mg_textruns_get_color(const TEXTRUNSINFO* runinfo, int index);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // _MG_NEWGDI_TEXTRUNSINFO_H

