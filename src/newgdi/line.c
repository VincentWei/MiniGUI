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
** line.c: drawing of line, rectangle, and spline.
**
**      Spline drawing algorithm comes from Allegro by 
**      Shawn Hargreaves and others.
**      So thank them for their great work and good copyright statement.
**
**      "Allegro is a gift-software"
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
#include "fixedmath.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "memops.h"
#include "pixel_ops.h"
#include "cursor.h"

void GUIAPI MoveTo (HDC hdc, int x, int y)
{
    PDC pdc;
    pdc = dc_HDC2PDC(hdc);

    pdc->CurPenPos.x = x;
    pdc->CurPenPos.y = y;
}

void GUIAPI LineTo (HDC hdc, int x, int y)
{
    PCLIPRECT cliprect;
    PDC pdc;
    int startx, starty;

    pdc = dc_HDC2PDC(hdc);

    startx = pdc->CurPenPos.x;
    starty = pdc->CurPenPos.y;

    /* Move the current pen pos. */
    pdc->CurPenPos.x = x;
    pdc->CurPenPos.y = y;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    /* Transfer logical to device to screen here. */
    coor_LP2SP(pdc, &x, &y);
    coor_LP2SP(pdc, &startx, &starty);

    pdc->cur_pixel = pdc->pencolor;
    pdc->cur_ban = NULL;
    pdc->step = 1;
    if (startx == x && starty == y) {
        _set_pixel_helper (pdc, x, y);
        goto ret;
    }

    SetRect (&pdc->rc_output, startx, starty, x, y);
    NormalizeRect (&pdc->rc_output);
    InflateRect (&pdc->rc_output, 1, 1);

    ENTER_DRAWING (pdc);

    if (starty == y) { /* a horizontal line */
        _dc_draw_hline_clip (pdc, startx, x, y);
    }
    else if (startx == x) { /* a vertical line */
        _dc_draw_vline_clip (pdc, starty, y, x);
    }
    else {
        RECT eff_rc;

        cliprect = pdc->ecrgn.head;
        while (cliprect) {
            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)) {
                int x0 = startx, y0 = starty, x1 = x, y1 = y; 
                if (LineClipper (&eff_rc, &x0, &y0, &x1, &y1)) {
                    pdc->move_to (pdc, x0, y0);
                    LineGenerator (pdc, x0, y0, x1, y1, _dc_set_pixel_noclip);
                }
            }

            cliprect = cliprect->next;
        }
    }

    LEAVE_DRAWING (pdc);

ret:
    UNLOCK_GCRINFO (pdc);
}

void GUIAPI PolyLineTo (HDC hdc, const POINT* point, int poinum)
{
    int i;

    MoveTo (hdc, point[0].x, point[0].y);
    for (i = 1; i < poinum; i++)
        LineTo (hdc, point[i].x, point[i].y);
}

void GUIAPI Rectangle (HDC hdc, int x0, int y0, int x1, int y1)
{
    PDC pdc;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    /* Transfer logical to device to screen here. */
    coor_LP2SP (pdc, &x0, &y0); 
    coor_LP2SP (pdc, &x1, &y1); 
    SetRect (&pdc->rc_output, x0, y0, x1, y1);
    NormalizeRect (&pdc->rc_output);
    x0 = pdc->rc_output.left; y0 = pdc->rc_output.top;
    x1 = pdc->rc_output.right; y1 = pdc->rc_output.bottom;
    pdc->rc_output.right ++;
    pdc->rc_output.bottom ++;

    pdc->cur_pixel = pdc->pencolor;
    pdc->cur_ban = NULL;
    pdc->step = 1;

    if (x0 == x1 && y0 == y1) {
        _set_pixel_helper (pdc, x0, y0);
        goto ret;
    }

    ENTER_DRAWING (pdc);

    _dc_draw_hline_clip (pdc, x0, x1, y0);
    if (y0 != y1)
        _dc_draw_hline_clip (pdc, x0, x1, y1);
    _dc_draw_vline_clip (pdc, y0, y1, x0);
    if (x0 != x1)
        _dc_draw_vline_clip (pdc, y0, y1, x1);

    LEAVE_DRAWING (pdc);

ret:
    UNLOCK_GCRINFO (pdc);
}

void GUIAPI FocusRect (HDC hdc, int x0, int y0, int x1, int y1)
{
    PDC pdc;
    int oldrop;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    /* Transfer logical to device to screen here. */
    coor_LP2SP (pdc, &x0, &y0); 
    coor_LP2SP (pdc, &x1, &y1); 
    SetRect (&pdc->rc_output, x0, y0, x1, y1);
    NormalizeRect (&pdc->rc_output);
    x0 = pdc->rc_output.left; y0 = pdc->rc_output.top;
    x1 = pdc->rc_output.right; y1 = pdc->rc_output.bottom;
    pdc->rc_output.right ++;
    pdc->rc_output.bottom ++;

    pdc->cur_pixel = pdc->pencolor;
    pdc->cur_ban = NULL;
    pdc->step = 2;
    oldrop = SetRasterOperation (hdc, ROP_XOR);

    if (x0 == x1 && y0 == y1) {
        _set_pixel_helper (pdc, x0, y0);
        goto ret;
    }

    ENTER_DRAWING (pdc);

    _dc_draw_hline_clip (pdc, x0, x1, y0);
    if (y0 != y1)
        _dc_draw_hline_clip (pdc, x0, x1, y1);

    _dc_draw_vline_clip (pdc, y0, y1, x0);
    if (x0 != x1)
        _dc_draw_vline_clip (pdc, y0, y1, x1);

    LEAVE_DRAWING (pdc);
    SetRasterOperation (hdc, oldrop);

ret:
    UNLOCK_GCRINFO (pdc);
}

void GUIAPI DrawHVDotLine (HDC hdc, int x, int y, int w_h, BOOL H_V)
{
    PDC pdc;

    if (w_h < 1)
        return;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return;

    coor_LP2SP (pdc, &x, &y); 

    if (w_h == 1) {
        if ((H_V && !(x % 2)) || (!H_V && !(y % 2))) {
            _set_pixel_helper (pdc, x, y);
        }
        goto ret;
    }

    if (H_V) {
        SetRect (&pdc->rc_output, x, y, x + w_h /*+ 1*/, y + 2);
    }
    else {
        SetRect (&pdc->rc_output, x, y, x + 2, y + w_h + 1);
    }

    pdc->cur_pixel = pdc->pencolor;
    pdc->cur_ban = NULL;
    pdc->step = 2;

    ENTER_DRAWING (pdc);

    if (H_V)
        _dc_draw_hline_clip (pdc, x, x + w_h, y);
    else
        _dc_draw_vline_clip (pdc, y, y + w_h, x);

    LEAVE_DRAWING (pdc);

ret:
    UNLOCK_GCRINFO (pdc);
}

void calc_spline (const POINT* points, int npts, POINT* out_pts)
{
   /* Derivatives of x(t) and y(t). */
   double x, dx, ddx, dddx;
   double y, dy, ddy, dddy;
   int i;

   /* Temp variables used in the setup. */
   double dt, dt2, dt3;
   double xdt2_term, xdt3_term;
   double ydt2_term, ydt3_term;

   dt = 1.0 / (npts-1);
   dt2 = (dt * dt);
   dt3 = (dt2 * dt);

   /* x coordinates. */
   xdt2_term = 3 * (points[2].x - 2*points[1].x + points[0].x);
   xdt3_term = points[3].x + 3 * (-points[2].x + points[1].x) - points[0].x;

   xdt2_term = dt2 * xdt2_term;
   xdt3_term = dt3 * xdt3_term;

   dddx = 6*xdt3_term;
   ddx = -6*xdt3_term + 2*xdt2_term;
   dx = xdt3_term - xdt2_term + 3 * dt * (points[1].x - points[0].x);
   x = points[0].x;

   out_pts[0].x = points[0].x;

   x += .5;
   for(i=1; i<npts; i++){
      ddx += dddx;
      dx += ddx;
      x += dx;

      out_pts[i].x = (int)x;
   }

   /* y coordinates. */
   ydt2_term = 3 * (points[2].y - 2*points[1].y + points[0].y);
   ydt3_term = points[3].y + 3 * (-points[2].y + points[1].y) - points[0].y;

   ydt2_term = dt2 * ydt2_term;
   ydt3_term = dt3 * ydt3_term;

   dddy = 6*ydt3_term;
   ddy = -6*ydt3_term + 2*ydt2_term;
   dy = ydt3_term - ydt2_term + dt * 3 * (points[1].y - points[0].y);
   y = points[0].y;

   out_pts[0].y = points[0].y;

   y += .5;

   for(i=1; i<npts; i++){
      ddy += dddy;
      dy += ddy;
      y += dy;

      out_pts[i].y = (int)y;
   }
}

#define MAX_POINTS   64
/* 
 * SplineTo: Draws a bezier spline.
 */
void GUIAPI SplineTo (HDC hdc, const POINT* points)
{
#ifndef _MGHAVE_ADV_2DAPI
   int i;
   gal_pixel pen_color = GetPenColor (hdc);
#endif
   int num_points;
   POINT pts[MAX_POINTS];
   double tmp;

   /* Calculate the number of points to draw. We want to draw as few as
      possible without loosing image quality. This algorithm is rather
      random; I have no motivation for it at all except that it seems to work
      quite well. The length of the spline is approximated by the sum of
      distances from first to second to third to last point. The number of
      points to draw is then the square root of this distance. I first tried
      to make the number of points proportional to this distance without
      taking the square root of it, but then short splines kind of had too
      few points and long splines had too many. Since sqrt() increases more
      for small input than for large, it seems in a way logical to use it,
      but I don't precisely have any mathematical proof for it. So if someone
      has a better idea of how this could be done, don't hesitate to let us
      know...
   */

#undef DIST
#ifdef _MGHAVE_FIXED_MATH
   #define DIST(x, y) (fixtof (fixsqrt(fixadd (itofix ((x) * (x)), itofix ((y) * (y))))))
#else
   #define DIST(x, y) (sqrt ((x) * (x) + (y) * (y)))
#endif

   if (!points)
       return;

   tmp = DIST(points[1].x-points[0].x, points[1].y-points[0].y);
   tmp += DIST(points[2].x-points[1].x, points[2].y-points[1].y);
   tmp += DIST(points[3].x-points[2].x, points[3].y-points[2].y);

#ifdef _MGHAVE_FIXED_MATH
   num_points = fixtoi (fixmul (fixsqrt(ftofix (tmp)), ftofix (1.2)));
#else
   num_points = (int)(sqrt (tmp) * 1.2);
#endif

   if (num_points > MAX_POINTS)
      num_points = MAX_POINTS;

   calc_spline (points, num_points, pts);

#ifdef _MGHAVE_ADV_2DAPI
   PolyLineEx (hdc, pts, num_points);
#else /* !_MGHAVE_ADV_2DAPI */
   MoveTo (hdc, pts[0].x, pts[0].y);
   for (i = 1; i < num_points; i++) {
      LineTo (hdc, pts[i].x, pts[i].y);

      if (GetRasterOperation (hdc) == ROP_XOR)
            SetPixel (hdc, pts[i].x, pts[i].y, pen_color);
   }
#endif /* !_MGHAVE_ADV_2DAPI */
}

