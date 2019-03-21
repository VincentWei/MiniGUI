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

typedef struct _GLYPHRUN        GLYPHRUN;
typedef struct _SHAPEDGLYPH     SHAPEDGLYPH;

struct _SHAPEDGLYPH {
    Glyph32 gv;
    int     x_off;
    int     y_off;
    int     width;

    Uint32  is_cluster_start:1;
};

struct _GLYPHSTRING {
    SHAPEDGLYPH*    glyphs;
    int*            log_clusters;

    int             nr_glyphs;
    unsigned int    space;
};

struct _GLYPHRUN {
    struct list_head    list;
    TEXTRUN*            text;   // the text run to which this glyph run belongs
    GLYPHSTRING*        gs;     // the glyph string
    int                 offset; // the offset of the first uchar in the text run
    int                 nr_ucs; // the number of the uchars
};

struct _LAYOUTLINE {
    struct list_head    list;
    LAYOUTINFO*         layout;
    int*                log_widths; // the widths of the logical chars

    struct list_head    run_head;   // the list head for glyph runs

    int                 idx;        // the index in the uchar string
    int                 nr_ucs;     // the number of the uchars

    Uint8               first_line:1;   // is first line of the paragraph?
    Uint8               resolved_dir:3; // resolved direction of the line
    Uint8               all_even:1; // flag indicating all level is even
    Uint8               all_odd:1;  // flag indicating all level is odd
};

struct _LAYOUTINFO {
    const TEXTRUNSINFO* runinfo;
    const BreakOppo*    bos;

    Uint32              rf;
    int                 ls;
    int                 ws;
    int                 ts;

    struct list_head    line_head;  // the list head of lines

    LAYOUTLINE          *curr_line; // current line

    int                 left_ucs;   // the number of chars not laied out.
    Uint32              persist:1;  // persist lines?
    Uint32              single_paragraph:1;
};

typedef struct _GlyphItem GlyphItem;
typedef struct _GlyphItemIter GlyphItemIter;

struct _GlyphItem {
    TEXTRUN       *item;
    GLYPHSTRING   *glyphs;
};

struct _GlyphItemIter {
    const GlyphItem *glyph_item;
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

GLYPHSTRING* __mg_glyph_string_new(void);
void __mg_glyph_string_free(GLYPHSTRING* string);
void __mg_glyph_string_set_size(GLYPHSTRING* string, int new_len);
int __mg_glyph_string_get_width(const GLYPHSTRING* string);

BOOL __mg_glyph_item_iter_init_start (GlyphItemIter  *iter,
        const GlyphItem *glyph_item, const Uchar32 *text);

BOOL __mg_glyph_item_iter_init_end (GlyphItemIter *iter,
        const GlyphItem *glyph_item, const Uchar32 *text);

BOOL __mg_glyph_item_iter_prev_cluster (GlyphItemIter *iter);
BOOL __mg_glyph_item_iter_next_cluster (GlyphItemIter *iter);

void __mg_glyph_item_get_logical_widths(const GlyphItem* glyph_item,
            const Uchar32* ucs, int* log_widths);

void __mg_glyph_item_letter_space (const GlyphItem* glyph_item,
            const Uchar32* ucs, const BreakOppo* bos, int letter_spacing);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // _MG_NEWGDI_LAYOUTINFO_H

