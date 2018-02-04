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

#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE) 
#include "ourhdr.h"
#include "sockio.h"
#include "client.h"
#include "server.h"
#include "sharedres.h"
#include "drawsemop.h"
#endif
#include "misc.h"

#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE) 

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
#endif

int kernel_alloc_z_order_info (int nr_topmosts, int nr_normals)
{
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE) 
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
#else
    ZORDERNODE* znodes;
    void* maskrect_usage_bmp;

    __mg_zorder_info = (PZORDERINFO) calloc (1, 
                sizeof (ZORDERINFO) + SIZE_USAGE_BMP + 
                    sizeof (ZORDERNODE) * 
                            (DEF_NR_POPUPMENUS +    /* for the popup menus */
                            SHAREDRES_NR_GLOBALS +  /* for global window: 0*/
                            nr_topmosts +           /* for the topmost windows */
                            nr_normals)+            /* for the normal windows */
                            SIZE_MASKRECT_USAGE_BMP + 
                    sizeof (MASKRECT) * DEF_NR_MASKRECT);

    if (!__mg_zorder_info) {
        _MG_PRINTF ("KERNEL>ZOrder: calloc zorderinfo failure. \n");
        return -1;
    }

    __mg_zorder_info->size_usage_bmp = SIZE_USAGE_BMP;
    __mg_zorder_info->size_maskrect_usage_bmp = SIZE_MASKRECT_USAGE_BMP;

    __mg_zorder_info->max_nr_popupmenus = DEF_NR_POPUPMENUS;
    __mg_zorder_info->max_nr_globals = 0;
    __mg_zorder_info->max_nr_topmosts = nr_topmosts;
    __mg_zorder_info->max_nr_normals = nr_normals;

    __mg_zorder_info->nr_popupmenus = 0;
    __mg_zorder_info->nr_globals = 0;
    __mg_zorder_info->nr_topmosts = 0;
    __mg_zorder_info->nr_normals = 0;

    __mg_zorder_info->first_global = 0;
    __mg_zorder_info->first_topmost = 0;
    __mg_zorder_info->first_normal = 0;

    __mg_zorder_info->active_win = 0;

    __mg_zorder_info->cli_trackmenu = -1;
    __mg_zorder_info->ptmi_in_cli = (HWND)-1;

    /* Set zorder node usage map. */
    memset (__mg_zorder_info + 1, 0xFF, SIZE_USAGE_BMP);

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
    znodes [0].fortestinghwnd = HWND_DESKTOP;
    znodes [0].next = 0;
    znodes [0].prev = 0;
    znodes [0].idx_mask_rect = 0;

    __mg_slot_set_use ((unsigned char*)(__mg_zorder_info + 1), 0);
    __mg_slot_set_use ((unsigned char*)(maskrect_usage_bmp), 0);

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

void kernel_free_z_order_info (ZORDERINFO* zi)
{
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE) 
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
