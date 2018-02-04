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
** propsheet.h: the head file of PropSheet control.
**
** Originally by Wang Jian and Jiang Jun.
**
** Create date: 2001/11/19
*/

#ifndef GUI_PROPSHEET_IMPL_H_
#define GUI_PROPSHEET_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

typedef struct tagPROPPAGE
{
    char* title;                /* title of page */
    HICON icon;                 /* icon of page */
    int  width;                 /* width of title */
    HWND hwnd;                  /* container control */
    WNDPROC proc;               /* callback of page */

    struct tagPROPPAGE* next;   /* next page */
} PROPPAGE;
typedef PROPPAGE* PPROPPAGE;

typedef struct tagPROPSHEETDATA
{
    RECT     head_rc;            /* height of page title */
    int      head_width;         /* the effective width of head */
    int      page_count;         /* the number of pages */
    PROPPAGE *active;            /* index of active page */
    PROPPAGE *first_display_page;/* index of active page */
    PROPPAGE *head;              /* head of page list */
    BOOL     overload;           /* if too much head and 
                                    too long for display, 
                                    head_overload = TRUE */
} PROPSHEETDATA;
typedef PROPSHEETDATA* PPROPSHEETDATA;

BOOL RegisterPropSheetControl (void);

#ifdef __cplusplus
}
#endif

#endif // GUI_PROPSHEET_IMPL_H_


