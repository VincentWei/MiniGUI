/*
**  $Id: em85xxosd.h 7349 2007-08-16 04:54:21Z xgwang $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
*/


#ifndef _GAL_em85xxosd_h
#define _GAL_em85xxosd_h

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

struct GAL_PrivateVideoData {
    int status;      /* 0=never inited, 1=once inited, 2=inited. */
    RUA_handle handle;
    Uint8* osd_buffer;

    int w, h, pitch;
    void* fb;
    BOOL dirty;
};

#endif /* _GAL_em85xxosd_h */

