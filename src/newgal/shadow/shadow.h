/*
**  $Id: shadow.h 11437 2009-03-28 10:11:35Z weiym $
**  
**  Copyright (C) 2005 ~ 2007 Feynman Software.
*/

#ifndef _GAL_SHADOW_H
#define _GAL_SHADOW_H

//#include "sysvideo.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

typedef struct _ShadowFBHeader {
        unsigned int info_size;
        int width;
        int height;
        int depth;
        int pitch;
        int dirty;            /* true for dirty, and should reset to false after refreshing the dirty area */
        RECT dirty_rect;
        int palette_changed;  /* true for changed, and should reset to false after reflecting the change */
        int palette_offset;
        int fb_offset;
        Uint32 Rmask;
        Uint32 Gmask;
        Uint32 Bmask;
        Uint32 Amask;
        int firstcolor;
        int ncolors;
} ShadowFBHeader;

#define FLAG_REALFB_PREALLOC  0x01


typedef struct _RealFBInfo {
        DWORD flags;
        int height, width;
        int depth;
        int pitch;
        void* fb;
        void * real_device;
} RealFBInfo;

/* Private display data */

struct GAL_PrivateVideoData {
    RealFBInfo * realfb_info;
    /*GAL_VideoDevice *_real_device ;
    int w, h, pitch;
    void *fb;
    BOOL alloc_fb;
    BOOL dirty;
    RECT update;*/
    pthread_t update_th;
#ifdef _MGRM_PROCESSES
    int semid;
#else
    pthread_mutex_t update_lock;
#endif
};

typedef struct _ShadowFBOps {
        int (*init) (void);
        int (*get_realfb_info) (RealFBInfo* realfb_info);
        int (*release) (RealFBInfo* realfb_info);
        int (*set_palette) (RealFBInfo* realfb_info, int firstcolor, int ncolors, void* colors);
        void (*sleep) (void);
        void (*refresh) (ShadowFBHeader* shadowfb_header, RealFBInfo* realfb_info, void* update);
} ShadowFBOps;

ShadowFBOps * __mg_shadow_fb_ops;
#ifdef __cplusplus  
}
#endif  /* __cplusplus */

#endif /* _GAL_SHADOW_H */
