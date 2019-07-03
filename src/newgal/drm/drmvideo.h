/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2019, Beijing FMSoft Technologies Co., Ltd.
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

#ifndef _NEWGAL_DRMVIDEO_H
#define _NEWGAL_DRMVIDEO_H

#include <stdint.h>

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS   GAL_VideoDevice *this

/* Private display data */

#define LEN_DEVICE_NAME     31

typedef struct drm_mode_info DrmModeInfo;

typedef struct GAL_PrivateVideoData {
    char            dev_name[LEN_DEVICE_NAME + 1];
    int             dev_fd;

    DrmDriverOps*   driver_ops;
    DrmDriver*      driver;

    DrmModeInfo*    mode_list;
    GAL_Rect**      modes;

    int             bpp;
    uint32_t        width;
    uint32_t        height;
    uint32_t        pitch;
    uint32_t        size;

    uint32_t        scanout_buff_id;
    uint8_t*        scanout_fb;

    DrmModeInfo*    saved_info;
    drmModeCrtc*    saved_crtc;

    /* only valid when using DUMB frame buffer */
    uint32_t        handle;

    /* only valid when using DRM driver */
    uint32_t        console_buff_id;
} DrmVideoData;

#endif /* _NEWGAL_DRMVIDEO_H */

