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
** clip.c: Clipping operations of GDI.
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

void GUIAPI ExcludeClipRect (HDC hdc, const RECT* prc)
{
    PDC pdc;
    RECT rc;

    pdc = dc_HDC2PDC (hdc);

    rc = *prc;
    NormalizeRect (&rc);
    if (IsRectEmpty (&rc)) return;

    SubtractClipRect (&pdc->lcrgn, &rc);

    // Transfer logical to device to screen here.
    if (pdc->DevRC.left || pdc->DevRC.top) {
        coor_DP2SP (pdc, &rc.left, &rc.top);
        coor_DP2SP (pdc, &rc.right, &rc.bottom);
    }
    SubtractClipRect (&pdc->ecrgn, &rc);
}

static void clipOnChangeClipRegion (PDC pdc)
{
    if (dc_IsGeneralDC (pdc)) {
        /* for general DC, regenerate effective region. */
        LOCK_GCRINFO (pdc);
        dc_GenerateECRgn (pdc, TRUE);
        UNLOCK_GCRINFO (pdc);
    }
    else {
        dc_GenerateMemDCECRgn (pdc, TRUE);
    }
}

void GUIAPI IncludeClipRect (HDC hdc, const RECT* prc)
{
    PDC pdc;
    RECT rc;

    pdc = dc_HDC2PDC(hdc);

    rc = *prc;
    NormalizeRect(&rc);
    if (IsRectEmpty (&rc)) return;

    AddClipRect (&pdc->lcrgn, &rc);

    clipOnChangeClipRegion (pdc);
}

void GUIAPI ClipRectIntersect (HDC hdc, const RECT* prc)
{
    PDC pdc;
    RECT rc;

    pdc = dc_HDC2PDC(hdc);

    rc = *prc;
    NormalizeRect(&rc);
    if (IsRectEmpty (&rc)) return;

    IntersectClipRect (&pdc->lcrgn, &rc);

    // Transfer logical to device to screen here.
    if (pdc->DevRC.left || pdc->DevRC.top) {
        coor_DP2SP (pdc, &rc.left, &rc.top);
        coor_DP2SP (pdc, &rc.right, &rc.bottom);
    }
    IntersectClipRect (&pdc->ecrgn, &rc);
}

void GUIAPI SelectClipRect (HDC hdc, const RECT* prc)
{
    PDC pdc;
    RECT rc;

    pdc = dc_HDC2PDC(hdc);

    if (prc) {
        rc = *prc;
        NormalizeRect (&rc);
        if (IsRectEmpty (&rc))
            return;
    }
    else {
        rc = pdc->DevRC;
    }

#ifdef _REGION_DEBUG
    fprintf (stderr, "\n----------------------------\n");
    dbg_dumpRegion (&pdc->ecrgn);
#endif
    if (prc)
        SetClipRgn (&pdc->lcrgn, &rc);
    else
        MAKE_REGION_INFINITE(&pdc->lcrgn);

    clipOnChangeClipRegion(pdc);
#ifdef _REGION_DEBUG
    dbg_dumpRegion (&pdc->ecrgn);
    fprintf (stderr, "----------------------------\n");
#endif
}

void GUIAPI SelectClipRegion (HDC hdc, const CLIPRGN* pRgn)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);
    if (pdc == NULL ) {
        return;
    }

    if (pRgn == NULL) {
        MAKE_REGION_INFINITE(&pdc->lcrgn);
    }else{
        ClipRgnCopy (&pdc->lcrgn, pRgn);
    }
    clipOnChangeClipRegion (pdc);
}

int GUIAPI SelectClipRegionEx (HDC hdc, const CLIPRGN* pRgn, int fnMode)
{
    PDC pdc;
    BOOL retval = TRUE;

    pdc = dc_HDC2PDC (hdc);
    if (pdc == NULL || (fnMode != RGN_COPY && pRgn == NULL))
        return -1;

    switch (fnMode) {
    case RGN_DIFF:
        retval = SubtractRegion (&pdc->lcrgn, &pdc->lcrgn, pRgn);
        break;
    case RGN_AND:
        retval = ClipRgnIntersect (&pdc->lcrgn, &pdc->lcrgn, pRgn);
        break;
    case RGN_OR:
        retval = UnionRegion (&pdc->lcrgn, &pdc->lcrgn, pRgn);
        break;
    case RGN_XOR:
        retval = XorRegion (&pdc->lcrgn, &pdc->lcrgn, pRgn);
        break;
    default:
        if (pRgn == NULL) {
            MAKE_REGION_INFINITE(&pdc->lcrgn);
            retval = TRUE;
        }else{
            retval = ClipRgnCopy (&pdc->lcrgn, pRgn);
        }
        break;
    }

    if (!retval)
        return -1;

    clipOnChangeClipRegion (pdc);
    return pdc->lcrgn.type;
}

int GUIAPI OffsetClipRegion (HDC hdc, int off_x, int off_y)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);
    if (pdc == NULL)
        return -1;

    OffsetRegion (&pdc->lcrgn, off_x, off_y);
    clipOnChangeClipRegion (pdc);
    return pdc->lcrgn.type;
}

void GUIAPI GetBoundsRect (HDC hdc, RECT* pRect)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    if (pdc->lcrgn.type == NULLREGION)
        SetRectEmpty(pRect);
    else
        *pRect = pdc->lcrgn.rcBound;
}

BOOL GUIAPI PtVisible (HDC hdc, int x, int y)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);

    return PtInRegion (&pdc->lcrgn, x, y);
}

BOOL GUIAPI RectVisible (HDC hdc, const RECT* pRect)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);

    return RectInRegion (&pdc->lcrgn, pRect);
}

int GUIAPI GetClipBox (HDC hdc, RECT* clipbox)
{
    PDC pdc;
    RECT rc;

    pdc = dc_HDC2PDC (hdc);

    if (pdc == NULL || clipbox == NULL)
        return -1;

    if (pdc->lcrgn.type == NULLREGION)
        SetRectEmpty(clipbox);
    else
        *clipbox = pdc->lcrgn.rcBound;

    /* make the DevRC coordinate same as the lcrgn */
    SetRect(&rc, 0, 0, pdc->DevRC.right, pdc->DevRC.bottom);
    rc.right -= pdc->DevRC.left;
    rc.bottom -= pdc->DevRC.top;
    //IntersectRect(clipbox, clipbox, &pdc->DevRC);
    IntersectRect(clipbox, clipbox, &rc);

    return pdc->lcrgn.type;
}

int GUIAPI GetClipRegion (HDC hdc, CLIPRGN* cliprgn)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    if (pdc == NULL || cliprgn == NULL)
        return -1;

    if (!ClipRgnCopy (cliprgn, &pdc->lcrgn))
        return -1;

    return pdc->lcrgn.type;
}
