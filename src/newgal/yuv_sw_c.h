/*
**  $Id: yuv_sw_c.h 7344 2007-08-16 03:56:40Z xgwang $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

#include "newgal.h"
#include "sysvideo.h"

/* This is the software implementation of the YUV video overlay support */

extern GAL_Overlay *GAL_CreateYUV_SW(_THIS, int width, int height, Uint32 format, GAL_Surface *display);

extern int GAL_LockYUV_SW(_THIS, GAL_Overlay *overlay);

extern void GAL_UnlockYUV_SW(_THIS, GAL_Overlay *overlay);

extern int GAL_DisplayYUV_SW(_THIS, GAL_Overlay *overlay, GAL_Rect *dstrect);

extern void GAL_FreeYUV_SW(_THIS, GAL_Overlay *overlay);

