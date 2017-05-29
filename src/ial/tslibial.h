/*
** $Id$
**
** tslib.h:. the head file of Low Level Input Engine for TSLIB
**
** Copyright (C) 2008 Feynman Software.
**
** Created by Wei YongMing, 2008/09/20
*/

#ifndef GUI_IAL_TSLIB_H
    #define GUI_IAL_TSLIB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL  InitTSLibInput (INPUT* input, const char* mdev, const char* mtype);
void  TermTSLibInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_TSLIB_H */

