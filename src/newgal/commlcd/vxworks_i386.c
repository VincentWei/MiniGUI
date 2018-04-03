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
**  vxworks_i386.c: A subdriver of CommonLCD NEWGAL engine for VxWorks on i386.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_COMMLCD

/*WINML3.0 reference*/
#if defined (__VXWORKS__) && defined (__TARGET_VXI386__)

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "commlcd.h"

#include <ugl/uglugi.h>
#include <ugl/uglinfo.h>
#include <ugl/ugl.h>

static UGL_DEVICE_ID devId;
UGL_GC_ID gc;
    
static int a_init (void)
{
    if (uglInitialize() == UGL_STATUS_ERROR)
        return 1;
    
    return 0; 
}

static int a_getinfo (struct lcd_info *li)
{
    UGL_MODE_INFO modeInfo;

    
    /* Obtain display device identifier */
    devId = (UGL_DEVICE_ID) uglRegistryFind (UGL_DISPLAY_TYPE,  
                    0, 0, 0)->id;
   
    /* Create a graphics context */
    gc = uglGcCreate (devId);
    
#ifndef _VESA_SUPPORT
    uglInfo (devId, UGL_MODE_INFO_REQ, &modeInfo);
    li->type = COMMLCD_PSEUDO_RGB332;
#else
    uglPcBiosInfo (devId, UGL_MODE_INFO_REQ, &modeInfo);
    li->type = COMMLCD_TRUE_RGB565;
#endif

    li->height = modeInfo.height;
    li->width = modeInfo.width;
    li->fb = modeInfo.fbAddress;
    li->bpp = modeInfo.colorDepth;
    li->rlen = (li->bpp*li->width + 7) / 8;
    return 0;
}

#ifndef _VESA_SUPPORT    
static int a_setclut (int firstcolor, int ncolors, GAL_Color *colors)
{
    int i;
   
    UGL_ARGB argb [256];
    if (ncolors > 256) ncolors = 256;
    
    for (i = 0; i < ncolors; i++)
        argb [i] = UGL_MAKE_RGB (colors[i].r, colors[i].g, colors[i].b);
    
    uglClutSet (devId, firstcolor, argb, ncolors);
    return 1;
}
#endif

struct lcd_ops __mg_commlcd_ops = {
    a_init,
    a_getinfo,
    NULL,
    #ifndef _VESA_SUPPORT    
        a_setclut,
    #else
        NULL,
    #endif
    NULL
};

#endif /* __VXWORKS__ && __TARGET_VXI386__ */

#endif /* _MGGAL_COMMLCD */

