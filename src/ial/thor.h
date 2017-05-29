/*
** $Id: thor.h 2445 2003-09-04 03:40:35Z weiym $
**
** thor.h:. the head file of Low Level Input Engine for ptsc MX1
**
** Copyright (C) 2002 PTSC.
*/

#ifndef GUI_IAL_THOR_H
    #define GUI_IAL_THOR_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitTHORInput (INPUT* input, const char* mdev, const char* mtype);
void    TermTHORInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_THOR_H */


