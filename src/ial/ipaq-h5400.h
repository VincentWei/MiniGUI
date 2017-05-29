/*
** $Id: ipaq_h5400.h 7335 2007-08-16 03:38:27Z xgwang $
**
** ipaq_h5400.h:. the head file of Low Level Input Engine for iPAQ
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
**
** Created by Wei YongMing, 2001/08/20
*/

#ifndef GUI_IAL_IPAQ_H5400_H
    #define GUI_IAL_IPAQ_H5400_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitIPAQH5400Input (INPUT* input, const char* mdev, const char* mtype);
void    TermIPAQH5400Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_IPAQ_H5400_H */


