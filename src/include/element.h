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
** element.h: the head file of window element data.
** 
** Create date: 2004/05/10
*/

#ifndef GUI_ELEMENT_H
    #define GUI_ELEMENT_H

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* window element data struct */
typedef struct _wnd_element_data
{
    list_t      list;       /* list pointer */
    UINT        id;         /* the item type */
    DWORD       data;       /* the data of the item */
} WND_ELEMENT_DATA;

#define WED_OK              0
#define WED_NEW_DATA        1
#define WED_NOT_CHANGED     2

#define WED_INVARG          -1
#define WED_NODEFINED       -2
#define WED_NOTFOUND        -3
#define WED_MEMERR          -4

extern int free_window_element_data (HWND hwnd);
extern int append_window_element_data (WND_ELEMENT_DATA** wed, Uint32 id, DWORD data);
extern int set_window_element_data (HWND hwnd, Uint32 id, DWORD new_data, DWORD* old_data);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_ELEMENT_H */


