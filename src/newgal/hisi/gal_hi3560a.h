/*
**  $Id: nullvideo.h 12768 2010-04-22 05:28:30Z wanzheng $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

#ifndef _GAL_hi3560video_h
#define _GAL_hi3560video_h

#include "sysvideo.h"
#include "videomem-bucket.h"

#include "hi_tde_api.h"
#include "hi_tde_type.h"
#include "hi_tde_errcode.h"
#include "hi_unf_disp.h"
#include "hi_unf_vo.h"
#include "hifb.h"
/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

/* Private display data */

struct GAL_PrivateVideoData { /* for 3560a only */
    HI_HANDLE handle;
    int fd_fb;
    unsigned int smem_start; /* physics address */
    unsigned int smem_len;

    unsigned char *pixels;
    int width;
    int height;
    int pitch;
    int bpp;

    gal_vmbucket_t vmbucket;
    int mapped_memlen;
};

struct private_hwdata { /* for GAL_SURFACE */
    gal_vmblock_t *vmblock;
    unsigned int addr_phy;
};

#endif /* _GAL_s3c6410video_h */
