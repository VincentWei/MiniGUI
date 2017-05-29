/*
** $Id: dlcustom.h 6566 2006-05-10 01:44:57Z wanzheng $
**
** dlcustom.h:. the head file for dlcustom IAL Engine.
**
** Copyright (C) 2007 Feynman Software.
**
** Created by WanZheng, 2009/10/17
*/

#ifndef GUI_IAL_DLCUSTOM_H
#define GUI_IAL_DLCUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitDLCustomInput (INPUT* input, const char* mdev, const char* mtype);
void TermDLCustomInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_DLCUSTOM_H */
