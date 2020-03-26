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
 *   Copyright (C) 2019 ~ 2020, Beijing FMSoft Technologies Co., Ltd.
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

#if IS_SHAREDFB_SCHEMA_PROCS
/* we use shm_open for the shared shadow screen buffer among processes
   under MiniGUI-Processes runtime mode and shared frame buffer schema. */
#define SHMNAME_SHADOW_SCREEN_BUFFER "/minigui-procs-shadow-screen-buffer"
#endif  /* IS_SHAREDFB_SCHEMA_PROCS */

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
static int DRM_AllocDumbSurface(_THIS, GAL_Surface *surface);
static void DRM_FreeDumbSurface(_THIS, GAL_Surface *surface);

static int DRM_AllocSharedHWSurface(_THIS, GAL_Surface *surface,
            size_t* map_size, off_t* pixels_off, Uint32 rw_modes);
static int DRM_FreeSharedHWSurface(_THIS, GAL_Surface *surface);
static int DRM_AttachSharedHWSurface(_THIS, GAL_Surface *surface,
            int prime_fd, size_t mapsize, BOOL with_rw);
static int DRM_DettachSharedHWSurface(_THIS, GAL_Surface *surface);

static int DRM_SetCursor(_THIS, GAL_Surface *surface, int hot_x, int hot_y);
static int DRM_MoveCursor(_THIS, int x, int y);
static int DRM_SetCursor_Plane(_THIS, GAL_Surface *surface, int hot_x, int hot_y);
static int DRM_MoveCursor_Plane(_THIS, int x, int y);
static int DRM_SetCursor_SW(_THIS, GAL_Surface *surface, int hot_x, int hot_y);
static int DRM_MoveCursor_SW(_THIS, int x, int y);

#if 0   /* test code */
/* for asynchronous update */
#include <pthread.h>

static void* task_do_update (void* data);
static void DRM_UpdateRects_Async(_THIS, int numrects, GAL_Rect *rects);
static BOOL DRM_SyncUpdate_Async(_THIS);
#endif  /* test code */

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

static int translate_drm_format(uint32_t drm_format, Uint32* RGBAmasks,
        int* ret_cpp)
{
    int bpp = 0;
    int cpp = 0;

    switch (drm_format) {
    case DRM_FORMAT_RGB332:
        RGBAmasks[0] = 0xE0;
        RGBAmasks[1] = 0x1C;
        RGBAmasks[2] = 0x03;
        RGBAmasks[3] = 0x00;
        bpp = 8;
        cpp = 1;
        break;

    case DRM_FORMAT_BGR233:
        RGBAmasks[0] = 0x0E;
        RGBAmasks[1] = 0x38;
        RGBAmasks[2] = 0xC0;
        RGBAmasks[3] = 0x00;
        bpp = 8;
        cpp = 1;
        break;

    case DRM_FORMAT_XRGB4444:
        RGBAmasks[0] = 0x0F00;
        RGBAmasks[1] = 0x00F0;
        RGBAmasks[2] = 0x000F;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_XBGR4444:
        RGBAmasks[0] = 0x000F;
        RGBAmasks[1] = 0x00F0;
        RGBAmasks[2] = 0x0F00;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_RGBX4444:
        RGBAmasks[0] = 0xF000;
        RGBAmasks[1] = 0x0F00;
        RGBAmasks[2] = 0x00F0;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_BGRX4444:
        RGBAmasks[0] = 0x00F0;
        RGBAmasks[1] = 0x0F00;
        RGBAmasks[2] = 0xF000;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_ARGB4444:
        RGBAmasks[0] = 0x0F00;
        RGBAmasks[1] = 0x00F0;
        RGBAmasks[2] = 0x000F;
        RGBAmasks[3] = 0xF000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_ABGR4444:
        RGBAmasks[0] = 0x000F;
        RGBAmasks[1] = 0x00F0;
        RGBAmasks[2] = 0x0F00;
        RGBAmasks[3] = 0xF000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_RGBA4444:
        RGBAmasks[0] = 0xF000;
        RGBAmasks[1] = 0x0F00;
        RGBAmasks[2] = 0x00F0;
        RGBAmasks[3] = 0x000F;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_BGRA4444:
        RGBAmasks[0] = 0x00F0;
        RGBAmasks[1] = 0x0F00;
        RGBAmasks[2] = 0xF000;
        RGBAmasks[3] = 0x000F;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_XRGB1555:
        RGBAmasks[0] = 0x7C00;
        RGBAmasks[1] = 0x03E0;
        RGBAmasks[2] = 0x001F;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_XBGR1555:
        RGBAmasks[0] = 0x001F;
        RGBAmasks[1] = 0x03E0;
        RGBAmasks[2] = 0x7C00;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_RGBX5551:
        RGBAmasks[0] = 0xF800;
        RGBAmasks[1] = 0x07C0;
        RGBAmasks[2] = 0x003E;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_BGRX5551:
        RGBAmasks[0] = 0x003E;
        RGBAmasks[1] = 0x07C0;
        RGBAmasks[2] = 0xF800;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_ARGB1555:
        RGBAmasks[0] = 0x7C00;
        RGBAmasks[1] = 0x03E0;
        RGBAmasks[2] = 0x001F;
        RGBAmasks[3] = 0x8000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_ABGR1555:
        RGBAmasks[0] = 0x001F;
        RGBAmasks[1] = 0x03E0;
        RGBAmasks[2] = 0x7C00;
        RGBAmasks[3] = 0x8000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_RGBA5551:
        RGBAmasks[0] = 0xF800;
        RGBAmasks[1] = 0x07C0;
        RGBAmasks[2] = 0x003E;
        RGBAmasks[3] = 0x0001;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_BGRA5551:
        RGBAmasks[0] = 0x003E;
        RGBAmasks[1] = 0x07C0;
        RGBAmasks[2] = 0xF800;
        RGBAmasks[3] = 0x0001;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_RGB565:
        RGBAmasks[0] = 0xF800;
        RGBAmasks[1] = 0x07E0;
        RGBAmasks[2] = 0x001F;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_BGR565:
        RGBAmasks[0] = 0x001F;
        RGBAmasks[1] = 0x07E0;
        RGBAmasks[2] = 0xF800;
        RGBAmasks[3] = 0x0000;
        bpp = 16;
        cpp = 2;
        break;

    case DRM_FORMAT_RGB888:
        RGBAmasks[0] = 0xFF0000;
        RGBAmasks[1] = 0x00FF00;
        RGBAmasks[2] = 0x0000FF;
        RGBAmasks[3] = 0x000000;
        bpp = 24;
        cpp = 3;
        break;

    case DRM_FORMAT_BGR888:
        RGBAmasks[0] = 0x0000FF;
        RGBAmasks[1] = 0x00FF00;
        RGBAmasks[2] = 0xFF0000;
        RGBAmasks[3] = 0x000000;
        bpp = 24;
        cpp = 3;
        break;

    case DRM_FORMAT_XRGB8888:
        RGBAmasks[0] = 0x00FF0000;
        RGBAmasks[1] = 0x0000FF00;
        RGBAmasks[2] = 0x000000FF;
        RGBAmasks[3] = 0x00000000;
        bpp = 32;
        cpp = 4;
        break;

    case DRM_FORMAT_XBGR8888:
        RGBAmasks[0] = 0x000000FF;
        RGBAmasks[1] = 0x0000FF00;
        RGBAmasks[2] = 0x00FF0000;
        RGBAmasks[3] = 0x00000000;
        bpp = 32;
        cpp = 4;
        break;

    case DRM_FORMAT_RGBX8888:
        RGBAmasks[0] = 0xFF000000;
        RGBAmasks[1] = 0x00FF0000;
        RGBAmasks[2] = 0x0000FF00;
        RGBAmasks[3] = 0x00000000;
        bpp = 32;
        cpp = 4;
        break;

    case DRM_FORMAT_BGRX8888:
        RGBAmasks[0] = 0x0000FF00;
        RGBAmasks[1] = 0x00FF0000;
        RGBAmasks[2] = 0xFF000000;
        RGBAmasks[3] = 0x00000000;
        bpp = 32;
        cpp = 4;
        break;

    case DRM_FORMAT_ARGB8888:
        RGBAmasks[0] = 0x00FF0000;
        RGBAmasks[1] = 0x0000FF00;
        RGBAmasks[2] = 0x000000FF;
        RGBAmasks[3] = 0xFF000000;
        bpp = 32;
        cpp = 4;
        break;

    case DRM_FORMAT_ABGR8888:
        RGBAmasks[0] = 0x000000FF;
        RGBAmasks[1] = 0x0000FF00;
        RGBAmasks[2] = 0x00FF0000;
        RGBAmasks[3] = 0xFF000000;
        bpp = 32;
        cpp = 4;
        break;

    case DRM_FORMAT_RGBA8888:
        RGBAmasks[0] = 0xFF000000;
        RGBAmasks[1] = 0x00FF0000;
        RGBAmasks[2] = 0x0000FF00;
        RGBAmasks[3] = 0x000000FF;
        bpp = 32;
        cpp = 4;
        break;

    case DRM_FORMAT_BGRA8888:
        RGBAmasks[0] = 0x0000FF00;
        RGBAmasks[1] = 0x00FF0000;
        RGBAmasks[2] = 0xFF000000;
        RGBAmasks[3] = 0x000000FF;
        bpp = 32;
        cpp = 4;
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

    if (ret_cpp)
        *ret_cpp = cpp;

    return bpp;
}

static GAL_Surface* create_surface_from_buffer (_THIS,
        DrmSurfaceBuffer* surface_buffer, int depth, Uint32 *RGBAmasks)
{
    DrmVideoData* vdata = this->hidden;
    GAL_Surface* surface = NULL;
    size_t pixels_size = surface_buffer->height * surface_buffer->pitch;

    if (surface_buffer->size < surface_buffer->offset + pixels_size) {
        _WRN_PRINTF ("NEWGAL>DRM: the buffer size is not large enought!\n");
    }

    /* for dumb buffer, already mapped */
    if (surface_buffer->buff == NULL &&
            vdata->driver && vdata->driver_ops->map_buffer) {
        if (!vdata->driver_ops->map_buffer(vdata->driver, surface_buffer, 0)) {
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
    surface->w = surface_buffer->width;
    surface->h = surface_buffer->height;
    surface->pitch = surface_buffer->pitch;
    surface->pixels_off = 0;
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

static DrmSurfaceBuffer *drm_create_dumb_buffer(DrmVideoData* vdata,
        uint32_t drm_format, uint32_t hdr_size,
        int width, int height);

static void drm_destroy_dumb_buffer(DrmVideoData* vdata,
        DrmSurfaceBuffer *surface_buffer);
/*
 * The following helpers derived from DRM HOWTO by David Herrmann.
 *
 * drm_prepare
 * drm_find_crtc
 * drm_setup_connector
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

    if (vdata->dbl_buff) {
        if (vdata->real_screen) {
            GAL_FreeSurface (vdata->real_screen);
        }

#if IS_SHAREDFB_SCHEMA_PROCS
        if (mgIsServer && vdata->shadow_screen) {
            shm_unlink (SHMNAME_SHADOW_SCREEN_BUFFER);
        }
#endif
    }

#ifdef _MGSCHEMA_COMPOSITING
    if (vdata->cursor_buff) {
        drm_destroy_dumb_buffer (vdata, vdata->cursor_buff);
        vdata->cursor_buff = NULL;
    }
#endif  /* _MGSCHEMA_COMPOSITING */

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
    DrmDriverOps* (*get_exdrv) (const char*, int, int*);
    char* error;
    int version = 0;
    DrmDriverOps* ops;

#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
#endif
        filename = getenv ("MG_GAL_DRM_DRIVER");
        if (filename == NULL) {
            memset (buff, 0, sizeof (buff));
            if (GetMgEtcValue ("drm", "exdriver", buff, LEN_SO_NAME) < 0) {
                vdata->ex_driver = strdup("none");
                return NULL;
            }

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

    if (strcmp(filename, "none") == 0)
        return NULL;

    vdata->exdrv_handle = dlopen (filename, RTLD_LAZY);
    if (!vdata->exdrv_handle) {
        _ERR_PRINTF("NEWGAL>DRM: failed to open external DRM driver: %s (%s)\n",
                filename, dlerror());
        return NULL;
    }

    dlerror();    /* Clear any existing error */
    get_exdrv = dlsym (vdata->exdrv_handle, "__drm_ex_driver_get");
    error = dlerror();
    if (error) {
        _ERR_PRINTF("NEWGAL>DRM: failed to get symbol: %s\n", error);
        dlclose (vdata->exdrv_handle);
        vdata->exdrv_handle = NULL;
        return NULL;
    }

    /* check version here */
    ops = get_exdrv (driver_name, device_fd, &version);
    if (version != DRM_DRIVER_VERSION) {
        _ERR_PRINTF("NEWGAL>DRM: version does not match: required(%d), got(%d)\n",
                DRM_DRIVER_VERSION, version);
        dlclose (vdata->exdrv_handle);
        vdata->exdrv_handle = NULL;
        return NULL;
    }

    return ops;
}

#ifdef _MGRM_PROCESSES
int __drm_auth_client (int cli, uint32_t magic)
{
    GAL_VideoDevice *this = __mg_current_video;
    DrmVideoData* vdata;

    assert (sizeof(uint32_t) == sizeof(drm_magic_t));

    if (!this || this->VideoInit != DRM_VideoInit) {
        return -1;
    }

    vdata = this->hidden;
    if (drmAuthMagic(vdata->dev_fd, (drm_magic_t)magic)) {
        _WRN_PRINTF("NEWGAL>DRM: failed to call drmAuthMagic (%u): %m\n", magic);
        return -1;
    }

    _DBG_PRINTF("Client (%d) authenticated with magic (%u)\n", cli, magic);
    return 0;
}
#endif  /* _MGRM_PROCESSES */

static int open_drm_device(GAL_VideoDevice *device)
{
    char *driver_name;
    int fd;

    driver_name = find_driver_for_device(device->hidden->dev_name);

    _DBG_PRINTF("Try to load DRM driver: %s\n", driver_name);

    fd = drmOpen(driver_name, NULL);
    if (fd < 0) {
        _ERR_PRINTF("NEWGAL>DRM: drmOpen failed\n");
        free(driver_name);
        return -errno;
    }

#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
        if (drmSetMaster(fd)) {
            _ERR_PRINTF("NEWGAL>DRM: failed to call drmSetMaster: %m\n");
            _ERR_PRINTF("    You need to run `mginit` as root.\n");
            return -EACCES;
        }
    }
    else {
        drm_magic_t magic;
        REQUEST req;
        int auth_result;

        if (drmGetMagic(fd, &magic)) {
            _DBG_PRINTF("failed to call drmGetMagic: %m\n");
            return -errno;
        }

        req.id = REQID_AUTHCLIENT;
        req.data = &magic;
        req.len_data = sizeof (magic);
        if ((ClientRequest (&req, &auth_result, sizeof(int)) < 0)) {
            _ERR_PRINTF("NEWGAL>DRM: failed to call ClientRequest: %m\n");
            return -ECOMM;
        }

        if (auth_result) {
            _ERR_PRINTF("NEWGAL>DRM: failed to authenticate: %d\n", auth_result);
            return -EACCES;
        }
    }
#endif /* defined _MGRM_PROCESSES */

    device->hidden->driver_ops = load_external_driver (device->hidden,
            driver_name, fd);
    free (driver_name);

    /* get capabilities */
    {
        uint64_t has_dumb, cursor_width, cursor_height;

        /* check whether supports dumb buffer */
        if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 ||
                !has_dumb) {
            device->hidden->cap_dumb = 0;
        }
        else {
            device->hidden->cap_dumb = 1;
        }

        if (drmGetCap(fd, DRM_CAP_CURSOR_WIDTH, &cursor_width) < 0 ||
                cursor_width == 0) {
            device->hidden->cap_cursor_width = 0;
        }
        else {
            device->hidden->cap_cursor_width = cursor_width;
        }

        if (drmGetCap(fd, DRM_CAP_CURSOR_HEIGHT, &cursor_height) < 0 ||
                cursor_height == 0) {
            device->hidden->cap_cursor_height = 0;
        }
        else {
            device->hidden->cap_cursor_height = cursor_height;
        }

        _DBG_PRINTF ("DRM cursor cap: w(%d), h(%d)\n",
                (int)cursor_width, (int)cursor_height);
    }

    device->hidden->dev_fd = fd;
    if (device->hidden->driver_ops) {
        device->hidden->driver = device->hidden->driver_ops->create_driver(fd);
    }

    if (device->hidden->driver == NULL) {
        _WRN_PRINTF("failed to create DRM driver\n");
        device->hidden->driver_ops = NULL;

        /* check whether supports dumb buffer */
        if (!device->hidden->cap_dumb) {
            _ERR_PRINTF("NEWGAL>DRM: the DRM device '%s' does not support "
                    "dumb buffers\n",
                    device->hidden->dev_name);
            close(fd);
            device->hidden->dev_fd = -1;
            return -EOPNOTSUPP;
        }
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

#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif /* defined _MGRM_PROCESSES */
    {
        char tmp [8];
        if (GetMgEtcValue ("drm", "double_buffering", tmp, 8) < 0) {
            device->hidden->dbl_buff = 0;
        }
        else if (strcasecmp (tmp, "true") == 0 ||
                strcasecmp (tmp, "yes") == 0) {
            device->hidden->dbl_buff = 1;
        }
    }
#ifdef _MGRM_PROCESSES
    else {
        device->hidden->dbl_buff = SHAREDRES_VIDEO_DBL_BUFF;
    }
#endif /* defined _MGRM_PROCESSES */

    /* override double buffering */
#if IS_COMPOSITING_SCHEMA
    /* force to use double buffering to compositing schema */
    if (mgIsServer) {
        device->hidden->dbl_buff = 1;
#if 0   /* test code */
        device->UpdateRects = DRM_UpdateRects_Async;
        device->SyncUpdate = DRM_SyncUpdate_Async;
        pthread_mutex_init (&device->hidden->update_lock, NULL);
        sem_init (&device->hidden->sem_update, 0, 0);
        pthread_create (&device->hidden->update_th, NULL,
                        task_do_update, device);
#endif   /* test code */
    }
    else {
        device->hidden->dbl_buff = 0;
#if 0   /* test code */
        device->hidden->update_th = 0;
#endif  /* test code */
    }
#endif  /* IS_COMPOSITING_SCHEMA */

    if (device->hidden->dbl_buff) {
        device->UpdateRects = DRM_UpdateRects;
        device->SyncUpdate = DRM_SyncUpdate;
    }

    device->VideoInit = DRM_VideoInit;
    device->ListModes = DRM_ListModes;
    device->SetColors = DRM_SetColors;
    device->VideoQuit = DRM_VideoQuit;
    device->AllocHWSurface = DRM_AllocHWSurface_Accl;
    device->FreeHWSurface = DRM_FreeHWSurface_Accl;

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
    }

    if (device->SetVideoMode == NULL)
        device->SetVideoMode = DRM_SetVideoMode;

#if IS_COMPOSITING_SCHEMA
    if (device->hidden->driver) {
        device->AllocSharedHWSurface = DRM_AllocSharedHWSurface;
        device->FreeSharedHWSurface = DRM_FreeSharedHWSurface;
        device->AttachSharedHWSurface = DRM_AttachSharedHWSurface;
        device->DettachSharedHWSurface = DRM_DettachSharedHWSurface;
    }
#endif /* IS_COMPOSITING_SCHEMA */

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

#if 0   /* test code */
#ifdef _MBSCHEMA_COMPOSITING
    if (this->hidden->update_th) {
        /* send cancel request */
        pthread_cancel (this->hidden->update_th);
        pthread_join (this->hidden->update_th, NULL);
    }
#endif  /* _MBSCHEMA_COMPOSITING */
#endif  /* test code */

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
        _DBG_PRINTF ("drmModeAddFB2 failed: "
                "handle(%u), pitch(%u), offset(%u), w(%u), h(%u), f(%x): %m\n",
                handle, pitch, offset, width, height, drm_format);
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

#if IS_COMPOSITING_SCHEMA
/* check to see whether the plane is ok for cursor */
static uint32_t check_plane_suitable_for_cursor(DrmVideoData* vdata,
        drmModePlanePtr plane, uint32_t fourcc)
{
    int i, j, k;
    uint32_t plane_id = 0;

    for (i = 0; i < plane->count_formats; i++) {
        _DBG_PRINTF ("plane format (%d): %c%c%c%c\n", i,
                plane->formats[i], plane->formats[i] >> 8,
                plane->formats[i] >> 16, plane->formats[i] >> 24);
        if (plane->formats[i] != fourcc)
            continue;

        drmModeObjectPropertiesPtr props =
            drmModeObjectGetProperties(vdata->dev_fd, plane->plane_id,
                    DRM_MODE_OBJECT_PLANE);
        if (!props)
            continue;

        for (j = 0; j < props->count_props; j++) {
            drmModePropertyPtr prop =
                drmModeGetProperty (vdata->dev_fd, props->props[j]);

            _DBG_PRINTF ("prop name: %s\n", prop->name);
            if (strcmp(prop->name, "type") == 0 &&
                    drm_property_type_is(prop, DRM_MODE_PROP_ENUM)) {
                for (k = 0; k < prop->count_enums; k++) {
                    _DBG_PRINTF ("prop enum value (%d): %d\n",
                            k, (int)prop->enums[k].value);
                    if (prop->enums[k].value == DRM_PLANE_TYPE_CURSOR) {
                        plane_id = plane->plane_id;
                        break;
                    }
                }
            }

            drmModeFreeProperty(prop);
            if (plane_id)
                break;
        }

        drmModeFreeObjectProperties (props);
        if (plane_id)
            break;
    }

    return plane_id;
}

static int drm_setup_cursor_plane (DrmVideoData* vdata, uint32_t drm_format,
        uint32_t width, uint32_t height)
{
    uint32_t plane_id = 0;
    uint32_t handles[4], pitches[4], offsets[4];
    uint32_t plane_flags = 0;
    int i, ret = -1;
    drmModePlaneResPtr plane_res = NULL;

    vdata->cursor_plane_id = 0;

    if (vdata->cap_dumb &&
            vdata->cap_cursor_width >= CURSORWIDTH &&
            vdata->cap_cursor_height >= CURSORHEIGHT) {

        vdata->cursor_buff = drm_create_dumb_buffer (vdata, drm_format, 0,
                width, height);
        if (vdata->cursor_buff == NULL)
            return ret;

        ret = drmModeSetCursor2 (vdata->dev_fd, vdata->saved_info->crtc,
                vdata->cursor_buff->handle, width, height, 0, 0);
        if (ret == 0) {
            // Supported
            memset (vdata->cursor_buff->buff, 0, vdata->cursor_buff->size);
            return 0;
        }
    }

#if 0
    int pipe = -1;
    drmModeRes *res;

    res = drmModeGetResources(vdata->dev_fd);
    if (!res) return ret;
    for (i = 0; i < res->count_crtcs; i++) {
        if (vdata->saved_info->crtc == res->crtcs[i]) {
            pipe = i;
            break;
        }
    }
    drmModeFreeResources(res);
    assert (pipe >= 0);
#endif

    plane_res = drmModeGetPlaneResources (vdata->dev_fd);
    if (!plane_res) {
        _WRN_PRINTF( "drmModeGetPlaneResources failed: %m\n");
        return -1;
    }

    for (i = 0; i < plane_res->count_planes; i ++) {
        drmModePlanePtr plane =
            drmModeGetPlane(vdata->dev_fd, plane_res->planes[i]);
        if (plane == NULL) {
            _WRN_PRINTF( "drmModeGetPlane failed: %m\n");
            break;
        }

        if ((plane->crtc_id == 0 ||
                    plane->crtc_id == vdata->saved_info->crtc)) {
            plane_id = check_plane_suitable_for_cursor(vdata, plane,
                    drm_format);
        }
        else {
            _WRN_PRINTF( "possible_crtcs: %x, crtc_id: %u\n",
                    plane->possible_crtcs, plane->crtc_id);
        }

        drmModeFreePlane(plane);
        if (plane_id)
            break;
    }

    drmModeFreePlaneResources (plane_res);

    if (plane_id == 0) {
        _WRN_PRINTF( "no available plane for cursor in format %c%c%c%c\n",
                drm_format, drm_format >> 8, drm_format >> 16, drm_format >> 24);
        goto error;
    }

    handles[0] = vdata->cursor_buff->handle;
    pitches[0] = vdata->cursor_buff->pitch;
    offsets[0] = vdata->cursor_buff->offset;

    ret = drmModeAddFB2(vdata->dev_fd, width, height, drm_format,
            handles, pitches, offsets, &vdata->cursor_buff->fb_id,
            plane_flags);
    if (ret) {
        _WRN_PRINTF ("drmModeAddFB2 failed: %m\n");
        goto error;
    }

    /* note src coords (last 4 args) are in Q16 format */
    if (drmModeSetPlane(vdata->dev_fd, plane_id,
                vdata->saved_info->crtc, vdata->cursor_buff->fb_id,
                plane_flags, 0, 0, width, height,
                0, 0, width << 16, height << 16)) {
        _WRN_PRINTF ("failed to enable cursor plane: %m\n");
        goto error;
    }

    vdata->cursor_plane_id = plane_id;
    return 0;

error:
    if (vdata->cursor_buff) {
        drm_destroy_dumb_buffer (vdata, vdata->cursor_buff);
        vdata->cursor_buff = NULL;
    }

    return ret;
}
#endif  /* IS_COMPOSITING_SCHEMA */

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
        uint32_t drm_format, uint32_t hdr_size, int width, int height)
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
        _ERR_PRINTF("NEWGAL>DRM: Failed to create dumb buffer (%d): %m\n", errno);
        return NULL;
    }

    if ((surface_buffer = malloc (sizeof (DrmSurfaceBuffer))) == NULL) {
        _ERR_PRINTF("NEWGAL>DRM: Failed to allocate memory\n");
        goto err_destroy;
    }

    surface_buffer->handle = creq.handle;
    surface_buffer->prime_fd = -1;
    surface_buffer->name = 0;
    surface_buffer->fb_id = 0;
    surface_buffer->drm_format = drm_format;
    surface_buffer->width = creq.width;
    surface_buffer->height = creq.height - nr_header_lines;
    surface_buffer->pitch = creq.pitch;
    surface_buffer->bpp = bpp;
    surface_buffer->cpp = cpp;
    surface_buffer->offset = creq.pitch * nr_header_lines;
    surface_buffer->size = creq.size;

    _DBG_PRINTF ("Surface buffer info: handle(%u), w(%u), h(%u), "
            "pitch(%u), size(%lu), offset(%lu)\n",
            surface_buffer->handle,
            surface_buffer->width, surface_buffer->height,
            surface_buffer->pitch,
            surface_buffer->size, surface_buffer->offset);

    /* prepare buffer for memory mapping */
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = surface_buffer->handle;
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        _ERR_PRINTF("NEWGAL>DRM: cannot map dumb buffer (%d): %m\n", errno);
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
        _ERR_PRINTF("NEWGAL>DRM: Failed to allocate memory\n");
        goto error;
    }

    surface_buffer->handle = handle;
    surface_buffer->prime_fd = -1;
    surface_buffer->name = 0;
    surface_buffer->fb_id = 0;
    surface_buffer->size = size;

    /* prepare buffer for memory mapping */
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = surface_buffer->handle;
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        _ERR_PRINTF("NEWGAL>DRM: cannot map dumb buffer (%u): %m\n", mreq.handle);
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
    DrmSurfaceBuffer *surface_buffer = NULL;

    /* open named buffer */
    memset(&oreq, 0, sizeof(oreq));
    oreq.name = name;
    ret = drmIoctl(vdata->dev_fd, DRM_IOCTL_GEM_OPEN, &oreq);
    if (ret < 0) {
        _ERR_PRINTF("NEWGAL>DRM: Failed to open named buffer (%u): %m\n", name);
        return NULL;
    }

    surface_buffer = drm_create_dumb_buffer_from_handle (vdata,
            oreq.handle, oreq.size);

    if (surface_buffer == NULL) {
        struct drm_gem_close creq;
        memset(&creq, 0, sizeof(creq));
        creq.handle = oreq.handle;
        drmIoctl(vdata->dev_fd, DRM_IOCTL_GEM_CLOSE, &creq);
    }

    surface_buffer->handle = oreq.handle;
    surface_buffer->prime_fd = -1;
    surface_buffer->name = name;
    surface_buffer->fb_id = 0;
    surface_buffer->size = oreq.size;

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

    surface_buffer->handle = handle;
    surface_buffer->prime_fd = prime_fd;
    surface_buffer->name = 0;
    surface_buffer->fb_id = 0;
    surface_buffer->size = size;

    return surface_buffer;
}

static void drm_destroy_dumb_buffer(DrmVideoData* vdata,
        DrmSurfaceBuffer *surface_buffer)
{
    if (surface_buffer->fb_id) {
        drmModeRmFB (vdata->dev_fd, surface_buffer->fb_id);
    }

    if (surface_buffer->buff)
        munmap (surface_buffer->buff, surface_buffer->size);

    assert (surface_buffer->handle);

    if (surface_buffer->prime_fd >= 0)
        close (surface_buffer->prime_fd);

    if (surface_buffer->name) {
        struct drm_gem_close creq;
        memset(&creq, 0, sizeof(creq));
        creq.handle = surface_buffer->handle;
        drmIoctl(vdata->dev_fd, DRM_IOCTL_GEM_CLOSE, &creq);
    }
    else {
        struct drm_mode_destroy_dumb dreq;

        memset (&dreq, 0, sizeof(dreq));
        dreq.handle = surface_buffer->handle;
        drmIoctl(vdata->dev_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    }

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
    Uint32 RGBAmasks[4];
    int ret;

    drm_format = get_drm_format_from_etc(&bpp);
    if (drm_format == 0) {
        return NULL;
    }

    if (translate_drm_format(drm_format, RGBAmasks, NULL) == 0) {
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
                drm_format, 0, info->width, info->height);
        vdata->driver_ops->map_buffer(vdata->driver, real_buffer, 1);
    }
    else {
        real_buffer = drm_create_dumb_buffer (vdata, drm_format, 0,
                info->width, info->height);
    }

    if (real_buffer == NULL || real_buffer->buff == NULL) {
        _ERR_PRINTF("NEWGAL>DRM: "
                "failed to create and map buffer for real screen\n");
        return NULL;
    }

    vdata->saved_info = info;

    /* setup real_buffer as the scanout buffer */
    ret = drm_setup_scanout_buffer (vdata, real_buffer->handle,
            real_buffer->drm_format, real_buffer->width, real_buffer->height,
            real_buffer->pitch, real_buffer->offset);
    if (ret) {
        _ERR_PRINTF("NEWGAL>DRM: cannot setup scanout buffer\n");
        goto error;
    }

    vdata->real_screen = create_surface_from_buffer (this, real_buffer,
            bpp, RGBAmasks);
    if (vdata->real_screen == NULL)
        goto error;

    if (vdata->dbl_buff) {
        uint32_t hdr_size = sizeof (GAL_ShadowSurfaceHeader);
        GAL_ShadowSurfaceHeader* hdr;

        shadow_buffer = NULL;
        if (vdata->driver) {
            assert (vdata->driver_ops->create_buffer);
            shadow_buffer = vdata->driver_ops->create_buffer(vdata->driver,
                    drm_format, hdr_size,
                    info->width, info->height);
            vdata->driver_ops->map_buffer(vdata->driver, shadow_buffer, 0);
        }

        if (shadow_buffer == NULL || shadow_buffer->buff == NULL) {
            // do not use dumb buffer for shadow screen.
#if IS_SHAREDFB_SCHEMA_PROCS
            vdata->shadow_screen = GAL_CreateRGBSurfaceInShm (
                    SHMNAME_SHADOW_SCREEN_BUFFER, TRUE, 0666, hdr_size,
                    real_buffer->width, real_buffer->height, real_buffer->bpp,
                    RGBAmasks[0], RGBAmasks[1], RGBAmasks[2], RGBAmasks[3]);
            /* initialize the header */
            hdr = (GAL_ShadowSurfaceHeader *)
                ((uint8_t*)vdata->shadow_screen->pixels -
                 vdata->shadow_screen->pixels_off);
            SetRectEmpty (&hdr->dirty_rc);
#else
            vdata->shadow_screen = GAL_CreateRGBSurface (GAL_SWSURFACE,
                    real_buffer->width, real_buffer->height, real_buffer->bpp,
                    RGBAmasks[0], RGBAmasks[1], RGBAmasks[2], RGBAmasks[3]);

            SetRectEmpty (&vdata->dirty_rc);
#endif
        }
        else {
            vdata->shadow_screen = create_surface_from_buffer (this,
                    shadow_buffer, bpp, RGBAmasks);

            hdr = (GAL_ShadowSurfaceHeader*)shadow_buffer->buff;
            SetRectEmpty (&hdr->dirty_rc);
        }

        if (vdata->shadow_screen == NULL) {
            _ERR_PRINTF("NEWGAL>DRM: faile to create shadow screen\n");
            goto error;
        }

        GAL_SetClipRect (vdata->real_screen, NULL);
        GAL_SetColorKey (vdata->shadow_screen, 0, 0);
        GAL_SetAlpha (vdata->shadow_screen, 0, 0);
    }

#if IS_COMPOSITING_SCHEMA
    drm_setup_cursor_plane (vdata,
            DRM_FORMAT_ARGB8888, CURSORWIDTH, CURSORHEIGHT);

    if (vdata->cursor_buff && !vdata->cursor_plane_id) {
        _DBG_PRINTF("We are using legacy hardware cursor\n");
        this->SetCursor = DRM_SetCursor;
        this->MoveCursor = DRM_MoveCursor;
        this->AllocDumbSurface = DRM_AllocDumbSurface;
        this->FreeDumbSurface = DRM_FreeDumbSurface;
    }
    else if (vdata->cursor_plane_id) {
        _DBG_PRINTF("We are using plane for cursor\n");
        this->SetCursor = DRM_SetCursor_Plane;
        this->MoveCursor = DRM_MoveCursor_Plane;
        this->AllocDumbSurface = NULL;
        this->FreeDumbSurface = NULL;
    }
    else {
        _WRN_PRINTF("We are using software-simulated hardware cursor\n");
        this->SetCursor = DRM_SetCursor_SW;
        this->MoveCursor = DRM_MoveCursor_SW;
        this->AllocDumbSurface = NULL;
        this->FreeDumbSurface = NULL;
    }
    this->info.hw_cursor = 1;
#endif /* IS_COMPOSITING_SCHEMA */

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

#if IS_COMPOSITING_SCHEMA
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
            surface->w, surface->h);
    if (surface_buffer == NULL) {
        return -1;
    }

    surface->pitch = surface_buffer->pitch;
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
#endif  /* IS_COMPOSITING_SCHEMA */

#if IS_COMPOSITING_SCHEMA
static int DRM_AllocSharedHWSurface(_THIS, GAL_Surface *surface,
            size_t* map_size, off_t* pixels_off, Uint32 rw_modes)
{
    DrmVideoData* vdata = this->hidden;
    uint32_t drm_format;
    size_t hdr_size;
    DrmSurfaceBuffer* surface_buffer;

    drm_format = translate_gal_format(surface->format);
    if (drm_format == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported pixel format, "
                "RGBA masks (0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
                surface->format->Rmask, surface->format->Gmask,
                surface->format->Bmask, surface->format->Amask);
        return -1;
    }

    hdr_size = sizeof (GAL_SharedSurfaceHeader);
    if (vdata->driver_ops) {
        surface_buffer = vdata->driver_ops->create_buffer(vdata->driver,
                drm_format, hdr_size, surface->w, surface->h);
        vdata->driver_ops->map_buffer(vdata->driver, surface_buffer, 0);
    }
    else {
        surface_buffer = drm_create_dumb_buffer(vdata,
                drm_format, hdr_size, surface->w, surface->h);
    }

    if (surface_buffer == NULL || surface_buffer->buff == NULL) {
        _ERR_PRINTF("NEWGAL>DRM: Failed to create shared hardware surface: "
                "size (%d x %d)\n", surface->w, surface->h);
        return -1;
    }

    /* get the prime fd */
    if (drmPrimeHandleToFD (vdata->dev_fd, surface_buffer->handle,
                DRM_RDWR | DRM_CLOEXEC, &surface_buffer->prime_fd)) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot get prime fd: %m\n");
        goto error;
    }

    *map_size = surface_buffer->size;
    *pixels_off = surface_buffer->offset;

    /* go for success */
    /* the caller will set the pixels and flags
    surface->pixels = surface_buffer->buff + surface_buffer->offset;
    surface->flags |= GAL_HWSURFACE;
    */
    surface->pitch = surface_buffer->pitch;
    surface->shared_header = (GAL_SharedSurfaceHeader*)surface_buffer->buff;
    surface->hwdata = (struct private_hwdata *)surface_buffer;

    return surface_buffer->prime_fd;

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
        if (vdata->driver_ops) {
            vdata->driver_ops->unmap_buffer(vdata->driver, surface_buffer);
            vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);
        }
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

    if (lseek(prime_fd, 0, SEEK_END) != mapsize)
        _WRN_PRINTF("lseek failed on prime fd: %d (%lu)\n", prime_fd, mapsize);

    if (vdata->driver_ops && vdata->driver_ops->create_buffer_from_prime_fd) {
        surface_buffer = vdata->driver_ops->create_buffer_from_prime_fd(
                vdata->driver, prime_fd, mapsize);
        if (surface_buffer == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: failed to create buffer from prime fd: "
                    "%d!\n", prime_fd);
            goto error;
        }

        if (!vdata->driver_ops->map_buffer(vdata->driver, surface_buffer, 0)) {
            _ERR_PRINTF ("NEWGAL>DRM: cannot map hardware buffer: %m\n");
            goto error;
        }
    }
    else {
        /* for shared dumb buffer, the caller uses mmap directly */
        surface_buffer = drm_create_dumb_buffer_from_prime_fd(
                vdata, prime_fd, mapsize);
    }

    if (surface_buffer) {
        surface->shared_header = (GAL_SharedSurfaceHeader*)surface_buffer->buff;
        surface->hwdata = (struct private_hwdata *)surface_buffer;
        retval = 0;
    }

    return retval;

error:
    if (surface_buffer) {
        if (vdata->driver_ops &&
                vdata->driver_ops->create_buffer_from_prime_fd) {
            vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);
        }
        else
            drm_destroy_dumb_buffer(vdata, surface_buffer);
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
            if (surface_buffer->buff)
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
    int retval;
    DrmSurfaceBuffer* surface_buffer = NULL;

    if (this->hidden->cursor == surface &&
            this->hidden->hot_x == hot_x &&
            this->hidden->hot_y == hot_y) {
        return 0;
    }

    this->hidden->cursor = surface;
    this->hidden->hot_x = hot_x;
    this->hidden->hot_y = hot_y;

    if (surface) {
        surface_buffer = (DrmSurfaceBuffer*)surface->hwdata;
    }
    else {
        surface_buffer = this->hidden->cursor_buff;
    }

    assert (surface_buffer);

    retval = drmModeSetCursor2 (this->hidden->dev_fd,
            this->hidden->saved_info->crtc,
            surface_buffer->handle,
            surface_buffer->width, surface_buffer->height, hot_x, hot_y);

    if (retval) {
        _WRN_PRINTF ("failed to call drmModeSetCursor2(%u,%u,%d,%d,%d,%d):%m\n",
                this->hidden->saved_info->crtc, surface_buffer->handle,
                surface_buffer->width, surface_buffer->height, hot_x, hot_y);
    }

    return retval;
}

static int DRM_MoveCursor(_THIS, int x, int y)
{
    int retval = -1;

    if (this->hidden->csr_x != x ||
             this->hidden->csr_y != y) {

        this->hidden->csr_x = x;
        this->hidden->csr_y = y;

        retval = drmModeMoveCursor (this->hidden->dev_fd,
                this->hidden->saved_info->crtc, x, y);
        if (retval) {
            _WRN_PRINTF ("failed to call drmModeMoveCursor: %m\n");
        }
    }

    return retval;
}

static int DRM_SetCursor_Plane(_THIS, GAL_Surface *surface, int hot_x, int hot_y)
{
    int retval = -1;
    DrmVideoData* vdata = this->hidden;

    if (vdata->cursor == surface &&
            vdata->hot_x == hot_x &&
            vdata->hot_y == hot_y) {
        return 0;
    }

    vdata->cursor = surface;
    if (surface) {
        uint8_t *src, *dst;
        uint32_t i;
        uint32_t height, pitch;

        assert (vdata->cursor_buff);
        src = surface->pixels;
        dst = vdata->cursor_buff->buff + vdata->cursor_buff->offset;

        height = MIN(surface->h, vdata->cursor_buff->height);
        pitch = MIN(surface->pitch, vdata->cursor_buff->pitch);
        for (i = 0; i < height; i++) {
            memcpy (dst, src, pitch);
            dst += vdata->cursor_buff->pitch;
            src += surface->pitch;
        }

    }
    else {
        uint8_t *dst;
        dst = vdata->cursor_buff->buff + vdata->cursor_buff->offset;
        memset (dst, 0, vdata->cursor_buff->pitch * vdata->cursor_buff->height);
    }

    if (vdata->hot_x != hot_x || vdata->hot_y != hot_y) {
        vdata->hot_x = hot_x;
        vdata->hot_y = hot_y;

        retval = drmModeSetPlane(vdata->dev_fd, vdata->cursor_plane_id,
                    vdata->saved_info->crtc, vdata->cursor_buff->fb_id, 0,
                    vdata->csr_x - vdata->hot_x,
                    vdata->csr_y - vdata->hot_y,
                    vdata->cursor_buff->width,
                    vdata->cursor_buff->height,
                    0, 0,
                    vdata->cursor_buff->width << 16,
                    vdata->cursor_buff->height << 16);
    }

    return retval;
}

static int DRM_MoveCursor_Plane(_THIS, int x, int y)
{
    int retval = -1;
    DrmVideoData* vdata = this->hidden;

    if (vdata->csr_x != x || vdata->csr_y != y) {
        vdata->csr_x = x;
        vdata->csr_y = y;

        retval = drmModeSetPlane(vdata->dev_fd, vdata->cursor_plane_id,
                    vdata->saved_info->crtc, vdata->cursor_buff->fb_id, 0,
                    vdata->csr_x - vdata->hot_x,
                    vdata->csr_y - vdata->hot_y,
                    vdata->cursor_buff->width,
                    vdata->cursor_buff->height,
                    0, 0,
                    vdata->cursor_buff->width << 16,
                    vdata->cursor_buff->height << 16);
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
        this->SyncUpdate (this);

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
            if (drmIoctl(vdata->dev_fd, DRM_IOCTL_GEM_FLINK, &flink)) {
                _ERR_PRINTF ("NEWGAL>DRM: failed to flink real screen\n");
                return -1;
            }

            _DBG_PRINTF ("flink name of real screen: %u\n", flink.name);
            vdata->real_name = flink.name;
        }

        info->flags = vdata->real_screen->flags;
        info->width = surface_buffer->width;
        info->height = surface_buffer->height;
        info->pitch = surface_buffer->pitch;
        info->name = vdata->real_name;

        info->drm_format = surface_buffer->drm_format;
        info->size = surface_buffer->size;
        info->offset = surface_buffer->offset;
    }
    else if (strcmp (name, SYSSF_SHADOW_SCREEN) == 0 && vdata->shadow_screen) {
        if ((vdata->shadow_screen->flags & GAL_HWSURFACE) == GAL_SWSURFACE) {
            surface_buffer = (DrmSurfaceBuffer *)vdata->real_screen->hwdata;

            vdata->shadow_name = 0;

            info->drm_format = surface_buffer->drm_format;
            info->size = (size_t)vdata->shadow_screen->hwdata;
            info->offset = vdata->shadow_screen->pixels_off;
        }
        else {
            surface_buffer = (DrmSurfaceBuffer *)vdata->shadow_screen->hwdata;
            if (surface_buffer && vdata->shadow_name == 0) {
                struct drm_gem_flink flink;

                memset (&flink, 0, sizeof (flink));
                flink.handle = surface_buffer->handle;
                if (drmIoctl (vdata->dev_fd, DRM_IOCTL_GEM_FLINK, &flink)) {
                    _ERR_PRINTF ("NEWGAL>DRM: failed to flink shadow screen\n");
                    return -1;
                }

                _DBG_PRINTF ("flink name of shadow screen: %u\n", flink.name);
                vdata->shadow_name = flink.name;
            }

            if (surface_buffer) {
                info->drm_format = surface_buffer->drm_format;
                info->size = surface_buffer->size;
                info->offset = surface_buffer->offset;
            }
        }

        info->flags = vdata->shadow_screen->flags;
        info->width = surface_buffer->width;
        info->height = surface_buffer->height;
        info->pitch = surface_buffer->pitch;
        info->name = vdata->shadow_name;
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

    if (info.name) {
        vdata->shadow_screen = __drm_create_surface_from_name (this, info.name,
                info.drm_format, info.offset,
                info.width, info.height, info.pitch);

        if (vdata->shadow_screen == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: failed to create shadow screen surface "
                    "from name: %u!\n", info.name);
            goto error;
        }
    }
    else {
        vdata->shadow_screen = GAL_CreateRGBSurfaceInShm (
                SHMNAME_SHADOW_SCREEN_BUFFER, FALSE, 0666,
                sizeof (GAL_ShadowSurfaceHeader),
                info.width, info.height,
                vdata->real_screen->format->BitsPerPixel,
                vdata->real_screen->format->Rmask,
                vdata->real_screen->format->Gmask,
                vdata->real_screen->format->Bmask,
                vdata->real_screen->format->Amask);

        if (vdata->shadow_screen == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: failed to create shadow screen surface "
                    "from named shared memory: %s!\n",
                    SHMNAME_SHADOW_SCREEN_BUFFER);
            goto error;
        }
    }

    GAL_SetClipRect (vdata->real_screen, NULL);
    GAL_SetColorKey (vdata->shadow_screen, 0, 0);
    GAL_SetAlpha (vdata->shadow_screen, 0, 0);
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

    if (!vdata->driver)
        return -1;

    drm_format = translate_gal_format(surface->format);
    if (drm_format == 0) {
        _ERR_PRINTF("NEWGAL>DRM: not supported pixel format, "
                "RGBA masks (0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
                surface->format->Rmask, surface->format->Gmask,
                surface->format->Bmask, surface->format->Amask);
        return -1;
    }

    surface_buffer = vdata->driver_ops->create_buffer(vdata->driver, drm_format,
            0, surface->w, surface->h);
    if (surface_buffer == NULL) {
        return -1;
    }

    if (!vdata->driver_ops->map_buffer(vdata->driver, surface_buffer, 0)) {
        _ERR_PRINTF ("NEWGAL>DRM: cannot map hardware buffer: %m\n");
        goto error;
    }

    surface->pitch = surface_buffer->pitch;
    surface->pixels = surface_buffer->buff + surface_buffer->offset;
    surface->flags |= GAL_HWSURFACE;
    surface->hwdata = (struct private_hwdata *)surface_buffer;

    _DBG_PRINTF("allocated hardware surface: w(%d), h(%d), pitch(%d), "
            "RGBA masks (0x%08x, 0x%08x, 0x%08x, 0x%08x), pixels: %p\n",
            surface->w, surface->h, surface->pitch,
            surface->format->Rmask, surface->format->Gmask,
            surface->format->Bmask, surface->format->Amask,
            surface->pixels);
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

    if (vdata->driver) {
        surface_buffer = (DrmSurfaceBuffer*)surface->hwdata;
        if (surface_buffer) {
            if (surface_buffer->buff)
                vdata->driver_ops->unmap_buffer(vdata->driver, surface_buffer);
            vdata->driver_ops->destroy_buffer(vdata->driver, surface_buffer);
        }
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

    if ((src->flags & GAL_SRCPIXELALPHA) == GAL_SRCPIXELALPHA) {
        return vdata->driver_ops->alpha_pixel_blit(vdata->driver,
            src_buf, src_rc, dst_buf, dst_rc,
            COLOR_BLEND_PD_SRC_OVER);
    }
    else if ((src->flags & GAL_SRCALPHA) == GAL_SRCALPHA &&
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
    if ((src->flags & GAL_SRCPIXELALPHA) == GAL_SRCPIXELALPHA &&
            (vdata->driver_ops->alpha_pixel_blit == NULL)) {
        src->flags &= ~GAL_HWACCEL;
    }
    else if ((src->flags & GAL_SRCALPHA) == GAL_SRCALPHA &&
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
    RECT* dirty_rc;
    RECT bound;

#if IS_SHAREDFB_SCHEMA_PROCS
    GAL_ShadowSurfaceHeader* hdr;
    if (this->hidden->shadow_screen->flags & GAL_HWSURFACE) {
        hdr = (GAL_ShadowSurfaceHeader*)
            ((DrmSurfaceBuffer*)this->hidden->shadow_screen->hwdata)->buff;
    }
    else {
        hdr = (GAL_ShadowSurfaceHeader*)
            ((uint8_t*)this->hidden->shadow_screen->pixels -
            this->hidden->shadow_screen->pixels_off);
    }
    dirty_rc = &hdr->dirty_rc;
#else
    dirty_rc = &this->hidden->dirty_rc;
#endif

    bound = *dirty_rc;
    for (i = 0; i < numrects; i++) {
        RECT rc;

        SetRect (&rc, rects[i].x, rects[i].y,
                rects[i].x + rects[i].w, rects[i].y + rects[i].h);
        if (IsRectEmpty (&bound))
            bound = rc;
        else
            GetBoundRect (&bound, &bound, &rc);
    }

    *dirty_rc = bound;
    bound = GetScreenRect();
    if (!IntersectRect (dirty_rc, dirty_rc, &bound))
        SetRectEmpty (dirty_rc);
}

static BOOL DRM_SyncUpdate (_THIS)
{
    BOOL retval = FALSE;
    RECT* dirty_rc;
    RECT bound;

#if IS_SHAREDFB_SCHEMA_PROCS
    GAL_ShadowSurfaceHeader* hdr;
    if (this->hidden->shadow_screen->flags & GAL_HWSURFACE) {
        hdr = (GAL_ShadowSurfaceHeader*)
            ((DrmSurfaceBuffer*)this->hidden->shadow_screen->hwdata)->buff;
    }
    else {
        hdr = (GAL_ShadowSurfaceHeader*)
            ((uint8_t*)this->hidden->shadow_screen->pixels -
            this->hidden->shadow_screen->pixels_off);
    }
    dirty_rc = &hdr->dirty_rc;
#else
    dirty_rc = &this->hidden->dirty_rc;
#endif

    if (IsRectEmpty (dirty_rc))
        goto ret;

    bound = *dirty_rc;

    if (this->hidden->shadow_screen) {
#if 0   /* test code using memcpy */
        DrmSurfaceBuffer *real_buff, *shadow_buff;
        real_buff = (DrmSurfaceBuffer*)this->hidden->real_screen->hwdata;
        shadow_buff = (DrmSurfaceBuffer*)this->hidden->shadow_screen->hwdata;

        uint32_t i;
        uint8_t *src, *dst;
        size_t count = shadow_buff->cpp * RECTW (bound);

        src = shadow_buff->buff;
        src += shadow_buff->pitch * bound.top + shadow_buff->cpp * bound.left;
        src += shadow_buff->offset;

        dst = real_buff->buff;
        dst += real_buff->pitch * bound.top + real_buff->cpp * bound.left;
        dst += real_buff->offset;

        for (i = 0; i < RECTH (bound); i++) {
            memcpy (dst, src, count);
            src += shadow_buff->pitch;
            dst += shadow_buff->pitch;
        }
#else   /* test code using memcpy */
        GAL_Rect src_rect, dst_rect;
        src_rect.x = bound.left;
        src_rect.y = bound.top;
        src_rect.w = RECTW (bound);
        src_rect.h = RECTH (bound);
        dst_rect = src_rect;

        GAL_BlitSurface (this->hidden->shadow_screen, &src_rect,
                this->hidden->real_screen, &dst_rect);
#endif  /* use blitting */
    }

#ifdef _MGSCHEMA_COMPOSITING
    if (this->hidden->cursor && !this->hidden->cursor_buff) {
        RECT csr_rc, eff_rc;
        csr_rc.left = boxleft (this);
        csr_rc.top = boxtop (this);
        csr_rc.right = csr_rc.left + CURSORWIDTH;
        csr_rc.bottom = csr_rc.top + CURSORHEIGHT;

        if (IntersectRect (&eff_rc, &csr_rc, &bound)) {
            GAL_Rect src_rect, dst_rect;
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

    if (this->hidden->driver && this->hidden->driver_ops->flush_driver) {
        this->hidden->driver_ops->flush_driver(this->hidden->driver);
    }

    {
        drmModeClip clip = { bound.left, bound.top,
            bound.right, bound.bottom };

        drmModeDirtyFB(this->hidden->dev_fd,
                this->hidden->scanout_buff_id, &clip, 1);
    }

    SetRectEmpty (dirty_rc);
ret:
    return retval;
}

#if 0   /* test code */
#ifdef _MGSCHEMA_COMPOSITING
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
        DRM_SyncUpdate (this);
        pthread_mutex_unlock (&this->hidden->update_lock);

    } while (1);

    return NULL;
}

static void DRM_UpdateRects_Async (_THIS, int numrects, GAL_Rect *rects)
{
    pthread_mutex_lock (&this->hidden->update_lock);

    DRM_UpdateRects (this, numrects, rects);

    pthread_mutex_unlock (&this->hidden->update_lock);
    return;
}

static BOOL DRM_SyncUpdate_Async (_THIS)
{
    BOOL rc = FALSE;

    pthread_mutex_lock (&this->hidden->update_lock);

    if (!IsRectEmpty (&this->hidden->dirty_rc)) {
        // signal the update thread to do update
        sem_post (&this->hidden->sem_update);
        rc = TRUE;
    }

    pthread_mutex_unlock (&this->hidden->update_lock);
    return rc;
}
#endif  /* _MGSCHEMA_COMPOSITING */
#endif  /* test code */

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
            info->prime_fd = surface_buffer->prime_fd;
            info->name = surface_buffer->name;
            info->fb_id = surface_buffer->fb_id;
            info->width = surface_buffer->width;
            info->height = surface_buffer->height;
            info->pitch = surface_buffer->pitch;
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
    int depth, cpp;

    if (this && this->VideoInit != DRM_VideoInit) {
        return NULL;
    }

    depth = translate_drm_format(drm_format, RGBAmasks, &cpp);
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
        surface_buffer->dumb = 1;
    }
    else {
        surface_buffer = vdata->driver_ops->create_buffer_from_name(
                vdata->driver, name);
        if (surface_buffer == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: faile to create buffer from name: %u!\n",
                    name);
            return NULL;
        }
        surface_buffer->dumb = 0;
    }

    surface_buffer->fb_id = 0;
    surface_buffer->drm_format = drm_format;
    surface_buffer->bpp = depth;
    surface_buffer->cpp = cpp;
    surface_buffer->width = width;
    surface_buffer->height = height;
    surface_buffer->pitch = pitch;
    surface_buffer->offset = pixels_off;

    return create_surface_from_buffer (this, surface_buffer, depth, RGBAmasks);
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
    int depth, cpp;

    if (this && this->VideoInit != DRM_VideoInit) {
        return NULL;
    }

    depth = translate_drm_format (drm_format, RGBAmasks, &cpp);
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
        surface_buffer->dumb = 1;
    }
    else {
        surface_buffer = vdata->driver_ops->create_buffer_from_handle (
                vdata->driver, handle, size);
        if (surface_buffer == NULL) {
            _ERR_PRINTF ("NEWGAL>DRM: failed to create buffer from handle (%u): "
                   "%m!\n", handle);
            return NULL;
        }
        surface_buffer->dumb = 0;
    }

    surface_buffer->fb_id = 0;
    surface_buffer->drm_format = drm_format;
    surface_buffer->bpp = depth;
    surface_buffer->cpp = cpp;
    surface_buffer->width = width;
    surface_buffer->height = height;
    surface_buffer->pitch = pitch;
    surface_buffer->offset = pixels_off;

    return create_surface_from_buffer (this, surface_buffer, depth, RGBAmasks);
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
    int depth, cpp;

    if (this && this->VideoInit != DRM_VideoInit) {
        return NULL;
    }

    depth = translate_drm_format(drm_format, RGBAmasks, &cpp);
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

    surface_buffer->fb_id = 0;
    surface_buffer->drm_format = drm_format;
    surface_buffer->bpp = depth;
    surface_buffer->cpp = cpp;
    surface_buffer->width = width;
    surface_buffer->height = height;
    surface_buffer->pitch = pitch;
    surface_buffer->offset = pixels_off;

    return create_surface_from_buffer (this, surface_buffer, depth, RGBAmasks);
}

#endif /* _MGGAL_DRM */

