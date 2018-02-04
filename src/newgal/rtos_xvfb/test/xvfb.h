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

#ifndef XVFB_H
#define XVFB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <minigui/common.h>

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

    /* true for dirty, and should reset to false after refreshing the dirty area */
    int dirty;   
    int dirty_rc_l, dirty_rc_t, dirty_rc_r, dirty_rc_b;

    /* true for changed, and should reset to false after reflecting the change */
    int palette_changed;  
    int palette_offset;

    int fb_offset;
} XVFBHeader;

typedef struct _XVFBKEYDATA
{
    unsigned short key_code;
    unsigned short key_state;
} XVFBKEYDATA;

typedef struct _XVFBMOUSEDATA
{
    unsigned short x;
    unsigned short y;
    unsigned short btn;
} XVFBMOUSEDATA;

typedef struct _XVFBEVENT
{
    int event_type;
    union {
        XVFBKEYDATA kb_data;
        XVFBMOUSEDATA mouse_data;
    };
} XVFBEVENT;

typedef struct _XVFBPalEntry {
    unsigned char r, g, b, a;
} XVFBPalEntry;


extern XVFBHeader* __mg_rtos_xvfb_header;
extern void* __mg_rtos_xvfb_event_buffer;


/***
 * alloc virtual framebuffer
 */
XVFBHeader* xVFBAllocVirtualFrameBuffer (int width, int height, int depth,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask, BOOL MSBLeft);

/***
 * alloc event loop buffer
 */
void* xVFBCreateEventBuffer (int nr_events);

/***
 *notify new key and mouse event
 */
int xVFBNotifyNewEvent (const void* xvfb_event_buffer, XVFBEVENT* event);


#ifdef __cplusplus
}
#endif

#endif
