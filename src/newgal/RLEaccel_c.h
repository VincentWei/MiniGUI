/*
**  $Id: RLEaccel_c.h 7344 2007-08-16 03:56:40Z xgwang $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

/* Useful functions and variables from RLEaccel.c */

extern int GAL_RLESurface(GAL_Surface *surface);
extern int GAL_RLEBlit(GAL_Surface *src, GAL_Rect *srcrect,
                       GAL_Surface *dst, GAL_Rect *dstrect);
extern int GAL_RLEAlphaBlit(GAL_Surface *src, GAL_Rect *srcrect,
			    GAL_Surface *dst, GAL_Rect *dstrect);
extern void GAL_UnRLESurface(GAL_Surface *surface, int recode);

