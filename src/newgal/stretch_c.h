/*
**  $Id: stretch_c.h 6566 2006-05-10 01:44:57Z xwyan $
**  
**  Copyright (C) 2003 ~ 2006 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

/* Perform a stretch blit between two surfaces of the same format.
   NOTE:  This function is not safe to call from multiple threads!
*/
extern int GAL_SoftStretch(GAL_Surface *src, GAL_Rect *srcrect,
                           GAL_Surface *dst, GAL_Rect *dstrect);

