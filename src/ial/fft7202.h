/*
** $Id: fft7202.h 7335 2007-08-16 03:38:27Z xgwang $
**
** fft7202.h:. the head file of Low Level Input Engine for iPAQ
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Created by Yao yunyuna, 2001/08/20
*/

#ifndef GUI_IAL_IPAQ_H
    #define GUI_IAL_IPAQ_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitFFTInput (INPUT* input, const char* mdev, const char* mtype);
void    TermFFTInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_IPAQ_H */


