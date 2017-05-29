/*
** $Id: hh2440.h 7335 2007-08-16 03:38:27Z xgwang $
**
** hh2440.h: the head file of Low Level Input Engine for HHARM2440.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#ifndef GUI_IAL_HH2440_H
#define GUI_IAL_HH2440_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

BOOL	InitHH2440Input (INPUT* input, const char* mdev, const char* mtype);
void	TermHH2440Input (void);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* GUI_IAL_HH2440_H */


