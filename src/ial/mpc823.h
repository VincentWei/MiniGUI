/*
** $Id: mpc823.h 7335 2007-08-16 03:38:27Z xgwang $
**
** mpc823.h:. the head file of Low Level Input Engine for sinovee sc823 board.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Created by Zhong Shuyi, 2003/06/04
*/

#ifndef GUI_IAL_MPC823_H
    #define GUI_IAL_MPC823_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitMPC823Input (INPUT* input, const char* mdev, const char* mtype);
void    TermMPC823Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_MPC823_H */


