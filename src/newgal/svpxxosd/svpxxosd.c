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
** $Id: svpxxosd.c 8944 2007-12-29 08:29:16Z xwyan $
**
** svpxxosd.c: graphic engine for WINBOND OSD.
**
** Copyright (C) 2003 ~ 2006 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Current maintainer: <liu jianjun> (<falistar@minigui.org>).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_SVPXXOSD

#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#include "svpxxosd.h"


#define SVPXXOSDVID_DRIVER_NAME "svpxxosd"
#include "svphead.h"

/* Initialization/Query functions */
static int SVPXXOSD_VideoInit (_THIS, GAL_PixelFormat * vformat);
static GAL_Rect **SVPXXOSD_ListModes (_THIS, GAL_PixelFormat * format,
                      Uint32 flags);
static GAL_Surface *SVPXXOSD_SetVideoMode (_THIS, GAL_Surface * current,
                       int width, int height, int bpp,
                       Uint32 flags);
static int SVPXXOSD_SetColors (_THIS, int first, int count,
                   GAL_Color * palette);
static void SVPXXOSD_VideoQuit (_THIS);

/* Hardware surface functions */
static int SVPXXOSD_AllocHWSurface (_THIS, GAL_Surface * surface);
static void SVPXXOSD_FreeHWSurface (_THIS, GAL_Surface * surface);

/* SVPXXOSD driver bootstrap functions */

static int
SVPXXOSD_Available (void)
{
    return (1);
}

static void
SVPXXOSD_DeleteDevice (GAL_VideoDevice * device)
{
    free (device->hidden);
    free (device);
}

static GAL_VideoDevice * SVPXXOSD_CreateDevice (int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *) malloc (sizeof (GAL_VideoDevice));
    if (device) {
        memset (device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *)
        malloc ((sizeof *device->hidden));
    }

    if ((device == NULL) || (device->hidden == NULL)) {
        GAL_OutOfMemory ();
        if (device) {
            free (device);
        }
        return (0);
    }
    memset (device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = SVPXXOSD_VideoInit;
    device->ListModes = SVPXXOSD_ListModes;
    device->SetVideoMode = SVPXXOSD_SetVideoMode;
    device->SetColors = SVPXXOSD_SetColors;
    device->VideoQuit = SVPXXOSD_VideoQuit;
#ifndef _MGRM_THREADS
    device->RequestHWSurface = NULL;
#endif
    device->AllocHWSurface = SVPXXOSD_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = SVPXXOSD_FreeHWSurface;
    device->UpdateRects = NULL;

    device->free = SVPXXOSD_DeleteDevice;

    return device;
}

VideoBootStrap SVPXXOSD_bootstrap = {
    SVPXXOSDVID_DRIVER_NAME, "SVPxx OSD video driver",
    SVPXXOSD_Available, SVPXXOSD_CreateDevice
};

static int SVPXXOSD_VideoInit (_THIS, GAL_PixelFormat * vformat)
{
    VIDEO_MODE_INFO info;
#ifndef _SVPXXOSD_8BPP_     
    uint16 *ptrVideo;
#else
    uint8 *ptrVideo;
#endif
    
    XY_SIZE desk_sz;
    //set default mode :16bit graphics display,4CIF capture,CIF encode and CIF decode
    SetW9961Application (AP_GFX_ENC_DEC, BPP_16, FOUR_CIF, CIF, CIF);

    //set display mode
#ifndef _SVPXXOSD_8BPP_     
    SetDisplayMode (MODE_BPP_16);
#else
    SetDisplayMode (MODE_BPP_8);
#endif
    
    if (GetVideoModeInfo (&info) == Successful) {
        printf ("SVPXXOSD: screen width = %d\n", info.vmiScreenWidth);
        printf ("SVPXXOSD: screen height= %d\n", info.vmiScreenHeight);
        printf ("SVPXXOSD: screen bpp   = %d\n", info.vmiBpp);
        printf ("SVPXXOSD: screen stride= %d\n", info.vmiStride);
    }
    else
        goto fail;
    GetDesktopSize(&desk_sz);
    printf("GetDesktopSize(&desk_sz): %dx%d\n",desk_sz.w,desk_sz.h);
    //get graphics display buffer
#ifndef _SVPXXOSD_8BPP_     
    ptrVideo = (uint16 *) GetVideoBaseAddr ();   
#else 
    ptrVideo = (uint8 *) GetVideoBaseAddr ();   
#endif
    
    if (ptrVideo == 0)
        goto fail;

    printf ("SVPXXOSD: graphics base = 0x%lx\n", ptrVideo);
    
    this->hidden->w = info.vmiScreenWidth;
    this->hidden->h = info.vmiScreenHeight;
    this->hidden->pitch = info.vmiStride;
    this->hidden->fb = ptrVideo;

    this->hidden->status = 2;

    if (info.vmiBpp == 16) {
        vformat->BitsPerPixel = 16;
        vformat->BytesPerPixel = 2;
        vformat->Bmask = 0x0000F800;
        vformat->Gmask = 0x000007E0;
        vformat->Rmask = 0x0000001F;
    }
    else if ( info.vmiBpp == 8 ){
        vformat->BitsPerPixel = 8;
        vformat->BytesPerPixel = 1;
        vformat->Bmask = 0xE0;
        vformat->Gmask = 0x1C;
        vformat->Rmask = 0x03;
    }

    /* We're done! */
    return (0);

  fail:
    return -1;
}

static GAL_Rect ** SVPXXOSD_ListModes (_THIS, GAL_PixelFormat * format, Uint32 flags)
{
    return (GAL_Rect**)-1;
}

static GAL_Surface * SVPXXOSD_SetVideoMode (_THIS, GAL_Surface * current,
               int width, int height, int bpp, Uint32 flags)
{
    /* Set up the new mode framebuffer */
    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w = this->hidden->w;
    current->h = this->hidden->h;
    current->pitch = this->hidden->pitch;
    current->pixels = this->hidden->fb;

    /* We're done */
    return (current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int SVPXXOSD_AllocHWSurface (_THIS, GAL_Surface * surface)
{
    return (-1);
}

static void SVPXXOSD_FreeHWSurface (_THIS, GAL_Surface * surface)
{
    surface->pixels = NULL;
}
 
static GAL_Color pal_tmp[300];

static int SVPXXOSD_SetColors (_THIS, int first, int count, GAL_Color * palette)
{
#ifdef _SVPXXOSD_8BPP_
    int i;

    for (i = first; i < first + count; i++) {
        pal_tmp[i].r = palette[i].b ;
        pal_tmp[i].g = palette[i].g ;
        pal_tmp[i].b = palette[i].r ;
    }
    
    sETpALETTE (first, pal_tmp, count);
#endif /* _SVPXXOSD_8BPP_ */
    return 0;
}

static void SVPXXOSD_VideoQuit (_THIS)
{
    if (this->hidden->status != 2)
        return;
    this->hidden->status = 1;
    return;
}

#endif /* _MGGAL_SVPXXOSD */
