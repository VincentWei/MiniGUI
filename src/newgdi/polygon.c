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
** polygon.c: monoton vertical polygon and general polygon generators.
**
**      Monotone vertical polygon generator comes from 
**      "Michael Abrash's Graphics Programming Black Book Special Edition"
**      by Michael Abrash.
**
**      General polygon generator comes from Allegro by 
**      Shawn Hargreaves and others. 
**      Thank them for their great work and good copyright statement.
**
**      "Allegro is a gift-software"
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/10/31.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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
#include "pixel_ops.h"
#include "cursor.h"
#include "polygon.h"

/* 
 * Returns TRUE if polygon described by passed-in vertex list is 
 * monotone with respect to a vertical line, FALSE otherwise. 
 * Doesn't matter if polygon is simple (non-self-intersecting) or not.
 */

#define SIGNUM(a) ((a>0)?1:((a<0)?-1:0))

BOOL GUIAPI PolygonIsMonotoneVertical (const POINT* pts, int vertices)
{
    int i, delta_y, prev_delta_y;
    int nr_yreversals = 0;

    /* Three or fewer points can't make a non-vertical-monotone polygon */
    if (vertices < 4)
        return TRUE;

    /* The polygon must be a closed one */
    if (pts[0].x != pts[vertices-1].x || pts[0].y != pts[vertices-1].y)
        return FALSE;

    /* Scan to the first non-horizontal edge */
    prev_delta_y = SIGNUM(pts[vertices-1].y - pts[0].y);
    i = 0;
    while ((prev_delta_y == 0) && (i < (vertices-1))) {
        prev_delta_y = SIGNUM(pts[i].y - pts[i+1].y);
        i++;
    }

    if (i == (vertices-1))
        return TRUE;  /* polygon is a flat line */

    /* 
     * Now count Y reversals. Might miss one reversal, at the last vertex, 
     * but because reversal counts must be even, being off by one 
     * isn't a problem 
     */
    do {
        if ((delta_y = SIGNUM(pts[i].y - pts[i+1].y)) != 0) {
            if (delta_y != prev_delta_y) {
                /* Switched Y direction; not vertical-monotone if
                   reversed Y direction as many as three times */
                if (++nr_yreversals > 2)
                    return FALSE;
                prev_delta_y = delta_y;
            }
        }
    } while (i++ < (vertices-1));

   return TRUE;  /* it's a vertical-monotone polygon */
}

typedef struct _hline {
    int x1, x2;
} HLINE;

/* 
 * Scan converts an edge from (x1, y1) to (x2, y2), not including 
 * the point at (x2, y2). This avoids overlapping the end of 
 * one line with the start of the next, and causes the bottom 
 * scan line of the polygon not to be drawn. If skip_first != 0, 
 * the point at (x1, y1) isn't drawn. For each scan line, 
 * the pixel closest to the scanned line without being to the 
 * left of the scanned line is chosen
 */ 

static void scan_edge (int x1, int y1, int x2, int y2, int set_x1, int skip_first, HLINE** hlines_p)
{
#ifdef _MGHAVE_FIXED_MATH
    int y, delta_x, delta_y, dir;
    fixed inverse_slope;
    HLINE* hlines;

    /* Calculate x and y lengths of the line and the inverse slope */
    delta_x = x2 - x1;
    if ((delta_y = y2 - y1) <= 0)
        return;     /* guard against 0-length and horizontal edges */
    if (delta_x < 0) {
        dir = -1;
        inverse_slope = fixdiv (itofix (-delta_x), itofix (delta_y));
    }
    else {
        dir = 1;
        inverse_slope = fixdiv (itofix (delta_x), itofix (delta_y));
    }

    /* 
     * Store the x coordinate of the pixel closest to but not to 
     * the left of the line for each y coordinate between y1 and y2, 
     * not including y2 and also not including y1 if skip_first != 0 
     */

    hlines = *hlines_p;
    for (y = y1 + skip_first; y < y2; y++, hlines++) {
        if (set_x1)
            hlines->x1 = x1 + fixceil (fixmul (itofix (y-y1), inverse_slope)) * dir;
        else
            hlines->x2 = x1 + fixceil (fixmul (itofix (y-y1), inverse_slope)) * dir;
    }
    *hlines_p = hlines;
#else
    int y, delta_x, delta_y;
    double inverse_slope;
    HLINE* hlines;

    /* Calculate x and y lengths of the line and the inverse slope */
    delta_x = x2 - x1;
    if ((delta_y = y2 - y1) <= 0)
        return;     /* guard against 0-length and horizontal edges */
    inverse_slope = (double)delta_x / (double)delta_y;

    /* 
     * Store the x coordinate of the pixel closest to but not to 
     * the left of the line for each y coordinate between y1 and y2, 
     * not including y2 and also not including y1 if skip_first != 0 
     */

    hlines = *hlines_p;
    for (y = y1 + skip_first; y < y2; y++, hlines++) {
        if (set_x1)
            hlines->x1 = x1 + (int)ceil ((y-y1) * inverse_slope);
        else
            hlines->x2 = x1 + (int)ceil ((y-y1) * inverse_slope);
    }
    *hlines_p = hlines;
#endif
}

/* 
 * "Monoton vertical" means "monotone with respect to a vertical line"; 
 * that is, every horizontal line drawn through the polygon at any point 
 * would cross exactly two active edges (neither horizontal lines 
 * nor zero-length edges count as active edges; both are acceptable 
 * anywhere in the polygon). Right & left edges may cross (polygons may be nonsimple). 
 * Polygons that are not convex according to this definition won't be drawn properly. 
 */

/* Advances the index by one vertex forward through the vertex list,
wrapping at the end of the list */
#define INDEX_FORWARD(Index) \
    Index = (Index + 1) % vertices;

/* Advances the index by one vertex backward through the vertex list,
wrapping at the start of the list */
#define INDEX_BACKWARD(Index) \
    Index = (Index - 1 + vertices) % vertices;

/* Advances the index by one vertex either forward or backward through
the vertex list, wrapping at either end of the list */
#define INDEX_MOVE(Index,Direction)                 \
    if (Direction > 0)                              \
        Index = (Index + 1) % vertices;             \
    else                                            \
        Index = (Index - 1 + vertices) % vertices;

/* Monotone vertical polygon generator */
BOOL GUIAPI MonotoneVerticalPolygonGenerator (void* context, const POINT* pts, int vertices, CB_POLYGON cb)
{
    int i, min_index, max_index, min_point_y, max_point_y;
    int cur_index, prev_index;
    HLINE *hlines, *working_hlines;
    int nr_hlines;

    if (vertices < 3)
        return TRUE;  /* reject null polygons */

    /* Scan the list to find the top and bottom of the polygon */
    max_point_y = min_point_y = pts[min_index = max_index = 0].y;
    for (i = 1; i < vertices; i++) {
        if (pts[i].y < min_point_y)
            min_point_y = pts[min_index = i].y; /* new top */
        else if (pts[i].y > max_point_y)
            max_point_y = pts[max_index = i].y; /* new bottom */
    }

    /* Set the # of scan lines in the polygon, skipping the bottom edge */
    if ((nr_hlines = max_point_y - min_point_y) <= 0) {
        return TRUE;  /* there's nothing to draw, so we're done */
    }

    /* Get memory in which to store the line list we generate */
    if ((hlines = (HLINE*) (malloc (sizeof (HLINE) * nr_hlines))) == NULL) {
        return FALSE;  /* couldn't get memory for the line list */
    }

    /* Scan the first edge and store the boundary points in the list */
    /* Initial pointer for storing scan converted first-edge coords */
    working_hlines = hlines;
    prev_index = cur_index = min_index;
    /* Start from the top of the first edge */
    /* Scan convert each line in the first edge from top to bottom */
    do {
        INDEX_BACKWARD(cur_index);
        scan_edge (pts[prev_index].x, pts[prev_index].y,
                   pts[cur_index].x, pts[cur_index].y, 1, 0, &working_hlines);
        prev_index = cur_index;
    } while (cur_index != max_index);

    /* Scan the second edge and store the boundary points in the list */
    working_hlines = hlines;
    prev_index = cur_index = min_index;
    /* Scan convert the second edge, top to bottom */
    do {
        INDEX_FORWARD(cur_index);
        scan_edge (pts[prev_index].x, pts[prev_index].y,
                   pts[cur_index].x, pts[cur_index].y, 0, 0, &working_hlines);
        prev_index = cur_index;
    } while (cur_index != max_index);

    for (i = 0; i < nr_hlines; i++) {
        cb (context, hlines [i].x1, hlines [i].x2, i + min_point_y);
    }

    /* Release the line list's memory and we're successfully done */
    free (hlines);

    return TRUE;
}

#ifdef POLYGON_64_SUPPORT
static void Neg64( POLYGON_Int64 *x )
{
    /* Remember that -(0x80000000) == 0x80000000 with 2-complement! */
    /* We take care of that here.                                   */

    x->hi ^= 0xFFFFFFFFUL;
    x->lo ^= 0xFFFFFFFFUL;
    x->lo++;

    if ( !x->lo )
    {
        x->hi++;
        if ( x->hi == 0x80000000UL )  /* Check -MaxInt32 - 1 */
        {
            x->lo--;
            x->hi--;  /* We return 0x7FFFFFFF! */
        }
    }
}

static POLYGON_Int32 Div64by32( POLYGON_Int64 *x, POLYGON_Int32 y )
{
    POLYGON_Int64 tmp;
    POLYGON_Int32 s;
    POLYGON_Word32 q, r, i, lo;

    tmp.hi = x->hi;
    tmp.lo = x->lo;

    s  = tmp.hi; 
    if ( s < 0 ) 
        Neg64( &tmp );
    s ^= y;
    y = ABS( y );

    /* Shortcut */
    if ( tmp.hi == 0 ){
        q = tmp.lo / y;
        return ( s < 0 ) ? -(POLYGON_Int32)q : (POLYGON_Int32)q;
    }

    r  = tmp.hi;
    lo = tmp.lo;

    if ( r >= (POLYGON_Word32)y )   /* we know y is to be treated as unsigned here */
        return ( s < 0 ) ? 0x80000001UL : 0x7FFFFFFFUL;
                            /* Return Max/Min Int32 if divide overflow */
                            /* This includes division by zero!         */
    q = 0;
    for ( i = 0; i < 32; i++ ){
        r <<= 1;
        q <<= 1;
        r  |= lo >> 31;

        if ( r >= (POLYGON_Word32)y ){
            r -= y;
            q |= 1;
        }
        lo <<= 1;
    }
    return ( s < 0 ) ? -(POLYGON_Int32)q : (POLYGON_Int32)q;
}

void  Add64( POLYGON_Int64*  x, POLYGON_Int64*  y, POLYGON_Int64*  z )
{
    register POLYGON_Word32  lo, hi;

    lo = x->lo + y->lo;
    hi = x->hi + y->hi + ( lo < x->lo );

    z->lo = lo;
    z->hi = hi;
}

void  Sub64( POLYGON_Int64*  x, POLYGON_Int64*  y, POLYGON_Int64*  z )
{ 
    register POLYGON_Word32  lo, hi;

    lo = x->lo - y->lo; 
    hi = x->hi - y->hi - ( (POLYGON_Int32)lo < 0 );

    z->lo = lo;
    z->hi = hi; 
} 

void  MulTo64( POLYGON_Int32  x, POLYGON_Int32  y, POLYGON_Int64*  z )
{
    POLYGON_Int32   s;
    POLYGON_Word32  lo1, hi1, lo2, hi2, lo, hi, i1, i2;

    s  = x; x = ABS( x );
    s ^= y; y = ABS( y );

    lo1 = x & 0x0000FFFF;  hi1 = x >> 16;
    lo2 = y & 0x0000FFFF;  hi2 = y >> 16;

    lo = lo1*lo2;
    i1 = lo1*hi2;
    i2 = lo2*hi1;
    hi = hi1*hi2;

    /* Check carry overflow of i1 + i2 */
    if ( i2 )
    {
        if ( i1 >= (POLYGON_Word32)-(POLYGON_Int32)i2 )
            hi += 1L << 16;
        i1 += i2;
    }

    i2 = i1 >> 16;
    i1 = i1 << 16;
    /* Check carry overflow of i1 + lo */
    if ( i1 )
    {
        if ( lo >= (POLYGON_Word32)-(POLYGON_Int32)i1 )
            hi++;
        lo += i1;
    }

    hi += i2;

    z->lo = lo;
    z->hi = hi;

    if ( s < 0 )
        Neg64( z );
}

void Int64AddInt32(POLYGON_Int64 *x, POLYGON_Int32 y, POLYGON_Int64 *z)
{
    POLYGON_Int64 tmp;
    MulTo64(y, 1, &tmp);
    Add64(x, &tmp, z);
}

BOOL IsSmall(POLYGON_Int64 *x, POLYGON_Int32 x_off, POLYGON_Int64 *y, POLYGON_Int32 y_off)
{
    POLYGON_Int64 z1, z2, z3;
    int i;
    
    Int64AddInt32(x, x_off, &z1);
    Int64AddInt32(y, y_off, &z2);
    Sub64(&z1, &z2, &z3);
    i = Div64by32(&z3, 1);
    if (i<0)
        return TRUE;
    else
        return FALSE;
}
#endif

/* _fill_edge_structure:
 *  Polygon helper function: initialises an edge structure for the 2d
 *  rasteriser.
 */
void _fill_edge_structure (POLYGON_EDGE *edge, const int *i1, const int *i2)
{
#ifdef POLYGON_64_SUPPORT
    POLYGON_Int64 dx64;
#endif
    if (i2[1] < i1[1]) {
        const int *it;

        it = i1;
        i1 = i2;
        i2 = it;
    }

    edge->top = i1[1];
    edge->bottom = i2[1] - 1;
#ifndef POLYGON_64_SUPPORT
    edge->dx = ((i2[0] - i1[0]) << POLYGON_FIX_SHIFT) / (i2[1] - i1[1]);
    edge->x = (i1[0] << POLYGON_FIX_SHIFT) + (1<<(POLYGON_FIX_SHIFT-1)) - 1;
#else
    MulTo64(i2[0] - i1[0], 1<<POLYGON_FIX_SHIFT, &dx64);
    edge->dx = Div64by32(&dx64, (i2[1] - i1[1]));
    MulTo64(i1[0], 1<<POLYGON_FIX_SHIFT, &(edge->x));
#endif
    edge->prev = NULL;
    edge->next = NULL;

#ifndef POLYGON_64_SUPPORT
    if (edge->dx < 0)
        edge->x += MIN(edge->dx+(1<<POLYGON_FIX_SHIFT), 0);

    edge->w = MAX(ABS(edge->dx)-(1<<POLYGON_FIX_SHIFT), 0);
#else
    if (edge->dx < 0) 
        Int64AddInt32(&(edge->x), MIN(edge->dx+(1<<POLYGON_FIX_SHIFT), 0), &(edge->x));

    edge->w = MAX(ABS(edge->dx)-(1<<POLYGON_FIX_SHIFT), 0);
#endif
}

/* _add_edge:
 *  Adds an edge structure to a linked list, returning the new head pointer.
 */
POLYGON_EDGE *_add_edge (POLYGON_EDGE *list, POLYGON_EDGE *edge, int sort_by_x)
{
    POLYGON_EDGE *pos = list;
    POLYGON_EDGE *prev = NULL;

    if (sort_by_x) {
        while ( (pos) && 
#ifndef POLYGON_64_SUPPORT
                (pos->x + (pos->w + pos->dx) /2 ) <
                (edge->x + (edge->w + edge->dx)/2)){
#else
                IsSmall(&(pos->x), (pos->w + pos->dx)/2, 
                            &(edge->x), (edge->w + edge->dx)/2)) {
#endif
            prev = pos;
            pos = pos->next;
        }
    }
    else {
        while ((pos) && (pos->top < edge->top)) {
            prev = pos;
            pos = pos->next;
        }
    }

    edge->next = pos;
    edge->prev = prev;

    if (pos)
        pos->prev = edge;

    if (prev) {
        prev->next = edge;
        return list;
    }
    else
        return edge;
}

/* _remove_edge:
 *  Removes an edge structure from a list, returning the new head pointer.
 */

POLYGON_EDGE *_remove_edge(POLYGON_EDGE *list, POLYGON_EDGE *edge)
{
    if (edge->next) 
        edge->next->prev = edge->prev;

    if (edge->prev) {
        edge->prev->next = edge->next;
        return list;
    }
    else
        return edge->next;
}

/* General polygon generator */
BOOL GUIAPI PolygonGeneratorEx (void* context, const POINT* pts, int vertices,
        CB_POLYGON cb, RECT* rc_output)
{
    int c;
    int top = INT_MAX;
    int bottom = INT_MIN;
    const int *i1, *i2;
    void* _scratch_mem;
    const int* points = (int*) pts;
    POLYGON_EDGE *edge, *next_edge;
    POLYGON_EDGE *active_edges = NULL;
    POLYGON_EDGE *inactive_edges = NULL;

    /* allocate some space and fill the edge table */
    _scratch_mem = malloc (sizeof(POLYGON_EDGE) * vertices);
    if (!(edge = (POLYGON_EDGE *)_scratch_mem))
        return FALSE;

    i1 = points;
    i2 = points + (vertices-1) * 2;

    for (c=0; c<vertices; c++) {

        if (i1[1] != i2[1]) {
            _fill_edge_structure (edge, i1, i2);

            if (edge->bottom >= edge->top) {

                if (edge->top < top)
                    top = edge->top;

                if (edge->bottom > bottom)
                    bottom = edge->bottom;

                inactive_edges = _add_edge(inactive_edges, edge, FALSE);
                edge++;
            }
        }
        i2 = i1;
        i1 += 2;
    }

    /* if rc_output not NULL, the scanlines's range is 
     * limit in the RECT. */
    if(rc_output){
        _MG_PRINTF("rc_output:(%d,%d)\n", rc_output->top, rc_output->bottom);
        edge = inactive_edges;
        while ((edge)) {
            next_edge = edge->next;
            /* add the edge between the rc_ouput. */
            if(edge->top < rc_output->top 
                    && edge->bottom > rc_output->top) {
                inactive_edges = _remove_edge(inactive_edges, edge);
                active_edges = _add_edge(active_edges, edge, TRUE);
#ifndef POLYGON_64_SUPPORT
                edge->x += (edge->dx * (rc_output->top - edge->top));
#else
                Int64AddInt32(&(edge->x), edge->dx * (rc_output->top - edge->top), &(edge->x));
#endif
                _MG_PRINTF("start check newly active edes c=%d,top=%d,"
                        "bottom=%d.\n", c, edge->top, edge->bottom);
            }
            /* add the edge outside the rc_ouput. */
            else if(edge->bottom < rc_output->top 
                    || edge->top > rc_output->bottom){
                inactive_edges = _remove_edge(inactive_edges, edge);
                _MG_PRINTF("start delete edes c=%d,top=%d,"
                        "bottom=%d.\n", c, edge->top, edge->bottom);
            }
            edge = next_edge;
        }

        /* limit the scanlines inside the rc_ouput. */
        top = rc_output->top;
        bottom = rc_output->bottom;
    }

    /* for each scanline in the polygon... */
    for (c=top; c<=bottom; c++) {

        /* check for newly active edges */
        edge = inactive_edges;
        while ((edge) && (edge->top == c)) {
            next_edge = edge->next;
            inactive_edges = _remove_edge(inactive_edges, edge);
            active_edges = _add_edge(active_edges, edge, TRUE);
            _MG_PRINTF("check newly active edes c=%d,top=%d,"
                    "bottom=%d.\n", c, edge->top, edge->bottom);
            edge = next_edge;
        }

        /* draw horizontal line segments */
        edge = active_edges;
        while ((edge) && (edge->next)) {
#ifndef POLYGON_64_SUPPORT
            int x1 = edge->x>>POLYGON_FIX_SHIFT;
            int x2 = ((edge->next->x+edge->next->w)>>POLYGON_FIX_SHIFT);
#else
            int x1 = Div64by32(&(edge->x), 1<<POLYGON_FIX_SHIFT);
            POLYGON_Int64 tmp;
            int x2;
            Int64AddInt32(&(edge->next->x), (edge->next->w), &tmp);
            x2 = Div64by32(&tmp, 1<<POLYGON_FIX_SHIFT);
#endif
            cb (context, x1, x2, c); 
            edge = edge->next->next;
        }

        /* update edges, sorting and removing dead ones */
        edge = active_edges;
        while (edge) {
            next_edge = edge->next;
            if (c >= edge->bottom) {
                active_edges = _remove_edge(active_edges, edge);
                _MG_PRINTF("delete edes c=%d,top=%d,"
                        "bottom=%d.\n", c, edge->top, edge->bottom);
            }
            else {
#ifndef POLYGON_64_SUPPORT
                edge->x += edge->dx;
#else
                Int64AddInt32(&(edge->x), edge->dx, &(edge->x));
#endif
                while ((edge->prev) && 
#ifndef POLYGON_64_SUPPORT
                        (edge->x+edge->w/2 < edge->prev->x+edge->prev->w/2)) {
#else
                        IsSmall(&(edge->x), edge->w/2, &(edge->prev->x), edge->prev->w/2)) {
#endif
                    if (edge->next)
                        edge->next->prev = edge->prev;
                    edge->prev->next = edge->next;
                    edge->next = edge->prev;
                    edge->prev = edge->prev->prev;
                    edge->next->prev = edge;
                    if (edge->prev)
                        edge->prev->next = edge;
                    else
                        active_edges = edge;
                }
            }
            edge = next_edge;
        }
    }

    free (_scratch_mem);
    return TRUE;
}

static POINT* convert_vertices (PDC pdc, const POINT* pts, int vertices, RECT* rcOutput)
{
    int i;
    POINT* points;

    if (vertices < 3) return NULL;

    if (!(points = malloc (sizeof (POINT) * vertices))) {
        return NULL;
    }

    /* Transfer logical to device to screen and find rcOutput here. */
    points [0] = pts [0];
    coor_LP2SP (pdc, &points [0].x, &points [0].y);
    rcOutput->left = rcOutput->right = points [0].x;
    rcOutput->top = rcOutput->bottom = points [0].y;

    for (i = 1; i < vertices; i++) {
        points [i] = pts [i];
        coor_LP2SP (pdc, &points [i].x, &points [i].y);
        if (points [i].x < rcOutput->left) {
            rcOutput->left = points [i].x;
        }
        else if (points [i].x > rcOutput->right) {
            rcOutput->right = points [i].x;
        }
        if (points [i].y < rcOutput->top) {
            rcOutput->top = points [i].y;
        }
        else if (points [i].y > rcOutput->bottom) {
            rcOutput->bottom = points [i].y;
        }
    }

    return points;
}

BOOL GUIAPI FillPolygon (HDC hdc, const POINT* pts, int vertices)
{
    PDC pdc;
    RECT rc_tmp;
    POINT* points;
    BOOL is_mv;

    if (vertices < 3) return FALSE;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return TRUE;

    is_mv = PolygonIsMonotoneVertical (pts, vertices);

    if (!(points = convert_vertices (pdc, pts, vertices, &pdc->rc_output)))
        return FALSE;

    pdc->cur_pixel = pdc->brushcolor;
    pdc->cur_ban = NULL;

    rc_tmp = pdc->rc_output;
    pdc->rc_output.right ++;
    pdc->rc_output.bottom ++;
    ENTER_DRAWING (pdc);

    if (rc_tmp.left == rc_tmp.right) {
        _dc_draw_vline_clip (pdc, rc_tmp.top, rc_tmp.bottom, rc_tmp.left);
    }
    else if (rc_tmp.top == rc_tmp.bottom) {
        _dc_draw_hline_clip (pdc, rc_tmp.left, rc_tmp.right, rc_tmp.top);
    }
    else {
#ifdef _MGHAVE_ADV_2DAPI
        if (pdc->brush_type == BT_SOLID) {
            if (is_mv)
                MonotoneVerticalPolygonGenerator (pdc, points, 
                                vertices, _dc_draw_hline_clip);
            else
                PolygonGeneratorEx(pdc, points, vertices, 
                        _dc_draw_hline_clip, &pdc->rc_output);
        }
        else {
            if (is_mv)
                MonotoneVerticalPolygonGenerator (pdc, points, 
                                vertices, _dc_fill_hline_clip);
            else
                PolygonGeneratorEx(pdc, points, vertices,
                        _dc_draw_hline_clip, &pdc->rc_output);
        }
#else
        if (is_mv)
            MonotoneVerticalPolygonGenerator (pdc, points, 
                            vertices, _dc_draw_hline_clip);
        else
            PolygonGeneratorEx(pdc, points, vertices, 
                    _dc_draw_hline_clip, &pdc->rc_output);
#endif
    }

    LEAVE_DRAWING (pdc);

    UNLOCK_GCRINFO (pdc);

    free (points);
    return TRUE;
}

