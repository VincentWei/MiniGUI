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
** gridview.c: an grid control
**
** Created by Peng Ke at 2004/11/29
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _GRIDVIEW_IMPL_H_
#define _GRIDVIEW_IMPL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

#ifdef _MGCTRL_GRIDVIEW
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"

#include "scrolled.h"

#define psvscr (&view->scrdata)
//#define GV_LEFTMARGIN           5
#define GV_RIGHTMARGIN          5
//#define GV_TOPMARGIN            5
#define GV_BOTTOMMARGIN         5

typedef enum _gvGridCellType
{
    GVCT_TABLEHEADER =   1,
    GVCT_IMPL_H_EADER,
    GVCT_TEXT,
    GVCT_NUMBER,
    GVCT_CHECKBOX,
    GVCT_SELECTION,
    GVCT_CUSTOM =   255
} gvGridCellType;

typedef struct _gvGridViewData gvGridViewData;
typedef struct _gvGridCellData gvGridCellData;

typedef struct _gvGridCellDataTableHeader
{
    int width;
    int height;
    char* text;
    BOOL col_first;
    gal_pixel color_fg;
    gal_pixel color_bg;
    PLOGFONT font;
    PBITMAP image;
    gvGridCellData* def_cell;
} gvGridCellDataTableHeader;

typedef struct _gvGridCellDataHeader
{
    int size;
    int end_xy;
    char* text;
    gal_pixel color_fg;
    gal_pixel color_bg;
    PLOGFONT font;
    PBITMAP image;
    gvGridCellData* def_cell;
} gvGridCellDataHeader;

typedef struct _gvGridCellDataText
{
    char* text;
    gal_pixel color_fg;
    gal_pixel color_bg;
    PLOGFONT font;
    PBITMAP image;
} gvGridCellDataText;

typedef struct _gvGridCellDataNumber
{
    double num;
    char* format;
    gal_pixel color_fg;
    gal_pixel color_bg;
    PLOGFONT font;
    PBITMAP image;
} gvGridCellDataNumber;

typedef struct _gvGridCellDataCheckBox
{
    HWND handle;
    BOOL dirty;
    BOOL checked;
    char* text;
    gal_pixel color_fg;
    gal_pixel color_bg;
    PLOGFONT font;
    PBITMAP image;
} gvGridCellDataCheckBox;

typedef struct _gvGridCellDataSelection
{
    HWND handle;
    BOOL dirty;
    int cur_index;
    char* text;
    gal_pixel color_fg;
    gal_pixel color_bg;
    PLOGFONT font;
    PBITMAP image;
} gvGridCellDataSelection;

#if 0
typedef struct _gvGridCellDataCustom
{
    DWORD dwAddData;
    FUNC* on_paint;
    FUNC* on_edit;
    FUNC* end_edit;
    FUNC* on_destroy;
} gvGridCellDataCustom;
#endif

typedef struct _GridCellFormat
{
    DWORD mask;
    DWORD text_format;
    gal_pixel color_fg;
    gal_pixel color_bg;
    gal_pixel color_border;
    gal_pixel hl_color_fg;
    gal_pixel hl_color_bg;
    gal_pixel hl_color_border;
    PLOGFONT font;
} GridCellFormat;

typedef int (*gvGridCellCallback_init)(gvGridCellData* cell);
typedef int (*gvGridCellCallback_merge)(gvGridCellData* cell, gvGridCellData* data);
typedef int (*gvGridCellCallback_copy)(gvGridCellData* to, gvGridCellData* from);
typedef void (*gvGridCellCallback_get_format)(gvGridCellData* cell, 
                                    HWND hWnd, HDC hdc, GridCellFormat* format);
typedef int (*gvGridCellCallback_draw)(gvGridCellData* cell, gvGridViewData* view,
                                        HDC hdc, int row, int col, RECT* rect);
typedef int (*gvGridCellCallback_draw_content)(gvGridCellData* cell, gvGridViewData* view,
                                        HDC hdc, int row, int col, RECT* rect, GridCellFormat* format);
typedef HWND (*gvGridCellCallback_get_control)(gvGridCellData* cell, gvGridViewData* view, int row, int col);
typedef int (*gvGridCellCallback_begin_edit)(gvGridCellData* cell, gvGridViewData* view,
                                        int row, int col, RECT* rect);
typedef int (*gvGridCellCallback_end_edit)(gvGridCellData* cell, gvGridViewData* view, int row, int col);
typedef int (*gvGridCellCallback_on_destroy)(gvGridCellData* cell);

typedef int (*gvGridCellCallback_get_text_value)(gvGridCellData* cell, int row, int col, char* buf, int buf_len);
typedef void (*gvGridCellCallback_set_text_value)(gvGridCellData* cell, int row, int col, char* text);
typedef double (*gvGridCellCallback_get_double_value)(gvGridCellData* cell, int row, int col);

struct _GridCellFunc
{
    gvGridCellCallback_init init;
    gvGridCellCallback_merge merge;
    gvGridCellCallback_copy copy;
    gvGridCellCallback_get_text_value get_text_value;
    gvGridCellCallback_set_text_value set_text_value;
    gvGridCellCallback_get_double_value get_double_value;
    gvGridCellCallback_get_control get_control;
    gvGridCellCallback_get_format get_format;
    gvGridCellCallback_draw draw;
    gvGridCellCallback_draw_content draw_content;
    gvGridCellCallback_draw invisible_draw;
    gvGridCellCallback_begin_edit begin_edit;
    gvGridCellCallback_end_edit end_edit;
    gvGridCellCallback_on_destroy on_destroy;
};
typedef struct _GridCellFunc GridCellFunc;

extern GridCellFunc* gvGridCellTypeNull;
extern GridCellFunc* gvGridCellTypeTableHeader;
extern GridCellFunc* gvGridCellTypeHeader;
extern GridCellFunc* gvGridCellTypeText;
extern GridCellFunc* gvGridCellTypeNumber;
extern GridCellFunc* gvGridCellTypeCheckBox;
extern GridCellFunc* gvGridCellTypeSelection;

struct _gvGridCellData
{
    GridCellFunc* func;
    DWORD mask;
    DWORD style;
    int celltype;
    union
    {
        gvGridCellDataTableHeader table;
        gvGridCellDataHeader header;
        gvGridCellDataText   text;
        gvGridCellDataNumber number;
        gvGridCellDataCheckBox checkbox;
        gvGridCellDataSelection selection;
//        gvGridCellDataCustom custom;
    } data;
};

struct _gvGridDataModal
{
    int (*get_col_count)(gvGridViewData* view);
    int (*get_row_count)(gvGridViewData* view);
    gvGridCellData* (*get_value_at)(gvGridViewData* view, int row, int col);
    int (*set_value_at)(gvGridViewData* view, int row, int col, gvGridCellData* data);
    gvGridCellData* (*get_value_or_default_at)(gvGridViewData* view, int row, int col);
    int (*add_new_row)(gvGridViewData* view, int index);
    int (*delete_row)(gvGridViewData* view, int index);
    int (*add_new_col)(gvGridViewData* view, int index);
    int (*delete_col)(gvGridViewData* view, int index);
    void* data;
};
typedef struct _gvGridDataModal gvGridDataModal;

gvGridDataModal* GridDataModalArray_new_modal(int row, int col);
void GridDataModalArray_delete_modal(gvGridDataModal* modal);

typedef struct _gvGridCellDepedence gvGridCellDepedence;
struct _gvGridCellDepedence
{
    int id;
    GRIDCELLS source;
    GRIDCELLS target;
    GRIDCELLEVALCALLBACK callback;
    DWORD dwAddData;
    gvGridCellDepedence* next;
};

int gvGridCellDep_add_dep(gvGridViewData* view, gvGridCellDepedence* dep);
int gvGridCellDep_delete_dep(gvGridViewData* view, int id);
int gvGridCellDep_update_dep(gvGridViewData* view, GRIDCELLS* cells);

struct _gvGridViewData
{
    /* scrollview object */
    SCRDATA scrdata;

    int ctrl_id;
    HWND hCtrl;
    HWND hEdit;

    GRIDCELLS highlight_cells;
    //GRIDCELLS selected_cells;
    
    int index_drag;
    int drag_x;
    int drag_y;
    
    int ContX;
    int ContY;

    int cur_cursor_id;
    int mouse_status;

    gvGridDataModal* modal;
    int depend_cur_id;
    gvGridCellDepedence* depend_list;

    // will delete, use selected_cells
    int current_row; // base 1? 
    int current_col; // base 1? 

    //will delete, use mouse_status
    //int is_mouse_down;
    //int is_drag;

    RECT cells_rect;
};

// mouse status mask 
enum
{
    MOUSE_DEFAULT,
    MOUSE_CELL_SELECT,
    MOUSE_ROW_SELECT,
    MOUSE_COLUMN_SELECT,
    MOUSE_ROW_RESIZE,
    MOUSE_COLUMN_RESIZE
};

typedef struct _GridCellEditData
{
    gvGridViewData* view;
    gvGridCellData* cell;
    int row;
    int col;
    int is_default;
    int commit;
    WNDPROC old_proc;
}GridCellEditData;

int gvGridCells_is_in_cells(GRIDCELLS* cells, int row, int col);
int gvGridCells_intersect(GRIDCELLS* first, GRIDCELLS* second);

BOOL RegisterGridViewControl (void);
#endif /* _MGCTRL_GRIDVIEW */

#endif /* _GRIDVIEW_IMPL_H_ */


