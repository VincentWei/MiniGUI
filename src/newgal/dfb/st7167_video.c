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
**  This is the ST7167 platform DirectFB NEWGAL engine.
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

