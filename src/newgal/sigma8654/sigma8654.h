/*
**  $Id: em86gfx.h 7351 2007-08-16 04:55:58Z xgwang $
**  
**  em86gfx.c: NEWGAL driver for EM86xx GFX.
**
**  Copyright (C) 2007 Feynman Software.
*/

#ifndef _GAL_GFXVIDEO_H
#define _GAL_GFXVIDEO_H

#ifndef ALLOW_OS_CODE
#define ALLOW_OS_CODE
#endif 
#define EM86XX_CHIP EM86XX_CHIPID_TANGO3
#define EM86XX_REVISION 3
#define WITH_THREADS

#include "dcc/include/dcc.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define _THIS	GAL_VideoDevice * this

struct GAL_PrivateVideoData {
    struct RUA * pRUA;
    struct DCC * pDCC;
    struct DCCOSDProfile DCC_profile;
    struct DCCVideoSource * pOSD_source;
    RMuint32 OSD_scaler;
    RMuint32 LumaAddr;
    RMuint8* LumaAddrMapped;
    RMuint32 LumaSize;
    RMuint32 ChromaAddr;
    RMuint8* ChromaAddrMapped;
    RMuint32 ChromaSize;

    struct GFXEngine_Open_type GFX_profile;
    RMuint32 GFXtarget;
};


inline void * RMMalloc(RMuint32 size)
{
    return malloc(size);
}

inline void RMFree(void * ptr)
{
    free (ptr);
}

inline void * RMMemset(void * s, RMuint8 c, RMuint32 n)
{
    return memset(s, c, n);
}

inline void * RMMemcpy(void * dest, const void * src, RMuint32 n)
{
    return memcpy(dest, src, n);
}

inline RMint32 RMMemcmp(const void * s1, const void * s2, RMuint32 n)
{
    return memcmp(s1, s2, n);
}

inline void * RMCalloc(RMuint32 nmemb, RMuint32 size)
{
    void * ptr = RMMalloc(nmemb * size);
    if (ptr != NULL) RMMemset(ptr, 0, nmemb * size);
    return ptr;
}



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _GAL_GFXVIDEO_H */
