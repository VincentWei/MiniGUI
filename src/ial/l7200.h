/*
** $Id: l7200.h 7335 2007-08-16 03:38:27Z xgwang $
**
** ipaq.h:. the head file of Low Level Input Engine for iPAQ
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Created by Wei YongMing, 2001/08/20
*/

#ifndef GUI_IAL_L7200_H
    #define GUI_IAL_L7200_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BOOL InitL7200Input (INPUT* input, const char* mdev, const char* mtype);
void TermL7200Input (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GUI_IAL_L7200_H */
