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

#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "ctrl/gridview.h"
#include "scrolled.h"
#include "gridview_impl.h"

static void update_viewport (gvGridViewData *view, int from, int to, int is_row)
{
    RECT r;
    
    GetClientRect (view->hCtrl, &r);

    if (is_row) {        
        r.top = from;
        ScrollWindow (view->hCtrl, 0, to, &r, NULL);
    } 
    else {
        r.left = from;
        ScrollWindow (view->hCtrl, to, 0, &r, NULL);
    }
    UpdateWindow(view->hCtrl, FALSE);
}

// scroll function {{{

static BOOL gridview_make_rect_visible(gvGridViewData* view, RECT* rect)
{
    RECT r;
    RECT r_client;
    gvGridCellData* table;
    int head_w, head_h;
    table = view->modal->get_value_at(view, 0, 0);
    head_w = table->data.table.width;
    head_h = table->data.table.height;
    GetClientRect(view->hCtrl, &r_client);
    r = *rect;
    scrolled_window_to_content(&view->scrdata, &r.right, &r.bottom);
    scrolled_window_to_content(&view->scrdata, &r.left, &r.top);
    r.left -= head_w;
    r.top -= head_h;
    if(RECTW(r) > RECTW(r_client))
        r.right = r.left + RECTW(r_client);
    if(RECTH(r) > RECTH(r_client))
        r.bottom = r.top + RECTH(r_client);
    scrolled_make_pos_visible(view->hCtrl, &view->scrdata, r.right-1, r.bottom-1);
    scrolled_make_pos_visible(view->hCtrl, &view->scrdata, r.left, r.top);
    return TRUE;
}

void gridview_set_content(HWND hWnd, PSCRDATA pscrdata, BOOL visChanged)
{
    gvGridViewData* view = NULL;
    int w, h;
    
    view = (gvGridViewData*) GetWindowAdditionalData2(hWnd);
    
    w = view->ContX - pscrdata->nContX;
    h = view->ContY - pscrdata->nContY;
    
    view->ContX = pscrdata->nContX;
    view->ContY = pscrdata->nContY;
    
    if (w != 0) 
        update_viewport (view, view->cells_rect.left, w, 0);

    if (h != 0)
        update_viewport (view, view->cells_rect.top, h, 1);
}

// }}}

// static functions for row/column and xy {{{
static void update_row_col_end_xy(gvGridViewData* view, int count, int header_size, int is_row)
{
    gvGridCellData* cell;
    int cur_end_xy = header_size;
    int i;

    for(i=1; i<=count; i++)
    {
        if (is_row)
            cell = view->modal->get_value_at(view, i, 0);
        else
            cell = view->modal->get_value_at(view, 0, i);
        //MGU_Assert(cell != NULL);
        if (cell->data.header.size < 0) {
            cell->data.header.size = header_size;
            cell->data.header.end_xy = cur_end_xy + header_size;
        }
        else
            cell->data.header.end_xy = cur_end_xy + cell->data.header.size;
        cur_end_xy = cell->data.header.end_xy;
    }
    if (is_row)
        scrolled_set_cont_height(view->hCtrl, &view->scrdata, cur_end_xy);
    else
        scrolled_set_cont_width(view->hCtrl, &view->scrdata, cur_end_xy);
}

static void update_row_col_size(gvGridViewData* view)
{
    gvGridCellData* cell;
    int row, col;
    row = view->modal->get_row_count(view);
    col = view->modal->get_col_count(view);

    cell = view->modal->get_value_at(view, 0, 0);
    //MGU_Assert(cell != NULL);
    if(cell->data.table.width <= 0)
        cell->data.table.width = 40;
    if(cell->data.table.height <= 0)
        cell->data.table.height = 20;

    GetClientRect(view->hCtrl, &view->cells_rect);
    view->cells_rect.left += cell->data.table.width;
    view->cells_rect.top += cell->data.table.height;
    if(view->cells_rect.left > view->cells_rect.right)
        view->cells_rect.right = view->cells_rect.left;
    if(view->cells_rect.top > view->cells_rect.bottom)
        view->cells_rect.bottom = view->cells_rect.top;

    scrolled_set_visible (view->hCtrl, &view->scrdata);
    
    update_row_col_end_xy(view, row, cell->data.table.height, 1);
    update_row_col_end_xy(view, col, cell->data.table.width, 0);

}

static void get_cell_rect(gvGridViewData* view, int row, int col, RECT* r)
{
    int row_count, col_count;
    gvGridCellData* cell;
    if(row<0 || col<0)
        return;
    row_count = view->modal->get_row_count(view);
    col_count = view->modal->get_col_count(view);
    if(row>row_count || col>col_count)
        return;
    switch(row)
    {
    case 0:
        r->top = 0;
        cell = view->modal->get_value_at(view, 0, 0);
        r->bottom = cell->data.table.height;
        break;
    case 1:
        cell = view->modal->get_value_at(view, 0, 0);
        r->top = cell->data.table.height;
        cell = view->modal->get_value_at(view, row, 0);
        r->bottom = cell->data.header.end_xy;
        break;
    default:
        cell = view->modal->get_value_at(view, row-1, 0);
        r->top = cell->data.header.end_xy;
        cell = view->modal->get_value_at(view, row, 0);
        r->bottom = cell->data.header.end_xy;
        break;
    }
    switch(col)
    {
    case 0:
        r->left = 0;
        cell = view->modal->get_value_at(view, 0, 0);
        r->right = cell->data.table.width;
        break;
    case 1:
        cell = view->modal->get_value_at(view, 0, 0);
        r->left = cell->data.table.width;
        cell = view->modal->get_value_at(view, 0, col);
        r->right = cell->data.header.end_xy;
        break;
    default:
        cell = view->modal->get_value_at(view, 0, col-1);
        r->left = cell->data.header.end_xy;
        cell = view->modal->get_value_at(view, 0, col);
        r->right = cell->data.header.end_xy;
        break;
    }
    // scroll rect
    {
        RECT r_tmp;
        memcpy(&r_tmp, r, sizeof(RECT));
        scrolled_content_to_window(&view->scrdata, &r_tmp.left, &r_tmp.top);
        scrolled_content_to_window(&view->scrdata, &r_tmp.right, &r_tmp.bottom);
        if(row!=0)
        {
            r->top = r_tmp.top;
            r->bottom = r_tmp.bottom;
        }
        if(col!=0)
        {
            r->left = r_tmp.left;
            r->right = r_tmp.right;
        }
    }
}

//TODO add scroll support
static void get_cell_from_xy(gvGridViewData* view, int x, int y, int* row, int* col)
{
    int i;
    int row_count, col_count;
    gvGridCellData* table;
    gvGridCellData* cell;
    row_count = view->modal->get_row_count(view);
    col_count = view->modal->get_col_count(view);
    table = view->modal->get_value_at(view, 0, 0);
    *row = -1;
    *col = -1;
    if(y < table->data.table.height)
        *row = 0;
    else
    {
        for(i=1; i<=row_count; i++)
        {
            cell = view->modal->get_value_at(view, i, 0);
            //if(y < cell->data.header.end_xy)
            if(y < (cell->data.header.end_xy - psvscr->nContY))
            {
                *row = i;
                break;
            }
        }
    }
    if(x < table->data.table.width)
        *col = 0;
    else
    {
        for(i=1; i<=col_count; i++)
        {
            cell = view->modal->get_value_at(view, 0, i);
            //if(x < cell->data.header.end_xy)
            if(x < (cell->data.header.end_xy - psvscr->nContX))
            {
                *col = i;
                break;
            }
        }
    }
}
// }}}

// grid cells functions {{{

static int gvGridCells_get_start_stop(int begin, int width, int* start, int* stop)
{
    if(width < -1)
    {
        begin = begin + width + 1;
        width = - width;
    }
    *start = begin;
    *stop = begin + width;
    return 1;
}

int gvGridCells_is_in_cells(GRIDCELLS* cells, int row, int col)
{
    int start, end;
    /* header not select*/
    if(row == 0 || col == 0)
        return FALSE;
    /* null select*/
    if(cells->width == 0 || cells->height == 0)
        return FALSE;
    /* row region*/
    gvGridCells_get_start_stop(cells->row, cells->height, &start, &end);
    if(start <= end && !(row >= start && row < end))
        return FALSE;
    /* col region*/
    gvGridCells_get_start_stop(cells->column, cells->width, &start, &end);
    if(start <= end && !(col >= start && col < end))
        return FALSE;

    return TRUE;
}

static int gvGridCells_lineseg_intersect(int begin, int width, int begin2, int width2)
{
    int start, stop;
    int start2, stop2;
    gvGridCells_get_start_stop(begin, width, &start, &stop);
    gvGridCells_get_start_stop(begin2, width2, &start2, &stop2);
    /* has null select*/
    if(start == stop || start2 == stop2)
        return FALSE;
    /* has a all select*/
    if(start > stop || start2 > stop2)
        return TRUE;
    if(start >= stop2 || stop <= start2)
        return FALSE;

    return TRUE;
}

int gvGridCells_intersect(GRIDCELLS* first, GRIDCELLS* second)
{
    if(!gvGridCells_lineseg_intersect(first->row, first->height, second->row, second->height))
        return FALSE;
    if(!gvGridCells_lineseg_intersect(first->column, first->width, second->column, second->width))
        return FALSE;
    return TRUE;
}

static void gvGridCells_get_cells_rect(gvGridViewData* view, GRIDCELLS* cells, RECT* rect)
{
    RECT rc_client;
    RECT r;
    int row, col, row_end, col_end;
    if(cells->height == 0 || cells->width == 0)
    {
        SetRectEmpty(rect);
        return;
    }
    GetClientRect(view->hCtrl, &rc_client);
    get_cell_rect(view, 0, 0, &r);
    rc_client.left = r.right;
    rc_client.top = r.bottom;
    gvGridCells_get_start_stop(cells->row, cells->height, &row, &row_end);
    gvGridCells_get_start_stop(cells->column, cells->width, &col, &col_end);
    if(row > row_end) /* select col*/
    {
        rect->top = rc_client.top;
        rect->bottom = rc_client.bottom;
    }
    else
    {
        get_cell_rect(view, row, 0, &r);
        rect->top = r.top;
        if(row != row_end-1)
            get_cell_rect(view, row_end-1, 0, &r);
        rect->bottom = r.bottom;
    }
    if(col > col_end)
    {
        rect->left = rc_client.left;
        rect->right = rc_client.right;
    }
    else
    {
        get_cell_rect(view, 0, col, &r);
        rect->left = r.left;
        if(col != col_end-1)
            get_cell_rect(view, 0, col_end-1, &r);
        rect->right = r.right;
    }
}

// }}}

// set_cell_value {{{

static int set_cell_value(gvGridViewData* view, GRIDCELLDATA* data, int row, int col)
{
    int need_set_value = FALSE;
    gvGridCellData* cell = view->modal->get_value_at(view, row, col);
    gvGridCellData newcell;
    gvGridCellData celldata; 
    if(cell == NULL)
    {
        cell = &newcell;
        need_set_value = TRUE;
    }
    switch (data->style & GV_TYPE_MASK)
    {
        case GV_TYPE_HEADER:
        {
            char *buff = NULL;
            GRIDCELLDATAHEADER* header;
            if(need_set_value)
                gvGridCellTypeHeader->init(cell);
            gvGridCellTypeHeader->init(&celldata);
            celldata.mask = data->mask;
            celldata.style |= data->style;
            celldata.data.header.end_xy = -1;
            celldata.data.header.color_bg = data->color_bg;
            celldata.data.header.color_fg = data->color_fg;
            celldata.data.header.image = data->image;
            celldata.data.header.font = data->font;
            header = (GRIDCELLDATAHEADER*)data->content;
            celldata.data.header.size = header->size;
            if (header->buff != NULL) {
                int len;
                len = MIN(strlen(header->buff), header->len_buff);
                buff = (char *) malloc (sizeof(char)*(len + 1));
                strncpy(buff, header->buff, len);
                buff[len] = '\0';
            }
            celldata.data.header.text = buff;
            cell->func->merge(cell, &celldata);
            if (buff != NULL)
                free (buff);
            break;
        }
        case GV_TYPE_TEXT:
        {
            char *buff = NULL;
            GRIDCELLDATATEXT* text;
            if(need_set_value)
                gvGridCellTypeText->init(cell);
            gvGridCellTypeText->init(&celldata);
            celldata.mask = data->mask;
            celldata.style |= data->style;
            celldata.data.text.color_bg = data->color_bg;
            celldata.data.text.color_fg = data->color_fg;
            celldata.data.text.image = data->image;
            celldata.data.text.font = data->font;
            text = (GRIDCELLDATATEXT*)data->content;
            if (text->buff != NULL) {
                int len;
                len = MIN(strlen(text->buff), text->len_buff);
                buff = (char *) malloc (sizeof(char)*(len + 1));
                strncpy (buff, text->buff, len);
                buff[len] = '\0';
            }
            celldata.data.text.text = buff;
            cell->func->merge(cell, &celldata);
            if (buff != NULL)
                free (buff);
            
            break;
        }
        case GV_TYPE_NUMBER:
        {
            char *buff = NULL;
            GRIDCELLDATANUMBER* number;
            if(need_set_value)
                gvGridCellTypeNumber->init(cell);
            gvGridCellTypeNumber->init(&celldata);
            celldata.mask = data->mask;
            celldata.style |= data->style;
            celldata.data.number.color_bg = data->color_bg;
            celldata.data.number.color_fg = data->color_fg;
            celldata.data.number.image = data->image;
            celldata.data.number.font = data->font;
            number = (GRIDCELLDATANUMBER*)data->content;           
            celldata.data.number.num = number->number;
            if (number->format != NULL) {
                int len;
                len = MIN(strlen(number->format), number->len_format);
                buff = (char *) malloc (sizeof(char)*(len + 1));
                strncpy (buff, number->format, len);
                buff[len] = '\0';
            }
            celldata.data.number.format = buff;
            cell->func->merge(cell, &celldata);
            if (buff != NULL)
                free (buff);
            
            break;
        }
        case GV_TYPE_CHECKBOX:
        {
            char *buff = NULL;
            GRIDCELLDATACHECKBOX* checkbox;
            if(need_set_value)
                gvGridCellTypeCheckBox->init(cell);
            gvGridCellTypeCheckBox->init(&celldata);
            celldata.mask = data->mask;
            celldata.style |= data->style;
            celldata.data.checkbox.color_bg = data->color_bg;
            celldata.data.checkbox.color_fg = data->color_fg;
            celldata.data.checkbox.image = data->image;
            celldata.data.checkbox.font = data->font;
            checkbox = (GRIDCELLDATACHECKBOX*) data->content;
            celldata.data.checkbox.checked = checkbox->checked;            
            if (checkbox->text != NULL) {
                int len;
                len = MIN(strlen(checkbox->text), checkbox->len_text);
                buff = (char *) malloc (sizeof(char)*(len + 1));
                strncpy (buff, checkbox->text, len);
                buff[len] = '\0';
            }
            celldata.data.checkbox.text = buff;
            cell->func->merge(cell, &celldata);
            if (buff != NULL)
                free (buff);
            
            break;
        }
        case GV_TYPE_SELECTION:
        {
            char *buff = NULL;
            GRIDCELLDATASELECTION* selection;
            if(need_set_value)
                gvGridCellTypeSelection->init(cell);
            gvGridCellTypeSelection->init(&celldata);
            celldata.mask = data->mask;
            celldata.style |= data->style;
            celldata.data.selection.color_bg = data->color_bg;
            celldata.data.selection.color_fg = data->color_fg;
            celldata.data.selection.image = data->image;
            celldata.data.selection.font = data->font;
            selection = (GRIDCELLDATASELECTION*) data->content;
            celldata.data.selection.cur_index = selection->cur_index;
            if (selection->selections != NULL) {
                int len;
                len = MIN(strlen(selection->selections), selection->len_sel);
                buff = (char *) malloc (sizeof(char)*(len + 1));
                strncpy (buff, selection->selections, len);
                buff[len] = '\0';
            }
            celldata.data.selection.text = buff;
            cell->func->merge(cell, &celldata);
            if (buff != NULL)
                free (buff);
            
            break;
        }
        default:
            return GRID_ERR;
    }
//    cell->func->merge(cell, &celldata);
    if(need_set_value)
        view->modal->set_value_at(view, row, col, cell);
    return GRID_OKAY;
}

// }}}

// fetch_cell_value {{{

static int fetch_cell_value(gvGridViewData* view, GRIDCELLDATA* data, int row, int col)
{
    gvGridCellData* cell;
    GRIDCELLDATAHEADER* header;
    GRIDCELLDATATEXT* text;
    GRIDCELLDATANUMBER* number;
    GRIDCELLDATASELECTION* selection;
    GRIDCELLDATACHECKBOX* checkbox;
    cell = view->modal->get_value_at(view, row, col);
    if ((data->style & GV_TYPE_MASK) == GV_TYPE_HEADER)
    {
        if (cell->celltype != GV_TYPE_HEADER)
            return GRID_ERR;
        header = (GRIDCELLDATAHEADER*)data->content;
        if (header == NULL)
        {
            data->style = cell->style;
            data->mask = cell->mask;
            return GRID_OKAY;
        }
        if ((data->style & GV_TYPE_MASK) != GV_TYPE_HEADER)
            return GRID_ERR;
        if (header->buff == NULL)
        {
            if (cell->data.header.text != NULL)
                header->len_buff = strlen(cell->data.header.text);
            else
                header->len_buff = 0;
            return GRID_OKAY; 
        }
        if (header->len_buff != 0)
        {
            data->style = cell->style;
            data->mask = cell->mask;
            strncpy(header->buff , 
                    cell->data.header.text,
                    header->len_buff);
            header->buff[header->len_buff] = '\0';
            header->size  = cell->data.header.size;
            data->color_bg = cell->data.header.color_bg;
            data->color_fg = cell->data.header.color_fg;
            header->len_buff = strlen(cell->data.header.text);
            return GRID_OKAY; 
        }
    }
    else if ((data->style & GV_TYPE_MASK) == GV_TYPE_TEXT)
    {
        if (cell->celltype != GV_TYPE_TEXT)
            return GRID_ERR;
        text = (GRIDCELLDATATEXT*)data->content;
        if (text == NULL)
        {
            data->style = cell->style;
            data->mask = cell->mask;
            return GRID_OKAY;
        }
        if (text->buff == NULL)
        {
            if (cell->data.text.text != NULL)
                text->len_buff = strlen(cell->data.text.text);
            else
                text->len_buff = 0;
            return GRID_OKAY; 
        }
        if (text->len_buff != 0)
        {
            strncpy(text->buff , 
                    cell->data.text.text,
                    text->len_buff);
            text->buff[text->len_buff] = '\0';
            data->style = cell->style;
            data->mask = cell->mask;
            data->color_bg = cell->data.text.color_bg;
            data->color_fg = cell->data.text.color_fg;
            text->len_buff = strlen(cell->data.text.text);
            return GRID_OKAY; 
        }
    }
    else if((data->style & GV_TYPE_MASK) == GV_TYPE_NUMBER)
    {
        if (cell->celltype != GV_TYPE_NUMBER)
            return GRID_ERR;
        number = (GRIDCELLDATANUMBER*)data->content;
        if (number == NULL)
        {
            data->style = cell->style;
            data->mask = cell->mask;
            return GRID_OKAY;
        }
        number->number = cell->data.number.num;
        if (number->format == NULL)
        {
            if (cell->data.number.format != NULL)
                number->len_format = strlen(cell->data.number.format);
            else
                number->len_format = 0;
            return GRID_OKAY; 
        }
        if (number->len_format > 0)
        {
            strncpy(number->format, 
                    cell->data.number.format,
                    number->len_format);
            number->format[number->len_format] = '\0';
            data->style = cell->style;
            data->mask = cell->mask;
            number->len_format = strlen(cell->data.number.format);
            return GRID_OKAY;
        }
    }
    else if((data->style & GV_TYPE_MASK) == GV_TYPE_SELECTION)
    {
        if (cell->celltype != GV_TYPE_SELECTION)
            return GRID_ERR;
        selection = (GRIDCELLDATASELECTION*)data->content;
        if (selection == NULL)
        {
            data->style = cell->style;
            data->mask = cell->mask;
            return GRID_OKAY;
        }
        if (selection->selections == NULL)
        {
            if (cell->data.selection.text != NULL)
                selection->len_sel = strlen(cell->data.selection.text);
            else
                selection->len_sel = 0;
            return GRID_OKAY; 
        }
        if (selection->len_sel != 0)
        {
            strncpy(selection->selections, 
                    cell->data.selection.text,
                    selection->len_sel);
            selection->selections[selection->len_sel] = '\0';
            data->style = cell->style;
            data->mask = cell->mask;
            data->color_bg = cell->data.selection.color_bg;
            data->color_fg = cell->data.selection.color_fg;
            selection->cur_index = cell->data.selection.cur_index;
            selection->len_sel = strlen(cell->data.selection.text);
            return GRID_OKAY; 
        }
    }
    else if((data->style & GV_TYPE_MASK) == GV_TYPE_CHECKBOX)
    {
        if (cell->celltype != GV_TYPE_CHECKBOX)
            return GRID_ERR;
        checkbox = (GRIDCELLDATACHECKBOX*)data->content;
        if (checkbox == NULL)
        {
            data->style = cell->style;
            data->mask = cell->mask;
            return GRID_OKAY;
        }
        if (checkbox->text == NULL)
        {
            if (cell->data.checkbox.text != NULL)
                checkbox->len_text = strlen(cell->data.checkbox.text);
            else
                checkbox->len_text = 0;
            return GRID_OKAY; 
        }
        if (checkbox->len_text != 0)
        {
            strncpy(checkbox->text, 
                    cell->data.checkbox.text,
                    checkbox->len_text);
            checkbox->text[checkbox->len_text] = '\0';
            checkbox->checked = cell->data.checkbox.checked;
            data->style = cell->style;
            data->mask = cell->mask;
            checkbox->len_text = strlen(cell->data.checkbox.text);
            return GRID_OKAY; 
        }
    }
    return GRID_ERR;
}

// }}}

// set highlight & current cells {{{

static void set_highlight_cells(gvGridViewData* view, int row, int col, int width, int height)
{
    RECT rect;
    gvGridCells_get_cells_rect(view, &view->highlight_cells, &rect);
    InvalidateRect(view->hCtrl, &rect, FALSE);

    view->highlight_cells.row = row;
    view->highlight_cells.column = col;
    view->highlight_cells.width = width;
    view->highlight_cells.height = height;
    gvGridCells_get_cells_rect(view, &view->highlight_cells, &rect);
    InvalidateRect(view->hCtrl, &rect, FALSE);

//    gridview_make_rect_visible(view, &rect);
}

static void set_current_cell(gvGridViewData* view, int row, int col)
{
    RECT rect;
    int count;
    if(row < 1) row = 1;
    if(col < 1) col = 1;
    count = view->modal->get_row_count(view);
    if(row >= count) row = count;
    count = view->modal->get_col_count(view);
    if(col >= count) col = count;
    if(view->current_col != col || view->current_row != row)
    {
        get_cell_rect(view, view->current_row, view->current_col, &rect);
        InvalidateRect (view->hCtrl, &rect, FALSE);
        get_cell_rect(view, row, col, &rect);
        InvalidateRect (view->hCtrl, &rect, FALSE);
        
        view->current_col = col;
        view->current_row = row;

        gridview_make_rect_visible(view, &rect);
        NotifyParent(view->hCtrl, view->ctrl_id, GRIDN_FOCUSCHANGED);
    }
}

static void set_highlight_cells_to(gvGridViewData* view, int row, int col)
{
    int x;
    int r, c, w, h;
    int row_count, col_count;

    row_count = view->modal->get_row_count(view);
    col_count = view->modal->get_col_count(view);

    if(row <= 0 || col <= 0 || row > row_count || col > col_count)
        return;
    
    r = view->highlight_cells.row;
    c = view->highlight_cells.column;

    x = (row>=r)?1:-1;
    h = row - r + x;
    x = (col>=c)?1:-1;
    w = col - c + x;
    set_highlight_cells(view, r, c, w, h);
    set_current_cell(view, row, col);
}

// }}}

// dependence operation {{{

static int gvGridCellDep_add_dep_to_list(gvGridViewData* view, gvGridCellDepedence* dep)
{
    gvGridCellDepedence *new_dep;
    new_dep = malloc(sizeof(gvGridCellDepedence));
    memcpy(new_dep, dep, sizeof(gvGridCellDepedence)); 
    new_dep->next = NULL;
    view->depend_cur_id++;
    new_dep->id = view->depend_cur_id;
    if(view->depend_list == NULL)
        view->depend_list = new_dep;
    else
    {
        gvGridCellDepedence* list = view->depend_list;
        while(list->next != NULL)
            list = list->next;
        list->next = new_dep;
    }
    return new_dep->id;
}

int gvGridCellDep_add_dep(gvGridViewData* view, gvGridCellDepedence* dep)
{
    int ret;
    gvGridCellDepedence* head = view->depend_list;
    if (gvGridCells_intersect(&dep->source, &dep->target))
        return -1;
    while(head != NULL)
    {
        if (gvGridCells_intersect(&head->target, &dep->target))
            return -1;
        head = head->next;
    }
    ret = gvGridCellDep_add_dep_to_list(view, dep);
    gvGridCellDep_update_dep(view, &dep->source);
    return ret;
}

int gvGridCellDep_delete_dep(gvGridViewData* view, int id)
{
    int ret = -1;
    gvGridCellDepedence *prev_dep;
    gvGridCellDepedence *cur_dep;
    if(view->depend_list == NULL)
        return -1;
    cur_dep = view->depend_list;
    if (cur_dep->id == id)
    {
        view->depend_list = cur_dep->next;
        free(cur_dep);
        return id;
    }
    prev_dep = view->depend_list;
    while(prev_dep != NULL)
    {
        cur_dep = prev_dep->next;
        if (cur_dep != NULL && cur_dep->id == id)
        {
            prev_dep->next = cur_dep->next;
            free(cur_dep);
            ret = id;
            break;
        }
        prev_dep = cur_dep;
    }
    return ret;
}

int gvGridCellDep_update_dep(gvGridViewData* view, GRIDCELLS* cells)
{
    gvGridCellDepedence* dep = view->depend_list;
    while (dep)
    {
        if (gvGridCells_intersect(&dep->source, cells))
            dep->callback(&dep->target, &dep->source, dep->dwAddData);
        dep = dep->next;
    }
    return 0;
}

// }}}

// draw cell {{{

static BOOL gridview_is_pt_in_hwnd_rect(HWND hParent, HWND hWnd, int x, int y)
{
    RECT r;
    HWND hChild = 0;

    GetWindowRect(hWnd, &r);

    if(PtInRect(&r, x, y))
        return TRUE;

    ClientToScreen(hParent, &x, &y);
    ScreenToClient(hWnd, &x, &y);

    while ((hChild = GetNextChild(hWnd, hChild)) != 0)
    {
        GetWindowRect(hChild, &r);
        if(PtInRect(&r, x, y))
            return TRUE;
    }
    return FALSE;
}

static BOOL gridview_is_pt_in_control(gvGridViewData* view, int x, int y)
{
    int row, col;
    gvGridCellData* cell;
    get_cell_from_xy(view, x, y, &row, &col);
    cell = view->modal->get_value_at(view, row, col);
    if(cell != NULL)
    {
        HWND ctrl = cell->func->get_control(cell, view, row, col);
        if(ctrl != HWND_INVALID && gridview_is_pt_in_hwnd_rect(view->hCtrl, ctrl, x, y))
            return TRUE;
    }
    return FALSE;
}

static BOOL is_cell_visible(gvGridViewData* view, gvGridCellData* cell, HDC hdc, RECT* r)
{
    if(cell == NULL || cell->func->invisible_draw == NULL)
        return RectVisible(hdc, r);
    return DoesIntersect(r, &view->cells_rect);
}

static void draw_cell(gvGridViewData* view, HWND hWnd, HDC hdc, int row, int col)
{
    RECT r;
    gvGridCellData* cell;

    cell = view->modal->get_value_or_default_at(view, row, col);
    get_cell_rect(view, row, col, &r);
    if(cell == NULL)
        return;

    if(is_cell_visible(view, cell, hdc, &r))
    {
        cell->func->draw(cell, view, hdc, row, col, &r);
        if(row==0 || col==0)
            ExcludeClipRect(hdc, &r);
    }
    else if(cell->func->invisible_draw != NULL)
        cell->func->invisible_draw(cell, view, hdc, row, col, &r);
}

// }}}

// mouse status event {{{

static int change_mouse_status(gvGridViewData* view, int x, int y)
{
    int row, col;
    RECT r;
    get_cell_from_xy(view, x, y, &row, &col);
    get_cell_rect(view, row, col, &r);
    if (row == 0)
    {
        view->mouse_status = MOUSE_COLUMN_SELECT;
        if (col < 1)
            return 0;
        if (x >= r.right -2)
        {
            view->mouse_status = MOUSE_COLUMN_RESIZE;
            view->index_drag = col;
        }
    }
    else if (col == 0)
    {
        view->mouse_status = MOUSE_ROW_SELECT;
        if (row < 1)
            return 0;
        if (y >= r.bottom -2)
        {
            view->mouse_status = MOUSE_ROW_RESIZE;
            view->index_drag = row;
        }
    }
    else
        view->mouse_status = MOUSE_CELL_SELECT;
    return 0;
}

static int select_cell_mouse_down(gvGridViewData* view, int x, int y)
{
    int row, col;
    get_cell_from_xy(view, x, y, &row, &col);
    set_highlight_cells(view, row, col, 1, 1);
    set_current_cell(view, row, col);
    NotifyParent(view->hCtrl, view->ctrl_id, GRIDN_CELLCLK);
    return 0;
}

static int select_head_mouse_down(gvGridViewData* view, int x, int y, int isrow)
{
    int row, col;
    get_cell_from_xy(view, x, y, &row, &col);
    if (isrow) {
        set_highlight_cells(view, row, col, -1, 1);
        set_current_cell(view, row, 1);
    }
    else {
        set_highlight_cells(view, row, col, 1, -1);
        set_current_cell(view, 1, col);
    }
    NotifyParent(view->hCtrl, view->ctrl_id, GRIDN_HEADLDOWN);
    return 0;
}

static int select_cell_mouse_up(gvGridViewData* view, int x, int y)
{
    return 0;
}

static int select_row_mouse_up(gvGridViewData* view, int x, int y)
{
    NotifyParent(view->hCtrl, view->ctrl_id, GRIDN_HEADLUP);
    return 0;
}

static int select_col_mouse_up(gvGridViewData* view, int x, int y)
{
    NotifyParent(view->hCtrl, view->ctrl_id, GRIDN_HEADLUP);
    return 0;
}

static int select_cell_mouse_move(gvGridViewData* view, int x, int y)
{
    int row, col, r1, c1;
    RECT rect;
    
    get_cell_from_xy(view, x, y, &row, &col);
    get_cell_rect (view, 0, 0, &rect);

    if (row == 0) {
        get_cell_from_xy(view, x, y + RECTH(rect), &r1, &c1);
        if (r1 != 1)
            row = r1 -1;            
    }
    if (col == 0) {
        get_cell_from_xy(view, x + RECTW(rect), y, &r1, &c1);
        if (c1 != 1)
            col = c1 -1;
    }
    set_highlight_cells_to(view, row, col);
    return 0;
}

static int select_row_mouse_move(gvGridViewData* view, int x, int y)
{
    int row, col;
    int begin_row, begin_col;
    int height;
    RECT rect;
    
    get_cell_from_xy(view, x, y, &row, &col);
    if (row < 0 || col < 0) 
        return -1;
    get_cell_rect (view, 0, 0, &rect);
    
    if(row == 0) {
        row = 1;
    }
    begin_row = view->highlight_cells.row;
    begin_col = view->highlight_cells.column;
    y = (row >= begin_row)?1:-1;
    height = row - begin_row + y;
    set_highlight_cells(view, begin_row, begin_col, -1, height);
    set_current_cell(view, row, 1);
    return 0;
}

static int select_col_mouse_move(gvGridViewData* view, int x, int y)
{
    int row, col;
    int begin_row, begin_col;
    int width;
    RECT rect;
    
    get_cell_from_xy(view, x, y, &row, &col);
    if (row < 0 || col < 0) 
        return -1;
    get_cell_rect (view, 0, 0, &rect);
    if(col == 0)
        col = 1;
    begin_row = view->highlight_cells.row;
    begin_col = view->highlight_cells.column;
    x = (col >= begin_col)?1:-1;
    width = col - begin_col + x;
    set_highlight_cells(view, begin_row, begin_col, width, -1);
    set_current_cell (view, 1, col);
    return 0;
}

static int resize_row_mouse_down(gvGridViewData* view, int x, int y)
{
    HDC hdc;
    RECT r;
    int oldrop;

    hdc = GetClientDC (view->hCtrl);
    oldrop = SetRasterOperation (hdc, ROP_XOR);
    GetClientRect (view->hCtrl, &r);
    SetPenColor (hdc, PIXEL_lightwhite);
    LineEx (hdc, 0, y, RECTW(r), y);
    SetRasterOperation (hdc, oldrop);
    ReleaseDC (hdc);
    
    view->drag_y = y;
    return 0;
}

static int resize_col_mouse_down(gvGridViewData* view, int x, int y)
{
    HDC hdc;
    RECT r;
    int oldrop;

    hdc = GetClientDC (view->hCtrl);
    oldrop = SetRasterOperation (hdc, ROP_XOR);
    GetClientRect (view->hCtrl, &r);
    SetPenColor (hdc, PIXEL_lightwhite);
    LineEx (hdc, x, 0, x, RECTW(r));
    SetRasterOperation (hdc, oldrop);
    ReleaseDC (hdc);
    
    view->drag_x = x;
    return 0;
}

static int resize_row_mouse_up(gvGridViewData* view, int x, int y)
{   
    HDC hdc;
    RECT r;
    RECT rect;
    int oldrop;

    hdc = GetClientDC (view->hCtrl);
    oldrop = SetRasterOperation (hdc, ROP_XOR);
    GetClientRect (view->hCtrl, &r);
    SetPenColor (hdc, PIXEL_lightwhite);
    LineEx (hdc, 0, view->drag_y, RECTW(r), view->drag_y);
    SetRasterOperation (hdc, oldrop);
    ReleaseDC (hdc);
    
    if (y < view->drag_y) y = view->drag_y;
    
    get_cell_rect (view, view->index_drag, 0, &rect);
    SendMessage (view->hCtrl, GRIDM_SETROWHEIGHT, view->index_drag, y-rect.top+2);
    
    return 0;
}

/*
static BOOL find_checkbox(gvGridViewData* view)
{
    gvGridCellData* cell;
    int row_count;
    row_count = view->modal->get_row_count(view);
    while (row_count > 0)
    {
        cell = view->modal->get_value_at(view, row_count, view->index_drag);
        if (cell != NULL && (cell->celltype == GV_TYPE_CHECKBOX))
            return TRUE;
        row_count --;
    }
    return FALSE;
}
*/

static int resize_col_mouse_up(gvGridViewData* view, int x, int y)
{
    HDC hdc;
    RECT r;
    RECT rect;
    int oldrop;

    hdc = GetClientDC (view->hCtrl);
    oldrop = SetRasterOperation (hdc, ROP_XOR);
    GetClientRect (view->hCtrl, &r);
    SetPenColor (hdc, PIXEL_lightwhite);
    LineEx (hdc, view->drag_x, 0, view->drag_x, RECTH(r));
    SetRasterOperation (hdc, oldrop);
    ReleaseDC (hdc);
    
    if (x < view->drag_x) x = view->drag_x;

    get_cell_rect (view, 0, view->index_drag, &rect);
    SendMessage (view->hCtrl, GRIDM_SETCOLWIDTH, view->index_drag, x-rect.left+2);
/*
    if (view->index_drag) {
        cell->data.header.size = (view->drag_x-rect.left);
        if (cell->data.header.size < 50 && find_checkbox(view))
            cell->data.header.size = 50;
    } else
        cell->data.table.height = (view->drag_x-rect.left);
*/
    return 0;
}

static int resize_row_mouse_move(gvGridViewData* view, int x, int y)
{
    HDC hdc;
    RECT r1;
    RECT r2;
    int oldrop;
    
    get_cell_rect (view, view->index_drag, 0, &r1);
    if (y <= r1.top) 
        return 0;
    
    hdc = GetClientDC (view->hCtrl);
    GetClientRect (view->hCtrl, &r2);
    oldrop = SetRasterOperation (hdc, ROP_XOR);
    SetPenColor (hdc, PIXEL_lightwhite); 
    LineEx (hdc, 0, view->drag_y, RECTW(r2), view->drag_y);
    LineEx (hdc, 0, y, RECTW(r2), y);
    SetRasterOperation (hdc, oldrop);
    ReleaseDC (hdc);
    
    view->drag_y = y;
    return 0;
}

static int resize_col_mouse_move(gvGridViewData* view, int x, int y)
{
    HDC hdc;
    RECT r1;
    RECT r2;
    int oldrop;
    
    get_cell_rect (view, 0, view->index_drag,  &r1);
    if (x <= r1.left) 
        return 0;
    
    hdc = GetClientDC (view->hCtrl);
    GetClientRect (view->hCtrl, &r2);
    oldrop = SetRasterOperation (hdc, ROP_XOR);
    SetPenColor (hdc, PIXEL_lightwhite); 
    LineEx (hdc, view->drag_x, 0, view->drag_x, RECTW(r2));
    LineEx (hdc, x, 0, x, RECTW(r2));
    SetRasterOperation (hdc, oldrop);
    ReleaseDC (hdc);
    
    view->drag_x = x;
    return 0;
}

static int default_mouse_down(gvGridViewData* view, int x, int y)
{
    int row, col;
    RECT rect;
    get_cell_from_xy(view, x, y, &row, &col);
    get_cell_rect(view, row, col, &rect);
    gridview_make_rect_visible(view, &rect);
    //set_current_cell(view, row, col);
    return 0;
}

static int mouse_default_move(gvGridViewData* view, int x, int y)
{
    int row, col;
    int cursor_id = 0;
    RECT r;
    get_cell_from_xy(view, x, y, &row, &col);
    get_cell_rect(view, row, col, &r);
    if ((row == 0) && (col == 0))
        return 0;
    if (row == 0 &&  (x >= r.right -2 && x <= r.right))
            cursor_id = IDC_SPLIT_VERT;
    else if (col == 0 && (y >= r.bottom-2 && y <= r.bottom))
            cursor_id = IDC_SPLIT_HORZ;
    SetCursor(GetSystemCursor(cursor_id));
    return 0;
}

// }}}

// gridview_init {{{
static int gridview_init(HWND hWnd, GRIDVIEWDATA* data)
{
    gvGridViewData* view;
    gvGridCellData cell;
    RECT rcWnd;
    int i;

    view = (gvGridViewData*)calloc(1, sizeof(gvGridViewData));
    if(view == NULL)
        return -1;
    // set window edit rows & edit cols
    view->current_row = 1;
    view->current_col = 1;
    // set window handles
    view->ctrl_id = GetDlgCtrlID(hWnd);
    view->hCtrl = hWnd;
    view->hEdit = HWND_INVALID;
    //initialize modal
    view->modal = GridDataModalArray_new_modal(data->nr_rows, data->nr_cols);
    //set table properties
    gvGridCellTypeTableHeader->init(&cell);
    cell.data.table.width = data->col_width;
    cell.data.table.height = data->row_height;
    cell.data.table.def_cell = malloc(sizeof(gvGridCellData));
    gvGridCellTypeText->init(cell.data.table.def_cell);
    view->modal->set_value_at(view, 0, 0, &cell);
    //set all headers
    gvGridCellTypeHeader->init(&cell);
    cell.data.header.size = data->row_height;
    for(i=1; i<=data->nr_rows; i++)
        view->modal->set_value_at(view, i, 0, &cell);
    cell.data.header.size = data->col_width;
    for(i=1; i<=data->nr_cols; i++)
        view->modal->set_value_at(view, 0, i, &cell);

    //initialize scroll
    GetClientRect (hWnd, &rcWnd);
    scrolled_init (hWnd, &view->scrdata,
            RECTW(rcWnd), RECTH(rcWnd));
    scrolled_init_margins (&view->scrdata, 0, 0, 0, 0);
    view->scrdata.move_content = gridview_set_content;

    //set add data
    SetWindowAdditionalData2(hWnd, (DWORD)view);

    SetFocusChild(hWnd);
    return 0;
}

// }}}

// Grid Array Modal {{{

typedef struct _GridDataModalArray GridDataModalArray;
struct _GridDataModalArray
{
    int row;
    int col;
    gvGridCellData** cells;
};

static inline int array_xy_of(GridDataModalArray* array, int row, int col)
{
    return (array->col+1)*row+col;
}

static int GridDataModalArray_get_col_count(gvGridViewData* view)
{
    GridDataModalArray* array = (GridDataModalArray*)view->modal->data;
    return array->col;
}

static int GridDataModalArray_get_row_count(gvGridViewData* view)
{
    GridDataModalArray* array = (GridDataModalArray*)view->modal->data;
    return array->row;
}

static gvGridCellData* GridDataModalArray_get_value_at(gvGridViewData* view, int row, int col)
{
    GridDataModalArray* array = (GridDataModalArray*)view->modal->data;
    gvGridCellData* cell;

    if(row < 0 || col < 0 || row > array->row || col > array->col)
        return NULL;
    cell = array->cells[array_xy_of(array, row, col)];
    return cell;
}

static int GridDataModalArray_set_value_at(gvGridViewData* view, int row, int col, gvGridCellData* data)
{
    GridDataModalArray* array = (GridDataModalArray*)view->modal->data;
    gvGridCellData* cell = NULL;
    if(row < 0 || col < 0 || row > array->row || col > array->col)
        return -1;
    cell = array->cells[array_xy_of(array, row, col)];
    if(cell != NULL && cell->func->on_destroy != NULL)
        cell->func->on_destroy(cell);
    if(data != NULL)
    {
        if(cell == NULL)
            cell = malloc(sizeof(gvGridCellData));
        memcpy(cell, data, sizeof(gvGridCellData));
    }
    else if(cell != NULL)
    {
        free(cell);
        cell = NULL;
    }
    array->cells[array_xy_of(array, row, col)] = cell;
    return 0;
}

static gvGridCellData* GridDataModalArray_get_value_or_default_at(gvGridViewData* view, int row, int col)
{
    gvGridCellData* cell;
    cell = view->modal->get_value_at(view, row, col);
    if(cell != NULL || row == 0 || col == 0)
        return cell;
    do
    {
        BOOL col_first;
        gvGridCellData* table;
        int r1, r2, c1, c2;
        table = view->modal->get_value_at(view, 0, 0);
        col_first = table->data.table.col_first;
        if(col_first)
            { r1 = 0; c1 = col; r2 = row; c2 = 0; }
        else
            { r1 = row; c1 = 0; r2 = 0; c2 = col; }
        cell = view->modal->get_value_at(view, r1, c1);
        cell = cell->data.header.def_cell;
        if(cell != NULL)
            break;
        cell = view->modal->get_value_at(view, r2, c2);
        cell = cell->data.header.def_cell;
        if(cell != NULL)
            break;
        cell = table->data.table.def_cell;
    }
    while(0);
    return cell;
}

static int GridDataModalArray_add_new_row(gvGridViewData* view, int index)
{
    GridDataModalArray* array = (GridDataModalArray*)view->modal->data;
    int row_length = array->col+1;
    int row_byte_length = row_length*sizeof(gvGridCellData*);
    gvGridCellData** new_line_begining;
    if (view->current_row >= index)
        view->current_row++;
    if(index < 0 || index > array->row)
        index = array->row;
    array->cells = realloc(array->cells, (array->row+2)*row_byte_length);
    new_line_begining = &array->cells[row_length*(index+1)];
    if(index < array->row)
    {
        gvGridCellData** move_line_begining = &array->cells[row_length*array->row];
        for(; move_line_begining >= new_line_begining;
                move_line_begining -= row_length)
            memcpy(move_line_begining+row_length, move_line_begining, row_byte_length);
    }
    memset(new_line_begining, 0, row_byte_length);
    array->row++;
    return index+1;
}

static int GridDataModalArray_delete_row(gvGridViewData* view, int index)
{
    GridDataModalArray* array = (GridDataModalArray*)view->modal->data;
    int row_length = array->col+1;
    int row_byte_length = row_length*sizeof(gvGridCellData*);
    int i;
    gvGridCellData* cell;
    if (view->current_row > index && view->current_row > 1)
        view->current_row--;
    if(index < 0 || index >= array->row)
        index = array->row-1;
    index++;
    for(i=0; i<=array->col; i++)
    {
        cell = array->cells[index*row_length+i];
        if(cell != NULL)
            cell->func->on_destroy(cell);
    }
    if(index < array->row)
    {
        gvGridCellData** end_line_begining = &array->cells[row_length*array->row];
        gvGridCellData** del_line_begining = &array->cells[row_length*index];
        i = index;
        for(; del_line_begining < end_line_begining;
                del_line_begining += row_length)
            memcpy(del_line_begining, del_line_begining+row_length, row_byte_length);
    }
    array->cells = realloc(array->cells, (array->row)*row_byte_length);
    array->row--;
    return 0;
}

static int GridDataModalArray_add_new_col(gvGridViewData* view, int index)
{
    GridDataModalArray* array = (GridDataModalArray*)view->modal->data;
    gvGridCellData** new_cells;
    int i, j;
    if (view->current_col >= index)
        view->current_col++;
    if(index < 0 || index > array->col)
        index = array->col;
    new_cells = malloc((array->row+1)*(array->col+2)*sizeof(gvGridCellData*));
    for(i=0; i<=array->row; i++)
    {
        for(j=0; j<=array->col+1; j++)
        {
            if(j==index+1)
                new_cells[i*(array->col+2)+j] = NULL;
            else if(j < index+1)
                new_cells[i*(array->col+2)+j] = array->cells[i*(array->col+1)+j];
            else
                new_cells[i*(array->col+2)+j] = array->cells[i*(array->col+1)+j-1];
        }
    }
    array->col++;
    free(array->cells);
    array->cells = new_cells;
    return index+1;
}

static int GridDataModalArray_delete_col(gvGridViewData* view, int index)
{
    GridDataModalArray* array = (GridDataModalArray*)view->modal->data;
    gvGridCellData** new_cells;
    int i, j;
    gvGridCellData* cell;
    if (view->current_col > index && view->current_col > 1)
        view->current_col--;
    if(index < 0 || index >= array->col)
        index = array->col-1;
    new_cells = malloc((array->row+1)*(array->col)*sizeof(gvGridCellData*));
    for(i=0; i<=array->row; i++)
    {
        for(j=0; j<=array->col; j++)
        {
            if(j == index+1)
            {
                cell = array->cells[i*(array->col+1)+j];
                if(cell != NULL)
                    cell->func->on_destroy(cell);
                free(cell);
            }
            else if(j < index+1)
                new_cells[i*(array->col)+j] = array->cells[i*(array->col+1)+j];
            else
                new_cells[i*(array->col)+j-1] = array->cells[i*(array->col+1)+j];
        }
    }
    array->col--;
    free(array->cells);
    array->cells = new_cells;
    return 0;
}

gvGridDataModal* GridDataModalArray_new_modal(int row, int col)
{
    gvGridDataModal* modal;
    GridDataModalArray* array;

    modal = malloc(sizeof(gvGridDataModal));
    modal->get_col_count = GridDataModalArray_get_col_count;
    modal->get_row_count = GridDataModalArray_get_row_count;
    modal->get_value_at = GridDataModalArray_get_value_at;
    modal->set_value_at = GridDataModalArray_set_value_at;
    modal->get_value_or_default_at = GridDataModalArray_get_value_or_default_at;
    modal->add_new_row = GridDataModalArray_add_new_row;
    modal->delete_row = GridDataModalArray_delete_row;
    modal->add_new_col = GridDataModalArray_add_new_col;
    modal->delete_col = GridDataModalArray_delete_col;
    array = malloc(sizeof(GridDataModalArray));
    array->row = row;
    array->col = col;
    array->cells = calloc((row+1)*(col+1), sizeof(gvGridCellData*));
    modal->data = array;
    return modal;
}

void GridDataModalArray_delete_modal(gvGridDataModal* modal)
{
    int i;
    GridDataModalArray* array = (GridDataModalArray*)modal->data;
    for(i=(array->row+1)*(array->col+1)-1; i>=0; i--)
    {
        gvGridCellData* cell = array->cells[i];
        if(cell != NULL)
        {
            cell->func->on_destroy(cell);
            free(cell);
        }
    }
    free(array->cells);
    free(array);
    free(modal);
}

// }}}


static LRESULT GridViewCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    gvGridViewData* view = NULL;
    if (message != MSG_CREATE)
        view = (gvGridViewData*) GetWindowAdditionalData2(hWnd);
    switch (message)
    {
// MiniGUI Window Messages {{{
        case MSG_CREATE:
        {
            return gridview_init(hWnd, (GRIDVIEWDATA*)lParam);
        }

        case MSG_SIZECHANGED:
        {
            RECT rect;

            update_row_col_size(view);
           
            GetClientRect(hWnd, &rect); 
            view->scrdata.visibleWidth = RECTW(rect);
            view->scrdata.visibleHeight = RECTH(rect);
            view->scrdata.hScrollPageVal = RECTW(view->cells_rect); 
            view->scrdata.vScrollPageVal =  RECTH(view->cells_rect); 
            break;
        }
            
        case MSG_PAINT:
        {
            int i, j;
            int row, col;
            HDC hdc = BeginPaint(hWnd);

            if(view->hEdit != HWND_INVALID)
            {
                RECT r;
                GetWindowRect(view->hEdit, &r);
                ExcludeClipRect(hdc, &r);
            }
            row = view->modal->get_row_count(view);
            col = view->modal->get_col_count(view);
            
            //draw cells
            for(i=1; i<=row; i++)
            {
                for(j=1; j<=col; j++)
                    draw_cell(view, hWnd, hdc, i, j);
            }
            
            //draw table header & row header
            j=0;
            for(i=0; i<=row; i++)
                draw_cell(view, hWnd, hdc, i, j);
            //draw col header
            i=0;
            for(j=1; j<=col; j++)
                draw_cell(view, hWnd, hdc, i, j);

            EndPaint(hWnd, hdc);
            return 0;
        }

        case MSG_DESTROY:
        {
            if(view->depend_list != NULL)
            {
                gvGridCellDepedence* list = view->depend_list;
                while(list != NULL)
                {
                    view->depend_list = list->next;
                    free(list);
                    list = view->depend_list;
                }
            }
            GridDataModalArray_delete_modal(view->modal);
            free (view);
            break;
        }
// }}}
// MiniGUI mouse Messages {{{
        case MSG_LBUTTONDBLCLK:
        {
            int x = LOSWORD (lParam);
            int y = HISWORD (lParam);
            int row, col;
            //view->is_mouse_down = FALSE;
            get_cell_from_xy(view, x, y, &row, &col);
            if(row > 0 && col > 0)
            {
                RECT rect;
                gvGridCellData* cell;
                set_current_cell(view, row, col);
                set_highlight_cells(view, row, col, 1, 1);
                get_cell_rect(view, row, col, &rect);
                cell = view->modal->get_value_or_default_at(view, row, col);
                cell->func->begin_edit(cell, view, row, col, &rect);
                NotifyParent(view->hCtrl, view->ctrl_id, GRIDN_CELLDBCLK);
            }
            break;
        }
        case MSG_LBUTTONDOWN:
        {
            BOOL is_capture;
            int x = LOSWORD (lParam);
            int y = HISWORD (lParam);
        
            if(view->hEdit != HWND_INVALID)
            {
                int row, col;
                if(!gridview_is_pt_in_hwnd_rect(view->hCtrl, view->hEdit, x, y))
                {
                    SendMessage(view->hEdit, MSG_KEYDOWN, SCANCODE_ENTER, 0);
                    get_cell_from_xy(view, x, y, &row, &col);
                    set_current_cell(view, row, col);
                    set_highlight_cells(view, row, col, 1, 1);
                }
                break;
            }

            is_capture = (GetCapture() == hWnd);

            if (is_capture)
                ScreenToClient(view->hCtrl, &x, &y);
            else if(gridview_is_pt_in_control(view, x, y))
                break;
            else
                SetCapture(hWnd);

            change_mouse_status(view, x, y);
            switch(view->mouse_status)
            {
                case MOUSE_CELL_SELECT:
                    select_cell_mouse_down(view, x, y);
                    break;
                case MOUSE_ROW_SELECT:
                    select_head_mouse_down(view, x, y, 1);
                    break;
                case MOUSE_COLUMN_SELECT:
                    select_head_mouse_down(view, x, y, 0);
                    break;
                case MOUSE_ROW_RESIZE:
                    resize_row_mouse_down(view, x, y);
                    break;
                case MOUSE_COLUMN_RESIZE:
                    resize_col_mouse_down(view, x, y);
                    break;
                case MOUSE_DEFAULT:
                default:
                    default_mouse_down(view, x, y);
                    break;
            }
            if(is_capture)
                return 0;
            break;
        }
        case MSG_LBUTTONUP:
        {
            BOOL is_capture;
            int x = LOSWORD (lParam);
            int y = HISWORD (lParam);
            is_capture = (GetCapture() == hWnd);
            if (is_capture)
            {
                ScreenToClient(hWnd, &x, &y);
                ReleaseCapture();
            }
            else if(view->hEdit != HWND_INVALID)
                break;

            switch(view->mouse_status)
            {
                case MOUSE_CELL_SELECT:
                    select_cell_mouse_up(view, x, y);
                    break;
                case MOUSE_ROW_SELECT:
                    select_row_mouse_up(view, x, y);
                    break;
                case MOUSE_COLUMN_SELECT:
                    select_col_mouse_up(view, x, y);
                    break;
                case MOUSE_ROW_RESIZE:
                    resize_row_mouse_up(view, x, y);
                    break;
                case MOUSE_COLUMN_RESIZE:
                    resize_col_mouse_up(view, x, y);
                    break;
                case MOUSE_DEFAULT:
                default:
                    break;
            }
            view->mouse_status = MOUSE_DEFAULT;
            if(is_capture)
                return 0;
            break;
        }
        case MSG_MOUSEMOVE:
        {
            BOOL is_capture;
            RECT rect;
            int x = LOSWORD (lParam);
            int y = HISWORD (lParam);
            
            GetClientRect(hWnd, &rect);
            if(view->hEdit != HWND_INVALID)
                break;
            is_capture = (GetCapture() == hWnd);
            if (is_capture) {
                ScreenToClient(view->hCtrl, &x, &y);
                if (x < 0) x = 0;
                if (y < 0) y = 0;
                if (x > RECTW(rect)) x = RECTW(rect);
                if (y > RECTH(rect)) y = RECTH(rect);
            }

            switch(view->mouse_status)
            {
                case MOUSE_CELL_SELECT:
                    select_cell_mouse_move(view, x, y);
                    break;
                case MOUSE_ROW_SELECT:
                    select_row_mouse_move(view, x, y);
                    break;
                case MOUSE_COLUMN_SELECT:
                    select_col_mouse_move(view, x, y);
                    break;
                case MOUSE_ROW_RESIZE:
                    resize_row_mouse_move(view, x, y);
                    break;
                case MOUSE_COLUMN_RESIZE:
                    resize_col_mouse_move(view, x, y);
                    break;
                case MOUSE_DEFAULT:
                default:
                    mouse_default_move(view, x, y);
                    break;
            }
            if(is_capture)
                return 0;
            break;
        }
// }}}
// MiniGUI key Messages {{{
        case MSG_KEYDOWN:
        {
            if(view->hEdit != HWND_INVALID)
                return DefaultControlProc(hWnd, message, wParam, lParam);
            else
            {
                BOOL need_break = FALSE;
                switch (wParam)
                {
                    case SCANCODE_TAB:
                        set_current_cell(view, view->current_row, view->current_col+1);
                        set_highlight_cells(view, view->current_row, view->current_col, 1, 1);
                        break;
                    case SCANCODE_ENTER:
                        set_current_cell(view, view->current_row+1, view->current_col);
                        set_highlight_cells(view, view->current_row, view->current_col, 1, 1);    
                        break;
                    case SCANCODE_CURSORBLOCKUP:
                        if (lParam & KS_SHIFT)
                            set_highlight_cells_to(view, view->current_row-1, view->current_col);
                        else {
                            set_current_cell(view, view->current_row-1, view->current_col);
                            set_highlight_cells(view, view->current_row, view->current_col, 1, 1);    
                        }
                        break;
                    case SCANCODE_CURSORBLOCKDOWN:
                        if (lParam & KS_SHIFT) {
                            set_highlight_cells_to(view, view->current_row+1, view->current_col);
                        }
                        else {
                            set_current_cell(view, view->current_row+1, view->current_col);
                            set_highlight_cells(view, view->current_row, view->current_col, 1, 1);    
                        }
                        break;
                    case SCANCODE_CURSORBLOCKLEFT:
                        if (lParam & KS_SHIFT)
                            set_highlight_cells_to(view, view->current_row, view->current_col-1);
                        else {
                            set_current_cell(view, view->current_row, view->current_col-1);
                            set_highlight_cells(view, view->current_row, view->current_col, 1, 1);    
                        }
                        break;
                    case SCANCODE_CURSORBLOCKRIGHT:
                        if (lParam & KS_SHIFT)
                            set_highlight_cells_to(view, view->current_row, view->current_col + 1);
                        else {
                            set_current_cell(view, view->current_row, view->current_col + 1);
                            set_highlight_cells(view, view->current_row, view->current_col, 1, 1);
                        }
                        break;
                    case SCANCODE_HOME:
                        {
                            if (view->current_col == 0)
                                return 0;
                            if (lParam & KS_SHIFT)
                                set_highlight_cells_to(view, view->current_row, 1);
                            else {
                                set_current_cell(view, view->current_row, 1);
                                set_highlight_cells(view, view->current_row, view->current_col, 1, 1);
                            }
                        }
                        break;
                    case SCANCODE_END:
                        {
                            int col = view->current_col;
                            int ncols = view->modal->get_col_count(view);
                            if (col == 0 || col > ncols)
                                return 0;
                            if (lParam & KS_SHIFT)
                                set_highlight_cells_to(view,  view->current_row, ncols);
                            else {
                                set_current_cell(view, view->current_row, ncols);
                                set_highlight_cells(view, view->current_row, view->current_col, 1, 1);
                            }
                        }
                        break;
                    case SCANCODE_PAGEDOWN:
                        {
                            int row, col;
                            int count;
                            RECT rect;
                            RECT cell_rect;

                            count = view->modal->get_row_count(view);
                            GetClientRect(view->hCtrl, &rect);
                            get_cell_rect (view, count, 0, &cell_rect);

                            if (rect.bottom > cell_rect.bottom) 
                                get_cell_from_xy(view, cell_rect.left + 1, cell_rect.bottom - 1, &row, &col);
                            else 
                                get_cell_from_xy(view, rect.left  -1, rect.bottom - 1, &row, &col);

                            SendMessage (view->hCtrl, MSG_VSCROLL, SB_PAGEDOWN, 0);

                            if (lParam & KS_SHIFT)
                                set_highlight_cells_to(view, row, view->current_col);
                            else {
                                set_current_cell(view, row, view->current_col);
                                set_highlight_cells(view, view->current_row, view->current_col, 1, 1);
                            }
                        }
                        break;
                    case SCANCODE_PAGEUP:
                        {
                            int row, col;
                            gvGridCellData* table;

                            table = view->modal->get_value_at(view, 0, 0);
                            get_cell_from_xy(view, table->data.table.width,
                                             table->data.table.height,
                                             &row, &col);
                            
                            SendMessage (view->hCtrl, MSG_VSCROLL, SB_PAGEUP, 0);
                            
                            if (lParam & KS_SHIFT)
                                set_highlight_cells_to(view, row, view->current_col);
                            else {
                                set_current_cell(view, row, view->current_col);
                                set_highlight_cells(view, view->current_row, view->current_col, 1, 1);
                            }
                        }
                        break;
                    case SCANCODE_ESCAPE:
                        if (view->highlight_cells.width != 0 || view->highlight_cells.height != 0)
                        {
                            set_highlight_cells(view, view->current_row, view->current_col, 1, 1);
                        }
                        break;
                    default:
                        need_break = TRUE;
                        break;
                }
                NotifyParent(view->hCtrl, view->ctrl_id, GRIDN_KEYDOWN);
                if(need_break)
                    break;
                else
                    return DefaultControlProc(hWnd, message, wParam, lParam);
            }
            break;
        }
        case MSG_CHAR:
        {
            gvGridCellData* cell;
            RECT rect;
            if(view->current_row > 0 && view->current_col > 0)
            {
                if (view->hEdit == HWND_INVALID && isalnum(wParam))
                {
                    get_cell_rect(view, view->current_row, view->current_col, &rect);
                    gridview_make_rect_visible(view, &rect);
                    cell = view->modal->get_value_or_default_at(
                            view, view->current_row, view->current_col);
                    get_cell_rect(view, view->current_row, view->current_col, &rect);
                    cell->func->begin_edit(cell, view, view->current_row, view->current_col, &rect);
                }
            }
            return DefaultControlProc(hWnd, message, wParam, lParam);
        }
// }}}
// MiniGUI scroll Messages {{{
        case MSG_HSCROLL:
        {
            if (view->hEdit != HWND_INVALID)
                SendMessage(view->hEdit, MSG_KEYDOWN, SCANCODE_ENTER, 0);
            break;
        }
        case MSG_VSCROLL:
        {
            if (view->hEdit != HWND_INVALID)
                SendMessage(view->hEdit, MSG_KEYDOWN, SCANCODE_ENTER, 0);
            break;
        }
// }}}
// Grid View Messages {{{
        case GRIDM_SETCELLPROPERTY:
        {
            int i, j;
            GRIDCELLS* cells = (GRIDCELLS*)wParam;
            GRIDCELLDATA* data = (GRIDCELLDATA*)lParam;
            if (cells == NULL || data == NULL || data->content == NULL) 
                return GRID_ERR;
            
            for (i = 0; i<cells->height; i++)
            {
                for (j = 0; j<cells->width; j++)
                    set_cell_value(view, data, cells->row + i, cells->column +j);
            }

            NotifyParent(view->hCtrl, view->ctrl_id, GRIDN_CELLTEXTCHANGED);

            //gvGridCells_get_cells_rect(view, &cells, &r);
            //InvalidateRect(hWnd, &r, TRUE);
            InvalidateRect(hWnd, NULL, FALSE);
            if(cells->row == 0 || cells->column == 0)
                update_row_col_size(view);
            gvGridCellDep_update_dep(view, cells);
            return 0;
        }
        
        case GRIDM_GETCELLPROPERTY:
        {
            GRIDCELLS* cells = (GRIDCELLS*)wParam;
            GRIDCELLDATA* data = (GRIDCELLDATA*)lParam;
            if (data == NULL)
                return GRID_ERR;
            return fetch_cell_value(view, data, cells->row, cells->column);
        }
        
        case GRIDM_ADDCOLUMN:
        {
            int index = wParam;
            GRIDCELLDATA* data = (GRIDCELLDATA*)lParam;
            GRIDCELLDATAHEADER* header;
            gvGridCellData cell;
            RECT rect;
            RECT header_rect;
            
            if(data != NULL
                    && ( (data->style & GV_TYPE_MASK) != GV_TYPE_HEADER
                        || data->content == NULL))
                return GRID_ERR;
            
            index = view->modal->add_new_col(view, index);
            if(index == -1)
                return GRID_ERR;
            gvGridCellTypeHeader->init(&cell);
            if(data != NULL)
            {
                header = (GRIDCELLDATAHEADER*)data->content;
                cell.data.header.size = header->size;
                cell.data.header.text = strdup(header->buff);
            }
            else
            {
                cell.data.header.size = -1;
                cell.data.header.text = NULL;
            }
            view->modal->set_value_at(view, 0, index, &cell);

            update_row_col_size(view);
            
            get_cell_rect (view, 0, index, &rect);
            
            // Update column header default content, 
            header_rect.bottom = rect.bottom; 
            header_rect.top = rect.top;
            header_rect.left = view->cells_rect.left;
            header_rect.right = view->cells_rect.right;

            // Change cols position
            if (rect.left <= view->cells_rect.left) {
                InvalidateRect (hWnd, &header_rect, FALSE);
                update_viewport (view, view->cells_rect.left, RECTW(rect), 0);
            }
            if (rect.left > view->cells_rect.left && 
                    rect.left < view->cells_rect.right) {
                InvalidateRect (hWnd, &header_rect, FALSE);
                update_viewport (view, rect.left, RECTW(rect), 0);
            }

            // Change current col
            if (view->current_col >= index) {
                set_highlight_cells(view, view->current_row, view->current_col, 1, 1);
            }   
            
            return GRID_OKAY;
        }

        case GRIDM_DELCOLUMN:
        {
            int index = lParam;
            int count;
            RECT rect;
            RECT header_rect;
            
            count = view->modal->get_col_count(view);
            if(index >= count)
                return GRID_ERR;
            if (count == 1) {
                gvGridCellData *cell;
                int row_count = view->modal->get_row_count(view);
                
                if (row_count > 1) {
                    int i;
                    for (i = 1; i < row_count; i++){
                        cell = view->modal->get_value_at(view, i, 1);
                        if (cell != NULL && cell->func->on_destroy != NULL)
                            cell->func->on_destroy(cell);
                    }
                    InvalidateRect (hWnd, NULL, TRUE);
                }

                set_current_cell (view, 1, 1);

                return GRID_OKAY;
            }

            get_cell_rect (view, 0, index, &rect);

            view->modal->delete_col(view, index);        

            update_row_col_size(view);

            // Update column header default content, 
            header_rect.bottom = rect.bottom; 
            header_rect.top = rect.top;
            header_rect.left = view->cells_rect.left;
            header_rect.right = view->cells_rect.right;
            
            // Change cols position
            if (rect.left <= view->cells_rect.left) {
                InvalidateRect (hWnd, &header_rect, TRUE);
                update_viewport (view, view->cells_rect.left, -RECTW(rect), 0);
            }
            
            if (rect.left > view->cells_rect.left &&
                    rect.left <= view->cells_rect.right) {
                InvalidateRect (hWnd, &header_rect, TRUE);
                update_viewport (view, rect.left, -RECTW(rect), 0);
            }

            // Change current col
            if (view->current_col >= index)
                set_highlight_cells (view, view->current_row, view->current_col, 1, 1);

            return GRID_OKAY;
        }

        case GRIDM_ADDROW:
        {
            int index = wParam;
            GRIDCELLDATA* data = (GRIDCELLDATA*)lParam;
            GRIDCELLDATAHEADER* header;
            gvGridCellData cell;
            RECT rect;
            RECT header_rect;
            
            if(data != NULL                    
                    && ( (data->style & GV_TYPE_MASK) != GV_TYPE_HEADER
                    ||  data->content == NULL))
                return GRID_ERR;
            index = view->modal->add_new_row(view, index);
            if(index == -1)
                return GRID_ERR;
            gvGridCellTypeHeader->init(&cell);
            if(data != NULL) {
                header = (GRIDCELLDATAHEADER*)data->content;
                cell.data.header.size = header->size;
                cell.data.header.text = strdup(header->buff);
            }
            else {
                cell.data.header.size = -1;
                cell.data.header.text = NULL;
            }
            view->modal->set_value_at(view, index, 0, &cell);

            update_row_col_size(view);

            get_cell_rect (view, index, 0, &rect);

            header_rect.left = rect.left;
            header_rect.right = rect.right;
            header_rect.top = view->cells_rect.top;
            header_rect.bottom = view->cells_rect.bottom;

            if (rect.top <= view->cells_rect.top) {
                InvalidateRect (hWnd, &header_rect, TRUE);
                update_viewport (view, view->cells_rect.top, RECTH(rect), 1);
            }
            if (rect.top > view->cells_rect.top &&
                    rect.top < view->cells_rect.bottom) {
                InvalidateRect (hWnd, &header_rect, TRUE);
                update_viewport (view, rect.top, RECTH(rect), 1);
            }
            
            if (view->current_row >= index) {
                set_highlight_cells(view, view->current_row, view->current_col, 1, 1);
            }
            
            return GRID_OKAY;
        }

        case GRIDM_DELROW:
        {
            int index = lParam;
            int count;
            RECT rect;
            RECT header_rect;
            
            count = view->modal->get_row_count(view);
            if(index >= count)
                return GRID_ERR;
            
            if (count == 1) {
                gvGridCellData *cell;
                int col_count = view->modal->get_col_count(view);
                
                if (col_count > 1) {
                    int i;
                    for (i = 1; i < col_count; i++){
                        cell = view->modal->get_value_at(view, 1, i);
                        if (cell != NULL && cell->func->on_destroy != NULL)
                            cell->func->on_destroy(cell);
                    }
                    InvalidateRect (hWnd, NULL, TRUE);
                }

                set_current_cell (view, 1, 1);

                return GRID_OKAY;
            }
            
            get_cell_rect (view, index, 0, &rect);
            
            view->modal->delete_row(view, index);

            update_row_col_size(view);

            // Updata row header default content
            header_rect.left = rect.left;
            header_rect.right = rect.right;
            header_rect.top = view->cells_rect.top;
            header_rect.bottom = view->cells_rect.bottom;

            // Change rows position
            if (rect.top <= view->cells_rect.top) {
                InvalidateRect (hWnd, &header_rect, TRUE);
                update_viewport (view, view->cells_rect.top, -RECTH(rect), 1);
            }
            if (rect.top > view->cells_rect.top &&
                    rect.top < view->cells_rect.bottom) {
                InvalidateRect (hWnd, &header_rect, TRUE);
                update_viewport (view, rect.bottom, -RECTH(rect), 1);
            }

            // Change current row
            if (view->current_row >= index)
                set_highlight_cells(view, view->current_row, view->current_col, 1, 1);

            return GRID_OKAY;
        }

        case GRIDM_GETROWCOUNT:
        {
            return view->modal->get_row_count(view);
        }

        case GRIDM_GETCOLCOUNT:
        {
            return view->modal->get_col_count(view);
        }

        case GRIDM_SETCOLWIDTH:
        {
            gvGridCellData* col_head;
            int size, col, width;
            RECT up_rect;
            RECT re_rect;
            
            col = (int) wParam;
            width = (int) lParam;

            col_head = view->modal->get_value_at(view, 0, col);
            if (col_head == NULL)
                return -1;

            if (col == 0) {
                size = width - col_head->data.table.width;
                col_head->data.table.width = width;
            }
            else {
                size = width - col_head->data.header.size;
                col_head->data.header.size = width;
                col_head->data.header.end_xy += size;
            }
            
            get_cell_rect (view, 0, col, &up_rect);
            
            update_row_col_size(view);

            if (up_rect.right >= view->cells_rect.left &&
                    up_rect.left <= view->cells_rect.right) {
                update_viewport (view, up_rect.right, size, 0);
                GetClientRect (hWnd, &re_rect);
                re_rect.left = up_rect.left;
                re_rect.right = up_rect.right;
                InvalidateRect (view->hCtrl, &re_rect, TRUE);
            }

            if (up_rect.left < view->cells_rect.left)
                update_viewport (view, view->cells_rect.left, size, 0);
            
            break;
        }

        case GRIDM_GETCOLWIDTH:
        {
            int col = (int) lParam;
            gvGridCellData* col_head;
            
            col_head = view->modal->get_value_at(view, 0, col);
            if (col_head == NULL)
                return GRID_ERR;

            if (col == 0)
                return col_head->data.table.width;
            else
                return col_head->data.header.size;
        }

        case GRIDM_SETROWHEIGHT:
        {
            gvGridCellData* row_head;
            int size, row, height;
            RECT up_rect;
            RECT re_rect;
            
            row = (int) wParam;
            height = (int) lParam;
            
            row_head = view->modal->get_value_at(view, wParam, 0);
            if (row_head == NULL)
                return -1;

            if (row == 0) {
                size = height - row_head->data.table.height;
                row_head->data.table.height = height;
            } 
            else {                
                size = height - row_head->data.header.size;
                row_head->data.header.size = height;
                row_head->data.header.end_xy += size;
            }

            get_cell_rect (view, row, 0, &up_rect);
            
            update_row_col_size(view);
            
            if (up_rect.top >= view->cells_rect.top &&
                    up_rect.top <= view->cells_rect.bottom) {
                update_viewport (view, up_rect.bottom, size, 1);

                GetClientRect (hWnd, &re_rect);
                re_rect.top = up_rect.top;
                re_rect.bottom = up_rect.bottom;
                InvalidateRect (view->hCtrl, &re_rect, TRUE);
            }

            if (up_rect.top < view->cells_rect.top)
                update_viewport (view, view->cells_rect.top, size, 1);

            break;
        }

        case GRIDM_GETROWHEIGHT:
        {
            int row = (int) lParam;
            gvGridCellData* row_head;
            
            row_head = view->modal->get_value_at(view, row, 0);
            if (row_head == NULL)
                return GRID_ERR;

            if (row == 0)
                return row_head->data.table.height;
            else
                return row_head->data.header.size;
        }

        case GRIDM_SETNUMFORMAT:
        {
            GRIDCELLS* cells = (GRIDCELLS*)wParam;
            char *format = (char *)lParam;
            gvGridCellData* set_cell = NULL;
            int i, j;
            RECT rect;

            if (format == NULL || cells == NULL) 
                break;

            for (i = cells->row; i < cells->row + cells->height; ++i) 
            {
                for (j = cells->column; j < cells->column + cells->width; ++j)
                {
                    set_cell = view->modal->get_value_at(view, i, j);

                    if (set_cell == NULL) {
                        continue;
                    }
                    if (set_cell->celltype != GV_TYPE_NUMBER)
                        continue;
                    if (set_cell->data.number.format != NULL)
                        free(set_cell->data.number.format);
                    set_cell->data.number.format = strdup(format);
                }
            }
            gvGridCells_get_cells_rect(view, cells, &rect);
            InvalidateRect(view->hCtrl, &rect, FALSE);
            break;
        }

        case GRIDM_SETSELECTED:
        {
            GRIDCELLS* cells = (GRIDCELLS*)lParam;
            set_highlight_cells(view, cells->row, cells->column, cells->width, cells->height);
            return 0;
        }

        case GRIDM_GETSELECTED:
        {
            GRIDCELLS* cells = (GRIDCELLS*)lParam;
            *cells = view->highlight_cells;
            break;
        }

        case GRIDM_ADDDEPENDENCE:
        {
            GRIDCELLDEPENDENCE* dep = (GRIDCELLDEPENDENCE*)lParam;
            gvGridCellDepedence depedence;
            depedence.source.row = dep->source.row;
            depedence.source.column = dep->source.column;
            depedence.source.width = dep->source.width;
            depedence.source.height = dep->source.height;
            depedence.target.row = dep->target.row;
            depedence.target.column = dep->target.column;
            depedence.target.width = dep->target.width;
            depedence.target.height = dep->target.height;
            depedence.callback = dep->callback;
            depedence.dwAddData = dep->dwAddData;
            return gvGridCellDep_add_dep(view, &depedence);
        }

        case GRIDM_DELDEPENDENCE:
        {
            if (gvGridCellDep_delete_dep(view, wParam) == -1) 
                return GRID_ERR;
            return GRID_OKAY;
        }

        case GRIDM_ENDCELLEDIT:
        {
            GRIDCELLS cells;
            GridCellEditData* editdata = (GridCellEditData*)lParam;
            editdata->commit = (int)wParam;
            editdata->cell->func->end_edit(editdata->cell, editdata->view, editdata->row, editdata->col);
            cells.width = cells.height = 1;
            cells.row = editdata->row;
            cells.column = editdata->col;
            gvGridCellDep_update_dep(view, &cells);
            NotifyParent(view->hCtrl, view->ctrl_id, GRIDN_CELLTEXTCHANGED);
            return 0;
        }
// }}}
    }
    return DefaultScrolledProc (hWnd, message, wParam, lParam);
}

BOOL RegisterGridViewControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName    = CTRL_GRIDVIEW;
    WndClass.dwStyle        = WS_NONE;
    WndClass.dwExStyle      = WS_EX_NONE;
    WndClass.hCursor        = GetSystemCursor (0);
    WndClass.iBkColor       = GetWindowElementPixel (HWND_DESKTOP, WE_BGC_WINDOW);
    WndClass.WinProc        = GridViewCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#endif /* _MGCTRL_GRIDVIEW */

