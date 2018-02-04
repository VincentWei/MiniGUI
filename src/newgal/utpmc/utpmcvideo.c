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
**  $Id: utpmcvideo.c 8944 2007-12-29 08:29:16Z xwyan $
**  
**  This is NEWGAL engine for UTStarcom PocketMedia 
**  based-on ARM7TDMI + uClinux + uClibc.
**
**  Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#include <stdio.h>
#include "common.h"

#ifdef _MGRM_THREADS

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
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

#include "utpmcvideo.h"

#undef TEST
#undef USE_CACHE

#define DEV_FB       "/dev/fb0"

#define UTPMCVID_DRIVER_NAME    "utpmc"

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

#ifdef USE_CACHE
static void get_ycrcb_16bit (RGB_YCRCB_MAP* map, Uint16 pixel, Uint8* yy, Uint8* cr, Uint8* cb)
{
    int i;
    Uint8 key;
    RGB_YCRCB_PAIR *tmp, *tail, *new_pair;

    /* find the map pair in the last_used array. */
    for (i = 0; i < map->nr_last_used; i++) {
        if (map->last_used [i]->rgb_pixel == pixel) {
            *yy = map->last_used [i]->yy;
            *cr = map->last_used [i]->cr;
            *cb = map->last_used [i]->cb;
            map->last_used [i]->used_count ++;
            return;
        }
    }

    /* find the map pair in the cache line of the pixel. */
    key = ((pixel & 0xE000) >> 8) | ((pixel & 0x0700) >> 6) | ((pixel & 0x0018) >> 3);
    tail = NULL;
    tmp = map->cached [key];
    while (tmp) {
        if (tmp->rgb_pixel == pixel) {
            *yy = tmp->yy;
            *cr = tmp->cr;
            *cb = tmp->cb;
            tmp->used_count ++;
            new_pair = tmp;
            goto last_used;
        }

        tail = tmp;
        tmp = tmp->next;
    }

    /* not found, calculate YCrCb. */
    RGB2YCrCb (((pixel & 0xF800) >> 8), ((pixel & 0x07E0) >> 3), ((pixel & 0x001F) << 3), 
                yy, cb, cr);

    /* try to find a free pair and insert it to the cache line. */
    if (map->free_pairs) {
        new_pair = map->free_pairs;
        map->free_pairs = new_pair->next;

        if (tail)
            tail->next = new_pair;
        else
            map->cached [key] = new_pair;
        new_pair->next = NULL;
    }
    else {
        /* no free pair, find the least-used pair. */
        Uint32 min = UINT_MAX;
        RGB_YCRCB_PAIR *lu = NULL;

        tmp = map->cached [key];
        while (tmp) {
            if (tmp->used_count <= min) {
                min = tmp->used_count;
                lu = tmp;
            }
            tmp = tmp->next;
        }

        new_pair = lu;
    }

    /* thing to be messy ...*/
    if (new_pair == NULL) {
        fprintf (stderr, "WARNING: thing to be messy...\n");
        return;
    }

    new_pair->rgb_pixel = pixel;
    new_pair->used_count = 1;

    new_pair->yy = *yy;
    new_pair->cr = *cr;
    new_pair->cb = *cb;

last_used:

    /* update the last used array */
    if (map->nr_last_used < LAST_USED_PAIRS) {
        map->last_used [map->nr_last_used] = new_pair;
        map->nr_last_used ++;
    }
    else {
        /* find the least used pair in the last used array and replace it. */
        Uint32 min = new_pair->used_count;
        int lu = -1;

        for (i = 0; i < LAST_USED_PAIRS; i++) {
            if (map->last_used [i]->used_count <= min) {
                min = map->last_used [i]->used_count;
                lu = i;
            }
        }

        if (lu >= 0)
            map->last_used [lu] = new_pair;
    }
}

#ifdef TEST
static void dump_map_table (RGB_YCRCB_MAP* map)
{
    int i;
    RGB_YCRCB_PAIR *tmp;

    printf ("The map table: nr_last_used: %d\n", map->nr_last_used);
    for (i = 0; i < map->nr_last_used; i++) {
        printf ("The last used NO %d, %p\n", i, map->last_used [i]);
    }

    for (i = 0; i < 256; i++) {
        int no;
        printf ("Cache line for the key %d:\n", i);
        tmp = map->cached [i];

        no = 0;
        while (tmp) {
            printf ("   pair %d (%p): pixel: %d -> (%d, %d, %d), used_count: %d\n", no, tmp,
                    tmp->rgb_pixel, tmp->yy, tmp->cb, tmp->cr, tmp->used_count);
            tmp = tmp->next;
            no ++;
        }
        printf ("End of cache line for the key %d:\n", i);
    }
}
#endif /* TEST */

static RGB_YCRCB_MAP* init_rgb2ycrcb_map_16bit (void)
{
    int i;
    RGB_YCRCB_MAP* map;
    RGB_YCRCB_PAIR* free_pairs;

    map = calloc (1, sizeof (RGB_YCRCB_MAP));

    if (map == NULL)
        return NULL;

    map->free_pairs = calloc (FREE_PAIRS, sizeof (RGB_YCRCB_PAIR));
    if (map->free_pairs == NULL) {
        free (map);
        return NULL;
    }

    /* make list for free pairs */
    free_pairs = map->free_pairs;
    for (i = 0; i < FREE_PAIRS - 1; i++) {
        free_pairs->next = free_pairs + 1;
        free_pairs ++;
    }
    free_pairs->next = NULL;

    {
        Uint16 r, g, b;
        Uint8 yy, cr, cb;
        for (r = 0; r <= 7; r++) {
            for (g = 0; g <= 7; g++) {
                for (b = 0; b <= 3; b++) {
                    get_ycrcb_16bit (map, (r << 13) | (g << 8) | (b << 3), &yy, &cr, &cb);
                }
            }
        }
    }

#ifdef TEST
    /* dump the initial map table */
    dump_map_table (map);
#endif

    return map;
}

#else /* USE_CACHE */

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

#endif /* !USE_CACHE */

static void* task_do_update_16bit (void* data)
{
    _THIS;

    this = data;

    while (this->hidden->shadow) {
        Uint32 w, h;
        Uint8 *src_buff;
        Uint8 *dst_buff;
#ifdef USE_CACHE
        RGB_YCRCB_MAP* map = (RGB_YCRCB_MAP*) (this->hidden->rgb2ycrcb_map);
#else
        YCRCB *ycrcb = (YCRCB*) (this->hidden->rgb2ycrcb_map);
#endif

        usleep (100000);    // 100 ms

        if (this->hidden->dirty) {
            int i, j;
            RECT bound;
            Uint8 yy, cr, cb;
            Uint16 *src_line;
            Uint8 *dst_line;

            pthread_mutex_lock (&this->hidden->lock);

            bound = this->hidden->update;
            round_rect_to_even (&bound);

            w = RECTW (bound);
            h = RECTH (bound);

            src_buff = this->hidden->shadow 
                    + this->screen->pitch * bound.top
                    + this->screen->format->BytesPerPixel * bound.left;

            dst_buff = this->hidden->fb + this->hidden->fb_pitch * bound.top
                    + (bound.left << 1);

            for (i = 0; i < h; i++) {
                src_line = (Uint16*)src_buff;
                dst_line = dst_buff;

                for (j = 0; j < w; j++) {
#ifdef USE_CACHE
                    get_ycrcb_16bit (map, *src_line, &yy, &cr, &cb);
#else
                    yy = ycrcb [*src_line].yy;
                    cr = ycrcb [*src_line].cr;
                    cb = ycrcb [*src_line].cb;
#endif

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

                src_buff += this->hidden->sh_pitch;
                dst_buff += this->hidden->fb_pitch;
            }

            this->hidden->dirty = FALSE;
            SetRect (&this->hidden->update, 0, 0, 0, 0);

            pthread_mutex_unlock (&this->hidden->lock);
        }
    }

    return NULL;
}

static void* task_do_update_8bit (void* data)
{
    _THIS;

    this = data;

    while (this->hidden->shadow) {
        Uint32 w, h;
        Uint8 *src_buff, *dst_buff;
        YCRCB *ycrcb = (YCRCB*) (this->hidden->rgb2ycrcb_map);

        usleep (100000);    // 100 ms

        if (this->hidden->dirty) {
            int i, j;
            RECT bound;
            Uint8 yy, cr, cb;
            Uint8 *src_line, *dst_line;

            pthread_mutex_lock (&this->hidden->lock);

            bound = this->hidden->update;
            round_rect_to_even (&bound);

            w = RECTW (bound);
            h = RECTH (bound);

            src_buff = this->hidden->shadow 
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

                src_buff += this->hidden->sh_pitch;
                dst_buff += this->hidden->fb_pitch;
            }

            this->hidden->dirty = FALSE;
            SetRect (&this->hidden->update, 0, 0, 0, 0);

            pthread_mutex_unlock (&this->hidden->lock);
        }
    }

    return NULL;
}

/* UTPMC driver bootstrap functions */

static int UTPMC_Available (void)
{
    return (1);
}

static void UTPMC_DeleteDevice (GAL_VideoDevice *device)
{
    pthread_mutex_destroy (&device->hidden->lock);

    munmap (device->hidden->fb, device->hidden->fb_size);
    close (device->hidden->fd);
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
    }

    if ((device == NULL) || (device->hidden == NULL)) {
        GAL_OutOfMemory();
        if (device)
            free (device);
        return 0;
    }

    memset (device->hidden, 0, (sizeof *device->hidden));
    device->hidden->fd = -1;

    /* Set the function pointers */
    device->VideoInit = UTPMC_VideoInit;
    device->ListModes = UTPMC_ListModes;
    device->SetVideoMode = UTPMC_SetVideoMode;
    device->SetColors = UTPMC_SetColors;
    device->VideoQuit = UTPMC_VideoQuit;
    device->AllocHWSurface = UTPMC_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = UTPMC_FreeHWSurface;
    device->UpdateRects = UTPMC_UpdateRects;

    device->free = UTPMC_DeleteDevice;

    return device;
}

VideoBootStrap UTPMC_bootstrap = {
    UTPMCVID_DRIVER_NAME, "UTPMC video driver",
    UTPMC_Available, UTPMC_CreateDevice
};

static int UTPMC_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    struct fb_fix_screeninfo finfo;

    fprintf (stderr, "WARNING: You are using the PMC video driver for UTStarcom!\n");

    this->hidden->fd = open (DEV_FB, O_RDWR);
    if (this->hidden->fd < 0) {
        fprintf (stderr, "UTPMC NEWGAL Engine: can not open FrameBuffer: %s!\n", DEV_FB);
        return -1;
    }

#ifndef TEST
    if (ioctl (this->hidden->fd, FBIOGET_FSCREENINFO, &finfo) < 0) {
        fprintf (stderr, "UTPMC NEWGAL Engine: can not get fixed screen info.\n");
        return -1;
    }

    this->hidden->fb_size = finfo.smem_len;
    this->hidden->fb = mmap (NULL, finfo.smem_len,
                      PROT_READ | PROT_WRITE, 0, this->hidden->fd, 0);
    
    fprintf (stderr, "UTPMC NEWGAL Engine: mapped address: %p, length: %d.\n",
                this->hidden->fb, this->hidden->fb_size);
#else
    this->hidden->fb_size = 1024*768*2;
    this->hidden->fb = mmap (NULL, this->hidden->fb_size,
                      PROT_READ | PROT_WRITE, MAP_SHARED, this->hidden->fd, 0);
#endif

    /* Determine the screen depth (use default 16-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;

    this->hidden->dirty = FALSE;
    SetRect (&this->hidden->update, 0, 0, 0, 0);

    pthread_mutex_init (&this->hidden->lock, NULL);

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
#ifndef TEST
    struct fb_var_screeninfo vinfo;

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
#endif /* !TEST */

    if (this->hidden->shadow) {
        free (this->hidden->shadow);
    }

    this->hidden->shadow = malloc (width * height * (bpp / 8));
    if (!this->hidden->shadow) {
        fprintf (stderr, "UTPMC NEWGAL Engine: Couldn't allocate shadow buffer for requested mode\n");
        return NULL;
    }
    memset (this->hidden->shadow, 0, width * height * (bpp / 8));

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, bpp, 0, 0, 0, 0)) {
        fprintf (stderr, "UTPMC NEWGAL Engine: Couldn't allocate new pixel format for requested mode\n");
        goto error;
    }

    /* Set up the new mode framebuffer */
    current->flags = flags & GAL_FULLSCREEN;
    this->hidden->w = current->w = width;
    this->hidden->h = current->h = height;
    current->pitch = current->w * (bpp / 8);
    current->pixels = this->hidden->shadow;

    this->hidden->fb_pitch = width * 2;
    if (bpp == 8) {
        this->hidden->sh_pitch = width;
        this->hidden->rgb2ycrcb_map = calloc (256, sizeof (YCRCB));
    }
    else {
        this->hidden->sh_pitch = this->hidden->fb_pitch;
        this->hidden->rgb2ycrcb_map = init_rgb2ycrcb_map_16bit ();
    }

    if (this->hidden->rgb2ycrcb_map == NULL) {
        fprintf (stderr, "UTPMC NEWGAL Engine: Couldn't allocate RGB to YCrCb map table.\n");
        goto error;
    }

    /* Start updater */
    {
        int ret;

#if 0
        pthread_attr_t new_attr;
        pthread_attr_init (&new_attr);
        pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
        /* Stack with 1024 bytes long is enough for updater */
        pthread_attr_setstacksize (&new_attr, 1024);
        pthread_attr_destroy (&new_attr);
#endif

        ret = pthread_create (&this->hidden->th, NULL, 
            (bpp == 8) ? task_do_update_8bit : task_do_update_16bit, this);

        if (ret != 0) {
            fprintf (stderr, "UTPMC NEWGAL Engine: Couldn't start updater\n");
            goto error_map;
        }
    }

    /* We're done */
    return current;

error_map:
    free (this->hidden->rgb2ycrcb_map);
error:
    free (this->hidden->shadow);
    this->hidden->shadow = NULL;
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
    YCRCB* ycrcb = (YCRCB*) (this->hidden->rgb2ycrcb_map);

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

    pthread_mutex_lock (&this->hidden->lock);

    bound = this->hidden->update;
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
            this->hidden->update = bound;
            this->hidden->dirty = TRUE;
        }
    }

    pthread_mutex_unlock (&this->hidden->lock);
}

static void UTPMC_VideoQuit (_THIS)
{
    if (this->hidden->shadow) {
        void* tmp = this->hidden->shadow;
        void* ret_value;

        this->hidden->dirty = FALSE;
        this->hidden->shadow = NULL;
        pthread_join (this->hidden->th, &ret_value);

        free (this->hidden->rgb2ycrcb_map);
        free (tmp);
    }

    if (this->screen && this->screen->pixels) {
        this->screen->pixels = NULL;
    }
}

#endif /* _MGGAL_UTPMC */

#endif
