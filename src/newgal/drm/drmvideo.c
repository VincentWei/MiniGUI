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

#ifdef _MGGAL_DRM

#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <drm/drm.h>
#include <drm/drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "minigui.h"
#include "newgal.h"
#include "exstubs.h"

#include "cursor.h"
#include "pixels_c.h"
#include "drmvideo.h"

#ifdef _MGRM_PROCESSES
#include "client.h"
#include "sharedres.h"
#endif

#define DRM_DRIVER_NAME "drm"

/* DRM engine methods for both dumb buffer and acclerated buffers */
static int DRM_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **DRM_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static int DRM_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void DRM_VideoQuit(_THIS);
static int DRM_Suspend(_THIS);
static int DRM_Resume(_THIS);
#ifdef _MGRM_PROCESSES
static void DRM_CopyVideoInfoToSharedRes(_THIS);
#endif

/* DRM engine methods for dumb buffer */
static GAL_Surface *DRM_SetVideoMode(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags);

static int DRM_AllocDumbSurface(_THIS, GAL_Surface *surface);
static void DRM_FreeDumbSurface(_THIS, GAL_Surface *surface);

/* DRM engine methods accelerated */
static int DRM_AllocHWSurface_Accl(_THIS, GAL_Surface *surface);
static void DRM_FreeHWSurface_Accl(_THIS, GAL_Surface *surface);
static int DRM_CheckHWBlit_Accl(_THIS, GAL_Surface *src, GAL_Surface *dst);
static int DRM_FillHWRect_Accl(_THIS, GAL_Surface *dst, GAL_Rect *rect,
        Uint32 color);

static void DRM_UpdateRects(_THIS, int numrects, GAL_Rect *rects);
static BOOL DRM_SyncUpdate(_THIS);

static int DRM_SetHWColorKey_Accl(_THIS, GAL_Surface *surface, Uint32 key);
static int DRM_SetHWAlpha_Accl(_THIS, GAL_Surface *surface, Uint8 value);

#if IS_SHAREDFB_SCHEMA_PROCS
/* DRM engine methods for clients under sharedfb schema and MiniGUI-Processes */
static GAL_Surface *DRM_SetVideoMode_Client(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags);
#endif  /* IS_SHAREDFB_SCHEMA_PROCS */

#if IS_COMPOSITING_SCHEMA
/* DRM engine methods for compositing schema */
static int DRM_AllocSharedHWSurface(_THIS, GAL_Surface *surface,
            size_t* pixels_size, off_t* pixels_off, Uint32 rw_modes);
static int DRM_FreeSharedHWSurface(_THIS, GAL_Surface *surface);
static int DRM_AttachSharedHWSurface(_THIS, GAL_Surface *surface,
            int prime_fd, size_t mapsize, BOOL with_rw);
static int DRM_DettachSharedHWSurface(_THIS, GAL_Surface *surface);

static int DRM_SetCursor(_THIS, GAL_Surface *surface, int hot_x, int hot_y);
static int DRM_MoveCursor(_THIS, int x, int y);
static int DRM_SetCursor_SW(_THIS, GAL_Surface *surface, int hot_x, int hot_y);
static int DRM_MoveCursor_SW(_THIS, int x, int y);
#endif  /* IS_COMPOSITING_SCHEMA */

/* DRM driver bootstrap functions */
static int DRM_Available(void)
{
    return drmAvailable();
}

/* format conversion helpers */
static int drm_format_to_bpp(uint32_t drm_format,
        int* bpp, int* cpp)
{
    switch (drm_format) {
    case DRM_FORMAT_RGB332:
    case DRM_FORMAT_BGR233:
        *bpp = 8;
        *cpp = 1;
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
        *cpp = 2;
        break;

    case DRM_FORMAT_RGB888:
    case DRM_FORMAT_BGR888:
        *bpp = 24;
        *cpp = 3;
        break;

    case DRM_FORMAT_XRGB8888:
    case DRM_FORMAT_XBGR8888:
    case DRM_FORMAT_RGBX8888:
    case DRM_FORMAT_BGRX8888:
    case DRM_FORMAT_ARGB8888:
    case DRM_FORMAT_ABGR8888:
    case DRM_FORMAT_RGBA8888:
    case DRM_FORMAT_BGRA8888:
        *bpp = 32;
        *cpp = 4;
        break;

    default:
        return -1;
    }

    return 0;
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

#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
#endif
        if (GetMgEtcValue ("drm", "pixelformat", fourcc, 4) < 0) {
            format = get_def_drm_format(*bpp);
        }
        else {
            format = fourcc_code(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);
        }
#ifdef _MGRM_PROCESSES
    }
    else {
        format = SHAREDRES_VIDEO_DRM_FORMAT;
    }
#endif

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

static GAL_Surface* create_surface_from_buffer (_THIS,
        DrmSurfaceBuffer* surface_buffer, int depth, Uint32 *RGBAmasks,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    DrmVideoData* vdata = this->hidden;
    GAL_Surface* surface = NULL;
    size_t pixels_size = height * pitch;

    if (surface_buffer->size < surface_buffer->offset + pixels_size) {
        _WRN_PRINTF ("NEWGAL>DRM: the buffer size is not large enought!\n");
    }

    /* for dumb buffer, already mapped */
    if (surface_buffer->buff == NULL &&
            vdata->driver && vdata->driver_ops->map_buffer) {
        if (vdata->driver_ops->map_buffer(vdata->driver, surface_buffer) == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: cannot map hardware buffer: %m\n");
            goto error;
        }
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
#if IS_COMPOSITING_SCHEMA
    surface->shared_header = NULL;
    surface->dirty_info = NULL;
#endif
    surface->pixels = surface_buffer->buff + surface_buffer->offset;
    surface->hwdata = (struct private_hwdata *)surface_buffer;

    /* The surface is ready to go */
    surface->refcount = 1;

    GAL_SetClipRect(surface, NULL);

#ifdef _MGUSE_UPDATE_REGION
    /* Initialize update region */
    InitClipRgn (&surface->update_region, &__mg_free_update_region_list);
#endif

    return(surface);

error:
    if (surface)
        GAL_FreeSurface(surface);

    return NULL;
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

    int        width;
    int        height;
    int        pitch;

    uint32_t   conn;
    uint32_t   crtc;

    drmModeModeInfo mode;
};

/*
 * drm_cleanup(vdata): This cleans up all the devices we created during
 * drm_prepare(). It resets the CRTCs to their saved states and deallocates
 * all memory.
 */
static void drm_cleanup(DrmVideoData* vdata)
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
            _ERR_PRINTF ("NEWGAL>DRM: failed to restore CRTC for "
                    "connector %u (%d): %m\n",
                    vdata->saved_info->conn, errno);
        }

        drmModeFreeCrtc(vdata->saved_crtc);
    }

    if (vdata->dbl_buff && vdata->real_screen) {
        GAL_FreeSurface (vdata->real_screen);
    }

    if (vdata->scanout_buff_id) {
        /* remove frame buffer */
        drmModeRmFB(vdata->dev_fd, vdata->scanout_buff_id);
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

    close (vdata->dev_fd);
}

static void DRM_DeleteDevice(GAL_VideoDevice *device)
{
    drm_cleanup(device->hidden);

    if (device->hidden->dev_name)
        free (device->hidden->dev_name);

    if (device->hidden->ex_driver)
        free (device->hidden->ex_driver);

    if (device->hidden->driver && device->hidden->driver_ops) {
        device->hidden->driver_ops->destroy_driver(device->hidden->driver);
    }

    if (device->hidden->exdrv_handle)
        dlclose(device->hidden->exdrv_handle);

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

    _DBG_PRINTF("device link path: %s\n", device_link_path);

    free (device_path);
    driver = strrchr (device_link_path, '/');
    if (driver == NULL)
        return NULL;

    return strdup (driver + strlen ("/"));
}

static DrmDriverOps* load_external_driver (DrmVideoData* vdata,
        const char* driver_name, int device_fd)
{
    const char* filename = NULL;
    char buff[LEN_SO_NAME + 1];
    DrmDriverOps* (*get_exdrv) (const char* driver_name, int device_fd);
    char* error;

#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
#endif
        filename = getenv ("MG_GAL_DRM_DRIVER");
        if (filename == NULL) {
            memset (buff, 0, sizeof (buff));
            if (GetMgEtcValue ("drm", "exdriver", buff, LEN_SO_NAME) < 0)
                return NULL;

            filename = buff;
        }
        vdata->ex_driver = strdup(filename);
#ifdef _MGRM_PROCESSES
    }
    else {
        filename = SHAREDRES_VIDEO_EXDRIVER;
        vdata->ex_driver = strdup(filename);
    }
#endif

    vdata->exdrv_handle = dlopen (filename, RTLD_LAZY);
    if (!vdata->exdrv_handle) {
        _WRN_PRINTF("failed to open specified external DRM driver: %s (%s)\n",
                filename, dlerror());
        return NULL;
    }

    dlerror();    /* Clear any existing error */
    get_exdrv = dlsym (vdata->exdrv_handle, "__drm_ex_driver_get");
    error = dlerror();
    if (error) {
        _WRN_PRINTF("failed to get symbol: %s\n", error);
        dlclose (vdata->exdrv_handle);
        vdata->exdrv_handle = NULL;
        return NULL;
    }

    return get_exdrv (driver_name, device_fd);
}

static int open_drm_device(GAL_VideoDevice *device)
{
    char *driver_name;
    int device_fd;

    driver_name = find_driver_for_device(device->hidden->dev_name);

    _DBG_PRINTF("Try to load DRM driver: %s\n", driver_name);

    device_fd = drmOpen(driver_name, NULL);
    if (device_fd < 0) {
        _ERR_PRINTF("NEWGAL>DRM: drmOpen failed\n");
        free(driver_name);
        return -errno;
    }

    device->hidden->driver_ops = load_external_driver (device->hidden,
            driver_name, device_fd);

    free (driver_name);

    /* get capabilities */
    {
        uint64_t has_dumb, cursor_width, cursor_height;

        /* check whether supports dumb buffer */
        if (drmGetCap(device_fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 ||
                !has_dumb) {
            device->hidden->cap_dumb = 0;
        }
        else {
            device->hidden->cap_dumb = 1;
        }

        if (drmGetCap(device_fd, DRM_CAP_CURSOR_WIDTH, &cursor_width) < 0 ||
                cursor_width == 0) {
            device->hidden->cap_cursor_width = 0;
        }
        else {
            device->hidden->cap_cursor_width = cursor_width;
        }

        if (drmGetCap(device_fd, DRM_CAP_CURSOR_HEIGHT, &cursor_height) < 0 ||
                cursor_height == 0) {
            device->hidden->cap_cursor_height = 0;
        }
        else {
            device->hidden->cap_cursor_height = cursor_height;
        }
    }

    if (device->hidden->driver == NULL) {
        /* check whether supports dumb buffer */
        if (!device->hidden->cap_dumb) {
            _ERR_PRINTF("NEWGAL>DRM: the DRM device '%s' does not support "
                    "dumb buffers\n",
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
        _WRN_PRINTF("failed to create DRM driver\n");
        device->hidden->driver_ops = NULL;
    }

    return 0;
}

static GAL_VideoDevice *DRM_CreateDevice(int devindex)
{
    char dev_name [LEN_DEVICE_NAME + 1];
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if (device) {
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
#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
#endif
        if (GetMgEtcValue ("drm", "device", dev_name, LEN_DEVICE_NAME) < 0) {
            strcpy(dev_name, "/dev/dri/card0");
            _WRN_PRINTF("No drm.device defined, use default '/dev/dri/card0'\n");
        }
        device->hidden->dev_name = strdup(dev_name);
#ifdef _MGRM_PROCESSES
    }
    else {
        device->hidden->dev_name = strdup (SHAREDRES_VIDEO_DEVICE);
    }
#endif

    device->hidden->dev_fd = -1;
    open_drm_device(device);
    if (device->hidden->dev_fd < 0) {
        return NULL;
    }

    /* check double buffering */
#if IS_COMPOSITING_SCHEMA
    /* force to use double buffering to compositing schema */
    if (mgIsServer)
        device->hidden->dbl_buff = 1;
    else
        device->hidden->dbl_buff = 0;
#else   /* IS_COMPOSITING_SCHEMA */

# if IS_SHAREDFB_SCHEMA_PROCS
    if (mgIsServer) {
# endif /* IS_SHAREDFB_SCHEMA_PROCS */
        char tmp [8];
        if (GetMgEtcValue ("drm", "double_buffering", tmp, 8) < 0) {
            device->hidden->dbl_buff = 0;
        }
        else if (strcasecmp (tmp, "true") == 0 ||
                strcasecmp (tmp, "yes") == 0) {
            device->hidden->dbl_buff = 1;
        }
# if IS_SHAREDFB_SCHEMA_PROCS
    }
    else {
        device->hidden->dbl_buff = SHAREDRES_VIDEO_DBL_BUFF;
    }
# endif /* IS_SHAREDFB_SCHEMA_PROCS */

#endif  /* not IS_COMPOSITING_SCHEMA */

    device->VideoInit = DRM_VideoInit;
    device->ListModes = DRM_ListModes;
    device->SetColors = DRM_SetColors;
    device->VideoQuit = DRM_VideoQuit;
    if (device->hidden->driver) {
        /* Use accelerated driver */
#ifdef _MGRM_PROCESSES
#   ifdef _MGSCHEMA_COMPOSITING
        if (mgIsServer) {
            device->SetVideoMode = DRM_SetVideoMode;
        }
        else {
            device->SetVideoMode = NULL;    // client never calls this method.
        }
#   else    /* defined _MGSCHEMA_COMPOSITING */
        if (mgIsServer) {
            device->SetVideoMode = DRM_SetVideoMode;
        }
        else {
            device->SetVideoMode = DRM_SetVideoMode_Client;
        }
#   endif   /* not defined _MGSCHEMA_COMPOSITING */
        device->RequestHWSurface = NULL;    // always be NULL
        device->CopyVideoInfoToSharedRes = DRM_CopyVideoInfoToSharedRes;
#endif   /* defined _MGRM_PROCESSES */

        device->AllocHWSurface = DRM_AllocHWSurface_Accl;
        device->FreeHWSurface = DRM_FreeHWSurface_Accl;
    }
    else {
        /* Use DUMB buffer */
#ifdef _MGRM_PROCESSES
#   ifdef _MGSCHEMA_COMPOSITING
        if (mgIsServer) {
            device->SetVideoMode = DRM_SetVideoMode;
        }
        else {
            device->SetVideoMode = NULL;    // client never calls this method.
        }
#   else    /* defined _MGSCHEMA_COMPOSITING */
        if (mgIsServer) {
            device->SetVideoMode = DRM_SetVideoMode;
        }
        else {
            device->SetVideoMode = DRM_SetVideoMode_Client;
        }

#   endif   /* not defined _MGSCHEMA_COMPOSITING */
        device->RequestHWSurface = NULL;    // always be NULL
        device->CopyVideoInfoToSharedRes = DRM_CopyVideoInfoToSharedRes;
#endif  /* defined _MGRM_PROCESSES */
        device->AllocHWSurface = DRM_AllocDumbSurface;
        device->FreeHWSurface = DRM_FreeDumbSurface;
    }

    if (device->SetVideoMode == NULL)
        device->SetVideoMode = DRM_SetVideoMode;

    if (device->hidden->dbl_buff) {
        device->UpdateRects = DRM_UpdateRects;
        device->SyncUpdate = DRM_SyncUpdate;
    }

#if IS_COMPOSITING_SCHEMA
    device->AllocSharedHWSurface = DRM_AllocSharedHWSurface;
    device->FreeSharedHWSurface = DRM_FreeSharedHWSurface;
    device->AttachSharedHWSurface = DRM_AttachSharedHWSurface;
    device->DettachSharedHWSurface = DRM_DettachSharedHWSurface;
    if (device->hidden->cap_dumb) {
        device->AllocDumbSurface = DRM_AllocDumbSurface;
        device->FreeDumbSurface = DRM_FreeDumbSurface;
    }
    else {
        device->AllocDumbSurface = NULL;
        device->FreeDumbSurface = NULL;
    }

    if (device->hidden->cap_dumb &&
            device->hidden->cap_cursor_width >= CURSORWIDTH &&
            device->hidden->cap_cursor_height >= CURSORHEIGHT) {
        device->info.hw_cursor = 1;
        device->SetCursor = DRM_SetCursor;
        device->MoveCursor = DRM_MoveCursor;
    }
    else {
        device->info.hw_cursor = 1;
        device->SetCursor = DRM_SetCursor_SW;
        device->MoveCursor = DRM_MoveCursor_SW;
    }
#endif

    /* set accelerated methods in DRM_VideoInit */
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->Suspend = DRM_Suspend;
    device->Resume = DRM_Resume;
    device->free = DRM_DeleteDevice;

    return device;
}

VideoBootStrap DRM_bootstrap = {
    DRM_DRIVER_NAME, "Linux DRI video driver",
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
        _ERR_PRINTF("NEWGAL>DRM: ignoring unused connector %u\n",
            conn->connector_id);
        return -ENOENT;
    }

    /* check if there is at least one valid mode */
    if (conn->count_modes == 0) {
        _ERR_PRINTF("NEWGAL>DRM: no valid mode for connector %u\n",
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
        _ERR_PRINTF("NEWGAL>DRM: no valid crtc for connector %u\n",
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
            _ERR_PRINTF("NEWGAL>DRM: cannot retrieve DRM connector %u:%u(%d): "
                    "%m\n", i, res->connectors[i], errno);
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
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
    {
        int n = 0;
        struct drm_mode_info *iter;

        drm_prepare(this->hidden);

        for (iter = this->hidden->mode_list; iter; iter = iter->next) {
            _DBG_PRINTF("mode #%d: %ux%u, conn: %u, crtc: %u\n", n,
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
    }
#ifdef _MGRM_PROCESSES
    else {
        int bpp, cpp;
        drm_format_to_bpp (SHAREDRES_VIDEO_DRM_FORMAT, &bpp, &cpp);

        vformat->BitsPerPixel = bpp;
        vformat->BytesPerPixel = cpp;
    }
#endif

    if (this->hidden->driver) {
        if (this->hidden->driver_ops->clear_buffer) {
            this->info.blit_fill = 1;
            this->FillHWRect = DRM_FillHWRect_Accl;
        }
        else
            this->FillHWRect = NULL;

        if (this->hidden->driver_ops->check_blit) {
            this->CheckHWBlit = DRM_CheckHWBlit_Accl;
            this->SetHWColorKey = DRM_SetHWColorKey_Accl;
            this->SetHWAlpha = DRM_SetHWAlpha_Accl;

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

static int DRM_Resume(_THIS)
{
    DrmVideoData* vdata = this->hidden;
    int ret = -1;

    _DBG_PRINTF ("%s called\n", __FUNCTION__);

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

static int DRM_Suspend(_THIS)
{
    DrmVideoData* vdata = this->hidden;
    int ret = -1;

    _DBG_PRINTF ("%s called\n", __FUNCTION__);

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

#ifdef _MGRM_PROCESSES
static void DRM_CopyVideoInfoToSharedRes(_THIS)
{
    DrmVideoData* vdata = this->hidden;

    SHAREDRES_VIDEO_DRM_FORMAT =
        ((DrmSurfaceBuffer*)vdata->real_screen->hwdata)->drm_format;

    strncpy (SHAREDRES_VIDEO_EXDRIVER, vdata->ex_driver, LEN_EXDRIVER_NAME);
    SHAREDRES_VIDEO_EXDRIVER[LEN_EXDRIVER_NAME] = 0;

    strncpy (SHAREDRES_VIDEO_DEVICE, vdata->dev_name, LEN_DEVICE_NAME);
    SHAREDRES_VIDEO_EXDRIVER[LEN_DEVICE_NAME] = 0;

    SHAREDRES_VIDEO_DBL_BUFF = vdata->dbl_buff;
}
#endif  /* _MGRM_PROCESSES */

#if 0   /* deprecated code */
/*
 * drm_create_dumb_fb(vdata, width, height, bpp):
 * Call this function to create a so called "dumb buffer".
 * We can use it for unaccelerated software rendering on the CPU.
 */
static int drm_create_dumb_fb(DrmVideoData* vdata,
        uint32_t width, uint32_t height, uint32_t drm_format, int bpp)
{
    struct drm_mode_create_dumb creq;
    struct drm_mode_destroy_dumb dreq;
    struct drm_mode_map_dumb mreq;
    uint32_t handles[4], pitches[4], offsets[4];
    int ret;

    /* create dumb buffer */
    memset(&creq, 0, sizeof(creq));
    creq.width = width;
    creq.height = height;
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

    ret = drmModeAddFB2(vdata->dev_fd, vdata->width, vdata->height, drm_format,
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
#endif   /* deprecated code */

static int drm_setup_scanout_buffer (DrmVideoData* vdata,
        uint32_t handle, uint32_t drm_format,
        uint32_t width, uint32_t height, uint32_t pitch, uint32_t offset)
{
    uint32_t handles[4], pitches[4], offsets[4];
    int ret;

    handles[0] = handle;
    pitches[0] = pitch;
    offsets[0] = offset;

    ret = drmModeAddFB2(vdata->dev_fd, width, height, drm_format,
            handles, pitches, offsets, &vdata->scanout_buff_id, 0);
    if (ret) {
        _DBG_PRINTF ("drmModeAddFB2 failed: %m\n");
        return ret;
    }

    /* perform actual modesetting on the found connector+CRTC */
    vdata->saved_crtc = drmModeGetCrtc(vdata->dev_fd, vdata->saved_info->crtc);
    //vdata->console_buff_id = vdata->saved_crtc->buffer_id;

    ret = drmModeSetCrtc(vdata->dev_fd, vdata->saved_info->crtc,
            vdata->scanout_buff_id, 0, 0, &vdata->saved_info->conn, 1,
            &vdata->saved_info->mode);
    if (ret) {
        _DBG_PRINTF ("cannot set CRTC for connector %u: %m\n",
                vdata->saved_info->conn);

        drmModeRmFB(vdata->dev_fd, vdata->scanout_buff_id);
        vdata->scanout_buff_id = 0;

        drmModeFreeCrtc(vdata->saved_crtc);
        vdata->saved_crtc = NULL;
    }

    return ret;
}

static inline uint32_t nr_lines_for_header (uint32_t header_size,
        uint32_t width, uint32_t height, int cpp)
{
    uint32_t min_pitch = width * cpp;
    uint32_t nr_lines = header_size / min_pitch;

    if (header_size % min_pitch)
        nr_lines++;

    assert (nr_lines > 0);
    return nr_lines;
}

static DrmSurfaceBuffer *drm_create_dumb_buffer(DrmVideoData* vdata,
        uint32_t drm_format, uint32_t hdr_size,
        int width, int height, int *pitch)
{
    struct drm_mode_create_dumb creq;
    struct drm_mode_destroy_dumb dreq;
    struct drm_mode_map_dumb mreq;
    int bpp, cpp, ret;
    int nr_header_lines = 0;    // the number of lines reserved for header
    DrmSurfaceBuffer *surface_buffer = NULL;

    ret = drm_format_to_bpp (drm_format, &bpp, &cpp);
    assert (ret == 0);

    /* create dumb buffer */
    memset(&creq, 0, sizeof(creq));
    if (hdr_size) {
        creq.width = width;
        nr_header_lines = nr_lines_for_header (hdr_size, width, height, cpp);
        creq.height = height + nr_header_lines;
    }
    else {
        creq.width = width;
        creq.height = height;
    }
    creq.bpp = bpp;
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (ret < 0) {
        _WRN_PRINTF("Failed to create dumb buffer (%d): %m\n", errno);
        return NULL;
    }

    if ((surface_buffer = malloc (sizeof (DrmSurfaceBuffer))) == NULL) {
        _WRN_PRINTF("Failed to allocate memory\n");
        goto err_destroy;
    }

    surface_buffer->handle = creq.handle;
    surface_buffer->drm_format = drm_format;
    //surface_buffer->width = creq.width;
    //surface_buffer->height = creq.height - nr_header_lines;
    //surface_buffer->bpp = bpp;
    //surface_buffer->cpp = cpp;
    //surface_buffer->foreign = 0;
    //surface_buffer->dumb = 0;

    surface_buffer->offset = creq.pitch * nr_header_lines;
    surface_buffer->size = creq.size;
    *pitch = creq.pitch;

    _WRN_PRINTF ("Surface buffer info: w(%u), h(%u), pitch(%u), size (%lu), offset (%lu)\n",
            width, height, creq.pitch, surface_buffer->size, surface_buffer->offset);

    /* prepare buffer for memory mapping */
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = surface_buffer->handle;
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        _WRN_PRINTF("NEWGAL>DRM: cannot map dumb buffer (%d): %m\n", errno);
        goto err_fb;
    }

    /* perform actual memory mapping */
    surface_buffer->buff = mmap(0, surface_buffer->size,
            PROT_READ | PROT_WRITE, MAP_SHARED,
            vdata->dev_fd, mreq.offset);
    if (surface_buffer->buff == MAP_FAILED) {
        _ERR_PRINTF("NEWGAL>DRM: cannot mmap dumb buffer (%d): %m\n", errno);
        goto err_fb;
    }

    return surface_buffer;

err_fb:
    free (surface_buffer);

err_destroy:
    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = creq.handle;
    drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    return NULL;
}

static DrmSurfaceBuffer *drm_create_dumb_buffer_from_handle(DrmVideoData* vdata,
        uint32_t handle, size_t size)
{
    int ret;
    DrmSurfaceBuffer *surface_buffer = NULL;
    struct drm_mode_map_dumb mreq;

    if ((surface_buffer = malloc (sizeof (DrmSurfaceBuffer))) == NULL) {
        _WRN_PRINTF("Failed to allocate memory\n");
        goto error;
    }

    surface_buffer->handle = handle;
    surface_buffer->size = size;

    /* prepare buffer for memory mapping */
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = surface_buffer->handle;
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        _WRN_PRINTF("NEWGAL>DRM: cannot map dumb buffer (%u): %m\n", mreq.handle);
        goto error;
    }

    /* perform actual memory mapping */
    surface_buffer->buff = mmap(0, surface_buffer->size,
            PROT_READ | PROT_WRITE, MAP_SHARED,
            vdata->dev_fd, mreq.offset);
    if (surface_buffer->buff == MAP_FAILED) {
        _ERR_PRINTF("NEWGAL>DRM: cannot mmap dumb buffer (%u): %m\n", errno);
        goto error;
    }

    return surface_buffer;

error:
    if (surface_buffer)
        free (surface_buffer);

    return NULL;
}

static DrmSurfaceBuffer *drm_create_dumb_buffer_from_name(DrmVideoData* vdata,
        uint32_t name)
{
    int ret;
    struct drm_gem_open oreq;
    struct drm_gem_close creq;
    DrmSurfaceBuffer *surface_buffer = NULL;

    /* open named buffer */
    memset(&oreq, 0, sizeof(oreq));
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_GEM_OPEN, &creq);
    if (ret < 0) {
        _WRN_PRINTF("Failed to open named buffer (%d): %m\n", errno);
        return NULL;
    }

    surface_buffer = drm_create_dumb_buffer_from_handle (vdata,
            oreq.handle, oreq.size);

    if (surface_buffer == NULL) {
        memset(&creq, 0, sizeof(creq));
        creq.handle = oreq.handle;
        drmIoctl(vdata->dev_fd, DRM_IOCTL_GEM_CLOSE, &creq);
    }

    return surface_buffer;
}

static DrmSurfaceBuffer *drm_create_dumb_buffer_from_prime_fd(DrmVideoData* vdata,
        int prime_fd, size_t size)
{
    int ret;
    uint32_t handle;
    DrmSurfaceBuffer *surface_buffer;

    if (size == 0) {
        off_t seek = lseek (prime_fd, 0, SEEK_END);
        if (seek != -1)
            size = seek;
        else {
            _ERR_PRINTF("NEWGAL>DRM: Failed to get size of buffer from fd (%d): "
                    "%m\n", prime_fd);
            return NULL;
        }

        _DBG_PRINTF("size got by calling lseek: %lu\n", size);
    }

	ret = drmPrimeFDToHandle (vdata->dev_fd, prime_fd, &handle);
	if (ret) {
		_ERR_PRINTF ("NEWGAL>DRM: failed to obtain handle from fd (%d): %m\n",
                prime_fd);
        return NULL;
	}

#if 0
    surface_buffer->handle = 0;
    surface_buffer->size = size;

    /* perform actual memory mapping */
    surface_buffer->buff = mmap(0, size,
            PROT_READ | PROT_WRITE, MAP_SHARED, prime_fd, 0);
    if (surface_buffer->buff == MAP_FAILED) {
        _ERR_PRINTF("NEWGAL>DRM: cannot mmap dumb buffer for prime fd (%d): "
                "%m\n", prime_fd);
        goto error;
    }
#else
    surface_buffer = drm_create_dumb_buffer_from_handle (vdata, handle, size);
    if (surface_buffer == NULL) {
        struct drm_mode_destroy_dumb dreq;
        memset(&dreq, 0, sizeof(dreq));
        dreq.handle = handle;
        drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    }
#endif

    return surface_buffer;
}

static void drm_destroy_dumb_buffer(DrmVideoData* vdata,
        DrmSurfaceBuffer *surface_buffer)
{
    struct drm_mode_destroy_dumb dreq;

    if (surface_buffer->buff)
        munmap (surface_buffer->buff, surface_buffer->size);

    /* XXX: we need to distinguish the source of the dumb buffer here. */
    memset (&dreq, 0, sizeof(dreq));
    dreq.handle = surface_buffer->handle;
    drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    free (surface_buffer);
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
static GAL_Surface *DRM_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    uint32_t drm_format;
    DrmVideoData* vdata = this->hidden;
    DrmModeInfo* info;
    DrmSurfaceBuffer *real_buffer = NULL, *shadow_buffer = NULL;
    int real_pitch, shadow_pitch;
    Uint32 RGBAmasks[4];
    int ret;

    drm_format = get_drm_format_from_etc(&bpp);
    if (drm_format == 0) {
        return NULL;
    }

    if (translate_drm_format(drm_format, RGBAmasks) == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported DRM format: %u\n",
            drm_format);
        return NULL;
    }

    /* find the connector+CRTC suitable for the resolution requested */
    info = find_mode(vdata, width, height);
    if (info == NULL) {
        _ERR_PRINTF("NEWGAL>DRM: cannot find a CRTC for video mode: %dx%d-%d\n",
            width, height, bpp);
        return NULL;
    }

    _DBG_PRINTF("going setting video mode: %dx%d-%dbpp\n",
            info->width, info->height, bpp);

    if (vdata->driver) {
        assert (vdata->driver_ops->create_buffer);
        real_buffer = vdata->driver_ops->create_buffer(vdata->driver,
                drm_format, 0, info->width, info->height, &real_pitch);

        vdata->driver_ops->map_buffer(vdata->driver, real_buffer);
    }
    else {
        real_buffer = drm_create_dumb_buffer (vdata, drm_format, 0,
                info->width, info->height, &real_pitch);
    }

    if (real_buffer == NULL || real_buffer->buff == NULL) {
        _ERR_PRINTF("NEWGAL>DRM: "
                "failed to create and map buffer for real screen\n");
        return NULL;
    }

    if (vdata->dbl_buff) {
        GAL_ShadowSurfaceHeader *hdr;
        uint32_t hdr_size = sizeof (GAL_ShadowSurfaceHeader);

        if (vdata->driver) {
            assert (vdata->driver_ops->create_buffer);
            shadow_buffer = vdata->driver_ops->create_buffer(vdata->driver,
                    drm_format, hdr_size,
                    info->width, info->height, &shadow_pitch);
            vdata->driver_ops->map_buffer(vdata->driver, shadow_buffer);
        }
        else {
            shadow_buffer = drm_create_dumb_buffer (vdata,
                    drm_format, hdr_size,
                    info->width, info->height, &shadow_pitch);
        }

        if (shadow_buffer == NULL || shadow_buffer->buff == NULL) {
            _ERR_PRINTF("NEWGAL>DRM: "
                    "faile to create and map buffer for shadow screen\n");
            goto error;
        }

        /* initialize the header */
        hdr = (GAL_ShadowSurfaceHeader *)shadow_buffer->buff;
#if IS_SHAREDFB_SCHEMA_PROCS
        ret = sem_init(&hdr->sem_lock, 1, 1);
        if (ret) {
            _ERR_PRINTF("NEWGAL>DRM: "
                    "cannot initialize the semaphore lock for dirty rectangle\n");
            goto error;
        }
#endif
        SetRectEmpty (&hdr->dirty_rc);
    }

    vdata->saved_info = info;

    /* setup real_buffer as the scanout buffer */
    ret = drm_setup_scanout_buffer (vdata, real_buffer->handle,
            real_buffer->drm_format, info->width, info->height,
            real_pitch, real_buffer->offset);
    if (ret) {
        _ERR_PRINTF("NEWGAL>DRM: cannot setup scanout buffer\n");
        goto error;
    }

    vdata->real_screen = create_surface_from_buffer (this, real_buffer,
            bpp, RGBAmasks, info->width, info->height, real_pitch);
    if (vdata->real_screen == NULL)
        goto error;

    if (shadow_buffer) {
        vdata->shadow_screen = create_surface_from_buffer (this, shadow_buffer,
                bpp, RGBAmasks, info->width, info->height, shadow_pitch);

        if (vdata->shadow_screen == NULL)
            goto error;

        GAL_SetClipRect (vdata->real_screen, NULL);
        GAL_SetColorKey (vdata->shadow_screen, 0, 0);
        GAL_SetAlpha (vdata->shadow_screen, 0, 0);
    }

    GAL_FreeSurface (current);
    if (vdata->shadow_screen)
        return vdata->shadow_screen;

    return vdata->real_screen;

error:
    if (vdata->scanout_buff_id) {
        drmModeRmFB(vdata->dev_fd, vdata->scanout_buff_id);
        vdata->scanout_buff_id = 0;

        drmModeFreeCrtc(vdata->saved_crtc);
        vdata->saved_crtc = NULL;
    }

    if (vdata->shadow_screen) {
        GAL_FreeSurface (vdata->shadow_screen);
        vdata->shadow_screen = NULL;
    }
    else if (shadow_buffer) {
        if (vdata->driver) {
            if (shadow_buffer->buff)
                vdata->driver_ops->unmap_buffer(vdata->driver, shadow_buffer);
            vdata->driver_ops->destroy_buffer(vdata->driver, shadow_buffer);
        }
        else {
            drm_destroy_dumb_buffer (vdata, shadow_buffer);
        }
    }

    if (vdata->real_screen) {
        GAL_FreeSurface (vdata->real_screen);
        vdata->real_screen = NULL;
    }
    else if (real_buffer) {
        if (vdata->driver) {
            if (real_buffer->buff)
                vdata->driver_ops->unmap_buffer(vdata->driver, real_buffer);
            vdata->driver_ops->destroy_buffer(vdata->driver, real_buffer);
        }
        else {
            drm_destroy_dumb_buffer (vdata, real_buffer);
        }
    }

    return NULL;
}

static int DRM_AllocDumbSurface (_THIS, GAL_Surface *surface)
{
    DrmVideoData* vdata = this->hidden;
    uint32_t drm_format;
    DrmSurfaceBuffer* surface_buffer;

    drm_format = translate_gal_format(surface->format);
    if (drm_format == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported pixel format, "
                "RGBA masks (0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
                surface->format->Rmask, surface->format->Gmask,
                surface->format->Bmask, surface->format->Amask);
        return -1;
    }

    surface_buffer = drm_create_dumb_buffer(vdata, drm_format, 0,
            surface->w, surface->h, &surface->pitch);
    if (surface_buffer == NULL) {
        return -1;
    }

    surface->pixels = surface_buffer->buff + surface_buffer->offset;
    surface->flags |= GAL_HWSURFACE;
    surface->hwdata = (struct private_hwdata *)surface_buffer;
    return 0;
}

static void DRM_FreeDumbSurface (_THIS, GAL_Surface *surface)
{
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer* surface_buffer;

    surface_buffer = (DrmSurfaceBuffer*)surface->hwdata;
    if (surface_buffer) {
        drm_destroy_dumb_buffer (vdata, surface_buffer);
    }

    surface->pixels = NULL;
    surface->hwdata = NULL;
}

#if IS_COMPOSITING_SCHEMA
static int DRM_AllocSharedHWSurface(_THIS, GAL_Surface *surface,
            size_t* pixels_size, off_t* pixels_off, Uint32 rw_modes)
{
    DrmVideoData* vdata = this->hidden;
    uint32_t drm_format;
    size_t hdr_size;
    DrmSurfaceBuffer* surface_buffer;
    int prime_fd;

    drm_format = translate_gal_format(surface->format);
    if (drm_format == 0) {
        _WRN_PRINTF("not supported pixel format, "
                "RGBA masks (0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
                surface->format->Rmask, surface->format->Gmask,
                surface->format->Bmask, surface->format->Amask);
        return -1;
    }

    hdr_size = sizeof (GAL_SharedSurfaceHeader);
    if (vdata->driver_ops) {
        surface_buffer = vdata->driver_ops->create_buffer(vdata->driver,
                drm_format, hdr_size, surface->w, surface->h, &surface->pitch);
    }
    else {
        surface_buffer = drm_create_dumb_buffer(vdata,
                drm_format, hdr_size, surface->w, surface->h, &surface->pitch);
    }

    if (surface_buffer == NULL) {
        _WRN_PRINTF("Failed to create shared hardware surface: size (%d x %d)\n",
                surface->w, surface->h);
        return -1;
    }

    /* get the prime fd */
    if (drmPrimeHandleToFD (vdata->dev_fd, surface_buffer->handle,
                DRM_RDWR | DRM_CLOEXEC, &prime_fd)) {
        _WRN_PRINTF ("cannot get prime fd: %m\n");
        goto error;
    }

    *pixels_size = surface->h * surface->pitch;
    *pixels_off = surface_buffer->offset;

    /* go for success */
    /* the caller will set the pixels and flags
    surface->pixels = surface_buffer->buff + surface_buffer->offset;
    surface->flags |= GAL_HWSURFACE;
    */
    surface->shared_header = (GAL_SharedSurfaceHeader*)surface_buffer->buff;
    surface->hwdata = (struct private_hwdata *)surface_buffer;

    return prime_fd;

error:
    if (surface_buffer) {
        if (vdata->driver_ops)
            vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);
        else
            drm_destroy_dumb_buffer(vdata, surface_buffer);
    }

    return -1;
}

static int DRM_FreeSharedHWSurface(_THIS, GAL_Surface *surface)
{
    int retval = -1;
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer* surface_buffer;

    surface_buffer = (DrmSurfaceBuffer*)surface->hwdata;
    if (surface_buffer) {
        if (vdata->driver_ops)
            vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);
        else
            drm_destroy_dumb_buffer(vdata, surface_buffer);

        retval = 0;
    }

    return retval;
}

static int DRM_AttachSharedHWSurface(_THIS, GAL_Surface *surface,
            int prime_fd, size_t mapsize, BOOL with_rw)
{
    int retval = -1;
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer* surface_buffer = NULL;

    if (vdata->driver_ops) {
        surface_buffer = vdata->driver_ops->create_buffer_from_prime_fd (
                vdata->driver, prime_fd, mapsize);
        if (surface_buffer == NULL) {
            _WRN_PRINTF ("failed to create buffer from prime fd: %d!\n",
                    prime_fd);
            goto error;
        }

        if (vdata->driver_ops->map_buffer (vdata->driver,
                    surface_buffer) == NULL) {
            _WRN_PRINTF ("cannot map hardware buffer: %m\n");
            goto error;
        }

        surface->shared_header = (GAL_SharedSurfaceHeader*)surface_buffer->buff;
        surface->hwdata = (struct private_hwdata *)surface_buffer;
        retval = 0;
    }

    /* for shared dumb buffer, the caller uses mmap directly */

    return retval;

error:
    if (surface_buffer) {
        vdata->driver_ops->destroy_buffer (vdata->driver, surface_buffer);
    }

    return retval;
}

static int DRM_DettachSharedHWSurface(_THIS, GAL_Surface *surface)
{
    int retval = -1;
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer* surface_buffer;

    surface_buffer = (DrmSurfaceBuffer*)surface->hwdata;
    if (surface_buffer) {
        if (vdata->driver) {
            vdata->driver_ops->unmap_buffer(vdata->driver, surface_buffer);
            vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);
        }
        else {
            drm_destroy_dumb_buffer(vdata, surface_buffer);
        }

        surface->pixels = NULL;
        surface->hwdata = NULL;
        retval = 0;
    }

    return retval;
}

static int DRM_SetCursor(_THIS, GAL_Surface *surface, int hot_x, int hot_y)
{
    int retval = -1;
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer* surface_buffer;

    surface_buffer = (DrmSurfaceBuffer*)surface->hwdata;
    if (surface_buffer) {
        retval = drmModeSetCursor2 (vdata->dev_fd, vdata->saved_crtc->crtc_id,
                surface_buffer->handle, surface->w, surface->h, hot_x, hot_y);
        if (retval)
            _WRN_PRINTF ("failed to call drmModeSetCursor2: %m\n");
    }

    return retval;
}

static int DRM_MoveCursor(_THIS, int x, int y)
{
    int retval = -1;
    DrmVideoData* vdata = this->hidden;

    retval = drmModeMoveCursor (vdata->dev_fd, vdata->saved_crtc->crtc_id, x, y);
    if (retval) {
        _WRN_PRINTF ("failed to call drmModeMoveCursor: %m\n");
    }

    return retval;
}

static inline int boxleft (_THIS)
{
    if (this->hidden->cursor == NULL)
        return -100;
    return this->hidden->csr_x - this->hidden->hot_x;
}

static inline int boxtop (_THIS)
{
    if (this->hidden->cursor == NULL)
        return -100;
    return this->hidden->csr_y - this->hidden->hot_y;
}

static int DRM_SetCursor_SW (_THIS, GAL_Surface *surface, int hot_x, int hot_y)
{
    GAL_Rect rect;

    if (this->hidden->cursor == surface &&
            this->hidden->hot_x == hot_x &&
            this->hidden->hot_y == hot_y) {
        return 0;
    }

    /* update screen to hide old cursor */
    if (this->hidden->cursor) {
        rect.x = boxleft (this);
        rect.y = boxtop (this);
        rect.w = CURSORWIDTH;
        rect.h = CURSORHEIGHT;

        this->hidden->cursor = NULL;
        this->UpdateRects (this, 1, &rect);
    }

    this->hidden->cursor = surface;
    this->hidden->hot_x = hot_x;
    this->hidden->hot_y = hot_y;

    /* update screen to show new cursor */
    if (this->hidden->cursor) {
        rect.x = boxleft (this);
        rect.y = boxtop (this);
        rect.w = CURSORWIDTH;
        rect.h = CURSORHEIGHT;
        this->UpdateRects (this, 1, &rect);
    }

    this->SyncUpdate (this);
    return 0;
}

static int DRM_MoveCursor_SW (_THIS, int x, int y)
{
    if (this->hidden->csr_x == x &&
             this->hidden->csr_y == y) {
        return 0;
    }

    if (this->hidden->cursor) {
        GAL_Surface* tmp;
        GAL_Rect rect;
        rect.x = boxleft (this);
        rect.y = boxtop (this);
        rect.w = CURSORWIDTH;
        rect.h = CURSORHEIGHT;

        /* update screen to hide cursor */
        tmp = this->hidden->cursor;
        this->hidden->cursor = NULL;
        this->UpdateRects (this, 1, &rect);

        /* update screen to show cursor */
        this->hidden->cursor = tmp;
        this->hidden->csr_x = x;
        this->hidden->csr_y = y;

        rect.x = boxleft (this);
        rect.y = boxtop (this);
        rect.w = CURSORWIDTH;
        rect.h = CURSORHEIGHT;
        this->UpdateRects (this, 1, &rect);
        this->SyncUpdate (this);
    }
    else {
        this->hidden->csr_x = x;
        this->hidden->csr_y = y;
    }

    return 0;
}

#endif  /* IS_COMPOSITING_SCHEMA */

#if 0   /* deprecated code */
/* DRM engine methods for accelerated buffers */
static GAL_Surface *DRM_SetVideoMode_Accl(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags)
{
    DrmVideoData* vdata = this->hidden;
    DrmModeInfo* info;
    uint32_t drm_format, buff_id;
    Uint32 RGBAmasks[4];
    DrmSurfaceBuffer* scanout_buff = NULL;

    drm_format = get_drm_format_from_etc(&bpp);
    if (drm_format == 0) {
        return NULL;
    }

    /* find the connector+CRTC suitable for the resolution requested */
    info = find_mode(vdata, width, height);
    if (info == NULL) {
        _ERR_PRINTF("NEWGAL>DRM: cannot find a CRTC for video mode: %dx%d-%d\n",
            width, height, bpp);
        return NULL;
    }

    _DBG_PRINTF("going setting video mode: %dx%d-%dbpp\n",
            info->width, info->height, bpp);

#if 0
    if (drmSetMaster(this->hidden->dev_fd)) {
        _ERR_PRINTF("NEWGAL>DRM: failed to call drmSetMaster: %m\n");
        return NULL;
    }
#endif

    /* create the scanout buffer */
    scanout_buff = vdata->driver_ops->create_buffer(vdata->driver, drm_format,
            0, info->width, info->height, &info->pitch);
    if (scanout_buff == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot create scanout buffer: %m\n");
        goto error;
    }

    /* set up it as frame buffer */
    {
        uint32_t handles[4], pitches[4], offsets[4];
        handles[0] = scanout_buff->handle;
        pitches[0] = info->pitch;
        offsets[0] = scanout_buff->offset;

        if (drmModeAddFB2(vdata->dev_fd,
                info->width, info->height, drm_format,
                handles, pitches, offsets, &buff_id, 0) != 0) {
            _ERR_PRINTF ("NEWGAL>DRM: cannot set up scanout frame buffer: %m\n");
            goto error;
        }
    }

    if (NULL == vdata->driver_ops->map_buffer(vdata->driver, scanout_buff)) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot map scanout frame buffer: %m\n");
        goto error;
    }

    vdata->width = info->width;
    vdata->height = info->height;
    vdata->bpp = bpp;
    vdata->pitch = info->pitch;
    vdata->size = info->pitch * info->height;
    vdata->handle = 0;
    vdata->scanout_buff_id = buff_id;
    vdata->scanout_fb = scanout_buff->buff + scanout_buff->offset;

    _DBG_PRINTF("scanout frame buffer: size (%dx%d), pitch(%d)\n",
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

    _DBG_PRINTF("real screen mode: %dx%d-%dbpp\n",
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
#endif   /* deprecated code */

#if IS_SHAREDFB_SCHEMA_PROCS

int __drm_get_shared_screen_surface (const char *name, SHAREDSURFINFO* info)
{
    GAL_VideoDevice *this = __mg_current_video;
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer* surface_buffer = NULL;

    if (strcmp (name, SYSSF_REAL_SCREEN) == 0) {
        assert (vdata->real_screen);

        surface_buffer = (DrmSurfaceBuffer *)vdata->real_screen->hwdata;

        if (vdata->real_name == 0) {
            struct drm_gem_flink flink;

            memset (&flink, 0, sizeof (flink));
            flink.handle = surface_buffer->handle;
            if (drmIoctl(vdata->dev_fd, DRM_IOCTL_GEM_FLINK, &flink))
                return -1;

            vdata->real_name = flink.name;
        }

        info->flags = vdata->real_screen->flags;
        info->width = vdata->real_screen->w;
        info->height = vdata->real_screen->h;
        info->pitch = vdata->real_screen->pitch;
        info->name = vdata->real_name;
    }
    else if (strcmp (name, SYSSF_SHADOW_SCREEN) == 0 && vdata->shadow_screen) {
        surface_buffer = (DrmSurfaceBuffer *)vdata->shadow_screen->hwdata;
        if (vdata->shadow_name == 0) {
            struct drm_gem_flink flink;

            memset (&flink, 0, sizeof (flink));
            flink.handle = surface_buffer->handle;
            if (drmIoctl (vdata->dev_fd, DRM_IOCTL_GEM_FLINK, &flink))
                return -1;

            vdata->shadow_name = flink.name;
        }

        info->flags = vdata->shadow_screen->flags;
        info->name = vdata->shadow_name;
    }

    if (surface_buffer) {
        info->drm_format = surface_buffer->drm_format;
        info->size = surface_buffer->size;
        info->offset = surface_buffer->offset;
    }
    else {
        memset (info, 0, sizeof (*info));
    }

    return 0;
}

/* DRM engine method for clients under sharedfb schema and MiniGUI-Processes  */
static GAL_Surface *DRM_SetVideoMode_Client(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags)
{
    DrmVideoData* vdata = this->hidden;
    REQUEST req;
    SHAREDSURFINFO info;

    req.id = REQID_GETSHAREDSURFACE;
    req.data = SYSSF_REAL_SCREEN;
    req.len_data = strlen (SYSSF_REAL_SCREEN) + 1;

    if ((ClientRequestEx (&req, NULL, 0, &info, sizeof (SHAREDSURFINFO)) < 0)) {
        goto error;
    }

    _DBG_PRINTF ("REQID_GETSHAREDSURFACE for %s: name(%u), flags(%x), "
            "size (%lu), offset (%lu)\n",
            SYSSF_REAL_SCREEN,
            info.name, info.flags, info.size, info.offset);

    vdata->real_screen = __drm_create_surface_from_name (this, info.name,
            info.drm_format, info.offset, info.width, info.height, info.pitch);

    if (vdata->real_screen == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: failed to create real screen surface: %u!\n",
                info.name);
        goto error;
    }

    if (SHAREDRES_VIDEO_DBL_BUFF == 0) {
        GAL_FreeSurface (current);
        return vdata->real_screen;
    }

    /* get shadow surface */
    req.id = REQID_GETSHAREDSURFACE;
    req.data = SYSSF_SHADOW_SCREEN;
    req.len_data = strlen (SYSSF_SHADOW_SCREEN) + 1;

    if ((ClientRequestEx (&req, NULL, 0, &info, sizeof (SHAREDSURFINFO)) < 0)) {
        goto error;
    }

    _DBG_PRINTF ("REQID_GETSHAREDSURFACE for %s: name(%u), flags(%x), "
            "size (%lu), offset (%lu)\n",
            SYSSF_SHADOW_SCREEN,
            info.name, info.flags, info.size, info.offset);

    vdata->shadow_screen = __drm_create_surface_from_name (this, info.name,
            info.drm_format, info.offset, info.width, info.height, info.pitch);

    if (vdata->shadow_screen == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: failed to create shadow screen surface: %u!\n",
                info.name);
        goto error;
    }

    GAL_FreeSurface (current);
    return vdata->shadow_screen;

error:
    if (vdata->real_screen) {
        GAL_FreeSurface (vdata->real_screen);
    }

    return NULL;
}
#endif  /* IS_SHAREDFB_SCHEMA_PROCS */

static int DRM_AllocHWSurface_Accl(_THIS, GAL_Surface *surface)
{
    DrmVideoData* vdata = this->hidden;
    uint32_t drm_format;
    DrmSurfaceBuffer* surface_buffer;

    drm_format = translate_gal_format(surface->format);
    if (drm_format == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported pixel format, "
                "RGBA masks (0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
                surface->format->Rmask, surface->format->Gmask,
                surface->format->Bmask, surface->format->Amask);
        return -1;
    }

    surface_buffer = vdata->driver_ops->create_buffer(vdata->driver, drm_format,
            0, surface->w, surface->h, &surface->pitch);
    if (surface_buffer == NULL) {
        return -1;
    }

    if (vdata->driver_ops->map_buffer(vdata->driver, surface_buffer) == NULL) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot map hardware buffer: %m\n");
        goto error;
    }

    surface->pixels = surface_buffer->buff + surface_buffer->offset;
    surface->flags |= GAL_HWSURFACE;
    surface->hwdata = (struct private_hwdata *)surface_buffer;
    return 0;

error:
    if (surface_buffer)
        vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);

    return -1;
}

static void DRM_FreeHWSurface_Accl(_THIS, GAL_Surface *surface)
{
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer* surface_buffer;

    surface_buffer = (DrmSurfaceBuffer*)surface->hwdata;
    if (surface_buffer) {
        if (surface_buffer->buff)
            vdata->driver_ops->unmap_buffer(vdata->driver, surface_buffer);
        vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);
    }

    surface->pixels = NULL;
    surface->hwdata = NULL;
}

static int DRM_HWBlit(GAL_Surface *src, GAL_Rect *src_rc,
                       GAL_Surface *dst, GAL_Rect *dst_rc)
{
    GAL_VideoDevice *this = __mg_current_video;
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer *src_buf, *dst_buf;

    src_buf = (DrmSurfaceBuffer*)src->hwdata;
    dst_buf = (DrmSurfaceBuffer*)dst->hwdata;

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

static int DRM_CheckHWBlit_Accl(_THIS, GAL_Surface *src, GAL_Surface *dst)
{
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer *src_buf, *dst_buf;
    int accelerated;

    src_buf = (DrmSurfaceBuffer*)src->hwdata;
    dst_buf = (DrmSurfaceBuffer*)dst->hwdata;

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
        src->map->hw_blit = DRM_HWBlit;
    }
    else {
        src->map->hw_blit = NULL;
        src->flags &= ~GAL_HWACCEL;
        accelerated = 0;
    }

    return accelerated;
}

static int DRM_FillHWRect_Accl(_THIS, GAL_Surface *dst, GAL_Rect *rect,
        Uint32 color)
{
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer *dst_buf;

    dst_buf = (DrmSurfaceBuffer*)dst->hwdata;
    return vdata->driver_ops->clear_buffer(vdata->driver, dst_buf, rect, color);
}

static void DRM_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;
    GAL_ShadowSurfaceHeader* hdr;

    assert (this->hidden->dbl_buff && this->hidden->shadow_screen);

    hdr = (GAL_ShadowSurfaceHeader*)
        ((DrmSurfaceBuffer*)this->hidden->shadow_screen->hwdata)->buff;

#if IS_SHAREDFB_SCHEMA_PROCS
    sem_wait(&hdr->sem_lock);
#endif

    bound = hdr->dirty_rc;
    for (i = 0; i < numrects; i++) {
        RECT rc;

        SetRect (&rc, rects[i].x, rects[i].y,
                rects[i].x + rects[i].w, rects[i].y + rects[i].h);
        if (IsRectEmpty (&bound))
            bound = rc;
        else
            GetBoundRect (&bound, &bound, &rc);
    }

    hdr->dirty_rc = bound;

#if IS_SHAREDFB_SCHEMA_PROCS
    sem_post(&hdr->sem_lock);
#endif
}

static BOOL DRM_SyncUpdate (_THIS)
{
    BOOL retval = FALSE;
    RECT bound;
    GAL_ShadowSurfaceHeader* hdr;

    assert (this->hidden->dbl_buff && this->hidden->shadow_screen);

    hdr = (GAL_ShadowSurfaceHeader*)
        ((DrmSurfaceBuffer*)this->hidden->shadow_screen->hwdata)->buff;

#if IS_SHAREDFB_SCHEMA_PROCS
    sem_wait(&hdr->sem_lock);
#endif

    if (IsRectEmpty (&hdr->dirty_rc))
        goto ret;

    bound = hdr->dirty_rc;

    if (this->hidden->real_screen) {
        GAL_Rect src_rect, dst_rect;
        src_rect.x = bound.left;
        src_rect.y = bound.top;
        src_rect.w = RECTW (bound);
        src_rect.h = RECTH (bound);
        dst_rect = src_rect;

        GAL_BlitSurface (this->hidden->shadow_screen, &src_rect,
                this->hidden->real_screen, &dst_rect);
#ifdef _MGSCHEMA_COMPOSITING
        if (this->hidden->cursor) {
            RECT csr_rc, eff_rc;
            csr_rc.left = boxleft (this);
            csr_rc.top = boxtop (this);
            csr_rc.right = csr_rc.left + CURSORWIDTH;
            csr_rc.bottom = csr_rc.top + CURSORHEIGHT;

            if (IntersectRect (&eff_rc, &csr_rc, &bound)) {
                src_rect.x = eff_rc.left - csr_rc.left;
                src_rect.y = eff_rc.top - csr_rc.top;
                src_rect.w = RECTW (eff_rc);
                src_rect.h = RECTH (eff_rc);

                dst_rect.x = eff_rc.left;
                dst_rect.y = eff_rc.top;
                dst_rect.w = src_rect.w;
                dst_rect.h = src_rect.h;
                GAL_BlitSurface (this->hidden->cursor, &src_rect,
                        this->hidden->real_screen, &dst_rect);
            }
        }
#endif  /* _MGSCHEMA_COMPOSITING */
    }

ret:
#if IS_SHAREDFB_SCHEMA_PROCS
    sem_post(&hdr->sem_lock);
#endif
    return retval;
}

static int DRM_SetHWColorKey_Accl(_THIS, GAL_Surface *surface, Uint32 key)
{
    return 0;
}

static int DRM_SetHWAlpha_Accl(_THIS, GAL_Surface *surface, Uint8 value)
{
    return 0;
}

MG_EXPORT int drmGetDeviceFD (GHANDLE video)
{
    GAL_VideoDevice *this = (GAL_VideoDevice *)video;
    if (this && this->VideoInit == DRM_VideoInit) {
        DrmVideoData* vdata = this->hidden;
        return vdata->dev_fd;
    }

    return -1;
}

/* called by drmGetSurfaceInfo */
BOOL __drm_get_surface_info (GAL_Surface *surface, DrmSurfaceInfo* info)
{
    GAL_VideoDevice *this = (GAL_VideoDevice *)surface->video;

    if (this && this->VideoInit == DRM_VideoInit &&
            (surface->flags & GAL_HWSURFACE)) {
        DrmSurfaceBuffer* surface_buffer = (DrmSurfaceBuffer*)surface->hwdata;

        if (surface_buffer) {
            info->handle = surface_buffer->handle;
            info->width = surface->w;
            info->height = surface->h;
            info->pitch = surface->pitch;
            info->drm_format = surface_buffer->drm_format;
            info->size = surface_buffer->size;
            info->offset = surface_buffer->offset;
            return TRUE;
        }
    }

    return FALSE;
}

/* called by drmCreateDCFromName */
GAL_Surface* __drm_create_surface_from_name (GHANDLE video,
            uint32_t name, uint32_t drm_format, uint32_t pixels_off,
            uint32_t width, uint32_t height, uint32_t pitch)
{
    GAL_VideoDevice *this = (GAL_VideoDevice *)video;
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer* surface_buffer;
    Uint32 RGBAmasks[4];
    int depth;

    if (this && this->VideoInit != DRM_VideoInit) {
        return NULL;
    }

    depth = translate_drm_format(drm_format, RGBAmasks);
    if (depth == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported DRM format: %u\n", drm_format);
        return NULL;
    }

    if (vdata->driver == NULL ||
            vdata->driver_ops->create_buffer_from_name == NULL) {
        surface_buffer = drm_create_dumb_buffer_from_name (vdata, name);
        if (surface_buffer == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: failed to create dumb buffer from name: "
                    "%u!\n", name);
            return NULL;
        }
    }
    else {
        surface_buffer = vdata->driver_ops->create_buffer_from_name(
                vdata->driver, name);
        if (surface_buffer == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: faile to create buffer from name: %u!\n",
                    name);
            return NULL;
        }
    }

    surface_buffer->drm_format = drm_format;
    surface_buffer->offset = pixels_off;

    return create_surface_from_buffer (this, surface_buffer, depth,
            RGBAmasks, width, height, pitch);
}

/* called by drmCreateDCFromHandle */
GAL_Surface* __drm_create_surface_from_handle (GHANDLE video, uint32_t handle,
        unsigned long size, uint32_t drm_format, uint32_t pixels_off,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    GAL_VideoDevice *this = (GAL_VideoDevice *)video;
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer* surface_buffer;
    Uint32 RGBAmasks[4];
    int depth;

    if (this && this->VideoInit != DRM_VideoInit) {
        return NULL;
    }

    depth = translate_drm_format (drm_format, RGBAmasks);
    if (depth == 0) {
        _ERR_PRINTF ("NEWGAL>DRM: not supported drm format: %u\n", drm_format);
        return NULL;
    }

    if (vdata->driver == NULL ||
            vdata->driver_ops->create_buffer_from_handle == NULL) {
        surface_buffer = drm_create_dumb_buffer_from_handle (vdata,
                handle, size);
        if (surface_buffer == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: failed to create dumb buffer from handle "
                    "(%u): %m!\n", handle);
            return NULL;
        }
    }
    else {
        surface_buffer = vdata->driver_ops->create_buffer_from_handle (
                vdata->driver, handle, size);
        if (surface_buffer == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: failed to create buffer from handle (%u): "
                   "%m!\n", handle);
            return NULL;
        }
    }

    surface_buffer->drm_format = drm_format;
    surface_buffer->offset = pixels_off;

    return create_surface_from_buffer (this, surface_buffer, depth,
            RGBAmasks, width, height, pitch);
}

/* called by drmCreateDCFromPrimeFd */
GAL_Surface* __drm_create_surface_from_prime_fd (GHANDLE video,
        int prime_fd, size_t size, uint32_t drm_format, uint32_t pixels_off,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    GAL_VideoDevice *this = (GAL_VideoDevice *)video;
    DrmVideoData* vdata = this->hidden;
    DrmSurfaceBuffer* surface_buffer;
    Uint32 RGBAmasks[4];
    int depth;

    if (this && this->VideoInit != DRM_VideoInit) {
        return NULL;
    }

    depth = translate_drm_format(drm_format, RGBAmasks);
    if (depth == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported drm format: %u\n",
            drm_format);
        return NULL;
    }

    if (vdata->driver == NULL ||
            vdata->driver_ops->create_buffer_from_prime_fd == NULL) {
        surface_buffer = drm_create_dumb_buffer_from_prime_fd (vdata,
                prime_fd, size);
        if (surface_buffer == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: failed to create dumb buffer from prime "
                    "fd: %d!\n", prime_fd);
            return NULL;
        }
    }
    else {
        surface_buffer = vdata->driver_ops->create_buffer_from_prime_fd (
                vdata->driver, prime_fd, size);
        if (surface_buffer == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: failed to create buffer from prime "
                    "fd: %d!\n", prime_fd);
            return NULL;
        }
    }

    surface_buffer->drm_format = drm_format;
    surface_buffer->offset = pixels_off;

    return create_surface_from_buffer (this, surface_buffer, depth,
            RGBAmasks, width, height, pitch);
}

#endif /* _MGGAL_DRM */

