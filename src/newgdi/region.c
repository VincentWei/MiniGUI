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
** region.c: Device-independent multi-rectangle clipping routines.
** 
** GDI region objects. Shamelessly ripped out from the X11 distribution
** Thanks for the nice licence.
**
** Copyright 1993, 1994, 1995 Alexandre Julliard
** Modifications and additions: Copyright 1998 Huw Davies
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/10/17
**
** Derived from old file: ../kernel/cliprect.c
*/

/************************************************************************

Copyright (c) 1987, 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts.

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

************************************************************************/
/*
 * The functions in this file implement the Region abstraction, similar to one
 * used in the X11 sample server. A Region is simply an area, as the name
 * implies, and is implemented as a "y-x-banded" array of rectangles. To
 * explain: Each Region is made up of a certain number of rectangles sorted
 * by y coordinate first, and then by x coordinate.
 *
 * Furthermore, the rectangles are banded such that every rectangle with a
 * given upper-left y coordinate (y1) will have the same lower-right y
 * coordinate (y2) and vice versa. If a rectangle has scanlines in a band, it
 * will span the entire vertical distance of the band. This means that some
 * areas that could be merged into a taller rectangle will be represented as
 * several shorter rectangles to account for shorter rectangles to its left
 * or right but within its "vertical scope".
 *
 * An added constraint on the rectangles is that they must cover as much
 * horizontal area as possible. E.g. no two rectangles in a band are allowed
 * to touch.
 *
 * Whenever possible, bands will be merged together to cover a greater vertical
 * distance (and thus reduce the number of rectangles). Two bands can be merged
 * only if the bottom of one touches the top of the other and they have
 * rectangles in the same places (of the same width, of course). This maintains
 * the y-x-banding that's so nice to have...
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"
#include "gdi.h"
#include "cliprect.h"
#include "gal.h"
#include "blockheap.h"

typedef void (*voidProcp1)(CLIPRGN *region, const CLIPRECT *r1, const CLIPRECT *r1End,
                          const CLIPRECT *r2, const CLIPRECT *r2End, int top, int bottom);
typedef void (*voidProcp2)(CLIPRGN *region, const CLIPRECT *r, const CLIPRECT *rEnd,
                          int top, int bottom);

/*  1 if two RECTs overlap.
 *  0 if two RECTs do not overlap.
 */
#define EXTENTCHECK(r1, r2) \
        ((r1)->right > (r2)->left && \
         (r1)->left < (r2)->right && \
         (r1)->bottom > (r2)->top && \
         (r1)->top < (r2)->bottom)

/*
 * Allocate a new clipping rect and add it to the region.
 */
#define NEWCLIPRECT(region, rect) \
       {\
            rect = ClipRectAlloc(region->heap);\
            rect->next = NULL;\
            rect->prev = region->tail;\
            if (region->tail)\
                region->tail->next = rect;\
            region->tail = rect;\
            if (region->head == NULL)\
                region->head = rect;\
       }

#define REGION_NOT_EMPTY(region) region->head

#define INRECT(r, x, y) \
      ( ( ((r).right >  x)) && \
        ( ((r).left <= x)) && \
        ( ((r).bottom >  y)) && \
        ( ((r).top <= y)) )

extern BLOCKHEAP __mg_FreeClipRectList;

/* return TRUE if point is in region */
BOOL GUIAPI PtInRegion (PCLIPRGN region, int x, int y)
{
    int top;
    PCLIPRECT cliprect = region->head;

    if (region->type == NULLREGION) {
        return FALSE;
    }

    /* check with bounding rect of clipping region */
    if (y >= region->tail->rc.bottom || y < region->head->rc.top)
        return FALSE;

    /* find the ban in which this point lies */
    cliprect = region->head;
    while (cliprect && y >= cliprect->rc.bottom) {
        cliprect = cliprect->next;
    }

    if (!cliprect) return FALSE;

    /* check in this ban */
    top = cliprect->rc.top;
    while (cliprect && cliprect->rc.top == top) {
        if (INRECT (cliprect->rc, x, y))
            return TRUE;

        cliprect = cliprect->next;
    }

    return FALSE;
}

/* Returns TRUE if rect is at least partly inside region */
BOOL GUIAPI RectInRegion (PCLIPRGN region, const RECT* rect)
{
    PCLIPRECT cliprect = region->head;
    BOOL ret = FALSE;

    if (cliprect && EXTENTCHECK (&region->rcBound, rect)) {
        while (cliprect) {
            if (cliprect->rc.bottom <= rect->top) {
                cliprect = cliprect->next;
                continue;             /* not far enough down yet */
            }

            if (cliprect->rc.top >= rect->bottom) {
                ret = FALSE;          /* too far down */
                break;
            }

            if (cliprect->rc.right <= rect->left) {
                cliprect = cliprect->next;
                continue;              /* not far enough over yet */
            }

            if (cliprect->rc.left >= rect->right) {
                cliprect = cliprect->next;
                continue;
            }

            ret = TRUE;
            break;
        }
    }

    return ret;
}

/* Init a region */
void GUIAPI InitClipRgn (PCLIPRGN pRgn, PBLOCKHEAP heap)
{
     pRgn->type = NULLREGION;
     SetRectEmpty (&pRgn->rcBound);
     pRgn->head = NULL;
     pRgn->tail = NULL;

     pRgn->heap = heap;   // read-only field.
}

/* return bound rect of a region */
void GUIAPI GetClipRgnBoundRect (PCLIPRGN pRgn, PRECT pRect)
{
     *pRect = pRgn->rcBound;
}

BOOL GUIAPI IsEmptyClipRgn (const CLIPRGN* pRgn)
{
     if (pRgn->head == NULL)
         return TRUE;

     return FALSE;
}

void GUIAPI EmptyClipRgn (PCLIPRGN pRgn)
{
    PCLIPRECT pCRect, pTemp;

    pCRect = pRgn->head;
    while (pCRect) {
        pTemp = pCRect->next;
        FreeClipRect (pRgn->heap, pCRect);
        pCRect = pTemp;
    }

    pRgn->type = NULLREGION;
    SetRectEmpty (&pRgn->rcBound);
    pRgn->head = NULL;
    pRgn->tail = NULL;
}

/* Creates a region */
PCLIPRGN GUIAPI CreateClipRgn (void)
{
    PCLIPRGN pRgn = malloc (sizeof(CLIPRGN));
    InitClipRgn (pRgn, &__mg_FreeClipRectList);
    return pRgn;
}

/* Destroys a region */
void GUIAPI DestroyClipRgn (PCLIPRGN pRegion)
{
    EmptyClipRgn (pRegion);
    free (pRegion);
}

/* Reset a region */
BOOL GUIAPI SetClipRgn (PCLIPRGN pRgn, const RECT* pRect)
{
    PCLIPRECT pClipRect;

    if (IsRectEmpty (pRect))
        return FALSE;

    // empty rgn first
    EmptyClipRgn (pRgn);

    // get a new clip rect from free list
    pClipRect = ClipRectAlloc (pRgn->heap);
    if (pClipRect == NULL)
        return FALSE;

    // set clip rect
    pClipRect->rc = *pRect;
    pClipRect->next = NULL;
    pClipRect->prev = NULL;

    pRgn->type = SIMPLEREGION;
    pRgn->head = pRgn->tail = pClipRect;
    pRgn->rcBound = *pRect;

    return TRUE;
} 

BOOL GUIAPI ClipRgnCopy (PCLIPRGN pDstRgn, const CLIPRGN* pSrcRgn)
{
    PCLIPRECT pcr;
    PCLIPRECT pnewcr, prev;

    // return false if the destination region is not an empty one.
    if (pDstRgn == pSrcRgn)
        return FALSE;

    EmptyClipRgn (pDstRgn);
    if (!(pcr = pSrcRgn->head))
        return TRUE;

    pnewcr = ClipRectAlloc (pDstRgn->heap);

    pDstRgn->head = pnewcr;
    pnewcr->rc = pcr->rc;

    prev = NULL;
    while (pcr->next) {

        pnewcr->next = ClipRectAlloc (pDstRgn->heap);
        pnewcr->prev = prev;

        prev = pnewcr;
        pcr = pcr->next;
        pnewcr = pnewcr->next;

        pnewcr->rc = pcr->rc;
    }

    pnewcr->prev = prev;
    pnewcr->next = NULL;
    pDstRgn->tail = pnewcr;

    pDstRgn->type = pSrcRgn->type;
    pDstRgn->rcBound = pSrcRgn->rcBound; 

    return TRUE;
}

/* Re-calculate the rcBound of a region */
static void REGION_SetExtents (CLIPRGN *region)
{
    PCLIPRECT cliprect;
    RECT *pExtents;

    if (region->head == NULL) {
        region->rcBound.left = 0; region->rcBound.top = 0;
        region->rcBound.right = 0; region->rcBound.bottom = 0;
        return;
    }

    pExtents = &region->rcBound;

    /*
     * Since head is the first rectangle in the region, it must have the
     * smallest top and since tail is the last rectangle in the region,
     * it must have the largest bottom, because of banding. Initialize left and
     * right from head and tail, resp., as good things to initialize them
     * to...
     */
    pExtents->left = region->head->rc.left;
    pExtents->top = region->head->rc.top;
    pExtents->right = region->tail->rc.right;
    pExtents->bottom = region->tail->rc.bottom;

    cliprect = region->head;
    while (cliprect) {
        if (cliprect->rc.left < pExtents->left)
            pExtents->left = cliprect->rc.left;
        if (cliprect->rc.right > pExtents->right)
            pExtents->right = cliprect->rc.right;

        cliprect = cliprect->next;
    }
}

#ifdef _REGION_DEBUG
void dbg_dumpRegion (CLIPRGN* region)
{
    CLIPRECT *cliprect;

    if (!(cliprect = region->head)) {
        fprintf (stderr, "region: %p is a null region.\n", region);
    }
    else {
        fprintf (stderr, "start of region: %p.\n", region);
        fprintf (stderr, "head of region: %p.\n", region->head);
        fprintf (stderr, "tail of region: %p.\n", region->tail);
        fprintf (stderr, "Bound of region: (%d, %d, %d, %d)\n", 
                        region->rcBound.left,
                        region->rcBound.top,
                        region->rcBound.right,
                        region->rcBound.bottom);
        while (cliprect) {
            fprintf (stderr, "cliprect %p: (%d, %d, %d, %d)\n", cliprect, 
                            cliprect->rc.left, cliprect->rc.top,
                            cliprect->rc.right,
                            cliprect->rc.bottom);

            cliprect = cliprect->next;
        }
        fprintf (stderr, "end of region: %p.\n", region);
    }

}
#endif

/***********************************************************************
 *           REGION_Coalesce
 *
 *      Attempt to merge the rects in the current band with those in the
 *      previous one. Used only by REGION_RegionOp.
 *
 * Results:
 *      The new index for the previous band.
 *
 * Side Effects:
 *      If coalescing takes place:
 *          - rectangles in the previous band will have their bottom fields
 *            altered.
 *          - some clipping rect will be deleted.
 *
 */
static CLIPRECT* REGION_Coalesce (
             CLIPRGN *region,      /* Region to coalesce */
             CLIPRECT *prevStart,  /* start of previous band */
             CLIPRECT *curStart    /* start of current band */
) {
    CLIPRECT *newStart;         /* Start of new band */
    CLIPRECT *pPrevRect;        /* Current rect in previous band */
    CLIPRECT *pCurRect;         /* Current rect in current band */
    CLIPRECT *temp;             /* Temporary clipping rect */
    int curNumRects;            /* Number of rectangles in current band */
    int prevNumRects;           /* Number of rectangles in previous band */
    int bandtop;                /* top coordinate for current band */

    if (prevStart == NULL) prevStart = region->head;
    if (curStart == NULL) curStart = region->head;

    if (prevStart == curStart)
        return prevStart;

    newStart = pCurRect = curStart;

    pPrevRect = prevStart;
    temp = prevStart;
    prevNumRects = 0;
    while (temp != curStart) {
        prevNumRects ++;
        temp = temp->next;
    }

    /*
     * Figure out how many rectangles are in the current band. Have to do
     * this because multiple bands could have been added in REGION_RegionOp
     * at the end when one region has been exhausted.
     */
    pCurRect = curStart;
    bandtop = pCurRect->rc.top;
    curNumRects = 0;
    while (pCurRect && (pCurRect->rc.top == bandtop)) {
        curNumRects ++;
        pCurRect = pCurRect->next;
    }
    
    if (pCurRect) {
        /*
         * If more than one band was added, we have to find the start
         * of the last band added so the next coalescing job can start
         * at the right place... (given when multiple bands are added,
         * this may be pointless -- see above).
         */
        temp = region->tail;
        while (temp->prev->rc.top == temp->rc.top) {
            temp = temp->prev;
        }
        newStart = temp;
    }

    if ((curNumRects == prevNumRects) && (curNumRects != 0)) {
        pCurRect = curStart;
        /*
         * The bands may only be coalesced if the bottom of the previous
         * matches the top scanline of the current.
         */
        if (pPrevRect->rc.bottom == pCurRect->rc.top) {
            /*
             * Make sure the bands have rects in the same places. This
             * assumes that rects have been added in such a way that they
             * cover the most area possible. I.e. two rects in a band must
             * have some horizontal space between them.
             */
            do {
                if ((pPrevRect->rc.left != pCurRect->rc.left) ||
                    (pPrevRect->rc.right != pCurRect->rc.right))
                {
                    /*
                     * The bands don't line up so they can't be coalesced.
                     */
                    return newStart;
                }
                pPrevRect = pPrevRect->next;
                pCurRect = pCurRect->next;
            } while (--prevNumRects);

            /*
             * If only one band was added to the region, we have to backup
             * newStart to the start of the previous band.
             */
            if (pCurRect == NULL) {
                newStart = prevStart;
            }

            /*
             * The bands may be merged, so set the bottom of each rect
             * in the previous band to that of the corresponding rect in
             * the current band.
             */
            /*
             * for implementation of MiniGUI, we should free 
             * the clipping rects merged.
             */
            pCurRect = curStart;
            pPrevRect = prevStart;
            do {
                pPrevRect->rc.bottom = pCurRect->rc.bottom;
                pPrevRect = pPrevRect->next;

                if (pCurRect->next)
                    pCurRect->next->prev = pCurRect->prev;
                else
                    region->tail = pCurRect->prev;
                if (pCurRect->prev)
                    pCurRect->prev->next = pCurRect->next;
                else
                    region->head = pCurRect->next;

                temp = pCurRect->next;
                FreeClipRect (region->heap, pCurRect);
                pCurRect = temp;
            } while (--curNumRects);

            /*
             *
             * If more than one band was added to the region, copy the
             * other bands down. The assumption here is that the other bands
             * came from the same region as the current one and no further
             * coalescing can be done on them since it's all been done
             * already... newStart is already in the right place.
             */
            /* no need to copy for implementation of MiniGUI -- they are freed.
            if (temp == regionEnd) {
                newStart = prevStart;
            }

            else {
                do {
                    *pPrevRect++ = *pCurRect++;
                } while (pCurRect != regionEnd);
            }
            */
            
        }
    }
    return (newStart);
}

/***********************************************************************
 *           REGION_RegionOp
 *
 *      Apply an operation to two regions. Called by Union,
 *      Xor, Subtract, Intersect...
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      The new region is overwritten.
 *
 * Notes:
 *      The idea behind this function is to view the two regions as sets.
 *      Together they cover a rectangle of area that this function divides
 *      into horizontal bands where points are covered only by one region
 *      or by both. For the first case, the nonOverlapFunc is called with
 *      each the band and the band's upper and lower rcBound. For the
 *      second, the overlapFunc is called to process the entire band. It
 *      is responsible for clipping the rectangles in the band, though
 *      this function provides the boundaries.
 *      At the end of each band, the new region is coalesced, if possible,
 *      to reduce the number of rectangles in the region.
 *
 */
static void
REGION_RegionOp(
            CLIPRGN *newReg, /* Place to store result */
            const CLIPRGN *reg1,   /* First region in operation */
            const CLIPRGN *reg2,   /* 2nd region in operation */
            voidProcp1 overlapFunc,     /* Function to call for over-lapping bands */
            voidProcp2 nonOverlap1Func, /* Function to call for non-overlapping bands in region 1 */
            voidProcp2 nonOverlap2Func  /* Function to call for non-overlapping bands in region 2 */
) {
    CLIPRGN my_dst;
    CLIPRGN* pdst;
    const CLIPRECT *r1;                 /* Pointer into first region */
    const CLIPRECT *r2;                 /* Pointer into 2d region */
    const CLIPRECT *r1BandEnd;          /* End of current band in r1 */
    const CLIPRECT *r2BandEnd;          /* End of current band in r2 */
    int ybot;                           /* Bottom of intersection */
    int ytop;                           /* Top of intersection */
    CLIPRECT* prevBand;                 /* start of previous band in newReg */
    CLIPRECT* curBand;                  /* start of current band in newReg */
    int top;                            /* Top of non-overlapping band */
    int bot;                            /* Bottom of non-overlapping band */
    
    /*
     * Initialization:
     *  set r1, r2, r1End and r2End appropriately, preserve the important
     * parts of the destination region until the end in case it's one of
     * the two source regions, then mark the "new" region empty, allocating
     * another array of rectangles for it to use.
     */
    r1 = reg1->head;
    r2 = reg2->head;

    /*
     * newReg may be one of the src regions so we can't empty it. We keep a 
     * note of its rects pointer (so that we can free them later), preserve its
     * rcBound and simply set numRects to zero. 
     */
    /*
    oldRects = newReg->rects;
    newReg->numRects = 0;
     */

    /* 
     * for implementation of MiniGUI, we create an empty region.
     */
    if (newReg == reg1 || newReg == reg2) {
        InitClipRgn (&my_dst, newReg->heap);
        pdst = &my_dst;
    }
    else {
        EmptyClipRgn (newReg);
        pdst = newReg;
    }

    /*
     * Allocate a reasonable number of rectangles for the new region. The idea
     * is to allocate enough so the individual functions don't need to
     * reallocate and copy the array, which is time consuming, yet we don't
     * have to worry about using too much memory. I hope to be able to
     * nuke the Xrealloc() at the end of this function eventually.
     */

    /* for implementation of MiniGUI, dst always is an empty region.
    newReg->size = MAX(reg1->numRects,reg2->numRects) * 2;

    if (! (newReg->rects = malloc( sizeof(CLIPRECT) * newReg->size )))
    {
        newReg->size = 0;
        return;
    }
     */

    
    /*
     * Initialize ybot and ytop.
     * In the upcoming loop, ybot and ytop serve different functions depending
     * on whether the band being handled is an overlapping or non-overlapping
     * band.
     *  In the case of a non-overlapping band (only one of the regions
     * has points in the band), ybot is the bottom of the most recent
     * intersection and thus clips the top of the rectangles in that band.
     * ytop is the top of the next intersection between the two regions and
     * serves to clip the bottom of the rectangles in the current band.
     *  For an overlapping band (where the two regions intersect), ytop clips
     * the top of the rectangles of both regions and ybot clips the bottoms.
     */
    if (reg1->rcBound.top < reg2->rcBound.top)
        ybot = reg1->rcBound.top;
    else
        ybot = reg2->rcBound.top;
    
    /*
     * prevBand serves to mark the start of the previous band so rectangles
     * can be coalesced into larger rectangles. qv. miCoalesce, above.
     * In the beginning, there is no previous band, so prevBand == curBand
     * (curBand is set later on, of course, but the first band will always
     * start at index 0). prevBand and curBand must be indices because of
     * the possible expansion, and resultant moving, of the new region's
     * array of rectangles.
     */
    prevBand = pdst->head;
    
    do {
        curBand = pdst->tail;

        /*
         * This algorithm proceeds one source-band (as opposed to a
         * destination band, which is determined by where the two regions
         * intersect) at a time. r1BandEnd and r2BandEnd serve to mark the
         * rectangle after the last one in the current band for their
         * respective regions.
         */
        r1BandEnd = r1;
        while (r1BandEnd && (r1BandEnd->rc.top == r1->rc.top))
            r1BandEnd = r1BandEnd->next;
        
        r2BandEnd = r2;
        while (r2BandEnd && (r2BandEnd->rc.top == r2->rc.top))
            r2BandEnd = r2BandEnd->next;
        
        /*
         * First handle the band that doesn't intersect, if any.
         *
         * Note that attention is restricted to one band in the
         * non-intersecting region at once, so if a region has n
         * bands between the current position and the next place it overlaps
         * the other, this entire loop will be passed through n times.
         */
        if (r1->rc.top < r2->rc.top) {
            top = MAX (r1->rc.top, ybot);
            bot = MIN (r1->rc.bottom, r2->rc.top);

            if ((top != bot) && (nonOverlap1Func != NULL))
                (* nonOverlap1Func) (pdst, r1, r1BandEnd, top, bot);

            ytop = r2->rc.top;
        }
        else if (r2->rc.top < r1->rc.top) {
            top = MAX (r2->rc.top, ybot);
            bot = MIN (r2->rc.bottom, r1->rc.top);

            if ((top != bot) && (nonOverlap2Func != NULL))
                (* nonOverlap2Func) (pdst, r2, r2BandEnd, top, bot);

            ytop = r1->rc.top;
        }
        else {
            ytop = r1->rc.top;
        }

        /*
         * If any rectangles got added to the region, try and coalesce them
         * with rectangles from the previous band. Note we could just do
         * this test in miCoalesce, but some machines incur a not
         * inconsiderable cost for function calls, so...
         */
        if (pdst->tail != curBand) {
            prevBand = REGION_Coalesce (pdst, prevBand, curBand);
        }

        /*
         * Now see if we've hit an intersecting band. The two bands only
         * intersect if ybot > ytop
         */
        ybot = MIN (r1->rc.bottom, r2->rc.bottom);
        curBand = pdst->tail;
        if (ybot > ytop)
            (* overlapFunc) (pdst, r1, r1BandEnd, r2, r2BandEnd, ytop, ybot);
        
        if (pdst->tail != curBand)
            prevBand = REGION_Coalesce (pdst, prevBand, curBand);

        /*
         * If we've finished with a band (bottom == ybot) we skip forward
         * in the region to the next band.
         */
        if (r1->rc.bottom == ybot)
            r1 = r1BandEnd;
        if (r2->rc.bottom == ybot)
            r2 = r2BandEnd;
    } while (r1 && r2);

    /*
     * Deal with whichever region still has rectangles left.
     */
    curBand = pdst->tail;
    if (r1) {
        if (nonOverlap1Func != NULL) {
            do {
                r1BandEnd = r1;
                while ((r1BandEnd) && (r1BandEnd->rc.top == r1->rc.top)) {
                    r1BandEnd = r1BandEnd->next;
                }
                (* nonOverlap1Func) (pdst, r1, r1BandEnd,
                                     MAX (r1->rc.top, ybot), r1->rc.bottom);
                r1 = r1BandEnd;
            } while (r1);
        }
    }
    else if ((r2) && (nonOverlap2Func != NULL))
    {
        do {
            r2BandEnd = r2;
            while ((r2BandEnd) && (r2BandEnd->rc.top == r2->rc.top)) {
                 r2BandEnd = r2BandEnd->next;
            }
            (* nonOverlap2Func) (pdst, r2, r2BandEnd,
                                MAX (r2->rc.top, ybot), r2->rc.bottom);
            r2 = r2BandEnd;
        } while (r2);
    }

    if (pdst->tail != curBand)
        (void) REGION_Coalesce (pdst, prevBand, curBand);

    /*
     * A bit of cleanup. To keep regions from growing without bound,
     * we shrink the array of rectangles to match the new number of
     * rectangles in the region. This never goes to 0, however...
     *
     * Only do this stuff if the number of rectangles allocated is more than
     * twice the number of rectangles in the region (a simple optimization...).
     */

#if 0 // MiniGUI implementation does not need to do this
    if (newReg->numRects < (newReg->size >> 1))
    {
        if (REGION_NOT_EMPTY(newReg))
        {
            CLIPRECT *prev_rects = newReg->rects;
            newReg->size = newReg->numRects;
            newReg->rects = realloc( newReg->rects, sizeof(CLIPRECT) * newReg->size );
            if (! newReg->rects)
                newReg->rects = prev_rects;
        }
        else
        {
            /*
             * No point in doing the extra work involved in an Xrealloc if
             * the region is empty
             */
            newReg->size = 1;
            free( newReg->rects );
            newReg->rects = malloc( sizeof(CLIPRECT) );
        }
    }
    free( oldRects );
#else
    if (pdst != newReg) {
        EmptyClipRgn (newReg);
        *newReg = my_dst;
    }
#endif
}

/***********************************************************************
 *          Region Intersection
 ***********************************************************************/


/***********************************************************************
 *             REGION_IntersectO
 *
 * Handle an overlapping band for REGION_Intersect.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      Rectangles may be added to the region.
 *
 */
static void
REGION_IntersectO (CLIPRGN *region, const CLIPRECT *r1, const CLIPRECT *r1End,
                        const CLIPRECT *r2, const CLIPRECT *r2End, int top, int bottom)

{
    int       left, right;
    CLIPRECT  *newcliprect;

    while (r1 && r2 && (r1 != r1End) && (r2 != r2End))
    {
        left  = MAX (r1->rc.left, r2->rc.left);
        right = MIN (r1->rc.right, r2->rc.right);

        /*
         * If there's any overlap between the two rectangles, add that
         * overlap to the new region.
         * There's no need to check for subsumption because the only way
         * such a need could arise is if some region has two rectangles
         * right next to each other. Since that should never happen...
         */
        if (left < right) {
            NEWCLIPRECT (region, newcliprect);

            newcliprect->rc.left = left;
            newcliprect->rc.top = top;
            newcliprect->rc.right = right;
            newcliprect->rc.bottom = bottom;
        }

        /*
         * Need to advance the pointers. Shift the one that extends
         * to the right the least, since the other still has a chance to
         * overlap with that region's next rectangle, if you see what I mean.
         */
        if (r1->rc.right < r2->rc.right) {
            r1 = r1->next;
        }
        else if (r2->rc.right < r1->rc.right) {
            r2 = r2->next;
        }
        else {
            r1 = r1->next;
            r2 = r2->next;
        }
    }
}

/***********************************************************************
 *             Region Union
 ***********************************************************************/

/***********************************************************************
 *             REGION_UnionNonO
 *
 *      Handle a non-overlapping band for the union operation. Just
 *      Adds the rectangles into the region. Doesn't have to check for
 *      subsumption or anything.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      region->numRects is incremented and the final rectangles overwritten
 *      with the rectangles we're passed.
 *
 */
static void
REGION_UnionNonO (CLIPRGN *region, const CLIPRECT *r, const CLIPRECT *rEnd, int top, int bottom)
{
    CLIPRECT *newcliprect;

    while (r && r != rEnd) {
        NEWCLIPRECT (region, newcliprect);
        newcliprect->rc.left = r->rc.left;
        newcliprect->rc.top = top;
        newcliprect->rc.right = r->rc.right;
        newcliprect->rc.bottom = bottom;

        r = r->next;
    }
}

/***********************************************************************
 *             REGION_UnionO
 *
 *      Handle an overlapping band for the union operation. Picks the
 *      left-most rectangle each time and merges it into the region.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      Rectangles are overwritten in region->rects and region->numRects will
 *      be changed.
 *
 */
static void
REGION_UnionO(CLIPRGN *region, const CLIPRECT *r1, const CLIPRECT *r1End,
                           const CLIPRECT *r2, const CLIPRECT *r2End, int top, int bottom)
{
    CLIPRECT *newcliprect;

#define MERGERECT(r) \
    if ((region->head) &&  \
        (region->tail->rc.top == top) &&  \
        (region->tail->rc.bottom == bottom) &&  \
        (region->tail->rc.right >= r->rc.left))  \
    {  \
        if (region->tail->rc.right < r->rc.right)  \
        {  \
            region->tail->rc.right = r->rc.right;  \
        }  \
    }  \
    else  \
    {  \
        NEWCLIPRECT(region, newcliprect);  \
        newcliprect->rc.top = top;  \
        newcliprect->rc.bottom = bottom;  \
        newcliprect->rc.left = r->rc.left;  \
        newcliprect->rc.right = r->rc.right;  \
    }  \
    r = r->next;
    
    while (r1 && r2 && (r1 != r1End) && (r2 != r2End))
    {
        if (r1->rc.left < r2->rc.left)
        {
            MERGERECT(r1);
        }
        else
        {
            MERGERECT(r2);
        }
    }
    
    if (r1 && r1 != r1End)
    {
        do {
            MERGERECT(r1);
        } while (r1 && r1 != r1End);
    }
    else while (r2 && r2 != r2End)
    {
        MERGERECT(r2);
    }
}

/***********************************************************************
 *             Region Subtraction
 ***********************************************************************/

/***********************************************************************
 *             REGION_SubtractNonO1
 *
 *      Deal with non-overlapping band for subtraction. Any parts from
 *      region 2 we discard. Anything from region 1 we add to the region.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      region may be affected.
 *
 */
static void
REGION_SubtractNonO1 (CLIPRGN *region, const CLIPRECT *r, const CLIPRECT *rEnd,
                        int top, int bottom)
{
    CLIPRECT *newcliprect;
        
    while (r && r != rEnd) {
        NEWCLIPRECT(region, newcliprect);
        newcliprect->rc.left = r->rc.left;
        newcliprect->rc.top = top;
        newcliprect->rc.right = r->rc.right;
        newcliprect->rc.bottom = bottom;
        r = r->next;
    }
}


/***********************************************************************
 *             REGION_SubtractO
 *
 *      Overlapping band subtraction. x1 is the left-most point not yet
 *      checked.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      region may have rectangles added to it.
 *
 */
static void
REGION_SubtractO (CLIPRGN *region, const CLIPRECT *r1, const CLIPRECT *r1End,
                        const CLIPRECT *r2, const CLIPRECT *r2End, int top, int bottom)
{
    CLIPRECT *newcliprect;
    int left;
    
    left = r1->rc.left;
    while (r1 && r2 && (r1 != r1End) && (r2 != r2End)) {
        if (r2->rc.right <= left) {
            /*
             * Subtrahend missed the boat: go to next subtrahend.
             */
            r2 = r2->next;
        }
        else if (r2->rc.left <= left)
        {
            /*
             * Subtrahend preceeds minuend: nuke left edge of minuend.
             */
            left = r2->rc.right;
            if (left >= r1->rc.right)
            {
                /*
                 * Minuend completely covered: advance to next minuend and
                 * reset left fence to edge of new minuend.
                 */
                r1 = r1->next;
                if (r1 != r1End)
                    left = r1->rc.left;
            }
            else
            {
                /*
                 * Subtrahend now used up since it doesn't extend beyond
                 * minuend
                 */
                r2 = r2->next;
            }
        }
        else if (r2->rc.left < r1->rc.right)
        {
            /*
             * Left part of subtrahend covers part of minuend: add uncovered
             * part of minuend to region and skip to next subtrahend.
             */
            NEWCLIPRECT(region, newcliprect);
            newcliprect->rc.left = left;
            newcliprect->rc.top = top;
            newcliprect->rc.right = r2->rc.left;
            newcliprect->rc.bottom = bottom;
            left = r2->rc.right;
            if (left >= r1->rc.right)
            {
                /*
                 * Minuend used up: advance to new...
                 */
                r1 = r1->next;
                if (r1 != r1End)
                    left = r1->rc.left;
            }
            else
            {
                /*
                 * Subtrahend used up
                 */
                r2 = r2->next;
            }
        }
        else
        {
            /*
             * Minuend used up: add any remaining piece before advancing.
             */
            if (r1->rc.right > left)
            {
                NEWCLIPRECT(region, newcliprect);
                newcliprect->rc.left = left;
                newcliprect->rc.top = top;
                newcliprect->rc.right = r1->rc.right;
                newcliprect->rc.bottom = bottom;
            }
            r1 = r1->next;
            if (r1 != r1End)
                left = r1->rc.left;
        }
    }

    /*
     * Add remaining minuend rectangles to region.
     */
    while (r1 && r1 != r1End)
    {
        NEWCLIPRECT(region, newcliprect);
        newcliprect->rc.left = left;
        newcliprect->rc.top = top;
        newcliprect->rc.right = r1->rc.right;
        newcliprect->rc.bottom = bottom;
        r1 = r1->next;
        if (r1 != r1End)
            left = r1->rc.left;
    }
}

/***********************************************************************
 *             IntersectRegion
 */
BOOL GUIAPI ClipRgnIntersect (CLIPRGN *dst, const CLIPRGN *src1, const CLIPRGN *src2)
{
    /* check for trivial reject */
    if ( (!(src1->head)) || (!(src2->head))  ||
        (!EXTENTCHECK(&src1->rcBound, &src2->rcBound)))
    {
        EmptyClipRgn (dst);
        return FALSE;
    }
    else
        REGION_RegionOp (dst, src1, src2, 
            REGION_IntersectO, NULL, NULL);
    
    /*
     * Can't alter dst's rcBound before we call miRegionOp because
     * it might be one of the source regions and miRegionOp depends
     * on the rcBound of those regions being the same. Besides, this
     * way there's no checking against rectangles that will be nuked
     * due to coalescing, so we have to examine fewer rectangles.
     */
    REGION_SetExtents(dst);
    dst->type = (dst->head) ? COMPLEXREGION : NULLREGION ;

    return TRUE;
}

/***********************************************************************
 *             SubtractRegion
 *
 *      Subtract rgnS from rgnM and leave the result in rgnD.
 *      S stands for subtrahend, M for minuend and D for difference.
 *
 * Results:
 *      TRUE.
 *
 * Side Effects:
 *      regD is overwritten.
 *
 */
BOOL GUIAPI SubtractRegion (CLIPRGN *rgnD, const CLIPRGN *rgnM, const CLIPRGN *rgnS)
{
    /* check for trivial reject */
    if ( (!(rgnM->head)) || (!(rgnS->head))  ||
            (!EXTENTCHECK (&rgnM->rcBound, &rgnS->rcBound)) ) {
        CopyRegion (rgnD, rgnM);
        return TRUE;
    }
 
    REGION_RegionOp (rgnD, rgnM, rgnS, REGION_SubtractO, 
                REGION_SubtractNonO1, NULL);

    /*
     * Can't alter newReg's rcBound before we call miRegionOp because
     * it might be one of the source regions and miRegionOp depends
     * on the rcBound of those regions being the unaltered. Besides, this
     * way there's no checking against rectangles that will be nuked
     * due to coalescing, so we have to examine fewer rectangles.
     */
    REGION_SetExtents (rgnD);
    rgnD->type = (rgnD->head) ? COMPLEXREGION : NULLREGION;

    return TRUE;
}

/***********************************************************************
 *             UnionRegion
 */
BOOL GUIAPI UnionRegion (CLIPRGN *dst, const CLIPRGN *src1, const CLIPRGN *src2)
{
    /*  checks all the simple cases */

    /*
     * Region 1 and 2 are the same or region 1 is empty
     */
    if ( (src1 == src2) || (!(src1->head)) ) {
        if (dst != src2)
            CopyRegion (dst, src2);
        return TRUE;
    }

    /*
     * if nothing to union (region 2 empty)
     */
    if (!(src2->head)) {
        if (dst != src1)
            CopyRegion (dst, src1);
        return TRUE;
    }

    /*
     * Region 1 completely subsumes region 2
     */
    if ((src1->head == src1->tail) && 
        (src1->rcBound.left <= src2->rcBound.left) &&
        (src1->rcBound.top <= src2->rcBound.top) &&
        (src1->rcBound.right >= src2->rcBound.right) &&
        (src1->rcBound.bottom >= src2->rcBound.bottom))
    {
        if (dst != src1)
            CopyRegion (dst, src1);
        return TRUE;
    }

    /*
     * Region 2 completely subsumes region 1
     */
    if ((src2->head == src2->tail) && 
        (src2->rcBound.left <= src1->rcBound.left) &&
        (src2->rcBound.top <= src1->rcBound.top) &&
        (src2->rcBound.right >= src1->rcBound.right) &&
        (src2->rcBound.bottom >= src1->rcBound.bottom))
    {
        if (dst != src2)
            CopyRegion(dst, src2);
        return TRUE;
    }

    REGION_RegionOp (dst, src1, src2, REGION_UnionO, 
                REGION_UnionNonO, REGION_UnionNonO);

    REGION_SetExtents (dst);
    dst->type = (dst->head) ? COMPLEXREGION : NULLREGION ;
#if 0
    dst->rcBound.left = MIN (src1->rcBound.left, src2->rcBound.left);
    dst->rcBound.top = MIN (src1->rcBound.top, src2->rcBound.top);
    dst->rcBound.right = MAX (src1->rcBound.right, src2->rcBound.right);
    dst->rcBound.bottom = MAX (src1->rcBound.bottom, src2->rcBound.bottom);
    dbg_dumpRegion (dst);
#endif

    return TRUE;
}

/* XorRegion */
BOOL GUIAPI XorRegion (CLIPRGN *dst, const CLIPRGN *src1, const CLIPRGN *src2)
{
    CLIPRGN tmpa, tmpb;

    InitClipRgn (&tmpa, src1->heap);
    InitClipRgn (&tmpb, src2->heap);

    SubtractRegion (&tmpa, src1, src2);
    SubtractRegion (&tmpb, src2, src1);
    UnionRegion (dst, &tmpa, &tmpb);

    EmptyClipRgn (&tmpa);
    EmptyClipRgn (&tmpb);

    return TRUE;
}

/* Adds a rectangle to a region */
BOOL GUIAPI AddClipRect (PCLIPRGN region, const RECT *rect)
{
    CLIPRGN my_region;
    CLIPRECT my_cliprect;

    if (IsRectEmpty (rect))
        return FALSE;

#ifdef _REGION_DEBUG
    fprintf (stderr, "\n***************Before Union by rect (%d, %d, %d, %d):\n",
                    rect->left, rect->top, rect->right, rect->bottom);
    dbg_dumpRegion (region);
#endif

    my_cliprect.rc = *rect;
    my_cliprect.next = NULL;
    my_cliprect.prev = NULL;

    my_region.type = SIMPLEREGION;
    my_region.rcBound = *rect;
    my_region.head = &my_cliprect;
    my_region.tail = &my_cliprect;
    my_region.heap = NULL;

    UnionRegion (region, region, &my_region);

#ifdef _REGION_DEBUG
    dbg_dumpRegion (region);
    fprintf (stderr, "***************After Union\n");
#endif

    return TRUE;
}

/* Intersect a rect with a region */
BOOL GUIAPI IntersectClipRect (PCLIPRGN region, const RECT* rect)
{
    CLIPRGN my_region;
    CLIPRECT my_cliprect;

    if (IsRectEmpty (rect)) {
        EmptyClipRgn (region);
        return TRUE;
    }

#ifdef _REGION_DEBUG
    fprintf (stderr, "\n***************before intersect by rect (%d, %d, %d, %d):\n",
                    rect->left, rect->top, rect->right, rect->bottom);
    dbg_dumpRegion (region);
#endif

    my_cliprect.rc = *rect;
    my_cliprect.next = NULL;
    my_cliprect.prev = NULL;

    my_region.type = SIMPLEREGION;
    my_region.rcBound = *rect;
    my_region.head = &my_cliprect;
    my_region.tail = &my_cliprect;
    my_region.heap = NULL;

    ClipRgnIntersect (region, region, &my_region);

#ifdef _REGION_DEBUG
    dbg_dumpRegion (region);
    fprintf (stderr, "***************After intersect\n");
#endif

    return TRUE;
}

BOOL GUIAPI SubtractClipRect (PCLIPRGN region, const RECT* rect)
{
    CLIPRGN my_region;
    CLIPRECT my_cliprect;

    if (IsRectEmpty (rect) || !DoesIntersect (&region->rcBound, rect))
        return FALSE;

#ifdef _REGION_DEBUG
    fprintf (stderr, "\n***************Before subtract by rect (%d, %d, %d, %d):\n",
                    rect->left, rect->top, rect->right, rect->bottom);
    dbg_dumpRegion (region);
#endif

    my_cliprect.rc = *rect;
    my_cliprect.next = NULL;
    my_cliprect.prev = NULL;

    my_region.type = SIMPLEREGION;
    my_region.rcBound = *rect;
    my_region.head = &my_cliprect;
    my_region.tail = &my_cliprect;
    my_region.heap = NULL;

    SubtractRegion (region, region, &my_region);

#ifdef _REGION_DEBUG
    dbg_dumpRegion (region);
    fprintf (stderr, "***************After subtraction\n");
#endif

    return TRUE;
}

#if 1
void GUIAPI OffsetRegionEx (PCLIPRGN region, 
        const RECT *rcClient, const RECT *rcScroll, int x, int y)
{
    CLIPRECT* cliprect = region->head;
    //RECT old_cliprc, rc_array[4];
    //int i, nCount = 0;
    RECT old_cliprc, rc;
    PCLIPRECT pTemp;

    if (!rcClient || !rcScroll)
        return;

    if (!IntersectRect (&rc, rcClient, rcScroll))
        return;

#ifdef _REGION_DEBUG
    fprintf (stderr, "***************enter OffsetRegionEx\n");
    dbg_dumpRegion (region);
#endif
    while (cliprect) {
        /*not in scroll window region, return*/
        if (!DoesIntersect (&cliprect->rc, &rc)) {
            cliprect = cliprect->next;
#ifdef _REGION_DEBUG
            fprintf (stderr, "needn't scroll this cliprect.\n");
#endif
            continue;
        }

        /*not covered, recalculate cliprect*/
        if (!IsCovered (&cliprect->rc, &rc)) {
            CopyRect (&old_cliprc, &cliprect->rc);
            IntersectRect (&cliprect->rc, &old_cliprc, &rc);
#if 0
            //nCount = SubtractRect (rc_array, &rc, &old_cliprc);
            nCount = SubtractRect (rc_array, &old_cliprc, &rc);
#ifdef _REGION_DEBUG
            fprintf (stderr, "add new %d cliprect to region.\n", nCount);
#endif
            for (i = 0; i < nCount; i++) {
                AddClipRect (region, &rc_array[i]);
            }
#endif
        }

        OffsetRect (&cliprect->rc, x, y);
#ifdef _REGION_DEBUG
        fprintf (stderr, "offset current cliprect. \n");
#endif

        /*if not intersect, remove current cliprect from list*/
        if (!DoesIntersect (&cliprect->rc, &rc)) {
            pTemp = cliprect->next;

            if(cliprect->next)
                cliprect->next->prev = cliprect->prev;
            else
                region->tail = cliprect->prev;
            if(cliprect->prev)
                cliprect->prev->next = cliprect->next;
            else
                region->head = cliprect->next;

            FreeClipRect (region->heap, cliprect);
            cliprect = pTemp;
#ifdef _REGION_DEBUG
            fprintf (stderr, "remove current cliprect. \n");
#endif
            continue;
        }

        /*if intersect, tune cliprect*/
        if (!IsCovered (&cliprect->rc, &rc)) {
            CopyRect (&old_cliprc, &cliprect->rc);
            IntersectRect (&cliprect->rc, &old_cliprc, &rc);
#ifdef _REGION_DEBUG
            fprintf (stderr, "tune current cliprect. \n");
#endif
        }

        if (region->head) {
            REGION_SetExtents(region);
        }
        cliprect = cliprect->next;
    }

#ifdef _REGION_DEBUG
    dbg_dumpRegion (region);
    fprintf (stderr, "***************after OffsetRegionEx\n");
#endif
}
#else
void GUIAPI OffsetRegionEx (PCLIPRGN region, 
        const RECT *rcClient, const RECT *rcScroll, int x, int y)
{
    CLIPRECT* cliprect = region->head;
    RECT old_cliprc, rc;
    PCLIPRECT pTemp;

    if (!rcClient || !rcScroll)
        return;

    if (!IntersectRect (&rc, rcClient, rcScroll))
        return;

#ifdef _REGION_DEBUG
    fprintf (stderr, "***************enter OffsetRegionEx\n");
    dbg_dumpRegion (region);
#endif
    while (cliprect) {
        OffsetRect (&cliprect->rc, x, y);
#ifdef _REGION_DEBUG
        fprintf (stderr, "offset current cliprect. \n");
#endif
        /*if not intersect, remove current cliprect from list*/
        if (!DoesIntersect (&cliprect->rc, &rc)) {
            pTemp = cliprect->next;

            if(cliprect->next)
                cliprect->next->prev = cliprect->prev;
            else
                region->tail = cliprect->prev;
            if(cliprect->prev)
                cliprect->prev->next = cliprect->next;
            else
                region->head = cliprect->next;

            FreeClipRect (region->heap, cliprect);
            cliprect = pTemp;
#ifdef _REGION_DEBUG
            fprintf (stderr, "remove current cliprect. \n");
#endif
            continue;
        }

        /*if intersect, tune cliprect*/
        if (!IsCovered (&cliprect->rc, &rc)) {
            CopyRect (&old_cliprc, &cliprect->rc);
            IntersectRect (&cliprect->rc, &old_cliprc, &rc);
#ifdef _REGION_DEBUG
            fprintf (stderr, "tune current cliprect. \n");
#endif
        }

        cliprect = cliprect->next;
    }
    /* recalc rcBound after Region true ok.*/
    if (region->head) {
        REGION_SetExtents(region);
    }

#ifdef _REGION_DEBUG
    dbg_dumpRegion (region);
    fprintf (stderr, "***************after OffsetRegionEx\n");
#endif

}
#endif

void GUIAPI OffsetRegion (PCLIPRGN region, int x, int y)
{
    CLIPRECT* cliprect = region->head;

    while (cliprect) {
        OffsetRect (&cliprect->rc, x, y);
        cliprect = cliprect->next;
    }

    if (region->head) {
        OffsetRect (&region->rcBound, x, y);
    }
}

static void cb_region (void* context, int x1, int x2, int y)
{
#if 0
    CLIPRGN* region = (CLIPRGN*) context;
    CLIPRECT* newcliprect;

    newcliprect = ClipRectAlloc (region->heap);
    if (x2 > x1) {
        newcliprect->rc.left = x1;
        newcliprect->rc.right = x2 + 1;
    }
    else {
        newcliprect->rc.left = x2;
        newcliprect->rc.right = x1 + 1;
    }
    newcliprect->rc.top = y;
    newcliprect->rc.bottom = y + 1;

    if (region->head == NULL 
            || (newcliprect->rc.top >= region->tail->rc.bottom 
                && (newcliprect->rc.left != region->tail->rc.left
                    || newcliprect->rc.right != region->tail->rc.right))) {
        /* simply append to tail */
        goto append;
    }
    else if (newcliprect->rc.top == region->tail->rc.bottom 
                && newcliprect->rc.left == region->tail->rc.left
                && newcliprect->rc.right == region->tail->rc.right) {
        /* merge with the tail */
        region->tail->rc.bottom += 1;
        FreeClipRect (region->heap, newcliprect);
    }
    else if (newcliprect->rc.bottom == region->head->rc.top
                && newcliprect->rc.left == region->head->rc.left
                && newcliprect->rc.right == region->head->rc.right) {
        /* merge with the head */
        region->head->rc.top -= 1;
        FreeClipRect (region->heap, newcliprect);
    }
    else if (newcliprect->rc.top > region->tail->rc.bottom) {
        /* simply append to tail */
        goto append;
    }
    else if (newcliprect->rc.bottom < region->head->rc.top) {
        /* simply insert to head */
        newcliprect->prev = NULL;
        newcliprect->next = region->head;
        region->head->prev = newcliprect;
        region->head = newcliprect;
    }
    else {
        /* find a position to insert */
        CLIPRECT* cliprect = region->head;
        CLIPRECT* prev;

        printf ("get here.\n");
        printf ("new cliprc: (%d, %d, %d, %d)\n", 
                        newcliprect->rc.left, newcliprect->rc.top,
                        newcliprect->rc.right, newcliprect->rc.bottom);
        printf ("head cliprc: (%d, %d, %d, %d)\n", 
                        region->head->rc.left, region->head->rc.top,
                        region->head->rc.right, region->head->rc.bottom);
        printf ("tail cliprc: (%d, %d, %d, %d)\n", 
                        region->tail->rc.left, region->tail->rc.top,
                        region->tail->rc.right, region->tail->rc.bottom);

        while (cliprect && newcliprect->rc.top >= cliprect->rc.top) {
            cliprect = cliprect->next;
        }

        if (cliprect == NULL) /* simply append to the tail */
            goto append;

        printf ("current cliprc: (%d, %d, %d, %d)\n", 
                        cliprect->rc.left, cliprect->rc.top,
                        cliprect->rc.right, cliprect->rc.bottom);

        /* merge with prev or next? */
        if ((prev = cliprect->prev) 
                && prev->rc.bottom == newcliprect->rc.top
                && prev->rc.left == newcliprect->rc.left
                && prev->rc.right == newcliprect->rc.right) {
            prev->rc.bottom += 1;
            FreeClipRect (region->heap, newcliprect);
            printf ("merge with previous.\n");
        }
        else if (cliprect->rc.top == newcliprect->rc.bottom
                && cliprect->rc.left == newcliprect->rc.left
                && cliprect->rc.right == newcliprect->rc.right) {
            cliprect->rc.top -= 1;
            FreeClipRect (region->heap, newcliprect);
            printf ("merge with current.\n");
        }
        else {
            /* insert before of current cliprc */
            cliprect->prev->next = newcliprect;
            newcliprect->prev = cliprect->prev;
            cliprect->prev = newcliprect;
            newcliprect->next = cliprect;
            printf ("insert before of current.\n");
        }
    }

    return;

append:
    newcliprect->next = NULL;
    newcliprect->prev = region->tail;
    if (region->tail)
        region->tail->next = newcliprect;
    region->tail = newcliprect;
    if (region->head == NULL)
        region->head = newcliprect;
#else
    CLIPRGN* region = (CLIPRGN*) context;
    CLIPRGN newregion;
    CLIPRECT newcliprect;

    if (x2 > x1) {
        newcliprect.rc.left = x1;
        newcliprect.rc.right = x2 + 1;
    }
    else {
        newcliprect.rc.left = x2;
        newcliprect.rc.right = x1 + 1;
    }
    newcliprect.rc.top = y;
    newcliprect.rc.bottom = y + 1;

    newcliprect.next = NULL;
    newcliprect.prev = NULL;

    newregion.type = SIMPLEREGION;
    newregion.rcBound = newcliprect.rc;
    newregion.head = &newcliprect;
    newregion.tail = &newcliprect;
    newregion.heap = NULL;

    if (IsEmptyClipRgn (region))
        CopyRegion (region, &newregion);
    else
        UnionRegion (region, region, &newregion);
#endif
}

#ifndef _MG_MINIMALGDI
BOOL GUIAPI InitCircleRegion (PCLIPRGN dst, int x, int y, int r)
{
    EmptyClipRgn (dst);

    if (r < 1) {
        CLIPRECT* newcliprect;

        NEWCLIPRECT (dst, newcliprect);
        newcliprect->rc.left = x;
        newcliprect->rc.top = y;
        newcliprect->rc.right = x + 1;
        newcliprect->rc.bottom = y + 1;
        return TRUE;
    }

    CircleGenerator (dst, x, y, r, cb_region);

    return TRUE;
}

BOOL GUIAPI InitEllipseRegion (PCLIPRGN dst, int x, int y, int rx, int ry)
{
    EmptyClipRgn (dst);

    if (rx < 1 || ry < 1) {
        CLIPRECT* newcliprect;

        NEWCLIPRECT (dst, newcliprect);
        newcliprect->rc.left = x;
        newcliprect->rc.top = y;
        newcliprect->rc.right = x + 1;
        newcliprect->rc.bottom = y + 1;
        return TRUE;
    }

    EllipseGenerator (dst, x, y, rx, ry, cb_region);

    return TRUE;
}
#endif

BOOL GUIAPI InitPolygonRegion (PCLIPRGN dst, const POINT* pts, int vertices)
{
    EmptyClipRgn (dst);

    if (PolygonIsMonotoneVertical (pts, vertices)) {
        if (MonotoneVerticalPolygonGenerator (dst, pts, vertices, cb_region))
            goto ok;
    }
    else if (PolygonGenerator (dst, pts, vertices, cb_region)) {
        goto ok;
    }

    return FALSE;

ok:
   return TRUE;
}

