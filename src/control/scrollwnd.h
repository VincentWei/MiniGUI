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
** scrollwnd.h: header file of ScrollView control.
*/

#ifndef _SCROLLWND_H
#define _SCROLLWND_H


#include "list.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define CTRL_CONTAINER   "container"


/* default size and position values */
#define HSCROLL                 5   // h scroll value
#define VSCROLL                 15  // v scroll value

typedef struct _containerdata
{
    /* scroll view window */
    HWND hSV;
    /* container flags, status ... */
    DWORD flags;

} CONTDATA;
typedef CONTDATA* PCONTDATA;

typedef struct _swdata
{
    SCRDATA scrdata;

    /* handle of static container */
    HWND hContainer;
    /* private data of container */
    PCONTDATA pContdata;

    /* scroll view flags, status ... */
    DWORD flags;

} SWDATA;
typedef SWDATA *PSWDATA;

int     scrollwnd_init (HWND hWnd, PSWDATA psv);
void    scrollwnd_destroy (PSWDATA pswdata);
void    scrollwnd_hscroll (HWND hWnd, PSWDATA pswdata, WPARAM wParam, LPARAM lParam);
void    scrollwnd_vscroll (HWND hWnd, PSWDATA pswdata, WPARAM wParam, LPARAM lParam);
void    scrollwnd_reset_content (HWND hWnd, PSWDATA pswdata);
int     scrollwnd_SetContWidth (HWND hWnd, PSWDATA pswdata, int cont_w);
void    scrollwnd_set_cont_pos (HWND hWnd, PSWDATA pswdata, int cont_x, int cont_y);
void    scrollwnd_make_pos_visible (HWND hWnd, PSWDATA pswdata, int pos_x, int pos_y);
void    scrollwnd_refresh_container (HWND hWnd, PSWDATA pswdata);
void    scrollwnd_set_scrollval (PSWDATA pswdata, int hval, int vval);
void    scrollwnd_set_scrollpageval (PSWDATA pswdata, int hval, int vval);


static inline int scrollwnd_get_contwidth (PSWDATA pswdata)
{
    return pswdata->scrdata.nContWidth;
}

static inline WNDPROC scrollwnd_set_container_proc (PSWDATA pswdata, WNDPROC newproc)
{
    return SetWindowCallbackProc (pswdata->hContainer, newproc);
}

static inline HWND scrollwnd_get_window (HWND hcontainer)
{
    return GetParent (GetParent (hcontainer));
}

static inline void scrollwnd_refresh_content (PSWDATA pswdata)
{
    InvalidateRect (pswdata->hContainer, NULL, TRUE);
}


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _SCROLLWND_H */

