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
**  $Id: mb93493video.c 8944 2007-12-29 08:29:16Z xwyan $
**  
**  Copyright (C) 2004 ~ 2007 Feynman Software.
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
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#include "vdc_init.h"
#include "mb93493video.h"

#define MB93493VID_DRIVER_NAME    "mb93493"

#define VRAM_W                  720     /* width */
#define VRAM_H                  576     /* high  */
#define VRAM_PIXEL_LEN          2       /* color mode = 16bit/pix */
#define LCD_BACKGROUND_COLOR    0x80    /* LCD background color */
#define SHOWBUF_LEN             300*1024/* show buffur length */

#define VRAM_LEN                (VRAM_W*VRAM_H*VRAM_PIXEL_LEN)
#define VRAM_PAGELEN            (VRAM_LEN/2)

#define RGB_RMASK               0x7c00
#define RGB_GMASK               0x03e0
#define RGB_BMASK               0x001f
#define RGB_BPP                 16
#define RGB_PIXEL_LEN           2

unsigned char   *VRAM_addr;     /* point to display memory of VDC */

static unsigned char *y_table = NULL;
static unsigned char *u_table = NULL;
static unsigned char *v_table = NULL;

typedef struct _YUV_Package
{
    unsigned char y0;
    unsigned char u;
    unsigned char y1;
    unsigned char v;
} YUV_Package;

static inline int _rgb2yuv_y(int r, int g, int b)
{
    return (unsigned char)(( 299*r + 587*g + 114*b)/1000);
}

// [0, 0x100) => [-436, 436] => [0, 873) => [0, 0x100)
static inline int _rgb2yuv_u(int r, int g, int b)
{
    return (unsigned char)((-147*r - 289*g + 436*b + 436*0x100)/873);
}

// [0, 0x100) => [-615, 615] => [0, 1231) => [0, 0x100)
static inline int _rgb2yuv_v(int r, int g, int b)
{
    return (unsigned char)(( 615*r - 515*g - 100*b + 615*0x100)/1231);
}

static void init_yuv_table(void)
{
    unsigned int i;
    int r, g, b;
    if(y_table != NULL)
        return;
    y_table = calloc(1, 2<<15);
    if(y_table == NULL)
        goto error_free;
    u_table = calloc(1, 2<<15);
    if(u_table == NULL)
        goto error_free;
    v_table = calloc(1, 2<<15);
    if(v_table == NULL)
        goto error_free;
    for(i=0; i<(2<<15); i++)
    {
        r = (i & RGB_RMASK) >> 7;
        g = (i & RGB_GMASK) >> 2;
        b = (i & RGB_BMASK) << 3;
        y_table[i] = _rgb2yuv_y(r, g, b);
        u_table[i] = _rgb2yuv_u(r, g, b);
        v_table[i] = _rgb2yuv_v(r, g, b);
    }
    return;
error_free:
    if(y_table)
        free(y_table);
    y_table = NULL;
    if(u_table)
        free(u_table);
    u_table = NULL;
    if(v_table)
        free(v_table);
    v_table = NULL;
    return;
}
static inline int rgb2yuv_y(Uint16 rgb)
{
    return y_table[rgb];
}
static inline int rgb2yuv_u(Uint16 rgb)
{
    return u_table[rgb];
}
static inline int rgb2yuv_v(Uint16 rgb)
{
    return v_table[rgb];
}

static BOOL is_suspend;

#if 0
static sigset_t oldmask;

#define BLOCK_ALARM                    \
do {                            \
    sigset_t newmask;            \
    sigemptyset (&newmask);            \
    sigaddset (&newmask, SIGALRM);            \
    sigprocmask (SIG_BLOCK, &newmask, &oldmask);    \
} while (0);

#define UNBLOCK_ALARM                    \
    sigprocmask (SIG_SETMASK, &oldmask, NULL);
#else
#define BLOCK_ALARM
#define UNBLOCK_ALARM
#endif

/* Initialization/Query functions */
static int MB93493_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **MB93493_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *MB93493_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int MB93493_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void MB93493_UpdateRects (_THIS, int numrects, GAL_Rect *rects);
static void MB93493_VideoQuit(_THIS);

/* Hardware surface functions */
static int MB93493_AllocHWSurface(_THIS, GAL_Surface *surface);
static void MB93493_FreeHWSurface(_THIS, GAL_Surface *surface);

static void* task_do_update (void* data);

/* MB93493 driver bootstrap functions */

static int MB93493_Available(void)
{
    return (1);
}

static void MB93493_DeleteDevice(GAL_VideoDevice *device)
{
    pthread_mutex_destroy (&device->hidden->lock);
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice *MB93493_CreateDevice(int devindex)
{
    GAL_VideoDevice *device;

    if(y_table == NULL)
        init_yuv_table();
    if(y_table == NULL)
    {
        GAL_OutOfMemory();
        return 0;
    }
    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)calloc(1, sizeof(GAL_VideoDevice));
    if (device == NULL)
    {
        GAL_OutOfMemory();
        return 0;
    }
    device->hidden = (struct GAL_PrivateVideoData *)calloc(1, (sizeof *device->hidden));
    if(device->hidden == NULL)
    {
        GAL_OutOfMemory();
        free(device);
        return 0;
    }
    pthread_mutex_init (&device->hidden->lock, NULL);

    /* Set the function pointers */
    device->VideoInit = MB93493_VideoInit;
    device->ListModes = MB93493_ListModes;
    device->SetVideoMode = MB93493_SetVideoMode;
    device->SetColors = MB93493_SetColors;
    device->VideoQuit = MB93493_VideoQuit;
#ifndef _MGRM_THREADS
    device->RequestHWSurface = NULL;
#endif
    device->AllocHWSurface = MB93493_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = MB93493_FreeHWSurface;
    device->UpdateRects = MB93493_UpdateRects;
    device->free = MB93493_DeleteDevice;

    return device;
}

VideoBootStrap MB93493_bootstrap = {
    MB93493VID_DRIVER_NAME, "MB93493 YUV framebuffer driver",
    MB93493_Available, MB93493_CreateDevice
};

static int MB93493_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    fprintf(stderr, "WARNING: You are using the mb93493 video driver!\n");

    VRAM_addr = NULL;
    if (open_vdc_device(MODE_PAL) != 0) {
        fprintf(stderr, "MB93493 NEWGAL Engine: can not open vdc device!\n");
        this->hidden->buffer = NULL;
        this->hidden->is_opened = FALSE;
        return -1;
    }
    this->hidden->is_opened = TRUE;

    is_suspend = FALSE;

    this->hidden->dirty = FALSE;
    SetRect (&this->hidden->update, 0, 0, 0, 0);

    /* Determine the screen depth (use default 8-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
    vformat->BitsPerPixel = RGB_BPP;
    vformat->BytesPerPixel = RGB_PIXEL_LEN;

    /* We're done! */
    return(0);
}

static GAL_Rect standard_mode = {0, 0, VRAM_W, VRAM_H};
static GAL_Rect mini_mode = {0, 0, VRAM_W/2, VRAM_H/2};
static GAL_Rect* modes []  = {
    &standard_mode,
    &mini_mode,
    NULL
};

static GAL_Rect **MB93493_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    if (format->BitsPerPixel == 16)
        return modes;
    return NULL;
}

static GAL_Surface *MB93493_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    int ret;
    if ( this->hidden->buffer )
        free( this->hidden->buffer );
    this->hidden->buffer = NULL;
    this->hidden->buffer = calloc (1, width * height * (bpp / 8));
    if ( this->hidden->buffer == NULL )
        goto error_free;

    /* Allocate the new pixel format for the screen */
    if ( ! GAL_ReallocFormat (current, bpp, RGB_RMASK, RGB_GMASK, RGB_BMASK, 0) )
        goto error_free;

    /* Set up the new mode framebuffer */
    current->flags = flags & GAL_FULLSCREEN;
    this->hidden->w = current->w = width;
    this->hidden->h = current->h = height;
    current->pitch = current->w * (bpp / 8);
    current->pixels = this->hidden->buffer;
    if(width == VRAM_W/2)
        this->hidden->mode = 1;

    ret = pthread_create (&this->hidden->th, NULL, task_do_update, this);
    if(ret!=0)
        goto error_free;

    /* We're done */
    return (current);

error_free:
    fprintf (stderr, "MB93493 NEWGAL Engine: Couldn't init.\n");
    if(this->hidden->buffer)
        free(this->hidden->buffer);
    this->hidden->buffer = 0;
    return NULL;
}

/* We don't actually allow hardware surfaces other than the main one */
static int MB93493_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return (-1);
}
static void MB93493_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}
static int MB93493_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    return (1);
}

static inline void round_rect_to_even (RECT* rc)
{
    if (rc->left % 2) rc->left --;
    if (rc->right % 2) rc->right ++;
}

static inline void update_rgb_to_yuv_packet(Uint16* rgbbuf, YUV_Package* yuv)
{
    yuv->y0 = (unsigned char) rgb2yuv_y(*rgbbuf);
    yuv->y1 = (unsigned char) rgb2yuv_y(*(rgbbuf+1));
    yuv->u  = (unsigned char) ((rgb2yuv_u(*rgbbuf) + rgb2yuv_u(*(rgbbuf+1)))>>1);
    yuv->v  = (unsigned char) ((rgb2yuv_v(*rgbbuf) + rgb2yuv_v(*(rgbbuf+1)))>>1);
}

static inline void update_rgb_to_yuv_packet_minimode(Uint16* rgbbuf, YUV_Package* yuv)
{
    yuv->y0 = (unsigned char) rgb2yuv_y(*rgbbuf);
    yuv->u  = (unsigned char) rgb2yuv_u(*rgbbuf);
    yuv->v  = (unsigned char) rgb2yuv_v(*rgbbuf);
    yuv->y1 = yuv->y0;
}

static inline void update_rect_to_yuv_buffer_page(_THIS, RECT* rect, int idx, int mode_alter)
{
    YUV_Package *yuv;
    unsigned char* yuvrow;
    unsigned char* rgbrow;
    int row, col;
    int rgb_row_len, yuv_row_len;
    int row_alter = (mode_alter == 2)?1:2;
    idx %= 2;
    rgb_row_len = this->hidden->w*RGB_PIXEL_LEN;
    yuv_row_len = VRAM_W*VRAM_PIXEL_LEN;
    row = rect->top;
    if(mode_alter == 2)
    {
        if(idx && !(row%2)) row++;
        if(!idx && row%2) row++;
    }
    rgbrow = this->hidden->buffer + rgb_row_len*row;
    yuvrow = VRAM_addr + idx*VRAM_PAGELEN + yuv_row_len*row;


    if(VRAM_addr == NULL || this->hidden->buffer == NULL)
        return;
    for(; row<rect->bottom; row+=mode_alter)
    {
        yuv = (YUV_Package*)(yuvrow + rect->left*VRAM_PIXEL_LEN*row_alter);
        for(col=rect->left; col < rect->right; col+=mode_alter)
        {
            if(mode_alter == 2)
                update_rgb_to_yuv_packet((Uint16*)(rgbrow+RGB_PIXEL_LEN*col), yuv);
            else
                update_rgb_to_yuv_packet_minimode((Uint16*)(rgbrow+RGB_PIXEL_LEN*col), yuv);
            //memset(yuv, 0x80, sizeof(YUV_Package));
            yuv++;
        }
        rgbrow += rgb_row_len*mode_alter;
        yuvrow += yuv_row_len;
    }
    vdc_set_data(idx);
}

static inline void update_rect_to_yuv_buffer(_THIS, RECT* rect)
{
    int mode_alter = 1;
    if(this->hidden->mode == 0)
        mode_alter = 2;
    update_rect_to_yuv_buffer_page(this, rect, 0, mode_alter);
    update_rect_to_yuv_buffer_page(this, rect, 1, mode_alter);
}

static void* task_do_update (void* data)
{
    _THIS;
    this = data;
    while (this->hidden->buffer && VRAM_addr) {
        usleep (10000);    // 10 ms

        if (is_suspend)
            continue;

        if (this->hidden->dirty) {
            RECT bound;
            pthread_mutex_lock (&this->hidden->lock);

            bound = this->hidden->update;
            SetRect (&this->hidden->update, 0, 0, 0, 0);
            this->hidden->dirty = FALSE;
            round_rect_to_even (&bound);
            BLOCK_ALARM
            update_rect_to_yuv_buffer(this, &bound);
            UNBLOCK_ALARM
            pthread_mutex_unlock (&this->hidden->lock);
        }
        else
            vdc_set_data(0);
    }
    return 0;
}

static void MB93493_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;

    pthread_mutex_lock (&this->hidden->lock);

    bound = this->hidden->update;
    for (i = 0; i < numrects; i++) {
        RECT rc;

        SetRect (&rc, rects[i].x, rects[i].y, 
                        rects[i].x + rects[i].w, rects[i].y + rects[i].h);
        if (IsRectEmpty (&bound))
            bound = rc;
        else
            GetBoundRect (&bound, &bound, &rc);
    }

    if (!IsRectEmpty (&bound)) {
        if (IntersectRect (&bound, &bound, &g_rcScr)) {
            this->hidden->update = bound;
            this->hidden->dirty = TRUE;
        }
    }

    pthread_mutex_unlock (&this->hidden->lock);
}


static void MB93493_VideoQuit(_THIS)
{
    if (this->hidden->is_opened) {
        //close_vdc_device();
        VRAM_addr = NULL;
        if(this->hidden->buffer)
            free(this->hidden->buffer);
        this->hidden->buffer = NULL;
        this->hidden->is_opened = FALSE;
        //pthread_join(this->hidden->th, NULL);
    }
    if (this->screen->pixels != NULL)
    {
        free(this->screen->pixels);
        this->screen->pixels = NULL;
    }
}

int mb93493_suspend_spi (void)
{
    if (is_suspend)
        return -1;

    BLOCK_ALARM
    is_suspend = TRUE;
    return 0;
}

void mb93493_resume_spi (void)
{
    if (!is_suspend)
        return;
    is_suspend = FALSE;
    UNBLOCK_ALARM
    MB93493_UpdateRects (current_video, 1, &standard_mode);
}

