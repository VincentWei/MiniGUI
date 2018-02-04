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

#ifndef _GAL_pcxvfb_h
#define _GAL_pcxvfb_h

#include <sys/types.h>

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

#define QT_VFB_MOUSE_PIPE	"/tmp/.qtvfb_mouse-%d"
#define QT_VFB_KEYBOARD_PIPE	"/tmp/.qtvfb_keyboard-%d"

int __mg_pcxvfb_server_sockfd;
int __mg_pcxvfb_client_sockfd;

typedef struct _XVFbHeader
{
	unsigned int info_size;

    int width;
    int height;
    int depth;
    int pitch;

    int dirty;
	int dirty_rc_l;
	int dirty_rc_t;
	int dirty_rc_r;
	int dirty_rc_b;

	int palette_changed;
	int palette_offset;

	int fb_offset;

    int MSBLeft;
    
    int Rmask;
    int Gmask;
    int Bmask;
    int Amask;
}XVFBHeader;

typedef struct _XVFBPalEntry {
        unsigned char r, g, b, a;
} XVFBPalEntry;

/* Private display data */
struct GAL_PrivateVideoData {
    unsigned char* shmrgn;
    XVFBHeader* hdr;
};

#endif /* _GAL_pcxvfb_h */

