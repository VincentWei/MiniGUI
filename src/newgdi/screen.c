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
** screen.c: Screen operations of GDI
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

