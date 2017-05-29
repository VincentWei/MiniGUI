/*
** $Id: screen.c 8944 2007-12-29 08:29:16Z xwyan $
**
** screen.c: Screen operations of GDI
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/06/12, derived from original gdi.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "cursor.h"

#ifdef _MGMISC_SAVESCREEN
BOOL GUIAPI SaveScreenRectContent (const RECT* rc, const char* filename)
{
    RECT rcWin;
    BITMAP bitmap;
    int save_ret;

    if (!IntersectRect (&rcWin, rc, &g_rcScr))
        return FALSE;

    bitmap.bmWidth = RECTW (rcWin);
    bitmap.bmHeight = RECTH (rcWin);

    if (bitmap.bmWidth == 0 || bitmap.bmHeight == 0) {
#ifdef _DEBUG
        fprintf (stderr, "SaveMainWindowContent: Empty Rect.\n");
#endif
        return FALSE;
    }
    
    bitmap.bmBits = NULL;
    GetBitmapFromDC (HDC_SCREEN, rcWin.left, rcWin.top,
                    RECTW (rcWin), RECTH (rcWin), &bitmap);

    if (!bitmap.bmBits) {
#ifdef _DEBUG
        fprintf (stderr, "SaveMainWindowContent: SaveBox error.\n");
#endif
        return FALSE;
    }
    
    save_ret = SaveBitmap (HDC_SCREEN, &bitmap, filename);
    free (bitmap.bmBits);
    return (save_ret == 0);
}

BOOL GUIAPI SaveMainWindowContent (HWND hWnd, const char* filename)
{
    RECT rcScreen;
    RECT rcWin;

    MG_CHECK_RET (MG_IS_WINDOW(hWnd), FALSE);

    SetRect (&rcScreen, 0, 0, WIDTHOFPHYGC, HEIGHTOFPHYGC);
    if (hWnd != HWND_DESKTOP) {
        GetWindowRect (hWnd, &rcWin);
        if (!IntersectRect (&rcWin, &rcWin, &rcScreen))
            return FALSE;
    }
    else
        rcWin = rcScreen;

    return SaveScreenRectContent (&rcWin, filename);
}
#endif /* _MGMISC_SAVESCREEN */

