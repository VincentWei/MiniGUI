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

