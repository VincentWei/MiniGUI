/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
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

