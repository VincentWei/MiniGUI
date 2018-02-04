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

/***********************************************************
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


Copyright 1989 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XFree86: xc/programs/Xserver/mi/mispans.c,v 3.1 1998/10/04 09:39:33 dawes Exp $ */
/* $TOG: mispans.c /main/7 1998/02/09 14:48:44 kaleb $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
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
#include "mispans.h"
#include "midc.h"

/*

These routines maintain lists of Spans, in order to implement the
``touch-each-pixel-once'' rules of wide lines and arcs.

Written by Joel McCormack, Summer 1989.

*/


void miInitSpanGroup(SpanGroup* spanGroup)
{
    spanGroup->size = 0;
    spanGroup->count = 0;
    spanGroup->group = NULL;
    spanGroup->ymin = SHRT_MAX;
    spanGroup->ymax = SHRT_MIN;
}

#define YMIN(spans) (spans->points[0].y)
#define YMAX(spans)  (spans->points[spans->count-1].y)

void miSubtractSpans (SpanGroup* spanGroup, Spans* sub)
{
    int         i, subCount, spansCount;
    int         ymin, ymax, xmin, xmax;
    Spans*      spans;
    Span*    subPt, *spansPt;
    int         extra;

    ymin = YMIN(sub);
    ymax = YMAX(sub);
    spans = spanGroup->group;
    for (i = spanGroup->count; i; i--, spans++) {
        if (YMIN(spans) <= ymax && ymin <= YMAX(spans)) {
            subCount = sub->count;
            subPt = sub->points;
            spansCount = spans->count;
            spansPt = spans->points;
            extra = 0;
            for (;;) {
                while (spansCount && spansPt->y < subPt->y) {
                    spansPt++; spansCount--;
                }
                if (!spansCount)
                    break;

                while (subCount && subPt->y < spansPt->y) {
                    subPt++; subCount--;
                }
                if (!subCount)
                    break;

                if (subPt->y == spansPt->y) {
                    xmin = subPt->x;
                    xmax = xmin + subPt->width;

                    if (xmin >= (spansPt->x + spansPt->width) || spansPt->x >= xmax) {
                        ;
                    }
                    else if (xmin <= spansPt->x) {
                        if (xmax >= (spansPt->x + spansPt->width)) {
                            memmove (spansPt, spansPt + 1, sizeof *spansPt * (spansCount - 1));
                            spansPt--;
                            spans->count--;
                            extra++;
                        }
                        else {
                          spansPt->width -= xmax - spansPt->x;
                          spansPt->x = xmax;
                        }
                    }
                    else {
                        if (xmax >= (spansPt->x + spansPt->width)) {
                          spansPt->width = xmin - spansPt->x;
                        }
                        else {
                            if (!extra) {
                                Span* newPt;

#define EXTRA 8
                                newPt = (Span*) realloc (spans->points, (spans->count + EXTRA) * sizeof (Span));
                                if (!newPt)
                                    break;
                                spansPt = newPt + (spansPt - spans->points);
                                spans->points = newPt;
                                extra = EXTRA;
                            }
                            memmove (spansPt + 1, spansPt, sizeof *spansPt * (spansCount));
                            spans->count++;
                            extra--;
                            spansPt->width = xmin - spansPt->x;
                            spansPt++;
                            spansPt->width -= xmax - spansPt->x;
                            spansPt->x = xmax;
                        }
                    }
                }

                spansPt++;
                spansCount--;
            }
        }
    }
}
    
void miAppendSpans(SpanGroup* spanGroup, SpanGroup* otherGroup, Spans* spans)
{
    register    int ymin, ymax;
    register    int spansCount;

    spansCount = spans->count;
    if (spansCount > 0) {
        if (spanGroup->size == spanGroup->count) {
            spanGroup->size = (spanGroup->size + 8) * 2;
            spanGroup->group = (Spans *) realloc(spanGroup->group, sizeof(Spans) * spanGroup->size);
         }

        spanGroup->group[spanGroup->count] = *spans;
        (spanGroup->count)++;

        ymin = spans->points[0].y;
        if (ymin < spanGroup->ymin)
            spanGroup->ymin = ymin;

        ymax = spans->points[spansCount - 1].y;
        if (ymax > spanGroup->ymax)
            spanGroup->ymax = ymax;

        if (otherGroup && otherGroup->ymin < ymax && ymin < otherGroup->ymax) {
            miSubtractSpans (otherGroup, spans);
        }
    }
    else {
        free (spans->points);
    }
} /* AppendSpans */

void miFreeSpanGroup(SpanGroup* spanGroup)
{
    if (spanGroup->group != NULL) {
        free(spanGroup->group);
        spanGroup->group = NULL;
    }
}

static void QuickSortSpansX (register Span* points, register int numSpans)
{
    register int        x;
    register int        i, j, m;
    register Span*   r;

/* Always called with numSpans > 1 */
/* Sorts only by x, as all y should be the same */

#define ExchangeSpans(a, b)                                     \
{                                                               \
    Span     tpt;                                            \
    tpt = points[a]; points[a] = points[b]; points[b] = tpt;    \
}

    do {
        if (numSpans < 9) {
            /* Do insertion sort */
            register int xprev;

            xprev = points[0].x;
            i = 1;
            do { /* while i != numSpans */
                x = points[i].x;
                if (xprev > x) {
                    /* points[i] is out of order.  Move into proper location. */
                    Span tpt;
                    int            k;

                    for (j = 0; x >= points[j].x; j++) {}
                    tpt = points[i];
                    for (k = i; k != j; k--) {
                        points[k] = points[k-1];
                    }
                    points[j] = tpt;
                    x = points[i].x;
                } /* if out of order */
                xprev = x;
                i++;
            } while (i != numSpans);
            return;
        }

        /* Choose partition element, stick in location 0 */
        m = numSpans / 2;
        if (points[m].x > points[0].x)
            ExchangeSpans(m, 0);
        if (points[m].x > points[numSpans-1].x)
            ExchangeSpans(m, numSpans-1);
        if (points[m].x > points[0].x)
            ExchangeSpans(m, 0);
        x = points[0].x;

        /* Partition array */
        i = 0;
        j = numSpans;
        do {
            r = &(points[i]);
            do {
                r++;
                i++;
            } while (i != numSpans && r->x < x);
            r = &(points[j]);
            do {
                r--;
                j--;
            } while (x < r->x);
            if (i < j) ExchangeSpans(i, j);
        } while (i < j);

        /* Move partition element back to middle */
        ExchangeSpans(0, j);

        /* Recurse */
        if (numSpans-j-1 > 1)
            QuickSortSpansX (&points[j+1], numSpans-j-1);
        numSpans = j;
    } while (numSpans > 1);
}

static int UniquifySpansX (Spans* spans, register Span* newPoints)
{
    register int    newx1, newx2, oldpt, i, y;
    Span            *oldPoints, *startNewPoints = newPoints;

/* Always called with numSpans > 1 */
/* Uniquify the spans, and stash them into newPoints and newWidths.  Return the
   number of unique spans. */

    oldPoints = spans->points;

    y = oldPoints->y;
    newx1 = oldPoints->x;
    newx2 = newx1 + oldPoints->width;

    for (i = spans->count-1; i != 0; i--) {
        oldPoints++;
        oldpt = oldPoints->x;
        if (oldpt > newx2) {
            /* Write current span, start a new one */
            newPoints->x = newx1;
            newPoints->y = y;
            newPoints->width = newx2 - newx1;
            newPoints++;
            newx1 = oldpt;
            newx2 = oldpt + oldPoints->width;
        } else {
            /* extend current span, if old extends beyond new */
            oldpt = oldpt + oldPoints->width;
            if (oldpt > newx2) newx2 = oldpt;
        }
    }

    /* Write final span */
    newPoints->x = newx1;
    newPoints->width = newx2 - newx1;
    newPoints->y = y;

    return (newPoints - startNewPoints) + 1;
}

void miDisposeSpanGroup (SpanGroup* spanGroup)
{
    int     i;
    Spans   *spans;

    for (i = 0; i < spanGroup->count; i++) {
        spans = spanGroup->group + i;
        free (spans->points);
    }
    spanGroup->count = 0;
}

void miFillUniqueSpanGroup (PDC pdc, SpanGroup* spanGroup, BOOL useBrush)
{
    register int    i;
    register Spans  *spans;
    register Spans  *yspans;
    register int    *ysizes;
    register int    ymin, ylength;

    /* Outgoing spans for one big call to FillSpans */
    register Span*  points;
    register int    count;

    if (spanGroup->count == 0)
        return;

    if (spanGroup->count == 1) {
        /* Already should be sorted, unique */
        spans = spanGroup->group;
        if (useBrush && pdc->brush_type != BT_SOLID && pdc->brush_stipple)
            _dc_fill_spans_brush (pdc, spans->points, spans->count, FALSE);
        else
            _dc_fill_spans (pdc, spans->points, spans->count, FALSE);
        free(spans->points);
    }
    else {
        /* Yuck.  Gross.  Radix sort into y buckets, then sort x and uniquify */
        /* This seems to be the fastest thing to do.  I've tried sorting on
           both x and y at the same time rather than creating into all those
           y buckets, but it was somewhat slower. */

        ymin    = spanGroup->ymin;
        ylength = spanGroup->ymax - ymin + 1;

        /* Allocate Spans for y buckets */
        yspans = (Spans *) malloc(ylength * sizeof(Spans));
        ysizes = (int *) malloc(ylength * sizeof (int));

        if (!yspans || !ysizes) {
            if (yspans)
                free (yspans);
            if (ysizes)
                free (ysizes);
            miDisposeSpanGroup (spanGroup);
            return;
        }
        
        for (i = 0; i != ylength; i++) {
            ysizes[i]        = 0;
            yspans[i].count  = 0;
            yspans[i].points = NULL;
        }

        /* Go through every single span and put it into the correct bucket */
        count = 0;
        for (i = 0, spans = spanGroup->group; i != spanGroup->count; i++, spans++) {
            int index;
            int j;

            for (j = 0, points = spans->points; j != spans->count; j++, points++) {
                index = points->y - ymin;
                if (index >= 0 && index < ylength) {
                    Spans *newspans = &(yspans[index]);
                    if (newspans->count == ysizes[index]) {
                        Span* newpoints;

                        ysizes[index] = (ysizes[index] + 8) * 2;
                        newpoints = (Span*) realloc( newspans->points, ysizes[index] * sizeof(Span));

                        if (!newpoints) {
                            int k;

                            for (k = 0; k < ylength; k++) {
                                free (yspans[k].points);
                            }

                            free (yspans);
                            free (ysizes);
                            miDisposeSpanGroup (spanGroup);
                            return;
                        }

                        newspans->points = newpoints;
                    }

                    newspans->points[newspans->count] = *points;
                    (newspans->count)++;
                } /* if y value of span in range */
            } /* for j through spans */

            count += spans->count;
            free(spans->points);
            spans->points = NULL;
        } /* for i thorough Spans */

        /* Now sort by x and uniquify each bucket into the final array */
        points = (Span*) malloc(count * sizeof(Span));
        if (!points) {
            int i;

            for (i = 0; i < ylength; i++) {
                free (yspans[i].points);
            }
            free (yspans);
            free (ysizes);
            if (points)
                free (points);
            return;
        }

        count = 0;
        for (i = 0; i != ylength; i++) {
            int ycount = yspans[i].count;
            if (ycount > 0) {
                if (ycount > 1) {
                    QuickSortSpansX (yspans[i].points, ycount);
                    count += UniquifySpansX (&(yspans[i]), &(points[count]));
                } else {
                    points[count] = yspans[i].points[0];
                    count++;
                }
                free(yspans[i].points);
            }
        }

        if (useBrush && pdc->brush_type != BT_SOLID && pdc->brush_stipple)
            _dc_fill_spans_brush (pdc, points, count, FALSE);
        else
            _dc_fill_spans (pdc, points, count, FALSE);
        free(points);
        free(yspans);
        free(ysizes);                /* use (DE)ALLOCATE_LOCAL for these? */
    }

    spanGroup->count = 0;
    spanGroup->ymin = SHRT_MAX;
    spanGroup->ymax = SHRT_MIN;
}

void miFillSpanGroup (PDC pdc, SpanGroup* spanGroup, BOOL useBrush)
{
    register int    i;
    register Spans  *spans;

    for (i = 0, spans = spanGroup->group; i != spanGroup->count; i++, spans++) {
        if (useBrush && pdc->brush_type != BT_SOLID && pdc->brush_stipple)
            _dc_fill_spans_brush (pdc, spans->points, spans->count, FALSE);
        else
            _dc_fill_spans (pdc, spans->points, spans->count, FALSE);
        free(spans->points);
    }

    spanGroup->count = 0;
    spanGroup->ymin = SHRT_MAX;
    spanGroup->ymax = SHRT_MIN;
}

#endif /* _MGHAVE_ADV_2DAPI */

