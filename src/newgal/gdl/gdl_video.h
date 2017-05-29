/* $Id: gdl_video.h 11871 2010-10-20 14:36:53 dongkai $
**
** Copyright (C) 2003 ~ 2010 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming
*/

#ifndef _GAL_GDL_h
#define _GAL_GDL_h

#include <sys/types.h>

#include "sysvideo.h"
#include "libgdl.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

/* Private display data */
struct GAL_PrivateVideoData
{
    gdl_display_id_t display;
    gdl_plane_id_t plane_id;
    gdl_surface_id_t screen_surf;

    int width;
    int height;
    int depth;
    int linestep;
};

struct private_hwdata { /* for GAL_SURFACE */
    gdl_surface_info_t info;
};
#endif /* _GAL_GDL_h */

