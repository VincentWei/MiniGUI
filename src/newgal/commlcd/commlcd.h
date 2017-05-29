/*
**  $Id: commlcd.h 7419 2007-08-21 03:29:23Z weiym $
**  
**  Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#ifndef _GAL_COMMLCD_H
#define _GAL_COMMLCD_H

#include "sysvideo.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

/* Private display data */

struct GAL_PrivateVideoData {
    int w, h, pitch;
    void *fb;
};

/* The pixel format defined by depth */
#define COMMLCD_PSEUDO_RGB332    1
#define COMMLCD_TRUE_RGB555      2

#define COMMLCD_TRUE_RGB565      3
#define COMMLCD_TRUE_RGB888      4
#define COMMLCD_TRUE_RGB0888     5

struct commlcd_info {
    short height, width;  // Size of the screen
    short bpp;            // Depth (bits-per-pixel)
    short type;           // Pixel type
    short rlen;           // Length of one scan line in bytes
    void  *fb;            // Frame buffer
};

struct commlcd_ops {
    /* return value: zero for OK */
    int (*init) (void);
    /* return value: zero for OK */
    int (*getinfo) (struct commlcd_info *li);
    /* return value: zero for OK */
    int (*release) (void);
    /* return value: number set, zero on error */
    int (*setclut) (int firstcolor, int ncolors, GAL_Color *colors);
};

extern struct commlcd_ops __mg_commlcd_ops;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _GAL_COMMLCD_H */
