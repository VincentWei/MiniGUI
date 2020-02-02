///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** $Id: zorder.h 13674 2010-12-06 06:45:01Z wanzheng $
**
** zorder.h: this head file for zorder structures.
*/

#ifndef GUI_ZORDER_H
    #define GUI_ZORDER_H

#include "constants.h"

typedef struct _RECT4MASK {
    unsigned short left, top, right, bottom;
} RECT4MASK;

typedef struct _MASKRECT {
    unsigned short left, top, right, bottom;
    /* It's index value.*/
    unsigned short next;
    /* It's index value.*/
    unsigned short prev;
} MASKRECT;
typedef MASKRECT* PMASKRECT;

/* This structure should keep the same layout with ZNODEHEADER */
typedef struct _ZORDERNODE {
    DWORD           flags;          /* znode flags */
    char            *caption;       /* caption */
    HWND            hwnd;           /* which window of the client? */
    HWND            main_win;       /* handle to the main window */

    RECT            rc;             /* rect on the screen */
    int             cli;            /* which client? */
    unsigned int    age;            /* znode age */

#ifdef _MGSCHEMA_COMPOSITING
    unsigned int    changes;        /* count for changes of content */
    int             ct;             /* the compositing type */
    DWORD           ct_arg;         /* the argument for compositing */
    HDC             mem_dc;         /* the memory DC for this znode */
    unsigned int    dirty_age;      /* the dirty age of this znode */
    int             nr_dirty_rcs;   /* the number of dirty rects */
    const RECT*     dirty_rcs;      /* the pointer to the dirty rectangles */
#else   /* defined _MGSCHEMA_COMPOSITING */
    RECT            dirty_rc;       /* dirty rect */
#endif  /* not defined _MGSCHEMA_COMPOSITING */

    int             idx_mask_rect;  /* the first position of the mask rects. */

    int             next;
    int             prev;

    void*           priv_data;      /* the private data of this znode. */
} ZORDERNODE;
typedef ZORDERNODE* PZORDERNODE;

typedef struct _ZORDERINFO {
    int             size_usage_bmp;

    int             max_nr_popupmenus;
    int             max_nr_globals;
    int             max_nr_topmosts;
    int             max_nr_normals;

    int             nr_popupmenus;
    int             nr_globals;
    int             nr_topmosts;
    int             nr_normals;

    int             first_global;
    int             first_topmost;
    int             first_normal;

    int             active_win;

    int             cli_trackmenu;

    /* The size of usage bitmap for mask rect. */
    int             size_maskrect_usage_bmp;

#ifdef _MGRM_THREADS
# ifndef __NOUNIX__
    pthread_rwlock_t rwlock;
# else
    pthread_mutex_t  rwlock;
# endif
#elif defined(_MGRM_PROCESSES)
    int             zi_semid;
    int             zi_semnum;
#endif

    /* move to last for alignment */
    HWND            ptmi_in_cli;
} ZORDERINFO;
typedef ZORDERINFO* PZORDERINFO;

#define GET_ZORDERNODE(zi) (ZORDERNODE*)((char*)((zi)+1)+ \
        (zi)->size_usage_bmp + \
        sizeof(ZORDERNODE)*(DEF_NR_POPUPMENUS))

#define GET_MENUNODE(zi) \
        (ZORDERNODE*)((char*)((zi)+1)+ \
                (zi)->size_usage_bmp)

#if defined(_MGRM_THREADS) || defined(_MGRM_STANDALONE)
#   define SHAREDRES_NR_GLOBALS        0
#endif

#define GET_MASKRECT_USAGEBMP(zi) \
        ((char *)((zi) + 1) +  \
                (zi)->size_usage_bmp + \
            sizeof (ZORDERNODE) *(DEF_NR_POPUPMENUS + \
                SHAREDRES_NR_GLOBALS +zi->max_nr_topmosts + \
                (zi)->max_nr_normals))

#define GET_MASKRECT(zi) ((MASKRECT *)( \
                GET_MASKRECT_USAGEBMP(zi) + \
                (zi)->size_maskrect_usage_bmp))

/* Since 4.2.0: maximal number of fixed znodes: 4 */
#define SIZE_USAGE_BMP \
        ROUND_TO_MULTIPLE((7 + SHAREDRES_NR_GLOBALS + nr_topmosts + nr_normals + \
            DEF_NR_FIXEDZNODES) / 8, 8)

#define SIZE_MASKRECT_USAGE_BMP \
        ROUND_TO_MULTIPLE((7 + DEF_NR_MASKRECT) / 8, 8)

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int __kernel_alloc_z_order_info (int nr_topmosts, int nr_normals);
void __kernel_free_z_order_info (ZORDERINFO* zi);
int __kernel_change_z_order_mask_rect (HWND pWin, const RECT4MASK* rc, int nr_rc);
int __kernel_get_window_region (HWND pWin, CLIPRGN* region);
int __kernel_get_next_znode (const ZORDERINFO* zi, int from);
int __kernel_get_prev_znode (const ZORDERINFO* zi, int from);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_ZORDER_H

