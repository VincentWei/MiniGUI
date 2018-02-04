/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
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
