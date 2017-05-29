/*
** $Id: yuv.c 7359 2007-08-16 05:08:40Z xgwang $
**
** yuv.c: YUV overlay support.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/11/06
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "pixel_ops.h"
#include "cursor.h"

/****************************** YUV overlay support **************************/
/* Create a YUV overlay */
GAL_Overlay* GUIAPI CreateYUVOverlay (int width, int height, Uint32 format, HDC hdc)
{
    PDC pdc = dc_HDC2PDC (hdc);

    return GAL_CreateYUVOverlay (width, height, format, pdc->surface);
}

/* Display a YUV overlay */
void GUIAPI DisplayYUVOverlay (GAL_Overlay* overlay, const RECT* dstrect)
{
    GAL_Rect dst;
    PDC pdc;
    RECT rcOutput = {0};

    rcOutput.right = WIDTHOFPHYGC;
    rcOutput.bottom = HEIGHTOFPHYGC;

    if (!(pdc = __mg_check_ecrgn (HDC_SCREEN)))
        return;

    if (dstrect) {
        rcOutput = *dstrect;
        dst.x = dstrect->left;
        dst.y = dstrect->top;
        dst.w = RECTWP(dstrect);
        dst.h = RECTHP(dstrect);
    }
    else {
        dst.x = 0;
        dst.y = 0;
        dst.w = WIDTHOFPHYGC;
        dst.h = HEIGHTOFPHYGC;
    }

    pdc->rc_output = rcOutput;
    ENTER_DRAWING (pdc);
    GAL_DisplayYUVOverlay (overlay, &dst);
    LEAVE_DRAWING (pdc);
    UNLOCK_GCRINFO (pdc);
}

