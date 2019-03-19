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
** glyphruninfo.h: Internal interface related GLYPHRUNINFO.
**
** Create by WEI Yongming at 2019/03/15
*/

#ifndef _MG_NEWGDI_GLYPHRUNINFO_H
    #define _MG_NEWGDI_GLYPHRUNINFO_H

#include "list.h"

typedef Glyph32 (*CB_GET_GLYPH_INFO) (void* engine,
        void* glyph_infos, int index, int* cluster);

typedef BOOL (*CB_DESTROY_GLYPHS) (void* engine, void* glyph_infos);

typedef BOOL (*CB_DESTROY_ENGINE) (void* engine);

typedef struct _SHAPPINGENGINE {
    /* The pointer to the shaping engine */
    void*               engine;

    /* the callback reverse the glyphs */
    CB_REVERSE_ARRAY    reverse_glyphs;

    /* The callback returns the glyph value and cluster
       from a void glyph information */
    CB_GET_GLYPH_INFO   get_glyph_info;

    /* the callback destroy the shaped glyph info */
    CB_DESTROY_GLYPHS   destroy_glyphs;

    /* the callback destroy the shapping engine instance */
    CB_DESTROY_ENGINE   destroy_engine;
} SHAPPINGENGINE;

typedef struct _GLYPHRUN {
    struct list_head list;
    LOGFONT*    lf;     // the logfont for this run
    void*       gs;     // the shaped glyph information

    int         nr_gs;  // the number of shaped glyphs
    int         si;     // start index in the Unicode string
    int         nr_ucs; // the number of Unicode characters

    Uint32      lc:8;       // language code
    Uint32      st:8;       // script type
    Uint32      level:8;    // the bidi level
    Uint32      dir:4;      // the run direction
    Uint32      ort:2;      // the glyph orientation
} GLYPHRUN;

// NOTE: we arrange the fields carefully to avoid wasting space when
// we allocate an array of this structure due to the alignment.
typedef struct _MAPL2G {
    GLYPHRUN*       glyph_run;      // the glyph run this char belongs to.
    int             glyph_index;    // the index of the glyph in the run.
} MAPL2G;

typedef struct _UCHARCOLORMAP {
    struct list_head    list;
    int                 si;
    int                 len;
    RGBCOLOR            color;
} UCHARCOLORMAP;

struct _GLYPHRUNINFO {
    /* The following fields will be initialized by CreateGlyphRunInfo. */
    const Uchar32*      ucs;    // the uchars
    LOGFONT*            lf;     // the logfont specified

    UCHARCOLORMAP       cm_head;// the head of color map list of the characters
                                // change by calling SetPartColorInGlyphRunInfo
    struct list_head    run_head;   // glyph runs
                                // change by SetPartFontInGlyphRunInfo

    int             nr_ucs;     // number of uchars
    Uint32          lc:8;       // language code specified
    Uint32          st:8;       // script type specified
    Uint32          ort_base:3; // the glyph orientation specified
    Uint32          ort_rsv:3;  // the glyph orientation resolved
    Uint32          ort_plc:2;  // the glyph orientation policy specified
    Uint32          run_dir:4;  // the run direction specified
    Uint32          base_dir:1; // the paragraph direction; 0 for LTR, 1 for RTL
    Uint32          all_even:1; // flag indicating all level is even
    Uint32          all_odd:1;  // flag indicating all level is odd

    /* The following fields will be initialized by the shapping engine. */
    SHAPPINGENGINE  se;     // the shapping engine
    MAPL2G*         l2g;    // the logical character to glyph map
    GLYPHEXTINFO*   ges;    // the glyph extent information

    Uint32          rf;     // the rendering flags
};

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

RGBCOLOR __mg_glyphruns_get_color(const GLYPHRUNINFO* runinfo, int index);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // _MG_NEWGDI_GLYPHRUNINFO_H

