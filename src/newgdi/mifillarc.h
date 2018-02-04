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

/************************************************************
Copyright 1989, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

********************************************************/

/* $XFree86: xc/programs/Xserver/mi/mifillarc.h,v 3.3 1998/10/04 09:39:27 dawes Exp $ */
/* $TOG: mifillarc.h /main/11 1998/02/09 14:46:57 kaleb $ */

#ifndef MIFILLARC_H
#define MIFILLARC_H 1

#define FULLCIRCLE (360 * 64)

typedef struct _miFillArc {
    int xorg, yorg;
    int y;
    int dx, dy;
    int e;
    int ym, yk, xm, xk;
} miFillArcRec;

/* could use 64-bit integers */
typedef struct _miFillArcD {
    int xorg, yorg;
    int y;
    int dx, dy;
    double e;
    double ym, yk, xm, xk;
} miFillArcDRec;

#define miFillArcEmpty(arc) (!(arc)->angle2 || \
			     !(arc)->width || !(arc)->height || \
			     (((arc)->width == 1) && ((arc)->height & 1)))

#define miCanFillArc(arc) (((arc)->width == (arc)->height) || \
			   (((arc)->width <= 800) && ((arc)->height <= 800)))

#define MIFILLARCSETUP() \
    x = 0; \
    y = info.y; \
    e = info.e; \
    xk = info.xk; \
    xm = info.xm; \
    yk = info.yk; \
    ym = info.ym; \
    dx = info.dx; \
    dy = info.dy; \
    xorg = info.xorg; \
    yorg = info.yorg

#define MIFILLARCSTEP(slw) \
    e += yk; \
    while (e >= 0) \
    { \
	x++; \
	xk -= xm; \
	e += xk; \
    } \
    y--; \
    yk -= ym; \
    slw = (x << 1) + dx; \
    if ((e == xk) && (slw > 1)) \
	slw--

#define MIFILLCIRCSTEP(slw) MIFILLARCSTEP(slw)
#define MIFILLELLSTEP(slw) MIFILLARCSTEP(slw)

#define miFillArcLower(slw) (((y + dy) != 0) && ((slw > 1) || (e != xk)))

typedef struct _miSliceEdge {
    int	    x;
    int     stepx;
    int	    deltax;
    int	    e;
    int	    dy;
    int	    dx;
} miSliceEdgeRec, *miSliceEdgePtr;

typedef struct _miArcSlice {
    miSliceEdgeRec edge1, edge2;
    int min_top_y, max_top_y;
    int min_bot_y, max_bot_y;
    BOOL edge1_top, edge2_top;
    BOOL flip_top, flip_bot;
} miArcSliceRec;

#define MIARCSLICESTEP(edge) \
    edge.x -= edge.stepx; \
    edge.e -= edge.dx; \
    if (edge.e <= 0) \
    { \
	edge.x -= edge.deltax; \
	edge.e += edge.dy; \
    }

#define miFillSliceUpper(slice) \
		((y >= slice.min_top_y) && (y <= slice.max_top_y))

#define miFillSliceLower(slice) \
		((y >= slice.min_bot_y) && (y <= slice.max_bot_y))

#define MIARCSLICEUPPER(xl,xr,slice,slw) \
    xl = xorg - x; \
    xr = xl + slw - 1; \
    if (slice.edge1_top && (slice.edge1.x < xr)) \
	xr = slice.edge1.x; \
    if (slice.edge2_top && (slice.edge2.x > xl)) \
	xl = slice.edge2.x;

#define MIARCSLICELOWER(xl,xr,slice,slw) \
    xl = xorg - x; \
    xr = xl + slw - 1; \
    if (!slice.edge1_top && (slice.edge1.x > xl)) \
	xl = slice.edge1.x; \
    if (!slice.edge2_top && (slice.edge2.x < xr)) \
	xr = slice.edge2.x;

#define MIWIDEARCSETUP(x,y,dy,slw,e,xk,xm,yk,ym) \
    x = 0; \
    y = slw >> 1; \
    yk = y << 3; \
    xm = 8; \
    ym = 8; \
    if (dy) \
    { \
	xk = 0; \
	if (slw & 1) \
	    e = -1; \
	else \
	    e = -(y << 2) - 2; \
    } \
    else \
    { \
	y++; \
	yk += 4; \
	xk = -4; \
	if (slw & 1) \
	    e = -(y << 2) - 3; \
	else \
	    e = - (y << 3); \
    }

#define MIFILLINARCSTEP(slw) \
    ine += inyk; \
    while (ine >= 0) \
    { \
	inx++; \
	inxk -= inxm; \
	ine += inxk; \
    } \
    iny--; \
    inyk -= inym; \
    slw = (inx << 1) + dx; \
    if ((ine == inxk) && (slw > 1)) \
	slw--

#define miFillInArcLower(slw) (((iny + dy) != 0) && \
			       ((slw > 1) || (ine != inxk)))

/* extern int miFreeArcCache (void* data, unsigned int id); */
extern void mg_miFreeArcCache (void);
extern struct finalSpan *realAllocSpan (void);
extern void miFillArcSetup (register miArc* arc, register miFillArcRec* info);
extern void miFillArcDSetup (register miArc* arc, register miFillArcDRec* info);
extern void miEllipseAngleToSlope (int angle, int width, int height, int* dxp, int* dyp, double* d_dxp, double* d_dyp);
extern void miFillArcSliceSetup (register miArc* arc, register miArcSliceRec* slice, PDC pdc);

#endif
