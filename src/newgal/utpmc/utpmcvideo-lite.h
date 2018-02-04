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
** $Id: utpmcvideo-lite.h 7357 2007-08-16 05:04:38Z xgwang $
**  
** This is NEWGAL engine for UTStarcom PocketMedia Center based-on 
** ARM7TDMI/uClinux/uClibc for MiniGUI-Lite.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#ifndef _GAL_UTPMVIDEO_H
#define _GAL_UTPMVIDEO_H

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

/* Private display data */

typedef struct __attribute__ ((packed)) _YCrCb
{
    Uint8 yy, cb, cr;
} YCRCB;

/* private data for the client */
typedef struct UTPMC_ShadowInfo {
    char signature [8];

    YCRCB* rgb2ycrcb_map;

    Uint16 w, h;
    Uint16 depth;
    Uint16 pitch;

    BOOL dirty;
    RECT update;

    Uint8 buff [0];
} UTPMC_SHADOWINFO;

#define _MY_STACK_SIZE      2048

/* private data */
struct GAL_PrivateVideoData {
    int fd;
    Uint8 *fb;
    Uint32 fb_size;
    Uint16 fb_pitch;

    Uint8* stack;

    int fd_lock;

    UTPMC_SHADOWINFO* shadow_info;
};

#endif /* _GAL_UTPMVIDEO_H */

