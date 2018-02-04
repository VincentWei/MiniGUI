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
** $Id: bf533video.h 7345 2007-08-16 03:57:17Z xgwang $
**  
** Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#ifndef _GAL_BF533VIDEO_H
#define _GAL_BF533VIDEO_H

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

#define OSD_AGAIN           0
#define OSD_AGAIN1          0xFFFF
#define OSD_OK              1
#define OSD_ERROR           2

#define OSD_WIDTH           640
#define OSD_HEIGHT          480

#define OSD_OP_OPEN         1
#define OSD_OP_CLOSE        2
#define OSD_OP_SET_PALETTE  3
#define OSD_OP_UPDATE_RECT  4

typedef struct _osd_op_header {
    Uint8 app_id [4];
    Uint32 op_id;
    Uint32 data_len;
    Uint16 x, y;
    Uint16 w, h;
    Uint16 pitch, pad;
} OSD_OP_HEADER;

/* Private display data */

struct GAL_PrivateVideoData {
    int fd_spi;
    int w, h;
    Uint8 *buffer;

    int  fd_lock;

    BOOL dirty;
    RECT update;
};

#if 0
    pthread_t th;
    pthread_mutex_t lock;
#endif

#endif /* _GAL_BF533VIDEO_H */
