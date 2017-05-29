/*
** $Id: winfb.c 8944 2007-12-29 08:29:16Z xwyan $
**  
** winfb.c: win32 virtual fb implementation and ipc operations
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
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
