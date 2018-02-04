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

/* Gamma correction support */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef HAVE_MATH_H
#include <math.h>
#endif

#include "newgal.h"
#include "sysvideo.h"

#ifdef HAVE_MATH_H
static void CalculateGammaRamp(float gamma, Uint16 *ramp)
{
	int i;

	/* 0.0 gamma is all black */
	if ( gamma <= 0.0 ) {
		for ( i=0; i<256; ++i ) {
			ramp[i] = 0;
		}
		return;
	} else
	/* 1.0 gamma is identity */
	if ( gamma == 1.0 ) {
		for ( i=0; i<256; ++i ) {
			ramp[i] = (i << 8) | i;
		}
		return;
	} else
	/* Calculate a real gamma ramp */
	{ int value;
		gamma = 1.0f / gamma;
		for ( i=0; i<256; ++i ) {
			value = (int)(pow((double)i/256.0, gamma)*65535.0+0.5);
			if ( value > 65535 ) {
				value = 65535;
			}
			ramp[i] = (Uint16)value;
		}
	}
}
static void CalculateGammaFromRamp(float *gamma, Uint16 *ramp)
{
	/* The following is adapted from a post by Garrett Bass on OpenGL
	   Gamedev list, March 4, 2000.
	 */
	float sum = 0.0;
	int i, count = 0;

	*gamma = 1.0;
	for ( i = 1; i < 256; ++i ) {
	    if ( (ramp[i] != 0) && (ramp[i] != 65535) ) {
	        double B = (double)i / 256.0;
	        double A = ramp[i] / 65535.0;
	        sum += (float) ( log(A) / log(B) );
	        count++;
	    }
	}
	if ( count && sum ) {
		*gamma = 1.0f / (sum / count);
	}
}
#endif /* HAVE_MATH_H */

int GAL_SetGamma(float red, float green, float blue)
{
	int succeeded;
	GAL_VideoDevice *video = current_video;
	GAL_VideoDevice *this  = current_video;	

	succeeded = -1;
#ifdef HAVE_MATH_H
	/* Prefer using SetGammaRamp(), as it's more flexible */
	{
		Uint16 ramp[3][256];

		CalculateGammaRamp(red, ramp[0]);
		CalculateGammaRamp(green, ramp[1]);
		CalculateGammaRamp(blue, ramp[2]);
		succeeded = GAL_SetGammaRamp(ramp[0], ramp[1], ramp[2]);
	}
#else
	GAL_SetError("NEWGAL: Gamma correction not supported.\n");
#endif
	if ( (succeeded < 0) && video->SetGamma ) {
		GAL_ClearError();
		succeeded = video->SetGamma(this, red, green, blue);
	}
	return succeeded;
}

/* Calculating the gamma by integrating the gamma ramps isn't exact,
   so this function isn't officially supported.
*/
int GAL_GetGamma(float *red, float *green, float *blue)
{
	int succeeded;
	GAL_VideoDevice *video = current_video;
	GAL_VideoDevice *this  = current_video;	

	succeeded = -1;
#ifdef HAVE_MATH_H
	/* Prefer using GetGammaRamp(), as it's more flexible */
	{
		Uint16 ramp[3][256];

		succeeded = GAL_GetGammaRamp(ramp[0], ramp[1], ramp[2]);
		if ( succeeded >= 0 ) {
			CalculateGammaFromRamp(red, ramp[0]);
			CalculateGammaFromRamp(green, ramp[1]);
			CalculateGammaFromRamp(blue, ramp[2]);
		}
	}
#else
	GAL_SetError("NEWGAL: Gamma correction not supported.\n");
#endif
	if ( (succeeded < 0) && video->GetGamma ) {
		GAL_ClearError();
		succeeded = video->GetGamma(this, red, green, blue);
	}
	return succeeded;
}

int GAL_SetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue)
{
	int succeeded;
	GAL_VideoDevice *video = current_video;
	GAL_VideoDevice *this  = current_video;	
	GAL_Surface *screen = GAL_PublicSurface;

	/* Verify the screen parameter */
	if ( !screen ) {
		GAL_SetError("NEWGAL: No video mode has been set.\n");
		return -1;
	}

	/* Lazily allocate the gamma tables */
	if ( ! video->gamma ) {
		GAL_GetGammaRamp(0, 0, 0);
	}

	/* Fill the gamma table with the new values */
	if ( red ) {
		memcpy(&video->gamma[0*256], red, 256*sizeof(*video->gamma));
	}
	if ( green ) {
		memcpy(&video->gamma[1*256], green, 256*sizeof(*video->gamma));
	}
	if ( blue ) {
		memcpy(&video->gamma[2*256], blue, 256*sizeof(*video->gamma));
	}

	/* Gamma correction always possible on split palettes */
	if ( (screen->flags & GAL_HWPALETTE) == GAL_HWPALETTE ) {
		GAL_Palette *pal = screen->format->palette;

		/* If physical palette has been set independently, use it */
		if(video->physpal)
		        pal = video->physpal;
		      
		GAL_SetPalette(screen, GAL_PHYSPAL,
			       pal->colors, 0, pal->ncolors);
		return 0;
	}

	/* Try to set the gamma ramp in the driver */
	succeeded = -1;
	if ( video->SetGammaRamp ) {
		succeeded = video->SetGammaRamp(this, video->gamma);
	} else {
		GAL_SetError("NEWGAL: Gamma ramp manipulation not supported.\n");
	}
	return succeeded;
}

int GAL_GetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue)
{
	GAL_VideoDevice *video = current_video;
	GAL_VideoDevice *this  = current_video;	

	/* Lazily allocate the gamma table */
	if ( ! video->gamma ) {
		video->gamma = malloc(3*256*sizeof(*video->gamma));
		if ( ! video->gamma ) {
			GAL_OutOfMemory();
			return -1;
		}
		if ( video->GetGammaRamp ) {
			/* Get the real hardware gamma */
			video->GetGammaRamp(this, video->gamma);
		} else {
			/* Assume an identity gamma */
			int i;
			for ( i=0; i<256; ++i ) {
				video->gamma[0*256+i] = (i << 8) | i;
				video->gamma[1*256+i] = (i << 8) | i;
				video->gamma[2*256+i] = (i << 8) | i;
			}
		}
	}

	/* Just copy from our internal table */
	if ( red ) {
		memcpy(red, &video->gamma[0*256], 256*sizeof(*red));
	}
	if ( green ) {
		memcpy(green, &video->gamma[1*256], 256*sizeof(*green));
	}
	if ( blue ) {
		memcpy(blue, &video->gamma[2*256], 256*sizeof(*blue));
	}
	return 0;
}

