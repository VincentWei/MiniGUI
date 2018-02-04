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
** wvfb.c: Windows virtual FrameBuffer based video driver implementation.
*/

#include "mgconfig.h"

#ifdef _MGGAL_WVFB

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "wvfb.h"
#include "winfb.h"

/* Initialization/Query functions */
static int WVFB_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **WVFB_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *WVFB_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int WVFB_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void WVFB_VideoQuit(_THIS);

/* Hardware surface functions */
static int WVFB_AllocHWSurface(_THIS, GAL_Surface *surface);
static void WVFB_FreeHWSurface(_THIS, GAL_Surface *surface);


/* ----------------------------------------------------------------------------------- */

static int WVFB_Available (void)
{
    return win_FbAvailable ();
}

static void WVFB_DeleteDevice (GAL_VideoDevice *device)
{
    free (device->hidden);
    free (device);
}

static void WVFB_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;
    struct GAL_PrivateVideoData* data = this->hidden;

    win_FbLock ();

    bound = this->hidden->hdr->update;
    if (bound.right == -1) bound.right = 0;
    if (bound.bottom == -1) bound.bottom = 0;

    for (i = 0; i < numrects; i++) {
        RECT rc;

        SetRect (&rc, rects[i].x, rects[i].y, 
                        rects[i].x + rects[i].w, rects[i].y + rects[i].h);
        if (IsRectEmpty (&bound))
            bound = rc;
        else if (!IsRectEmpty (&rc))
            GetBoundRect (&bound, &bound, &rc);
    }

    this->hidden->hdr->update = bound;
    this->hidden->hdr->dirty = TRUE;

    win_FbUnlock ();
}

static GAL_VideoDevice *WVFB_CreateDevice (int devindex)
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
    this->VideoInit = WVFB_VideoInit;
    this->ListModes = WVFB_ListModes;
    this->SetVideoMode = WVFB_SetVideoMode;
    this->SetColors = WVFB_SetColors;
    this->VideoQuit = WVFB_VideoQuit;

    this->AllocHWSurface = WVFB_AllocHWSurface;
    this->FreeHWSurface = WVFB_FreeHWSurface;

    this->UpdateRects = WVFB_UpdateRects;

    this->CheckHWBlit = NULL;
    this->FillHWRect = NULL;
    this->SetHWColorKey = NULL;
    this->SetHWAlpha = NULL;

    this->free = WVFB_DeleteDevice;

    return this;
}

VideoBootStrap WVFB_bootstrap = {
    "wvfb", "Windows Virtual FrameBuffer",
    WVFB_Available, WVFB_CreateDevice
};

static int WVFB_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    struct GAL_PrivateVideoData* data = this->hidden;

    //TODO
    //check available

    data->shmrgn = win_FbInit (0, 0, 0);
    if ((int)data->shmrgn == -1 || data->shmrgn == NULL) {
        GAL_SetError ("NEWGAL>WVFB: Unable to attach to virtual FrameBuffer server.\n");
        return -1;
    }
    data->hdr = (struct WVFbHeader *) data->shmrgn;

    vformat->BitsPerPixel = data->hdr->depth;

    switch (vformat->BitsPerPixel) {
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
            vformat->Rmask = 0x00FF0000;
            vformat->Gmask = 0x0000FF00;
            vformat->Bmask = 0x000000FF;
            break;
        default:
            GAL_SetError ("NEWGAL>WVFB: Not supported depth: %d.\n", vformat->BitsPerPixel);
            return -1;
    }

    return 0;
}

GAL_Rect **WVFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    return (GAL_Rect **) -1;
}

GAL_Surface *WVFB_SetVideoMode (_THIS, GAL_Surface *current,
                                int width, int height, int bpp, Uint32 flags)
{
    /* Set up the mode framebuffer */
    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w = this->hidden->hdr->width;
    current->h = this->hidden->hdr->height;
    current->pitch = this->hidden->hdr->linestep;
    current->pixels = this->hidden->shmrgn + this->hidden->hdr->dataoffset;

    /* We're done */
    return current;
}

/* We don't actually allow hardware surfaces other than the main one */
static int WVFB_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return -1;
}

static void WVFB_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int WVFB_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    int i, pixel = firstcolor;

    for (i = 0; i < ncolors; i++) {
        this->hidden->hdr->clut [pixel] 
                = (0xff << 24) | ((colors[i].r & 0xff) << 16) | ((colors[i].g & 0xff) << 8) | (colors[i].b & 0xff);
        pixel ++;
    }

    return 0;
}

static void WVFB_VideoQuit (_THIS)
{
    win_FbClose ();
}

#endif /* _MGGAL_WVFB */
