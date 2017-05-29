/*
** $Id: random.h 7335 2007-08-16 03:38:27Z xgwang $
**
** random.h: head file for random IAL Engine
**
** Copyright (C) 2005 ~ 2007  Feynman Software.
**
** Created by Wei YongMing, 2005/06/08
*/

#ifndef _IAL_RANDOM_H
#define _IAL_RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitRandomInput (INPUT* input, const char* mdev, const char* mtype);
void TermRandomInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _IAL_RANDOM_H */

