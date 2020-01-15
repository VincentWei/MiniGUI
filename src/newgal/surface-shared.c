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

#define _DEBUG
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
 * Create a shared empty RGB surface of the appropriate depth
 */
GAL_Surface * GAL_CreateSharedRGBSurface (GAL_VideoDevice *video,
            Uint32 flags, Uint32 rw_modes, int width, int height, int depth,
            Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    GAL_Surface *screen;
    GAL_Surface *surface;
    BOOL sem_inited = FALSE;

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
    surface->offset = 0;
    // for off-screen surface, DPI always be the default value
    surface->dpi = GDCAP_DPI_DEFAULT;
    surface->hwdata = NULL;
    surface->map = NULL;
    surface->format_version = 0;
    surface->shared_header = NULL;
    GAL_SetClipRect (surface, NULL);

#ifdef _MGUSE_SYNC_UPDATE
    /* Initialize update region */
    InitClipRgn (&surface->update_region, &__mg_free_update_region_list);
#endif

    /* Get the pixels */
    {
        int fd = -1;
        size_t buf_size;
        off_t buf_off;
        off_t map_size;
        void* data_map;
        GAL_SharedSurfaceHeader* hdr;
        int byhw = 1;

        if ((flags & GAL_HWSURFACE) == GAL_HWSURFACE &&
                video->AllocSharedHWSurface) {
            fd = video->AllocSharedHWSurface (video, surface,
                    &buf_size, &buf_off, rw_modes);
        }

        if (fd < 0) { // fallback to use software surface
            off_t file_size;

            byhw = 0;
            buf_size = (surface->h * surface->pitch);
            buf_off = sizeof (GAL_SharedSurfaceHeader);

            /* rounde file size to multiple of page size */
            file_size = buf_off + buf_size;
            file_size = ROUND_TO_MULTIPLE(file_size, getpagesize ());
            buf_size = file_size - buf_off;

            _DBG_PRINTF("shared surface: size (%d x %d), pitch (%d), file_size (%lu)\n",
                    surface->w, surface->h, surface->pitch, file_size);

            fd = __mg_create_anonymous_file (file_size, NULL, rw_modes);
            if (fd < 0) {
                goto error;
            }
        }

        map_size = buf_off + buf_size;
        data_map = mmap (NULL, map_size, PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, 0);
        if (data_map == MAP_FAILED) {
            close (fd);
            goto error;
        }

        surface->shared_header = hdr = (GAL_SharedSurfaceHeader*)data_map;

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
        hdr->buf_size   = buf_size;

#if 1
        /* Use a unnamed POSIX semaphore shared between processes */
        if (sem_init (&hdr->sem_lock, 1, 1) < 0) {
            if (errno == ENOSYS) {
                _ERR_PRINTF("The system does not support process-shared semaphore.\n");
            }
            goto error;
        }
        sem_inited = TRUE;
#else
        hdr->semid      = -1;
        hdr->sem_num    = __mg_alloc_mutual_sem (&hdr->semid);
        if (hdr->sem_num < 0)
            goto error;
#endif

        surface->pixels = surface->shared_header->buf;
        // memset (surface->pixels, 0, buf_size);

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
#if 1
            if (sem_inited) {
                sem_destroy (&surface->shared_header->sem_lock);
            }
#else
            if (surface->shared_header->semid >= 0) {
                assert (surface->shared_header->sem_num >= 0);
                __mg_free_mutual_sem (surface->shared_header->sem_num);
            }
#endif

            if (surface->shared_header->byhw) {
                assert(video->FreeSharedHWSurface);
                video->FreeSharedHWSurface (video, surface);
                surface->hwdata = NULL; /* set to NULL for GAL_FreeSurface */
            }
            else {
                close (surface->shared_header->fd);
                munmap (surface->shared_header,
                    surface->shared_header->buf_size
                        + sizeof(GAL_SharedSurfaceHeader));
            }

            surface->shared_header = NULL;
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

#if 1
    sem_destroy (&surface->shared_header->sem_lock);
#else
    if (surface->shared_header->semid >= 0) {
        assert (surface->shared_header->sem_num >= 0);
        __mg_free_mutual_sem (surface->shared_header->sem_num);
    }
#endif

    if (surface->shared_header->byhw) {
        assert (video->FreeSharedHWSurface);
        video->FreeSharedHWSurface (video, surface);
        surface->hwdata = NULL;
    }
    else {
        // the file descriptor may have been closed.
        if (surface->shared_header->fd >= 0)
            close (surface->shared_header->fd);
        munmap (surface->shared_header,
            surface->shared_header->buf_size
                + sizeof (GAL_SharedSurfaceHeader));
    }

    surface->pixels = NULL;
    surface->shared_header = NULL;
}

/*
 * Attach to a shared RGB surface
 */
GAL_Surface * GAL_AttachSharedRGBSurface (int fd, size_t map_size,
        Uint32 flags, BOOL with_wr)
{
    GAL_VideoDevice *video;
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
            return NULL;
        }

        surface->hwdata = NULL;
        surface->shared_header = (GAL_SharedSurfaceHeader*) data_map;
    }

    /* map successfully */
    hdr = surface->shared_header;

    surface->pixels = hdr->buf;
    surface->format = GAL_AllocFormat (hdr->depth,
            hdr->Rmask, hdr->Gmask, hdr->Bmask, hdr->Amask);
    if (surface->format == NULL) {
        goto error;
    }

    surface->w = hdr->width;
    surface->h = hdr->height;
    surface->pitch = GAL_CalculatePitch (surface);
    surface->pixels = NULL;
    surface->offset = 0;
    // for off-screen surface, DPI always be the default value
    surface->dpi = GDCAP_DPI_DEFAULT;
    surface->map = NULL;
    surface->format_version = 0;
    GAL_SetClipRect (surface, NULL);

#ifdef _MGUSE_SYNC_UPDATE
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
    if (data_map != MAP_FAILED) {
        if (video && video->DettachSharedHWSurface) {
            video->DettachSharedHWSurface (video, surface);
        }
        else
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

    if (video && video->DettachSharedHWSurface) {
        video->DettachSharedHWSurface (video, surface);
        surface->hwdata = NULL;
    }

    munmap (surface->shared_header,
        surface->shared_header->buf_size
            + sizeof (GAL_SharedSurfaceHeader));
    surface->pixels = NULL;
    surface->shared_header = NULL;
}

GAL_Surface *GAL_CreateSurfaceForZNode (int width, int height)
{
    _DBG_PRINTF("size: %d x %d\n", width, height);

    if (IsServer()) {
        return GAL_CreateRGBSurface (
            GAL_HWSURFACE, width, height, __gal_screen->format->BitsPerPixel,
            __gal_screen->format->Rmask, __gal_screen->format->Gmask,
            __gal_screen->format->Bmask, __gal_screen->format->Amask);
    }

    return GAL_CreateSharedRGBSurface (__gal_screen->video,
        GAL_HWSURFACE, 0600, width, height, __gal_screen->format->BitsPerPixel,
        __gal_screen->format->Rmask, __gal_screen->format->Gmask,
        __gal_screen->format->Bmask, __gal_screen->format->Amask);
}

#endif /* IS_COMPOSITING_SCHEMA */

