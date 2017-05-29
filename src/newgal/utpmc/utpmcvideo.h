/*
** $Id: utpmcvideo.h 7357 2007-08-16 05:04:38Z xgwang $
**  
** This is NEWGAL engine for UTStarcom PocketMedia based-on 
** ARM7TDMI/uClinux/uClibc.
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

typedef struct _RGB_YCrCb_Pair
{
    Uint16 rgb_pixel;
    Uint8 cb, cr, yy;
    Uint32 used_count;
    struct _RGB_YCrCb_Pair* next;
} RGB_YCRCB_PAIR;

#define LAST_USED_PAIRS     8
#define FREE_PAIRS          1024

typedef struct _RGB_YCrCb_MAP
{
    int nr_last_used;
    RGB_YCRCB_PAIR* last_used [LAST_USED_PAIRS];
    RGB_YCRCB_PAIR* cached [256];
    RGB_YCRCB_PAIR* free_pairs;
} RGB_YCRCB_MAP;

struct GAL_PrivateVideoData {
    int fd;
    Uint8 *fb;
    Uint32 fb_size;
    Uint16 fb_pitch;

    Uint16 w, h;

    Uint8 *shadow;
    Uint16 sh_pitch;

    BOOL dirty;
    RECT update;

    pthread_t th;
    pthread_mutex_t lock;

    void* rgb2ycrcb_map;
};

#endif /* _GAL_UTPMVIDEO_H */
