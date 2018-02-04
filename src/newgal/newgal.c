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
/*
** The New Graphics Abstract Layer of MiniGUI.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/10/07
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "misc.h"
#include "license.h"


GAL_Surface* __gal_screen;

#define LEN_ENGINE_NAME 10
#define LEN_MODE        20

BOOL GAL_ParseVideoMode (const char* mode, int* w, int* h, int* depth)
{
    const char* tmp;

    *w = atoi (mode);

    tmp = strchr (mode, 'x');
    if (tmp == NULL)
        return FALSE;
    *h = atoi (tmp + 1);

    tmp = strrchr (mode, '-');
    if (tmp == NULL)
        return FALSE;

    *depth = atoi (tmp + 1);

    return TRUE;
}

int mg_InitGAL (void)
{
    int i;
    int w, h, depth;
    char engine [LEN_ENGINE_NAME + 1];
    char mode [LEN_MODE + 1];

#if defined (WIN32) || !defined(__NOUNIX__)
	char* env_value;
#endif

    LICENSE_CHECK_CUSTIMER_ID ();

#ifndef __NOUNIX__
    if ((env_value = getenv ("MG_GAL_ENGINE"))) {
        strncpy (engine, env_value, LEN_ENGINE_NAME);
        engine [LEN_ENGINE_NAME] = '\0';
    }
    else
#endif
#ifndef _MG_MINIMALGDI
    if (GetMgEtcValue ("system", "gal_engine", engine, LEN_ENGINE_NAME) < 0) {
        return ERR_CONFIG_FILE;
    }
#else /* _MG_MINIMALGDI */
#   ifdef _MGGAL_PCXVFB
    strcpy(engine, "pc_xvfb");
#   else
    strcpy(engine, "dummy");
#   endif
#endif /* _MG_MINIMALGDI */
    
    if (GAL_VideoInit (engine, 0)) {
        GAL_VideoQuit ();
        fprintf (stderr, "NEWGAL: Does not find matched engine: %s.\n", engine);
        return ERR_NO_MATCH;
    }

#if defined (WIN32) || !defined(__NOUNIX__)
    if ((env_value = getenv ("MG_DEFAULTMODE"))) {
        strncpy (mode, env_value, LEN_MODE);
        mode [LEN_MODE] = '\0';
    }
    else
#endif
    if (GetMgEtcValue (engine, "defaultmode", mode, LEN_MODE) < 0)
        if (GetMgEtcValue ("system", "defaultmode", mode, LEN_MODE) < 0)
            return ERR_CONFIG_FILE;

    if (!GAL_ParseVideoMode (mode, &w, &h, &depth)) {
        GAL_VideoQuit ();
        fprintf (stderr, "NEWGAL: bad video mode parameter: %s.\n", mode);
        return ERR_CONFIG_FILE;
    }

    if (!(__gal_screen = GAL_SetVideoMode (w, h, depth, GAL_HWPALETTE))) {
        GAL_VideoQuit ();
        fprintf (stderr, "NEWGAL: Set video mode failure.\n");
        return ERR_GFX_ENGINE;
    }

#ifndef _MGRM_THREADS
    if (w != __gal_screen->w || h != __gal_screen->h) {
        fprintf (stderr, "The resolution specified in MiniGUI.cfg is not "
                        "the same as the actual resolution: %dx%d.\n" 
                        "This may confuse the clients. Please change it.\n", 
                         __gal_screen->w, __gal_screen->h);
        GAL_VideoQuit ();
        return ERR_GFX_ENGINE;
    }
#endif

    for (i = 0; i < 17; i++) {
        SysPixelIndex [i] = GAL_MapRGB (__gal_screen->format, 
                        SysPixelColor [i].r, 
                        SysPixelColor [i].g, 
                        SysPixelColor [i].b);
    }
    return 0;
}

