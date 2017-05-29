/*
** $Id: map.c 12533 2010-02-05 06:26:59Z houhuihua $
**
** map.c: Mapping operations of GDI.
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

/****************************** Mapping support ******************************/
void GUIAPI GetDCLCS (HDC hdc, int which, POINT* pt)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    if (which < NR_DC_LCS_PTS && which >= 0) {
        POINT* pts = &pdc->ViewOrig;

        *pt = pts [which];
    }
}

void GUIAPI SetDCLCS (HDC hdc, int which, const POINT* pt)
{
    PDC pdc;

    if (hdc == HDC_SCREEN || hdc == HDC_SCREEN_SYS)
        return;

    pdc = dc_HDC2PDC(hdc);
    if (which < NR_DC_LCS_PTS && which >= 0) {
        POINT* pts = &pdc->ViewOrig;

        pts [which] = *pt;
    }
}

