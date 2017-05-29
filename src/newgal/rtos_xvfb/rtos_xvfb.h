/* $Id$
**
** rtos_xvfb.h: RTOS X virtual FrameBuffer head file.
** 
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming
*/

#ifndef _GAL_rtos_xvfb_h
#define _GAL_rtos_xvfb_h

#include <sys/types.h>

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

typedef struct _XVFBHeader {
    unsigned int info_size;
    int width;
    int height;
    int depth;

    /* The flag indicating the Most Significant Bits (MSB) 
     * is left when depth is less than 8. */
    Uint8  MSBLeft;

    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;

    int pitch;
    int dirty;            
    int dirty_rc_l, dirty_rc_t, dirty_rc_r, dirty_rc_b;
    int palette_changed;  
    int palette_offset;
    int fb_offset;
} XVFBHeader;

typedef struct _XVFBPalEntry {
    unsigned char r, g, b, a;
} XVFBPalEntry;

extern XVFBHeader* __mg_rtos_xvfb_header;

/* Private display data */
struct GAL_PrivateVideoData {
    unsigned char* shmrgn;
    XVFBHeader* hdr;
};

#endif /* _GAL_qvfb_h */

