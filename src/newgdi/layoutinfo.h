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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

/*
** layoutinfo.h: Internal interface related to LAYOUTINFO.
**
** Create by WEI Yongming at 2019/03/20
*/

#ifndef _MG_NEWGDI_LAYOUTINFO_H
    #define _MG_NEWGDI_LAYOUTINFO_H

#include "list.h"
#include "textrunsinfo.h"

typedef struct _GlyphRun        GlyphRun;
typedef struct _ShapedGlyph     ShapedGlyph;

struct _ShapedGlyph {
    Glyph32 gv;
    int     x_off;
    int     y_off;
    int     width;

    Uint32  is_cluster_start:1;
};

struct _GlyphString {
    ShapedGlyph*    glyphs;
    int*            log_clusters;

    int             nr_glyphs;
    unsigned int    space;
};

#define LAYOUTRUN_FLAG_ORPHAN   0x01

struct _LayoutRun {
    LOGFONT*        lf;     // the logfont for this run
    const Uchar32*  ucs;    // the uchar string

    int             si;     // the start index of this run
    int             len;    // the length of the uchar string
    Uint32          lc:8;   // language code
    Uint32          st:8;   // script type
    Uint32          el:8;   // the bidi embedding level
    Uint32          dir:4;  // the run direction
    Uint32          ort:2;  // the glyph orientation
    Uint32          flags:2;// other flags
};

struct _GlyphRun {
    struct list_head    list;
    LayoutRun*          lrun;   // the layout run corresponding to the glyph run
    GlyphString*        gstr;   // the glyph string
};

struct _LAYOUTLINE {
    struct list_head    list;
    LAYOUTINFO*         layout;
    int*                log_widths; // the widths of the logical chars

    struct list_head    gruns;      // the list head for glyph runs

    int                 si;         // the start index in the uchar string
    int                 len;        // the length of line (number of uchars)

    int                 nr_runs;
    Uint8               is_paragraph_start:1;// is first line of the paragraph?
    Uint8               resolved_dir:3; // resolved direction of the line
};

struct _LAYOUTINFO {
    const TEXTRUNSINFO* truninfo;
    const BreakOppo*    bos;
    const int*          tabs;       // tabstop array

    Uint32              rf;         // rendering flags
    int                 ls;         // letter spacing
    int                 ws;         // word spacing
    int                 ts;         // tab size

    struct list_head    lines;      // the list head of lines

    int                 nr_tabs;    // number of tabstops
    int                 nr_left_ucs;// the number of chars not laied out
    int                 nr_lines;   // the number of lines

    Uint32              persist:1;  // persist lines?
    Uint32              single_paragraph:1;
    Uint32              is_wrapped:1;
    Uint32              is_ellipsized:1;
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

LOGFONT* __mg_create_logfont_for_layout(const LAYOUTINFO* layout,
        const char* fontname, GlyphOrient ort);
void __mg_release_logfont_for_layout(const LAYOUTINFO* layout,
        const char* fontname, GlyphOrient ort);

GlyphRun *__mg_glyph_run_split (GlyphRun *orig, int split_index);
void __mg_glyph_run_free(GlyphRun* run);

LayoutRun* __mg_layout_run_new_orphan(const LAYOUTINFO* layout,
        const TextRun* trun, const Uchar32* ucs, int nr_ucs);
LayoutRun* __mg_layout_run_new_from(const LAYOUTINFO* layout,
        const TextRun* trun);
LayoutRun* __mg_layout_run_new_from_offset(const LAYOUTINFO* layout,
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

int __mg_shape_layout_run(const TEXTRUNSINFO* info, const LayoutRun* run,
        GlyphString* glyphs);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // _MG_NEWGDI_LAYOUTINFO_H

