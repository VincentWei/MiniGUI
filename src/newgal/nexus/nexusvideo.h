
/*
**  $Id$
**  
**  Copyright (C) 2003 ~ 2009 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

#ifndef _GAL_nexusvideo_h
#define _GAL_nexusvideo_h

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

/* Private display data */

struct GAL_PrivateVideoData
{
    void   *privateData;
};

int     Nexus_ServerOnGetSurface(REQ_NEXUS_GETSURFACE * request,
                                 REP_NEXUS_GETSURFACE * reply);

#endif /* _GAL_nexusvideo_h */
