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
** rtos_xvfb.c: RTOS X virtual FrameBuffer based video driver implementation.
*/

#include "common.h"

#ifdef _MGGAL_RTOSXVFB 

#include <string.h>
#include <stdio.h>

#include "minigui.h"
#include "common.h"
#include "misc.h"
#include "newgal.h"
#include "sysvideo.h"
#include "rtos_xvfb.h"

XVFBHeader* __mg_rtos_xvfb_header;
static RECT buf_rc;

static int RTOS_XVFB_VideoInit (_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **RTOS_XVFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *RTOS_XVFB_SetVideoMode (_THIS, GAL_Surface *current,
                                            int width, int height, int bpp, Uint32 flags);
static int RTOS_XVFB_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static int RTOS_XVFB_AllocHWSurface(_THIS, GAL_Surface *surface);
static void RTOS_XVFB_FreeHWSurface(_THIS, GAL_Surface *surface);
static void RTOS_XVFB_VideoQuit (_THIS);

/* Allocate Virtual Frame Buffer.
* Return value: NULL on error, else the pointer to a XVFBHeader struct.
*/
XVFBHeader* xVFBAllocVirtualFrameBuffer (int width, int height, int depth,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask, BOOL MSBLeft)
{
    int nr_entries = 0;
    int data_size;
    XVFBHeader* rtos_xvfb_buf;
    

    if (depth <= 8)
        nr_entries = 1 << depth;    

    data_size = (width*depth+31)/32*4*height + sizeof(XVFBHeader) + nr_entries * sizeof(XVFBPalEntry);
    rtos_xvfb_buf = (XVFBHeader *)malloc (data_size);  


    bzero(rtos_xvfb_buf, sizeof(*rtos_xvfb_buf));

    rtos_xvfb_buf->width = width;
    rtos_xvfb_buf->height = height;
    rtos_xvfb_buf->depth = depth;
    rtos_xvfb_buf->pitch = (width*depth+31)/32*4;
    rtos_xvfb_buf->palette_changed = 0;
    rtos_xvfb_buf->palette_offset = sizeof (XVFBHeader);

    rtos_xvfb_buf->Rmask = Rmask;
    rtos_xvfb_buf->Gmask = Gmask;
    rtos_xvfb_buf->Bmask = Bmask;
    rtos_xvfb_buf->Amask = Amask;
    rtos_xvfb_buf->MSBLeft = MSBLeft;

    rtos_xvfb_buf->fb_offset = sizeof (XVFBHeader) + nr_entries * sizeof(XVFBPalEntry);

#ifdef _DEBUG
    fprintf (stderr, "xVFB is allocated. width=%d, height=%d, depth=%d \n",
             rtos_xvfb_buf->width, rtos_xvfb_buf->height, rtos_xvfb_buf->depth);
#endif

    return rtos_xvfb_buf;
}


/* Allocate Virtual Frame Buffer.
* Return value: NULL on error, else the pointer to a XVFBHeader struct.
*/
void xVFBFreeVirtualFrameBuffer (XVFBHeader *fb) 
{
    free (fb);
}


/* QVFB driver bootstrap functions */
static int RTOS_XVFB_Available (void)
{
    return 1;
}

static void RTOS_XVFB_DeleteDevice (GAL_VideoDevice *device)
{
    free (device->hidden);
    free (device);
}

static void RTOS_XVFB_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;
    XVFBHeader* hdr = this->hidden->hdr;    

    SetRect (&bound,
             hdr->dirty_rc_l, hdr->dirty_rc_t,
             hdr->dirty_rc_r, hdr->dirty_rc_b);

    if (bound.right == -1) bound.right = 0;
    if (bound.bottom == -1) bound.bottom = 0;

    for (i = 0; i < numrects; i++) {
        RECT rc;

        SetRect (&rc, rects[i].x, rects[i].y, 
                        rects[i].x + rects[i].w, rects[i].y + rects[i].h);
        if (IsRectEmpty (&bound))
            bound = rc;
        else
            GetBoundRect (&bound, &bound, &rc);
    }


    IntersectRect(&bound, &buf_rc, &bound);

    hdr->dirty_rc_l = bound.left;
    hdr->dirty_rc_t = bound.top;
    hdr->dirty_rc_r = bound.right;
    hdr->dirty_rc_b = bound.bottom;
    
    hdr->dirty = TRUE;
}

static GAL_VideoDevice *RTOS_XVFB_CreateDevice (int devindex)
{
    GAL_VideoDevice *this;

    /* Initialize all variables that we clean on shutdown */
    this = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if (this) {
        memset (this, 0, (sizeof *this));
        this->hidden = (struct GAL_PrivateVideoData *) malloc ((sizeof *this->hidden));
    }

    if ((this == NULL) || (this->hidden == NULL)) {
        GAL_OutOfMemory ();
        if (this) 
        {
            free (this);
        }
        return (0);
    }
    memset (this->hidden, 0, (sizeof *this->hidden));

    /* Set the function pointers */
    this->VideoInit = RTOS_XVFB_VideoInit;
    this->ListModes = RTOS_XVFB_ListModes;
    this->SetVideoMode = RTOS_XVFB_SetVideoMode;
    this->SetColors = RTOS_XVFB_SetColors;
    this->VideoQuit = RTOS_XVFB_VideoQuit;

    this->AllocHWSurface = RTOS_XVFB_AllocHWSurface;
    this->FreeHWSurface = RTOS_XVFB_FreeHWSurface;

    this->UpdateRects = RTOS_XVFB_UpdateRects;

    this->CheckHWBlit = NULL;
    this->FillHWRect = NULL;
    this->SetHWColorKey = NULL;
    this->SetHWAlpha = NULL;

    this->free = RTOS_XVFB_DeleteDevice;

    return this;
}

VideoBootStrap RTOS_XVFB_bootstrap = {
    "rtos_xvfb", "RTOS x Virtual FrameBuffer",
    RTOS_XVFB_Available, RTOS_XVFB_CreateDevice
};

static int RTOS_XVFB_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    if (!this) {
        GAL_SetError ("NEWGAL>RTOS_XVFB: current Video Deviceis NULL. \n");
        return -1;
    }

    struct GAL_PrivateVideoData* data = this->hidden;

    data->hdr = __mg_rtos_xvfb_header;
    data->shmrgn = __mg_rtos_xvfb_header;    

    buf_rc.left = 0;
    buf_rc.top = 0;
    buf_rc.right = __mg_rtos_xvfb_header->width;
    buf_rc.bottom = __mg_rtos_xvfb_header->height;



    if (data->hdr == NULL) {
        GAL_SetError ("NEWGAL>RTOS_XVFB: RTOS X virtual frame buffer is NULL. "
                      "please create xVFB Virtual Frame Buffer.\n");
        return -1;
    }

    vformat->BitsPerPixel = data->hdr->depth;
    switch (vformat->BitsPerPixel) {
#ifdef _MGGAL_SHADOW
        case 1:
        case 2:
        case 4:
            vformat->MSBLeft = __mg_rtos_xvfb_header->MSBLeft;
            fprintf(stderr, "vformat->MSBLeft= %d\n", vformat->MSBLeft);
            break;
#endif        
        case 8:
        case 16:
        case 24:
        case 32:
            vformat->BytesPerPixel = vformat->BitsPerPixel / 8;
            vformat->Rmask = __mg_rtos_xvfb_header->Rmask;
            vformat->Gmask = __mg_rtos_xvfb_header->Gmask;
            vformat->Bmask = __mg_rtos_xvfb_header->Bmask;
            vformat->Amask = __mg_rtos_xvfb_header->Amask;
            break;

        default:
            GAL_SetError ("NEWGAL>RTOS_XVFB: Not supported depth: %d, "
                "please try to use Shadow NEWGAL engine with targetname qvfb.\n", vformat->BitsPerPixel);
            printf("$$$$$$ vformat->BlitsPerpixel = %d\n", vformat->BitsPerPixel);
            return -1;
    }

    return 0;
}

static GAL_Rect **RTOS_XVFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    return (GAL_Rect **) -1;
}

static GAL_Surface *RTOS_XVFB_SetVideoMode (_THIS, GAL_Surface *current,
                                int width, int height, int bpp, Uint32 flags)
{
    /* Set up the mode framebuffer */
    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w = this->hidden->hdr->width;
    current->h = this->hidden->hdr->height;
    current->pitch = this->hidden->hdr->pitch;
    current->pixels = this->hidden->shmrgn + this->hidden->hdr->fb_offset;
    current->format->MSBLeft = __mg_rtos_xvfb_header->MSBLeft; //FIXME

    /* We're done */
    return current;
}

static int
RTOS_XVFB_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    int i;
    int depth = this->hidden->hdr->depth;
    int sect_len = (256-1) / ((1<<depth)-1);

    int set_num = ((1<<depth)-firstcolor)<ncolors ? 
                        ((1<<depth)-firstcolor):ncolors;
    int seted_num = 0;;

    XVFBPalEntry tmp;

    XVFBPalEntry *palette;
    palette = (XVFBPalEntry *)((BYTE *)this->hidden->hdr 
                               + this->hidden->hdr->palette_offset) + firstcolor;

    for (i=0; i<ncolors; i+=sect_len)
    {
        palette->r = colors[i].r;
        palette->g = colors[i].g;
        palette->b = colors[i].b;
        palette->a = colors[i].a;
        palette++;

        seted_num++;
        if (seted_num >= set_num)
            break;
    }

    if (depth == 1)
    {
        palette -= 2;
        tmp = palette[1];
        palette[1] = palette[0];
        palette[0] = tmp;
    }

    this->hidden->hdr->palette_changed = 1;
    
    return 1;
}

/* We don't actually allow hardware surfaces other than the main one */
static int RTOS_XVFB_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return -1;
}

static void RTOS_XVFB_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static void RTOS_XVFB_VideoQuit (_THIS)
{
    this->hidden->shmrgn = NULL;

}

#endif  /* End of _MGGAL_RTOSXVFB */

