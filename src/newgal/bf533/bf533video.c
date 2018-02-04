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
**  $Id: bf533video.c 8944 2007-12-29 08:29:16Z xwyan $
**  
**  Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sched.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#include "bf533video.h"

#define BF533VID_DRIVER_NAME    "bf533"
#define DEV_SPI                 "/dev/spi"
#define LOCK_FILE               "/var/tmp/fmmg"

static OSD_OP_HEADER osd_op_header = {
    "fmmg", 0, 0
};

static Uint16 osd_ret = OSD_OK;
static BOOL spi_ok;

#if 0
static sigset_t oldmask;

#define BLOCK_ALARM                    \
do {                            \
    sigset_t newmask;            \
    sigemptyset (&newmask);            \
    sigaddset (&newmask, SIGALRM);            \
    sigprocmask (SIG_BLOCK, &newmask, &oldmask);    \
} while (0);

#define UNBLOCK_ALARM                    \
    sigprocmask (SIG_SETMASK, &oldmask, NULL);
#else
#define BLOCK_ALARM
#define UNBLOCK_ALARM
#endif

/* Initialization/Query functions */
static int BF533_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **BF533_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *BF533_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int BF533_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void BF533_UpdateRects (_THIS, int numrects, GAL_Rect *rects);
static void BF533_VideoQuit(_THIS);

/* Hardware surface functions */
static int BF533_AllocHWSurface(_THIS, GAL_Surface *surface);
static void BF533_FreeHWSurface(_THIS, GAL_Surface *surface);

static char stack_updater [1024];

static int task_do_update (void* data);

/* BF533 driver bootstrap functions */

static int BF533_Available(void)
{
    return (1);
}

static void BF533_DeleteDevice(GAL_VideoDevice *device)
{
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice *BF533_CreateDevice(int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if ( device ) {
        memset(device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *)
                calloc(1, (sizeof *device->hidden));
    }
    if ( (device == NULL) || (device->hidden == NULL) ) {
        GAL_OutOfMemory();
        if ( device ) {
            free(device);
        }
        return(0);
    }
    memset(device->hidden, 0, (sizeof *device->hidden));
    device->hidden->fd_spi = -1;
    device->hidden->fd_lock = -1;

    /* Set the function pointers */
    device->VideoInit = BF533_VideoInit;
    device->ListModes = BF533_ListModes;
    device->SetVideoMode = BF533_SetVideoMode;
    device->SetColors = BF533_SetColors;
    device->VideoQuit = BF533_VideoQuit;
#ifndef _MGRM_THREADS
    device->RequestHWSurface = NULL;
#endif
    device->AllocHWSurface = BF533_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = BF533_FreeHWSurface;
    device->UpdateRects = BF533_UpdateRects;

    device->free = BF533_DeleteDevice;

    return device;
}

VideoBootStrap BF533_bootstrap = {
    BF533VID_DRIVER_NAME, "BF533 OSD video driver",
    BF533_Available, BF533_CreateDevice
};

static int BF533_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    fprintf(stderr, "NEWGAL>BF533: Calling init method!\n");

    this->hidden->fd_spi = open (DEV_SPI, O_RDWR);
    if (this->hidden->fd_spi < 0) {
        fprintf(stderr, "NEWGAL>BF533: can not open SPI: %s!\n", DEV_SPI);
        return -1;
    }

    this->hidden->fd_lock = open (LOCK_FILE, O_CREAT | O_RDWR, 0600);
    if (this->hidden->fd_lock < 0) {
        fprintf(stderr, "NEWGAL>BF533: can not open lock file: %s!\n", LOCK_FILE);
        return -1;
    }
    spi_ok = TRUE;

    this->hidden->dirty = FALSE;
    SetRect (&this->hidden->update, 0, 0, 0, 0);

    /* Determine the screen depth (use default 8-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;

    /* We're done! */
    return(0);
}

static GAL_Rect standard_mode = {0, 0, OSD_WIDTH, OSD_HEIGHT};
static GAL_Rect* modes []  = {
    &standard_mode,
    NULL
};

static GAL_Rect **BF533_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    if (format->BitsPerPixel == 8 || format->BitsPerPixel == 16) {
        return modes;
    }
    
    return NULL;
}

static GAL_Surface *BF533_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    if ( this->hidden->buffer ) {
        free( this->hidden->buffer );
    }

    this->hidden->buffer = malloc (width * height * (bpp / 8));
    if ( ! this->hidden->buffer ) {
        fprintf (stderr, "NEWGAL>BF533: Couldn't allocate buffer for requested mode\n");
        return(NULL);
    }

    memset (this->hidden->buffer, 0, width * height * (bpp / 8));

    /* Allocate the new pixel format for the screen */
    if ( ! GAL_ReallocFormat (current, bpp, 0, 0, 0, 0) ) {
        free (this->hidden->buffer);
        this->hidden->buffer = NULL;
        fprintf (stderr, "NEWGAL>BF533: Couldn't allocate new pixel format for requested mode\n");
        return (NULL);
    }

    /* Open bf533 OSD */
    osd_op_header.op_id = OSD_OP_OPEN;
    osd_op_header.data_len = sizeof (Uint32);

    BLOCK_ALARM
    write (this->hidden->fd_spi, &osd_op_header, sizeof (OSD_OP_HEADER));
    write (this->hidden->fd_spi, &bpp, sizeof (Uint32));
#if 0
    do {
        read (this->hidden->fd_spi, &osd_ret, sizeof (Uint16));
    } while (osd_ret == OSD_AGAIN || osd_ret == OSD_AGAIN1);
#endif
    UNBLOCK_ALARM

    if (osd_ret != OSD_OK) {
        free (this->hidden->buffer);
        this->hidden->buffer = NULL;
        fprintf (stderr, "NEWGAL>BF533: Couldn't open BF533 OSD\n");
        return NULL;
    }

    /* Set up the new mode framebuffer */
    current->flags = flags & GAL_FULLSCREEN;
    this->hidden->w = current->w = width;
    this->hidden->h = current->h = height;
    current->pitch = current->w * (bpp / 8);
    current->pixels = this->hidden->buffer;

    clone (task_do_update, stack_updater + 1024, CLONE_VM | CLONE_FS | CLONE_FILES, this);

    /* We're done */
    return (current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int BF533_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return (-1);
}
static void BF533_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int BF533_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    osd_op_header.op_id = OSD_OP_SET_PALETTE;
    osd_op_header.data_len = sizeof (Uint32) * 2 + sizeof (GAL_Color) * ncolors;

    BLOCK_ALARM
    write (this->hidden->fd_spi, &osd_op_header, sizeof (OSD_OP_HEADER));

    write (this->hidden->fd_spi, &firstcolor, sizeof (Uint32));
    write (this->hidden->fd_spi, &ncolors, sizeof (Uint32));
    write (this->hidden->fd_spi, colors, sizeof (GAL_Color) * ncolors);

#if 0
    do {
        read (this->hidden->fd_spi, &osd_ret, sizeof (Uint16));
    } while (osd_ret == OSD_AGAIN || osd_ret == OSD_AGAIN1);
#endif
    UNBLOCK_ALARM

    if (osd_ret != OSD_OK) {
        fprintf (stderr, "NEWGAL>BF533: Couldn't set OSD palette\n");
        return 0;
    }

    return (1);
}

static void round_rect_to_even (RECT* rc)
{
    if (rc->left % 2) rc->left --;
    if (rc->right % 2) rc->right ++;
}

static int task_do_update (void* data)
{
    _THIS;
    int fd_lock = open (LOCK_FILE, O_RDWR);

    this = data;

    if (fd_lock < 0) {
        fprintf(stderr, "NEWGAL>BF533>updater: can not open lock file: %s!\n", LOCK_FILE);
        return 1;
    }

    while (this->hidden->fd_lock >= 0) {
        Uint32 w, h, pitch;
        Uint8* update_buff;

        usleep (100000);    // 100 ms

        if (!spi_ok)
            continue;

        if (this->hidden->dirty) {
            int i;
            RECT bound;

            flock (fd_lock, LOCK_EX);

            bound = this->hidden->update;
            round_rect_to_even (&bound);

#if 0
            printf ("task_do_update: Do update rect: (%d, %d, %d, %d).\n", 
                        bound.left, bound.top, bound.right, bound.bottom);
#endif

            w = RECTW (bound);
            h = RECTH (bound);
            pitch = w * this->screen->format->BytesPerPixel;

            osd_op_header.op_id = OSD_OP_UPDATE_RECT;
            osd_op_header.x = bound.left;
            osd_op_header.y = bound.top;
            osd_op_header.w = w;
            osd_op_header.h = h;
            osd_op_header.pitch = pitch;
            osd_op_header.data_len = pitch * h;

            BLOCK_ALARM
            write (this->hidden->fd_spi, &osd_op_header, sizeof (OSD_OP_HEADER));

            update_buff = this->hidden->buffer 
                    + this->screen->pitch * bound.top
                    + this->screen->format->BytesPerPixel * bound.left;

            for (i = 0; i < h; i++) {
                write (this->hidden->fd_spi, update_buff, pitch);
                update_buff += this->screen->pitch;
            }

#if 0
            do {
                read (this->hidden->fd_spi, &osd_ret, sizeof (Uint16));
            } while (osd_ret == OSD_AGAIN || osd_ret == OSD_AGAIN1);
#endif
            UNBLOCK_ALARM

            if (osd_ret != OSD_OK) {
                fprintf (stderr, "NEWGAL>BF533: Error when updating OSD\n");
            }

            this->hidden->dirty = FALSE;
            SetRect (&this->hidden->update, 0, 0, 0, 0);
            flock (fd_lock, LOCK_UN);
        }
    }

    close (fd_lock);
    return 0;
}

static void BF533_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;

    flock (this->hidden->fd_lock, LOCK_EX);

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

    flock (this->hidden->fd_lock, LOCK_UN);
}

static void BF533_VideoQuit(_THIS)
{
    if (this->hidden->fd_spi >= 0) {

        close (this->hidden->fd_lock);
        this->hidden->fd_lock = -1;

        osd_op_header.op_id = OSD_OP_CLOSE;
        osd_op_header.data_len = 0;

        BLOCK_ALARM
        write (this->hidden->fd_spi, &osd_op_header, sizeof (OSD_OP_HEADER));
#if 0
        do {
            read (this->hidden->fd_spi, &osd_ret, sizeof (Uint16));
        } while (osd_ret == OSD_AGAIN || osd_ret == OSD_AGAIN1);
#endif
        UNBLOCK_ALARM

        if (osd_ret != OSD_OK) {
            fprintf (stderr, "NEWGAL>BF533: Couldn't close BF533 OSD\n");
        }

        close (this->hidden->fd_spi);
        this->hidden->fd_spi = -1;
    }

    if (this->screen->pixels != NULL)
    {
        free(this->screen->pixels);
        this->screen->pixels = NULL;
    }
}

int bf533_suspend_spi (void)
{
    if (!spi_ok)
        return -1;

    spi_ok = FALSE;
    BLOCK_ALARM
    return current_video->hidden->fd_spi;
}

void bf533_resume_spi (void)
{
    if (spi_ok)
        return;

    spi_ok = TRUE;
    UNBLOCK_ALARM
    BF533_UpdateRects (current_video, 1, &standard_mode);
}

