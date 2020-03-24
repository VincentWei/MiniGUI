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
    unsigned int    lock_count;     /* the lock count */

#ifdef _MGSCHEMA_COMPOSITING
    unsigned int    changes;        /* count for changes of content */
    int             ct;             /* the compositing type */
    DWORD           ct_arg;         /* the argument for compositing */
    HDC             mem_dc;         /* the memory DC for this znode */
    unsigned int    dirty_age;      /* the dirty age of this znode */
    int             nr_dirty_rcs;   /* the number of dirty rects */
    const RECT*     dirty_rcs;      /* the pointer to the dirty rectangles */
#else   /* defined _MGSCHEMA_COMPOSITING */
    unsigned int    age;            /* znode age */
    RECT            dirty_rc;       /* dirty rectangle */
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
    int             max_nr_nodes_in_levels[0];
    int             max_nr_tooltips;
    int             max_nr_globals;
    int             max_nr_screenlocks;
    int             max_nr_dockers;
    int             max_nr_topmosts;
    int             max_nr_normals;
    int             max_nr_launchers;

    int             nr_popupmenus;
    int             nr_nodes_in_levels[0];
    int             nr_tooltips;
    int             nr_globals;
    int             nr_screenlocks;
    int             nr_dockers;
    int             nr_topmosts;
    int             nr_normals;
    int             nr_launchers;

    int             first_in_levels[0];
    int             first_tooltip;
    int             first_global;
    int             first_screenlock;
    int             first_docker;
    int             first_topmost;
    int             first_normal;
    int             first_launcher;

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

#define MAX_NR_SPECIAL_ZNODES(zi)                           \
    (zi->max_nr_tooltips + zi->max_nr_globals +             \
     zi->max_nr_screenlocks + zi->max_nr_dockers +          \
     zi->max_nr_launchers + NR_FIXED_ZNODES)

#define MAX_NR_GENERAL_ZNODES(zi)                           \
    (zi->max_nr_topmosts + zi->max_nr_normals)

#define MAX_NR_ZNODES(zi)                                   \
    MAX_NR_SPECIAL_ZNODES(zi) + MAX_NR_GENERAL_ZNODES(zi)

#define MAX_NR_ZNODES_WITH_PPP(zi)                          \
    (MAX_NR_ZNODES(zi) + (zi)->max_nr_popupmenus)

#define GET_ZORDERNODE(zi)                                  \
    (ZORDERNODE*)((char*)((zi)+1) +                         \
            (zi)->size_usage_bmp +                          \
            sizeof(ZORDERNODE)*((zi)->max_nr_popupmenus))

#define GET_MENUNODE(zi)                                    \
    (ZORDERNODE*)((char*)((zi)+1)+ (zi)->size_usage_bmp)

#define GET_MASKRECT_USAGEBMP(zi)                           \
    ((char *)((zi) + 1) + (zi)->size_usage_bmp +            \
     sizeof (ZORDERNODE) * (MAX_NR_ZNODES_WITH_PPP(zi)))

#define GET_MASKRECT(zi)                                    \
    ((MASKRECT *)(GET_MASKRECT_USAGEBMP(zi) +               \
        (zi)->size_maskrect_usage_bmp))

/* round to multiple of 8 to aoivd alignment issue */
#define SIZE_MASKRECT_USAGE_BMP                             \
    ROUND_TO_MULTIPLE((DEF_NR_MASKRECT >> 3), 8)

#define NR_SPECIAL_ZNODES(zi)                               \
    (zi->nr_tooltips + zi->nr_globals +                     \
     zi->nr_screenlocks + zi->nr_dockers +                  \
     zi->nr_launchers + NR_FIXED_ZNODES)

#define NR_GENERAL_ZNODES(zi)                               \
    (zi->nr_topmosts + zi->nr_normals)

#define NR_ZNODES(zi)                                       \
    NR_SPECIAL_ZNODES(zi) + NR_GENERAL_ZNODES(zi)

/* Since 5.0.0: more levels  */
#define SIZE_USAGE_BMP_GENERAL(max_nr_t, max_nr_n)          \
    ((7 + (max_nr_t) + (max_nr_n)) >> 3)

#define SIZE_USAGE_BMP_SPECIAL(max_nr_g)                    \
    ((7 + DEF_NR_TOOLTIPS + max_nr_g +                      \
      DEF_NR_SCREENLOCKS + DEF_NR_DOCKERS +                 \
      DEF_NR_LAUNCHERS + NR_FIXED_ZNODES) >> 3)

/* Since 5.0.0: round to multiple of 8 to aoivd alignment issue */
#define SIZE_USAGE_BMP(max_nr_g, max_nr_t, max_nr_n)        \
    ROUND_TO_MULTIPLE(                                      \
            SIZE_USAGE_BMP_GENERAL(max_nr_t, max_nr_n) +    \
            SIZE_USAGE_BMP_SPECIAL(max_nr_g), 8)

#define LEN_USAGE_BMP_GENERAL(zi)                           \
    ((7 + MAX_NR_GENERAL_ZNODES(zi)) >> 3)

#define LEN_USAGE_BMP_SPECIAL(zi)                           \
    ((7 + MAX_NR_SPECIAL_ZNODES(zi)) >> 3)

/* Since 5.0.0: round to multiple of 8 to aoivd alignment issue */
#define LEN_USAGE_BMP(zi)                                   \
    ROUND_TO_MULTIPLE(                                      \
            (LEN_USAGE_BMP_GENERAL(zi) +                    \
             LEN_USAGE_BMP_SPECIAL(zi)), 8)

#define IS_INVALID_ZIDX(zi, idx)                            \
    MG_UNLIKELY ((idx) > MAX_NR_ZNODES(zi) || ((idx) < 0))

#define IS_INVALID_ZIDX_LOOSE(zi, idx)                      \
    MG_UNLIKELY ((idx) > MAX_NR_ZNODES(zi))

#define IS_TYPE_GENERAL(type)                               \
    (type == ZOF_TYPE_HIGHER ||                             \
     type == ZOF_TYPE_NORMAL)

#define IS_TYPE_SPECIAL(type)                               \
    (type == ZOF_TYPE_TOOLTIP ||                            \
     type == ZOF_TYPE_GLOBAL ||                             \
     type == ZOF_TYPE_SCREENLOCK ||                         \
     type == ZOF_TYPE_DOCKER ||                             \
     type == ZOF_TYPE_LAUNCHER)

#define ZOF_TYPE_TO_LEVEL_IDX(type)     ((int)(8 - ((type) >> 28)))

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int __kernel_alloc_z_order_info (int nr_topmosts, int nr_normals);
void __kernel_free_z_order_info (ZORDERINFO* zi);
int __kernel_get_window_region (HWND pWin, CLIPRGN* region);
int __kernel_get_next_znode (const ZORDERINFO* zi, int from);
int __kernel_get_prev_znode (const ZORDERINFO* zi, int from);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_ZORDER_H

