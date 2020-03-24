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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#ifndef _GAL_sysvideo_h
#define _GAL_sysvideo_h

/* This file prototypes the video driver implementation.
   This is designed to be easily converted to C++ in the future.
 */

/* The GAL video driver */
struct REQ_HWSURFACE;
struct REP_HWSURFACE;

#define DEV_NAME_LEN                23
typedef struct _VIDEO_MEM_INFO
{
    int type;
    char video_dev_name [DEV_NAME_LEN+1];
    int video_mem_len;
    int video_mem_offset;
    int video_pitch;
} VIDEO_MEM_INFO;


/* Define the GAL video driver structure */
#define _THIS    GAL_VideoDevice *_this
#ifndef _STATUS
#define _STATUS    GAL_status *status
#endif

struct GAL_VideoDevice {
    /* * * */
    /* The name of this video driver */
    const char *name;

    /* * * */
    /* Initialization/Query functions */

    /* Initialize the native video subsystem, filling 'vformat' with the
       "best" display pixel format, returning 0 or -1 if there's an error.
     */
    int (*VideoInit)(_THIS, GAL_PixelFormat *vformat);

    /* List the available video modes for the given pixel format, sorted
       from largest to smallest.
     */
    GAL_Rect **(*ListModes)(_THIS, GAL_PixelFormat *format, Uint32 flags);

    /* Set the requested video mode, returning a surface which will be
       set to the GAL_VideoSurface.  The width and height will already
       be verified by ListModes(), and the video subsystem is free to
       set the mode to a supported bit depth different from the one
       specified -- the desired bpp will be emulated with a shadow
       surface if necessary.  If a new mode is returned, this function
       should take care of cleaning up the current mode.
     */
    GAL_Surface *(*SetVideoMode)(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags);

#if 0
    /* Toggle the fullscreen mode */
    int (*ToggleFullScreen)(_THIS, int on);

    /* This is called after the video mode has been set, to get the
       initial mouse state.  It should queue events as necessary to
       properly represent the current mouse focus and position.
     */
    void (*UpdateMouse)(_THIS);
#endif

    /* Set the color entries { firstcolor .. (firstcolor+ncolors-1) }
       of the physical palette to those in 'colors'. If the device is
       using a software palette (GAL_HWPALETTE not set), then the
       changes are reflected in the logical palette of the screen
       as well.
       The return value is 1 if all entries could be set properly
       or 0 otherwise.
     */
    int (*SetColors)(_THIS, int firstcolor, int ncolors,
             GAL_Color *colors);

    /* This pointer should exist in the native video subsystem and should
       point to an appropriate update function for the current video mode
     */
    void (*UpdateRects)(_THIS, int numrects, GAL_Rect *rects);

    /* Synchronize the dirty content */
    BOOL (*SyncUpdate)(_THIS);

    /* Reverse the effects VideoInit() -- called if VideoInit() fails
       or if the application is shutting down the video subsystem.
    */
    void (*VideoQuit)(_THIS);

    /* * * */
    /* Hardware acceleration functions */

    /* Information about the video hardware */
    GAL_VideoInfo info;

#ifdef _MGRM_PROCESSES
    /* Copy video information to shared resource segment */
    void (*CopyVideoInfoToSharedRes)(_THIS);
    /* Request a surface in video memory */
    void (*RequestHWSurface)(_THIS, const REQ_HWSURFACE* request,
            REP_HWSURFACE* reply);
#endif

    /* Allocates a surface in video memory */
    int (*AllocHWSurface)(_THIS, GAL_Surface *surface);

#if IS_COMPOSITING_SCHEMA
    /* Allocate a shared surface in hardware video memory.
       Set to NULL if no hardware shared surface supported.
       Return the PRIME file descriptor if success, otherwize -1. */
    int (*AllocSharedHWSurface)(_THIS, GAL_Surface *surface,
            size_t* map_size, off_t* pixels_off, Uint32 rw_modes);

    /* Free a shared surface in hardware video memory.
       Set to NULL if no hardware shared surface supported.
       Return 0 if success, otherwize -1. */
    int (*FreeSharedHWSurface)(_THIS, GAL_Surface *surface);

    /* Attach to a shared surface in hardware video memory.
       Set to NULL if no hardware shared surface supported.
       Return 0 if success, otherwize -1. */
    int (*AttachSharedHWSurface)(_THIS, GAL_Surface *surface,
            int prime_fd, size_t mapsize, BOOL with_rw);

    /* Dettach from a shared surface in hardware video memory.
       Set to NULL if no hardware shared surface supported.
       Return 0 if success, otherwise -1. */
    int (*DettachSharedHWSurface)(_THIS, GAL_Surface *surface);

    /* Allocate a dumb surface from hardware.
       Set to NULL if dumb surface is not supported.
       Return 0 if success, otherwise -1. */
    int (*AllocDumbSurface)(_THIS, GAL_Surface *surface);

    /* Free a dumb surface allocated from hardware.
       Set to NULL if dumb surface is not supported.
       Return 0 if success, otherwise -1. */
    void (*FreeDumbSurface)(_THIS, GAL_Surface *surface);

    /* Set hardware cursor.
       Set to NULL or return -1 if no hardware cursor support. */
    int (*SetCursor)(_THIS, GAL_Surface *surface, int hot_x, int hot_y);

    /* Move hardware cursor to new position.
       Set to NULL or return -1 if no hardware cursor support. */
    int (*MoveCursor)(_THIS, int x, int y);
#endif /* IS_COMPOSITING_SCHEMA */

    /* Set the hardware accelerated blit function, if any, based
       on the current flags of the surface (colorkey, alpha, etc.)
     */
    int (*CheckHWBlit)(_THIS, GAL_Surface *src, GAL_Surface *dst);

    /* Fills a surface rectangle with the given color */
    int (*FillHWRect)(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color);

    /* Set video mem colorkey and accelerated blit function */
    int (*SetHWColorKey)(_THIS, GAL_Surface *surface, Uint32 key);

    /* Set per surface hardware alpha value */
    int (*SetHWAlpha)(_THIS, GAL_Surface *surface, Uint8 value);

#if 0
    /* Returns a readable/writable surface */
    int (*LockHWSurface)(_THIS, GAL_Surface *surface);
    void (*UnlockHWSurface)(_THIS, GAL_Surface *surface);

    /* Performs hardware flipping */
    int (*FlipHWSurface)(_THIS, GAL_Surface *surface);
#endif

    /* Frees a previously allocated video surface */
    void (*FreeHWSurface)(_THIS, GAL_Surface *surface);

    Uint16 *gamma;

    /* * * */
    /* Data common to all drivers */
    GAL_Surface *screen;
    GAL_Palette *physpal;    /* physical palette, if != logical palette */
    char *wm_title;
    char *wm_icon;
    int offset_x;
    int offset_y;

    /* Driver information flags */
    int handles_any_size;    /* Driver handles any size video mode */

    /* * * */
    /* Data private to this driver */
    struct GAL_PrivateVideoData *hidden;

    /* * * */
    /* The function used to dispose of this structure */
    void (*free)(_THIS);

    /* DeleteSurface - called if this->info->mlt_surfaces is true
     * to delete a specific surface. * */
    void (*DeleteSurface) (_THIS, GAL_Surface* surface);

    /* Set surface Palette */
    int (*SetSurfaceColors) (GAL_Surface* surface, int firstcolor, int ncolors,
             GAL_Color *colors);

    /* Get Video Memory info */
    int (*GetFBInfo) (VIDEO_MEM_INFO *video_mem_info);

    /* Update Rects */
    void (*UpdateSurfaceRects) (_THIS, GAL_Surface* surface, int numrects, GAL_Rect *rects);

    /* Since 4.0.0; used for VT switching */
    int (*Suspend) (_THIS);
    int (*Resume) (_THIS);
};

#undef _THIS

typedef struct VideoBootStrap {
    const char *name;
    const char *desc;
    int (*available)(void);
    GAL_VideoDevice *(*create)(int devindex);
} VideoBootStrap;

#ifdef _MGGAL_DUMMY
extern VideoBootStrap DUMMY_bootstrap;
#endif
#ifdef _MGGAL_FBCON
extern VideoBootStrap FBCON_bootstrap;
#endif
#ifdef _MGGAL_QVFB
extern VideoBootStrap QVFB_bootstrap;
#endif
#ifdef _MGGAL_PCXVFB
extern VideoBootStrap PCXVFB_bootstrap;
#endif
#ifdef _MGGAL_COMMLCD
extern VideoBootStrap COMMLCD_bootstrap;
#endif
#ifdef _MGGAL_SHADOW
extern VideoBootStrap SHADOW_bootstrap;
#endif
#ifdef _MGGAL_MLSHADOW
extern VideoBootStrap MLSHADOW_bootstrap;
#endif
#ifdef _MGGAL_EM85XXYUV
extern VideoBootStrap EM85XXYUV_bootstrap;
#endif
#ifdef _MGGAL_EM85XXOSD
extern VideoBootStrap EM85XXOSD_bootstrap;
#endif
#ifdef _MGGAL_X11
extern VideoBootStrap X11_bootstrap;
#endif
#ifdef _MGGAL_DGA
extern VideoBootStrap DGA_bootstrap;
#endif
#ifdef _MGGAL_GGI
extern VideoBootStrap GGI_bootstrap;
#endif
#ifdef _MGGAL_VGL
extern VideoBootStrap VGL_bootstrap;
#endif
#ifdef _MGGAL_SVGALIB
extern VideoBootStrap SVGALIB_bootstrap;
#endif
#ifdef _MGGAL_AALIB
extern VideoBootStrap AALIB_bootstrap;
#endif
#ifdef _MGGAL_SVPXXOSD
extern VideoBootStrap SVPXXOSD_bootstrap;
#endif
#ifdef _MGGAL_BF533
extern VideoBootStrap BF533_bootstrap;
#endif
#ifdef _MGGAL_MB93493
extern VideoBootStrap MB93493_bootstrap;
#endif
#ifdef _MGGAL_WVFB
extern VideoBootStrap WVFB_bootstrap;
#endif
#ifdef _MGGAL_UTPMC
extern VideoBootStrap UTPMC_bootstrap;
#endif
#ifdef _MGGAL_DFB
extern VideoBootStrap DFB_bootstrap;
#endif
#ifdef _MGGAL_EM86GFX
extern VideoBootStrap EM86GFX_bootstrap;
#endif
#ifdef _MGGAL_HI3510
extern VideoBootStrap HI3510_bootstrap;
#endif
#ifdef _MGGAL_HI3560
extern VideoBootStrap HI3560_bootstrap;
#endif
#ifdef _MGGAL_HI3560A
extern VideoBootStrap HI3560A_bootstrap;
#endif
#ifdef _MGGAL_NEXUS
extern VideoBootStrap NEXUS_bootstrap;
#endif
#ifdef _MGGAL_S3C6410
extern VideoBootStrap S3C6410_bootstrap;
#endif
#ifdef _MGGAL_SIGMA8654
extern VideoBootStrap SIGMA8654GFX_bootstrap;
#endif
#ifdef _MGGAL_MSTAR
extern VideoBootStrap MSTAR_bootstrap;
#endif
#ifdef _MGGAL_CUSTOMGAL
extern VideoBootStrap CUSTOMGAL_bootstrap;
#endif
#ifdef _MGGAL_RTOSXVFB
extern VideoBootStrap RTOS_XVFB_bootstrap;
#endif
#ifdef _MGGAL_GDL
extern VideoBootStrap GDL_bootstrap;
#endif
#ifdef _MGGAL_STGFB
extern VideoBootStrap STGFB_bootstrap;
#endif
#ifdef _MGGAL_USVFB
extern VideoBootStrap USVFB_bootstrap;
#endif
#ifdef _MGGAL_DRM
extern VideoBootStrap DRM_bootstrap;
#endif

/* This is the current video device */
extern GAL_VideoDevice *__mg_current_video;

#define GAL_VideoSurface        (__mg_current_video->screen)
#define GAL_PublicSurface       (__mg_current_video->screen)

void Slave_FreeSurface (GAL_Surface *surface);

#endif /* _GAL_sysvideo_h */

