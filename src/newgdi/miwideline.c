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
Copyright 1988, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.
*/

/*
 * Author:  Keith Packard, MIT X Consortium 
 * 
 * Port to MiniGUI by Wei Yongming.
 *
 * Mostly integer wideline code.  Uses a technique similar to
 * bresenham zero-width lines, except walks an X edge
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

#include "mi.h"
#include "midc.h"
#include "miwideline.h"
#include "misc.h"

#ifdef ICEILTEMPDECL
ICEILTEMPDECL
#endif

static void miLineArc (PDC pdc, SpanDataPtr spanData,
        LineFacePtr leftFace, LineFacePtr rightFace, double xorg, double yorg, BOOL isInt);

/*
 * spans-based polygon filler
 */

void miFillPolyHelper (PDC pdc, SpanDataPtr spanData, int y, int overall_height,
                  PolyEdgePtr left, PolyEdgePtr right, int left_count, int right_count, BOOL isDash)
{
    register int    left_x = 0, left_e = 0;
    int             left_stepx = 0;
    int             left_signdx = 0;
    int             left_dy = 0, left_dx = 0;

    register int    right_x = 0, right_e = 0;
    int             right_stepx = 0;
    int             right_signdx = 0;
    int             right_dy = 0, right_dx = 0;

    int             height = 0;
    int             left_height = 0, right_height = 0;

    register Span*  ppt;
    Span*           pptInit = NULL;
    int             xorg;
    Spans           spanRec;

    left_height = 0;
    right_height = 0;
    
    if (!spanData) {
        pptInit = (Span*) ALLOCATE_LOCAL (overall_height * sizeof(*ppt));
        if (!pptInit)
            return;
        ppt = pptInit;
    }
    else {
        spanRec.points = (Span*) malloc (overall_height * sizeof (*ppt));
        if (!spanRec.points)
            return;
        ppt = spanRec.points;
    }

    xorg = 0;
    while ((left_count || left_height) && (right_count || right_height)) {
        MIPOLYRELOADLEFT
        MIPOLYRELOADRIGHT

        height = left_height;
        if (height > right_height)
            height = right_height;

        left_height -= height;
        right_height -= height;

        while (--height >= 0) {
            if (right_x >= left_x) {
                ppt->y = y;
                ppt->x = left_x + xorg;
                ppt->width = right_x - left_x + 1;
                ppt++;
            }
            y++;
            
            MIPOLYSTEPLEFT

            MIPOLYSTEPRIGHT
        }
    }

    if (!spanData) {
        if (isDash) {
            if (pdc->brush_type != BT_SOLID && pdc->brush_stipple)
                _dc_fill_spans_brush (pdc, pptInit, ppt - pptInit, FALSE);
            else {
                pdc->cur_pixel = pdc->brushcolor;
                _dc_fill_spans (pdc, pptInit, ppt - pptInit, FALSE);
            }
        }
        else {
            pdc->cur_pixel = pdc->pencolor;
            _dc_fill_spans (pdc, pptInit, ppt - pptInit, FALSE);
        }
        DEALLOCATE_LOCAL (pptInit);
    }
    else {
        spanRec.count = ppt - spanRec.points;
        AppendSpanGroup (pdc, &spanRec, spanData)
    }
}

static void miFillRectPolyHelper (PDC pdc, SpanDataPtr spanData, 
                    int x, int y, int w, int h)
{
    register Span*  ppt;
    Spans           spanRec;

    if (!spanData) {
        _dc_fill_rect (pdc, x, y, w, h);
    }
    else {
        spanRec.points = (Span*) malloc (h * sizeof (*ppt));
        if (!spanRec.points)
            return;

        ppt = spanRec.points;
        while (h--) {
            ppt->x = x;
            ppt->y = y;
            ppt->width = w;
            ppt++;
            y++;
        }
        spanRec.count = ppt - spanRec.points;
        AppendSpanGroup (pdc, &spanRec, spanData)
    }
}

int miPolyBuildEdge (double x0, double y0, double k, /* x0 * dy - y0 * dx */
                 register int dx, register int dy, 
                int xi, int yi, int left, register PolyEdgePtr edge)
{
    int x, y, e;
    int xady;

    if (dy < 0) {
        dy = -dy;
        dx = -dx;
        k = -k;
    }

#ifdef NOTDEF
    {
        double realk, kerror;
        realk = x0 * dy - y0 * dx;
        kerror = Fabs (realk - k);
        if (kerror > .1)
            printf ("realk: %g k: %g\n", realk, k);
    }
#endif

    y = ICEIL (y0);
    xady = ICEIL (k) + y * dx;

    if (xady <= 0)
        x = - (-xady / dy) - 1;
    else
        x = (xady - 1) / dy;

    e = xady - x * dy;

    if (dx >= 0) {
        edge->signdx = 1;
        edge->stepx = dx / dy;
        edge->dx = dx % dy;
    }
    else {
        edge->signdx = -1;
        edge->stepx = - (-dx / dy);
        edge->dx = -dx % dy;
        e = dy - e + 1;
    }

    edge->dy = dy;
    edge->x = x + left + xi;
    edge->e = e - dy;        /* bias to compare against 0 instead of dy */
    return y + yi;
}

#define StepAround(v, incr, max) (((v) + (incr) < 0) ? (max - 1) : ((v) + (incr) == max) ? 0 : ((v) + (incr)))

int miPolyBuildPoly (register PolyVertexPtr vertices, register PolySlopePtr slopes, 
                int count, int xi, int yi, PolyEdgePtr left, PolyEdgePtr right, 
                int* pnleft, int* pnright, int* h)
{
    register int    i;
    register int    s;
    register int    nright, nleft;
    int             top, bottom;
    double          miny, maxy;
    int             j;
    int             clockwise;
    int             slopeoff;
    int             y, lasty = 0, bottomy, topy = 0;

    /* find the top of the polygon */
    maxy = miny = vertices[0].y;
    bottom = top = 0;
    for (i = 1; i < count; i++) {
        if (vertices[i].y < miny) {
            top = i;
            miny = vertices[i].y;
        }
        if (vertices[i].y >= maxy) {
            bottom = i;
            maxy = vertices[i].y;
        }
    }
    clockwise = 1;
    slopeoff = 0;

    i = top;
    j = StepAround (top, -1, count);

    if (slopes[j].dy * slopes[i].dx > slopes[i].dy * slopes[j].dx) {
        clockwise = -1;
        slopeoff = -1;
    }

    bottomy = ICEIL (maxy) + yi;

    nright = 0;

    s = StepAround (top, slopeoff, count);
    i = top;
    while (i != bottom) {
        if (slopes[s].dy != 0) {
            y = miPolyBuildEdge (vertices[i].x, vertices[i].y,
                        slopes[s].k, slopes[s].dx, slopes[s].dy,
                        xi, yi, 0, &right[nright]);
            if (nright != 0)
                right[nright-1].height = y - lasty;
            else
                topy = y;
            nright++;
            lasty = y;
        }

        i = StepAround (i, clockwise, count);
        s = StepAround (s, clockwise, count);
    }

    if (nright != 0)
        right[nright-1].height = bottomy - lasty;

    if (slopeoff == 0)
        slopeoff = -1;
    else
        slopeoff = 0;

    nleft = 0;
    s = StepAround (top, slopeoff, count);
    i = top;
    while (i != bottom) {
        if (slopes[s].dy != 0) {
            y = miPolyBuildEdge (vertices[i].x, vertices[i].y, slopes[s].k, 
                        slopes[s].dx,  slopes[s].dy, xi, yi, 1, &left[nleft]);
    
            if (nleft != 0)
                    left[nleft-1].height = y - lasty;
            nleft++;
            lasty = y;
        }

        i = StepAround (i, -clockwise, count);
        s = StepAround (s, -clockwise, count);
    }

    if (nleft != 0)
        left[nleft-1].height = bottomy - lasty;

    *pnleft = nleft;
    *pnright = nright;
    *h = bottomy - topy;

    return topy;
}

static void miLineOnePoint (PDC pdc, SpanDataPtr spanData, int x, int y)
{
    Span  span;

    span.x = x;
    span.y = y;
    span.width = 1;

    pdc->cur_pixel = pdc->pencolor;
    _dc_fill_spans (pdc, &span, 1, FALSE);
}

static void miLineJoin (PDC pdc, SpanDataPtr spanData, LineFacePtr pLeft, LineFacePtr pRight)
{
    double          mx = 0, my = 0;
    double          denom = 0.0;
    PolyVertexRec   vertices[4];
    PolySlopeRec    slopes[4];
    int             edgecount;
    PolyEdgeRec     left[4], right[4];
    int             nleft, nright;
    int             y, height;
    int             swapslopes;
    int             joinStyle = pdc->pen_join_style;
    int             lw = pdc->pen_width;

    if (lw == 1 && !spanData) {
        /* Lines going in the same direction have no join */
        if ((pLeft->dx >= 0) == (pRight->dx <= 0))
            return;

        if (joinStyle != PT_JOIN_ROUND) {
            denom = - pLeft->dx * (double)pRight->dy + pRight->dx * (double)pLeft->dy;
            if (denom == 0)
                return;        /* no join to draw */
        }

        if (joinStyle != PT_JOIN_MITER) {
            miLineOnePoint (pdc, spanData, pLeft->x, pLeft->y);
            return;
        }
    } else {
        if (joinStyle == PT_JOIN_ROUND) {
            miLineArc(pdc, spanData, pLeft, pRight,
                      (double)0.0, (double)0.0, TRUE);
            return;
        }
        denom = - pLeft->dx * (double)pRight->dy + pRight->dx * (double)pLeft->dy;
        if (denom == 0.0)
            return;        /* no join to draw */
    }

    swapslopes = 0;
    if (denom > 0) {
        pLeft->xa = -pLeft->xa;
        pLeft->ya = -pLeft->ya;
        pLeft->dx = -pLeft->dx;
        pLeft->dy = -pLeft->dy;
    }
    else {
        swapslopes = 1;
        pRight->xa = -pRight->xa;
        pRight->ya = -pRight->ya;
        pRight->dx = -pRight->dx;
        pRight->dy = -pRight->dy;
    }

    vertices[0].x = pRight->xa;
    vertices[0].y = pRight->ya;
    slopes[0].dx = -pRight->dy;
    slopes[0].dy =  pRight->dx;
    slopes[0].k = 0;

    vertices[1].x = 0;
    vertices[1].y = 0;
    slopes[1].dx =  pLeft->dy;
    slopes[1].dy = -pLeft->dx;
    slopes[1].k = 0;

    vertices[2].x = pLeft->xa;
    vertices[2].y = pLeft->ya;

    if (joinStyle == PT_JOIN_MITER) {
        my = (pLeft->dy  * (pRight->xa * pRight->dy - pRight->ya * pRight->dx) -
              pRight->dy * (pLeft->xa  * pLeft->dy  - pLeft->ya  * pLeft->dx )) / denom;

        if (pLeft->dy != 0) {
            mx = pLeft->xa + (my - pLeft->ya) *
                            (double) pLeft->dx / (double) pLeft->dy;
        }
        else {
            mx = pRight->xa + (my - pRight->ya) *
                            (double) pRight->dx / (double) pRight->dy;
        }

        /* check miter limit */
        if ((mx * mx + my * my) * 4 > SQSECANT * lw * lw)
            joinStyle = PT_JOIN_BEVEL;
    }

    if (joinStyle == PT_JOIN_MITER) {
        slopes[2].dx = pLeft->dx;
        slopes[2].dy = pLeft->dy;
        slopes[2].k =  pLeft->k;

        if (swapslopes) {
            slopes[2].dx = -slopes[2].dx;
            slopes[2].dy = -slopes[2].dy;
            slopes[2].k  = -slopes[2].k;
        }
        vertices[3].x = mx;
        vertices[3].y = my;
        slopes[3].dx = pRight->dx;
        slopes[3].dy = pRight->dy;
        slopes[3].k  = pRight->k;
        if (swapslopes) {
            slopes[3].dx = -slopes[3].dx;
            slopes[3].dy = -slopes[3].dy;
            slopes[3].k  = -slopes[3].k;
        }
        edgecount = 4;
    }
    else {
        double  scale, dx, dy, adx, ady;

        adx = dx = pRight->xa - pLeft->xa;
        ady = dy = pRight->ya - pLeft->ya;
        if (adx < 0)
            adx = -adx;
        if (ady < 0)
            ady = -ady;
        scale = ady;
        if (adx > ady)
            scale = adx;
        slopes[2].dx = (dx * 65536) / scale;
        slopes[2].dy = (dy * 65536) / scale;
        slopes[2].k = ((pLeft->xa + pRight->xa) * slopes[2].dy -
                       (pLeft->ya + pRight->ya) * slopes[2].dx) / 2.0;
        edgecount = 3;
    }

    y = miPolyBuildPoly (vertices, slopes, edgecount, pLeft->x, pLeft->y,
                   left, right, &nleft, &nright, &height);
    miFillPolyHelper (pdc, spanData, y, height, left, right, nleft, nright, FALSE);
}

static int miLineArcI (PDC pdc, int xorg, int yorg, Span* points)
{
    register Span* tpts, *bpts;
    register int x, y, e, ex, slw;

    tpts = points;
    slw = pdc->pen_width;
    if (slw == 1) {
        tpts->x = xorg;
        tpts->y = yorg;
        tpts->width = 1;
        return 1;
    }

    bpts = tpts + slw;
    y = (slw >> 1) + 1;
    if (slw & 1)
        e = - ((y << 2) + 3);
    else
        e = - (y << 3);
    ex = -4;
    x = 0;
    while (y) {
        e += (y << 3) - 4;
        while (e >= 0) {
            x++;
            e += (ex = -((x << 3) + 4));
        }
        y--;
        slw = (x << 1) + 1;
        if ((e == ex) && (slw > 1))
            slw--;
        tpts->x = xorg - x;
        tpts->y = yorg - y;
        tpts->width = slw;
        tpts++;

        if ((y != 0) && ((slw > 1) || (e != ex))) {
            bpts--;
            bpts->x = xorg - x;
            bpts->y = yorg + y;
            bpts->width = slw;
        }
    }

    return (pdc->pen_width);
}

#define CLIPSTEPEDGE(edgey, edge, edgeleft) \
    if (ybase == edgey) \
    { \
        if (edgeleft) \
        { \
            if (edge->x > xcl) \
                xcl = edge->x; \
        } \
        else \
        { \
            if (edge->x < xcr) \
                xcr = edge->x; \
        } \
        edgey++; \
        edge->x += edge->stepx; \
        edge->e += edge->dx; \
        if (edge->e > 0) \
        { \
            edge->x += edge->signdx; \
            edge->e -= edge->dy; \
        } \
    }

static int miLineArcD (PDC pdc, double xorg, double yorg, Span* points,
                PolyEdgePtr edge1, int edgey1, BOOL edgeleft1, PolyEdgePtr edge2, int edgey2, BOOL edgeleft2)
{
    register Span* pts;
    double radius, x0, y0, el, er, yk, xlk, xrk, k;
    int xbase, ybase, y, boty, xl, xr, xcl, xcr;
    int ymin, ymax;
    BOOL edge1IsMin, edge2IsMin;
    int ymin1, ymin2;

    pts = points;
    xbase = floor(xorg);
    x0 = xorg - xbase;
    ybase = ICEIL (yorg);
    y0 = yorg - ybase;
    xlk = x0 + x0 + 1.0;
    xrk = x0 + x0 - 1.0;
    yk = y0 + y0 - 1.0;
    radius = ((double)pdc->pen_width) / 2.0;
    y = floor(radius - y0 + 1.0);
    ybase -= y;
    ymin = ybase;
    ymax = 65536;
    edge1IsMin = FALSE;
    ymin1 = edgey1;

    if (edge1->dy >= 0) {
        if (!edge1->dy) {
            if (edgeleft1)
                edge1IsMin = TRUE;
            else
               ymax = edgey1;
            edgey1 = 65536;
        }
        else {
            if ((edge1->signdx < 0) == edgeleft1)
                    edge1IsMin = TRUE;
            }
    }

    edge2IsMin = FALSE;
    ymin2 = edgey2;
    if (edge2->dy >= 0) {
        if (!edge2->dy) {
            if (edgeleft2)
                edge2IsMin = TRUE;
            else
                ymax = edgey2;
            edgey2 = 65536;
        }
        else {
            if ((edge2->signdx < 0) == edgeleft2)
                    edge2IsMin = TRUE;
            }
    }

    if (edge1IsMin) {
        ymin = ymin1;
        if (edge2IsMin && ymin1 > ymin2)
            ymin = ymin2;
    } else if (edge2IsMin)
        ymin = ymin2;

    el = radius * radius - ((y + y0) * (y + y0)) - (x0 * x0);
    er = el + xrk;
    xl = 1;
    xr = 0;
    if (x0 < 0.5) {
        xl = 0;
        el -= xlk;
    }
    boty = (y0 < -0.5) ? 1 : 0;

    if (ybase + y - boty > ymax)
        boty = ymax - ybase - y;

    while (y > boty) {
        k = (y << 1) + yk;
        er += k;
        while (er > 0.0) {
            xr++;
            er += xrk - (xr << 1);
        }
        el += k;
        while (el >= 0.0) {
            xl--;
            el += (xl << 1) - xlk;
        }
        y--;
        ybase++;
        if (ybase < ymin)
            continue;
        xcl = xl + xbase;
        xcr = xr + xbase;
        CLIPSTEPEDGE(edgey1, edge1, edgeleft1);
        CLIPSTEPEDGE(edgey2, edge2, edgeleft2);
        if (xcr >= xcl)
        {
            pts->x = xcl;
            pts->y = ybase;
            pts->width = xcr - xcl + 1;
            pts++;
        }
    }

    er = xrk - (xr << 1) - er;
    el = (xl << 1) - xlk - el;
    boty = floor(-y0 - radius + 1.0);
    if (ybase + y - boty > ymax)
        boty = ymax - ybase - y;

    while (y > boty) {
        k = (y << 1) + yk;
        er -= k;
        while ((er >= 0.0) && (xr >= 0)) {
            xr--;
            er += xrk - (xr << 1);
        }
        el -= k;
        while ((el > 0.0) && (xl <= 0)) {
            xl++;
            el += (xl << 1) - xlk;
        }
        y--;
        ybase++;
        if (ybase < ymin)
            continue;
        xcl = xl + xbase;
        xcr = xr + xbase;
        CLIPSTEPEDGE(edgey1, edge1, edgeleft1);
        CLIPSTEPEDGE(edgey2, edge2, edgeleft2);
        if (xcr >= xcl) {
            pts->x = xcl;
            pts->y = ybase;
            pts->width = xcr - xcl + 1;
            pts++;
        }
    }

    return (pts - points);
}

int miRoundJoinFace (register LineFacePtr face, register PolyEdgePtr edge, BOOL* leftEdge)
{
    int     y;
    int     dx, dy;
    double  xa, ya;
    BOOL    left;

    dx = -face->dy;
    dy = face->dx;
    xa = face->xa;
    ya = face->ya;
    left = 1;

    if (ya > 0) {
        ya = 0.0;
        xa = 0.0;
    }
    if (dy < 0 || (dy == 0 && dx > 0)) {
        dx = -dx;
        dy = -dy;
        left = !left;
    }

    if (dx == 0 && dy == 0)
        dy = 1;

    if (dy == 0) {
        y = ICEIL (face->ya) + face->y;
        edge->x = -32767;
        edge->stepx = 0;
        edge->signdx = 0;
        edge->e = -1;
        edge->dy = 0;
        edge->dx = 0;
        edge->height = 0;
    }
    else {
        y = miPolyBuildEdge (xa, ya, 0.0, dx, dy, face->x, face->y, !left, edge);
        edge->height = 32767;
    }

    *leftEdge = !left;
    return y;
}

void miRoundJoinClip (register LineFacePtr pLeft, register LineFacePtr pRight, 
                PolyEdgePtr edge1, PolyEdgePtr edge2, int* y1, int* y2, BOOL* left1, BOOL* left2)
{
    double  denom;

    denom = - pLeft->dx * (double)pRight->dy + pRight->dx * (double)pLeft->dy;

    if (denom >= 0) {
        pLeft->xa = -pLeft->xa;
        pLeft->ya = -pLeft->ya;
    }
    else {
        pRight->xa = -pRight->xa;
        pRight->ya = -pRight->ya;
    }

    *y1 = miRoundJoinFace (pLeft, edge1, left1);
    *y2 = miRoundJoinFace (pRight, edge2, left2);
}

int miRoundCapClip (register LineFacePtr face, BOOL isInt, register PolyEdgePtr edge, BOOL* leftEdge)
{
    int             y;
    register int    dx, dy;
    double          xa, ya, k;
    BOOL            left;

    dx = -face->dy;
    dy = face->dx;
    xa = face->xa;
    ya = face->ya;
    k = 0.0;

    if (!isInt)
        k = face->k;

    left = 1;
    if (dy < 0 || (dy == 0 && dx > 0)) {
        dx = -dx;
        dy = -dy;
        xa = -xa;
        ya = -ya;
        left = !left;
    }

    if (dx == 0 && dy == 0)
        dy = 1;

    if (dy == 0) {
        y = ICEIL (face->ya) + face->y;
        edge->x = -32767;
        edge->stepx = 0;
        edge->signdx = 0;
        edge->e = -1;
        edge->dy = 0;
        edge->dx = 0;
        edge->height = 0;
    }
    else {
        y = miPolyBuildEdge (xa, ya, k, dx, dy, face->x, face->y, !left, edge);
        edge->height = 32767;
    }

    *leftEdge = !left;
    return y;
}

static void miLineArc (PDC pdc, SpanDataPtr spanData, 
                register LineFacePtr leftFace, register LineFacePtr rightFace, 
                double xorg, double yorg, BOOL isInt)
{
    Span*       points;
    int         xorgi = 0, yorgi = 0;
    Spans       spanRec;
    int         n;
    PolyEdgeRec edge1, edge2;
    int         edgey1, edgey2;
    BOOL        edgeleft1, edgeleft2;

    if (isInt) {
        xorgi = leftFace ? leftFace->x : rightFace->x;
        yorgi = leftFace ? leftFace->y : rightFace->y;
    }

    edgey1 = 65536;
    edgey2 = 65536;
    edge1.x = 0; /* not used, keep memory checkers happy */
    edge1.dy = -1;
    edge2.x = 0; /* not used, keep memory checkers happy */
    edge2.dy = -1;
    edgeleft1 = FALSE;
    edgeleft2 = FALSE;

    if (((pdc->pen_type != PT_SOLID || pdc->pen_width > 2) &&
            (pdc->pen_cap_style == PT_CAP_ROUND && pdc->pen_join_style != PT_JOIN_ROUND)) ||
            (pdc->pen_join_style == PT_JOIN_ROUND && pdc->pen_cap_style == PT_CAP_BUTT)) {

        if (isInt) {
            xorg = (double) xorgi;
            yorg = (double) yorgi;
        }

        if (leftFace && rightFace) {
            miRoundJoinClip (leftFace, rightFace, &edge1, &edge2,
                             &edgey1, &edgey2, &edgeleft1, &edgeleft2);
        }
        else if (leftFace) {
            edgey1 = miRoundCapClip (leftFace, isInt, &edge1, &edgeleft1);
        }
        else if (rightFace) {
            edgey2 = miRoundCapClip (rightFace, isInt, &edge2, &edgeleft2);
        }
        isInt = FALSE;
    }

    if (!spanData) {
        points = (Span*)ALLOCATE_LOCAL(sizeof(Span) * pdc->pen_width);
        if (!points)
            return;
    }
    else {
        points = (Span*) malloc (pdc->pen_width * sizeof (Span));
        if (!points)
            return;
        spanRec.points = points;
    }

    if (isInt)
        n = miLineArcI (pdc, xorgi, yorgi, points);
    else
        n = miLineArcD (pdc, xorg, yorg, points,
                       &edge1, edgey1, edgeleft1,
                       &edge2, edgey2, edgeleft2);

    if (!spanData) {
        pdc->cur_pixel = pdc->pencolor;
        _dc_fill_spans (pdc, points, n, FALSE);
        DEALLOCATE_LOCAL(points);
    }
    else {
        spanRec.count = n;
        AppendSpanGroup (pdc, &spanRec, spanData)
    }
}

void miLineProjectingCap (PDC pdc, SpanDataPtr spanData, 
                register LineFacePtr face, BOOL isLeft, double xorg, double yorg, BOOL isInt)
{
    int         xorgi = 0, yorgi = 0;
    int         lw;
    PolyEdgeRec lefts[2], rights[2];
    int         lefty, righty, topy, bottomy;
    PolyEdgePtr left, right;
    PolyEdgePtr top, bottom;
    double      xa,ya;
    double      k;
    double      xap, yap;
    int         dx, dy;
    double      projectXoff, projectYoff;
    double      maxy;
    int         finaly;
    
    if (isInt) {
        xorgi = face->x;
        yorgi = face->y;
    }

    lw = pdc->pen_width;
    dx = face->dx;
    dy = face->dy;
    k = face->k;
    if (dy == 0) {
        lefts[0].height = lw;
        lefts[0].x = xorgi;
        if (isLeft)
            lefts[0].x -= (lw >> 1);
        lefts[0].stepx = 0;
        lefts[0].signdx = 1;
        lefts[0].e = -lw;
        lefts[0].dx = 0;
        lefts[0].dy = lw;
        rights[0].height = lw;
        rights[0].x = xorgi;
        if (!isLeft)
            rights[0].x += ((lw + 1) >> 1);
        rights[0].stepx = 0;
        rights[0].signdx = 1;
        rights[0].e = -lw;
        rights[0].dx = 0;
        rights[0].dy = lw;
        miFillPolyHelper (pdc, spanData, yorgi - (lw >> 1), lw, lefts, rights, 1, 1, 0);
    }
    else if (dx == 0)
    {
        topy = yorgi;
        bottomy = yorgi + dy;
        if (isLeft)
            topy -= (lw >> 1);
        else
            bottomy += (lw >> 1);
        lefts[0].height = bottomy - topy;
        lefts[0].x = xorgi - (lw >> 1);
        lefts[0].stepx = 0;
        lefts[0].signdx = 1;
        lefts[0].e = -dy;
        lefts[0].dx = dx;
        lefts[0].dy = dy;

        rights[0].height = bottomy - topy;
        rights[0].x = lefts[0].x + (lw-1);
        rights[0].stepx = 0;
        rights[0].signdx = 1;
        rights[0].e = -dy;
        rights[0].dx = dx;
        rights[0].dy = dy;
        miFillPolyHelper (pdc, spanData, topy, bottomy - topy, lefts, rights, 1, 1, 0);
    }
    else {
        xa = face->xa;
        ya = face->ya;
        projectXoff = -ya;
        projectYoff = xa;

        if (dx < 0) {
            right = &rights[1];
            left = &lefts[0];
            top = &rights[0];
            bottom = &lefts[1];
        }
        else {
            right = &rights[0];
            left = &lefts[1];
            top = &lefts[0];
            bottom = &rights[1];
        }

        if (isLeft) {
            righty = miPolyBuildEdge (xa, ya, k, dx, dy, xorgi, yorgi, 0, right);
            
            xa = -xa;
            ya = -ya;
            k = -k;
            lefty = miPolyBuildEdge (xa - projectXoff, ya - projectYoff,
                                     k, dx, dy, xorgi, yorgi, 1, left);
            if (dx > 0) {
                ya = -ya;
                xa = -xa;
            }
            xap = xa - projectXoff;
            yap = ya - projectYoff;
            topy = miPolyBuildEdge (xap, yap, xap * dx + yap * dy,
                                    -dy, dx, xorgi, yorgi, dx > 0, top);
            bottomy = miPolyBuildEdge (xa, ya,
                                       0.0, -dy, dx, xorgi, yorgi, dx < 0, bottom);
            maxy = -ya;
        }
        else {
            righty = miPolyBuildEdge (xa - projectXoff, ya - projectYoff,
                     k, dx, dy, xorgi, yorgi, 0, right);
            
            xa = -xa;
            ya = -ya;
            k = -k;
            lefty = miPolyBuildEdge (xa, ya, k, dx, dy, xorgi, yorgi, 1, left);
            if (dx > 0) {
                ya = -ya;
                xa = -xa;
            }
            xap = xa - projectXoff;
            yap = ya - projectYoff;
            topy = miPolyBuildEdge (xa, ya, 0.0, -dy, dx, xorgi, xorgi, dx > 0, top);
            bottomy = miPolyBuildEdge (xap, yap, xap * dx + yap * dy,
                                       -dy, dx, xorgi, xorgi, dx < 0, bottom);
            maxy = -ya + projectYoff;
        }
        finaly = ICEIL(maxy) + yorgi;
        if (dx < 0) {
            left->height = bottomy - lefty;
            right->height = finaly - righty;
            top->height = righty - topy;
        }
        else {
            right->height =  bottomy - righty;
            left->height = finaly - lefty;
            top->height = lefty - topy;
        }
        bottom->height = finaly - bottomy;
        miFillPolyHelper (pdc, spanData, topy,
                     bottom->height + bottomy - topy, lefts, rights, 2, 2, 0);
    }
}

static void miWideSegment (PDC pdc, SpanDataPtr spanData,
               int x1, int y1, int x2, int y2, BOOL projectLeft, BOOL projectRight, 
                LineFacePtr leftFace, LineFacePtr rightFace)
{
    double      l, L, r;
    double      xa, ya;
    double      projectXoff = 0.0, projectYoff = 0.0;
    double      k;
    double      maxy;
    int         x, y;
    int         dx, dy;
    int         finaly;
    PolyEdgePtr left, right;
    PolyEdgePtr top, bottom;
    int         lefty, righty, topy, bottomy;
    int         signdx;
    PolyEdgeRec lefts[2], rights[2];
    LineFacePtr tface;
    int         lw = pdc->pen_width;

    /* draw top-to-bottom always */
    if (y2 < y1 || (y2 == y1 && x2 < x1)) {
        x = x1;
        x1 = x2;
        x2 = x;

        y = y1;
        y1 = y2;
        y2 = y;

        x = projectLeft;
        projectLeft = projectRight;
        projectRight = x;

        tface = leftFace;
        leftFace = rightFace;
        rightFace = tface;
    }

    dy = y2 - y1;
    signdx = 1;
    dx = x2 - x1;
    if (dx < 0)
        signdx = -1;

    leftFace->x = x1;
    leftFace->y = y1;
    leftFace->dx = dx;
    leftFace->dy = dy;

    rightFace->x = x2;
    rightFace->y = y2;
    rightFace->dx = -dx;
    rightFace->dy = -dy;

    if (dy == 0) {
        rightFace->xa = 0;
        rightFace->ya = (double) lw / 2.0;
        rightFace->k = -(double) (lw * dx) / 2.0;
        leftFace->xa = 0;
        leftFace->ya = -rightFace->ya;
        leftFace->k = rightFace->k;
        x = x1;
        if (projectLeft)
            x -= (lw >> 1);
        y = y1 - (lw >> 1);
        dx = x2 - x;
        if (projectRight)
            dx += ((lw + 1) >> 1);
        dy = lw;
        miFillRectPolyHelper (pdc, spanData, x, y, dx, dy);
    }
    else if (dx == 0) {
        leftFace->xa =  (double) lw / 2.0;
        leftFace->ya = 0;
        leftFace->k = (double) (lw * dy) / 2.0;
        rightFace->xa = -leftFace->xa;
        rightFace->ya = 0;
        rightFace->k = leftFace->k;
        y = y1;
        if (projectLeft)
            y -= lw >> 1;
        x = x1 - (lw >> 1);
        dy = y2 - y;
        if (projectRight)
            dy += ((lw + 1) >> 1);
        dx = lw;
        miFillRectPolyHelper (pdc, spanData, x, y, dx, dy);
    }
    else {
        l = ((double) lw) / 2.0;
        L = hypot ((double) dx, (double) dy);

        if (dx < 0) {
            right = &rights[1];
            left = &lefts[0];
            top = &rights[0];
            bottom = &lefts[1];
        }
        else {
            right = &rights[0];
            left = &lefts[1];
            top = &lefts[0];
            bottom = &rights[1];
        }
        r = l / L;

        /* coord of upper bound at integral y */
        ya = -r * dx;
        xa = r * dy;

        if (projectLeft | projectRight) {
            projectXoff = -ya;
            projectYoff = xa;
        }

        /* xa * dy - ya * dx */
        k = l * L;

        leftFace->xa = xa;
        leftFace->ya = ya;
        leftFace->k = k;
        rightFace->xa = -xa;
        rightFace->ya = -ya;
        rightFace->k = k;

        if (projectLeft)
            righty = miPolyBuildEdge (xa - projectXoff, ya - projectYoff,
                                      k, dx, dy, x1, y1, 0, right);
        else
            righty = miPolyBuildEdge (xa, ya,
                                      k, dx, dy, x1, y1, 0, right);

        /* coord of lower bound at integral y */
        ya = -ya;
        xa = -xa;

        /* xa * dy - ya * dx */
        k = - k;

        if (projectLeft)
            lefty = miPolyBuildEdge (xa - projectXoff, ya - projectYoff,
                                     k, dx, dy, x1, y1, 1, left);
        else
            lefty = miPolyBuildEdge (xa, ya,
                                     k, dx, dy, x1, y1, 1, left);

        /* coord of top face at integral y */

        if (signdx > 0) {
            ya = -ya;
            xa = -xa;
        }

        if (projectLeft) {
            double xap = xa - projectXoff;
            double yap = ya - projectYoff;
            topy = miPolyBuildEdge (xap, yap, xap * dx + yap * dy,
                                    -dy, dx, x1, y1, dx > 0, top);
        }
        else
            topy = miPolyBuildEdge (xa, ya, 0.0, -dy, dx, x1, y1, dx > 0, top);

        /* coord of bottom face at integral y */

        if (projectRight) {
            double xap = xa + projectXoff;
            double yap = ya + projectYoff;
            bottomy = miPolyBuildEdge (xap, yap, xap * dx + yap * dy,
                                       -dy, dx, x2, y2, dx < 0, bottom);
            maxy = -ya + projectYoff;
        }
        else {
            bottomy = miPolyBuildEdge (xa, ya,
                                       0.0, -dy, dx, x2, y2, dx < 0, bottom);
            maxy = -ya;
        }

        finaly = ICEIL (maxy) + y2;

        if (dx < 0) {
            left->height = bottomy - lefty;
            right->height = finaly - righty;
            top->height = righty - topy;
        }
        else {
            right->height =  bottomy - righty;
            left->height = finaly - lefty;
            top->height = lefty - topy;
        }

        bottom->height = finaly - bottomy;
        miFillPolyHelper (pdc, spanData, topy,
                          bottom->height + bottomy - topy, lefts, rights, 2, 2, 0);
    }
}

SpanDataPtr miSetupSpanData (PDC pdc, SpanDataPtr spanData, int npt)
{
    if ((npt < 3 && pdc->pen_cap_style != PT_CAP_ROUND) || miSpansEasyRop(pdc->rop))
        return (SpanDataPtr) NULL;

    if (pdc->pen_type == PT_DOUBLE_DASH)
        miInitSpanGroup (&spanData->bgGroup);

    miInitSpanGroup (&spanData->fgGroup);
    return spanData;
}

void miCleanupSpanData (PDC pdc, SpanDataPtr spanData)
{
    if (pdc->pen_type == PT_DOUBLE_DASH) {
        pdc->cur_pixel = pdc->brushcolor;
        miFillUniqueSpanGroup (pdc, &spanData->bgGroup, TRUE);
        miFreeSpanGroup (&spanData->bgGroup);
    }

    pdc->cur_pixel = pdc->pencolor;
    miFillUniqueSpanGroup (pdc, &spanData->fgGroup, FALSE);
    miFreeSpanGroup (&spanData->fgGroup);
}

void miWideLine (PDC pdc, register int npt, register POINT* pPts)
{
    int             x1, y1, x2, y2;
    SpanDataRec     spanDataRec;
    SpanDataPtr     spanData;
    BOOL            projectLeft, projectRight;
    LineFaceRec     leftFace, rightFace, prevRightFace;
    LineFaceRec     firstFace;
    register int    first;
    BOOL            somethingDrawn = FALSE;
    BOOL            selfJoin;

    spanData = miSetupSpanData (pdc, &spanDataRec, npt);
    pdc->cur_pixel = pdc->pencolor;

    x2 = pPts->x;
    y2 = pPts->y;
    first = TRUE;
    selfJoin = FALSE;

    if (npt > 1 && x2 == pPts[npt-1].x && y2 == pPts[npt-1].y)
        selfJoin = TRUE;

    projectLeft = pdc->pen_cap_style == PT_CAP_PROJECTING && !selfJoin;
    projectRight = FALSE;

    while (--npt) {
        x1 = x2;
        y1 = y2;
        ++pPts;
        x2 = pPts->x;
        y2 = pPts->y;

        if (x1 != x2 || y1 != y2) {
            somethingDrawn = TRUE;
            if (npt == 1 && pdc->pen_cap_style == PT_CAP_PROJECTING && !selfJoin)
                    projectRight = TRUE;
            miWideSegment (pdc, spanData, x1, y1, x2, y2,
                                  projectLeft, projectRight, &leftFace, &rightFace);
            if (first) {
                if (selfJoin)
                    firstFace = leftFace;
                else if (pdc->pen_cap_style == PT_CAP_ROUND) {
                    if (pdc->pen_width == 1 && !spanData)
                        miLineOnePoint (pdc, spanData, x1, y1);
                    else
                        miLineArc (pdc, spanData, &leftFace, (LineFacePtr) NULL,
                                           (double)0.0, (double)0.0, TRUE);
                }
            }
            else {
                miLineJoin (pdc, spanData, &leftFace, &prevRightFace);
            }
            prevRightFace = rightFace;
            first = FALSE;
            projectLeft = FALSE;
        }

        if (npt == 1 && somethingDrawn) {
            if (selfJoin)
                miLineJoin (pdc, spanData, &firstFace, &rightFace);
            else if (pdc->pen_cap_style == PT_CAP_ROUND) {
                if (pdc->pen_width == 1 && !spanData)
                    miLineOnePoint (pdc, spanData, x2, y2);
                else
                    miLineArc (pdc, spanData, (LineFacePtr) NULL, &rightFace,
                               (double)0.0, (double)0.0, TRUE);
            }
        }
    }

    /* handle crock where all points are coincedent */
    if (!somethingDrawn) {
        projectLeft = pdc->pen_cap_style == PT_CAP_PROJECTING;
        miWideSegment (pdc, spanData, x2, y2, x2, y2, projectLeft, projectLeft,
                       &leftFace, &rightFace);

        if (pdc->pen_cap_style == PT_CAP_ROUND) {
            miLineArc (pdc, spanData, &leftFace, (LineFacePtr) NULL,
                       (double)0.0, (double)0.0, TRUE);
            rightFace.dx = -1;        /* sleezy hack to make it work */
            miLineArc (pdc, spanData, (LineFacePtr) NULL, &rightFace,
                        (double)0.0, (double)0.0, TRUE);
        }
    }

    if (spanData)
        miCleanupSpanData (pdc, spanData);
}

#define V_TOP           0
#define V_RIGHT         1
#define V_BOTTOM        2
#define V_LEFT          3

static void miWideDashSegment (PDC pdc, SpanDataPtr spanData, int* pDashOffset, int* pDashIndex,
            int x1, int y1, int x2, int y2, BOOL projectLeft, BOOL projectRight, 
            LineFacePtr leftFace, LineFacePtr rightFace)
{
    int             dashIndex, dashRemain;
    const unsigned char*  pDash;
    double          L, l;
    double          k;
    PolyVertexRec   vertices[4];
    PolyVertexRec   saveRight={0,0}, saveBottom={0,0};
    PolySlopeRec    slopes[4];
    PolyEdgeRec     left[2], right[2];
    LineFaceRec     lcapFace, rcapFace;
    int             nleft, nright;
    int             h;
    int             y;
    int             dy, dx;
    double          LRemain;
    double          r;
    double          rdx, rdy;
    double          dashDx, dashDy;
    double          saveK = 0.0;
    BOOL            first = TRUE;
    double          lcenterx, lcentery, rcenterx = 0.0, rcentery = 0.0;
    
    dx = x2 - x1;
    dy = y2 - y1;
    dashIndex = *pDashIndex;
    pDash = pdc->dash_list;
    dashRemain = pDash[dashIndex] - *pDashOffset;

#if 0
    gal_pixel       fgPixel;
    gal_pixel       bgPixel;

    fgPixel = pdc->pencolor;
    bgPixel = pdc->bkcolor;
    if (pdc->brush_type == BT_OPAQUE_STIPPLED || pdc->brush_type == BT_TILED) {
        bgPixel = fgPixel;
    }
#endif

    l = ((double) pdc->pen_width) / 2.0;
    if (dx == 0) {
        L = dy;
        rdx = 0;
        rdy = l;
        if (dy < 0) {
            L = -dy;
            rdy = -l;
        }
    }
    else if (dy == 0) {
        L = dx;
        rdx = l;
        rdy = 0;
        if (dx < 0) {
            L = -dx;
            rdx = -l;
        }
    }
    else {
        L = hypot ((double) dx, (double) dy);
        r = l / L;

        rdx = r * dx;
        rdy = r * dy;
    }
    k = l * L;
    LRemain = L;

    /* All position comments are relative to a line with dx and dy > 0,
     * but the code does not depend on this */
    /* top */
    slopes[V_TOP].dx = dx;
    slopes[V_TOP].dy = dy;
    slopes[V_TOP].k = k;
    /* right */
    slopes[V_RIGHT].dx = -dy;
    slopes[V_RIGHT].dy = dx;
    slopes[V_RIGHT].k = 0;
    /* bottom */
    slopes[V_BOTTOM].dx = -dx;
    slopes[V_BOTTOM].dy = -dy;
    slopes[V_BOTTOM].k = k;
    /* left */
    slopes[V_LEFT].dx = dy;
    slopes[V_LEFT].dy = -dx;
    slopes[V_LEFT].k = 0;

    /* preload the start coordinates */
    vertices[V_RIGHT].x = vertices[V_TOP].x = rdy;
    vertices[V_RIGHT].y = vertices[V_TOP].y = -rdx;

    vertices[V_BOTTOM].x = vertices[V_LEFT].x = -rdy;
    vertices[V_BOTTOM].y = vertices[V_LEFT].y = rdx;

    if (projectLeft) {
        vertices[V_TOP].x -= rdx;
        vertices[V_TOP].y -= rdy;

        vertices[V_LEFT].x -= rdx;
        vertices[V_LEFT].y -= rdy;

        slopes[V_LEFT].k = rdx * dx + rdy * dy;
    }

    lcenterx = x1;
    lcentery = y1;

    if (pdc->pen_cap_style == PT_CAP_ROUND) {
        lcapFace.dx = dx;
        lcapFace.dy = dy;
        lcapFace.x = x1;
        lcapFace.y = y1;

        rcapFace.dx = -dx;
        rcapFace.dy = -dy;
        rcapFace.x = x1;
        rcapFace.y = y1;
    }

    while (LRemain > dashRemain) {
        dashDx = (dashRemain * dx) / L;
        dashDy = (dashRemain * dy) / L;

        rcenterx = lcenterx + dashDx;
        rcentery = lcentery + dashDy;

        vertices[V_RIGHT].x += dashDx;
        vertices[V_RIGHT].y += dashDy;

        vertices[V_BOTTOM].x += dashDx;
        vertices[V_BOTTOM].y += dashDy;

        slopes[V_RIGHT].k = vertices[V_RIGHT].x * dx + vertices[V_RIGHT].y * dy;

        if (pdc->pen_type == PT_DOUBLE_DASH || !(dashIndex & 1)) {

            if (pdc->pen_type == PT_ON_OFF_DASH &&
                 pdc->pen_cap_style == PT_CAP_PROJECTING) {

                saveRight = vertices[V_RIGHT];
                saveBottom = vertices[V_BOTTOM];
                saveK = slopes[V_RIGHT].k;
                
                if (!first) {
                    vertices[V_TOP].x -= rdx;
                    vertices[V_TOP].y -= rdy;
    
                    vertices[V_LEFT].x -= rdx;
                    vertices[V_LEFT].y -= rdy;

                    slopes[V_LEFT].k = vertices[V_LEFT].x * slopes[V_LEFT].dy -
                                       vertices[V_LEFT].y * slopes[V_LEFT].dx;
                }
                
                vertices[V_RIGHT].x += rdx;
                vertices[V_RIGHT].y += rdy;

                vertices[V_BOTTOM].x += rdx;
                vertices[V_BOTTOM].y += rdy;

                slopes[V_RIGHT].k = vertices[V_RIGHT].x * slopes[V_RIGHT].dy -
                                   vertices[V_RIGHT].y * slopes[V_RIGHT].dx;
            }

            y = miPolyBuildPoly (vertices, slopes, 4, x1, y1,
                                      left, right, &nleft, &nright, &h);
            miFillPolyHelper (pdc, spanData, y, h, left, right, nleft, nright, dashIndex & 1);

            if (pdc->pen_width == PT_ON_OFF_DASH) {
                switch (pdc->pen_cap_style) {
                case PT_CAP_PROJECTING:
                    vertices[V_BOTTOM] = saveBottom;
                    vertices[V_RIGHT] = saveRight;
                    slopes[V_RIGHT].k = saveK;
                    break;

                case PT_CAP_ROUND:
                    if (!first) {
                        if (dx < 0) {
                            lcapFace.xa = -vertices[V_LEFT].x;
                            lcapFace.ya = -vertices[V_LEFT].y;
                            lcapFace.k = slopes[V_LEFT].k;
                        }
                        else {
                            lcapFace.xa = vertices[V_TOP].x;
                            lcapFace.ya = vertices[V_TOP].y;
                            lcapFace.k = -slopes[V_LEFT].k;
                        }
                        miLineArc (pdc, spanData, &lcapFace, (LineFacePtr) NULL,
                                          lcenterx, lcentery, FALSE);
                    }

                    if (dx < 0) {
                        rcapFace.xa = vertices[V_BOTTOM].x;
                        rcapFace.ya = vertices[V_BOTTOM].y;
                        rcapFace.k = slopes[V_RIGHT].k;
                    }
                    else {
                        rcapFace.xa = -vertices[V_RIGHT].x;
                        rcapFace.ya = -vertices[V_RIGHT].y;
                        rcapFace.k = -slopes[V_RIGHT].k;
                    }
                    miLineArc (pdc, spanData, (LineFacePtr) NULL, &rcapFace,
                               rcenterx, rcentery, FALSE);
                    break;

                default:
                  break;
                    }
            }
        }

        LRemain -= dashRemain;
        ++dashIndex;
        if (dashIndex == pdc->dash_list_len)
            dashIndex = 0;
        dashRemain = pDash[dashIndex];

        lcenterx = rcenterx;
        lcentery = rcentery;

        vertices[V_TOP] = vertices[V_RIGHT];
        vertices[V_LEFT] = vertices[V_BOTTOM];
        slopes[V_LEFT].k = -slopes[V_RIGHT].k;
        first = FALSE;
    }

    if (pdc->pen_type == PT_DOUBLE_DASH || !(dashIndex & 1)) {
        vertices[V_TOP].x -= dx;
        vertices[V_TOP].y -= dy;

        vertices[V_LEFT].x -= dx;
        vertices[V_LEFT].y -= dy;

        vertices[V_RIGHT].x = rdy;
        vertices[V_RIGHT].y = -rdx;

        vertices[V_BOTTOM].x = -rdy;
        vertices[V_BOTTOM].y = rdx;

        if (projectRight) {
            vertices[V_RIGHT].x += rdx;
            vertices[V_RIGHT].y += rdy;
    
            vertices[V_BOTTOM].x += rdx;
            vertices[V_BOTTOM].y += rdy;
            slopes[V_RIGHT].k = vertices[V_RIGHT].x *
                                slopes[V_RIGHT].dy -
                                vertices[V_RIGHT].y *
                                slopes[V_RIGHT].dx;
        }
        else
            slopes[V_RIGHT].k = 0;

        if (!first && pdc->pen_type == PT_ON_OFF_DASH &&
            pdc->pen_cap_style == PT_CAP_PROJECTING) {
            vertices[V_TOP].x -= rdx;
            vertices[V_TOP].y -= rdy;

            vertices[V_LEFT].x -= rdx;
            vertices[V_LEFT].y -= rdy;
            slopes[V_LEFT].k = vertices[V_LEFT].x *
                               slopes[V_LEFT].dy -
                               vertices[V_LEFT].y *
                               slopes[V_LEFT].dx;
        }
        else
            slopes[V_LEFT].k += dx * dx + dy * dy;

        y = miPolyBuildPoly (vertices, slopes, 4, x2, y2,
                             left, right, &nleft, &nright, &h);

        miFillPolyHelper (pdc, spanData, y, h, left, right, nleft, nright, dashIndex & 1);

        if (!first && pdc->pen_type == PT_ON_OFF_DASH &&
            pdc->pen_cap_style == PT_CAP_ROUND) {
            lcapFace.x = x2;
            lcapFace.y = y2;

            if (dx < 0) {
                lcapFace.xa = -vertices[V_LEFT].x;
                lcapFace.ya = -vertices[V_LEFT].y;
                lcapFace.k = slopes[V_LEFT].k;
            }
            else {
                lcapFace.xa = vertices[V_TOP].x;
                lcapFace.ya = vertices[V_TOP].y;
                lcapFace.k = -slopes[V_LEFT].k;
            }
            miLineArc (pdc, spanData, &lcapFace, (LineFacePtr) NULL,
                       rcenterx, rcentery, FALSE);
        }
    }

    dashRemain = ((double) dashRemain) - LRemain;
    if (dashRemain == 0) {
        dashIndex++;
        if (dashIndex == pdc->dash_list_len)
            dashIndex = 0;
        dashRemain = pDash[dashIndex];
    }

    leftFace->x = x1;
    leftFace->y = y1;
    leftFace->dx = dx;
    leftFace->dy = dy;
    leftFace->xa = rdy;
    leftFace->ya = -rdx;
    leftFace->k = k;

    rightFace->x = x2;
    rightFace->y = y2;
    rightFace->dx = -dx;
    rightFace->dy = -dy;
    rightFace->xa = -rdy;
    rightFace->ya = rdx;
    rightFace->k = k;

    *pDashIndex = dashIndex;
    *pDashOffset = pDash[dashIndex] - dashRemain;
}

void miWideDash (PDC pdc, register int npt, register POINT* pPts)
{
    int             x1, y1, x2, y2;
    BOOL            projectLeft, projectRight;
    LineFaceRec     leftFace, rightFace, prevRightFace;
    LineFaceRec     firstFace;
    int             first;
    int             dashIndex, dashOffset;
    register int    prevDashIndex;
    SpanDataRec     spanDataRec;
    SpanDataPtr     spanData;
    BOOL            somethingDrawn = FALSE;
    BOOL            selfJoin;
    BOOL            endIsFg = FALSE, startIsFg = FALSE;
    BOOL            firstIsFg = FALSE, prevIsFg = FALSE;

    if (npt == 0)
        return;

    if (pdc->pen_type == PT_DOUBLE_DASH && 
        (pdc->brush_type == BT_OPAQUE_STIPPLED || pdc->brush_type == BT_TILED)) {
        miWideLine (pdc, npt, pPts);
        return;
    }

    spanData = miSetupSpanData (pdc, &spanDataRec, npt);

    x2 = pPts->x;
    y2 = pPts->y;
    first = TRUE;
    selfJoin = FALSE;
    if (x2 == pPts[npt-1].x && y2 == pPts[npt-1].y) {
        selfJoin = TRUE;
    }

    projectLeft = pdc->pen_cap_style == PT_CAP_PROJECTING && !selfJoin;
    projectRight = FALSE;
    dashIndex = 0;
    dashOffset = 0;
    miStepDash (pdc->dash_offset, &dashIndex, pdc->dash_list, pdc->dash_list_len, &dashOffset);

    while (--npt) {
        x1 = x2;
        y1 = y2;
        ++pPts;
        x2 = pPts->x;
        y2 = pPts->y;

        if (x1 != x2 || y1 != y2) {

            somethingDrawn = TRUE;
            if (npt == 1 && pdc->pen_cap_style == PT_CAP_PROJECTING && (!selfJoin || !firstIsFg))
                projectRight = TRUE;

            prevDashIndex = dashIndex;
            miWideDashSegment (pdc, spanData, &dashOffset, &dashIndex,
                                x1, y1, x2, y2,
                                projectLeft, projectRight, &leftFace, &rightFace);
            startIsFg = !(prevDashIndex & 1);
            endIsFg = (dashIndex & 1) ^ (dashOffset != 0);

            if (pdc->pen_type == PT_DOUBLE_DASH || startIsFg) {
                pdc->cur_pixel = startIsFg ? pdc->pencolor : pdc->bkcolor;
                if (first || (pdc->pen_type == PT_ON_OFF_DASH && !prevIsFg)) {
                    if (first && selfJoin) {
                        firstFace = leftFace;
                        firstIsFg = startIsFg;
                    }
                    else if (pdc->pen_cap_style == PT_CAP_ROUND)
                        miLineArc (pdc, spanData, &leftFace, (LineFacePtr) NULL,
                                          (double)0.0, (double)0.0, TRUE);
                }
                else {
                    miLineJoin (pdc, spanData, &leftFace, &prevRightFace);
                }
            }
            prevRightFace = rightFace;
            prevIsFg = endIsFg;
            first = FALSE;
            projectLeft = FALSE;
        }

        if (npt == 1 && somethingDrawn) {
            if (pdc->pen_type == PT_DOUBLE_DASH || endIsFg) {
                pdc->cur_pixel = endIsFg ? pdc->pencolor : pdc->bkcolor;
                if (selfJoin && (pdc->pen_type == PT_DOUBLE_DASH || firstIsFg)) {
                    miLineJoin (pdc, spanData, &firstFace, &rightFace);
                }
                else {
                    if (pdc->pen_cap_style == PT_CAP_ROUND)
                        miLineArc (pdc, spanData, (LineFacePtr) NULL, &rightFace,
                                    (double)0.0, (double)0.0, TRUE);
                }
            }
            else {
                /* glue a cap to the start of the line if
                 * we're OnOffDash and ended on odd dash
                 */
                if (selfJoin && firstIsFg) {
                    pdc->cur_pixel = pdc->pencolor;
                    if (pdc->pen_cap_style == PT_CAP_PROJECTING)
                        miLineProjectingCap (pdc, spanData, &firstFace, TRUE,
                                    (double)0.0, (double)0.0, TRUE);
                    else if (pdc->pen_cap_style == PT_CAP_ROUND)
                        miLineArc (pdc, spanData, &firstFace, (LineFacePtr) NULL,
                                    (double)0.0, (double)0.0, TRUE);
                }
            }
        }
    }

    /* handle crock where all points are coincident */
    if (!somethingDrawn && (pdc->pen_type == PT_DOUBLE_DASH || !(dashIndex & 1))) {
        /* not the same as endIsFg computation above */
        pdc->cur_pixel = (dashIndex & 1) ? pdc->bkcolor : pdc->pencolor;
        switch (pdc->pen_cap_style) {
        case PT_CAP_ROUND:
            miLineArc (pdc, spanData, (LineFacePtr) NULL, (LineFacePtr) NULL,
                       (double)x2, (double)y2, FALSE);
            break;

        case PT_CAP_PROJECTING:
            x1 = pdc->pen_width;
            miFillRectPolyHelper (pdc, spanData, x2 - (x1 >> 1), y2 - (x1 >> 1), x1, x1);
            break;
        default:
          break;
        }
    }
    if (spanData)
        miCleanupSpanData (pdc, spanData);
}

#endif /* _MGHAVE_ADV_2DAPI */

