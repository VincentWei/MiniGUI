/*
**  $Id: yuv.c 7344 2007-08-16 03:56:40Z xgwang $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

/* This is the implementation of the YUV video surface support */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "newgal.h"
#include "sysvideo.h"
#include "yuvfuncs.h"
#include "yuv_sw_c.h"


GAL_Overlay *GAL_CreateYUVOverlay(int w, int h, Uint32 format,
                                  GAL_Surface *display)
{
	GAL_VideoDevice *video = current_video;
	GAL_VideoDevice *this  = current_video;
	const char *yuv_hwaccel;
	GAL_Overlay *overlay;

	overlay = NULL;

	/* Display directly on video surface, if possible */
	if ( getenv("GAL_VIDEO_YUV_DIRECT") ) {
		if ( (display == GAL_PublicSurface) &&
		     ((GAL_VideoSurface->format->BytesPerPixel == 2) ||
		      (GAL_VideoSurface->format->BytesPerPixel == 4)) ) {
			display = GAL_VideoSurface;
		}
	}
        yuv_hwaccel = getenv("GAL_VIDEO_YUV_HWACCEL");
	if ( ((display == GAL_VideoSurface) && video->CreateYUVOverlay) &&
	     (!yuv_hwaccel || (atoi(yuv_hwaccel) > 0)) ) {
		overlay = video->CreateYUVOverlay(this, w, h, format, display);
	}
	/* If hardware YUV overlay failed ... */
	if ( overlay == NULL ) {
		overlay = GAL_CreateYUV_SW(this, w, h, format, display);
	}
	return overlay;
}

int GAL_LockYUVOverlay(GAL_Overlay *overlay)
{
	return overlay->hwfuncs->Lock(current_video, overlay);
}

void GAL_UnlockYUVOverlay(GAL_Overlay *overlay)
{
	overlay->hwfuncs->Unlock(current_video, overlay);
}

int GAL_DisplayYUVOverlay(GAL_Overlay *overlay, GAL_Rect *dstrect)
{
	return overlay->hwfuncs->Display(current_video, overlay, dstrect);
}

void GAL_FreeYUVOverlay(GAL_Overlay *overlay)
{
	if ( overlay ) {
		if ( overlay->hwfuncs ) {
			overlay->hwfuncs->FreeHW(current_video, overlay);
		}
		free(overlay);
	}
}

