/*
**  $Id: em86gfx.h 7351 2007-08-16 04:55:58Z xgwang $
**  
**  em86gfx.c: NEWGAL driver for EM86xx GFX.
**
**  Copyright (C) 2007 Feynman Software.
*/

#ifndef _sigma8654_pri_h_ 
#define _sigma8654_pri_h_

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

typedef struct _Sigma8654Private_HWSurface_hwdata
{
    RMuint32 gfx_addr;
    RMint32 size;
    RMuint32 unmapped_addr;
    RMuint32 surface_addr;
    struct DCCVideoSource* osd_source;
}Sigma8654Private_HWSurface_hwdata;


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _GAL_GFXVIDEO_H */
