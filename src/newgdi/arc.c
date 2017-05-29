/*
** $Id: arc.c 8944 2007-12-29 08:29:16Z xwyan $
**
** arc.c: drawing and filling arc, circle, and ellipse.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/10/12, derived from original draw.c
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

#define _USE_GENERATOR  1

/************************ Circle and Ellipse *******************************/

#ifdef _HAVE_MATH_LIB

void GUIAPI CircleArc (HDC hdc, int sx, int sy, int r, int ang1, int ang2)
{
    PDC pdc;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    coor_LP2SP (pdc, &sx, &sy);
    pdc->cur_pixel = pdc->pencolor;
    pdc->cur_ban = NULL;

    if (r < 1) {
        _set_pixel_helper (pdc, sx, sy);
        goto ret;
    }

    pdc->rc_output.left = sx - r;
    pdc->rc_output.top  = sy - r;
    pdc->rc_output.right = sx + r + 1;
    pdc->rc_output.bottom = sy + r + 1;

    ENTER_DRAWING (pdc);

    CircleArcGenerator (pdc, sx, sy, r, ang1, ang2, _dc_set_pixel_clip);

    LEAVE_DRAWING (pdc);

ret:
    UNLOCK_GCRINFO (pdc);
}

#endif /* _HAVE_MATH_LIB */

void GUIAPI Ellipse (HDC hdc, int sx, int sy, int rx, int ry)
{
    PDC pdc;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    coor_LP2SP (pdc, &sx, &sy);

    pdc->cur_pixel = pdc->pencolor;
    pdc->cur_ban = NULL;

    if (rx < 1 || ry < 1) {
        _set_pixel_helper (pdc, sx, sy);
        goto ret;
    }

    pdc->rc_output.left = sx - rx;
    pdc->rc_output.top  = sy - ry;
    pdc->rc_output.right = sx + rx + 1;
    pdc->rc_output.bottom = sy + ry + 1;

    ENTER_DRAWING (pdc);

    EllipseGenerator (pdc, sx, sy, rx, ry, _dc_set_pixel_pair_clip);

    LEAVE_DRAWING (pdc);

ret:
    UNLOCK_GCRINFO (pdc);
}

void GUIAPI FillEllipse (HDC hdc, int sx, int sy, int rx, int ry)
{
    PDC pdc;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    coor_LP2SP (pdc, &sx, &sy);
    pdc->cur_pixel = pdc->brushcolor;
    pdc->cur_ban = NULL;

    if (rx < 1 || ry < 1) {
        _set_pixel_helper (pdc, sx, sy);
        goto ret;
    }

    pdc->rc_output.left = sx - rx;
    pdc->rc_output.top  = sy - ry;
    pdc->rc_output.right = sx + rx + 1;
    pdc->rc_output.bottom = sy + ry + 1;

    ENTER_DRAWING (pdc);

#ifdef _MGHAVE_ADV_2DAPI
    if (pdc->brush_type == BT_SOLID)
        EllipseGenerator (pdc, sx, sy, rx, ry, _dc_draw_hline_clip);
    else
        EllipseGenerator (pdc, sx, sy, rx, ry, _dc_fill_hline_clip);
#else
    EllipseGenerator (pdc, sx, sy, rx, ry, _dc_draw_hline_clip);
#endif

    LEAVE_DRAWING (pdc);

ret:
    UNLOCK_GCRINFO (pdc);
}

void GUIAPI Circle (HDC hdc, int sx, int sy, int r)
{
    PDC pdc;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    coor_LP2SP (pdc, &sx, &sy);

    pdc->cur_pixel = pdc->pencolor;
    pdc->cur_ban = NULL;

    if (r < 1) {
        _set_pixel_helper (pdc, sx, sy);
        goto ret;
    }

    pdc->rc_output.left = sx - r;
    pdc->rc_output.top  = sy - r;
    pdc->rc_output.right = sx + r + 1;
    pdc->rc_output.bottom = sy + r + 1;

    ENTER_DRAWING (pdc);

    CircleGenerator (pdc, sx, sy, r, _dc_set_pixel_pair_clip);

    LEAVE_DRAWING (pdc);

ret:
    UNLOCK_GCRINFO (pdc);
}

void GUIAPI FillCircle (HDC hdc, int sx, int sy, int r)
{
    PDC pdc;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    coor_LP2SP (pdc, &sx, &sy);

    pdc->cur_pixel = pdc->brushcolor;
    pdc->cur_ban = NULL;

    if (r < 1) {
        _set_pixel_helper (pdc, sx, sy);
        goto ret;
    }

    pdc->rc_output.left = sx - r;
    pdc->rc_output.top  = sy - r;
    pdc->rc_output.right = sx + r + 1;
    pdc->rc_output.bottom = sy + r + 1;

    ENTER_DRAWING (pdc);

#ifdef _MGHAVE_ADV_2DAPI
    if (pdc->brush_type == BT_SOLID)
        CircleGenerator (pdc, sx, sy, r, _dc_draw_hline_clip);
    else
        CircleGenerator (pdc, sx, sy, r, _dc_fill_hline_clip);
#else
    CircleGenerator (pdc, sx, sy, r, _dc_draw_hline_clip);
#endif

    LEAVE_DRAWING (pdc);

ret:
    UNLOCK_GCRINFO (pdc);
}

