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
 *   Copyright (C) 2019 ~ 2020, Beijing FMSoft Technologies Co., Ltd.
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

#ifndef _NEWGAL_DRIVIDEO_H
#define _NEWGAL_DRIVIDEO_H

#include <stdint.h>

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS   GAL_VideoDevice *this

/* Private display data */

typedef struct drm_mode_info DrmModeInfo;

typedef struct GAL_PrivateVideoData {
    /* For compositing schema, we force to use double buffering */
#ifdef _MGSCHEMA_COMPOSITING
    GAL_Surface *real_screen, *shadow_screen;

    DrmSurfaceBuffer *cursor_buff;
    uint32_t cursor_plane_id;

    /* Used to simulate the hardware cursor when hardware cursor not available. */
    GAL_Surface *cursor;
    int csr_x, csr_y;
    int hot_x, hot_y;

#if 0   /* test code */
    /* for asynchronous update */
    pthread_t update_th;
    pthread_mutex_t update_lock;
    sem_t sem_update;
#endif  /* test code */
#else   /* defined _MGSCHEMA_COMPOSITING */
    /* When double buffering supported, the real surface represents the ultimate
     * scan-out frame buffer, and the shadow screen represents the rendering
     * surface. When double buffering disabled, shadow_screen is NULL. */
    GAL_Surface *real_screen, *shadow_screen;

    /* the global names of real screen and shadow screen */
    uint32_t        real_name, shadow_name;
#endif  /* not defined _MGSCHEMA_COMPOSITING */

#if !IS_SHAREDFB_SCHEMA_PROCS
    RECT dirty_rc;
#endif

    char*           dev_name;
    char*           ex_driver;
    int             dev_fd;

    /* capabilities */
    uint32_t        cap_cursor_width;
    uint32_t        cap_cursor_height;
    uint32_t        cap_dumb:1;
    uint32_t        dbl_buff:1;
    uint32_t        scanout_buff_id;

    void*           exdrv_handle;
    DrmDriverOps*   driver_ops;
    DrmDriver*      driver;

    DrmModeInfo*    mode_list;
    GAL_Rect**      modes;

    DrmModeInfo*    saved_info;
    drmModeCrtc*    saved_crtc;

#if 0   /* deprecated code */
    uint32_t        console_buff_id;
    uint8_t*        scanout_fb;
#endif  /* deprecated code */
} DrmVideoData;

#endif /* _NEWGAL_DRIVIDEO_H */

