/*
**  $Id: st7167_video.c 13059 2010-07-30 07:44:50Z humingming $
**  
**  Copyright (C) 2003 ~ 2010 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming
**
**  All rights reserved by Feynman Software.
**
**  This is the ST7167 platform DirectFB NEWGAL engine.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#ifdef _MGGAL_DFB_ST7167

#include <linux/fb.h>
#include <directfb.h>

#include "dfbvideo.h"
#include "st7167_video.h"

extern GAL_FunctionTable mgGALFuncTable;


static int ST7167_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    /* call the common dfb function */
    if ((*mgGALFuncTable.VideoInit)(this, vformat) != 0)
        return -1;
    
    /* now st7167 dfb don't have fill acceleration, use fbcon directly better than use dfb */
    //this->info.blit_fill = 0;
    return 0;
}

static void ST7167_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    /* do nothing for this */
	return;
}

int mgChangeDeviceCallback (GAL_VideoDevice* device)
{
    if (device == NULL)
        return -1;
    
    device->VideoInit = ST7167_VideoInit;
    device->UpdateRects = ST7167_UpdateRects;
    return 0;
}

#endif /* _MGGAL_DFB_ST7167 */

