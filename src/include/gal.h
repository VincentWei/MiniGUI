/*
** $Id: gal.h 8944 2007-12-29 08:29:16Z xwyan $
**
** gal.h: the head file of Graphics Abstract Layer
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming
**
** Create date: 2001/10/07
*/

#ifndef GUI_GAL_H
    #define GUI_GAL_H

#ifndef _MGRM_THREADS

extern BOOL __mg_switch_away; // always be zero for clients.

#ifdef _MGRM_PROCESSES
extern GHANDLE __mg_layer;

void unlock_draw_sem (void);
void lock_draw_sem (void);
#endif

#endif

#include "newgal.h"

#define WIDTHOFPHYSCREEN      WIDTHOFPHYGC
#define HEIGHTOFPHYSCREEN     HEIGHTOFPHYGC

#endif  /* GUI_GAL_H */

