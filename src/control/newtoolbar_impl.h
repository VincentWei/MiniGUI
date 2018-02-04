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
** newtoolbar.h: the head file of NewToolBar control module.
**
** Create date: 2003/04/24
*/


#ifndef __NEWTOOLBAR_IMPL_H_
#define __NEWTOOLBAR_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#define WIDTH_SEPARATOR     5

#define GAP_BMP_TEXT_HORZ   2
#define GAP_BMP_TEXT_VERT   2

#define GAP_ITEM_ITEM_HORZ  2
#define GAP_ITEM_ITEM_VERT  2

#define MARGIN_HORZ         9

#define MARGIN_VERT         2
#define MARGIN_V_HORZ       2
#define MARGIN_V_VERT       9

typedef struct ntbItem
{
    struct ntbItem *next;
    struct ntbItem *prev;
    DWORD flags;
    int id;
    int bmp_cell;
    char text[NTB_TEXT_LEN + 1];
    char tip[NTB_TIP_LEN + 1];
    RECT rc_item;
    RECT rc_text;
    RECT rc_hotspot;
    HOTSPOTPROC hotspot_proc;
    DWORD add_data;
} NTBITEM;
typedef NTBITEM* PNTBITEM;

typedef struct ntbCtrlData {
    NTBITEM*        head;
    NTBITEM*        tail;

    DWORD           style;

    BITMAP*         image;
    int             nr_cells;
    int             nr_cols;

    int             w_cell;
    int             h_cell;

    int             nr_items;

    NTBITEM*        sel_item;
    BOOL            btn_down;
	HWND			hToolTip;
} NTBCTRLDATA;
typedef NTBCTRLDATA* PNTBCTRLDATA;

BOOL RegisterNewToolbarControl (void);

#ifdef  __cplusplus
}
#endif

#endif /* __NEWTOOLBAR_IMPL_H_ */


