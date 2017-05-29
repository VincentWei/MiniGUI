/*
** $Id: mb93493video.h 7353 2007-08-16 04:58:32Z xgwang $
**  
** Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#ifndef _GAL_MB93493VIDEO_H
#define _GAL_MB93493VIDEO_H

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

/* Private display data */

struct GAL_PrivateVideoData {

    int is_opened;
    int w, h;
    int mode;
    Uint8 *buffer;

    BOOL dirty;
    RECT update;

    pthread_t th;
    pthread_mutex_t lock;
};

#endif /* _GAL_MB93493VIDEO_H */
