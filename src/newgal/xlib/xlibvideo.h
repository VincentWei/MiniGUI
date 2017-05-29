/* $Id: pcxvfb.h 7946 2007-10-24 08:41:35Z wangjian $
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming
*/

#ifndef _GAL_pcxvfb_h
#define _GAL_pcxvfb_h

#include <sys/types.h>

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

#define X_VFB_MOUSE_PIPE	"/tmp/.xxvfb_mouse-%d"
#define X_VFB_KEYBOARD_PIPE	"/tmp/.xxvfb_keyboard-%d"

typedef struct _XXVFbHeader
{
	unsigned int info_size;

    int width;
    int height;
    int depth;
    int dev_depth;
    int pitch;

    int dirty;
	int dirty_rc_l;
	int dirty_rc_t;
	int dirty_rc_r;
	int dirty_rc_b;

	int palette_changed;
	int palette_offset;

	int fb_offset;

    int MSBLeft;
    
    int Rmask;
    int Gmask;
    int Bmask;
    int Amask;
}XXVFBHeader;

typedef struct _XXVFBPalEntry {
        unsigned char r, g, b, a;
} XXVFBPalEntry;

/* Private display data */
struct GAL_PrivateVideoData {
    unsigned char* shmrgn;
    XXVFBHeader* hdr;
};

#endif /* _GAL_pcxvfb_h */

