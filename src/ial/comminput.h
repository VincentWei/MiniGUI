/*
** $Id: comminput.h 7335 2007-08-16 03:38:27Z xgwang $
**
** comminput.h: head file for comm Input Engine
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Created by Zhong Shuyi, 2004/02/29
*/

#ifndef _IAL_COMM_H
#define _IAL_COMM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitCOMMInput (INPUT* input, const char* mdev, const char* mtype);
void TermCOMMInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _IAL_COMM_H */

