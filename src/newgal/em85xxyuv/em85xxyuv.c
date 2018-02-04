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
**  $Id: em85xxyuv.c 8944 2007-12-29 08:29:16Z xwyan $
**
**  em85xxyuv.c: NEWGAL driver for EM85xx YUV.
**  
**  Copyright (C) 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_EM85XXYUV

#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <signal.h>
#include <sched.h>
#include <errno.h>

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"


#include "realmagichwl_userland/caribbean_plainc.h"
#include "realmagichwl_kernelland/realmagichwl.h"
#include "realmagichwl_userland/realmagichwl_userland_api.h"
#include "realmagichwl_kernelland/include/rm84cmn.h"

#include "em85xxyuv.h"

#define EM85XXYUVVID_DRIVER_NAME "em85xxyuv"
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
 /* union semun is defined by including */
#else

 /* according to X/OPEN we have to define it ourselves */
union semun {
	int val;                  /* value for SETVAL */
	struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
	unsigned short *array;    /* array for GETALL, SETALL */
	/* Linux specific part: */
	struct seminfo *__buf;    /* buffer for IPC_INFO */
};
#endif
/* Initialization/Query functions */
static int EM85XXYUV_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **EM85XXYUV_ListModes(_THIS, GAL_PixelFormat *format, 
                Uint32 flags);
static GAL_Surface *EM85XXYUV_SetVideoMode(_THIS, GAL_Surface *current, 
                int width, int height, int bpp, Uint32 flags);
static int EM85XXYUV_SetColors(_THIS, int first, int count, GAL_Color *palette);
static void EM85XXYUV_VideoQuit(_THIS);

/* Hardware surface functions */
static int EM85XXYUV_AllocHWSurface(_THIS, GAL_Surface *surface);
static void EM85XXYUV_FreeHWSurface(_THIS, GAL_Surface *surface);
static void EM85XXYUV_UpdateRects (_THIS, int numrects, GAL_Rect *rects);

/* Implementation of lock based-on SysV semaphore */

#define KEY_SEM_SET     0x464D47C0

static int create_lock (void)
{
    int semid;
    union semun sunion;

    semid = semget (KEY_SEM_SET, 1, 0);
    if (semid != -1)
        semctl (semid, 0, IPC_RMID);

    semid = semget (KEY_SEM_SET, 1, 0666 | IPC_CREAT | IPC_EXCL);
    if (semid == -1) {
        return -1;
    }

    sunion.val = 1;
    semctl (semid, 0, SETVAL, sunion);

    return semid;
}

static void delete_lock (int semid)
{
    semctl (semid, 0, IPC_RMID);
}

/* Down/up a semaphore uninterruptablly. */
static void my_sem_op (int semid, int sem_num, int value)
{
    struct sembuf sb;

again:
    sb.sem_num = sem_num;
    sb.sem_op = value;
    sb.sem_flg = 0;

    if (semop (semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

#define update_lock(semid)  my_sem_op(semid, 0, -1)
#define update_unlock(semid) my_sem_op(semid, 0, 1)

static __inline BYTE rgb2y (BYTE r, BYTE g, BYTE b)
{
    DWORD y = 257*(DWORD)r + 504*(DWORD)g + 98*(DWORD)b + 16000;
    if (y > 255000)
        y = 255000;    
    y >>= 10;
    return (BYTE)y;
}

static __inline BYTE rgb2u (BYTE r, BYTE g, BYTE b)
{
    DWORD u = -148*(DWORD)r - 291*(DWORD)g + 449*(DWORD)b + 128000;
    if (u > 255000)
        u = 255000;    
    u >>= 10;
    return (BYTE)u;
}

static __inline BYTE rgb2v (BYTE r, BYTE g, BYTE b)
{
    DWORD v = 439*(DWORD)r - 368*(DWORD)g - 71*(DWORD)b + 128000;
    if (v > 255000)
        v = 255000;    
    v >>= 10;
    return (BYTE)v;
}

static void RGB2YUV (Uint8 r, Uint8 g, Uint8 b, Uint8* y, Uint8* u, Uint8* v)
{
    *y = rgb2y (r, g, b); /* 0.299*r +0.587*g + 0.114*b; */
    *u = rgb2u (r, g, b); /* (b-yuv[0])*0.565 + 128; */
    *v = rgb2v (r, g, b); /* (r-yuv[0])*0.713 + 128; */
}

static YUV* init_rgb2yuv_map_16bit (void)
{
    int i;
    YUV *map, *tmp;

    map = calloc (65536, sizeof (YUV));

    if (map == NULL)
        return NULL;

    tmp = map;
    for (i = 0; i < 65536; i++) {
        RGB2YUV (((i & 0xF800) >> 8), ((i & 0x07E0) >> 3), ((i & 0x001F) << 3), 
                &tmp->y, &tmp->u, &tmp->v);
        tmp++;
    }

    return map;
}

#define FRAME_WIDTH (720)
#define FRAME_HEIGHT (480)
#define FRAME_SIZE (FRAME_WIDTH * FRAME_HEIGHT)

static BYTE plane_Y [FRAME_SIZE];
static BYTE plane_UV [FRAME_SIZE/2];

static void round_rect_to_even (RECT* rc)
{
    if (rc->left % 2) rc->left --;
    if (rc->right % 2) rc->right ++;
}

static void convert2yuv (_THIS, int x, int y, int w, int h)
{
    int i, j;
    YUV *yuv = (YUV*) (this->hidden->rgb2yuv_map);
    Uint8 *src_buff;
    Uint8 *dst_plane_y, *dst_plane_uv;

    Uint16 *src_line;
    Uint8 *dst_line_y, *dst_line_uv;

    src_buff = this->hidden->shadow_fb
                    + this->screen->pitch * y
                    + this->screen->format->BytesPerPixel * x;

    dst_plane_y = plane_Y + FRAME_WIDTH * y + x;
    dst_plane_uv = plane_UV + FRAME_WIDTH * (y>>1) + x;

    for (i = 0; i < h; i++) {
        src_line = (Uint16*)src_buff;
        dst_line_y = dst_plane_y;
        dst_line_uv = dst_plane_uv;

        for (j = 0; j < w; j++) {
            dst_line_y [j] = yuv [*src_line].y;

            if ((i & 1) == 0 && (j & 1) == 0) {

#if 0
                /* flicker filter */
                BYTE u, v;
                Uint16* next_line;
                if (i + y == 0) {
                    next_line = src_line + FRAME_WIDTH;
                    u = yuv [*src_line].u >> 1;
                    u += yuv [*next_line].u >> 1;
                    v = yuv [*src_line].v >> 1;
                    v += yuv [*next_line].v >> 1;
                }
                else {
                    next_line = src_line - FRAME_WIDTH;
                    u = yuv [*next_line].u >> 2;
                    u += yuv [*src_line].u >> 1;

                    v = yuv [*next_line].v >> 2;
                    v += yuv [*src_line].v >> 1;

                    next_line = src_line + FRAME_WIDTH;
                    u += yuv [*next_line].u >> 2;
                    v += yuv [*next_line].v >> 2;
                }

                dst_line_uv [j] = u;
                dst_line_uv [j + 1] = v;
#else
                dst_line_uv [j] = yuv [*src_line].u;
                dst_line_uv [j + 1] = yuv [*src_line].v;
#endif
            }
            src_line++;
        }
        src_buff += this->screen->pitch;
        dst_plane_y += FRAME_WIDTH;

        if ((i & 1) == 0) {
            dst_plane_uv += FRAME_WIDTH;
        }
    }
}

static void transfer_yuv_frame (_THIS, int x, int y, int width, int height)
{
    RUA_handle h = this->hidden->handle;
    YUVframe f;
    int y_offset, uv_offset, y_xferlen, uv_xferlen, y_left, uv_left;
    BYTE* Y = plane_Y;
    BYTE* UV = plane_UV;
    
#if 0
    Wnd_type Wnd;
    evYUVWriteParams_type yuv_param;

    Wnd.x = 0;
    Wnd.y = 0;
    Wnd.w = FRAME_WIDTH;
    Wnd.h = FRAME_HEIGHT;

    yuv_param.wWidth = FRAME_WIDTH;
    yuv_param.wHeight = FRAME_HEIGHT;
    yuv_param.YUVFormat = YUV_420_UNPACKED;
    RUA_DECODER_SET_PROPERTY(h, VIDEO_SET, evYUVWriteParams, 
                    sizeof(yuv_param), &yuv_param);
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, evSourceWindow, sizeof(Wnd), &Wnd);
    RUA_DECODER_CLEAR_SCREEN(h);
#endif

    asm ("mcr p15, 0, r0, c7, c10, 0\n");

    /*
     * transfer the YUV frame
     * the maximum length for 1 transfer = 0xffff
     * so we do a loop for the complete transfer
     */

    y_offset = 0; 
    uv_offset = 0;
    y_left = FRAME_WIDTH * FRAME_HEIGHT;
    uv_left = FRAME_WIDTH * FRAME_HEIGHT / 2;

    while (y_left || uv_left) {
        /* calculate the proper transfer size */
        y_xferlen = y_left;
        if (y_left > 0xf000)
            y_xferlen = 0xf000;
        uv_xferlen = uv_left;
        if (uv_left > 0xf000)
            uv_xferlen = 0xf000;

        /* do the transfer */
        f.Yaddr = y_offset;

        /* 
         * don't worry about the warning from the compiler here 
         * we know that this cast is safe
         */
        f.pY = (DWORD *)Y;
        f.nYbytes = y_xferlen;
        f.UVaddr = uv_offset;

        /* 
         * don't worry about the warning from the compiler here 
         * we know that this cast is safe
         */
        f.pUV = (DWORD *)UV;
        f.nUVbytes = uv_xferlen;

        /* 4:2:0 format */
        RUA_DECODER_DISPLAY_YUV (h, &f);
        if (y_left) {
            y_left -= y_xferlen;
            y_offset += y_xferlen;
            Y += y_xferlen;
        }
        if (uv_left) {
            uv_left -= uv_xferlen;
            uv_offset += uv_xferlen;
            UV += uv_xferlen;
        }
    }
}

static char stack_updater [512];

static int task_do_update (void* data)
{
    _THIS;

    this = data;

    while (this->hidden->status == 2) {
        usleep (30000);    /* 30 ms */

        if (this->hidden->dirty) {
            RECT bound;

            update_lock (this->hidden->lock);

            bound = this->hidden->update;
            round_rect_to_even (&bound);

            convert2yuv (this, bound.left, bound.top, 
                            RECTW (bound), RECTH (bound));
            this->hidden->dirty = FALSE;
            SetRect (&this->hidden->update, 0, 0, 0, 0);

            update_unlock (this->hidden->lock);

            transfer_yuv_frame (this, bound.left, bound.top, 
                            RECTW (bound), RECTH (bound));
        }
    }

    return 0;
}


/* EM85XXYUV driver bootstrap functions */

static int EM85XXYUV_Available(void)
{
    return(1);
}

static void EM85XXYUV_DeleteDevice(GAL_VideoDevice *device)
{
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice *EM85XXYUV_CreateDevice(int devindex)
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
    device->VideoInit = EM85XXYUV_VideoInit;
    device->ListModes = EM85XXYUV_ListModes;
    device->SetVideoMode = EM85XXYUV_SetVideoMode;
    device->SetColors = EM85XXYUV_SetColors;
    device->VideoQuit = EM85XXYUV_VideoQuit;
#ifndef _MGRM_THREADS
    device->RequestHWSurface = NULL;
#endif
    device->AllocHWSurface = EM85XXYUV_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = EM85XXYUV_FreeHWSurface;
    device->UpdateRects = EM85XXYUV_UpdateRects;

    device->free = EM85XXYUV_DeleteDevice;

    return device;
}

VideoBootStrap EM85XXYUV_bootstrap = {
    EM85XXYUVVID_DRIVER_NAME, "EM85xx YUV video driver",
    EM85XXYUV_Available, EM85XXYUV_CreateDevice
};

#ifdef  _MGGAL_EM85XXOSD
RUA_handle __mg_em85xx_rua_handle;
#endif

static int EM85XXYUV_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    RUA_handle h;
    Wnd_type Wnd;
    evYUVWriteParams_type yuv_param;
    evOutputDevice_type OutputDevice;
    evTvStandard_type TvStandard;
    evTvOutputFormat_type TvOutputFormat;

    fprintf (stderr, "NEWGAL>EM85xxYUV: Calling init method!\n");

    if ((h = RUA_OpenDevice (0)) == -1) {
        fprintf (stderr, "NEWGAL>EM85xxYUV: Can't open kernel module\n");
        return -1;
    }

    this->hidden->handle = h;

#ifdef _MGGAL_EM85XXOSD
    __mg_em85xx_rua_handle = h;
#endif

    this->hidden->w = FRAME_WIDTH;
    this->hidden->h = FRAME_HEIGHT;
    this->hidden->shadow_pitch = FRAME_WIDTH * 2;
    this->hidden->shadow_fb = malloc (FRAME_WIDTH * FRAME_HEIGHT * 2);
    this->hidden->rgb2yuv_map = init_rgb2yuv_map_16bit ();

    if (this->hidden->shadow_fb == NULL ||
            this->hidden->rgb2yuv_map == NULL) {
        fprintf (stderr, "NEWGAL>EM85xxYUV: Can't allocate shadow FB\n");

        goto fail;
    }

#if 0
    /* XXX If we need to we can mmap the osdbuf from the device */
    asm ("mcr p15, 0, r0, c7, c10, 0\n");

    if (RUA_OSDFB_REFRESH (this->hidden->handle, &osdbuffer) != 0) {
        fprintf (stderr, "NEWGAL>EM85xxYUV: Error getting the OSD buffer.\n");
        goto fail;
    } 
    else {
        this->hidden->osd_buffer = osdbuffer.framebuffer;
        this->hidden->w = osdbuffer.width;
        this->hidden->h = osdbuffer.height;
        this->hidden->pitch = osdbuffer.width;
        this->hidden->fb = osdbuffer.framebuffer + 8 + 1024;
    }
#endif
 
    /* set the tv output to be ntsc */
    Wnd.x = 0;
    Wnd.y = 0;
    Wnd.w = FRAME_WIDTH;
    Wnd.h = FRAME_HEIGHT;
    TvStandard = evTvStandard_NTSC;
    TvOutputFormat = evTvOutputFormat_COMPOSITE;
    OutputDevice = evOutputDevice_TV;
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, 
                    evTvOutputFormat, sizeof(TvOutputFormat), &TvOutputFormat);
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, 
                    evTvStandard, sizeof(TvStandard), &TvStandard);
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, 
                    evOutputDevice, sizeof(OutputDevice), &OutputDevice);
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, 
                    evDestinationWindow, sizeof(Wnd), &Wnd);

    yuv_param.wWidth = FRAME_WIDTH;
    yuv_param.wHeight = FRAME_HEIGHT;
    yuv_param.YUVFormat = YUV_420_UNPACKED;
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, 
                    evYUVWriteParams, sizeof(yuv_param), &yuv_param);
    RUA_DECODER_SET_PROPERTY (h, VIDEO_SET, 
                    evSourceWindow, sizeof(Wnd), &Wnd);
    RUA_DECODER_CLEAR_SCREEN (h);

#if 0
    /* Setup the flicker filter
     * XXX - its also set in the kernel module, but it seems not to work 
     * very well. 
     * We reset it here, so that the microcode as already seen an osd frame.    
     * 0 <= flicker <= 15
     */
    flicker = 15;
    RUA_DECODER_SET_PROPERTY (this->hidden->handle, 
                    DECODER_SET, edecOsdFlicker, sizeof(flicker), &flicker);

    /* 
     * set the osd window destination
     * do not scale - just center the image
     */
    Wnd.x = (720 - osdbuffer.width) / 2;
    Wnd.y = (480 - osdbuffer.height) / 2;
    Wnd.w = osdbuffer.width;
    Wnd.h = osdbuffer.height;
    RUA_DECODER_SET_PROPERTY (h, OSD_SET, 
                    eOsdDestinationWindow, sizeof(Wnd), &Wnd);
#endif

    vformat->BitsPerPixel = 16;
    vformat->BytesPerPixel = 2;

    this->hidden->status = 2;
    this->hidden->dirty = FALSE;
    SetRect (&this->hidden->update, 0, 0, 0, 0);

    this->hidden->lock = create_lock ();
    if (this->hidden->lock == -1) {
        perror ("NEWGAL>EM85xxYUV: Can not create lock");
        goto fail;
    }

    /* We're done! */
    return (0);

fail:
    free (this->hidden->shadow_fb);
    this->hidden->shadow_fb = NULL;
    free (this->hidden->rgb2yuv_map);
    return -1;
}

static GAL_Rect mode = {0, 0, 720, 480};
static GAL_Rect* modes []  = {
    &mode,
    NULL
};

static GAL_Rect **EM85XXYUV_ListModes(_THIS, GAL_PixelFormat *format, 
                Uint32 flags)
{
    if (format->BitsPerPixel == 16) {
        return modes;
    }

    return NULL;
}

static GAL_Surface *EM85XXYUV_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    /* Set up the new mode framebuffer */
    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w = this->hidden->w;
    current->h = this->hidden->h;
    current->pitch = this->hidden->shadow_pitch;
    current->pixels = this->hidden->shadow_fb;

    if (!GAL_ReallocFormat (current, 16, 0, 0, 0, 0) ) {
        return (NULL);
    }

    clone (task_do_update, stack_updater + 512, 
                    CLONE_VM | CLONE_FS | CLONE_FILES, this);

    /* We're done */
    return (current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int EM85XXYUV_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return (-1);
}

static void EM85XXYUV_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int EM85XXYUV_SetColors(_THIS, int first, int count, GAL_Color *palette)
{
    /* do nothing of note. */
    return(1);
}

static void EM85XXYUV_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;

    update_lock (this->hidden->lock);

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

    update_unlock (this->hidden->lock);
}

static void EM85XXYUV_VideoQuit(_THIS)
{
    if (this->hidden->status != 2)
        return;
    this->hidden->status = 1;

    if (this->hidden->shadow_fb) {
        void* tmp = this->hidden->shadow_fb;

        this->hidden->dirty = FALSE;
        this->hidden->shadow_fb = NULL;

        free (this->hidden->rgb2yuv_map);
        free (tmp);
    }

    if (this->screen && this->screen->pixels) {
        this->screen->pixels = NULL;
    }

    delete_lock (this->hidden->lock);
    RUA_ReleaseDevice (this->hidden->handle);

    return;
}

#endif /* _MGGAL_EM85XXYUV */

