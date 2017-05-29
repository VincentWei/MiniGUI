/*
** $Id: dummy.h 6566 2006-05-10 01:44:57Z xwyan $
**
** custom.h:. the head file for Custom IAL Engine.
**
** Copyright (C) 2007 Feynman Software.
**
** Created by Wei YongMing, 2007/06/06
*/

#ifndef GUI_IAL_CUSTOM_H
    #define GUI_IAL_CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitCustomInput (INPUT* input, const char* mdev, const char* mtype);
void TermCustomInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_CUSTOM_H */
