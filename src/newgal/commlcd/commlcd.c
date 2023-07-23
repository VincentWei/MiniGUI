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
/*
**  commlcd.c: CommonLCD NEWGAL video driver.
**    Support for common LCD driver under eCos, uC/OS-II, VxWorks, ...
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "misc.h"

#ifdef _MGGAL_COMMLCD

#include "commlcd.h"

#define COMMLCDVID_DRIVER_NAME "commlcd"

/* Initialization/Query functions */
static int COMMLCD_VideoInit (_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **COMMLCD_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *COMMLCD_SetVideoMode (_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int COMMLCD_SetColors (_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void COMMLCD_VideoQuit (_THIS);

/* Hardware surface functions */
static int COMMLCD_AllocHWSurface (_THIS, GAL_Surface *surface);
static void COMMLCD_FreeHWSurface (_THIS, GAL_Surface *surface);

/* COMMLCD driver bootstrap functions */

static int COMMLCD_Available(void)
{
    return 1;
}

static void COMMLCD_DeleteDevice(GAL_VideoDevice *device)
{
    free (device->hidden);
    free (device);
}

static void COMMLCD_UpdateRects_Sync (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;

    bound = this->hidden->rc_dirty;
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
        RECT rcScr = GetScreenRect();

        if (IntersectRect (&bound, &bound, &rcScr)) {
            this->hidden->rc_dirty = bound;
            this->hidden->dirty = TRUE;
        }
    }
}

static BOOL COMMLCD_SyncUpdate_Sync (_THIS)
{
    if (!IsRectEmpty (&this->hidden->rc_dirty)) {
        __mg_commlcd_ops.update (&this->hidden->rc_dirty);
        SetRect (&this->hidden->rc_dirty, 0, 0, 0, 0);
        this->hidden->dirty = FALSE;
        return TRUE;
    }

    return FALSE;
}

static BOOL COMMLCD_SyncUpdate_Async (_THIS)
{
    BOOL rc = FALSE;

    pthread_mutex_lock (&this->hidden->update_lock);

    if (this->hidden->dirty) {
        // signal the update thread to do update
        sem_post (&this->hidden->sem_update);
        rc = TRUE;
    }

    pthread_mutex_unlock (&this->hidden->update_lock);
    return rc;
}

static void* task_do_update (void* data)
{
    _THIS;
    this = data;

    if (pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL))
        return NULL;

    do {
        sem_wait (&this->hidden->sem_update);

        pthread_testcancel ();

        pthread_mutex_lock (&this->hidden->update_lock);
        if (this->hidden->dirty) {
            __mg_commlcd_ops.update (&this->hidden->rc_dirty);

            SetRect (&this->hidden->rc_dirty, 0, 0, 0, 0);
            this->hidden->dirty = FALSE;
        }
        pthread_mutex_unlock (&this->hidden->update_lock);

#if 0 /* since 5.0.0: use semaphore */
        __mg_os_time_delay(50); /* 50 ms */
#endif
    } while (1);

    return NULL;
}

static void COMMLCD_UpdateRects_Async (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;

    pthread_mutex_lock (&this->hidden->update_lock);

    bound = this->hidden->rc_dirty;
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
        RECT rcScr = GetScreenRect();

        if (IntersectRect (&bound, &bound, &rcScr)) {
            this->hidden->rc_dirty = bound;
            this->hidden->dirty = TRUE;
        }
    }

    pthread_mutex_unlock (&this->hidden->update_lock);
    return;
}

static GAL_VideoDevice *COMMLCD_CreateDevice (int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if ( device ) {
        memset(device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *)
                malloc((sizeof *device->hidden));
    }
    if ( (device == NULL) || (device->hidden == NULL) ) {
        GAL_OutOfMemory();
        if ( device ) {
            free(device);
        }
        return(0);
    }
    memset(device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = COMMLCD_VideoInit;
    device->ListModes = COMMLCD_ListModes;
    device->SetVideoMode = COMMLCD_SetVideoMode;
    device->SetColors = COMMLCD_SetColors;
    device->VideoQuit = COMMLCD_VideoQuit;
    device->AllocHWSurface = COMMLCD_AllocHWSurface;
    device->FreeHWSurface = COMMLCD_FreeHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->UpdateRects = NULL;
    device->SyncUpdate = NULL;

    device->free = COMMLCD_DeleteDevice;
    return device;
}

VideoBootStrap COMMLCD_bootstrap = {
    COMMLCDVID_DRIVER_NAME, "Common LCD video driver",
    COMMLCD_Available, COMMLCD_CreateDevice
};

static int COMMLCD_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    _ERR_PRINTF ("NEWGAL>COMMLCD: Calling init method!\n");

    /* Initialize LCD screen */
    if (__mg_commlcd_ops.init ()) {
        _ERR_PRINTF ("NEWGAL>COMMLCD: Couldn't initialize LCD\n");
        return -1;
    }

    /* Determine the screen depth (use default 8-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;

    /* We're done! */
    return(0);
}

static GAL_Surface *COMMLCD_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    Uint32 Rmask = 0, Gmask = 0, Bmask = 0, Amask = 0;
    struct commlcd_info li;
    memset (&li, 0, sizeof (struct commlcd_info));

    if (__mg_commlcd_ops.getinfo (&li, width, height, bpp)) {
        _ERR_PRINTF ("NEWGAL>COMMLCD: "
                "Couldn't get the LCD information\n");
        return NULL;
    }

    if (li.bpp != bpp) {
        _ERR_PRINTF ("NEWGAL>COMMLCD: "
                "Returned color depth (%d) does not matched the requested color depth (%d)\n",
                li.bpp, bpp);
    }

    this->hidden->w = li.width;
    this->hidden->h = li.height;
    this->hidden->pitch = li.pitch;
    this->hidden->fb = li.fb;

    switch (li.type) {
    case COMMLCD_PSEUDO_RGB332:
        Rmask = 0xE0;
        Gmask = 0x1C;
        Bmask = 0x03;
        break;
    case COMMLCD_TRUE_RGB555:
        Rmask = 0x7C00;
        Gmask = 0x03E0;
        Bmask = 0x001F;
        break;
    case COMMLCD_TRUE_RGB565:
        Rmask = 0xF800;
        Gmask = 0x07E0;
        Bmask = 0x001F;
        break;
    case COMMLCD_TRUE_RGB888:
    case COMMLCD_TRUE_RGB0888:
        Rmask = 0x00FF0000;
        Gmask = 0x0000FF00;
        Bmask = 0x000000FF;
        break;
    case COMMLCD_TRUE_ARGB1555:
        Amask = 0x8000;
        Rmask = 0x7C00;
        Gmask = 0x03E0;
        Bmask = 0x001F;
        break;
    case COMMLCD_TRUE_ARGB8888:
        Amask = 0xFF000000;
        Rmask = 0x00FF0000;
        Gmask = 0x0000FF00;
        Bmask = 0x000000FF;
        break;
    case COMMLCD_TRUE_ABRG8888:
        Amask = 0xFF000000;
        Bmask = 0x00FF0000;
        Rmask = 0x0000FF00;
        Gmask = 0x000000FF;
        break;
    }

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, li.bpp, Rmask, Gmask, Bmask, Amask)) {
        if (__mg_commlcd_ops.release)
            __mg_commlcd_ops.release ();

        this->hidden->fb = NULL;
        _ERR_PRINTF ("NEWGAL>COMMLCD: "
                "Couldn't allocate new pixel format for requested mode\n");
        return NULL;
    }

    /* Set up the new mode framebuffer */
    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w = this->hidden->w;
    current->h = this->hidden->h;
    current->pitch = this->hidden->pitch;
    current->pixels = this->hidden->fb;

    if (li.update_method == COMMLCD_UPDATE_NONE) {
        this->UpdateRects = NULL;
        this->SyncUpdate = NULL;
        this->hidden->update_th = 0;
    }
    else if (li.update_method == COMMLCD_UPDATE_ASYNC) {
        this->UpdateRects = COMMLCD_UpdateRects_Async;
        this->SyncUpdate = COMMLCD_SyncUpdate_Async;

        pthread_mutex_init (&this->hidden->update_lock, NULL);
        sem_init (&this->hidden->sem_update, 0, 0);
#if 0
        pthread_attr_t new_attr;

        pthread_attr_init (&new_attr);
        pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
        pthread_create (&this->hidden->update_th, &new_attr,
                        task_do_update, this);
        pthread_attr_destroy (&new_attr);
#else
        pthread_create (&this->hidden->update_th, NULL,
                        task_do_update, this);
#endif
    }
    else {
        this->UpdateRects = COMMLCD_UpdateRects_Sync;
        this->SyncUpdate = COMMLCD_SyncUpdate_Sync;
        this->hidden->update_th = 0;
    }

    /* We're done */
    return current;
}

static void COMMLCD_VideoQuit (_THIS)
{
    if (this->screen && this->screen->pixels) {
        this->screen->pixels = NULL;
    }

    if (this->hidden->update_th) {
        /* send cancel request */
        pthread_cancel (this->hidden->update_th);
        pthread_join (this->hidden->update_th, NULL);
    }

    if (__mg_commlcd_ops.release)
        __mg_commlcd_ops.release ();

    return;
}

static GAL_Rect **COMMLCD_ListModes (_THIS, GAL_PixelFormat *format,
                Uint32 flags)
{
    return (GAL_Rect **) -1;
}

/* We don't actually allow hardware surfaces other than the main one */
static int COMMLCD_AllocHWSurface (_THIS, GAL_Surface *surface)
{
    return -1;
}

static void COMMLCD_FreeHWSurface (_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int COMMLCD_SetColors (_THIS, int firstcolor, int ncolors,
                GAL_Color *colors)
{
    if (__mg_commlcd_ops.setclut)
        return __mg_commlcd_ops.setclut (firstcolor, ncolors, colors);

    return 0;
}

#endif /* _MGGAL_COMMLCD */

