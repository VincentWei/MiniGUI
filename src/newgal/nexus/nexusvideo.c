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
**  $Id$
**  
**  Copyright (C) 2003 ~ 2009 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

/* NEXUS GAL video driver implementation; 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#ifdef _MGRM_PROCESSES
#   include "client.h"
#endif

#ifdef _MGGAL_NEXUS

#include "nexusvideo_pri.h"
#include "nexusvideo.h"

#define NEXUSVID_DRIVER_NAME "nexus"

#ifdef _MGRM_PROCESSES
static REP_NEXUS_GETSURFACE reply_primary_surface_info;
#   define ISSERVER (mgIsServer)
#else
#   define ISSERVER (1)
#endif

/* Initialization/Query functions */
static int NEXUS_VideoInit(_THIS, GAL_PixelFormat * vformat);
static GAL_Rect **NEXUS_ListModes(_THIS, GAL_PixelFormat * format,
                                  Uint32 flags);
static GAL_Surface *NEXUS_SetVideoMode(_THIS, GAL_Surface * current, int width,
                                       int height, int bpp, Uint32 flags);
static int NEXUS_SetColors(_THIS, int firstcolor, int ncolors,
                           GAL_Color * colors);
static void NEXUS_VideoQuit(_THIS);

/* Hardware surface functions */
static int NEXUS_AllocHWSurface(_THIS, GAL_Surface * surface);
static void NEXUS_FreeHWSurface(_THIS, GAL_Surface * surface);
static int NEXUS_FillHWRect(_THIS, GAL_Surface * dst, GAL_Rect * rect,
                            Uint32 color);
static void NEXUS_UpdateSurfaceRects(_THIS, GAL_Surface* surface, 
        int numrects, GAL_Rect *rects);
static int NEXUS_CheckHWBlit(_THIS, GAL_Surface * src, GAL_Surface * dst);

/* NEXUS driver bootstrap functions */

static int
NEXUS_Available(void)
{
    return (1);
}

static void
NEXUS_DeleteDevice(GAL_VideoDevice * device)
{
    free(device->hidden);
    free(device);
}

static int NEXUS_SetHWColorKey(_THIS, GAL_Surface *surface, Uint32 key)
{
    return 0;
}

static GAL_VideoDevice *
NEXUS_CreateDevice(int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *) malloc(sizeof(GAL_VideoDevice));
    if (device)
    {
        memset(device, 0, (sizeof *device));
        device->hidden =
            (struct GAL_PrivateVideoData *)malloc((sizeof *device->hidden));
    }
    if ((device == NULL) || (device->hidden == NULL))
    {
        GAL_OutOfMemory();
        if (device)
        {
            free(device);
        }
        return (0);
    }
    memset(device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = NEXUS_VideoInit;
    device->ListModes = NEXUS_ListModes;
    device->SetVideoMode = NEXUS_SetVideoMode;
    device->SetColors = NEXUS_SetColors;
    device->VideoQuit = NEXUS_VideoQuit;
    device->AllocHWSurface = NEXUS_AllocHWSurface;
    device->CheckHWBlit = NEXUS_CheckHWBlit;
    device->FillHWRect = NEXUS_FillHWRect;
    device->SetHWColorKey = NEXUS_SetHWColorKey;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = NEXUS_FreeHWSurface;
    device->UpdateRects = NULL;
    device->UpdateSurfaceRects = NEXUS_UpdateSurfaceRects;

    device->free = NEXUS_DeleteDevice;

    return device;
}

VideoBootStrap NEXUS_bootstrap = {
    NEXUSVID_DRIVER_NAME, "Nexus video driver",
    NEXUS_Available, NEXUS_CreateDevice
};

static int
NEXUS_HWAccelBlit(GAL_Surface * src, GAL_Rect * srcrect, GAL_Surface * dst,
                  GAL_Rect * dstrect)
{
    int flag = 0;
    unsigned int colorkey_src = 0, colorkey_dst = 0, alpha_src = 0;
    if ((src->flags & GAL_SRCCOLORKEY)){
        flag |= 1;
        colorkey_src = src->format->colorkey;
    }
    if ((dst->flags & GAL_SRCCOLORKEY)){
        flag |= 2;
        colorkey_dst = dst->format->colorkey;
    }
    if ((src->flags & GAL_SRCALPHA)){
        flag |= 4;
        alpha_src = src->format->alpha;
    }

    return NexusPrivate_BitBlitEx(NULL, 
            (NexusPrivate_HWSurface_hwdata *) src->hwdata,
            srcrect->x, srcrect->y, srcrect->w, srcrect->h,
            (NexusPrivate_HWSurface_hwdata *) dst->hwdata,
            dstrect->x, dstrect->y, dstrect->w, dstrect->h,
            flag, colorkey_src, colorkey_dst, alpha_src
            );
}

static int
NEXUS_CheckHWBlit(_THIS, GAL_Surface * src, GAL_Surface * dst)
{
    // printf("Enter NEXUS_CheckHWBlit(src=%p, dst=%p)\n", src, dst);

    src->flags &= ~GAL_HWACCEL;
    src->map->hw_blit = NULL;

    // only supported the hw surface accelerated.
    if (!(src->flags & GAL_HWSURFACE) || !(dst->flags & GAL_HWSURFACE))
    {
        printf("src(%s) dst(%s)\n", 
                (src->flags & GAL_HWSURFACE) ? "HW" : "SW",
                (dst->flags & GAL_HWSURFACE) ? "HW" : "SW");
        return -1;
    }

    src->flags |= GAL_HWACCEL;
    src->map->hw_blit = NEXUS_HWAccelBlit;
    return 0;
}

static int
NEXUS_VideoInit(_THIS, GAL_PixelFormat * vformat)
{
    GAL_VideoInfo *video_info = &this->info;

    fprintf(stderr, "NEWGAL>NEXUS: Calling init method!\n");

    vformat->BitsPerPixel = 32;
    vformat->BytesPerPixel = 4;
    vformat->Amask = 0xff000000;
    vformat->Rmask = 0x00ff0000;
    vformat->Gmask = 0x0000ff00;
    vformat->Bmask = 0x000000ff;

    {
#define FLAGS_BLIT_FILL  0x01
#define FLAGS_BLIT_HW    0x02
#define FLAGS_BLIT_HW_CC    0x04
#define FLAGS_BLIT_HW_A    0x08
        char *env = getenv("MG_ENV_HWACCEL_FLAGS");
        int flags = 0;
        if (! env)
        {
            flags = FLAGS_BLIT_FILL | FLAGS_BLIT_HW | FLAGS_BLIT_HW_CC | FLAGS_BLIT_HW_A;
        }
        else
        {
            flags = atoi(env);
        }
        video_info->blit_fill = (flags & FLAGS_BLIT_FILL) ? 1 : 0; /* FillRect. Disabled because it is too slow with the stupid flush and wait policy */
        video_info->blit_hw = (flags & FLAGS_BLIT_HW) ? 1 : 0;   /* BitBlit */
        video_info->blit_hw_CC = (flags & FLAGS_BLIT_HW_CC) ? 1 : 0; /* Colorkey */
        video_info->blit_hw_A  = (flags & FLAGS_BLIT_HW_A) ? 1 : 0; /* Alpha */
    }
    video_info->mlt_surfaces = 1;

#ifdef _MGRM_PROCESSES
    memset(&reply_primary_surface_info, 0, sizeof(reply_primary_surface_info));
#endif

    /* We're done! */
    return (0);
}

static GAL_Rect *modes[16];

static GAL_Rect **
NEXUS_ListModes(_THIS, GAL_PixelFormat * format, Uint32 flags)
{
    if (format->BitsPerPixel == 32)
    {
        return NexusPrivate_get_modes_rects(modes, sizeof(modes)/sizeof(modes[0]));
    }

    return NULL;
}

#ifdef _MGRM_PROCESSES
int
Nexus_ServerOnGetSurface(REQ_NEXUS_GETSURFACE * request,
                         REP_NEXUS_GETSURFACE * reply)
{
    memcpy(reply, &reply_primary_surface_info,
           sizeof(reply_primary_surface_info));
    return 0;
}
#endif

static int
Nexus_ClientGetPrimarySurface(int width, int height, unsigned int *pPitch,
                              void **pPixels,
                              NexusPrivate_HWSurface_hwdata ** pHwdata)
{
#ifdef _MGRM_PROCESSES
    REQUEST cmd;
    REQ_NEXUS_GETSURFACE request;
    REP_NEXUS_GETSURFACE reply;

    cmd.id = REQID_NEXUS_CLIENT_GET_SURFACE;
    cmd.data = &request;
    cmd.len_data = sizeof(request);

    if (ClientRequest(&cmd, &reply, sizeof(reply)) < 0)
    {
        fprintf(stderr, "ClientRequest() failed\n");
        return -1;
    }
    if (reply.hwdata.surfaceHandle == NULL)
    {
        fprintf(stderr, "Failed to get main surface\n");
        return -1;
    }
    *pPitch = reply.pitch;
    *pHwdata = NexusPrivate_SurfaceDup(&reply.hwdata);
    *pPixels = NexusPrivate_SurfaceGetMemory(reply.hwdata.surfaceHandle);

#endif
    return 0;
}

static GAL_Surface *
NEXUS_SetVideoMode(_THIS, GAL_Surface * current, int width, int height, int bpp,
                   Uint32 flags)
{
    NexusPrivate_HWSurface_hwdata *hwdata;
    Uint32  pitch;
    void   *pixels;

    if (current->hwdata)
    {
        fprintf(stderr, "Should SetVideoMode from a SWSurface only!\n");
        return NULL;
    }

    if (NexusPrivate_get_mode(bpp, width, height) < 0)
    {
        fprintf(stderr, "unsupported mode %dx%d-%d\n", width, height, bpp);
        return NULL;
    }

    if (NexusPrivate_init(&this->hidden->privateData, ISSERVER, width, height) < 0)
    {
        return NULL;
    }

    /* Create or Get surface */
    if (ISSERVER)
    {
        if (NexusPrivate_CreateSurface
            (this->hidden->privateData, width, height, &pitch, &pixels,
             &hwdata) < 0)
        {
            return NULL;
        }
    }
    else
    {
        if (Nexus_ClientGetPrimarySurface
            (width, height, &pitch, &pixels, &hwdata) < 0)
        {
            fprintf(stderr, "Nexus_ClientGetPrimarySurface() Failed\n");
            return NULL;
        }
    }

    /* Clear screen */
    if (ISSERVER)
    {
        if (NexusPrivate_FillRect
            (this->hidden->privateData, hwdata, 0, 0, width, height,
             0x00000000))
        {
            NexusPrivate_FreeSurface(NULL, hwdata);
            return NULL;
        }
    }

    /* Select the surface as the primary surface */
    if (NexusPrivate_SelectSurface (this->hidden->privateData, hwdata, ISSERVER))
    {
        NexusPrivate_FreeSurface(NULL, hwdata);
        return NULL;
    }

    current->w = width;
    current->h = height;
    current->pitch = pitch;
    current->pixels = pixels;
    current->hwdata = (void *)hwdata;
    current->flags |= GAL_HWSURFACE;

#ifdef _MGRM_PROCESSES
    if (ISSERVER)
    {
        reply_primary_surface_info.width = width;
        reply_primary_surface_info.height = height;
        reply_primary_surface_info.pitch = pitch;
        memcpy(&reply_primary_surface_info.hwdata, hwdata,
               sizeof(reply_primary_surface_info.hwdata));
    }
#endif

    if (!GAL_ReallocFormat
        (current, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
    {
        if (hwdata)
        {
            NexusPrivate_FreeSurface(NULL, hwdata);
        }
        return NULL;
    }

    return current;
}

static int
NEXUS_AllocHWSurface(_THIS, GAL_Surface * surface)
{
    NexusPrivate_HWSurface_hwdata *hwdata;

#if 0
    fprintf(stderr, "Not supported yet: NEXUS_AllocHWSurface()\n");
    surface->hwdata = (void *)hwdata;
    surface->flags &= ~GAL_HWSURFACE;
    return -1;
#else
    if (NexusPrivate_CreateSurface
        (this->hidden->privateData, surface->w, surface->h, &surface->pitch,
         &surface->pixels, &hwdata) < 0)
    {
        surface->hwdata = (void *)hwdata;
        surface->flags &= ~GAL_HWSURFACE;
        return -1;
    }
    else
    {
        surface->hwdata = (void *)hwdata;
        surface->flags |= GAL_HWSURFACE;
        return 0;
    }
#endif
}

static void
NEXUS_FreeHWSurface(_THIS, GAL_Surface * surface)
{
    if (surface->hwdata)
    {
        NexusPrivate_FreeSurface(NULL,
                                 (NexusPrivate_HWSurface_hwdata *) surface->
                                 hwdata);
        surface->hwdata = NULL;
        surface->pixels = NULL;
    }
}

static int
NEXUS_FillHWRect(_THIS, GAL_Surface * dst, GAL_Rect * rect, Uint32 color)
{
    return NexusPrivate_FillRect(this->hidden->privateData,
                                 (NexusPrivate_HWSurface_hwdata *) dst->hwdata,
                                 rect->x, rect->y, rect->w, rect->h, color);
}

static int
NEXUS_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color * colors)
{
    /* do nothing */
    return (1);
}

static void 
NEXUS_UpdateSurfaceRects(_THIS, GAL_Surface* surface, int numrects, GAL_Rect *rects)
{
    NexusPrivate_Flush(this, (NexusPrivate_HWSurface_hwdata *) surface->hwdata);
}

/* Note:  If we are terminated, this could be called in the middle of
   another video routine -- notably UpdateRects.
 */
static void
NEXUS_VideoQuit(_THIS)
{
    NexusPrivate_close(this->hidden->privateData, ISSERVER);
}

#endif /* _MGGAL_NEXUS */
