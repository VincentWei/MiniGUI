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
static int DRM_Suspend_Dumb(_THIS);
static int DRM_Resume_Dumb(_THIS);

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
 * drm_find_crtc
 * drm_setup_connector
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

    uint32_t        width;
    uint32_t        height;
    uint32_t        conn;
    uint32_t        crtc;

    drmModeModeInfo mode;
};

/*
 * drm_cleanup(vdata): This cleans up all the devices we created during
 * drm_prepare(). It resets the CRTCs to their saved states and deallocates
 * all memory.
 */
static void drm_cleanup(DrmVideoData* vdata)
{
    struct drm_mode_info *iter;
    struct drm_mode_destroy_dumb dreq;

    if (vdata->modes) {
        int i = 0;
        while (vdata->modes[i]) {
            free (vdata->modes[i]);
            i++;
        }

        free(vdata->modes);
    }

    while (vdata->mode_list) {
        /* remove from global list */
        iter = vdata->mode_list;
        vdata->mode_list = iter->next;

        /* free allocated memory */
        free(iter);
    }

    if (vdata->saved_crtc) {
        /* restore saved CRTC configuration */
        int ret = drmModeSetCrtc(vdata->dev_fd,
                   vdata->saved_crtc->crtc_id,
                   vdata->saved_crtc->buffer_id,
                   vdata->saved_crtc->x,
                   vdata->saved_crtc->y,
                   &vdata->saved_info->conn, 1,
                   &vdata->saved_crtc->mode);
        if (ret) {
            _ERR_PRINTF ("NEWGAL>DRM: failed to restore CRTC for connector %u (%d): %m\n",
                vdata->saved_info->conn, errno);
        }

        drmModeFreeCrtc(vdata->saved_crtc);
    }

    if (vdata->fb) {
        /* unmap buffer */
        munmap(vdata->fb, vdata->size);

        /* delete framebuffer */
        drmModeRmFB(vdata->dev_fd, vdata->buff);

        /* delete dumb buffer */
        memset(&dreq, 0, sizeof(dreq));
        dreq.handle = vdata->handle;
        drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
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
        _ERR_PRINTF("NEWGAL>DRM: drmOpen failed\n");
        free(driver_name);
        return -errno;
    }

#ifdef __TARGET_EXTERNAL__
    device->hidden->driver_ops = __drm_ex_driver_get(driver_name);
#endif
    free (driver_name);

    if (device->hidden->driver_ops == NULL) {
        uint64_t has_dumb;

        /* check whether supports dumb buffer */
        if (drmGetCap(device_fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 ||
                !has_dumb) {
            _ERR_PRINTF("NEWGAL>DRM: the DRM device '%s' does not support dumb buffers\n",
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
        _WRN_PRINTF("NEWGAL>DRM: failed to create DRM driver");
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
        _WRN_PRINTF("NEWGAL>DRM: No drm.device defined, use the default '/dev/dri/card0'");
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
        device->Suspend = DRM_Suspend_Dumb;
        device->Resume = DRM_Resume_Dumb;
    }

    device->free = DRM_DeleteDevice;
    return device;
}

VideoBootStrap DRM_bootstrap = {
    DRM_DRIVER_NAME, "Linux DRM video driver",
    DRM_Available, DRM_CreateDevice
};

/*
 * drm_find_crtc(vdata, res, conn, info):
 * This small helper tries to find a suitable CRTC for the given connector.
 */
static int drm_find_crtc(DrmVideoData* vdata,
            drmModeRes *res, drmModeConnector *conn, struct drm_mode_info *info)
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
                info->crtc = crtc;
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
                info->crtc = crtc;
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
 * drm_setup_connector:
 * Set up a single connector.
 */
static int drm_setup_connector(DrmVideoData* vdata,
            drmModeRes *res, drmModeConnector *conn, struct drm_mode_info *info)
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
    memcpy(&info->mode, &conn->modes[0], sizeof(info->mode));
    info->width = conn->modes[0].hdisplay;
    info->height = conn->modes[0].vdisplay;
    _DBG_PRINTF("NEWGAL>DRM: mode for connector %u is %ux%u\n",
        conn->connector_id, info->width, info->height);

    /* find a crtc for this connector */
    ret = drm_find_crtc(vdata, res, conn, info);
    if (ret) {
        _DBG_PRINTF("NEWGAL>DRM: no valid crtc for connector %u\n",
            conn->connector_id);
        return ret;
    }

    return 0;
}

/*
 * drm_prepare:
 *  Collect the connnectors and mode information.
 */
static int drm_prepare(DrmVideoData* vdata)
{
    drmModeRes *res;
    drmModeConnector *conn;
    unsigned int i;
    struct drm_mode_info *info;
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
        info = malloc(sizeof(*info));
        memset(info, 0, sizeof(*info));
        info->conn = conn->connector_id;

        /* call helper function to prepare this connector */
        ret = drm_setup_connector(vdata, res, conn, info);
        if (ret) {
            if (ret != -ENOENT) {
                errno = -ret;
                _ERR_PRINTF("NEWGAL>DRM: cannot setup device for connector"
                    " %u:%u (%d): %m\n",
                    i, res->connectors[i], errno);
            }
            free(info);
            drmModeFreeConnector(conn);
            continue;
        }

        /* free connector vdata and link device into global list */
        drmModeFreeConnector(conn);
        info->next = vdata->mode_list;
        vdata->mode_list = info;
    }

    /* free resources again */
    drmModeFreeResources(res);
    return 0;
}

/* DRM engine methods for both dumb buffer and acclerated buffers */
static int DRM_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    int n = 0;
    struct drm_mode_info *iter;

    drm_prepare(this->hidden);

    for (iter = this->hidden->mode_list; iter; iter = iter->next) {
        _DBG_PRINTF("NEWGAL>DRM: mode #%d: %ux%u, conn: %u, crtc: %u\n", n,
                iter->width, iter->height, iter->conn, iter->crtc);
        n++;
    }

    if (n == 0) {
        return -1;
    }

    this->hidden->modes = calloc(n + 1, sizeof(GAL_Rect*));
    if (this->hidden->modes == NULL) {
        _ERR_PRINTF("NEWGAL>DRM: failed to allocate memory for modes (%d)\n",
            n);
        return -1;
    }

    n = 0;
    for (iter = this->hidden->mode_list; iter; iter = iter->next) {
        this->hidden->modes[n] = malloc(sizeof(GAL_Rect));
        this->hidden->modes[n]->x = 0;
        this->hidden->modes[n]->y = 0;
        this->hidden->modes[n]->w = iter->width;
        this->hidden->modes[n]->h = iter->height;
        n++;
    }

    vformat->BitsPerPixel = 32;
    vformat->BytesPerPixel = 4;

    /* We're done! */
    return(0);
}

static GAL_Rect **DRM_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    if (format->BitsPerPixel != 32) {
        return NULL;
    }

    return this->hidden->modes;
}

static int DRM_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return(1);
}

static void DRM_VideoQuit(_THIS)
{
    if (this->screen->pixels != NULL) {
        this->screen->pixels = NULL;
    }
}

/*
 * drm_create_dumb_fb(vdata, info):
 * Call this function to create a so called "dumb buffer".
 * We can use it for unaccelerated software rendering on the CPU.
 */
static int drm_create_dumb_fb(DrmVideoData* vdata, const DrmModeInfo* info,
        int depth, int bpp)
{
    struct drm_mode_create_dumb creq;
    struct drm_mode_destroy_dumb dreq;
    struct drm_mode_map_dumb mreq;
    int ret;

    /* create dumb buffer */
    memset(&creq, 0, sizeof(creq));
    creq.width = info->width;
    creq.height = info->height;
    creq.bpp = bpp;
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (ret < 0) {
        _ERR_PRINTF("NEWGAL>DRM: cannot create dumb buffer (%d): %m\n",
            errno);
        return -errno;
    }

    vdata->width = creq.width;
    vdata->height = creq.height;
    vdata->bpp = creq.bpp;
    vdata->pitch = creq.pitch;
    vdata->size = creq.size;
    vdata->handle = creq.handle;

    /* create framebuffer object for the dumb-buffer */
    ret = drmModeAddFB(vdata->dev_fd, vdata->width, vdata->height, depth, bpp,
                 vdata->pitch, vdata->handle, &vdata->buff);
    if (ret) {
        _ERR_PRINTF("NEWGAL>DRM: cannot create framebuffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_destroy;
    }

    /* prepare buffer for memory mapping */
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = vdata->handle;
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        _ERR_PRINTF("NEWGAL>DRM: cannot map dumb buffer (%d): %m\n", errno);
        ret = -errno;
        goto err_fb;
    }

    /* perform actual memory mapping */
    vdata->fb = mmap(0, vdata->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                vdata->dev_fd, mreq.offset);
    if (vdata->fb == MAP_FAILED) {
        _ERR_PRINTF("NEWGAL>DRM: cannot mmap dumb buffer (%d): %m\n", errno);
        ret = -errno;
        goto err_fb;
    }

    return 0;

err_fb:
    drmModeRmFB(vdata->dev_fd, vdata->buff);

err_destroy:
    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = vdata->handle;
    drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    return ret;
}

static DrmModeInfo* find_mode(DrmVideoData* vdata, int width, int height)
{
    DrmModeInfo *iter;

    for (iter = vdata->mode_list; iter; iter = iter->next) {
        if (iter->width >= width && iter->height >= height)
            return iter;
    }

    return NULL;
}

/* DRM engine methods for dumb buffers */
static GAL_Surface *DRM_SetVideoMode_Dumb(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    DrmVideoData* vdata = this->hidden;
    DrmModeInfo* info;
    int depth;
    int ret;

    if (bpp != 32) {
        _DBG_PRINTF("NEWGAL>DRM: force bpp (%d) to be 32\n", bpp);
        bpp = 32;
    }
    depth = 24;

    /* find the connector+CRTC suitable for the resolution requested */
    info = find_mode(vdata, width, height);
    if (info == NULL) {
        _ERR_PRINTF("NEWGAL>DRM: cannot find a CRTC for video mode: %dx%d-%dbpp\n",
            width, height, bpp);
        return NULL;
    }

    _DBG_PRINTF("NEWGAL>DRM: going setting video mode: %dx%d-%dbpp\n",
            info->width, info->height, bpp);

    /* create a dumb framebuffer for current CRTC */
    ret = drm_create_dumb_fb(this->hidden, info, depth, bpp);
    if (ret) {
        _ERR_PRINTF("NEWGAL>DRM: cannot create dumb framebuffer\n");
        return NULL;
    }

    /* perform actual modesetting on the found connector+CRTC */
    this->hidden->saved_crtc = drmModeGetCrtc(vdata->dev_fd, info->crtc);
    ret = drmModeSetCrtc(vdata->dev_fd, info->crtc, vdata->buff, 0, 0,
                     &info->conn, 1, &info->mode);
    if (ret) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot set CRTC for connector %u (%d): %m\n",
            info->conn, errno);

        drmModeFreeCrtc(this->hidden->saved_crtc);
        this->hidden->saved_crtc = NULL;
        return NULL;
    }

    this->hidden->saved_info = info;

    /* Set up the new mode framebuffer */
    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, bpp, 0, 0, 0, 0)) {
        _ERR_PRINTF ("NEWGAL>DRM: allocate new pixel format for requested mode\n");
        return NULL;
    }

    _DBG_PRINTF("NEWGAL>DRM: real screen mode: %dx%d-%dbpp\n", width, height, bpp);

    current->flags = flags & GAL_FULLSCREEN;
    current->w = width;
    current->h = height;
    current->pitch = this->hidden->pitch;
    current->pixels = this->hidden->fb;

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

static int DRM_Resume_Dumb(_THIS)
{
    DrmVideoData* vdata = this->hidden;
    int ret = -1;

    _DBG_PRINTF ("NEWGAL>DRM: %s called\n", __FUNCTION__);

    if (vdata->saved_info) {
        vdata->saved_crtc = drmModeGetCrtc(vdata->dev_fd,
                vdata->saved_info->crtc);
        ret = drmModeSetCrtc(vdata->dev_fd,
                vdata->saved_info->crtc,
                vdata->buff, 0, 0,
                &vdata->saved_info->conn, 1,
                &vdata->saved_info->mode);
    }

    if (ret) {
        _ERR_PRINTF ("NEWGAL>DRM: Failed to resume dumb frame buffer: %d.\n",
            ret);
    }

    return ret;
}

static int DRM_Suspend_Dumb(_THIS)
{
    DrmVideoData* vdata = this->hidden;
    int ret = -1;

    _DBG_PRINTF ("NEWGAL>DRM: %s called\n", __FUNCTION__);

    if (vdata->saved_crtc) {
        /* restore saved CRTC configuration */
        ret = drmModeSetCrtc(vdata->dev_fd,
                   vdata->saved_crtc->crtc_id,
                   vdata->saved_crtc->buffer_id,
                   vdata->saved_crtc->x,
                   vdata->saved_crtc->y,
                   &vdata->saved_info->conn, 1,
                   &vdata->saved_crtc->mode);

        drmModeFreeCrtc(vdata->saved_crtc);
        vdata->saved_crtc = NULL;
    }

    if (ret) {
        _ERR_PRINTF ("NEWGAL>DRM: Failed to suspend dumb frame buffer: %m.\n");
    }

    return ret;
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

