/*
** $Id: utpmcvideo-lite.h 7357 2007-08-16 05:04:38Z xgwang $
**  
** This is NEWGAL engine for UTStarcom PocketMedia Center based-on 
** ARM7TDMI/uClinux/uClibc for MiniGUI-Lite.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#ifndef _GAL_UTPMVIDEO_H
#define _GAL_UTPMVIDEO_H

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

/* Private display data */

typedef struct __attribute__ ((packed)) _YCrCb
{
    Uint8 yy, cb, cr;
} YCRCB;

/* private data for the client */
typedef struct UTPMC_ShadowInfo {
    char signature [8];

    YCRCB* rgb2ycrcb_map;

    Uint16 w, h;
    Uint16 depth;
    Uint16 pitch;

    BOOL dirty;
    RECT update;

    Uint8 buff [0];
} UTPMC_SHADOWINFO;

#define _MY_STACK_SIZE      2048

/* private data */
struct GAL_PrivateVideoData {
    int fd;
    Uint8 *fb;
    Uint32 fb_size;
    Uint16 fb_pitch;

    Uint8* stack;

    int fd_lock;

    UTPMC_SHADOWINFO* shadow_info;
};

#endif /* _GAL_UTPMVIDEO_H */

