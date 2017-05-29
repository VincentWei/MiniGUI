/*
** $Id: mstarial.h  7249 2010-03-24 03:38:27Z czzhao $
**
** mstarial.h: the head file of Low Level Input Engine for 
**            mstar remote controller 
**
** Copyright (C) 2007 ~ 2011 Feynman Software.
**
** Created by Zhao Chengzhang, 2010/06/14
*/

#ifndef _IAL_MSTAR_IR_H
#define _IAL_MSTAR_IR_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitMStarInput (INPUT* input, const char* mdev, const char* mtype);
void    TermMStarInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _IAL_MSTAR_IR_H */
