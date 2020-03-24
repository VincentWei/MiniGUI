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

/* Dummy GAL video driver implementation; this is just enough to make an
 *  GAL-based application THINK it's got a working video driver, for
 *  applications that call GAL_Init(GAL_INIT_VIDEO) when they don't need it,
 *  and also for use as a collection of stubs when porting GAL to a new
 *  platform for which you haven't yet written a valid video driver.
 *
 * This is also a great way to determine bottlenecks: if you think that GAL
 *  is a performance problem for a given platform, enable this driver, and
 *  then see if your application runs faster without video overhead.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#ifdef _MGGAL_DUMMY

#include "nullvideo.h"

#if defined(_MGRM_PROCESSES) && defined (_MGSCHEMA_SHAREDFB)
/* Since 5.0.0,
   we use shm_open for the shared frame buffer among processes
   under MiniGUI-Processes runtime mode and shared frame buffer schema. */
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>

#include "sharedres.h"

#define DUMMYVID_SHAREDMEM_NAME "/minigui-procs-dummy"
#endif

#define DUMMYVID_DRIVER_NAME "dummy"

/* Initialization/Query functions */
static int DUMMY_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **DUMMY_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *DUMMY_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int DUMMY_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void DUMMY_VideoQuit(_THIS);

/* Hardware surface functions */
static int DUMMY_AllocHWSurface(_THIS, GAL_Surface *surface);
static void DUMMY_FreeHWSurface(_THIS, GAL_Surface *surface);

/* DUMMY driver bootstrap functions */

static int DUMMY_Available(void)
{
    return(1);
}

static void DUMMY_DeleteDevice(GAL_VideoDevice *device)
{
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice *DUMMY_CreateDevice(int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if (device) {
        memset(device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *)
                malloc((sizeof *device->hidden));
    }
    if ((device == NULL) || (device->hidden == NULL)) {
        GAL_OutOfMemory();
        if (device) {
            free(device);
        }
        return(0);
    }
    memset (device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = DUMMY_VideoInit;
    device->ListModes = DUMMY_ListModes;
    device->SetVideoMode = DUMMY_SetVideoMode;
    device->SetColors = DUMMY_SetColors;
    device->VideoQuit = DUMMY_VideoQuit;
    device->AllocHWSurface = DUMMY_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = DUMMY_FreeHWSurface;

    device->free = DUMMY_DeleteDevice;

    return device;
}

VideoBootStrap DUMMY_bootstrap = {
    DUMMYVID_DRIVER_NAME, "Dummy video driver",
    DUMMY_Available, DUMMY_CreateDevice
};


static int DUMMY_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
#if defined(_MGRM_PROCESSES) && defined (_MGSCHEMA_SHAREDFB)
    struct GAL_PrivateVideoData* data = this->hidden;

    if (mgIsServer) {
        data->fd = shm_open (DUMMYVID_SHAREDMEM_NAME,
                O_CREAT | O_RDWR | O_CLOEXEC, 0666);
        if (data->fd < 0)
            return -1;
    }
    else {
        data->fd = shm_open (DUMMYVID_SHAREDMEM_NAME,
                O_RDWR | O_CLOEXEC, 0666);
        if (data->fd < 0)
            return -1;
    }
#endif

    /* Determine the screen depth (use default 8-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;

    /* We're done! */
    return(0);
}

static GAL_Rect **DUMMY_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    if (format->BitsPerPixel < 8) {
        return NULL;
    }

    return (GAL_Rect**) -1;
}

static GAL_Surface *DUMMY_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    int pitch;

#if defined(_MGRM_PROCESSES) && defined (_MGSCHEMA_SHAREDFB)
    if (mgIsServer) {

        pitch = width * ((bpp + 7) / 8);
        pitch = (pitch + 3) & ~3;
        this->hidden->length = pitch * height;

        if (ftruncate (this->hidden->fd, this->hidden->length)) {
            close (this->hidden->fd);
            this->hidden->fd = -1;
            _ERR_PRINTF ("NEWGAL>DUMMY: "
                    "failed to calling ftruncate to set length of shared FB\n");
            return NULL;
        }
    }
    else {
        /* get the video mode info from shared resource */
        bpp = SHAREDRES_VIDEO_DEPTH;
        width = SHAREDRES_VIDEO_HRES;
        height = SHAREDRES_VIDEO_VRES;

        pitch = width * ((bpp + 7) / 8);
        pitch = (pitch + 3) & ~3;
        this->hidden->length = pitch * height;
    }

    this->hidden->buffer =
        mmap (NULL, this->hidden->length, PROT_READ | PROT_WRITE, MAP_SHARED,
                this->hidden->fd, 0);
    if (this->hidden->buffer == MAP_FAILED) {
        _ERR_PRINTF ("NEWGAL>DUMMY: "
                "failed to map the shared FB object\n");
        close (this->hidden->fd);
        this->hidden->fd = -1;
        return NULL;
    }

    /* mmap succeed, close fd */
    close (this->hidden->fd);
    this->hidden->fd = -1;

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, bpp, 0, 0, 0, 0)) {
        munmap (this->hidden->buffer, this->hidden->length);
        close (this->hidden->fd);

        this->hidden->buffer = NULL;
        this->hidden->fd = -1;

        _ERR_PRINTF ("NEWGAL>DUMMY: "
                "Couldn't allocate new pixel format for requested mode\n");
        return (NULL);
    }

#else   /* defined(_MGRM_PROCESSES) && defined (_MGSCHEMA_SHAREDFB) */

    if (this->hidden->buffer) {
        free (this->hidden->buffer);
    }

    pitch = width * ((bpp + 7) / 8);
    pitch = (pitch + 3) & ~3;

    this->hidden->buffer = malloc (pitch * height);
    if (!this->hidden->buffer) {
        _ERR_PRINTF ("NEWGAL>DUMMY: "
                "Couldn't allocate buffer for requested mode\n");
        return NULL;
    }

    memset (this->hidden->buffer, 0, pitch * height);

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, bpp, 0, 0, 0, 0)) {
        free(this->hidden->buffer);
        this->hidden->buffer = NULL;
        _ERR_PRINTF ("NEWGAL>DUMMY: "
                "Couldn't allocate new pixel format for requested mode\n");
        return(NULL);
    }
#endif  /*! (defined(_MGRM_PROCESSES) && defined (_MGSCHEMA_SHAREDFB)) */

    /* Set up the new mode framebuffer */
    current->flags = GAL_FULLSCREEN | GAL_HWSURFACE;
    this->hidden->w = current->w = width;
    this->hidden->h = current->h = height;
    current->pitch = pitch;
    current->pixels = this->hidden->buffer;

    /* We're done */
    return (current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int DUMMY_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return(-1);
}
static void DUMMY_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int DUMMY_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another video routine -- notably UpdateRects.
*/
static void DUMMY_VideoQuit(_THIS)
{
#if defined(_MGRM_PROCESSES) && defined (_MGSCHEMA_SHAREDFB)
    if (this->hidden->buffer) {
        munmap (this->hidden->buffer, this->hidden->length);
        if (this->hidden->fd >= 0)
            close (this->hidden->fd);

        this->hidden->buffer = NULL;
        this->hidden->fd = -1;
    }

    if (mgIsServer) {
        shm_unlink (DUMMYVID_SHAREDMEM_NAME);
    }
#else
    if (this->screen->pixels != NULL) {
        free (this->screen->pixels);
        this->screen->pixels = NULL;
    }
#endif  /* not (defined(_MGRM_PROCESSES) && defined (_MGSCHEMA_SHAREDFB)) */
}

#endif /* _MGGAL_DUMMY */
