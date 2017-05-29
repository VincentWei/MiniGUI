/*
** $Id: c33l05.h 7166 2007-05-16 04:16:15Z weiym $
**
** c33l05.h: the head file of Low Level Input Engine for C33L05.
**
** Copyright (C) 2007 Feynman Software.
*/

#ifndef GUI_IAL_C33L05_H
#define GUI_IAL_C33L05_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

BOOL	InitC33L05Input (INPUT* input, const char* mdev, const char* mtype);
void	TermC33L05Input (void);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* GUI_IAL_C33L05_H */


