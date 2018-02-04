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
** Created by  Peng Ke at 2004/11/29
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

#include "ctrl/gridview.h"
#include "scrolled.h"
#include "gridview_impl.h"

// cell is_? op {{{
//extern int errno;

static inline BOOL GridCell_is_current(gvGridViewData* view, int row, int col)
{
    return (row != 0 && col != 0 && row==view->current_row && col==view->current_col);
}

static inline BOOL GridCell_is_highlight(gvGridViewData* view, int row, int col)
{
    return gvGridCells_is_in_cells(&view->highlight_cells, row, col);
}

static inline BOOL gvGridCell_is_readonly(gvGridCellData* cell)
{
    return cell != NULL && cell->style & GVS_READONLY;
}

static inline void GridCell_get_ctrl_rect(gvGridCellData* cell, gvGridViewData* view,
                                        int row, int col, RECT* rect, RECT* ctrl_rect)
{
    ctrl_rect->top = rect->top+2;
    ctrl_rect->bottom = rect->bottom-2;
    ctrl_rect->left = rect->left+3;
    ctrl_rect->right = rect->right-3;
}

// }}}


// Editor Proc {{{

static LRESULT GridCell_EditDefaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GridCellEditData* editdata
            = (GridCellEditData*)GetWindowAdditionalData(hWnd);
    switch(message)
    {
    case MSG_KEYDOWN:
        if(wParam == SCANCODE_ENTER)
            SendNotifyMessage(editdata->view->hCtrl, GRIDM_ENDCELLEDIT, TRUE, (DWORD)editdata);
        else if(wParam == SCANCODE_ESCAPE)
            SendNotifyMessage(editdata->view->hCtrl, GRIDM_ENDCELLEDIT, FALSE, (DWORD)editdata);
        break;
    case MSG_KILLFOCUS:
        SendNotifyMessage(editdata->view->hCtrl, GRIDM_ENDCELLEDIT, TRUE, (DWORD)editdata);
        break;
    }
    return editdata->old_proc(hWnd, message, wParam, lParam);
}

static LRESULT GridCell_EditBgColorProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GridCellEditData* editdata
            = (GridCellEditData*)GetWindowAdditionalData(hWnd);
    switch(message)
    {
    case MSG_ERASEBKGND:
        {
            BOOL need_release = FALSE;
            HDC hdc = (HDC)wParam;
            RECT* clip = (RECT*) lParam;
            RECT r;
            GridCellFormat format;
            if(hdc == 0)
            {
                hdc = GetClientDC(hWnd);
                need_release = TRUE;
            }
            if(clip)
                r = *clip;
            else
                GetClientRect(hWnd, &r);
            editdata->cell->func->get_format(editdata->cell, hWnd, hdc, &format);
            if(GridCell_is_highlight(editdata->view, editdata->row, editdata->col))
                SetBrushColor(hdc, format.hl_color_bg);
            else
                //SetBrushColor(hdc, format.color_bg);
                SetBrushColor(hdc, GetWindowBkColor (hWnd));
            FillBox(hdc, r.left, r.top, RECTW(r), RECTH(r));
            if(need_release)
                ReleaseDC(hdc);
        }
        return 0;
    }
    return GridCell_EditDefaultProc(hWnd, message, wParam, lParam);
    //return editdata->old_proc(hWnd, message, wParam, lParam);
}

// }}}

// Edit Ctrl Notify Callback {{{

static void GridCell_CheckBoxNotify(HWND hWnd, LINT id, int nc, DWORD add_data)
{
    GridCellEditData* editdata = (GridCellEditData*)GetWindowAdditionalData(hWnd);
    switch(nc)
    {
    case BN_CLICKED:
        SendNotifyMessage(editdata->view->hCtrl, GRIDM_ENDCELLEDIT, TRUE, (DWORD)editdata);
        break;
    }
    return;
}

static void GridCell_SelectionNotify(HWND hWnd, LINT id, int nc, DWORD add_data)
{
    GridCellEditData* editdata = (GridCellEditData*)GetWindowAdditionalData(hWnd);
    switch(nc)
    {
    case CBN_DROPDOWN:
        if (editdata->view->hEdit != HWND_INVALID)
        {
            if (editdata->view->hEdit != hWnd)
            {
                GridCellEditData* data = (GridCellEditData*)GetWindowAdditionalData(editdata->view->hEdit);
                SendMessage(editdata->view->hEdit, GRIDM_ENDCELLEDIT, FALSE, (DWORD)data);
            }
        }
        editdata->view->hEdit = hWnd;
        break;
    case CBN_SELECTOK:
    case CBN_CLOSEUP:
        SendNotifyMessage(editdata->view->hCtrl, GRIDM_ENDCELLEDIT, TRUE, (DWORD)editdata);
        break;
    }
}

// }}}

// Cell Draw util functions {{{

void GridCell_draw_border(HDC hdc, RECT* rect, GridCellFormat* format, int is_highlight)
{
    if(is_highlight)
        SetPenColor(hdc, format->hl_color_border);
    else
        SetPenColor(hdc, format->color_border);
/*
    LineEx(hdc, rect->left, rect->bottom-1, rect->right-1, rect->bottom-1);
    LineEx(hdc, rect->right-1, rect->top, rect->right-1, rect->bottom-1);
*/
    MoveTo(hdc, rect->left, rect->bottom-1);
    LineTo(hdc, rect->right-1, rect->bottom-1);
    MoveTo(hdc, rect->right-1, rect->top);
    LineTo(hdc, rect->right-1, rect->bottom-1);
}

void GridCell_draw_bold_border(HDC hdc, RECT* rect, GridCellFormat* format, int is_highlight)
{
    SetPenColor(hdc, PIXEL_black);
    Rectangle (hdc, rect->left, rect->top, rect->right - 1, rect->bottom - 1);
}

static DWORD halign_format_table[] = 
{
    GV_HALIGN_LEFT, DT_LEFT,
    GV_HALIGN_CENTER, DT_CENTER,
    GV_HALIGN_RIGHT, DT_RIGHT
};

static DWORD valign_format_table[] = 
{
    GV_VALIGN_TOP, DT_TOP,
    GV_VALIGN_CENTER, DT_VCENTER,
    GV_VALIGN_BOTTOM, DT_BOTTOM
};

DWORD GridCell_get_format_from_table(DWORD* table, int table_length, DWORD style, DWORD format_default)
{
    int i;
    for(i=0; i<table_length; i+=2)
    {
        if(style == table[i])
            return table[i+1];
    }
    return format_default;
}

DWORD GridCell_get_text_format(DWORD style, DWORD halign_default, DWORD valign_default)
{
    DWORD format = 0;
    format |= GridCell_get_format_from_table(halign_format_table,
                                            TABLESIZE(halign_format_table),
                                            style & GV_HALIGN_MASK, halign_default);
    if ( !(style & GVS_MULTLINE) )
    {
        format |= DT_SINGLELINE;
        format |= GridCell_get_format_from_table(valign_format_table,
                                                TABLESIZE(valign_format_table),
                                                style & GV_VALIGN_MASK, valign_default);
    }
    return format;
}

void GridCell_fill_background(HDC hdc, RECT* rect, GridCellFormat* format, int is_highlight)
{
    if(is_highlight)
        SetBrushColor(hdc, format->hl_color_bg);
    else
        SetBrushColor(hdc, format->color_bg);
    FillBox (hdc, rect->left, rect->top, rect->right-rect->left, rect->bottom-rect->top);
}

void GridCell_draw_image(HDC hdc, RECT* rect, DWORD mask, PBITMAP image)
{
    if(mask & GVITEM_IMAGE)
    {
        // draw image
#if 0
        if (cell->data.text.image != NULL)
        {
            MGU_Assert(cell->data.text.image->bmWidth > rect->right-rect->left);
            MGU_Assert(cell->data.text.image->bmHeight > rect->bottom-rect->top);
            FillBoxWithBitmap (hdc, 
                    rect->left, 
                    rect->top, 
                    cell->data.text.image->bmWidth, 
                    cell->data.text.image->bmHeight,
                    cell->data.text.image);
        }
#endif
    }
}

void GridCell_draw_text(HWND hWnd, HDC hdc, RECT* rect, const char* text, GridCellFormat* format, int is_highlight)
{
    PLOGFONT old_font = NULL;
    if(text != NULL)
    {
        gal_pixel text_color;
        if(format->mask & GVITEM_FONT)
            old_font = SelectFont(hdc, format->font);
        if(!is_highlight)
            text_color = format->color_fg;
        else
            text_color = format->hl_color_fg;
        SetTextColor(hdc, text_color);
        SetBkColor (hdc, GetWindowBkColor (hWnd));
        SetBkMode(hdc, BM_TRANSPARENT);
        DrawText(hdc, text, strlen(text), rect, format->text_format);
       
        if(format->mask & GVITEM_FONT)
            SelectFont(hdc, old_font);
    }
}

// }}}

// Null Cell Type functions {{{
int GridCellTypeNull_init(gvGridCellData* cell)
{
    memset(cell, 0, sizeof(gvGridCellData));
    cell->celltype = GV_TYPE_UNKNOWN;
    cell->func = gvGridCellTypeNull;
    return 0;
}

int GridCellTypeNull_merge(gvGridCellData* cell, gvGridCellData* data)
{
    return 0;
}

int GridCellTypeNull_copy(gvGridCellData* to, gvGridCellData* from)
{
    memcpy(to, from, sizeof(gvGridCellData));
    return 0;
}

int GridCellTypeNull_get_text_value(gvGridCellData* cell, int row, int col, char* buf, int buf_len)
{
    if(buf != NULL && buf_len > 0)
        *buf = 0;
    return 0;
}

void GridCellTypeNull_set_text_value(gvGridCellData* cell, int row, int col, char* buf)
{
}

double GridCellTypeNull_get_double_value(gvGridCellData* cell, int row, int col)
{
    double value;
    char* buf = NULL;
    int buf_len = cell->func->get_text_value(cell, row, col, NULL, 0) + 1;
    buf = malloc(buf_len);
    cell->func->get_text_value(cell, row, col, buf, buf_len);
    value = atof(buf);
    free(buf);
    return value;
}

HWND GridCellTypeNull_get_control(gvGridCellData* cell, gvGridViewData* view, int row, int col)
{
    return HWND_INVALID;
}

void GridCellTypeNull_get_format(gvGridCellData* cell, HWND hWnd, HDC hdc, GridCellFormat* format)
{
    memset(format, 0, sizeof(GridCellFormat));
}

int GridCellTypeNull_draw(gvGridCellData* cell, gvGridViewData* view,
                                    HDC hdc, int row, int col, RECT* rect)
{
    int is_highlight = FALSE;
    int is_current = FALSE;
    RECT r;
    GridCellFormat format;
    HWND ctrl;

    if(cell == NULL)
        return -1;

    memcpy(&r, rect, sizeof(r));
    rect = &r;

    is_highlight = gvGridCells_is_in_cells(&view->highlight_cells, row, col);
    is_current = (row != 0 && col != 0 && row==view->current_row && col==view->current_col);
    if(is_current)
        is_highlight = FALSE;

    ctrl = cell->func->get_control(cell, view, row, col);
    if(ctrl != HWND_INVALID)
    {
        RECT r;
        GetWindowRect(ctrl, &r);
        ExcludeClipRect(hdc, &r);
    }
    
    cell->func->get_format(cell, view->hCtrl, hdc, &format);

    GridCell_fill_background(hdc, rect, &format, is_highlight);

    GridCell_draw_border(hdc, rect, &format, is_highlight);
    //don't draw on border
    rect->right--;
    rect->bottom--;

    if(is_current)
        GridCell_draw_bold_border(hdc, rect, &format, is_highlight);

    return cell->func->draw_content(cell, view, hdc, row, col, rect, &format);
}

int GridCellTypeNull_draw_content(gvGridCellData* cell, gvGridViewData* view, HDC hdc,
                                    int row, int col, RECT* rect, GridCellFormat* format)
{
    char* buf = NULL;
    int buf_len = cell->func->get_text_value(cell, row, col, NULL, 0) + 1;
    int is_highlight = gvGridCells_is_in_cells(&view->highlight_cells, row, col);
    int is_current = (row != 0 && col != 0 
        && row==view->current_row && col==view->current_col);

    if(is_current)
        is_highlight = FALSE;
    buf = malloc(buf_len);
    cell->func->get_text_value(cell, row, col, buf, buf_len);
    rect->left += 3;
    rect->top += 2;
    rect->right -= 3;
    rect->bottom -= 2;
    GridCell_draw_text(view->hCtrl, hdc, rect, buf, format, is_highlight);
    free(buf);
    return 0;
}

#define     GridCellTypeNull_invisible_draw     NULL

int GridCellTypeNull_begin_edit(gvGridCellData* cell, gvGridViewData* view,
                                    int row, int col, RECT* rect)
{
    return 0;
}

int GridCellTypeNull_end_edit(gvGridCellData* cell, gvGridViewData* view, int row, int col)
{
    return 0;
}

int GridCellTypeNull_on_destroy(gvGridCellData* cell)
{
    return 0;
}

// }}}

// Table Header Type functions {{{

int GridCellTypeTableHeader_init(gvGridCellData* cell)
{
    memset(cell, 0, sizeof(gvGridCellData));
    cell->celltype = GV_TYPE_TABLEHEADER;
    cell->func = gvGridCellTypeTableHeader;
    return 0;
}

int GridCellTypeTableHeader_copy(gvGridCellData* to, gvGridCellData* from)
{
    memcpy(to, from, sizeof(gvGridCellData));
    if(from->data.table.text)
        to->data.table.text = strdup(from->data.table.text);
    if(from->data.table.def_cell)
    {
        to->data.table.def_cell = malloc(sizeof(gvGridCellData));
        from->data.table.def_cell->func->copy(to->data.table.def_cell, from->data.table.def_cell);
    }
    return 0;
}

int GridCellTypeTableHeader_get_text_value(gvGridCellData* cell, int row, int col, char* buf, int buf_len)
{
    int len = 0;
    if(cell->data.table.text != NULL)
    {
        len = strlen(cell->data.table.text);
        buf[buf_len-1] = 0;
        if(buf != NULL && buf_len > 0)
            strncpy(buf, cell->data.table.text, buf_len-1);
    }
    else if(buf != NULL && buf_len > 0)
        *buf = 0;
    return len;
}

void GridCellTypeTableHeader_set_text_value(gvGridCellData* cell, int row, int col, char* text)
{
    if(cell->data.table.text != NULL)
        free(cell->data.table.text);
    cell->data.table.text = strdup(text);
}

void GridCellTypeTableHeader_get_format(gvGridCellData* cell, HWND hWnd, HDC hdc, GridCellFormat* format)
{
    memset(format, 0, sizeof(GridCellFormat));
    format->mask = 0;
    
    //format->color_bg = GetWindowElementColor (BKC_BUTTON_DEF);
    //format->hl_color_bg = GetWindowElementColor (BKC_HILIGHT_NORMAL);
    format->color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_MAINC_THREED_BODY); 
    format->hl_color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_BGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_BGCOLOR)
        format->color_bg = format->hl_color_bg = cell->data.header.color_bg;
    
    //format->color_fg = GetWindowElementColor (FGC_BUTTON_NORMAL);
    //format->hl_color_fg = GetWindowElementColor (FGC_HILIGHT_NORMAL);
    format->color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_THREED_BODY); 
    format->hl_color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_FGCOLOR)
        format->color_fg = format->hl_color_fg = cell->data.header.color_fg;
    
    format->color_border = PIXEL_black;
    format->hl_color_border = PIXEL_black;
    
    format->text_format = GridCell_get_text_format(cell->style, DT_LEFT, DT_TOP);
    if(cell->mask & GVITEM_FONT && cell->data.table.font != NULL)
    {
        format->font = cell->data.table.font;
        format->mask |= GVITEM_FONT;
    }
}

int GridCellTypeTableHeader_on_destroy(gvGridCellData* cell)
{
    if(cell->data.table.text != NULL)
    {
        free(cell->data.table.text);
        cell->data.table.text = NULL;
    }
    if(cell->data.table.def_cell != NULL)
    {
        cell->data.table.def_cell->func->on_destroy(cell->data.table.def_cell);
        free(cell->data.table.def_cell);
        cell->data.table.def_cell = NULL;
    }
    return 0;
}

// }}}

// Row/Column Header Type functions {{{

int GridCellTypeHeader_init(gvGridCellData* cell)
{
    memset(cell, 0, sizeof(gvGridCellData));
    cell->celltype = GV_TYPE_HEADER;
    cell->func = gvGridCellTypeHeader;
    return 0;
}

int GridCellTypeHeader_merge(gvGridCellData* cell, gvGridCellData* data)
{
    if(cell == NULL || data == NULL)
        return -1;
    if(cell->celltype != data->celltype)
        return -1;
    if(cell->func != data->func)
        return -1;

    if(data->mask & GVITEM_STYLE)
        cell->style = data->style;

    if(data->mask & GVITEM_BGCOLOR)
    {
        cell->data.header.color_bg = data->data.header.color_bg;
        cell->mask |= GVITEM_BGCOLOR;
    }
    if(data->mask & GVITEM_FGCOLOR)
    {
        cell->data.header.color_fg = data->data.header.color_fg;
        cell->mask |= GVITEM_FGCOLOR;
    }
    if(data->mask & GVITEM_FONT)
    {
        if (cell->data.header.font != NULL)
            DestroyLogFont(cell->data.header.font);
        cell->data.header.font = data->data.header.font;
        cell->mask |= GVITEM_FONT;
    }
    if(data->mask & GVITEM_IMAGE)
    {
        if(cell->data.header.image != NULL)
            UnloadBitmap(cell->data.header.image);
        cell->data.header.image = data->data.header.image;
        cell->mask |= GVITEM_IMAGE;
    }
    if(data->mask & GVITEM_MAINCONTENT)
    {
        if(cell->data.header.text != NULL)
            free(cell->data.header.text);
        if (data->data.header.text != NULL) {
            cell->data.header.text = strdup(data->data.header.text);
        }
        else {
            cell->data.header.text = NULL; 
        }
        cell->mask |= GVITEM_MAINCONTENT;
    }
    if (data->mask & GVITEM_SIZE)
    {
        cell->data.header.size = data->data.header.size;
        cell->mask |= GVITEM_SIZE;
    }
    return 0;
}

int GridCellTypeHeader_copy(gvGridCellData* to, gvGridCellData* from)
{
    memcpy(to, from, sizeof(gvGridCellData));
    if(from->data.header.text)
        to->data.header.text = strdup(from->data.header.text);
    if(from->data.header.def_cell)
    {
        to->data.header.def_cell = malloc(sizeof(gvGridCellData));
        from->data.header.def_cell->func->copy(to->data.header.def_cell, from->data.header.def_cell);
    }
    return 0;
}

static int get_base26_text(char* buf, int buf_len, int num)
{
    int len;
    char* tmp = buf;
    if(buf != NULL && buf_len > 0)
        memset(buf, 0, buf_len);
    len = 0;
    do
    {
        if(buf != NULL && buf_len > 0 && tmp-buf < buf_len)
        {
            char x = (char)(num%26);
            if(tmp == buf || num > 26)
                *tmp = 'A'+x;
            else
                *tmp = 'A'+x-1;
            tmp++;
        }
        num = num/26;
        len++;
    }
    while(num > 0);
    if(buf != NULL && buf_len > 0)
    {
        tmp--;
        while(tmp > buf)
        {
            char c;
            c = *tmp;
            *tmp = *buf;
            *buf = c;
            tmp--;
            buf++;
        }
    }
    return len;
}

int GridCellTypeHeader_get_text_value(gvGridCellData* cell, int row, int col, char* buf, int buf_len)
{
    int len = 0;
    if(cell->data.header.text != NULL)
    {
        len = strlen(cell->data.header.text);
        if(buf != NULL && buf_len > 0)
        {
            buf[buf_len-1] = 0;
            strncpy(buf, cell->data.header.text, buf_len-1);
        }
    }
    else
    {
        if(row != 0)
        {
            if(buf != NULL && buf_len > 0)
            snprintf(buf, buf_len, "%d", row);
            len=0;
            do
            {
                row = row/10;
                len++;
            }
            while(row>0);
        }
        else
        {
            len = get_base26_text(buf, buf_len, col-1);
        }
    }
    return len;
}

void GridCellTypeHeader_set_text_value(gvGridCellData* cell, int row, int col, char* text)
{
    if(cell->data.header.text != NULL)
        free(cell->data.header.text);
    cell->data.header.text = strdup(text);
}

void GridCellTypeHeader_get_format(gvGridCellData* cell, HWND hWnd, HDC hdc, GridCellFormat* format)
{
    memset(format, 0, sizeof(GridCellFormat));
    
    format->mask = 0;
   // format->color_bg = GetWindowElementColor (BKC_BUTTON_DEF);
   // format->hl_color_bg = GetWindowElementColor (BKC_HILIGHT_NORMAL);
    format->color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_MAINC_THREED_BODY); 
    format->hl_color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_BGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_BGCOLOR)
        format->color_bg = format->hl_color_bg = cell->data.header.color_bg;
    
    //format->color_fg = GetWindowElementColor (FGC_BUTTON_NORMAL);
    //format->hl_color_fg = GetWindowElementColor (FGC_HILIGHT_NORMAL);
    format->color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_THREED_BODY); 
    format->hl_color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_THREED_BODY); 
    if(cell->mask & GVITEM_FGCOLOR)
        format->color_fg = format->hl_color_fg = cell->data.header.color_fg;
    
    format->color_border = PIXEL_black;
    format->hl_color_border = PIXEL_black;
    
    format->text_format = GridCell_get_text_format(cell->style, DT_CENTER, DT_VCENTER);
    if(cell->mask & GVITEM_FONT && cell->data.header.font != NULL)
    {
        format->font = cell->data.header.font;
        format->mask |= GVITEM_FONT;
    }
}

int GridCellTypeHeader_draw(gvGridCellData* cell, gvGridViewData* view,
                                    HDC hdc, int row, int col, RECT* rect)
{
    GridCellFormat format;
    const WINDOWINFO * win_info;

    if(cell == NULL)
        return -1;
    win_info = GetWindowInfo(view->hCtrl);

    cell->func->get_format(cell, view->hCtrl, hdc, &format);
    
    win_info->we_rdr->draw_header(view->hCtrl, hdc, rect, 0x0);

    rect->right--;
    rect->bottom--;
    
    return cell->func->draw_content(cell, view, hdc, row, col, rect, &format);
}

int GridCellTypeHeader_on_destroy(gvGridCellData* cell)
{
    if(cell->data.header.text != NULL)
    {
        free(cell->data.header.text);
        cell->data.header.text = NULL;
    }
    if(cell->data.header.def_cell != NULL)
    {
        cell->data.header.def_cell->func->on_destroy(cell->data.header.def_cell);
        free(cell->data.header.def_cell);
        cell->data.header.def_cell = NULL;
    }
    return 0;
}

// }}}

// Text Cell Type functions {{{

int GridCellTypeText_init(gvGridCellData* cell)
{
    memset(cell, 0, sizeof(gvGridCellData));
    cell->celltype = GV_TYPE_TEXT;
    cell->func = gvGridCellTypeText;
    return 0;
}

int GridCellTypeText_merge(gvGridCellData* cell, gvGridCellData* data)
{
    if(cell == NULL || data == NULL)
        return -1;
    if(cell->celltype != data->celltype)
        return -1;
    if(cell->func != data->func)
        return -1;

    if(data->mask & GVITEM_STYLE)
        cell->style = data->style;

    if(data->mask & GVITEM_BGCOLOR)
    {
        cell->data.text.color_bg = data->data.text.color_bg;
        cell->mask |= GVITEM_BGCOLOR;
    }
    if(data->mask & GVITEM_FGCOLOR)
    {
        cell->data.text.color_fg = data->data.text.color_fg;
        cell->mask |= GVITEM_FGCOLOR;
    }
    if(data->mask & GVITEM_FONT)
    {
        if (cell->data.text.font != NULL)
            DestroyLogFont(cell->data.text.font);
        cell->data.text.font = data->data.text.font;
        cell->mask |= GVITEM_FONT;
    }
    if(data->mask & GVITEM_IMAGE)
    {
        if(cell->data.text.image != NULL)
            UnloadBitmap(cell->data.text.image);
        cell->data.text.image = data->data.text.image;
        cell->mask |= GVITEM_IMAGE;
    }
    if(data->mask & GVITEM_MAINCONTENT)
    {
        if(cell->data.text.text != NULL)
            free(cell->data.text.text);
        if (data->data.text.text != NULL) {
            cell->data.text.text = strdup(data->data.text.text);
        }
        else {
            cell->data.text.text = NULL;
        }
        cell->mask |= GVITEM_MAINCONTENT;
    }
    return 0;
}

int GridCellTypeText_copy(gvGridCellData* to, gvGridCellData* from)
{
    memcpy(to, from, sizeof(gvGridCellData));
    if(from->data.text.text != NULL)
        to->data.text.text = strdup(from->data.text.text);
    return 0;
}

int GridCellTypeText_get_text_value(gvGridCellData* cell, int row, int col, char* buf, int buf_len)
{
    int len = 0;
    if(cell->data.text.text != NULL)
    {
        len = strlen(cell->data.text.text);
        if(buf != NULL && buf_len > 0)
        {
            buf[buf_len-1] = 0;
            strncpy(buf, cell->data.text.text, buf_len-1);
        }
    }
    else if(buf != NULL && buf_len > 0)
        *buf = 0;
    return len;
}

void GridCellTypeText_set_text_value(gvGridCellData* cell, int row, int col, char* text)
{
    if(cell->data.text.text != NULL)
        free(cell->data.text.text);
    cell->data.text.text = strdup(text);
}

void GridCellTypeText_get_format(gvGridCellData* cell, HWND hWnd, HDC hdc, GridCellFormat* format)
{
    memset(format, 0, sizeof(GridCellFormat));
    format->mask = 0;
    
    // format->color_bg = GetWindowElementColor (BKC_EDIT_DEF);
    // format->hl_color_bg = GetWindowElementColor (BKC_HILIGHT_NORMAL);
    format->color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_BGC_WINDOW); 
    format->hl_color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_BGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_BGCOLOR) {
        format->color_bg = cell->data.text.color_bg;
        format->hl_color_bg = ~cell->data.text.color_bg;
    }
   
    // format->color_fg = GetWindowElementColor (FGC_CONTROL_NORMAL);
    // format->hl_color_fg = GetWindowElementColor (FGC_HILIGHT_NORMAL);
    format->color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_WINDOW); 
    format->hl_color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_FGCOLOR) {
        format->color_fg = cell->data.text.color_fg;
        format->hl_color_fg = ~cell->data.text.color_fg;
    }

    format->hl_color_border = PIXEL_lightgray;
    
    if(cell->style & GVS_MULTLINE)
        format->text_format = GridCell_get_text_format(cell->style, DT_LEFT, DT_TOP);
    else
        format->text_format = GridCell_get_text_format(cell->style, DT_LEFT, DT_VCENTER);
    if(cell->mask & GVITEM_FONT && cell->data.text.font != NULL)
    {
        format->font = cell->data.text.font;
        format->mask |= GVITEM_FONT;
    }
}

int GridCellTypeText_draw_content(gvGridCellData* cell, gvGridViewData* view, HDC hdc,
                                  int row, int col, RECT* rect, GridCellFormat* format)
{
    int is_highlight = gvGridCells_is_in_cells(&view->highlight_cells, row, col);
    int is_current = (row != 0 && col != 0 && row==view->current_row && col==view->current_col);
    
    if(is_current)
        is_highlight = FALSE;
    rect->left += 3;
    rect->top += 2;
    rect->right -= 3;
    rect->bottom -= 2;
    GridCell_draw_text(view->hCtrl, hdc, rect, cell->data.text.text, format, is_highlight);
    return 0;
}

HWND GridCellTypeText_create_edit_ctrl(gvGridViewData* view, gvGridCellData* cell, int row, int col, RECT* rect, const char* text)
{
    int is_default = FALSE;
    char* ctrl_type = CTRL_SLEDIT;
    GridCellEditData* editdata = NULL;
    gvGridCellData* real_cell = view->modal->get_value_at(view, row, col);
    if(real_cell == NULL || real_cell != cell)
        is_default = TRUE;
    if(cell == NULL)
        return HWND_INVALID;
    if( cell->style & GVS_MULTLINE )
        ctrl_type = CTRL_MLEDIT;
    else
        ctrl_type = CTRL_SLEDIT;
    if(view->hEdit != HWND_INVALID)
    {
        editdata = (GridCellEditData*)GetWindowAdditionalData(view->hEdit);
        editdata->commit = FALSE;
        editdata->cell->func->end_edit(editdata->cell, editdata->view, editdata->row, editdata->col);
        editdata = NULL;
    }
    view->hEdit = CreateWindow(ctrl_type, text,
                WS_CHILD|WS_VISIBLE, col,
                rect->left, rect->top, RECTWP(rect), RECTHP(rect),
                view->hCtrl, 0);
    if(view->hEdit != HWND_INVALID)
    {
        editdata = calloc(1, sizeof(GridCellEditData));
        editdata->view = view;
        editdata->cell = cell;
        editdata->row = row;
        editdata->col = col;
        editdata->is_default = is_default;
        editdata->commit = FALSE;
        editdata->old_proc = SetWindowCallbackProc(view->hEdit, GridCell_EditDefaultProc);
        SetWindowAdditionalData(view->hEdit, (DWORD)editdata);
    }
    return view->hEdit;
}

int GridCellTypeText_begin_edit(gvGridCellData* cell, gvGridViewData* view,
                                        int row, int col, RECT* rect)
{
    RECT r;
    HWND hEdit = HWND_INVALID;
    int len;
    char* text;
    if( gvGridCell_is_readonly(cell) )
        return -1;
    len = cell->func->get_text_value(cell, row, col, NULL, 0) + 1;
    text = malloc(len);
    cell->func->get_text_value(cell, row, col, text, len);
    GridCell_get_ctrl_rect(cell, view, row, col, rect, &r);
    hEdit = GridCellTypeText_create_edit_ctrl(view, cell, row, col, &r, text);
    free(text);
    if (hEdit != HWND_INVALID)
        SetFocusChild(hEdit);
    return 0;
}

int GridCellTypeText_end_edit(gvGridCellData* cell, gvGridViewData* view, int row, int col)
{
    int len;
    HWND hEdit = view->hEdit;
    gvGridCellData tmp_cell;
    gvGridCellData* p_cell = cell;
    GridCellEditData* editdata;
    if(view->hEdit == HWND_INVALID)
        return -1;
    editdata = (GridCellEditData*)GetWindowAdditionalData(hEdit);
    if(editdata->commit)
    {
        char* buf;
        if(editdata->is_default)
        {
            p_cell = &tmp_cell;
            cell->func->copy(p_cell, cell);
            view->modal->set_value_at(view, row, col, p_cell);
            cell = view->modal->get_value_at(view, row, col);
        }
        len = GetWindowTextLength(hEdit)+1;
        buf = malloc(len);
        GetWindowText(hEdit, buf, len);
        cell->func->set_text_value(cell, row, col, buf);
        free(buf);
    }
    DestroyWindow(hEdit);
    view->hEdit = HWND_INVALID;
    free(editdata);
    InvalidateRect(view->hCtrl, NULL, FALSE);
    return 0;
}

int GridCellTypeText_on_destroy(gvGridCellData* cell)
{
    if(cell->data.text.text != NULL)
    {
        free(cell->data.text.text);
        cell->data.text.text = NULL;
    }
    return 0;
}

// }}}

// Number Cell Type functions {{{

int GridCellTypeNumber_init(gvGridCellData* cell)
{
    memset(cell, 0, sizeof(gvGridCellData));
    cell->celltype = GV_TYPE_NUMBER;
    cell->func = gvGridCellTypeNumber;
    return 0;
}

int GridCellTypeNumber_merge(gvGridCellData* cell, gvGridCellData* data)
{
    if(cell == NULL || data == NULL)
        return -1;
    if(cell->celltype != data->celltype)
        return -1;
    if(cell->func != data->func)
        return -1;

    if(data->mask & GVITEM_STYLE)
        cell->style = data->style;

    if(data->mask & GVITEM_BGCOLOR)
    {
        cell->data.number.color_bg = data->data.number.color_bg;
        cell->mask |= GVITEM_BGCOLOR;
    }
    if(data->mask & GVITEM_FGCOLOR)
    {
        cell->data.number.color_fg = data->data.number.color_fg;
        cell->mask |= GVITEM_FGCOLOR;
    }
    if(data->mask & GVITEM_FONT)
    {
        if (cell->data.number.font != NULL)
            DestroyLogFont(cell->data.number.font);
        cell->data.number.font = data->data.number.font;
        cell->mask |= GVITEM_FONT;
    }
    if(data->mask & GVITEM_IMAGE)
    {
        if(cell->data.number.image != NULL)
            UnloadBitmap(cell->data.number.image);
        cell->data.number.image = data->data.number.image;
        cell->mask |= GVITEM_IMAGE;
    }
    if(data->mask & GVITEM_MAINCONTENT)
    {
        cell->data.number.num = data->data.number.num;
        cell->mask |= GVITEM_MAINCONTENT;
    }
    if(data->data.number.format != NULL)
    {
        if(cell->data.number.format != NULL)
            free(cell->data.number.format);
        cell->data.number.format = strdup(data->data.number.format);
    }
    return 0;
}

int GridCellTypeNumber_copy(gvGridCellData* to, gvGridCellData* from)
{
    memcpy(to, from, sizeof(gvGridCellData));
    if(from->data.number.format != NULL)
        to->data.number.format = strdup(from->data.number.format);
    return 0;
}

int GridCellTypeNumber_get_text_value(gvGridCellData* cell, int row, int col, char* buf, int buf_len)
{
    int len = 0;
    const char* format = "%f";
/*    if(cell->data.number.format != NULL)
    {
        format = cell->data.number.format;
    }
    */
    if(buf != NULL && buf_len > 0)
    {
        buf[buf_len-1] = 0;
        len = snprintf(buf, buf_len-1, format, cell->data.number.num);
    }
    else
    {
        char tmp[64];
        len = snprintf(tmp, 63, format, cell->data.number.num);
    }
    return len;
}

void GridCellTypeNumber_set_text_value(gvGridCellData* cell, int row, int col, char* text)
{
    char INF[64];
    double number = atof(text);
    snprintf(INF, 63, "%f", number);
    if (INF[0] != 'I' && INF[0] != 'i')
        cell->data.number.num = number;
    else
        _MG_PRINTF ("EX_CTRL>Grid: The inputed number is too big!\n");
}

double GridCellTypeNumber_get_double_value(gvGridCellData* cell, int row, int col)
{
    return cell->data.number.num;
}

void GridCellTypeNumber_get_format(gvGridCellData* cell, HWND hWnd, HDC hdc, GridCellFormat* format)
{
    memset(format, 0, sizeof(GridCellFormat));
    format->mask = 0;
    
    // format->color_bg = GetWindowElementColor (BKC_EDIT_DEF);
    // format->hl_color_bg = GetWindowElementColor (BKC_HILIGHT_NORMAL);
    format->color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_BGC_WINDOW); 
    format->hl_color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_BGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_BGCOLOR) {
        format->color_bg = cell->data.number.color_bg;
        format->hl_color_bg = ~cell->data.number.color_bg;
    }
    
    // format->color_fg = GetWindowElementColor (FGC_CONTROL_NORMAL);
    // format->hl_color_fg = GetWindowElementColor (FGC_HILIGHT_NORMAL);
    format->color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_WINDOW); 
    format->hl_color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_FGCOLOR) {
        format->color_fg = cell->data.number.color_fg;
        format->hl_color_fg = ~cell->data.number.color_fg;
    }
    
    format->color_border = PIXEL_lightgray;
    format->hl_color_border = PIXEL_lightgray;
    
    format->text_format = GridCell_get_text_format(cell->style, DT_LEFT, DT_TOP);
    if(cell->mask & GVITEM_FONT && cell->data.number.font != NULL)
    {
        format->font = cell->data.number.font;
        format->mask |= GVITEM_FONT;
    }
}

int GridCellTypeNumber_draw_content(gvGridCellData* cell, gvGridViewData* view, HDC hdc,
                                    int row, int col, RECT* rect, GridCellFormat* format)
{
    char* buf = NULL;
    char buf_tmp[64];
    int buf_len;// = cell->func->get_text_value(cell, row, col, NULL, 0) + 1;
    const char* buf_format = "%f";
    int is_highlight = gvGridCells_is_in_cells(&view->highlight_cells, row, col);
    int is_current = (row != 0 && col != 0 && row==view->current_row && col==view->current_col);
    
    if(is_current)
        is_highlight = FALSE;
    
    if(cell->data.number.format != NULL) {
        buf_len = snprintf (buf_tmp, 63, cell->data.number.format, cell->data.number.num);
    }
    else {
        buf_len = snprintf (buf_tmp, 9, buf_format, cell->data.number.num);
    }

#ifdef WIN32
    buf_tmp[buf_len] = 0;
#endif

    buf = malloc(buf_len + 1);
    strcpy (buf, buf_tmp);

    rect->left += 3;
    rect->top += 2;
    rect->right -= 3;
    rect->bottom -= 2;

    GridCell_draw_text(view->hCtrl, hdc, rect, buf, format, is_highlight);
    free(buf);
    return 0;
}

int GridCellTypeNumber_on_destroy(gvGridCellData* cell)
{
    if(cell->data.number.format != NULL)
    {
        free(cell->data.number.format);
        cell->data.number.format = NULL;
    }
    return 0;
}

// }}}

// CheckBox Cell Type functions {{{

int GridCellTypeCheckBox_init(gvGridCellData* cell)
{
    memset(cell, 0, sizeof(gvGridCellData));
    cell->celltype = GV_TYPE_CHECKBOX;
    cell->func = gvGridCellTypeCheckBox;
    cell->data.checkbox.handle = HWND_INVALID;
    return 0;
}

int GridCellTypeCheckBox_merge(gvGridCellData* cell, gvGridCellData* data)
{
    if(cell == NULL || data == NULL)
        return -1;
    if(cell->celltype != data->celltype)
        return -1;
    if(cell->func != data->func)
        return -1;

    if(data->mask & GVITEM_STYLE)
    {
        cell->style = data->style;
        cell->data.checkbox.dirty = TRUE;
    }

    if(data->mask & GVITEM_BGCOLOR)
    {
        cell->data.checkbox.color_bg = data->data.checkbox.color_bg;
        cell->mask |= GVITEM_BGCOLOR;
    }
    if(data->mask & GVITEM_FGCOLOR)
    {
        cell->data.checkbox.color_fg = data->data.checkbox.color_fg;
        cell->mask |= GVITEM_FGCOLOR;
    }
    if(data->mask & GVITEM_FONT)
    {
        if (cell->data.checkbox.font != NULL)
            DestroyLogFont(cell->data.checkbox.font);
        cell->data.checkbox.font = data->data.checkbox.font;
        cell->mask |= GVITEM_FONT;
    }
    if(data->mask & GVITEM_IMAGE)
    {
        if(cell->data.checkbox.image != NULL)
            UnloadBitmap(cell->data.checkbox.image);
        cell->data.checkbox.image = data->data.checkbox.image;
        cell->mask |= GVITEM_IMAGE;
        cell->data.checkbox.dirty = TRUE;
    }
    if(data->mask & GVITEM_MAINCONTENT)
    {
        cell->data.checkbox.checked = data->data.checkbox.checked;
        if(cell->data.checkbox.text != NULL)
            free(cell->data.checkbox.text);
        if (data->data.checkbox.text != NULL) {
            cell->data.checkbox.text = strdup(data->data.checkbox.text);
        }
        else {
            cell->data.checkbox.text = NULL; 
        }

        cell->mask |= GVITEM_MAINCONTENT;
        cell->data.checkbox.dirty = TRUE;
    }
    return 0;
}

int GridCellTypeCheckBox_copy(gvGridCellData* to, gvGridCellData* from)
{
    memcpy(to, from, sizeof(gvGridCellData));
    to->data.checkbox.handle = HWND_INVALID;
    to->data.checkbox.dirty = FALSE;
    if(from->data.checkbox.text != NULL)
        to->data.checkbox.text = strdup(from->data.checkbox.text);
    return 0;
}

int GridCellTypeCheckBox_get_text_value(gvGridCellData* cell, int row, int col, char* buf, int buf_len)
{
    int len = 4;
    if(buf != NULL && buf_len > 0)
    {
        buf[buf_len-1] = 0;
        strncpy(buf, "Yes", buf_len-1);
    }
    return len;
}

void GridCellTypeCheckBox_set_text_value(gvGridCellData* cell, int row, int col, char* text)
{
    if(strcasecmp(text, "yes") || strcasecmp(text, "checked"))
        cell->data.checkbox.checked = TRUE;
    else
        cell->data.checkbox.checked = FALSE;
    cell->data.checkbox.dirty = TRUE;
}

void GridCellTypeCheckBox_get_format(gvGridCellData* cell, HWND hWnd, HDC hdc, GridCellFormat* format)
{
    memset(format, 0, sizeof(GridCellFormat));
    format->mask = 0;
    
    //format->color_bg = GetWindowElementColor (BKC_CONTROL_DEF);
    //format->hl_color_bg = GetWindowElementColor (BKC_HILIGHT_NORMAL);
    format->color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_MAINC_THREED_BODY); 
    format->hl_color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_BGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_BGCOLOR) {
        format->color_bg = cell->data.checkbox.color_bg;
        format->hl_color_bg = ~cell->data.checkbox.color_bg;
    }
    
    //format->color_fg = GetWindowElementColor (FGC_CONTROL_NORMAL);
    //format->hl_color_fg = GetWindowElementColor (FGC_HILIGHT_NORMAL);
    format->color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_THREED_BODY); 
    format->hl_color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_FGCOLOR) {
        format->color_fg = cell->data.checkbox.color_fg;
        format->hl_color_fg = ~cell->data.checkbox.color_fg;
    }
    
    format->color_border = PIXEL_lightgray;
    format->hl_color_border = PIXEL_lightgray;
    
    if(cell->style & GVS_MULTLINE)
        format->text_format = GridCell_get_text_format(cell->style, DT_LEFT, DT_TOP);
    else
        format->text_format = GridCell_get_text_format(cell->style, DT_LEFT, DT_VCENTER);
    if(cell->mask & GVITEM_FONT && cell->data.checkbox.font != NULL)
    {
        format->font = cell->data.checkbox.font;
        format->mask |= GVITEM_FONT;
    }
}

HWND GridCellTypeCheckBox_get_control(gvGridCellData* cell, gvGridViewData* view, int row, int col)
{
    return cell->data.checkbox.handle;
}

HWND GridCellTypeCheckBox_create_checkbox_ctrl(gvGridViewData* view, gvGridCellData* cell, int row, int col, RECT* rect, const char* text)
{
    HWND ctrl;
    GridCellEditData* editdata = NULL;
    if(cell == NULL)
        return HWND_INVALID;
    if(cell->data.checkbox.handle != HWND_INVALID)
        return cell->data.checkbox.handle;
    ctrl = CreateWindow(CTRL_BUTTON,
                text,
                WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BS_NOTIFY,
                row*0x10000+col,
                rect->left, rect->top, RECTWP(rect), RECTHP(rect),
                view->hCtrl, 0);
    if(ctrl == HWND_INVALID)
    {
        cell->data.checkbox.handle = HWND_INVALID;
        return HWND_INVALID;
    }
    cell->data.checkbox.handle = ctrl;
    editdata = calloc(1, sizeof(GridCellEditData));
    editdata->view = view;
    editdata->cell = cell;
    editdata->row = row;
    editdata->col = col;
    editdata->is_default = FALSE;
    editdata->commit = FALSE;
    SetWindowAdditionalData(ctrl, (DWORD)editdata);
    SetNotificationCallback(ctrl, GridCell_CheckBoxNotify);
    editdata->old_proc = SetWindowCallbackProc(ctrl, GridCell_EditBgColorProc);
    UpdateWindow(ctrl, TRUE);
    return ctrl;
}

int GridCellTypeCheckBox_draw_content(gvGridCellData* cell, gvGridViewData* view, HDC hdc,
                                  int row, int col, RECT* rect, GridCellFormat* format)
{
    RECT r;
    RECT r_bound;
    HWND ctrl = cell->data.checkbox.handle;
    
    GridCell_get_ctrl_rect(cell, view, row, col, rect, &r);
    GetBoundRect(&r_bound, &r, &view->cells_rect);
    
    if(EqualRect(&r_bound, &view->cells_rect))
    {
        int checked = cell->data.checkbox.checked?BST_CHECKED:BST_UNCHECKED;
        if(ctrl == HWND_INVALID)
        {
            char* text = cell->data.checkbox.text;
            if(text == NULL)
                text = "Yes/No";
            ctrl = GridCellTypeCheckBox_create_checkbox_ctrl(view, cell, row, col, &r, text);
        }
        else
            MoveWindow(ctrl, r.left, r.top, RECTW(r), RECTH(r), TRUE);
        
        UpdateWindow(ctrl, TRUE);

        SendMessage(ctrl, BM_SETCHECK, checked, 0);
    }
    else
        cell->func->invisible_draw(cell, view, hdc, row, col, rect);
    return 0;
}

int GridCellTypeCheckBox_invisible_draw(gvGridCellData* cell, gvGridViewData* view,
                                    HDC hdc, int row, int col, RECT* rect)
{
    HWND ctrl = cell->data.checkbox.handle;
    if(ctrl != HWND_INVALID)
    {
        GridCellEditData* editdata = (GridCellEditData*)GetWindowAdditionalData(ctrl);
        DestroyWindow(ctrl);
        cell->data.checkbox.handle = HWND_INVALID;
        free(editdata);
    }
    return 0;
}

int GridCellTypeCheckBox_begin_edit(gvGridCellData* cell, gvGridViewData* view,
                                        int row, int col, RECT* rect)
{
    return 0;
}

int GridCellTypeCheckBox_end_edit(gvGridCellData* cell, gvGridViewData* view, int row, int col)
{
    int checked;
    GridCellEditData* editdata;
    HWND ctrl;
    if(cell == NULL)
        return -1;
    ctrl = cell->data.checkbox.handle;
    if (ctrl == HWND_INVALID)
        return -1;
    editdata = (GridCellEditData*)GetWindowAdditionalData(ctrl);
    if(editdata->commit)
    {
        checked = SendMessage(ctrl, BM_GETCHECK, 0, 0);
        cell->data.checkbox.checked = (checked == BST_CHECKED);
    }
    else
    {
        checked = cell->data.checkbox.checked?BST_CHECKED:BST_UNCHECKED;
        SendMessage(ctrl, BM_SETCHECK, checked, 0);
    }
    return 0;
}

int GridCellTypeCheckBox_on_destroy(gvGridCellData* cell)
{
    if(cell->data.checkbox.text != NULL)
    {
        free(cell->data.checkbox.text);
        cell->data.checkbox.text = NULL;
    }
    if(cell->data.checkbox.handle != HWND_INVALID)
    {
        HWND ctrl = cell->data.checkbox.handle;
        GridCellEditData* editdata = (GridCellEditData*)GetWindowAdditionalData(ctrl);
        DestroyWindow(ctrl);
        cell->data.checkbox.handle = HWND_INVALID;
        free(editdata);
    }
    return 0;
}

// }}}

// Selection Cell Type functions {{{

int GridCellTypeSelection_init(gvGridCellData* cell)
{
    memset(cell, 0, sizeof(gvGridCellData));
    cell->celltype = GV_TYPE_SELECTION;
    cell->func = gvGridCellTypeSelection;
    cell->data.selection.handle = HWND_INVALID;
    return 0;
}

int GridCellTypeSelection_merge(gvGridCellData* cell, gvGridCellData* data)
{
    if(cell == NULL || data == NULL)
        return -1;
    if(cell->celltype != data->celltype)
        return -1;
    if(cell->func != data->func)
        return -1;

    if(data->mask & GVITEM_STYLE)
    {
        cell->style = data->style;
        cell->data.selection.dirty = TRUE;
    }

    if(data->mask & GVITEM_BGCOLOR)
    {
        cell->data.selection.color_bg = data->data.selection.color_bg;
        cell->mask |= GVITEM_BGCOLOR;
    }
    if(data->mask & GVITEM_FGCOLOR)
    {
        cell->data.selection.color_fg = data->data.selection.color_fg;
        cell->mask |= GVITEM_FGCOLOR;
    }
    if(data->mask & GVITEM_FONT)
    {
        if (cell->data.selection.font != NULL)
            DestroyLogFont(cell->data.selection.font);
        cell->data.selection.font = data->data.selection.font;
        cell->mask |= GVITEM_FONT;
    }
    if(data->mask & GVITEM_IMAGE)
    {
        if(cell->data.selection.image != NULL)
            UnloadBitmap(cell->data.selection.image);
        cell->data.selection.image = data->data.selection.image;
        cell->mask |= GVITEM_IMAGE;
        cell->data.selection.dirty = TRUE;
    }
    if(data->mask & GVITEM_MAINCONTENT)
    {
        cell->data.selection.cur_index = data->data.selection.cur_index;
        if(cell->data.selection.text != NULL)
            free(cell->data.selection.text);
        if (data->data.selection.text != NULL) {
            cell->data.selection.text = strdup(data->data.selection.text);
        }
        else {
            cell->data.selection.text = NULL;
        }
        cell->mask |= GVITEM_MAINCONTENT;
        cell->data.selection.dirty = TRUE;
    }
    return 0;
}

int GridCellTypeSelection_copy(gvGridCellData* to, gvGridCellData* from)
{
    memcpy(to, from, sizeof(gvGridCellData));
    to->data.selection.handle = HWND_INVALID;
    to->data.selection.dirty = FALSE;
    if(from->data.selection.text != NULL)
        to->data.selection.text = strdup(from->data.selection.text);
    return 0;
}

int GridCellTypeSelection_get_text_value(gvGridCellData* cell, int row, int col, char* buf, int buf_len)
{
    int len = 4;
    if(buf != NULL && buf_len > 0)
    {
        buf[buf_len-1] = 0;
        strncpy(buf, "0\n1\n2\n", buf_len-1);
    }
    return len;
}

void GridCellTypeSelection_set_text_value(gvGridCellData* cell, int row, int col, char* text)
{
// to change
    return;
}

void GridCellTypeSelection_get_format(gvGridCellData* cell, HWND hWnd, HDC hdc, GridCellFormat* format)
{
    memset(format, 0, sizeof(GridCellFormat));
    format->mask = 0;
    
    //format->color_bg = GetWindowElementColor (BKC_CONTROL_DEF);
    //format->hl_color_bg = GetWindowElementColor (BKC_HILIGHT_NORMAL);
    format->color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_MAINC_THREED_BODY); 
    format->hl_color_bg = GetWindowElementPixelEx(hWnd, hdc, WE_BGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_BGCOLOR) {
        format->color_bg = cell->data.selection.color_bg;
        format->hl_color_bg = ~cell->data.selection.color_bg;
    }
    
    //format->color_fg = GetWindowElementColor (FGC_CONTROL_NORMAL);
    //format->hl_color_fg = GetWindowElementColor (FGC_HILIGHT_NORMAL);
    format->color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_THREED_BODY); 
    format->hl_color_fg = GetWindowElementPixelEx(hWnd, hdc, WE_FGC_HIGHLIGHT_ITEM); 
    if(cell->mask & GVITEM_FGCOLOR) {
        format->color_fg = cell->data.selection.color_fg;
        format->hl_color_fg = ~cell->data.selection.color_fg;
    }
    
    format->color_border = PIXEL_lightgray;
    format->hl_color_border = PIXEL_lightgray;
    
    if(cell->style & GVS_MULTLINE)
        format->text_format = GridCell_get_text_format(cell->style, DT_LEFT, DT_TOP);
    else
        format->text_format = GridCell_get_text_format(cell->style, DT_LEFT, DT_VCENTER);
    if(cell->mask & GVITEM_FONT && cell->data.selection.font != NULL)
    {
        format->font = cell->data.selection.font;
        format->mask |= GVITEM_FONT;
    }
}

HWND GridCellTypeSelection_get_control(gvGridCellData* cell, gvGridViewData* view, int row, int col)
{
    return cell->data.selection.handle;
}

HWND GridCellTypeSelection_create_selection_ctrl(gvGridViewData* view, gvGridCellData* cell,
                                            int row, int col, RECT* rect, const char* text)
{
    HWND ctrl;
    GridCellEditData* editdata = NULL;
    char* content = strdup(text);
    char* tok = content;
    char *tmp = NULL;
        
    if(cell == NULL)
        return HWND_INVALID;
    if(cell->data.selection.handle != HWND_INVALID)
        return cell->data.selection.handle;

    ctrl = CreateWindow(CTRL_COMBOBOX,
                0,
                WS_VISIBLE|CBS_DROPDOWNLIST|CBS_READONLY|CBS_NOTIFY|CBS_EDITNOBORDER,
                row*0x10000+col,
                rect->left, rect->top, RECTWP(rect), RECTHP(rect),
                view->hCtrl, 0);

    if(ctrl == HWND_INVALID) {
        cell->data.selection.handle = HWND_INVALID;
        return HWND_INVALID;
    }

    do {
        tmp = tok;
        tok = strchr(tok, '\n');
        if(tok == NULL)
            break;
        *tok = 0;
        SendMessage(ctrl, CB_ADDSTRING, 0, (LPARAM)tmp);
        tok++;
    } while(*tok != 0);
    
    free(content);
    
    SendMessage(ctrl, CB_SETCURSEL, cell->data.selection.cur_index, 0);
    
    cell->data.selection.handle = ctrl;
    editdata = calloc(1, sizeof(GridCellEditData));
    editdata->view = view;
    editdata->cell = cell;
    editdata->row = row;
    editdata->col = col;
    editdata->is_default = FALSE;
    editdata->commit = FALSE;
    SetWindowAdditionalData(ctrl, (DWORD)editdata);
    SetNotificationCallback(ctrl, GridCell_SelectionNotify);
    editdata->old_proc = SetWindowCallbackProc(ctrl, GridCell_EditBgColorProc);
    UpdateWindow(ctrl, TRUE);
    return ctrl;
}

int GridCellTypeSelection_draw_content(gvGridCellData* cell, gvGridViewData* view, HDC hdc,
                                  int row, int col, RECT* rect, GridCellFormat* format)
{
    RECT r;
    RECT r_bound;
    HWND ctrl = cell->data.selection.handle;
    GridCell_get_ctrl_rect(cell, view, row, col, rect, &r);
    GetBoundRect(&r_bound, &r, &view->cells_rect);
    if(EqualRect(&r_bound, &view->cells_rect))
    {
        if(ctrl == HWND_INVALID)
        {
            char* text = cell->data.selection.text;
            if(text == NULL)
                text = "Yes\nNo\n";
            ctrl = GridCellTypeSelection_create_selection_ctrl(view, cell, row, col, &r, text);
        }
        else
            MoveWindow(ctrl, r.left, r.top, RECTW(r), RECTH(r), TRUE);

        UpdateWindow(ctrl, TRUE);
    }
    else
        cell->func->invisible_draw(cell, view, hdc, row, col, rect);
    return 0;
}

int GridCellTypeSelection_invisible_draw(gvGridCellData* cell, gvGridViewData* view,
                                    HDC hdc, int row, int col, RECT* rect)
{
    HWND ctrl = cell->data.selection.handle;
    if(ctrl != HWND_INVALID)
    {
        GridCellEditData* editdata = (GridCellEditData*)GetWindowAdditionalData(ctrl);
        DestroyWindow(ctrl);
        cell->data.selection.handle = HWND_INVALID;
        free(editdata);
    }
    return 0;
}

int GridCellTypeSelection_begin_edit(gvGridCellData* cell, gvGridViewData* view,
                                        int row, int col, RECT* rect)
{
    return 0;
}

int GridCellTypeSelection_end_edit(gvGridCellData* cell, gvGridViewData* view, int row, int col)
{
    GridCellEditData* editdata;
    HWND ctrl;
    int index;
    if(cell == NULL)
        return -1;
    ctrl = cell->data.selection.handle;
    if(ctrl == HWND_INVALID)
        return -1;
    editdata = (GridCellEditData*)GetWindowAdditionalData(ctrl);
    index = SendMessage(ctrl, CB_GETCURSEL, 0, 0);
    if(editdata->commit)
    {
        index = SendMessage(ctrl, CB_GETCURSEL, 0, 0);
        cell->data.selection.cur_index = index;
    }
    else
        cell->data.selection.cur_index = 0;
    if(view->hEdit == ctrl)
        view->hEdit = HWND_INVALID;
    return 0;
}

int GridCellTypeSelection_on_destroy(gvGridCellData* cell)
{
    if(cell->data.selection.text != NULL)
    {
        free(cell->data.selection.text);
        cell->data.selection.text = NULL;
    }
    if(cell->data.selection.handle != HWND_INVALID)
    {
        HWND ctrl = cell->data.selection.handle;
        GridCellEditData* editdata = (GridCellEditData*)GetWindowAdditionalData(ctrl);
        DestroyWindow(ctrl);
        cell->data.selection.handle = HWND_INVALID;
        free(editdata);
    }
    return 0;
}

// }}}

static GridCellFunc _gvGridCellTypeNull =
{
    GridCellTypeNull_init,
    GridCellTypeNull_merge,
    GridCellTypeNull_copy,
    GridCellTypeNull_get_text_value,
    GridCellTypeNull_set_text_value,
    GridCellTypeNull_get_double_value,
    GridCellTypeNull_get_control,
    GridCellTypeNull_get_format,
    GridCellTypeNull_draw,
    GridCellTypeNull_draw_content,
    GridCellTypeNull_invisible_draw,
    GridCellTypeNull_begin_edit,
    GridCellTypeNull_end_edit,
    GridCellTypeNull_on_destroy
};
GridCellFunc* gvGridCellTypeNull = &_gvGridCellTypeNull;

static GridCellFunc _gvGridCellTypeTableHeader =
{
    GridCellTypeTableHeader_init,
    GridCellTypeNull_merge,
    GridCellTypeTableHeader_copy,
    GridCellTypeTableHeader_get_text_value,
    GridCellTypeTableHeader_set_text_value,
    GridCellTypeNull_get_double_value,
    GridCellTypeNull_get_control,
    GridCellTypeTableHeader_get_format,
    GridCellTypeNull_draw,
    GridCellTypeNull_draw_content,
    GridCellTypeNull_invisible_draw,
    GridCellTypeNull_begin_edit,
    GridCellTypeNull_end_edit,
    GridCellTypeTableHeader_on_destroy
};
GridCellFunc *gvGridCellTypeTableHeader = &_gvGridCellTypeTableHeader;

static GridCellFunc _gvGridCellTypeHeader =
{
    GridCellTypeHeader_init,
    GridCellTypeHeader_merge,
    GridCellTypeHeader_copy,
    GridCellTypeHeader_get_text_value,
    GridCellTypeHeader_set_text_value,
    GridCellTypeNull_get_double_value,
    GridCellTypeNull_get_control,
    GridCellTypeHeader_get_format,
    GridCellTypeHeader_draw,
    GridCellTypeNull_draw_content,
    GridCellTypeNull_invisible_draw,
    GridCellTypeNull_begin_edit,
    GridCellTypeNull_end_edit,
    GridCellTypeHeader_on_destroy
};
GridCellFunc *gvGridCellTypeHeader = &_gvGridCellTypeHeader;

static GridCellFunc _gvGridCellTypeText =
{
    GridCellTypeText_init,
    GridCellTypeText_merge,
    GridCellTypeText_copy,
    GridCellTypeText_get_text_value,
    GridCellTypeText_set_text_value,
    GridCellTypeNull_get_double_value,
    GridCellTypeNull_get_control,
    GridCellTypeText_get_format,
    GridCellTypeNull_draw,
    GridCellTypeText_draw_content,
    GridCellTypeNull_invisible_draw,
    GridCellTypeText_begin_edit,
    GridCellTypeText_end_edit,
    GridCellTypeText_on_destroy
};
GridCellFunc* gvGridCellTypeText = &_gvGridCellTypeText;

static GridCellFunc _gvGridCellTypeNumber =
{
    GridCellTypeNumber_init,
    GridCellTypeNumber_merge,
    GridCellTypeNumber_copy,
    GridCellTypeNumber_get_text_value,
    GridCellTypeNumber_set_text_value,
    GridCellTypeNumber_get_double_value,
    GridCellTypeNull_get_control,
    GridCellTypeNumber_get_format,
    GridCellTypeNull_draw,
    GridCellTypeNumber_draw_content,
    GridCellTypeNull_invisible_draw,
    GridCellTypeText_begin_edit,
    GridCellTypeText_end_edit,
    GridCellTypeNumber_on_destroy
};
GridCellFunc* gvGridCellTypeNumber = &_gvGridCellTypeNumber;

static GridCellFunc _gvGridCellTypeCheckBox =
{
    GridCellTypeCheckBox_init,
    GridCellTypeCheckBox_merge,
    GridCellTypeCheckBox_copy,
    GridCellTypeCheckBox_get_text_value,
    GridCellTypeCheckBox_set_text_value,
    GridCellTypeNull_get_double_value,
    GridCellTypeCheckBox_get_control,
    GridCellTypeCheckBox_get_format,
    GridCellTypeNull_draw,
    GridCellTypeCheckBox_draw_content,
    GridCellTypeCheckBox_invisible_draw,
    GridCellTypeCheckBox_begin_edit,
    GridCellTypeCheckBox_end_edit,
    GridCellTypeCheckBox_on_destroy
};
GridCellFunc* gvGridCellTypeCheckBox = &_gvGridCellTypeCheckBox;

static GridCellFunc _gvGridCellTypeSelection =
{
    GridCellTypeSelection_init,
    GridCellTypeSelection_merge,
    GridCellTypeSelection_copy,
    GridCellTypeSelection_get_text_value,
    GridCellTypeSelection_set_text_value,
    GridCellTypeNull_get_double_value,
    GridCellTypeSelection_get_control,
    GridCellTypeSelection_get_format,
    GridCellTypeNull_draw,
    GridCellTypeSelection_draw_content,
    GridCellTypeSelection_invisible_draw,
    GridCellTypeSelection_begin_edit,
    GridCellTypeSelection_end_edit,
    GridCellTypeSelection_on_destroy
};
GridCellFunc* gvGridCellTypeSelection = &_gvGridCellTypeSelection;


#endif /* _MGCTRL_GRIDVIEW */

