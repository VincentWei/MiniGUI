/*
** $Id: hi3510.h 7269 2007-07-03 08:10:14Z xwyan $
**
** hi3510.h: head file of Input Engine for hi3510
**
** Copyright (C) 2007 Feynman Software.
**
** All rights reserved by Feynman Software.
*/

#ifndef _IAL_hi3510_h
#define _IAL_hi3510_h

#include "common.h"
#include "ial.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitHI3510Input (INPUT* input, const char* mdev, const char* mtype);
void TermHI3510Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _IAL_hi3510_h */


