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
#ifndef _XXVFBHDR_H
#define _XXVFBHDR_H
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>


#define MOUSE_TYPE   0
#define KB_TYPE      1
#define CAPTION_TYPE 2
#define IME_TYPE     3
#define IME_MESSAGE_TYPE 4
#define SHOW_HIDE_TYPE   5
#define XXVFB_CLOSE_TYPE  0xFFFFFFFF

typedef struct _XXVFbHeader
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
}XXVFBHeader;

typedef struct _XXVFBPalEntry {
    unsigned char r, g, b, a;
} XXVFBPalEntry;

typedef struct _XXVFBInfo
{
    Display *display;
    Window win;
    GC gc;
    Visual *visual;
    Colormap colormap;
    int dev_depth;
    int sockfd;

    XXVFBHeader* xhdr;
}XXVFBInfo;

typedef struct _XXVFBKeyData
{
    unsigned short key_code;
    unsigned short key_state;
} XXVFBKeyData;

typedef struct _XXVFBMouseData
{
    unsigned short x;
    unsigned short y;
    unsigned int   button;
} XXVFBMouseData;

typedef struct _XXVFBEventData
{
    int event_type;
    union 
    {
        XXVFBKeyData key;
        XXVFBMouseData mouse;
    } data;
} XXVFBEventData;

#endif /* _XXVFBHDR_H */
