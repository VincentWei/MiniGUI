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
#include "constants.h"

#if IS_COMPOSITING_SCHEMA

#include <unistd.h>
#include <sys/mman.h>

#include "misc.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "blit.h"

/*
 * Create a cursor surface
 */
GAL_Surface * GAL_CreateCursorSurface (GAL_VideoDevice *video,
            int width, int height)
{
    GAL_Surface *surface;

    /* Check to see if we desire the surface in video memory */
    if (video == NULL) {
        video = __mg_current_video;
    }

    /* Allocate the surface */
    surface = (GAL_Surface *)malloc (sizeof (*surface));
    if (surface == NULL) {
        goto error;
    }

    surface->flags = 0;
    surface->flags |= GAL_SRCPIXELALPHA;

    /* Always use ARGB8888 pixel format for cursor */
    surface->format = GAL_AllocFormat (32,
                0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
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
    if (video == NULL || video->AllocDumbSurface == NULL ||
            video->AllocDumbSurface (video, surface) < 0) {

        // fallback to allocate in system memory
        surface->hwdata = NULL;
        surface->pixels = malloc (surface->h*surface->pitch);
        if (surface->pixels == NULL) {
            goto error;
        }

        surface->video = NULL;
        surface->flags &= ~GAL_HWSURFACE;
    }
    else {
        surface->video = video;
        surface->flags |= GAL_HWSURFACE;
    }

    surface->map = GAL_AllocBlitMap();
    if (surface->map == NULL) {
        goto error;
    }

    /* The surface is ready to go */
    surface->refcount = 1;
    return (surface);

error:
    if (surface) {
        if (surface->hwdata) {
            assert (video->FreeDumbSurface);
            video->FreeDumbSurface (video, surface);
            surface->hwdata = NULL;
        }
        else {
            free (surface->pixels);
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
void GAL_FreeCursorSurface (GAL_Surface *surface)
{
    GAL_VideoDevice *video = surface->video;

    if (surface->hwdata) {
        assert (video->FreeDumbSurface);
        video->FreeDumbSurface (video, surface);
        surface->hwdata = NULL; /* set to NULL for GAL_FreeSurface */
    }
    else {
        free (surface->pixels);
    }

    surface->pixels = NULL; /* set to NULL for GAL_FreeSurface */
    GAL_FreeSurface (surface);
}

void GAL_SetCursor (GAL_Surface* surface, int hot_x, int hot_y)
{
    GAL_VideoDevice *video = __mg_current_video;
    if (video->SetCursor) {
        video->SetCursor (video, surface, hot_x, hot_y);
    }
    else {
        _WRN_PRINTF("called but current video has no SetCursor method\n");
    }
}

/* Move cursor. */
void GAL_MoveCursor (GAL_Surface* surface, int x, int y)
{
    GAL_VideoDevice *video = __mg_current_video;

    if (video->MoveCursor) {
        video->MoveCursor (video, x, y);
    }
    else {
        _WRN_PRINTF("called but current video has no MoveCursor method\n");
    }
}

#endif /* IS_COMPOSITING_SCHEMA */

