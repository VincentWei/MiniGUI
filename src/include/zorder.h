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
** $Id: zorder.h 13674 2010-12-06 06:45:01Z wanzheng $
**
** zorder.h: this head file for zorder structures.
*/

#ifndef GUI_ZORDER_H
    #define GUI_ZORDER_H

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

#define ZOF_REFERENCE       0x00000001
#define ZOF_VISIBLE         0x00000002
#define ZOF_DISABLED        0x00000004
#define ZOF_FLAG_MASK       0x0000000F

#define ZOF_TYPE_DESKTOP    0x50000000
#define ZOF_TYPE_MENU       0x40000000
#define ZOF_TYPE_GLOBAL     0x30000000
#define ZOF_TYPE_TOPMOST    0x20000000
#define ZOF_TYPE_NORMAL     0x10000000
#define ZOF_TYPE_MASK       0xF0000000

#define ZOF_TF_MAINWIN      0x01000000
#define ZOF_TF_TOOLWIN      0x02000000
/* TOPFOREVER can be composite with other type.*/
#define ZOF_TF_TOPFOREVER   0x04000000

#define ZOF_TW_TBROUNDCNS   0x00300000
#define ZOF_TW_BROUNDCNS    0x00200000
#define ZOF_TW_TROUNDCNS    0x00100000

#define ZOF_TW_FLAG_MASK    0x00F00000
#define ZOF_TYPE_FLAG_MASK  0xFFF00000

#ifndef MAX_CAPTION_LEN
#define MAX_CAPTION_LEN     39 
#endif 

typedef struct _ZORDERNODE
{
    DWORD           flags;

    char            caption[MAX_CAPTION_LEN+1];

    RECT            rc;
    RECT            dirty_rc;
    unsigned int    age;

    int             cli;    /* which client? */
    HWND            fortestinghwnd;   /* which window of the client? */
    HWND            main_win;   /* handle to the main window */

    int             next;
    int             prev;

    /*The first position of mask rect.*/
    int             idx_mask_rect; 
} ZORDERNODE;
typedef ZORDERNODE* PZORDERNODE;

typedef struct _ZORDERINFO
{
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
    HWND            ptmi_in_cli;

    int             zi_semid;
    int             zi_semnum;

    /* The usage bitmap for mask rect. */
    int             size_maskrect_usage_bmp;
#ifdef _MGRM_THREADS
#ifndef __NOUNIX__
    pthread_rwlock_t rwlock;
#else
    pthread_mutex_t  rwlock;
#endif
#endif
} ZORDERINFO;
typedef ZORDERINFO* PZORDERINFO;

#define DEF_NR_MASKRECT         1024
#define DEF_NR_POPUPMENUS       16
#define DEF_NR_TOPMOSTS         16
#define DEF_NR_NORMALS          128

#if defined(_MGRM_THREADS) || defined(_MGRM_STANDALONE)
#define SHAREDRES_NR_GLOBALS    16//0 
#endif

#define GET_ZORDERNODE(zi) (ZORDERNODE*)((char*)((zi)+1)+ \
        (zi)->size_usage_bmp + \
        sizeof(ZORDERNODE)*(DEF_NR_POPUPMENUS))

#define GET_MENUNODE(zi) \
        (ZORDERNODE*)((char*)((zi)+1)+ \
                (zi)->size_usage_bmp)

int kernel_alloc_z_order_info (int nr_topmosts, int nr_normals);
void kernel_free_z_order_info (ZORDERINFO* zi);
int kernel_change_z_order_mask_rect (HWND pWin, const RECT4MASK* rc, int nr_rc);
int kernel_get_window_region (HWND pWin, CLIPRGN* region);

#define GET_MASKRECT_USAGEBMP(zi) \
        ((char *)((zi) + 1) +  \
                (zi)->size_usage_bmp + \
            sizeof (ZORDERNODE) *(DEF_NR_POPUPMENUS + \
                SHAREDRES_NR_GLOBALS +zi->max_nr_topmosts + \
                (zi)->max_nr_normals))

#define GET_MASKRECT(zi) ((MASKRECT *)( \
                GET_MASKRECT_USAGEBMP(zi) + \
                (zi)->size_maskrect_usage_bmp))

#define SIZE_USAGE_BMP \
        ((7 + SHAREDRES_NR_GLOBALS + nr_topmosts + nr_normals) / 8)

#define SIZE_MASKRECT_USAGE_BMP \
        ((7 + DEF_NR_MASKRECT) / 8)

#endif // GUI_ZORDER_H

