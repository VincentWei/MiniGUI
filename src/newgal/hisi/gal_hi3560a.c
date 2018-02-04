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
#define dbg() printf("%s %d\n", __FUNCTION__, __LINE__)

/*
**  $Id: gal_hi3560a.c 12768 2010-04-22 05:28:30Z dongkai $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#ifdef _MGGAL_HI3560A

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "gal_hi3560a.h"

#ifdef _MGRM_PROCESSES
#   include "client.h"
#endif

#define HI3560AVID_DRIVER_NAME "hi3560"

#ifdef _MGRM_PROCESSES
#   define ISSERVER (mgIsServer)
#else
#   define ISSERVER (1)
#endif

/* Initialization/Query functions */
static int HI3560A_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **HI3560A_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *HI3560A_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int HI3560A_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void HI3560A_VideoQuit(_THIS);

/* Hardware surface functions */
#ifdef _MGRM_PROCESSES
static void HI3560A_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply);
#endif
static int HI3560A_AllocHWSurface(_THIS, GAL_Surface *surface);
static void HI3560A_FreeHWSurface(_THIS, GAL_Surface *surface);
static int HI3560A_CheckHWBlit(_THIS, GAL_Surface * src, GAL_Surface * dst);
static int HI3560A_HWAccelBlit(GAL_Surface * src, GAL_Rect * srcrect, GAL_Surface * dst, GAL_Rect * dstrect);
static int HI3560A_FillHWRect(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color);

#ifdef FBCON_DEBUG
static void print_vinfo(struct fb_var_screeninfo *vinfo)
{
    fprintf(stderr, "Printing vinfo:\n");
    fprintf(stderr, "txres: %d\n", vinfo->xres);
    fprintf(stderr, "tyres: %d\n", vinfo->yres);
    fprintf(stderr, "txres_virtual: %d\n", vinfo->xres_virtual);
    fprintf(stderr, "tyres_virtual: %d\n", vinfo->yres_virtual);
    fprintf(stderr, "txoffset: %d\n", vinfo->xoffset);
    fprintf(stderr, "tyoffset: %d\n", vinfo->yoffset);
    fprintf(stderr, "tbits_per_pixel: %d\n", vinfo->bits_per_pixel);
    fprintf(stderr, "tgrayscale: %d\n", vinfo->grayscale);
    fprintf(stderr, "tnonstd: %d\n", vinfo->nonstd);
    fprintf(stderr, "tactivate: %d\n", vinfo->activate);
    fprintf(stderr, "theight: %d\n", vinfo->height);
    fprintf(stderr, "twidth: %d\n", vinfo->width);
    fprintf(stderr, "taccel_flags: %d\n", vinfo->accel_flags);
    fprintf(stderr, "tpixclock: %d\n", vinfo->pixclock);
    fprintf(stderr, "tleft_margin: %d\n", vinfo->left_margin);
    fprintf(stderr, "tright_margin: %d\n", vinfo->right_margin);
    fprintf(stderr, "tupper_margin: %d\n", vinfo->upper_margin);
    fprintf(stderr, "tlower_margin: %d\n", vinfo->lower_margin);
    fprintf(stderr, "thsync_len: %d\n", vinfo->hsync_len);
    fprintf(stderr, "tvsync_len: %d\n", vinfo->vsync_len);
    fprintf(stderr, "tsync: %d\n", vinfo->sync);
    fprintf(stderr, "tvmode: %d\n", vinfo->vmode);
    fprintf(stderr, "tred: %d/%d\n", vinfo->red.length, vinfo->red.offset);
    fprintf(stderr, "tgreen: %d/%d\n", vinfo->green.length, vinfo->green.offset);
    fprintf(stderr, "tblue: %d/%d\n", vinfo->blue.length, vinfo->blue.offset);
    fprintf(stderr, "talpha: %d/%d\n", vinfo->transp.length, vinfo->transp.offset);
}

static void print_finfo(struct fb_fix_screeninfo *finfo)
{
    fprintf(stderr, "Printing finfo:\n");
    fprintf(stderr, "tsmem_start = %p\n", (char *)finfo->smem_start);
    fprintf(stderr, "tsmem_len = %d\n", finfo->smem_len);
    fprintf(stderr, "ttype = %d\n", finfo->type);
    fprintf(stderr, "ttype_aux = %d\n", finfo->type_aux);
    fprintf(stderr, "tvisual = %d\n", finfo->visual);
    fprintf(stderr, "txpanstep = %d\n", finfo->xpanstep);
    fprintf(stderr, "typanstep = %d\n", finfo->ypanstep);
    fprintf(stderr, "tywrapstep = %d\n", finfo->ywrapstep);
    fprintf(stderr, "tline_length = %d\n", finfo->line_length);
    fprintf(stderr, "tmmio_start = %p\n", (char *)finfo->mmio_start);
    fprintf(stderr, "tmmio_len = %d\n", finfo->mmio_len);
    fprintf(stderr, "taccel = %d\n", finfo->accel);
}
#endif


static int surface2tde(GAL_Surface* origin, TDE2_SURFACE_S* new)
{
    if (NULL != origin && NULL != new) {
        memset(new, 0, sizeof(TDE2_SURFACE_S));
        if (4 == origin->format->BytesPerPixel) {
            new->enColorFmt = TDE2_COLOR_FMT_ARGB8888;
            new->bAlphaMax255 = HI_TRUE;
        } else if (2 == origin->format->BytesPerPixel) {
            new->enColorFmt = TDE2_COLOR_FMT_ARGB1555;
            new->bAlphaMax255 = HI_FALSE;
        }
        else {
            fprintf(stderr, ">> surface2tde: error pixel format.\n");
            return -1;
        }
        new->u32PhyAddr= origin->hwdata->addr_phy;
        new->u32Width  = origin->w;
        new->u32Height = origin->h;
        new->u32Stride = origin->pitch;
        //printf(">> surface2tde phyaddr = %p, w = %d, h = %d, pitch = %d.\n",
                //new->u32PhyAddr, new->u32Width, new->u32Height, new->u32Stride);
        return 0;
    }
    return -1;
}

/* HI3560 driver bootstrap functions */

static int HI3560A_Available(void)
{
    int console;
    const char *GAL_fbdev;

    GAL_fbdev = getenv("FRAMEBUFFER");
    if ( GAL_fbdev == NULL ) {
        GAL_fbdev = "/dev/fb/0";
    }

    console = open(GAL_fbdev, O_RDWR, 0);
    if ( console >= 0 ) {
        close(console);
    }
    return(console >= 0);
}

static void HI3560A_DeleteDevice(GAL_VideoDevice *device)
{
    if (ISSERVER) {
        gal_vmbucket_destroy(&device->hidden->vmbucket);
    }
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice *HI3560A_CreateDevice(int devindex)
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
    device->VideoInit = HI3560A_VideoInit;
    device->ListModes = HI3560A_ListModes;
    device->SetVideoMode = HI3560A_SetVideoMode;
    device->SetColors = HI3560A_SetColors;
    device->VideoQuit = HI3560A_VideoQuit;
#ifdef _MGRM_PROCESSES
    device->RequestHWSurface = HI3560A_RequestHWSurface;
#endif
    device->AllocHWSurface = HI3560A_AllocHWSurface;
    device->CheckHWBlit = HI3560A_CheckHWBlit;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FillHWRect = HI3560A_FillHWRect;
    device->FreeHWSurface = HI3560A_FreeHWSurface;

    device->free = HI3560A_DeleteDevice;

    return device;
}

VideoBootStrap HI3560A_bootstrap = {
    HI3560AVID_DRIVER_NAME, "HI3560 video driver",
    HI3560A_Available, HI3560A_CreateDevice
};

static int HI3560A_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    struct GAL_PrivateVideoData *hidden;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    int fd_fb = -1;
    unsigned char *pixels = MAP_FAILED;
    const char *GAL_fbdev = NULL;
    HI_UNF_OUTPUT_INTERFACE_S   stVO_Mode ;
    HI_UNF_DISP_INIT_PARA_S stPara;

    stVO_Mode.enDacMode[0] = HI_UNF_DAC_MODE_PR ;
    stVO_Mode.enDacMode[1] = HI_UNF_DAC_MODE_PB ;
    stVO_Mode.enDacMode[2] = HI_UNF_DAC_MODE_Y ;
    stVO_Mode.enDacMode[3] = HI_UNF_DAC_MODE_CVBS ;
#if 1
    HI_UNF_DISP_Init();
    HI_UNF_DISP_Create(&stPara, &this->hidden->handle);
    HI_UNF_DISP_SetEnable(this->hidden->handle, HI_TRUE);
    HI_UNF_DISP_SetDacMode(this->hidden->handle , &stVO_Mode);
    HI_TDE2_Open();

    fprintf (stderr, "NEWGAL>HI3560: Calling init method!\n");
    GAL_fbdev = getenv("FRAMEBUFFER");
    if ( GAL_fbdev == NULL ) {
        GAL_fbdev = "/dev/fb/0";
    }

    fd_fb = open(GAL_fbdev, O_RDWR);
    if (fd_fb < 0) {
        perror("open()");
        goto err;
    }

    if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &finfo) < 0) {
        perror("ioctl(FBIOGET_FSCREENINFO)");
        goto err;
    }
    assert(finfo.smem_start && finfo.smem_len);

    if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("ioctl(FBIOGET_VSCREENINFO)");
        goto err;
    }

    pixels = (unsigned char *) mmap(NULL, finfo.smem_len,
            PROT_READ|PROT_WRITE, MAP_SHARED, fd_fb, 0);
#else
    puts("use malloc replace mmap.");
    pixels = (unsigned char *)malloc(720*576*2); 
#endif
    if (pixels == MAP_FAILED) {
        perror("mmap()");
        goto err;
    }

    /* Determine the screen depth (use default 8-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;

    this->hidden = (struct GAL_PrivateVideoData *) calloc(1, sizeof(struct GAL_PrivateVideoData));
    hidden = this->hidden;
    hidden->fd_fb = fd_fb;
    hidden->smem_start = finfo.smem_start;
    printf("+=+=+=+= mmap video mem to %p.\n", hidden->smem_start);
    hidden->smem_len = finfo.smem_len;
    hidden->pitch = finfo.line_length;
    hidden->pixels = pixels;
    hidden->width = vinfo.width;
    hidden->height = vinfo.height;
    this->hidden->mapped_memlen = finfo.smem_len;
    if (ISSERVER) {
        memset (hidden->pixels, 0, hidden->smem_len);
        gal_vmbucket_init(&hidden->vmbucket, hidden->pixels, hidden->smem_len);
    }

    {
        GAL_VideoInfo *video_info = &this->info;
        video_info->blit_fill = 1;
        video_info->blit_hw = 1;
        video_info->blit_hw_CC = 1;
        video_info->blit_hw_A  = 1;
    }

    return(0);

err:
    assert(0);
    if (pixels != MAP_FAILED) {
        munmap(pixels, finfo.smem_len);
    }
    if (fd_fb >= 0) {
        close(fd_fb);
    }
    return -1;
}

static GAL_Rect **HI3560A_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    /* any size is ok */
    return (GAL_Rect**) -1;
}

static void HI3560A_FreeHWSurfaces(_THIS)
{
#if 0
    gal_vmbucket_t *bucket, *freeable;

    bucket = surfaces.next;
    while ( bucket ) {
        freeable = bucket;
        bucket = bucket->next;
        free(freeable);
    }
    surfaces.next = NULL;
#endif
}

#if 0
static GAL_Surface *HI3560A_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    int i;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;
    unsigned char *surfaces_mem;
    int surfaces_len;

#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
    {
        /* Restore the original palette */
        //FB_RestorePalette (this);
    }

    /* Set the video mode and get the final screen format */
    if (ioctl(this->hidden->fd_fb, FBIOGET_VSCREENINFO, &vinfo) < 0 ) {
        GAL_SetError("NEWGAL>HI3560A: Couldn't get console screen info");
        return(NULL);
    }
#ifdef FBCON_DEBUG
    fprintf(stderr, "NEWGAL>HI3560A: Printing original vinfo:\n");
    print_vinfo(&vinfo);
#endif

#ifdef _MGRM_PROCESSES
    if (!mgIsServer) {
        vinfo.xres = width;
        vinfo.yres = height;
    }
    if ( mgIsServer && ((vinfo.xres != width) || (vinfo.yres != height) ||
         (vinfo.bits_per_pixel != bpp) /* || (flags & GAL_DOUBLEBUF) */) ) {
#else
    if ( ((vinfo.xres != width) || (vinfo.yres != height) ||
         (vinfo.bits_per_pixel != bpp) /* || (flags & GAL_DOUBLEBUF) */) ) {
#endif
        vinfo.activate = FB_ACTIVATE_NOW;
        vinfo.accel_flags = 0;
        vinfo.bits_per_pixel = bpp;
        vinfo.xres = width;
        vinfo.xres_virtual = width;
        vinfo.yres = height;
        vinfo.yres_virtual = height;
        vinfo.xoffset = 0;
        vinfo.yoffset = 0;
        vinfo.red.length = vinfo.red.offset = 0;
        vinfo.green.length = vinfo.green.offset = 0;
        vinfo.blue.length = vinfo.blue.offset = 0;
        vinfo.transp.length = vinfo.transp.offset = 0;
#ifdef FBCON_DEBUG
        fprintf(stderr, "NEWGAL>HI3560A: Printing wanted vinfo:\n");
        print_vinfo(&vinfo);
#endif
        if ( ioctl(this->hidden->fd_fb, FBIOPUT_VSCREENINFO, &vinfo) < 0 ) {
            vinfo.yres_virtual = height;
            if ( ioctl(this->hidden->fd_fb, FBIOPUT_VSCREENINFO, &vinfo) < 0 ) {
                GAL_SetError("NEWGAL>HI3560A: Couldn't set console screen info");
                return(NULL);
            }
        }
    } else {
        int maxheight;

        /* Figure out how much video memory is available */
        maxheight = height;
        if ( vinfo.yres_virtual > maxheight ) {
            vinfo.yres_virtual = maxheight;
        }
    }
    //cache_vinfo = vinfo;
#ifdef FBCON_DEBUG
    fprintf (stderr, "NEWGAL>HI3560A: Printing actual vinfo:\n");
    print_vinfo(&vinfo);
#endif

    Rmask = 0;
    for ( i=0; i<vinfo.red.length; ++i ) {
        Rmask <<= 1;
        Rmask |= (0x00000001<<vinfo.red.offset);
    }
    Gmask = 0;
    for ( i=0; i<vinfo.green.length; ++i ) {
        Gmask <<= 1;
        Gmask |= (0x00000001<<vinfo.green.offset);
    }
    Bmask = 0;
    for ( i=0; i<vinfo.blue.length; ++i ) {
        Bmask <<= 1;
        Bmask |= (0x00000001<<vinfo.blue.offset);
    }
    Amask = 0;
    for ( i=0; i<vinfo.transp.length; ++i ) {
        Amask <<= 1;
        Amask |= (0x00000001<<vinfo.transp.offset);
    }

    if (!GAL_ReallocFormat(current, vinfo.bits_per_pixel,
                                      Rmask, Gmask, Bmask, Amask) ) {
        return(NULL);
    }
    if ( vinfo.bits_per_pixel < 8 ) {
        current->format->MSBLeft = !(vinfo.red.msb_right);
    }

    /* Get the fixed information about the console hardware.
       This is necessary since finfo.line_length changes.
     */
    if ( ioctl(this->hidden->fd_fb, FBIOGET_FSCREENINFO, &finfo) < 0 ) {
        GAL_SetError("NEWGAL>HI3560A: Couldn't get console hardware info");
        return(NULL);
    }

    /* Save hardware palette, if needed */
    //FB_SavePalette(this, &finfo, &vinfo);

    /* Set up the new mode framebuffer */
    current->flags = (GAL_FULLSCREEN|GAL_HWSURFACE);
    current->w = vinfo.xres;
    current->h = vinfo.yres;
    current->pitch = finfo.line_length;
    current->pixels = this->hidden->pixels;
    hidden->bpp = bpp;

    /* Set up the information for hardware surfaces */
    surfaces_mem = (char *)current->pixels +
                            vinfo.yres_virtual*current->pitch;
    surfaces_len = (this->hidden->mapped_memlen-(surfaces_mem-this->hidden->pixels));

#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
    {
        HI3560A_FreeHWSurfaces(this);
        // TODO: change it.
        //FB_InitHWSurfaces(this, current, surfaces_mem, surfaces_len);
    }
#ifdef _MGRM_PROCESSES
    else {
        current->hwdata = NULL;
    }
#endif

    /* Let the application know we have a hardware palette */
    switch (finfo.visual) {
        case FB_VISUAL_PSEUDOCOLOR:
            current->flags |= GAL_HWPALETTE;
        break;

        default:
        break;
    }

#ifdef _MGHAVE_PCIACCESS
    if (pci_accel_driver) /* Init accelerated hardware via pciaccess */
        FB_InitPCIAccelDriver (this, current);
#endif

    /* We're done */
    return(current);
}
#else
static GAL_Surface *HI3560A_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    struct GAL_PrivateVideoData *hidden = this->hidden;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    int i;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;
    unsigned char *surfaces_mem;
    int surfaces_len;

    /* Set the video mode and get the final screen format */
    if (ioctl(this->hidden->fd_fb, FBIOGET_VSCREENINFO, &vinfo) < 0 ) {
        GAL_SetError("NEWGAL>HI3560A: Couldn't get console screen info");
        return(NULL);
    }
#ifdef FBCON_DEBUG
    fprintf(stderr, "NEWGAL>HI3560A: Printing original vinfo:\n");
    print_vinfo(&vinfo);
#endif

#ifdef _MGRM_PROCESSES
    if (!mgIsServer) {
        vinfo.xres = width;
        vinfo.yres = height;
    }
    if ( mgIsServer && 
         ((vinfo.xres != width) || (vinfo.yres != height) ||
         (vinfo.bits_per_pixel != bpp) /* || (flags & GAL_DOUBLEBUF) */) ) {
#else
    if ( ((vinfo.xres != width) || (vinfo.yres != height) ||
         (vinfo.bits_per_pixel != bpp) /* || (flags & GAL_DOUBLEBUF) */) ) {
#endif
        vinfo.activate = FB_ACTIVATE_NOW;
        vinfo.accel_flags = 0;
        vinfo.bits_per_pixel = bpp;
        vinfo.xres = width;
        vinfo.xres_virtual = width;
        vinfo.yres = height;
        vinfo.yres_virtual = height;
        vinfo.xoffset = 0;
        vinfo.yoffset = 0;
        vinfo.red.length = vinfo.red.offset = 0;
        vinfo.green.length = vinfo.green.offset = 0;
        vinfo.blue.length = vinfo.blue.offset = 0;
        vinfo.transp.length = vinfo.transp.offset = 0;
#ifdef FBCON_DEBUG
        fprintf(stderr, "NEWGAL>HI3560A: Printing wanted vinfo:\n");
        print_vinfo(&vinfo);
#endif
        if ( ioctl(this->hidden->fd_fb, FBIOPUT_VSCREENINFO, &vinfo) < 0 ) {
            vinfo.yres_virtual = height;
            if ( ioctl(this->hidden->fd_fb, FBIOPUT_VSCREENINFO, &vinfo) < 0 ) {
                GAL_SetError("NEWGAL>HI3560A: Couldn't set console screen info");
                return(NULL);
            }
        }
        this->hidden->pitch = finfo.line_length;
        this->hidden->width = vinfo.width;
        this->hidden->height = vinfo.height;
    } else {
        int maxheight;

        /* Figure out how much video memory is available */
        maxheight = height;
        if ( vinfo.yres_virtual > maxheight ) {
            vinfo.yres_virtual = maxheight;
        }
    }
    //cache_vinfo = vinfo;
#ifdef FBCON_DEBUG
    fprintf (stderr, "NEWGAL>HI3560A: Printing actual vinfo:\n");
    print_vinfo(&vinfo);
#endif

    Rmask = 0;
    for ( i=0; i<vinfo.red.length; ++i ) {
        Rmask <<= 1;
        Rmask |= (0x00000001<<vinfo.red.offset);
    }
    Gmask = 0;
    for ( i=0; i<vinfo.green.length; ++i ) {
        Gmask <<= 1;
        Gmask |= (0x00000001<<vinfo.green.offset);
    }
    Bmask = 0;
    for ( i=0; i<vinfo.blue.length; ++i ) {
        Bmask <<= 1;
        Bmask |= (0x00000001<<vinfo.blue.offset);
    }
    Amask = 0;
    for ( i=0; i<vinfo.transp.length; ++i ) {
        Amask <<= 1;
        Amask |= (0x00000001<<vinfo.transp.offset);
    }

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, bpp, Rmask, Gmask, Bmask, Amask)) {
        fprintf (stderr, "NEWGAL>S3C6410: "
                "Couldn't allocate new pixel format for requested mode\n");
        return(NULL);
    }

#if 0
    /* Set up the new mode framebuffer */
    current->flags = (GAL_FULLSCREEN|GAL_HWSURFACE);
    current->w = width;
    current->h = height;
    current->pitch = hidden->pitch;
    hidden->bpp = bpp;

    if (ISSERVER) {
        HI3560A_AllocHWSurface(this, current);
    } else {
        current->hwdata = (struct private_hwdata *)calloc(1, sizeof(struct private_hwdata));
        current->hwdata->vmblock = NULL;
        current->hwdata->addr_phy = hidden->smem_start + 0;

        current->pixels = hidden->pixels + 0;
        current->pitch = this->hidden->pitch;
        current->flags |= GAL_HWSURFACE;
    }
#else
    if ( vinfo.bits_per_pixel < 8 ) {
        current->format->MSBLeft = !(vinfo.red.msb_right);
    }

    /* Get the fixed information about the console hardware.
       This is necessary since finfo.line_length changes.
     */
    if ( ioctl(this->hidden->fd_fb, FBIOGET_FSCREENINFO, &finfo) < 0 ) {
        GAL_SetError("NEWGAL>HI3560A: Couldn't get console hardware info");
        return(NULL);
    }

    /* Save hardware palette, if needed */
    //FB_SavePalette(this, &finfo, &vinfo);

    /* Set up the new mode framebuffer */
    current->flags = (GAL_FULLSCREEN|GAL_HWSURFACE);
    hidden->width = current->w = vinfo.xres;
    hidden->height = current->h = vinfo.yres;
    hidden->pitch = current->pitch = finfo.line_length;
    current->pixels = this->hidden->pixels;
    hidden->bpp = bpp;

    if (ISSERVER) {
        HI3560A_AllocHWSurface(this, current);
    } else {
        current->hwdata = (struct private_hwdata *)calloc(1, sizeof(struct private_hwdata));
        current->hwdata->vmblock = NULL;
        current->hwdata->addr_phy = hidden->smem_start + 0;

        current->pixels = hidden->pixels + 0;
        current->pitch = this->hidden->pitch;
        current->flags |= GAL_HWSURFACE;
    }

    /* Let the application know we have a hardware palette */
    switch (finfo.visual) {
        case FB_VISUAL_PSEUDOCOLOR:
            current->flags |= GAL_HWPALETTE;
        break;

        default:
        break;
    }


#endif

    return current;
}
#endif

static int
HI3560A_CheckHWBlit(_THIS, GAL_Surface * src, GAL_Surface * dst)
{
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
    src->map->hw_blit = HI3560A_HWAccelBlit;
    return 0;
}

static int
tde_bitblit(GAL_Surface *src, GAL_Rect *srcrect, GAL_Surface *dst, GAL_Rect *dstrect,
        gal_pixel colorkey, int alpha, int flags)
{
    int s32Ret = 0;
    TDE_HANDLE tdeHandle;
    TDE2_OPT_S opt = {0};
    TDE2_SURFACE_S tde_src;
    TDE2_SURFACE_S tde_dst;
    if (NULL != src && NULL != srcrect && 
            NULL != dst && NULL != dstrect) {
        if (0 == surface2tde(src, &tde_src) &&
                0 == surface2tde(dst, &tde_dst)) {
            memset(&opt, 0, sizeof(opt));
            if (0 == flags) {
                tdeHandle = HI_TDE2_BeginJob();
                s32Ret = HI_TDE2_QuickCopy(tdeHandle, 
                        &tde_src, (TDE2_RECT_S*)srcrect, 
                        &tde_dst, (TDE2_RECT_S*)dstrect);
                if(0 != s32Ret) {
                    printf ("Line:%d,HI_TDE2_QuickCopy failed,ret = %x!\n", __LINE__, s32Ret);
                    HI_TDE2_CancelJob(tdeHandle);
                    return -2;
                }
                HI_TDE2_EndJob(tdeHandle, HI_FALSE, HI_TRUE, 20);
                return 0;
            }
#if 0
            TDE2_SURFACE_S stScreen;
            memset(&stScreen, 0, sizeof(stScreen));
            stScreen.enColorFmt = TDE2_COLOR_FMT_ARGB8888;
            stScreen.u32PhyAddr = 0xc4084000;
            stScreen.u32Width = 720;
            stScreen.u32Height = 576;
            stScreen.u32Stride = 2880;
            stScreen.bAlphaMax255 = HI_TRUE;

            TDE2_RECT_S stSrcRect={0, 0, 360, 576};
            TDE2_RECT_S stDstRect={360, 0, 360, 576};
            int s32Ret = 0;
            TDE2_OPT_S stOpt = {0};
            memset (&stOpt, 0, sizeof(stOpt));
#if 1
            //point Alpha form foregound or backgound
            stOpt.enOutAlphaFrom = TDE2_OUTALPHA_FROM_FOREGROUND;    
#else
            //layer Alpha
            stOpt.enOutAlphaFrom = TDE2_OUTALPHA_FROM_GLOBALALPHA;    
            stOpt.u8GlobalAlpha = 90;
#endif
            stOpt.enColorKeyMode = TDE2_COLORKEY_MODE_FOREGROUND;   //对前景色做colorkey操作
            stOpt.unColorKeyValue.struCkARGB.stAlpha.bCompIgnore = HI_TRUE;
            stOpt.enAluCmd = TDE2_ALUCMD_ROP;
            stOpt.enRopCode_Alpha = TDE2_ROP_MERGEPEN;
            stOpt.enRopCode_Color = TDE2_ROP_COPYPEN;
            //stOpt.bDeflicker = HI_TRUE;//启用抗闪烁
            stOpt.bResize = HI_FALSE;//进行缩放


            tdeHandle = HI_TDE2_BeginJob();

            s32Ret = HI_TDE2_Bitblit(tdeHandle, 
                    &stScreen, &stDstRect, 
                    &stScreen, &stSrcRect, 
                    &stScreen, &stSrcRect, 
                    &stOpt);
            if(s32Ret != 0)
            {
                printf (">>Line:%d,HI_TDE2_Bitblit failed,ret=0x%x!\n", __LINE__, s32Ret);
                HI_TDE2_CancelJob(tdeHandle);
                return -1;
            }
            HI_TDE2_EndJob(tdeHandle, HI_FALSE, HI_TRUE, 20);
            return 0;
#else
            opt.enAluCmd = TDE2_ALUCMD_NONE;
            opt.enOutAlphaFrom = TDE2_OUTALPHA_FROM_NORM;
            opt.enColorKeyMode = TDE2_COLORKEY_MODE_NONE;
            opt.enRopCode_Color = TDE2_ROP_COPYPEN;
            opt.u8GlobalAlpha = 0xff;
            //opt.bDeflicker = HI_TRUE;//启用抗闪烁，开了在投影仪上更闪
            opt.bResize = HI_FALSE;
            opt.unColorKeyValue.struCkARGB.stAlpha.bCompIgnore = HI_TRUE;

            if (flags & 4) { /* Pixel Alpha */
                printf("==================== surface has pixels alpha.\n");
                opt.enAluCmd = TDE2_ALUCMD_BLEND;
            }
            if (flags & 2) { /* Alpha */
                printf("==================== surface has src alpha = %d.\n", alpha);
                opt.enAluCmd = TDE2_ALUCMD_BLEND;
                opt.u8GlobalAlpha = alpha;
            }
            if (flags & 1) { /* Colorkey */
                Uint8 a = 0, r = 0, g = 0, b = 0;
                printf("==================== surface has color key = %d.\n", colorkey);
                opt.enColorKeyMode = TDE2_COLORKEY_MODE_FOREGROUND;

                GAL_GetRGBA(colorkey, src->format, &r, &g, &b, &a);

                memset (&opt.unColorKeyValue, 0, sizeof(opt.unColorKeyValue));
                opt.unColorKeyValue.struCkARGB.stAlpha.u8CompMin = a;
                opt.unColorKeyValue.struCkARGB.stAlpha.u8CompMax = a;
                opt.unColorKeyValue.struCkARGB.stAlpha.bCompOut = 0;
                opt.unColorKeyValue.struCkARGB.stAlpha.bCompIgnore = HI_FALSE;

                opt.unColorKeyValue.struCkARGB.stRed.u8CompMin = r;
                opt.unColorKeyValue.struCkARGB.stRed.u8CompMax = r;
                opt.unColorKeyValue.struCkARGB.stRed.bCompOut = 0;
                opt.unColorKeyValue.struCkARGB.stRed.bCompIgnore = HI_FALSE;

                opt.unColorKeyValue.struCkARGB.stGreen.u8CompMin = g;
                opt.unColorKeyValue.struCkARGB.stGreen.u8CompMax = g;
                opt.unColorKeyValue.struCkARGB.stGreen.bCompOut = 0;
                opt.unColorKeyValue.struCkARGB.stGreen.bCompIgnore = HI_FALSE;

                opt.unColorKeyValue.struCkARGB.stBlue.u8CompMin = b;
                opt.unColorKeyValue.struCkARGB.stBlue.u8CompMax = b;
                opt.unColorKeyValue.struCkARGB.stBlue.bCompOut = 0;
                opt.unColorKeyValue.struCkARGB.stBlue.bCompIgnore = HI_FALSE;
            }
            printf("blit surface %p-(%d, %d, %d, %d) to %p-(%d, %d, %d, %d).\n",
                    tde_src.u32PhyAddr, srcrect->x,srcrect->y, srcrect->w, srcrect->h,
                    tde_dst.u32PhyAddr, dstrect->x,dstrect->y, srcrect->w, srcrect->h);
            tdeHandle = HI_TDE2_BeginJob();
            s32Ret = HI_TDE2_Bitblit(tdeHandle, 
                    &tde_dst, (TDE2_RECT_S*)dstrect, 
                    &tde_src, (TDE2_RECT_S*)srcrect,
                    &tde_dst, (TDE2_RECT_S*)dstrect, 
                    &opt);
            if(0 != s32Ret) {
                printf ("Line:%d,HI_TDE2_Bitblit failed,ret = %x.\n", __LINE__, s32Ret);
                HI_TDE2_CancelJob(tdeHandle);
                return -2;
            }
            HI_TDE2_EndJob(tdeHandle, HI_FALSE, HI_TRUE, 20);
#endif
            return 0;
        }
    }
    return -1;
}

static int
HI3560A_HWAccelBlit(GAL_Surface *src, GAL_Rect *srcrect, GAL_Surface *dst, GAL_Rect *dstrect)
{
    // TODO: alpha, alpha channel, colorkey
    gal_pixel colorkey = 0;
    int alpha = 0;
    int s32Ret = 0;
    int flags = 0;
    TDE2_SURFACE_S tde_src;
    TDE2_SURFACE_S tde_dst;
    if ((src->flags & GAL_SRCCOLORKEY)){
        colorkey = src->format->colorkey;
        flags |= 1;
    }
    if ((src->flags & GAL_SRCALPHA)){
        alpha = src->format->alpha;
        flags |= 2;
    }
    if ((src->flags & GAL_SRCPIXELALPHA)){
        flags |= 4;
    }

    return tde_bitblit(src, srcrect, dst, dstrect, colorkey, alpha, flags);
}

#ifdef _MGRM_PROCESSES
static void HI3560A_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply) {
    gal_vmblock_t *block;

    if (request->bucket == NULL) { /* alloc */
        dbg();
        block = gal_vmbucket_alloc(&this->hidden->vmbucket, request->pitch, request->h);
        if (block) {
            reply->offset = block->offset;
            reply->pitch = block->pitch;
            reply->bucket = block;
        }else{
            reply->bucket = NULL;
        }
    }else{ /* free */
        dbg();
        printf("on request free: %d, %d, %p\n", request->offset, request->pitch, request->bucket);
        if (request->offset != -1) {
            gal_vmbucket_free(&this->hidden->vmbucket, (gal_vmblock_t *)request->bucket);
        }
    }
}
#endif

/* We don't actually allow hardware surfaces other than the main one */
static int HI3560A_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    int offset, pitch;
    struct GAL_PrivateVideoData *hidden = this->hidden;
    gal_vmblock_t *block;
    dbg();

    if (surface->format->BytesPerPixel != (this->hidden->bpp / 8)) {
        surface->flags &= ~GAL_HWSURFACE;
        dbg();
        return -1;
    }

    if (ISSERVER) {
        block = gal_vmbucket_alloc(&hidden->vmbucket, surface->pitch, surface->h);
        if (block) {
            offset = block->offset;
            pitch = block->pitch;
            printf("HWSurface block offset = %d, pitch = %d.\n", offset, pitch);
        } else {
            puts("Not enough video memory.");
        }
#ifdef _MGRM_PROCESSES
    }else{ // for client
        REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, 0, NULL};
        REP_HWSURFACE reply = {0, 0, NULL};

        REQUEST req;

        req.id = REQID_HWSURFACE;
        req.data = &request;
        req.len_data = sizeof (REQ_HWSURFACE);

        ClientRequest (&req, &reply, sizeof (REP_HWSURFACE));

        block = (gal_vmblock_t *)reply.bucket;
        offset = reply.offset;
        pitch = reply.pitch;
#endif
    }

    if (block) {
        surface->hwdata = (struct private_hwdata *)calloc(1, sizeof(struct private_hwdata));
        surface->hwdata->vmblock = block;
        surface->hwdata->addr_phy = hidden->smem_start + offset;

        surface->pixels = hidden->pixels + offset;
        surface->pitch = pitch;
        memset (surface->pixels, 0, surface->pitch * surface->h);
        surface->flags |= GAL_HWSURFACE;
        dbg();
        return 0;
    }else{
        surface->flags &= ~GAL_HWSURFACE;
        dbg();
        return -1;
    }
}

static int HI3560A_FillHWRect(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
{
    TDE_HANDLE tdeHandle;
    int ret = 0;
    if (NULL != dst && NULL != rect) {
        TDE2_SURFACE_S surface;
        if (0 == surface2tde(dst, &surface)) {
            //printf("===FillHWRect===>w = %d, h = %d, p = %d, c = %X.\n",
            //        dst->w, dst->h, dst->pitch, color);
            tdeHandle = HI_TDE2_BeginJob();
            ret = HI_TDE2_QuickFill(tdeHandle, &surface, (TDE2_RECT_S*)rect, color);
            if (0 != ret) {
                fprintf(stderr, "HI_TDE2_QuickCopy failed, result = %x.\n", ret);
                HI_TDE2_CancelJob(tdeHandle);
                return -1;
            }
            HI_TDE2_EndJob(tdeHandle, HI_FALSE, HI_TRUE, 20); 
            return 0;
        }
    }
    return -1;
}

static void HI3560A_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    struct GAL_PrivateVideoData *hidden = this->hidden;
    struct private_hwdata *hwdata = surface->hwdata;

    if (hwdata->vmblock) {
        if (ISSERVER) {
            dbg();
            gal_vmbucket_free(&hidden->vmbucket, hwdata->vmblock);
#ifdef _MGRM_PROCESSES
        }else{
            dbg();
            REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, -1, surface->hwdata->vmblock};
            REP_HWSURFACE reply = {0, 0};

            REQUEST req;

            req.id = REQID_HWSURFACE;
            req.data = &request;
            req.len_data = sizeof (REQ_HWSURFACE);

            ClientRequest (&req, &reply, sizeof (REP_HWSURFACE));
#endif
        }
    }

    free(surface->hwdata);
    surface->hwdata = NULL;
    surface->pixels = NULL;
}

static int HI3560A_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another video routine -- notably UpdateRects.
*/
static void HI3560A_VideoQuit(_THIS)
{
    struct GAL_PrivateVideoData *data = this->hidden;
    memset(data->pixels, 0, data->smem_len);
    munmap(data->pixels, data->smem_len);
    close(data->fd_fb);

    HI_TDE2_Close();
    HI_UNF_DISP_SetEnable(this->hidden->handle, HI_FALSE);
    HI_UNF_DISP_Destroy(this->hidden->handle);
    HI_UNF_DISP_DeInit();
}

#endif /* _MGGAL_HI3560 */
