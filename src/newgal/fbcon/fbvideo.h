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

#ifndef _GAL_fbvideo_h
#define _GAL_fbvideo_h

#include <pthread.h>
#include <sys/types.h>
#include <termios.h>
#include <linux/fb.h>

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

/* This is the structure we use to keep track of video memory */
typedef struct vidmem_bucket {
    struct vidmem_bucket *prev;
    int used;
    int dirty;
    char *base;
    unsigned int size;
    struct vidmem_bucket *next;
} vidmem_bucket;

/* Private display data */
struct GAL_PrivateVideoData {
    /* When double buffering supported, the real surface represents the ultimate
     * scan-out frame buffer, and the shadow screen represents the rendering
     * surface. When double buffering disabled, shadow_screen is NULL.
     *
     * For compositing schema, we force to use double buffering.
     * For threads and standalone mode, controlled by `fbcon.double_buffering`
     * run-time option. Not supported for processes mode.
     */
    /* start of header for shadow screen */
    int magic, version;
    GAL_Surface *real_screen, *shadow_screen;
    RECT dirty_rc;

#ifdef _MGSCHEMA_COMPOSITING
    /* Used to simulate the hardware cursor. */
    GAL_Surface *cursor;
    int csr_x, csr_y;
    int hot_x, hot_y;
#endif
    /* end of header for shadow screen */

    int console_fd;
    struct fb_var_screeninfo currt_vinfo;
    struct fb_var_screeninfo saved_vinfo;
    int saved_cmaplen;
    __u16 *saved_cmap;

    /* Since 5.0.13, async updater */
    int             updater_ready;
    int             update_interval;
    RECT            update_rect;
    pthread_t       update_thd;
    pthread_mutex_t update_lock;
    sem_t           sync_sem;

#ifdef _DEBUG
    struct timespec ts_start;
    unsigned        frames;

#endif
#ifdef _MGHAVE_PCIACCESS
    int pci_accel_driver;
#endif

    char *mapped_mem;
    int mapped_memlen;
    int mapped_offset;
    char *mapped_io;
    long mapped_iolen;

    vidmem_bucket surfaces;
    int surfaces_memtotal;
    int surfaces_memleft;

    void (*wait_vbl)(_THIS);
    void (*wait_idle)(_THIS);
};

/* Old variable names */
#define console_fd          (this->hidden->console_fd)
#define currt_vinfo         (this->hidden->currt_vinfo)
#define saved_vinfo         (this->hidden->saved_vinfo)
#define saved_cmaplen       (this->hidden->saved_cmaplen)
#define saved_cmap          (this->hidden->saved_cmap)
#define mapped_mem          (this->hidden->mapped_mem)
#define mapped_memlen       (this->hidden->mapped_memlen)
#define mapped_offset       (this->hidden->mapped_offset)
#define mapped_io           (this->hidden->mapped_io)
#define mapped_iolen        (this->hidden->mapped_iolen)
#define surfaces            (this->hidden->surfaces)
#define surfaces_memtotal   (this->hidden->surfaces_memtotal)
#define surfaces_memleft    (this->hidden->surfaces_memleft)
#define wait_vbl            (this->hidden->wait_vbl)
#define wait_idle           (this->hidden->wait_idle)

#ifdef _MGHAVE_PCIACCESS

#define pci_accel_driver    (this->hidden->pci_accel_driver)
/* Defined in pcivideo.c */
extern int FB_ProbePCIAccelDriver (_THIS, const struct fb_fix_screeninfo* finfo);
extern int FB_InitPCIAccelDriver (_THIS, const GAL_Surface* current);
extern int FB_CleanupPCIAccelDriver (_THIS);
#endif  /* _MGHAVE_PCIACCESS */

/* Accelerator types that are supported by the driver, but are not
   necessarily in the kernel headers on the system we compile on.
*/
#ifndef FB_ACCEL_MATROX_MGAG400
#define FB_ACCEL_MATROX_MGAG400    26    /* Matrox G400            */
#endif
#ifndef FB_ACCEL_3DFX_BANSHEE
#define FB_ACCEL_3DFX_BANSHEE    31    /* 3Dfx Banshee            */
#endif

/* These functions are defined in GAL_fbvideo.c */
extern void FB_SavePaletteTo(_THIS, int palette_len, __u16 *area);
extern void FB_RestorePaletteFrom(_THIS, int palette_len, __u16 *area);

/* These are utility functions for working with video surfaces */

static inline void FB_AddBusySurface(GAL_Surface *surface)
{
    ((vidmem_bucket *)surface->hwdata)->dirty = 1;
}

static inline int FB_IsSurfaceBusy(GAL_Surface *surface)
{
    return ((vidmem_bucket *)surface->hwdata)->dirty;
}

static inline void FB_WaitBusySurfaces(_THIS)
{
    vidmem_bucket *bucket;

    /* Wait for graphic operations to complete */
    wait_idle(this);

    /* Clear all surface dirty bits */
    for ( bucket=&surfaces; bucket; bucket=bucket->next ) {
        bucket->dirty = 0;
    }
}

static inline void FB_dst_to_xy(_THIS, GAL_Surface *dst, int *x, int *y)
{
    *x = (long)((char *)dst->pixels - mapped_mem)%this->screen->pitch;
    *y = (long)((char *)dst->pixels - mapped_mem)/this->screen->pitch;
    if ( dst == this->screen ) {
        *x += this->offset_x;
        *y += this->offset_y;
    }
}

#endif /* _GAL_fbvideo_h */

