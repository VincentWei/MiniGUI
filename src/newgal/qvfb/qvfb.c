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
** qvfb.c: Qt virtual FrameBuffer based video driver implementation.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#ifdef __ECOS__
/* for eCos Linux Synthetic target */
#include <cyg/hal/hal_io.h>
#else
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif
#include "minigui.h"
#include "misc.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "qvfb.h"

#ifdef __ECOS__

typedef int key_t;

#define shmget cyg_hal_sys_shmget
#define shmat cyg_hal_sys_shmat
#define shmdt cyg_hal_sys_shmdt
#define ftok cyg_hal_sys_ftok

#endif

/* Initialization/Query functions */
static int QVFB_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **QVFB_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *QVFB_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int QVFB_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void QVFB_VideoQuit(_THIS);

/* Hardware surface functions */
static int QVFB_AllocHWSurface(_THIS, GAL_Surface *surface);
static void QVFB_FreeHWSurface(_THIS, GAL_Surface *surface);

/* QVFB driver bootstrap functions */
static int QVFB_Available (void)
{
    char file [50];
    int display;
    key_t key;
    int shmid;

    if (GetMgEtcIntValue ("qvfb", "display", &display) < 0)
        display = 0;

    sprintf (file, QT_VFB_MOUSE_PIPE, display);
    key = ftok (file, 'b');

    shmid = shmget (key, 0, 0);
    return shmid != -1;
}

static void QVFB_DeleteDevice (GAL_VideoDevice *device)
{
    free (device->hidden);
    free (device);
}

static void QVFB_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;

    RECT bound = this->hidden->hdr->update;

    if (bound.right == -1) bound.right = 0;
    if (bound.bottom == -1) bound.bottom = 0;

    for (i = 0; i < numrects; i++) {
        RECT rc;

        SetRect (&rc, rects[i].x, rects[i].y, 
                        rects[i].x + rects[i].w, rects[i].y + rects[i].h);
        if (IsRectEmpty (&bound))
            bound = rc;
        else
            GetBoundRect (&bound, &bound, &rc);
    }

    this->hidden->hdr->update = bound;
    this->hidden->hdr->dirty = TRUE;
}

static GAL_VideoDevice *QVFB_CreateDevice (int devindex)
{
    GAL_VideoDevice *this;

    /* Initialize all variables that we clean on shutdown */
    this = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if (this) {
        memset (this, 0, (sizeof *this));
        this->hidden = (struct GAL_PrivateVideoData *) malloc ((sizeof *this->hidden));
    }

    if ((this == NULL) || (this->hidden == NULL)) {
        GAL_OutOfMemory ();
        if (this) free (this);
        return (0);
    }
    memset (this->hidden, 0, (sizeof *this->hidden));

    /* Set the function pointers */
    this->VideoInit = QVFB_VideoInit;
    this->ListModes = QVFB_ListModes;
    this->SetVideoMode = QVFB_SetVideoMode;
    this->SetColors = QVFB_SetColors;
    this->VideoQuit = QVFB_VideoQuit;

    this->AllocHWSurface = QVFB_AllocHWSurface;
    this->FreeHWSurface = QVFB_FreeHWSurface;

    this->UpdateRects = QVFB_UpdateRects;

    this->CheckHWBlit = NULL;
    this->FillHWRect = NULL;
    this->SetHWColorKey = NULL;
    this->SetHWAlpha = NULL;

    this->free = QVFB_DeleteDevice;

    return this;
}

VideoBootStrap QVFB_bootstrap = {
    "qvfb", "Qt Virtual FrameBuffer",
    QVFB_Available, QVFB_CreateDevice
};

static int QVFB_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    char file [50];
    int display;
    key_t key;
    int shmid;
    struct GAL_PrivateVideoData* data = this->hidden;

    if (GetMgEtcIntValue ("qvfb", "display", &display) < 0)
        display = 0;

    sprintf (file, QT_VFB_MOUSE_PIPE, display);
    key = ftok (file, 'b');

    shmid = shmget (key, 0, 0);
    if (shmid != -1)
        data->shmrgn = (unsigned char *)shmat (shmid, 0, 0);

    if ((int)data->shmrgn == -1 || data->shmrgn == NULL) {
        GAL_SetError ("NEWGAL>QVFB: Unable to attach to virtual FrameBuffer server.\n");
        return -1;
    }

     data->hdr = (struct QVFbHeader *) data->shmrgn;

    vformat->BitsPerPixel = data->hdr->depth;
    switch (vformat->BitsPerPixel) {
#ifdef _MGGAL_SHADOW
        case 1:
            break;
        case 4:
            break;
#endif
        case 8:
            vformat->BytesPerPixel = 1;
            data->hdr->numcols = 256;
            break;
        case 12:
            vformat->BitsPerPixel = 16;
            vformat->BytesPerPixel = 2;
            vformat->Rmask = 0x00000F00;
            vformat->Gmask = 0x000000F0;
            vformat->Bmask = 0x0000000F;
            break;
        case 16:
            vformat->BytesPerPixel = 2;
            vformat->Rmask = 0x0000F800;
            vformat->Gmask = 0x000007E0;
            vformat->Bmask = 0x0000001F;
            break;
        case 32:
            vformat->BytesPerPixel = 4;
            vformat->Amask = 0xFF000000;
            vformat->Rmask = 0x00FF0000;
            vformat->Gmask = 0x0000FF00;
            vformat->Bmask = 0x000000FF;
            break;
        default:
            GAL_SetError ("NEWGAL>QVFB: Not supported depth: %d, "
                "please try to use Shadow NEWGAL engine with targetname qvfb.\n", vformat->BitsPerPixel);
            return -1;
    }

    return 0;
}

static GAL_Rect **QVFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    return (GAL_Rect **) -1;
}

static GAL_Surface *QVFB_SetVideoMode (_THIS, GAL_Surface *current,
                                int width, int height, int bpp, Uint32 flags)
{
    /* Set up the mode framebuffer */
    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w = this->hidden->hdr->width;
    current->h = this->hidden->hdr->height;
    current->pitch = this->hidden->hdr->linestep;
    current->pixels = this->hidden->shmrgn + this->hidden->hdr->dataoffset;

#if 0
    /* houhh, if need set this for qvfb?*/
    struct GAL_PrivateVideoData* data = this->hidden;

    if (bpp == 32) {
        Uint32 Rmask;
        Uint32 Gmask;
        Uint32 Bmask;
        Uint32 Amask;

        Amask = 0xFF000000;
        Rmask = 0x00FF0000;
        Gmask = 0x0000FF00;
        Bmask = 0x000000FF;

        if (!GAL_ReallocFormat(current, bpp, Rmask, Gmask, Bmask, Amask) ) {
            return(NULL);
        }
    }
#endif

    return current;
}

/* We don't actually allow hardware surfaces other than the main one */
static int QVFB_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return -1;
}

static void QVFB_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int QVFB_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    int bpp = this->screen->format->BitsPerPixel;
    int i, pixel = firstcolor;

    if (bpp < 8) {
        int num = 1 <<  bpp;
        pixel = firstcolor >> bpp;
        colors = __gal_screen->format->palette->colors;
        for (i = pixel; i < num; i++) {
            int index = i << (8 - bpp);
            if (index) index--;
            this->hidden->hdr->clut [pixel] 
                = (0xff << 24) | ((colors[index].r & 0xff) << 16) | ((colors[index].g & 0xff) << 8) | (colors[index].b & 0xff);
            pixel ++;
        }
        /* set final color. */
        this->hidden->hdr->clut [(1<<bpp)-1] 
            = (0xff << 24) | ((colors[255].r & 0xff) << 16) | ((colors[255].g & 0xff) << 8) | (colors[255].b & 0xff);

    }
    else {
        for (i = 0; i < ncolors; i++) {
            this->hidden->hdr->clut [pixel]
                = (0xff << 24) | ((colors[i].r & 0xff) << 16) | ((colors[i].g & 0xff) << 8) | (colors[i].b & 0xff);
            pixel ++;
        }
    }

    return 1;
}

static void QVFB_VideoQuit (_THIS)
{
    shmdt (this->hidden->shmrgn);

}

