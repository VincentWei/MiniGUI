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

#include <sys/mman.h>
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

/* DRM engine methods for both dumb buffer and acclerated buffers */
static int DRM_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **DRM_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static int DRM_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void DRM_VideoQuit(_THIS);

/* DRM engine operators for dumb buffer */
static GAL_Surface *DRM_SetVideoMode_Dumb(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags);
static int DRM_AllocHWSurface_Dumb(_THIS, GAL_Surface *surface);
static void DRM_FreeHWSurface_Dumb(_THIS, GAL_Surface *surface);

/* DRM engine operators accelerated */
static GAL_Surface *DRM_SetVideoMode_Accl(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags);
static int DRM_AllocHWSurface_Accl(_THIS, GAL_Surface *surface);
static void DRM_FreeHWSurface_Accl(_THIS, GAL_Surface *surface);
static int DRM_CheckHWBlit_Accl(_THIS, GAL_Surface *src, GAL_Surface *dst);
static int DRM_FillHWRect_Accl(_THIS, GAL_Surface *dst, GAL_Rect *rect,
        Uint32 color);
static int DRM_SetHWColorKey_Accl(_THIS, GAL_Surface *surface, Uint32 key);
static int DRM_SetHWAlpha_Accl(_THIS, GAL_Surface *surface, Uint8 value);

/* DRM driver bootstrap functions */
static int DRM_Available(void)
{
    return drmAvailable();
}

/*
 * The following helpers derived from DRM HOWTO by David Herrmann.
 *
 * drm_prepare
 * drm_setup_dev
 * drm_create_dumb_fb
 * drm_cleanup
 *
 * Copyright 2012-2017 David Herrmann <dh.herrmann@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

struct drm_mode_info {
    struct drm_mode_info *next;

    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t size;
    uint32_t handle;
    uint8_t *map;

    drmModeModeInfo mode;
    uint32_t fb;
    uint32_t conn;
    uint32_t crtc;
    drmModeCrtc *saved_crtc;
};

/*
 * modeset_cleanup(vdata): This cleans up all the devices we created during
 * modeset_prepare(). It resets the CRTCs to their saved states and deallocates
 * all memory.
 */
static void drm_cleanup(DrmVideoData* vdata)
{
    struct drm_mode_info *iter;
    struct drm_mode_destroy_dumb dreq;

    while (vdata->mode_list) {
        /* remove from global list */
        iter = vdata->mode_list;
        vdata->mode_list = iter->next;

        /* restore saved CRTC configuration */
        drmModeSetCrtc(vdata->dev_fd,
                   iter->saved_crtc->crtc_id,
                   iter->saved_crtc->buffer_id,
                   iter->saved_crtc->x,
                   iter->saved_crtc->y,
                   &iter->conn,
                   1,
                   &iter->saved_crtc->mode);
        drmModeFreeCrtc(iter->saved_crtc);

        /* unmap buffer */
        munmap(iter->map, iter->size);

        /* delete framebuffer */
        drmModeRmFB(vdata->dev_fd, iter->fb);

        /* delete dumb buffer */
        memset(&dreq, 0, sizeof(dreq));
        dreq.handle = iter->handle;
        drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

        /* free allocated memory */
        free(iter);
    }
}

static void DRM_DeleteDevice(GAL_VideoDevice *device)
{
    if (device->hidden->driver && device->hidden->driver_ops) {
        device->hidden->driver_ops->destroy_driver(device->hidden->driver);
    }

    drm_cleanup(device->hidden);

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

    _DBG_PRINTF("NEWGAL>DRM: Try to load DRM driver: %s\n", driver_name);

    device_fd = drmOpen(driver_name, NULL);
    if (device_fd < 0) {
        _ERR_PRINTF("drmOpen failed");
        free(driver_name);
        return -errno;
    }

    if (strcmp(driver_name, "i915") == 0) {
        _DBG_PRINTF("NEWGAL>DRM: found driver: i915\n");
        // XXX: built-in driver operators for i915.
    }
    else {
#ifdef __TARGET_EXTERNAL__
        device->hidden->driver_ops = __drm_ex_driver_get(driver_name);
#endif
    }

    free (driver_name);
    if (device->hidden->driver_ops == NULL) {
        uint64_t has_dumb;

        /* check whether supports dumb buffer */
        if (drmGetCap(device_fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 ||
                !has_dumb) {
            _WRN_PRINTF("the DRM device '%s' does not support dumb buffers\n",
                    device->hidden->dev_name);
            close(device_fd);
            return -EOPNOTSUPP;
        }

        device->hidden->dev_fd = device_fd;
        return 0;
    }

    device->hidden->dev_fd = device_fd;
    device->hidden->driver = device->hidden->driver_ops->create_driver(device_fd);
    if (device->hidden->driver == NULL) {
        _WRN_PRINTF("failed to create DRM driver");
        device->hidden->driver_ops = NULL;
    }

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

    device->VideoInit = DRM_VideoInit;
    device->ListModes = DRM_ListModes;
    device->SetColors = DRM_SetColors;
    device->VideoQuit = DRM_VideoQuit;
    if (device->hidden->driver) {
        /* Use accelerated driver */
        device->SetVideoMode = DRM_SetVideoMode_Accl;
#ifndef _MGRM_THREADS
        device->RequestHWSurface = NULL;
#endif
        device->AllocHWSurface = DRM_AllocHWSurface_Accl;
        device->CheckHWBlit = DRM_CheckHWBlit_Accl;
        device->FillHWRect = DRM_FillHWRect_Accl;
        device->SetHWColorKey = DRM_SetHWColorKey_Accl;
        device->SetHWAlpha = DRM_SetHWAlpha_Accl;
        device->FreeHWSurface = DRM_FreeHWSurface_Accl;
    }
    else {
        /* Use DUMB buffer */
        device->SetVideoMode = DRM_SetVideoMode_Dumb;
#ifndef _MGRM_THREADS
        device->RequestHWSurface = NULL;
#endif
        device->AllocHWSurface = DRM_AllocHWSurface_Dumb;
        device->CheckHWBlit = NULL;
        device->FillHWRect = NULL;
        device->SetHWColorKey = NULL;
        device->SetHWAlpha = NULL;
        device->FreeHWSurface = DRM_FreeHWSurface_Dumb;
    }

    device->free = DRM_DeleteDevice;
    return device;
}

VideoBootStrap DRM_bootstrap = {
    DRM_DRIVER_NAME, "Linux DRM video driver",
    DRM_Available, DRM_CreateDevice
};

/*
 * modeset_find_crtc(vdata, res, conn, dev):
 * This small helper tries to find a suitable CRTC for the given connector.
 */
static int drm_find_crtc(DrmVideoData* vdata,
            drmModeRes *res, drmModeConnector *conn, struct drm_mode_info *dev)
{
    drmModeEncoder *enc;
    unsigned int i, j;
    int32_t crtc;
    struct drm_mode_info *iter;

    /* first try the currently conected encoder+crtc */
    if (conn->encoder_id)
        enc = drmModeGetEncoder(vdata->dev_fd, conn->encoder_id);
    else
        enc = NULL;

    if (enc) {
        if (enc->crtc_id) {
            crtc = enc->crtc_id;
            for (iter = vdata->mode_list; iter; iter = iter->next) {
                if (iter->crtc == crtc) {
                    crtc = -1;
                    break;
                }
            }

            if (crtc >= 0) {
                drmModeFreeEncoder(enc);
                dev->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }

    /* If the connector is not currently bound to an encoder or if the
     * encoder+crtc is already used by another connector (actually unlikely
     * but lets be safe), iterate all other available encoders to find a
     * matching CRTC. */
    for (i = 0; i < conn->count_encoders; ++i) {
        enc = drmModeGetEncoder(vdata->dev_fd, conn->encoders[i]);
        if (!enc) {
            _ERR_PRINTF("NEWGAL>DRM: cannot retrieve encoder %u:%u (%d): %m\n",
                i, conn->encoders[i], errno);
            continue;
        }

        /* iterate all global CRTCs */
        for (j = 0; j < res->count_crtcs; ++j) {
            /* check whether this CRTC works with the encoder */
            if (!(enc->possible_crtcs & (1 << j)))
                continue;

            /* check that no other device already uses this CRTC */
            crtc = res->crtcs[j];
            for (iter = vdata->mode_list; iter; iter = iter->next) {
                if (iter->crtc == crtc) {
                    crtc = -1;
                    break;
                }
            }

            /* we have found a CRTC, so save it and return */
            if (crtc >= 0) {
                drmModeFreeEncoder(enc);
                dev->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }

    _ERR_PRINTF("NEWGAL>DRM: cannot find suitable CRTC for connector %u\n",
        conn->connector_id);
    return -ENOENT;
}

/*
 * drm_create_dumb_fb(vdata, dev):
 * Call this function to create a so called "dumb buffer".
 * We can use it for unaccelerated software rendering on the CPU.
 */
static int drm_create_dumb_fb(DrmVideoData* vdata, struct drm_mode_info *dev)
{
    struct drm_mode_create_dumb creq;
    struct drm_mode_destroy_dumb dreq;
    struct drm_mode_map_dumb mreq;
    int ret;

    /* create dumb buffer */
    memset(&creq, 0, sizeof(creq));
    creq.width = dev->width;
    creq.height = dev->height;
    creq.bpp = 32;
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (ret < 0) {
        _ERR_PRINTF("NEWGAL>DRM: cannot create dumb buffer (%d): %m\n",
            errno);
        return -errno;
    }
    dev->stride = creq.pitch;
    dev->size = creq.size;
    dev->handle = creq.handle;

    /* create framebuffer object for the dumb-buffer */
    ret = drmModeAddFB(vdata->dev_fd, dev->width, dev->height, 24, 32, dev->stride,
               dev->handle, &dev->fb);
    if (ret) {
        _ERR_PRINTF("NEWGAL>DRM: cannot create framebuffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_destroy;
    }

    /* prepare buffer for memory mapping */
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = dev->handle;
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        _ERR_PRINTF("NEWGAL>DRM: cannot map dumb buffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_fb;
    }

    /* perform actual memory mapping */
    dev->map = mmap(0, dev->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                vdata->dev_fd, mreq.offset);
    if (dev->map == MAP_FAILED) {
        _ERR_PRINTF("NEWGAL>DRM: cannot mmap dumb buffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_fb;
    }

    /* clear the framebuffer to 0 */
    memset(dev->map, 0, dev->size);
    return 0;

err_fb:
    drmModeRmFB(vdata->dev_fd, dev->fb);

err_destroy:
    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = dev->handle;
    drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    return ret;
}

/*
 * drm_setup_dev:
 * Set up a single connector.
 */
static int drm_setup_dev(DrmVideoData* vdata,
            drmModeRes *res, drmModeConnector *conn, struct drm_mode_info *dev)
{
    int ret;

    /* check if a monitor is connected */
    if (conn->connection != DRM_MODE_CONNECTED) {
        _DBG_PRINTF("NEWGAL>DRM: ignoring unused connector %u\n",
            conn->connector_id);
        return -ENOENT;
    }

    /* check if there is at least one valid mode */
    if (conn->count_modes == 0) {
        _DBG_PRINTF("NEWGAL>DRM: no valid mode for connector %u\n",
            conn->connector_id);
        return -EFAULT;
    }

    /* copy the mode information into our device structure */
    memcpy(&dev->mode, &conn->modes[0], sizeof(dev->mode));
    dev->width = conn->modes[0].hdisplay;
    dev->height = conn->modes[0].vdisplay;
    _DBG_PRINTF("NEWGAL>DRM: mode for connector %u is %ux%u\n",
        conn->connector_id, dev->width, dev->height);

    /* find a crtc for this connector */
    ret = drm_find_crtc(vdata, res, conn, dev);
    if (ret) {
        _DBG_PRINTF("NEWGAL>DRM: no valid crtc for connector %u\n",
            conn->connector_id);
        return ret;
    }

    /* create a framebuffer for this CRTC */
    ret = drm_create_dumb_fb(vdata, dev);
    if (ret) {
        _DBG_PRINTF("NEWGAL>DRM: cannot create framebuffer for connector %u\n",
            conn->connector_id);
        return ret;
    }

    return 0;
}

static int drm_prepare(DrmVideoData* vdata)
{
    drmModeRes *res;
    drmModeConnector *conn;
    unsigned int i;
    struct drm_mode_info *dev;
    int ret;

    /* retrieve resources */
    res = drmModeGetResources(vdata->dev_fd);
    if (!res) {
        _ERR_PRINTF("NEWGAL>DRM: cannot retrieve DRM resources (%d): %m\n",
            errno);
        return -errno;
    }

    /* iterate all connectors */
    for (i = 0; i < res->count_connectors; ++i) {
        /* get information for each connector */
        conn = drmModeGetConnector(vdata->dev_fd, res->connectors[i]);
        if (!conn) {
            _ERR_PRINTF("NEWGAL>DRM: cannot retrieve DRM connector %u:%u(%d): %m\n",
                i, res->connectors[i], errno);
            continue;
        }

        /* create a device structure */
        dev = malloc(sizeof(*dev));
        memset(dev, 0, sizeof(*dev));
        dev->conn = conn->connector_id;

        /* call helper function to prepare this connector */
        ret = drm_setup_dev(vdata, res, conn, dev);
        if (ret) {
            if (ret != -ENOENT) {
                errno = -ret;
                _ERR_PRINTF("NEWGAL>DRM: cannot setup device for connector"
                    " %u:%u (%d): %m\n",
                    i, res->connectors[i], errno);
            }
            free(dev);
            drmModeFreeConnector(conn);
            continue;
        }

        /* free connector vdata and link device into global list */
        drmModeFreeConnector(conn);
        dev->next = vdata->mode_list;
        vdata->mode_list = dev;
    }

    /* free resources again */
    drmModeFreeResources(res);
    return 0;
}

/* DRM engine methods for both dumb buffer and acclerated buffers */
static int DRM_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    _DBG_PRINTF("NEWGAL>DRM: Calling %s\n", __FUNCTION__);

    drm_prepare(this->hidden);

    struct drm_mode_info *iter;
    int i = 0;
    for (iter = this->hidden->mode_list; iter; iter = iter->next) {
        _MG_PRINTF("mode #%d: %ux%u(%u), fb: %u, conn: %u, crtc: %u\n", i,
                iter->width, iter->height, iter->stride,
                iter->fb, iter->conn, iter->crtc);
        i++;
    }

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

static int DRM_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return(1);
}

static void DRM_VideoQuit(_THIS)
{
    if (this->screen->pixels != NULL) {
        free(this->screen->pixels);
        this->screen->pixels = NULL;
    }
}

/* DRM engine methods for dumb buffers */
static GAL_Surface *DRM_SetVideoMode_Dumb(_THIS, GAL_Surface *current,
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

static int DRM_AllocHWSurface_Dumb(_THIS, GAL_Surface *surface)
{
    return(-1);
}

static void DRM_FreeHWSurface_Dumb(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

/* DRM engine methods for accelerated buffers */
static GAL_Surface *DRM_SetVideoMode_Accl(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags)
{
    return NULL;
}

static int DRM_AllocHWSurface_Accl(_THIS, GAL_Surface *surface)
{
    return 0;
}

static void DRM_FreeHWSurface_Accl(_THIS, GAL_Surface *surface)
{
}

static int DRM_CheckHWBlit_Accl(_THIS, GAL_Surface *src, GAL_Surface *dst)
{
    return 0;
}

static int DRM_FillHWRect_Accl(_THIS, GAL_Surface *dst, GAL_Rect *rect,
        Uint32 color)
{
    return 0;
}

static int DRM_SetHWColorKey_Accl(_THIS, GAL_Surface *surface, Uint32 key)
{
    return 0;
}

static int DRM_SetHWAlpha_Accl(_THIS, GAL_Surface *surface, Uint8 value)
{
    return 0;
}

#endif /* _MGGAL_DRM */

