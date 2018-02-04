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
** winfb.c: win32 virtual fb implementation and ipc operations
*/

#include "mgconfig.h"

#ifdef _MGGAL_WVFB

#include <stdio.h>
#include <stdlib.h>
#include "windows.h"

struct WVFbHeader
{
    int width;
    int height;
    int depth;
    int linestep;
    int dataoffset;
    RECT update;
    BYTE dirty;
    int  numcols;
    unsigned int clut[256];
};

static HANDLE hMutex;
static HANDLE hScreen;
static LPVOID lpScreen;

/* decide if win fb is available */
int win_FbAvailable (void)
{
    hMutex = OpenMutex (MUTEX_MODIFY_STATE, FALSE, "WVFBScreenObject");
    if (hMutex == 0 || hMutex == (HANDLE)ERROR_FILE_NOT_FOUND) {
        fprintf (stderr, "WVFB is not available!\n");
        return 0;
    }

    return 1;
}

/* Shared map file intialize */
void* win_FbInit (int w, int h, int depth)
{
    hScreen = OpenFileMapping (FILE_MAP_ALL_ACCESS, FALSE, "WVFBScreenMap");
    if (hScreen == NULL)
    { 
        fprintf (stderr, "WVFB NEWGAL Engine: Could not open file mapping object WVFBScreenMap."); 
        return NULL;
    } 

    lpScreen = MapViewOfFile (hScreen, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (lpScreen == NULL) 
    { 
        fprintf(stderr, "WVFB NEWGAL Engine: Could not map view of file.");
        return NULL;
    }
    
    return (void*)lpScreen;
}

/* close map file */
void win_FbClose (void)
{
    UnmapViewOfFile (lpScreen);
    CloseHandle (hScreen);
    CloseHandle (hMutex);
}

int win_FbUpdate ()
{
    struct WVFbHeader *hdr;
    hdr = (struct WVFbHeader *)lpScreen;
    return 0;
}

int win_SetVideoMode (int width, int height, int bpp)
{
    return 0;
}

void win_FbLock (void)
{
    WaitForSingleObject (hMutex, INFINITE);
}

void win_FbUnlock (void)
{
    ReleaseMutex (hMutex);
}

#endif /* _MGGAL_WVFB */
