/*
** $Id: dfb.h 7335 2007-08-16 03:38:27Z xgwang $
**
** dfb.h: Low Level Input Engine for DirectFB.
** 
** Copyright (C) 2005 ~ 2007, Feynman Software.
**
** All rights reserved by Feynman Software.
**
** Created by Xiaogang Du, 2005/12/15
*/

#ifndef	_DFB_H_
#define _DFB_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

struct _dfb2mgkey {
    unsigned int dfbkey;
    unsigned char mgkey;
};

BOOL    InitDFBInput (INPUT* input, const char* mdev, const char* mtype);
void    TermDFBInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif	/* _DFB_H_ */
