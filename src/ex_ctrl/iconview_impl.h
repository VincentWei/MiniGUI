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
** iconview.h: header file of ScrollView control.
*/

#ifndef _ICONVIEW_IMPL_H_
#define _ICONVIEW_IMPL_H_


#include "listmodel.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*
#define IVST_NORMAL     0x0000
#define IVST_FROZEN     0x0001L
*/

/* default size and position values */
#define HSCROLL                 5   // h scroll value
#define VSCROLL                 15  // v scroll value

#if 1
#define IV_LEFTMARGIN           5
#define IV_TOPMARGIN            5
#define IV_RIGHTMARGIN          5
#define IV_BOTTOMMARGIN         5
#else
#define IV_LEFTMARGIN           0
#define IV_TOPMARGIN            0
#define IV_RIGHTMARGIN          0
#define IV_BOTTOMMARGIN         0
#endif

#define IV_DEF_ITEMWIDTH       48
#define IV_DEF_ITEMHEIGHT      48


typedef SVITEMOPS IVITEMOPS;

/* iconview item struct */
typedef struct _IconviewItem
{
    MgItem    mgitem;

    //int       nItemWidth;   // height of an item
    //DWORD     flags;         // item flags
    PBITMAP   bmp;
    char*     label;
    DWORD     addData;       // item additional data
} IconviewItem;

typedef struct _IconviewList
{
    MgList mglist;

    /* icon box width */
    int nItemWidth;
    /* icon box height */
    int nItemHeight;

    int nCol;

    //DWORD flags;

} IconviewList;

typedef struct _IconviewData
{
    /* must be the first member */
    SCRDATA scrdata;

    /* iconview item list, list data model */
    IconviewList ivlist;

    /* icon view flags, status ... */
    DWORD flags;

} IconviewData;


#define pivscr ((PSCRDATA)pivdata)
#define hivwnd (pivscr->hSV)

/* ------------------------- external api -------------------------- */

int     iconview_init               (HWND hWnd, IconviewData* psv);
void    iconview_destroy            (IconviewData* pivdata);

HITEM   iconview_add_item           (HWND hWnd, IconviewData* pivdata, HITEM hsvi, PIVITEMINFO pii, int *idx);
HITEM   iconview_add_item_ex        (HWND hWnd, IconviewData* pivdata, HITEM hsvi, 
                                       HITEM nexthsvi, PIVITEMINFO pii, int *idx);
int     iconview_del_item           (HWND hWnd, IconviewData* pivdata, int nItem, HITEM hsvi);

void    iconview_reset_content      (HWND hWnd, IconviewData* pivdata);
int     iconview_is_in_item         (IconviewData* pivdata, int mousex, int mousey, HITEM *phsvi);
void    iconview_set_ivlist         (HWND hWnd, PSCRDATA pscrdata, BOOL visChanged);

int iconview_get_item_pos (IconviewData* pivdata, HITEM hsvi, int *x, int *y);

static inline void iconview_set_autosort (IconviewData* pivdata)
{
    mglist_set_autosort ((MgList *)&pivdata->ivlist);
}

static inline int iconview_get_total_item_height (IconviewData* pivdata)
{
    return ((MgList *)&pivdata->ivlist)->nTotalItemH;
}

static inline void iconview_select_item (IconviewData* pivdata, HITEM hsvi, BOOL bSel)
{
    if (!hsvi) return;
    mglist_select_item ((MgList*)&pivdata->ivlist, (MgItem *)hsvi, bSel);
}

/* unselect all and select this one */
static inline void iconview_select_one_item (IconviewData* pivdata, HITEM hsvi)
{
    if (!hsvi) return;

    mglist_unselect_all ((MgList *)&pivdata->ivlist);
    mglist_select_item ((MgList*)&pivdata->ivlist, (MgItem *)hsvi, TRUE);
}

static inline void iconview_unselect_all (IconviewData* pivdata)
{
    mglist_unselect_all ((MgList *)&pivdata->ivlist);
}

static inline void iconview_select_all (IconviewData* pivdata)
{
    mglist_select_all ((MgList*)&pivdata->ivlist);
}

static inline int iconview_get_item_count (IconviewData* pivdata)
{
    return mglist_get_item_count((MgList *)&pivdata->ivlist);
}

static inline HITEM iconview_get_hilighted_item (IconviewData* pivdata)
{
    return (HITEM)mglist_get_hilighted_item((MgList *)&pivdata->ivlist);
}

static inline HITEM iconview_get_item_by_index (IconviewData* pivdata, int nItem)
{
    return (HITEM)mglist_getitem_byindex ((MgList *)&pivdata->ivlist, nItem);
}

static inline HITEM iconview_get_next_item (IconviewData* pivdata, HITEM hivi)
{
    return (HITEM)mglist_get_next_item((MgList *)&pivdata->ivlist, (MgItem *)hivi);
}

static inline HITEM iconview_get_prev_item (IconviewData* pivdata, HITEM hivi)
{
    return (HITEM)mglist_get_prev_item((MgList *)&pivdata->ivlist, (MgItem *)hivi);
}


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _ICONVIEW_IMPL_H_ */

