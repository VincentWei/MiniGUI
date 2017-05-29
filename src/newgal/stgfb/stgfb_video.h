/*
**  $Id: stgfb_video.h 13621 2010-11-10 08:14:16Z humingming $
**  
**  Copyright (C) 2003 ~ 2010 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming
**
**  All rights reserved by Feynman Software.
**
**  This is the NEWGAL STGFB engine, based ST native ioctl.
**
*/

#ifdef _MGGAL_STGFB

#ifndef _GAL_stgfbvideo_h
#define _GAL_stgfbvideo_h

#include <sys/types.h>
#include <linux/fb.h>
#include <stgxobj.h>

#include "sysvideo.h"
#include "videomem-bucket.h"


/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this


/* for GAL_SURFACE */
struct private_hwdata { 
    char* user_addr;  /* video memory address, for user space */

    int offset;       /* video memory address offset, for process-version client use this to access */
    gal_vmblock_t* block; /* memory manager block, for process-version client do NOT use this to access */

    STGXOBJ_Bitmap_t st_bitmap; 
};


/* Private display data */
struct GAL_PrivateVideoData {
    int console_fd;
    int stlayer_fd;

    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;

    int mapped_memlen;      /* video memory total len */
    int mapped_offset;      /* for page align offset */
    char *mapped_mem;       /* video memory address, for user space */

    gal_vmbucket_t vmem_bucket; /* video memory manager */

    STLAYER_ViewPortHandle_t viewport_handle;
    STGXOBJ_Bitmap_t screen_bmp;
};

/* old variable names */
#define console_fd          (this->hidden->console_fd)
#define stlayer_fd          (this->hidden->stlayer_fd)
#define finfo               (this->hidden->finfo)
#define vinfo               (this->hidden->vinfo)
#define mapped_mem          (this->hidden->mapped_mem)
#define mapped_offset       (this->hidden->mapped_offset)
#define mapped_memlen       (this->hidden->mapped_memlen)
#define vmem_bucket         (this->hidden->vmem_bucket)
#define screen_bmp          (this->hidden->screen_bmp)
#define viewport_handle     (this->hidden->viewport_handle)

#endif /* _GAL_stgfbvideo_h */

#endif /* _MGGAL_STGFB */

