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
** $Id: hi3560_fb.h 11368 2009-03-05 09:50:30Z tangjianbin $
**
** hi3560_fb.h: header file. 
**
** Copyright (C) 2007 Feynman Software.
**
** All rights reserved by Feynman Software.
*/

#ifndef __HIFB_H__
#define __HIFB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"

#define IOC_TYPE_HIFB       'F'
#define FBIOGET_COLORKEY_HIFB       _IOR(IOC_TYPE_HIFB, 90, fb_colorkey)
#define FBIOPUT_COLORKEY_HIFB       _IOW(IOC_TYPE_HIFB, 91, fb_colorkey)
#define FBIOGET_ALPHA_HIFB          _IOR(IOC_TYPE_HIFB, 92, fb_alpha)
#define FBIOPUT_ALPHA_HIFB          _IOW(IOC_TYPE_HIFB, 93, fb_alpha)
#define FBIOGET_SCREEN_ORIGIN_HIFB  _IOR(IOC_TYPE_HIFB, 94, fb_point)
#define FBIOPUT_SCREEN_ORIGIN_HIFB  _IOW(IOC_TYPE_HIFB, 95, fb_point)
#define FBIOGET_GAMMAP_HIFB          _IOR(IOC_TYPE_HIFB, 96, fb_gammap)
#define FBIOPUT_GAMMAP_HIFB          _IOR(IOC_TYPE_HIFB, 97, fb_gammap)

#define FB_ACCEL_HIFB         0x9E    /*hi 2d accerlerate*/

/*hifb extend*/
typedef struct tagfb_colorkey
{
    HI_BOOL bKeyEnable; /* colorkey enable flag */
    HI_BOOL bMaskEnable; /* key mask enable flag */
    HI_U32 u32Key; /* colorkey value, maybe contains alpha */
    HI_U8 u8RedMask; /* red mask */
    HI_U8 u8GreenMask; /* green mask */
    HI_U8 u8BlueMask; /* blue mask */
    HI_U8 u8Reserved;
/*    unsigned long key;
    unsigned char key_enable;	
    unsigned char mask_enable;	
    unsigned char rmask;
    unsigned char gmask;
    unsigned char bmask;
    char  reserved[3];*/
}fb_colorkey;

typedef struct tagfb_alpha
{
    HI_BOOL bAlphaEnable;   /* alpha enable flag */
    HI_BOOL bAlphaChannel;  /* alpha channel enable flag */
    HI_U8 u8Alpha0;         /* alpha0 value, used in ARGB1555 */
    HI_U8 u8Alpha1;         /* alpha1 value, used in ARGB1555 */
    HI_U8 u8GlobalAlpha;    /* global alpha value */
    HI_U8 u8Reserved;
//    unsigned char alpha0;   /* the value of alpha0 register */
//    unsigned char alpha1;   /* the value of alpha1 register */
//    unsigned char AOE;      /* channel alpha enable flag */
//    unsigned char AEN;      /* pixel alpha enable flag */
}fb_alpha;

typedef struct tagfb_point
{
    unsigned short x;       /* horizontal position */
    unsigned short y;       /* vertical position */
}fb_point;


typedef struct tagfb_gammap
{
    unsigned char enable;   /* gamma table enable flag */
    unsigned int start;     /* first entry */
    unsigned int len;       /* table length */
    unsigned char *red;     /* red gamma values */
    unsigned char *green;   /* green gamma values */
    unsigned char *blue;    /* blue gamma values */
}fb_gammap;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HIFB_INC_VOU_H__ */

