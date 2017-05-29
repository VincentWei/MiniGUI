/*
** $Id: fxrm9200.h 7335 2007-08-16 03:38:27Z xgwang $
**
** fxrm9200.h:. the head file of Low Level Input Engine for fuxu rm9200.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Created by pan weiguo, 2004/07/20
*/

#ifndef GUI_IAL_FXRM9200_H
    #define GUI_IAL_FXRM9200_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitRm9200Input (INPUT* input, const char* mdev, const char* mtype);
void    TermRm9200Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_FXRM9200_H */


