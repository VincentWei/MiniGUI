/* $Id: qvfb.h 11824 2009-07-10 09:51:54Z wanzheng $
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming
*/

#ifndef _GAL_qvfb_h
#define _GAL_qvfb_h

#include <sys/types.h>

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

/* #define QT_VERSION 4 */
#define QT_VFB_MOUSE_PIPE	"/tmp/.qtvfb_mouse-%d"
#define QT_VFB_KEYBOARD_PIPE	"/tmp/.qtvfb_keyboard-%d"

struct QVFbHeader
{
    int width;
    int height;
    int depth;
    int linestep;
    int dataoffset;
    RECT update;
    BYTE dirty;
    int  numcols;
    unsigned int clut[256];
};

struct QVFbKeyData
{
/* #if QT_VERSION>=4 */
#if 0
    unsigned int keycode;
    unsigned int modifiers;
    unsigned short int unicode;
    BOOL press;
    BOOL repeat;
#else
    unsigned int unicode;
    unsigned int modifiers;
    BOOL press;
    BOOL repeat;
#endif
};

/* Private display data */
struct GAL_PrivateVideoData {
    unsigned char* shmrgn;
    struct QVFbHeader* hdr;
};

#endif /* _GAL_qvfb_h */

