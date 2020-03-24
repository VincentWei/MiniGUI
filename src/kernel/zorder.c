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
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
** zorder.c: zorder operation set.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <math.h>

#include "common.h"

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "license.h"
#ifdef _MG_ENABLE_LICENSE
#   include "../sysres/license/c_files/03_progressbar.dat.c"
#endif
#ifndef __TARGET_UNKNOWN__
unsigned int g_license_message_offset;
#endif

#ifdef _MGRM_PROCESSES
#   include "ourhdr.h"
#   include "sockio.h"
#   include "client.h"
#   include "server.h"
#   include "sharedres.h"
#   include "drawsemop.h"
#endif
#include "misc.h"

#ifdef _MGRM_PROCESSES

#define SHM_PARAM 0644

inline static key_t get_layer_shm_key (void)
{
    static BYTE last_layer_key = 0x10;
    key_t key;

    if (last_layer_key == 0xFF)
        return -1;

    key = (key_t)(IPC_KEY_BASE + last_layer_key);

    last_layer_key ++;
    return key;
}
#endif  /* defined _MGRM_PROCESSES */

int __kernel_alloc_z_order_info (int nr_topmosts, int nr_normals)
{
#ifdef _MGRM_PROCESSES
    int size_usage_bmp = SIZE_USAGE_BMP (SHAREDRES_NR_GLOBALS,
            nr_topmosts, nr_normals);
    key_t shm_key;
    int zorder_shmid;

    if ((shm_key = get_layer_shm_key ()) == -1) {
        return -1;
    }

    zorder_shmid = shmget (shm_key,
            sizeof (ZORDERINFO) +
            size_usage_bmp +
            sizeof (ZORDERNODE) *
            (DEF_NR_POPUPMENUS +        /* for the popup menus */
                DEF_NR_TOOLTIPS +       /* for the tooltip ones */
                SHAREDRES_NR_GLOBALS +  /* for the global ones */
                DEF_NR_SCREENLOCKS +    /* for the screenlock ones */
                DEF_NR_DOCKERS +        /* for the docker ones */
                nr_topmosts +           /* for the higher windows */
                nr_normals +            /* for the normal windows */
                DEF_NR_LAUNCHERS +      /* for the launcher ones */
                NR_FIXED_ZNODES) +      /* for the fixed znodes */
            SIZE_MASKRECT_USAGE_BMP +
            sizeof (MASKRECT) * DEF_NR_MASKRECT,
            SHM_PARAM | IPC_CREAT | IPC_EXCL);

    return zorder_shmid;
#else   /* defined _MGRM_PROCESSES */
    int size_usage_bmp = SIZE_USAGE_BMP (DEF_NR_GLOBALS,
            nr_topmosts, nr_normals);
    ZORDERNODE* znodes;
    void* maskrect_usage_bmp;

    __mg_zorder_info = (PZORDERINFO) malloc (
            sizeof (ZORDERINFO) +
            size_usage_bmp + 
            sizeof (ZORDERNODE) *
            (DEF_NR_POPUPMENUS +        /* for the popup menus */
                DEF_NR_TOOLTIPS +       /* for the tooltip ones */
                DEF_NR_GLOBALS +        /* for global windows */
                DEF_NR_SCREENLOCKS +    /* for the screenlock ones */
                DEF_NR_DOCKERS +        /* for the docker ones */
                nr_topmosts +           /* for the topmost windows */
                nr_normals +            /* for the normal windows */
                DEF_NR_LAUNCHERS +      /* for the launcher ones */
                NR_FIXED_ZNODES) +      /* for the fixed znodes */
            SIZE_MASKRECT_USAGE_BMP +
            sizeof (MASKRECT) * DEF_NR_MASKRECT);

    if (!__mg_zorder_info) {
        _MG_PRINTF ("KERNEL>ZOrder: calloc zorderinfo failure. \n");
        return -1;
    }

    __mg_zorder_info->size_usage_bmp = size_usage_bmp;
    __mg_zorder_info->size_maskrect_usage_bmp = SIZE_MASKRECT_USAGE_BMP;

    __mg_zorder_info->max_nr_popupmenus = DEF_NR_POPUPMENUS;
    __mg_zorder_info->max_nr_tooltips = DEF_NR_TOOLTIPS;
    __mg_zorder_info->max_nr_globals = DEF_NR_GLOBALS;
    __mg_zorder_info->max_nr_screenlocks = DEF_NR_SCREENLOCKS;
    __mg_zorder_info->max_nr_dockers = DEF_NR_DOCKERS;
    __mg_zorder_info->max_nr_topmosts = nr_topmosts;
    __mg_zorder_info->max_nr_normals = nr_normals;
    __mg_zorder_info->max_nr_launchers = DEF_NR_LAUNCHERS;

    __mg_zorder_info->nr_popupmenus = 0;
    __mg_zorder_info->nr_tooltips = 0;
    __mg_zorder_info->nr_globals = 0;
    __mg_zorder_info->nr_screenlocks = 0;
    __mg_zorder_info->nr_dockers = 0;
    __mg_zorder_info->nr_topmosts = 0;
    __mg_zorder_info->nr_normals = 0;
    __mg_zorder_info->nr_launchers = 0;

    __mg_zorder_info->first_tooltip = 0;
    __mg_zorder_info->first_global = 0;
    __mg_zorder_info->first_screenlock = 0;
    __mg_zorder_info->first_docker = 0;
    __mg_zorder_info->first_topmost = 0;
    __mg_zorder_info->first_normal = 0;
    __mg_zorder_info->first_launcher = 0;

    __mg_zorder_info->active_win = 0;

    __mg_zorder_info->cli_trackmenu = -1;
    __mg_zorder_info->ptmi_in_cli = (HWND)-1;

    /* Set zorder node usage map. */
    memset (__mg_zorder_info + 1, 0xFF, size_usage_bmp);

    /* Set zorder mask rect usage map. */
    maskrect_usage_bmp = GET_MASKRECT_USAGEBMP(__mg_zorder_info);
    memset (maskrect_usage_bmp, 0xFF,
            __mg_zorder_info->size_maskrect_usage_bmp);

    /* init z-order node for desktop */
    znodes = GET_ZORDERNODE(__mg_zorder_info);

    znodes [0].flags = ZOF_TYPE_DESKTOP | ZOF_VISIBLE;
    znodes [0].rc = g_rcScr;
    znodes [0].age = 0;
    znodes [0].cli = 0;
    znodes [0].hwnd = HWND_DESKTOP;
    znodes [0].next = 0;
    znodes [0].prev = 0;
    znodes [0].idx_mask_rect = 0;

    __mg_slot_set_use ((unsigned char*)(__mg_zorder_info + 1), 0);
    __mg_slot_set_use ((unsigned char*)(maskrect_usage_bmp), 0);

#if 0   /* deprecated code */
    /* Since 5.0.0; allocate znodes for other fixed main windows */
    {
        int i;
        static int fixed_ztypes [] = { ZNIT_TOOLTIP };

        for (i = 0; i < TABLESIZE (fixed_ztypes); i++) {
            znodes [i + ZNIDX_FIRST].flags = fixed_ztypes [i];
#ifndef _MGSCHEMA_COMPOSITING
            znodes [i + ZNIDX_FIRST].age = 0;
            znodes [i + ZNIDX_FIRST].dirty_rc.left = 0;
            znodes [i + ZNIDX_FIRST].dirty_rc.top = 0;
            znodes [i + ZNIDX_FIRST].dirty_rc.right = 0;
            znodes [i + ZNIDX_FIRST].dirty_rc.bottom = 0;
#endif
            znodes [i + ZNIDX_FIRST].cli = -1;
            znodes [i + ZNIDX_FIRST].hwnd = HWND_NULL;
            znodes [i + ZNIDX_FIRST].next = 0;
            znodes [i + ZNIDX_FIRST].prev = 0;
            znodes [i + ZNIDX_FIRST].idx_mask_rect = 0;
            znodes [i + ZNIDX_FIRST].priv_data = NULL;

            SetRectEmpty (&znodes [i + ZNIDX_FIRST].rc);
            __mg_slot_set_use ((unsigned char*)(__mg_zorder_info + 1),
                    i + ZNIDX_FIRST);
        }
    }
#endif  /* deprecated code */

#ifdef _MGRM_THREADS
#ifndef __NOUNIX__
    pthread_rwlock_init(&__mg_zorder_info->rwlock, NULL);
#else
    pthread_mutex_init(&__mg_zorder_info->rwlock, NULL);
#endif
#endif
    return 0;
#endif
}

void __kernel_free_z_order_info (ZORDERINFO* zi)
{
#if defined(_MGRM_PROCESSES)
    if (shmdt (zi) < 0)
        perror ("Detaches shared zorder nodes");
#else

#ifdef _MGRM_THREADS
#ifndef __NOUNIX__
    pthread_rwlock_destroy(&zi->rwlock);
#else
    pthread_mutex_destroy(&zi->rwlock);
#endif
#endif
    free (zi);
    __mg_zorder_info = NULL;
#endif
}

#if 0   /* deprecated code */
#if IS_SHAREDFB_SCHEMA_PROCS

int __kernel_alloc_z_order_info (int nr_topmosts, int nr_normals)
{
    key_t shm_key;
    int zorder_shmid;

    if ((shm_key = get_layer_shm_key ()) == -1) {
        return -1;
    }

    zorder_shmid = shmget (shm_key,
                sizeof (ZORDERINFO) + SIZE_USAGE_BMP +
                sizeof (ZORDERNODE) *
                        (DEF_NR_POPUPMENUS + /* for the popup menus */
                        SHAREDRES_NR_GLOBALS + /* for the global windows */
                        nr_topmosts +       /* for the topmost windows */
                        nr_normals)+        /* for the normal windows */
                        SIZE_MASKRECT_USAGE_BMP +
                sizeof (MASKRECT) * DEF_NR_MASKRECT,
                SHM_PARAM | IPC_CREAT | IPC_EXCL);

    return zorder_shmid;
}

#else /* not IS_SHAREDFB_SCHEMA_PROCS */

ZORDERINFO* __kernel_alloc_z_order_info (int nr_topmosts, int nr_normals)
{
    ZORDERINFO* zi;
    ZORDERNODE* znodes;
    void* maskrect_usage_bmp;

    zi = (PZORDERINFO) calloc (1,
                sizeof (ZORDERINFO) + SIZE_USAGE_BMP +
                    sizeof (ZORDERNODE) *
                            (DEF_NR_POPUPMENUS +    /* for the popup menus */
                            SHAREDRES_NR_GLOBALS +  /* for global window: 0 */
                            nr_topmosts +           /* for the topmost windows */
                            nr_normals)+            /* for the normal windows */
                            SIZE_MASKRECT_USAGE_BMP +
                    sizeof (MASKRECT) * DEF_NR_MASKRECT);

    if (!zi) {
        _WRN_PRINTF ("KERNEL>ZOrder: calloc zorderinfo failure. \n");
        return NULL;
    }

    zi->size_usage_bmp = SIZE_USAGE_BMP;
    zi->size_maskrect_usage_bmp = SIZE_MASKRECT_USAGE_BMP;

    zi->max_nr_popupmenus = DEF_NR_POPUPMENUS;
    zi->max_nr_globals = 0;
    zi->max_nr_topmosts = nr_topmosts;
    zi->max_nr_normals = nr_normals;

    zi->nr_popupmenus = 0;
    zi->nr_globals = 0;
    zi->nr_topmosts = 0;
    zi->nr_normals = 0;

    zi->first_global = 0;
    zi->first_topmost = 0;
    zi->first_normal = 0;

    zi->active_win = 0;

    zi->cli_trackmenu = -1;
    zi->ptmi_in_cli = (HWND)-1;

    /* Set zorder node usage map. */
    memset (zi + 1, 0xFF, SIZE_USAGE_BMP);

    /* Set zorder mask rect usage map. */
    maskrect_usage_bmp = GET_MASKRECT_USAGEBMP(zi);
    memset (maskrect_usage_bmp, 0xFF,
            zi->size_maskrect_usage_bmp);

    /* init z-order node for desktop */
    znodes = GET_ZORDERNODE(zi);

    znodes [0].flags = ZOF_TYPE_DESKTOP | ZOF_VISIBLE;
    znodes [0].rc = g_rcScr;
    znodes [0].age = 0;
    znodes [0].cli = 0;
    znodes [0].hwnd = HWND_DESKTOP;
    znodes [0].next = 0;
    znodes [0].prev = 0;
    znodes [0].idx_mask_rect = 0;

    __mg_slot_set_use ((unsigned char*)(zi + 1), 0);
    __mg_slot_set_use ((unsigned char*)(maskrect_usage_bmp), 0);

#ifdef _MGRM_THREADS
#ifndef __NOUNIX__
    pthread_rwlock_init(&zi->rwlock, NULL);
#else
    pthread_mutex_init(&zi->rwlock, NULL);
#endif
#endif /* _MGRM_THREADS */

    return zi;
}

#endif /* not IS_SHAREDFB_SCHEMA_PROCS */

void __kernel_free_z_order_info (ZORDERINFO* zi)
{
#if IS_SHAREDFB_SCHEMA_PROCS
    if (shmdt (zi) < 0)
        perror ("Detaches shared zorder nodes");
#else /* not IS_SHAREDFB_SCHEMA_PROCS */

#ifdef _MGRM_THREADS
#ifndef __NOUNIX__
    pthread_rwlock_destroy(&zi->rwlock);
#else
    pthread_mutex_destroy(&zi->rwlock);
#endif
#endif /* _MGRM_THREADS */

    free (zi);
#endif /* not IS_SHAREDFB_SCHEMA_PROCS */
}

#endif  /* deprecated code */

