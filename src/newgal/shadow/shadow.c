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
 *   Copyright (C) 2002~2023, Beijing FMSoft Technologies Co., Ltd.
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
 **  shadow.c: Shadow NEWGAL video driver.
 **    Can be used to provide support for no-access to frame buffer directly.
 **    Can be used to provide support for depth less than 8bpp.
 **    DONOT use this engine with compositing schema.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <time.h>

#define _DEBUG

#include "common.h"

#if __LINUX__
#include <unistd.h>
#endif

#ifdef _MGGAL_SHADOW

#ifdef _MGRM_PROCESSES

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>

union semun {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
};

#endif

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#define SHADOWVID_DRIVER_NAME "shadow"
#include "shadow.h"

#define FLAG_REALFB_PREALLOC  0x01
#define _ROT_DIR_CW 0x02
#define _ROT_DIR_CCW 0x04
#define _ROT_DIR_HFLIP 0x08
#define _ROT_DIR_VFLIP 0x10

#define SHADOW_SHM_KEY 0x4D475344
#define SHADOW_SEM_KEY 0x4D475344

#define PALETTE_SIZE 1024

extern void _get_dst_rect_cw (RECT* dst_rect, const RECT* src_rect, RealFBInfo *realfb_info);
extern void _get_dst_rect_hflip (RECT* src_rect, RealFBInfo *realfb_info);
extern void _get_dst_rect_vflip (RECT* src_rect, RealFBInfo *realfb_info);
extern void _get_dst_rect_ccw (RECT* dst_rect, const RECT* src_rect, RealFBInfo *realfb_info);
extern GAL_VideoDevice *GAL_GetVideo(const char* driver_name, BOOL check_compos);

#ifdef _DEBUG
static double get_elapsed_seconds(const struct timespec *ts_from,
        const struct timespec *ts_to)
{
    struct timespec ts_curr;
    time_t ds;
    long dns;

    if (ts_to == NULL) {
        clock_gettime(CLOCK_REALTIME, &ts_curr);
        ts_to = &ts_curr;
    }

    ds = ts_to->tv_sec - ts_from->tv_sec;
    dns = ts_to->tv_nsec - ts_from->tv_nsec;
    return ds + dns * 1.0E-9;
}
#endif

static void refresh_ccw_32bpp(ShadowFBHeader* shadowfb_header,
        RealFBInfo* realfb_info, void* update)
{
    RECT src_update = *(RECT*)update;
    RECT dst_update;
    const BYTE *src_bits;
    BYTE *dst_line;
    int dst_width, dst_height;
    int x, y;

#if 0 // def _DEBUG
    struct timespec ts_start;
    clock_gettime(CLOCK_REALTIME, &ts_start);
#endif

    _get_dst_rect_ccw(&dst_update, &src_update, realfb_info);
    dst_width = RECTW(dst_update);
    dst_height = RECTH(dst_update);
    BYTE line_pixels[dst_width * 4];

    /* Copy the bits from Shadow FrameBuffer to real FrameBuffer */
    src_bits = (BYTE *)shadowfb_header + shadowfb_header->fb_offset;
    src_bits += src_update.top * shadowfb_header->pitch +
        src_update.left * 4;
    dst_line = (BYTE *)realfb_info->fb + (dst_update.bottom - 1) *
        realfb_info->pitch;

    for (x = 0; x < dst_height; x++) {
        /* Copy the bits from vertical line to horizontal line */
        const BYTE* ver_bits = src_bits;
        BYTE* hor_bits = line_pixels;

        for (y = 0; y < dst_width; y++) {
            *(Uint32 *)hor_bits = *(Uint32 *)ver_bits;
            ver_bits += shadowfb_header->pitch;
            hor_bits += 4;
        }

        memcpy(dst_line + (dst_update.left << 2), line_pixels,
                dst_width << 2);
        src_bits += 4;
        dst_line -= realfb_info->pitch;
    }

#if 0 // def _DEBUG
    double elapsed = get_elapsed_seconds(&ts_start, NULL);
    _MG_PRINTF("Cosumed time to rotate dirty rect (ccw): %f (seconds)\n", elapsed);
#endif
}

static void refresh_cw_32bpp(ShadowFBHeader* shadowfb_header,
        RealFBInfo* realfb_info, void* update)
{
    RECT src_update = *(RECT*)update;
    RECT dst_update;
    const BYTE *src_bits;
    BYTE *dst_line;
    int dst_width, dst_height;
    int x, y;

#if 0 // def _DEBUG
    struct timespec ts_start;
    clock_gettime(CLOCK_REALTIME, &ts_start);
#endif

    _get_dst_rect_cw(&dst_update, &src_update, realfb_info);
    dst_width = RECTW(dst_update);
    dst_height = RECTH(dst_update);
    BYTE line_pixels[dst_width * 4];

    /* Copy the bits from Shadow FrameBuffer to real FrameBuffer */
    src_bits = (BYTE *)shadowfb_header + shadowfb_header->fb_offset;
    src_bits += (src_update.bottom - 1) * shadowfb_header->pitch +
        src_update.left * 4;
    dst_line = (BYTE *)realfb_info->fb + dst_update.top *
        realfb_info->pitch;

    int src_pitch = shadowfb_header->pitch / 4;
    int dst_pitch = realfb_info->pitch / 4;
    for (x = 0; x < dst_height; x++) {
        /* Copy the bits from vertical line to horizontal line */
        const Uint32 *src_pixels = (const Uint32 *)src_bits;
        Uint32 *dst_pixels = (Uint32 *)dst_line;
        dst_pixels += dst_update.left;

        for (y = 0; y < dst_width; y++) {
            *dst_pixels = *src_pixels;
            src_pixels -= src_pitch;
            dst_pixels += 1;
        }

        src_bits += 4;
        dst_line += realfb_info->pitch;
    }

#if 0 // def _DEBUG
    double elapsed = get_elapsed_seconds(&ts_start, NULL);
    _MG_PRINTF("Cosumed time to rotate dirty rect (cw): %f (seconds)\n", elapsed);
#endif
}

extern void refresh_normal_msb_left (ShadowFBHeader * shadowfb_header,
        RealFBInfo *realfb_info, void* update);

extern void refresh_cw_msb_left (ShadowFBHeader *shadowfb_header,
        RealFBInfo *realfb_info, void* update);

extern void refresh_ccw_msb_left (ShadowFBHeader* shadowfb_header,
        RealFBInfo* realfb_info, void* update);

extern void refresh_hflip_msb_left (ShadowFBHeader* shadowfb_header,
        RealFBInfo* realfb_info, void* update);

extern void refresh_vflip_msb_left (ShadowFBHeader* shadowfb_header,
        RealFBInfo* realfb_info, void* update);

extern void refresh_normal_msb_right (ShadowFBHeader * shadowfb_header,
        RealFBInfo *realfb_info, void* update);

extern void refresh_cw_msb_right (ShadowFBHeader *shadowfb_header,
        RealFBInfo *realfb_info, void* update);

extern void refresh_ccw_msb_right (ShadowFBHeader* shadowfb_header,
        RealFBInfo* realfb_info, void* update);

extern void refresh_hflip_msb_right (ShadowFBHeader* shadowfb_header,
        RealFBInfo* realfb_info, void* update);

extern void refresh_vflip_msb_right (ShadowFBHeader* shadowfb_header,
        RealFBInfo* realfb_info, void* update);

/* Initialization/Query functions */
static int SHADOW_VideoInit (_THIS, GAL_PixelFormat *vformat);
static int RealEngine_SetPalette(RealFBInfo *realfb_info, int firstcolor, int ncolors, void *color);
static GAL_Rect **SHADOW_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *SHADOW_SetVideoMode (_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int SHADOW_SetColors (_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void SHADOW_VideoQuit (_THIS);
static BOOL SHADOW_SyncUpdate (_THIS);
static BOOL SHADOW_SyncUpdateAsync (_THIS);

/* Hardware surface functions */
static int SHADOW_AllocHWSurface (_THIS, GAL_Surface *surface)
{
    GAL_VideoDevice *real_device;
    real_device = this->hidden->realfb_info->real_device;
    return real_device->AllocHWSurface(real_device, surface);
}

static void SHADOW_FreeHWSurface (_THIS, GAL_Surface *surface)
{
    GAL_VideoDevice *real_device;
    if (this->hidden->realfb_info) {
        real_device = this->hidden->realfb_info->real_device;
        real_device->FreeHWSurface(real_device, surface);
    }
}

static int SHADOW_CheckHWBlit(_THIS, GAL_Surface *src, const GAL_Rect *srcrc,
            GAL_Surface *dst, const GAL_Rect *dstrc, DWORD op)
{
    GAL_VideoDevice *real_device;
    real_device = this->hidden->realfb_info->real_device;
    return real_device->CheckHWBlit(real_device, src, srcrc, dst, dstrc, op);
}

static int SHADOW_FillHWRect(_THIS, GAL_Surface *dst, const GAL_Rect *dstrc,
            Uint32 color)
{
    GAL_VideoDevice *real_device;
    real_device = this->hidden->realfb_info->real_device;
    return real_device->FillHWRect(real_device, dst, dstrc, color);
}

#ifdef _MGRM_PROCESSES
static int shmid;
/* Down/up a semaphore uninterruptablly. */
static void _sysvipc_sem_op (int semid, int sem_num, int value)
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
#endif

static ShadowFBHeader* _shadowfbheader;

DWORD __mg_shadow_rotate_flags;
extern void (*__mg_ial_change_mouse_xy_hook)(int* x, int* y);
extern GAL_Surface* __gal_screen;
static ShadowFBOps shadow_fb_ops;

/* SHADOW driver bootstrap functions */
static int SHADOW_Available(void)
{
    return 1;
}

static void SHADOW_DeleteDevice(_THIS)
{
    if (this->hidden->realfb_info) {
        shadow_fb_ops.release(this->hidden->realfb_info);
        this->hidden->realfb_info = NULL;
    }

    free(this->hidden);
    free(this);
}

static void SHADOW_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;

#ifdef _MGRM_PROCESSES
    _sysvipc_sem_op (this->hidden->semid, 0, -1);
#endif

    bound = _shadowfbheader->dirty_rect;

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
        RECT rcScr = GetScreenRect();

        if (IntersectRect (&bound, &bound, &rcScr)) {
            _shadowfbheader->dirty_rect = bound;
            _shadowfbheader->dirty = TRUE;
        }
    }

#ifdef _MGRM_PROCESSES
    _sysvipc_sem_op (this->hidden->semid, 0, 1);
#endif
}

static GAL_VideoDevice *SHADOW_CreateDevice(int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if (device) {
        memset (device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *)
            malloc (sizeof(struct GAL_PrivateVideoData));
    }
    if ( (device == NULL) || (device->hidden == NULL)) {
        GAL_OutOfMemory ();
        if (device) {
            free (device);
        }
        return (0);
    }
    memset (device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = SHADOW_VideoInit;
    device->ListModes = SHADOW_ListModes;
    device->SetVideoMode = SHADOW_SetVideoMode;
    device->SetColors = SHADOW_SetColors;
    device->VideoQuit = SHADOW_VideoQuit;
    device->AllocHWSurface = NULL;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->FreeHWSurface = NULL;
    device->UpdateRects = SHADOW_UpdateRects;
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
        device->SyncUpdate = SHADOW_SyncUpdate;
    else
        device->SyncUpdate = NULL;
#else
    device->SyncUpdate = SHADOW_SyncUpdate;
#endif

    device->free = SHADOW_DeleteDevice;
    return device;
}

VideoBootStrap SHADOW_bootstrap = {
    SHADOWVID_DRIVER_NAME, "Shadow LCD video driver",
    SHADOW_Available, SHADOW_CreateDevice
};

#define LEN_MODE    32

static int RealEngine_GetInfo (RealFBInfo * realfb_info)
{
    GAL_PixelFormat real_vformat;
    char engine[LEN_ENGINE_NAME + 1], mode[LEN_MODE+1];
    char rotate_screen[LEN_MODE+1];
    int w, h, depth, pitch_size;
    GAL_VideoDevice* real_device;

    if(realfb_info == NULL)
        return -1;

    if (GetMgEtcValue ("shadow", "real_engine",
                engine, LEN_ENGINE_NAME) < 0) {
        return -1;
    }
    if (GetMgEtcValue ("shadow", "rotate_screen",
                rotate_screen, LEN_MODE) < 0) {
        memset(rotate_screen, 0, sizeof(rotate_screen));
    }

    if (GetMgEtcValue (engine, "defaultmode", mode, LEN_MODE) < 0){
        if (GetMgEtcValue ("system", "defaultmode", mode, LEN_MODE) < 0)
            return -1;
    }

    w = atoi (mode);
    h = atoi (strchr (mode, 'x') + 1);
    depth = atoi (strrchr (mode, '-') + 1);

    memset (&real_vformat, 0, sizeof(real_vformat));
    real_device = GAL_GetVideo (engine, FALSE);

    if (real_device == NULL) {
        _ERR_PRINTF ("NEWGAL>SHADOW: can not find real engine (%s)\n", engine);
        return -1;
    }

    realfb_info->real_device = real_device;
    real_device->VideoInit(realfb_info->real_device, &real_vformat);

    GAL_Surface *prev_surf, *screen;
    prev_surf = GAL_CreateRGBSurface (GAL_SWSURFACE,
            0, 0, real_vformat.BitsPerPixel, real_vformat.Rmask,
            real_vformat.Gmask, real_vformat.Bmask, real_vformat.Amask);

    real_device->screen = NULL;
    /* VW: SetVideoMode may return a new surface */
    screen = real_device->SetVideoMode(realfb_info->real_device,
            prev_surf, w, h, depth, GAL_HWPALETTE);
    real_device->screen = (screen != NULL) ? screen : prev_surf;
    if (real_device->screen == NULL) {
        _ERR_PRINTF ("NEWGAL>SHADOW: can't create screen of real engine.\n");
        return -1;
    }

    realfb_info->height = real_device->screen->h;
    realfb_info->width = real_device->screen->w;
    realfb_info->depth = real_device->screen->format->BitsPerPixel;
    realfb_info->fb = real_device->screen->pixels;
    realfb_info->pitch = real_device->screen->pitch;
    realfb_info->flags = 0;

    if (realfb_info->depth <= 8)
        pitch_size = realfb_info->width;
    else
        pitch_size = realfb_info->pitch;

    if (!strncmp(rotate_screen, "cw", sizeof(rotate_screen)))
        realfb_info->flags = _ROT_DIR_CW;
    else if (!strncmp(rotate_screen, "ccw", sizeof(rotate_screen)))
        realfb_info->flags = _ROT_DIR_CCW;
    else if (!strncmp(rotate_screen, "hflip", sizeof(rotate_screen)))
        realfb_info->flags = _ROT_DIR_HFLIP;
    else if (!strncmp(rotate_screen, "vflip", sizeof(rotate_screen)))
        realfb_info->flags = _ROT_DIR_VFLIP;

    __mg_shadow_rotate_flags = realfb_info->flags;

    if (real_device->screen->format->MSBLeft) {
        if (realfb_info->flags & _ROT_DIR_CW)
            shadow_fb_ops.refresh = refresh_cw_msb_left;
        else if (realfb_info->flags & _ROT_DIR_CCW)
            shadow_fb_ops.refresh = refresh_ccw_msb_left;
        else if (realfb_info->flags & _ROT_DIR_HFLIP)
            shadow_fb_ops.refresh = refresh_hflip_msb_left;
        else if(realfb_info->flags & _ROT_DIR_VFLIP)
            shadow_fb_ops.refresh = refresh_vflip_msb_left;
        else
            shadow_fb_ops.refresh = refresh_normal_msb_left;
    }
    else {
        if (realfb_info->flags & _ROT_DIR_CW)
            shadow_fb_ops.refresh = refresh_cw_msb_right;
        else if (realfb_info->flags & _ROT_DIR_CCW)
            shadow_fb_ops.refresh = refresh_ccw_msb_right;
        else if (realfb_info->flags & _ROT_DIR_HFLIP)
            shadow_fb_ops.refresh = refresh_hflip_msb_right;
        else if (realfb_info->flags & _ROT_DIR_VFLIP)
            shadow_fb_ops.refresh = refresh_vflip_msb_right;
        else
            shadow_fb_ops.refresh = refresh_normal_msb_right;
    }

    return 0;
}

static int RealEngine_Release(RealFBInfo * realfb_info)
{
    if (realfb_info && realfb_info->real_device) {
        GAL_VideoDevice *video = realfb_info->real_device;
        GAL_VideoDevice *this  = realfb_info->real_device;

        if (realfb_info->flags == FLAG_REALFB_PREALLOC)
            realfb_info->fb = NULL;

        video->VideoQuit (this);
        if (video->physpal)
        {
            free (video->physpal->colors);
            free (video->physpal);
            video->physpal = NULL;
        }

        /* Finish cleaning up video subsystem */
        video->free (this);
    }

    return 0;
}

static int RealEngine_SetPalette(RealFBInfo *realfb_info, int firstcolor,
        int ncolors, void *colors)
{
    GAL_Color* pal_colors;
    pal_colors = (GAL_Color*)((char*)_shadowfbheader + _shadowfbheader->palette_offset);

    if (colors != pal_colors + firstcolor)
        memcpy (pal_colors + firstcolor, colors, ncolors * sizeof(GAL_Color));

    return 1;
}

static int RealEngine_Init(void)
{
    shadow_fb_ops.get_realfb_info = RealEngine_GetInfo;
    shadow_fb_ops.init = RealEngine_Init;
    shadow_fb_ops.release = RealEngine_Release;
    shadow_fb_ops.set_palette = RealEngine_SetPalette;

    return 0;
}

static int SHADOW_LockHWSurface(_THIS, GAL_Surface *surface)
{
    if (surface == this->screen && this->hidden->async_update) {
#if USE_UPDATE_SEM
        if (sem_wait(&this->hidden->update_sem))
            _ERR_PRINTF("Failed sem_wait(): %m\n");
#else
        if (pthread_mutex_lock(&this->hidden->update_lock))
            _ERR_PRINTF("Failed pthread_mutex_lock(): %m\n");
#endif
        return 0;
    }

    return -1;
}

static void SHADOW_UnlockHWSurface(_THIS, GAL_Surface *surface)
{
    if (surface == this->screen && this->hidden->async_update) {
#if USE_UPDATE_SEM
        if (sem_post(&this->hidden->update_sem))
            _ERR_PRINTF("Failed sem_post(): %m\n");
#else
        if (pthread_mutex_unlock(&this->hidden->update_lock))
            _ERR_PRINTF("Failed pthread_mutex_unlock(): %m\n");
#endif
    }
}

static BOOL SHADOW_SyncUpdateAsync(_THIS)
{
    if (this->hidden->async_update) {
#if USE_UPDATE_SEM
        if (sem_wait(&this->hidden->update_sem))
            _ERR_PRINTF("Failed sem_wait(): %m\n");
#else
        if (pthread_mutex_lock(&this->hidden->update_lock))
            _ERR_PRINTF("Failed pthread_mutex_lock(): %m\n");
#endif
    }

    if (_shadowfbheader->dirty || _shadowfbheader->palette_changed) {
        GAL_VideoDevice *real_device;
        real_device = this->hidden->realfb_info->real_device;
        assert(real_device);

        if (_shadowfbheader->palette_changed) {
            real_device->SetColors(real_device, _shadowfbheader->firstcolor,
                    _shadowfbheader->ncolors,
                    (GAL_Color*)((char*)_shadowfbheader +
                        _shadowfbheader->palette_offset));
            SetRect(&_shadowfbheader->dirty_rect, 0, 0,
                    _shadowfbheader->width, _shadowfbheader->height);
        }

        GetBoundRect (&this->hidden->update_rect,
                &this->hidden->update_rect, &_shadowfbheader->dirty_rect);
        SetRect(&_shadowfbheader->dirty_rect, 0, 0, 0, 0);
        _shadowfbheader->dirty = FALSE;
        _shadowfbheader->palette_changed = FALSE;
    }

    if (this->hidden->async_update) {
#if USE_UPDATE_SEM
        if (sem_post(&this->hidden->update_sem))
            _ERR_PRINTF("Failed sem_post(): %m\n");
#else
        if (pthread_mutex_unlock(&this->hidden->update_lock))
            _ERR_PRINTF("Failed pthread_mutex_unlock(): %m\n");
#endif
    }

    return TRUE;
}

static void
update_helper(_THIS, GAL_VideoDevice *real_device, RECT *update_rect)
{
    BlitCopyOperation op = BLIT_COPY_TRANSLATE;
    RECT dirty_rect;

    if (this->hidden->realfb_info->flags & _ROT_DIR_CW) {
        op = BLIT_COPY_ROT_90;
        _get_dst_rect_cw(&dirty_rect, update_rect, this->hidden->realfb_info);
    }
    else if (this->hidden->realfb_info->flags & _ROT_DIR_CCW) {
        op = BLIT_COPY_ROT_270;
        _get_dst_rect_ccw(&dirty_rect, update_rect, this->hidden->realfb_info);
    }
    else if (this->hidden->realfb_info->flags & _ROT_DIR_HFLIP) {
        op = BLIT_COPY_FLIP_H;
        dirty_rect = *update_rect;
        _get_dst_rect_hflip(&dirty_rect, this->hidden->realfb_info);
    }
    else if (this->hidden->realfb_info->flags & _ROT_DIR_VFLIP) {
        op = BLIT_COPY_FLIP_V;
        dirty_rect = *update_rect;
        _get_dst_rect_vflip(&dirty_rect, this->hidden->realfb_info);
    }
    else {
        dirty_rect = *update_rect;
    }

    GAL_Rect dst_rc;
    dst_rc.x = dirty_rect.left;
    dst_rc.y = dirty_rect.top;
    dst_rc.w = RECTW(dirty_rect);
    dst_rc.h = RECTH(dirty_rect);

    BOOL hw_ok = FALSE;
    if ((dst_rc.w * dst_rc.h) >= this->hidden->min_pixels_using_hwaccl) {
        if (real_device->CopyHWSurface) {
            GAL_Rect src_rc = {
                update_rect->left, update_rect->top,
                RECTWP(update_rect), RECTHP(update_rect) };

            if (real_device->CopyHWSurface(real_device,
                        this->screen, &src_rc,
                        real_device->screen, &dst_rc, op) == 0) {
                hw_ok = TRUE;
            }
        }
    }

    if (!hw_ok) {
        shadow_fb_ops.refresh(_shadowfbheader,
                this->hidden->realfb_info, update_rect);
    }

    if (real_device->UpdateRects)
        real_device->UpdateRects(real_device, 1, &dst_rc);
    if (real_device->SyncUpdate)
        real_device->SyncUpdate(real_device);

    SetRectEmpty(update_rect);
}

static void *task_do_update(void *data)
{
    _THIS = data;

    if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))
        goto error;

    GAL_VideoDevice *real_device;
    real_device = this->hidden->realfb_info->real_device;

    BOOL vbl_ok = FALSE;
    if (real_device->WaitVBlank) {
        vbl_ok = real_device->WaitVBlank(real_device);
    }

    this->hidden->async_update = 1;
    sem_post(&this->hidden->sync_sem);

#ifdef _DEBUG
    clock_gettime(CLOCK_REALTIME, &this->hidden->ts_start);
#endif

    do {
        if (vbl_ok) {
            real_device->WaitVBlank(real_device);
        }
        else {
            usleep(this->hidden->update_interval * 1000);
        }

#ifdef _DEBUG
        this->hidden->frames++;
#endif

#if USE_UPDATE_SEM
        if (sem_wait(&this->hidden->update_sem))
            _ERR_PRINTF("Failed sem_wait(): %m\n");
#else
        if (pthread_mutex_lock(&this->hidden->update_lock))
            _ERR_PRINTF("Failed pthread_mutex_lock(): %m\n");
#endif

        if (RECTH(this->hidden->update_rect)) {
            update_helper(this, real_device, &this->hidden->update_rect);
        }

#if USE_UPDATE_SEM
        if (sem_post(&this->hidden->update_sem))
            _ERR_PRINTF("Failed sem_post(): %m\n");
#else
        if (pthread_mutex_unlock(&this->hidden->update_lock))
            _ERR_PRINTF("Failed pthread_mutex_unlock(): %m\n");
#endif

        pthread_testcancel();
    } while (1);

    return NULL;

error:
    this->hidden->async_update = 0;
    sem_post(&this->hidden->sync_sem);
    return NULL;
}

static int create_async_updater(_THIS)
{
    if (sem_init(&this->hidden->sync_sem, 0, 0)) {
        _ERR_PRINTF("Failed sem_init(): %m\n");
        return -1;
    }

#if USE_UPDATE_SEM
    if (sem_init(&this->hidden->update_sem, 0, 1)) {
        _ERR_PRINTF("Failed sem_init(): %m\n");
        return -1;
    }
#else
    if (pthread_mutex_init(&this->hidden->update_lock, NULL)) {
        _ERR_PRINTF("Failed pthread_mutex_init(): %m\n");
        return -1;
    }
#endif

    pthread_attr_t attr;
    pthread_attr_init(&attr);
#ifdef __LINUX__
    if (geteuid() == 0) {
        struct sched_param sp = { 99 };
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        pthread_attr_setschedparam(&attr, &sp);
    }
#endif

    if (pthread_create(&this->hidden->update_thd, &attr,
            task_do_update, this)) {
        _ERR_PRINTF("Failed pthread_create(): %m\n");
        return -1;
    }

    sem_wait(&this->hidden->sync_sem);
    pthread_attr_destroy(&attr);

    return this->hidden->async_update ? 0 : -1;
}

static void cancel_async_updater(_THIS)
{
    if (this->hidden->async_update && this->LockHWSurface) {
        pthread_cancel(this->hidden->update_thd);
        pthread_join(this->hidden->update_thd, NULL);
#ifdef _DEBUG
        double elapsed = get_elapsed_seconds(&this->hidden->ts_start, NULL);
        _DBG_PRINTF("Frames per second: %f\n", this->hidden->frames / elapsed);
#endif

#if USE_UPDATE_SEM
        sem_destroy(&this->hidden->update_sem);
#else
        pthread_mutex_destroy(&this->hidden->update_lock);
#endif
        sem_destroy(&this->hidden->sync_sem);
    }
}

#if 0 /* multiple updaters */
struct update_thd_args {
    _THIS;
    int number;
};

static void* task_do_update(void* data)
{
    struct update_thd_args args;
    args = *(struct update_thd_args *)data;

    if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))
        return NULL;

    sem_post(&args.this->hidden->sync_sem);

    do {
        sem_wait(&args.this->hidden->update_sems[args.number]);

        pthread_testcancel();

        RECT *dirty_rc = args.this->hidden->dirty_rcs + args.number;

        if (RECTHP(dirty_rc)) {
            shadow_fb_ops.refresh(_shadowfbheader,
                    args.this->hidden->realfb_info, dirty_rc);
        }

        /* notify main thread for done of update */
        sem_post(&args.this->hidden->sync_sem);
    } while (1);

    return NULL;
}

static void schedule_updaters(_THIS, RECT *dirty_rc)
{
    int w = RECTWP(dirty_rc);
    int h = RECTHP(dirty_rc);
    int i;

    if (h < this->hidden->nr_updaters || (w * h) < MIN_PIXELS_USING_UPDATER) {
        shadow_fb_ops.refresh(_shadowfbheader,
            this->hidden->realfb_info, dirty_rc);
        return;
    }

    /* partition the dirty rectangle horizontally */
    int span = h / (this->hidden->nr_updaters);
    if (span == 0) {
        memset(this->hidden->dirty_rcs, 0, sizeof(this->hidden->dirty_rcs));
        for (i = 0; i < h; i++) {
            this->hidden->dirty_rcs[i].left = dirty_rc->left;
            this->hidden->dirty_rcs[i].right = dirty_rc->right;
            if (i == 0)
                this->hidden->dirty_rcs[i].top = dirty_rc->top;
            else
                this->hidden->dirty_rcs[i].top =
                    this->hidden->dirty_rcs[i-1].bottom;
            this->hidden->dirty_rcs[i].bottom =
                this->hidden->dirty_rcs[i].top + 1;
        }
    }
    else {
        for (i = 0; i < this->hidden->nr_updaters; i++) {
            this->hidden->dirty_rcs[i].left = dirty_rc->left;
            this->hidden->dirty_rcs[i].right = dirty_rc->right;
            if (i == 0)
                this->hidden->dirty_rcs[i].top = dirty_rc->top;
            else
                this->hidden->dirty_rcs[i].top =
                    this->hidden->dirty_rcs[i-1].bottom;

            this->hidden->dirty_rcs[i].bottom =
                this->hidden->dirty_rcs[i].top + span;
        }
        this->hidden->dirty_rcs[i - 1].bottom = dirty_rc->bottom;
    }

    /* notify extra updaters */
    for (i = 0; i < this->hidden->nr_updaters; i++) {
        sem_post(&this->hidden->update_sems[i]);
    }

    /* update the last rectangle
    shadow_fb_ops.refresh(_shadowfbheader, this->hidden->realfb_info,
            this->hidden->dirty_rcs + this->hidden->nr_updaters);
    */

    /* wait for the finish of extra updaters */
    for (i = 0; i < this->hidden->nr_updaters; i++) {
        sem_wait(&this->hidden->sync_sem);
    }
}

static BOOL SHADOW_SyncUpdateConcurrently (_THIS);
static BOOL SHADOW_SyncUpdateConcurrently (_THIS)
{
    RECT dirty_rect;
    GAL_Rect update_rect;

    SetRect(&dirty_rect, 0, 0, _shadowfbheader->width, _shadowfbheader->height);

    if (_shadowfbheader->dirty || _shadowfbheader->palette_changed) {
        GAL_VideoDevice *real_device;
        real_device = this->hidden->realfb_info->real_device;
        assert(real_device);

        if (_shadowfbheader->palette_changed) {
            real_device->SetColors (real_device, _shadowfbheader->firstcolor,
                    _shadowfbheader->ncolors,
                    (GAL_Color*)((char*)_shadowfbheader + _shadowfbheader->palette_offset));
            SetRect (&_shadowfbheader->dirty_rect, 0, 0,
                    _shadowfbheader->width, _shadowfbheader->height);
        }

        schedule_updaters(this, &_shadowfbheader->dirty_rect);

        if (this->hidden->realfb_info->flags & _ROT_DIR_CW) {
            _get_dst_rect_cw (&dirty_rect, &(_shadowfbheader->dirty_rect),
                    this->hidden->realfb_info);
        }
        else if (this->hidden->realfb_info->flags & _ROT_DIR_CCW) {
            _get_dst_rect_ccw (&dirty_rect, &(_shadowfbheader->dirty_rect),
                    this->hidden->realfb_info);
        }
        else if (this->hidden->realfb_info->flags & _ROT_DIR_HFLIP) {
            dirty_rect = _shadowfbheader->dirty_rect;
            _get_dst_rect_hflip (&dirty_rect, this->hidden->realfb_info);
        }
        else if (this->hidden->realfb_info->flags & _ROT_DIR_VFLIP) {
            dirty_rect = _shadowfbheader->dirty_rect;
            _get_dst_rect_vflip (&dirty_rect, this->hidden->realfb_info);
        }
        else {
            dirty_rect = _shadowfbheader->dirty_rect;
        }

        update_rect.x = dirty_rect.left;
        update_rect.y = dirty_rect.top;
        update_rect.w = dirty_rect.right - dirty_rect.left;
        update_rect.h = dirty_rect.bottom - dirty_rect.top;

        if (real_device->UpdateRects)
            real_device->UpdateRects(real_device, 1, &update_rect);
        if (real_device->SyncUpdate)
            real_device->SyncUpdate(real_device);

        SetRect (&_shadowfbheader->dirty_rect, 0, 0, 0, 0);
        _shadowfbheader->dirty = FALSE;
        _shadowfbheader->palette_changed = FALSE;
    }

    return TRUE;
}

static int create_extra_updaters(_THIS)
{
    if (sem_init(&this->hidden->sync_sem, 0, 0))
        return -1;

    struct update_thd_args args = { this, 0 };

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    for (int i = 0; i < this->hidden->nr_updaters; i++) {
        if (sem_init(&this->hidden->update_sems[i], 0, 0))
            return -1;

        struct sched_param sp = { 90 };
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        pthread_attr_setschedparam(&attr, &sp);

        args.number = i;
        if (pthread_create(&this->hidden->update_thds[i], &attr,
                task_do_update, &args))
            return -1;

        sem_wait(&this->hidden->sync_sem);
    }
    pthread_attr_destroy(&attr);

    /*
    for (int i = 0; i < this->hidden->nr_updaters; i++) {
        sem_post(&this->hidden->sync_sem);
    }*/

    return 0;
}
#endif

static int SHADOW_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
        union semun sunion;
#endif
        if (RealEngine_Init() < 0) {
            return -1;
        }
        else {
            shadow_fb_ops.init();
        }

#ifdef _MGRM_PROCESSES
        this->hidden->semid = semget (SHADOW_SEM_KEY, 1, 0666 | IPC_CREAT | IPC_EXCL);
        if (this->hidden->semid < 0) {
            perror ("NEWGAL>SHADOW: semget");
            return -1;
        }

        sunion.val = 1;
        semctl (this->hidden->semid, 0, SETVAL, sunion);
    }
    else {
        this->hidden->semid = semget (SHADOW_SEM_KEY, 1, 0);
        if (this->hidden->semid < 0) {
            perror ("NEWGAL>SHADOW: semget");
            return -1;
        }
    }
#endif

    char tmp[8];
    if (GetMgEtcValue ("shadow", "async_update", tmp, sizeof(tmp)) < 0) {
            this->hidden->async_update = 0;
    }
    else if (strcasecmp(tmp, "true") == 0 ||
            strcasecmp(tmp, "yes") == 0) {
        this->hidden->async_update = 1;
    }
    else {
        this->hidden->async_update = 0;
    }

    if (this->hidden->async_update) {
        if (GetMgEtcIntValue("shadow", "update_interval",
                    &this->hidden->update_interval) < 0) {
            this->hidden->update_interval = 20;
        }
        else if (this->hidden->update_interval < 0 ||
                this->hidden->update_interval > 50) {
            this->hidden->update_interval = 20;
        }
    }

    if (GetMgEtcIntValue("shadow", "min_pixels_using_hwaccl",
                &this->hidden->min_pixels_using_hwaccl) < 0) {
        this->hidden->min_pixels_using_hwaccl = 4096;
    }
    else if (this->hidden->min_pixels_using_hwaccl < 0) {
        this->hidden->min_pixels_using_hwaccl = 4096;
    }

#if 0   /* code for multiple updaters */
    char nr_updaters[LEN_MODE+1] = "0";
    if (GetMgEtcValue ("shadow", "nr_updaters",
                nr_updaters, LEN_MODE) < 0) {
        // keep default
    }

    this->hidden->nr_updaters = atoi(nr_updaters);
    if (this->hidden->nr_updaters < 0)
        this->hidden->nr_updaters = 0;
    else if (this->hidden->nr_updaters > MAX_NR_UPDATERS)
        this->hidden->nr_updaters = MAX_NR_UPDATERS;

    if (this->hidden->nr_updaters > 0) {
        if (create_extra_updaters(this)) {
            perror ("NEWGAL>SHADOW: create_updaters");
        }
    }
#endif

    /* We're done! */
    return 0;
}

static BOOL SHADOW_SyncUpdate (_THIS)
{
    RECT dirty_rect;
    GAL_Rect update_rect;

    if (_shadowfbheader->dirty || _shadowfbheader->palette_changed) {
        GAL_VideoDevice *real_device;
        real_device = this->hidden->realfb_info->real_device;

        if (_shadowfbheader->palette_changed) {
            real_device->SetColors (real_device, _shadowfbheader->firstcolor,
                    _shadowfbheader->ncolors,
                    (GAL_Color *)((char *)_shadowfbheader +
                        _shadowfbheader->palette_offset));
            SetRect (&_shadowfbheader->dirty_rect, 0, 0,
                    _shadowfbheader->width, _shadowfbheader->height);
        }

        if (RECTH(_shadowfbheader->dirty_rect)) {
            update_helper(this, real_device, &(_shadowfbheader->dirty_rect));
        }

        _shadowfbheader->dirty = FALSE;
        _shadowfbheader->palette_changed = FALSE;
    }

    return TRUE;
}

static void change_mouseXY_cw(int* x, int* y)
{
    int tmp;
    if (*x > (__gal_screen->h-1)) *x = (__gal_screen->h-1);
    if (*y > (__gal_screen->w-1)) *y = (__gal_screen->w-1);
    tmp = *x;
    *x = *y;
    *y = (__gal_screen->h-1) - tmp;
}

static void change_mouseXY_ccw(int* x, int* y)
{
    int tmp;
    if (*x > (__gal_screen->h-1)) *x = (__gal_screen->h-1);
    if (*y > (__gal_screen->w-1)) *y = (__gal_screen->w-1);
    tmp = *y;
    *y = *x;
    *x = (__gal_screen->w-1) - tmp;
}

static void change_mouseXY_hflip(int* x, int* y)
{
    *x = __gal_screen->w - *x;
}

static void change_mouseXY_vflip(int* x, int* y)
{
    *y = __gal_screen->h - *y;
}

#ifndef _MGRM_PROCESSES
/*******************************************/
/*****************Thread   Mode*************/
/*******************************************/
static GAL_Surface *SHADOW_SetVideoMode(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags)
{
    int size;
    int ret;
    RealFBInfo *realfb_info;
    ShadowFBHeader shadowfbheader;
    GAL_VideoDevice *real_device;

    size = 0;
    ret = 0;

    bzero(&shadowfbheader, sizeof(shadowfbheader));
    realfb_info = calloc (1, sizeof(RealFBInfo));
    if (shadow_fb_ops.get_realfb_info (realfb_info))
    {
        _ERR_PRINTF ("NEWGAL>SHADOW: "
                "Couldn't get the real engine information\n");
        return NULL;
    }

    real_device = realfb_info->real_device;
    if (realfb_info->depth <= 8)
        shadowfbheader.depth = 8;
    else
        shadowfbheader.depth = realfb_info->depth;

    if (realfb_info->flags & _ROT_DIR_CW) {
        __mg_ial_change_mouse_xy_hook = change_mouseXY_cw;
    }
    else if (realfb_info->flags & _ROT_DIR_CCW) {
        __mg_ial_change_mouse_xy_hook = change_mouseXY_ccw;
    }
    else if (realfb_info->flags & _ROT_DIR_HFLIP) {
        __mg_ial_change_mouse_xy_hook = change_mouseXY_hflip;
    }
    else if (realfb_info->flags & _ROT_DIR_VFLIP) {
        __mg_ial_change_mouse_xy_hook = change_mouseXY_vflip;
    }

    if((realfb_info->flags & _ROT_DIR_CW) ||
            (realfb_info->flags & _ROT_DIR_CCW)) {
        shadowfbheader.width = realfb_info->height;
        shadowfbheader.height = realfb_info->width;
    }
    else {
        shadowfbheader.width = realfb_info->width;
        shadowfbheader.height = realfb_info->height;
    }

    shadowfbheader.pitch = ((shadowfbheader.width * shadowfbheader.depth) + 31) / 32*4;

    if (!(realfb_info->flags & FLAG_REALFB_PREALLOC)) {
        size = shadowfbheader.pitch * shadowfbheader.height;
    }

    if (shadowfbheader.depth <= 8)
        size += PALETTE_SIZE;

    size += sizeof (ShadowFBHeader);
    _shadowfbheader = (ShadowFBHeader *) malloc (size);
    memcpy(_shadowfbheader, &shadowfbheader, sizeof(ShadowFBHeader));

    /* INIT  Share Memory  ShadowFBHeader */
    this->hidden->realfb_info = realfb_info;
    if ((realfb_info->real_device))
        this->hidden->realfb_info->fb = real_device->screen->pixels;
    _shadowfbheader->info_size = size;

    _shadowfbheader->dirty = FALSE;
    SetRect (&(_shadowfbheader->dirty_rect), 0, 0, 0, 0);
    _shadowfbheader->palette_changed = FALSE;
    _shadowfbheader->palette_offset = sizeof(ShadowFBHeader);

    if(shadowfbheader.depth <= 8)
        _shadowfbheader->fb_offset = _shadowfbheader->palette_offset+PALETTE_SIZE;
    else
        _shadowfbheader->fb_offset = _shadowfbheader->palette_offset;

    if ((realfb_info->real_device))
        GAL_SetClipRect (real_device->screen, NULL);

    if (!GAL_ReallocFormat (current, _shadowfbheader->depth,
                real_device->screen->format->Rmask,
                real_device->screen->format->Gmask,
                real_device->screen->format->Bmask,
                real_device->screen->format->Amask)) {
        if (shadow_fb_ops.release)
            shadow_fb_ops.release (realfb_info);
        _ERR_PRINTF ("NEWGAL>SHADOW: "
                "Couldn't allocate new pixel format for requested mode");
        return (NULL);
    }

    /* Set up the new mode framebuffer */
    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w = _shadowfbheader->width;
    current->h = _shadowfbheader->height;
    current->pitch = _shadowfbheader->pitch;
    current->pixels = (char *)_shadowfbheader + _shadowfbheader->fb_offset;

    if (_shadowfbheader->depth == 32) {
        if (this->hidden->realfb_info->flags & _ROT_DIR_CCW)
            shadow_fb_ops.refresh = refresh_ccw_32bpp;
        else if (this->hidden->realfb_info->flags & _ROT_DIR_CW)
            shadow_fb_ops.refresh = refresh_cw_32bpp;
    }

    if (this->hidden->async_update) {
        create_async_updater(this);
        if (this->hidden->async_update) {
            current->flags |= GAL_ASYNCBLIT;
            this->LockHWSurface = SHADOW_LockHWSurface;
            this->UnlockHWSurface = SHADOW_UnlockHWSurface;
            this->SyncUpdate = SHADOW_SyncUpdateAsync;
        }
    }

    if (real_device->AllocHWSurface) {
        this->AllocHWSurface = SHADOW_AllocHWSurface;
    }
    if (real_device->CheckHWBlit) {
        this->CheckHWBlit = SHADOW_CheckHWBlit;
    }
    if (real_device->FillHWRect) {
        this->FillHWRect = SHADOW_FillHWRect;
    }
    if (real_device->FreeHWSurface) {
        this->FreeHWSurface = SHADOW_FreeHWSurface;
    }

    /* We're done */
    return (current);
}

static void SHADOW_VideoQuit (_THIS)
{
#if 0 /* Code for multiple updaters */
    if (this->hidden->nr_updaters > 0) {
        for (int i = 0; i < this->hidden->nr_updaters; i++) {
            pthread_cancel(this->hidden->update_thds[i]);
            sem_post(&this->hidden->update_sems[i]);
            pthread_join(this->hidden->update_thds[i], NULL);
            sem_destroy(&this->hidden->update_sems[i]);
        }

        sem_destroy(&this->hidden->sync_sem);
    }
#endif

    cancel_async_updater(this);

    if (_shadowfbheader) {
        _shadowfbheader->dirty = FALSE;
        free(_shadowfbheader);
        _shadowfbheader = NULL;
    }

    GAL_FreeSurface(this->screen);
}

#else

/*******************************************/
/*****************Proc   Mode***************/
/*******************************************/

static GAL_Surface *SHADOW_SetVideoMode(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags)
{
    int ret, size;

    RealFBInfo * realfb_info;
    GAL_VideoDevice* real_device = NULL;
    realfb_info = NULL;
    shmid = 0;
    size = 0;
    ret = 0;

    if (mgIsServer) {
        ShadowFBHeader shadowfbheader;

        realfb_info = malloc (sizeof(RealFBInfo));
        if (shadow_fb_ops.get_realfb_info (realfb_info)){
            _ERR_PRINTF ("NEWGAL>SHADOW: "
                    "Couldn't get the real engine information\n");
        }
        if (realfb_info->real_device)
            real_device = realfb_info->real_device;

        if (!(realfb_info->flags & FLAG_REALFB_PREALLOC)) {
            if (realfb_info->depth <= 8)
                shadowfbheader.depth = 8;
            else
                shadowfbheader.depth = realfb_info->depth;
        }
        else {
            _ERR_PRINTF ("NEWGAL>SHADOW: "
                    "FLAG_REALFB_PREALLOC is TRUE, Can't run in proc mode!");
            return NULL;
        }

        if (realfb_info->flags & _ROT_DIR_CW) {
            __mg_ial_change_mouse_xy_hook = change_mouseXY_cw;
        }
        else if (realfb_info->flags & _ROT_DIR_CCW) {
            __mg_ial_change_mouse_xy_hook = change_mouseXY_ccw;
        }
        else if (realfb_info->flags & _ROT_DIR_HFLIP) {
            __mg_ial_change_mouse_xy_hook = change_mouseXY_hflip;
        }
        else if (realfb_info->flags & _ROT_DIR_VFLIP) {
            __mg_ial_change_mouse_xy_hook = change_mouseXY_vflip;
        }

        if((realfb_info->flags & _ROT_DIR_CW) ||
                (realfb_info->flags & _ROT_DIR_CCW)) {
            shadowfbheader.width = realfb_info->height;
            shadowfbheader.height = realfb_info->width;
        }
        else {
            shadowfbheader.width = realfb_info->width;
            shadowfbheader.height = realfb_info->height;
        }

        shadowfbheader.pitch = ((shadowfbheader.width * shadowfbheader.depth) + 31) / 32*4;

        size = shadowfbheader.pitch * shadowfbheader.height;
        if (shadowfbheader.depth <= 8)
            size += PALETTE_SIZE;
        size += sizeof (ShadowFBHeader);

        shmid = shmget (SHADOW_SHM_KEY, size, IPC_CREAT | 0666 | IPC_EXCL);
        if (shmid == -1){
            perror("NEWGAL>SHADOW: shmget");
            shadow_fb_ops.release(realfb_info);
            return NULL;
        }

        _shadowfbheader = (ShadowFBHeader *) shmat (shmid, 0, 0);
        memcpy (_shadowfbheader, &shadowfbheader, sizeof (ShadowFBHeader));

        this->hidden->realfb_info = realfb_info;
        if ((realfb_info->real_device))
            this->hidden->realfb_info->fb = real_device->screen->pixels;

        /* INIT  Share Memory  ShadowFBHeader */
        _shadowfbheader->info_size = size;
        _shadowfbheader->dirty = FALSE;
        SetRect (&(_shadowfbheader->dirty_rect), 0, 0, 0, 0);
        _shadowfbheader->palette_changed = FALSE;
        _shadowfbheader->palette_offset = sizeof(ShadowFBHeader);

        if(shadowfbheader.depth <= 8)
            _shadowfbheader->fb_offset = _shadowfbheader->palette_offset + PALETTE_SIZE;
        else
            _shadowfbheader->fb_offset = _shadowfbheader->palette_offset;

        _shadowfbheader->Rmask = real_device->screen->format->Rmask;
        _shadowfbheader->Gmask = real_device->screen->format->Gmask;
        _shadowfbheader->Bmask = real_device->screen->format->Bmask;
        _shadowfbheader->Amask = real_device->screen->format->Amask;

        if ((realfb_info->real_device))
            GAL_SetClipRect (real_device->screen, NULL);
    }
    else {
        shmid = shmget(SHADOW_SHM_KEY, 0, 0);
        _shadowfbheader = (ShadowFBHeader *) shmat (shmid, 0, 0);
    }

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, _shadowfbheader->depth,
                _shadowfbheader->Rmask, _shadowfbheader->Gmask,
                _shadowfbheader->Bmask, _shadowfbheader->Amask)){
        if (shadow_fb_ops.release)
            shadow_fb_ops.release (realfb_info);

        _ERR_PRINTF ("NEWGAL>SHADOW: "
                "Couldn't allocate new pixel format for requested mode");
        return (NULL);
    }

    /* Set up the new mode framebuffer */

    current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w = _shadowfbheader->width;
    current->h = _shadowfbheader->height;
    current->pitch = _shadowfbheader->pitch;
    current->pixels = (char *)(_shadowfbheader) + _shadowfbheader->fb_offset;

    /* We're done */
    return (current);
}

static void SHADOW_VideoQuit (_THIS)
{
    ShadowFBHeader* tmp;
    union semun ignored;

    if (!mgIsServer) {
        _shadowfbheader->dirty = FALSE;
        if (this->screen->pixels) {
            shmdt(_shadowfbheader);
            this->screen->pixels = NULL;
        }
    }
    else {
        tmp = _shadowfbheader;
        _shadowfbheader = NULL;
        shmdt (tmp);
        shadow_fb_ops.release(this->hidden->realfb_info);
        this->hidden->realfb_info = NULL;

        if (shmctl (shmid, IPC_RMID, NULL))
            perror ("NEWGAL>SHADOW: shmctl");

        if (semctl (this->hidden->semid, 0, IPC_RMID, ignored) < 0)
            perror ("NEWGAL>SHADOW: smmctl");
    }
}

#endif

static GAL_Rect **SHADOW_ListModes (_THIS, GAL_PixelFormat *format,
        Uint32 flags)
{
    return (GAL_Rect **) -1;
}

static int SHADOW_SetColors (_THIS, int firstcolor, int ncolors,
        GAL_Color *colors)
{
    _shadowfbheader->firstcolor = firstcolor;
    _shadowfbheader->ncolors = ncolors;
#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
#endif
        if ((shadow_fb_ops.set_palette != NULL)
                && (_shadowfbheader->depth <= 8)){
            shadow_fb_ops.set_palette(this->hidden->realfb_info,
                    firstcolor, ncolors, colors);
            _shadowfbheader->palette_changed = TRUE;
        }

        return 0;
#ifdef _MGRM_PROCESSES
    }
    else {
#endif
        if (_shadowfbheader->depth <= 8){
            RealEngine_SetPalette(this->hidden->realfb_info, firstcolor,
                    ncolors, colors);
            _shadowfbheader->palette_changed = TRUE;
        }
#ifdef _MGRM_PROCESSES
    }
#endif

    return 0;
}

#endif /* _MGGAL_SHADOW */

#if 0 /* Code for multiple updaters */
    if (this->hidden->nr_updaters > 0) {
        this->SyncUpdate = SHADOW_SyncUpdateConcurrently;
    }
#endif

#if 0 /* code for testing pixman */

static void refresh_by_using_pixman(ShadowFBHeader *shadowfb_header,
        RealFBInfo *realfb_info, void *update)
{
    RECT src_rc = *(RECT *)update;
    RECT dst_rc;
    int dst_x, dst_y;

    if (realfb_info->flags & _ROT_DIR_CW) {
        _get_dst_rect_cw(&dst_rc, update, realfb_info);
        dst_x = -realfb_info->real_device->screen->h;
        dst_y = 0;
    }
    else if (realfb_info->flags & _ROT_DIR_CCW) {
        _get_dst_rect_ccw(&dst_rc, update, realfb_info);
        dst_x = 0; // -realfb_info->real_device->screen->w / 2;
        dst_y = realfb_info->real_device->screen->h / 4;
    }

    int w = RECTW(dst_rc);
    int h = RECTH(dst_rc);

    pixman_region32_t clip_region;
    pixman_region32_init_rect(&clip_region, dst_rc.left, dst_rc.top, w, h);
    pixman_image_set_clip_region32(realfb_info->dst_img, &clip_region);

    pixman_image_set_filter(realfb_info->src_img, PIXMAN_FILTER_FAST, NULL, 0);
    pixman_image_set_transform(realfb_info->src_img, &realfb_info->transform);
    pixman_image_composite32(PIXMAN_OP_SRC, realfb_info->src_img, NULL,
            realfb_info->dst_img,
            0, 0, 0, 0,
            dst_x, dst_y,
            realfb_info->real_device->screen->h,
            realfb_info->real_device->screen->h);
    pixman_region32_fini(&clip_region);

    pixman_image_set_transform(realfb_info->src_img, NULL);
    pixman_image_set_clip_region32(realfb_info->dst_img, NULL);
}

    GAL_CreatePixmanImage(current);
    GAL_CreatePixmanImage(real_device->screen);
    realfb_info->src_img = current->pix_img;
    realfb_info->dst_img = real_device->screen->pix_img;
    if (realfb_info->src_img && realfb_info->dst_img &&
            (this->hidden->realfb_info->flags & _ROT_DIR_CW ||
             this->hidden->realfb_info->flags & _ROT_DIR_CCW)) {
        double fscale_x, fscale_y;
        double rotation = 0.0;
        pixman_f_transform_t ftransform;

        if (this->hidden->realfb_info->flags == 0) {
            fscale_x = current->w * 1.0 / real_device->screen->w;
            fscale_y = current->h * 1.0 / real_device->screen->h;
        }
        else {
            fscale_x = current->w * 1.0 / real_device->screen->h;
            fscale_y = current->h * 1.0 / real_device->screen->w;
        }

        if (this->hidden->realfb_info->flags & _ROT_DIR_CW) {
            rotation = - M_PI / 2.0;
        }
        else if (this->hidden->realfb_info->flags & _ROT_DIR_CCW) {
            rotation = M_PI / 2.0;
        }

        pixman_f_transform_init_identity(&ftransform);
        if (rotation != 0.0) {
            double cx = current->w / 2.0;
            double cy = current->h / 2.0;
            pixman_f_transform_translate(&ftransform, NULL, -cx, -cy);
            pixman_f_transform_rotate(&ftransform, NULL,
                    cos(rotation), sin(rotation));
            pixman_f_transform_translate(&ftransform, NULL, cy, cx);
        }

        pixman_f_transform_scale(&ftransform, NULL, fscale_x, fscale_y);
        pixman_transform_from_pixman_f_transform(&realfb_info->transform,
                &ftransform);
        shadow_fb_ops.refresh = refresh_by_using_pixman;
    }
    else {
        if (realfb_info->src_img) {
            pixman_image_unref(realfb_info->src_img);
            realfb_info->src_img = NULL;
        }

        if (realfb_info->dst_img) {
            pixman_image_unref(realfb_info->dst_img);
            realfb_info->dst_img = NULL;
        }
    }

        if (this->hidden->realfb_info->src_img)
            pixman_image_unref(this->hidden->realfb_info->src_img);
        if (this->hidden->realfb_info->dst_img)
            pixman_image_unref(this->hidden->realfb_info->dst_img);

    pixman_image_t *src_img;
    pixman_image_t *dst_img;
    pixman_transform_t transform;
#endif
