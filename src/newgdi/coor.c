/*
** $Id: coor.c 12533 2010-02-05 06:26:59Z houhuihua $
**
** Coordinates operations of GDI.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/06/12, derived from original gdi.c
*/

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"

/************************ Coordinate system transfer *************************/
void GUIAPI SPtoLP(HDC hdc, POINT* pPt)
{
    /* DK[01/14/10]: Add condition to process case of hdc equal to 
     * HDC_SCREEN_SYS for fix bug 4348. */
    if (HDC_SCREEN == hdc || HDC_SCREEN_SYS == hdc) return;

    coor_SP2LP ((PDC)hdc, &pPt->x, &pPt->y);
}

void GUIAPI LPtoSP(HDC hdc, POINT* pPt)
{
    /* DK[01/14/10]: Add condition to process case of hdc equal to 
     * HDC_SCREEN_SYS for fix bug 4348. */
    if (HDC_SCREEN == hdc || HDC_SCREEN_SYS == hdc) return;

    coor_LP2SP ((PDC)hdc, &pPt->x, &pPt->y);
}

void GUIAPI LPtoDP(HDC hdc, POINT* pPt)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);

    if (pdc->mapmode != MM_TEXT) {
        pPt->x = (pPt->x - pdc->WindowOrig.x)
             * pdc->ViewExtent.x / pdc->WindowExtent.x
             + pdc->ViewOrig.x;

        pPt->y = (pPt->y - pdc->WindowOrig.y)
             * pdc->ViewExtent.y / pdc->WindowExtent.y
             + pdc->ViewOrig.y;
    }
}

void GUIAPI DPtoLP (HDC hdc, POINT* pPt)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    if (pdc->mapmode != MM_TEXT) {
        pPt->x = (pPt->x - pdc->ViewOrig.x)
             * pdc->WindowExtent.x / pdc->ViewExtent.x
             + pdc->WindowOrig.x;

        pPt->y = (pPt->y - pdc->ViewOrig.y)
             * pdc->WindowExtent.y / pdc->ViewExtent.y
             + pdc->WindowOrig.y;
    }
}

