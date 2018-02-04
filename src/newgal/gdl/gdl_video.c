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
** gdl_video.c: Intel GDL based video driver implementation.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "minigui.h"
#include "misc.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "gdl_video.h"

/* Initialization/Query functions */
static int GDL_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **GDL_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *GDL_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int GDL_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void GDL_DeleteDevice (GAL_VideoDevice *device);
static void GDL_VideoQuit(_THIS);

/* Hardware surface functions */
static void GDL_UpdateRects (_THIS, int numrects, GAL_Rect *rects);
static int GDL_AllocHWSurface(_THIS, GAL_Surface *surface);
static void GDL_FreeHWSurface(_THIS, GAL_Surface *surface);

/* GDL driver bootstrap functions */
static int GDL_Available (void)
{
    return 1;
}

static GAL_VideoDevice *GDL_CreateDevice (int devindex)
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
    this->VideoInit = GDL_VideoInit;
    this->ListModes = GDL_ListModes;
    this->SetVideoMode = GDL_SetVideoMode;
    /* this->SetColors = GDL_SetColors; */
    this->VideoQuit = GDL_VideoQuit;

    this->AllocHWSurface = GDL_AllocHWSurface;
    this->FreeHWSurface = GDL_FreeHWSurface;

    this->UpdateRects = GDL_UpdateRects;

    this->CheckHWBlit = NULL;
    this->FillHWRect = NULL;
    this->SetHWColorKey = NULL;
    this->SetHWAlpha = NULL;

    this->free = GDL_DeleteDevice;

    return this;
}

static void GDL_DeleteDevice (GAL_VideoDevice *device)
{
    free (device->hidden);
    free (device);
}

VideoBootStrap GDL_bootstrap = {
    "gdl", "Intel GDL",
    GDL_Available, GDL_CreateDevice
};

static int GDL_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    struct GAL_PrivateVideoData* data = this->hidden;
    gdl_plane_id_t plane = GDL_PLANE_ID_UPP_B;
    int display = 0;
    gdl_uint32 pf = 0;

    if (GetMgEtcIntValue ("gdl", "display", &display) < 0) {
        display = GDL_DISPLAY_ID_0;
    }
    data->display = display;

    if (GDL_SUCCESS != gdl_init(NULL)) {
        fprintf(stderr, "GDL init failed!\n");
        return -1;
    }

    if (GDL_SUCCESS == gdl_plane_get_uint(plane, GDL_PLANE_PIXEL_FORMAT, &pf)) {
        switch (pf) {
            case GDL_PF_ARGB_32:
                data->depth = 32;
                break;
            case GDL_PF_RGB_24:
                data->depth = 24;
                break;
            case GDL_PF_RGB_16:
                data->depth = 16;
                break;
            case GDL_PF_RGB_15:
                data->depth = 15;
                break;
            case GDL_PF_ARGB_16_4444:
                data->depth = 12;
                break;
            case GDL_PF_RGB_8:
                data->depth = 8;
                break;
            default:
                data->depth = 32;
                break;
        }

        vformat->BitsPerPixel = data->depth;
        switch (vformat->BitsPerPixel) {
#ifdef _MGGAL_SHADOW
            case 1:
                break;
            case 4:
                break;
#endif
            case 8:
                vformat->BytesPerPixel = 1;
                /*data->numcols = 256;*/
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
                GAL_SetError ("NEWGAL>GDL: Not supported depth: %d, "
                        "please try to use Shadow NEWGAL engine with targetname gdl.\n", vformat->BitsPerPixel);
                return -1;
        }
    }

    return 0;
}

static GAL_Rect **GDL_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    return (GAL_Rect **) -1;
}

static gdl_pixel_format_t pixel_format_conversion(int BitsPerPixel)
{
    gdl_pixel_format_t pf;
    switch (BitsPerPixel) {
        case 8:
            pf = GDL_PF_RGB_8;
            break;
        case 15:
            pf = GDL_PF_RGB_15;
            break;
        case 16:
            pf = GDL_PF_ARGB_16_1555;
            break;
        case 24:
            pf = GDL_PF_RGB_24;
            break;
        case 32:
            pf = GDL_PF_ARGB_32;
            break;
        default:
            pf = GDL_PF_ARGB_32;
            break;
    }
    return pf;
}

static GAL_Surface *GDL_SetVideoMode (_THIS, GAL_Surface *current,
                                int width, int height, int bpp, Uint32 flags)
{
    struct GAL_PrivateVideoData* hidden = this->hidden;
    gdl_display_info_t  di;
    gdl_plane_id_t plane = GDL_PLANE_ID_UPP_B;

    di.id           = hidden->display;
    di.flags        = 0;
    di.bg_color     = 0;
    di.color_space = GDL_COLOR_SPACE_RGB;
    /* di.color_space = (di.tvmode.height <= 576)
        ? GDL_COLOR_SPACE_BT601
        : GDL_COLOR_SPACE_BT709; */
    
    di.gamma        = GDL_GAMMA_LINEAR;
    di.tvmode.width      = width;
    di.tvmode.height     = height;
    di.tvmode.refresh    =  GDL_REFRESH_50;
    di.tvmode.interlaced = GDL_FALSE;

    if (GDL_SUCCESS == gdl_set_display_info(&di)) {
        gdl_rectangle_t rect;
        gdl_pixel_format_t pf = pixel_format_conversion(bpp);
        gdl_color_space_t   color_space = GDL_COLOR_SPACE_RGB;

        if (GDL_SUCCESS == gdl_get_display_info(hidden->display, &di)) {
            rect.origin.x = 0;
            rect.origin.y = 0;
            rect.height   = di.tvmode.height;
            rect.width    = di.tvmode.width;
        }

        if (GDL_SUCCESS != gdl_plane_reset(plane)) {
            fprintf(stderr, "GDL plane reset failed!\n");
            return current;
        }

        pf = pixel_format_conversion(bpp);

        gdl_plane_config_begin(plane);
        (gdl_plane_set_rect(GDL_PLANE_DST_RECT, &rect));
        (gdl_plane_set_uint(GDL_PLANE_PIXEL_FORMAT, pf));
        (gdl_plane_set_uint(GDL_PLANE_SRC_COLOR_SPACE,
                            color_space));
        gdl_plane_config_end(GDL_FALSE);
        hidden->plane_id = plane;

        /* Set up the mode GDL */
        current->flags = GAL_FULLSCREEN;
        current->w = hidden->width = width;
        current->h = hidden->height = height;
        GDL_AllocHWSurface(this, current);
        printf("screen phys base pixels = %p.\n", current->hwdata->info.heap_phys_addr);
        printf("screen phys pixels = %p.\n", current->hwdata->info.phys_addr);
        printf("screen pixels = %p.\n", current->pixels);
        printf("surface %d plane %d.\n", current->hwdata->info.id, hidden->plane_id);
        hidden->linestep = current->pitch;
        hidden->screen_surf = current->hwdata->info.id;
    }

    return current;
}

/* We don't actually allow hardware surfaces other than the main one */
static int GDL_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    gdl_surface_info_t info;
    gdl_pixel_format_t pf = GDL_PF_ARGB_32;//pixel_format_conversion(surface->format->BitsPerPixel);
    if (GDL_SUCCESS == 
            gdl_alloc_surface (pf, surface->w, surface->h, 0, &info)) {
        gdl_color_t  color = {255, 255, 255, 255};
        surface->hwdata = (struct private_hwdata *)calloc(1, sizeof(struct private_hwdata));
        if (NULL != surface->hwdata) {
            memcpy(&surface->hwdata->info, &info, sizeof(info));
            surface->pitch = info.pitch;
            surface->flags |= GAL_HWSURFACE;
            gdl_clear_surface(info.id, &color);
            gdl_map_surface (info.id, &surface->pixels, NULL);
            return 0;
        } else {
            GAL_OutOfMemory ();
        }
    } else {
        surface->flags &= ~GAL_HWSURFACE;
    }
    return -1;
}

static void GDL_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    if (surface->hwdata->info.id != GDL_SURFACE_INVALID)
    {   
        gdl_unmap_surface (surface->hwdata->info.id);
        gdl_free_surface(surface->hwdata->info.id);
    }
    free(surface->hwdata);
    surface->hwdata = NULL;
    surface->pixels = NULL;
}

static void GDL_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    struct GAL_PrivateVideoData* hidden = this->hidden;
    if (GAL_VideoSurface->hwdata->info.id != GDL_SURFACE_INVALID) {
        gdl_unmap_surface (GAL_VideoSurface->hwdata->info.id);
        /* use sync flip bacause it will map surface immediately after flip. */
        /*gdl_wait_for_vblank(NULL);*/
        gdl_flip(hidden->plane_id, GAL_VideoSurface->hwdata->info.id, GDL_FLIP_ASYNC);
        gdl_map_surface (GAL_VideoSurface->hwdata->info.id, &GAL_VideoSurface->pixels, NULL);
    }
    /*
    RECT bound = this->hidden->update;

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

    this->hidden->update = bound;
    this->hidden->dirty = TRUE;
    */
}

#if 0
static int GDL_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
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
            this->hidden->clut [pixel] 
                = (0xff << 24) | ((colors[index].r & 0xff) << 16) | ((colors[index].g & 0xff) << 8) | (colors[index].b & 0xff);
            pixel ++;
        }
        /* set final color. */
        this->hidden->clut [(1<<bpp)-1] 
            = (0xff << 24) | ((colors[255].r & 0xff) << 16) | ((colors[255].g & 0xff) << 8) | (colors[255].b & 0xff);

    }
    else {
        for (i = 0; i < ncolors; i++) {
            this->hidden->clut [pixel]
                = (0xff << 24) | ((colors[i].r & 0xff) << 16) | ((colors[i].g & 0xff) << 8) | (colors[i].b & 0xff);
            pixel ++;
        }
    }

    return 1;
}
#endif

static void GDL_VideoQuit (_THIS)
{
    gdl_plane_config_end(GDL_TRUE);  /* Terminate any incomplete plane config */
    gdl_close();
}

