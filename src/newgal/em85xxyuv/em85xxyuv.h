/*
**  $Id: em85xxyuv.h 7350 2007-08-16 04:55:17Z xgwang $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
*/


#ifndef _GAL_em85xxyuv_h
#define _GAL_em85xxyuv_h

#include "sysvideo.h"

#define BYTE        RMuint8
#define LONG        RMint32
#define ULONG       RMuint32
#define ULONGLONG   RMuint64
#define BOOL        RMbool
#define BOOLEAN     RMbool
#define HANDLE      void *
#define UCHAR       RMuint8
#define USHORT      RMuint16
#define PULONG      RMuint32 *
#define PVOID       void *

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

/* Private display data */

typedef struct __attribute__ ((packed)) _YUV
{
    Uint8 y, u, v;
} YUV;

struct GAL_PrivateVideoData {
    int status;      /* 0=never inited, 1=once inited, 2=inited. */
    RUA_handle handle;

    int w, h;
    Uint16 shadow_pitch;
    Uint8 *shadow_fb;

    BOOL dirty;
    RECT update;
	void* rgb2yuv_map;
    int lock;
};

#endif /* _GAL_em85xxyuv_h */

