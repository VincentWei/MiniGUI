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
** advapi.c: advanced 2D APIs.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2003/12/24
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGHAVE_ADV_2DAPI

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

#include "mi.h"

/************************ Pen and brush attributes ***************************/

void GUIAPI SetPenDashes (HDC hdc, int dash_offset, const unsigned char* dash_list, int n)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);
    pdc->dash_offset = dash_offset;
    pdc->dash_list = dash_list;
    pdc->dash_list_len = n;
}

void GUIAPI SetBrushInfo (HDC hdc, const BITMAP* tile, const STIPPLE* stipple)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    pdc->brush_tile = tile;
    pdc->brush_stipple = stipple;
}

void GUIAPI SetBrushOrigin (HDC hdc, int x, int y)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    pdc->brush_orig.x = x;
    pdc->brush_orig.y = y;
}

void GUIAPI LineEx (HDC hdc, int x1, int y1, int x2, int y2)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    if (pdc->pen_type == PT_SOLID && pdc->pen_width == 0) {
        MoveTo (hdc, x1, y1);
        LineTo (hdc, x2, y2);
    }
    else {
        POINT pts[2];

        if (!(pdc = __mg_check_ecrgn (hdc)))
            return;

        coor_LP2SP (pdc, &x1, &y1);
        pts [0].x = x1; pts[0].y = y1;
        coor_LP2SP (pdc, &x2, &y2);
        pts [1].x = x2; pts[1].y = y2;

        pdc->rc_output.left = MIN (x1, x2) - pdc->pen_width;
        pdc->rc_output.top  = MIN (y1, y2) - pdc->pen_width;
        pdc->rc_output.right = MAX (x1, x2) + 1 + pdc->pen_width;
        pdc->rc_output.bottom = MAX (y1, y2) + 1 + pdc->pen_width;

        pdc->cur_ban = NULL;
        ENTER_DRAWING (pdc);

        if (pdc->pen_width == 0 && pdc->pen_type != PT_SOLID) {
            pdc->pen_width = 1;
            miWideDash (pdc, 2, pts);
            pdc->pen_width = 0;
        }
        else {
            if (pdc->pen_type != PT_SOLID)
                miWideDash (pdc, 2, pts);
            else
                miWideLine (pdc, 2, pts);
        }

        LEAVE_DRAWING (pdc);

        UNLOCK_GCRINFO (pdc);
    }
}

static void pt_lp2sp_helper (PDC pdc, const POINT* src, POINT* dst, int nr, RECT* bound)
{
    int i;

    for (i = 0; i < nr; i++) {
        dst[i] = src[i];
        coor_LP2SP (pdc, &dst[i].x, &dst[i].y);

        if (i == 0) {
            bound->left = dst[i].x;
            bound->top  = dst[i].y;
            bound->right = dst[i].x;
            bound->bottom = dst[i].y;
        }
        else {
            if (dst[i].x < bound->left)
                bound->left = dst[i].x;
            if (dst[i].y < bound->top)
                bound->top  = dst[i].y;
            if (dst[i].x > bound->right)
                bound->right = dst[i].x;
            if (dst[i].y > bound->bottom)
                bound->bottom = dst[i].y;
        }
    }

    bound->left -= pdc->pen_width;
    bound->top -= pdc->pen_width;
    bound->right += pdc->pen_width + 1;
    bound->bottom += pdc->pen_width + 1;
    if (pdc->pen_join_style == PT_JOIN_MITER) {
        InflateRect (bound, pdc->pen_width >> 3, pdc->pen_width >> 3);
    }
}

void GUIAPI PolyLineEx (HDC hdc, const POINT* pts, int nr_pts)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    if (nr_pts < 2)
        return;

    if (pdc->pen_type == PT_SOLID && pdc->pen_width == 0) {
        PolyLineTo (hdc, pts, nr_pts);
    }
    else {
        POINT* my_pts;

        if (!(pdc = __mg_check_ecrgn (hdc)))
            return;

        my_pts = ALLOCATE_LOCAL (sizeof (POINT) * nr_pts);
        if (my_pts == NULL)
            return;

        pt_lp2sp_helper (pdc, pts, my_pts, nr_pts, &pdc->rc_output);

        pdc->cur_ban = NULL;

        ENTER_DRAWING (pdc);

        if (pdc->pen_width == 0 && pdc->pen_type != PT_SOLID) {
            pdc->pen_width = 1;
            miWideDash (pdc, nr_pts, my_pts);
            pdc->pen_width = 0;
        }
        else {
            if (pdc->pen_type != PT_SOLID)
                miWideDash (pdc, nr_pts, my_pts);
            else
                miWideLine (pdc, nr_pts, my_pts);
        }

        LEAVE_DRAWING (pdc);

        DEALLOCATE_LOCAL (my_pts);

        UNLOCK_GCRINFO (pdc);
    }
}

void GUIAPI ArcEx (HDC hdc, int x, int y, int width, int height, int ang1, int ang2)
{
    PDC pdc;
    ARC arc;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    coor_LP2SP (pdc, &x, &y);
    
    pdc->rc_output.left = x - pdc->pen_width;
    pdc->rc_output.top  = y - pdc->pen_width;
    pdc->rc_output.right = x + width + pdc->pen_width + 1;
    pdc->rc_output.bottom = y + height + pdc->pen_width + 1;

    pdc->cur_ban = NULL;

    ENTER_DRAWING (pdc);

    arc.x = x;
    arc.y = y;
    arc.width = width;
    arc.height = height;
    arc.angle1 = ang1;
    arc.angle2 = ang2;

    miPolyArc (pdc, 1, &arc);

    LEAVE_DRAWING (pdc);

    UNLOCK_GCRINFO (pdc);
}

static void arc_lp2sp_helper (PDC pdc, const ARC* src, ARC* dst, int nr, RECT* bound)
{
    int i;

    for (i = 0; i < nr; i++) {
        dst[i] = src[i];
        coor_LP2SP (pdc, &dst[i].x, &dst[i].y);

        if (i == 0) {
            bound->left = dst[i].x;
            bound->top  = dst[i].y;
            bound->right = dst[i].x + dst[i].width;
            bound->bottom = dst[i].y + dst[i].height;
        }
        else {
            int tmp;

            tmp = dst[i].x;
            if (tmp < bound->left)
                bound->left = tmp;

            tmp = dst[i].y;
            if (tmp < bound->top)
                bound->top  = tmp;

            tmp = dst[i].x + dst[i].width;
            if (tmp > bound->right)
                bound->right = tmp;

            tmp = dst[i].y + dst[i].height;
            if (tmp > bound->bottom)
                bound->bottom = tmp;
        }
    }

    bound->left -= pdc->pen_width;
    bound->top -= pdc->pen_width;
    bound->right += pdc->pen_width + 1;
    bound->bottom += pdc->pen_width + 1;
    if (pdc->pen_join_style == PT_JOIN_MITER) {
        InflateRect (bound, pdc->pen_width >> 3, pdc->pen_width >> 3);
    }
}

void GUIAPI PolyArcEx (HDC hdc, const ARC* arcs, int nr_arcs)
{
    PDC pdc;
    ARC* my_arcs;

    if (nr_arcs < 1)
        return;

    my_arcs = ALLOCATE_LOCAL (sizeof (ARC) * nr_arcs);
    if (my_arcs == NULL)
        return;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    arc_lp2sp_helper (pdc, arcs, my_arcs, nr_arcs, &pdc->rc_output);

    pdc->cur_ban = NULL;

    ENTER_DRAWING (pdc);

    miPolyArc (pdc, nr_arcs, my_arcs);

    LEAVE_DRAWING (pdc);

    DEALLOCATE_LOCAL (my_arcs);

    UNLOCK_GCRINFO (pdc);
}

void GUIAPI FillArcEx (HDC hdc, int x, int y, int width, int height, int ang1, int ang2)
{
    PDC pdc;
    ARC arc;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    coor_LP2SP (pdc, &x, &y);
    
    arc.x = x;
    arc.y = y;
    arc.width = width;
    arc.height = height;
    arc.angle1 = ang1;
    arc.angle2 = ang2;

    pdc->rc_output.left = x;
    pdc->rc_output.top  = y;
    pdc->rc_output.right = x + width + 1;
    pdc->rc_output.bottom = y + height + 1;

    pdc->cur_ban = NULL;

    ENTER_DRAWING (pdc);

    miPolyFillArc (pdc, 1, &arc);

    LEAVE_DRAWING (pdc);

    UNLOCK_GCRINFO (pdc);
}

void GUIAPI PolyFillArcEx (HDC hdc, const ARC* arcs, int nr_arcs)
{
    PDC pdc;
    ARC* my_arcs;

    if (nr_arcs < 1)
        return;

    my_arcs = ALLOCATE_LOCAL (sizeof (ARC) * nr_arcs);
    if (my_arcs == NULL)
        return;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    arc_lp2sp_helper (pdc, arcs, my_arcs, nr_arcs, &pdc->rc_output);

    pdc->cur_ban = NULL;

    ENTER_DRAWING (pdc);

    miPolyFillArc (pdc, nr_arcs, my_arcs);

    LEAVE_DRAWING (pdc);

    DEALLOCATE_LOCAL (my_arcs);

    UNLOCK_GCRINFO (pdc);
}

BOOL GUIAPI RoundRect (HDC hdc, int x0, int y0, int x1, int y1, int rw, int rh)
{
    int cw, ch, a;

    if (x0 >= x1 || y0 >= y1)
        return FALSE;
    
    cw = rw*2;
    ch = rh*2;
   
    if (cw > (x1-x0) || ch > (y1-y0) || cw < 0 || ch < 0)
        return FALSE;
    
    if (rw == 0 || rh == 0) {
        POINT pts [5];
        pts[0].x = x0; pts[0].y = y0;
        pts[1].x = x1; pts[1].y = y0;
        pts[2].x = x1; pts[2].y = y1;
        pts[3].x = x0; pts[3].y = y1;
        pts[4].x = x0; pts[4].y = y0;

        FillBox (hdc, x0, y0, (x1-x0), (y1-y0));
        PolyLineEx(hdc, pts, 5);
        return TRUE;
    }

    a = 90*64;

    FillArcEx (hdc, x0, y0, cw, ch, a, a);
    FillArcEx (hdc, x1-cw, y0, cw, ch, 0, a);
    FillArcEx (hdc, x1-cw, y1-ch, cw, ch, -a, a);
    FillArcEx (hdc, x0, y1-ch, cw, ch, -a, -a);
    
    FillBox (hdc,x0 + rw, y0, (x1-x0)-cw, rh);
    FillBox (hdc, x0, y0 + rh, x1-x0, (y1-y0)-ch);
    FillBox (hdc,x0 + rw, y1-rh, (x1-x0)-cw, rh);

    ArcEx (hdc, x0, y0, cw, ch, a, a);
    ArcEx (hdc, x1-cw, y0, cw, ch, 0, a);
    ArcEx (hdc, x1-cw, y1-ch, cw, ch, -a, a);
    ArcEx (hdc, x0, y1-ch, cw, ch, -a, -a);

    LineEx (hdc, x0 + rw, y0, x1 - rw, y0);
    LineEx (hdc, x0 + rw, y1, x1 - rw, y1);
    LineEx (hdc, x0, y0 + rh, x0, y1 - rh);
    LineEx (hdc, x1, y0 + rh, x1, y1 - rh);

    return TRUE;
}

#endif /* _MGHAVE_ADV_2DAPI */

