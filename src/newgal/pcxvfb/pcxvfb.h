/* $Id: pcxvfb.h 13674 2010-12-06 06:45:01Z wanzheng $
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming
*/

#ifndef _GAL_pcxvfb_h
#define _GAL_pcxvfb_h

#include <sys/types.h>

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

#define QT_VFB_MOUSE_PIPE	"/tmp/.qtvfb_mouse-%d"
#define QT_VFB_KEYBOARD_PIPE	"/tmp/.qtvfb_keyboard-%d"

int __mg_pcxvfb_server_sockfd;
int __mg_pcxvfb_client_sockfd;

typedef struct _XVFbHeader
{
	unsigned int info_size;

    int width;
    int height;
    int depth;
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
}XVFBHeader;

typedef struct _XVFBPalEntry {
        unsigned char r, g, b, a;
} XVFBPalEntry;

/* Private display data */
struct GAL_PrivateVideoData {
    unsigned char* shmrgn;
    XVFBHeader* hdr;
};

#endif /* _GAL_pcxvfb_h */

