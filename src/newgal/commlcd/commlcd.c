/*
**  $Id: commlcd.c 8944 2007-12-29 08:29:16Z xwyan $
**  
**  commlcd.c: CommonLCD NEWGAL video driver.
**    Support for common LCD driver under eCos, uC/OS-II, VxWorks, ...
**
**  Copyright (C) 2003 ~ 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#ifdef _MGGAL_COMMLCD

#include "commlcd.h"

#define COMMLCDVID_DRIVER_NAME "commlcd"

/* Initialization/Query functions */
static int COMMLCD_VideoInit (_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **COMMLCD_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *COMMLCD_SetVideoMode (_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int COMMLCD_SetColors (_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void COMMLCD_VideoQuit (_THIS);

/* Hardware surface functions */
static int COMMLCD_AllocHWSurface (_THIS, GAL_Surface *surface);
static void COMMLCD_FreeHWSurface (_THIS, GAL_Surface *surface);

/* COMMLCD driver bootstrap functions */

static int COMMLCD_Available(void)
{
    return 1;
}

static void COMMLCD_DeleteDevice(GAL_VideoDevice *device)
{
    free (device->hidden);
    free (device);
}

static GAL_VideoDevice *COMMLCD_CreateDevice(int devindex)
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
    device->VideoInit = COMMLCD_VideoInit;
    device->ListModes = COMMLCD_ListModes;
    device->SetVideoMode = COMMLCD_SetVideoMode;
    device->SetColors = COMMLCD_SetColors;
    device->VideoQuit = COMMLCD_VideoQuit;
#ifndef _MGRM_THREADS
    device->RequestHWSurface = NULL;
#endif
    device->AllocHWSurface = COMMLCD_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = COMMLCD_FreeHWSurface;
    
    device->free = COMMLCD_DeleteDevice;
    return device;
}

VideoBootStrap COMMLCD_bootstrap = {
    COMMLCDVID_DRIVER_NAME, "Common LCD video driver",
    COMMLCD_Available, COMMLCD_CreateDevice
};

static int COMMLCD_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    fprintf (stderr, "NEWGAL>COMMLCD: Calling init method!\n");

    /* Initialize LCD screen */
    if (__mg_commlcd_ops.init ()) {
        fprintf (stderr, "NEWGAL>COMMLCD: Couldn't initialize LCD\n");
        return -1;
    }

    /* Determine the screen depth (use default 8-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;

    /* We're done! */
    return(0);
}

static GAL_Surface *COMMLCD_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    struct commlcd_info li;	
    if (__mg_commlcd_ops.getinfo (&li)) {
        fprintf (stderr, "NEWGAL>COMMLCD: "
                "Couldn't get the LCD information\n");
        return NULL;
    }

    this->hidden->w = li.width;
    this->hidden->h = li.height;
    this->hidden->pitch = li.rlen;
    this->hidden->fb = li.fb;

    memset (li.fb, 0, li.rlen * height);

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, li.bpp, 0, 0, 0, 0)) {
        if (__mg_commlcd_ops.release)
            __mg_commlcd_ops.release ();

        this->hidden->fb = NULL;
        fprintf (stderr, "NEWGAL>COMMLCD: "
                "Couldn't allocate new pixel format for requested mode\n");
        return NULL;
    }

    /* Set up the new mode framebuffer */
    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w = this->hidden->w;
    current->h = this->hidden->h;
    current->pitch = this->hidden->pitch;
    current->pixels = this->hidden->fb;

    /* We're done */
    return current;
}

static void COMMLCD_VideoQuit(_THIS)
{
    if (this->screen && this->screen->pixels) {
        this->screen->pixels = NULL;
    }

    if (__mg_commlcd_ops.release)
        __mg_commlcd_ops.release ();

    return;
}

static GAL_Rect **COMMLCD_ListModes (_THIS, GAL_PixelFormat *format, 
                Uint32 flags)
{
        return (GAL_Rect **) -1;
}

/* We don't actually allow hardware surfaces other than the main one */
static int COMMLCD_AllocHWSurface (_THIS, GAL_Surface *surface)
{
    return -1;
}

static void COMMLCD_FreeHWSurface (_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int COMMLCD_SetColors (_THIS, int firstcolor, int ncolors, 
                GAL_Color *colors)
{
    if (__mg_commlcd_ops.setclut)
    	return __mg_commlcd_ops.setclut (firstcolor, ncolors, colors);

    return 0;
}

#endif /* _MGGAL_COMMLCD */

