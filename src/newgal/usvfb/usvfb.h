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

#ifndef _GAL_USVFB_H
#define _GAL_USVFB_H

#include "sysvideo.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

/* Private display data */
struct GAL_PrivateVideoData {
    int w, h, pitch;
    void *fb;

    int fd_socket;

    int dirty;
    RECT rc_dirty;
    pthread_t update_th;
    pthread_mutex_t update_lock;
};

/* The pixel format defined by depth */
#define USVFB_PSEUDO_RGB332    1
#define USVFB_TRUE_RGB555      2
#define USVFB_TRUE_RGB565      3
#define USVFB_TRUE_RGB888      4
#define USVFB_TRUE_RGB0888     5
#define USVFB_TRUE_ARGB1555    6
#define USVFB_TRUE_ARGB8888    7

struct _vfb_info {
    short height;         // vertical resolution of the screen
    short width;          // horinzontal resolution of the screen
    short bpp;            // Depth (bits-per-pixel)
    short type;           // Pixel type
    short rlen;           // Length of one scan line in bytes
    void  *fb;            // Frame buffer

    short async_update;   // update asynchronously or synchronously. 0 for synchronously; others for asynchronously.
};

#define USS_PATH    "/var/tmp/web-display-server"
#define USC_PATH    "/var/tmp/web-display-client-P%d"

#define USC_PERM    S_IRWXU            /* rwx for user only */

#define FT_VFBINFO      10
#define FT_PING         11
#define FT_PONG         12
#define FT_EVENT        13
#define FT_DIRTYPIXELS  14

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _GAL_USVFB_H */
