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
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include "define.h"
#include "xxvfbhdr.h"
#include "xxvfb_common.h"

extern XXVFBInfo xInfo;

void DrawImage(int x, int y, int w, int h, char* data)
{
    XImage *pImage = XCreateImage(xInfo.display,xInfo.visual,xInfo.dev_depth,ZPixmap,
            0,data,w,h,8,0);
#if 0
    fprintf(stderr,"width=%d,height=%d\n",pImage->width,pImage->height);
    fprintf(stderr,"xoffset=%d\n",pImage->xoffset);
    fprintf(stderr,"format=%d\n",pImage->format);
    fprintf(stderr,"bitmap_bit_order=%d\n",pImage->bitmap_bit_order);
    fprintf(stderr,"bitmap_unit=%d\n",pImage->bitmap_unit);
    fprintf(stderr,"bitmap_pad=%d\n",pImage->bitmap_pad);
    fprintf(stderr,"bytes_per_line=%d\n",pImage->bytes_per_line);
    fprintf(stderr,"r=0x%x,g=0x%x,b=0x%x\n",pImage->red_mask, pImage->green_mask, pImage->blue_mask);
    fprintf(stderr,"x=%d,y=%d,w=%d,h=%d\n",x,y,w,h);
#endif

    if(pImage == NULL)
    {
        fprintf(stderr,"Create Image Error\n");
        return;
    }

    XPutImage(xInfo.display, xInfo.win, xInfo.gc, pImage, 0, 0, x, y, w, h);

    XFlush(xInfo.display);
    XDestroyImage(pImage);
}
