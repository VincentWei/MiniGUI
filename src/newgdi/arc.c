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
** arc.c: drawing and filling arc, circle, and ellipse.
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

