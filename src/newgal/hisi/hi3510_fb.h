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
** $Id: hi3510_fb.h 7268 2007-07-03 08:08:43Z xwyan $
**
** hi3510_fb.h: header file. 
**
** Copyright (C) 2007 Feynman Software.
**
** All rights reserved by Feynman Software.
*/

#ifndef __HI_3510FB_H__
#define __HI_3510FB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define FBIOGET_COLORKEY_HI3510       0x4690  /*hi3510 extend*/
#define FBIOPUT_COLORKEY_HI3510       0x4691  /*hi3510 extend*/
#define FBIOGET_ALPHA_HI3510          0x4692  /*hi3510 extend*/
#define FBIOPUT_ALPHA_HI3510          0x4693  /*hi3510 extend*/
#define FBIOPUT_SCREEN_ORIGIN_HI3510  0x4694  /*hi3510 extend*/
#define FBIOGET_SCREEN_ORIGIN_HI3510  0x4695  /*hi3510 extend*/

#define FB_ACCEL_HI3510         0x9E    /*hi3510 2d accerlerate*/

/*hi3510fb extend*/
typedef struct tagfb_colorkey
{
    unsigned long key;
    unsigned char key_enable;	
    unsigned char mask_enable;	
    unsigned char rmask;
    unsigned char gmask;
    unsigned char bmask;
    char  reserved[3];
}fb_colorkey;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_INC_VOU_H__ */

