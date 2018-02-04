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
**  $Id: em85xxosd.c 8944 2007-12-29 08:29:16Z xwyan $
**
**  em85xxosd.c: NEWGAL driver for EM85xx OSD.
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sched.h>

#include "common.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#include "realmagichwl_userland/caribbean_plainc.h"
#include "realmagichwl_kernelland/realmagichwl.h"
#include "realmagichwl_userland/realmagichwl_userland_api.h"
#include "realmagichwl_kernelland/include/rm84cmn.h"

#include "em85xxosd.h"

// rgb 2 yuv:
// Y  =  0.257r + 0.504g + 0.098b + 16
// Cb = -0.148r - 0.291g + 0.439b + 128
// Cr =  0.439r + 0.368g + 0.071b + 128
static RMuint8 rgb2y (RMuint8 r, RMuint8 g, RMuint8 b)
{
	RMint32 f = 257*(RMint32)r + 504*(RMint32)g + 98*(RMint32)b + 16000;
	if (f > 255000)
		f = 255000;
	f = f / 1000;
	return (RMuint8)f;
}
static RMuint8 rgb2u (RMuint8 r, RMuint8 g, RMuint8 b)
{
	RMint32 f = -148*(RMint32)r - 291*(RMint32)g + 439*(RMint32)b + 128000;
	if (f > 255000)
		f = 255000;
	if (f < 0)
		f = 0;
	f = f / 1000;
	return (RMuint8)f;
}
static RMuint8 rgb2v (RMuint8 r, RMuint8 g, RMuint8 b)
{
	RMint32 f = 439*(RMint32)r - 368*(RMint32)g - 71*(RMint32)b + 128000;
	if (f > 255000)
		f = 255000;
	if (f < 0)
		f = 0;
	f = f / 1000;
	return (RMuint8)f;
}

#define EM85XXOSDVID_DRIVER_NAME "em85xxosd"

/* Initialization/Query functions */
static int EM85XXOSD_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **EM85XXOSD_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *EM85XXOSD_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int EM85XXOSD_SetColors(_THIS, int first, int count, GAL_Color *palette);
static void EM85XXOSD_VideoQuit(_THIS);

/* Hardware surface functions */
static int EM85XXOSD_AllocHWSurface(_THIS, GAL_Surface *surface);
static void EM85XXOSD_FreeHWSurface(_THIS, GAL_Surface *surface);
static void EM85XXOSD_UpdateRects (_THIS, int numrects, GAL_Rect *rects);

/* EM85XXOSD driver bootstrap functions */

static int EM85XXOSD_Available(void)
{
    return(1);
}

static char stack_updater [512];

static int task_do_update (void* data)
{
    _THIS;
    this = data;

    while (this->hidden->status == 2) {
        usleep (50000);    // 50 ms

        if (this->hidden->dirty) {
            asm ("mcr p15, 0, r0, c7, c10, 0\n");
            RUA_OSDFB_REFRESH (this->hidden->handle, 0);   // manually refresh now
            this->hidden->dirty = FALSE;
        }
    }

    return 0;
}

static void EM85XXOSD_DeleteDevice(GAL_VideoDevice *device)
{
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice *EM85XXOSD_CreateDevice(int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if ( device ) {
        memset(device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *)
                malloc((sizeof *device->hidden));
    }
    if ( (device == NULL) || (device->hidden == NULL) ) {
        GAL_OutOfMemory();
        if ( device ) {
            free(device);
        }
        return(0);
    }
    memset(device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = EM85XXOSD_VideoInit;
    device->ListModes = EM85XXOSD_ListModes;
    device->SetVideoMode = EM85XXOSD_SetVideoMode;
    device->SetColors = EM85XXOSD_SetColors;
    device->VideoQuit = EM85XXOSD_VideoQuit;
#ifndef _MGRM_THREADS
    device->RequestHWSurface = NULL;
#endif
    device->AllocHWSurface = EM85XXOSD_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = EM85XXOSD_FreeHWSurface;
    device->UpdateRects = EM85XXOSD_UpdateRects;

    device->free = EM85XXOSD_DeleteDevice;

    return device;
}

VideoBootStrap EM85XXOSD_bootstrap = {
    EM85XXOSDVID_DRIVER_NAME, "EM85xx OSD video driver",
    EM85XXOSD_Available, EM85XXOSD_CreateDevice
};

#if 0
static void test_fb (RUA_handle handle, OSDBuffer* osdbuffer)
{
    int i;
    Uint8* pOSDHeader = (Uint8*)(osdbuffer->framebuffer);
    Uint8* pPalette = pOSDHeader + 8;
    Uint8* pBits = pPalette + 1024;

    for (i=0; i<256; i++) {
        pPalette[0] = 0x00;                // alpha (0x00 = transparent, 0xff = opaque)
        if (i)
            pPalette[0] = 0xff;
        pPalette[1] = rgb2y (0, 0, 0);    // y
        pPalette[2] = rgb2u (0, 0, 0);    // u
        pPalette[3] = rgb2v (0, 0, 0);    // v
        pPalette += 4;
    }
    // some pre-defined colours
    pPalette = pOSDHeader + 8;
    pPalette += 4;
    pPalette[0] = 0xff;
    pPalette[1] = rgb2y (0, 0, 0);            // y
    pPalette[2] = rgb2u (0, 0, 0);            // u
    pPalette[3] = rgb2v (0, 0, 0);            // v
    pPalette += 4;
    pPalette[0] = 0xff;
    pPalette[1] = rgb2y (0xff, 0xff, 0);    // y
    pPalette[2] = rgb2u (0xff, 0xff, 0);    // u
    pPalette[3] = rgb2v (0xff, 0xff, 0);    // v
    pPalette += 4;
    pPalette[0] = 0x40;
    pPalette[1] = rgb2y (0xff, 0xff, 0xff);        // y
    pPalette[2] = rgb2u (0xff, 0xff, 0xff);        // u
    pPalette[3] = rgb2v (0xff, 0xff, 0xff);        // v

    for (i=0; i<osdbuffer->width * osdbuffer->height; i++)
        pBits[i] = 2;

    asm ("mcr p15, 0, r0, c7, c10, 0\n");
    RUA_OSDFB_REFRESH (handle, 0);   // manually refresh now
}
#endif

#ifdef _MGGAL_EM85XXYUV
extern RUA_handle __mg_em85xx_rua_handle;
#endif

static int EM85XXOSD_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    unsigned long flicker;
    RUA_handle h;
    OSDBuffer osdbuffer;
    Wnd_type Wnd;
    evOutputDevice_type OutputDevice;
    evTvStandard_type TvStandard;
    evTvOutputFormat_type TvOutputFormat;

    fprintf (stderr, "NEWGAL>EM85xxOSD: Calling init method!\n");

#ifdef _MGGAL_EM85XXYUV
	h = __mg_em85xx_rua_handle;
#else
    if ((h = RUA_OpenDevice (0)) == -1) {
        fprintf (stderr, "NEWGAL>EM85xxOSD: Fatal error: can't open kernel module\n");
        return -1;
    }
#endif

    /* If we need to we can mmap the osdbuf from the device */
    this->hidden->handle = h;
    asm ("mcr p15, 0, r0, c7, c10, 0\n");
    if (RUA_OSDFB_REFRESH (this->hidden->handle, &osdbuffer) != 0) {
        fprintf (stderr, "NEWGAL>EM85xxOSD: Error getting the OSD buffer.\n");
        goto fail;
    } else {
        this->hidden->osd_buffer = osdbuffer.framebuffer;
        this->hidden->w = osdbuffer.width;
        this->hidden->h = osdbuffer.height;
        this->hidden->pitch = osdbuffer.width;
        this->hidden->fb = osdbuffer.framebuffer + 8 + 1024;
    
        osdbuffer.framebuffer[0] = 0x3e;
        osdbuffer.framebuffer[1] = ((osdbuffer.width*osdbuffer.height+1024+8) >> 16) & 0xff;
        osdbuffer.framebuffer[2] = ((osdbuffer.width*osdbuffer.height+1024+8) >>  8) & 0xff;
        osdbuffer.framebuffer[3] = ((osdbuffer.width*osdbuffer.height+1024+8) >>  0) & 0xff;
        osdbuffer.framebuffer[4] = (osdbuffer.width >> 8) & 0xff;
        osdbuffer.framebuffer[5] = (osdbuffer.width >> 0) & 0xff;
        osdbuffer.framebuffer[6] = (osdbuffer.height >> 8) & 0xff;
        osdbuffer.framebuffer[7] = (osdbuffer.height >> 0) & 0xff;

    }
    
    if (osdbuffer.bpp != 8) {
        GAL_SetError ("NEWGAL>EM85xxOSD: Not supported depth: %d.\n", vformat->BitsPerPixel);
        return -1;
    }
	//这里可以添加防止闪烁

    /* Setup the flicker filter
     * XXX - its also set in the kernel module, but it seems not to work very well
     *  We reset it here, so that the microcode as already seen an osd frame.    
     * 0 <= flicker <= 15*/
    flicker = 15;
    RUA_DECODER_SET_PROPERTY (this->hidden->handle, DECODER_SET, edecOsdFlicker, sizeof(flicker), &flicker);

    asm ("mcr p15, 0, r0, c7, c10, 0\n");
    ioctl (h, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0);

    /* set the tv output to be ntsc */
    Wnd.x = 0;
    Wnd.y = 0;
    Wnd.w = 720;
    Wnd.h = 480;
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, evValidWindow, sizeof(Wnd), &Wnd);

    TvOutputFormat = evTvOutputFormat_COMPOSITE;
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, evTvOutputFormat, sizeof(TvOutputFormat), &TvOutputFormat);

    TvStandard = evTvStandard_NTSC;
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, evTvStandard, sizeof(TvStandard), &TvStandard);

    OutputDevice = evOutputDevice_TV;
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, evOutputDevice, sizeof(OutputDevice), &OutputDevice);

    /* 
     * set the osd window destination
     * do not scale - just center the image
     */

    Wnd.w = 680;
    Wnd.h = 480;
    Wnd.x = (720 - Wnd.w)/2;
    Wnd.y = (480 - Wnd.h)/2;
    RUA_DECODER_SET_PROPERTY (h, OSD_SET, eOsdDestinationWindow, sizeof(Wnd), &Wnd);

    Wnd.x = 0;
    Wnd.y = 0;
    Wnd.w = 720;
    Wnd.h = 480;
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, evDestinationWindow, sizeof(Wnd), &Wnd);

    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;
    vformat->Rmask = 0x00;
    vformat->Gmask = 0x00;
    vformat->Bmask = 0x00;

    this->hidden->status = 2;
    this->hidden->dirty = FALSE;

#if 0
    test_fb (this->hidden->handle, &osdbuffer);
    sleep (3);
#endif

    /* We're done! */
    return(0);

fail:
    return -1;
}

static GAL_Rect **EM85XXOSD_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    return (GAL_Rect **) -1;
}

static GAL_Surface *EM85XXOSD_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    /* Set up the new mode framebuffer */
    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN | GAL_HWPALETTE;
    current->w = this->hidden->w;
    current->h = this->hidden->h;
    current->pitch = this->hidden->pitch;
    current->pixels = this->hidden->fb;

    if (!GAL_ReallocFormat (current, 8, 0, 0, 0, 0) ) {
        return (NULL);
    }

    clone (task_do_update, stack_updater + 512, CLONE_VM | CLONE_FS | CLONE_FILES, this);

    /* We're done */
    return (current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int EM85XXOSD_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return(-1);
}
static void EM85XXOSD_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

#define START       0
#define END         65536
#define PRECISION   20

/* This trick transform [0..255] range into [0..65535] range (instead of about 0..255*256) */
#define RANGE8TO16(x) (((x)<<8)|(x))

// see video demystified page 43
static void gammacorrectedrgbtoyuv (Uint16 R, Uint16 G, Uint16 B, Uint16 *y, Uint16 *u, Uint16 *v)
{
    long yraw, uraw, vraw;
    
    yraw = ( 257*R  +504*G + 98*B)/1000 + RANGE8TO16(16);
    uraw = (-148*R  -291*G +439*B)/1000 + RANGE8TO16(128);
    vraw = ( 439*R  -368*G - 71*B)/1000 + RANGE8TO16(128);

    /* Obviously the computation of yraw garantees >= RANGE8TO16(16) ;-)
       This is also true for uraw and vraw */
    
    *y = MAX(MIN(yraw,RANGE8TO16(235)),RANGE8TO16(16)); 
    *u = MAX(MIN(uraw,RANGE8TO16(240)),RANGE8TO16(16));
    *v = MAX(MIN(vraw,RANGE8TO16(240)),RANGE8TO16(16));
}

static int EM85XXOSD_SetColors(_THIS, int first, int count, GAL_Color *palette)
{
    int         i;
    Uint8*      pal = this->hidden->osd_buffer + 8;
    GAL_Color*  p;


    /* convert palette to quasar format*/
    pal += first * 4;
    p = palette;
    for (i = first; i < (first + count); i++) {
#if 0
        Uint16 Y, U, V;
        Uint16 R, G, B;

        /* RGB->YUVe computation. */
        R = RANGE8TO16 (p->r);
        G = RANGE8TO16 (p->g);
        B = RANGE8TO16 (p->b);
    
        gammacorrectedrgbtoyuv (R, G, B, &Y, &U, &V);

        /* hardcode alpha blending values */
        if (i == 0)
            pal[0] = 0x00;
        else if (i == 6)
            pal[0] = 0x66;
        else if (i == 15)
            pal[0] = 0x80;
        else if (i == 242)
            pal[0] = 0x80;
        else
            pal[0] = 0xff;

        pal[1] = (Uint8)(Y >> 8);
        pal[2] = (Uint8)(U >> 8);
        pal[3] = (Uint8)(V >> 8);
#else
        if (i == 0)
            pal[0] = 0x00;
        else if (i == 6)
            pal[0] = 0x66;
        else if (i == 15)
            pal[0] = 0x80;
        else if (i == 242)
            pal[0] = 0x80;
        else
            pal[0] = 0xff;

        pal[1] = rgb2y (p->r, p->g, p->b);
        pal[2] = rgb2u (p->r, p->g, p->b);
        pal[3] = rgb2v (p->r, p->g, p->b);
#endif

        pal += 4;
        p ++;
    }

    return 1;
}

static void EM85XXOSD_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    this->hidden->dirty = TRUE;
}

static void EM85XXOSD_VideoQuit(_THIS)
{
    if (this->hidden->status != 2)
        return;
    this->hidden->status = 1;

#ifndef _MGGAL_EM85XXYUV
    RUA_ReleaseDevice (this->hidden->handle);
#endif

    return;
}

