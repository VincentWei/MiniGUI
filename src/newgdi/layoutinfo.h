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
    SHAPEDGLYPH*  glyphs;
    int*        log_clusters;

    int         nr_glyphs;
    int         reserved;
};

struct _GLYPHRUN {
    struct list_head    list;
    TEXTRUN*            text_run;   // the text run to which this glyph run belongs
    GLYPHSTRING*        glyphstr;   // the glyph string
};

struct _LAYOUTLINE {
    struct list_head    list;
    LAYOUTINFO*         layout_info;
    int*                log_widths; // the widths of the logical chars

    struct list_head    run_head;   // the list head for glyph runs

    int                 idx;        // the index in uchar string
    int                 len;        // the length in uchar string

    Uint8               first_line:1;   // is first line of the paragraph?
    Uint8               resolved_dir:3; // resolved direction of the line
    Uint8               all_even:1; // flag indicating all level is even
    Uint8               all_odd:1;  // flag indicating all level is odd
};

struct _GLYPHLAYOUTINFO {
    const TEXTRUNSINFO* runinfo;
    const BreakOppo*    break_oppos;

    Uint32              render_flags;
    int                 letter_spacing;
    int                 word_spacing;
    int                 tab_size;

    struct list_head    line_head;  // the list head of lines

    LAYOUTLINE          *curr_line; // current line
    int                 left_ucs;   // the number of chars not laied out.
};

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // _MG_NEWGDI_LAYOUTINFO_H

