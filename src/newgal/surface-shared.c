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
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "constants.h"

#if IS_COMPOSITING_SCHEMA

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "misc.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "blit.h"

/*
 * Create a shared empty RGB surface of the appropriate depth and pixel format
 */
GAL_Surface * GAL_CreateSharedRGBSurface (GAL_VideoDevice *video,
            Uint32 flags, Uint32 rw_modes, int width, int height, int depth,
            Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    GAL_Surface *screen;
    GAL_Surface *surface;

    if (video == NULL) {
        video = __mg_current_video;
    }

    assert (width > 0 && height > 0);

    /* Check to see if we desire the surface in video memory */
    screen = GAL_PublicSurface;
    if (screen && ((screen->flags & GAL_HWSURFACE) == GAL_HWSURFACE)) {
        if ((flags & (GAL_SRCCOLORKEY | GAL_SRCALPHA)) != 0) {
            flags |= GAL_HWSURFACE;
        }

        if ((flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
            if (! video->info.blit_hw_CC) {
                flags &= ~GAL_HWSURFACE;
            }
        }
        if ((flags & GAL_SRCALPHA) == GAL_SRCALPHA) {
            if (! video->info.blit_hw_A) {
                flags &= ~GAL_HWSURFACE;
            }
        }
    }
    else {
        flags &= ~GAL_HWSURFACE;
    }

    /* Allocate the surface */
    surface = (GAL_Surface *)malloc (sizeof (*surface));
    if (surface == NULL) {
        goto error;
    }

    if ((flags & GAL_HWSURFACE) == GAL_HWSURFACE)
        surface->video = video;
    else
        surface->video = NULL;

    surface->flags = GAL_SWSURFACE;
    if (Amask) {
        surface->flags |= GAL_SRCPIXELALPHA;
    }

    surface->format = GAL_AllocFormat (depth, Rmask, Gmask, Bmask, Amask);
    if (surface->format == NULL) {
        free (surface);
        surface = NULL;
        goto error;
    }

    surface->w = width;
    surface->h = height;
    surface->pitch = GAL_CalculatePitch (surface);
    surface->pixels = NULL;
    surface->pixels_off = 0;
    // for off-screen surface, DPI always be the default value
    surface->dpi = GDCAP_DPI_DEFAULT;
    surface->hwdata = NULL;
    surface->map = NULL;
    surface->format_version = 0;
    surface->shared_header = NULL;
    surface->dirty_info = NULL;
    GAL_SetClipRect (surface, NULL);

#ifdef _MGUSE_UPDATE_REGION
    /* Initialize update region */
    InitClipRgn (&surface->update_region, &__mg_free_update_region_list);
#endif

    /* Get the pixels */
    {
        int fd = -1;
        size_t map_size;
        off_t pixels_off;
        GAL_SharedSurfaceHeader* hdr;
        int byhw = 1;

        if ((flags & GAL_HWSURFACE) == GAL_HWSURFACE &&
                video->AllocSharedHWSurface) {
            fd = video->AllocSharedHWSurface (video, surface,
                    &map_size, &pixels_off, rw_modes);
            hdr = surface->shared_header;
        }

        if (fd < 0) { // fallback to use software surface
            size_t pixels_size;
            void* data_map;
            off_t file_size;

            byhw = 0;
            pixels_size = (surface->h * surface->pitch);
            pixels_off = sizeof (GAL_SharedSurfaceHeader);

            file_size = pixels_off + pixels_size;
#if 0
            /* rounde file size to multiple of page size */
            file_size = ROUND_TO_MULTIPLE(file_size, getpagesize ());
            pixels_size = file_size - pixels_off;
#endif

            _DBG_PRINTF("shared surface (%d x %d): pitch(%d), filesize(%lu)\n",
                    surface->w, surface->h, surface->pitch, file_size);

            fd = __mg_create_anonymous_file (file_size, NULL, rw_modes);
            if (fd < 0) {
                goto error;
            }

            surface->video = NULL;

            map_size = file_size;
            data_map = mmap (NULL, map_size, PROT_READ | PROT_WRITE,
                    MAP_SHARED, fd, 0);
            if (data_map == MAP_FAILED) {
                close (fd);
                goto error;
            }

            surface->shared_header = hdr = (GAL_SharedSurfaceHeader*)data_map;
        }

        /* fill fileds of shared header */
        memset (hdr, 0, sizeof(GAL_SharedSurfaceHeader));
        hdr->creator    = getpid();
        hdr->fd         = fd;
        hdr->byhw       = byhw;
        hdr->width      = surface->w;
        hdr->height     = surface->h;
        hdr->pitch      = surface->pitch;
        hdr->depth      = depth;
        hdr->Rmask      = Rmask;
        hdr->Gmask      = Gmask;
        hdr->Bmask      = Bmask;
        hdr->Amask      = Amask;
        hdr->map_size   = map_size;
        hdr->pixels_off = pixels_off;

        surface->dirty_info = &hdr->dirty_info;
        surface->dirty_info->dirty_age       = 0;
        surface->dirty_info->nr_dirty_rcs    = 0;

        /* allocate semaphore from semaphore for shared surface */
        if (IsServer() && __gal_fake_screen == NULL) {
            // use 0 for wallpaper pattern,
            // because the semaphore set manager is not ready.
            hdr->sem_num = 0;
        }
        else {
            hdr->sem_num = __mg_alloc_sem_for_shared_surf();
            if (hdr->sem_num < 0) {
                goto error;
            }
        }

        surface->pixels = (uint8_t*)surface->shared_header + pixels_off;

        if (byhw)
            surface->flags |= GAL_HWSURFACE;
        else
            surface->flags &= ~GAL_HWSURFACE;
    }

    /* Allocate an empty mapping */
    surface->map = GAL_AllocBlitMap();
    if (surface->map == NULL) {
        goto error;
    }

    /* The surface is ready to go */
    surface->refcount = 1;
    return(surface);

error:
    if (surface) {
        if (surface->shared_header) {
#if 0
            if (sem_inited) {
                sem_destroy (&surface->shared_header->sem_lock);
            }
#endif

            if (surface->shared_header->sem_num >= 0) {
                __mg_free_sem_for_shared_surf (surface->shared_header->sem_num);
            }

            if (surface->shared_header->byhw) {
                assert(video->FreeSharedHWSurface);
                video->FreeSharedHWSurface (video, surface);
                surface->hwdata = NULL; /* set to NULL for GAL_FreeSurface */
            }
            else {
                close (surface->shared_header->fd);
                munmap (surface->shared_header,
                    surface->shared_header->map_size);
            }

            surface->shared_header = NULL;
            surface->dirty_info = NULL;
        }

        surface->pixels = NULL;
        GAL_FreeSurface (surface);
    }

    GAL_OutOfMemory ();
    return NULL;
}

/*
 * Free data of a shared surface created by the above function.
 */
void GAL_FreeSharedSurfaceData (GAL_Surface *surface)
{
    GAL_VideoDevice *video = surface->video;

    assert (surface->shared_header);

#if 0
    sem_destroy (&surface->shared_header->sem_lock);
#endif

    if (surface->shared_header->sem_num >= 0) {
        __mg_free_sem_for_shared_surf (surface->shared_header->sem_num);
    }

    // the file descriptor may have been closed.
    if (surface->shared_header->fd >= 0) {
        close (surface->shared_header->fd);
        surface->shared_header->fd = -1;
    }

    if (surface->shared_header->byhw) {
        assert (video->FreeSharedHWSurface);
        video->FreeSharedHWSurface (video, surface);
        surface->hwdata = NULL;
    }
    else {
        munmap (surface->shared_header,
            surface->shared_header->map_size);
    }

    surface->pixels = NULL;
    surface->shared_header = NULL;
    surface->dirty_info = NULL;
}

/*
 * Attach to a shared RGB surface
 */
GAL_Surface * GAL_AttachSharedRGBSurface (int fd, size_t map_size,
        Uint32 flags, BOOL with_wr)
{
    GAL_VideoDevice *video = NULL;
    GAL_Surface *surface;
    void* data_map = MAP_FAILED;
    GAL_SharedSurfaceHeader* hdr;

    if (map_size == 0) {
        struct stat statbuf;
        if (fstat (fd, &statbuf)) {
            _ERR_PRINTF("NEWGAL: Failed to get map size of shared RGB surface (%d): %m\n", fd);
            return NULL;
        }

        map_size = statbuf.st_size;
        _DBG_PRINTF("map_size got by calling fstat: %lu\n", map_size);
    }

    /* Allocate the surface */
    surface = (GAL_Surface *)malloc (sizeof (*surface));
    if (surface == NULL) {
        goto error;
    }

    if ((flags & GAL_HWSURFACE) == GAL_HWSURFACE) {
        video = __mg_current_video;
    }
    else {
        video = NULL;
    }

    surface->video = video;
    surface->flags = flags;

    surface->shared_header = NULL;
    surface->hwdata = NULL;
    if (video && video->AttachSharedHWSurface) {
        // this method should fill hwdata and shared_header fields if success
        video->AttachSharedHWSurface (video, surface, fd, map_size, with_wr);
    }

    if (surface->shared_header == NULL) {
        // fallback to software
        int prot = PROT_READ;

        if (with_wr) {
            prot |= PROT_WRITE;
        }

        data_map = mmap (NULL, map_size, prot, MAP_SHARED, fd, 0);
        if (data_map == MAP_FAILED) {
            _ERR_PRINTF("NEWGAL: Failed to map shared RGB surface: %d\n", fd);
            goto error;
        }

        surface->hwdata = NULL;
        surface->video = NULL;
        surface->shared_header = (GAL_SharedSurfaceHeader*) data_map;
    }

    /* map successfully */
    hdr = surface->shared_header;

    if (surface->hwdata)
        surface->flags |= GAL_HWSURFACE;
    else
        surface->flags &= ~GAL_HWSURFACE;

    surface->dirty_info = &hdr->dirty_info;
    surface->pixels = (uint8_t*)hdr + hdr->pixels_off;
    surface->format = GAL_AllocFormat (hdr->depth,
            hdr->Rmask, hdr->Gmask, hdr->Bmask, hdr->Amask);
    if (surface->format == NULL) {
        goto error;
    }

    surface->w = hdr->width;
    surface->h = hdr->height;
    surface->pitch = hdr->pitch;
    surface->pixels_off = 0;
    // for off-screen surface, DPI always be the default value
    surface->dpi = GDCAP_DPI_DEFAULT;
    surface->map = NULL;
    surface->format_version = 0;
    GAL_SetClipRect (surface, NULL);

#ifdef _MGUSE_UPDATE_REGION
    /* Initialize update region */
    InitClipRgn (&surface->update_region, &__mg_free_update_region_list);
#endif

    /* Allocate an empty mapping */
    surface->map = GAL_AllocBlitMap();
    if (surface->map == NULL) {
        goto error;
    }

    /* The surface is ready to go */
    surface->refcount = 1;
    return (surface);

error:
    if (video && surface && surface->hwdata) {
        assert (video->DettachSharedHWSurface);
        video->DettachSharedHWSurface (video, surface);
    }
    else if (data_map != MAP_FAILED) {
        munmap (data_map, map_size);
    }

    if (surface)
        GAL_FreeSurface (surface);

    GAL_OutOfMemory ();
    return NULL;
}

/*
 * Free data of a shared surface created by the above function.
 */
void GAL_DettachSharedSurfaceData (GAL_Surface *surface)
{
    GAL_VideoDevice *video = surface->video;

    assert (surface->shared_header);

    if (video && surface->hwdata) {
        assert (video->DettachSharedHWSurface);
        video->DettachSharedHWSurface (video, surface);
        surface->hwdata = NULL;
    }
    else {
        munmap (surface->shared_header,
            surface->shared_header->map_size);
    }

    surface->pixels = NULL;
    surface->shared_header = NULL;
    surface->dirty_info = NULL;
}

static inline
GAL_Surface *create_surface_for_znode (GAL_VideoDevice* video,
        int width, int height, int bpp,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    GAL_Surface * surface;

    if (width <= 0) width = 8;
    if (height <= 0) height = 1;

    if (IsServer()) {
        surface = GAL_CreateRGBSurface (
            GAL_HWSURFACE, width, height, bpp, Rmask, Gmask, Bmask, Amask);

        if (surface) {
            surface->dirty_info = calloc (1, sizeof (GAL_DirtyInfo));
            if (surface->dirty_info == NULL) {
                GAL_FreeSurface (surface);
                GAL_OutOfMemory ();
                return (NULL);
            }
        }

        return surface;
    }

    return GAL_CreateSharedRGBSurface (video,
        GAL_HWSURFACE, 0600, width, height, bpp, Rmask, Gmask, Bmask, Amask);
}

GAL_Surface *GAL_CreateSurfaceForZNodeAs (const GAL_Surface* ref_surf,
        int width, int height)
{
    return create_surface_for_znode (__gal_screen->video,
            width, height, ref_surf->format->BitsPerPixel,
            ref_surf->format->Rmask, ref_surf->format->Gmask,
            ref_surf->format->Bmask, ref_surf->format->Amask);
}

GAL_Surface *GAL_CreateSurfaceForZNode (unsigned surf_flag, int width, int height)
{
    int bpp;
    Uint32 Rmask, Gmask, Bmask, Amask;

    _DBG_PRINTF("type: %u, size: %d x %d\n", surf_flag, width, height);

    switch (surf_flag & ST_PIXEL_MASK) {
    case ST_PIXEL_ARGB4444:
        bpp = 16;
        Amask = 0xF000;
        Rmask = 0x0F00;
        Gmask = 0x00F0;
        Bmask = 0x000F;
        break;

    case ST_PIXEL_ARGB1555:
        bpp = 16;
        Amask = 0x8000;
        Rmask = 0x7B00;
        Gmask = 0x03E0;
        Bmask = 0x001F;
        break;

    case ST_PIXEL_ARGB8888:
        bpp = 32;
        Amask = 0xFF000000;
        Rmask = 0x00FF0000;
        Gmask = 0x0000FF00;
        Bmask = 0x000000FF;
        break;

    default:
        bpp = __gal_screen->format->BitsPerPixel;
        Amask = __gal_screen->format->Amask;
        Rmask = __gal_screen->format->Rmask;
        Gmask = __gal_screen->format->Gmask;
        Bmask = __gal_screen->format->Bmask;
        break;
    }

    return create_surface_for_znode (__gal_screen->video,
            width, height, bpp, Rmask, Gmask, Bmask, Amask);
}

#endif /* IS_COMPOSITING_SCHEMA */

