/*
** $Id: auto.h 6566 2006-05-10 01:44:57Z xwyan $
**
** auto.h: head file for automatic Input Engine
**
** Copyright (C) 2004  ~ 2006Feynman Software.
**
** Created by Wei YongMing, 2004/01/29
*/

#ifndef _IAL_AUTO_H
#define _IAL_AUTO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitAutoInput (INPUT* input, const char* mdev, const char* mtype);
void TermAutoInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _IAL_AUTO_H */

