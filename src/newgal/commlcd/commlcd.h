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

#ifndef _GAL_COMMLCD_H
#define _GAL_COMMLCD_H

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

    int dirty;
    RECT rc_dirty;
    pthread_t update_th;
    pthread_mutex_t update_lock;
};

/* The pixel format defined by depth */
#define COMMLCD_PSEUDO_RGB332    1
#define COMMLCD_TRUE_RGB555      2

#define COMMLCD_TRUE_RGB565      3
#define COMMLCD_TRUE_RGB888      4
#define COMMLCD_TRUE_RGB0888     5

#define COMMLCD_TRUE_BGR565      6
#define COMMLCD_TRUE_BGR888      7

struct commlcd_info {
    short height, width;  // Size of the screen
    short bpp;            // Depth (bits-per-pixel)
    short type;           // Pixel type
    short rlen;           // Length of one scan line in bytes
    void  *fb;            // Frame buffer
};

struct commlcd_ops {
    /* return value: zero for OK */
    int (*init) (void);
    /* return value: zero for OK */
    int (*getinfo) (struct commlcd_info *li);
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
