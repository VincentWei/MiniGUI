//
// $Id: wvfb.h 7335 2007-08-16 03:38:27Z xgwang $
//
// wvfb.h: head file of Input Engine for Windows Virtual FrameBuffer
//
// Copyright (C) 2001 ~ 2002, Wei YongMing 
// Copyright (C) 2003 ~ 2007, Feynman Software
//
// Created by Wei YongMing, 2000/11/26
//

#ifndef _IAL_wvfb_h
#define _IAL_wvfb_h

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitWVFBInput (INPUT* input, const char* mdev, const char* mtype);
void TermWVFBInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _IAL_wvfb_h */

