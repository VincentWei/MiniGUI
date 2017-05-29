/*
** $Id: evmv10.h 7335 2007-08-16 03:38:27Z xgwang $
**
** evmv10.h:. the head file of Low Level Input Engine for EVMV10
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Created by Yao Yunyuan, 2004/07/23
*/

#ifndef GUI_IAL_EVMV10_H
    #define GUI_IAL_EVMV10_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitXscaleEVMV10Input (INPUT* input, const char* mdev, const char* mtype);
void    TermXscaleEVMV10Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_EVMV10_H */


