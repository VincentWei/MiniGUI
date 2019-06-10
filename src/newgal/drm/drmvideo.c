/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2019, Beijing FMSoft Technologies Co., Ltd.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _DEBUG
#include "common.h"

#ifdef _MGGAL_DRM

#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <drm/drm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "minigui.h"
#include "newgal.h"
#include "exstubs.h"

#include "pixels_c.h"
#include "drmvideo.h"

#define DRM_DRIVER_NAME "drm"

/* Initialization/Query functions */
static int DRM_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **DRM_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *DRM_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int DRM_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void DRM_VideoQuit(_THIS);

/* Hardware surface functions */
static int DRM_AllocHWSurface(_THIS, GAL_Surface *surface);
static void DRM_FreeHWSurface(_THIS, GAL_Surface *surface);

/* DRM driver bootstrap functions */

static int DRM_Available(void)
{
    return drmAvailable();
}

static void DRM_DeleteDevice(GAL_VideoDevice *device)
{
    if (device->hidden->driver && device->hidden->driver_ops) {
        device->hidden->driver_ops->destroy_driver(device->hidden->driver);
    }

    free(device->hidden);
    free(device);
}

static char* find_driver_for_device (const char *dev_name)
{
    char *driver;
    int major_number, minor_number;
    struct stat file_attrs;
    char *device_path;
    char device_link_path[PATH_MAX + 1] = "";
    int ret;

    if (stat (dev_name, &file_attrs) < 0) {
        _ERR_PRINTF("NEWGAL>DRM: failed to call stat on %s\n", dev_name);
        return NULL;
    }

    if (!S_ISCHR (file_attrs.st_mode)) {
        _ERR_PRINTF("NEWGAL>DRM: %s is not a character device\n", dev_name);
        return NULL;
    }

    major_number = major (file_attrs.st_rdev);
    minor_number = minor (file_attrs.st_rdev);

    ret = asprintf (&device_path, "/sys/dev/char/%d:%d/device/driver",
            major_number, minor_number);
    if (ret < 0) {
        _ERR_PRINTF("NEWGAL>DRM: failed to call asprintf to build device path\n");
        return NULL;
    }

    if (readlink (device_path, device_link_path,
            sizeof (device_link_path) - 1) < 0) {
        free (device_path);
        return NULL;
    }

    _DBG_PRINTF("NEWGAL>DRM: device link path: %s\n", device_link_path);

    free (device_path);
    driver = strrchr (device_link_path, '/');
    if (driver == NULL)
        return NULL;

    return strdup (driver + strlen ("/"));
}

static int open_drm_device(GAL_VideoDevice *device)
{
    char *driver_name;
    int device_fd;

    driver_name = find_driver_for_device(device->hidden->dev_name);
    _DBG_PRINTF("NEWGAL>DRM: Trye to load DRM driver: %s\n", driver_name);
    device_fd = drmOpen(driver_name, NULL);

    if (device_fd < 0) {
        _ERR_PRINTF("drmOpen failed");
        free(driver_name);
        return -1;
    }

    if (strcmp(driver_name, "i915") == 0) {
        _DBG_PRINTF("NEWGAL>DRM: found driver: i915\n");
    }
    else {
#ifdef __TARGET_EXTERNAL__
        device->hidden->driver_ops = __drm_ex_driver_get(driver_name);
#endif
    }

    free (driver_name);
    if (device->hidden->driver_ops == NULL) {
        close (device_fd);
        return -1;
    }

    device->hidden->driver = device->hidden->driver_ops->create_driver(device_fd);
    if (device->hidden->driver == NULL) {
        close(device_fd);
        return -1;
    }

    device->hidden->dev_fd = device_fd;
    return 0;
}

static GAL_VideoDevice *DRM_CreateDevice(int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if ( device ) {
        memset(device, 0, (sizeof (*device)));
        device->hidden = (struct GAL_PrivateVideoData *)
                malloc((sizeof (*device->hidden)));
    }
    if ((device == NULL) || (device->hidden == NULL)) {
        GAL_OutOfMemory();
        if (device) {
            free(device);
        }
        return NULL;
    }

    memset(device->hidden, 0, (sizeof (*device->hidden)));
    if (GetMgEtcValue ("drm", "device",
            device->hidden->dev_name, LEN_DEVICE_NAME) < 0) {
        strcpy(device->hidden->dev_name, "/dev/dri/card0");
        _WRN_PRINTF("No drm.device defined, use the default '/dev/dri/card0'");
    }

    device->hidden->dev_fd = -1;
    open_drm_device(device);
    if (device->hidden->dev_fd < 0) {
        return NULL;
    }

    /* Set the function pointers */
    device->VideoInit = DRM_VideoInit;
    device->ListModes = DRM_ListModes;
    device->SetVideoMode = DRM_SetVideoMode;
    device->SetColors = DRM_SetColors;
    device->VideoQuit = DRM_VideoQuit;
#ifndef _MGRM_THREADS
    device->RequestHWSurface = NULL;
#endif
    device->AllocHWSurface = DRM_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = DRM_FreeHWSurface;

    device->free = DRM_DeleteDevice;

    return device;
}

VideoBootStrap DRM_bootstrap = {
    DRM_DRIVER_NAME, "Linux DRM video driver",
    DRM_Available, DRM_CreateDevice
};


static int DRM_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    _DBG_PRINTF("NEWGAL>DRM: Calling init method!\n");

    /* Determine the screen depth (use default 8-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;

    /* We're done! */
    return(0);
}

static GAL_Rect **DRM_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    if (format->BitsPerPixel < 8) {
        return NULL;
    }

    return (GAL_Rect**) -1;
}

static GAL_Surface *DRM_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    int pitch;

    if (this->hidden->buffer) {
        free (this->hidden->buffer);
    }

    pitch = width * ((bpp + 7) / 8);
    pitch = (pitch + 3) & ~3;

    this->hidden->buffer = malloc (pitch * height);
    if (!this->hidden->buffer) {
        _ERR_PRINTF ("NEWGAL>DRM: "
                "Couldn't allocate buffer for requested mode\n");
        return NULL;
    }

    memset (this->hidden->buffer, 0, pitch * height);

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, bpp, 0, 0, 0, 0)) {
        free(this->hidden->buffer);
        this->hidden->buffer = NULL;
        _ERR_PRINTF ("NEWGAL>DRM: "
                "Couldn't allocate new pixel format for requested mode\n");
        return(NULL);
    }

    /* Set up the new mode framebuffer */
    current->flags = flags & GAL_FULLSCREEN;
    this->hidden->w = current->w = width;
    this->hidden->h = current->h = height;
    current->pitch = pitch;
    current->pixels = this->hidden->buffer;

    /* We're done */
    return(current);
}

static int DRM_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return(-1);
}

static void DRM_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int DRM_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another video routine -- notably UpdateRects.
*/
static void DRM_VideoQuit(_THIS)
{
    if (this->screen->pixels != NULL) {
        free(this->screen->pixels);
        this->screen->pixels = NULL;
    }
}

#endif /* _MGGAL_DRM */

