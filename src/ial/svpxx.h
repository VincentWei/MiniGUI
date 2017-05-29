/*
** $Id: svpxx.h 7335 2007-08-16 03:38:27Z xgwang $
**
** svpxx.h: text definitions for FHAS.
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
#ifndef GUI_IAL_SVPXX_H
#define GUI_IAL_SVPXX_H


typedef unsigned char uint8;
#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

    BOOL InitSvpxxInput (INPUT * input, const char *mdev, const char *mtype);
    void TermSvpxxInput (void);

    extern int KeyPadIRInit (void);
    extern int PollingKeyPadIR (uint8 * key);


#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif				/* GUI_IAL_SVPXX_H */
