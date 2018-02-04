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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "fixedmath.h"

/* Line clipper */
/*
  This is a line-clipper using the algorithm by cohen-sutherland.

  It is modified to do pixel-perfect clipping. This means that it
  will generate the same endpoints that would be drawn if an ordinary
  bresenham line-drawer where used and only visible pixels drawn.

  It can be used with a bresenham-like linedrawer if it is modified to
  start with a correct error-term.
*/

#define OC_LEFT 1
#define OC_RIGHT 2
#define OC_TOP 4
#define OC_BOTTOM 8

/* Outcodes:
+-> x
|         |        | 
V  0101 | 0100 | 0110
y ---------------------
    0001 | 0000 | 0010
  ---------------------
    1001 | 1000 | 1010
          |        | 
 */
#define outcode(code, xx, yy) \
{\
  code = 0;\
 if (xx < cliprc->left)\
     code |= OC_LEFT;\
  else if (xx >= cliprc->right)\
     code |= OC_RIGHT;\
  if (yy < cliprc->top)\
     code |= OC_TOP;\
  else if (yy >= cliprc->bottom)\
     code |= OC_BOTTOM;\
}

/*
  Calculates |_ a/b _| with mathematically correct floor
  */
static int FloorDiv(int a, int b)
{
     int floor;
     if (b>0) {
          if (a>0) {
                return a /b;
          } else {
                floor = -((-a)/b);
                if ((-a)%b != 0)
                     floor--;
          }
          return floor;
     } else {
          if (a>0) {
                floor = -(a/(-b));
                if (a%(-b) != 0)
                     floor--;
                return floor;
          } else {
                return (-a)/(-b);
          }
     }
}
/*
  Calculates |^ a/b ^| with mathamatically correct floor
  */
static int CeilDiv(int a,int b)
{
     if (b>0)
          return FloorDiv(a-1,b)+1;
     else
          return FloorDiv(-a-1,-b)+1;
}

BOOL GUIAPI LineClipper (const RECT* cliprc, int *_x0, int *_y0, int *_x1, int *_y1)
{
     int first, last, code;
     int x0, y0, x1, y1;
     int x, y;
     int dx, dy;
     int xmajor;
     int slope;
     
     if (*_x0 == *_x1 && *_y0 == *_y1) { /* a pixel*/
          return PtInRect (cliprc, *_x0, *_y0);
     }
     else if (*_x0 == *_x1) { /* a vertical line */
          int sy, ey;

          if (*_y1 > *_y0) {
                sy = *_y0;
                ey = *_y1;
          }
          else {
                sy = *_y1;
                ey = *_y0;
          }

          if ( (*_x0 >= cliprc->right) || (sy >= cliprc->bottom) || 
                          (*_x0 < cliprc->left) || (ey < cliprc->top) )
                return FALSE;

          if ( (*_x0 >= cliprc->left) && (sy >= cliprc->top) && 
                          (*_x0 < cliprc->right) && (ey < cliprc->bottom) )
                return TRUE;
                          
          if (sy < cliprc->top)
                sy = cliprc->top;
          if (ey >= cliprc->bottom)
                ey = cliprc->bottom - 1;

          if (ey < sy)
                return FALSE;

          *_y0 = sy;
          *_y1 = ey;
          return TRUE;
     }
     else if (*_y0 == *_y1) { /* a horizontal line */
          int sx, ex;

          if (*_x1 > *_x0) {
                sx = *_x0;
                ex = *_x1;
          }
          else {
                sx = *_x1;
                ex = *_x0;
          }

          if ( (sx >= cliprc->right) || (*_y0 >= cliprc->bottom) || 
                          (ex < cliprc->left) || (*_y0 < cliprc->top) )
                return FALSE;

          if ( (sx >= cliprc->left) && (*_y0 >= cliprc->top) && 
                          (ex < cliprc->right) && (*_y0 < cliprc->bottom) )
                return TRUE;
                          
          if (sx < cliprc->left)
                sx = cliprc->left;
          if (ex >= cliprc->right)
                ex = cliprc->right - 1;

          if (ex < sx)
                return FALSE;

          *_x0 = sx;
          *_x1 = ex;
          return TRUE;
     }

     first = 0;
     last = 0;
     outcode (first, *_x0, *_y0);
     outcode (last, *_x1, *_y1);

     if ((first | last) == 0) {
          return TRUE; /* Trivially accepted! */
     }

     if ((first & last) != 0) {
          return FALSE; /* Trivially rejected! */
     }

     x0 = *_x0; y0 = *_y0;
     x1 = *_x1; y1 = *_y1;

     dx = x1 - x0;
     dy = y1 - y0;
  
     xmajor = (ABS (dx) > ABS (dy));
     slope = ((dx>=0) && (dy>=0)) || ((dx<0) && (dy<0));
  
     while (TRUE) {
          code = first;
          if (first == 0)
                code = last;

          if (code & OC_LEFT) {
                x = cliprc->left;
                if (xmajor) {
                     y = *_y0 + FloorDiv (dy * (x - *_x0) * 2 + dx, 2 * dx);
                } else {
                     if (slope) {
                          y = *_y0 + CeilDiv (dy * ((x - *_x0) * 2 - 1), 2 * dx);
                     } else {
                          y = *_y0 + FloorDiv (dy * ((x - *_x0) * 2 - 1), 2 * dx);
                     }
                }
          } else if (code & OC_RIGHT) {
                x = cliprc->right - 1;
                if (xmajor) {
                     y = *_y0 +  FloorDiv (dy * (x - *_x0) * 2 + dx, 2 * dx);
                } else {
                     if (slope) {
                          y = *_y0 + CeilDiv (dy * ((x - *_x0) * 2 + 1), 2 * dx) - 1;
                     } else {
                          y = *_y0 + FloorDiv (dy * ((x - *_x0) * 2 + 1), 2 * dx) + 1;
                     }
                }
          } else if (code & OC_TOP) {
                y = cliprc->top;
                if (xmajor) {
                     if (slope) {
                          x = *_x0 + CeilDiv (dx * ((y - *_y0) * 2 - 1), 2 * dy);
                     } else {
                          x = *_x0 + FloorDiv (dx * ((y - *_y0) * 2 - 1), 2 * dy);
                     }
                } else {
                     x = *_x0 +  FloorDiv (dx * (y - *_y0) * 2 + dy, 2 * dy);
                }
          } else { /* OC_BOTTOM */
                y = cliprc->bottom - 1;
                if (xmajor) {
                     if (slope) {
                          x = *_x0 + CeilDiv (dx * ((y - *_y0) * 2 + 1), 2 * dy) - 1;
                     } else {
                          x = *_x0 + FloorDiv (dx * ((y - *_y0) * 2 + 1), 2 * dy) + 1;
                     }
                } else {
                     x = *_x0 +  FloorDiv (dx * (y - *_y0) * 2 + dy, 2 * dy);
                }
          }

          if (first) {
                x0 = x;
                y0 = y;
                outcode (first, x0, y0);
          } else {
                x1 = x;
                y1 = y;
                last = code;
                outcode (last, x1, y1);
          }
     
          if ((first & last) != 0) {
                return FALSE; /* Trivially rejected! */
          }

          if ((first | last) == 0) {
                *_x0 = x0; *_y0 = y0;
                *_x1 = x1; *_y1 = y1;
                return TRUE; /* Trivially accepted! */
          }
     }
}

/* Breshenham line generator */
void GUIAPI LineGenerator (void* context, int x1, int y1, int x2, int y2, CB_LINE cb)
{
     int xdelta;      /* width of rectangle around line */
     int ydelta;      /* height of rectangle around line */
     int xinc;         /* increment for moving x coordinate */
     int yinc;         /* increment for moving y coordinate */
     int rem;          /* current remainder */
     
     cb (context, 0, 0);

     if (x1 == x2 && y1 == y2) { /* a pixel */
          return;
     }
     else if (x1 == x2) { /* a vertical line */
          int dir = (y2 > y1) ? 1 : -1;

          do {
                cb (context, 0, dir);
                y1 += dir;
          } while (y1 != y2);
          return;
     }
     else if (y1 == y2) { /* a horizontal line */
          int dir = (x2 > x1) ? 1 : -1;

          do {
                cb (context, dir, 0);
                x1 += dir;
          } while (x1 != x2);
          return;
     }

     xdelta = x2 - x1;
     ydelta = y2 - y1;
     if (xdelta < 0) xdelta = -xdelta;
     if (ydelta < 0) ydelta = -ydelta;

     xinc = (x2 > x1) ? 1 : -1;
     yinc = (y2 > y1) ? 1 : -1;

     if (xdelta >= ydelta) {
          rem = xdelta >> 1;
          while (x1 != x2) {
                x1 += xinc;
                rem += ydelta;
                if (rem >= xdelta) {
                     rem -= xdelta;
                     y1 += yinc;
                     cb (context, xinc, yinc);
                }
                else
                     cb (context, xinc, 0);
          }
     } else {
          rem = ydelta >> 1;
          while (y1 != y2) {
                y1 += yinc;
                rem += xdelta;
                if (rem >= ydelta) {
                     rem -= ydelta;
                     x1 += xinc;
                     cb (context, xinc, yinc);
                }
                else
                     cb (context, 0, yinc);
          }
     }
}

/* Circle generator */
void GUIAPI CircleGenerator (void* context, int x, int y, int r, CB_CIRCLE cb)
{
    int cx = 0;
    int cy = r;
    int df = 1 - r; 
    int d_e = 3;
    int d_se = -2 * r + 5;

    do {
        cb (context, x-cx, x+cx, y+cy); 
        if (cy) 
            cb (context, x-cx, x+cx, y-cy); 

        if (cx != cy) {
            cb (context, x-cy, x+cy, y+cx);
            if (cx)
                cb (context, x-cy, x+cy, y-cx); 
        }

        if (df < 0)  {
            df += d_e;
            d_e += 2;
            d_se += 2;
        }
        else { 
            df += d_se;
            d_e += 2;
            d_se += 4;
            cy--;
        } 

        cx++; 

    } while (cx <= cy);
}

/* Ellipse generator */
void GUIAPI EllipseGenerator (void* context, int x, int y, int rx, int ry, CB_ELLIPSE cb)
{
    int ix, iy;
    int h, i, j, k;
    int oh, oi, oj, ok;

    if (rx < 1) 
        rx = 1; 

    if (ry < 1) 
        ry = 1;

    h = i = j = k = 0xFFFF;

    if (rx > ry) {
        ix = 0; 
        iy = rx * 64;

        do {
            oh = h;
            oi = i;
            oj = j;
            ok = k;

            h = (ix + 32) >> 6; 
            i = (iy + 32) >> 6;
            j = (h * ry) / rx; 
            k = (i * ry) / rx;

            if (((h != oh) || (k != ok)) && (h < oi)) {
                cb (context, x-h, x+h, y+k);
                if (k) {
                    cb (context, x-h, x+h, y-k); 
                }
            }

            if (((i != oi) || (j != oj)) && (h < i)) {
                cb (context, x-i, x+i, y+j); 
                if (j) {
                    cb (context, x-i, x+i, y-j); 
                }
            }

            ix = ix + iy / rx; 
            iy = iy - ix / rx;

        } while (i > h);
    } 
    else {
        ix = 0; 
        iy = ry * 64;

        do {
            oh = h;
            oi = i;
            oj = j;
            ok = k;

            h = (ix + 32) >> 6; 
            i = (iy + 32) >> 6;
            j = (h * rx) / ry; 
            k = (i * rx) / ry;

            if (((j != oj) || (i != oi)) && (h < i)) {
                cb (context, x-j, x+j, y+i);
                if (i) {
                    cb (context, x-j, x+j, y-i); 
                }
            }

            if (((k != ok) || (h != oh)) && (h < oi)) {
                cb (context, x-k, x+k, y+h);
                if (h) {
                    cb (context, x-k, x+k, y-h);
                }
            }

            ix = ix + iy / ry; 
            iy = iy - ix / ry;

        } while(i > h);
    }
}

#ifdef _HAVE_MATH_LIB

#ifndef M_PI
#define M_PI          3.14159265358979323846
#endif

/* get_point_on_arc:
 *  Helper function for the CircleArcGenerator () function, converting from (radius, angle)
 *  to (x, y).
 */
static inline void get_point_on_arc (int r, int a, int *out_x, int *out_y)
{
    double s, c;
    double double_a = a * (M_PI / (180.0 * 64));

    s = sin (double_a);
    c = cos (double_a);
    s = -s * r;
    c = c * r;
    *out_x = (int)((c < 0) ? (c - 0.5) : (c + 0.5));
    *out_y = (int)((s < 0) ? (s - 0.5) : (s + 0.5));
}

void GUIAPI CircleArcGenerator (void* context, int x, int y, int r, int ang1, int ang2, CB_ARC cb)
{
    /* start position */
    int sx, sy;
    /* current position */
    int px, py;
    /* end position */
    int ex, ey;
    /* square of radius of circle */
    long rr;
    /* difference between main radius squared and radius squared of three
        potential next points */
    long rr1, rr2, rr3;
    /* square of x and of y */
    unsigned long xx, yy, xx_new, yy_new;
    /* start quadrant, current quadrant and end quadrant */
    int sq, q, qe;
    /* direction of movement */
    int dx, dy;
    /* temporary variable for determining if we have reached end point */
    int det;

    if (ang2 < 0) { /* swap the ang1 and ang2 */
        int tmp = ang1;
        ang1 = ang1 + ang2;
        ang2 = tmp;
    }
    else
        ang2 += ang1;
    
    /* Calculate the start point and the end point. */
    /* We have to flip y because bitmaps count y coordinates downwards. */
    get_point_on_arc (r, ang1, &sx, &sy);
    px = sx;
    py = sy;
    get_point_on_arc (r, ang2, &ex, &ey);

    rr = r*r;
    xx = px*px;
    yy = py*py - rr;

    /* Find start quadrant. */
    if (px >= 0) {
        if (py <= 0)
            q = 0;                                    /* quadrant 0 */
        else
            q = 3;                                    /* quadrant 3 */
    }
    else {
        if (py < 0)
            q = 1;                                    /* quadrant 1 */
        else
            q = 2;                                    /* quadrant 2 */
    }
    sq = q;

    /* Find end quadrant. */
    if (ex >= 0) {
        if (ey <= 0)
            qe = 0;                                  /* quadrant 0 */
        else
            qe = 3;                                  /* quadrant 3 */
    }
    else {
        if (ey < 0)
            qe = 1;                                  /* quadrant 1 */
        else
            qe = 2;                                  /* quadrant 2 */
    }

    if (q > qe) {
        /* qe must come after q. */
        qe += 4;
    }
    else if (q == qe) {
        /* If q==qe but the beginning comes after the end, make qe be
         * strictly after q.
         */
#if 0
        if (((ang2&0xffffff) < (ang1&0xffffff)) ||
          (((ang1&0xffffff) < 0x400000) && ((ang2&0xffffff) >= 0xc00000)))
#else
        if (ang2 < ang1)
#endif
            qe += 4;
    }

    /* initial direction of movement */
    if (((q+1)&2) == 0)
        dy = -1;
    else
        dy = 1;
    if ((q&2) == 0)
        dx = -1;
    else
        dx = 1;

    while (TRUE) {
        /* Change quadrant when needed.
         * dx and dy determine the possible directions to go in this
         * quadrant, so they must be updated when we change quadrant.
         */
        if ((q&1) == 0) {
            if (px == 0) {
                if (qe == q)
                 break;
             q++;
             dy = -dy;
         }
        }
        else {
            if (py == 0) {
             if (qe == q)
                 break;
             q++;
             dx = -dx;
         }
        }

        /* Are we in the final quadrant? */
        if (qe == q) {
         /* Have we reached (or passed) the end point both in x and y? */
         det = 0;

         if (dy > 0) {
             if (py >= ey)
                 det++;
         }
         else {
             if (py <= ey)
                 det++;
         }
         if (dx > 0) {
             if (px >= ex)
                 det++;
         }
         else {
             if (px <= ex)
                 det++;
         }
         
         if (det == 2)
             break;
        }

        cb (context, x + px, y + py);

        /* From here, we have only 3 possible directions of movement, eg.
         * for the first quadrant:
         *
         *     .........
         *     .........
         *     ......21.
         *     ......3*.
         *
         * These are reached by adding dx to px and/or adding dy to py.
         * We need to find which of these points gives the best
         * approximation of the (square of the) radius.
         */

        xx_new = (px+dx) * (px+dx);
        yy_new = (py+dy) * (py+dy) - rr;
        rr1 = xx_new + yy;
        rr2 = xx_new + yy_new;
        rr3 = xx + yy_new;

        /* Set rr1, rr2, rr3 to be the difference from the main radius of the
         * three points.
         */
        if (rr1 < 0)
            rr1 = -rr1;
        if (rr2 < 0)
            rr2 = -rr2;
        if (rr3 < 0)
            rr3 = -rr3;

        if (rr3 >= MIN(rr1, rr2)) {
            px += dx;
        xx = xx_new;
        }
        if (rr1 > MIN(rr2, rr3)) {
            py += dy;
        yy = yy_new;
        }
    }
    /* Only draw last point if it doesn't overlap with first one. */
    if ((px != sx) || (py != sy) || (sq == qe))
        cb (context, x + px, y + py);
}

#endif /* _HAVE_MATH_LIB */

