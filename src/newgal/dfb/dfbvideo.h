/*
**  $Id: dfbvideo.h 13674 2010-12-06 06:45:01Z wanzheng $
**  
**  Copyright (C) 2003 ~ 2010 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming
**
**  All rights reserved by Feynman Software.
**
**  This is the NEWGAL common DirectFB engine, base on DirectFB 1.0.
**
*/

#ifndef _GAL_dfbvideo_h
#define _GAL_dfbvideo_h

#include "sysvideo.h"

#define DFB_DEBUG 1
#define RETCHECK(con, output, action)                               \
{                                                                   \
    do {                                                            \
        if (con) {                                                  \
            if (DFB_DEBUG)                                          \
                fprintf(stderr, "NEWGAL>DFB:%s<%d>: %s\n",          \
                        __FILE__, __LINE__, output);                \
            action;                                                 \
        }                                                           \
    } while(0);                                                     \
}                                                                   \

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

/* for GAL_SURFACE */
struct private_hwdata { 
    IDirectFBSurface* dfb_surface;

    /* platform data */
    void* platform_data;
};

/* Private display data */
struct GAL_PrivateVideoData {
    int w, h;
    void *framebuffer;

    int fd_spi;
    Uint8 *buffer;
	Uint8 *shadow;

    int fd_lock;

    BOOL dirty;
    RECT update;
	pthread_t th;
	pthread_mutex_t lock;

    /* platform data */
    void* platform_data;
};

/* DirectFB common engine function address */
typedef struct _GAL_FunctionTable {
    int (*VideoInit)(_THIS, GAL_PixelFormat *vformat);
    GAL_Rect **(*ListModes)(_THIS, GAL_PixelFormat *format, Uint32 flags);
    GAL_Surface *(*SetVideoMode)(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
    int (*ToggleFullScreen)(_THIS, int on);
    int (*SetColors)(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
    void (*UpdateRects)(_THIS, int numrects, GAL_Rect *rects);
    void (*VideoQuit)(_THIS);
#ifdef _MGRM_PROCESSES
    void (*RequestHWSurface)(_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply);
#endif
    int (*AllocHWSurface)(_THIS, GAL_Surface *surface);
    int (*CheckHWBlit)(_THIS, GAL_Surface *src, GAL_Surface *dst);
    int (*HWAccelBlit)(GAL_Surface *src, GAL_Rect *srcrect, GAL_Surface *dst, GAL_Rect *dstrect);
    int (*FillHWRect)(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color);
    int (*SetHWColorKey)(_THIS, GAL_Surface *surface, Uint32 key);
    int (*SetHWAlpha)(_THIS, GAL_Surface *surface, Uint8 value);
    void (*FreeHWSurface)(_THIS, GAL_Surface *surface);
    void (*free)(_THIS);
    void (*DeleteSurface) (_THIS, GAL_Surface* surface);
    int (*SetSurfaceColors) (GAL_Surface* surface, int firstcolor, int ncolors, GAL_Color *colors);
    int (*GetFBInfo) (VIDEO_MEM_INFO *video_mem_info);
    void (*UpdateSurfaceRects) (_THIS, GAL_Surface* surface, int numrects, GAL_Rect *rects);
} GAL_FunctionTable;

extern GAL_FunctionTable mgGALFuncTable;

#endif /* _GAL_dfbvideo_h */

