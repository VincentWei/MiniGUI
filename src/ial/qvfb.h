/*
** $Id: qvfb.h 7335 2007-08-16 03:38:27Z xgwang $
**
** qvfb.h: head file of Input Engine for Qt Virtual FrameBuffer
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Created by Wei YongMing, 2000/11/26
*/

#ifndef _IAL_qvfb_h
#define _IAL_qvfb_h

#define QT_VFB_MOUSE_PIPE       "/tmp/.qtvfb_mouse-%d"
#define QT_VFB_KEYBOARD_PIPE    "/tmp/.qtvfb_keyboard-%d"

struct QVFbKeyData
{
    unsigned int unicode;
    unsigned int modifiers;
    BYTE press;
    BYTE repeat;
};

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitQVFBInput (INPUT* input, const char* mdev, const char* mtype);
void TermQVFBInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _IAL_qvfb_h */

