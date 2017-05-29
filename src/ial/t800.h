/*
** $Id: t800.h 2445 2003-09-04 03:40:35Z weiym $
**
** t800.h:. the head file of IAL Engine for MT T800 device.
**
** Copyright (C) 2002 MT.
*/

#ifndef GUI_IAL_T800_H
    #define GUI_IAL_T800_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitT800Input (INPUT* input, const char* mdev, const char* mtype);
void TermT800Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_T800_H */


