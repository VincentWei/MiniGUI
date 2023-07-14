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
 *   Copyright (C) 2002~2023, Beijing FMSoft Technologies Co., Ltd.
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

#ifndef _GAL_SHADOW_H
#define _GAL_SHADOW_H

#include <pthread.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

#define MAX_NR_UPDATERS              6
#define MIN_PIXELS_USING_UPDATER     4096

typedef struct _ShadowFBHeader {
    unsigned int info_size;
    int width;
    int height;
    int depth;
    int pitch;

    int dirty:1;
    int palette_changed:1;

    RECT dirty_rect;

    int palette_offset;
    int firstcolor;
    int ncolors;
    int fb_offset;

    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;

} ShadowFBHeader;

#define FLAG_REALFB_PREALLOC  0x01

typedef struct _RealFBInfo {
    DWORD flags;
    int height, width;
    int depth;
    int pitch;
    void *fb;

    GAL_VideoDevice *real_device;

} RealFBInfo;

/* Private display data */

struct GAL_PrivateVideoData {
    RealFBInfo * realfb_info;
#ifdef _MGRM_PROCESSES
    int semid;
#endif

    /* async updater */
    int             async_update;
    int             update_interval;
    RECT            update_rect;
    pthread_t       update_thd;
    pthread_mutex_t update_lock;
    sem_t           sync_sem;
};

typedef struct _ShadowFBOps {
    int (*init) (void);
    int (*get_realfb_info) (RealFBInfo* realfb_info);
    int (*release) (RealFBInfo* realfb_info);
    int (*set_palette) (RealFBInfo* realfb_info, int firstcolor,
            int ncolors, void* colors);
    void (*refresh) (ShadowFBHeader* shadowfb_header,
            RealFBInfo* realfb_info, void* update);
} ShadowFBOps;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _GAL_SHADOW_H */
