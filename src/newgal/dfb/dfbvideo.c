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
**  This is the NEWGAL common DirectFB engine, base on DirectFB 1.0.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#ifdef _MGGAL_DFB

#include <linux/fb.h>
#include <directfb.h>

#include "dfbvideo.h"


#define DFBVID_DRIVER_NAME "dfb"

#ifdef _MGRM_PROCESSES
#   include "client.h"
#   define ISSERVER (mgIsServer)
#else
#   define ISSERVER (1)
#endif


/* TODO: include yourself platform code here */
#ifdef _MGGAL_ST7167
#   include "st7167_video.h"
#else
    /* no platform define mgChangeDeviceCallback do noting */
    int mgChangeDeviceCallback (GAL_VideoDevice* device) { return 0; }
#endif


/* The root interface of DirectFB,
 * Note: dfb ial need this two variables. */
IDirectFB *__mg_dfb = NULL;

/* The primary layer of DirectFB */
IDirectFBSurface *pFrameBuffer = NULL;


/* Initialization/Query functions */
static int DFB_VideoInit (_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **DFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *DFB_SetVideoMode (_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int DFB_SetColors (_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void DFB_VideoQuit (_THIS);
static void DFB_UpdateRects (_THIS, int numrects, GAL_Rect *rects);

/* Hardware surface functions */
static void DFB_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply);
static int DFB_AllocHWSurface (_THIS, GAL_Surface *surface);
static void DFB_FreeHWSurface (_THIS, GAL_Surface *surface);
static int DFB_HWAccelBlit (GAL_Surface *src, GAL_Rect *srcrect, GAL_Surface *dst, GAL_Rect *dstrect);
static int DFB_CheckHWBlit (_THIS, GAL_Surface * src, GAL_Surface * dst);
static int DFB_FillHWRect (_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color);


/* common dfb function address */
GAL_FunctionTable mgGALFuncTable = {
    DFB_VideoInit,
    DFB_ListModes,
    DFB_SetVideoMode,
    DFB_SetColors,
    DFB_UpdateRects,
    DFB_VideoQuit,
#ifdef _MGRM_PROCESSES
    DFB_RequestHWSurface,
#endif
    DFB_AllocHWSurface,
    DFB_CheckHWBlit,
    DFB_HWAccelBlit,
    DFB_FillHWRect,
    DFB_FreeHWSurface,
    NULL,
};


static int DFB_Available (void)
{
    return (1);
}

static void DFB_DeleteDevice (GAL_VideoDevice *device)
{
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice *DFB_CreateDevice (int devindex)
{
    GAL_VideoDevice *device;
    
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if (device) {
        memset(device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *)malloc((sizeof *device->hidden));
    }
    if ((device == NULL) || (device->hidden == NULL)) {
        GAL_OutOfMemory();
        if (device) {
            free(device);
        }

        return 0;
    }

    memset(device->hidden, 0, (sizeof *device->hidden));
     
    if (ISSERVER) {
        device->VideoInit = DFB_VideoInit;
        device->ListModes = DFB_ListModes;
        device->SetVideoMode = DFB_SetVideoMode;
        device->SetColors = DFB_SetColors;
        device->VideoQuit = DFB_VideoQuit;
        device->UpdateRects = DFB_UpdateRects;
#ifdef _MGRM_PROCESSES
        device->RequestHWSurface = DFB_RequestHWSurface;
#endif
        device->AllocHWSurface = DFB_AllocHWSurface;
        device->CheckHWBlit = DFB_CheckHWBlit;
        device->FillHWRect = DFB_FillHWRect;
        device->SetHWColorKey = NULL;
        device->SetHWAlpha = NULL;
        device->FreeHWSurface = DFB_FreeHWSurface;
        device->free = DFB_DeleteDevice;
       
        /* platform callback function */
        if ( mgChangeDeviceCallback(device) != 0 ) {
            fprintf (stderr, "NEWGAL>DFB: Change Device failed\n");
            return 0;
        }
    }

    return device;
}

VideoBootStrap DFB_bootstrap = {
    DFBVID_DRIVER_NAME, "Video driver on DirectFB",
    DFB_Available, DFB_CreateDevice
};

static int DFB_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    DFBSurfacePixelFormat pixelformat;
    DFBSurfaceDescription dsc;
    DFBGraphicsDeviceDescription dev_dsc;

    /* only the server init dfb and create the primary surface */
    if (ISSERVER) {
        /* Initialise DirectFB. */
        RETCHECK(DirectFBInit(NULL, NULL) != DFB_OK, "dfb init failed", goto init_err);
        
        /* Create the DirectFB root interface. */
        RETCHECK(DirectFBCreate(&__mg_dfb) != DFB_OK, "dfb create failed", goto init_err);
        
        /* Use full screen mode so that a surface has full control of a layer and no windows are created. */
        RETCHECK(__mg_dfb->SetCooperativeLevel(__mg_dfb, DFSCL_FULLSCREEN) != DFB_OK, 
                "SetCooperativeLevel failed", goto init_err);
        
        /* Set the surface description - specify which fields are set and set them. */
        dsc.flags = DSDESC_CAPS; 
        dsc.caps  = DSCAPS_PRIMARY | DSCAPS_VIDEOONLY;
        
        /* Create the frame buffer primary surface by passing our surface description. */
        RETCHECK(__mg_dfb->CreateSurface(__mg_dfb, &dsc, &pFrameBuffer) != DFB_OK, 
                "dfb create primary failed", goto init_err);
    }

    this->hidden->framebuffer = pFrameBuffer;

    /* Determine the screen depth and pixel format */
    pFrameBuffer->GetPixelFormat(pFrameBuffer, &pixelformat);

    /* TODO: add your platform pixelformt here */
    switch (pixelformat) {
        case DSPF_ARGB1555:
            vformat->BitsPerPixel = 16; 
            vformat->BytesPerPixel = 2;
            break;

        case DSPF_RGB555:
            vformat->BitsPerPixel = 16; 
            vformat->BytesPerPixel = 2;
            break;

        case DSPF_RGB32:
            vformat->BitsPerPixel = 32;
            vformat->BytesPerPixel = 4;
            break;

		case DSPF_ARGB:
			vformat->BitsPerPixel = 32;
			vformat->BytesPerPixel = 4;
			break;

        default:
            fprintf(stderr, "NEWGAL>DFB: Unsupported pixel format:%x \n", pixelformat);
            vformat->BitsPerPixel = 8;
            vformat->BytesPerPixel = 1;
            break;
    }

    RETCHECK(__mg_dfb->GetDeviceDescription(__mg_dfb, &dev_dsc) != DFB_OK, 
            "dfb get dev_dsc failed", goto init_err); 
 
    /* set the acceleration info flags */
    this->info.hw_available = 1;
    this->info.video_mem = dev_dsc.video_memory / 1024;
    this->info.blit_fill = 1;
    this->info.blit_hw = 1;
    this->info.blit_hw_A = 1;
    this->info.blit_hw_CC = 1;

    /* We're done! */
    return 0;

init_err:
    fprintf(stderr, "NEWGAL>DFB: Init Error\n");
    return -1;
}

static GAL_Rect **DFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags)
{
	return (GAL_Rect **)-1;
}

static void DFB_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
	__mg_dfb->WaitForSync(__mg_dfb);
}

#if 0
static void *task_do_update(void *data)
{
	_THIS;
	//IDirectFBSurface *pFrameBuffer = this->hidden->framebuffer;

	this = data;

	while(this->hidden->shadow) {
		usleep(100*1000);

		if(this->hidden->dirty) {
			pthread_mutex_lock(&this->hidden->lock);

			fprintf(stderr, ">>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
			pFrameBuffer->Flip(pFrameBuffer, NULL, triple ? DSFLIP_ONSYNC : DSFLIP_WAITFORSYNC );
			fprintf(stderr, "<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

			pthread_mutex_unlock(&this->hidden->lock);

			this->hidden->dirty = FALSE;
		}
	}
}
#endif

static GAL_Surface *DFB_SetVideoMode (_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    if (!ISSERVER) {
        current = current_video->screen;
        return (current);
    }

    int depth;
    Uint32 Rmask, Gmask, Bmask, Amask;
    DFBSurfacePixelFormat pixelformat;

    pFrameBuffer->GetSize(pFrameBuffer, &current->w, &current->h);

    pFrameBuffer->GetPixelFormat(pFrameBuffer, &pixelformat);

    /* TODO: add your platform pixelformt here */
    switch (pixelformat) {
        case DSPF_ARGB1555:
            depth = 16;
            Amask = 0x8000;
            Rmask = 0x7C00;
            Gmask = 0x03E0;
            Bmask = 0x001F;
            break;

        case DSPF_RGB555:
            depth = 16;
            Rmask = 0x7C00;
            Gmask = 0x03E0;
            Bmask = 0x001F;
            break;

        case DSPF_RGB16:
            depth = 16;
            Rmask = 0xF800;
            Gmask = 0x07E0;
            Bmask = 0x001F;
            break;

        case DSPF_RGB24:
            depth = 24;
            Rmask = 0x00FF0000;
            Gmask = 0x0000FF00;
            Bmask = 0x000000FF;
            break;

        case DSPF_RGB32:
            depth = 32;
            Rmask = 0x00FF0000;
            Gmask = 0x0000FF00;
            Bmask = 0x000000FF;
            break;

        case DSPF_ARGB:
            depth = 32;
            Amask = 0xFF000000;
            Rmask = 0x00FF0000;
            Gmask = 0x0000FF00;
            Bmask = 0x000000FF;
            break;

        case DSPF_ARGB2554:
            depth = 16;
            Amask = 0xC000;
            Rmask = 0x3E00;
            Gmask = 0x01F0;
            Bmask = 0x000F;
            break;

        case DSPF_ARGB4444:
            depth = 16;
            Amask = 0xF000;
            Rmask = 0x0F00;
            Gmask = 0x00F0;
            Bmask = 0x000F;
            break;

        default:
            fprintf(stderr, "NEWGAL>DFB: Unsupported pixel format: %x\n", pixelformat);
            return (NULL);
    }

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat(current, depth, Rmask, Gmask, Bmask, Amask)) {
        fprintf(stderr, "NEWGAL>DFB: Couldn't allocate new pixel format for requested mode\n");
        return (NULL);
    }

    /* Set up the new mode framebuffer */
    current->flags = GAL_FULLSCREEN | GAL_HWSURFACE;
    this->hidden->w = current->w;
    this->hidden->h = current->h;
    
	pFrameBuffer->Lock(pFrameBuffer, DSLF_READ | DSLF_WRITE, &current->pixels, (int*)&current->pitch);

    /* We're done */
    return (current);
}

static int DFB_AllocHWSurface (_THIS, GAL_Surface *surface)
{
    REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, 0, NULL};
    REP_HWSURFACE reply = {0, 0, NULL};
    IDirectFBSurface *hw_surface = NULL;

    surface->hwdata = NULL;

#ifdef _MGRM_PROCESSES
    if (ISSERVER) {
        DFB_RequestHWSurface(this, &request, &reply);
    }
    else {
        REQUEST req;

        req.id = REQID_HWSURFACE;
        req.data = &request;
        req.len_data = sizeof(REQ_HWSURFACE);

        ClientRequest(&req, &reply, sizeof (REP_HWSURFACE));
    }
#else
    DFB_RequestHWSurface(this, &request, &reply);
#endif

    surface->hwdata = (struct private_hwdata*)reply.bucket;
    if (surface->hwdata == NULL)
        return -1;

    hw_surface = surface->hwdata->dfb_surface;
    if (hw_surface == NULL)
        return -1;

    surface->flags |= GAL_HWSURFACE;
	hw_surface->Lock(hw_surface, DSLF_READ | DSLF_WRITE, &surface->pixels, (int*)&surface->pitch);

    return 0;
}

static void DFB_FreeHWSurface (_THIS, GAL_Surface *surface)
{
    if (surface->hwdata == NULL) {
        surface->pixels = NULL;
        return;
    }

    REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, 0, surface->hwdata};
    REP_HWSURFACE reply = {0, 0};

    request.bucket = (void*)surface->hwdata;

#ifdef _MGRM_PROCESSES
    if (ISSERVER) {
        DFB_RequestHWSurface(this, &request, &reply);
    }
    else {
        REQUEST req;

        req.id = REQID_HWSURFACE;
        req.data = &request;
        req.len_data = sizeof (REQ_HWSURFACE);

        ClientRequest(&req, &reply, sizeof (REP_HWSURFACE));
    }
#else
    DFB_RequestHWSurface(this, &request, &reply);
#endif

    free(surface->hwdata);
    surface->pixels = NULL;
    surface->hwdata = NULL;
}

static void DFB_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply)
{
    /* alloc hw surface */
    if (request->bucket == NULL) {     
        DFBSurfaceDescription dsc;
        IDirectFBSurface *hw_surface = NULL;

        struct private_hwdata *hwdata = (struct private_hwdata *)
            calloc(1, sizeof(struct private_hwdata));
        if (hwdata == NULL)
            return;
        
        dsc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
        dsc.caps  = DSCAPS_VIDEOONLY; 
        dsc.width = request->w;
        dsc.height = request->h;

        RETCHECK(__mg_dfb->CreateSurface(__mg_dfb, &dsc, &hw_surface) != DFB_OK, "dfb create surface failed", 
                free(hwdata);hwdata=NULL);

        hwdata->dfb_surface = hw_surface;
        /* platform data will process in platform file */
        hwdata->platform_data = NULL;

        reply->bucket = (void*)hwdata;

    }
    /* free hw surface */
    else { 
        struct private_hwdata* hwdata = (struct private_hwdata*)request->bucket;
        hwdata->dfb_surface->Unlock(hwdata->dfb_surface);
        hwdata->dfb_surface->ReleaseSource(hwdata->dfb_surface);
        hwdata->dfb_surface->Release(hwdata->dfb_surface);
    }
}

static int DFB_FillHWRect (_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
{
    if ((dst->hwdata == NULL && dst != GAL_VideoSurface)) {
        fprintf(stderr, "NEWGAL>DFB: src_hwdata or dst_hwdata NULL\n");
        return -1;
    }

    Uint8 r, g, b, a;
    Uint32 draw_flags = DSDRAW_NOFX;
    DFBRegion dfb_update_rg;
    IDirectFBSurface* dfb_dst_surface;

    if (dst == GAL_VideoSurface)
        dfb_dst_surface = pFrameBuffer;
    else
        dfb_dst_surface = dst->hwdata->dfb_surface;

    if (dfb_dst_surface == NULL) {
        fprintf(stderr, "NEWGAL>DFB: dst_surface NULL\n");
        return -1;
    }

    if (rect == NULL) {
        fprintf(stderr, "NEWGAL>DFB: rect NULL\n");
        return -1;
    }

    dfb_update_rg.x1 = rect->x;
    dfb_update_rg.y1 = rect->y;
    dfb_update_rg.x2 = rect->x + rect->w;
    dfb_update_rg.y2 = rect->y + rect->h;

    /* make sure the dfb surface is unlock. */
    dfb_dst_surface->Unlock(dfb_dst_surface);

    /* set the fill color */
    GAL_GetRGBA(color, dst->format, &r, &g, &b, &a);
    RETCHECK(dfb_dst_surface->SetColor(dfb_dst_surface, r, g, b, a) != DFB_OK, 
            "dfb set color failed", ;);

    /* set the dfb fill flags. */
    //fprintf(stderr, "NEWGAL>DFB: draw flags:%x\n", draw_flags); 
    RETCHECK(dfb_dst_surface->SetDrawingFlags(dfb_dst_surface, draw_flags) != DFB_OK, 
            "dfb set draw flags failed", ;);
    
    /* call dfb FillRectangle to do hardware acceleration. */
    RETCHECK(dfb_dst_surface->FillRectangle(dfb_dst_surface, rect->x, rect->y, rect->w, rect->h), 
            "dfb fill failed", goto fill_err);

    /* call dfb Filp to update the blit rect. */
    RETCHECK(dfb_dst_surface->Flip(dfb_dst_surface, &dfb_update_rg, DSFLIP_WAITFORSYNC) != DFB_OK, 
            "dfb Flip failed", goto fill_err);

    /* restore the color, becareful if not restore it,
     * maybe other draw operation not visible. */
    RETCHECK(dfb_dst_surface->SetColor(dfb_dst_surface, 0xFF, 0xFF, 0xFF, 0xFF) != DFB_OK, 
            "dfb set color failed", ;);

    /* when acceleration is done, remember to lock the dfb surface, 
     * because minigui surface need the dfb surface pixels. */
    dfb_dst_surface->Lock(dfb_dst_surface, DSLF_READ | DSLF_WRITE, &dst->pixels, (int*)&dst->pitch);
    return 0;

fill_err:
    dfb_dst_surface->Lock(dfb_dst_surface, DSLF_READ | DSLF_WRITE, &dst->pixels, (int*)&dst->pitch);
    return -1;
}

static int DFB_HWAccelBlit (GAL_Surface *src, GAL_Rect *srcrect, GAL_Surface *dst, GAL_Rect *dstrect)
{
    /* the video surface don't need hwdata.
     * because it's has create a global dfb primary surface. */
    if ((src->hwdata == NULL && src != GAL_VideoSurface) || (dst->hwdata == NULL && dst != GAL_VideoSurface)) {
        fprintf(stderr, "NEWGAL>DFB: src_hwdata or dst_hwdata NULL\n");
        return -1;
    }

    int dst_x, dst_y;
    Uint8 r, g, b;
    Uint32 blit_flags = DSBLIT_NOFX;

    DFBRegion dfb_update_rg;
    DFBRectangle dfb_src_rc;

    IDirectFBSurface* dfb_src_surface;
    IDirectFBSurface* dfb_dst_surface;

    if (src == GAL_VideoSurface)
        dfb_src_surface = pFrameBuffer;
    else
        dfb_src_surface = src->hwdata->dfb_surface;

    if (dst == GAL_VideoSurface)
        dfb_dst_surface = pFrameBuffer;
    else
        dfb_dst_surface = dst->hwdata->dfb_surface;

    if (dfb_src_surface == NULL || dfb_dst_surface == NULL) {
        fprintf(stderr, "NEWGAL>DFB: src_surface or dst_surface NULL\n");
        return -1;
    }

    if (srcrect == NULL || dstrect == NULL) {
        fprintf(stderr, "NEWGAL>DFB: srcrect or dstrect NULL\n");
        return -1;
    }

    dfb_src_rc.x = srcrect->x;
    dfb_src_rc.y = srcrect->y;
    dfb_src_rc.w = srcrect->w;
    dfb_src_rc.h = srcrect->h;

    dst_x = dstrect->x;
    dst_y = dstrect->y;

    dfb_update_rg.x1 = dstrect->x;
    dfb_update_rg.y1 = dstrect->y;
    dfb_update_rg.x2 = dstrect->x + dstrect->w;
    dfb_update_rg.y2 = dstrect->y + dstrect->h;

    /* make sure the dfb surface is unlock. */
    dfb_src_surface->Unlock(dfb_src_surface);
    dfb_dst_surface->Unlock(dfb_dst_surface);

    /* TODO: colorkey, alpha, pixel-alpha etc. */
    if (src->flags & GAL_SRCCOLORKEY) {
        blit_flags |= DSBLIT_SRC_COLORKEY;
   
        Uint8* pixels = (Uint8*)src->pixels;
        GAL_GetRGB(src->format->colorkey, src->format, &r, &g, &b);

        RETCHECK(dfb_src_surface->SetSrcColorKey(dfb_src_surface, r, g, b) != DFB_OK, 
                "dfb set src_color_key failed", ;);
    }

    if (src->flags & GAL_SRCALPHA) {
        blit_flags |= DSBLIT_BLEND_COLORALPHA;
        RETCHECK(dfb_dst_surface->SetColor(dfb_dst_surface, 0xFF, 0xFF, 0xFF, src->format->alpha) != DFB_OK, 
                "dfb set color failed", ;);
    }

    if (src->flags & GAL_SRCPIXELALPHA) {
        blit_flags |= DSBLIT_BLEND_ALPHACHANNEL;
    }

    /* set the dfb Blit flags. */
    RETCHECK(dfb_dst_surface->SetBlittingFlags(dfb_dst_surface, blit_flags) != DFB_OK, 
            "dfb set blit flags failed", ;);

    /* call dfb Blit to do hardware acceleration. */
    RETCHECK(dfb_dst_surface->Blit(dfb_dst_surface, dfb_src_surface, &dfb_src_rc, dst_x, dst_y) != DFB_OK, 
            "dfb bilt failed", goto blit_err);

    /* call dfb Filp to update the blit rect. */
    RETCHECK(dfb_dst_surface->Flip(dfb_dst_surface, &dfb_update_rg, DSFLIP_WAITFORSYNC) != DFB_OK, 
            "dfb Flip failed", goto blit_err);

    /* restore the color, becareful if not restore it,
     * maybe other draw operation not visible. */
    RETCHECK(dfb_dst_surface->SetColor(dfb_dst_surface, 0xFF, 0xFF, 0xFF, 0xFF) != DFB_OK, 
            "dfb set color failed", ;);

    /* when acceleration is done, remember to lock the dfb surface, 
     * because minigui surface need the dfb surface pixels. */
    dfb_src_surface->Lock(dfb_src_surface, DSLF_READ | DSLF_WRITE, &src->pixels, (int*)&src->pitch);
    dfb_dst_surface->Lock(dfb_dst_surface, DSLF_READ | DSLF_WRITE, &dst->pixels, (int*)&dst->pitch);
    return 0;

blit_err:
    dfb_src_surface->Lock(dfb_src_surface, DSLF_READ | DSLF_WRITE, &src->pixels, (int*)&src->pitch);
    dfb_dst_surface->Lock(dfb_dst_surface, DSLF_READ | DSLF_WRITE, &dst->pixels, (int*)&dst->pitch);
    return -1;
}

static int DFB_CheckHWBlit (_THIS, GAL_Surface * src, GAL_Surface * dst)
{
    src->flags &= ~GAL_HWACCEL;
    src->map->hw_blit = NULL;

    /* only supported the hw surface accelerated. */
    if (!(src->flags & GAL_HWSURFACE) || !(dst->flags & GAL_HWSURFACE)) {
        fprintf(stderr, "src(%s) dst(%s)\n", 
                (src->flags & GAL_HWSURFACE) ? "HW" : "SW",
                (dst->flags & GAL_HWSURFACE) ? "HW" : "SW");
        return -1;
    }

    src->flags |= GAL_HWACCEL;
    src->map->hw_blit = DFB_HWAccelBlit;
    return 0;
}

static int DFB_SetColors (_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return (1);
}

/* Note: If we are terminated, this could be called in the middle of
 * another video routine -- notably UpdateRects. */
static void DFB_VideoQuit (_THIS)
{
#ifdef _MGRM_PROCESSES
    if (!ISSERVER)
        return;
#endif

	pFrameBuffer->Unlock(pFrameBuffer);
    pFrameBuffer->ReleaseSource(pFrameBuffer);
    pFrameBuffer->Release(pFrameBuffer);

    __mg_dfb->Release(__mg_dfb);
}

#endif /* _MGGAL_DFB */

