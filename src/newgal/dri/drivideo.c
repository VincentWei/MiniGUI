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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _DEBUG
#include "common.h"

#ifdef _MGGAL_DRI

#include <sys/mman.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <drm/drm.h>
#include <drm/drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "minigui.h"
#include "newgal.h"
#include "exstubs.h"

#include "pixels_c.h"
#include "drivideo.h"

#define DRI_DRIVER_NAME "dri"

/* DRM engine methods for both dumb buffer and acclerated buffers */
static int DRI_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **DRI_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static int DRI_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void DRI_VideoQuit(_THIS);
static int DRI_Suspend(_THIS);
static int DRI_Resume(_THIS);

/* DRM engine operators for dumb buffer */
static GAL_Surface *DRI_SetVideoMode_Dumb(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags);
static int DRI_AllocHWSurface_Dumb(_THIS, GAL_Surface *surface);
static void DRI_FreeHWSurface_Dumb(_THIS, GAL_Surface *surface);

/* DRM engine operators accelerated */
static GAL_Surface *DRI_SetVideoMode_Accl(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags);
static int DRI_AllocHWSurface_Accl(_THIS, GAL_Surface *surface);
static void DRI_FreeHWSurface_Accl(_THIS, GAL_Surface *surface);
static int DRI_CheckHWBlit_Accl(_THIS, GAL_Surface *src, GAL_Surface *dst);
static int DRI_FillHWRect_Accl(_THIS, GAL_Surface *dst, GAL_Rect *rect,
        Uint32 color);
static void DRI_UpdateRects_Accl (_THIS, int numrects, GAL_Rect *rects);
static int DRI_SetHWColorKey_Accl(_THIS, GAL_Surface *surface, Uint32 key);
static int DRI_SetHWAlpha_Accl(_THIS, GAL_Surface *surface, Uint8 value);

/* DRM driver bootstrap functions */
static int DRI_Available(void)
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
static void drm_cleanup(DriVideoData* vdata)
{
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

    if (vdata->scanout_fb) {
        if (vdata->driver_ops) {
            // do nothing for hardware surface.
        }
        else {
            /* dumb buffer */
            struct drm_mode_destroy_dumb dreq;

            /* unmap buffer */
            munmap(vdata->scanout_fb, vdata->size);

            /* delete framebuffer */
            drmModeRmFB(vdata->dev_fd, vdata->scanout_buff_id);

            /* delete dumb buffer */
            memset(&dreq, 0, sizeof(dreq));
            dreq.handle = vdata->handle;
            drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
        }
    }

    if (vdata->modes) {
        int i = 0;
        while (vdata->modes[i]) {
            free (vdata->modes[i]);
            i++;
        }

        free(vdata->modes);
    }

    while (vdata->mode_list) {
        struct drm_mode_info *iter;

        /* remove from global list */
        iter = vdata->mode_list;
        vdata->mode_list = iter->next;

        /* free allocated memory */
        free(iter);
    }
}

static void DRI_DeleteDevice(GAL_VideoDevice *device)
{
    drm_cleanup(device->hidden);

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

    _DBG_PRINTF("NEWGAL>DRM: Try to load DRM driver: %s\n", driver_name);

    device_fd = drmOpen(driver_name, NULL);
    if (device_fd < 0) {
        _ERR_PRINTF("NEWGAL>DRM: drmOpen failed\n");
        free(driver_name);
        return -errno;
    }

#ifdef __TARGET_EXTERNAL__
    device->hidden->driver_ops = __dri_ex_driver_get(driver_name);
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
        device->hidden->driver = NULL;
        return 0;
    }

    device->hidden->dev_fd = device_fd;
    device->hidden->driver = device->hidden->driver_ops->create_driver(device_fd);
    if (device->hidden->driver == NULL) {
        _WRN_PRINTF("NEWGAL>DRM: failed to create DRM driver\n");
        device->hidden->driver_ops = NULL;
    }

    return 0;
}

static GAL_VideoDevice *DRI_CreateDevice(int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if ( device ) {
        memset(device, 0, (sizeof (*device)));
        device->hidden = (struct GAL_PrivateVideoData *)
                calloc(1, (sizeof (*device->hidden)));
    }
    if ((device == NULL) || (device->hidden == NULL)) {
        GAL_OutOfMemory();
        if (device) {
            free(device);
        }
        return NULL;
    }

    memset(device->hidden, 0, (sizeof (*device->hidden)));
    if (GetMgEtcValue ("dri", "device",
            device->hidden->dev_name, LEN_DEVICE_NAME) < 0) {
        strcpy(device->hidden->dev_name, "/dev/dri/card0");
        _WRN_PRINTF("NEWGAL>DRM: No dri.device defined, use the default '/dev/dri/card0'\n");
    }

    device->hidden->dev_fd = -1;
    open_drm_device(device);
    if (device->hidden->dev_fd < 0) {
        return NULL;
    }

    device->VideoInit = DRI_VideoInit;
    device->ListModes = DRI_ListModes;
    device->SetColors = DRI_SetColors;
    device->VideoQuit = DRI_VideoQuit;
    if (device->hidden->driver) {
        /* Use accelerated driver */
        device->SetVideoMode = DRI_SetVideoMode_Accl;
#ifndef _MGRM_THREADS
        device->RequestHWSurface = NULL;
#endif
        device->AllocHWSurface = DRI_AllocHWSurface_Accl;
        device->FreeHWSurface = DRI_FreeHWSurface_Accl;
        device->UpdateRects = DRI_UpdateRects_Accl;
    }
    else {
        /* Use DUMB buffer */
        device->SetVideoMode = DRI_SetVideoMode_Dumb;
#ifndef _MGRM_THREADS
        device->RequestHWSurface = NULL;
#endif
        device->AllocHWSurface = DRI_AllocHWSurface_Dumb;
        device->FreeHWSurface = DRI_FreeHWSurface_Dumb;
        device->UpdateRects = NULL;
    }

    /* set accelerated methods in DRI_VideoInit */
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->Suspend = DRI_Suspend;
    device->Resume = DRI_Resume;
    device->free = DRI_DeleteDevice;

    return device;
}

VideoBootStrap DRI_bootstrap = {
    DRI_DRIVER_NAME, "Linux DRI video driver",
    DRI_Available, DRI_CreateDevice
};

/*
 * drm_find_crtc(vdata, res, conn, info):
 * This small helper tries to find a suitable CRTC for the given connector.
 */
static int drm_find_crtc(DriVideoData* vdata,
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
static int drm_setup_connector(DriVideoData* vdata,
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
static int drm_prepare(DriVideoData* vdata)
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
static int DRI_VideoInit(_THIS, GAL_PixelFormat *vformat)
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

    if (this->hidden->driver) {
        if (this->hidden->driver_ops->clear_buffer) {
            this->info.blit_fill = 1;
            this->FillHWRect = DRI_FillHWRect_Accl;
        }
        else
            this->FillHWRect = NULL;

        if (this->hidden->driver_ops->check_blit) {
            this->CheckHWBlit = DRI_CheckHWBlit_Accl;
            this->SetHWColorKey = DRI_SetHWColorKey_Accl;
            this->SetHWAlpha = DRI_SetHWAlpha_Accl;

            this->info.blit_hw = 1;
            if (this->hidden->driver_ops->alpha_blit)
                this->info.blit_hw_A = 1;
            if (this->hidden->driver_ops->key_blit)
                this->info.blit_hw_CC = 1;
        }
        else {
            this->CheckHWBlit = NULL;
            this->SetHWColorKey = NULL;
            this->SetHWAlpha = NULL;
        }
    }

    /* We're done! */
    return(0);
}

static GAL_Rect **DRI_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    if (format->BitsPerPixel != 32) {
        return NULL;
    }

    return this->hidden->modes;
}

static int DRI_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return(1);
}

static void DRI_VideoQuit(_THIS)
{
    if (this->screen->pixels != NULL) {
        this->screen->pixels = NULL;
    }
}

static int DRI_Resume(_THIS)
{
    DriVideoData* vdata = this->hidden;
    int ret = -1;

    _DBG_PRINTF ("NEWGAL>DRM: %s called\n", __FUNCTION__);

    if (vdata->saved_info) {
        vdata->saved_crtc = drmModeGetCrtc(vdata->dev_fd,
                vdata->saved_info->crtc);
        ret = drmModeSetCrtc(vdata->dev_fd,
                vdata->saved_info->crtc,
                vdata->scanout_buff_id, 0, 0,
                &vdata->saved_info->conn, 1,
                &vdata->saved_info->mode);
    }

    if (ret) {
        _ERR_PRINTF ("NEWGAL>DRM: Failed to resume dumb frame buffer: %d.\n",
            ret);
    }

    return ret;
}

static int DRI_Suspend(_THIS)
{
    DriVideoData* vdata = this->hidden;
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

/*
 * drm_create_dumb_fb(vdata, info):
 * Call this function to create a so called "dumb buffer".
 * We can use it for unaccelerated software rendering on the CPU.
 */
static int drm_create_dumb_fb(DriVideoData* vdata, const DrmModeInfo* info,
        uint32_t format, int bpp)
{
    struct drm_mode_create_dumb creq;
    struct drm_mode_destroy_dumb dreq;
    struct drm_mode_map_dumb mreq;
    uint32_t handles[4], pitches[4], offsets[4];
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
    handles[0] = vdata->handle;
    pitches[0] = vdata->pitch;
    offsets[0] = 0;

    ret = drmModeAddFB2(vdata->dev_fd, vdata->width, vdata->height, format,
            handles, pitches, offsets, &vdata->scanout_buff_id, 0);
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
    vdata->scanout_fb = mmap(0, vdata->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                vdata->dev_fd, mreq.offset);
    if (vdata->scanout_fb == MAP_FAILED) {
        _ERR_PRINTF("NEWGAL>DRM: cannot mmap dumb buffer (%d): %m\n", errno);
        ret = -errno;
        goto err_fb;
    }

    return 0;

err_fb:
    drmModeRmFB(vdata->dev_fd, vdata->scanout_buff_id);

err_destroy:
    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = vdata->handle;
    drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    return ret;
}

static DrmModeInfo* find_mode(DriVideoData* vdata, int width, int height)
{
    DrmModeInfo *iter;

    for (iter = vdata->mode_list; iter; iter = iter->next) {
        if (iter->width >= width && iter->height >= height)
            return iter;
    }

    return NULL;
}

static inline uint32_t get_def_drm_format(int bpp)
{
    switch (bpp) {
    case 32:
        return DRM_FORMAT_XRGB8888;
    case 24:
        return DRM_FORMAT_RGB888;
    case 16:
        return DRM_FORMAT_RGB565;
    case 8:
        return DRM_FORMAT_RGB332;
    default:
        break;
    }

    return DRM_FORMAT_RGB565;
}

static uint32_t get_drm_format_from_etc(int* bpp)
{
    uint32_t format;
    char fourcc[8] = {};

    if (GetMgEtcValue ("dri", "pixelformat",
            fourcc, 4) < 0) {
        return get_def_drm_format(*bpp);
    }

    format = fourcc_code(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);
    switch (format) {
    case DRM_FORMAT_RGB332:
    case DRM_FORMAT_BGR233:
        *bpp = 8;
        break;

    case DRM_FORMAT_XRGB4444:
    case DRM_FORMAT_XBGR4444:
    case DRM_FORMAT_RGBX4444:
    case DRM_FORMAT_BGRX4444:
    case DRM_FORMAT_ARGB4444:
    case DRM_FORMAT_ABGR4444:
    case DRM_FORMAT_RGBA4444:
    case DRM_FORMAT_BGRA4444:
    case DRM_FORMAT_XRGB1555:
    case DRM_FORMAT_XBGR1555:
    case DRM_FORMAT_RGBX5551:
    case DRM_FORMAT_BGRX5551:
    case DRM_FORMAT_ARGB1555:
    case DRM_FORMAT_ABGR1555:
    case DRM_FORMAT_RGBA5551:
    case DRM_FORMAT_BGRA5551:
    case DRM_FORMAT_RGB565:
    case DRM_FORMAT_BGR565:
        *bpp = 16;
        break;

    case DRM_FORMAT_RGB888:
    case DRM_FORMAT_BGR888:
        *bpp = 24;
        break;

    case DRM_FORMAT_XRGB8888:
    case DRM_FORMAT_XBGR8888:
    case DRM_FORMAT_RGBX8888:
    case DRM_FORMAT_BGRX8888:
    case DRM_FORMAT_ARGB8888:
    case DRM_FORMAT_ABGR8888:
    case DRM_FORMAT_RGBA8888:
    case DRM_FORMAT_BGRA8888:
#if 0
    case DRM_FORMAT_XRGB2101010:
    case DRM_FORMAT_XBGR2101010:
    case DRM_FORMAT_RGBX1010102:
    case DRM_FORMAT_BGRX1010102:
    case DRM_FORMAT_ARGB2101010:
    case DRM_FORMAT_ABGR2101010:
    case DRM_FORMAT_RGBA1010102:
    case DRM_FORMAT_BGRA1010102:
#endif
        *bpp = 32;
        break;
    default:
        _ERR_PRINTF("NEWGAL>DRM: not supported pixel format: %s\n",
            fourcc);
        return 0;
        break;
    }

    return format;
}

struct rgbamasks_drm_format_map {
    uint32_t drm_format;
    Uint32 Rmask, Gmask, Bmask, Amask;
};

static struct rgbamasks_drm_format_map _format_map_8bpp [] = {
    { DRM_FORMAT_RGB332,    0xE0, 0x1C, 0x03, 0x00 },
    { DRM_FORMAT_BGR233,    0x0E, 0x38, 0xC0, 0x00 },
};

static struct rgbamasks_drm_format_map _format_map_16bpp [] = {
    { DRM_FORMAT_XRGB4444,  0x0F00, 0x00F0, 0x000F, 0x0000 },
    { DRM_FORMAT_XBGR4444,  0x000F, 0x00F0, 0x0F00, 0x0000 },
    { DRM_FORMAT_RGBX4444,  0xF000, 0x0F00, 0x00F0, 0x0000 },
    { DRM_FORMAT_BGRX4444,  0x00F0, 0x0F00, 0xF000, 0x0000 },
    { DRM_FORMAT_ARGB4444,  0x0F00, 0x00F0, 0x000F, 0xF000 },
    { DRM_FORMAT_ABGR4444,  0x000F, 0x00F0, 0x0F00, 0xF000 },
    { DRM_FORMAT_RGBA4444,  0xF000, 0x0F00, 0x00F0, 0x000F },
    { DRM_FORMAT_BGRA4444,  0x00F0, 0x0F00, 0xF000, 0x000F },
    { DRM_FORMAT_XRGB1555,  0x7C00, 0x03E0, 0x001F, 0x0000 },
    { DRM_FORMAT_XBGR1555,  0x001F, 0x03E0, 0x7C00, 0x0000 },
    { DRM_FORMAT_RGBX5551,  0xF800, 0x07C0, 0x003E, 0x0000 },
    { DRM_FORMAT_BGRX5551,  0x003E, 0x07C0, 0xF800, 0x0000 },
    { DRM_FORMAT_ARGB1555,  0x7C00, 0x03E0, 0x001F, 0x8000 },
    { DRM_FORMAT_ABGR1555,  0x001F, 0x03E0, 0x7C00, 0x8000 },
    { DRM_FORMAT_RGBA5551,  0xF800, 0x07C0, 0x003E, 0x0001 },
    { DRM_FORMAT_BGRA5551,  0x003E, 0x07C0, 0xF800, 0x0001 },
    { DRM_FORMAT_RGB565,    0xF800, 0x07E0, 0x001F, 0x0000 },
    { DRM_FORMAT_BGR565,    0x001F, 0x07E0, 0xF800, 0x0000 },
};

static struct rgbamasks_drm_format_map _format_map_24bpp [] = {
    { DRM_FORMAT_RGB888,    0xFF0000, 0x00FF00, 0x0000FF, 0x000000 },
    { DRM_FORMAT_BGR888,    0x0000FF, 0x00FF00, 0xFF0000, 0x000000 },
};

static struct rgbamasks_drm_format_map _format_map_32bpp [] = {
    { DRM_FORMAT_XRGB8888,  0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { DRM_FORMAT_XBGR8888,  0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000 },
    { DRM_FORMAT_RGBX8888,  0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000 },
    { DRM_FORMAT_BGRX8888,  0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000 },
    { DRM_FORMAT_ARGB8888,  0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 },
    { DRM_FORMAT_ABGR8888,  0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 },
    { DRM_FORMAT_RGBA8888,  0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF },
    { DRM_FORMAT_BGRA8888,  0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF },
};

static uint32_t translate_gal_format(const GAL_PixelFormat *gal_format)
{
    struct rgbamasks_drm_format_map* map;
    size_t i, n;

    switch (gal_format->BitsPerPixel) {
    case 8:
        map = _format_map_8bpp;
        n = TABLESIZE(_format_map_8bpp);
        break;

    case 16:
        map = _format_map_16bpp;
        n = TABLESIZE(_format_map_16bpp);
        break;

    case 24:
        map = _format_map_24bpp;
        n = TABLESIZE(_format_map_24bpp);
        break;

    case 32:
        map = _format_map_32bpp;
        n = TABLESIZE(_format_map_32bpp);
        break;

    default:
        return 0;
    }

    for (i = 0; i < n; i++) {
        if (gal_format->Rmask == map[i].Rmask &&
                gal_format->Gmask == map[i].Gmask &&
                gal_format->Bmask == map[i].Bmask &&
                gal_format->Amask == map[i].Amask) {
            return map[i].drm_format;
        }
    }

    return 0;
}

static int translate_drm_format(uint32_t drm_format, Uint32* RGBAmasks)
{
    int bpp = 0;

    switch (drm_format) {
    case DRM_FORMAT_RGB332:
        RGBAmasks[0] = 0xE0;
        RGBAmasks[1] = 0x1C;
        RGBAmasks[2] = 0x03;
        RGBAmasks[3] = 0x00;
        bpp = 8;
        break;

    case DRM_FORMAT_BGR233:
        RGBAmasks[0] = 0x0E;
        RGBAmasks[1] = 0x38;
        RGBAmasks[2] = 0xC0;
        RGBAmasks[3] = 0x00;
        bpp = 8;
        break;

    case DRM_FORMAT_XRGB4444:
        RGBAmasks[0] = 0x0F00;
        RGBAmasks[1] = 0x00F0;
        RGBAmasks[2] = 0x000F;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        break;

    case DRM_FORMAT_XBGR4444:
        RGBAmasks[0] = 0x000F;
        RGBAmasks[1] = 0x00F0;
        RGBAmasks[2] = 0x0F00;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        break;

    case DRM_FORMAT_RGBX4444:
        RGBAmasks[0] = 0xF000;
        RGBAmasks[1] = 0x0F00;
        RGBAmasks[2] = 0x00F0;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        break;

    case DRM_FORMAT_BGRX4444:
        RGBAmasks[0] = 0x00F0;
        RGBAmasks[1] = 0x0F00;
        RGBAmasks[2] = 0xF000;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        break;

    case DRM_FORMAT_ARGB4444:
        RGBAmasks[0] = 0x0F00;
        RGBAmasks[1] = 0x00F0;
        RGBAmasks[2] = 0x000F;
        RGBAmasks[3] = 0xF000;
        bpp = 16;
        break;

    case DRM_FORMAT_ABGR4444:
        RGBAmasks[0] = 0x000F;
        RGBAmasks[1] = 0x00F0;
        RGBAmasks[2] = 0x0F00;
        RGBAmasks[3] = 0xF000;
        bpp = 16;
        break;

    case DRM_FORMAT_RGBA4444:
        RGBAmasks[0] = 0xF000;
        RGBAmasks[1] = 0x0F00;
        RGBAmasks[2] = 0x00F0;
        RGBAmasks[3] = 0x000F;
        bpp = 16;
        break;

    case DRM_FORMAT_BGRA4444:
        RGBAmasks[0] = 0x00F0;
        RGBAmasks[1] = 0x0F00;
        RGBAmasks[2] = 0xF000;
        RGBAmasks[3] = 0x000F;
        bpp = 16;
        break;

    case DRM_FORMAT_XRGB1555:
        RGBAmasks[0] = 0x7C00;
        RGBAmasks[1] = 0x03E0;
        RGBAmasks[2] = 0x001F;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        break;

    case DRM_FORMAT_XBGR1555:
        RGBAmasks[0] = 0x001F;
        RGBAmasks[1] = 0x03E0;
        RGBAmasks[2] = 0x7C00;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        break;

    case DRM_FORMAT_RGBX5551:
        RGBAmasks[0] = 0xF800;
        RGBAmasks[1] = 0x07C0;
        RGBAmasks[2] = 0x003E;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        break;

    case DRM_FORMAT_BGRX5551:
        RGBAmasks[0] = 0x003E;
        RGBAmasks[1] = 0x07C0;
        RGBAmasks[2] = 0xF800;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        break;

    case DRM_FORMAT_ARGB1555:
        RGBAmasks[0] = 0x7C00;
        RGBAmasks[1] = 0x03E0;
        RGBAmasks[2] = 0x001F;
        RGBAmasks[3] = 0x8000;
        bpp = 16;
        break;

    case DRM_FORMAT_ABGR1555:
        RGBAmasks[0] = 0x001F;
        RGBAmasks[1] = 0x03E0;
        RGBAmasks[2] = 0x7C00;
        RGBAmasks[3] = 0x8000;
        bpp = 16;
        break;

    case DRM_FORMAT_RGBA5551:
        RGBAmasks[0] = 0xF800;
        RGBAmasks[1] = 0x07C0;
        RGBAmasks[2] = 0x003E;
        RGBAmasks[3] = 0x0001;
        bpp = 16;
        break;

    case DRM_FORMAT_BGRA5551:
        RGBAmasks[0] = 0x003E;
        RGBAmasks[1] = 0x07C0;
        RGBAmasks[2] = 0xF800;
        RGBAmasks[3] = 0x0001;
        bpp = 16;
        break;

    case DRM_FORMAT_RGB565:
        RGBAmasks[0] = 0xF800;
        RGBAmasks[1] = 0x07E0;
        RGBAmasks[2] = 0x001F;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        break;

    case DRM_FORMAT_BGR565:
        RGBAmasks[0] = 0x001F;
        RGBAmasks[1] = 0x07E0;
        RGBAmasks[2] = 0xF800;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        break;

    case DRM_FORMAT_RGB888:
        RGBAmasks[0] = 0xFF0000;
        RGBAmasks[1] = 0x00FF00;
        RGBAmasks[2] = 0x0000FF;
        RGBAmasks[3] = 0x000000;
        bpp = 24;
        break;

    case DRM_FORMAT_BGR888:
        RGBAmasks[0] = 0x0000FF;
        RGBAmasks[1] = 0x00FF00;
        RGBAmasks[2] = 0xFF0000;
        RGBAmasks[3] = 0x000000;
        bpp = 24;
        break;

    case DRM_FORMAT_XRGB8888:
        RGBAmasks[0] = 0x00FF0000;
        RGBAmasks[1] = 0x0000FF00;
        RGBAmasks[2] = 0x000000FF;
        RGBAmasks[3] = 0x00000000;
        bpp = 32;
        break;

    case DRM_FORMAT_XBGR8888:
        RGBAmasks[0] = 0x000000FF;
        RGBAmasks[1] = 0x0000FF00;
        RGBAmasks[2] = 0x00FF0000;
        RGBAmasks[3] = 0x00000000;
        bpp = 32;
        break;

    case DRM_FORMAT_RGBX8888:
        RGBAmasks[0] = 0xFF000000;
        RGBAmasks[1] = 0x00FF0000;
        RGBAmasks[2] = 0x0000FF00;
        RGBAmasks[3] = 0x00000000;
        bpp = 32;
        break;

    case DRM_FORMAT_BGRX8888:
        RGBAmasks[0] = 0x0000FF00;
        RGBAmasks[1] = 0x00FF0000;
        RGBAmasks[2] = 0xFF000000;
        RGBAmasks[3] = 0x00000000;
        bpp = 32;
        break;

    case DRM_FORMAT_ARGB8888:
        RGBAmasks[0] = 0x00FF0000;
        RGBAmasks[1] = 0x0000FF00;
        RGBAmasks[2] = 0x000000FF;
        RGBAmasks[3] = 0xFF000000;
        bpp = 32;
        break;

    case DRM_FORMAT_ABGR8888:
        RGBAmasks[0] = 0x000000FF;
        RGBAmasks[1] = 0x0000FF00;
        RGBAmasks[2] = 0x00FF0000;
        RGBAmasks[3] = 0xFF000000;
        bpp = 32;
        break;

    case DRM_FORMAT_RGBA8888:
        RGBAmasks[0] = 0xFF000000;
        RGBAmasks[1] = 0x00FF0000;
        RGBAmasks[2] = 0x0000FF00;
        RGBAmasks[3] = 0x000000FF;
        bpp = 32;
        break;

    case DRM_FORMAT_BGRA8888:
        RGBAmasks[0] = 0x0000FF00;
        RGBAmasks[1] = 0x00FF0000;
        RGBAmasks[2] = 0xFF000000;
        RGBAmasks[3] = 0x000000FF;
        bpp = 32;
        break;

#if 0
    case DRM_FORMAT_XRGB2101010:
        RGBAmasks[0] = 0x3FF00000;
        RGBAmasks[1] = 0x000FFC00;
        RGBAmasks[2] = 0x000003FF;
        RGBAmasks[3] = 0x00000000;
        break;

    case DRM_FORMAT_XBGR2101010:
        RGBAmasks[0] = 0x000003FF;
        RGBAmasks[1] = 0x000FFC00;
        RGBAmasks[2] = 0x3FF00000;
        RGBAmasks[3] = 0x00000000;
        break;

    case DRM_FORMAT_RGBX1010102:
        RGBAmasks[0] = 0xFFC00000;
        RGBAmasks[1] = 0x003FF000;
        RGBAmasks[2] = 0x00000FFC;
        RGBAmasks[3] = 0x00000000;
        break;

    case DRM_FORMAT_BGRX1010102:
        RGBAmasks[0] = 0x00000FFC;
        RGBAmasks[1] = 0x003FF000;
        RGBAmasks[2] = 0xFFC00000;
        RGBAmasks[3] = 0x00000000;
        break;

    case DRM_FORMAT_ARGB2101010:
        RGBAmasks[0] = 0x3FF00000;
        RGBAmasks[1] = 0x000FFC00;
        RGBAmasks[2] = 0x000003FF;
        RGBAmasks[3] = 0xC0000000;
        break;

    case DRM_FORMAT_ABGR2101010:
        RGBAmasks[0] = 0x000003FF;
        RGBAmasks[1] = 0x000FFC00;
        RGBAmasks[2] = 0x3FF00000;
        RGBAmasks[3] = 0xC0000000;
        break;

    case DRM_FORMAT_RGBA1010102:
        RGBAmasks[0] = 0xFFC00000;
        RGBAmasks[1] = 0x003FF000;
        RGBAmasks[2] = 0x00000FFC;
        RGBAmasks[3] = 0x00000003;
        break;

    case DRM_FORMAT_BGRA1010102:
        RGBAmasks[0] = 0x00000FFC;
        RGBAmasks[1] = 0x003FF000;
        RGBAmasks[2] = 0xFFC00000;
        RGBAmasks[3] = 0x00000003;
        break;
#endif

    default:
        break;
    }

    return bpp;
}

/* DRM engine methods for dumb buffers */
static GAL_Surface *DRI_SetVideoMode_Dumb(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    uint32_t drm_format;
    DriVideoData* vdata = this->hidden;
    DrmModeInfo* info;
    Uint32 RGBAmasks[4];
    int ret;

    drm_format = get_drm_format_from_etc(&bpp);
    if (drm_format == 0) {
        return NULL;
    }

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
    ret = drm_create_dumb_fb(this->hidden, info, drm_format, bpp);
    if (ret) {
        _ERR_PRINTF("NEWGAL>DRM: cannot create dumb framebuffer\n");
        return NULL;
    }

    /* perform actual modesetting on the found connector+CRTC */
    this->hidden->saved_crtc = drmModeGetCrtc(vdata->dev_fd, info->crtc);
    ret = drmModeSetCrtc(vdata->dev_fd, info->crtc, vdata->scanout_buff_id, 0, 0,
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
    if (translate_drm_format(drm_format, RGBAmasks) == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported drm format: %u\n",
            drm_format);
        return NULL;
    }

    if (!GAL_ReallocFormat (current, bpp, RGBAmasks[0], RGBAmasks[1],
            RGBAmasks[2], RGBAmasks[3])) {
        _ERR_PRINTF ("NEWGAL>DRM: "
                "failed to allocate new pixel format for requested mode\n");
        return NULL;
    }

    _DBG_PRINTF("NEWGAL>DRM: real screen mode: %dx%d-%dbpp\n",
            width, height, bpp);

    current->flags = flags & GAL_FULLSCREEN;
    current->w = width;
    current->h = height;
    current->pitch = this->hidden->pitch;
    current->pixels = this->hidden->scanout_fb;

    /* We're done */
    return(current);
}

static int DRI_AllocHWSurface_Dumb(_THIS, GAL_Surface *surface)
{
    return(-1);
}

static void DRI_FreeHWSurface_Dumb(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

/* DRM engine methods for accelerated buffers */
static GAL_Surface *DRI_SetVideoMode_Accl(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags)
{
    DriVideoData* vdata = this->hidden;
    DrmModeInfo* info;
    uint32_t drm_format;
    Uint32 RGBAmasks[4];
    DriSurfaceBuffer* scanout_buff = NULL;

    drm_format = get_drm_format_from_etc(&bpp);
    if (drm_format == 0) {
        return NULL;
    }

    /* find the connector+CRTC suitable for the resolution requested */
    info = find_mode(vdata, width, height);
    if (info == NULL) {
        _ERR_PRINTF("NEWGAL>DRM: cannot find a CRTC for video mode: %dx%d-%dbpp\n",
            width, height, bpp);
        return NULL;
    }

    _DBG_PRINTF("NEWGAL>DRM: going setting video mode: %dx%d-%dbpp\n",
            info->width, info->height, bpp);

#if 0
    if (drmSetMaster(this->hidden->dev_fd)) {
        _ERR_PRINTF("NEWGAL>DRM: failed to call drmSetMaster: %m\n");
        return NULL;
    }
#endif

    /* create the scanout buffer and set up it as frame buffer */
    scanout_buff = vdata->driver_ops->create_buffer(vdata->driver, drm_format,
            info->width, info->height);
    if (scanout_buff == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot create scanout frame buffer: %m\n");
        goto error;
    }
    /* not a foreign surface */
    scanout_buff->foreign = 0;

    if (NULL == vdata->driver_ops->map_buffer(vdata->driver, scanout_buff)) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot map scanout frame buffer: %m\n");
        goto error;
    }

    vdata->width = info->width;
    vdata->height = info->height;
    vdata->bpp = bpp;
    vdata->pitch = scanout_buff->pitch;
    vdata->size = scanout_buff->pitch * info->height;
    vdata->handle = 0;
    vdata->scanout_buff_id = scanout_buff->id;
    vdata->scanout_fb = scanout_buff->pixels;

    _DBG_PRINTF("NEWGAL>DRM: scanout frame buffer: size (%dx%d), pitch(%d)\n",
            vdata->width, vdata->height, vdata->pitch);

    /* get console buffer id */
    vdata->saved_crtc = drmModeGetCrtc(vdata->dev_fd, info->crtc);
    vdata->console_buff_id = vdata->saved_crtc->buffer_id;

    /* perform actual modesetting on the found connector+CRTC */
    if (drmModeSetCrtc(vdata->dev_fd, info->crtc, vdata->scanout_buff_id, 0, 0,
                     &info->conn, 1, &info->mode)) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot set CRTC for connector %u (%d): %m\n",
            info->conn, errno);

        goto error;
    }

    this->hidden->saved_info = info;

    /* Allocate the new pixel format for the screen */
    if (translate_drm_format(drm_format, RGBAmasks) == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported drm format: %u\n",
            drm_format);
        return NULL;
    }

    if (!GAL_ReallocFormat (current, bpp, RGBAmasks[0], RGBAmasks[1],
            RGBAmasks[2], RGBAmasks[3])) {
        _ERR_PRINTF ("NEWGAL>DRM: "
                "failed to allocate new pixel format for requested mode\n");
        return NULL;
    }

    _DBG_PRINTF("NEWGAL>DRM: real screen mode: %dx%d-%dbpp\n",
        width, height, bpp);

    current->flags |= (GAL_FULLSCREEN | GAL_HWSURFACE);
    current->w = width;
    current->h = height;
    current->pitch = this->hidden->pitch;
    current->pixels = this->hidden->scanout_fb;
    current->hwdata = (struct private_hwdata *)scanout_buff;

    /* We're done */
    return(current);

error:

    if (vdata->saved_crtc) {
        drmModeFreeCrtc(vdata->saved_crtc);
        vdata->saved_crtc = NULL;
    }

    if (scanout_buff) {
        vdata->driver_ops->unmap_buffer(vdata->driver, scanout_buff);
        vdata->scanout_fb = NULL;
    }

    if (vdata->scanout_buff_id) {
        vdata->driver_ops->destroy_buffer(vdata->driver, scanout_buff);
        vdata->scanout_buff_id = 0;
    }

    return NULL;
}

static int DRI_AllocHWSurface_Accl(_THIS, GAL_Surface *surface)
{
    DriVideoData* vdata = this->hidden;
    uint32_t drm_format;
    DriSurfaceBuffer* surface_buffer;

    drm_format = translate_gal_format(surface->format);
    if (drm_format == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported pixel format, RGBA masks (0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
            surface->format->Rmask, surface->format->Gmask,
            surface->format->Bmask, surface->format->Amask);
        return -1;
    }

    surface_buffer = vdata->driver_ops->create_buffer(vdata->driver, drm_format,
            surface->w, surface->h);
    if (surface_buffer == NULL) {
        return -1;
    }
    /* not a foreign surface */
    surface_buffer->foreign = 0;

    if (vdata->driver_ops->map_buffer(vdata->driver, surface_buffer) == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot map hardware buffer: %m\n");
        goto error;
    }

    surface->pixels = surface_buffer->pixels;
    surface->flags |= GAL_HWSURFACE;
    surface->pitch = surface_buffer->pitch;
    surface->hwdata = (struct private_hwdata *)surface_buffer;
    return 0;

error:
    if (surface_buffer)
        vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);

    return -1;
}

static void DRI_FreeHWSurface_Accl(_THIS, GAL_Surface *surface)
{
    DriVideoData* vdata = this->hidden;
    DriSurfaceBuffer* surface_buffer;

    surface_buffer = (DriSurfaceBuffer*)surface->hwdata;
    if (surface_buffer) {
        if (surface_buffer->pixels)
            vdata->driver_ops->unmap_buffer(vdata->driver, surface_buffer);
        vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);
    }

    surface->pixels = NULL;
    surface->hwdata = NULL;
}

static int DRI_HWBlit(GAL_Surface *src, GAL_Rect *src_rc,
                       GAL_Surface *dst, GAL_Rect *dst_rc)
{
    GAL_VideoDevice *this = __mg_current_video;
    DriVideoData* vdata = this->hidden;
    DriSurfaceBuffer *src_buf, *dst_buf;

    src_buf = (DriSurfaceBuffer*)src->hwdata;
    dst_buf = (DriSurfaceBuffer*)dst->hwdata;

    if ((src->flags & GAL_SRCALPHA) == GAL_SRCALPHA &&
            (src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
        return vdata->driver_ops->alpha_key_blit(vdata->driver,
            src_buf, src_rc, dst_buf, dst_rc,
            src->format->alpha, src->format->colorkey);
    }
    else if ((src->flags & GAL_SRCALPHA) == GAL_SRCALPHA) {
        return vdata->driver_ops->alpha_blit(vdata->driver,
            src_buf, src_rc, dst_buf, dst_rc,
            src->format->alpha);
    }
    else if ((src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
        return vdata->driver_ops->key_blit(vdata->driver,
            src_buf, src_rc, dst_buf, dst_rc,
            src->format->colorkey);
    }
    else {
        return vdata->driver_ops->copy_blit(vdata->driver,
            src_buf, src_rc, dst_buf, dst_rc, COLOR_LOGICOP_COPY);
    }

    return 0;
}

static int DRI_CheckHWBlit_Accl(_THIS, GAL_Surface *src, GAL_Surface *dst)
{
    DriVideoData* vdata = this->hidden;
    DriSurfaceBuffer *src_buf, *dst_buf;
    int accelerated;

    src_buf = (DriSurfaceBuffer*)src->hwdata;
    dst_buf = (DriSurfaceBuffer*)dst->hwdata;

    /* Set initial acceleration on */
    src->flags |= GAL_HWACCEL;

    /* Set the surface attributes */
    if ((src->flags & GAL_SRCALPHA) == GAL_SRCALPHA &&
            (src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY &&
            (vdata->driver_ops->alpha_key_blit == NULL)) {
        src->flags &= ~GAL_HWACCEL;
    }
    else if ((src->flags & GAL_SRCALPHA) == GAL_SRCALPHA &&
            (vdata->driver_ops->alpha_blit == NULL)) {
        src->flags &= ~GAL_HWACCEL;
    }
    else if ((src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY &&
            (vdata->driver_ops->key_blit == NULL)) {
        src->flags &= ~GAL_HWACCEL;
    }
    else if (vdata->driver_ops->copy_blit == NULL) {
        src->flags &= ~GAL_HWACCEL;
    }

    /* Check to see if final surface blit is accelerated */
    accelerated = !!(src->flags & GAL_HWACCEL);
    if (accelerated &&
            vdata->driver_ops->check_blit(vdata->driver, src_buf, dst_buf) == 0) {
        src->map->hw_blit = DRI_HWBlit;
    }
    else {
        src->map->hw_blit = NULL;
        src->flags &= ~GAL_HWACCEL;
        accelerated = 0;
    }

    return accelerated;
}

static int DRI_FillHWRect_Accl(_THIS, GAL_Surface *dst, GAL_Rect *rect,
        Uint32 color)
{
    DriVideoData* vdata = this->hidden;
    DriSurfaceBuffer *dst_buf;

    dst_buf = (DriSurfaceBuffer*)dst->hwdata;
    return vdata->driver_ops->clear_buffer(vdata->driver, dst_buf, rect, color);
}

static void DRI_UpdateRects_Accl (_THIS, int numrects, GAL_Rect *rects)
{
    DriVideoData* vdata = this->hidden;

    if (vdata->driver_ops->flush_driver)
        vdata->driver_ops->flush_driver(vdata->driver);
}

static int DRI_SetHWColorKey_Accl(_THIS, GAL_Surface *surface, Uint32 key)
{
    return 0;
}

static int DRI_SetHWAlpha_Accl(_THIS, GAL_Surface *surface, Uint8 value)
{
    return 0;
}

MG_EXPORT int driGetDeviceFD (GHANDLE video)
{
    GAL_VideoDevice *this = (GAL_VideoDevice *)video;
    if (this && this->VideoInit == DRI_VideoInit) {
        DriVideoData* vdata = this->hidden;
        return vdata->dev_fd;
    }

    return -1;
}

/* called by driGetSurfaceInfo */
BOOL __dri_get_surface_info (GAL_Surface *surface, DriSurfaceInfo* info)
{
    GAL_VideoDevice *this = (GAL_VideoDevice *)surface->video;

    if (this && this->VideoInit == DRI_VideoInit &&
            (surface->flags & GAL_HWSURFACE)) {
        DriSurfaceBuffer* surface_buffer = (DriSurfaceBuffer*)surface->hwdata;

        if (surface_buffer) {
            info->handle = surface_buffer->handle;
            info->id = surface_buffer->id;
            info->width = surface_buffer->width;
            info->height = surface_buffer->height;
            info->pitch = surface_buffer->pitch;
            info->drm_format = surface_buffer->drm_format;
            info->size = surface_buffer->size;
            return TRUE;
        }
    }

    return FALSE;
}

/* called by driCreateDCFromName */
GAL_Surface* __dri_create_surface_from_name (GHANDLE video,
            uint32_t name, uint32_t drm_format,
            unsigned int width, unsigned int height, uint32_t pitch)
{
    GAL_VideoDevice *this = (GAL_VideoDevice *)video;
    DriVideoData* vdata = this->hidden;
    DriSurfaceBuffer* surface_buffer;
    GAL_Surface* surface = NULL;
    Uint32 RGBAmasks[4];
    int depth;

    if (this && this->VideoInit != DRI_VideoInit) {
        return NULL;
    }

    if (vdata->driver_ops->create_buffer_from_name == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: not implemented method: create_buffer_from_name!\n");
        return NULL;
    }

    depth = translate_drm_format(drm_format, RGBAmasks);
    if (depth == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported drm format: %u\n",
            drm_format);
        return NULL;
    }

    surface_buffer = vdata->driver_ops->create_buffer_from_name(vdata->driver,
            name, drm_format, width, height, pitch);
    if (surface_buffer == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: create_buffer_from_name failed!\n");
        return NULL;
    }
    /* not a foreign surface */
    surface_buffer->foreign = 0;

    if (vdata->driver_ops->map_buffer(vdata->driver, surface_buffer) == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot map hardware buffer: %m\n");
        goto error;
    }

    /* Allocate the surface */
    surface = (GAL_Surface *)malloc (sizeof(*surface));
    if (surface == NULL) {
        goto error;
    }

    /* Allocate the format */
    surface->format = GAL_AllocFormat(depth, RGBAmasks[0], RGBAmasks[1],
                RGBAmasks[2], RGBAmasks[3]);
    if (surface->format == NULL) {
        goto error;
        return(NULL);
    }

    /* Allocate an empty mapping */
    surface->map = GAL_AllocBlitMap();
    if (surface->map == NULL) {
        goto error;
    }

    surface->format_version = 0;
    surface->video = this;
    surface->flags = GAL_HWSURFACE;
    surface->dpi = GDCAP_DPI_DEFAULT;
    surface->w = width;
    surface->h = height;
    surface->pitch = pitch;
    surface->offset = 0;
    surface->pixels = surface_buffer->pixels;
    surface->hwdata = (struct private_hwdata *)surface_buffer;

    /* The surface is ready to go */
    surface->refcount = 1;

    GAL_SetClipRect(surface, NULL);

#ifdef _MGUSE_SYNC_UPDATE
    /* Initialize update region */
    InitClipRgn (&surface->update_region, &__mg_free_update_region_list);
#endif

    return(surface);

error:
    if (surface)
        GAL_FreeSurface(surface);

    if (surface_buffer)
        vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);

    return NULL;
}

/* called by driCreateDCFromHandle */
GAL_Surface* __dri_create_surface_from_handle (GHANDLE video,
            uint32_t handle, unsigned long size, uint32_t drm_format,
            unsigned int width, unsigned int height, uint32_t pitch)
{
    GAL_VideoDevice *this = (GAL_VideoDevice *)video;
    DriVideoData* vdata = this->hidden;
    DriSurfaceBuffer* surface_buffer;
    GAL_Surface* surface = NULL;
    Uint32 RGBAmasks[4];
    int depth;

    if (this && this->VideoInit != DRI_VideoInit) {
        return NULL;
    }

    if (vdata->driver_ops->create_buffer_from_handle == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: not implemented method: create_buffer_from_handle!\n");
        return NULL;
    }

    depth = translate_drm_format (drm_format, RGBAmasks);
    if (depth == 0) {
        _ERR_PRINTF ("NEWGAL>DRM: not supported drm format: %u\n",
            drm_format);
        return NULL;
    }

    surface_buffer = vdata->driver_ops->create_buffer_from_handle (vdata->driver,
            handle, size, drm_format, width, height, pitch);
    if (surface_buffer == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: create_buffer_from_handle failed!\n");
        return NULL;
    }
    /* this is a foreign surface */
    surface_buffer->foreign = 1;

    if (vdata->driver_ops->map_buffer (vdata->driver, surface_buffer) == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot map hardware buffer: %m\n");
        goto error;
    }

    /* Allocate the surface */
    surface = (GAL_Surface *)malloc (sizeof(*surface));
    if (surface == NULL) {
        goto error;
    }

    /* Allocate the format */
    surface->format = GAL_AllocFormat (depth, RGBAmasks[0], RGBAmasks[1],
                RGBAmasks[2], RGBAmasks[3]);
    if (surface->format == NULL) {
        goto error;
        return(NULL);
    }

    /* Allocate an empty mapping */
    surface->map = GAL_AllocBlitMap ();
    if (surface->map == NULL) {
        goto error;
    }

    surface->format_version = 0;
    surface->video = this;
    surface->flags = GAL_HWSURFACE;
    surface->dpi = GDCAP_DPI_DEFAULT;
    surface->w = width;
    surface->h = height;
    surface->pitch = pitch;
    surface->offset = 0;
    surface->pixels = surface_buffer->pixels;
    surface->hwdata = (struct private_hwdata *)surface_buffer;

    /* The surface is ready to go */
    surface->refcount = 1;

    GAL_SetClipRect(surface, NULL);

#ifdef _MGUSE_SYNC_UPDATE
    /* Initialize update region */
    InitClipRgn (&surface->update_region, &__mg_free_update_region_list);
#endif

    return(surface);

error:
    if (surface)
        GAL_FreeSurface(surface);

    if (surface_buffer)
        vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);

    return NULL;
}

/* called by driCreateDCFromPrimeFd */
GAL_Surface* __dri_create_surface_from_prime_fd (GHANDLE video,
            int prime_fd, unsigned long size, uint32_t drm_format,
            unsigned int width, unsigned int height, uint32_t pitch)
{
    GAL_VideoDevice *this = (GAL_VideoDevice *)video;
    DriVideoData* vdata = this->hidden;
    DriSurfaceBuffer* surface_buffer;
    GAL_Surface* surface = NULL;
    Uint32 RGBAmasks[4];
    int depth;

    if (this && this->VideoInit != DRI_VideoInit) {
        return NULL;
    }

    if (vdata->driver_ops->create_buffer_from_prime_fd == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: not implemented method: create_buffer_from_prime_fd.\n");
        return NULL;
    }

    depth = translate_drm_format(drm_format, RGBAmasks);
    if (depth == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported drm format: %u\n",
            drm_format);
        return NULL;
    }

    surface_buffer = vdata->driver_ops->create_buffer_from_prime_fd (vdata->driver,
            prime_fd, size, drm_format, width, height, pitch);
    if (surface_buffer == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: create_buffer_from_prime_fd failed!\n");
        return NULL;
    }
    /* not a foreign surface */
    surface_buffer->foreign = 1;

    if (vdata->driver_ops->map_buffer (vdata->driver, surface_buffer) == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot map hardware buffer: %m\n");
        goto error;
    }

    /* Allocate the surface */
    surface = (GAL_Surface *)malloc (sizeof(*surface));
    if (surface == NULL) {
        goto error;
    }

    /* Allocate the format */
    surface->format = GAL_AllocFormat(depth, RGBAmasks[0], RGBAmasks[1],
                RGBAmasks[2], RGBAmasks[3]);
    if (surface->format == NULL) {
        goto error;
        return(NULL);
    }

    /* Allocate an empty mapping */
    surface->map = GAL_AllocBlitMap();
    if (surface->map == NULL) {
        goto error;
    }

    surface->format_version = 0;
    surface->video = this;
    surface->flags = GAL_HWSURFACE;
    surface->dpi = GDCAP_DPI_DEFAULT;
    surface->w = width;
    surface->h = height;
    surface->pitch = pitch;
    surface->offset = 0;
    surface->pixels = surface_buffer->pixels;
    surface->hwdata = (struct private_hwdata *)surface_buffer;

    /* The surface is ready to go */
    surface->refcount = 1;

    GAL_SetClipRect(surface, NULL);

#ifdef _MGUSE_SYNC_UPDATE
    /* Initialize update region */
    InitClipRgn (&surface->update_region, &__mg_free_update_region_list);
#endif

    return(surface);

error:
    if (surface)
        GAL_FreeSurface(surface);

    if (surface_buffer)
        vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);

    return NULL;
}

#endif /* _MGGAL_DRI */

