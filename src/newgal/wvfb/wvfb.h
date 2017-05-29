/* $Id: wvfb.h 6341 2006-03-22 08:27:57Z weiym $
**
** wvfb.h
*/

#ifndef _GAL_wvfb_h
#define _GAL_wvfb_h


#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

struct WVFbHeader
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

struct WVFbKeyData
{
    unsigned int unicode;
    unsigned int modifiers;
    BOOL press;
    BOOL repeat;
};

/* Private display data */
struct GAL_PrivateVideoData {
    unsigned char* shmrgn;
    struct WVFbHeader* hdr;
};

#endif /* _GAL_wvfb_h */

