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
**  $Id$
**  
**  Copyright (C) 2003 ~ 2009 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

#ifndef _nexusvideo_pri_h
#define _nexusvideo_pri_h

typedef struct _NexusPrivate_HWSurface_hwdata
{
    void   *display;
    void   *surfaceHandle;
    void   *gfx;
    int     isCopied;
} NexusPrivate_HWSurface_hwdata;

GAL_Rect **NexusPrivate_get_modes_rects(GAL_Rect ** modes, int size);
int NexusPrivate_get_mode(int bpp, int width, int height);

int     NexusPrivate_init(void **pThis, int isServer, int width, int height);
void    NexusPrivate_close(void *_this, int isServer);
int     NexusPrivate_CreateSurface(void *_this, int w, int h,
                                   unsigned int *pPitch, void **pPixels,
                                   NexusPrivate_HWSurface_hwdata ** pHwdata);
NexusPrivate_HWSurface_hwdata *
NexusPrivate_SurfaceDup(NexusPrivate_HWSurface_hwdata * src);
void   *NexusPrivate_SurfaceGetMemory(void *_surfaceHandle);
void    NexusPrivate_FreeSurface(void *_this,
                                 NexusPrivate_HWSurface_hwdata * hwdata);
int     NexusPrivate_FillRect(void *_this,
                              NexusPrivate_HWSurface_hwdata * hwdata, int x,
                              int y, int w, int h, unsigned int color);
int     NexusPrivate_BitBlitEx(void *_this, NexusPrivate_HWSurface_hwdata * src,
                     int src_x, int src_y, int src_w, int src_h,
                     NexusPrivate_HWSurface_hwdata * dst, int dst_x, int dst_y,
                     int dst_w, int dst_h,
                     int flag, unsigned int colorkey_src, unsigned int colorkey_dst, 
                     unsigned int alpha_src);
int     NexusPrivate_SelectSurface(void *_this,
                                   NexusPrivate_HWSurface_hwdata * hwdata,
                                   int isServer);
void    NexusPrivate_Flush(void *_this, NexusPrivate_HWSurface_hwdata *hwdata);

void   *NexusPrivate_GetDisplay(void);
#endif /* _nexusvideo_pri_h */
