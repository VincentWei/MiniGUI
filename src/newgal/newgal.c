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
/*
** The New Graphics Abstract Layer of MiniGUI.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/10/07
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "constants.h"
#include "newgal.h"
#include "misc.h"
#include "license.h"
#include "sharedres.h"

GAL_Surface* __gal_screen;

#ifdef _MGSCHEMA_COMPOSITING
GAL_Surface* __gal_fake_screen;
#endif

RECT GUIAPI GetScreenRect (void)
{
    static RECT rc;

    if (RECTH (rc) == 0) {
#ifdef _MGSCHEMA_COMPOSITING
        rc.left = 0;
        rc.top = 0;
        if (mgIsServer) {
            rc.right = __gal_screen->w;
            rc.bottom = __gal_screen->h;
        }
        else {
            rc.right = SHAREDRES_VIDEO_HRES;
            rc.bottom = SHAREDRES_VIDEO_VRES;
        }
#else
        rc.left = 0;
        rc.top = 0;
        rc.right = __gal_screen->w;
        rc.bottom = __gal_screen->h;
#endif
    }

    return rc;
}

BOOL GAL_ParseVideoMode (const char* mode, int* w, int* h, int* depth)
{
    const char* tmp;

    *w = atoi (mode);

    tmp = strchr (mode, 'x');
    if (tmp == NULL)
        return FALSE;
    *h = atoi (tmp + 1);

    tmp = strrchr (mode, '-');
    if (tmp == NULL)
        return FALSE;

    *depth = atoi (tmp + 1);

    return TRUE;
}

static void get_engine_from_etc (char* engine)
{
#if defined (WIN32) || !defined(__NOUNIX__)
    char* env_value;

    if ((env_value = getenv ("MG_GAL_ENGINE"))) {
        strncpy (engine, env_value, LEN_ENGINE_NAME);
        engine [LEN_ENGINE_NAME] = '\0';
    }
    else
#endif
#ifndef _MG_MINIMALGDI
    if (GetMgEtcValue ("system", "gal_engine", engine, LEN_ENGINE_NAME) < 0) {
        engine [0] = '\0';
    }
#else /* _MG_MINIMALGDI */
#   ifdef _MGGAL_PCXVFB
    strcpy(engine, "pc_xvfb");
#   else
    strcpy(engine, "dummy");
#   endif
#endif /* _MG_MINIMALGDI */
}

static void get_mode_from_etc (const char* engine, char* mode)
{
#if defined (WIN32) || !defined(__NOUNIX__)
    char* env_value;

    if ((env_value = getenv ("MG_DEFAULTMODE"))) {
        strncpy (mode, env_value, LEN_VIDEO_MODE);
        mode [LEN_VIDEO_MODE] = '\0';
    }
    else
#endif
    if (GetMgEtcValue (engine, "defaultmode", mode, LEN_VIDEO_MODE) < 0)
        if (GetMgEtcValue ("system", "defaultmode", mode, LEN_VIDEO_MODE) < 0)
            mode [0] = '\0';
}

static int get_dpi_from_etc (const char* engine)
{
    int dpi;

    if (GetMgEtcIntValue (engine, "dpi", &dpi) < 0)
        dpi = GDCAP_DPI_DEFAULT;
    else if (dpi < GDCAP_DPI_MINIMAL)
        dpi = GDCAP_DPI_MINIMAL;

    return dpi;
}

#ifdef _MGSCHEMA_COMPOSITING

#define LEN_WALLPAPER_PATTER_SIZE       31
#include "client.h"

static GAL_Surface* create_wp_surface(GAL_Surface* screen)
{
    GAL_Surface* wp_surf;
    if (IsServer()) {
        int size[2];
        char wp_size [LEN_WALLPAPER_PATTER_SIZE + 1];

        if (GetMgEtcValue ("compositing_schema", "wallpaper_pattern_size",
                    wp_size, LEN_WALLPAPER_PATTER_SIZE) < 0) {
            strcpy (wp_size, "empty");
        }

        // parse wallpaper pattern size
        if (strcasecmp (wp_size, "full") == 0) {
            size[0] = screen->w;
            size[1] = screen->h;
        }
        else if (strcasecmp (wp_size, "half") == 0) {
            size[0] = screen->w >> 1;
            size[1] = screen->h >> 1;
        }
        else if (strcasecmp (wp_size, "quarter") == 0) {
            size[0] = screen->w >> 2;
            size[1] = screen->h >> 2;
        }
        else if (strcasecmp (wp_size, "octant") == 0) {
            size[0] = screen->w >> 3;
            size[1] = screen->h >> 3;
        }
        else if (strcasecmp (wp_size, "empty") == 0) {
            size[0] = 0;
            size[1] = 0;
        }
        else if (__mg_extract_integers (wp_size, 'x', size, 2) < 2) {
            size[0] = 0;
            size[1] = 0;
        }

        _DBG_PRINTF ("wallpaper pattern size: %d x %d\n", size[0], size[1]);

        if (size[0] > 0 && size[1] > 0) {
            wp_surf = GAL_CreateSharedRGBSurface (screen->video,
                        GAL_HWSURFACE, 0666, size[0], size[1],
                        screen->format->BitsPerPixel,
                        screen->format->Rmask, screen->format->Gmask,
                        screen->format->Bmask, screen->format->Amask);
            _DBG_PRINTF ("GAL_CreateSharedRGBSurface: map size: %lu, fd: %d, RGBAmasks: %x, %x, %x, %x\n",
                    wp_surf->shared_header->map_size, wp_surf->shared_header->fd,
                    wp_surf->format->Rmask, wp_surf->format->Gmask,
                    wp_surf->format->Bmask, wp_surf->format->Amask);
        }
        else {
            goto empty;
        }

        return wp_surf;
    }
    else {
        REQUEST req;
        int fd = -1;
        SHAREDSURFINFO info;

        req.id = REQID_GETSHAREDSURFACE;
        req.data = SYSSF_WALLPAPER_PATTER;
        req.len_data = strlen (SYSSF_WALLPAPER_PATTER) + 1;

        if ((ClientRequestEx2 (&req, NULL, 0, -1,
                &info, sizeof (SHAREDSURFINFO), &fd) < 0) || (fd < 0))
            goto empty;

        _DBG_PRINTF ("REQID_GETSHAREDSURFACE: size (%lu), flags (0x%x), fd: %d\n",
                    info.size, info.flags, fd);
        wp_surf = GAL_AttachSharedRGBSurface (fd, info.size,
            info.flags, TRUE);
        close (fd);

        return wp_surf;
    }

empty:
    _DBG_PRINTF ("creating an empty wallpaper pattern surface\n");
    if (IsServer()) {
        return GAL_CreateRGBSurface (GAL_SWSURFACE, 0, 0,
                    screen->format->BitsPerPixel,
                    screen->format->Rmask, screen->format->Gmask,
                    screen->format->Bmask, screen->format->Amask);
    }
    else {
        return GAL_CreateRGBSurface (GAL_SWSURFACE, 0, 0,
                    SHAREDRES_VIDEO_DEPTH,
                    SHAREDRES_VIDEO_RMASK, SHAREDRES_VIDEO_GMASK,
                    SHAREDRES_VIDEO_BMASK, SHAREDRES_VIDEO_AMASK);
    }
}

#endif

int mg_InitGAL (char* engine, char* mode)
{
    int i;
    int w, h, depth;
    BOOL need_set_mode = TRUE;

    LICENSE_CHECK_CUSTIMER_ID ();

#ifdef _MGRM_PROCESSES
    if (IsServer()) {
        get_engine_from_etc (engine);
    }
    else {
        strncpy (engine, SHAREDRES_VIDEO_ENGINE, LEN_ENGINE_NAME);
        engine [LEN_ENGINE_NAME] = '\0';

        strncpy (mode, SHAREDRES_VIDEO_MODE, LEN_VIDEO_MODE);
        mode [LEN_VIDEO_MODE] = '\0';

#ifdef _MGSCHEMA_COMPOSITING
        need_set_mode = FALSE;
#endif
    }
#else
    get_engine_from_etc (engine);
#endif /* _MGRM_PROCESSES */

    if (engine[0] == 0) {
        return ERR_CONFIG_FILE;
    }

    if (GAL_VideoInit (engine, 0)) {
        GAL_VideoQuit ();
        _ERR_PRINTF ("NEWGAL: Does not find matched engine: %s.\n", engine);
        _ERR_PRINTF ("    Or failed to initialize the engine.\n");
        return ERR_NO_MATCH;
    }

    if (need_set_mode) {
        get_mode_from_etc (engine, mode);

        if (mode[0] == 0) {
            return ERR_CONFIG_FILE;
        }

        if (!GAL_ParseVideoMode (mode, &w, &h, &depth)) {
            GAL_VideoQuit ();
            _ERR_PRINTF ("NEWGAL: bad video mode parameter: %s.\n", mode);
            return ERR_CONFIG_FILE;
        }

        if (!(__gal_screen = GAL_SetVideoMode (w, h, depth, GAL_HWPALETTE))) {
            GAL_VideoQuit ();
            _ERR_PRINTF ("NEWGAL: Set video mode failure.\n");
            return ERR_GFX_ENGINE;
        }
    }

#ifdef _MGSCHEMA_COMPOSITING
    if (!(__gal_fake_screen = create_wp_surface(__gal_screen))) {
        GAL_VideoQuit ();
        _ERR_PRINTF ("NEWGAL: Failed to create wallpaper pattern surface.\n");
        return ERR_GFX_ENGINE;
    }

    if (!IsServer()) {
        __gal_screen = __gal_fake_screen;

        GAL_SetVideoModeInfo(__gal_screen);
    }
#endif

#if 0 /* no need when we use the video info in shared resource */
    if (w != __gal_screen->w || h != __gal_screen->h) {
        _ERR_PRINTF ("The resolution specified in MiniGUI.cfg is not "
                     "the same as the actual resolution: %dx%d.\n"
                     "This may confuse the clients. Please change it.\n",
                         __gal_screen->w, __gal_screen->h);
        GAL_VideoQuit ();
        return ERR_GFX_ENGINE;
    }
#endif

    if (need_set_mode) {
        __gal_screen->dpi = get_dpi_from_etc (engine);
    }
#ifdef _MGRM_PROCESSES
    else {
        __gal_screen->dpi = SHAREDRES_VIDEO_DPI;
    }
#endif /* _MGRM_PROCESSES */

    for (i = 0; i < 17; i++) {
        SysPixelIndex [i] = GAL_MapRGBA (__gal_screen->format,
                        SysPixelColor [i].r, SysPixelColor [i].g,
                        SysPixelColor [i].b, SysPixelColor [i].a);
    }

    return 0;
}

