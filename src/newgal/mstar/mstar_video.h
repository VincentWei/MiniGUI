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
#ifndef _GAL_MStar_video_h
#define _GAL_MStar_video_h

#define MSOS_TYPE_LINUX 1
#define MSTAR_T3_PLATFORM     1

#include <sys/types.h>
#include <termios.h> 
#include <MsCommon.h>
#include <drvSEM.h>
#include <apiGFX.h>
#include <apiGOP.h>
#include <drvMMIO.h>
#include <drvSYS.h>
//#include <SDL/SDL_mutex.h>

#ifndef MSTAR_T3_PLATFORM
#include <drvPQ.h>
#else
#include <drvSYS.h>
#endif

#include <madp.h>
#include <MMAP.h>


#define MAX_MSTAR_LAYER 5 
#define _THIS   GAL_VideoDevice * this 

/* This is the structure we use to keep track of video memory */
typedef struct vidmem_bucket {
    struct vidmem_bucket *prev;
    short used;
    char bscreen;
    char dirty;
    unsigned int clientpid;
    unsigned int remotebucket;
    unsigned int fbid;
    unsigned int fbaway;
    char *base;
    unsigned int size;
    struct vidmem_bucket *next;
} vidmem_bucket;


typedef struct { 
    Uint32                      ge_src_colorkey_enabled  : 1;
    Uint32                      ge_dst_colorkey_enabled  : 1;
    Uint32                      ge_alpha_blend_enabled   : 1;
    Uint32                      gop_stretch_enabled      : 1;
    Uint32                      reserved_mask            : 28; 

    int                      lcd_width;
    int                      lcd_height;
    int                      lcd_offset;
    int                      lcd_pitch;
    int                      lcd_size;

    /* state validation */
    int                      v_flags;

    /* cached values */
    unsigned long            dst_phys;
    int                      dst_pitch;
    int                      dst_bpp;
    int                      dst_index; 
    Uint32                   dst_ge_format;
    unsigned int             dst_w;
    unsigned int             dst_h;
    Uint32                   dst_ge_clr_key;

    unsigned long            src_phys;
    int                      src_pitch;
    int                      src_bpp;
    int                      src_index;
    Uint32                   src_ge_format;
    unsigned int             src_w;
    unsigned int             src_h;
    Uint32                   src_ge_clr_key;

    unsigned long            mask_phys;
    int                      mask_pitch; 
    int                      mask_index;

    Uint32                   ge_render_coef;
    Uint32                   ge_render_alpha_from;

    int                      ckey_b_enabled;
    int                      color_change_enabled;
    int                      mask_enabled; 
    unsigned int             input_mask;

    int                      matrix[6];
    Uint32                   rclr;
    Uint32                   color16;

    int                     gfx_gop_index;
    int                     gfx_h_offset;
    int                     gfx_v_offset; 
    int                     gfx_max_width;
    int                     gfx_max_height;
    unsigned long           mst_miu1_cpu_offset;
} MSTARDeviceData;

struct GAL_PrivateVideoData
{
    short	    initialized;  
    short       bdoublebuf;
    unsigned short u16FBId[2];
    int         flip;

    unsigned long dfb_base; 
    unsigned char *video_phys;                    /* Physical base address of video memory */
    unsigned int  video_length;                  /* Size of video memory */
    unsigned char* mmio_phys;                     /* Physical base address of MMIO area */
    unsigned int  mmio_length;                   /* Size of MMIO area */

    unsigned int video_miu;

    int nummodes;
    GAL_Rect *modelist[5];

    unsigned long switched_away;

    MSTARDeviceData sdev;

    int current_vt;
    int console_fd;
    int saved_vt;
    int keyboard_fd;
    int saved_kbd_mode;
    struct termios saved_kbd_termios;

    int mouse_fd;
    int ir_fd;
    int gwinid;

    int layerid[MAX_MSTAR_LAYER];
    unsigned int layeraddr[MAX_MSTAR_LAYER];
    unsigned int layerfbid[MAX_MSTAR_LAYER];

    Uint32 screen_arealen; 

    vidmem_bucket surfs;
    int memtotal;
    int memleft; 
};

#define HIDDEN              (this->hidden)
#define surfaces            (this->hidden->surfs)
#define surfaces_memtotal	(this->hidden->memtotal)
#define surfaces_memleft	(this->hidden->memleft) 
#define console_fd          (this->hidden->console_fd)
#define current_vt          (this->hidden->current_vt)
#define saved_vt            (this->hidden->saved_vt)
#define ir_fd				(this->hidden->ir_fd)
#define keyboard_fd		    (this->hidden->keyboard_fd)
#define saved_kbd_mode		(this->hidden->saved_kbd_mode)
#define saved_kbd_termios	(this->hidden->saved_kbd_termios)
#define mouse_fd		    (this->hidden->mouse_fd)
#define screen_arealen		(this->hidden->screen_arealen)
//#define hw_lock			    (this->hidden->hw_lock)
#define switched_away		(this->hidden->switched_away)

#endif /* _GAL_MStar_video_h */
