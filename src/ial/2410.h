/*
** $Id: 2410.h 3206 2004-04-01 08:57:13Z weiym $
**
** 2410.h: 2410 the head file of Low Level Input Engine for S3C2410.
**
** Copyright (C) 2004 Feynman Software.
*/

#ifndef GUI_IAL_2410_H
#define GUI_IAL_2410_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

BOOL	Init2410Input (INPUT* input, const char* mdev, const char* mtype);
void	Term2410Input (void);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* GUI_IAL_2410_H */


