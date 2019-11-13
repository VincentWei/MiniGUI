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
 *   Copyright (C) 2002~2019, Beijing FMSoft Technologies Co., Ltd.
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
** layoutinfo.h: Internal interface related to LAYOUT.
**
** Create by WEI Yongming at 2019/03/20
*/

#ifndef _MG_NEWGDI_LAYOUT_H
    #define _MG_NEWGDI_LAYOUT_H

#include "list.h"
#include "textruns.h"

typedef struct _GlyphRun        GlyphRun;
typedef struct _ShapedGlyph     ShapedGlyph;

struct _ShapedGlyph {
    Glyph32 gv;
    int     x_off;
    int     y_off;

    Uint32  width:14;
    Uint32  height:14;
    Uint32  is_cluster_start:1;
    Uint32  hanged:2;
};

struct _GlyphString {
    ShapedGlyph*    glyphs;
    int*            log_clusters;

    int             nr_glyphs;
    unsigned int    space;
};

#define LAYOUTRUN_FLAG_NO_SHAPING           TEXTRUN_FLAG_NO_SHAPING
#define LAYOUTRUN_FLAG_UPRIGHT              TEXTRUN_FLAG_UPRIGHT
#define LAYOUTRUN_FLAG_ELLIPSIS             0x04
#define LAYOUTRUN_FLAG_CENTERED_BASELINE    0x08

struct _LayoutRun {
    LOGFONT*        lf;         // the logfont for this run
    const Uchar32*  ucs;        // the uchar string

    int             si;         // the start index of this run
    int             len;        // the length of the uchar string
    Uint32          lc:8;       // language code
    Uint32          st:8;       // script type
    Uint32          el:8;       // the bidi embedding level
    Uint32          dir:2;      // the run direction; value rage: [0, 3]
    Uint32          ort:2;      // the glyph orientation; value range: [0, 3]
    Uint32          flags:4;    // other flags
};

struct _GlyphRun {
    struct list_head    list;
    LayoutRun*          lrun;   // the layout run corresponding to the glyph run
    GlyphString*        gstr;   // the glyph string
};

typedef enum {
    LINE_DIRECTION_LTR,
    LINE_DIRECTION_RTL,
    LINE_DIRECTION_WEAK_LTR,
    LINE_DIRECTION_WEAK_RTL,
    LINE_DIRECTION_NEUTRAL
} LineDirection;

struct _LAYOUTLINE {
    struct list_head    list;
    LAYOUT*             layout;
    int*                log_widths; // the widths of the logical chars

    struct list_head    gruns;      // the list head for glyph runs

    int                 si;         // the start index in the uchar string
    int                 len;        // the length of line (number of uchars)
    int                 max_extent; // the max extent specified for this line
    int                 nr_runs;    // number of glyph/layout runs
    int                 width;      // actual width
    int                 height;     // actual height

    Uint32              resolved_dir:4;     // resolved direction of the line
    Uint32              is_paragraph_start:1;// is first line of the paragraph?
    Uint32              is_last_line:1;     // is the last line.
    Uint32              is_wrapped:1;       // is wrapped?
    Uint32              is_ellipsized:1;    // is ellipsized?
};

struct _LAYOUT {
    const TEXTRUNS*     truns;
    const BreakOppo*    bos;
    const int*          tabs;       // tabstop array
    int                 nr_tabs;    // number of tabstops

    Uint32              rf;         // rendering flags
    int                 ls;         // letter spacing
    int                 ws;         // word spacing
    int                 ts;         // tab size
    int                 max_ext;    // fixed maximal line extent
    int                 indent;     // indent value

    struct list_head    lines;      // the list head of lines

    LOGFONT*            lf_upright; // the default upright logfont
    int                 nr_left_ucs;// the number of chars not laied out
    int                 nr_lines;   // the number of lines

    Uint32              grv_base:4; // the base gravity
    Uint32              grv_plc:2;  // the gravity policy specified
    Uint32              persist:1;  // persist lines?
    Uint32              single_paragraph:1;
};

typedef struct _GlyphRunIter GlyphRunIter;

struct _GlyphRunIter {
    const GlyphRun *glyph_run;
    const Uchar32 *text;

    int start_glyph;
    int start_index;
    int start_char;

    int end_glyph;
    int end_index;
    int end_char;
};

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

LOGFONT* __mg_create_logfont_for_layout(const LAYOUT* layout,
        const char* fontname, GlyphOrient ort);
void __mg_release_logfont_for_layout(const LAYOUT* layout,
        const char* fontname, GlyphOrient ort);

GlyphRun *__mg_glyph_run_split (GlyphRun *orig, int split_index);
void __mg_glyph_run_free(GlyphRun* run);

LayoutRun* __mg_layout_run_new_ellipsis(const LAYOUT* layout,
        const TextRun* trun, const Uchar32* ucs, int nr_ucs);
LayoutRun* __mg_layout_run_new_from(const LAYOUT* layout,
        const TextRun* trun);
LayoutRun* __mg_layout_run_new_from_offset(const LAYOUT* layout,
        const TextRun* trun, int offset);

void __mg_layout_run_free(LayoutRun* run);

LayoutRun* __mg_layout_run_copy(const LayoutRun* run);
LayoutRun* __mg_layout_run_split(LayoutRun *orig, int split_index);

void __mg_layout_line_free_runs(LAYOUTLINE* line);

GlyphString* __mg_glyph_string_new(void);
void __mg_glyph_string_free(GlyphString* string);
void __mg_glyph_string_set_size(GlyphString* string, int new_len);
int __mg_glyph_string_get_width(const GlyphString* string);

BOOL __mg_glyph_run_iter_init_start (GlyphRunIter  *iter,
        const GlyphRun *glyph_run, const Uchar32 *text);

BOOL __mg_glyph_run_iter_init_end (GlyphRunIter *iter,
        const GlyphRun *glyph_run, const Uchar32 *text);

BOOL __mg_glyph_run_iter_prev_cluster (GlyphRunIter *iter);
BOOL __mg_glyph_run_iter_next_cluster (GlyphRunIter *iter);

void __mg_glyph_run_get_logical_widths(const GlyphRun* glyph_run,
        const Uchar32* ucs, int* log_widths);

void __mg_glyph_run_letter_space (const GlyphRun* glyph_run,
        const Uchar32* ucs, const BreakOppo* bos, int letter_spacing);

BOOL __mg_layout_line_ellipsize(LAYOUTLINE *line, int goal_width);

int __mg_shape_layout_run(const TEXTRUNS* info, const LayoutRun* run,
        GlyphString* glyphs);

void __mg_reverse_shaped_glyphs(ShapedGlyph* glyphs, int len);
void __mg_reverse_log_clusters(int* clusters, int len);

int __mg_layout_get_line_offset(const LAYOUT *layout,
        const LAYOUTLINE *line);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // _MG_NEWGDI_LAYOUT_H

