/*
** $Id: bf533video.h 7345 2007-08-16 03:57:17Z xgwang $
**  
** Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#ifndef _GAL_BF533VIDEO_H
#define _GAL_BF533VIDEO_H

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

#define OSD_AGAIN           0
#define OSD_AGAIN1          0xFFFF
#define OSD_OK              1
#define OSD_ERROR           2

#define OSD_WIDTH           640
#define OSD_HEIGHT          480

#define OSD_OP_OPEN         1
#define OSD_OP_CLOSE        2
#define OSD_OP_SET_PALETTE  3
#define OSD_OP_UPDATE_RECT  4

typedef struct _osd_op_header {
    Uint8 app_id [4];
    Uint32 op_id;
    Uint32 data_len;
    Uint16 x, y;
    Uint16 w, h;
    Uint16 pitch, pad;
} OSD_OP_HEADER;

/* Private display data */

struct GAL_PrivateVideoData {
    int fd_spi;
    int w, h;
    Uint8 *buffer;

    int  fd_lock;

    BOOL dirty;
    RECT update;
};

#if 0
    pthread_t th;
    pthread_mutex_t lock;
#endif

#endif /* _GAL_BF533VIDEO_H */
