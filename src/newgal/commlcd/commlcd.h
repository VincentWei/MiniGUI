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

#ifndef _GAL_COMMLCD_H
#define _GAL_COMMLCD_H

#include <semaphore.h>

#include "sysvideo.h"

#include "exstubs.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

/* Private display data */

struct GAL_PrivateVideoData {
    int w, h, pitch;
    void *fb;

    int dirty;
    RECT rc_dirty;
    pthread_t update_th;
    pthread_mutex_t update_lock;
    sem_t sem_update;
};

struct commlcd_ops {
    /* return value: zero for OK */
    int (*init) (void);
    /* return value: zero for OK */
    int (*getinfo) (struct commlcd_info *li, int width, int height, int bpp);
    /* return value: zero for OK */
    int (*release) (void);
    /* return value: number set, zero on error */
    int (*setclut) (int firstcolor, int ncolors, GAL_Color *colors);
    /* return value: number set, zero on error */
    int (*update) (const RECT* rc_dirty);
};

extern struct commlcd_ops __mg_commlcd_ops;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _GAL_COMMLCD_H */
