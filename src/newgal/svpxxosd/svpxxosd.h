/*
** $Id: svpxxosd.h 7356 2007-08-16 05:03:46Z xgwang $
**
** svpxxosd.h: text definitions for FHAS.
**
** Copyright (C) 2003 ~ 2007 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Current maintainer: <liu jianjun> (<falistar@minigui.org>).
*/

#ifndef _GAL_svpxxosd_h
#define _GAL_svpxxosd_h

/* define this if you want 8bpp */
/* #undef _SVPXXOSD_8BPP_ */
#define _SVPXXOSD_8BPP_     1

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

/* Private display data */

struct GAL_PrivateVideoData
{
    int status;			/* 0=never inited, 1=once inited, 2=inited. */
    int w, h, pitch;
    void *fb;
};

#endif /* GAL_svpxxosd_h */
