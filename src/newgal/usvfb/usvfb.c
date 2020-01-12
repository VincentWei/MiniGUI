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
 *usvfb.c: UnixSocket Virtual Frame Buffer NEWGAL video driver.
 *
 \verbatim

    The USVFB NEWGAL/IAL engines implement the communicate with the Web Display Server
    in the following manner:

    1. A websocket client connects to the server, tells the server the demo it want to run.
       It will also tell the server the resolution (width and height in pixels) to show the demo.

    2. The server forks and execute the MiniGUI program specified by the websocket client
       in the child. Before this, the server should set the following environment variables
       for the MiniGUI client.

        * MG_GAL_ENGINE: should be usvfb
        * MG_DEFAULTMODE: should be a pattern like 320x240-16bpp
        * MG_IAL_ENGINE: shold be usvfb

    3. The server creates a UNIX domain socket at a well-known path (/var/tmp/mg-remote-sever)
       and wait for the connection from the MiniGUI client.

    4. The MiniGUI client connects to the UNIX socket, creates a shared memory as a
       virtual frame buffer, and sends the identifier of the shared memory to the server
       via the UNIX socket.

    5. Note that the resolution of the virtual frame buffer should be sent by the server as
       the first message.

    6. The server accepts the connection request from the MiniGUI client, gets the
       identifier of the shared memory sent by the client via the UNIX socket,
       and attaches to the shared memory.

    7. The MiniGUI client sends the dirty rectangle information via the
       UNIX socket, and the server then sends the information and the pixel data
       to the websocket client.

    8. The server gets the input events from the websocket client and sends them
       to the MiniGUI client via the UNIX socket.

    9. Note that the MiniGUI client should run in MiniGUI-Processes or MiniGUI-Standalone
       runmode.

    Copyright (C) 2018, Beijing FMSoft Technologies Co., Ltd (www.fmsoft.cn).

 \endverbatim
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#include <pthread.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#ifdef _MGGAL_USVFB

#include "usvfb.h"

#define USVFBVID_DRIVER_NAME "usvfb"

/* Initialization/Query functions */
static int USVFB_VideoInit (_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **USVFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *USVFB_SetVideoMode (_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int USVFB_SetColors (_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void USVFB_VideoQuit (_THIS);

/* Hardware surface functions */
static int USVFB_AllocHWSurface (_THIS, GAL_Surface *surface);
static void USVFB_FreeHWSurface (_THIS, GAL_Surface *surface);

struct _frame_header {
    int type;
    size_t payload_len;
    unsigned char payload[0];
};

int __mg_usvfb_fd = -1;
static struct _vfb_info _vfb_info;

static int a_init (void)
{
    int len, retval;
    struct sockaddr_un unix_addr;

    /* create a Unix domain stream socket */
    if ((__mg_usvfb_fd = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) {
        retval = 1;
        goto error;
    }

    /* fill socket address structure w/our address */
    memset (&unix_addr, 0, sizeof (unix_addr));
    unix_addr.sun_family = AF_UNIX;
    sprintf (unix_addr.sun_path, USC_PATH, getpid());
    len = sizeof (unix_addr.sun_family) + strlen (unix_addr.sun_path);

    unlink (unix_addr.sun_path);        /* in case it already exists */
    if (bind (__mg_usvfb_fd, (struct sockaddr *) &unix_addr, len) < 0) {
        retval = 2;
        goto error;
    }

    if (chmod (unix_addr.sun_path, USC_PERM) < 0) {
        retval = 3;
        goto error;
    }

    /* fill socket address structure w/server's addr */
    memset (&unix_addr, 0, sizeof (unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy (unix_addr.sun_path, USS_PATH);
    len = sizeof (unix_addr.sun_family) + strlen (unix_addr.sun_path);

    if (connect (__mg_usvfb_fd, (struct sockaddr *) &unix_addr, len) < 0) {
        retval = 4;
        goto error;
    }

    return 0;

error:
    if (__mg_usvfb_fd) {
        close (__mg_usvfb_fd);
    }

    return retval;
}

/* return zero for success */
static int a_getinfo (struct _vfb_info *li, int width, int height, int bpp)
{
    ssize_t n;
    int retval = 0;
    struct _frame_header header;

    if (width <= 0 || width > 1024 || height <= 0 || height > 768
            || (bpp != 16 && bpp != 32)) {
        retval = 1;
        goto error;
    }

    _vfb_info.width = width;
    _vfb_info.height = height;
    if (bpp == 16) {
        _vfb_info.type = USVFB_TRUE_RGB565;
        _vfb_info.rlen = (_vfb_info.width * 2 + 3)/4*4;
        _vfb_info.bpp = 16;
    }
    else {
        _vfb_info.type = USVFB_TRUE_RGB0888;
        _vfb_info.rlen = (_vfb_info.width * 4 + 3)/4*4;
        _vfb_info.bpp = 32;
    }

    _vfb_info.fb = calloc (height * _vfb_info.rlen, sizeof (char));
    if (_vfb_info.fb == NULL) {
        retval = 2;
        goto error;
    }

    header.type = FT_VFBINFO;
    header.payload_len = sizeof (struct _vfb_info);
    n = write (__mg_usvfb_fd, &header, sizeof (struct _frame_header));
    n += write (__mg_usvfb_fd, &_vfb_info, sizeof (struct _vfb_info));
    if (n != sizeof (struct _frame_header) + header.payload_len) {
        retval = 3;
        goto error;
    }

    _vfb_info.async_update = 0;
    memcpy (li, &_vfb_info, sizeof (struct _vfb_info));
    return retval;

error:
    if (__mg_usvfb_fd) {
        close (__mg_usvfb_fd);
    }

    return retval;
}

static int a_update (const RECT* rc_dirty)
{
    int i, Bpp, dirty_w, dirty_h;
    unsigned char* pixels;
    ssize_t n;
    size_t bytes;
    struct _frame_header header;

    dirty_w = RECTWP (rc_dirty);
    dirty_h = RECTHP (rc_dirty);

    if (dirty_w < 0 || rc_dirty->left < 0 || (rc_dirty->left + dirty_w) > _vfb_info.width
            || dirty_h < 0 || rc_dirty->top < 0 || (rc_dirty->top + dirty_h) > _vfb_info.height) {
        return 1;
    }

    Bpp = 2;
    if (_vfb_info.bpp == 32)
        Bpp = 4;

    bytes = dirty_w * dirty_h * Bpp;

    header.type = FT_DIRTYPIXELS;
    header.payload_len = sizeof (RECT) + bytes;

    n = write (__mg_usvfb_fd, &header, sizeof (struct _frame_header));
    n += write (__mg_usvfb_fd, rc_dirty, sizeof (RECT));

    pixels = _vfb_info.fb + _vfb_info.rlen * rc_dirty->top + rc_dirty->left * Bpp;
    for (i = 0; i < dirty_h; i++) {
        n += write (__mg_usvfb_fd, pixels, dirty_w * Bpp);

        pixels += _vfb_info.rlen;
    }

    if (n != header.payload_len) {
        return 2;
    }

    return 0;
}

static int a_release (void)
{
    free (_vfb_info.fb);
    close (__mg_usvfb_fd);

    return 0;
}

/* USVFB driver bootstrap functions */
static int USVFB_Available(void)
{
    return 1;
}

static void USVFB_DeleteDevice(GAL_VideoDevice *device)
{
    free (device->hidden);
    free (device);
}

static void USVFB_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;

    if (this->hidden->update_th == 0) {
        /* sync update */
        SetRect (&bound, 0, 0, 0, 0);
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
            a_update (&bound);
        }

        return;
    }

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

static GAL_VideoDevice *USVFB_CreateDevice (int devindex)
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
    device->VideoInit = USVFB_VideoInit;
    device->ListModes = USVFB_ListModes;
    device->SetVideoMode = USVFB_SetVideoMode;
    device->SetColors = USVFB_SetColors;
    device->VideoQuit = USVFB_VideoQuit;
    device->AllocHWSurface = USVFB_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = USVFB_FreeHWSurface;
    device->UpdateRects = USVFB_UpdateRects;

    device->free = USVFB_DeleteDevice;
    return device;
}

VideoBootStrap USVFB_bootstrap = {
    USVFBVID_DRIVER_NAME, "UnixSocket Virtial Frame Buffer video driver",
    USVFB_Available, USVFB_CreateDevice
};

static int USVFB_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    /* Initialize LCD screen */
    if (a_init ()) {
        _ERR_PRINTF ("NEWGAL>USVFB: Couldn't initialize\n");
        return -1;
    }

    /* Determine the screen depth (use default 8-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;

    /* We're done! */
    return(0);
}

static void* task_do_update (void* data)
{
    _THIS;
    this = data;

    do {
        pthread_mutex_lock (&this->hidden->update_lock);
        if (this->hidden->dirty) {
            a_update (&this->hidden->rc_dirty);

            SetRect (&this->hidden->rc_dirty, 0, 0, 0, 0);
            this->hidden->dirty = FALSE;
        }
        pthread_mutex_unlock (&this->hidden->update_lock);
        usleep (50*1000); /* 50 ms */
    } while (1);

    return NULL;
}

static GAL_Surface *USVFB_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    Uint32 Rmask = 0, Gmask = 0, Bmask = 0, Amask = 0;
    int retval;
    struct _vfb_info li;
    memset (&li, 0, sizeof (struct _vfb_info));

    if ((retval = a_getinfo (&li, width, height, bpp))) {
        _ERR_PRINTF ("NEWGAL>USVFB: Couldn't get the VFB information: %d (%d, %d, %d)\n", retval, width, height, bpp);
        return NULL;
    }

    this->hidden->w = li.width;
    this->hidden->h = li.height;
    this->hidden->pitch = li.rlen;
    this->hidden->fb = li.fb;

    memset (li.fb, 0, li.rlen * height);

    switch (li.type) {
    case USVFB_PSEUDO_RGB332:
        Rmask = 0xE0;
        Gmask = 0x1C;
        Bmask = 0x03;
        break;
    case USVFB_TRUE_RGB555:
        Rmask = 0x7C00;
        Gmask = 0x03E0;
        Bmask = 0x001F;
        break;
    case USVFB_TRUE_RGB565:
        Rmask = 0xF800;
        Gmask = 0x07E0;
        Bmask = 0x001F;
        break;
    case USVFB_TRUE_RGB888:
    case USVFB_TRUE_RGB0888:
        Rmask = 0x00FF0000;
        Gmask = 0x0000FF00;
        Bmask = 0x000000FF;
        break;
    case USVFB_TRUE_ARGB1555:
        Amask = 0x8000;
        Rmask = 0x7C00;
        Gmask = 0x03E0;
        Bmask = 0x001F;
        break;
    case USVFB_TRUE_ARGB8888:
        Amask = 0xFF000000;
        Rmask = 0x00FF0000;
        Gmask = 0x0000FF00;
        Bmask = 0x000000FF;
        break;
    }

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, li.bpp, Rmask, Gmask, Bmask, Amask)) {
        a_release ();

        this->hidden->fb = NULL;
        fprintf (stderr, "NEWGAL>USVFB: "
                "Couldn't allocate new pixel format for requested mode\n");
        return NULL;
    }

    /* Set up the new mode framebuffer */
    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w = this->hidden->w;
    current->h = this->hidden->h;
    current->pitch = this->hidden->pitch;
    current->pixels = this->hidden->fb;

    if (li.async_update) {
        pthread_attr_t new_attr;

        pthread_attr_init (&new_attr);
        pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
        pthread_create (&this->hidden->update_th, &new_attr,
                        task_do_update, this);
        pthread_attr_destroy (&new_attr);
        pthread_mutex_init(&this->hidden->update_lock, NULL);
    }
    else {
        this->hidden->update_th = 0;
    }

    /* We're done */
    return current;
}

static void USVFB_VideoQuit (_THIS)
{
    if (this->screen && this->screen->pixels) {
        this->screen->pixels = NULL;
    }

    if (this->hidden->update_th) {
        /* quit the update task */
    }

    a_release ();
    return;
}

static GAL_Rect **USVFB_ListModes (_THIS, GAL_PixelFormat *format,
                Uint32 flags)
{
    return (GAL_Rect **) -1;
}

/* We don't actually allow hardware surfaces other than the main one */
static int USVFB_AllocHWSurface (_THIS, GAL_Surface *surface)
{
    return -1;
}

static void USVFB_FreeHWSurface (_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int USVFB_SetColors (_THIS, int firstcolor, int ncolors,
                GAL_Color *colors)
{
    return 0;
}

#endif /* _MGGAL_USVFB */

