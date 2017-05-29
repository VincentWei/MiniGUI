/*
**  $Id: yuvfuncs.h 7344 2007-08-16 03:56:40Z xgwang $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

/* This is the definition of the YUV video surface function structure */

#include "newgal.h"
#include "sysvideo.h"

#ifndef _THIS
#define _THIS	GAL_VideoDevice *_this
#endif
struct private_yuvhwfuncs {
	int (*Lock)(_THIS, GAL_Overlay *overlay);
	void (*Unlock)(_THIS, GAL_Overlay *overlay);
	int (*Display)(_THIS, GAL_Overlay *overlay, GAL_Rect *dstrect);
	void (*FreeHW)(_THIS, GAL_Overlay *overlay);
};
