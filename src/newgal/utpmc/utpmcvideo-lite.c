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
**  $Id: utpmcvideo-lite.c 8944 2007-12-29 08:29:16Z xwyan $
**  
**  This is NEWGAL engine for UTStarcom PocketMedia Center based-on 
**  ARM7TDMI/uClinux/uClibc for MiniGUI-Lite.
**
**  Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#include <stdio.h>

#include "common.h"

#ifndef _MGRM_THREADS

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#ifdef _MGGAL_UTPMC

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#include "utpmcvideo-lite.h"

#define _MY_PRINTF(fmt...) fprintf (stderr, fmt)
// #define _MY_PRINTF(fmt...)

#define DEV_FB                  "/dev/fb0"

#define UTPMCVID_DRIVER_NAME    "utpmc"

#define LOCK_FILE               "/var/tmp/fmmg"

/* Initialization/Query functions */
static int UTPMC_VideoInit (_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **UTPMC_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *UTPMC_SetVideoMode (_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int UTPMC_SetColors (_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void UTPMC_UpdateRects (_THIS, int numrects, GAL_Rect *rects);
static void UTPMC_VideoQuit (_THIS);

/* Hardware surface functions */
static int UTPMC_AllocHWSurface (_THIS, GAL_Surface *surface);
static void UTPMC_FreeHWSurface (_THIS, GAL_Surface *surface);

static void RGB2YCrCb (Uint8 r, Uint8 g, Uint8 b, Uint8* yy, Uint8* cb, Uint8* cr)
{
    *yy = (Uint8)((76 * r + 148 * g + 29 * b) >> 8);
    *cb = (Uint8)(128 + ((-43 * r - 85 * g + 128 * b) >> 8));
    *cr = (Uint8)(128 + ((128 * r - 107 * g - 21 * b) >> 8));
}

static void round_rect_to_even (RECT* rc)
{
    if (rc->left % 2) rc->left --;
    if (rc->right % 2) rc->right ++;
}

static YCRCB* init_rgb2ycrcb_map_16bit (void)
{
    int i;
    YCRCB *map, *tmp;

    map = calloc (65536, sizeof (YCRCB));

    if (map == NULL)
        return NULL;

    tmp = map;
    for (i = 0; i < 65536; i++) {
        RGB2YCrCb (((i & 0xF800) >> 8), ((i & 0x07E0) >> 3), ((i & 0x001F) << 3), 
                &tmp->yy, &tmp->cb, &tmp->cr);
        tmp++;
    }

    return map;
}

static int upd_fd_lock = -1;

static int task_do_update_16bit (void* data)
{
    _THIS;

    upd_fd_lock = open (LOCK_FILE, O_RDWR);

    if (upd_fd_lock < 0) {
        fprintf (stderr, "UTPMC NEWGAL Engine updater: can not open lock file: %s!\n", LOCK_FILE);
        return 1;
    }

    this = data;
    while (this->hidden->shadow_info) {
        Uint32 w, h;
        Uint8 *src_buff;
        Uint8 *dst_buff;
        YCRCB *ycrcb = (YCRCB*) (this->hidden->shadow_info->rgb2ycrcb_map);

        usleep (100000);    // 100 ms

        if (this->hidden->shadow_info->dirty) {
            int i, j;
            RECT bound;
            Uint8 yy, cr, cb;
            Uint16 *src_line;
            Uint8 *dst_line;

            flock (upd_fd_lock, LOCK_EX);

            bound = this->hidden->shadow_info->update;
            round_rect_to_even (&bound);

            w = RECTW (bound);
            h = RECTH (bound);

            src_buff = this->hidden->shadow_info->buff
                    + this->screen->pitch * bound.top
                    + this->screen->format->BytesPerPixel * bound.left;

            dst_buff = this->hidden->fb + this->hidden->fb_pitch * bound.top
                    + (bound.left << 1);

            for (i = 0; i < h; i++) {
                src_line = (Uint16*)src_buff;
                dst_line = dst_buff;

                for (j = 0; j < w; j++) {
                    yy = ycrcb [*src_line].yy;
                    cr = ycrcb [*src_line].cr;
                    cb = ycrcb [*src_line].cb;

                    if (j % 2 == 0) {
                        dst_line [0] = cb;
                        dst_line [1] = yy;
                        dst_line [2] = cr;
                    }
                    else {
                        dst_line [1] = yy;
                    }
                    
                    src_line++;
                    dst_line += 2;
                }

                src_buff += this->hidden->shadow_info->pitch;
                dst_buff += this->hidden->fb_pitch;
            }

            this->hidden->shadow_info->dirty = FALSE;
            SetRect (&this->hidden->shadow_info->update, 0, 0, 0, 0);

            flock (upd_fd_lock, LOCK_UN);
        }
    }

    close (upd_fd_lock);
    upd_fd_lock = -1;

    return 0;
}

static int task_do_update_8bit (void* data)
{
    _THIS;

    upd_fd_lock = open (LOCK_FILE, O_RDWR);

    if (upd_fd_lock < 0) {
        fprintf (stderr, "NEWGAL UTPMC Engine updater: can not open lock file: %s!\n", LOCK_FILE);
        return 1;
    }

    this = data;
    while (this->hidden->shadow_info) {
        Uint32 w, h;
        Uint8 *src_buff, *dst_buff;
        YCRCB *ycrcb = (YCRCB*) (this->hidden->shadow_info->rgb2ycrcb_map);

        usleep (100000);    // 100 ms

        if (this->hidden->shadow_info->dirty) {
            int i, j;
            RECT bound;
            Uint8 yy, cr, cb;
            Uint8 *src_line, *dst_line;

            flock (upd_fd_lock, LOCK_EX);

            bound = this->hidden->shadow_info->update;
            round_rect_to_even (&bound);

            w = RECTW (bound);
            h = RECTH (bound);

            src_buff = this->hidden->shadow_info->buff
                    + this->screen->pitch * bound.top
                    + this->screen->format->BytesPerPixel * bound.left;

            dst_buff = this->hidden->fb + this->hidden->fb_pitch * bound.top
                    + (bound.left << 1);

            for (i = 0; i < h; i++) {
                src_line = src_buff;
                dst_line = dst_buff;

                for (j = 0; j < w; j++) {
                    yy = ycrcb [*src_line].yy;
                    cr = ycrcb [*src_line].cr;
                    cb = ycrcb [*src_line].cb;

                    if (j % 2 == 0) {
                        dst_line [0] = cb;
                        dst_line [1] = yy;
                        dst_line [2] = cr;
                    }
                    else {
                        dst_line [1] = yy;
                    }
                    
                    src_line++;
                    dst_line += 2;
                }

                src_buff += this->hidden->shadow_info->pitch;
                dst_buff += this->hidden->fb_pitch;
            }

            this->hidden->shadow_info->dirty = FALSE;
            SetRect (&this->hidden->shadow_info->update, 0, 0, 0, 0);

            flock (upd_fd_lock, LOCK_UN);
        }
    }

    close (upd_fd_lock);
    upd_fd_lock = -1;
    return 0;
}

/* UTPMC driver bootstrap functions */

static int UTPMC_Available (void)
{
    return (1);
}

static void UTPMC_DeleteDevice (GAL_VideoDevice *device)
{
    if (mgIsServer) {
        free (device->hidden->stack);
        munmap (device->hidden->fb, device->hidden->fb_size);
        close (device->hidden->fd);
    }

    close (device->hidden->fd_lock);
    free (device->hidden);
    free (device);
}

static GAL_VideoDevice *UTPMC_CreateDevice (int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *) calloc (1, sizeof(GAL_VideoDevice));
    if (device) {
        memset (device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *) calloc (1, (sizeof *device->hidden));

        if (device->hidden == NULL)
            goto error;

        if (mgIsServer) {
            /* allocate stack for the updater */
            device->hidden->stack = malloc (_MY_STACK_SIZE);
            if (device->hidden->stack == NULL)
                goto error;
        }
    }
    else
        goto error;

    memset (device->hidden, 0, (sizeof *device->hidden));
    device->hidden->fd = -1;

    /* Set the function pointers */
    device->VideoInit = UTPMC_VideoInit;
    device->ListModes = UTPMC_ListModes;
    device->SetVideoMode = UTPMC_SetVideoMode;
    device->SetColors = UTPMC_SetColors;
    device->VideoQuit = UTPMC_VideoQuit;
#ifndef _MGRM_THREADS
    device->RequestHWSurface = NULL;
#endif
    device->AllocHWSurface = UTPMC_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = UTPMC_FreeHWSurface;
    device->UpdateRects = UTPMC_UpdateRects;

    device->free = UTPMC_DeleteDevice;

    return device;

error:
    GAL_OutOfMemory ();
    if (device) {
        if (device->hidden)
            free (device->hidden);
        free (device);
    }
    return 0;
}

VideoBootStrap UTPMC_bootstrap = {
    UTPMCVID_DRIVER_NAME, "UTPMC video driver for MiniGUI-Lite",
    UTPMC_Available, UTPMC_CreateDevice
};

static int UTPMC_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    struct fb_fix_screeninfo finfo;

    fprintf (stderr, "WARNING: You are using the PMC video driver for UTStarcom!\n");

    if (mgIsServer) {
        this->hidden->fd = open (DEV_FB, O_RDWR);
        if (this->hidden->fd < 0) {
            fprintf (stderr, "UTPMC NEWGAL Engine: can not open FrameBuffer: %s!\n", DEV_FB);
            return -1;
        }

        if (ioctl (this->hidden->fd, FBIOGET_FSCREENINFO, &finfo) < 0) {
            fprintf (stderr, "UTPMC NEWGAL Engine: can not get fixed screen info.\n");
            return -1;
        }

        this->hidden->fb_size = finfo.smem_len;
        this->hidden->fb = mmap (NULL, finfo.smem_len,
                        PROT_READ | PROT_WRITE, 0, this->hidden->fd, 0);
    
        fprintf (stderr, "UTPMC NEWGAL Engine: mapped address: %p, length: %d.\n",
                    this->hidden->fb, this->hidden->fb_size);

        /* Determine the screen depth (use default 16-bit depth) */
        /* we change this during the GAL_SetVideoMode implementation... */
        vformat->BitsPerPixel = 8;
        vformat->BytesPerPixel = 1;

        this->hidden->fd_lock = open (LOCK_FILE, O_CREAT | O_RDWR, 0600);
        if (this->hidden->fd_lock < 0) {
            fprintf(stderr, "UTPMC NEWGAL Engine: can not open lock file: %s!\n", LOCK_FILE);
            return -1;
        }

    }
    else {
        this->hidden->fd_lock = open (LOCK_FILE, O_RDWR, 0600);
        if (this->hidden->fd_lock < 0) {
            fprintf (stderr, "UTPMC NEWGAL Engine: can not open lock file: %s!\n", LOCK_FILE);
            return -1;
        }
            
        read (this->hidden->fd_lock, &this->hidden->shadow_info, sizeof (UTPMC_SHADOWINFO*));

        _MY_PRINTF ("Shadow info read from the lock file: %p\n", this->hidden->shadow_info);
        if (this->hidden->shadow_info) {
            _MY_PRINTF ("Signature in the shadow info: %s!\n", this->hidden->shadow_info->signature);
        }
    }

    /* We're done! */
    return 0;
}

static GAL_Rect mode1 = {0, 0, 320, 240};
static GAL_Rect mode2 = {0, 0, 640, 480};
static GAL_Rect mode3 = {0, 0, 1024, 768};
static GAL_Rect* modes []  = {
    &mode1,
    &mode2,
    &mode3,
    NULL
};

static GAL_Rect **UTPMC_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    if (format->BitsPerPixel == 8 || format->BitsPerPixel == 16) {
        return modes;
    }

    return NULL;
}

static GAL_Surface *UTPMC_SetVideoMode (_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    struct fb_var_screeninfo vinfo;

    if (mgIsServer) {
        size_t size_shadowinfo;

        /* Set underlay FrameBuffer. */
        /* Set the video mode and get the final screen format */
        if (ioctl (this->hidden->fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
            fprintf (stderr, "UTPMC NEWGAL Engine: Couldn't get current FB solution.\n");
            return NULL;
        }

        fprintf (stderr, "UTPMC NEWGAL Engine: Info from FB:\n");
        fprintf (stderr, "\txres: %d, yres: %d.\n", vinfo.xres, vinfo.yres);
        fprintf (stderr, "\t:bits_per_pixel: %d.\n\n", vinfo.bits_per_pixel);

        if (width != vinfo.xres || height != vinfo.yres) {
            vinfo.activate = FB_ACTIVATE_NOW;
            vinfo.accel_flags = 0;
            vinfo.bits_per_pixel = 16;
            vinfo.xres = width;
            vinfo.yres = height;
            vinfo.xres_virtual = width;
            vinfo.yres_virtual = height;
            vinfo.xoffset = 0;
            vinfo.yoffset = 0;
            vinfo.red.length = vinfo.red.offset = 0;
            vinfo.green.length = vinfo.green.offset = 0;
            vinfo.blue.length = vinfo.blue.offset = 0;
            vinfo.transp.length = vinfo.transp.offset = 0;
            if (ioctl (this->hidden->fd, FBIOPUT_VSCREENINFO, &vinfo) < 0) {
                fprintf (stderr, "UTPMC NEWGAL Engine: Couldn't set new FB solution.\n");
                return NULL;
            }
        }

        if (this->hidden->shadow_info) {
            free (this->hidden->shadow_info);
        }

        size_shadowinfo = sizeof (UTPMC_SHADOWINFO) + width * height * (bpp / 8);
        this->hidden->shadow_info = calloc (1, size_shadowinfo);
        if (!this->hidden->shadow_info) {
            fprintf (stderr, "UTPMC NEWGAL Engine: Couldn't allocate shadow buffer for requested mode.\n");
            return NULL;
        }

        /* write the address of shadow_info to the lock file */
        write (this->hidden->fd_lock, &this->hidden->shadow_info, sizeof (UTPMC_SHADOWINFO*));
        strcpy (this->hidden->shadow_info->signature, "UTPMC");
    }
    else {
        if (bpp != this->hidden->shadow_info->depth
                || width != this->hidden->shadow_info->w
                || height != this->hidden->shadow_info->h) {
            fprintf (stderr, "UTPMC NEWGAL Engine: the requested mode is not same as the server.\n");
            return NULL;
        }
    }

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, bpp, 0, 0, 0, 0)) {
        fprintf (stderr, "UTPMC NEWGAL Engine: Couldn't allocate new pixel format for requested mode.\n");
        free (this->hidden->shadow_info);
        return NULL;
    }

    /* Set up the new mode framebuffer */
    current->flags = flags & GAL_FULLSCREEN;
    current->pitch = width * (bpp / 8);
    current->pixels = this->hidden->shadow_info->buff;
    current->w = width;
    current->h = height;

    if (mgIsServer) {
        this->hidden->shadow_info->depth = bpp;
        this->hidden->shadow_info->w = current->w;
        this->hidden->shadow_info->h = current->h;
        this->hidden->fb_pitch = width * 2;

        if (bpp == 8) {
            this->hidden->shadow_info->pitch = width;
            this->hidden->shadow_info->rgb2ycrcb_map = calloc (256, sizeof (YCRCB));
        }
        else {
            this->hidden->shadow_info->pitch = this->hidden->fb_pitch;
            this->hidden->shadow_info->rgb2ycrcb_map = init_rgb2ycrcb_map_16bit ();
        }

        if (this->hidden->shadow_info->rgb2ycrcb_map == NULL) {
            fprintf (stderr, "UTPMC NEWGAL Engine: Couldn't allocate RGB to YCrCb map table.\n");
            goto error;
        }

        /* Start updater */
        {
            int ret;

            ret = clone ((bpp == 8) ? task_do_update_8bit : task_do_update_16bit, 
                        this->hidden->stack + _MY_STACK_SIZE, 
                        CLONE_VM | CLONE_FS | CLONE_FILES, this);

            if (ret < 0) {
                fprintf (stderr, "UTPMC NEWGAL Engine: Couldn't start updater.\n");
                goto error_map;
            }
        }
    }

    /* We're done */
    return current;

error_map:
    free (this->hidden->shadow_info->rgb2ycrcb_map);
error:
    free (this->hidden->shadow_info);
    this->hidden->shadow_info = NULL;
    return NULL;
}

/* We don't actually allow hardware surfaces other than the main one */
static int UTPMC_AllocHWSurface (_THIS, GAL_Surface *surface)
{
    return -1;
}
static void UTPMC_FreeHWSurface (_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int UTPMC_SetColors (_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    int i;
    YCRCB* ycrcb = (YCRCB*) (this->hidden->shadow_info->rgb2ycrcb_map);

    if (firstcolor < 0 || firstcolor + ncolors > 256)
        return 1;

    ycrcb += firstcolor;

    for (i = 0; i < ncolors; i++) {
        RGB2YCrCb (colors->r, colors->g, colors->b, 
                    &ycrcb->yy, &ycrcb->cb, &ycrcb->cr);
        ycrcb++;
        colors++;
    }

    return 0;
}

static void UTPMC_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;

    flock (this->hidden->fd_lock, LOCK_EX);

    bound = this->hidden->shadow_info->update;
    for (i = 0; i < numrects; i++) {
        RECT rc;

        SetRect (&rc, rects[i].x, rects[i].y, 
                        rects[i].x + rects[i].w, rects[i].y + rects[i].h);
        if (IsRectEmpty (&bound))
            bound = rc;
        else
            GetBoundRect (&bound, &bound, &rc);
    }

    if (!IsRectEmpty (&bound)) {
        if (IntersectRect (&bound, &bound, &g_rcScr)) {
            this->hidden->shadow_info->update = bound;
            this->hidden->shadow_info->dirty = TRUE;
        }
    }

    flock (this->hidden->fd_lock, LOCK_UN);
}

static void UTPMC_VideoQuit (_THIS)
{
    if (mgIsServer && this->hidden->shadow_info) {
        void* tmp = this->hidden->shadow_info;

        this->hidden->shadow_info->dirty = FALSE;
        free (this->hidden->shadow_info->rgb2ycrcb_map);

        this->hidden->shadow_info = NULL;
        /* wait updater to quit */
        while (upd_fd_lock >= 0);

        free (tmp);
    }

    if (this->screen && this->screen->pixels) {
        this->screen->pixels = NULL;
    }
}

#endif /* _MGGAL_UTPMC */
#endif 
