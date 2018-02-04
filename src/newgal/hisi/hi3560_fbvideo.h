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
** $Id: hi3560_fbvideo.h 12360 2009-12-16 09:56:03Z dongkai $
**
** hi3560_fbvideo.h: header file for hi3560 framebuffer video
**
** Copyright (C) 2007 Feynman Software.
**
** All rights reserved by Feynman Software.
*/
#ifndef _GAL_fbvideo_h
#define _GAL_fbvideo_h

#include <sys/types.h>
#include <linux/fb.h>
#include "sysvideo.h"
#include "hi_api.h"
#include "api_display.h"
#include "hi_type.h"


#define GAL_mutexP(lock)    lock++;
#define GAL_mutexV(lock)    lock--;
#define GAL_DestroyMutex(lock)  lock=0;
#define _TAP_ALL 1
#if (defined(_TAP_ALL) || defined(_TAP3_VERT_HORI) || defined(_TAP2_VERT)  \
     || defined(_TAP2_VERT_HORI)|| defined(_TAP3_VERT))  \
     && defined(_USE_2D_ACCEL) && defined(_USE_DBUF) 
#define TAP_MIN_VALUE   1
#define TAP_2_VERT      1    /*2-tap vertical antiflicker*/
#define TAP_2_VERT_HORI 2    /*2-tap vertical and horizontal antiflicker*/
#define TAP_3_VERT      3    /*3-tap vertical antiflicker*/
#define TAP_3_VERT_HORI 4    /*3-tap vertical and horizontal antiflicker*/
#define TAP_MAX_VALUE   4   
#endif

#define _THIS GAL_VideoDevice *this

/* This is the structure we use to keep track of video memory */
typedef struct vidmem_bucket {
    struct vidmem_bucket *prev;
    int used;
    Uint8 *base;
    unsigned int size;
    struct vidmem_bucket *next;
} vidmem_bucket;

/* Private display data */
struct GAL_PrivateVideoData {
    int console_fd1;
    struct fb_fix_screeninfo finfo;
    unsigned char *mapped_mem;
    int mapped_memlen;
    int mapped_offset;
    unsigned char *mapped_io;
    long mapped_iolen;
  
    vidmem_bucket surfaces;
    int surfaces_memtotal;
    int surfaces_memleft;

    GAL_mutex *hw_lock;

    void (*wait_vbl)(_THIS);
    void (*wait_idle)(_THIS);
#if (defined(_TAP_ALL) || defined(_TAP3_VERT_HORI) || defined(_TAP2_VERT) \
     || defined(_TAP2_VERT_HORI)|| defined(_TAP3_VERT)) \
     && defined(_USE_2D_ACCEL) && defined(_USE_DBUF) 
    /*these item record physics addr, 
      for antiflicker.*/
    Uint32 uiMiddleBuf1;
    Uint32 uiMiddleBuf2;
    Uint32 uiMiddleBuf3;
    Uint32 uiCurrentBuf;
    Uint32 uiShadowBuf;

    /*antiflicker level
      TAP_2_VERT       2 tap vertical antiflicker
      TAP_2_VERT_HORI  2 tap average antiflicker
      TAP_3_VERT       3 tap vertical antiflicker
      TAP_4_VERT_HORI  3 tap average antiflicker
      other denote don't antiflicker
    */
    Uint32 uiTapLevel;
#endif
};

#ifndef _MGRM_THREADS
#define console_fd         (((struct GAL_PrivateVideoData*)(this->gamma))->console_fd1)
#define saved_finfo        (((struct GAL_PrivateVideoData*)(this->gamma))->finfo)
#define saved_cmaplen      (((struct GAL_PrivateVideoData*)(this->gamma))->saved_cmaplen)
#define saved_cmap         (((struct GAL_PrivateVideoData*)(this->gamma))->saved_cmap)
#define mapped_mem         (((struct GAL_PrivateVideoData*)(this->gamma))->mapped_mem)
#define mapped_memlen      (((struct GAL_PrivateVideoData*)(this->gamma))->mapped_memlen)
#define mapped_offset      (((struct GAL_PrivateVideoData*)(this->gamma))->mapped_offset)
#define mapped_io          (((struct GAL_PrivateVideoData*)(this->gamma))->mapped_io)
#define mapped_iolen       (((struct GAL_PrivateVideoData*)(this->gamma))->mapped_iolen)
#define surfaces           (((struct GAL_PrivateVideoData*)(this->gamma))->surfaces)
#define surfaces_memtotal  (((struct GAL_PrivateVideoData*)(this->gamma))->surfaces_memtotal)
#define surfaces_memleft   (((struct GAL_PrivateVideoData*)(this->gamma))->surfaces_memleft)
#define hw_lock            (((struct GAL_PrivateVideoData*)(this->gamma))->hw_lock)
#define wait_vbl           (((struct GAL_PrivateVideoData*)(this->gamma))->wait_vbl)
#define wait_idle          (((struct GAL_PrivateVideoData*)(this->gamma))->wait_idle)
#define middle_buf1        (((struct GAL_PrivateVideoData*)(this->gamma))->uiMiddleBuf1)
#define middle_buf2        (((struct GAL_PrivateVideoData*)(this->gamma))->uiMiddleBuf2)
#define middle_buf3        (((struct GAL_PrivateVideoData*)(this->gamma))->uiMiddleBuf3)
#define current_buf        (((struct GAL_PrivateVideoData*)(this->gamma))->uiCurrentBuf)
#define shadow_buf         (((struct GAL_PrivateVideoData*)(this->gamma))->uiShadowBuf)
#define tap_level          (((struct GAL_PrivateVideoData*)(this->gamma))->uiTapLevel)
BOOL FB_CreateDevice(GAL_VideoDevice *device, const Sint8* pszLayerName);
#else
#define console_fd         (g_stHidden_3560.console_fd1)
#define saved_finfo        (g_stHidden_3560.finfo)
#define saved_cmaplen      (g_stHidden_3560.saved_cmaplen)
#define saved_cmap         (g_stHidden_3560.saved_cmap)
#define mapped_mem         (g_stHidden_3560.mapped_mem)
#define mapped_memlen      (g_stHidden_3560.mapped_memlen)
#define mapped_offset      (g_stHidden_3560.mapped_offset)
#define mapped_io          (g_stHidden_3560.mapped_io)
#define mapped_iolen       (g_stHidden_3560.mapped_iolen)
#define surfaces           (g_stHidden_3560.surfaces)
#define surfaces_memtotal  (g_stHidden_3560.surfaces_memtotal)
#define surfaces_memleft   (g_stHidden_3560.surfaces_memleft)
#define hw_lock            (g_stHidden_3560.hw_lock)
#define wait_vbl           (g_stHidden_3560.wait_vbl)
#define wait_idle          (g_stHidden_3560.wait_idle)
#define middle_buf1        (g_stHidden_3560.uiMiddleBuf1)
#define middle_buf2        (g_stHidden_3560.uiMiddleBuf2)
#define middle_buf3        (g_stHidden_3560.uiMiddleBuf3)
#define current_buf        (g_stHidden_3560.uiCurrentBuf)
#define shadow_buf         (g_stHidden_3560.uiShadowBuf)
#define tap_level          (g_stHidden_3560.uiTapLevel)


BOOL FB_CreateDevice(GAL_VideoDevice *device);
#endif

//////////////////////////////////////////////////////////////
//Added by l56244 on 27,July 2006
typedef char                DEV_NAME;
typedef struct HW_HIDISPLAY_ATTR_S
{
     HI_BOOL             enable;
     DISPLAY_MODE_E      display_mode;
     HI_BOOL             gpc_layer_enable[VOU_GPC_LAYER_HC+1];
     DISPLAY_LCD_CFG_S   lcd_config;
     DEV_NAME            dev_name[15];
}HWDISPLAY_ATTR_S;



void hwDisplaySetting(HWDISPLAY_ATTR_S *);
int HI3560_GetVideoFD(void);
void* HI3560_GetMM_0(void);
#define HI3560_AF_NONE             0
#define HI3560_AF_VOU_VDF          1
#define HI3560_AF_VOU_HVDF         2
#define HI3560_AF_TDE              3
#define HI3560_AF_TDE_VDF          4
#define HI3560_AF_TDE_HVDF         5

//Added Ends


#endif /* _GAL_fbvideo_h */

