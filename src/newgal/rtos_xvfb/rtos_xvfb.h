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
** rtos_xvfb.h: RTOS X virtual FrameBuffer head file.
*/

#ifndef _GAL_rtos_xvfb_h
#define _GAL_rtos_xvfb_h

#include <sys/types.h>

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

typedef struct _XVFBHeader {
    unsigned int info_size;
    int width;
    int height;
    int depth;

    /* The flag indicating the Most Significant Bits (MSB) 
     * is left when depth is less than 8. */
    Uint8  MSBLeft;

    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;

    int pitch;
    int dirty;            
    int dirty_rc_l, dirty_rc_t, dirty_rc_r, dirty_rc_b;
    int palette_changed;  
    int palette_offset;
    int fb_offset;
} XVFBHeader;

typedef struct _XVFBPalEntry {
    unsigned char r, g, b, a;
} XVFBPalEntry;

extern XVFBHeader* __mg_rtos_xvfb_header;

/* Private display data */
struct GAL_PrivateVideoData {
    unsigned char* shmrgn;
    XVFBHeader* hdr;
};

#endif /* _GAL_qvfb_h */

